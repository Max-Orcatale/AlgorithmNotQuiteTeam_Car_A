#ifndef __LINE_FOLLOW_CTRL_H
#define __LINE_FOLLOW_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "line_sensor_i2c.h"

typedef struct
{
    int16_t base_speed;
    int16_t kp;
    int16_t kd;
    int16_t last_error;
} LineFollowCtrl_t;

void LineFollow_Init(LineFollowCtrl_t *ctrl);
void LineFollow_Update(LineFollowCtrl_t *ctrl, const LineSensorData_t *data);

#ifdef __cplusplus
}
#endif

#endif
