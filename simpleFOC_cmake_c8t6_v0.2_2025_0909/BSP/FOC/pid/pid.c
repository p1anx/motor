
#include "pid.h"
#include "foc_base.h"

void PIDController_init(PIDController *pid, float P, float I, float D,
                        float ramp, float limit) {
  pid->P = P;
  pid->I = I;
  pid->D = D;
  pid->output_ramp = ramp; // output derivative limit [volts/second]
  pid->limit = limit;      // output supply limit     [volts]
  pid->integral_prev = 0.0;
  pid->error_prev = 0.0;
  pid->output_prev = 0.0;
  pid->timestamp_prev = _micros();
}

// PID controller function
float PIDController_update(PIDController *pid, float error) {
  // calculate the time from the last call
  unsigned long timestamp_now = _micros();
  float Ts = (timestamp_now - pid->timestamp_prev) * TIME_MICROS;
  // quick fix for strange cases (micros overflow)
  if (Ts <= 0 || Ts > 0.5)
    Ts = 1e-3;

  // u(s) = (P + I/s + Ds)e(s)
  // Discrete implementations
  // proportional part
  // u_p  = P *e(k)
  float proportional = pid->P * error;
  // Tustin transform of the integral part
  // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
  float integral =
      pid->integral_prev + pid->I * Ts * 0.5f * (error + pid->error_prev);
  // antiwindup - limit the output
  integral = _constrain(integral, -pid->limit, pid->limit);
  // Discrete derivation
  // u_dk = D(ek - ek_1)/Ts
  float derivative = pid->D * (error - pid->error_prev) / Ts;

  // sum all the components
  float output = proportional + integral + derivative;
  // antiwindup - limit the output variable

  // printf("output = %f\n", output);
  output = _constrain(output, -pid->limit, pid->limit);

  // limit the acceleration by ramping the output
  float output_rate = (output - pid->output_prev) / Ts;
  if (output_rate > pid->output_ramp)
    output = pid->output_prev + pid->output_ramp * Ts;
  else if (output_rate < -pid->output_ramp)
    output = pid->output_prev - pid->output_ramp * Ts;

  // saving for the next pass
  pid->integral_prev = integral;
  pid->output_prev = output;
  pid->error_prev = error;
  pid->timestamp_prev = timestamp_now;
  return output;
}

// // PID controller function
// float PIDController_update(PIDController *pid, float error) {
//   // calculate the time from the last call
//   unsigned long timestamp_now = _micros();
//   float Ts = (timestamp_now - pid->timestamp_prev) * TIME_MICROS;
//   // quick fix for strange cases (micros overflow)
//   if (Ts <= 0 || Ts > 0.5)
//     Ts = 1e-3;
//
//   // u(s) = (P + I/s + Ds)e(s)
//   // Discrete implementations
//   // proportional part
//   // u_p  = P *e(k)
//   float proportional = pid->P * error;
//   // Tustin transform of the integral part
//   // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
//   float integral =
//       pid->integral_prev + pid->I * Ts * 0.5f * (error + pid->error_prev);
//   // antiwindup - limit the output
//   integral = _constrain(integral, -pid->limit, pid->limit);
//   // Discrete derivation
//   // u_dk = D(ek - ek_1)/Ts
//   float derivative = pid->D * (error - pid->error_prev) / Ts;
//
//   // sum all the components
//   float output = proportional + integral + derivative;
//   // antiwindup - limit the output variable
//
//   output = _constrain(output, -pid->limit, pid->limit);
//
//   // limit the acceleration by ramping the output
//   float output_rate = (output - pid->output_prev) / Ts;
//   if (output_rate > pid->output_ramp)
//     output = pid->output_prev + pid->output_ramp * Ts;
//   else if (output_rate < -pid->output_ramp)
//     output = pid->output_prev - pid->output_ramp * Ts;
//
//   // saving for the next pass
//   pid->integral_prev = integral;
//   pid->output_prev = output;
//   pid->error_prev = error;
//   pid->timestamp_prev = timestamp_now;
//   return output;
// }
