	#ifndef __DELAY_H
	#define __DELAY_H
	#include "main.h" // 确保包含HAL库头文件，以获取uint32_t等定义
	void delay_init(void);   // 初始化DWT计数器，需在main函数开头调用一次
	void delay_us(uint32_t us); // 微秒级延时函数
	void delay_ms(uint32_t ms); // 毫秒级延时函数(基于DWT实现，可不依赖SysTick)
	#endif /* __DELAY_H */

