/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_epittimer.c
作者	   : 刘有为
版本	   : V1.0
描述	   : EPIT定时器驱动文件。
其他	   : 配置EPIT定时器，实现EPIT定时器中断处理函数
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/6 刘有为创建
***************************************************************/

#include "epit_timer.h"
#include "bsp_int.h"
#include "bsp_led.h"

/*
 * @description		: 初始化EPIT定时器.
 *					  EPIT定时器是32位向下计数器,时钟源使用ipg=66Mhz		 
 * @param - frac	: 分频值，范围为0~4095，分别对应1~4096分频。
 * @param - value	: 倒计数值。
 * @return 			: 无
 */
void epit1_init(unsigned int frac, unsigned int value)
{
	if (0xfff < frac)
	{
		frac = 0xfff;
	}

	EPIT1->CR = 0;

	/*
	 * CR寄存器:
	 * bit25:24 01 时钟源选择Peripheral clock=66MHz
	 * bit15:4  frac 分频值
	 * bit3:	1  当计数器到0的话从LR重新加载数值
	 * bit2:	1  比较中断使能
	 * bit1:    1  初始计数值来源于LR寄存器值
	 * bit0:    0  先关闭EPIT1
	 */
	EPIT1->CR = (1<<24 | frac << 4 | 1<<3 | 1<<2 | 1<<1);

	/* 倒计数值 */
	EPIT1->LR = value;

	/* 比较寄存器，当计数器值和此寄存器值相等的话就会产生中断 */
	EPIT1->CMPR	= 0;

	/* 使能GIC中对应的中断 */
	GIC_EnableIRQ(EPIT1_IRQn);

	/* 注册中断服务函数 */
	system_register_irq_handler(EPIT1_IRQn, (system_irq_handler_t)epit1_irq_handler, NULL);

	/* 使能EPIT1 */
	EPIT1->CR |= 1<<0;
}

/*
 * @description			: EPIT中断处理函数
 * @param				: 无
 * @return 				: 无
 */
void epit1_irq_handler(void)
{
	static unsigned char state = 0;

	state = !state;

	/* 判断比较事件发生,  */
	if (EPIT1->SR & (1<<0))
	{
		/* 定时器周期到，反转LED */
		led_switch(LED0, state);
	}

	/* 清除中断标志位 */
	EPIT1->SR |= 1<<0;
}

