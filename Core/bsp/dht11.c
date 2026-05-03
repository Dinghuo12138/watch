	#include "GPIO.h"
	#include "delay.h"
	#include "DHT11.h"
	void DHT11_Rst(void) // 起始信号函数
	{
		DHT11_Mode(out);  // 设置推挽输出模式
		DHT11_LOW;        // 引脚输出低电平
		delay_ms(20);     // Tbe: 20ms (18-30ms 范围)
		DHT11_HIGH;       // 引脚输出高电平
		delay_us(13);     // Tgo: 13us (10-35us 范围)
		DHT11_Mode(in);   // 释放总线，切换为输入
	}
	void DHT11_Mode(uint8_t mode) // 设置输出模式函数 
	{
		GPIO_InitTypeDef GPIO_InitStruct; // GPIO 结构体
		if(mode == out) // 设置推挽输出模式
		{
			HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);
			GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		}
		else // 设置输入模式
		{
			HAL_GPIO_WritePin(DHT11_GPIO_PORT, DHT11_GPIO_PIN, GPIO_PIN_SET);
			GPIO_InitStruct.Pin = DHT11_GPIO_PIN;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLUP; // 输入需配置为上拉模式
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		}
		HAL_GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitStruct); // 初始化 GPIO
	}
	uint8_t DHT11_Check(void) // 反馈检测函数 - 等待 DHT11 响应信号
	{
		uint8_t retry = 0;
		DHT11_Mode(in);
		// 步骤 1: 等待 DHT11 拉低 (Trel: 响应低电平 80us)
		while(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
		{
			retry++;
			delay_us(1);
		}
		if(retry >= 100) return 1;
		// 步骤 2: 等待 DHT11 拉高 (Treh: 响应高电平 87us)
		retry = 0;
		while(!HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
		{
			retry++;
			delay_us(1);
		}
		if(retry >= 100) return 1;
		// 步骤 3: 等待 DHT11 再次拉低 (响应结束，准备发送数据)
		retry = 0;
		while(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
		{
			retry++;
			delay_us(1);
		}
		if(retry >= 100) return 1;
		return 0; // 检测成功
	}
	uint8_t DHT11_Read_Bit(void) // 优化位读取时序逻辑
	{
		uint8_t retry = 0;
		uint8_t bit = 0;
		// 步骤 1: 等待 50us 低电平结束 (此时从Check或上一位结束，引脚应处于低电平)
		retry = 0;
		while(!HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
		{
			retry++;
			delay_us(1);
		}
		// 步骤 2: 延时 40us 后采样 (TH0=24us 为"0", TH1=71us 为"1")
		delay_us(40);
		if(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN))
			bit = 1;
		else
			bit = 0;
		// 步骤 3: 等待高电平结束 (如果当前位是1，需等待其高电平结束；如果是0，引脚已是低电平，瞬间跳出)
		retry = 0;
		while(HAL_GPIO_ReadPin(DHT11_GPIO_PORT, DHT11_GPIO_PIN) && retry < 100)
		{
			retry++;
			delay_us(1);
		}
		return bit;
	}
	uint8_t DHT11_Read_Byte(void) // 组合函数
	{
		uint8_t data = 0;
		for(uint8_t i = 0; i < 8; i++)
		{
			data <<= 1;
			data |= DHT11_Read_Bit();
		}
		return data;
	}
	// 支持浮点数与负温度解析，严格遵循2022版说明书5.2节
	uint8_t DHT11_Read_Data(float *temp, float *humi) 
	{
		uint8_t buf[5];
		DHT11_Rst();
		if(DHT11_Check() == 0)
		{
			for(uint8_t i = 0; i < 5; i++)
			{
				buf[i] = DHT11_Read_Byte();
			}
			if((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
			{
				// 湿度解析：整数 + 小数/10
				*humi = (float)buf[0] + (float)buf[1] / 10.0f;
				// 温度解析：判断符号位
				uint8_t temp_int = buf[2];
				uint8_t temp_dec = buf[3];
				if(temp_dec & 0x80) // 检查Bit7符号位，1表示负温度
				{
					*temp = 0 - ((float)temp_int + (float)(temp_dec & 0x7F) / 100.0f);
				}
				else
				{
					*temp = (float)temp_int + (float)temp_dec / 100.0f;
				}
				return 0; // 成功
			}
			else
			{
				return 2; // 校验和错误
			}
		}
		else
		{
			return 1; // DHT11 无响应
		}
	}
	uint8_t DHT11_Init(void)
	{
		DHT11_Rst();
		return DHT11_Check();
	}
	
	