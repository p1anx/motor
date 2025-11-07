#include "foc_base.h"
#include "BLDCMotor.h"
#include "foc_motor.h"
#include "math.h"
#include "pwm.h"
#include "mymain.h"

#define PP 7
int pwmA, pwmB, pwmC;
int pole_pairs = 7;
float voltage_limit = 10;
float voltage_power_supply = 12.0;
float velocity_limit = 5;
float shaft_angle = 0, open_loop_timestamp = 0;
float zero_electric_angle = 0, Ualpha, Ubeta = 0, Ua = 0, Ub = 0, Uc = 0, dc_a = 0, dc_b = 0, dc_c = 0;

int foc_modulation = SinePWM;

void _delay(unsigned long ms)
{
    HAL_Delay(ms);
}

unsigned long _micros(void)
{
    return HAL_GetTick(); // get microseconds
}

void _writeDutyCycle3PWM(float dc_a, float dc_b, float dc_c, int pinA, int pinB, int pinC)
{
    // transform duty cycle from [0,1] to [0,4095]
    // TIM1->CCR1 = (int)(dc_a * _PWM_RANGE);
    //	TIM1->CCR2 = (int)(dc_b * _PWM_RANGE);
    //	TIM4->CCR4 = (int)(dc_c * _PWM_RANGE);

    int pwm_range = pwm_tim.Instance->ARR;
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_1, dc_a * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_4, dc_b * pwm_range);
    __HAL_TIM_SET_COMPARE(&pwm_tim, TIM_CHANNEL_4, dc_c * pwm_range);
}
// int array instead of float array
// 2x storage save (int 2Byte float 4 Byte )
// sin*10000
int sine_array[200] = {0,    79,   158,  237,  316,  395,  473,  552,  631,  710,  789,  867,  946,  1024, 1103, 1181, 1260, 1338, 1416, 1494, 1572, 1650, 1728, 1806, 1883,  1961, 2038, 2115, 2192,
                       2269, 2346, 2423, 2499, 2575, 2652, 2728, 2804, 2879, 2955, 3030, 3105, 3180, 3255, 3329, 3404, 3478, 3552, 3625, 3699, 3772, 3845, 3918, 3990, 4063,  4135, 4206, 4278, 4349,
                       4420, 4491, 4561, 4631, 4701, 4770, 4840, 4909, 4977, 5046, 5113, 5181, 5249, 5316, 5382, 5449, 5515, 5580, 5646, 5711, 5775, 5839, 5903, 5967, 6030,  6093, 6155, 6217, 6279,
                       6340, 6401, 6461, 6521, 6581, 6640, 6699, 6758, 6815, 6873, 6930, 6987, 7043, 7099, 7154, 7209, 7264, 7318, 7371, 7424, 7477, 7529, 7581, 7632, 7683,  7733, 7783, 7832, 7881,
                       7930, 7977, 8025, 8072, 8118, 8164, 8209, 8254, 8298, 8342, 8385, 8428, 8470, 8512, 8553, 8594, 8634, 8673, 8712, 8751, 8789, 8826, 8863, 8899, 8935,  8970, 9005, 9039, 9072,
                       9105, 9138, 9169, 9201, 9231, 9261, 9291, 9320, 9348, 9376, 9403, 9429, 9455, 9481, 9506, 9530, 9554, 9577, 9599, 9621, 9642, 9663, 9683, 9702, 9721,  9739, 9757, 9774, 9790,
                       9806, 9821, 9836, 9850, 9863, 9876, 9888, 9899, 9910, 9920, 9930, 9939, 9947, 9955, 9962, 9969, 9975, 9980, 9985, 9989, 9992, 9995, 9997, 9999, 10000, 10000};

// function approximating the sine calculation by using fixed size array
// ~40us (float array)
// ~50us (int array)
// precision +-0.005
// it has to receive an angle in between 0 and 2PI
float _sin(float a)
{
    if (a < _PI_2)
    {
        // return sine_array[(int)(199.0*( a / (_PI/2.0)))];
        // return sine_array[(int)(126.6873* a)];           // float array optimized
        return 0.0001 * sine_array[_round(126.6873 * a)]; // int array optimized
    }
    else if (a < _PI)
    {
        // return sine_array[(int)(199.0*(1.0 - (a-_PI/2.0) / (_PI/2.0)))];
        // return sine_array[398 - (int)(126.6873*a)];          // float array
        // optimized
        return 0.0001 * sine_array[398 - _round(126.6873 * a)]; // int array optimized
    }
    else if (a < _3PI_2)
    {
        // return -sine_array[(int)(199.0*((a - _PI) / (_PI/2.0)))];
        // return -sine_array[-398 + (int)(126.6873*a)];           // float array
        // optimized
        return -0.0001 * sine_array[-398 + _round(126.6873 * a)]; // int array optimized
    }
    else
    {
        // return -sine_array[(int)(199.0*(1.0 - (a - 3*_PI/2) / (_PI/2.0)))];
        // return -sine_array[796 - (int)(126.6873*a)];           // float array
        // optimized
        return -0.0001 * sine_array[796 - _round(126.6873 * a)]; // int array optimized
    }
}

