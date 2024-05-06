/***************************************************************
Copyright © liuyouwei Co., Ltd. 1998-2019. All rights reserved.
文件名	: 	 start.s
作者	   : 刘有为
版本	   : V1.0
描述	   : ZERO-I.MX6UL/I.MX6ULL开发板启动文件，完成C环境初始化，
		 C环境初始化完成以后跳转到C代码。
其他	   : 无
论坛 	   : www.wtmembed.com
日志	   : 初版 2024/4/28 刘有为修改
**************************************************************/

.global _start  		/* 全局标号 */

/*
 * 描述：	_start函数，首先是中断向量表的创建
 * 参考文档:ARM Cortex-A(armV7)编程手册V4.0.pdf P42，3 ARM Processor Modes and Registers（ARM处理器模型和寄存器）
 * 		 	ARM Cortex-A(armV7)编程手册V4.0.pdf P165 11.1.1 Exception priorities(异常)
 */
_start:
	ldr pc, =Reset_Handler			/* 复位中断 	*/
	ldr pc, =Undefined_Handler		/* 未定义中断 */
	ldr pc, =SVC_Handler			/* SVC(Supervisor)中断 */
	ldr pc, =PrefAbort_Handler		/* 预取终止中断 */
	ldr pc, =DataAbort_Handler		/* 数据终止中断 */
	ldr pc, =NotUsed_Handler		/* 未使用中断 */
	ldr pc, =IRQ_Handler			/* IRQ中断 */
	ldr pc, =FIQ_Handler			/* FIQ(快速中断) */

/* 复位中断 */
Reset_Handler:
	cpsid i	/* 禁用IRQ中断 */

	/* 关闭I,DCache和MMU 
	 * 采取读-改-写的方式。
	 */
	mrc p15 , 0, r0, c1, c0, 0		/* 读取CP15的C1寄存器到R0中 */
	bic r0, r0, #(0x1 << 12) 		/* 清除C1寄存器的bit12位(I位)，关闭I Cache */
	bic r0, r0, #(0x1 << 2)			/* 清除C1寄存器的bit2(C位)，关闭D Cache */
	bic r0, r0, #0x2				/* 清除C1寄存器的bit1(A位)，关闭对齐 */
	bic r0, r0, #(0x1 << 11)		/* 清除C1寄存器的bit11(Z位)，关闭分支预测 */
	bic r0, r0, #0x1				/* 清除C1寄存器的bit0(M位)，关闭MMU */
	mcr p15, 0, r0, c1, c0, 0		/* 将r0寄存器中的值写入到CP15的C1寄存器中 */

#if 0
	/* 汇编版本设置中断向量表偏移 */
	ldr r0, =0X87800000
	dsb	/* 数据同步屏障 */
	isb	/* 指令同步屏障 */
	mcr	p15, 0, r0, c12, c0, 0
	dsb
	isb
#endif

	/* 设置各个模式下的栈指针，
	 * 注意：IMX6UL的堆栈是向下增长的！
	 * 堆栈指针地址一定要是4字节地址对齐的！！！
	 * DDR范围:0X80000000~0X9FFFFFFF
	 */

	/* 进入IRQ模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f		/* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 */
	orr r0, r0, #0x12		/* r0或上0x13,表示使用IRQ模式 */
	msr cpsr, r0			/* 将r0 的数据写入到cpsr_c中 */
	ldr sp, =0x80600000		/* 设置IRQ模式下的栈首地址为0X80600000,大小为2MB */

	/* 进入SYS模式 */
	bic r0, r0, #0x1f		/* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 	*/
	orr r0, r0, #0x1f		/* r0或上0x1f,表示使用SYS模式 */
	msr cpsr, r0			/* 将r0 的数据写入到cpsr_c中 */
	ldr sp, =0x80400000		/* 设置SYS模式下的栈首地址为0X80400000,大小为2MB */

	/* 进入SVC模式 */
	mrs r0, cpsr
	bic r0, r0, #0x1f		/* 将r0寄存器中的低5位清零，也就是cpsr的M0~M4 */
	orr r0, r0, #0x13		/* r0或上0x13,表示使用SVC模式 */
	msr cpsr, r0
	ldr sp, =0X80200000		/* 设置SVC模式下的栈首地址为0X80200000,大小为2MB */

	/* 打开全局中断 */
	cpsie i

#if 0
	/* 使能IRQ中断 */
	mrs r0, cpsr
	bic r0, r0, #0x80		/* 将r0寄存器中bit7清零，也就是CPSR中的I位清零，表示允许IRQ中断 */
	msr cpsr, r0
#endif

	b main

/* 未定义中断 */
Undefined_Handler:
	ldr r0, =Undefined_Handler
	bx r0						/* 间接跳转到r0存放的地址中 */

SVC_Handler:
	ldr r0, =SVC_Handler
	bx r0

/* 预取终止中断 */
PrefAbort_Handler:
	ldr r0, =PrefAbort_Handler	
	bx r0

/* 数据终止中断 */
DataAbort_Handler:
	ldr r0, =DataAbort_Handler
	bx r0

/* 未使用的中断 */
NotUsed_Handler:

	ldr r0, =NotUsed_Handler
	bx r0

/* IRQ中断！重点！！！！！ */
IRQ_Handler:
	push {lr}			/* 保存lr地址 */
	push {r0-r3, r12}	/* 保存r0-r3，r12寄存器 */

	mrs r0, spsr		/*  读取spsr寄存器到r0 */
	push {r0}				/* 保存spsr寄存器 */

	/* 从CP15的C0寄存器内的值到R1寄存器中
	 * 参考文档ARM Cortex-A(armV7)编程手册V4.0.pdf P49
	 * Cortex-A7 Technical ReferenceManua.pdf P68 P138
	 */
	mrc p15, 4, r1, c15, c0, 0

	/* GIC基地址加0X2000，也就是GIC的CPU接口端基地址 */
	add r1, r1, #0x2000

	/* GIC的CPU接口端基地址加0X0C就是GICC_IAR寄存器，
	 * GICC_IAR寄存器保存这当前发生中断的中断号，我们要根据
	 * 这个中断号来绝对调用哪个中断服务函数
	 */
	 ldr r0, [r1, #0XC]

	/* 保存r0,r1 */
	 push {r0, r1}

	/* 进入SVC模式，允许其他中断再次进去 */
	cps #0x13

	/* 保存SVC模式的lr寄存器 */
	push {lr}

	/* 加载C语言中断处理函数到r2寄存器中 */
	ldr r2, =system_irq_handler
	blx r2

	/* 执行完C语言中断服务函数，lr出栈 */
	pop {lr}

	/* 进入IRQ模式 */
	cps #0x12
	pop {r0, r1}

	/* 中断执行完成，写EOIR */
	str r0, [r1, #0X10]

	pop {r0}

	/* 恢复spsr */
	msr spsr_cxsf, r0

	pop {r0-r3, r12}

	/* 将lr-4赋给pc */
	subs pc, lr, #4

/* FIQ中断 */
FIQ_Handler:
	ldr r0, =FIQ_Handler	
	bx r0	
