/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_key.h
作者	   : 刘有为
版本	   : V1.0
描述	   : 按键驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include "imx6ul.h"

/* 定义按键值 */
enum KEY_VALUE
{
	KEY_NONE   = 0,
	KEY0_VALUE,
	KEY1_VALUE,
	KEY2_VALUE,
};

void key_init(void);
int key_get_value(void);

#endif /* _BSP_KEY_H */

