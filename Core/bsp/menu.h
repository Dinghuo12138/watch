#ifndef _HAL_MENU_
#define _HAL_MENU_

#include "main.h"
#include "oled.h"
#include "font.h"
void menu_init(void);
void show_clock_UI(void);
uint8_t home_page(void);
uint8_t set_page(void);
void set_view_UI(void);
uint8_t menu_ui(void);
uint8_t watch_timer(void);
#endif
