#!/usr/bin/env python3
import os
import json
import capnp
import numpy as np
from enum import Enum
from collections import defaultdict

from cereal import log, messaging
from openpilot.common.transformations.orientation import rot_from_euler
from openpilot.common.realtime import config_realtime_process
from openpilot.common.params import Params
from openpilot.selfdrive.locationd.helpers import rotate_std
from openpilot.selfdrive.locationd.models.pose_kf import PoseKalman, States
from openpilot.selfdrive.locationd.models.constants import ObservationKind, GENERATED_DIR

ACCEL_SANITY_CHECK = 100.0  # m/s^2
ROTATION_SANITY_CHECK = 10.0  # rad/s
TRANS_SANITY_CHECK = 200.0  # m/s
CALIB_RPY_SANITY_CHECK = 0.5  # rad (+- 30 deg)
MIN_STD_SANITY_CHECK = 1e-5  # m or rad
MAX_FILTER_REWIND_TIME = 0.8  # s
MAX_SENSOR_TIME_DIFF = 0.1  # s
YAWRATE_CROSS_ERR_CHECK_FACTOR = 30
INPUT_INVALID_THRESHOLD = 0.5
INPUT_INVALID_DECAY = 0.9993  # ~10 secs to resume after a bad input
POSENET_STD_INITIAL_VALUE = 10.0
POSENET_STD_HIST_HALF = 20


def init_xyz_measurement(measurement: capnp._DynamicStructBuilder, values: np.ndarray, stds: np.ndarray, valid: bool):
  assert len(values) == len(stds) == 3
  for i, d in enumerate(("x", "y", "z")):
    setattr(measurement, d, float(values[i]))
    setattr(measurement, f"{d}Std", float(stds[i]))
  measurement.valid = valid


class HandleLogResult(Enum):
  SUCCESS = 0
  TIMING_INVALID = 1
  INPUT_INVALID = 2
  SENSOR_SOURCE_INVALID = 3


