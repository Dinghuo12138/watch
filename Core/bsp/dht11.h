#ifndef __DHT11_H
#define __DHT11_H

#include "GPIO.h"
#include "main.h"

#define out 1
#define in 0

#define DHT11_GPIO_PORT GPIOA
#define DHT11_GPIO_PIN GPIO_PIN_11

#define DHT11_LOW HAL_GPIO_WritePin(DHT11_GPIO_PORT,DHT11_GPIO_PIN,GPIO_PIN_RESET);
#define DHT11_HIGH HAL_GPIO_WritePin(DHT11_GPIO_PORT,DHT11_GPIO_PIN,GPIO_PIN_SET);

void DHT11_Rst(void);
void DHT11_Mode(uint8_t mode);
uint8_t DHT11_Check(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(float *temp,float *humi);
uint8_t DHT11_Init (void);

#endif
