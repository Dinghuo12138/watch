#include "myrtc.h"
#include "oled.h"
#include "font.h"
#include "rtc.h"
#include "key.h"
#include "settim.h"
#include "led.h"
#include "mpu6050.h"

uint8_t key_num = 0;
uint8_t select_flag = 1; // 首页选项，1菜单，2设置
extern struct keys key[3];

//函数声明
uint8_t watch_time_page(void);
uint8_t light_ui_select(void);
uint8_t mpu6500_pages(void);

void menu_init()
{
	MyRTC_Init();
}

/*************************首页首页时钟****************************/
void show_clock_UI(void)
{
	MyRTC_ReadTime();
	OLED_Printf(0, 0, OLED_6X8, "%d-%d-%d", MyRTC_Time[0], MyRTC_Time[1], MyRTC_Time[2]);
	OLED_Printf(16, 16, OLED_12X24, "%02d:%02d:%02d", MyRTC_Time[3], MyRTC_Time[4], MyRTC_Time[5]);
	OLED_ShowString(0, 48, "菜单", OLED_8X16);
	OLED_ShowString(96, 48, "设置", OLED_8X16);
}
/*************************首页按键操作****************************/
uint8_t home_page(void)
{
	while (1)
	{
		if (key[0].single_flag == 1)
		{
			select_flag++;
			if (select_flag >= 3)
				select_flag = 1;
			key[0].single_flag = 0;
		}
		if (key[1].single_flag == 1)
		{
			select_flag--;
			if (select_flag < 1)
				select_flag = 2;
			key[1].single_flag = 0;
		}
		if (key[2].single_flag == 1)
		{
			OLED_Clear();
			OLED_Update();
			key[2].single_flag = 0;
			return select_flag;
		}
		switch (select_flag)
		{
		case 1:
		{
			show_clock_UI();
			OLED_ReverseArea(0, 48, 32, 16);
			OLED_Update();
			break;
		}
		case 2:
		{
			show_clock_UI();
			OLED_ReverseArea(96, 48, 32, 16);
			OLED_Update();
			break;
		}
		}
	}
}

/*********************************设置页面********************************/
void set_view_UI(void)
{
	OLED_ShowImage(0, 0, 16, 16, Return); // 显示返回键
	OLED_ShowString(0, 16, "设置时间日期", OLED_8X16);
}
uint8_t set_flag = 1;
uint8_t set_page()
{
	while (1)
	{
		uint8_t set_flag_temp=0;
		if (key[0].single_flag == 1)
		{
			set_flag++;
			if (set_flag >= 3)
				set_flag = 1;
			key[0].single_flag = 0;
		}
		if (key[1].single_flag == 1)
		{
			set_flag--;
			if (set_flag < 1)
				set_flag = 2;
			key[1].single_flag = 0;
		}
		if (key[2].single_flag == 1)
		{
			OLED_Clear();
			OLED_Update();
			set_flag_temp=set_flag;
			key[2].single_flag = 0;
		}
		if(set_flag_temp==1) return 0;
		else if(set_flag_temp==2) settim_view();
		switch (set_flag)
		{
		case 1:
		{
			set_view_UI();
			OLED_ReverseArea(0, 0, 16, 16);
			OLED_Update();
			break;
		}
		case 2:
		{
			set_view_UI();
			OLED_ReverseArea(0, 16, 96, 16);
			OLED_Update();
			break;
		}
		}
	}
}

/***************************菜单页面*****************************/
uint8_t now_selection=0;//本次选项
uint8_t new_selection=0;//下次选项
uint8_t speed_move=4;//移动速度
uint8_t x_now=48;//当前选项的x坐标
uint8_t move_flag=0;//1：开始移动，0：停止移动

