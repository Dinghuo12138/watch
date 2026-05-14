#ifndef _HAL_MPU6050_
#define _HAL_MPU6050_

#include "main.h"
#include "menu.h"
#include <stdint.h>

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
void Sensor_WriteReg(uint16_t Sensor_add,uint8_t reg,uint8_t data);
uint8_t Sensor_ReadReg(uint16_t Sensor_add,uint8_t reg);
uint8_t mpu6050_getID(void);
void mpu6050_GetValue(struct mpu6050_OUT *data);
uint8_t max30102_getID(void);


void max30102_init(void);
void max30102_GetValue(void);
void max30102_clear_fifo(void);
void max30102_stop(void);

extern uint8_t max30102_sample_valid;
extern uint8_t max30102_status1;
extern uint8_t max30102_mode_reg;
extern uint32_t max30102_wr_ptr;
extern uint32_t max30102_rd_ptr;
#endif


