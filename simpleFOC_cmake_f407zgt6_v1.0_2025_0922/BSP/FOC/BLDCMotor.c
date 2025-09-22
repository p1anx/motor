
#include "BLDCMotor.h"
#include "foc_base.h"
#include "foc_motor.h"
#include "mymain.h"
#include "stm32f4xx_hal_gpio.h"

void BLDCMotor_enable(BLDCMotor_t *motor)
{
    HAL_GPIO_WritePin(motor->enable_Port, motor->enable_Pin, SET);
}
void BLDCDriver_writeDutyCycle3PWM(float dc_a, float dc_b, float dc_c)
{
    // transform duty cycle from [0,1] to [0,4095]
    // TIM1->CCR1 = (int)(dc_a * _PWM_RANGE);
    //	TIM1->CCR2 = (int)(dc_b * _PWM_RANGE);
    //	TIM4->CCR4 = (int)(dc_c * _PWM_RANGE);

    // int pwm_range = htim1.Instance->ARR;
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dc_a * pwm_range);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, dc_b * pwm_range);
    // pwm_range = htim4.Instance->ARR;
    // __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, dc_c * pwm_range);
}
void BLDCDriver3PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc)
{

    // limit the voltage in driver
    Ua = _constrain(Ua, 0.0, driver->voltage_limit);
    Ub = _constrain(Ub, 0.0, driver->voltage_limit);
    Uc = _constrain(Uc, 0.0, driver->voltage_limit);
    // calculate duty cycle
    // limited in [0,1]
    float dc_a = _constrain(Ua / driver->voltage_power_supply, 0.0, 1.0);
    float dc_b = _constrain(Ub / driver->voltage_power_supply, 0.0, 1.0);
    float dc_c = _constrain(Uc / driver->voltage_power_supply, 0.0, 1.0);

    // hardware specific writing
    // hardware specific function - depending on driver and mcu
    // _writeDutyCycle3PWM(dc_a, dc_b, dc_c, driver->pwmA, driver->pwmB,
    //                     driver->pwmC);
    BLDCDriver_writeDutyCycle3PWM(dc_a, dc_b, dc_c);
}
void BLDCDriver_writeDutyCycle6PWM(float dc_a, float dc_b, float dc_c)
{

    int pwm_range = pwm_tim.Instance->ARR;
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, dc_a * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_2, dc_b * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_3, dc_c * pwm_range);
}
void BLDCDriver6PWM_setPwm(BLDCDriver_t *driver, float Ua, float Ub, float Uc)
{

    // limit the voltage in driver
    Ua = _constrain(Ua, 0.0, driver->voltage_limit);
    Ub = _constrain(Ub, 0.0, driver->voltage_limit);
    Uc = _constrain(Uc, 0.0, driver->voltage_limit);
    // calculate duty cycle
    // limited in [0,1]
    float dc_a = _constrain(Ua / driver->voltage_power_supply, 0.0, 1.0);
    float dc_b = _constrain(Ub / driver->voltage_power_supply, 0.0, 1.0);
    float dc_c = _constrain(Uc / driver->voltage_power_supply, 0.0, 1.0);

    // hardware specific writing
    // hardware specific function - depending on driver and mcu
    // _writeDutyCycle3PWM(dc_a, dc_b, dc_c, driver->pwmA, driver->pwmB,
    //                     driver->pwmC);
    BLDCDriver_writeDutyCycle6PWM(dc_a, dc_b, dc_c);
}
void BLDCMotor_setPhaseVoltage(BLDCMotor_t *motor, float Uq, float Ud, float angle_el)
{
    if (!motor || !motor->driver)
        return;

    // Check if this is a 6PWM driver by checking function pointer
    // For 6PWM drivers, we can use centered modulation more effectively
    const int centered = 1;
    int sector;
    float _ca, _sa;

    switch (motor->foc_motor.foc_modulation)
    {
    case FOCModulationType_Trapezoid_120:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 5
        {
            static int trap_120_map[6][3] = {
                {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
                {1, -1, 0}, {1, 0, -1} // each is 60 degrees with values for 3 phases
                                       // of 1=positive -1=negative 0=high-z
            };
            // static int trap_120_state = 0;
            sector = (int)(6 * (_normalizeAngle(angle_el + _PI / 6.0f + motor->foc_motor.zero_electric_angle) / _2PI)); // adding PI/6 to align with other modes

            motor->Ua = Uq + trap_120_map[sector][0] * Uq;
            motor->Ub = Uq + trap_120_map[sector][1] * Uq;
            motor->Uc = Uq + trap_120_map[sector][2] * Uq;

            if (centered)
            {
                motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
            }
        }
        break;

    case FOCModulationType_Trapezoid_150:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 8
        {
            static int trap_150_map[12][3] = {
                {0, 1, -1}, {-1, 1, -1}, {-1, 1, 0},  {-1, 1, 1}, {-1, 0, 1}, {-1, -1, 1}, {0, -1, 1},
                {1, -1, 1}, {1, -1, 0},  {1, -1, -1}, {1, 0, -1}, {1, 1, -1} // each is 30 degrees with values for 3 phases
                                                                             // of 1=positive -1=negative 0=high-z
            };
            // static int trap_150_state = 0;
            sector = (int)(12 * (_normalizeAngle(angle_el + _PI / 6.0f + motor->foc_motor.zero_electric_angle) / _2PI)); // adding PI/6 to align with other modes

            motor->Ua = Uq + trap_150_map[sector][0] * Uq;
            motor->Ub = Uq + trap_150_map[sector][1] * Uq;
            motor->Uc = Uq + trap_150_map[sector][2] * Uq;

            // center
            if (centered)
            {
                motor->Ua += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Ub += (motor->driver->voltage_limit) / 2 - Uq;
                motor->Uc += (motor->driver->voltage_limit) / 2 - Uq;
            }
        }
        break;

    case FOCModulationType_SinePWM:
        // Sinusoidal PWM modulation
        // Inverse Park + Clarke transformation

        // angle normalization in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle);
        _ca = _cos(angle_el);
        _sa = _sin(angle_el);
        // Inverse park transform
        motor->Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
        motor->Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

        // Clarke transform
        // Check if using 6PWM driver - can handle bipolar voltages better
        // For 6PWM, we can center around 0V instead of VCC/2
        motor->Ua = motor->Ualpha;
        motor->Ub = -0.5f * motor->Ualpha + _SQRT3_2 * motor->Ubeta;
        motor->Uc = -0.5f * motor->Ualpha - _SQRT3_2 * motor->Ubeta;

        // For compatibility with 3PWM drivers, add offset if needed
        if (centered)
        {
            motor->Ua += motor->driver->voltage_limit / 2;
            motor->Ub += motor->driver->voltage_limit / 2;
            motor->Uc += motor->driver->voltage_limit / 2;
        }

        if (!centered)
        {
            float Umin = fminf(motor->Ua, fminf(motor->Ub, motor->Uc));
            motor->Ua -= Umin;
            motor->Ub -= Umin;
            motor->Uc -= Umin;
        }

        // printf("it's in setPhaseVoltage\n");
        break;

    case FOCModulationType_SpaceVectorPWM:
        // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
        // https://www.youtube.com/watch?v=QMSWUMEAejg

        // if negative voltages change inverse the phase
        // angle + 180degrees
        if (Uq < 0)
            angle_el += _PI;
        Uq = fabsf(Uq);

        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + motor->foc_motor.zero_electric_angle + _PI_2);

        // find the sector we are in currently
        sector = (int)floorf(angle_el / _PI_3) + 1;
        // calculate the duty cycles
        float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / motor->driver->voltage_limit;
        float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0f) * _PI_3) * Uq / motor->driver->voltage_limit;
        // two versions possible
        float T0 = 0; // pulled to 0 - better for low power supply voltage
        if (centered)
        {
            T0 = 1 - T1 - T2; // centered around driver->voltage_limit/2
        }

        // calculate the duty cycles(times)
        float Ta, Tb, Tc;
        switch (sector)
        {
        case 1:
            Ta = T1 + T2 + T0 / 2;
            Tb = T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 2:
            Ta = T1 + T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T0 / 2;
            break;
        case 3:
            Ta = T0 / 2;
            Tb = T1 + T2 + T0 / 2;
            Tc = T2 + T0 / 2;
            break;
        case 4:
            Ta = T0 / 2;
            Tb = T1 + T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 5:
            Ta = T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T2 + T0 / 2;
            break;
        case 6:
            Ta = T1 + T2 + T0 / 2;
            Tb = T0 / 2;
            Tc = T1 + T0 / 2;
            break;
        default:
            // possible error state
            Ta = 0;
            Tb = 0;
            Tc = 0;
        }

        // calculate the phase voltages and center
        motor->Ua = Ta * motor->driver->voltage_limit;
        motor->Ub = Tb * motor->driver->voltage_limit;
        motor->Uc = Tc * motor->driver->voltage_limit;
        break;
    }

    // set the voltages in driver
    // Use the driver's function pointer table for flexibility (3PWM, 6PWM, etc.)
    // if (motor->driver && motor->driver->functions && motor->driver->functions->setPwm)
    // {
    //     motor->driver->functions->setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
    // }
    // printf("ua = %f, ub = %f, uc = %f\n", motor->Ua, motor->Ub, motor->Uc);
    BLDCDriver6PWM_setPwm(motor->driver, motor->Ua, motor->Ub, motor->Uc);
}

