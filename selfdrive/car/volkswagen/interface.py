from cereal import car
from selfdrive.config import Conversions as CV
from selfdrive.controls.lib.drive_helpers import create_event, EventTypes as ET
from selfdrive.controls.lib.vehicle_model import VehicleModel
from selfdrive.car.volkswagen.values import CAR, BUTTON_STATES
from selfdrive.car.volkswagen.carstate import CarState, get_mqb_pt_can_parser, get_mqb_cam_can_parser
from common.params import Params
from selfdrive.car import STD_CARGO_KG, scale_rot_inertia, scale_tire_stiffness, gen_empty_fingerprint
from selfdrive.car.interfaces import CarInterfaceBase

GEAR = car.CarState.GearShifter

class CANBUS:
  pt = 0
  cam = 2

class CarInterface(CarInterfaceBase):
  def __init__(self, CP, CarController):
    self.CP = CP
    self.CC = None

    self.frame = 0

    self.gasPressedPrev = False
    self.brakePressedPrev = False
    self.cruiseStateEnabledPrev = False
    self.displayMetricUnitsPrev = None
    self.buttonStatesPrev = BUTTON_STATES.copy()

    # *** init the major players ***
    self.CS = CarState(CP, CANBUS)
    self.VM = VehicleModel(CP)
    self.pt_cp = get_mqb_pt_can_parser(CP, CANBUS)
    self.cam_cp = get_mqb_cam_can_parser(CP, CANBUS)

    # sending if read only is False
    if CarController is not None:
      self.CC = CarController(CANBUS, CP.carFingerprint)

  @staticmethod
  def compute_gb(accel, speed):
    return float(accel) / 4.0

  @staticmethod
  def get_params(candidate, fingerprint=gen_empty_fingerprint(), has_relay=False, car_fw=[]):
    ret = car.CarParams.new_message()

    ret.carFingerprint = candidate
    ret.isPandaBlack = has_relay

    if candidate == CAR.GOLF:
      # Set common MQB parameters that will apply globally
      ret.carName = "volkswagen"
      ret.safetyModel = car.CarParams.SafetyModel.volkswagen
      ret.enableCruise = True # Stock ACC still controls acceleration and braking
      ret.openpilotLongitudinalControl = False
      ret.steerControlType = car.CarParams.SteerControlType.torque

      # Additional common MQB parameters that may be overridden per-vehicle
      ret.steerRateCost = 0.5
      ret.steerActuatorDelay = 0.05 # Hopefully all MQB racks are similar here
      ret.steerLimitTimer = 0.4
      ret.steerMaxBP = [0.]  # m/s
      ret.steerMaxV = [1.]

      # As a starting point for speed-adjusted lateral tuning, use the example
      # map speed breakpoints from a VW Tiguan (SSP 399 page 9). It's unclear
      # whether the driver assist map breakpoints have any direct bearing on
      # HCA assist torque, but if they're good breakpoints for the driver,
      # they're probably good breakpoints for HCA as well. OP won't be driving
      # 250kph/155mph but it provides interpolation scaling above 100kmh/62mph.
      ret.lateralTuning.pid.kpBP = [0., 15 * CV.KPH_TO_MS, 50 * CV.KPH_TO_MS]
      ret.lateralTuning.pid.kiBP = [0., 15 * CV.KPH_TO_MS, 50 * CV.KPH_TO_MS]

      # FIXME: Per-vehicle parameters need to be reintegrated.
      # For the time being, per-vehicle stuff is being archived since we
      # can't auto-detect very well yet. Now that tuning is figured out,
      # averaged params should work reasonably on a range of cars. Owners
      # can tweak here, as needed, until we have car type auto-detection.

      ret.mass = 1700 + STD_CARGO_KG
      ret.wheelbase = 2.75
      ret.centerToFront = ret.wheelbase * 0.45
      ret.steerRatio = 15.6
      ret.lateralTuning.pid.kf = 0.00006
      ret.lateralTuning.pid.kpV = [0.15, 0.25, 0.60]
      ret.lateralTuning.pid.kiV = [0.05, 0.05, 0.05]
      tire_stiffness_factor = 0.6

    ret.enableCamera = True # Stock camera detection doesn't apply to VW
    ret.transmissionType = car.CarParams.TransmissionType.automatic
    ret.steerRatioRear = 0.

    # No support for OP longitudinal control on Volkswagen at this time.
    ret.gasMaxBP = [0.]
    ret.gasMaxV = [0.]
    ret.brakeMaxBP = [0.]
    ret.brakeMaxV = [0.]
    ret.longitudinalTuning.deadzoneBP = [0.]
    ret.longitudinalTuning.deadzoneV = [0.]
    ret.longitudinalTuning.kpBP = [0.]
    ret.longitudinalTuning.kpV = [0.]
    ret.longitudinalTuning.kiBP = [0.]
    ret.longitudinalTuning.kiV = [0.]

    # TODO: get actual value, for now starting with reasonable value for
    # civic and scaling by mass and wheelbase
    ret.rotationalInertia = scale_rot_inertia(ret.mass, ret.wheelbase)

    # TODO: start from empirically derived lateral slip stiffness for the civic and scale by
    # mass and CG position, so all cars will have approximately similar dyn behaviors
    ret.tireStiffnessFront, ret.tireStiffnessRear = scale_tire_stiffness(ret.mass, ret.wheelbase, ret.centerToFront,
                                                                         tire_stiffness_factor=tire_stiffness_factor)

    return ret

  # returns a car.CarState
  def update(self, c, can_strings):
    canMonoTimes = []
    events = []
    buttonEvents = []
    params = Params()

    # Process the most recent CAN message traffic, and check for validity
    # The camera CAN has no signals we use at this time, but we process it
    # anyway so we can test connectivity with can_valid
    self.pt_cp.update_strings(can_strings)
    self.cam_cp.update_strings(can_strings)

    ret = self.CS.update(self.pt_cp)
    ret.canValid = self.pt_cp.can_valid and self.cam_cp.can_valid
    ret.steeringRateLimited = self.CC.steer_rate_limited if self.CC is not None else False

    # Update the EON metric configuration to match the car at first startup,
    # or if there's been a change.
    if self.CS.displayMetricUnits != self.displayMetricUnitsPrev:
      params.put("IsMetric", "1" if self.CS.displayMetricUnits else "0")

    # Check for and process state-change events (button press or release) from
    # the turn stalk switch or ACC steering wheel/control stalk buttons.
    for button in self.CS.buttonStates:
      if self.CS.buttonStates[button] != self.buttonStatesPrev[button]:
        be = car.CarState.ButtonEvent.new_message()
        be.type = button
        be.pressed = self.CS.buttonStates[button]
        buttonEvents.append(be)

    # Vehicle operation safety checks and events
    if ret.doorOpen:
      events.append(create_event('doorOpen', [ET.NO_ENTRY, ET.SOFT_DISABLE]))
    if ret.seatbeltUnlatched:
      events.append(create_event('seatbeltNotLatched', [ET.NO_ENTRY, ET.SOFT_DISABLE]))
    if ret.gearShifter == GEAR.reverse:
      events.append(create_event('reverseGear', [ET.NO_ENTRY, ET.IMMEDIATE_DISABLE]))
    if not ret.gearShifter in [GEAR.drive, GEAR.eco, GEAR.sport]:
      events.append(create_event('wrongGear', [ET.NO_ENTRY, ET.SOFT_DISABLE]))
    if self.CS.stabilityControlDisabled:
      events.append(create_event('espDisabled', [ET.NO_ENTRY, ET.SOFT_DISABLE]))
    if self.CS.parkingBrakeSet:
      events.append(create_event('parkBrake', [ET.NO_ENTRY, ET.USER_DISABLE]))

    # Vehicle health safety checks and events
    if self.CS.accFault:
      events.append(create_event('radarFault', [ET.NO_ENTRY, ET.IMMEDIATE_DISABLE]))
    if self.CS.steeringFault:
      events.append(create_event('steerTempUnavailable', [ET.NO_ENTRY, ET.WARNING]))

    # Per the Comma safety model, disable on pedals rising edge or when brake
    # is pressed and speed isn't zero.
    if (ret.gasPressed and not self.gasPressedPrev) or \
            (ret.brakePressed and (not self.brakePressedPrev or not ret.standstill)):
      events.append(create_event('pedalPressed', [ET.NO_ENTRY, ET.USER_DISABLE]))
    if ret.gasPressed:
      events.append(create_event('pedalPressed', [ET.PRE_ENABLE]))

    # Engagement and longitudinal control using stock ACC. Make sure OP is
    # disengaged if stock ACC is disengaged.
    if not ret.cruiseState.enabled:
      events.append(create_event('pcmDisable', [ET.USER_DISABLE]))
    # Attempt OP engagement only on rising edge of stock ACC engagement.
    elif not self.cruiseStateEnabledPrev:
      events.append(create_event('pcmEnable', [ET.ENABLE]))

    ret.events = events
    ret.buttonEvents = buttonEvents
    ret.canMonoTimes = canMonoTimes

    # update previous car states
    self.gasPressedPrev = ret.gasPressed
    self.brakePressedPrev = ret.brakePressed
    self.cruiseStateEnabledPrev = ret.cruiseState.enabled
    self.displayMetricUnitsPrev = self.CS.displayMetricUnits
    self.buttonStatesPrev = self.CS.buttonStates.copy()

    self.CS.out = ret.as_reader()
    return  self.CS.out

  def apply(self, c):
    can_sends = self.CC.update(c.enabled, self.CS, self.frame, c.actuators,
                   c.hudControl.visualAlert,
                   c.hudControl.audibleAlert,
                   c.hudControl.leftLaneVisible,
                   c.hudControl.rightLaneVisible)
    self.frame += 1
    return can_sends
