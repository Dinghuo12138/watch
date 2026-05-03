#include "settim.h"
#include "oled.h"
#include "font.h"
#include "key.h"
#include "MyRTC.h"

extern struct keys key[3];

void settim_first_page()
{
    OLED_ShowImage(0, 0, 16, 16, Return);
    OLED_Printf(0, 16, OLED_8X16, "年:%04d", MyRTC_Time[0]);
    OLED_Printf(0, 32, OLED_8X16, "月:%02d", MyRTC_Time[1]);
    OLED_Printf(0, 48, OLED_8X16, "日:%02d", MyRTC_Time[2]);
}
void settim_second_page()
{
    OLED_Printf(0, 0, OLED_8X16, "时:%02d", MyRTC_Time[3]);
    OLED_Printf(0, 16, OLED_8X16, "分:%02d", MyRTC_Time[4]);
    OLED_Printf(0, 32, OLED_8X16, "秒:%02d", MyRTC_Time[5]);
}

//通过改变RTC_TIME[]数组中的值来改变首页显示时钟的时间
void change_RTC_time(uint8_t i,uint8_t flag){//i:MyRTC_Time[i];flag=1:加，flag=0：减
	if(flag==1) MyRTC_Time[i]++;
	else MyRTC_Time[i]--;
	MyRTC_SetTime();//设置时间，调用此函数，全局数组里时间值刷新到RTC硬件电路
}

/********************年月日时分秒按键加减确认********************/
uint8_t setYear(){
	while(1){
	if (key[0].single_flag == 1)
        {
            change_RTC_time(0,1);
            key[0].single_flag = 0;
        }
        else if (key[1].single_flag == 1)
        {
            change_RTC_time(0,0);
            key[1].single_flag = 0;
        }
        else if (key[2].single_flag == 1)
        {
			key[2].single_flag = 0;
			return 0;
            
        }
		settim_first_page();
		OLED_ReverseArea(24,16,32,16);
		OLED_Update();
	}
}
uint8_t setMonth(){
	while(1){
	if (key[0].single_flag == 1)
        {
            change_RTC_time(1,1);
			if(MyRTC_Time[1]>=13) {MyRTC_Time[1]=1;}
            key[0].single_flag = 0;
        }
        else if (key[1].single_flag == 1)
        {
            change_RTC_time(1,0);
			if(MyRTC_Time[1]<1) {MyRTC_Time[1]=12;}
            key[1].single_flag = 0;
        }
        else if (key[2].single_flag == 1)
        {
			key[2].single_flag = 0;
			return 0;
            
        }
		settim_first_page();
		OLED_ReverseArea(24,32,16,16);
		OLED_Update();
	}
}
uint8_t setDay(){
	while(1){
	if (key[0].single_flag == 1)
        {
            change_RTC_time(2,1);
			if(MyRTC_Time[2]>=30) {MyRTC_Time[2]=1;}
            key[0].single_flag = 0;
        }
        else if (key[1].single_flag == 1)
        {
            change_RTC_time(2,0);
			if(MyRTC_Time[2]<=0) {MyRTC_Time[2]=30;}
            key[1].single_flag = 0;
        }
        else if (key[2].single_flag == 1)
        {
			key[2].single_flag = 0;
			return 0;
            
        }
		settim_first_page();
		OLED_ReverseArea(24,48,16,16);
		OLED_Update();
	}
}
uint8_t setHour(){
	while(1){
	if (key[0].single_flag == 1)
        {
            change_RTC_time(3,1);
			if(MyRTC_Time[3]>=24) {MyRTC_Time[3]=0;}
            key[0].single_flag = 0;
        }
        else if (key[1].single_flag == 1)
        {
			if(MyRTC_Time[3]<=0) {MyRTC_Time[3]=23;}
			else change_RTC_time(3,0);
            key[1].single_flag = 0;
        }
        else if (key[2].single_flag == 1)
        {
			key[2].single_flag = 0;
			return 0;
            
        }
		settim_second_page();
		OLED_ReverseArea(24,0,16,16);
		OLED_Update();
	}
}
uint8_t setMin(){
	while(1){
	if (key[0].single_flag == 1)
        {
            change_RTC_time(4,1);
			if(MyRTC_Time[4]>=60) {MyRTC_Time[4]=0;}
            key[0].single_flag = 0;
        }
        else if (key[1].single_flag == 1)
        {
            
			if(MyRTC_Time[4]<=0) {MyRTC_Time[4]=59;}
			else change_RTC_time(4,0);
            key[1].single_flag = 0;
        }
        else if (key[2].single_flag == 1)
        {
			key[2].single_flag = 0;
			return 0;
            

        }
		settim_second_page();
		OLED_ReverseArea(24,16,16,16);
		OLED_Update();
	}
}
uint8_t setSecond(){
	while(1){
	if (key[0].single_flag == 1)
        {
            change_RTC_time(5,1);
			if(MyRTC_Time[5]>=60) {MyRTC_Time[5]=0;}
            key[0].single_flag = 0;
        }
        else if (key[1].single_flag == 1)
        {
            
			if(MyRTC_Time[5]<=0) {MyRTC_Time[5]=59;}
			else change_RTC_time(5,0);
            key[1].single_flag = 0;
        }
        else if (key[2].single_flag == 1)
        {
			key[2].single_flag = 0;
			return 0;
            
        }
		settim_second_page();
		OLED_ReverseArea(24,32,16,16);
		OLED_Update();
	}
}
/********************年月日时分秒按键加减确认********************/