void BLDCMotor_init(BLDCMotor_t *motor, int pp)
{
    if (!motor)
        return;

    // Initialize base FOCMotor
    FOCMotor_init(&motor->foc_motor);

    // save pole pairs number
    motor->foc_motor.pole_pairs = pp;

    // Initialize phase voltages
    motor->Ua = 0;
    motor->Ub = 0;
    motor->Uc = 0;
    motor->Ualpha = 0;
    motor->Ubeta = 0;

    // Initialize driver pointer
    motor->driver = NULL;

    // Initialize open loop timestamp
    motor->open_loop_timestamp = 0;
}

void BLDCMotor_linkDriver(BLDCMotor_t *motor, BLDCDriver_t *_driver)
{
    if (!motor)
        return;
    motor->driver = _driver;
    printf("linked to driver\n");
}

void BLDCMotor_velocityOpenloop(BLDCMotor_t *motor, float target_velocity)
{
    if (!motor)
        return;

    // get current timestamp
    unsigned long now_us = _micros();
    // calculate the sample time from last call
    float Ts = (now_us - motor->open_loop_timestamp) * 1e-3f;

    // calculate the necessary angle to achieve target velocity
    motor->foc_motor.shaft_angle = _normalizeAngle(motor->foc_motor.shaft_angle + target_velocity * Ts);

    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0, _electricalAngle(motor->foc_motor.shaft_angle, motor->foc_motor.pole_pairs));

    // save timestamp for next call
    motor->open_loop_timestamp = now_us;
}

