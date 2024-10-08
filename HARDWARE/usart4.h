#ifndef __UART4_H_
#define __UART4_H_
#include "sys.h"

#define UART4_REC_LEN					256	//定义最大接收字节数
#define EN_UART4_RX						1	//串口接收使能。
											//1：允许接收
											//0：禁止接收

/******************************************************************************
                              外部调用功能函数
******************************************************************************/

#if EN_UART4_RX	//使能了串口接收

	extern uint8_t  UART4_RX_BUF[UART4_REC_LEN];	//接收缓冲，最大UART4_REC_LEN个字节，末字节为换行符 
	extern uint16_t UART4_RX_COUNT;				//UART4接收到数据个数
	extern uint8_t F_UART4_RX_FINISH;				//接收状态标记,1:UART4接收完成，0：UART4接收未完成
	extern uint8_t F_UART4_RX_RECEIVING;			//串口接收正在进行中标记,1:进行中，0：暂停或结束
	extern uint8_t UART4_RX_TIMEOUT_COUNT;			//串口接收超时计数。串口超过3ms未接收到数据，视为超时。单位：ms

#endif
void MY_NVIC_Init(uint8_t NVIC_PreemptionPriority,uint8_t NVIC_Subpriority,uint8_t NVIC_Channel,uint32_t NVIC_Group);//中断初始化

void UART4_Init(uint32_t bound);	//初始化IO 串口1
void UART4_SendData(uint16_t Data);
void UART4_SendStr(uint8_t *Data);
void UART4_SendStrlen(char *Data, uint8_t len);


#endif

