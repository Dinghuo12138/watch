#ifndef __HAL_MAX30102
#define __HAL_MAX30102
#include "main.h"



//芯片ID寄存器(Part ID寄存器)，默认值0x15
#define MAX30102_REG_ID 0xFF

//复位芯片(Mode Configuration,bit6 是 RESET位，写1后芯片复位，自动清零)
#define MAX30102_REG_RESET 0x09

// 清空 FIFO(FIFO Write Pointer   Overflow Counter   FIFO Read Pointer   开始采集前全部清0)
#define MAX30102_REG_FIFO_WR_PTR 0x04//先进先出写指针寄存器
#define MAX30102_REG_FIFO_OF_CN 0x05// FIFO溢出计数寄存器
#define MAX30102_REG_FIFO_RD_PTR 0x06// 先进先出读指针寄存器

// 配置 FIFO(FIFO Configuration,   作用：采样平均、FIFO 满处理、快满中断阈值)
#define MAX30102_REG_FIFO_CONFIG 0x08   //常用初始值0x4F,高 3 位是采样平均 SMP_AVE，bit4 是 FIFO 满后是否回卷，低 4 位是 FIFO almost full 阈值

// 配置采样率 / ADC量程 / LED脉宽(SpO2 参数)
#define MAX30102_REG_SPO2_CONFIG 0x0A   //初始值0x27,高 3 位是采样率，bit3 是 ADC量程，低 4 位是 LED脉宽
// 配置红光和红外 LED 电流(LED Pulse Amplitude)

#define MAX30102_REG_RED_PA 0x0C   //红外 LED 电流，初始值0x24,每步 0.2mA(0xFF 最大 50mA)
#define MAX30102_REG_IR_PA 0x0D   //红光 LED 电流，初始值0x24,每步 0.2mA

// 进入 SpO2 模式(工作模式:Mode Configuration)
#define MAX30102_REG_MODE_CONFIG 0x09   //初始值0x03,bit(2-0) 是工作模式，常用 SpO2 模式 0x03 （即011，spO2模式，红光+红外）

// 读取 FIFO 数据(IFO Data Register)
#define MAX30102_REG_FIFO_DATA 0x07   //FIFO 数据寄存器，连续读 6 字节，红光和红外各 3 字节，24 位数据，最高位无效

//温度整数部分测量
#define MAX30102_REG_TEMP_INTEGER 0x1F

//温度小数部分测量
#define MAX30102_REG_TEMP_FRACTION 0x20

//启动温度测量
#define MAX30102_REG_TEMP_CONFIG 0x21

#endif

