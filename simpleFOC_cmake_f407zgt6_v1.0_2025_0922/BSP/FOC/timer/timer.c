#include "BLDCMotor.h"
#include "encoder_as5600.h"
#include "foc_motor.h"
#include "tim.h"
extern BLDCMotor_t motor;
extern AS5600_t as5600;
void timer_init(TIM_HandleTypeDef *tim, int update_ms)
{
    int arr = 999;
    if (HAL_TIM_Base_Start_IT(tim) != HAL_OK)
    {
        printf("failed to start pid update timer\n");
    }
    __HAL_TIM_SET_PRESCALER(tim, 8399);
    __HAL_TIM_SET_AUTORELOAD(tim, update_ms * 10 - 1);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        // motor.angle = BLDCMotor_getAngle(&motor);
        // motor.angle = FOCMotor_shaftAngle(&motor.foc_motor);
        // motor.velocity = BLDCMotor_getVelocity(&motor);
        // motor.angle = AS5600_ReadAngleRadians(&as5600);
        // printf("angle = %f\n", motor.angle);

        // BLDCMotor_run(&motor);
        // AS5600_GetAngularSpeed(&as5600, , bool update)
        // printf("hello timer\n");
    }
}
