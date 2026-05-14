#include "key.h"
#include "menu.h"

struct keys key[3]={0,0,0,0,0};

uint16_t run_tim[3];
volatile uint16_t count_mpu6050=0;//mpu6050计数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM2){
		key[0].key_state=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1);
		key[1].key_state=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6);
		key[2].key_state=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4);
		uint8_t i=0;
		for(i=0;i<3;i++){
			switch (key[i].judge_state)
			{
				case 0:
				{
					if(key[i].key_state==0){
						key[i].judge_state=1;
					}
				}
				break;
				case 1:
				{
					if(key[i].key_state==0){
						key[i].judge_state=2;
					}
					else{
						key[i].judge_state=0;
					}
				}
				break;
				case 2:
				{
					if(key[i].key_state==1){
						key[i].judge_state=0;
						if(key[i].key_tim<200){
						key[i].single_flag=1;
						}
						key[i].key_tim=0;
						key[i].long_flag=0;
						}
					else{
						key[i].key_tim++;
						if(key[i].key_tim>200){
							key[i].long_flag=1;
							key[i].single_flag=0;
						}
					}
				}
				break;
			}
		}
		watch_timer();
	}
}

//按键任务函数
volatile uint8_t key_event=0;
void Key_Task(void)
{
    if(key[0].single_flag)
    {
        key_event = KEY_EVENT_KEY0;
        key[0].single_flag = 0;
    }
    else if(key[1].single_flag)
    {
        key_event = KEY_EVENT_KEY1;
        key[1].single_flag = 0;
    }
    else if(key[2].single_flag)
    {
        key_event = KEY_EVENT_KEY2;
        key[2].single_flag = 0;
    }
}

//页面调用的按键事件函数
uint8_t Key_GetEvent(void)
{
    uint8_t event = key_event;
    key_event = KEY_EVENT_NONE;
    return event;
}

