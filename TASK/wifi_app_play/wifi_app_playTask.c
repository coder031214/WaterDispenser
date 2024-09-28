#include "FreeRTOS.h"
#include "task.h"
#include "sys.h"
#include "delay.h"
#include "wifi_app_playTask.h"
#include "actuator_app_playTask.h"
#include "usart.h"
#include "beep.h"
#include "led.h"
#include "hal_uart3.h"
#include "stmflash.h"
#include "WiFiToCloud.h"
#include "tim7.h"
#include <string.h>	//�ڴ������غ�����
#include <stdio.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "user_app_playTask.h"
#include "SYN6288.h"
extern int Drink_over;
extern int wificonnect;
uint32_t SAVE_ADDR =0x08070000;
Sensor_Data_t Last_Sensor_Data;
Sensor_Data_t Sensor_Data;
u16 *  TEXT_Buffer[4];
u16  * Read_Buffer[4];
// ����ȫ�ֱ���
 int  timestamp = 1694575776; // ����ʱ���Ϊ1631350215
// ����������ڴ洢��ǰʱ��
 int hours = 0;
 int minutes = 0;
 int seconds = 0;

// ��������
//void convert_timestamp_to_time(void);
void TIM1_Int_Init(u16 arr,u16 psc);
void wifi_connect(Sensor_Data_t *Sensor_Data);

//	u8 datatemp[SIZE];//��ȡFlash������ݵĵط�
//�Ĵ���������
uint8_t UART_RX_BUF[256];//���յ��Ĵ������ݻ�����
uint8_t UART_TX_BUF[256];//���յ��Ĵ������ݻ�����

uint8_t lock=1;

