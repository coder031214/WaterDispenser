#include "FreeRTOS.h"
#include "task.h"
#include "sg90.h"
#include "actuator_app_playTask.h"
#include "gpio.h"
#include "led.h"
#include "usart2.h"
#include "Get_Sensors_DataTask.h"
#include "user_app_playTask.h"
#include "delay.h"
#include "beep.h"
extern  int Light;

void actuator_app_init(void)
{
	TIM3_PWM_Init(99,35);  //���PWM��ʼ��void TIM3_PWM_Init(u16 crr,u16 psc);//
	Usart2_Init(9600);  //�������ڳ�ʼ��
	my_LED_Init();
	my_LED1_Init();
	
}


void actuator_app_play_TASK(Sensor_Data_t *Sensor_Data)  
{
	printf("actuator_app_play_TASK Start\r\n");
	while(1)
	{	
		
		

	if(Sensor_Data->Water_Low)//ˮλ��������ˮ��
	{
		// PWMSet=110; 
		TIM_SetCompare2(TIM3,50);
	}
	if(Sensor_Data->Water_Hight)//����ֹͣˮ��
	{
		// PWMSet=0; 
		TIM_SetCompare2(TIM3,0);
	}
	
	if(Sensor_Data->Water_Low&Sensor_Data->Water_Hight)//��ˮλ��ˮλͬʱ������
	{
		BEEP=1;
		delay_ms(400);
		BEEP=0;
		delay_ms(600);
		Sensor_Data->ErrorAlarm=1;
		printf("-------------------------------------------------------------ErrorAlarm==1\r\n");
//		------------------sprintf"Error1"
	}
	else
	Sensor_Data->ErrorAlarm=0;		
	
	if(Sensor_Data->TDS_Source>=100)//ˮ�ʼ��TDS2>100���������������
	{
		BEEP=1;
		delay_ms(100);
		BEEP=0;
		delay_ms(900);
//		sprintf"Waring1"
		Sensor_Data->WaringAlarm=1;
		printf("-----------------Waring1\r\n");
		Sensor_Data->WaringAlarm=1;
	}
	
			
//	else		Sensor_Data->WaringAlarm=0;
	
	
	
	
	
	
	if(!Sensor_Data->Light_Heat)  //����״ָ̬ʾ��
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_0); 
		
	} 
	else  
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_0); 
	}
	
	if(!Sensor_Data->Light_Normal)  //���³�ˮָʾ��
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_1); 
		
	} 
	else  
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_1); 	
	}

	if(!Sensor_Data->Light_Hot)  //��ˮ��ˮָʾ��
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_2); 
		
	} 
	else  
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2); 	
	}

	
	
	
//	if(Sensor_Data->Temp_Hot < Sensor_Data->Temp_SetHot)  //�����趨�¶�ֵ�Զ�����
//	{
//		Sensor_Data->Light_Heat=1;
//	} 
//	else  
//	{
//		Sensor_Data->Light_Heat=0;	
//	}

	if(Sensor_Data->Water_Quantity>=1000)  //��ˮ���������ֵֹͣ��ˮ
	{
		
		Sensor_Data->Light_Normal=0;
		Sensor_Data->Light_Hot=0;
		Sensor_Data->Water_Quantity=0;
		
		
	}


	
	
	
  vTaskDelay(50);
	}
	
}

