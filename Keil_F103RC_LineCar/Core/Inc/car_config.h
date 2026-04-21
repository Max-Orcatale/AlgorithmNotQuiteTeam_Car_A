#ifndef __CAR_CONFIG_H
#define __CAR_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

/* ======================== 硬件常量配置 ======================== */

/* 8路寻线模块I2C信息（来自商家例程） */
#define LINE_I2C_HANDLE               hi2c2
#define LINE_SENSOR_I2C_ADDR_7BIT     (0x12U)
#define LINE_SENSOR_REG_ADJUST_MODE   (0x01U)
#define LINE_SENSOR_REG_DATA           (0x30U)

/* 轮速PWM范围（与原工程TIM8周期一致） */
#define MOTOR_PWM_MAX                 (2000)
#define MOTOR_PWM_MIN                 (-2000)

/* 寻线控制参数（可在线调） */
#define FOLLOW_BASE_SPEED             (1200)   /* 基础速度 */
#define FOLLOW_KP                     (380)    /* 比例系数 */
#define FOLLOW_KD                     (220)    /* 微分系数 */
#define FOLLOW_LOST_LINE_BRAKE_SPEED  (600)    /* 丢线时刹车速度 */

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim8;

#ifdef __cplusplus
}
#endif

#endif
