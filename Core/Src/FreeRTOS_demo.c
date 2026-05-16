#include "FreeRTOS_demo.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key.h"
#include "led.h"
#include "stdio.h"
#include "string.h"
#include "menu.h"
#include "oled.h"
#include "font.h"
#include "mpu6050.h"

//freertos_start
#define start_task_stack_size 256
#define start_task_priority   1
TaskHandle_t start_task_handle;     
//Key_Task
#define Key_Task_stack_size 128
#define Key_Task_priority   2
TaskHandle_t Key_Task_handle = NULL;
//UI_Task
#define UI_Task_stack_size 512
#define UI_Task_priority   4
TaskHandle_t UI_Task_handle = NULL;
//Sensor_Task
#define Sensor_Task_stack_size 512
#define Sensor_Task_priority   3
TaskHandle_t Sensor_Task_handle = NULL;
//Led_Task
#define Led_Task_stack_size 128
#define Led_Task_priority   5
TaskHandle_t Led_Task_handle = NULL;

//函数声明
void start_task(void *pvParameters);
void Key_Task_RTOS(void *pvParameters);
void UI_Task_RTOS(void *pvParameters);
void Sensor_Task_RTOS(void *pvParameters);
void Led_Task_RTOS(void *pvParameters);

void freertos_start(){
	xTaskCreate( (TaskFunction_t) 				start_task,//指向任务函数的指针
				 (char* )						"start_task",//任务的描述性名称，用于方便调试，也可用于任务句柄
				 (configSTACK_DEPTH_TYPE) 		start_task_stack_size,//任务分配的堆栈大小（字）
				 (void *)						NULL,//作为参数传递所创建任务的值
				 (UBaseType_t )					start_task_priority,//任务优先级
				 (TaskHandle_t *) 					&start_task_handle//用于将句柄传递给xTaskCreat()函数创建的任务
				 );
	vTaskStartScheduler();//开启任务调度器
}

void start_task(void *pvParameters){
	//进入临界区，临界区里的代码不会被打断
	taskENTER_CRITICAL();
		xTaskCreate((TaskFunction_t) 				Key_Task_RTOS,
					(char *)					 	"Key_Task_RTOS",
					(configSTACK_DEPTH_TYPE )		Key_Task_stack_size,
					(void *)						NULL,
					(UBaseType_t )					Key_Task_priority,
					(TaskHandle_t *) 					&Key_Task_handle
			 );
		xTaskCreate((TaskFunction_t) 				UI_Task_RTOS,
					(char *)					 	"UI_Task_RTOS",
					(configSTACK_DEPTH_TYPE )		UI_Task_stack_size,
					(void *)						NULL,
					(UBaseType_t )					UI_Task_priority,
					(TaskHandle_t *) 					&UI_Task_handle
			 );
		xTaskCreate((TaskFunction_t) 				Sensor_Task_RTOS,
					(char *)					 	"Sensor_Task_RTOS",
					(configSTACK_DEPTH_TYPE )		Sensor_Task_stack_size,
					(void *)						NULL,
					(UBaseType_t )					Sensor_Task_priority,
					(TaskHandle_t *) 					&Sensor_Task_handle
			 );
		xTaskCreate((TaskFunction_t) 				Led_Task_RTOS,
					(char *)					 	"Led_Task_RTOS",
					(configSTACK_DEPTH_TYPE )		Led_Task_stack_size,
					(void *)						NULL,
					(UBaseType_t )					Led_Task_priority,
					(TaskHandle_t *) 					&Led_Task_handle
			 );
		//推出临界区
		taskEXIT_CRITICAL();
		vTaskDelete(NULL);
		
}

//Key_Task
void Key_Task_RTOS(void *pvParameters){
    while(1){
		Key_Task();
       	vTaskDelay(5);
    }
}

//UI_Task
void UI_Task_RTOS(void *pvParameters){
	while(1){
		UI_Task();
       	vTaskDelay(10);
	}
}

//Sensor_Task
void Sensor_Task_RTOS(void *pvParameters){
	while(1){
		Sensor_Task();
	   	vTaskDelay(10);
	}
}
//Led_Task
void Led_Task_RTOS(void *pvParameters){
	while(1){
		Led_Task();
	   	vTaskDelay(30);
	}
}