/*******************************************************************
*������char *USER_GetJsonValue(char *cJson, char *Tag)
*���ܣ�jsonΪ�ַ������У���json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ
*���룺
		char *cJson json�ַ���
		char *Tag Ҫ�����Ķ����ǩ
*�����������ֵ���ַ�����ʽ����ʼ��ַ
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
char *USER_GetJsonValue(char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[10];
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		//printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
	{
		if((*target<='9')&&(*target>='0'))
		{
			temp[i]=*target;
		}
		else
		{
			break;
		}
	}
	temp[i+1] = '\0';
	//printf("��ֵ=%s\r\n",temp);
	return (char *)temp;
}

bool substringMatch(const char *text, const char *pattern) {
    int i, j;
    int textLen = strlen(text);
    int patternLen = strlen(pattern);
    
    for (i = 0; i <= textLen - patternLen; i++) {
        for (j = 0; j < patternLen; j++) {
            if (text[i + j] != pattern[j]) {
                break;
            }
        }
        
        if (j == patternLen) {
            return true;  // �Ӵ�ƥ��ɹ�
        }
    }
    
    return false;  // �Ӵ�ƥ��ʧ��
}




uint32_t TimeCount=0;
uint8_t IpData[128];
/////////////////////////////
//wifi��ʼ��
//wifi����
//////////////////////////////
void wifi_app_init(Sensor_Data_t *Sensor_Data)
{
	
	//printf("----------------------USART3_RX_COUNT%d\r\n",USART3_RX_COUNT);
	USART3_Init(115200);

	TIM7_Int_Init(7200-1,9);		//10ms

	wifi_connect(Sensor_Data);	  //��wifi_app_play�н���
	
	memset(IpData, 0x00, 128);  //����IpData
	TIM1_Int_Init(10000-1,7200-1);	
	TIM_Cmd(TIM1,ENABLE);//������ʱ��1
}


void ESP8266_Res(void)
{
		//ESP8266��λ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOG,&GPIO_InitStructure);
	
	GPIO_ResetBits(ESP8266_ResGPIO, ESP8266_ResPIN);
	delay_ms(50);
	GPIO_SetBits(ESP8266_ResGPIO, ESP8266_ResPIN);
	delay_ms(200);
}
void wifi_connect(Sensor_Data_t *Sensor_Data)
{
	int16_t temp;
	u8 i,TryCount;
	//ESP8266_Res();
	
	
	//���ӷ�����
	for(TryCount=0; TryCount<3; TryCount++)
	{
		temp=ConnectToServer((char *)MY_DEVICE_ID, (char *)MA_SECRET_KEY);		//return = 0 ,sucess
		                                                                      //return < 0 ,error  
		
		if(temp != 0)  //ʧ��
		{	printf("TCP����ʧ��\r\n");
			for(i=0;i<temp;i++)//�����̵���˸�����жϷ����˺�������
			{
				BEEP=0;
				delay_ms(200);
				BEEP=1;
				delay_ms(200);
			} 
		}
		else  //�������ӳɹ�
		{
			for(i=0;i<3;i++)
			{
				BEEP=1;
				delay_ms(200);
				BEEP=0;
				delay_ms(200);
				Sensor_Data->connect=1;
				printf("�������ӳɹ�\r\n");
				
	
				printf("���븴λ״̬\r\n");
			}
			break;
		}
	}

}

#include <stdio.h>
#include <time.h>

//void convertTimestampToTime( uint32_t timestamp, uint32_t *hours, uint32_t *minutes,uint32_t *seconds) {
//    struct tm* timeinfo;
//    timeinfo = localtime(&timestamp);
//    
//    *hours = timeinfo->tm_hour;
//    *minutes = timeinfo->tm_min;
//    *seconds = timeinfo->tm_sec;

//   
//}


void convert_timestamp_to_time(void)
{
    // ����Сʱ
    hours = timestamp / 3600%24+8	;

    // �������
    minutes = (timestamp % 3600) / 60;

    // ��������
    seconds = (timestamp % 3600) % 60;
	
	
}


void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) 
	{
        // �ڴ˴������ϣ���ڶ�ʱ�ж���ִ�еĴ���
        timestamp++;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // ����жϱ�־λ
    }
}

void TIM1_Int_Init(u16 arr,u16 psc)
{	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
	
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	
}







/*******************************************************************
*������void USER_DataAnalysisProcess(char *RxBuf,Sensor_Data_t *Sensor_Data)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�����
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf,Sensor_Data_t *Sensor_Data)
{
	printf("-------------------------1\r\n");
	//char *cmdid = NULL;
	uint8_t TxetBuf[128];
	if(substringMatch((const char *)RxBuf, (const char *)"\"tag\":\"Machine_State\"")==true)//����/��������
	{												    //"\"tag\":\"Machine_State\"""\"data\":\"1\""
		printf("-------------------------2\r\n");
		memset(TxetBuf,0x00,128);//��ջ���
		if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"1\"") ==true))//����
		{													 //"\"data\":\"1\""
			  
			printf("�ƶ˿������\r\n");
			Sensor_Data->Machine_State=2;
			beep_play(2);  //�����������Σ�������		"tag:Machine_State""data:1"


		}
		else if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"0\"") == true))//����
		{
			printf("�ƶ˹������\r\n");
			Sensor_Data->Machine_State=3;
			beep_play(1);  //��������һ�Σ��������
			
		}			
	}
	if(substringMatch((const char *)RxBuf, (const char *)"time")==true)//����/��������
	{
		
		
		char original_str[50] ;
		strcpy(original_str, RxBuf);
		int start_pos =8;
		int end_pos = 19;
		int length = end_pos - start_pos;
		char result[length + 1];
		strncpy(result, original_str + start_pos, length);
		result[length] = '\0';
		printf("ʱ����ǡ�������������������������������������������%s\r\n",result);
		timestamp=(int) strtol(result, NULL, 10);
		printf("ʱ����ǡ�������������������������������������������%d\r\n",timestamp);	


	}
	if(substringMatch((const char *)RxBuf, (const char *)"\"tag\":\"LED\"")==true)//����/��������
	{
		
		printf("-------------------------3\r\n");
		printf("tag:LEDr\n");
		memset(TxetBuf,0x00,128);//��ջ���
		if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"0\"") ==true))//����
		{
			  
			printf("�ƶ˿��ƣ�\r\n");
			Sensor_Data->Light_Heat=0; 

		}
		else if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"1\"") == true))//����
		{
			printf("�ƶ˹صƣ�\r\n");
			Sensor_Data->Light_Heat=1;
			
		}			
	}
	
	if(substringMatch((const char *)RxBuf, (const char *)"Temp_SetHot")==true)//����/��������
	{
		int S;
		char original_str[50] ;
		strcpy(original_str, RxBuf);
		int start_pos =29;
		int end_pos = 32;
		int length = end_pos - start_pos;
		char result[length + 1];
		strncpy(result, original_str + start_pos, length);
		result[length] = '\0';
		
		S=atoi(result);
		
		Sensor_Data->Temp_SetHot=S;
		printf("Temp_SetHot��������������������������������������������%d\r\n",S);	
	}
	
}


void Sensor_Flash(Sensor_Data_t *Sensor_Data,u32 WriteAddr)
{
	TEXT_Buffer[0]=(u16*)Sensor_Data->Machine_ID; 
	TEXT_Buffer[1]=(u16*)Sensor_Data->User_ID;
	TEXT_Buffer[2]=(u16*)Sensor_Data->Water_Quantity;	
	TEXT_Buffer[3]=(u16*)timestamp;	
	
	
	STMFLASH_Write(WriteAddr,(u16*)TEXT_Buffer,SIZE);	//�������ݵ�Flash
	
	
}	

void Send_DATA_Flash(u32 WriteAddr)
{
	
	int last_timestr;
	int Last_Sensor_Data_Machine_ID;
	int Last_Sensor_Data_Pump;		    
	int Last_Sensor_Data_Water_Quantity;
	   			    
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	

	STMFLASH_Read(WriteAddr,(u16*)Read_Buffer,SIZE);

	Last_Sensor_Data_Machine_ID=	  (int)Read_Buffer[0];
	Last_Sensor_Data_Pump=		      (int) Read_Buffer[1];
	Last_Sensor_Data_Water_Quantity= (int)Read_Buffer[2];
	last_timestr=				      (int)Read_Buffer[3];
	
	printf("------------------%d\r\n",Last_Sensor_Data_Machine_ID);
	printf("------------------%d\r\n",Last_Sensor_Data_Pump);
	printf("------------------%d\r\n",Last_Sensor_Data_Water_Quantity);
	printf("------------------%d\r\n",last_timestr);
	
	
	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"User_ID\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Last_Sensor_Data_Machine_ID,Last_Sensor_Data_Pump,last_timestr);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Quantity\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}", Last_Sensor_Data_Water_Quantity, Last_Sensor_Data_Pump,last_timestr);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));	
	
	
}







void wifi_app_play_TASK(Sensor_Data_t *Sensor_Data,Sensor_Data_t *Last_Sensor_Data)
{
	uint8_t i;
	int connect_state=0;
	printf("wifi_app_play_TASK Start\r\n");
	while(1)
	{  

    if(Sensor_Data->connect==0)  //������
		{
			
			wifi_connect(Sensor_Data); 
			if(Drink_over==1)
			{
				Drink_over=0;
				
				Sensor_Flash(Sensor_Data,SAVE_ADDR+connect_state*16);
				
				
				STMFLASH_Read(SAVE_ADDR+connect_state*16,(u16 *)Read_Buffer,SIZE); 
				Sensor_Data->Water_Quantity=0;
				
				printf("-----------------------------------Sensor_Data->Machine_ID��%d\r\n",Read_Buffer[0]); 
				printf("--------------------------------------Sensor_Data->User_ID��%d\r\n",Read_Buffer[1]);
				printf("-------------------------------Sensor_Data->Water_Quantity��%d\r\n",Read_Buffer[2]);
				printf("-------------------------------------------------timestamp��%d\r\n",Read_Buffer[3]);
				
			
				
				connect_state++;
				printf("---------------------------------------������һ����ˮ����connect_state:%d\r\n",connect_state);
				printf("---------------------------------------FLASH_SAVE_ADDR%d\r\n",SAVE_ADDR);
				
				printf("��������\r\n");
				
				
				
			}
			
			if(wificonnect==1)
			{
				wificonnect=0;
//				wifi_connect(Sensor_Data); 
			 //��������
				
		//ѡ�񱳾�����2��(0���ޱ�������  1-15���������ֿ�ѡ)
		//m[0~16]:0��������Ϊ������16���������������
		//v[0~16]:0�ʶ�����Ϊ������16�ʶ��������
		//t[0~5]:0�ʶ�����������5�ʶ��������
		//���������ù�����ο������ֲ�
//		SYN_FrameInfo(0, "[m0][v5][t5]ңң����");
		//��ͣ�ϳɣ���ʱû���õ�������չʾ�����÷�
		//YS_SYN_Set(SYN_SuspendCom);
		
			}
			
			if(connect_state)
			{
				for(int z=1;z<=connect_state;z++)
				{
				STMFLASH_Read(SAVE_ADDR+(z-1)*16,(u16 *)Read_Buffer,SIZE);  	
				printf("-----------------------------------�ڼ������ݣ�%d\r\n",z); 	
				printf("-----------------------------------Sensor_Data->Machine_ID��%d\r\n",Read_Buffer[0]); 
				printf("--------------------------------------Sensor_Data->User_ID��%d\r\n",Read_Buffer[1]);
				printf("-------------------------------Sensor_Data->Water_Quantity��%d\r\n",Read_Buffer[2]);
				printf("-------------------------------------------------timestamp��%d\r\n",Read_Buffer[3]);
				}
			
			}
			 
		}
			
		if(Sensor_Data->connect==1)//����ܾã�����һ������
		{	
			
			
			if(F_USART3_RX_FINISH)
			{					
				ESP8266_GetIpData((uint8_t *)AT_RX_BUF, (char *)IpData);  //�Ӵ���3��ȡ��ƽ̨��������
				printf("--------------------------------���ݶ�ȡ�ɹ���%s\r\n",AT_RX_BUF);
			
				USER_DataAnalysisProcess((char *)IpData,Sensor_Data);      //����ƽ̨�������ݽ��н���
				printf("--------------------------------���ݽ����ɹ�%s\r\n",IpData);
				F_USART3_RX_FINISH=0;
				memset(IpData, 0x00, 128); //��ջ��棬׼���´ν���
				ClrAtRxBuf();  //��մ��ڻ��棬׼���´ν���
			}
//			RestCheck(lock,(char *)"2022-10-11 14:10:26",Sensor_Data);
//			error_back(Sensor_Data);
			if(i++%5==0)
			{
			RestCheck(lock,(char *)"2022-10-11 14:10:26",Sensor_Data);
			ESP8266_SendSensor(lock, (char *)"2022-10-11 14:10:26",Sensor_Data);
				//Send_DATA_Flash(Last_Sensor_Data,FLASH_SAVE_ADDR)	;
			}	
			if(connect_state)
			{
				for(int z=1;z<=connect_state;z++)
				{
					printf("-----------------------------------------------------------��ʼ�ϴ���ʷ����\r\n");
					Send_DATA_Flash(SAVE_ADDR+(z-1)*16)	;
					
					printf("-------------------------------------------------------------�ϴ���һ����ʷ��ˮ����:%d\r\n",z);
				}
				
				connect_state=0;
			}
			
			if(Drink_over==1)
			{
				Drink_over=0;
				ESP8266_SendSensor(lock, (char *)"2022-10-11 14:10:26",Sensor_Data);	
				Sensor_Data->Water_Quantity=0;				
				printf("----------u------------F_USART3_RX_FINISH----------%d\r\n",F_USART3_RX_FINISH);
				printf("-----------u-----------USART3_RX_COUNT----------%d\r\n",USART3_RX_COUNT);
				printf("-------------------------------------------------------------�����ϴ��ɹ�\r\n");
				printf("-------------------------------------------------------------�ϴ���һ����ˮ����\r\n");
				
			}
			
						
			if(i++%5==0)
			{
			
				u2_printf("Machine_ID:%d\r\n",Sensor_Data->Machine_ID);  //�������
				u2_printf("User_ID:%s\r\n",Sensor_Data->User_ID);
				u2_printf("Machine_State:%d\r\n",Sensor_Data->Machine_State);
				u2_printf("Water_Quantity:%d\r\n",Sensor_Data->Water_Quantity);
				u2_printf("V_Normal:%d\r\n",Sensor_Data->V_Normal);
				u2_printf("V_Source:%d\r\n",Sensor_Data->V_Source);
				
				u2_printf("Temp_Normal:%d\r\n",Sensor_Data->Temp_Normal);
				u2_printf("Temp_Hot:%d\r\n",Sensor_Data->Temp_Hot);
				u2_printf("Temp_SetHot:%d\r\n",Sensor_Data->Temp_SetHot);
				u2_printf("TDS_Normal:%d\r\n",Sensor_Data->TDS_Normal);
				u2_printf("TDS_Source:%d\r\n",Sensor_Data->TDS_Source);
				
				u2_printf("Water_Hight:%d\r\n",Sensor_Data->Water_Hight);
				u2_printf("Water_Low:%d\r\n",Sensor_Data->Water_Low);
				u2_printf("WaringAlarm:%d\r\n",Sensor_Data->WaringAlarm);
				u2_printf("ErrorAlarm:%d\r\n",Sensor_Data->ErrorAlarm);
				u2_printf("connect:%d\r\n",Sensor_Data->connect);
				
				
			}
			
			
			
			ClrAtRxBuf();

		}
				
		vTaskDelay(500);
	}
}
