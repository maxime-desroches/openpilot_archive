#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void live_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_9(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_12(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_35(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_32(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_update_33(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void live_H(double *in_vec, double *out_5649736723393595680);
void live_err_fun(double *nom_x, double *delta_x, double *out_1817682482156157428);
void live_inv_err_fun(double *nom_x, double *true_x, double *out_2063642465723510922);
void live_H_mod_fun(double *state, double *out_8875414428119132416);
void live_f_fun(double *state, double dt, double *out_4816933065227157419);
void live_F_fun(double *state, double dt, double *out_8784471241999669549);
void live_h_4(double *state, double *unused, double *out_4227406610325000507);
void live_H_4(double *state, double *unused, double *out_536664744342837146);
void live_h_9(double *state, double *unused, double *out_1214526371738644518);
void live_H_9(double *state, double *unused, double *out_6750554190921610324);
void live_h_10(double *state, double *unused, double *out_6497611180128088395);
void live_H_10(double *state, double *unused, double *out_6689006730597112935);
void live_h_12(double *state, double *unused, double *out_8062157107846117300);
void live_H_12(double *state, double *unused, double *out_7130463569339613346);
void live_h_35(double *state, double *unused, double *out_6913039161023487124);
void live_H_35(double *state, double *unused, double *out_8570717472044924561);
void live_h_32(double *state, double *unused, double *out_2862990461008711013);
void live_H_32(double *state, double *unused, double *out_2961162097970510699);
void live_h_13(double *state, double *unused, double *out_7204966353320088844);
void live_H_13(double *state, double *unused, double *out_7539104421654297247);
void live_h_14(double *state, double *unused, double *out_1214526371738644518);
void live_H_14(double *state, double *unused, double *out_6750554190921610324);
void live_h_33(double *state, double *unused, double *out_3182937775845598389);
void live_H_33(double *state, double *unused, double *out_5420160467406066957);
void live_predict(double *in_x, double *in_P, double *in_Q, double dt);
}