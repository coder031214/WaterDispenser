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


//���Ź�����
#define IWDG_TASK_PRIO		1
#define IWDG_STK_SIZE		64
TaskHandle_t IWDG_Task_Handler;
void IWDG_Task(void *pvParameters);

#define START_TASK_PRIO		6  //�������ȼ�
#define START_STK_SIZE 		128  //�����ջ��С	
TaskHandle_t StartTask_Handler;//������
void start_task(void *pvParameters);//������


#define Get_Sensors_Data_TASK_PRIO		5//�������ȼ�
#define Get_Sensors_Data_STK_SIZE 		256 //�����ջ��С
TaskHandle_t Get_Sensors_DataTask_Handler;//������


#define lcd_app_play_TASK_PRIO		4//�������ȼ�
#define lcd_app_play_STK_SIZE 		512  //�����ջ��С
TaskHandle_t lcd_app_playTask_Handler;//������


#define user_app_play_TASK_PRIO		3//�������ȼ�
#define user_app_play_STK_SIZE 		256 //�����ջ��С
TaskHandle_t user_app_playTask_Handler;//������


#define actuator_app_play_TASK_PRIO		2//�������ȼ�
#define actuator_app_play_STK_SIZE 		256 //�����ջ��С
TaskHandle_t actuator_app_playTask_Handler;//������


#define wifi_app_play_TASK_PRIO		7//�������ȼ�
#define wifi_app_play_STK_SIZE 		256 //�����ջ��С
TaskHandle_t wifi_app_playTask_Handler;//������

u8 model=0;
u8 temp1_set=28,temp2_set=25,shine_set=85; 
extern Sensor_Data_t Sensor_Data;
extern const char main_page[1][240*320/8];

extern u16 buffer[2]; // ����һ������Ϊ1�����黺����
extern u16 buffer2[2]; 

int a,b;


int main(void)
{
	W25Q64_Init();
	delay_init();	    				//��ʱ������ʼ��	  
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����4	 	 
	uart_init(115200);					//��ʼ������
	
	UART4_Init(9600);
	LED_Init();		  					//��ʼ��LED
	BEEP_Init();			    //LED�˿ڳ�ʼ��
 	LCD_Init();  					//LCD��ʼ��
	Draw_word(0,0,240,320,0x0000,main_page[0]);
	user_app_init();    //���ؽ���������ʼ��
	actuator_app_init();//����ִ������ʼ��
	sensor_app_init();  //���д�������ʼ��
	LCD_Chinese_Init(); //���ֳ�ʼ��
	lcd_app_darw_main();  //���Ƴ�ʼLCDҳ�� 
	wifi_app_init(&Sensor_Data);    //����wifi��ʼ��	
	
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
//		STMFLASH_Write(FLASH_SAVE_ADDR,buffer,2);		//��ָ����ַ��ʼд��ָ�����ȵ�����
		
	
	
	
	
	
	
	
	//Iwdg_Init(5, 1250); 	//128��Ƶ��ÿ��625�Σ�����1250�Σ�4s
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������              
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
	  printf("Start_task start\r\n");
    taskENTER_CRITICAL();           //�����ٽ���
	//��ƽ̨�ϱ��·���������								
   xTaskCreate((TaskFunction_t )wifi_app_play_TASK,     	
                (const char*    )"wifi_app_play_TASK",   	
                (uint16_t       )wifi_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )wifi_app_play_TASK_PRIO,	
                (TaskHandle_t*  )&wifi_app_playTask_Handler);  	
//	//���Ź�����
//	xTaskCreate((TaskFunction_t)IWDG_Task,
//								"IWDG",
//								IWDG_STK_SIZE,
//								NULL,
//								IWDG_TASK_PRIO, 
//				(TaskHandle_t*)&IWDG_Task_Handler);
    //��ȡ��������������
    xTaskCreate((TaskFunction_t )Get_Sensors_Data_TASK,     	
                (const char*    )"Get_Sensors_Data_TASK",   	
                (uint16_t       )Get_Sensors_Data_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )Get_Sensors_Data_TASK_PRIO,	
                (TaskHandle_t*  )&Get_Sensors_DataTask_Handler);   
    //����LCD����ˢ������
    xTaskCreate((TaskFunction_t )lcd_app_play_TASK,     
                (const char*    )"lcd_app_play_TASK",   
                (uint16_t       )lcd_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,
                (UBaseType_t    )lcd_app_play_TASK_PRIO,
                (TaskHandle_t*  )&lcd_app_playTask_Handler); 
		//���ؼ��̽�������						
   xTaskCreate((TaskFunction_t )user_app_play_TASK,     	
                (const char*    )"user_app_play_TASK",   	
                (uint16_t       )user_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )user_app_play_TASK_PRIO,	
                (TaskHandle_t*  )&user_app_playTask_Handler); 
   //ִ������������								
   xTaskCreate((TaskFunction_t )actuator_app_play_TASK,     	
                (const char*    )"actuator_app_play_TASK",   	
                (uint16_t       )actuator_app_play_STK_SIZE, 
                (void*          )&Sensor_Data,				
                (UBaseType_t    )actuator_app_play_TASK_PRIO,	
                (TaskHandle_t*  )&actuator_app_playTask_Handler);
   							
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


