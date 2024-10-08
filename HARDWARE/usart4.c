/******************************************************************************
*
*
*
*
*
*
*
*
*
******************************************************************************/

#include "usart4.h"
#include "usart.h"
#include "stm32f10x_rcc.h"
//#include "timer.h"
#include "stm32f10x.h"  //包含需要的头文件
#include "sys.h"

void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_Subpriority,uint8_t NVIC_Channel,uint32_t NVIC_Group)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	//检测输入参数
	assert_param(IS_NVIC_PREEMPTION_PRIORITY(NVIC_PreemptionPriority));  
	assert_param(IS_NVIC_SUB_PRIORITY(NVIC_Subpriority));
	
		NVIC_PriorityGroupConfig(NVIC_Group);	//设置中断分组	
	
	NVIC_InitStructure.NVIC_IRQChannel = NVIC_Channel;								//设置中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_PreemptionPriority;	//抢先优先级  	 	 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_Subpriority;				//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//使能中断

	NVIC_Init(&NVIC_InitStructure);	

}
/******************************************************************************
* Function Name --> 初始化IO 串口4
* Description   --> none
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_Init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#ifdef EN_UART4_RX	//如果使能了接收中断
	
	UART4->CR1 |= 1 << 8;	//PE中断使能
	UART4->CR1 |= 1 << 5;	//接收缓冲区非空中断使能
	
//	MY_NVIC_Init(3, 2, 52, NVIC_PriorityGroup_2);	//中断分组2
	
#endif
	 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);	//使能UART4，GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	USART_DeInit(UART4);  //复位串口4
	
	//UART4_TX   PC.10 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化PC10
   
	//UART4_RX	  PC.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);  //初始化PC11

	//USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;	//设置波特率，一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;	//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	
	USART_Init(UART4, &USART_InitStructure); //初始化串口
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(UART4, ENABLE);                    //使能串口 


//	TIM7_Int_Init(100-1,7200-1);		//10ms

	//TIM_Cmd(TIM7,ENABLE);			//
}

/******************************************************************************
* Function Name --> 串口4发送一个字节数据
* Description   --> nonevoid USART1_SendData(uint16_t Data)
* Input         --> bound：波特率	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendData(uint16_t Data)
{
	while((UART4->SR & 0x40) == 0);	//等待发送完毕
	USART_SendData(UART4, Data);
}

/******************************************************************************
* Function Name --> 串口4发送一串数据
* Description   --> none
* Input         --> *Data:字符串	
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_SendStr(uint8_t *Data)
{
	while(*Data!='\0')
	{
	    UART4_SendData((uint16_t )(*Data));
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
void UART4_SendStrlen(char *Data, uint8_t len)
{
	while(len--)
	{
		UART4_SendData((uint16_t )(*Data));
		Data++;
	}
}

//=========================================================
 
#ifdef EN_UART4_RX   //如果使能了接收

//=========================================================

//串口4中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误 

uint8_t UART4_RX_BUF[UART4_REC_LEN];	//接收缓冲,最大UART4_REC_LEN个字节
uint8_t UART4_RX_Bluetooth[UART4_REC_LEN];	//接收缓冲,最大UART4_REC_LEN个字节 

//接收状态
uint16_t UART4_RX_COUNT=0;	//接收到的有效字节数目，最大USART1_REC_LEN字节
uint8_t F_UART4_RX_FINISH=0;//接收状态标记,1:UART4接收完成，0：UART4接收未完成
uint8_t F_UART4_RX_RECEIVING=0;//串口接收正在进行中标记,1:进行中，0：暂停或结束
uint8_t UART4_RX_TIMEOUT_COUNT=0;	//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

/******************************************************************************
* Function Name --> 串口4接收中断服务程序
* Description   --> none
* Input         --> none
* Output        --> none
* Reaturn       --> none 
******************************************************************************/
void UART4_IRQHandler(void)
{
//	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收到数据
//	{
//		
//		F_UART4_RX_RECEIVING=1;//正在接收串口数据
//		UART4_RX_TIMEOUT_COUNT=0;//超时计数清零
//		UART4_RX_BUF[UART4_RX_COUNT] = USART_ReceiveData(UART4);	//读取接收到的数据
//		u1_printf("UART41!%d\r\n",UART4_RX_BUF);
//		u1_printf("UART42!%2s\r\n",UART4_RX_BUF);
//		u1_printf("UART43!%c\r\n",UART4_RX_BUF);
//		u1_printf("0UART4!%s\r\n",UART4_RX_BUF[0]);
//		u1_printf("1UART4!%s\r\n",UART4_RX_BUF[1]);
//		u1_printf("2UART4!%s\r\n",UART4_RX_BUF[2]);
//		u1_printf("3UART4!%s\r\n",UART4_RX_BUF[3]);
//		UART4_RX_COUNT++;
////		if(UART4_RX_COUNT>=5)//读取到了五个数据/闭眼/打哈欠/注意力不集中/打电话/抽烟/
////		{UART4_RX_COUNT = 0x0000;
////			if(UART4_RX_BUF[0]==01){
////			eye=1;   //眨眼布尔型数据	
////				}
////			else eye=0; 
////						if(UART4_RX_BUF[1]==01){
////			yawn=1;   //打哈欠布尔型数据	
////				}
////						else yawn=0;
////									if(UART4_RX_BUF[2]==01){
////			attention=1;   //注意力不集中布尔型数据	
////				}
////									else attention=0;
////												if(UART4_RX_BUF[3]==01){
////			phone=1;   //打电话布尔型数据	
////				}
////												else phone=0;
////															if(UART4_RX_BUF[4]==01){
////			smoking=1;   //抽烟布尔型数据
////				}
////															else smoking=0;
////u1_printf("UART4_RX_BUF;%s\r\n",UART4_RX_BUF);
////u1_printf("eye=%d yawn=%d attention=%d phone=%d smoking=%d \r\n",eye,yawn,attention,phone,smoking);
////		
////		{UART4_RX_COUNT = 0x0000;}}	
//		if(UART4_RX_COUNT>UART4_REC_LEN)//缓存区溢出
//		{UART4_RX_COUNT = 0x0000;}	
//		//end 接收未完成   		 		 
//	
//	}	//end 接收到数据
//	USART_ClearFlag(UART4,USART_FLAG_TC);

	
	
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(UART4);		//读取接收到的数据
		UART4_RX_BUF[UART4_RX_COUNT]=Res;
		//USART1TxChar(Res);
		if((UART4_RX_BUF[UART4_RX_COUNT] == 0x0a) && (UART4_RX_BUF[UART4_RX_COUNT - 1] == 0x0d))
			F_UART4_RX_RECEIVING = 1;
		UART4_RX_COUNT++;
		
		if(UART4_RX_COUNT > (UART4_REC_LEN - 1))
			UART4_RX_COUNT = 0;
	}
	USART_ClearFlag(UART4,USART_FLAG_TC);
}


void USART4_RX_Data()
{
	u8 len=0;
	u8 i=0;
	if(F_UART4_RX_RECEIVING==1)
		{	
							   
			len=UART4_RX_COUNT&0X7FFF;//得到此次接收到的数据长度
			UART4_RX_BUF[len]=0;	 	//加入结束符
			
			if(len>UART4_REC_LEN-2)
			{
				len=UART4_REC_LEN-1;
				UART4_RX_BUF[len]=0;	 	//加入结束符
			}
			
			UART4_RX_BUF[UART4_REC_LEN-1]=0x01;
		for(i=0;i<len;i++)
			{UART4_RX_Bluetooth[i]=UART4_RX_BUF[i];
			}
			printf("USART4接收到蓝牙为：%s\r\n",UART4_RX_BUF);
			printf("USART4接收到蓝牙为：%s\r\n",UART4_RX_Bluetooth);
			UART4_RX_COUNT=0;
		}
 
}

//=========================================================

#endif	//end使能接收

//=========================================================





