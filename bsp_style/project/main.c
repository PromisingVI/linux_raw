/**************************************************************
Copyright © zuozhongkai Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 mian.c
作者	   : 左忠凯
版本	   : V1.0
描述	   : I.MX6U开发板裸机实验13 串口实验
其他	   : 本实验我们学习如何使用I.MX6的串口，实现串口收发数据，了解
		 I.MX6的串口工作原理。
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2019/1/15 左忠凯创建
**************************************************************/
#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_uart.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcdapi.h"
#include "bsp_rtc.h"
#include "bsp_i2c.h"
#include "bsp_ap3216c.h"
#include "bsp_icm20608.h"
#include "bsp_ft5xx6.h"
#include "bsp_gt9147.h"

/* 背景颜色索引 */
unsigned int backcolor[10] = {
	LCD_BLUE, 		LCD_GREEN, 		LCD_RED, 	LCD_CYAN, 	LCD_YELLOW, 
	LCD_LIGHTBLUE, 	LCD_DARKBLUE, 	LCD_WHITE, 	LCD_BLACK, 	LCD_ORANGE
};

/*
 * @description	: 指定的位置显示整数数据
 * @param - x	: X轴位置
 * @param - y 	: Y轴位置
 * @param - size: 字体大小
 * @param - num : 要显示的数据
 * @return 		: 无
 */
void integer_display(unsigned short x, unsigned short y, unsigned char size, signed int num)
{
	char buf[200];
	
	lcd_fill(x, y, x + 50, y + size, tftlcd_dev.backcolor);
	
	memset(buf, 0, sizeof(buf));
	if(num < 0)
		sprintf(buf, "-%d", -num);
	else 
		sprintf(buf, "%d", num);
	lcd_show_string(x, y, 50, size, size, buf); 
}

/*
 * @description	: 指定的位置显示小数数据,比如5123，显示为51.23
 * @param - x	: X轴位置
 * @param - y 	: Y轴位置
 * @param - size: 字体大小
 * @param - num : 要显示的数据，实际小数扩大100倍，
 * @return 		: 无
 */
void decimals_display(unsigned short x, unsigned short y, unsigned char size, signed int num)
{
	signed int integ; 	/* 整数部分 */
	signed int fract;	/* 小数部分 */
	signed int uncomptemp = num; 
	char buf[200];

	if(num < 0)
		uncomptemp = -uncomptemp;
	integ = uncomptemp / 100;
	fract = uncomptemp % 100;

	memset(buf, 0, sizeof(buf));
	if(num < 0)
		sprintf(buf, "-%d.%d", integ, fract);
	else 
		sprintf(buf, "%d.%d", integ, fract);
	lcd_fill(x, y, x + 60, y + size, tftlcd_dev.backcolor);
	lcd_show_string(x, y, 60, size, size, buf); 
}

/*
 * @description	: 使能I.MX6U的硬件NEON和FPU
 * @param 		: 无
 * @return 		: 无
 */
 void imx6ul_hardfpu_enable(void)
{
	uint32_t cpacr;
	uint32_t fpexc;

	/* 使能NEON和FPU */
	cpacr = __get_CPACR();
	cpacr = (cpacr & ~(CPACR_ASEDIS_Msk | CPACR_D32DIS_Msk))
		   |  (3UL << CPACR_cp10_Pos) | (3UL << CPACR_cp11_Pos);
	__set_CPACR(cpacr);
	fpexc = __get_FPEXC();
	fpexc |= 0x40000000UL;	
	__set_FPEXC(fpexc);
}

/*
 * @description	: main函数
 * @param 		: 无
 * @return 		: 无
 */
