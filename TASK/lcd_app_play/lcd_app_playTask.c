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
 

u8 lcd_data[12];			//存放LCD ID字符串
//extern Sensor_Data_t Last_Sensor_Data;
extern 	char usernumber[32] ;

extern int timestamp; // 假设时间戳为1631350215
// 定义变量用于存储当前时间
extern int hours ;
extern int minutes ;
extern int seconds ;

char timeStr[9];  // 用于存储时分秒的字符串，格式为"HH:MM:SS\0"



void LCD_Chinese_Init(void)
{
	u8 key;
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
//	f_mount(fs[0],"0:",1); 		//挂载SD卡   注释
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	while(font_init()) 			//检查字库
	{
		printf("需要更新字库!\r\n");
		//while(SD_Init());			//检SD卡
		key=update_font(20,110,16,"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			printf("字库更新失败！");
		} 		         
	}
	printf("已有字库！\r\n");
	
	 
}
void lcd_app_darw_main(void )
{	
	 
     POINT_COLOR=RED;
     printf("lcddev_id:%04X\r\n",lcddev.id);//将LCD ID打印到lcd_id数组。
     LCD_Clear(WHITE);	

	Show_Str(48,0,200,24,"饮水机",24,0);
			
	POINT_COLOR=BLACK;
	
	
	//Show_Str(150,16*2,200,16,"00:00:00",16,0);//时间
	
	Show_Str(0,16*2,200,16,"饮水机ID：",16,0);//本车ID
	
	Show_Str(0,16*3	,200,16,"用户ID：",16,0);//用户ID
	
	Show_Str(0,16*4	,200,16,"运行状态：",16,0);//运行状态
	
	Show_Str(0,16*5	,200,16,"设定水温：",16,0);//设定水温
	
	Show_Str(0,16*6,200,16,"水箱温度T1：",16,0);	//水箱温度T1
	
	Show_Str(0,16*7,200,16,"热水水温T2：",16,0);	//热水水温T2
	
	Show_Str(0,16*8	,200,16,"水泵：",16,0);//水泵
	
	Show_Str(0,16*9	,200,16,"水流量F：",16,0);//水流量F	
	
	Show_Str(0,16*10,200,16,"常温V1：",16,0);//常温水电压V1	
	
	Show_Str(0,16*11,200,16,"水源V2：",16,0);//水源电压V2	
	
	Show_Str(0,16*12,200,16,"水源TDS1：",16,0);	//水源水质TDS1
	
	Show_Str(0,16*13,200,16,"水箱TDS2：",16,0);	//水箱水质TDS2	

	Show_Str(0,16*14,200,16,"高水位：",16,0);//水箱水质TDS2
	
	Show_Str(0,16*15,200,16,"低水位：",16,0);//水箱水质TDS2
	
	Show_Str(0,16*16,200,16,"加热：",16,0);		//热水加热器P
	
	Show_Str(0,16*17,200,16,"常温：",16,0);//电磁阀S1
	
	Show_Str(0,16*18,200,16,"热水：",16,0);	//电磁阀S2	
	
	Show_Str(0,16*19,200,16,"网络：",16,0);//网络
	
	
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

//STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)TEXT_Buffer,SIZE);		//从指定地址开始写入指定长度的数据
//   		//从指定地址开始读出指定长度的数据

//printf("--------------------------------TEXT_Buffer = %s\r\n",TEXT_Buffer);	
//	

		
		
		
		
		
		
		
	while(1)
	{	
		
		 convert_timestamp_to_time();
		 Show_Str(165,16*2,200,16,":",16,0);//网络
		 Show_Str(185,16*2,200,16,":",16,0);//网络 
		 LCD_ShowxNum(150,16*2,hours,2,16,0x80);  //显示 数字
		 LCD_ShowxNum(170,16*2,minutes,2,16,0x80);  //显示 数字
		 LCD_ShowxNum(190,16*2,seconds,2,16,0x80);  //显示 数字
				//本车ID
	      LCD_ShowxNum(80,16*2,(u32)Sensor_Data->Machine_ID,4,16,0x00);  //显示 数字

			//用户ID
	      LCD_ShowString(80,16*3, 64, 16, 16, (unsigned char*)Sensor_Data->User_ID);  //显示
				
				//运行状态
	      LCD_ShowxNum(80,16*4,(u32)Sensor_Data->Machine_State,1,16,0x00);  //显示 数字	
											
				//设定水温
	      LCD_ShowxNum(90,16*5,Sensor_Data->Temp_SetHot,3,16,0x00);  //显示 数字
									
				//水箱温度T1
	      LCD_ShowxNum(90,16*6,(u32)Sensor_Data->Temp_Normal,2,16,0x00);  //显示 数字
				
				//热水水温T2
	      LCD_ShowxNum(90,16*7,(u32)Sensor_Data->Temp_Hot,2,16,0x00);  //显示 数字	

				//水泵				
	      LCD_ShowxNum(110,16*8,(u32)Sensor_Data->Pump,4,16,0x00);  //显示 数字
		  
				//水流量
		  LCD_ShowxNum(110,16*9,(u32)Sensor_Data->Water_Quantity,4,16,0x00);  //显示 数字设置

				//常温水电压V1
	      LCD_ShowxNum(80,16*10,(u32)Sensor_Data->V_Normal,4,16,0x00);  //显示 数字
				
				//水源电压V2
	      LCD_ShowxNum(80,16*11,(u32)Sensor_Data->V_Source,4,16,0x00);  //显示 数字
											
				//水源水质TDS1
	      LCD_ShowxNum(150,16*12,(u32)Sensor_Data->TDS_Source,4,16,0x00);  //显示 数字
				//水箱水质TDS2
	      LCD_ShowxNum(150,16*13,(u32)Sensor_Data->TDS_Normal,4,16,0x00);  //显示 数字
				
				//Show_Str(0,16*4	,200,16,"运行状态：",16,0);//运行状态
				
//				//热水加热器P
//				if(Sensor_Data->Machine_State==0)
//				Show_Str(80,16*4	,200,16,"运行",16,1);             //填充单色
//				else if(Sensor_Data->Machine_State==1)
//				Show_Str(80,16*4	,200,16,"运行",16,1);             //填充单色
//				else if(Sensor_Data->Machine_State==2)
//				Show_Str(80,16*4	,200,16,"暂停",16,1);
				
				//热水加热器P
				if(Sensor_Data->Water_Hight)
				LCD_Fill(80, 16*14, 96, 16*15, RED);                   //填充单色
				else
				LCD_Fill(80, 16*14, 96, 16*15, GREEN);                   //填充单色
				
				//电磁阀S1                 //黑色

				if(Sensor_Data->Water_Low)
				LCD_Fill(80, 16*15, 96, 16*16, RED);                   //填充单色
				else
				LCD_Fill(80, 16*15, 96, 16*16, GREEN);
						
				//热水加热器P
				if(!Sensor_Data->Light_Heat)
				LCD_Fill(80, 16*16, 96, 16*17, RED);                   //填充单色
				else
				LCD_Fill(80, 16*16, 96, 16*17, GREEN);                   //填充单色
				
				//电磁阀S1                 //常温

				if(!Sensor_Data->Light_Normal)
				LCD_Fill(80, 16*17, 96, 16*18, RED);                   //填充单色
				else
				LCD_Fill(80, 16*17, 96, 16*18, GREEN);
				 
				//电磁阀S2 				热水
				if(!Sensor_Data->Light_Hot)
				LCD_Fill(80, 16*18, 96, 16*19, RED);                   //填充单色
				else
				LCD_Fill(80, 16*18, 96, 16*19, GREEN);
				
   				
				//网络
				if(Sensor_Data->connect)
				LCD_Fill(80, 16*19, 96, 16*20, GREEN);                   //填充单色
				else
				LCD_Fill(80, 16*19, 96, 16*20, RED);                   //填充单色
        
				vTaskDelay(50);
				
	 }

}
