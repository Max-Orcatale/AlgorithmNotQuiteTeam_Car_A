#ifndef __MOTOR_DRV_H
#define __MOTOR_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "car_config.h"

typedef enum
{
    MOTOR_ID_A = 0,
    MOTOR_ID_B,
    MOTOR_ID_C,
    MOTOR_ID_D,
    MOTOR_ID_MAX
} MotorId_t;

void MotorDrv_Init(void);
void MotorDrv_SetSpeed(MotorId_t id, int16_t speed);
void MotorDrv_SetAllSpeed(int16_t speed_a, int16_t speed_b, int16_t speed_c, int16_t speed_d);
void MotorDrv_StopAll(void);

#ifdef __cplusplus
}
#endif

#endif
