/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	iwdg.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		独立看门狗初始化、喂狗
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "task.h"


//硬件驱动
#include "iwdg.h"




/*
************************************************************
*	函数名称：	Iwdg_Init
*
*	函数功能：	IWDG初始化
*
*	入口参数：	psc：分频系数
*				arr：重载值
*
*	返回参数：	无
*
*	说明：		独立看门狗是固定40KHz时钟
************************************************************
*/
void Iwdg_Init(unsigned char psc, unsigned short arr)
{

	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//使能对寄存器IWDG_PR和IWDG_RLR的写操作
	
	IWDG_SetPrescaler(psc);							//设置IWDG预分频值:设置IWDG预分频值为64
													//0		4分频
													//1		8分频
													//2		16分频
													//3		32分频
													//4		64分频
													//5		128分频
													//6		256分频
													//7		256分频
	
	IWDG_SetReload(arr);							//设置IWDG重装载值		每次喂狗后从重载值开始递减
	
	IWDG_ReloadCounter();							//按照IWDG重装载寄存器的值重装载IWDG计数器
	
	IWDG_Enable();									//使能IWDG

}

/*
************************************************************
*	函数名称：	Iwdg_Feed
*
*	函数功能：	喂狗
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		在看门狗定时周期内调用
************************************************************
*/
void Iwdg_Feed(void)
{
	
 	IWDG_ReloadCounter();
	
}
/*
************************************************************
*	函数名称：	IWDG_Task
*
*	函数功能：	清除看门狗
*
*	入口参数：	void类型的参数指针
*
*	返回参数：	无
*
*	说明：		看门狗任务
************************************************************
*/
void IWDG_Task(void *pdata)
{

	while(1)
	{
	
		Iwdg_Feed(); 		//喂狗
		
		vTaskDelay(50); 	//挂起任务250ms
	
	}

}
