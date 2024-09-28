#include "led.h"
#include "gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOE, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
 GPIO_SetBits(GPIOB,GPIO_Pin_4);						 //PB.5 �����

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 �˿�����, �������
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
 GPIO_SetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 ����� 
}
 
void my_LED_Init(void){
	
GPIOx_Init( Vehicle_light_GPIO_PORT,   Vehicle_light_PIN,   OUTPUT_AF,    GPIO_Speed_50MHz );
GPIOx_Init( Vehicle_lift_GPIO_PORT,   Vehicle_lift_PIN,   OUTPUT_AF,    GPIO_Speed_50MHz );
GPIOx_Init( Vehicle_right_GPIO_PORT,   Vehicle_right_PIN,   OUTPUT_AF,    GPIO_Speed_50MHz );	
}

void my_LED1_Init(void){
STM32_GPIOx_Init(PE6_IN_Init);
STM32_GPIOx_Init(PE0_IN_Init);
STM32_GPIOx_Init(PE1_IN_Init);
STM32_GPIOx_Init(PE2_IN_Init);
}
