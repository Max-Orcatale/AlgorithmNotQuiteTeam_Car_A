/*
 * 基于 STM32F103RC + HAL 的4电机小车寻线示例（Keil风格）
 *
 * 说明：
 * 1) 本文件展示应用层逻辑，外设底层初始化可由CubeMX生成后拷入Keil工程。
 * 2) 关键外设：I2C2(PB10/PB11) + TIM8(4路PWM) + 方向GPIO。
 */

#include "stm32f1xx_hal.h"
#include "car_config.h"
#include "line_sensor_i2c.h"
#include "line_follow_ctrl.h"
#include "motor_drv.h"
#include "route_grid.h"

/* 以下函数建议使用CubeMX生成，并加入Keil工程 */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM8_Init(void);

I2C_HandleTypeDef hi2c2;
TIM_HandleTypeDef htim8;

int main(void)
{
    LineSensorData_t sensor_data;
    LineFollowCtrl_t follow;
    RouteGridCtrl_t route_ctrl;

    /*
     * 路线示例（可改）：
     * 1) 先走2格，左转
     * 2) 再走1格，右转
     * 3) 再走3格，掉头
     * 4) 最后走1格，结束
     */
    static const RouteStep_t route_table[] = {
        {2, TURN_LEFT},
        {1, TURN_RIGHT},
        {3, TURN_BACK},
        {1, TURN_STRAIGHT}
    };

    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_I2C2_Init();
    MX_TIM8_Init();

    MotorDrv_Init();
    LineFollow_Init(&follow);
    RouteGrid_Init(&route_ctrl, &follow, route_table, (uint16_t)(sizeof(route_table) / sizeof(route_table[0])));

    /* 模块上电稳定 */
    HAL_Delay(3000);

    while (1)
    {
        if (LineSensor_Read(&sensor_data) == HAL_OK)
        {
            RouteGrid_Update(&route_ctrl, &sensor_data);
        }
        else
        {
            /* I2C读失败：立即停车，确保安全 */
            MotorDrv_StopAll();
        }

        HAL_Delay(5); /* 200Hz控制节拍 */
    }
}

/* ------------------- 下面是可直接使用的HAL初始化模板 ------------------- */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

static void MX_I2C2_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 400000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c2);
}

static void MX_TIM8_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 1;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 2000;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_PWM_Init(&htim8);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4);

    /* 高级定时器TIM8需要主输出使能 */
    __HAL_TIM_MOE_ENABLE(&htim8);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (i2cHandle->Instance == I2C2)
    {
        __HAL_RCC_I2C2_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /* PB10=SCL, PB11=SDA */
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (tim_pwmHandle->Instance == TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        /* PC6~PC9 -> TIM8_CH1~CH4 */
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
}
