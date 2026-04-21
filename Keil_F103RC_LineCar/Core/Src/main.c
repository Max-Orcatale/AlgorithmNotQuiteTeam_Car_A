#include "main.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"

#include "car_config.h"
#include "line_sensor_i2c.h"
#include "line_follow_ctrl.h"
#include "motor_drv.h"
#include "route_grid.h"

void SystemClock_Config(void);

int main(void)
{
    LineSensorData_t sensor_data;
    LineFollowCtrl_t follow;
    RouteGridCtrl_t route_ctrl;

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
    RouteGrid_Init(&route_ctrl,
                   &follow,
                   route_table,
                   (uint16_t)(sizeof(route_table) / sizeof(route_table[0])));

    HAL_Delay(3000);

    while (1)
    {
        if (LineSensor_Read(&sensor_data) == HAL_OK)
        {
            RouteGrid_Update(&route_ctrl, &sensor_data);
        }
        else
        {
            MotorDrv_StopAll();
        }

        HAL_Delay(5);
    }
}

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
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
}
#endif
