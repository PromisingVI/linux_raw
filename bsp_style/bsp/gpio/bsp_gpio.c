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

	/* 中断功能配置 */
	gpio_int_config(base, pin, config->interrupt_mode);
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

/*
 * @description  			: 设置GPIO的中断配置功能
 * @param - base 			: 要配置的IO所在的GPIO组。
 * @param - pin  			: 要配置的GPIO脚号。
 * @param - pinInterruptMode: 中断模式，参考枚举类型gpio_interrupt_mode_t
 * @return		 			: 无
 */
void gpio_int_config(GPIO_Type *base, int pin, gpio_interrupt_mode_t pin_mode)
{
	volatile uint32_t *icr = NULL;
	uint32_t icrShift = 0;

	icrShift = pin;
	base->EDGE_SEL &= ~(1 << pin);

	if (16 > pin)
	{
		/* 低16位 */
		icr = &(base->ICR1);
	}
	else
	{
		/* 高16位 */
		icr = &(base->ICR1);
		icrShift -= 16;
	}

	switch(pin_mode)
	{
		case k_gpio_int_low_level:
			*icr &= ~(3U << (2 * icrShift));
			break;

		case k_gpio_int_high_level:
			*icr = (*icr & (~(3U << (2 * icrShift)))) | (1U << (2 * icrShift));
			break;

		case k_gpio_int_rise_edge:
			*icr = (*icr & (~(3U << (2 * icrShift)))) | (2U << (2 * icrShift));
			break;

		case k_gpio_int_fall_edge:
			*icr |= (3U << (2 * icrShift));
			break;

		case k_gpio_int_rise_or_fall_edge:
			base->EDGE_SEL |= (1U << pin);
			break;

		default:
			break;
	}
}

/*
 * @description  			: 使能GPIO的中断功能
 * @param - base 			: 要使能的IO所在的GPIO组。
 * @param - pin  			: 要使能的GPIO在组内的编号。
 * @return		 			: 无
 */
void gpio_enable_int(GPIO_Type *base, int pin)
{
	base->IMR |= (1 << pin);
}

/*
 * @description  			: 禁止GPIO的中断功能
 * @param - base 			: 要禁止的IO所在的GPIO组。
 * @param - pin  			: 要禁止的GPIO在组内的编号。
 * @return		 			: 无
 */
void gpio_disable_int(GPIO_Type *base, int pin)
{
	base->IMR &= ~(1 << pin);
}

/*
 * @description  			: 清除中断标志位(写1清除)
 * @param - base 			: 要清除的IO所在的GPIO组。
 * @param - pin  			: 要清除的GPIO掩码。
 * @return		 			: 无
 */
void gpio_clear_int_flag(GPIO_Type *base, int pin)
{
	base->ISR |= (1 << pin);
}

