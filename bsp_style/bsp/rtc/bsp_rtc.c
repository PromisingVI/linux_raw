/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_rtc.c
作者	   : 刘有为
版本	   : V1.0
描述	   : RTC驱动文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/9 刘有为创建
***************************************************************/
#include "bsp_rtc.h"
#include "stdio.h"

/* 
 * 描述:初始化RTC
 */
void rtc_init(void)
{
	struct rtc_datetime rtcdate = {0};
	unsigned int year;  /* 范围为:1970 ~ 2099 		*/
	unsigned int month;  /* 范围为:1 ~ 12				*/
	unsigned int day;    /* 范围为:1 ~ 31 (不同的月，天数不同).*/
	unsigned int hour;   /* 范围为:0 ~ 23 			*/
	unsigned int minute; /* 范围为:0 ~ 59				*/
	unsigned int second; /* 范围为:0 ~ 59				*/

	/*
     * 设置HPCOMR寄存器
     * bit[31] 1 : 允许访问SNVS寄存器，一定要置1
     * bit[8]  1 : 此位置1，需要签署NDA协议才能看到此位的详细说明，
     *             这里不置1也没问题
	 */
	SNVS->HPCOMR |= (1<<31) | (1<<8);

	printf("请按照 年 月 日 时 分 秒 的格式输入当前日期:");
	scanf("%u %u %u %u %u %u",
		&year, &month, &day,
		&hour, &minute, &second);
	rtcdate.year = year;
	rtcdate.month = month;
	rtcdate.day = day;
	rtcdate.hour = hour;
	rtcdate.minute = minute;
	rtcdate.second = second;
	RTC_INFO("set time %u-%u-%u %u-%u-%u", rtcdate.year, rtcdate.month, rtcdate.day, rtcdate.hour, rtcdate.minute, rtcdate.second);
	rtc_setdatetime(&rtcdate);

	rtc_enable();
}

/*
 * 描述: 开启RTC
 */
void rtc_enable(void)
{
	/*
	 * LPCR寄存器bit0置1，使能RTC
 	 */
 	SNVS->LPCR |= (1 << 0);
 	/* 等待使能完成 */
 	while (0 == (SNVS->LPCR & 0x01));
}

/*
 * 描述: 关闭RTC
 */
void rtc_disable(void)
{
	/*
	 * LPCR寄存器bit0置0，关闭RTC
 	 */
 	SNVS->LPCR &= ~(1 << 0);
 	/* 等待关闭完成 */
 	while (1 == (SNVS->LPCR & 0x01));
}

/*
 * @description	: 判断指定年份是否为闰年，闰年条件如下:
 * @param - year: 要判断的年份
 * @return 		: RTC_IS_LEAP_YEAR 是闰年，RTC_NOT_LEAP_YEAR 不是闰年
 */
unsigned char rtc_isleapyear(unsigned short year)
{
	if (0 == year % 400)
	{
		return RTC_IS_LEAP_YEAR;
	}
	else
	{
		return ((0 == year % 4) && (0 != year % 100)) ? RTC_IS_LEAP_YEAR : RTC_NOT_LEAP_YEAR;
	}
}

/*
 * @description		: 将时间转换为秒数
 * @param - datetime: 要转换日期和时间。
 * @return 			: 转换后的秒数
 */
unsigned int rtc_coverdate_to_seconds(struct rtc_datetime *datetime)
{
	unsigned short i = 0;
	unsigned int seconds = 0;
	unsigned int days = 0;
	unsigned short monthdays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};

	if (NULL == datetime)
	{
		RTC_ERR("param null!");
		return 0;
	}

	for (i = 1970; i < datetime->year; i++)
	{
		days += DAYS_IN_A_YEAR;
		if (RTC_IS_LEAP_YEAR == rtc_isleapyear(i))
		{
			days += 1;
		}
	}

	days += monthdays[datetime->month];
	if (RTC_IS_LEAP_YEAR == rtc_isleapyear(i) && 3 <= datetime->month)
	{
		days += 1;
	}
	days += datetime->day - 1;

	seconds = days * SECONDS_IN_A_DAY +
			  datetime->hour * SECONDS_IN_A_HOUR +
			  datetime->minute * SECONDS_IN_A_MINUTE +
			  datetime->second;

	return seconds;
}

