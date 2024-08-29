#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                       Code generated with SymPy 1.12                       *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_904715839093012703) {
   out_904715839093012703[0] = delta_x[0] + nom_x[0];
   out_904715839093012703[1] = delta_x[1] + nom_x[1];
   out_904715839093012703[2] = delta_x[2] + nom_x[2];
   out_904715839093012703[3] = delta_x[3] + nom_x[3];
   out_904715839093012703[4] = delta_x[4] + nom_x[4];
   out_904715839093012703[5] = delta_x[5] + nom_x[5];
   out_904715839093012703[6] = delta_x[6] + nom_x[6];
   out_904715839093012703[7] = delta_x[7] + nom_x[7];
   out_904715839093012703[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_358663293418942235) {
   out_358663293418942235[0] = -nom_x[0] + true_x[0];
   out_358663293418942235[1] = -nom_x[1] + true_x[1];
   out_358663293418942235[2] = -nom_x[2] + true_x[2];
   out_358663293418942235[3] = -nom_x[3] + true_x[3];
   out_358663293418942235[4] = -nom_x[4] + true_x[4];
   out_358663293418942235[5] = -nom_x[5] + true_x[5];
   out_358663293418942235[6] = -nom_x[6] + true_x[6];
   out_358663293418942235[7] = -nom_x[7] + true_x[7];
   out_358663293418942235[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_7368800320472365502) {
   out_7368800320472365502[0] = 1.0;
   out_7368800320472365502[1] = 0;
   out_7368800320472365502[2] = 0;
   out_7368800320472365502[3] = 0;
   out_7368800320472365502[4] = 0;
   out_7368800320472365502[5] = 0;
   out_7368800320472365502[6] = 0;
   out_7368800320472365502[7] = 0;
   out_7368800320472365502[8] = 0;
   out_7368800320472365502[9] = 0;
   out_7368800320472365502[10] = 1.0;
   out_7368800320472365502[11] = 0;
   out_7368800320472365502[12] = 0;
   out_7368800320472365502[13] = 0;
   out_7368800320472365502[14] = 0;
   out_7368800320472365502[15] = 0;
   out_7368800320472365502[16] = 0;
   out_7368800320472365502[17] = 0;
   out_7368800320472365502[18] = 0;
   out_7368800320472365502[19] = 0;
   out_7368800320472365502[20] = 1.0;
   out_7368800320472365502[21] = 0;
   out_7368800320472365502[22] = 0;
   out_7368800320472365502[23] = 0;
   out_7368800320472365502[24] = 0;
   out_7368800320472365502[25] = 0;
   out_7368800320472365502[26] = 0;
   out_7368800320472365502[27] = 0;
   out_7368800320472365502[28] = 0;
   out_7368800320472365502[29] = 0;
   out_7368800320472365502[30] = 1.0;
   out_7368800320472365502[31] = 0;
   out_7368800320472365502[32] = 0;
   out_7368800320472365502[33] = 0;
   out_7368800320472365502[34] = 0;
   out_7368800320472365502[35] = 0;
   out_7368800320472365502[36] = 0;
   out_7368800320472365502[37] = 0;
   out_7368800320472365502[38] = 0;
   out_7368800320472365502[39] = 0;
   out_7368800320472365502[40] = 1.0;
   out_7368800320472365502[41] = 0;
   out_7368800320472365502[42] = 0;
   out_7368800320472365502[43] = 0;
   out_7368800320472365502[44] = 0;
   out_7368800320472365502[45] = 0;
   out_7368800320472365502[46] = 0;
   out_7368800320472365502[47] = 0;
   out_7368800320472365502[48] = 0;
   out_7368800320472365502[49] = 0;
   out_7368800320472365502[50] = 1.0;
   out_7368800320472365502[51] = 0;
   out_7368800320472365502[52] = 0;
   out_7368800320472365502[53] = 0;
   out_7368800320472365502[54] = 0;
   out_7368800320472365502[55] = 0;
   out_7368800320472365502[56] = 0;
   out_7368800320472365502[57] = 0;
   out_7368800320472365502[58] = 0;
   out_7368800320472365502[59] = 0;
   out_7368800320472365502[60] = 1.0;
   out_7368800320472365502[61] = 0;
   out_7368800320472365502[62] = 0;
   out_7368800320472365502[63] = 0;
   out_7368800320472365502[64] = 0;
   out_7368800320472365502[65] = 0;
   out_7368800320472365502[66] = 0;
   out_7368800320472365502[67] = 0;
   out_7368800320472365502[68] = 0;
   out_7368800320472365502[69] = 0;
   out_7368800320472365502[70] = 1.0;
   out_7368800320472365502[71] = 0;
   out_7368800320472365502[72] = 0;
   out_7368800320472365502[73] = 0;
   out_7368800320472365502[74] = 0;
   out_7368800320472365502[75] = 0;
   out_7368800320472365502[76] = 0;
   out_7368800320472365502[77] = 0;
   out_7368800320472365502[78] = 0;
   out_7368800320472365502[79] = 0;
   out_7368800320472365502[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_4013165196797027769) {
   out_4013165196797027769[0] = state[0];
   out_4013165196797027769[1] = state[1];
   out_4013165196797027769[2] = state[2];
   out_4013165196797027769[3] = state[3];
   out_4013165196797027769[4] = state[4];
   out_4013165196797027769[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_4013165196797027769[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_4013165196797027769[7] = state[7];
   out_4013165196797027769[8] = state[8];
}
void F_fun(double *state, double dt, double *out_5916203413356354496) {
   out_5916203413356354496[0] = 1;
   out_5916203413356354496[1] = 0;
   out_5916203413356354496[2] = 0;
   out_5916203413356354496[3] = 0;
   out_5916203413356354496[4] = 0;
   out_5916203413356354496[5] = 0;
   out_5916203413356354496[6] = 0;
   out_5916203413356354496[7] = 0;
   out_5916203413356354496[8] = 0;
   out_5916203413356354496[9] = 0;
   out_5916203413356354496[10] = 1;
   out_5916203413356354496[11] = 0;
   out_5916203413356354496[12] = 0;
   out_5916203413356354496[13] = 0;
   out_5916203413356354496[14] = 0;
   out_5916203413356354496[15] = 0;
   out_5916203413356354496[16] = 0;
   out_5916203413356354496[17] = 0;
   out_5916203413356354496[18] = 0;
   out_5916203413356354496[19] = 0;
   out_5916203413356354496[20] = 1;
   out_5916203413356354496[21] = 0;
   out_5916203413356354496[22] = 0;
   out_5916203413356354496[23] = 0;
   out_5916203413356354496[24] = 0;
   out_5916203413356354496[25] = 0;
   out_5916203413356354496[26] = 0;
   out_5916203413356354496[27] = 0;
   out_5916203413356354496[28] = 0;
   out_5916203413356354496[29] = 0;
   out_5916203413356354496[30] = 1;
   out_5916203413356354496[31] = 0;
   out_5916203413356354496[32] = 0;
   out_5916203413356354496[33] = 0;
   out_5916203413356354496[34] = 0;
   out_5916203413356354496[35] = 0;
   out_5916203413356354496[36] = 0;
   out_5916203413356354496[37] = 0;
   out_5916203413356354496[38] = 0;
   out_5916203413356354496[39] = 0;
   out_5916203413356354496[40] = 1;
   out_5916203413356354496[41] = 0;
   out_5916203413356354496[42] = 0;
   out_5916203413356354496[43] = 0;
   out_5916203413356354496[44] = 0;
   out_5916203413356354496[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_5916203413356354496[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_5916203413356354496[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5916203413356354496[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5916203413356354496[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_5916203413356354496[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_5916203413356354496[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_5916203413356354496[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_5916203413356354496[53] = -9.8000000000000007*dt;
   out_5916203413356354496[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_5916203413356354496[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_5916203413356354496[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5916203413356354496[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5916203413356354496[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_5916203413356354496[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_5916203413356354496[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_5916203413356354496[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5916203413356354496[62] = 0;
   out_5916203413356354496[63] = 0;
   out_5916203413356354496[64] = 0;
   out_5916203413356354496[65] = 0;
   out_5916203413356354496[66] = 0;
   out_5916203413356354496[67] = 0;
   out_5916203413356354496[68] = 0;
   out_5916203413356354496[69] = 0;
   out_5916203413356354496[70] = 1;
   out_5916203413356354496[71] = 0;
   out_5916203413356354496[72] = 0;
   out_5916203413356354496[73] = 0;
   out_5916203413356354496[74] = 0;
   out_5916203413356354496[75] = 0;
   out_5916203413356354496[76] = 0;
   out_5916203413356354496[77] = 0;
   out_5916203413356354496[78] = 0;
   out_5916203413356354496[79] = 0;
   out_5916203413356354496[80] = 1;
}
void h_25(double *state, double *unused, double *out_1315116038371644190) {
   out_1315116038371644190[0] = state[6];
}
void H_25(double *state, double *unused, double *out_1700777048979036122) {
   out_1700777048979036122[0] = 0;
   out_1700777048979036122[1] = 0;
   out_1700777048979036122[2] = 0;
   out_1700777048979036122[3] = 0;
   out_1700777048979036122[4] = 0;
   out_1700777048979036122[5] = 0;
   out_1700777048979036122[6] = 1;
   out_1700777048979036122[7] = 0;
   out_1700777048979036122[8] = 0;
}
void h_24(double *state, double *unused, double *out_4177045402045008997) {
   out_4177045402045008997[0] = state[4];
   out_4177045402045008997[1] = state[5];
}
void H_24(double *state, double *unused, double *out_471872550026463444) {
   out_471872550026463444[0] = 0;
   out_471872550026463444[1] = 0;
   out_471872550026463444[2] = 0;
   out_471872550026463444[3] = 0;
   out_471872550026463444[4] = 1;
   out_471872550026463444[5] = 0;
   out_471872550026463444[6] = 0;
   out_471872550026463444[7] = 0;
   out_471872550026463444[8] = 0;
   out_471872550026463444[9] = 0;
   out_471872550026463444[10] = 0;
   out_471872550026463444[11] = 0;
   out_471872550026463444[12] = 0;
   out_471872550026463444[13] = 0;
   out_471872550026463444[14] = 1;
   out_471872550026463444[15] = 0;
   out_471872550026463444[16] = 0;
   out_471872550026463444[17] = 0;
}
void h_30(double *state, double *unused, double *out_1590310100656150079) {
   out_1590310100656150079[0] = state[4];
}
void H_30(double *state, double *unused, double *out_4219110007486284749) {
   out_4219110007486284749[0] = 0;
   out_4219110007486284749[1] = 0;
   out_4219110007486284749[2] = 0;
   out_4219110007486284749[3] = 0;
   out_4219110007486284749[4] = 1;
   out_4219110007486284749[5] = 0;
   out_4219110007486284749[6] = 0;
   out_4219110007486284749[7] = 0;
   out_4219110007486284749[8] = 0;
}
void h_26(double *state, double *unused, double *out_3778597743357140337) {
   out_3778597743357140337[0] = state[7];
}
void H_26(double *state, double *unused, double *out_2040726269895020102) {
   out_2040726269895020102[0] = 0;
   out_2040726269895020102[1] = 0;
   out_2040726269895020102[2] = 0;
   out_2040726269895020102[3] = 0;
   out_2040726269895020102[4] = 0;
   out_2040726269895020102[5] = 0;
   out_2040726269895020102[6] = 0;
   out_2040726269895020102[7] = 1;
   out_2040726269895020102[8] = 0;
}
void h_27(double *state, double *unused, double *out_1918431923048121707) {
   out_1918431923048121707[0] = state[3];
}
void H_27(double *state, double *unused, double *out_2044346695685859838) {
   out_2044346695685859838[0] = 0;
   out_2044346695685859838[1] = 0;
   out_2044346695685859838[2] = 0;
   out_2044346695685859838[3] = 1;
   out_2044346695685859838[4] = 0;
   out_2044346695685859838[5] = 0;
   out_2044346695685859838[6] = 0;
   out_2044346695685859838[7] = 0;
   out_2044346695685859838[8] = 0;
}
void h_29(double *state, double *unused, double *out_3438403775131955419) {
   out_3438403775131955419[0] = state[1];
}
void H_29(double *state, double *unused, double *out_4729341351800676933) {
   out_4729341351800676933[0] = 0;
   out_4729341351800676933[1] = 1;
   out_4729341351800676933[2] = 0;
   out_4729341351800676933[3] = 0;
   out_4729341351800676933[4] = 0;
   out_4729341351800676933[5] = 0;
   out_4729341351800676933[6] = 0;
   out_4729341351800676933[7] = 0;
   out_4729341351800676933[8] = 0;
}
void h_28(double *state, double *unused, double *out_558804392038751879) {
   out_558804392038751879[0] = state[0];
}
void H_28(double *state, double *unused, double *out_353057665268853641) {
   out_353057665268853641[0] = 1;
   out_353057665268853641[1] = 0;
   out_353057665268853641[2] = 0;
   out_353057665268853641[3] = 0;
   out_353057665268853641[4] = 0;
   out_353057665268853641[5] = 0;
   out_353057665268853641[6] = 0;
   out_353057665268853641[7] = 0;
   out_353057665268853641[8] = 0;
}
void h_31(double *state, double *unused, double *out_5413419336651016054) {
   out_5413419336651016054[0] = state[8];
}
void H_31(double *state, double *unused, double *out_2666934372128371578) {
   out_2666934372128371578[0] = 0;
   out_2666934372128371578[1] = 0;
   out_2666934372128371578[2] = 0;
   out_2666934372128371578[3] = 0;
   out_2666934372128371578[4] = 0;
   out_2666934372128371578[5] = 0;
   out_2666934372128371578[6] = 0;
   out_2666934372128371578[7] = 0;
   out_2666934372128371578[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_904715839093012703) {
  err_fun(nom_x, delta_x, out_904715839093012703);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_358663293418942235) {
  inv_err_fun(nom_x, true_x, out_358663293418942235);
}
void car_H_mod_fun(double *state, double *out_7368800320472365502) {
  H_mod_fun(state, out_7368800320472365502);
}
void car_f_fun(double *state, double dt, double *out_4013165196797027769) {
  f_fun(state,  dt, out_4013165196797027769);
}
void car_F_fun(double *state, double dt, double *out_5916203413356354496) {
  F_fun(state,  dt, out_5916203413356354496);
}
void car_h_25(double *state, double *unused, double *out_1315116038371644190) {
  h_25(state, unused, out_1315116038371644190);
}
void car_H_25(double *state, double *unused, double *out_1700777048979036122) {
  H_25(state, unused, out_1700777048979036122);
}
void car_h_24(double *state, double *unused, double *out_4177045402045008997) {
  h_24(state, unused, out_4177045402045008997);
}
void car_H_24(double *state, double *unused, double *out_471872550026463444) {
  H_24(state, unused, out_471872550026463444);
}
void car_h_30(double *state, double *unused, double *out_1590310100656150079) {
  h_30(state, unused, out_1590310100656150079);
}
void car_H_30(double *state, double *unused, double *out_4219110007486284749) {
  H_30(state, unused, out_4219110007486284749);
}
void car_h_26(double *state, double *unused, double *out_3778597743357140337) {
  h_26(state, unused, out_3778597743357140337);
}
void car_H_26(double *state, double *unused, double *out_2040726269895020102) {
  H_26(state, unused, out_2040726269895020102);
}
void car_h_27(double *state, double *unused, double *out_1918431923048121707) {
  h_27(state, unused, out_1918431923048121707);
}
void car_H_27(double *state, double *unused, double *out_2044346695685859838) {
  H_27(state, unused, out_2044346695685859838);
}
void car_h_29(double *state, double *unused, double *out_3438403775131955419) {
  h_29(state, unused, out_3438403775131955419);
}
void car_H_29(double *state, double *unused, double *out_4729341351800676933) {
  H_29(state, unused, out_4729341351800676933);
}
void car_h_28(double *state, double *unused, double *out_558804392038751879) {
  h_28(state, unused, out_558804392038751879);
}
void car_H_28(double *state, double *unused, double *out_353057665268853641) {
  H_28(state, unused, out_353057665268853641);
}
void car_h_31(double *state, double *unused, double *out_5413419336651016054) {
  h_31(state, unused, out_5413419336651016054);
}
void car_H_31(double *state, double *unused, double *out_2666934372128371578) {
  H_31(state, unused, out_2666934372128371578);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