void BLDCMotor_angleOpenloop(BLDCMotor_t *motor, float target_angle)
{
    if (!motor)
        return;

    // get current timestamp
    unsigned long now_us = _micros();
    // calculate the sample time from last call
    float Ts = (now_us - motor->open_loop_timestamp) * 1e-6f;

    // calculate the necessary angle to move from current position towards target
    // angle with maximal velocity (velocity_limit)
    if (fabsf(target_angle - motor->foc_motor.shaft_angle) > fabsf(motor->foc_motor.velocity_limit * Ts))
        motor->foc_motor.shaft_angle += _sign(target_angle - motor->foc_motor.shaft_angle) * fabsf(motor->foc_motor.velocity_limit) * Ts;
    else
        motor->foc_motor.shaft_angle = target_angle;

    // set the maximal allowed voltage (voltage_limit) with the necessary angle
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_limit, 0, _electricalAngle(motor->foc_motor.shaft_angle, motor->foc_motor.pole_pairs));

    // save timestamp for next call
    motor->open_loop_timestamp = now_us;
}

int BLDCMotor_alignSensor(BLDCMotor_t *motor)
{
    if (!motor)
        return 0;

    //  if(monitor_port) monitor_port->println("MOT: Align sensor.");
    // align the electrical phases of the motor and sensor
    // set angle -90 degrees

    float start_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    for (int i = 0; i <= 5; i++)
    {
        float angle = _3PI_2 + _2PI * i / 6.0f;
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
        _delay(200);
    }
    float mid_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    for (int i = 5; i >= 0; i--)
    {
        float angle = _3PI_2 + _2PI * i / 6.0f;
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_sensor_align, 0, angle);
        _delay(200);
    }
    if (mid_angle < start_angle)
    {
        //    if(monitor_port) monitor_port->println("MOT: natural_direction==CCW");
        if (motor->foc_motor.sensor)
        {
            motor->foc_motor.sensor->natural_direction = Direction_CCW;
        }
    }
    else if (mid_angle == start_angle)
    {
        //    if(monitor_port) monitor_port->println("MOT: Sensor failed to notice
        //    movement");
    }
    else
    {
        //    if(monitor_port) monitor_port->println("MOT: natural_direction==CW");
    }

    // let the motor stabilize for 2 sec
    _delay(2000);
    return 0;
}

