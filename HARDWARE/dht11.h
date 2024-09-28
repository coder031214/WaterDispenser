#ifndef __DHT11_H
#define __DHT11_H 
#include "sys.h"  //������Ҫ��ͷ�ļ�
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//DHT11������ʪ�ȴ�������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//IO��������
#define DHT11_IO_IN()  {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=8<<12;}
#define DHT11_IO_OUT() {GPIOA->CRH&=0XFFFF0FFF;GPIOA->CRH|=3<<12;}

#define DHT11_IO_IN2()  {GPIOB->CRL &= 0xFFF0FFFF; GPIOB->CRL |= 8 << 20;}
#define DHT11_IO_OUT2() {GPIOB->CRL &= 0xFFF0FFFF; GPIOB->CRL |= 3 << 20;}


////IO��������											   
#define	DHT11_DQ_OUT PAout(11) //���ݶ˿�	PA0 
#define	DHT11_DQ_IN  PAin(11)  //���ݶ˿�	PA0 

#define	DHT11_DQ_OUT2 PBout(7) //���ݶ˿�	PA0 
#define	DHT11_DQ_IN2  PBin(7)  //���ݶ˿�	PA0 

u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    


u8 DHT11_Init2(void);//��ʼ��DHT11
u8 DHT11_Read_Data2(u8 *temp,u8 *humi);//��ȡ��ʪ��
u8 DHT11_Read_Byte2(void);//����һ���ֽ�
u8 DHT11_Read_Bit2(void);//����һ��λ
u8 DHT11_Check2(void);//����Ƿ����DHT11
void DHT11_Rst2(void);//��λDHT11   
#endif















