# F103RC LineCar (Keil 可直接打开的 HAL 工程骨架)

## 1. 工程入口
- Keil 工程文件：`MDK-ARM/IR_I2C_Test.uvprojx`
- Target 名称：`F103RC_LineCar`
- 芯片：`STM32F103RC`

## 2. 主要功能
- I2C2(PB10/PB11) 读取 8 路寻线模块（地址 0x12, 寄存器 0x30）
- TIM8 四路 PWM（PC6/7/8/9）控制 4 个直流减速电机
- 方向引脚：A=PA11, B=PC10, C=PA8, D=PA12
- 支持寻线 PD 控制
- 支持“全黑=过一格”的走格子 + 路线转向（左/右/掉头）

## 3. 工程分组
- Application/MDK-ARM: 启动文件
- Application/User/Core: 主流程、外设初始化、业务模块
- Drivers/STM32F1xx_HAL_Driver: HAL 依赖源码
- Drivers/CMSIS: 系统文件

## 4. 烧录前检查
1. Keil Pack 中安装 `Keil.STM32F1xx_DFP`
2. 下载器设置为 ST-Link（或你自己的烧录器）
3. 硬件接线与引脚一致
4. 第一次上车建议降低 `FOLLOW_BASE_SPEED`

## 5. 默认路线（main.c）
```
{2, TURN_LEFT},
{1, TURN_RIGHT},
{3, TURN_BACK},
{1, TURN_STRAIGHT}
```

## 6. 常用调参
- `Core/Inc/car_config.h`:
  - `FOLLOW_BASE_SPEED`
  - `FOLLOW_KP`
  - `FOLLOW_KD`
- `Core/Src/route_grid.c`:
  - `left_turn_ms/right_turn_ms/back_turn_ms`
  - `turn_speed`
