#include "FreeRTOS.h"
#include "task.h"
#include "key1.h"
#include "user_app_playTask.h"
#include "Get_Sensors_DataTask.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "delay.h"
#include "W25Q64.h"


#include "stmflash.h"
int direction_model=0;
int Light=0;
uint8_t Temp_Set;
int Drink_over;
int wificonnect;

		
u16 TEXT_Buffer_temp[1];
u16 datatamp[SIZE_temp];
	  



void user_app_init(void)
{
	key_init();
	W25Q64_Init();	
}

keyType keyValueTemp ;
void user_app_play_TASK(Sensor_Data_t *Sensor_Data)  //����ȫ����exti.c���жϺ�����
{
	
	STMFLASH_Read(FLASH_SAVE_ADDR_temp,(u16 *)datatamp,SIZE_temp);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
	Sensor_Data->Temp_SetHot=datatamp[0];
	printf("--------------------------------test = %d\r\n",datatamp[0]);
	
	
	
	  while(1)
  {

	  
	keyValueTemp=KEY_ScanTriggerOnce();	
	  
	if(keyValueTemp != noneKey)
    printf("keyValueTemp = %d\r\n",keyValueTemp);	
//	if(Sensor_Data->User_ID!=NULL)
//	{
		if(keyValueTemp==s106)//
			{
				if(Sensor_Data->Light_Hot==1&Sensor_Data->Light_Normal==0)
				{
					Drink_over=1;
				}
				Sensor_Data->Light_Hot=!Sensor_Data->Light_Hot;			//���ȵ�
				Sensor_Data->Light_Normal=0;
			}
		if(keyValueTemp==s116)//������s5�� -��ŷ�S1����ˮ
			{
				if(Sensor_Data->Light_Hot==0&Sensor_Data->Light_Normal==1)
				{
					Drink_over=1;
				}			
				Sensor_Data->Light_Normal=!Sensor_Data->Light_Normal;
				Sensor_Data->Light_Hot=0;
			}	
		if(keyValueTemp==s121)//������s9�� -��ŷ�S2��ˮ
			{
//				Sensor_Data->Light_Heat=!Sensor_Data->Light_Heat;			
				SYN_FrameInfo(0, "[m0][v5][t5]ңң����");
				wificonnect=1;
			}
			

		if(keyValueTemp==s120)//�趨ˮ��
			{
				Sensor_Data->Temp_SetHot++;
				if(Sensor_Data->Temp_SetHot>=101)
					Sensor_Data->Temp_SetHot=0;
				
				TEXT_Buffer_temp[0]=Sensor_Data->Temp_SetHot;
				STMFLASH_Write(FLASH_SAVE_ADDR_temp,(u16 *)TEXT_Buffer_temp,SIZE_temp);		//��ָ����ַ��ʼд��ָ�����ȵ�����


				printf("--------------------------------readarr = %d\r\n",TEXT_Buffer_temp[0]);	
	
			}
			
		if(K_PF7==0)//�趨ˮ��
			{
				
				Sensor_Data->Temp_SetHot++;
				if(Sensor_Data->Temp_SetHot==100)
					Sensor_Data->Temp_SetHot=0;
				
			}
			vTaskDelay(50);
		}
	//}
}

