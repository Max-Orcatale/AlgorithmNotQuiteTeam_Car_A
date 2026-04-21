#ifndef __ROUTE_GRID_H
#define __ROUTE_GRID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "line_follow_ctrl.h"

typedef enum
{
    TURN_STRAIGHT = 0, /* 不转弯，继续下一段 */
    TURN_LEFT,
    TURN_RIGHT,
    TURN_BACK
} TurnAction_t;

typedef struct
{
    uint16_t cells;      /* 本段前进的格子数（每遇到1次“8路全黑”算1格） */
    TurnAction_t turn;   /* 走完本段后的转向动作 */
} RouteStep_t;

typedef enum
{
    ROUTE_STATE_FOLLOW = 0,
    ROUTE_STATE_TURNING,
    ROUTE_STATE_FINISHED
} RouteState_t;

typedef struct
{
    LineFollowCtrl_t *follow_ctrl;

    const RouteStep_t *steps;
    uint16_t step_count;

    uint16_t current_step;
    uint16_t cells_in_step;

    uint8_t last_all_black;
    uint32_t last_black_tick;

    RouteState_t state;
    uint32_t turn_end_tick;

    uint16_t left_turn_ms;
    uint16_t right_turn_ms;
    uint16_t back_turn_ms;

    int16_t turn_speed;
} RouteGridCtrl_t;

void RouteGrid_Init(RouteGridCtrl_t *ctrl,
                    LineFollowCtrl_t *follow_ctrl,
                    const RouteStep_t *steps,
                    uint16_t step_count);

void RouteGrid_Update(RouteGridCtrl_t *ctrl, const LineSensorData_t *data);

uint16_t RouteGrid_GetCurrentStep(const RouteGridCtrl_t *ctrl);
uint16_t RouteGrid_GetCellsInStep(const RouteGridCtrl_t *ctrl);
RouteState_t RouteGrid_GetState(const RouteGridCtrl_t *ctrl);

#ifdef __cplusplus
}
#endif

#endif