int main(void)
{
 	unsigned char i = 0;
	unsigned char state = OFF;

	imx6ul_hardfpu_enable();	/* 使能I.MX6U的硬件浮点 			*/
	int_init(); 				/* 初始化中断(一定要最先调用！) */
	imx6u_clkinit();			/* 初始化系统时钟 					*/
	delay_init();				/* 初始化延时 					*/
	clk_enable();				/* 使能所有的时钟 					*/
	led_init();					/* 初始化led 					*/
	beep_init();				/* 初始化beep	 				*/
	uart_init();				/* 初始化串口，波特率115200 */
	lcd_init();					/* 初始化LCD 					*/		
	
	/* 初始化触摸屏					*/ 
	gt9147_init();
	if(gt_init_fail==1) //判断gt系列初始化是否失败
	{
		ft5426_init();
	}

	tftlcd_dev.forecolor = LCD_RED;
	lcd_show_string(50, 10, 400, 24, 24, (char*)"IMX6U-ALPHA TOUCH SCREEN TEST");  
	lcd_show_string(50, 40, 200, 16, 16, (char*)"TOUCH SCREEN TEST");  
	lcd_show_string(50, 60, 200, 16, 16, (char*)"ATOM@ALIENTEK");  
	lcd_show_string(50, 80, 200, 16, 16, (char*)"2019/3/27");  
	
	lcd_show_string(50, 110, 400, 16, 16,	(char*)"TP Num	:");  
	lcd_show_string(50, 130, 200, 16, 16,	(char*)"Point0 X:");  
	lcd_show_string(50, 150, 200, 16, 16,	(char*)"Point0 Y:");  
	lcd_show_string(50, 170, 200, 16, 16,	(char*)"Point1 X:");  
	lcd_show_string(50, 190, 200, 16, 16,	(char*)"Point1 Y:");  
	lcd_show_string(50, 210, 200, 16, 16,	(char*)"Point2 X:");  
	lcd_show_string(50, 230, 200, 16, 16,	(char*)"Point2 Y:");  
	lcd_show_string(50, 250, 200, 16, 16,	(char*)"Point3 X:");  
	lcd_show_string(50, 270, 200, 16, 16,	(char*)"Point3 Y:");  
	lcd_show_string(50, 290, 200, 16, 16,	(char*)"Point4 X:");  
	lcd_show_string(50, 310, 200, 16, 16,	(char*)"Point4 Y:");  
	tftlcd_dev.forecolor = LCD_BLUE;
	
	while(1)					
	{
		if(gt_init_fail==1) {
			lcd_shownum(50 + 72, 110, ft5426_dev.point_num , 1, 16);
			lcd_shownum(50 + 72, 130, ft5426_dev.x[0], 5, 16);
			lcd_shownum(50 + 72, 150, ft5426_dev.y[0], 5, 16);
			lcd_shownum(50 + 72, 170, ft5426_dev.x[1], 5, 16);
			lcd_shownum(50 + 72, 190, ft5426_dev.y[1], 5, 16);
			lcd_shownum(50 + 72, 210, ft5426_dev.x[2], 5, 16);
			lcd_shownum(50 + 72, 230, ft5426_dev.y[2], 5, 16);
			lcd_shownum(50 + 72, 250, ft5426_dev.x[3], 5, 16);
			lcd_shownum(50 + 72, 270, ft5426_dev.y[3], 5, 16);
			lcd_shownum(50 + 72, 290, ft5426_dev.x[4], 5, 16);
			lcd_shownum(50 + 72, 310, ft5426_dev.y[4], 5, 16);
		} else {
			lcd_shownum(50 + 72, 110, gt9147_dev.point_num , 1, 16);
			lcd_shownum(50 + 72, 130, gt9147_dev.x[0], 5, 16);
			lcd_shownum(50 + 72, 150, gt9147_dev.y[0], 5, 16);
			lcd_shownum(50 + 72, 170, gt9147_dev.x[1], 5, 16);
			lcd_shownum(50 + 72, 190, gt9147_dev.y[1], 5, 16);
			lcd_shownum(50 + 72, 210, gt9147_dev.x[2], 5, 16);
			lcd_shownum(50 + 72, 230, gt9147_dev.y[2], 5, 16);
			lcd_shownum(50 + 72, 250, gt9147_dev.x[3], 5, 16);
			lcd_shownum(50 + 72, 270, gt9147_dev.y[3], 5, 16);
			lcd_shownum(50 + 72, 290, gt9147_dev.x[4], 5, 16);
			lcd_shownum(50 + 72, 310, gt9147_dev.y[4], 5, 16);
		}
		delayms(10);
		i++;
	
		if(i == 50)
		{	
			i = 0;
			state = !state;
			led_switch(LED0,state); 
		}
	}
	return 0;
}