/*
 * @description	: 获取RTC当前秒数
 * @param 		: 无
 * @return 		: 当前秒数 
 */
unsigned int rtc_getseconds(void)
{
	unsigned int seconds = 0;
	seconds = (SNVS->LPSRTCMR << 17) | (SNVS->LPSRTCLR >> 15);
	return seconds;
}

/*
 * @description		: 设置时间和日期
 * @param - datetime: 要设置的日期和时间
 * @return 			: 无
 */
void rtc_setdatetime(struct rtc_datetime *datetime)
{
	unsigned int seconds = 0;
	unsigned int ori_lpcr = SNVS->LPCR; 

	if (NULL == datetime)
	{
		RTC_ERR("param null!");
		return;
	}

	/* 设置寄存器HPRTCMR和HPRTCLR的时候一定要先关闭RTC */
	rtc_disable();

	/* 先将时间转换为秒 */
	seconds = rtc_coverdate_to_seconds(datetime);

	SNVS->LPSRTCMR = (unsigned int)(seconds >> 17); /* 设置高16位 */
	SNVS->LPSRTCLR = (unsigned int)(seconds << 15); /* 设置地16位 */

	/* 如果此前RTC是打开的在设置完RTC时间以后需要重新打开RTC */
	if (ori_lpcr & 0x1)
	{
		rtc_enable();
	}
}

/*
 * @description		: 将秒数转换为时间
 * @param - seconds	: 要转换的秒数
 * @param - datetime: 转换后的日期和时间
 * @return 			: 无
 */
void rtc_convertseconds_to_datetime(u64 seconds, struct rtc_datetime *datetime)
{
	int i = 0;
	u64 secondsRemaining = 0;
	u64 days = 0;
	unsigned short daysInYear = 0;
	unsigned char daysPerMonth[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

	if (NULL == datetime)
	{
		RTC_ERR("param null!");
		return;
	}

	/* 剩余秒数初始化 */
	secondsRemaining = seconds;
	days = secondsRemaining / SECONDS_IN_A_DAY + 1;
	/*计算天数以后剩余的秒数 */
	secondsRemaining = seconds % SECONDS_IN_A_DAY;

	/* 计算时、分、秒 */
	datetime->hour = secondsRemaining / SECONDS_IN_A_HOUR;
	secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
	datetime->minute = secondsRemaining / SECONDS_IN_A_MINUTE;
	datetime->second = secondsRemaining % SECONDS_IN_A_MINUTE;

	/* 计算年 */
	daysInYear = DAYS_IN_A_YEAR;
	datetime->year = YEAR_RANGE_START;
	while (days > daysInYear)
	{
		/* 根据天数计算年 */
		days -= daysInYear;
		datetime->year++;

		/* 处理闰年 */
		if (RTC_IS_LEAP_YEAR == rtc_isleapyear(datetime->year))
		{
			daysInYear = DAYS_IN_A_YEAR + 1;
		}
		else
		{
			daysInYear = DAYS_IN_A_YEAR;
		}
	}

	/*根据剩余的天数计算月份 */
	if (RTC_IS_LEAP_YEAR == rtc_isleapyear(datetime->year))
	{
		daysPerMonth[2] = 29U;
	}

	for (i = 0; i < 12; i++)
	{
		if (days <= daysPerMonth[i])
		{
			datetime->month = i;
			break;
		}
		else
		{
			days -= daysPerMonth[i];
		}
	}

	datetime->day = days;
}

/*
 * @description		: 获取当前时间
 * @param - datetime: 获取到的时间，日期等参数
 * @return 			: 无 
 */
void rtc_getdatetime(struct rtc_datetime *datetime)
{
	u64 seconds = 0;

	if (NULL == datetime)
	{
		RTC_ERR("param null!");
		return;
	}

	seconds = rtc_getseconds();
	rtc_convertseconds_to_datetime(seconds, datetime);
}

