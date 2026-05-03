#ifndef _HAL_MYI2C_
#define _HAL_MYI2C_

#include "main.h"
void myi2c_W_SCL(uint8_t pin_value);
//控制写入sda电平函数，0：低电平，1：高电平
void myi2c_W_SDA(uint8_t pin_value);

//控制读取sda电平函数，返回0：低电平，返回1：高电平
uint8_t myi2c_R_SDA(void);

//初始化函数
void MyI2C_Init(void);

//基本模块代码

//起始
void myi2c_start(void);

//终止
void myi2c_stop(void);

//发送一个字节
void myi2c_sendByte(uint8_t Byte);

//接收一个字节
uint8_t myi2c_receiveByte(void);

//发送应答
void myi2c_sendack(uint8_t ack);

//接收应答
uint8_t myi2c_receiveack(void);

//遍历查询从机地址
void myi2c_addr_scan(void);



#endif


