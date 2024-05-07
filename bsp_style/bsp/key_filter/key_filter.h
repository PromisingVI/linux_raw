/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 key_filter.h
作者	   : 刘有为
版本	   : V1.0
描述	   : 定时器按键消抖驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/6 刘有为创建
***************************************************************/
#ifndef _KEY_FILTER_H
#define _KEY_FILTER_H

#include "imx6ul.h"

void key_filter_init(void);
void key_filter_timer_init(unsigned int value);
void key_filter_timer_stop(void);
void key_filter_timer_restart(unsigned int value);
void key_filter_timer_irq_handler(void);
void gpio1_16_31_irq_handler(void);

#endif /* _KEY_FILTER_H */