class LocationEstimator:
  def __init__(self, debug: bool):
    self.kf = PoseKalman(GENERATED_DIR, MAX_FILTER_REWIND_TIME)

    self.debug = debug

    self.posenet_stds = [POSENET_STD_INITIAL_VALUE] * (POSENET_STD_HIST_HALF * 2)
    self.car_speed = 0.0
    self.camodo_yawrate_distribution = np.array([0.0, 10.0])  # mean, std
    self.device_from_calib = np.eye(3)

    obs_kinds = [ObservationKind.PHONE_ACCEL, ObservationKind.PHONE_GYRO, ObservationKind.CAMERA_ODO_ROTATION, ObservationKind.CAMERA_ODO_TRANSLATION]
    self.observations = {kind: np.zeros(3, dtype=np.float32) for kind in obs_kinds}
    self.observation_errors = {kind: np.zeros(3, dtype=np.float32) for kind in obs_kinds}

  def reset(self, t: float, x_initial: np.ndarray = PoseKalman.initial_x, P_initial: np.ndarray = PoseKalman.initial_P):
    self.kf.reset(t, x_initial, P_initial)

  def _validate_sensor_source(self, source: log.SensorEventData.SensorSource):
    # some segments have two IMUs, ignore the second one
    return source != log.SensorEventData.SensorSource.bmx055

  def _validate_sensor_time(self, sensor_time: float, t: float):
    # ignore empty readings
    if sensor_time == 0:
      return False

    # sensor time and log time should be close
    sensor_time_invalid = abs(sensor_time - t) > MAX_SENSOR_TIME_DIFF
    if sensor_time_invalid:
      print("Sensor reading ignored, sensor timestamp more than 100ms off from log time")
    return not sensor_time_invalid

  def _validate_timestamp(self, t: float):
    invalid = not np.isnan(self.kf.t) and (self.kf.t - t) > MAX_FILTER_REWIND_TIME
    if invalid:
      print("Observation timestamp is older than the max rewind threshold of the filter")
    return not invalid

  def _finite_check(self, t: float):
    all_finite = np.all(np.isfinite(self.kf.x)) and np.all(np.isfinite(self.kf.P))
    if not all_finite:
      print("Non-finite values detected, kalman reset")
      self.reset(t)

  def handle_log(self, t: float, which: str, msg: capnp._DynamicStructReader) -> HandleLogResult:
    if not self._validate_timestamp(t):
      return HandleLogResult.TIMING_INVALID

    if which == "accelerometer" and msg.which() == "acceleration":
      sensor_time = msg.timestamp * 1e-9

      if not self._validate_sensor_time(sensor_time, t) or not self._validate_timestamp(sensor_time):
        return HandleLogResult.TIMING_INVALID

      if not self._validate_sensor_source(msg.source):
        return HandleLogResult.SENSOR_SOURCE_INVALID

      v = msg.acceleration.v
      meas = np.array([-v[2], -v[1], -v[0]])
      if np.linalg.norm(meas) >= ACCEL_SANITY_CHECK:
        return HandleLogResult.INPUT_INVALID

      acc_res = self.kf.predict_and_observe(sensor_time, ObservationKind.PHONE_ACCEL, meas)
      if acc_res is not None:
        _, _, _, _, _, _, (acc_err,), _, _ = acc_res
        self.observation_errors[ObservationKind.PHONE_ACCEL] = np.array(acc_err)
        self.observations[ObservationKind.PHONE_ACCEL] = meas

    elif which == "gyroscope" and msg.which() == "gyroUncalibrated":
      sensor_time = msg.timestamp * 1e-9

      if not self._validate_sensor_time(sensor_time, t) or not self._validate_timestamp(sensor_time):
        return HandleLogResult.TIMING_INVALID

      if not self._validate_sensor_source(msg.source):
        return HandleLogResult.SENSOR_SOURCE_INVALID

      v = msg.gyroUncalibrated.v
      meas = np.array([-v[2], -v[1], -v[0]])

      gyro_bias = self.kf.x[States.GYRO_BIAS]
      gyro_camodo_yawrate_err = np.abs((meas[2] - gyro_bias[2]) - self.camodo_yawrate_distribution[0])
      gyro_camodo_yawrate_err_threshold = YAWRATE_CROSS_ERR_CHECK_FACTOR * self.camodo_yawrate_distribution[1]
      gyro_valid = gyro_camodo_yawrate_err < gyro_camodo_yawrate_err_threshold

      if np.linalg.norm(meas) >= ROTATION_SANITY_CHECK or not gyro_valid:
        return HandleLogResult.INPUT_INVALID

      gyro_res = self.kf.predict_and_observe(sensor_time, ObservationKind.PHONE_GYRO, meas)
      if gyro_res is not None:
        _, _, _, _, _, _, (gyro_err,), _, _ = gyro_res
        self.observation_errors[ObservationKind.PHONE_GYRO] = np.array(gyro_err)
        self.observations[ObservationKind.PHONE_GYRO] = meas

    elif which == "carState":
      self.car_speed = abs(msg.vEgo)

    elif which == "liveCalibration":
      if len(msg.rpyCalib) > 0:
        calib = np.array(msg.rpyCalib)
        if calib.min() < -CALIB_RPY_SANITY_CHECK or calib.max() > CALIB_RPY_SANITY_CHECK:
          return HandleLogResult.INPUT_INVALID

        self.device_from_calib = rot_from_euler(calib)
        self.calibrated = msg.calStatus == log.LiveCalibrationData.Status.calibrated

    elif which == "cameraOdometry":
      rot_device = np.matmul(self.device_from_calib, np.array(msg.rot))
      trans_device = np.matmul(self.device_from_calib, np.array(msg.trans))

      if np.linalg.norm(rot_device) > ROTATION_SANITY_CHECK or np.linalg.norm(trans_device) > TRANS_SANITY_CHECK:
        return HandleLogResult.INPUT_INVALID

      rot_calib_std = np.array(msg.rotStd)
      trans_calib_std = np.array(msg.transStd)

      if rot_calib_std.min() <= MIN_STD_SANITY_CHECK or trans_calib_std.min() <= MIN_STD_SANITY_CHECK:
        return HandleLogResult.INPUT_INVALID

      if np.linalg.norm(rot_calib_std) > 10 * ROTATION_SANITY_CHECK or np.linalg.norm(trans_calib_std) > 10 * TRANS_SANITY_CHECK:
        return HandleLogResult.INPUT_INVALID

      self.posenet_stds.pop(0)
      self.posenet_stds.append(trans_calib_std[0])

      # Multiply by 10 to avoid to high certainty in kalman filter because of temporally correlated noise
      rot_calib_std *= 10
      trans_calib_std *= 10

      rot_device_std = rotate_std(self.device_from_calib, rot_calib_std)

      cam_odo_rot_res = self.kf.predict_and_observe(t, ObservationKind.CAMERA_ODO_ROTATION, rot_device)
      cam_odo_trans_res = self.kf.predict_and_observe(t, ObservationKind.CAMERA_ODO_TRANSLATION, trans_device)
      self.camodo_yawrate_distribution =  np.array([rot_device[2], rot_device_std[2]])
      if cam_odo_rot_res is not None:
        _, _, _, _, _, _, (cam_odo_rot_err,), _, _ = cam_odo_rot_res
        self.observation_errors[ObservationKind.CAMERA_ODO_ROTATION] = np.array(cam_odo_rot_err)
        self.observations[ObservationKind.CAMERA_ODO_ROTATION] = rot_device
      if cam_odo_trans_res is not None:
        _, _, _, _, _, _, (cam_odo_trans_err,), _, _ = cam_odo_trans_res
        self.observation_errors[ObservationKind.CAMERA_ODO_TRANSLATION] = np.array(cam_odo_trans_err)
        self.observations[ObservationKind.CAMERA_ODO_TRANSLATION] = trans_device

    self._finite_check(t)
    return HandleLogResult.SUCCESS

  def get_msg(self, sensors_valid: bool, inputs_valid: bool, filter_valid: bool):
    state, cov = self.kf.x, self.kf.P
    std = np.sqrt(np.diag(cov))

    orientation_ned, orientation_ned_std = state[States.NED_ORIENTATION], std[States.NED_ORIENTATION]
    velocity_device, velocity_device_std = state[States.DEVICE_VELOCITY], std[States.DEVICE_VELOCITY]
    angular_velocity_device, angular_velocity_device_std = state[States.ANGULAR_VELOCITY], std[States.ANGULAR_VELOCITY]
    acceleration_device, acceleration_device_std = state[States.ACCELERATION], std[States.ACCELERATION]

    msg = messaging.new_message("livePose")
    msg.valid = filter_valid

    livePose = msg.livePose
    init_xyz_measurement(livePose.orientationNED, orientation_ned, orientation_ned_std, filter_valid)
    init_xyz_measurement(livePose.velocityDevice, velocity_device, velocity_device_std, filter_valid)
    init_xyz_measurement(livePose.angularVelocityDevice, angular_velocity_device, angular_velocity_device_std, filter_valid)
    init_xyz_measurement(livePose.accelerationDevice, acceleration_device, acceleration_device_std, filter_valid)
    if self.debug:
      livePose.debugFilterState.value = state.tolist()
      livePose.debugFilterState.std = std.tolist()
      livePose.debugFilterState.valid = filter_valid
      livePose.debugFilterState.observations = [
        {'kind': k, 'value': self.observations[k].tolist(), 'error': self.observation_errors[k].tolist()}
        for k in self.observations.keys()
      ]

    old_mean = np.mean(self.posenet_stds[:POSENET_STD_HIST_HALF])
    new_mean = np.mean(self.posenet_stds[POSENET_STD_HIST_HALF:])
    std_spike = (new_mean / old_mean) > 4.0 and new_mean > 7.0

    livePose.inputsOK = inputs_valid
    livePose.posenetOK = not std_spike or self.car_speed <= 5.0
    livePose.sensorsOK = sensors_valid

    return msg


