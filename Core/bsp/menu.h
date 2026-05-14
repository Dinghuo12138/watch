#ifndef _HAL_MENU_
#define _HAL_MENU_

#include "main.h"
#include "oled.h"
#include "font.h"
void menu_init(void);
void show_clock_UI(void);
void home_page(void);
void set_page(void);
void set_view_UI(void);
void menu_ui(void);
void watch_timer(void);
void UI_Task(void);
void Led_Task(void);
void Sensor_Task(void);

typedef enum    //页面枚举 
{
	PAGE_HOME,
    PAGE_MENU,
    PAGE_SET,
    PAGE_SETTIME,
    PAGE_STOPWATCH,
    PAGE_LIGHT,
    PAGE_MPU6050,
    PAGE_TUOLUOYI,
    PAGE_STATE,
    PAGE_MAX30102
}pages_t;

extern pages_t page_current;    //当前页面

#endif
