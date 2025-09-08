#include "motor.h"
#include "mymain.h"

extern Motor_InitStruct motor1;
extern Motor_InitStruct motor2;
extern GlobalVar_Struct gTime;
int g_tim_flag = -1;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
      /* USER CODE BEGIN Callback 1 */
        if(htim->Instance == ENCODER1_TIM){
#if MOTOR1_ON
          encoder_overflow_cnt_fun(&motor1);
#endif
        }
        else if(htim->Instance ==ENCODER2_TIM){
#if MOTOR2_ON
          encoder_overflow_cnt_fun(&motor2);
#endif
        }
        else if(htim->Instance == PID_UPDATE_TIM){
          pid();
          gTime.time_flag++;

          if(g_tim_flag == 1){
            tim_control_position();
          }
        }
    

}