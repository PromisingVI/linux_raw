/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_gpio.c
作者	   : 刘有为
版本	   : V1.0
描述	   : GPIO操作文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/29 刘有为创建
***************************************************************/

#include "bsp_gpio.h"

void gpio_init(GPIO_Type *base, int pin, GPIO_PIN_CONFIG *config)
{
	if (KGPIO_DIGITAL_INPUT == config->direction)
	{
		base->GDIR &= ~(1 << pin);
	}
	else
	{
		base->GDIR |= 1 << pin;
		/* 设置默认输出电平 */
		gpio_pin_write(base, pin, config->default_output);
	}
}

int gpio_pin_read(GPIO_Type *base, int pin)
{
	return (((base->DR) >> pin) & 0x1);
}

void gpio_pin_write(GPIO_Type *base, int pin, int value)
{
	if (0U == value)
	{
		base->DR &= ~(1U << pin); /* 输出低电平 */
	}
	else
	{
		 base->DR |= (1U << pin); /* 输出高电平 */
	}
}

