
#include "pid.h"

#include "stm32_hal.h"

PIDController_t pid;
void PIDController_init(PIDController_t *pid, float P, float I, float D, float ramp, float limit)
{
    pid->P = P;
    pid->I = I;
    pid->D = D;
    pid->output_ramp = ramp; // output derivative limit [volts/second]
    pid->limit = limit;      // output supply limit     [volts]
    pid->integral = 0.0f;
    pid->error = 0.0f;
    pid->output = 0.0f;
    pid->timestamp = getUs();
}
void PID_init(PIDController_t *pid)
{
    pid->integral = 0.0f;
    pid->error = 0.0f;
    pid->output = 0.0f;
    pid->timestamp = getUs();

}

// PID controller function
float g_out_rate;
float PIDController_update(PIDController_t *pid, float error)
{
    // calculate the time from the last call
#ifdef IS_FIXED_TIME
    float Ts = pid->update_t;
#else
    uint32_t delta_us;
    uint32_t timestamp_now = getUs();
    if (timestamp_now > pid->timestamp) {
        delta_us = (timestamp_now - pid->timestamp);
    }
    else {
        delta_us = (0xFFFFFFFF - pid->timestamp) + timestamp_now + 1;
    }
    float Ts = (float)delta_us / 1000000.0f;
    if (Ts <= 0 || Ts > 0.5)
        Ts = 1e-3f;
#endif


    // u(s) = (P + I/s + Ds)e(s)
    // Discrete implementations
    // proportional part
    // u_p  = P *e(k)
    float proportional = pid->P * error;
    // Tustin transform of the integral part
    // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
    float integral = pid->integral + pid->I * Ts * 0.5 * (error + pid->error);
    // static float integral = 0;
    // integral += pid->I * (error);
    // antiwindup - limit the output
    integral = _constrain(integral, -pid->limit, pid->limit);
    // Discrete derivation
    // u_dk = D(ek - ek_1)/Ts
    float derivative = pid->D * (error - pid->error) / Ts;

    // sum all the components
    float output = proportional + integral + derivative;
    // antiwindup - limit the output variable
    output = _constrain(output, -pid->limit, pid->limit);

    // limit the acceleration by ramping the output
   // float output_rate = (output - pid->output_prev) / Ts;
    // g_out_rate = output_rate;
    // if (output_rate > pid->output_ramp)
    //     output = pid->output_prev + pid->output_ramp * Ts;
    // else if (output_rate < -pid->output_ramp)
    //     output = pid->output_prev - pid->output_ramp * Ts;

    // saving for the next pass
    pid->integral = integral;
    pid->output = output;
    pid->error = error;
    // pid->timestamp_prev = timestamp_now;
    return output;
}
#if WITHOUT_TIMER
float PIDController_update(PIDController *pid, float error)
{
    // calculate the time from the last call
    unsigned long timestamp_now = _micros();
    float Ts = (timestamp_now - pid->timestamp_prev) * 1e-3;
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
    float integral = pid->integral_prev + pid->I * Ts * 0.5 * (error + pid->error_prev);
    // antiwindup - limit the output
    integral = _constrain(integral, -pid->limit, pid->limit);
    // Discrete derivation
    // u_dk = D(ek - ek_1)/Ts
    float derivative = pid->D * (error - pid->error_prev) / Ts;

    // sum all the components
    float output = proportional + integral + derivative;
    // antiwindup - limit the output variable
    output = _constrain(output, -pid->limit, pid->limit);

    // limit the acceleration by ramping the output
    // float output_rate = (output - pid->output_prev) / Ts;
    // if (output_rate > pid->output_ramp)
    //     output = pid->output_prev + pid->output_ramp * Ts;
    // else if (output_rate < -pid->output_ramp)
    //     output = pid->output_prev - pid->output_ramp * Ts;

    // saving for the next pass
    pid->integral_prev = integral;
    pid->output_prev = output;
    pid->error_prev = error;
    pid->timestamp_prev = timestamp_now;
    return output;
}
#endif

