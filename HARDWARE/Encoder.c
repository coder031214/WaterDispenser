#include "stm32f10x.h"                  // Device header

int16_t Encoder_Count=0;

void Encoder_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource1);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF, GPIO_PinSource2);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line1 | EXTI_Line2;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStructure);
}

int16_t Encoder_Get(void)
{
	if(Encoder_Count>21)
	{
			
			Encoder_Count=0;
	}
	
	return Encoder_Count;
}

//void EXTI0_IRQHandler(void)
//{
//	if (EXTI_GetITStatus(EXTI_Line0) == SET)
//	{
//		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
//		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)
//		{
//			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
//			{
//				Encoder_Count --;
//			}
//		}
//		EXTI_ClearITPendingBit(EXTI_Line0);
//	}
//}

void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		
		/*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_1) == 0)
		{
			if (GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_2) == 0)
			{
				Encoder_Count ++;
				
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}
