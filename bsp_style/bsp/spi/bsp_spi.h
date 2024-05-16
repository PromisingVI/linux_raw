/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 bsp_spi.h
作者	   : 刘有为
版本	   : V1.0
描述	   : SPI驱动头文件。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版V1.0 2024/5/15 刘有为创建
***************************************************************/
#ifndef _BSP_SPI_H
#define _BSP_SPI_H

#include "imx6ul.h"

#define SPI_INFO(format, ...)	printf("spi info(%s, %d): "format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define SPI_DBG(format, ...)	printf("spi warning(%s, %d): "format"\r\n", __func__, __LINE__, ##__VA_ARGS__)
#define SPI_ERR(format, ...)	printf("spi error(%s, %d): "format"\r\n", __func__, __LINE__, ##__VA_ARGS__)

/* 函数声明 */
void spi_init(ECSPI_Type *base);
unsigned char spich0_readwrite_byte(ECSPI_Type *base, unsigned char txdata);

#endif /* _BSP_SPI_H */

