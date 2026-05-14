#include "mpu6050.h"
#include "myi2c.h"
#include "i2c.h"
//寄存器地址定义
#include "mpu6050reg.h"
#include "max30102_reg.h"
#include <stdint.h>

#define MPU6050_ADDRESS 0xD0
#define MAX30102_ADDRESS 0xAE // I2C地址 (0x57<<1)


//指定寄存器写
void Sensor_WriteReg(uint16_t Sensor_add,uint8_t reg,uint8_t data){
	// myI2C_Start();
	// myI2C_SendByte(MPU6050_ADDRESS);
	// myI2C_ReceiveAck();
	// myI2C_SendByte(reg);
	// myI2C_ReceiveAck();
	// myI2C_SendByte(data);
	// myI2C_ReceiveAck();
	// myI2C_Stop();
	HAL_I2C_Mem_Write(&hi2c2, Sensor_add, reg, 1, &data, 1, 100);
}

//指定寄存器读

uint8_t Sensor_ReadReg(uint16_t Sensor_add,uint8_t reg){
	
	uint8_t data_read;
	
	// myI2C_Start();
	// myI2C_SendByte(MPU6050_ADDRESS);
	// myI2C_ReceiveAck();
	// myI2C_SendByte(reg);
	// myI2C_ReceiveAck();
	
	// myI2C_Start();
	// myI2C_SendByte(MPU6050_ADDRESS|0x01); 	//读写位置1，读
	// myI2C_ReceiveAck();
	// data_read=myI2C_ReceiveByte(); 				//接收指定寄存器的数据
	// myI2C_SendAck(1);						//主机给从机发送非应答，停止读

	// myI2C_Stop();
	HAL_I2C_Mem_Read(&hi2c2, Sensor_add, reg, 1, &data_read, 1, 100);
	return data_read;
}



/******************************************mpu6050*****************************************/
//mpu6050初始化
void mpu6050_init(){
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	Sensor_WriteReg(MPU6050_ADDRESS,MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为系统时钟源
	Sensor_WriteReg(MPU6050_ADDRESS,MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	Sensor_WriteReg(MPU6050_ADDRESS,MPU6050_SMPLRT_DIV, 0x04);		//采样率分频寄存器，配置采样率
	//----SMPLRT_DIV=陀螺仪输出频率/采样频率-1，采样频率=200，即delta_t=0.05s.
	Sensor_WriteReg(MPU6050_ADDRESS,MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF,陀螺仪输出频率降为1000Hz
	Sensor_WriteReg(MPU6050_ADDRESS,MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	Sensor_WriteReg(MPU6050_ADDRESS,MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}
//获取mpu6050寄存器ID
uint8_t mpu6050_getID(void){
	return Sensor_ReadReg(MPU6050_ADDRESS,MPU6050_WHO_AM_I);
}


//获取寄存器中的数据
void mpu6050_GetValue(struct mpu6050_OUT *data){
	uint8_t i;
	uint8_t buf[14];
	// for(i=0;i<14;i++){
	// 	buf[i]=Sensor_ReadReg(MPU6050_ADDRESS,MPU6050_ACCEL_XOUT_H+i);//循环接收数据0x3B->0x48
	// }
	HAL_I2C_Mem_Read(&hi2c2,MPU6050_ADDRESS,MPU6050_ACCEL_XOUT_H,I2C_MEMADD_SIZE_8BIT,buf,14,100);
	data->AccX=(int16_t)((uint16_t)buf[0]<<8|buf[1]);
	data->AccY=(int16_t)((uint16_t)buf[2]<<8|buf[3]);
	data->AccZ=(int16_t)((uint16_t)buf[4]<<8|buf[5]);
	data->temp=(int16_t)((uint16_t)buf[6]<<8|buf[7]);
	data->GyroX=(int16_t)((uint16_t)buf[8]<<8|buf[9]);
	data->GyroY=(int16_t)((uint16_t)buf[10]<<8|buf[11]);
	data->GyroZ=(int16_t)((uint16_t)buf[12]<<8|buf[13]);
}




/******************************************MAX30102*****************************************/

/*修改*/ //MAX30102初始化：复位后等待复位位清零，再配置FIFO、采样率、LED电流，最后进入SpO2模式
void max30102_init(void)
{
	uint16_t timeout = 100;

	/*修改*/ //先复位芯片，复位位会由芯片自动清零
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_MODE_CONFIG, 0x40);
	while ((Sensor_ReadReg(MAX30102_ADDRESS, MAX30102_REG_MODE_CONFIG) & 0x40) && timeout)
	{
		HAL_Delay(1);
		timeout--;
	}

	/*修改*/ //读取中断状态寄存器可以清除旧中断标志
	Sensor_ReadReg(MAX30102_ADDRESS, 0x00);
	Sensor_ReadReg(MAX30102_ADDRESS, 0x01);

	/*修改*/ //开始新一轮测量前清空FIFO指针，避免读到旧数据
	max30102_clear_fifo();

	/*修改*/ //FIFO配置：采样平均+不回卷+almost-full阈值
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_FIFO_CONFIG, 0x4F);

	/*修改*/ //SpO2配置：100sps、18bit脉宽，先用这个配置验证数据链路
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_SPO2_CONFIG, 0x27);

	/*修改*/ //LED电流，太小无波形，太大容易饱和；0x24适合作为初始值
	/*修改*/ // 红光略低、红外略高，降低R值偏大的概率；如果RED/IR接近饱和再继续下调
	/*修改*/ // R值仍然偏大，继续降低红光、提高红外，让RED/IR交流比例更接近正常范围
	/*修改*/ // 0x7F红外电流过大时可能饱和，先回到更稳的RED=0x1F、IR=0x3F
	/*修改*/ // R值只有0.40到0.50，说明红光比例偏低；提高红光、降低红外，把R拉回0.8到1.2附近
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_RED_PA, 0x3F);
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_IR_PA, 0x1F);

	/*修改*/ //最后进入SpO2模式，开始产生RED/IR FIFO数据
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_MODE_CONFIG, 0x03);
	HAL_Delay(20);
}

