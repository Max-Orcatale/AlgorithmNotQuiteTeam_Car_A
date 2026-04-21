#include "motor_drv.h"

/*
 * 引脚映射来源：myPROJECT/User/Components/y_motor/y_motor.c
 * PWM: TIM8_CH1~CH4 -> PC6 PC7 PC8 PC9
 * DIR: A=PA11, B=PC10, C=PA8, D=PA12
 */
#define MOTOR_A_DIR_GPIO_PORT    GPIOA
#define MOTOR_A_DIR_PIN          GPIO_PIN_11
#define MOTOR_B_DIR_GPIO_PORT    GPIOC
#define MOTOR_B_DIR_PIN          GPIO_PIN_10
#define MOTOR_C_DIR_GPIO_PORT    GPIOA
#define MOTOR_C_DIR_PIN          GPIO_PIN_8
#define MOTOR_D_DIR_GPIO_PORT    GPIOA
#define MOTOR_D_DIR_PIN          GPIO_PIN_12

static int16_t Motor_ClampSpeed(int16_t speed)
{
    if (speed > MOTOR_PWM_MAX)
    {
        return MOTOR_PWM_MAX;
    }
    if (speed < MOTOR_PWM_MIN)
    {
        return MOTOR_PWM_MIN;
    }
    return speed;
}

static void Motor_WriteOne(TIM_HandleTypeDef *htim,
                           uint32_t channel,
                           GPIO_TypeDef *dir_port,
                           uint16_t dir_pin,
                           int16_t speed)
{
    uint16_t pwm;
    int16_t s = Motor_ClampSpeed(speed);

    if (s >= 0)
    {
        pwm = (uint16_t)s;
        HAL_GPIO_WritePin(dir_port, dir_pin, GPIO_PIN_RESET);
    }
    else
    {
        pwm = (uint16_t)(MOTOR_PWM_MAX + s); /* 与原STD库实现兼容 */
        HAL_GPIO_WritePin(dir_port, dir_pin, GPIO_PIN_SET);
    }

    __HAL_TIM_SET_COMPARE(htim, channel, pwm);
}

void MotorDrv_Init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* 方向脚输出 */
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio.Pin = MOTOR_A_DIR_PIN | MOTOR_C_DIR_PIN | MOTOR_D_DIR_PIN;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = MOTOR_B_DIR_PIN;
    HAL_GPIO_Init(GPIOC, &gpio);

    /* TIM8 PWM启动（要求htim8已在外部初始化为ARR=2000） */
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);

    MotorDrv_StopAll();
}

void MotorDrv_SetSpeed(MotorId_t id, int16_t speed)
{
    switch (id)
    {
    case MOTOR_ID_A:
        Motor_WriteOne(&htim8, TIM_CHANNEL_2, MOTOR_A_DIR_GPIO_PORT, MOTOR_A_DIR_PIN, speed);
        break;
    case MOTOR_ID_B:
        Motor_WriteOne(&htim8, TIM_CHANNEL_4, MOTOR_B_DIR_GPIO_PORT, MOTOR_B_DIR_PIN, speed);
        break;
    case MOTOR_ID_C:
        Motor_WriteOne(&htim8, TIM_CHANNEL_1, MOTOR_C_DIR_GPIO_PORT, MOTOR_C_DIR_PIN, speed);
        break;
    case MOTOR_ID_D:
        Motor_WriteOne(&htim8, TIM_CHANNEL_3, MOTOR_D_DIR_GPIO_PORT, MOTOR_D_DIR_PIN, speed);
        break;
    default:
        break;
    }
}

void MotorDrv_SetAllSpeed(int16_t speed_a, int16_t speed_b, int16_t speed_c, int16_t speed_d)
{
    MotorDrv_SetSpeed(MOTOR_ID_A, speed_a);
    MotorDrv_SetSpeed(MOTOR_ID_B, speed_b);
    MotorDrv_SetSpeed(MOTOR_ID_C, speed_c);
    MotorDrv_SetSpeed(MOTOR_ID_D, speed_d);
}

void MotorDrv_StopAll(void)
{
    MotorDrv_SetAllSpeed(0, 0, 0, 0);
}
