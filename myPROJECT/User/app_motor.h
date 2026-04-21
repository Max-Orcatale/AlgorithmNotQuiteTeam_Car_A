/*
 * @文件描述: 
 * @作者: Q
 * @Date: 2023-02-13 16:09:51
 * @LastEditTime: 2023-02-15 15:02:45
 */
#ifndef _APP_MOTOR_H_
#define _APP_MOTOR_H_
#include "main.h"

#define PI					3.14159265358979f

#define PID_RATE 50 // PID频率，(进入PID函数的频率)

// 电机编码器分辨率
#define WHEEL_RESOLUTION 1040.0f // 26极磁编码器分辨率,开关霍尔：13*4*20（减速比）= 1040

// 小车参数
#define MEC_WHEEL_BASE 0.128f													// 轮距，左右轮的距离
#define MEC_ACLE_BASE 0.146f													// 轴距，前后轮的距离
#define MEC_WHEEL_DIAMETER 0.048f												// 轮子直径
#define MEC_WHEEL_SCALE (PI * MEC_WHEEL_DIAMETER * PID_RATE / WHEEL_RESOLUTION) // 轮子速度m/s与编码器转换系数


void app_motor_init(void);
void app_motor_run(void);

void motor_speed_set(float A,float B,float C,float D);
#endif
