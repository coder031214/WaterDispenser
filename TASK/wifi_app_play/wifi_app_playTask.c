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
#include <string.h>	//内存操作相关函数库
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
// 定义全局变量
 int  timestamp = 1694575776; // 假设时间戳为1631350215
// 定义变量用于存储当前时间
 int hours = 0;
 int minutes = 0;
 int seconds = 0;

// 函数声明
//void convert_timestamp_to_time(void);
void TIM1_Int_Init(u16 arr,u16 psc);
void wifi_connect(Sensor_Data_t *Sensor_Data);

//	u8 datatemp[SIZE];//读取Flash存放数据的地方
//寄存器及变量
uint8_t UART_RX_BUF[256];//接收到的串口数据缓存区
uint8_t UART_TX_BUF[256];//接收到的串口数据缓存区

uint8_t lock=1;

/*******************************************************************
*函数：char *USER_GetJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串转换为数值
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值的字符串形式的启始地址
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
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
		//printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
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
	//printf("数值=%s\r\n",temp);
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
            return true;  // 子串匹配成功
        }
    }
    
    return false;  // 子串匹配失败
}




uint32_t TimeCount=0;
uint8_t IpData[128];
/////////////////////////////
//wifi初始化
//wifi连接
//////////////////////////////
void wifi_app_init(Sensor_Data_t *Sensor_Data)
{
	
	//printf("----------------------USART3_RX_COUNT%d\r\n",USART3_RX_COUNT);
	USART3_Init(115200);

	TIM7_Int_Init(7200-1,9);		//10ms

	wifi_connect(Sensor_Data);	  //在wifi_app_play中进行
	
	memset(IpData, 0x00, 128);  //清零IpData
	TIM1_Int_Init(10000-1,7200-1);	
	TIM_Cmd(TIM1,ENABLE);//开启定时器1
}


void ESP8266_Res(void)
{
		//ESP8266复位
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
	
	
	//连接服务器
	for(TryCount=0; TryCount<3; TryCount++)
	{
		temp=ConnectToServer((char *)MY_DEVICE_ID, (char *)MA_SECRET_KEY);		//return = 0 ,sucess
		                                                                      //return < 0 ,error  
		
		if(temp != 0)  //失败
		{	printf("TCP连接失败\r\n");
			for(i=0;i<temp;i++)//根据绿灯闪烁次数判断发生了何种问题
			{
				BEEP=0;
				delay_ms(200);
				BEEP=1;
				delay_ms(200);
			} 
		}
		else  //网络连接成功
		{
			for(i=0;i<3;i++)
			{
				BEEP=1;
				delay_ms(200);
				BEEP=0;
				delay_ms(200);
				Sensor_Data->connect=1;
				printf("网络连接成功\r\n");
				
	
				printf("进入复位状态\r\n");
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
    // 计算小时
    hours = timestamp / 3600%24+8	;

    // 计算分钟
    minutes = (timestamp % 3600) / 60;

    // 计算秒钟
    seconds = (timestamp % 3600) % 60;
	
	
}


void TIM1_UP_IRQHandler(void) {
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) 
	{
        // 在此处添加您希望在定时中断中执行的代码
        timestamp++;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update); // 清除中断标志位
    }
}

void TIM1_Int_Init(u16 arr,u16 psc)
{	
	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	
}







/*******************************************************************
*函数：void USER_DataAnalysisProcess(char *RxBuf,Sensor_Data_t *Sensor_Data)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf,Sensor_Data_t *Sensor_Data)
{
	printf("-------------------------1\r\n");
	//char *cmdid = NULL;
	uint8_t TxetBuf[128];
	if(substringMatch((const char *)RxBuf, (const char *)"\"tag\":\"Machine_State\"")==true)//开锁/关锁请求
	{												    //"\"tag\":\"Machine_State\"""\"data\":\"1\""
		printf("-------------------------2\r\n");
		memset(TxetBuf,0x00,128);//清空缓存
		if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"1\"") ==true))//开锁
		{													 //"\"data\":\"1\""
			  
			printf("云端开锁命令！\r\n");
			Sensor_Data->Machine_State=2;
			beep_play(2);  //蜂鸣器响两次，代表开锁		"tag:Machine_State""data:1"


		}
		else if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"0\"") == true))//关锁
		{
			printf("云端关锁命令！\r\n");
			Sensor_Data->Machine_State=3;
			beep_play(1);  //蜂鸣器响一次，代表关锁
			
		}			
	}
	if(substringMatch((const char *)RxBuf, (const char *)"time")==true)//开锁/关锁请求
	{
		
		
		char original_str[50] ;
		strcpy(original_str, RxBuf);
		int start_pos =8;
		int end_pos = 19;
		int length = end_pos - start_pos;
		char result[length + 1];
		strncpy(result, original_str + start_pos, length);
		result[length] = '\0';
		printf("时间戳是——————————————————————%s\r\n",result);
		timestamp=(int) strtol(result, NULL, 10);
		printf("时间戳是——————————————————————%d\r\n",timestamp);	


	}
	if(substringMatch((const char *)RxBuf, (const char *)"\"tag\":\"LED\"")==true)//开锁/关锁请求
	{
		
		printf("-------------------------3\r\n");
		printf("tag:LEDr\n");
		memset(TxetBuf,0x00,128);//清空缓存
		if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"0\"") ==true))//开锁
		{
			  
			printf("云端开灯！\r\n");
			Sensor_Data->Light_Heat=0; 

		}
		else if((substringMatch((const char *)RxBuf, (const char *)"\"data\":\"1\"") == true))//关锁
		{
			printf("云端关灯！\r\n");
			Sensor_Data->Light_Heat=1;
			
		}			
	}
	
	if(substringMatch((const char *)RxBuf, (const char *)"Temp_SetHot")==true)//开锁/关锁请求
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
		printf("Temp_SetHot——————————————————————%d\r\n",S);	
	}
	
}


void Sensor_Flash(Sensor_Data_t *Sensor_Data,u32 WriteAddr)
{
	TEXT_Buffer[0]=(u16*)Sensor_Data->Machine_ID; 
	TEXT_Buffer[1]=(u16*)Sensor_Data->User_ID;
	TEXT_Buffer[2]=(u16*)Sensor_Data->Water_Quantity;	
	TEXT_Buffer[3]=(u16*)timestamp;	
	
	
	STMFLASH_Write(WriteAddr,(u16*)TEXT_Buffer,SIZE);	//发送数据到Flash
	
	
}	

void Send_DATA_Flash(u32 WriteAddr)
{
	
	int last_timestr;
	int Last_Sensor_Data_Machine_ID;
	int Last_Sensor_Data_Pump;		    
	int Last_Sensor_Data_Water_Quantity;
	   			    
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	

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

    if(Sensor_Data->connect==0)  //掉线了
		{
			
			wifi_connect(Sensor_Data); 
			if(Drink_over==1)
			{
				Drink_over=0;
				
				Sensor_Flash(Sensor_Data,SAVE_ADDR+connect_state*16);
				
				
				STMFLASH_Read(SAVE_ADDR+connect_state*16,(u16 *)Read_Buffer,SIZE); 
				Sensor_Data->Water_Quantity=0;
				
				printf("-----------------------------------Sensor_Data->Machine_ID：%d\r\n",Read_Buffer[0]); 
				printf("--------------------------------------Sensor_Data->User_ID：%d\r\n",Read_Buffer[1]);
				printf("-------------------------------Sensor_Data->Water_Quantity：%d\r\n",Read_Buffer[2]);
				printf("-------------------------------------------------timestamp：%d\r\n",Read_Buffer[3]);
				
			
				
				connect_state++;
				printf("---------------------------------------保存了一次饮水数据connect_state:%d\r\n",connect_state);
				printf("---------------------------------------FLASH_SAVE_ADDR%d\r\n",SAVE_ADDR);
				
				printf("掉线重连\r\n");
				
				
				
			}
			
			if(wificonnect==1)
			{
				wificonnect=0;
//				wifi_connect(Sensor_Data); 
			 //重新连接
				
		//选择背景音乐2。(0：无背景音乐  1-15：背景音乐可选)
		//m[0~16]:0背景音乐为静音，16背景音乐音量最大
		//v[0~16]:0朗读音量为静音，16朗读音量最大
		//t[0~5]:0朗读语速最慢，5朗读语速最快
		//其他不常用功能请参考数据手册
//		SYN_FrameInfo(0, "[m0][v5][t5]遥遥领先");
		//暂停合成，此时没有用到，用于展示函数用法
		//YS_SYN_Set(SYN_SuspendCom);
		
			}
			
			if(connect_state)
			{
				for(int z=1;z<=connect_state;z++)
				{
				STMFLASH_Read(SAVE_ADDR+(z-1)*16,(u16 *)Read_Buffer,SIZE);  	
				printf("-----------------------------------第几个数据：%d\r\n",z); 	
				printf("-----------------------------------Sensor_Data->Machine_ID：%d\r\n",Read_Buffer[0]); 
				printf("--------------------------------------Sensor_Data->User_ID：%d\r\n",Read_Buffer[1]);
				printf("-------------------------------Sensor_Data->Water_Quantity：%d\r\n",Read_Buffer[2]);
				printf("-------------------------------------------------timestamp：%d\r\n",Read_Buffer[3]);
				}
			
			}
			 
		}
			
		if(Sensor_Data->connect==1)//隔间很久，发送一次数据
		{	
			
			
			if(F_USART3_RX_FINISH)
			{					
				ESP8266_GetIpData((uint8_t *)AT_RX_BUF, (char *)IpData);  //从串口3获取云平台来的数据
				printf("--------------------------------数据读取成功：%s\r\n",AT_RX_BUF);
			
				USER_DataAnalysisProcess((char *)IpData,Sensor_Data);      //对云平台来的数据进行解析
				printf("--------------------------------数据解析成功%s\r\n",IpData);
				F_USART3_RX_FINISH=0;
				memset(IpData, 0x00, 128); //清空缓存，准备下次解析
				ClrAtRxBuf();  //清空串口缓存，准备下次接收
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
					printf("-----------------------------------------------------------开始上传历史数据\r\n");
					Send_DATA_Flash(SAVE_ADDR+(z-1)*16)	;
					
					printf("-------------------------------------------------------------上传了一次历史饮水数据:%d\r\n",z);
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
				printf("-------------------------------------------------------------数据上传成功\r\n");
				printf("-------------------------------------------------------------上传了一次饮水数据\r\n");
				
			}
			
						
			if(i++%5==0)
			{
			
				u2_printf("Machine_ID:%d\r\n",Sensor_Data->Machine_ID);  //蓝牙输出
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
