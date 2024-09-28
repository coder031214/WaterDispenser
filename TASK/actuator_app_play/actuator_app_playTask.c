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
	TIM3_PWM_Init(99,35);  //马达PWM初始化void TIM3_PWM_Init(u16 crr,u16 psc);//
	Usart2_Init(9600);  //蓝牙串口初始化
	my_LED_Init();
	my_LED1_Init();
	
}


void actuator_app_play_TASK(Sensor_Data_t *Sensor_Data)  
{
	printf("actuator_app_play_TASK Start\r\n");
	while(1)
	{	
		
		

	if(Sensor_Data->Water_Low)//水位过低启动水泵
	{
		// PWMSet=110; 
		TIM_SetCompare2(TIM3,50);
	}
	if(Sensor_Data->Water_Hight)//过高停止水泵
	{
		// PWMSet=0; 
		TIM_SetCompare2(TIM3,0);
	}
	
	if(Sensor_Data->Water_Low&Sensor_Data->Water_Hight)//低水位高水位同时不正常
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
	
	if(Sensor_Data->TDS_Source>=100)//水质检测TDS2>100向服务器发出警告
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
	
	
	
	
	
	
	if(!Sensor_Data->Light_Heat)  //加热状态指示灯
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_0); 
		
	} 
	else  
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_0); 
	}
	
	if(!Sensor_Data->Light_Normal)  //常温出水指示灯
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_1); 
		
	} 
	else  
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_1); 	
	}

	if(!Sensor_Data->Light_Hot)  //热水出水指示灯
	{
		GPIO_ResetBits(GPIOE,GPIO_Pin_2); 
		
	} 
	else  
	{
		GPIO_SetBits(GPIOE,GPIO_Pin_2); 	
	}

	
	
	
//	if(Sensor_Data->Temp_Hot < Sensor_Data->Temp_SetHot)  //低于设定温度值自动加热
//	{
//		Sensor_Data->Light_Heat=1;
//	} 
//	else  
//	{
//		Sensor_Data->Light_Heat=0;	
//	}

	if(Sensor_Data->Water_Quantity>=1000)  //出水量超过最大值停止出水
	{
		
		Sensor_Data->Light_Normal=0;
		Sensor_Data->Light_Hot=0;
		Sensor_Data->Water_Quantity=0;
		
		
	}


	
	
	
  vTaskDelay(50);
	}
	
}

