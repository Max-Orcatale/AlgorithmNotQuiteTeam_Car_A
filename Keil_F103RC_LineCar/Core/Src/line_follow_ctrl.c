#include "line_follow_ctrl.h"
#include "motor_drv.h"

/* 8路权重：左负右正，居中约为0 */
static const int8_t s_weight[8] = {-7, -5, -3, -1, 1, 3, 5, 7};

static int16_t LineFollow_ComputeError(const LineSensorData_t *data, uint8_t *valid)
{
    int16_t sum_w = 0;
    int16_t sum_n = 0;
    uint8_t i;

    for (i = 0; i < 8U; i++)
    {
        /* 默认按“1=检测到黑线”处理。若模块相反，改成 data->bit[i] == 0 */
        if (data->bit[i] == 1U)
        {
            sum_w += s_weight[i];
            sum_n++;
        }
    }

    if (sum_n == 0)
    {
        *valid = 0U;
        return 0;
    }

    *valid = 1U;
    return (int16_t)(sum_w / sum_n);
}

void LineFollow_Init(LineFollowCtrl_t *ctrl)
{
    if (ctrl == NULL)
    {
        return;
    }

    ctrl->base_speed = FOLLOW_BASE_SPEED;
    ctrl->kp = FOLLOW_KP;
    ctrl->kd = FOLLOW_KD;
    ctrl->last_error = 0;
}

void LineFollow_Update(LineFollowCtrl_t *ctrl, const LineSensorData_t *data)
{
    int16_t error;
    int16_t d_error;
    int32_t turn;
    int16_t left_speed;
    int16_t right_speed;
    uint8_t valid = 0U;

    if ((ctrl == NULL) || (data == NULL))
    {
        return;
    }

    error = LineFollow_ComputeError(data, &valid);
    if (valid == 0U)
    {
        /* 丢线：轻微反拖刹车，避免冲出跑道 */
        MotorDrv_SetAllSpeed(-FOLLOW_LOST_LINE_BRAKE_SPEED,
                             -FOLLOW_LOST_LINE_BRAKE_SPEED,
                             -FOLLOW_LOST_LINE_BRAKE_SPEED,
                             -FOLLOW_LOST_LINE_BRAKE_SPEED);
        return;
    }

    d_error = (int16_t)(error - ctrl->last_error);
    ctrl->last_error = error;

    turn = (int32_t)ctrl->kp * error + (int32_t)ctrl->kd * d_error;

    left_speed = (int16_t)(ctrl->base_speed + turn);
    right_speed = (int16_t)(ctrl->base_speed - turn);

    /* 4驱：A/C视为左侧，B/D视为右侧 */
    MotorDrv_SetAllSpeed(left_speed, right_speed, left_speed, right_speed);
}
