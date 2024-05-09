/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_lcd.c
作者	   : 刘有为
版本	   : V1.0
描述	   : LCD驱动文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/8 刘有为创建
***************************************************************/
#include "bsp_lcd.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
#include "stdio.h"

/* 液晶屏参数结构体 */
struct tftlcd_typedef tftlcd_dev;

static int lcd_init_dev_config(struct tftlcd_typedef *dev, unsigned short lcdid)
{
	if (NULL == dev)
	{
		LCD_ERR("param null!");
		return LCD_ERROR;
	}

	switch (lcdid)
	{
		case ATK4342:
			dev->height = 272;	
			dev->width = 480;
			dev->vspw = 1;
			dev->vbpd = 8;
			dev->vfpd = 8;
			dev->hspw = 1;
			dev->hbpd = 40;
			dev->hfpd = 5;

			/* 初始化LCD时钟 10.1MHz */
			lcdclk_init(27, 8, 8);
			break;

		case ATK4384:
			dev->height = 480;	
			dev->width = 800;
			dev->vspw = 3;
			dev->vbpd = 32;
			dev->vfpd = 13;
			dev->hspw = 48;
			dev->hbpd = 88;
			dev->hfpd = 40;

			/* 初始化LCD时钟 31.5MHz */
			lcdclk_init(42, 4, 8);
			break;

		case ATK7084:
			dev->height = 480;	
			dev->width = 800;
			dev->vspw = 1;
			dev->vbpd = 23;
			dev->vfpd = 22;
			dev->hspw = 1;
			dev->hbpd = 46;
			dev->hfpd = 210;

			/* 初始化LCD时钟 34.2MHz */
			lcdclk_init(30, 3, 7);
			break;

		case ATK7016:
			dev->height = 600;	
			dev->width = 1024;
			dev->vspw = 3;
			dev->vbpd = 20;
			dev->vfpd = 12;
			dev->hspw = 20;
			dev->hbpd = 140;
			dev->hfpd = 160;

			/* 初始化LCD时钟 51.2MHz */
			lcdclk_init(32, 3, 5);
			break;

		case ATK1018:
			dev->height = 800;	
			dev->width = 1280;
			dev->vspw = 3;
			dev->vbpd = 10;
			dev->vfpd = 10;
			dev->hspw = 10;
			dev->hbpd = 80;
			dev->hfpd = 70;

			/* 初始化LCD时钟 56MHz */
			lcdclk_init(35, 3, 5);
			break;

		case ATKVGA:
			dev->height = 768;	
			dev->width = 1366;
			dev->vspw = 3;
			dev->vbpd = 24;
			dev->vfpd = 3;
			dev->hspw = 143;
			dev->hbpd = 213;
			dev->hfpd = 70;

			/* 初始化LCD时钟 85MHz */
			lcdclk_init(32, 3, 3);
			break;

		default:
			LCD_ERR("invalied lcd id: 0x%x!", lcdid);
			return LCD_ERROR;
	}

	/* ARGB8888模式，每个像素4字节 */
	dev->pixsize = 4;
	dev->framebuffer = LCD_FRAMEBUF_ADDR;
	dev->backcolor = LCD_WHITE;
	dev->forecolor = LCD_BLACK;

	return LCD_OK;
}

/*
 * @description	: 始化LCD
 * @param 		: 无
 * @return 		: 无
 */
