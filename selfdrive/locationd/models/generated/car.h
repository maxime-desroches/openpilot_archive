#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_904715839093012703);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_358663293418942235);
void car_H_mod_fun(double *state, double *out_7368800320472365502);
void car_f_fun(double *state, double dt, double *out_4013165196797027769);
void car_F_fun(double *state, double dt, double *out_5916203413356354496);
void car_h_25(double *state, double *unused, double *out_1315116038371644190);
void car_H_25(double *state, double *unused, double *out_1700777048979036122);
void car_h_24(double *state, double *unused, double *out_4177045402045008997);
void car_H_24(double *state, double *unused, double *out_471872550026463444);
void car_h_30(double *state, double *unused, double *out_1590310100656150079);
void car_H_30(double *state, double *unused, double *out_4219110007486284749);
void car_h_26(double *state, double *unused, double *out_3778597743357140337);
void car_H_26(double *state, double *unused, double *out_2040726269895020102);
void car_h_27(double *state, double *unused, double *out_1918431923048121707);
void car_H_27(double *state, double *unused, double *out_2044346695685859838);
void car_h_29(double *state, double *unused, double *out_3438403775131955419);
void car_H_29(double *state, double *unused, double *out_4729341351800676933);
void car_h_28(double *state, double *unused, double *out_558804392038751879);
void car_H_28(double *state, double *unused, double *out_353057665268853641);
void car_h_31(double *state, double *unused, double *out_5413419336651016054);
void car_H_31(double *state, double *unused, double *out_2666934372128371578);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}