def main():
  config_realtime_process([0, 1, 2, 3], 5)

  DEBUG = bool(int(os.getenv("DEBUG", "0")))

  pm = messaging.PubMaster(['livePose'])
  sm = messaging.SubMaster(['accelerometer', 'gyroscope', 'carState', 'liveCalibration', 'cameraOdometry'])

  params = Params()

  estimator = LocationEstimator(DEBUG)

  filter_initialized = False
  critcal_services = ["accelerometer", "gyroscope", "liveCalibration", "cameraOdometry"]
  observation_timing_invalid = False
  observation_input_invalid = defaultdict(int)

  initial_pose = params.get("LocationFilterInitialState")
  mesh_xs = None
  if initial_pose is not None:
    initial_pose = json.loads(initial_pose)
    if "mesh_x" in initial_pose:
      mesh_xs = np.array(initial_pose["mesh_x"], dtype=np.float64)
      estimator.reset(None, mesh_xs[0], PoseKalman.initial_P)
    else:
      x_initial = np.array(initial_pose["x"], dtype=np.float64)
      P_initial = np.diag(np.array(initial_pose["P"], dtype=np.float64))
      estimator.reset(None, x_initial, P_initial)

  cnt = 0

  while True:
    sm.update()

    filter_initialized = sm.all_checks()
    if filter_initialized:
      observation_timing_invalid = False
      for which in sorted(sm.updated.keys(), key=lambda x: sm.logMonoTime[x]):
        if sm.valid[which]:
          t = sm.logMonoTime[which] * 1e-9
          res = estimator.handle_log(t, which, sm[which])
          if res == HandleLogResult.TIMING_INVALID:
            observation_timing_invalid = True
          elif res == HandleLogResult.INPUT_INVALID:
            observation_input_invalid[which] += 1
          else:
            observation_input_invalid[which] *= INPUT_INVALID_DECAY

    if sm.updated["cameraOdometry"]:
      critical_service_inputs_valid = all(observation_input_invalid[s] < INPUT_INVALID_THRESHOLD for s in critcal_services)
      inputs_valid = sm.all_valid() and critical_service_inputs_valid and not observation_timing_invalid
      sensors_valid = sm.all_checks(["accelerometer", "gyroscope"])

      estimator.reset(estimator.kf.filter.get_filter_time(), mesh_xs[cnt], PoseKalman.initial_P)

      msg = estimator.get_msg(sensors_valid, inputs_valid, filter_initialized)
      pm.send("livePose", msg)

      cnt = (cnt + 1) % len(mesh_xs)


if __name__ == "__main__":
  main()
