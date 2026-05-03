#include "mpu6050.h"
#include "myi2c.h"
#include "mpu6050reg.h"

#define MPU6050_ADDRESS 0xD0

//初始化
void mpu6050_init(){
	MyI2C_Init();
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);		//电源管理寄存器1，取消睡眠模式，选择时钟源为系统时钟源
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

//指定寄存器写
void MPU6050_WriteReg(uint8_t reg,uint8_t data){
	myi2c_start();
	myi2c_sendByte(MPU6050_ADDRESS);
	myi2c_receiveack();
	myi2c_sendByte(reg);
	myi2c_receiveack();
	myi2c_sendByte(data);
	myi2c_receiveack();
	myi2c_stop();
}

//指定寄存器读

uint8_t MPU6050_ReadReg(uint8_t reg){
	
	uint8_t data;
	
	myi2c_start();
	myi2c_sendByte(MPU6050_ADDRESS);
	myi2c_receiveack();
	myi2c_sendByte(reg); 
	myi2c_receiveack();	
	
	myi2c_start();
	myi2c_sendByte(MPU6050_ADDRESS|0x01); 	//读写位置1，读
	myi2c_receiveack();
	data=myi2c_receiveByte(); 				//接收指定寄存器的数据
	myi2c_sendack(1);						//主机给从机发送非应答，停止读

	myi2c_stop();
	
	return data;
}

//获取寄存器ID
uint8_t getID(void){
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}


//获取寄存器中的数据
void mpu6050_GetValue(struct mpu6050_OUT *data){
	uint8_t i;
	uint8_t buf[14];
	for(i=0;i<14;i++){
		buf[i]=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H+i);//循环接收数据0x3B->0x48
	}
	data->AccX=(buf[0]<<8)+buf[1];
	data->AccY=(buf[2]<<8)+buf[3];
	data->AccZ=(buf[4]<<8)+buf[5];
	data->temp=(buf[6]<<8)+buf[7];
	data->GyroX=(buf[8]<<8)+buf[9];
	data->GyroY=(buf[10]<<8)+buf[11];
	data->GyroZ=(buf[12]<<8)+buf[13];
}




