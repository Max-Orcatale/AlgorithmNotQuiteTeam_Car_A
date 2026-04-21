#include "line_sensor_i2c.h"

/*
 * @brief 设置8路寻线模块校准模式
 * @param enable 1进入校准, 0退出校准
 */
HAL_StatusTypeDef LineSensor_SetAdjustMode(uint8_t enable)
{
    uint8_t value = (enable != 0U) ? 1U : 0U;
    return HAL_I2C_Mem_Write(&LINE_I2C_HANDLE,
                             (LINE_SENSOR_I2C_ADDR_7BIT << 1),
                             LINE_SENSOR_REG_ADJUST_MODE,
                             I2C_MEMADD_SIZE_8BIT,
                             &value,
                             1,
                             20);
}

/*
 * @brief 读取8路寻线状态
 * @note  商家代码中x1=(buf>>7), x8=(buf>>0)，这里保持一致：
 *        bit[0]=x1(最高位), bit[7]=x8(最低位)
 */
HAL_StatusTypeDef LineSensor_Read(LineSensorData_t *data)
{
    uint8_t raw = 0xFF;
    HAL_StatusTypeDef ret;
    uint8_t i;

    if (data == NULL)
    {
        return HAL_ERROR;
    }

    ret = HAL_I2C_Mem_Read(&LINE_I2C_HANDLE,
                           (LINE_SENSOR_I2C_ADDR_7BIT << 1),
                           LINE_SENSOR_REG_DATA,
                           I2C_MEMADD_SIZE_8BIT,
                           &raw,
                           1,
                           20);
    if (ret != HAL_OK)
    {
        return ret;
    }

    data->raw_byte = raw;
    for (i = 0; i < 8U; i++)
    {
        data->bit[i] = (uint8_t)((raw >> (7U - i)) & 0x01U);
    }

    return HAL_OK;
}
