#include "led.h"
#include "lcd.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "beep.h"
#include "IWDG.h"
#include "Get_Sensors_DataTask.h"
#include "lcd_app_playTask.h"
#include "user_app_playTask.h"
#include "actuator_app_playTask.h"
#include "wifi_app_playTask.h"
#include "W25Q64.h"
#include "stmflash.h"
#include "usart4.h"


//看门狗任务
#define IWDG_TASK_PRIO		1
#define IWDG_STK_SIZE		64
TaskHandle_t IWDG_Task_Handler;
void IWDG_Task(void *pvParameters);

#define START_TASK_PRIO		6  //任务优先级
#define START_STK_SIZE 		128  //任务堆栈大小	
TaskHandle_t StartTask_Handler;//任务句柄
void start_task(void *pvParameters);//任务函数


#define Get_Sensors_Data_TASK_PRIO		5//任务优先级
#define Get_Sensors_Data_STK_SIZE 		256 //任务堆栈大小
TaskHandle_t Get_Sensors_DataTask_Handler;//任务句柄


#define lcd_app_play_TASK_PRIO		4//任务优先级
#define lcd_app_play_STK_SIZE 		512  //任务堆栈大小
TaskHandle_t lcd_app_playTask_Handler;//任务句柄


#define user_app_play_TASK_PRIO		3//任务优先级
#define user_app_play_STK_SIZE 		256 //任务堆栈大小
TaskHandle_t user_app_playTask_Handler;//任务句柄


#define actuator_app_play_TASK_PRIO		2//任务优先级
#define actuator_app_play_STK_SIZE 		256 //任务堆栈大小
TaskHandle_t actuator_app_playTask_Handler;//任务句柄


#define wifi_app_play_TASK_PRIO		7//任务优先级
#define wifi_app_play_STK_SIZE 		256 //任务堆栈大小
TaskHandle_t wifi_app_playTask_Handler;//任务句柄

u8 model=0;
u8 temp1_set=28,temp2_set=25,shine_set=85; 
extern Sensor_Data_t Sensor_Data;
extern const char main_page[1][240*320/8];

extern u16 buffer[2]; // 声明一个长度为1的数组缓冲区
extern u16 buffer2[2]; 

int a,b;


int main(void)
{
	W25Q64_Init();
	delay_init();	    				//延时函数初始化	  
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组4	 	 
	uart_init(115200);					//初始化串口
	
	UART4_Init(9600);
	LED_Init();		  					//初始化LED
	BEEP_Init();			    //LED端口初始化
 	LCD_Init();  					//LCD初始化
	Draw_word(0,0,240,320,0x0000,main_page[0]);
	user_app_init();    //本地交互按键初始化
	actuator_app_init();//所有执行器初始化
	sensor_app_init();  //所有传感器初始化
	LCD_Chinese_Init(); //汉字初始化
	lcd_app_darw_main();  //绘制初始LCD页面 
	wifi_app_init(&Sensor_Data);    //所有wifi初始化	
	
//	STMFLASH_Read(STM32_FLASH_BASE,buffer2,2);   
//	a=buffer2[0];
//	b=buffer2[1];
//	
//	
//	printf("Start_task start---------------------\r\n");
//	printf("Start_task start-----------------\r\n");
//	printf("Start_task start---------------\r\n");
//	printf("Start_task start--------------%d\r\n",a);
//	printf("Start_task start------------------%d\r\n",b);
//	
//	
//		buffer[0]=66;
//		buffer[1]=88;
//		STMFLASH_Write(FLASH_SAVE_ADDR,buffer,2);		//从指定地址开始写入指定长度的数据
		
	
	
	
	
	
	
	
	//Iwdg_Init(5, 1250); 	//128分频，每秒625次，重载1250次，4s
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
	  printf("Start_task start\r\n");
    taskENTER_CRITICAL();           //进入临界区
	//云平台上报下发数据任务								
   xTaskCreate((TaskFunction_t )wifi_app_play_TASK,     	
                (const char*    )"wifi_app_play_TASK",   	
                (uint16_t       )wifi_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )wifi_app_play_TASK_PRIO,	
                (TaskHandle_t*  )&wifi_app_playTask_Handler);  	
//	//看门狗任务
//	xTaskCreate((TaskFunction_t)IWDG_Task,
//								"IWDG",
//								IWDG_STK_SIZE,
//								NULL,
//								IWDG_TASK_PRIO, 
//				(TaskHandle_t*)&IWDG_Task_Handler);
    //获取传感器数据任务
    xTaskCreate((TaskFunction_t )Get_Sensors_Data_TASK,     	
                (const char*    )"Get_Sensors_Data_TASK",   	
                (uint16_t       )Get_Sensors_Data_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )Get_Sensors_Data_TASK_PRIO,	
                (TaskHandle_t*  )&Get_Sensors_DataTask_Handler);   
    //进行LCD数据刷新任务
    xTaskCreate((TaskFunction_t )lcd_app_play_TASK,     
                (const char*    )"lcd_app_play_TASK",   
                (uint16_t       )lcd_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,
                (UBaseType_t    )lcd_app_play_TASK_PRIO,
                (TaskHandle_t*  )&lcd_app_playTask_Handler); 
		//本地键盘交互任务						
   xTaskCreate((TaskFunction_t )user_app_play_TASK,     	
                (const char*    )"user_app_play_TASK",   	
                (uint16_t       )user_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )user_app_play_TASK_PRIO,	
                (TaskHandle_t*  )&user_app_playTask_Handler); 
   //执行器控制任务								
   xTaskCreate((TaskFunction_t )actuator_app_play_TASK,     	
                (const char*    )"actuator_app_play_TASK",   	
                (uint16_t       )actuator_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )actuator_app_play_TASK_PRIO,	
                (TaskHandle_t*  )&actuator_app_playTask_Handler);
   							
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


