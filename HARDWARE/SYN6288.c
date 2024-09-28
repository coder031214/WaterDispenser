#include "usart4.h"
#include "usart.h"
#include "stm32f10x_rcc.h"
//#include "timer.h"
#include "stm32f10x.h"  //������Ҫ��ͷ�ļ�
#include "sys.h"
#include <string.h>
#include "SYN6288.h"


/*
1.��Ŀ���ƣ������콢��SYN6288�����ϳ�ģ��STC89C52���Գ���
2.��ʾģ�飺��
3.ʹ�������keil4 for 51 �� keil5 for 51
4.������λ������
5.��Ŀ��ɣ�SYN6288�����ϳ�ģ��
6.��Ŀ���ܣ�������ת���������ʶ�����������������Ĭ��9600��
7.��Ҫԭ������ο�SYN6288�����ֲ�
8.�����ַ��https://lssz.tmall.com ���Ա��������������콢�ꡱ
9.��Ȩ�����������콢�����г��������������Ȩ�����뱾���Ʒ���׳��ۣ��벻Ҫ����������׷���䷨�����Σ�
���߶��壺(������ʹ�þ���Ϊ11.0592M�����س���ʱע�ⲻҪ����SYN6288ģ��)
		VCC--5V
		RXD--P3.1
		TXD--P3.0
		GND--GND
*/





/**************оƬ��������*********************/
char SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //ֹͣ�ϳ�
char SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //��ͣ�ϳ�
char SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //�ָ��ϳ�
char SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //״̬��ѯ
char SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //����POWER DOWN ״̬����

////����1��ʼ��
//void UART1_Init(void)
//{
//	SCON = 0x50;		//8λ����,�ɱ䲨����
//	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
//	TL1 = 0xFD;		//�趨��ʱ��ֵ
//	TH1 = 0xFD;		//�趨��ʱ����װֵ
//	ET1 = 0;		//��ֹ��ʱ��1�ж�
//	TR1 = 1;		//������ʱ��1
//	ES = 1;
//	EA = 1;
//}

////����2����һ�ֽ�����
//void UART1_SendByte(char DAT)
//{
//	SBUF = DAT;
//	while(TI == 0);
//	TI=0;
//}

////����2�����ַ�������
//void UART1_SendString(char *DAT, char len)
//{
//	char i;
//	for(i = 0; i < len; i++)
//	{
//		UART1_SendByte(*DAT++);
//	}
//}

////����1�ж�
//void Uart1_Isr() interrupt 4
//{
//	if(RI)
//	{
//		RI=0;
//	}

//}


//Music:  0:�ޱ�������  1~15:ѡ�񱳾�����
void SYN_FrameInfo(char Music, char *HZdata)
{
	/****************��Ҫ���͵��ı�**********************************/
	char  Frame_Info[50];
	char  HZ_Length;
	char  ecc  = 0;  			//����У���ֽ�
	int i = 0;
	HZ_Length = strlen((char*)HZdata); 			//��Ҫ�����ı��ĳ���

	/*****************֡�̶�������Ϣ**************************************/
	Frame_Info[0] = 0xFD ; 			//����֡ͷFD
	Frame_Info[1] = 0x00 ; 			//�������������ȵĸ��ֽ�
	Frame_Info[2] = HZ_Length + 3; 		//�������������ȵĵ��ֽ�
	Frame_Info[3] = 0x01 ; 			//���������֣��ϳɲ�������
	Frame_Info[4] = 0x01 | Music << 4 ; //����������������������趨

	/*******************У�������***************************************/
	for(i = 0; i < 5; i++)   				//���η��͹���õ�5��֡ͷ�ֽ�
	{
		ecc = ecc ^ (Frame_Info[i]);		//�Է��͵��ֽڽ������У��
	}

	for(i = 0; i < HZ_Length; i++)   		//���η��ʹ��ϳɵ��ı�����
	{
		ecc = ecc ^ (HZdata[i]); 				//�Է��͵��ֽڽ������У��
	}
	/*******************����֡��Ϣ***************************************/
	memcpy(&Frame_Info[5], HZdata, HZ_Length);
	Frame_Info[5 + HZ_Length] = ecc;
	UART4_SendStrlen(Frame_Info, 5 + HZ_Length + 1);
}


/***********************************************************
* ��    �ƣ� YS_SYN_Set(char *Info_data)
* ��    �ܣ� ������	�������
* ��ڲ����� *Info_data:�̶���������Ϣ����
* ���ڲ�����
* ˵    �����������������ã�ֹͣ�ϳɡ���ͣ�ϳɵ����� ��Ĭ�ϲ�����9600bps��
* ���÷�����ͨ�������Ѿ�������������������á�
**********************************************************/
void YS_SYN_Set(char *Info_data)
{
	char Com_Len;
	Com_Len = strlen((char*)Info_data);
	UART4_SendStrlen(Info_data, Com_Len);
}

//�����ʱ1ms
//void  delay(int uldata)
//{
//	int j  =  0;
//	int g  =  0;
//	for (j = 0; j < uldata; j++)
//		for (g = 0; g < 110; g++);
//}


//void  main(void)
//{
//	UART1_Init();
//	while(1)
//	{
//		//ѡ�񱳾�����2��(0���ޱ�������  1-15���������ֿ�ѡ)
//		//m[0~16]:0��������Ϊ������16���������������
//		//v[0~16]:0�ʶ�����Ϊ������16�ʶ��������
//		//t[0~5]:0�ʶ�����������5�ʶ��������
//		//���������ù�����ο������ֲ�
//		SYN_FrameInfo(2, "[v5][t5]��ӭʹ�������콢��SYN6288�����ϳ�ģ��");
//		//��ͣ�ϳɣ���ʱû���õ�������չʾ�����÷�
//		//YS_SYN_Set(SYN_SuspendCom);
//		delay(10000);
//	}

//}
