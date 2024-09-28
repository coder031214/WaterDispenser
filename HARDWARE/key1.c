#include "stdio.h"
#include "key1.h"
#include "GPIO.h"
#include "delay.h"

/******************************************************
函数名称：void keyInit(void)
功能：独立按键IO口初始化
输入：无
输出：无
******************************************************/
void key_init(void)
{
	STM32_GPIOx_Init(K_PF4_IN_Init);//GPIO输出方向初始化
	STM32_GPIOx_Init(K_PF5_IN_Init);
	STM32_GPIOx_Init(K_PF6_IN_Init);	
	STM32_GPIOx_Init(K_PF7_IN_Init);
}

/******************************************************
函数名称：keyType keyScanInd(void)
功能：独立按键扫描
输入：无
输出：按键键值
******************************************************/
keyType keyScanInd(void)
{
	static unsigned char keyPressCount=0;
	static keyType keyValueOlde=noneKey;
		  keyType keyValue=noneKey;
	static uint8_t PressFlash=1;  //无按键按下
		
	if(K_PF4==1 && K_PF5==1 && K_PF6==1&& K_PF7==1)
	{
		PressFlash = 1;
	}
	
	//检测K_PA1行的所有按键
	if(K_PF5!=1&& PressFlash )//K_PA1行有按键按下    PAin(3)
	{
	  printf("s1\n");
		keyValue=s106;
		PressFlash = 0;//标志为有按键按下	
	}	
	
	//检测K_PA3行的所有按键
	if(K_PF4!=1&& PressFlash )//K_PA3行有按键按下   	PAin(2)
	{
		printf("s5\n");
		keyValue=s116;
		PressFlash = 0;//标志为有按键按下	
	}
	
	//检测K_PF4行的所有按键
	if(K_PF6!=1&& PressFlash )//K_PF4行有按键按下  	PAin(1)
	{
		printf("s9\n");
		keyValue=s121;
		PressFlash = 0;//标志为有按键按下
	}
	
	//检测K_PF4行的所有按键
	if(K_PF7!=1&& PressFlash )//K_PF4行有按键按下  	PAin(1)
	{
		printf("s13\n");
		keyValue=s120;
		PressFlash = 0;//标志为有按键按下
	}
	
	if(keyValue==noneKey)//无按键按下
	{
		keyValueOlde=keyValue;
		keyPressCount=0;
	}
	//以下是按键去抖动处理
	if(keyValueOlde!=keyValue)
	{
		keyValueOlde=keyValue;
		keyPressCount=0;
		return keyValue;
	}
	else
	{
		if(keyPressCount>5)
		{
			return keyValue;
		}
		keyPressCount++;
		return noneKey;
	}

}

/******************************************************
函数名称：keyType KEY_ScanTriggerOnce(void)
功能：独立按键扫描，按下有效，仅触发一次
输入：无
输出：按键键值
******************************************************/
/*
enum keyValueType
{
	s101=0,s102,s103,s104,s105,s106,s107,s108,s109,s110,s111,s112,s113,
	s114,s115,s116,s117,s118,s119,s120,s121,s122,s123,s124,s125,noneKey
};

typedef enum keyValueType keyType;
*/
keyType KEY_ScanTriggerOnce(void)
{
	static keyType keyValueTemp=noneKey;//按键键值寄存器
	static keyType keyValueTempOlde=noneKey;//上一次按键键值寄存器
	
	keyValueTemp=keyScanInd();//按键扫描
	
	if(keyValueTempOlde != keyValueTemp)
	{//按键有切换，按键有效，且只触发一次
	
		keyValueTempOlde=keyValueTemp;//按键切换，更新上次按键值
	}
	else
	{//按键一直按下，或没按下，按键无效
		keyValueTemp=noneKey;
	}
	return keyValueTemp;
}
