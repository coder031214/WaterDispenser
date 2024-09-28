#ifndef __WIFI_APP_PLAYTASK_H
#define __WIFI_APP_PLAYTASK_H			   
#include "sys.h" 
#include "Get_Sensors_DataTask.h"
#define ESP8266_ResGPIO     GPIOG   
#define ESP8266_ResPIN     GPIO_Pin_1    
// ����ȫ�ֱ���
extern int timestamp; // ����ʱ���Ϊ1631350215
// ����������ڴ洢��ǰʱ��
extern int hours ;
extern int minutes ;
extern int seconds ;

// ��������
void convert_timestamp_to_time(void);
void ESP8266_Res(void);
void wifi_app_init(Sensor_Data_t *Sensor_Data);
void wifi_app_play_TASK(Sensor_Data_t *Sensor_Data,Sensor_Data_t *Last_Sensor_Data);

#endif

