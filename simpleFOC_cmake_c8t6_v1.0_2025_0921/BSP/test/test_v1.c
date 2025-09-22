#include "test_v1.h"
#include "currentSense.h"
#include "foc_base.h"
#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_adc_ex.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_tim_ex.h"
#include "test.h"
#include "tim.h"
#include "BLDCDriver.h"
#include "BLDCMotor.h"
#include "adc.h"

int adc_flag;
void test_pwm();
void test_openloop_velocity_current(void);

void test_openloop_velocity(void);
void test_v1_main(void)
{
    printf("hello cmake\n");
    HAL_Delay(1000);
    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    // HAL_Delay(1000);
    // test_openloop_velocity();
    test_openloop_velocity_current();
}
void test_pwm()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&htim1, 71);
    __HAL_TIM_SET_AUTORELOAD(&htim1, 100);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 20);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 30);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 40);
}
void test_openloop_velocity(void)
{

    BLDCMotor_t motor;
    BLDCDriver_t driver;
    Sensor_t sensor;

    BLDCMotor_init(&motor, 7);
    BLDCDriver3PWM_init(&driver, 12, 8);
    BLDCMotor_linkDriver(&motor, &driver);
    // FOCMotor_linkSensor(&motor, &sensor);

    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    printf("pp = %d\n", motor.foc_motor.pole_pairs);
    printf("power = %f v\n", motor.driver->voltage_power_supply);
    // printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&htim1, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim1, 3599);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 200 - 1);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 300 - 1);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 400 - 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);

    HAL_ADC_Start_IT(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc2);
    // driver.functions->enable(&driver);
    // BLDCMotor_setPhaseVoltage(&motor, 2, 0, _3PI_2);
    HAL_TIM_Base_Start_IT(&htim2);

    uint16_t ADC_Value;
    uint16_t adc_v[3];
    _delay(1000);
    static int i = 0;
    while (1)
    {
        BLDCMotor_move(&motor, 1);
    }
}

