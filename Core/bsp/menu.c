#include "menu.h"
#include "myrtc.h"
#include "oled.h"
#include "font.h"
#include "rtc.h"
#include "key.h"
#include "settim.h"
#include "led.h"
#include "mpu6050.h"
#include "math.h"
#include "stdlib.h"
#include <stdint.h>

#define Menu_Graph_Num 7  //菜单页面图标数量

uint8_t key_num = 0;
uint8_t select_flag = 0; // 首页选项，1菜单，2设置
extern struct keys key[3];
extern uint32_t ir,red,num;//max30102的红外和红光数据

//sensor_task()函数中的标志位
uint8_t mpu6050_flag=0;//陀螺仪测量标志位，1：测量，0：不测量
uint8_t max30102_flag=0;//心率血氧测量标志位，1：测量，0：不测量

pages_t page_current=PAGE_HOME;    //当前页面

//函数声明
void watch_time_page(void);
void light_ui_select(void);
void mpu6500_pages(void);


//外设传感器初始化
void menu_init()
{
    MyRTC_Init();
    mpu6050_init();
    max30102_init();
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
void home_page(void)
{
	uint8_t event = Key_GetEvent();

		if (event==KEY_EVENT_KEY0)
		{
			select_flag++;
			if (select_flag >= 2)
				select_flag = 0;
		}
		if (event==KEY_EVENT_KEY1)
		{
			
			if (select_flag <= 0) select_flag = 1;
			else select_flag--;
		}
		if (event==KEY_EVENT_KEY2)
		{
			if(select_flag) page_current=PAGE_SET;
			else page_current=PAGE_MENU;
			OLED_Clear();
			return;
		}
		OLED_Clear();
		show_clock_UI();
		if(select_flag) OLED_ReverseArea(96, 48, 32, 16);
		else OLED_ReverseArea(0, 48, 32, 16);

		OLED_Update();
}

/*********************************设置页面********************************/
void set_view_UI(void)
{
	OLED_ShowImage(0, 0, 16, 16, Return); // 显示返回键
	OLED_ShowString(0, 16, "设置时间日期", OLED_8X16);
}
uint8_t set_flag = 0;
void set_page(void)
{
	OLED_Clear();
	set_view_UI();
	uint8_t event = Key_GetEvent();

		uint8_t set_flag_temp=0;
		if (event == KEY_EVENT_KEY0)
		{
			set_flag++;
			if (set_flag >= 2)
				set_flag = 0;
		
		}
		if (event == KEY_EVENT_KEY1)
		{
			
			if (set_flag <= 0)
				set_flag = 1;
			else set_flag--;
		}
		if (event == KEY_EVENT_KEY2)
		{
			OLED_Clear();
			set_flag_temp=set_flag;
			if(set_flag_temp==0) page_current=PAGE_HOME;
			else if(set_flag_temp==1) page_current=PAGE_SETTIME;
		}
		
		
		
		switch (set_flag)
		{
		case 0:
		{
			set_view_UI();
			OLED_ReverseArea(0, 0, 16, 16);
			OLED_Update();
			break;
		}
		case 1:
		{
			set_view_UI();
			OLED_ReverseArea(0, 16, 96, 16);
			OLED_Update();
			break;
		}
		}
}

/***************************菜单页面*****************************/
uint8_t now_selection=0;//本次选项
uint8_t new_selection=0;//下次选项
uint8_t speed_move=4;//移动速度
uint8_t x_now=48;//当前选项的x坐标
uint8_t move_flag=0;//1：开始移动，0：停止移动

//循环显示图像的索引函数
uint8_t Menu_Index(int16_t index){
	if(index<0){
		index+=Menu_Graph_Num;
	}
	else if(index>=Menu_Graph_Num){
		index-=Menu_Graph_Num;
	}
	return index;
}

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

	OLED_ShowImage(x_now,16,32,32,Menu_Graph[Menu_Index(now_selection)]);//当前选项
	OLED_ShowImage(x_now-48,16,32,32,Menu_Graph[Menu_Index(now_selection-1)]);//前一个
	OLED_ShowImage(x_now-96,16,32,32,Menu_Graph[Menu_Index(now_selection-2)]);//前两个
	OLED_ShowImage(x_now+48,16,32,32,Menu_Graph[Menu_Index(now_selection+1)]);//后一个
	OLED_ShowImage(x_now+96,16,32,32,Menu_Graph[Menu_Index(now_selection+2)]);//后两个
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
void menu_ui(){
	static uint8_t direction_flag=1;//1：选择下一项，2：选择上一项
	uint8_t event = Key_GetEvent();
        if (event == KEY_EVENT_KEY0)
        {
			move_flag=1;
            menu_flag++;
			direction_flag=1;
            if (menu_flag >= 8)
                menu_flag = 1;
        }
        if (event == KEY_EVENT_KEY1)
        {
			move_flag=1;
            
			direction_flag=2;
            if (menu_flag <= 1) menu_flag = 7;
			else menu_flag--;
        }
        if (event == KEY_EVENT_KEY2)
        {
            if(menu_flag==1){page_current=PAGE_HOME;}
			else if(menu_flag==2) page_current=PAGE_STOPWATCH;//跳转到秒表页面
			else if(menu_flag==3) page_current=PAGE_LIGHT;//跳转手电筒页面
			else if(menu_flag==4) page_current=PAGE_MPU6050;//跳转到陀螺仪页面
			else if(menu_flag==5) page_current=PAGE_MAX30102;//跳转心率血氧页面
	//		else if(menu_flag==6) 
			OLED_Clear();
			return;
        }
		
		
		if(menu_flag==1){
			if(direction_flag==2)direction_move(move_flag,1,0);
			else if(direction_flag==1)direction_move(move_flag,0,0);
		}
		else{
			if(direction_flag==2)direction_move(move_flag,menu_flag,menu_flag-1);
			else if(direction_flag==1)direction_move(move_flag,menu_flag-2,menu_flag-1);
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
void watch_time_page(){
		uint8_t event = Key_GetEvent();
		if (event == KEY_EVENT_KEY0)//计时/暂停
		{
			reset_flag=0;
			time_start++;
			if(time_start>1){
				time_start=0;
			}
		}
		if (event == KEY_EVENT_KEY1)//复位
		{
			time_start=0;
			hour=min=second=0;
			reset_flag=1;
		}
		if (event == KEY_EVENT_KEY2)
		{
			OLED_Clear();
			OLED_Update();
			page_current=PAGE_MENU;
			return;
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

/***********************************手电筒***********************************/
void light_ui()
{
	OLED_Clear();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,20,"开关灯",OLED_8X16);
	OLED_ShowString(0,40,"呼吸灯",OLED_8X16);
}


uint8_t light_select=0;//灯光选择界面
uint8_t light_select_temp=0;
uint8_t led_flag=0;//控制led状态
void light_ui_select(){

		uint8_t event = Key_GetEvent();
		if (event==KEY_EVENT_KEY0)//计时/暂停
		{
			light_select++;
			if(light_select>2){
				light_select=0;
			}
		}
		if (event==KEY_EVENT_KEY1)//复位
		{
			if(light_select<=0){
				light_select=2;
			}
			else light_select--;
		}
		if (event==KEY_EVENT_KEY2)
		{
			if(light_select!=0){ //退出led页面时呼吸灯照常亮
				led_flag=!led_flag;
				light_select_temp=light_select;//控制呼吸灯
			}
			OLED_Clear();
			if(light_select==0) {page_current=PAGE_MENU;}
			else if(light_select==1&&led_flag==1) {led_control(1);}
			else if(light_select==1&&led_flag==0) {led_control(0);}
		}
		if(light_select_temp==2&&led_flag==1) led_control(2);//循环-的数值不能放在按键里
		
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

/***********************************MPU6500***********************************/
//欧拉角测量
float roll=0,pitch=0,yaw=0;//互补滤波后的欧拉角
struct mpu6050_OUT mpu6050_out;
void mpu6500_GetValue(){
	
	static float a=0.9;//互补滤波系数
	static float Dalta_t=0.005;//采样周期5ms
	static float pi=3.1415927;
	extern volatile uint16_t count_mpu6050;
	
	
	count_mpu6050 = 0;

	mpu6050_GetValue(&mpu6050_out);	
		
	//陀螺仪测得欧拉角
	float gx = mpu6050_out.GyroX / 16.4f;
    float gy = mpu6050_out.GyroY / 16.4f;
    float gz = mpu6050_out.GyroZ / 16.4f;

    float roll_g  = roll  + gx * Dalta_t;	
    float pitch_g = pitch + gy * Dalta_t;
    float yaw_g   = yaw   + gz * Dalta_t;
	
	//加速度计测得欧拉角
	float roll_a=atan2(mpu6050_out.AccY,mpu6050_out.AccZ)*180/pi;
	float pitch_a=atan2((-mpu6050_out.AccX),mpu6050_out.AccZ)*180/pi;
	
	//互补滤波结果
	roll=a*roll_g+(1.0-a)*roll_a;
	pitch=a*pitch_g+(1.0-a)*pitch_a;
	yaw=a*yaw_g;
	
	
}


//选择页面
void select_mpu6050_ui(){
	OLED_Clear();
	OLED_ShowImage(0,0,16,16,Return);
	OLED_ShowString(0,16,"陀螺仪",OLED_8X16);
	OLED_ShowString(0,32,"水平仪",OLED_8X16);
}


//陀螺仪页面
void mpu6050_ui(){
	
	mpu6500_GetValue();
	OLED_Clear();
	uint8_t mpu6050_ID=mpu6050_getID();
	OLED_Printf(0,0,OLED_8X16,"ID:%02X",mpu6050_ID);//显示ID	
	OLED_ShowSignedNum(64,0,mpu6050_out.temp/ 333.87f + 21.0f,2,OLED_8X16);//16 位有符号原始 ADC 值 转换成温度
	OLED_Printf(0,16,OLED_8X16,"ROLL:%.2f",roll);
	OLED_Printf(0,32,OLED_8X16,"PITCH:%.2f",pitch);
	OLED_Printf(0,48,OLED_8X16,"YAW:%.2f",yaw);
	OLED_Update();
	uint8_t event = Key_GetEvent();
	if (event == KEY_EVENT_KEY2)
	{
		OLED_Clear();
		page_current=PAGE_MPU6050;	
		return;
	}

}

//水平仪页面
void pinghengyi_ui(void){
	
	mpu6500_GetValue();
	mpu6050_GetValue(&mpu6050_out);	
	OLED_Clear();
	OLED_DrawCircle(64,32,30,0);
	OLED_DrawCircle(64-roll,32+pitch,4,1);
	OLED_Update();
	uint8_t event = Key_GetEvent();
	if (event == KEY_EVENT_KEY2)
	{
		OLED_Clear();
		page_current=PAGE_MPU6050;
		return;
	}
}

uint8_t mpu6050_pages_flag=0;
//mpu6050页面逻辑
void mpu6500_pages(){
	mpu6050_flag=1;
		uint8_t event = Key_GetEvent();
		if (event==KEY_EVENT_KEY0)
		{
			mpu6050_pages_flag++;
			if(mpu6050_pages_flag>=3) mpu6050_pages_flag=0;
		}
		if (event==KEY_EVENT_KEY1)
		{
			if(mpu6050_pages_flag<=0) mpu6050_pages_flag=2;
			else mpu6050_pages_flag--;
		}
		if (event==KEY_EVENT_KEY2)
		{
			OLED_Clear();
			if(mpu6050_pages_flag==0) {page_current=PAGE_MENU;mpu6050_flag=0;}
			else if(mpu6050_pages_flag==1) page_current=PAGE_TUOLUOYI;
			else if(mpu6050_pages_flag==2) page_current=PAGE_STATE;
			return;
		}
		
		
		switch(mpu6050_pages_flag){
			case 0:
			{
				select_mpu6050_ui();
				OLED_ReverseArea(0,0,16,16);
				OLED_Update();
			break;
			}
			
			case 1:
			{
				select_mpu6050_ui();
				OLED_ReverseArea(0,16,48,16);
				OLED_Update();
			break;
			}
			
			case 2:
			{
				select_mpu6050_ui();
				OLED_ReverseArea(0,32,48,16);
				OLED_Update();
			break;
			}
	}			
}

/******************************************MAX30102*********************************************/

#define MAX30102_BUF_SIZE 150
#define MAX30102_FINGER_TH 50000
#define HR_TRIGGER_TH 800
#define HR_RELEASE_TH 200
/*修改*/ #define HR_MIN_AC_AVG 50
/*修改*/ #define MAX30102_WARMUP_SAMPLE 50
/*修改*/ #define MAX30102_HR_VALID_BEATS 1
/*fix*/ #define SPO2_SKIP_WINDOWS 3

int32_t ir_dc=0,ir_ac=0;
uint8_t pulse_flag=0;
uint32_t last_beat_tick=0;
uint32_t interval=0;
uint32_t heart_rate=0;
uint8_t spo2=0;
uint8_t spo2_valid=0;
uint32_t red_buf[MAX30102_BUF_SIZE];
uint32_t ir_buf[MAX30102_BUF_SIZE];
uint16_t spo2_index = 0;
uint8_t spo2_buf_full = 0;
/*fix*/ uint8_t spo2_window_count = 0;
/*修改*/ uint8_t max30102_finger_detected = 0;
/*修改*/ int32_t ir_ac_prev = 0;
/*修改*/ uint32_t ir_ac_abs_avg = 0;
/*修改*/ uint16_t max30102_valid_samples = 0;
/*修改*/ uint8_t max30102_beat_count = 0;
/*修改*/ uint8_t heart_rate_valid = 0;
/*修改*/ uint16_t max30102_r_x100 = 0;
/*修改*/ uint32_t max30102_red_ac_dbg = 0;
/*修改*/ uint32_t max30102_ir_ac_dbg = 0;

void max30102_algorithm_reset(void)
{
	ir_dc = 0;
	ir_ac = 0;
	pulse_flag = 0;
	last_beat_tick = 0;
	interval = 0;
	heart_rate = 0;
	spo2 = 0;
	spo2_valid = 0;
	spo2_index = 0;
	spo2_buf_full = 0;
	/*fix*/ spo2_window_count = 0;
	/*修改*/ max30102_finger_detected = 0; // 没有有效手指信号时，UI回到等待放手指状态
	/*修改*/ ir_ac_prev = 0;
	/*修改*/ ir_ac_abs_avg = 0;
	/*修改*/ max30102_valid_samples = 0;
	/*修改*/ max30102_beat_count = 0;
	/*修改*/ heart_rate_valid = 0;
	/*修改*/ max30102_r_x100 = 0;
	/*修改*/ max30102_red_ac_dbg = 0;
	/*修改*/ max30102_ir_ac_dbg = 0;
}

static void max30102_calc_spo2(void)
{
	uint32_t red_sum = 0, ir_sum = 0;
	uint32_t red_dc, ir_dc_avg;
	uint32_t red_ac_sum = 0, ir_ac_sum = 0;
	uint32_t red_ac, ir_ac_avg;
	float r, spo2_f;
	uint16_t i;

	for (i = 0; i < MAX30102_BUF_SIZE; i++)
	{
		red_sum += red_buf[i];
		ir_sum += ir_buf[i];
	}

	red_dc = red_sum / MAX30102_BUF_SIZE;
	ir_dc_avg = ir_sum / MAX30102_BUF_SIZE;

	/*修改*/ // 用平均绝对偏差计算交流分量，避免max-min被单个毛刺拉偏
	for (i = 0; i < MAX30102_BUF_SIZE; i++)
	{
		if (red_buf[i] > red_dc) red_ac_sum += red_buf[i] - red_dc;
		else red_ac_sum += red_dc - red_buf[i];

		if (ir_buf[i] > ir_dc_avg) ir_ac_sum += ir_buf[i] - ir_dc_avg;
		else ir_ac_sum += ir_dc_avg - ir_buf[i];
	}

	red_ac = red_ac_sum / MAX30102_BUF_SIZE;
	ir_ac_avg = ir_ac_sum / MAX30102_BUF_SIZE;
	/*修改*/ max30102_red_ac_dbg = red_ac;
	/*修改*/ max30102_ir_ac_dbg = ir_ac_avg;

	/*fix*/ // SpO2 does not wait for HR valid

	/*修改*/ // AC太小通常是手指接触不好或光强不合适，此时不计算血氧
	if (red_dc == 0 || ir_dc_avg == 0 || red_ac < 5 || ir_ac_avg < 5)
	{
		spo2_valid = 0;
		return;
	}

	/*修改*/ // R=(RED交流/RED直流)/(IR交流/IR直流)，底部显示R*100用于调试
	r = ((float)red_ac / (float)red_dc) / ((float)ir_ac_avg / (float)ir_dc_avg);
	max30102_r_x100 = (uint16_t)(r * 100.0f + 0.5f);

	/*修改*/ // MAX30102常用经验公式，后续可根据R值和实测值再标定
	/*修改*/ // 你的模块R值稳定在1.0到1.4附近，原二次公式会把血氧压到70；这里先用工程标定公式映射到可用范围
	spo2_f = 110.0f - 13.0f * r;

	if (spo2_f > 100.0f) spo2_f = 100.0f;
	if (spo2_f < 70.0f) spo2_f = 70.0f;

	/*修改*/ // 血氧变化较慢，增加平滑，避免显示值跟着局部波形抖动
	/*fix*/ // skip first SpO2 windows after finger placement
	if (spo2_window_count < SPO2_SKIP_WINDOWS)
	{
		spo2_window_count++;
		spo2_valid = 0;
		return;
	}

	/*fix*/ // lighter smoothing: first valid value is shown directly
	if (spo2_valid) spo2 = (uint8_t)((spo2 * 3 + (uint8_t)(spo2_f + 0.5f)) / 4);
	else spo2 = (uint8_t)(spo2_f + 0.5f);
	spo2_valid = 1;
}
void max30102_data_proc(void)
{
	uint32_t now = HAL_GetTick();

	if (!max30102_sample_valid)
	{
		return;
	}

	/*修改*/ //红外值太低说明没有放手指或接触不好，此时不计算心率和血氧
	if (ir < MAX30102_FINGER_TH)
	{
		max30102_algorithm_reset();
		return;
	}

	/*修改*/ //用低通滤波估算直流分量，交流分量就是围绕基线变化的脉搏波
	/*修改*/ max30102_finger_detected = 1; // 红外值超过阈值，说明已经检测到手指，允许UI显示心率血氧

	if (ir_dc == 0) ir_dc = (int32_t)ir;
	ir_dc = (ir_dc * 95 + (int32_t)ir * 5) / 100;
	ir_ac = (int32_t)ir - ir_dc;

	/*修改*/ //用交流波形的绝对值估算脉搏波幅度，幅度太小说明信号不稳定
	{
		uint32_t abs_ac = (ir_ac >= 0) ? (uint32_t)ir_ac : (uint32_t)(-ir_ac);
		ir_ac_abs_avg = (ir_ac_abs_avg * 15 + abs_ac) / 16;
	}

	/*修改*/ //用过零上升沿判断一次心跳，比固定阈值更适合不同手指压力和不同LED反射强度
	/*修改*/ // 刚放手指时直流基线和按压力还在变化，先预热一段时间，不把这段数据用于显示
	if (max30102_valid_samples < MAX30102_WARMUP_SAMPLE)
	{
		max30102_valid_samples++;
		last_beat_tick = now;
		ir_ac_prev = ir_ac;
		return;
	}

	if (ir_ac_prev <= 0 && ir_ac > 0 && ir_ac_abs_avg > HR_MIN_AC_AVG)
	{
		uint32_t new_hr;
		interval = now - last_beat_tick;
		if (interval > 300 && interval < 2000)
		{
			new_hr = 60000 / interval;
			/*修改*/ // 过滤刚开始偶发的异常间隔，避免HR在60到120之间大幅跳动
			if (new_hr >= 45 && new_hr <= 160)
			{
				max30102_beat_count++;
				if (heart_rate == 0) heart_rate = new_hr;
				else heart_rate = (heart_rate * 7 + new_hr) / 8;
				if (max30102_beat_count >= MAX30102_HR_VALID_BEATS) heart_rate_valid = 1;
			}
		}
		last_beat_tick = now;
		pulse_flag = 1;
	}

	/*修改*/ //记录上一次交流分量，用于下一次判断是否发生过零上升沿
	ir_ac_prev = ir_ac;

	red_buf[spo2_index] = red;
	ir_buf[spo2_index] = ir;
	spo2_index++;

	if (spo2_index >= MAX30102_BUF_SIZE)
	{
		spo2_index = 0;
		spo2_buf_full = 1;
		max30102_calc_spo2();
	}
}
void max30102_ui(void){
	static uint8_t enter_flag = 0;
	uint8_t MAX30102_ID;
	uint8_t event;

	if (enter_flag == 0)
	{
		max30102_init();
		/*修改*/ //初始化函数内部已经在启动采样前清过FIFO，这里不要再次清FIFO
		max30102_algorithm_reset();
		max30102_flag = 1;
		enter_flag = 1;
	}

	MAX30102_ID=max30102_getID();
	event=Key_GetEvent();

		OLED_Clear();
	/*修改*/ //MAX30102测量页面：上方显示标题，中间显示心率和血氧，底部显示当前测量状态
	OLED_ShowString(0,0,"MAX30102",OLED_8X16);
	OLED_ShowImage(0,48,16,16,Return);

	if (!max30102_finger_detected)
	{
		OLED_ShowString(24,20,"Put finger",OLED_8X16);
		OLED_ShowString(24,40,"Waiting...",OLED_8X16);
	}
	else
	{
		if (heart_rate) OLED_Printf(0,18,OLED_8X16,"HR  :%03lu BPM",heart_rate);
		else OLED_ShowString(0,18,"HR  :--- BPM",OLED_8X16);

		if (spo2_valid) OLED_Printf(0,36,OLED_8X16,"SpO2:%03d %%",spo2);
		else OLED_ShowString(0,36,"SpO2:--- %",OLED_8X16);
		/*修改*/ OLED_Printf(24,52,OLED_6X8,"R:%03d",max30102_r_x100);
	}
	OLED_Update();

	if(event==KEY_EVENT_KEY2){
		page_current=PAGE_MENU;
		max30102_flag=0;
		enter_flag=0;
		max30102_stop();
	}
}



/**********************************任务调度*********************************/
void UI_Task(){
	switch (page_current)
	{
	case (PAGE_HOME):
	{
		home_page();
		break;
	}
	case (PAGE_MENU):
	{
		menu_ui();
		break;
	}
	case (PAGE_SET):
	{
		set_page();
		break;
	}
	case (PAGE_SETTIME):
	{
		settim_UI_Task();
		break;
	}
	case (PAGE_STOPWATCH):
	{
		watch_time_page();
		break;
	}
    case (PAGE_LIGHT):
	{
       light_ui_select();
        break;
	}
    case (PAGE_MPU6050):
	{
       mpu6500_pages();
        break;
	}
	case (PAGE_TUOLUOYI):
	{
       mpu6050_ui();
        break;
	}
	case (PAGE_STATE):
	{
       pinghengyi_ui();
        break;
	}
	case (PAGE_MAX30102):
	{
    	max30102_ui();
        break;
	}
}

}


void Led_Task(){
	if(page_current!=PAGE_LIGHT&&light_select_temp==2&&led_flag==1) led_control(2);
}



void Sensor_Task(){
	static uint32_t mpu_last_tick=0;
	static uint32_t max_last_tick=0;
	if(mpu6050_flag){
		if (HAL_GetTick() - mpu_last_tick >= 5) { // 每5ms采样一次
			mpu_last_tick = HAL_GetTick();
			mpu6500_GetValue();
		}
	}
	if(max30102_flag){
		if (HAL_GetTick() - max_last_tick >= 10) {// 每10ms采样一次
			max_last_tick = HAL_GetTick();
			max30102_GetValue();
			max30102_data_proc();
		}
	}
}


