#ifndef _HAL_MPU6050_
#define _HAL_MPU6050_

#include "main.h"

struct mpu6050_OUT{
	int16_t AccX;
	int16_t AccY;
	int16_t AccZ;
	double temp; 
	int16_t GyroX;
	int16_t GyroY;
	int16_t GyroZ;
};

void mpu6050_init(void);
void MPU6050_WriteReg(uint8_t reg,uint8_t data);
uint8_t MPU6050_ReadReg(uint8_t reg);
uint8_t getID(void);
void mpu6050_GetValue(struct mpu6050_OUT *data);
#endif


