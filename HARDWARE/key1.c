#include "stdio.h"
#include "key1.h"
#include "GPIO.h"
#include "delay.h"

/******************************************************
�������ƣ�void keyInit(void)
���ܣ���������IO�ڳ�ʼ��
���룺��
�������
******************************************************/
void key_init(void)
{
	STM32_GPIOx_Init(K_PF4_IN_Init);//GPIO��������ʼ��
	STM32_GPIOx_Init(K_PF5_IN_Init);
	STM32_GPIOx_Init(K_PF6_IN_Init);	
	STM32_GPIOx_Init(K_PF7_IN_Init);
}

/******************************************************
�������ƣ�keyType keyScanInd(void)
���ܣ���������ɨ��
���룺��
�����������ֵ
******************************************************/
keyType keyScanInd(void)
{
	static unsigned char keyPressCount=0;
	static keyType keyValueOlde=noneKey;
		  keyType keyValue=noneKey;
	static uint8_t PressFlash=1;  //�ް�������
		
	if(K_PF4==1 && K_PF5==1 && K_PF6==1&& K_PF7==1)
	{
		PressFlash = 1;
	}
	
	//���K_PA1�е����а���
	if(K_PF5!=1&& PressFlash )//K_PA1���а�������    PAin(3)
	{
	  printf("s1\n");
		keyValue=s106;
		PressFlash = 0;//��־Ϊ�а�������	
	}	
	
	//���K_PA3�е����а���
	if(K_PF4!=1&& PressFlash )//K_PA3���а�������   	PAin(2)
	{
		printf("s5\n");
		keyValue=s116;
		PressFlash = 0;//��־Ϊ�а�������	
	}
	
	//���K_PF4�е����а���
	if(K_PF6!=1&& PressFlash )//K_PF4���а�������  	PAin(1)
	{
		printf("s9\n");
		keyValue=s121;
		PressFlash = 0;//��־Ϊ�а�������
	}
	
	//���K_PF4�е����а���
	if(K_PF7!=1&& PressFlash )//K_PF4���а�������  	PAin(1)
	{
		printf("s13\n");
		keyValue=s120;
		PressFlash = 0;//��־Ϊ�а�������
	}
	
	if(keyValue==noneKey)//�ް�������
	{
		keyValueOlde=keyValue;
		keyPressCount=0;
	}
	//�����ǰ���ȥ��������
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
�������ƣ�keyType KEY_ScanTriggerOnce(void)
���ܣ���������ɨ�裬������Ч��������һ��
���룺��
�����������ֵ
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
	static keyType keyValueTemp=noneKey;//������ֵ�Ĵ���
	static keyType keyValueTempOlde=noneKey;//��һ�ΰ�����ֵ�Ĵ���
	
	keyValueTemp=keyScanInd();//����ɨ��
	
	if(keyValueTempOlde != keyValueTemp)
	{//�������л���������Ч����ֻ����һ��
	
		keyValueTempOlde=keyValueTemp;//�����л��������ϴΰ���ֵ
	}
	else
	{//����һֱ���£���û���£�������Ч
		keyValueTemp=noneKey;
	}
	return keyValueTemp;
}
