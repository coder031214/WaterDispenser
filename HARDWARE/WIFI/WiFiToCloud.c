	
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
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266设置为station模式
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetStation(void)
{
	ClrAtRxBuf();//清空缓存
	SendAtCmd((uint8_t *)AT_CWMODE,strlen(AT_CWMODE));   //AT_CWMODE	"AT+CWMODE_CUR=1" //设置为“station”模式
	                                                     //通过串口3发送(项目列表中名字写错成usart4了)
	delay_ms(100);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL) //判断是否设置成功   失败返回-1
	{		
		printf("ESP8266设置STA模式失败\r\n");
			return -1; 
	}
	return 0;                                                                              //成功返回0
}

/*******************************************************************
*函数：int8_t ESP8266_SetAP(void)
*功能：设置ESP8266要连接的热点名称和密码
*输入：char *wifi-热点名称 char *pwd-热点密码
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SetAP(char *wifi, char *pwd)
{
	uint8_t AtCwjap[MAX_AT_TX_LEN];
	memset(AtCwjap, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
	sprintf((char *)AtCwjap,"AT+CWJAP_CUR=\"%s\",\"%s\"",wifi, pwd);//AT+CWJAP_CUR=\"ZZ\",\"moneyle0803\"
//	printf("%s\r\n",AtCwjap);////////////////////////////////////////////////////////////
	SendAtCmd((uint8_t *)AtCwjap,strlen((const char *)AtCwjap));
	delay_ms(1500);
	if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
	{
		printf("ESP8266设置热点名称和密码失败\r\n");
		return -1;
	}
	return 0;
}

/*******************************************************************
*函数：int8_t ESP8266_SetStation(void)
*功能：ESP8266建立TCP连接
*输入：
	char *IpAddr-IP地址，例如：192.168.0.102
	uint16_t port-端口号，取值8080
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpStart(char *IpAddr, uint16_t port)
{
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
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
*函数：int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
*功能：ESP8266发送数据
*输入：
		char *IpBuf-IP数据
		uint8_t len-数据长度
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_IpSend(char *IpBuf, uint8_t len)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t IpSend[MAX_AT_TX_LEN];
	memset(IpSend, 0x00, MAX_AT_TX_LEN);//清空缓存
	//ClrAtRxBuf();//清空缓存
	sprintf((char *)IpSend,"AT+CIPSEND=%d",len);
	
	SendAtCmd((uint8_t *)IpSend,strlen((const char *)IpSend));
	delay_ms(3);
	//printf("返回数据%s\r\n",(const char *)AT_RX_BUF);
	
	
	for(TryGo = 0; TryGo<60; TryGo++)//最多等待时间100*60=6000ms
	{
			if(strstr((const char *)AT_RX_BUF, (const char *)"OK") == NULL)
			{
			
				error= -1;
				
			}
			else
			{
				ClrAtRxBuf();//清空缓存
				SendStrLen((uint8_t *)IpBuf, len);
				//printf("%s\r\n",IpBuf);////////////////////////////////////////////////////////////
				for(TryGo = 0; TryGo<60; TryGo++)//最多等待时间100*60=6000ms
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
*函数：int8_t ConnectToServer(void)
*功能：连接到服务器
*输入：无
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ConnectToServer(char *DeviceID, char *SecretKey)//DEVICE_ID "test21" SECRET_KEY "c9c54485d0d0499b8d4442aa31422743"
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	
	for(TryGo = 0; TryGo<3; TryGo++)        //*******尝试3次设置WiFi通讯模块工作模式****************************  成功0 失败-1
	{
		if(ESP8266_SetStation() == 0)//设置WiFi通讯模块工作模式
		{
			printf("ESP8266设置STA模式成功\r\n");
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
	
	
	for(TryGo = 0; TryGo<6; TryGo++)////////////////////////***************尝试3次设置热点名称和密码************************************ 成功0 失败-2 
	{
		if(ESP8266_SetAP((char *)WIFI_AP, (char *)WIFI_PWD) == 0)//设置热点名称和密码
		{//设置成功
			printf("ESP8266设置热点名称和密码成功\r\n");
			error = 0;
			LED0=0;//LED0亮6秒
			delay_ms(100);
			LED0=1;
			break;
		}
		else
		{//失败
			error = -2;
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	
	
	for(TryGo = 0; TryGo<3; TryGo++)///////////////***************尝试3次设置SERVER_IP和SERVER_PORT***********************************   成功0 失败-3
	{
		if(ESP8266_IpStart((char *)SERVER_IP,SERVER_PORT) == 0)//SERVER_IP	"120.77.58.34" SERVER_PORT	8700		
		{
			printf("设置SERVER_IP和SERVER_PORT（TCP）成功\r\n");
			LED1=0;
			delay_ms(100);
			LED1=1;
			error = 0;
			break;
		}
		else
		{
			error = -3;
			printf("设置SERVER_IP和SERVER_PORT（TCP）失败\r\n");
		}
	}
	if(error < 0)
	{
		return error;
	}
	
	/////////////////////////////////////////////////////////////////////*******************尝试50次连接服务器设备**********************成功0 失败-5
	//                                           test21          c9c54485d0d0499b8d4442aa31422743
	sprintf((char *)TxetBuf,"{\"t\":1,\"tag\":\"%s\",\"key\":\"%s\",\"ver\":\"v0.0.0.0\"}",DeviceID,SecretKey);
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
	

	printf("发送DeviceID,SecretKey\r\n");
//	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)              //发送设备信息， 成功0 失败-4
//	{//发送失败
//		error=-4;
//		printf("发送设备密钥失败\r\n");
//	}
//	else
		printf("发送设备密钥成功\r\n");
//	{//发送成功
//		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
//		{
//			
//			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为握手成功
//			{
//				error = -5;
//				 printf("握手失败\r\n");
//				printf("AT_RX_BUF: %s\r\n", (const char *)AT_RX_BUF);
//			}
//			else
//			{
//				error = 0;
//			  printf("握手成功\r\n");
//				printf("发送设备密钥成功\r\n");
//			
//			}
//			delay_ms(10);
//		}
//	}
	
	return error;
}

/*******************************************************************
*函数：int8_t ESP8266_SendSensor(uint8_t sensor)
*功能：发送传感数据到服务器
*输入：
	uint8_t sensor, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t ESP8266_SendSensor(uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data)
{
//	int w;
	int8_t error = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	
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
//		Sensor_Data->connect=0;  //没有回应，说明wifi连接已经断开		
//		for(i=0;i<3;i++)
//			{
//				BEEP=1;
//				delay_ms(200);
//				BEEP=0;
//				delay_ms(200);
//				printf("网络已断开\r\n");
//			}
//		}
//	}
	printf("上报设备ID成功 开始发送传感器设备数据\r\n");
	///////////////////
	//独立上传模式
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
		
	
	printf("上传了一次数据\r\n");
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
		printf("-------------------------------------------------------------WaringAlarm==1上传成功\r\n");
	}

	if(Sensor_Data->ErrorAlarm==1)
	{
		sprintf((char *)TxetBuf,"{\"sensor_tag\":\"ErrorAlarm\",\"value\":\"%d\",\"msgid\":\"001\"}",Sensor_Data->ErrorAlarm);
		ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf));
		
		printf("-------------------------------------------------------------ErrorAlarm==1上传成功\r\n");
	}
}






//int8_t RestCheck(uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data)
//{
//	int w;
//	
//	uint8_t TxetBuf[MAX_AT_TX_LEN];
//	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存            uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data
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
//		Sensor_Data->connect=0;  //没有回应，说明wifi连接已经断开		
//		for(i=0;i<3;i++)
//			{
//				BEEP=1;
//				delay_ms(200);
//				BEEP=0;
//				delay_ms(200);
//				printf("网络已断开\r\n");
//			}
//		}
//	}
//	else printf("设备在线\r\n");
//	return 0;
//}


int8_t RestCheck(uint8_t sensor, char *TimeStr,Sensor_Data_t *Sensor_Data)
{
	int i;
	uint8_t IpStart[MAX_AT_TX_LEN];
	memset(IpStart, 0x00, MAX_AT_TX_LEN);//清空缓存
	ClrAtRxBuf();//清空缓存
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
		Sensor_Data->connect=0;  //没有回应，说明wifi连接已经断开		
		for(i=0;i<3;i++)
			{
				BEEP=1;
				delay_ms(200);
				BEEP=0;
				delay_ms(200);
				printf("网络已断开\r\n");
			}
		}
	}
	return 0;
}

/*******************************************************************
*函数：uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
*功能：解析服务器数据
*输入：
		uint8_t *AtRxBuf ，原始AT串口缓存
		char *GetIpData ，截取出来的Itcp/ip数据
*输出：返回收到的IP数据长度
*特殊说明：
	AT+CIPSEND=76
	{"t":3,"datatype":2,"datas":{"alarm":{"2018-06-19 18:15:02":0}},"msgid":001}
	+IPD,29:{"msgid":1,"status":0,"t":4}
*******************************************************************/
uint8_t ESP8266_GetIpData(uint8_t *AtRxBuf, char *GetIpData)
{
	char *Point = NULL;
	uint8_t len = 0;
	printf("开始接收数据报\r\n");
	Point = strstr((const char *)AtRxBuf, (const char *)"+IPD,");
	if(Point!=NULL)//检查模块收到TCP/IP数据包？
	{
		printf("接收数据报\r\n");
		
		len=(Point[5]-'0')*10+Point[6]-'0';//这里IP数据长度不会超过100，故可以按“个位”和“十位”计算收到的IP数据长度
		Point = strstr((const char *)AtRxBuf, (const char *)":")+1;
		memcpy(GetIpData, Point, len);
		GetIpData[len]='\0';
		printf("收到IP数据----------------------------------------------------------------：%s\r\n",GetIpData);///////////////////////////////////////////////////////
		printf("收到IP数据长度=%d\r\n",len);///////////////////////////////////////////////////
	}
	
	return (len);
}

/*******************************************************************
*函数：void ESP8266_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void ESP8266_DataAnalysisProcess(char *RxBuf)
{
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//心跳请求？
	{
		ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//响应心跳
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//命令请求？
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"ctrl\"") != NULL)//开锁请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//开锁
			{
				ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP));//响应心跳
			}
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"defense\"") != NULL)//布防/撤防请求
		{
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//布防
			{
				printf("布防！\r\n");
				;//...
				;//...
				;//...
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//撤防
			{
				printf("撤防！\r\n");
				;//...
				;//...
				;//...
			}
		}
	}
}
