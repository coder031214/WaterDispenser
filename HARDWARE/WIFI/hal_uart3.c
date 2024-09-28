#include "usart4.h"
#include "hal_uart3.h"
#include "stm32f10x_rcc.h"


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
u32		USART3_RX_STA=0;
u8  USART3_RX_GPS[USART3_REC_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节 


bool substringMatch1(const char *text, const char *pattern) 
	{
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

/******************************************************************************
* Function Name --> 初始化IO 串口4
* Description   --> none
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART3_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#ifdef EN_USART3_RX	//如果使能了接收中断
	
	USART3->CR1 |= 1 << 8;	//PE中断使能
	USART3->CR1 |= 1 << 5;	//接收缓冲区非空中断使能
	
	MY_NVIC_Init(0,0, USART3_IRQn, NVIC_PriorityGroup_2);	//中断分组2  
	//诡异的量子平衡中，千万不要注释他，注释就出错！！！
	
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //串口3时钟使能

 	USART_DeInit(USART3);  //复位串口3
		 //USART3_TX   PB10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化PB10
   
    //USART3_RX	  PB11
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化PB11

	

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//设置波特率，一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(USART3, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
	//USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);//开启中断

	USART_Cmd(USART3, ENABLE);               
	
	
}

/******************************************************************************
* Function Name --> 串口4发送一个字节数据
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART3_SendData(uint16_t Data)
{
	while((USART3->SR & 0x40) == 0);	//等待发送完毕
	USART_SendData(USART3, Data);
}

/******************************************************************************
* Function Name --> 串口4发送一串数据
* Description   --> none
* Input         --> *Data:字符串	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART3_SendStr(uint8_t *Data)
{
	while(*Data!='\0')
	{
		USART3_SendData((uint16_t )(*Data));
		Data++;
	}
}

/******************************************************************************
* Function Name --> 串口4发送一串数据，指定数据长度
* Description   --> none
* Input         --> *Data:字符串，len长度	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void USART3_SendStrlen(uint8_t *Data, uint8_t len)//Data： AT+CIPSTART=\"TCP\",\"120.77.58.34"\",8700
{
	while(len--)
	{
		USART3_SendData((uint16_t )(*Data));
		Data++;
	}
}

//=========================================================
 
#ifdef EN_USART3_RX   //如果使能了接收

//=========================================================

//串口4中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误 

uint8_t USART3_RX_BUF[USART3_REC_LEN];	//接收缓冲,最大UART4_REC_LEN个字节

//接收状态
uint16_t USART3_RX_COUNT=0;	//接收到的有效字节数目，最大USART1_REC_LEN字节
uint8_t F_USART3_RX_FINISH=0;//接收状态标记,1:UART4接收完成，0：UART4接收未完成
uint8_t F_USART3_RX_RECEIVING=0;//串口接收正在进行中标记,1:进行中，0：暂停或结束
uint8_t USART3_RX_TIMEOUT_COUNT=0;	//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

/******************************************************************************
* Function Name --> 串口4接收中断服务程序
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/




void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!= RESET)  //接收到数据
	{

		if(substringMatch1((const char *)USART_ReceiveData(USART3),(const char *)"OK")==false|substringMatch1((const char *)USART_ReceiveData(USART3),(const char *)"+IPD,1:a")==false)
		{			
		USART3_RX_BUF[USART3_RX_COUNT] = USART_ReceiveData(USART3);	//读取接收到的数据
		USART3_RX_COUNT++;
		F_USART3_RX_RECEIVING=1;//正在接收串口数据
		//USART3_RX_TIMEOUT_COUNT=0;//超时计数清零		
		TIM_Cmd(TIM7,ENABLE);//开启定时器7	
		
		//USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
		}
	}
//	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
//		{
//			
//		F_USART3_RX_RECEIVING=0;
//		USART3_RX_TIMEOUT_COUNT=0;
//		F_USART3_RX_FINISH=1;

//		}			
	
}

	/*if((USART3_RX_BUF[USART3_RX_COUNT] == 0x0a) && (USART3_RX_BUF[USART3_RX_COUNT - 1] == 0x0d))  //接收满了
//		{
//				F_USART3_RX_RECEIVING=0;
//				USART3_RX_TIMEOUT_COUNT=0;
//				F_USART3_RX_FINISH=1;
//		}*/		
////		if(USART3_RX_COUNT>USART3_REC_LEN)//缓存区溢出
////		{USART3_RX_COUNT = 0x0000;}	
//		//end 接收未完成   		 		 
//	}	//end 接收到数据
//	
////	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
////		{
////			F_USART3_RX_FINISH=1;
////			USART_ClearITPendingBit(USART3,USART_IT_IDLE);
////			
//////			UART2_RX_BUF[UART2_RX_STA] = 0;
//////			UART2_RX_STA = 0;
//////			USART_ClearITPendingBit(USART2,USART_IT_IDLE);
//////			USART_ReceiveData(USART2); 
//////			rx = true;
////		}
//		
//}



//void USART3_IRQHandler(void)
//{
//	printf("---------------------------进入中断\r\n");
//	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收到数据
//	{
//		printf("-----------------------------------接收数据\r\n");
//		F_USART3_RX_RECEIVING=1;//正在接收串口数据
//		USART3_RX_TIMEOUT_COUNT=0;//超时计数清零
//		USART3_RX_BUF[USART3_RX_COUNT++] = USART_ReceiveData(USART3);	//读取接收到的数据
//		
//		/*if((USART3_RX_BUF[USART3_RX_COUNT] == 0x0a) && (USART3_RX_BUF[USART3_RX_COUNT - 1] == 0x0d))  //接收满了
//		{
//				F_USART3_RX_RECEIVING=0;
//				USART3_RX_TIMEOUT_COUNT=0;
//				F_USART3_RX_FINISH=1;
//		}*/
//		
//		
////		if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)
////		{
////			printf("----------------------------------------------接收完毕\r\n");
////		F_USART3_RX_RECEIVING=0;
////		USART3_RX_TIMEOUT_COUNT=0;
////		F_USART3_RX_FINISH=1;

////		}			
//	}	//end 接收到数据
//}


/*

void USART3_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else 
				{USART3_RX_STA|=0x8000;	//接收完成了 
					F_USART3_RX_FINISH=1;
				}
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
	USART3_RX_Data();

} 
	



void USART3_RX_Data()
{
	u8 len=0;
	u8 i=0;
	if(USART3_RX_STA&0x8000)
		{	
							   
			len=USART3_RX_STA&0X7FFF;//得到此次接收到的数据长度
			USART3_RX_BUF[len]=0;	 	//加入结束符
			
			if(len>USART3_REC_LEN-2)
			{
				len=USART3_REC_LEN-1;
				USART3_RX_BUF[len]=0;	 	//加入结束符
			}
			
			USART3_RX_BUF[USART3_REC_LEN-1]=0x01;
		for(i=0;i<USART3_REC_LEN;i++)
			{USART3_RX_GPS[i]=USART3_RX_BUF[i];
			}
			printf("USART3接收到为：%s\r\n",USART3_RX_BUF);
			printf("USART3接收到为：%s\r\n",USART3_RX_GPS);
			USART3_RX_STA=0;
		}
 
}
*/
//=========================================================

#endif	//end使能接收

//=========================================================





