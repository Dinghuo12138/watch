#include "myrtc.h"
#include "rtc.h"      // 必须包含，以使用CubeMX生成的 hrtc 句柄
#include <time.h>
	uint16_t MyRTC_Time[] = {2026,4, 21, 18, 51, 00}; // 年、月、日、时、分、秒
	/**
	  * 函    数：RTC初始化(应用层)
	  * 参    数：无
	  * 返 回 值：无
	  * 说    明：硬件初始化已由CubeMX的MX_RTC_Init完成，此处仅判断是否需要更新时间
	  */
	void MyRTC_Init(void)
	{
		// 硬件底层（PWR、BKP、LSE、预分频器）已在CubeMX生成的 rtc.c 中完成配置
		// 这里只做标志位判断，决定是否把数组里的时间写入RTC
		if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0xA5A5)
		{
			MyRTC_SetTime(); // 将全局数组的时间写入RTC计数器
			HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0xA5A5); // 写入标志位，代表已初始化
		}
	}
	/**
	  * 函    数：RTC设置时间
	  * 参    数：无
	  * 返 回 值：无
	  * 说    明：调用此函数后，全局数组里时间值将刷新到RTC硬件电路
	  */
	void MyRTC_SetTime(void)
	{
		time_t time_cnt;		// 定义秒计数器数据类型
		struct tm time_date;	// 定义日期时间数据类型
		time_date.tm_year = MyRTC_Time[0] - 1900;		// 将数组的时间赋值给日期时间结构体
		time_date.tm_mon  = MyRTC_Time[1] - 1;
		time_date.tm_mday = MyRTC_Time[2];
		time_date.tm_hour = MyRTC_Time[3];
		time_date.tm_min  = MyRTC_Time[4];
		time_date.tm_sec  = MyRTC_Time[5];
		// 调用mktime函数，将日期时间转换为秒计数器格式
		// - 8 * 60 * 60 为东八区的时区调整，将本地时间转为UTC时间存入计数器
		time_cnt = mktime(&time_date) - 8 * 60 * 60;	
		// F1系列HAL库无直接写CNT的函数，需直接操作寄存器
		HAL_RTC_WaitForSynchro(&hrtc);					// 等待同步
		while((RTC->CRL & RTC_CRL_RTOFF) == RESET);	// 等待上一次操作完成(RTOFF标志位)
		RTC->CRL |= RTC_CRL_CNF;						// 进入配置模式 (F1的HAL宏也是这样写的)
		RTC->CNTH = (time_cnt >> 16) & 0xFFFF;			// 写入计数器高16位
		RTC->CNTL = time_cnt & 0xFFFF;					// 写入计数器低16位
		RTC->CRL &= ~RTC_CRL_CNF;						// 退出配置模式
		while((RTC->CRL & RTC_CRL_RTOFF) == RESET);	// 等待写完成
	}
	/**
	  * 函    数：RTC读取时间
	  * 参    数：无
	  * 返 回 值：无
	  * 说    明：调用此函数后，RTC硬件电路里时间值将刷新到全局数组
	  */
	void MyRTC_ReadTime(void)
	{
		time_t time_cnt;		// 定义秒计数器数据类型
		struct tm time_date;	// 定义日期时间数据类型
		// F1系列无直接读CNT的宏，直接拼接寄存器（与HAL库底层读取逻辑一致）
		time_cnt = ((uint32_t)(RTC->CNTH) << 16) | (RTC->CNTL);
		// + 8 * 60 * 60 为东八区的时区调整，将UTC时间转回本地时间
		time_cnt += 8 * 60 * 60;						
		// 使用localtime函数，将秒计数器转换为日期时间格式
		time_date = *localtime(&time_cnt);				
		MyRTC_Time[0] = time_date.tm_year + 1900;		// 将日期时间结构体赋值给数组的时间
		MyRTC_Time[1] = time_date.tm_mon + 1;
		MyRTC_Time[2] = time_date.tm_mday;
		MyRTC_Time[3] = time_date.tm_hour;
		MyRTC_Time[4] = time_date.tm_min;
		MyRTC_Time[5] = time_date.tm_sec;
	}
