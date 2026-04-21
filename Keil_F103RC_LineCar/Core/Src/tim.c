#include "tim.h"

TIM_HandleTypeDef htim8;

void MX_TIM8_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 1;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 2000;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) { Error_Handler(); }
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_4) != HAL_OK) { Error_Handler(); }

    __HAL_TIM_MOE_ENABLE(&htim8);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    if (tim_pwmHandle->Instance == TIM8)
    {
        __HAL_RCC_TIM8_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
}