uint8_t settim_flag = 1;
uint8_t settim_view()
{
    while (1)
    {
		uint8_t settim_flag_temp=0;
        if (key[0].single_flag == 1)
        {
            settim_flag++;
            if (settim_flag >= 8)
                settim_flag = 1;
            key[0].single_flag = 0;
        }
        if (key[1].single_flag == 1)
        {
            settim_flag--;
            if (settim_flag < 1)
                settim_flag = 7;
            key[1].single_flag = 0;
        }
        if (key[2].single_flag == 1)
        {
            OLED_Clear();
            OLED_Update();
			settim_flag_temp=settim_flag;
            key[2].single_flag = 0;
        }
		
		if(settim_flag_temp==1){return 0;}
		else if(settim_flag_temp==2) setYear();
		else if(settim_flag_temp==3) setMonth();
		else if(settim_flag_temp==4) setDay();
		else if(settim_flag_temp==5) setHour();
		else if(settim_flag_temp==6) setMin();
		else if(settim_flag_temp==7) setSecond();
			
		
        switch (settim_flag)
        {
        case 1:
        {	
			OLED_Clear();
            settim_first_page();
            OLED_ReverseArea(0, 0, 16, 16);
            OLED_Update();
            break;
        }
        case 2:
        {
			OLED_Clear();
            settim_first_page();
            OLED_ReverseArea(0, 16, 16, 16);
            OLED_Update();
            break;
        }
        case 3:
        {
			OLED_Clear();
            settim_first_page();
            OLED_ReverseArea(0, 32, 16, 16);
            OLED_Update();
            break;
        }
        case 4:
        {
			OLED_Clear();
            settim_first_page();
            OLED_ReverseArea(0, 48, 16, 16);
            OLED_Update();
            break;
        }
        case 5:
        {
			OLED_Clear();
            settim_second_page();
            OLED_ReverseArea(0, 0, 16, 16);
            OLED_Update();
            break;
        }
        case 6:
        {
			OLED_Clear();
            settim_second_page();
            OLED_ReverseArea(0, 16, 16, 16);
            OLED_Update();
            break;
        }
        case 7:
        {
			OLED_Clear();
            settim_second_page();
            OLED_ReverseArea(0, 32, 16, 16);
            OLED_Update();
            break;
        }
        }
    }
}


