#include "FreeRTOS.h"
#include "task.h"
#include "hanzi.h"
#include "delay.h"
#include "lcd_app_playTask.h"
#include "lcd.h"
#include "Get_Sensors_DataTask.h"
#include "usart.h"
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "wifi_app_playTask.h"

#include "stmflash.h"
#include "delay.h"
#include "usart.h"
 

u8 lcd_data[12];			//���LCD ID�ַ���
//extern Sensor_Data_t Last_Sensor_Data;
extern 	char usernumber[32] ;

extern int timestamp; // ����ʱ���Ϊ1631350215
// ����������ڴ洢��ǰʱ��
extern int hours ;
extern int minutes ;
extern int seconds ;

char timeStr[9];  // ���ڴ洢ʱ������ַ�������ʽΪ"HH:MM:SS\0"



void LCD_Chinese_Init(void)
{
	u8 key;
	W25QXX_Init();				//��ʼ��W25Q128
 	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
//	f_mount(fs[0],"0:",1); 		//����SD��   ע��
 	f_mount(fs[1],"1:",1); 		//����FLASH.
	while(font_init()) 			//����ֿ�
	{
		printf("��Ҫ�����ֿ�!\r\n");
		//while(SD_Init());			//��SD��
		key=update_font(20,110,16,"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			printf("�ֿ����ʧ�ܣ�");
		} 		         
	}
	printf("�����ֿ⣡\r\n");
	
	 
}
void lcd_app_darw_main(void )
{	
	 
     POINT_COLOR=RED;
     printf("lcddev_id:%04X\r\n",lcddev.id);//��LCD ID��ӡ��lcd_id���顣
     LCD_Clear(WHITE);	

	Show_Str(48,0,200,24,"��ˮ��",24,0);
			
	POINT_COLOR=BLACK;
	
	
	//Show_Str(150,16*2,200,16,"00:00:00",16,0);//ʱ��
	
	Show_Str(0,16*2,200,16,"��ˮ��ID��",16,0);//����ID
	
	Show_Str(0,16*3	,200,16,"�û�ID��",16,0);//�û�ID
	
	Show_Str(0,16*4	,200,16,"����״̬��",16,0);//����״̬
	
	Show_Str(0,16*5	,200,16,"�趨ˮ�£�",16,0);//�趨ˮ��
	
	Show_Str(0,16*6,200,16,"ˮ���¶�T1��",16,0);	//ˮ���¶�T1
	
	Show_Str(0,16*7,200,16,"��ˮˮ��T2��",16,0);	//��ˮˮ��T2
	
	Show_Str(0,16*8	,200,16,"ˮ�ã�",16,0);//ˮ��
	
	Show_Str(0,16*9	,200,16,"ˮ����F��",16,0);//ˮ����F	
	
	Show_Str(0,16*10,200,16,"����V1��",16,0);//����ˮ��ѹV1	
	
	Show_Str(0,16*11,200,16,"ˮԴV2��",16,0);//ˮԴ��ѹV2	
	
	Show_Str(0,16*12,200,16,"ˮԴTDS1��",16,0);	//ˮԴˮ��TDS1
	
	Show_Str(0,16*13,200,16,"ˮ��TDS2��",16,0);	//ˮ��ˮ��TDS2	

	Show_Str(0,16*14,200,16,"��ˮλ��",16,0);//ˮ��ˮ��TDS2
	
	Show_Str(0,16*15,200,16,"��ˮλ��",16,0);//ˮ��ˮ��TDS2
	
	Show_Str(0,16*16,200,16,"���ȣ�",16,0);		//��ˮ������P
	
	Show_Str(0,16*17,200,16,"���£�",16,0);//��ŷ�S1
	
	Show_Str(0,16*18,200,16,"��ˮ��",16,0);	//��ŷ�S2	
	
	Show_Str(0,16*19,200,16,"���磺",16,0);//����
	
	
  }

#include "stm32f10x.h"



   
  

