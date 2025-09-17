#include "qam.h"
#include "stm32_hal.h"
#include "types.h"

void qam_init(QAM_t *qam)
{
    qam16_sequence(qam);
    printf("[ok] qam init\n");
}

void qam4_sequence(Motor_t *motor, QAM_t *qam)
{
    motor->target += 90;
}
void qam16_sequence(QAM_t *qam)
{
    float delta_angle = 22.5;
    for (int i = 0; i < 16; i++)
    {
        qam->angle16[i] = (i == 0) ? 0 : (qam->angle16[i - 1] + delta_angle);
    }
}
void qam16_sequence_v0(Motor_t *motor, QAM_t *qam)
{
    motor->target += 22.5;
}
void qam16_updateBits(Motor_t *motor, QAM_t *qam)
{
    static int i = 0;
    motor->target = qam->angle16[i];
    i++;
    if (i == 16)
    {
        i = 0;
    }
    // printf("target = %f\n", motor->target);
}

void qam_loop(Motor_t *motor, QAM_t *qam, int update_ms)
{
    uint32_t last_time = HAL_GetTick();
    const uint32_t loop_period = update_ms; // 500ms

    printf("going into loop\n");
    while (1)
    {
        uint32_t current_time = HAL_GetTick();

        if (current_time - last_time >= loop_period)
        {
            // 执行500ms周期任务
            // qam16_updateBits(motor, qam);
            qam4_sequence(motor, qam);
            // qam16_sequence_v0(motor, qam);

            // 更新时间戳，保持精确周期
            last_time = current_time;
            // 或者用这种方式避免累积误差：
            // last_time += loop_period;
        }
        printf("%.3f, %.3f\n", motor->target, motor->angle);

        // 其他非周期性任务可以在这里执行
    }
}

void qam_loop_template(QAM_t *qam, int ms)
{
    uint32_t last_time = HAL_GetTick();
    const uint32_t loop_period = ms; // 500ms

    while (1)
    {
        uint32_t current_time = HAL_GetTick();

        if (current_time - last_time >= loop_period)
        {
            // 执行500ms周期任务

            // 更新时间戳，保持精确周期
            last_time = current_time;
            // 或者用这种方式避免累积误差：
            // last_time += loop_period;
        }

        // 其他非周期性任务可以在这里执行
    }
}
