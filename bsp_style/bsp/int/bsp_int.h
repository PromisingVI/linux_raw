/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_int.h
作者	   : 刘有为
版本	   : V1.0
描述	   : 中断驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/3 刘有为创建
***************************************************************/

#ifndef _BSP_INT_H
#define _BSP_INT_H

#include "imx6ul.h"

/* 中断服务函数形式 */ 
typedef void (*system_irq_handler_t)(unsigned int gicc_iar, void *param);

/* 中断服务函数结构体 */
typedef struct _sys_irq_handle
{
	system_irq_handler_t irq_handler;	/* 中断服务函数 */
	void *user_param;					/* 中断服务函数参数 */
} _sys_irq_handle_t;

void int_init(void);
void system_irq_table_init(void);
void system_register_irq_handler(IRQn_Type irq, system_irq_handler_t handler, void *user_params);
void system_irq_handler(unsigned int gicc_iar);
void default_irq_handler(unsigned int gicc_iar, void *user_param);

#endif /* _BSP_INT_H */