// function approximating cosine calculation by using fixed size array
// ~55us (float array)
// ~56us (int array)
// precision +-0.005
// it has to receive an angle in between 0 and 2PI
float _cos(float a)
{
    float a_sin = a + _PI_2;
    a_sin = a_sin > _2PI ? a_sin - _2PI : a_sin;
    return _sin(a_sin);
}

// normalizing radian angle to [0,2PI]
float _normalizeAngle(float angle)
{
    float a = fmod(angle, _2PI);
    return a >= 0 ? a : (a + _2PI);
}
// Electrical angle calculation
float _electricalAngle(float shaft_angle, int pole_pairs)
{
    // shaft_angle = getAngle();
    // return _normalizeAngle(shaft_angle * pole_pairs);
    return (shaft_angle * pole_pairs);
}

float getElectricalAngle(void)
{
    shaft_angle = getAngle();
    // return _normalizeAngle(shaft_angle * pole_pairs);
    return _normalizeAngle(_electricalAngle(shaft_angle, PP) - zero_electric_angle);
}
float foc_getElectricalAngle(BLDCMotor_t *motor)
{
    shaft_angle = FOCMotor_shaftAngle(&motor->foc_motor);
    // return _normalizeAngle(shaft_angle * pole_pairs);
    return _normalizeAngle(_electricalAngle(shaft_angle, PP) - zero_electric_angle);
}
// Set voltage to the pwm pin
void setPwm(float Ua, float Ub, float Uc)
{

    // limit the voltage in driver
    // printf("before ua = %f, ub = %f, uc = %f\n", Ua, Ub, Uc);
    Ua = _constrain(Ua, 0.0, voltage_limit);
    Ub = _constrain(Ub, 0.0, voltage_limit);
    Uc = _constrain(Uc, 0.0, voltage_limit);
    // calculate duty cycle
    // limited in [0,1]
    float dc_a = _constrain(Ua / voltage_power_supply, 0.0, 1.0);
    float dc_b = _constrain(Ub / voltage_power_supply, 0.0, 1.0);
    float dc_c = _constrain(Uc / voltage_power_supply, 0.0, 1.0);
    // printf("ua = %f, ub = %f, uc = %f\n", Ua, Ub, Uc);
    // printf("dc_a = %f, dc_b = %f, dc_c = %f\n", dc_a, dc_b, dc_c);

    // hardware specific writing
    // hardware specific function - depending on driver and mcu
    _writeDutyCycle3PWM(dc_a, dc_b, dc_c, pwmA, pwmB, pwmC);
}
// Method using FOC to set Uq and Ud to the motor at the optimal angle
// Function implementing Space Vector PWM and Sine PWM algorithms
//
// Function using sine approximation
// regular sin + cos ~300us    (no memory usaage)
// approx  _sin + _cos ~110us  (400Byte ~ 20% of memory)
void setPhaseVoltage(float Uq, float Ud, float angle_el)
{

    const int centered = 1;
    int sector;
    float _ca, _sa;

    switch (foc_modulation)
    {
    case Trapezoid_120:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 5
        static int trap_120_map[6][3] = {
            {0, 1, -1}, {-1, 1, 0}, {-1, 0, 1}, {0, -1, 1},
            {1, -1, 0}, {1, 0, -1} // each is 60 degrees with values for 3 phases of
                                   // 1=positive -1=negative 0=high-z
        };
        // static int trap_120_state = 0;
        sector = 6 * (_normalizeAngle(angle_el + _PI / 6.0 + zero_electric_angle) / _2PI); // adding PI/6 to align with other modes

        Ua = Uq + trap_120_map[sector][0] * Uq;
        Ub = Uq + trap_120_map[sector][1] * Uq;
        Uc = Uq + trap_120_map[sector][2] * Uq;

        if (centered)
        {
            Ua += (voltage_limit) / 2 - Uq;
            Ub += (voltage_limit) / 2 - Uq;
            Uc += (voltage_limit) / 2 - Uq;
        }
        break;

    case Trapezoid_150:
        // see https://www.youtube.com/watch?v=InzXA7mWBWE Slide 8
        static int trap_150_map[12][3] = {
            {0, 1, -1}, {-1, 1, -1}, {-1, 1, 0},  {-1, 1, 1}, {-1, 0, 1}, {-1, -1, 1}, {0, -1, 1},
            {1, -1, 1}, {1, -1, 0},  {1, -1, -1}, {1, 0, -1}, {1, 1, -1} // each is 30 degrees with values for 3 phases
                                                                         // of 1=positive -1=negative 0=high-z
        };
        // static int trap_150_state = 0;
        sector = 12 * (_normalizeAngle(angle_el + _PI / 6.0 + zero_electric_angle) / _2PI); // adding PI/6 to align with other modes

        Ua = Uq + trap_150_map[sector][0] * Uq;
        Ub = Uq + trap_150_map[sector][1] * Uq;
        Uc = Uq + trap_150_map[sector][2] * Uq;

        // center
        if (centered)
        {
            Ua += (voltage_limit) / 2 - Uq;
            Ub += (voltage_limit) / 2 - Uq;
            Uc += (voltage_limit) / 2 - Uq;
        }

        break;

    case SinePWM:
        // Sinusoidal PWM modulation
        // Inverse Park + Clarke transformation

        // angle normalization in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + zero_electric_angle);
        _ca = _cos(angle_el);
        _sa = _sin(angle_el);
        // Inverse park transform
        Ualpha = _ca * Ud - _sa * Uq; // -sin(angle) * Uq;
        Ubeta = _sa * Ud + _ca * Uq;  //  cos(angle) * Uq;

        // Clarke transform
        Ua = Ualpha + voltage_limit / 2;
        Ub = -0.5 * Ualpha + _SQRT3_2 * Ubeta + voltage_limit / 2;
        Uc = -0.5 * Ualpha - _SQRT3_2 * Ubeta + voltage_limit / 2;

        if (!centered)
        {
            float Umin = fmin(Ua, fmin(Ub, Uc));
            Ua -= Umin;
            Ub -= Umin;
            Uc -= Umin;
        }

        break;

    case SpaceVectorPWM:
        // Nice video explaining the SpaceVectorModulation (SVPWM) algorithm
        // https://www.youtube.com/watch?v=QMSWUMEAejg

        // if negative voltages change inverse the phase
        // angle + 180degrees
        if (Uq < 0)
            angle_el += _PI;
        Uq = abs(Uq);

        // angle normalisation in between 0 and 2pi
        // only necessary if using _sin and _cos - approximation functions
        angle_el = _normalizeAngle(angle_el + zero_electric_angle + _PI_2);

        // find the sector we are in currently
        sector = floor(angle_el / _PI_3) + 1;
        // calculate the duty cycles
        float T1 = _SQRT3 * _sin(sector * _PI_3 - angle_el) * Uq / voltage_limit;
        float T2 = _SQRT3 * _sin(angle_el - (sector - 1.0) * _PI_3) * Uq / voltage_limit;
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
        Ua = Ta * voltage_limit;
        Ub = Tb * voltage_limit;
        Uc = Tc * voltage_limit;
        break;
    }

    // set the voltages in driver
    setPwm(Ua, Ub, Uc);
}