void lcd_init(void)
{
	unsigned short lcdid = 0;

	/* 读取屏幕ID值 */
	lcdid = lcd_read_panelid();
	tftlcd_dev.id = lcdid;
	LCD_INFO("lcd id: 0x%x", lcdid);

	/* 初始化IO */
	lcdgpio_init();

	/* 复位LCD */
	lcd_reset();
	delayms(10);

	/* 结束复位 */
	lcd_noreset();

	/* TFTLCD参数结构体初始化 */
	if (LCD_OK != lcd_init_dev_config(&tftlcd_dev, lcdid))
	{
		LCD_ERR("init tftlcd_dev failed!");
		return;
	}

	/* 初始化ELCDIF的CTRL寄存器
     * bit [31] 0 : 停止复位
     * bit [19] 1 : 旁路计数器模式
     * bit [17] 1 : LCD工作在dotclk模式
     * bit [15:14] 00 : 输入数据不交换
     * bit [13:12] 00 : CSC不交换
     * bit [11:10] 11 : 24位总线宽度
     * bit [9:8]   11 : 24位数据宽度,也就是RGB888
     * bit [5]     1  : elcdif工作在主模式
     * bit [1]     0  : 所有的24位均有效
	 */
	LCDIF->CTRL |= (1 << 19) | (1 << 17) | (0 << 14) | (0 << 12) | (3 << 10) | (3 << 8) | (1 << 5) | (0 << 1);

	/*
     * 初始化ELCDIF的寄存器CTRL1
     * bit [19:16]  : 0X7 ARGB模式下，传输24位数据，A通道不用传输
	 */
	LCDIF->CTRL1 |= (0x7 << 16);

	/*
      * 初始化ELCDIF的寄存器TRANSFER_COUNT寄存器
      * bit [31:16]  : 高度
      * bit [15:0]   : 宽度
	  */
	LCDIF->TRANSFER_COUNT = (tftlcd_dev.height << 16) | (tftlcd_dev.width);

	/*
     * 初始化ELCDIF的VDCTRL0寄存器
     * bit [29] 0 : VSYNC输出
     * bit [28] 1 : 使能ENABLE输出
     * bit [27] 0 : VSYNC低电平有效
     * bit [26] 0 : HSYNC低电平有效
     * bit [25] 0 : DOTCLK上升沿有效
     * bit [24] 1 : ENABLE信号高电平有效
     * bit [21] 1 : DOTCLK模式下设置为1
     * bit [20] 1 : DOTCLK模式下设置为1
     * bit [17:0] : vsw参数
	 */
	LCDIF->VDCTRL0 = 0;
	if (ATKVGA == lcdid)
	{
		/* VGA需要特殊处理 */
		LCDIF->VDCTRL0 |= (0 << 29) | (1 << 28) |
						  (0 << 27) | (0 << 26) | (1 << 25) |
						  (0 << 24) | (1 << 21) | (1 << 20) | (tftlcd_dev.vspw << 0);
	}
	else
	{
		LCDIF->VDCTRL0 |= (0 << 29) | (1 << 28) |
						  (0 << 27) | (0 << 26) | (0 << 25) |
						  (1 << 24) | (1 << 21) | (1 << 20) | (tftlcd_dev.vspw << 0);
	}

	/*
	 * 初始化ELCDIF的VDCTRL1寄存器
	 * 设置VSYNC总周期
	 */
	LCDIF->VDCTRL1 = tftlcd_dev.height + tftlcd_dev.vspw + tftlcd_dev.vfpd + tftlcd_dev.vbpd;

	/*
	  * 初始化ELCDIF的VDCTRL2寄存器
	  * 设置HSYNC周期
	  * bit[31:18] ：hsw
	  * bit[17:0]  : HSYNC总周期
	  */
	LCDIF->VDCTRL2 |= (tftlcd_dev.hspw << 18) | (tftlcd_dev.width + tftlcd_dev.hspw + tftlcd_dev.hfpd + tftlcd_dev.hbpd);

	/*
	 * 初始化ELCDIF的VDCTRL3寄存器
	 * 设置HSYNC周期
	 * bit[27:16] ：水平等待时钟数
	 * bit[15:0]  : 垂直等待时钟数
	 */
	LCDIF->VDCTRL3 |= ((tftlcd_dev.hspw + tftlcd_dev.hbpd) << 16) | (tftlcd_dev.vbpd + tftlcd_dev.vspw);

	/*
	 * 初始化ELCDIF的VDCTRL4寄存器
	 * 设置HSYNC周期
	 * bit[18] 1 : 当使用VSHYNC、HSYNC、DOTCLK的话此为置1
	 * bit[17:0]  : 宽度
	 */
	LCDIF->VDCTRL4 |= (1 << 18) | tftlcd_dev.width;

	/*
     * 初始化ELCDIF的CUR_BUF和NEXT_BUF寄存器
     * 设置当前显存地址和下一帧的显存地址
	 */
	LCDIF->CUR_BUF = tftlcd_dev.framebuffer;
	LCDIF->NEXT_BUF = tftlcd_dev.framebuffer;

	/* 使能LCD */
	lcd_enable();
	delayms(10);

	/* 清屏 */
	lcd_clear(LCD_WHITE);

	LCD_INFO("init lcd success!");
}