void test_openloop_velocity_current(void)
{

    BLDCMotor_t motor;
    BLDCDriver_t driver;
    Sensor_t sensor;

    BLDCMotor_init(&motor, 7);
    BLDCDriver3PWM_init(&driver, 12, 8);
    BLDCMotor_linkDriver(&motor, &driver);
    // FOCMotor_linkSensor(&motor, &sensor);

    motor.foc_motor.controller = ControlType_velocity_openloop;
    motor.foc_motor.foc_modulation = FOCModulationType_SpaceVectorPWM;

    printf("pp = %d\n", motor.foc_motor.pole_pairs);
    printf("power = %f v\n", motor.driver->voltage_power_supply);
    // printf("foc foc_modulation = %f\n", motor.foc_motor.foc_modulation);

    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
    __HAL_TIM_SET_PRESCALER(&htim1, 0);
    __HAL_TIM_SET_AUTORELOAD(&htim1, 3599);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 200 - 1);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 300 - 1);
    // __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 400 - 1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);

    HAL_ADC_Start_IT(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc1);
    HAL_ADCEx_Calibration_Start(&hadc2);
    // driver.functions->enable(&driver);
    // BLDCMotor_setPhaseVoltage(&motor, 2, 0, _3PI_2);
    HAL_TIM_Base_Start_IT(&htim2);

    uint16_t ADC_Value;
    uint16_t adc_v[3];
    CurrentSense_t currentSense;
    CurrentSense_Init(&currentSense);
    currentSense.adc1 = &hadc1;
    currentSense.adc2 = &hadc2;
    currentSense.adc = &hadc1;
    CurrentSense_getOffset(&currentSense);

    _delay(1000);
    static int i = 0;

    float i_a, i_b, i_c;
    while (1)
    {
        BLDCMotor_move(&motor, 1);
        currentSense.adc = &hadc1;
        i_a = CurrentSense_getCurrent(&currentSense);
        currentSense.adc = &hadc2;
        i_b = CurrentSense_getCurrent(&currentSense);
        i_c = -(i_a + i_b);

        printf("%f, %f, %f\n", i_a, i_b, i_c);

        // _delay(1000);
        // HAL_ADC_Start(&hadc1);
        // if (adc_flag == 1)
        // {
        //     adc_flag = 0;
        //     uint16_t adc_v[3];
        //     int i;
        //     HAL_ADC_Start(&hadc1);
        //     HAL_ADC_PollForConversion(&hadc1, 20);
        //     if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
        //     {
        //
        //         // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        //         // printf("ADC1 Reading : %d \r\n", ADC_Value);
        //         // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        //         i = 0;
        //         adc_v[i] = HAL_ADC_GetValue(&hadc1);
        //         printf("adc %d : %d \r\n", i, adc_v[i]);
        //         printf("adc %d : %.4f \r\n", i, adc_v[i] * 3.3f / 4096);
        //         // i++;
        //         // if (i == 3)
        //         // {
        //         //     i = 0;
        //         // }
        //         HAL_ADC_Start_IT(&hadc1);
        //     }
        // }
        // uint16_t adc_v[3];
        // int i;
        // HAL_ADC_Start(&hadc1);
        // HAL_ADC_PollForConversion(&hadc1, 20);
        // if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
        // {
        //
        //     // ADC_Value = HAL_ADC_GetValue(&hadc1); // 获取AD值
        //     // printf("ADC1 Reading : %d \r\n", ADC_Value);
        //     // printf("True Voltage value : %.4f \r\n", ADC_Value * 3.3f / 4096);
        //     i = 0;
        //     adc_v[i] = HAL_ADC_GetValue(&hadc1);
        //     // printf("adc %d : %d \r\n", i, adc_v[i]);
        //     // printf("adc %d : %.4f \r\n", i, adc_v[i] * 3.3f / 4096);
        // }
        // HAL_ADC_Start(&hadc2);
        // HAL_ADC_PollForConversion(&hadc2, 20);
        // if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
        // {
        //
        //     i = 1;
        //     adc_v[i] = HAL_ADC_GetValue(&hadc2);
        //     // printf("adc %d : %d \r\n", i, adc_v[i]);
        //     // printf("adc %d : %.4f \r\n", i, adc_v[i] * 3.3f / 4096);
        // }
        // float ua = adc_v[0] * 3.3f / 4096;
        // float ub = adc_v[1] * 3.3f / 4096;
        // float uc = -(ua + ub);
        // printf("%.4f,%.4f, %.4f\n", ua, ub, uc);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2)
    {
        // uint16_t adc_v[3];
        // int i;
        // HAL_ADC_Start(&hadc1);
        // HAL_ADC_PollForConversion(&hadc1, 20);
        // if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
        // {
        //
        //     i = 0;
        //     adc_v[i] = HAL_ADC_GetValue(&hadc1);
        //     printf("adc %d : %d \r\n", i, adc_v[i]);
        //     printf("adc %d : %.4f \r\n", i, adc_v[i] * 3.3f / 4096);
        // }
        // HAL_ADC_Start(&hadc2);
        // HAL_ADC_PollForConversion(&hadc2, 20);
        // if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC))
        // {
        //
        //     i = 1;
        //     adc_v[i] = HAL_ADC_GetValue(&hadc2);
        //     printf("adc %d : %d \r\n", i, adc_v[i]);
        //     printf("adc %d : %.4f \r\n", i, adc_v[i] * 3.3f / 4096);
        // }
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    // if (hadc == &hadc1)
    // {
    //     uint16_t adc_v[3];
    //     int i;
    //     i = 0;
    //     adc_v[i] = HAL_ADC_GetValue(&hadc1);
    //     printf("adc %d : %d \r\n", i, adc_v[i]);
    //     printf("adc %d : %.4f \r\n", i, (float)adc_v[i] * 3.3f / 4096);
    //     adc_flag = 1;
    //     HAL_ADC_Start_IT(&hadc1);
    // }
}
