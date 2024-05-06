/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_gpio.h
作者	   : 刘有为
版本	   : V1.0
描述	   : GPIO操作文件头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/
#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

#include "imx6ul.h"

#define _BSP_KEY_H

/* 枚举类型和结构体定义 */
typedef enum _GPIO_PIN_DIRECTION
{
	KGPIO_DIGITAL_INPUT = 0U,
	KGPIO_DIGITAL_OUTPUT = 1U,
} GPIO_PIN_DIRECTION;

typedef enum _gpio_interrupt_mode
{
	k_gpio_no_int_mode = 0U,
	k_gpio_int_low_level = 1U,
	k_gpio_int_high_level = 2U,
	k_gpio_int_rise_edge = 3U,
	k_gpio_int_fall_edge = 4U,
	k_gpio_int_rise_or_fall_edge = 5U,
} gpio_interrupt_mode_t;

typedef struct _GPIO_PIN_CONFIG
{
	GPIO_PIN_DIRECTION direction;			/* GPIO方向:输入还是输出 */
	uint8_t default_output;					/* 如果是输出的话，默认输出电平 */
	gpio_interrupt_mode_t interrupt_mode;	/* 中断方式 */
} GPIO_PIN_CONFIG;

/* 函数声明 */
void gpio_init(GPIO_Type *base, int pin, GPIO_PIN_CONFIG *config);
int gpio_pin_read(GPIO_Type *base, int pin);
void gpio_pin_write(GPIO_Type *base, int pin, int value);
void gpio_int_config(GPIO_Type *base, int pin, gpio_interrupt_mode_t pin_mode);
void gpio_enable_int(GPIO_Type *base, int pin);
void gpio_disable_int(GPIO_Type *base, int pin);
void gpio_clear_int_flag(GPIO_Type *base, int pin);

#endif /* _BSP_GPIO_H */