/*修改*/ //只清FIFO，不改变工作模式；进入测量页面时调用一次即可
void max30102_clear_fifo(void)
{
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_FIFO_WR_PTR, 0x00);
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_FIFO_OF_CN, 0x00);
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_FIFO_RD_PTR, 0x00);
}

/*修改*/ //退出页面后让MAX30102休眠，避免后台一直采样导致FIFO状态混乱
void max30102_stop(void)
{
	Sensor_WriteReg(MAX30102_ADDRESS, MAX30102_REG_MODE_CONFIG, 0x80);
}

//获取max30102寄存器ID
uint8_t max30102_getID(void){
	return Sensor_ReadReg(MAX30102_ADDRESS, MAX30102_REG_ID);
}


uint32_t ir=0,red=0,num=0;
uint8_t max30102_sample_valid=0;
/*修改*/ uint8_t max30102_status1=0;
/*修改*/ uint8_t max30102_mode_reg=0;
/*修改*/ uint32_t max30102_wr_ptr=0;
/*修改*/ uint32_t max30102_rd_ptr=0;//max30102的红外和红光数据以及数据数量
//获取max30102数据
void max30102_GetValue(void){
	//算出 FIFO 中有多少样本
	/*修改*/ max30102_status1=Sensor_ReadReg(MAX30102_ADDRESS, 0x00);
	/*修改*/ max30102_mode_reg=Sensor_ReadReg(MAX30102_ADDRESS, MAX30102_REG_MODE_CONFIG);
	/*修改*/ max30102_wr_ptr=Sensor_ReadReg(MAX30102_ADDRESS,MAX30102_REG_FIFO_WR_PTR)&0x1F;
	/*修改*/ max30102_rd_ptr=Sensor_ReadReg(MAX30102_ADDRESS,MAX30102_REG_FIFO_RD_PTR)&0x1F;
	num=(max30102_wr_ptr-max30102_rd_ptr)&0x1F;
	max30102_sample_valid=0;

	 if (num == 0)
    {
        return;
    }

	//SPO2模式每个样本6字节,每个通道3字节，连续读num个样本
	uint8_t buf[6];
	HAL_I2C_Mem_Read(&hi2c2, MAX30102_ADDRESS, MAX30102_REG_FIFO_DATA, I2C_MEMADD_SIZE_8BIT, buf, 6, 100);
	//合成18位数据
	red = ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | buf[2];
    red &= 0x03FFFF;

    ir = ((uint32_t)buf[3] << 16) | ((uint32_t)buf[4] << 8) | buf[5];
    ir &= 0x03FFFF;
	max30102_sample_valid=1;
}	

