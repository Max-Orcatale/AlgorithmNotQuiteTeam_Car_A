#ifndef __LINE_SENSOR_I2C_H
#define __LINE_SENSOR_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include "car_config.h"

typedef struct
{
    uint8_t bit[8];      /* bit[0]对应最左，bit[7]对应最右（可按实际安装调整） */
    uint8_t raw_byte;    /* 原始寄存器字节 */
} LineSensorData_t;

HAL_StatusTypeDef LineSensor_SetAdjustMode(uint8_t enable);
HAL_StatusTypeDef LineSensor_Read(LineSensorData_t *data);

#ifdef __cplusplus
}
#endif

#endif
