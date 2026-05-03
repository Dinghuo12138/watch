#include "myi2c.h"
#include "oled.h"
#include "delay.h"

//软件i2c实现mpu6050

#define myi2c_GPIO_PORT GPIOB
#define myi2c_GPIO_SCL GPIO_PIN_10
#define myi2c_GPIO_SDA GPIO_PIN_11

//控制写入scl电平函数，0：低电平，1：高电平
void myi2c_W_SCL(uint8_t pin_value){
	if(pin_value) HAL_GPIO_WritePin(myi2c_GPIO_PORT,myi2c_GPIO_SCL,GPIO_PIN_SET);
	else HAL_GPIO_WritePin(myi2c_GPIO_PORT,myi2c_GPIO_SCL,GPIO_PIN_RESET);
	delay_us(10);
}

//控制写入sda电平函数，0：低电平，1：高电平
void myi2c_W_SDA(uint8_t pin_value){
	if(pin_value) HAL_GPIO_WritePin(myi2c_GPIO_PORT,myi2c_GPIO_SDA,GPIO_PIN_SET);
	else HAL_GPIO_WritePin(myi2c_GPIO_PORT,myi2c_GPIO_SDA,GPIO_PIN_RESET);
	delay_us(10);
}

//控制读取sda电平函数，返回0：低电平，返回1：高电平
uint8_t myi2c_R_SDA(void){
	uint8_t sda_value=0;
	sda_value=HAL_GPIO_ReadPin(myi2c_GPIO_PORT,myi2c_GPIO_SDA);
	delay_us(10);
	return sda_value;
}

//初始化函数
void MyI2C_Init(void)
{
//	/*开启时钟*/
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//开启GPIOB的时钟
//	
//	/*GPIO初始化*/
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);					//将PB10和PB11引脚初始化为开漏输出
	
	/*设置默认电平*/
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_11,GPIO_PIN_SET);//设置PB10和PB11引脚初始化后默认为高电平（释放总线状态）
}

//基本模块代码

//起始
void myi2c_start(){
	myi2c_W_SDA(1);//先拉高sda，防止误读
	myi2c_W_SCL(1);
	myi2c_W_SDA(0);//拉低sda产生起始信号
	myi2c_W_SCL(0);//scl也拉低，即为了占用总线，也为了方便总线时序的拼接
}

//终止
void myi2c_stop(){
	myi2c_W_SDA(0);//先拉低sda
	myi2c_W_SCL(1);
	myi2c_W_SDA(1);//拉高sda产生终止信号
}

//发送一个字节
void myi2c_sendByte(uint8_t Byte){
	myi2c_W_SCL(0);//先拉低scl
	uint8_t i;
	for(i=0;i<8;i++){
		myi2c_W_SDA(Byte & 0x80>>i);//1000 0000,取出Byte的每一位
		myi2c_W_SCL(1);//释放scl，从机读取sda线上的数据
		myi2c_W_SCL(0);//拉低scl，开始发送下一位
	}	
}

//接收一个字节
uint8_t myi2c_receiveByte(void){
	uint8_t Byte=0x00;
	myi2c_W_SCL(0);//先拉低scl
	myi2c_W_SDA(1);//接受前显示放SDA，避免干扰从机数据发送（因为是弱上拉，如果数据是0，还会被下拉回0）
	uint8_t i;
	for(i=0;i<8;i++){
		myi2c_W_SCL(1);//释放scl
		if(myi2c_R_SDA()==1){//如果数据位=1，该位置1，否则置0（默认=0）
			
			Byte |= 0x80>>i;//按位或等，把每一位写入Byte
			
		}
		myi2c_W_SCL(0);//拉低scl
	}
	return Byte;
}

//发送应答
void myi2c_sendack(uint8_t ack){
	myi2c_W_SDA(ack);//主机把应答位写入SDA线
	myi2c_W_SCL(1);
	myi2c_W_SCL(0);
}

//接收应答
uint8_t myi2c_receiveack(){
	uint8_t ack;
	myi2c_W_SDA(1);//接受前显示放SDA，避免干扰从机数据发送
	myi2c_W_SCL(1);
	ack=myi2c_R_SDA();
	myi2c_W_SCL(0);//拉低SCL，开始下一个时序模块
	return ack;
}

//遍历查询从机地址
void myi2c_addr_scan(void){
	uint8_t addr;
	uint8_t ack;
	uint8_t found=0;
	OLED_Clear();
	OLED_ShowString(0,0,"I2C SCAN...",OLED_8X16);
	for(addr=1;addr<128;addr++){
		myi2c_start();
		// 发送写地址：7位地址左移1位，最低位0表示写
		myi2c_sendByte(addr<<1);
		ack = myi2c_receiveack();
		myi2c_stop();
		if(ack==0){
			// 找到有应答的设备
            OLED_ShowString(0, 16 + found * 16, "ADDR:", OLED_8X16);
            OLED_ShowHexNum(40, 16 + found * 16, addr<<1, 2, OLED_8X16);
			
			found++;

            if(found >= 3)
            {
                break;  // OLED 一屏放不下太多，先显示前三个
            }
		}
		HAL_Delay(5);
	}
	if(found == 0)
    {
        OLED_ShowString(0, 16, "No Device", OLED_8X16);
    }

    OLED_Update();
}