//图像移动函数
void menuui_move(void){
	OLED_Clear();
	OLED_ShowImage(42,10,44,44,Frame);
	if(now_selection<new_selection){//选择下一项
		x_now-=speed_move;
		if(x_now==0){
			now_selection++;
			move_flag=0;
			x_now=48;
		}
	}
	if(now_selection>new_selection){//选择上一项
		x_now+=speed_move;
		if(x_now==96){
			now_selection--;
			move_flag=0;
			x_now=48;
		}
	}
	OLED_ShowImage(x_now,16,32,32,Menu_Graph[now_selection]);//当前选择
	if(now_selection>=1){
	OLED_ShowImage(x_now-48,16,32,32,Menu_Graph[now_selection-1]);//前一个
	}
	if(now_selection>=2){
	OLED_ShowImage(x_now-96,16,32,32,Menu_Graph[now_selection-2]);//前两个
	}
	OLED_ShowImage(x_now+48,16,32,32,Menu_Graph[now_selection+1]);//后一个
	OLED_ShowImage(x_now+96,16,32,32,Menu_Graph[now_selection+2]);//后两个
	OLED_Update();
}

//方向选择函数
void direction_move(uint8_t move_flag1,uint8_t NOW_selection,uint8_t NEW_selection){
	if(move_flag1==1){
		now_selection=NOW_selection;
		new_selection=NEW_selection;
	}
	menuui_move();
}



uint8_t menu_flag=1;//菜单图标
//菜单页面显示ui函数
uint8_t menu_ui(){
	uint8_t direction_flag=1;//1：选择下一项，2：选择上一项
	while (1)
    {
		uint8_t menu_flag_temp=0;
        if (key[0].single_flag == 1)
        {
			move_flag=1;
            menu_flag++;
			direction_flag=1;
            if (menu_flag >= 8)
                menu_flag = 1;
            key[0].single_flag = 0;
        }
        if (key[1].single_flag == 1)
        {
			move_flag=1;
            menu_flag--;
			direction_flag=2;
            if (menu_flag < 1)
                menu_flag = 7;
            key[1].single_flag = 0;
        }
        if (key[2].single_flag == 1)
        {
            OLED_Clear();
            OLED_Update();
			menu_flag_temp=menu_flag;
            key[2].single_flag = 0;
        }
		
		if(menu_flag_temp==1){return 0;}
		else if(menu_flag_temp==2) watch_time_page();//跳转到秒表页面
		else if(menu_flag_temp==3) light_ui_select();//跳转手电筒页面
		else if(menu_flag_temp==4) mpu6500_pages();//跳转到陀螺仪页面
//		else if(menu_flag_temp==5) 
//		else if(menu_flag_temp==6) 
//		else if(menu_flag_temp==7) 
			
		if(menu_flag==1){
			if(direction_flag==2)direction_move(move_flag,1,0);
			else if(direction_flag==1)direction_move(move_flag,0,0);
		}
		else{
			if(direction_flag==2)direction_move(move_flag,menu_flag,menu_flag-1);
			else if(direction_flag==1)direction_move(move_flag,menu_flag-2,menu_flag-1);
		}
	}
}

/*********************************秒表*****************************************/

uint8_t second=0,min=0,hour=0;
void watch_timer_ui(){
	OLED_ShowImage(0, 0, 16, 16, Return); // 显示返回键
	OLED_Printf(32,20,OLED_8X16,"%02d:%02d:%02d",hour,min,second);
	OLED_ShowString(8,48,"开始",OLED_8X16);
	OLED_ShowString(48,48,"停止",OLED_8X16);
	OLED_ShowString(88,48,"清除",OLED_8X16);
}

uint8_t watch_timer_flag=0;//计时标志位,1开始，0停止

//计时函数
uint16_t count=0;
void watch_timer(){
	count++;
	if(count>=100)
	{	count=0;
		if(watch_timer_flag==1){
			second++;
			if(second>=60){
				second=0;
				min++;
				if(min>=60){
					min=0;
					hour++;
					if(hour>=99){
						hour=0;
					}
				}
			}
		}
		
	}
}

uint8_t time_start=0;//开始计时标志位,1：开始，0：停止
uint8_t reset_flag=0;//清零标志位，1：清零，0：没有
uint8_t watch_time_page(){
	while (1)
	{
		if (key[0].single_flag == 1)//计时/暂停
		{
			reset_flag=0;
			time_start++;
			if(time_start>1){
				time_start=0;
			}
			key[0].single_flag = 0;
		}
		if (key[1].single_flag == 1)//复位
		{
			time_start=0;
			hour=min=second=0;
			reset_flag=1;
			key[1].single_flag = 0;
		}
		if (key[2].single_flag == 1)
		{
			OLED_Clear();
			OLED_Update();
			key[2].single_flag = 0;
			return 0;
		}
		switch (time_start)
		{
		case 1:
		{
			watch_timer_ui();
			watch_timer_flag=1;
			OLED_ReverseArea(8,48,32,16);
			OLED_Update();
			break;
		}
		case 0:
		{
			watch_timer_ui();
			watch_timer_flag=0;
			if(!reset_flag) OLED_ReverseArea(48,48,32,16);
			else OLED_ReverseArea(88,48,32,16);
			OLED_Update();
			break;
		}
		}
	}
}

