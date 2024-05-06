/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_int.c
作者	   : 刘有为
版本	   : V1.0
描述	   : 中断驱动文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/3 刘有为创建
***************************************************************/

#include "bsp_int.h"

/* 中断嵌套计数器 */
static unsigned int g_irq_nesting;

/* 中断服务函数表 */
static _sys_irq_handle_t g_irq_table[NUMBER_OF_INT_VECTORS];

void int_init(void)
{
	GIC_Init();							/* 初始化GIC */
	system_irq_table_init();			/* 初始化中断表 */
	__set_VBAR((uint32_t)0x87800000);	/* 中断向量表偏移，偏移到起始地址 */
}

void system_irq_table_init(void)
{
	int i = 0;

	g_irq_nesting = 0;

	/* 先将所有的中断服务函数设置为默认值 */
	for (i = 0; i < NUMBER_OF_INT_VECTORS; i++)
	{
		system_register_irq_handler((IRQn_Type)i, default_irq_handler, NULL);
	}
}

void system_register_irq_handler(IRQn_Type irq, system_irq_handler_t handler, void *user_params)
{
	g_irq_table[irq].irq_handler = handler;
	g_irq_table[irq].user_param = user_params;
}

void system_irq_handler(unsigned int gicc_iar)
{
	uint32_t intNum = gicc_iar & 0x3FFUL;

	if ((1023 == intNum) || (NUMBER_OF_INT_VECTORS <= intNum))
	{
		return;
	}

	g_irq_nesting++;

	/* 根据传递进来的中断号，在irqTable中调用确定的中断服务函数*/
	g_irq_table[intNum].irq_handler(intNum, g_irq_table[intNum].user_param);

	/* 中断执行完成，中断嵌套寄存器减一 */
	g_irq_nesting--;
}

void default_irq_handler(unsigned int gicc_iar, void *user_param)
{
	while (1)
	{
	}
}