/*
 * 读取屏幕ID，
 * 描述：LCD_DATA23=R7(M0);LCD_DATA15=G7(M1);LCD_DATA07=B7(M2);
 * 		M2:M1:M0
 *		0 :0 :0	//4.3寸480*272 RGB屏,ID=0X4342
 *		0 :0 :1	//7寸800*480 RGB屏,ID=0X7084
 *	 	0 :1 :0	//7寸1024*600 RGB屏,ID=0X7016
 *  	1 :0 :1	//10.1寸1280*800,RGB屏,ID=0X1018
 *		1 :0 :0	//4.3寸800*480 RGB屏,ID=0X4384
 * @param 		: 无
 * @return 		: 屏幕ID
 */
unsigned short lcd_read_panelid(void)
{
	unsigned char idx = 0;
	gpio_pin_config_t idio_config;
	unsigned short panel_types[] = {
		ATK4342,
		ATK7084,
		ATK7016,
		0,
		ATK4384,
		ATK1018,
		0,
		ATKVGA
	};

	/* 配置屏幕ID信号线 */
	IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0);
	IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_GPIO3_IO03, 0X10B0);

	/* 打开模拟开关 */
	idio_config.direction = kGPIO_DigitalOutput;
	idio_config.outputLogic = 1;
	gpio_init(GPIO3, 3, &idio_config);

	/* 读取ID值，设置G7 B7 R7为输入 */
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_GPIO3_IO12, 0);		/* B7(M2) */
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_GPIO3_IO20, 0);		/* G7(M1) */
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_GPIO3_IO28, 0);		/* R7(M0) */
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_GPIO3_IO12, 0xF080);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_GPIO3_IO20, 0xF080);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_GPIO3_IO28, 0xF080);

	idio_config.direction = kGPIO_DigitalInput;
	gpio_init(GPIO3, 12, &idio_config);
	gpio_init(GPIO3, 20, &idio_config);
	gpio_init(GPIO3, 28, &idio_config);

	idx = (unsigned char)gpio_pinread(GPIO3, 28); 	/* 读取M0 */
	idx |= (unsigned char)gpio_pinread(GPIO3, 20) << 1;	/* 读取M1 */
	idx |= (unsigned char)gpio_pinread(GPIO3, 12) << 2;	/* 读取M2 */

	if (7 < idx)
	{
		LCD_ERR("get invalid idx: %u", idx);
	}

	return panel_types[idx];
}

/*
 * IO引脚: 	LCD_DATA00 -> LCD_B0
 *			LCD_DATA01 -> LCD_B1
 *			LCD_DATA02 -> LCD_B2
 *			LCD_DATA03 -> LCD_B3
 *			LCD_DATA04 -> LCD_B4
 *			LCD_DATA05 -> LCD_B5
 *			LCD_DATA06 -> LCD_B6
 *			LCD_DATA07 -> LCD_B7
 *
 *			LCD_DATA08 -> LCD_G0
 *			LCD_DATA09 -> LCD_G1
 *			LCD_DATA010 -> LCD_G2
 *			LCD_DATA011 -> LCD_G3
 *			LCD_DATA012 -> LCD_G4
 *			LCD_DATA012 -> LCD_G4
 *			LCD_DATA013 -> LCD_G5
 *			LCD_DATA014 -> LCD_G6
 *			LCD_DATA015 -> LCD_G7
 *
 *			LCD_DATA016 -> LCD_R0
 *			LCD_DATA017 -> LCD_R1
 *			LCD_DATA018 -> LCD_R2 
 *			LCD_DATA019 -> LCD_R3
 *			LCD_DATA020 -> LCD_R4
 *			LCD_DATA021 -> LCD_R5
 *			LCD_DATA022 -> LCD_R6
 *			LCD_DATA023 -> LCD_R7
 *
 *			LCD_CLK -> LCD_CLK
 *			LCD_VSYNC -> LCD_VSYNC
 *			LCD_HSYNC -> LCD_HSYNC
 *			LCD_DE -> LCD_DE
 *			LCD_BL -> GPIO1_IO08 
 */

/*
 * @description	: LCD GPIO初始化
 * @param 		: 无
 * @return 		: 无
 */
