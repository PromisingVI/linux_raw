/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_epittimer.h
作者	   : 刘有为
版本	   : V1.0
描述	   : EPIT定时器驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/6 刘有为创建
***************************************************************/
#ifndef _BSP_EPIT_TIMER_H
#define _BSP_EPIT_TIMER_H

#include "imx6ul.h"

void epit1_init(unsigned int frac, unsigned int value);
void epit1_irq_handler(void);

#endif /* _BSP_EPIT_TIMER_H */

