/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_delay.c
作者	   : 刘有为
版本	   : V1.0
描述	   : 延时文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/4/28 刘有为创建
***************************************************************/

#include "bsp_delay.h"

static void delay_short(volatile unsigned int n)
{
	while (n--) {}
}

void delay(volatile unsigned int n)
{
	while (n--)
	{
		delay_short(0x7ff);
	}
}

/*
 * @description	: 延时有关硬件初始化,主要是GPT定时器
				  GPT定时器时钟源选择ipg_clk=66Mhz
 * @param		: 无
 * @return 		: 无
 */
void delay_init(void)
{
	/* 清零，bit0也为0，即停止GPT       */
	GPT1->CR = 0;

	/* bit15置1进入软复位 */
	GPT1->CR = 1 << 15;

	/*等待复位完成 */
	while ((GPT1->CR >> 15) & 0x01);

	/*
	 * GPT的CR寄存器,GPT通用设置
	 * bit22:20	000 输出比较1的输出功能关闭，也就是对应的引脚没反应
	 * bit9:    0   Restart模式,当CNT等于OCR1的时候就产生中断
	 * bit8:6   001 GPT时钟源选择ipg_clk=66Mhz
	 * bit
	 */
	GPT1->CR = (1<<6);

	/*
	 * GPT的PR寄存器，GPT的分频设置
	 * bit11:0  设置分频值，设置为0表示1分频，
	 *          以此类推，最大可以设置为0XFFF，也就是最大4096分频
	 * 设置为65，即66分频，因此GPT1时钟为66M/(65+1)=1MHz
	 */
	GPT1->PR = 65;

	/*
	 * GPT的OCR1寄存器，GPT的输出比较1比较计数值，
	 *	GPT的时钟为1Mz，那么计数器每计一个值就是就是1us。
	 * 为了实现较大的计数，我们将比较值设置为最大的0XFFFFFFFF,
	 * 这样一次计满就是：0XFFFFFFFFus = 4294967296us = 4295s = 71.5min
	 * 也就是说一次计满最多71.5分钟，存在溢出
	 */
	GPT1->OCR[0] = 0XFFFFFFFF;

	/* 使能GPT1 */
	GPT1->CR |= 1<<0;

	/* 一下屏蔽的代码是GPT定时器中断代码，
	 * 如果想学习GPT定时器的话可以参考一下代码。
	 */
#if 0
	/*
	 * GPT的PR寄存器，GPT的分频设置
	 * bit11:0  设置分频值，设置为0表示1分频，
	 *          以此类推，最大可以设置为0XFFF，也就是最大4096分频
	 */
	GPT1->PR = 65;

	GPT1->OCR[0] = 500000;

	/*
	 * GPT的IR寄存器，使能通道1的比较中断
	 * bit0： 0 使能输出比较中断
	 */
	GPT1->IR |= 1 << 0;

	/*
	 * 使能GIC里面相应的中断，并且注册中断处理函数
	 */
	GIC_EnableIRQ(GPT1_IRQn);
	system_register_irq_handler(GPT1_IRQn, (system_irq_handler_t)gpt1_irq_handler, NULL);
#endif
}

/*
 * @description		: 微秒(us)级延时
 * @param - value	: 需要延时的us数,最大延时0XFFFFFFFFus
 * @return 			: 无
 */
void delayus(unsigned int t)
{
	unsigned long old_cnt = 0;
	unsigned long new_cnt = 0;
	unsigned long total_cnt = 0;

	old_cnt = GPT1->CNT;
	while(1)
	{
		/* GPT是向上计数器,并且没有溢出 */
		new_cnt = GPT1->CNT;
		if (new_cnt != old_cnt)
		{
			if (new_cnt > old_cnt)
			{
				total_cnt += new_cnt - old_cnt;
			}
			else
			{
				total_cnt += 0xFFFFFFFF - old_cnt + new_cnt;
			}
			old_cnt = new_cnt;
			if (total_cnt >= t)
			{
				break;
			}
		}
	}
}

void delayms(unsigned int t)
{
	int i = 0;

	for (i = 0; i < t; i++)
	{
		delayus(1000);
	}
}

#if 0
void gpt1_irq_handler(void)
{
	static unsigned char state = 0;

	state = !state;

	/*
     * GPT的SR寄存器，状态寄存器
     * bit2： 1 输出比较1发生中断
	 */
	if(GPT1->SR & (1<<0)) 
	{
		led_switch(LED0, state);
	}

	/* 清除中断标志位 */
	GPT1->SR |= 1<<0;
}
#endif

