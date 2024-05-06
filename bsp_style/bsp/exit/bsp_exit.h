/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_exit.h
作者	   : 刘有为
版本	   : V1.0
描述	   : 外部中断驱动头文件。
其他	   : 配置按键对应的GPIP为中断模式
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/6 刘有为创建
***************************************************************/

#ifndef _BSP_EXIT_H
#define _BSP_EXIT_H

#include "imx6ul.h"

void exit_init(void);
void gpio1_io18_irq_handler(void);

#endif /* _BSP_EXIT_H */