// Iterative function looping FOC algorithm, setting Uq on the Motor
// The faster it can be run the better
void BLDCMotor_loopFOC(BLDCMotor_t *motor)
{
    if (!motor)
        return;

    // shaft angle
    motor->foc_motor.shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    // set the phase voltage - FOC heart function :)
    BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, _electricalAngle(motor->foc_motor.shaft_angle, motor->foc_motor.pole_pairs));
}
void BLDCMotor_move(BLDCMotor_t *motor, float new_target)
{
    if (!motor)
        return;

    // set internal target variable
    if (new_target != NOT_SET)
        motor->foc_motor.target = new_target;
    // get angular velocity
    motor->foc_motor.shaft_velocity = FOCMotor_shaftVelocity(&motor->foc_motor);
    // choose control loop
    switch (motor->foc_motor.controller)
    {
    case ControlType_voltage:
        motor->foc_motor.voltage_q = motor->foc_motor.target;
        break;
    case ControlType_current:
        // motor->foc_motor.i_q = motor->foc_motor.target;
        motor->foc_motor.voltage_q = PIDController_update(motor->foc_motor.PID_current_q, motor->foc_motor.target - motor->foc_motor.i_q);
        motor->foc_motor.voltage_d = PIDController_update(motor->foc_motor.PID_current_d, 0 - motor->foc_motor.i_d);
        BLDCMotor_setPhaseVoltage(motor, motor->foc_motor.voltage_q, motor->foc_motor.voltage_d, _3PI_2);
        break;
    case ControlType_angle:
        // angle set point
        // include angle loop
        motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
        motor->foc_motor.shaft_velocity_sp = PIDController_update(&motor->foc_motor.P_angle, motor->foc_motor.shaft_angle_sp - motor->foc_motor.shaft_angle);
        motor->foc_motor.voltage_q = PIDController_update(&motor->foc_motor.PID_velocity, motor->foc_motor.shaft_velocity_sp - motor->foc_motor.shaft_velocity);
        break;
    case ControlType_velocity:
        // velocity set point
        // include velocity loop
        motor->foc_motor.shaft_velocity_sp = motor->foc_motor.target;
        motor->foc_motor.voltage_q = PIDController_update(&motor->foc_motor.PID_velocity, motor->foc_motor.shaft_velocity_sp - motor->foc_motor.shaft_velocity);
        break;
    case ControlType_velocity_openloop:
        // velocity control in open loop
        // loopFOC should not be called
        motor->foc_motor.shaft_velocity_sp = motor->foc_motor.target;
        BLDCMotor_velocityOpenloop(motor, motor->foc_motor.shaft_velocity_sp);
        break;
    case ControlType_angle_openloop:
        // angle control in open loop
        // loopFOC should not be called
        motor->foc_motor.shaft_angle_sp = motor->foc_motor.target;
        BLDCMotor_angleOpenloop(motor, motor->foc_motor.shaft_angle_sp);
        break;
    }
}
