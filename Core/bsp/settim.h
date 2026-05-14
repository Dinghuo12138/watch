#ifndef _HAL_SETTIM_
#define _HAL_SETTIM_

#include "main.h"

//选择模式处理函数
void settim_select(uint8_t event);

//编辑模式处理函数
void settim_edit(uint8_t event);
 
void settim_first_page(void);

void settim_second_page(void);

void settim_UI_Task(void);


#endif
