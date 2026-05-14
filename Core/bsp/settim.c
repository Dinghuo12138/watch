#include "settim.h"
#include "oled.h"
#include "font.h"
#include "key.h"
#include "MyRTC.h"
#include <stdint.h>
#include "menu.h"

extern struct keys key[3];


typedef enum        //当前字段
{
    SETTIME_BACK = 0,
    SETTIME_YEAR,
    SETTIME_MONTH,
    SETTIME_DAY,
    SETTIME_HOUR,
    SETTIME_MIN,
    SETTIME_SECOND
} SetTime_Current_t;

typedef enum    //当前模式
{
    SETTIME_Mode_Select = 0,    //选择字段
    SETTIME_Mode_edit           //编辑字段
}SetTime_Mode_t;

SetTime_Current_t settime_current=SETTIME_BACK;    //默认选择年
SetTime_Mode_t settime_mode=SETTIME_Mode_Select;    //默认选择字段

//选择模式处理函数
void settim_select(uint8_t event){
    if(event==KEY_EVENT_KEY0){
        settime_current++;
        if(settime_current>SETTIME_SECOND) settime_current=SETTIME_BACK;
    }
    else if(event==KEY_EVENT_KEY1){
        if(settime_current<=SETTIME_BACK) settime_current=SETTIME_SECOND;
        else settime_current--;
    }
    else if(event==KEY_EVENT_KEY2){
        
        if(settime_current==SETTIME_BACK) page_current=PAGE_SET;   //返回设置
        else settime_mode=SETTIME_Mode_edit;    //进入编辑模式
        OLED_Clear();
    }
}

//编辑模式处理函数
void settim_edit(uint8_t event){
    if(event==KEY_EVENT_KEY0){
        switch(settime_current){
            case SETTIME_YEAR: MyRTC_Time[0]++;break;
            case SETTIME_MONTH: {
                MyRTC_Time[1]++;
                    if (MyRTC_Time[1]>12) {
                            MyRTC_Time[1]=1;
                        }
                } break;
            case SETTIME_DAY: {
                MyRTC_Time[2]++;
                    if (MyRTC_Time[1]==2 && MyRTC_Time[2]>28) {
                            MyRTC_Time[2]=1;
                        }
                    else if ((MyRTC_Time[1]==4 || MyRTC_Time[1]==6 || MyRTC_Time[1]==9 || MyRTC_Time[1]==11) && MyRTC_Time[2]>30) {
                            MyRTC_Time[2]=1;
                        }
                    else if ((MyRTC_Time[1]==1 || MyRTC_Time[1]==3 || MyRTC_Time[1]==5 || MyRTC_Time[1]==7 || MyRTC_Time[1]==8 || MyRTC_Time[1]==10 || MyRTC_Time[1]==12) &&MyRTC_Time[2]>31) {
                            MyRTC_Time[2]=1;
                        }
            } break;
            case SETTIME_HOUR: {MyRTC_Time[3]++;
                        if (MyRTC_Time[3]>23) {
                            MyRTC_Time[3]=0;
                        }
                } break;
            case SETTIME_MIN:{MyRTC_Time[4]++;
                        if (MyRTC_Time[4]>59) {
                            MyRTC_Time[4]=0;
                        }
            } break;
            case SETTIME_SECOND: {MyRTC_Time[5]++;
                        if (MyRTC_Time[5]>59) {
                            MyRTC_Time[5]=0;
                        }
            };break;
        }
        MyRTC_SetTime();
    }
    else if(event==KEY_EVENT_KEY1){
        switch(settime_current){
            case SETTIME_YEAR: {MyRTC_Time[0]--;
                        if (MyRTC_Time[0]<1000) {
                            MyRTC_Time[0]=2099;
                        }
                } break;
            case SETTIME_MONTH: {MyRTC_Time[1]--;
                        if (MyRTC_Time[1]<1) {
                            MyRTC_Time[1]=12;
                        }
                } break;
            case SETTIME_DAY: {MyRTC_Time[2]--;
                        if (MyRTC_Time[2]<1) {
                            MyRTC_Time[2]=30;
                        }
                } break;
            case SETTIME_HOUR: {
                        if (MyRTC_Time[3]==0) {
                            MyRTC_Time[3]=23;   
                        }
                        else MyRTC_Time[3]--;
                } break;
            case SETTIME_MIN: {
                        if (MyRTC_Time[4]==0) {
                            MyRTC_Time[4]=59;
                        }
                        else MyRTC_Time[4]--;
                } break;
            case SETTIME_SECOND: {
                        if (MyRTC_Time[5]==0) {
                            MyRTC_Time[5]=59;
                        }
                        else MyRTC_Time[5]--;
                } break;
        }
        MyRTC_SetTime();
    }
    else if(event==KEY_EVENT_KEY2){
        settime_mode=SETTIME_Mode_Select;  //返回选择模式
    }
}



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

void settim_draw(){

        if(settime_current <= SETTIME_DAY)
        {
            settim_first_page();
        }
        else
        {
            settim_second_page();
        } 

        if (settime_mode==SETTIME_Mode_Select) {
            switch(settime_current){
            case SETTIME_BACK: OLED_ReverseArea(0, 0, 16, 16);break;
            case SETTIME_YEAR: OLED_ReverseArea(0, 16, 16, 16);break;
            case SETTIME_MONTH: OLED_ReverseArea(0, 32, 16, 16);break;
            case SETTIME_DAY: OLED_ReverseArea(0, 48, 16, 16);break;
            case SETTIME_HOUR: OLED_ReverseArea(0, 0, 16, 16);break;
            case SETTIME_MIN: OLED_ReverseArea(0, 16, 16, 16);break;
            case SETTIME_SECOND: OLED_ReverseArea(0, 32, 16, 16);break;
            }
        }
        else if (settime_mode==SETTIME_Mode_edit) {
            switch(settime_current){
            case SETTIME_YEAR: OLED_ReverseArea(24, 16, 32, 16);break;
            case SETTIME_MONTH: OLED_ReverseArea(24, 32, 16, 16);break;
            case SETTIME_DAY: OLED_ReverseArea(24, 48, 16, 16);break;
            case SETTIME_HOUR: OLED_ReverseArea(24, 0, 16, 16);break;
            case SETTIME_MIN: OLED_ReverseArea(24, 16, 16, 16);break;
            case SETTIME_SECOND: OLED_ReverseArea(24, 32, 16, 16);break;
            }
        }
    
    OLED_Update();
}

void settim_UI_Task(){
    OLED_Clear();
    uint8_t event=Key_GetEvent();
    if(settime_mode==SETTIME_Mode_Select){
        settim_select(event);
    }
    else if(settime_mode==SETTIME_Mode_edit){
        settim_edit(event);
    }
    settim_draw();
}

