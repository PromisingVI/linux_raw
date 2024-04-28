/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_led.h
作者	   : 刘有为
版本	   : V1.0
描述	   : LED驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28刘有为创建
***************************************************************/
#ifndef __BSP_LED_H
#define __BSP_LED_H

#include "imx6ul.h"

#define LED0	0

void led_init(void);
void led_switch(int led, int status);

#endif /* __BSP_LED_H */

