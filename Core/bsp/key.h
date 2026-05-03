#ifndef _HAL_KEY_
#define _HAL_KEY_

#include "main.h"

struct keys{
	char single_flag;
	char judge_state;
	char key_state;
	uint16_t key_tim;
	char long_flag;
};
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
#endif
