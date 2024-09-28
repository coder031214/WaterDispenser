/*
******************************************************************************
* @file    TIM/TimeBase/main.c 
* @author  产品中心.硬件部
* @version V1.0.0
* @date    2017.09.07
* @brief   通用定时器2
******************************************************************************
*/

//Includes
//#include "stm32f103_config.h"
#include "hal_timer2.h"
//#include "hal_buzzer.h"
#include "hal_uart3.h"
#include "hal_timer2.h"
#include "stm32f10x_tim.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

NVIC_InitTypeDef NVIC_InitStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t capture = 0;
__IO uint16_t CCR1_Val = 6000;//1ms,1KHz
__IO uint16_t CCR2_Val = 600;//0.1ms,10KHz
__IO uint16_t CCR3_Val = 120;//0.02ms,50KHz
__IO uint16_t CCR4_Val = 60000;//10ms,100Hz


/*******************************************************************
*函数：void TIM2_Init(void)
*功能：定时器2初始化
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void TIM2_Init(void)
{
	uint16_t PrescalerValue = 0;
	   
	/* --------------------------- System Clocks Configuration ---------------------*/
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* --------------------------NVIC Configuration -------------------------------*/
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
  printf("aaa");
  
	/* ------------------- TIM2 Configuration:Output Compare Timing Mode ---------*/

	/* 
	SystemCoreClock = SYSCLK_FREQ_72MHz;
	PrescalerValue = (uint16_t) (SystemCoreClock / 6000000) - 1=11;
	TIM2 counter clock at SystemCoreClock/(PrescalerValue+1)=72/(11+1) MHz = 6 MHz
	CC1 update rate = TIM2 counter clock / CCR1_Val = 1KHz
	CC2 update rate = TIM2 counter clock / CCR2_Val = 10KHz
	CC3 update rate = TIM2 counter clock / CCR3_Val = 0.25KHz
	CC4 update rate = TIM2 counter clock / CCR4_Val = 0.1KHz
	*/

	/* Compute the prescaler value */
	PrescalerValue = (uint16_t) (SystemCoreClock / 6000000) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
printf("bbb");
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, PrescalerValue, TIM_PSCReloadMode_Immediate);
printf("ccc");
	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
printf("ddd");
	/* Output Compare Timing Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;

	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* Output Compare Timing Mode configuration: Channel3 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
printf("eee");
	TIM_OC3Init(TIM2, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);

	/* Output Compare Timing Mode configuration: Channel4 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR4_Val;

	TIM_OC4Init(TIM2, &TIM_OCInitStructure);
printf("fff");
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);
printf("ggg");
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1 | TIM_IT_CC2 | TIM_IT_CC3 | TIM_IT_CC4, ENABLE);
printf("hhh");
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
printf("eee");
}

/**
  * @brief  This function handles TIM2 global interrupt request.
  * @param  None
  * @retval None
  */
/*
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		//1ms定时进程

		capture = TIM_GetCapture1(TIM2);
		TIM_SetCompare1(TIM2, capture + CCR1_Val);
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
*/

		//100us定时进程
		/***********************蜂鸣器鸣叫进程处理************************/
	//	HalBuzzerBlinkProcess();
		/***********************串口1接收超时进程处理************************/
	/*	if(F_USART1_RX_RECEIVING)//正在接收串口数据
		{
			USART1_RX_TIMEOUT_COUNT++;//串口超时计数
			if(USART1_RX_TIMEOUT_COUNT>3)//数据接收间隔超过3ms
			{//串口接收完成或结束
				F_USART1_RX_RECEIVING=0;
				USART1_RX_TIMEOUT_COUNT=0;
				F_USART1_RX_FINISH=1;
			}
		}*/
		/***********************串口2接收超时进程处理************************/
/*		if(F_USART3_RX_RECEIVING)//正在接收串口数据
		{
			USART3_RX_TIMEOUT_COUNT++;//串口超时计数
			if(USART3_RX_TIMEOUT_COUNT>3)//数据接收间隔超过3ms
			{//串口接收完成或结束
				F_USART3_RX_RECEIVING=0;
				USART3_RX_TIMEOUT_COUNT=0;
				F_USART3_RX_FINISH=1;
			}
		}

		capture = TIM_GetCapture2(TIM2);
		TIM_SetCompare2(TIM2, capture + CCR2_Val);
	}
	else if (TIM_GetITStatus(TIM2, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC3);

		//0.02ms,50KHz

		capture = TIM_GetCapture3(TIM2);
		TIM_SetCompare3(TIM2, capture + CCR3_Val);
	}
	else
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC4);

		//10ms定时进程

		capture = TIM_GetCapture4(TIM2);
		TIM_SetCompare4(TIM2, capture + CCR4_Val);
	}
}

*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