/***********************************手电筒***********************************/
void light_ui()
{
	OLED_Clear();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,20,"开关灯",OLED_8X16);
	OLED_ShowString(0,40,"呼吸灯",OLED_8X16);
}


uint8_t light_ui_select(){
	uint8_t light_select_temp=1;
	uint8_t light_select=1;//灯光选择界面
	uint8_t led_flag=0;//控制led状态
	while (1)
	{
		if (key[0].single_flag == 1)//计时/暂停
		{
			light_select++;
			if(light_select>2){
				light_select=0;
			}
			key[0].single_flag = 0;
		}
		if (key[1].single_flag == 1)//复位
		{
			if(light_select<=0){
				light_select=2;
			}
			else light_select--;
			key[1].single_flag = 0;
		}
		if (key[2].single_flag == 1)
		{
			led_flag=!led_flag;
			OLED_Clear();
			OLED_Update();
			light_select_temp=light_select;
			key[2].single_flag = 0;
		}
		
		if(light_select_temp==0) {return 0;}
		else if(light_select_temp==1&&led_flag==0) {led_control(1);}
		else if(light_select_temp==1&&led_flag==1) {led_control(0);}
		else if(light_select_temp==2) led_control(2);
			
		switch (light_select)
		{
		case 0:
		{
			light_ui();
			OLED_ReverseArea(0,0,16,16);
			OLED_Update();
			break;
		}
		case 1:
		{
			light_ui();
			OLED_ReverseArea(0,20,48,16);
			OLED_Update();
			break;
		}
		case 2:
		{
			light_ui();
			OLED_ReverseArea(0,40,48,16);
			OLED_Update();
			break;
		}
		}
	}
}

/***********************************MPU6500***********************************/
//陀螺仪页面显示
void mpu6500_ui(){
	
	struct mpu6050_OUT mpu6050_out;
	
	OLED_Clear();
	mpu6050_init();
	uint8_t ID=getID();
	OLED_Printf(0,0,OLED_8X16,"ID:%b",ID);//显示ID
	mpu6050_GetValue(&mpu6050_out);
	OLED_ShowSignedNum(48,0,mpu6050_out.temp/ 333.87f + 21.0f,2,OLED_8X16);//16 位有符号原始 ADC 值 转换成温度
	//检测量程
//	uint8_t acc_cfg;
//	acc_cfg = MPU6050_ReadReg(0x1C);

//	OLED_ShowHexNum(48, 0, acc_cfg, 2, OLED_8X16);
	//
	OLED_ShowSignedNum(0,16,mpu6050_out.AccX,5,OLED_8X16);
	OLED_ShowSignedNum(0,32,mpu6050_out.AccY,5,OLED_8X16);
	OLED_ShowSignedNum(0,48,mpu6050_out.AccZ,5,OLED_8X16);
	OLED_ShowSignedNum(55,16,mpu6050_out.GyroX,5,OLED_8X16);
	OLED_ShowSignedNum(55,32,mpu6050_out.GyroY,5,OLED_8X16);
	OLED_ShowSignedNum(55,48,mpu6050_out.GyroZ,5,OLED_8X16);
	OLED_Update();
}

//陀螺仪页面逻辑
uint8_t mpu6500_pages(){
	while (1)
	{
		if (key[0].single_flag == 1)//计时/暂停
		{
			
			key[0].single_flag = 0;
		}
		if (key[1].single_flag == 1)//复位
		{
			
			key[1].single_flag = 0;
		}
		if (key[2].single_flag == 1)
		{
			
			key[2].single_flag = 0;
			return 0;
		}
		mpu6500_ui();
	}
}








