#include "route_grid.h"
#include "motor_drv.h"

#define ROUTE_BLACK_DEBOUNCE_MS   (120U)  /* 全黑判定去抖 */

static uint8_t RouteGrid_IsAllBlack(const LineSensorData_t *data)
{
    uint8_t i;
    for (i = 0; i < 8U; i++)
    {
        if (data->bit[i] == 0U)
        {
            return 0U;
        }
    }
    return 1U;
}

static void RouteGrid_StartTurn(RouteGridCtrl_t *ctrl, TurnAction_t turn)
{
    uint32_t now = HAL_GetTick();

    switch (turn)
    {
    case TURN_LEFT:
        MotorDrv_SetAllSpeed(-ctrl->turn_speed, ctrl->turn_speed,
                             -ctrl->turn_speed, ctrl->turn_speed);
        ctrl->turn_end_tick = now + ctrl->left_turn_ms;
        ctrl->state = ROUTE_STATE_TURNING;
        break;

    case TURN_RIGHT:
        MotorDrv_SetAllSpeed(ctrl->turn_speed, -ctrl->turn_speed,
                             ctrl->turn_speed, -ctrl->turn_speed);
        ctrl->turn_end_tick = now + ctrl->right_turn_ms;
        ctrl->state = ROUTE_STATE_TURNING;
        break;

    case TURN_BACK:
        MotorDrv_SetAllSpeed(ctrl->turn_speed, -ctrl->turn_speed,
                             ctrl->turn_speed, -ctrl->turn_speed);
        ctrl->turn_end_tick = now + ctrl->back_turn_ms;
        ctrl->state = ROUTE_STATE_TURNING;
        break;

    case TURN_STRAIGHT:
    default:
        ctrl->state = ROUTE_STATE_FOLLOW;
        break;
    }
}

void RouteGrid_Init(RouteGridCtrl_t *ctrl,
                    LineFollowCtrl_t *follow_ctrl,
                    const RouteStep_t *steps,
                    uint16_t step_count)
{
    if ((ctrl == NULL) || (follow_ctrl == NULL) || (steps == NULL) || (step_count == 0U))
    {
        return;
    }

    ctrl->follow_ctrl = follow_ctrl;
    ctrl->steps = steps;
    ctrl->step_count = step_count;

    ctrl->current_step = 0U;
    ctrl->cells_in_step = 0U;

    ctrl->last_all_black = 0U;
    ctrl->last_black_tick = 0U;

    ctrl->state = ROUTE_STATE_FOLLOW;
    ctrl->turn_end_tick = 0U;

    /* 默认转向时间：需按底盘实际速度校准 */
    ctrl->left_turn_ms = 360U;
    ctrl->right_turn_ms = 360U;
    ctrl->back_turn_ms = 700U;

    ctrl->turn_speed = 950;
}

void RouteGrid_Update(RouteGridCtrl_t *ctrl, const LineSensorData_t *data)
{
    uint8_t all_black;
    uint32_t now;

    if ((ctrl == NULL) || (data == NULL) || (ctrl->state == ROUTE_STATE_FINISHED))
    {
        return;
    }

    now = HAL_GetTick();

    if (ctrl->state == ROUTE_STATE_TURNING)
    {
        if ((int32_t)(now - ctrl->turn_end_tick) >= 0)
        {
            ctrl->state = ROUTE_STATE_FOLLOW;
            MotorDrv_StopAll();
            HAL_Delay(30);
        }
        return;
    }

    /* 正常巡线 */
    LineFollow_Update(ctrl->follow_ctrl, data);

    all_black = RouteGrid_IsAllBlack(data);

    /* 边沿+去抖：从“非全黑”到“全黑”才算跨过1个格子 */
    if ((all_black == 1U) &&
        (ctrl->last_all_black == 0U) &&
        ((now - ctrl->last_black_tick) > ROUTE_BLACK_DEBOUNCE_MS))
    {
        ctrl->last_black_tick = now;
        ctrl->cells_in_step++;

        if (ctrl->cells_in_step >= ctrl->steps[ctrl->current_step].cells)
        {
            TurnAction_t turn = ctrl->steps[ctrl->current_step].turn;

            ctrl->cells_in_step = 0U;
            ctrl->current_step++;

            if (ctrl->current_step >= ctrl->step_count)
            {
                ctrl->state = ROUTE_STATE_FINISHED;
                MotorDrv_StopAll();
                return;
            }

            RouteGrid_StartTurn(ctrl, turn);
        }
    }

    ctrl->last_all_black = all_black;
}

uint16_t RouteGrid_GetCurrentStep(const RouteGridCtrl_t *ctrl)
{
    if (ctrl == NULL)
    {
        return 0U;
    }
    return ctrl->current_step;
}

uint16_t RouteGrid_GetCellsInStep(const RouteGridCtrl_t *ctrl)
{
    if (ctrl == NULL)
    {
        return 0U;
    }
    return ctrl->cells_in_step;
}

RouteState_t RouteGrid_GetState(const RouteGridCtrl_t *ctrl)
{
    if (ctrl == NULL)
    {
        return ROUTE_STATE_FINISHED;
    }
    return ctrl->state;
}
