/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_clk.c
作者	   : 刘有为
版本	   : V1.0
描述	   : 系统时钟驱动。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#include "bsp_clk.h"

void clk_enable(void)
{
	CCM->CCGR0 = 0XFFFFFFFF;
	CCM->CCGR1 = 0XFFFFFFFF;
	CCM->CCGR2 = 0XFFFFFFFF;
	CCM->CCGR3 = 0XFFFFFFFF;
	CCM->CCGR4 = 0XFFFFFFFF;
	CCM->CCGR5 = 0XFFFFFFFF;
	CCM->CCGR6 = 0XFFFFFFFF;
}

