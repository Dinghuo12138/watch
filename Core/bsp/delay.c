	#include "delay.h"
	/**
	  * @brief  初始化DWT计数器
	  * @note   必须在main函数的while(1)之前调用一次
	  */
	void delay_init(void)
	{
	    /* 使能DWT外设 */
	    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	    /* 清零DWT计数器 */
	    DWT->CYCCNT = 0;
	    /* 使能DWT计数器 */
	    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
	/**
	  * @brief  通用的微秒级延时函数
	  * @param  us: 需要延时的微秒数
	  * @note   使用DWT计数器，不影响SysTick和中断，适用于全系列STM32(Cortex-M3/M4/M7等)
	  */
	void delay_us(uint32_t us)
	{
	    uint32_t start_tick = DWT->CYCCNT;
	    /* SystemCoreClock是HAL库定义的系统主频变量，如72000000
	       计算需要的时钟周期数，us * (SystemCoreClock / 1000000) */
	    uint32_t ticks = us * (SystemCoreClock / 1000000);
	    /* 等待计数器增长到目标值 */
	    while ((DWT->CYCCNT - start_tick) < ticks);
	}
	/**
	  * @brief  通用的毫秒级延时函数
	  * @param  ms: 需要延时的毫秒数
	  * @note   这里基于delay_us实现，完全解放了SysTick，即使在SysTick被禁用或中断被屏蔽的场景也能正常延时
	  */
	void delay_ms(uint32_t ms)
	{
	    while (ms--)
	    {
	        delay_us(1000);
	    }
	}