void lcdgpio_init(void)
{
	gpio_pin_config_t gpio_config;

	/* 1、IO初始化复用功能 */
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA00_LCDIF_DATA00,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA01_LCDIF_DATA01,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA02_LCDIF_DATA02,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA03_LCDIF_DATA03,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA04_LCDIF_DATA04,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA05_LCDIF_DATA05,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA06_LCDIF_DATA06,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA07_LCDIF_DATA07,0);

	IOMUXC_SetPinMux(IOMUXC_LCD_DATA08_LCDIF_DATA08,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA09_LCDIF_DATA09,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA10_LCDIF_DATA10,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA11_LCDIF_DATA11,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA12_LCDIF_DATA12,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA13_LCDIF_DATA13,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA14_LCDIF_DATA14,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA15_LCDIF_DATA15,0);

	IOMUXC_SetPinMux(IOMUXC_LCD_DATA16_LCDIF_DATA16,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA17_LCDIF_DATA17,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA18_LCDIF_DATA18,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA19_LCDIF_DATA19,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA20_LCDIF_DATA20,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA21_LCDIF_DATA21,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA22_LCDIF_DATA22,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_DATA23_LCDIF_DATA23,0);

	IOMUXC_SetPinMux(IOMUXC_LCD_CLK_LCDIF_CLK,0);	
	IOMUXC_SetPinMux(IOMUXC_LCD_ENABLE_LCDIF_ENABLE,0);	
	IOMUXC_SetPinMux(IOMUXC_LCD_HSYNC_LCDIF_HSYNC,0);
	IOMUXC_SetPinMux(IOMUXC_LCD_VSYNC_LCDIF_VSYNC,0);

	/* 背光BL引脚 */
	IOMUXC_SetPinMux(IOMUXC_GPIO1_IO08_GPIO1_IO08,0);
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);

	/* 2、配置LCD IO属性	
	 *bit 16:0 HYS关闭
	 *bit [15:14]: 0 默认22K上拉
	 *bit [13]: 0 pull功能
	 *bit [12]: 0 pull/keeper使能 
	 *bit [11]: 0 关闭开路输出
	 *bit [7:6]: 10 速度100Mhz
	 *bit [5:3]: 111 驱动能力为R0/7
	 *bit [0]: 1 高转换率
	 */
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA00_LCDIF_DATA00,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA01_LCDIF_DATA01,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA02_LCDIF_DATA02,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA03_LCDIF_DATA03,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA04_LCDIF_DATA04,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA05_LCDIF_DATA05,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA06_LCDIF_DATA06,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA07_LCDIF_DATA07,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA08_LCDIF_DATA08,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA09_LCDIF_DATA09,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA10_LCDIF_DATA10,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA11_LCDIF_DATA11,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA12_LCDIF_DATA12,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA13_LCDIF_DATA13,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA14_LCDIF_DATA14,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA15_LCDIF_DATA15,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA16_LCDIF_DATA16,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA17_LCDIF_DATA17,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA18_LCDIF_DATA18,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA19_LCDIF_DATA19,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA20_LCDIF_DATA20,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA21_LCDIF_DATA21,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA22_LCDIF_DATA22,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_DATA23_LCDIF_DATA23,0xB9);

	IOMUXC_SetPinConfig(IOMUXC_LCD_CLK_LCDIF_CLK,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_ENABLE_LCDIF_ENABLE,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_HSYNC_LCDIF_HSYNC,0xB9);
	IOMUXC_SetPinConfig(IOMUXC_LCD_VSYNC_LCDIF_VSYNC,0xB9);

	/* 背光BL引脚 */
	IOMUXC_SetPinConfig(IOMUXC_GPIO1_IO08_GPIO1_IO08,0xB9);

	/* GPIO初始化 */
	gpio_config.direction = kGPIO_DigitalOutput;			/* 输出 			*/
	gpio_config.outputLogic = 1; 							/* 默认关闭背光 */
	gpio_init(GPIO1, 8, &gpio_config);						/* 背光默认打开 */
	gpio_pinwrite(GPIO1, 8, 1);								/* 打开背光     */
}

/*
 * @description		: LCD时钟初始化, LCD时钟计算公式如下：
 *                	  LCD CLK = 24 * loopDiv / prediv / div
 * @param -	loopDiv	: loopDivider值
 * @param -	prediv  : lcdifprediv值
 * @param -	div		: lcdifdiv值
 * @return 			: 无
 */
