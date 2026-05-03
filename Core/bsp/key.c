#include "key.h"
#include "menu.h"
struct keys key[3]={0,0,0,0,0};
uint16_t run_tim[3];
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



