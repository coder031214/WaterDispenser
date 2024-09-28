	
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "beep.h"
#include "led.h"
#include "delay.h"
#include "WiFiToCloud.h"

#include <delay.h>

extern int timestamp;
/*******************************************************************
*������int8_t ESP8266_SetStation(void)
*���ܣ�ESP8266����Ϊstationģʽ
*���룺��
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SetStation(void)
{
	ClrAtRxBuf();//��ջ���
	SendAtCmd((uint8_t *)AT_CWMODE,strlen(AT_CWMODE));   //AT_CWMODE	"AT+CWMODE_CUR=1" //����Ϊ��station��ģʽ
	                                                     //ͨ������3����(��Ŀ�б�������д���usart4��)
	delay_ms(100);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL) //�ж��Ƿ����óɹ�   ʧ�ܷ���-1
	{		
		printf("ESP8266����STAģʽʧ��\r\n");
			return -1; 
	}
	return 0;                                                                              //�ɹ�����0
}

/*******************************************************************
*������int8_t ESP8266_SetAP(void)
*���ܣ�����ESP8266Ҫ���ӵ��ȵ����ƺ�����
*���룺char *wifi-�ȵ����� char *pwd-�ȵ�����
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd)
{
	uint8_t AtCwjap[MAX_AT_TX_LEN];
	memset(AtCwjap, 0x00, MAX_AT_TX_LEN);//��ջ���
	ClrAtRxBuf();//��ջ���
	sprintf((char *)AtCwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"",wifi, pwd);//AT+CWJAP_CUR=\"ZZ\",\"moneyle0803\"
//	printf("%s\r\n",AtCwjap);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)AtCwjap,strlen((const char *)AtCwjap));
	delay_ms(1500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		printf("ESP8266�����ȵ����ƺ�����ʧ��\r\n");
		return -1;
	}
	return 0;
}

/*******************************************************************
*������int8_t ESP8266_SetStation(void)
*���ܣ�ESP8266����TCP����
*���룺
	char *IpAddr-IP��ַ�����磺192.168.0.102
	uint16_t port-�˿ںţ�ȡֵ8080
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port)
{
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//��ջ���
	ClrAtRxBuf();//��ջ���
	sprintf((char *)IpStart,"AT+CIPSTART=\"TCP\",\"%s\",%d",IpAddr, port);
	//AT+CIPSTART=\"TCP\",\"120.77.58.34"\",8700
	//printf("%s\r\n",IpStart);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpStart,strlen((const char *)IpStart));
	delay_ms(1500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		return -1;
	}
	return 0;
}



/*******************************************************************
*������int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*���ܣ�ESP8266��������
*���룺
		char *IpBuf-IP����
		uint8_t len-���ݳ���
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t IpSend[MAX_AT_TX_LEN];
	memset(IpSend, 0x00, MAX_AT_TX_LEN);//��ջ���
	//ClrAtRxBuf();//��ջ���
	sprintf((char *)IpSend,"AT+CIPSEND=%d",len);
	
	SendAtCmd((uint8_t *)IpSend,strlen((const char *)IpSend));
	delay_ms(3);
	//printf("��������%s\r\n",(const char *)AT_RX_BUF);
	
	
	for(TryGo = 0; TryGo<60; TryGo++)//���ȴ�ʱ��100*60=6000ms
	{
			if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
			{
			
				error= -1;
				
			}
			else
			{
				ClrAtRxBuf();//��ջ���
				SendStrLen((uint8_t *)IpBuf, len);
				//printf("%s\r\n",IpBuf);////////////////////////////////////////////////////////////
				for(TryGo = 0; TryGo<60; TryGo++)//���ȴ�ʱ��100*60=6000ms
				{
					if(strstr((const char *)AT_RX_BUF, (const char *)"SEND OK") == NULL)
					{
						error = -2;
					}
					else
					{
						error = 0;
						break;
					}
					delay_ms(2);
				}
				break;
				
			}
			delay_ms(2);
	}

	return error;
}

/*******************************************************************
*������int8_t ConnectToServer(void)
*���ܣ����ӵ�������
*���룺��
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey)//DEVICE_ID "test21" SECRET_KEY "c9c54485d0d0499b8d4442aa31422743"
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	
	for(TryGo = 0; TryGo<3; TryGo++)        //*******����3������WiFiͨѶģ�鹤��ģʽ****************************  �ɹ�0 ʧ��-1
	{
		if(ESP8266_SetStation() == 0)//����WiFiͨѶģ�鹤��ģʽ
		{
			printf("ESP8266����STAģʽ�ɹ�\r\n");
			//BEEP=1;
//			delay_ms(1000);
			//BEEP=0;
			error = 0;
			break;
		}
		else
		{
			error = -1;
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	
	for(TryGo = 0; TryGo<6; TryGo++)////////////////////////***************����3�������ȵ����ƺ�����************************************ �ɹ�0 ʧ��-2 
	{
		if(ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD) == 0)//�����ȵ����ƺ�����
		{//���óɹ�
			printf("ESP8266�����ȵ����ƺ�����ɹ�\r\n");
			error = 0;
			LED0=0;//LED0��6��
			delay_ms(100);
			LED0=1;
			break;
		}
		else
		{//ʧ��
			error = -2;
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	
	
	for(TryGo = 0; TryGo<3; TryGo++)///////////////***************����3������SERVER_IP��SERVER_PORT***********************************   �ɹ�0 ʧ��-3
	{
		if(ESP8266_IpStart((char *)SERVER_IP,SERVER_PORT) == 0)//SERVER_IP	"120.77.58.34" SERVER_PORT	8700		
		{
			printf("����SERVER_IP��SERVER_PORT��TCP���ɹ�\r\n");
			LED1=0;
			delay_ms(100);
			LED1=1;
			error = 0;
			break;
		}
		else
		{
			error = -3;
			printf("����SERVER_IP��SERVER_PORT��TCP��ʧ��\r\n");
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	/////////////////////////////////////////////////////////////////////*******************����50�����ӷ������豸**********************�ɹ�0 ʧ��-5
	//                                           test21          c9c54485d0d0499b8d4442aa31422743
	sprintf((char *)TxetBuf,"{\"t\":1,\"tag\":\"%s\",\"key\":\"%s\",\"ver\":\"v0.0.0.0\"}",DeviceID,SecretKey);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	

	printf("����DeviceID,SecretKey\r\n");
//	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)              //�����豸��Ϣ�� �ɹ�0 ʧ��-4
//	{//����ʧ��
//		error=-4;
//		printf("�����豸��Կʧ��\r\n");
//	}
//	else
		printf("�����豸��Կ�ɹ�\r\n");
//	{//���ͳɹ�
//		for(TryGo = 0; TryGo<50; TryGo++)//���ȴ�ʱ��50*10=500ms
//		{
//			
//			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//�����Ӧ״̬�Ƿ�Ϊ���ֳɹ�
//			{
//				error = -5;
//				 printf("����ʧ��\r\n");
//				printf("AT_RX_BUF: %s\r\n", (const char *)AT_RX_BUF);
//			}
//			else
//			{
//				error = 0;
//			  printf("���ֳɹ�\r\n");
//				printf("�����豸��Կ�ɹ�\r\n");
//			
//			}
//			delay_ms(10);
//		}
//	}
	
	return error;
}

/*******************************************************************
*������int8_t ESP8266_SendSensor(uint8_t sensor)
*���ܣ����ʹ������ݵ�������
*���룺
	uint8_t sensor, ������ֵ
	char *TimeStr���ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t ESP8266_SendSensor(uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data)
{
//	int w;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"Machine_ID\":{\"%s\":%d}},\"msgid\":001}",TimeStr,Sensor_Data->Machine_ID);
//	w=ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	printf("------------ESP8266_IpSend---------------:  %d\r\n",w);
//	if(w < 0)
//	{
//		
//		int i=0;
//		
//		delay_ms(10);
//		if(w==-1)
//		{
//			
//		Sensor_Data->connect=0;  //û�л�Ӧ��˵��wifi�����Ѿ��Ͽ�		
//		for(i=0;i<3;i++)
//			{
//				BEEP=1;
//				delay_ms(200);
//				BEEP=0;
//				delay_ms(200);
//				printf("�����ѶϿ�\r\n");
//			}
//		}
//	}
	printf("�ϱ��豸ID�ɹ� ��ʼ���ʹ������豸����\r\n");
	///////////////////
	//�����ϴ�ģʽ
	/////////////////////
	
//	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"User_ID\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->User_ID,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Pump\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->Pump,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//			
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Machine_State\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->Machine_State,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	
//	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Quantity\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}", Sensor_Data->Water_Quantity, Sensor_Data->User_ID,timestamp);

//	
////	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Quantity\",\"value\":%d,\"UserName\":\"%d\",\"device_time\":%d}", Sensor_Data->Water_Quantity, Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"V_Normal\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->V_Normal,Sensor_Data->User_ID,timestamp,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"V_Source\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->V_Source,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"TDS_Source\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->TDS_Source,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Temp_Normal\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->Temp_Normal,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Temp_Hot\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->Temp_Hot,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//		
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Temp_SetHot\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->Temp_SetHot,Sensor_Data->User_ID,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	
//	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"TDS_Normal\",\"value\":\"%d\",\"UserName\":\"%d\",\"device_time\":%d}",Sensor_Data->TDS_Normal,timestamp,timestamp);
//	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));



	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"User_ID\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->User_ID,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Pump\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Pump,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
			
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Machine_State\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Machine_State,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Quantity\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}", Sensor_Data->Water_Quantity, Sensor_Data->User_ID,timestamp);

	
//	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Quantity\",\"value\":%d,\"UserName\":\"%d\",\"created\":\"%d\"}", Sensor_Data->Water_Quantity, Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"V_Normal\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->V_Normal,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"V_Source\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->V_Source,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"TDS_Source\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->TDS_Source,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Temp_Normal\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Temp_Normal,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Temp_Hot\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Temp_Hot,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
		
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Temp_SetHot\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Temp_SetHot,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"TDS_Normal\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->TDS_Normal,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	 
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Hight\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Water_Hight,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"Water_Low\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->Water_Low,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

	
	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"WaringAlarm\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->WaringAlarm,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

	 sprintf((char *)TxetBuf,"{\"sensor_tag\":\"ErrorAlarm\",\"value\":\"%d\",\"UserName\":\"%d\",\"Created\":\"%d\"}",Sensor_Data->ErrorAlarm,Sensor_Data->User_ID,timestamp);
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));

//	sprintf((char *)TxetBuf,"{\"sensor_tag\":\"WaringAlarm\",\"value\":\"%d\",\"msgid\":\"001\"}",Sensor_Data->WaringAlarm);
//   ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//		

//		sprintf((char *)TxetBuf,"{\"sensor_tag\":\"ErrorAlarm\",\"value\":\"%d\",\"msgid\":\"001\"}",Sensor_Data->ErrorAlarm);
//		ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
		
	
	printf("�ϴ���һ������\r\n");
	return error;
}

void error_back(Sensor_Data_t *Sensor_Data)
{
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//
	if(Sensor_Data->WaringAlarm==1)
	{
		sprintf((char *)TxetBuf,"{\"sensor_tag\":\"WaringAlarm\",\"value\":\"%d\",\"msgid\":\"001\"}",Sensor_Data->WaringAlarm);
		ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
		printf("-------------------------------------------------------------WaringAlarm==1�ϴ��ɹ�\r\n");
	}

	if(Sensor_Data->ErrorAlarm==1)
	{
		sprintf((char *)TxetBuf,"{\"sensor_tag\":\"ErrorAlarm\",\"value\":\"%d\",\"msgid\":\"001\"}",Sensor_Data->ErrorAlarm);
		ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
		
		printf("-------------------------------------------------------------ErrorAlarm==1�ϴ��ɹ�\r\n");
	}
}






//int8_t RestCheck(uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data)
//{
//	int w;
//	
//	uint8_t TxetBuf[MAX_AT_TX_LEN];
//	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���            uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data
//	
//	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"Machine_ID\":{\"%s\":%d}},\"msgid\":001}","Machine rest OK!");
//	w=ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
//	printf("------------ESP8266_IpSend---------------:  %d\r\n",w);
//	if(w < 0)
//	{
//		
//		int i=0;
//		
//		delay_ms(10);
//		if(w==-1)
//		{
//			
//		Sensor_Data->connect=0;  //û�л�Ӧ��˵��wifi�����Ѿ��Ͽ�		
//		for(i=0;i<3;i++)
//			{
//				BEEP=1;
//				delay_ms(200);
//				BEEP=0;
//				delay_ms(200);
//				printf("�����ѶϿ�\r\n");
//			}
//		}
//	}
//	else printf("�豸����\r\n");
//	return 0;
//}


int8_t RestCheck(uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data)
{
	int i;
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//��ջ���
	ClrAtRxBuf();//��ջ���
	sprintf((char *)IpStart,"AT+CIPSTATUS");
	//AT+CIPSTART=\"TCP\",\"120.77.58.34"\",8700
	//printf("%s\r\n",IpStart);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)IpStart,strlen((const char *)IpStart));
	delay_ms(50);
	if(strstr((const char *)AT_RX_BUF, (const char *)"STATUS:") != NULL)
	{
		if(strstr((const char *)AT_RX_BUF, (const char *)"3") == NULL)
		{
		printf("------------------AT_RX_BUF=%s\r\n",AT_RX_BUF);
		Sensor_Data->connect=0;  //û�л�Ӧ��˵��wifi�����Ѿ��Ͽ�		
		for(i=0;i<3;i++)
			{
				BEEP=1;
				delay_ms(200);
				BEEP=0;
				delay_ms(200);
				printf("�����ѶϿ�\r\n");
			}
		}
	}
	return 0;
}

/*******************************************************************
*������uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
*���ܣ���������������
*���룺
		uint8_t *AtRxBuf ��ԭʼAT���ڻ���
		char *GetIpData ����ȡ������Itcp/ip����
*����������յ���IP���ݳ���
*����˵����
	AT+CIPSEND=76
	{"t":3,"datatype":2,"datas":{"alarm":{"2018-06-19 18:15:02":0}},"msgid":001}
	+IPD,29:{"msgid":1,"status":0,"t":4}
*******************************************************************/
uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
{
	char *Point = NULL;
	uint8_t len = 0;
	printf("��ʼ�������ݱ�\r\n");
	Point = strstr((const char *)AtRxBuf, (const char *)"+IPD,");
	if(Point!=NULL)//���ģ���յ�TCP/IP���ݰ���
	{
		printf("�������ݱ�\r\n");
		
		len=(Point[5]-'0')*10+Point[6]-'0';//����IP���ݳ��Ȳ��ᳬ��100���ʿ��԰�����λ���͡�ʮλ�������յ���IP���ݳ���
		Point = strstr((const char *)AtRxBuf, (const char *)":")+1;
		memcpy(GetIpData, Point, len);
		GetIpData[len]='\0';
		printf("�յ�IP����----------------------------------------------------------------��%s\r\n",GetIpData);///////////////////////////////////////////////////////
		printf("�յ�IP���ݳ���=%d\r\n",len);///////////////////////////////////////////////////
	}
	
	return (len);
}

/*******************************************************************
*������void ESP8266_DataAnalysisProcess(char *RxBuf)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�����
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf)
{
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//��������
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//��Ӧ����
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//��������
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"ctrl\"") != NULL)//��������
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//����
			{
				ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//��Ӧ����
			}
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"defense\"") != NULL)//����/��������
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//����
			{
				printf("������\r\n");
				;//...
				;//...
				;//...
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//����
			{
				printf("������\r\n");
				;//...
				;//...
				;//...
			}
		}
	}
}
