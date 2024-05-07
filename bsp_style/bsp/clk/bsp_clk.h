/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_clk.h
作者	   : 刘有为
版本	   : V1.0
描述	   : ZERO-I.MX6UL/I.MX6ULL开发板启动文件，完成C环境初始化，
		 C环境初始化完成以后跳转到C代码。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版 2024/4/28 刘有为修改
**************************************************************/

#ifndef __BSP_CLK_H
#define __BSP_CLK_H

#include "imx6ul.h"

/* 函数声明 */
void clk_enable(void);
void imx6u_clk_init(void);

#endif

