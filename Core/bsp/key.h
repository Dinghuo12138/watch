#ifndef _HAL_KEY_
#define _HAL_KEY_

#include "main.h"

//°´¼üÊÂ¼þ
#define KEY_EVENT_NONE     0
#define KEY_EVENT_KEY0     1
#define KEY_EVENT_KEY1     2
#define KEY_EVENT_KEY2     3

struct keys{
	volatile uint8_t single_flag;
	volatile uint8_t judge_state;
	volatile uint8_t key_state;
	volatile uint16_t key_tim;
	volatile uint8_t long_flag;
};


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
uint8_t Key_GetEvent(void);
void Key_Task(void);
#endif
