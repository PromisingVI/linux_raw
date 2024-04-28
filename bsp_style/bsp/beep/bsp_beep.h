/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_beep.h
作者	   : 刘有为
版本	   : V1.0
描述	   : 蜂鸣器驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#ifndef __BSP_BEEP_H
#define __BSP_BEEP_H

#include "imx6ul.h"

void beep_init(void);
void beep_switch(int status);

#endif /* __BSP_BEEP_H */

