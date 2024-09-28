#include "FreeRTOS.h"
#include "task.h"
#include "Get_Sensors_DataTask.h"
#include "dht11.h"
#include "adc.h"
#include "rc522.h" 
#include "lcd.h"
#include "lcd_app_playTask.h"
#include "TIM3_CAP.h"
#include "led.h"
#include "String.h"	
#include "usart.h"	
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stmflash.h"
#include "text.h"
#include "CountSensor.h"
#include "Encoder.h"


//����������
//  Sensor_Data_t Sensor_Data;
//  Sensor_Data_t Last_Sensor_Data;
  int start=1;
  char RFID_status;
  uint8_t RxBuffer[4];
 

u16 *buffer[1]; // ����һ������Ϊ1�����黺����
u16 *buffer2[1];

//void Sensor_Flash(Sensor_Data_t *Last_Sensor_Data,Sensor_Data_t *Sensor_Data)
//{
//	Last_Sensor_Data->Machine_ID=Sensor_Data->Machine_ID;
//	Last_Sensor_Data->Pump=Sensor_Data->Pump;
//	
//	Last_Sensor_Data->Machine_State=Sensor_Data->Machine_State;
//	Last_Sensor_Data->Water_Quantity=Sensor_Data->Water_Quantity;
//	
//	
//	Last_Sensor_Data->V_Normal=Sensor_Data->V_Normal;
//	Last_Sensor_Data->V_Source=Sensor_Data->V_Source;
//	Last_Sensor_Data->TDS_Source=Sensor_Data->TDS_Source;
//	Last_Sensor_Data->Temp_Normal=Sensor_Data->Temp_Normal;
//	Last_Sensor_Data->Temp_Hot=Sensor_Data->Temp_Hot;
//	Last_Sensor_Data->TDS_Normal=Sensor_Data->TDS_Normal;
//	Last_Sensor_Data->Light_Heat=Sensor_Data->Light_Heat;
//	Last_Sensor_Data->Light_Normal=Sensor_Data->Light_Normal;
//	Last_Sensor_Data->Light_Hot=Sensor_Data->Light_Hot;
//	Last_Sensor_Data->connect=Sensor_Data->connect;	
//}	

//void Send_DATA_Flash(Sensor_Data_t *Last_Sensor_Data)
//{
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Machine_ID);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Pump);

//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Machine_State);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Water_Quantity);

//	
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->V_Normal);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->V_Source);
//	sprintf((char *)TEXT_Buffer,"%.1f",Last_Sensor_Data->TDS_Source);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Temp_Normal);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Temp_Hot);
//	sprintf((char *)TEXT_Buffer,"%.1f",Last_Sensor_Data->TDS_Normal);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Light_Heat);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Light_Normal);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->Light_Hot);
//	sprintf((char *)TEXT_Buffer,"%d",Last_Sensor_Data->connect);

//	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)buffer,SIZE);	//�������ݵ�Flash
//}

//TEXT_Buffer=buffer;
//STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)TEXT_Buffer,2);
void sensor_app_init(void)
{
	 	
	adc_gpio_init();//ADC_IO��ʼ��
	adc_init();  //ADC��ʼ��
	
//	while(DHT11_Init())	//DHT11��ʼ��	//DHT11��ʼ��
//	{		
//		printf("DHT11_INIT error \r\n"); 
//	}
//	while(DHT11_Init2())	//DHT11��ʼ��	//DHT11��ʼ��
//	{		
//		printf("DHT11_INIT2 error \r\n"); 
//	}			
	printf("DHT11_INIT2 success \r\n");
	DHT11_Init();
//	DHT11_Init2();
	
	
	CountSensor_Init();
	LightSensor_Init();
	
	Encoder_Init();

}

char Card_ID[4];  //����Ϊȫ�ֱ��������ھ�̬�����ſɶԵ�ַ������
void RFID_User_ID(void)  //RFID��Ʒ��Դ
{
	PCD_Request(PICC_REQALL, RxBuffer);
    RFID_status=PCD_Anticoll(RxBuffer);
    sprintf(Card_ID,"%x%x%x%x",RxBuffer[0],RxBuffer[1],RxBuffer[2],RxBuffer[3]);
    if(RFID_status==PCD_OK)   			//Ѱ��
	{
	  printf("ID=%s\r\n",Card_ID);
	}
}
	

extern u8  USART_RX_GPS[USART_REC_LEN];

void Get_Sensors_Data_TASK(Sensor_Data_t *Sensor_Data)
{	
	printf("Get_Sensors_Data_TASK Start\r\n");
	
//	STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)buffer,SIZE);
//	
	while(1)
 {	
	

	u8 Temp_Normal,Temp_Hot,humi,humi2;


	DHT11_Read_Data(&Temp_Normal,&humi);
	DHT11_Read_Data(&Temp_Hot,&humi2);

	Sensor_Data->Machine_ID=1234;		//����ID��1234

	Sensor_Data->Temp_Hot=Temp_Hot+35;			//��
	Sensor_Data->Temp_Normal=Temp_Hot;	//����ˮ��	
	 
	//Sensor_Data->Pump=read_adc_value(0x01);		//ˮ��,PC1

	 ////Sensor_Data->Water_Quantity=read_adc_value(0x0A); //������ť��С��PC0
	 

	Sensor_Data->V_Normal=(read_adc_value(0x0A))/1000;
	Sensor_Data->V_Source=(read_adc_value(0x0A))/1000;
	 
	 
	Sensor_Data->TDS_Normal=110*(5-Sensor_Data->V_Normal)/Sensor_Data->V_Normal;
	Sensor_Data->TDS_Source=110*(5-Sensor_Data->V_Source)/Sensor_Data->V_Source;

	Sensor_Data->Water_Hight=!LightSensor_Get();
	Sensor_Data->Water_Low=CountSensor_Get();
	
	if(Sensor_Data->Light_Hot|Sensor_Data->Light_Normal==1)
	{
	Sensor_Data->Water_Quantity=50*Encoder_Get();
	}


	 
	  RC522_SPI_Init();//ˢ������io��ʼ��
	  PCD_Init();//ˢ�����ĵ�·��ʼ��
	  RFID_User_ID();  //��ȡ�û�ID
	  sprintf(Sensor_Data->User_ID,"%x%x%x%x",RxBuffer[0],RxBuffer[1],RxBuffer[2],RxBuffer[3]);
	  if(RFID_status==PCD_OK) //���½���ͻ������Ȩ����LCD
	  {
		LCD_Init();
		font_init();	  
		lcd_app_darw_main();
	  }
		
		vTaskDelay(50);

 }
}