void lcd_app_play_TASK(Sensor_Data_t *Sensor_Data)
	{
		
		
		
		printf("lcd_app_play_TASK\r\n");
		
		
//		
////u32 readarr;
////u16 writearr;
////u16 test=666;
////uint8_t MID=88;
////uint16_t DID;
////writearr=66;
//		
//const u8 TEXT_Buffer[]={"test"};
//u8 datatamp[SIZE];
//STMFLASH_Read(FLASH_SAVE_ADDR,(u16 *)datatamp,SIZE);
//printf("--------------------------------test = %s\r\n",datatamp);

//STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)TEXT_Buffer,SIZE);		//��ָ����ַ��ʼд��ָ�����ȵ�����
//   		//��ָ����ַ��ʼ����ָ�����ȵ�����

//printf("--------------------------------TEXT_Buffer = %s\r\n",TEXT_Buffer);	
//	

		
		
		
		
		
		
		
	while(1)
	{	
		
		 convert_timestamp_to_time();
		 Show_Str(165,16*2,200,16,":",16,0);//����
		 Show_Str(185,16*2,200,16,":",16,0);//���� 
		 LCD_ShowxNum(150,16*2,hours,2,16,0x80);  //��ʾ ����
		 LCD_ShowxNum(170,16*2,minutes,2,16,0x80);  //��ʾ ����
		 LCD_ShowxNum(190,16*2,seconds,2,16,0x80);  //��ʾ ����
				//����ID
	      LCD_ShowxNum(80,16*2,(u32)Sensor_Data->Machine_ID,4,16,0x00);  //��ʾ ����

			//�û�ID
	      LCD_ShowString(80,16*3, 64, 16, 16, (unsigned char*)Sensor_Data->User_ID);  //��ʾ
				
				//����״̬
	      LCD_ShowxNum(80,16*4,(u32)Sensor_Data->Machine_State,1,16,0x00);  //��ʾ ����	
											
				//�趨ˮ��
	      LCD_ShowxNum(90,16*5,Sensor_Data->Temp_SetHot,3,16,0x00);  //��ʾ ����
									
				//ˮ���¶�T1
	      LCD_ShowxNum(90,16*6,(u32)Sensor_Data->Temp_Normal,2,16,0x00);  //��ʾ ����
				
				//��ˮˮ��T2
	      LCD_ShowxNum(90,16*7,(u32)Sensor_Data->Temp_Hot,2,16,0x00);  //��ʾ ����	

				//ˮ��				
	      LCD_ShowxNum(110,16*8,(u32)Sensor_Data->Pump,4,16,0x00);  //��ʾ ����
		  
				//ˮ����
		  LCD_ShowxNum(110,16*9,(u32)Sensor_Data->Water_Quantity,4,16,0x00);  //��ʾ ��������

				//����ˮ��ѹV1
	      LCD_ShowxNum(80,16*10,(u32)Sensor_Data->V_Normal,4,16,0x00);  //��ʾ ����
				
				//ˮԴ��ѹV2
	      LCD_ShowxNum(80,16*11,(u32)Sensor_Data->V_Source,4,16,0x00);  //��ʾ ����
											
				//ˮԴˮ��TDS1
	      LCD_ShowxNum(150,16*12,(u32)Sensor_Data->TDS_Source,4,16,0x00);  //��ʾ ����
				//ˮ��ˮ��TDS2
	      LCD_ShowxNum(150,16*13,(u32)Sensor_Data->TDS_Normal,4,16,0x00);  //��ʾ ����
				
				//Show_Str(0,16*4	,200,16,"����״̬��",16,0);//����״̬
				
//				//��ˮ������P
//				if(Sensor_Data->Machine_State==0)
//				Show_Str(80,16*4	,200,16,"����",16,1);             //��䵥ɫ
//				else if(Sensor_Data->Machine_State==1)
//				Show_Str(80,16*4	,200,16,"����",16,1);             //��䵥ɫ
//				else if(Sensor_Data->Machine_State==2)
//				Show_Str(80,16*4	,200,16,"��ͣ",16,1);
				
				//��ˮ������P
				if(Sensor_Data->Water_Hight)
				LCD_Fill(80, 16*14, 96, 16*15, RED);                   //��䵥ɫ
				else
				LCD_Fill(80, 16*14, 96, 16*15, GREEN);                   //��䵥ɫ
				
				//��ŷ�S1                 //��ɫ

				if(Sensor_Data->Water_Low)
				LCD_Fill(80, 16*15, 96, 16*16, RED);                   //��䵥ɫ
				else
				LCD_Fill(80, 16*15, 96, 16*16, GREEN);
						
				//��ˮ������P
				if(!Sensor_Data->Light_Heat)
				LCD_Fill(80, 16*16, 96, 16*17, RED);                   //��䵥ɫ
				else
				LCD_Fill(80, 16*16, 96, 16*17, GREEN);                   //��䵥ɫ
				
				//��ŷ�S1                 //����

				if(!Sensor_Data->Light_Normal)
				LCD_Fill(80, 16*17, 96, 16*18, RED);                   //��䵥ɫ
				else
				LCD_Fill(80, 16*17, 96, 16*18, GREEN);
				 
				//��ŷ�S2 				��ˮ
				if(!Sensor_Data->Light_Hot)
				LCD_Fill(80, 16*18, 96, 16*19, RED);                   //��䵥ɫ
				else
				LCD_Fill(80, 16*18, 96, 16*19, GREEN);
				
   				
				//����
				if(Sensor_Data->connect)
				LCD_Fill(80, 16*19, 96, 16*20, GREEN);                   //��䵥ɫ
				else
				LCD_Fill(80, 16*19, 96, 16*20, RED);                   //��䵥ɫ
        
				vTaskDelay(50);
				
	 }

}