void lcdclk_init(unsigned char loopDiv, unsigned char prediv, unsigned char div)
{
	/* 先初始化video pll 
     * VIDEO PLL = OSC24M * (loopDivider + (denominator / numerator)) / postDivider
 	 *不使用小数分频器，因此denominator和numerator设置为0
 	 */
 	CCM_ANALOG->PLL_VIDEO_NUM = 0;		/* 不使用小数分频器 */
	CCM_ANALOG->PLL_VIDEO_DENOM = 0;

	/*
     * PLL_VIDEO寄存器设置
     * bit[13]:    1   使能VIDEO PLL时钟
     * bit[20:19]  2  设置postDivider为1分频
     * bit[6:0] : 32  设置loopDivider寄存器
	 */
	CCM_ANALOG->PLL_VIDEO =  (2 << 19) | (1 << 13) | (loopDiv << 0);

	/*
     * MISC2寄存器设置
     * bit[31:30]: 0  VIDEO的post-div设置，时钟源来源于postDivider，1分频
	 */
	CCM_ANALOG->MISC2 &= ~(3 << 30);
	CCM_ANALOG->MISC2 = 0 << 30;

	/* LCD时钟源来源与PLL5，也就是VIDEO           PLL  */
	CCM->CSCDR2 &= ~(7 << 15);  	
	CCM->CSCDR2 |= (2 << 15);			/* 设置LCDIF_PRE_CLK使用PLL5 */

	/* 设置LCDIF_PRE分频 */
	CCM->CSCDR2 &= ~(7 << 12);		
	CCM->CSCDR2 |= (prediv - 1) << 12;	/* 设置分频  */

	/* 设置LCDIF分频 */
	CCM->CBCMR &= ~(7 << 23);					
	CCM->CBCMR |= (div - 1) << 23;

	/* 设置LCD时钟源为LCDIF_PRE时钟 */
	CCM->CSCDR2 &= ~(7 << 9);					/* 清除原来的设置		 	*/
	CCM->CSCDR2 |= (0 << 9);					/* LCDIF_PRE时钟源选择LCDIF_PRE时钟 */
}

/*
 * @description	: 复位ELCDIF接口
 * @param 		: 无
 * @return 		: 无
 */
void lcd_reset(void)
{
	LCDIF->CTRL  = 1<<31; /* 强制复位 */
}

/*
 * @description	: 结束复位ELCDIF接口
 * @param 		: 无
 * @return 		: 无
 */
void lcd_noreset(void)
{
	LCDIF->CTRL  = 0<<31; /* 取消强制复位 */
}

/*
 * @description	: 使能ELCDIF接口
 * @param 		: 无
 * @return 		: 无
 */
void lcd_enable(void)
{
	LCDIF->CTRL |= 1<<0; /* 使能ELCDIF */
}

void video_pllinit(unsigned char loopdivi, unsigned char postdivi);

/*
 * @description		: 画点函数 
 * @param - x		: x轴坐标
 * @param - y		: y轴坐标
 * @param - color	: 颜色值
 * @return 			: 无
 */
inline void lcd_drawpoint(unsigned short x,unsigned short y,unsigned int color)
{
	*(unsigned int*)((unsigned int)tftlcd_dev.framebuffer + 
		             tftlcd_dev.pixsize * (tftlcd_dev.width * y+x)) = color;
}

/*
 * @description		: 读取指定点的颜色值
 * @param - x		: x轴坐标
 * @param - y		: y轴坐标
 * @return 			: 读取到的指定点的颜色值
 */
inline unsigned int lcd_readpoint(unsigned short x,unsigned short y)
{
	return *(unsigned int*)((unsigned int)tftlcd_dev.framebuffer + 
		   tftlcd_dev.pixsize * (tftlcd_dev.width * y + x));
}

/*
 * @description		: 清屏
 * @param - color	: 颜色值
 * @return 			: 读取到的指定点的颜色值
 */
void lcd_clear(unsigned int color)
{
	unsigned int num;
	unsigned int i = 0;
	unsigned int *startaddr=(unsigned int*)tftlcd_dev.framebuffer;	//指向帧缓存首地址

	num=(unsigned int)tftlcd_dev.width * tftlcd_dev.height;			//缓冲区总长度

	for(i = 0; i < num; i++)
	{
		startaddr[i] = color;
	}
}

/*
 * @description		: 以指定的颜色填充一块矩形
 * @param - x0		: 矩形起始点坐标X轴
 * @param - y0		: 矩形起始点坐标Y轴
 * @param - x1		: 矩形终止点坐标X轴
 * @param - y1		: 矩形终止点坐标Y轴
 * @param - color	: 要填充的颜色
 * @return 			: 读取到的指定点的颜色值
 */
void lcd_fill(unsigned    short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned int color)
{ 
    unsigned short x, y;

	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(x1 >= tftlcd_dev.width) x1 = tftlcd_dev.width - 1;
	if(y1 >= tftlcd_dev.height) y1 = tftlcd_dev.height - 1;
	
    for(y = y0; y <= y1; y++)
    {
        for(x = x0; x <= x1; x++)
			lcd_drawpoint(x, y, color);
    }
}