void alignSensor(void)
{
    // PP = _PP;
    // DIR = _DIR;
    setPhaseVoltage(3, 0, _3PI_2);

    _delay(1000); // 需要实现延时函数

    zero_electric_angle = getElectricalAngle();
    setPhaseVoltage(0, 0, _3PI_2);

    printf("0 eAngle = %.4f\n", zero_electric_angle);
}

void Clarke_Transform(float Ia, float Ib, float *I_alpha, float *I_beta)
{
    *I_alpha = Ia;
    *I_beta = _1_SQRT3 * (Ia + 2.0f * Ib);
}

/**
 * @brief Park变换：两相静止坐标系转换为两相旋转坐标系
 */
void Park_Transform(float I_alpha, float I_beta, float theta, float *Id, float *Iq)
{
    float cos_theta = _cos(theta);
    float sin_theta = _sin(theta);

    *Id = I_alpha * cos_theta + I_beta * sin_theta;
    *Iq = -I_alpha * sin_theta + I_beta * cos_theta;
    // *Iq = I_alpha * cos_theta + I_beta * sin_theta;
    // *Id =-I_alpha * sin_theta + I_beta * cos_theta;
}
void Debug_Transformations(void)
{
    printf("=== Coordinate Transform Check ===\n");

    // 测试：给定已知三相电流
    float Ia = 1.0f, Ib = -0.5f, Ic = -0.5f, i_alpha, i_beta, i_d, i_q;

    // Clarke变换
    Clarke_Transform(Ia, Ib, &i_alpha, &i_beta);
    printf("Clarke: alpha=%.3f, beta=%.3f\n", i_alpha, i_beta);

    // Park变换（angle=0时，d=alpha, q=beta）
    Park_Transform(i_alpha, i_beta, 0, &i_d, &i_q);
    printf("Park (angle=0): d=%.3f, q=%.3f\n", i_d, i_q);

    // 验证：angle=0时，d应该≈alpha, q应该≈-beta或beta
    if (fabsf(i_d - i_alpha) > 0.1f)
    {
        printf("⚠️  Park transform may be wrong!\n");
    }
}
