#ifndef __WIFI_APP_PLAYTASK_H
#define __WIFI_APP_PLAYTASK_H			   
#include "sys.h" 
#include "Get_Sensors_DataTask.h"
#define ESP8266_ResGPIO     GPIOG   
#define ESP8266_ResPIN     GPIO_Pin_1    
// 定义全局变量
extern int timestamp; // 假设时间戳为1631350215
// 定义变量用于存储当前时间
extern int hours ;
extern int minutes ;
extern int seconds ;

// 函数声明
void convert_timestamp_to_time(void);
void ESP8266_Res(void);
void wifi_app_init(Sensor_Data_t *Sensor_Data);
void wifi_app_play_TASK(Sensor_Data_t *Sensor_Data,Sensor_Data_t *Last_Sensor_Data);

#endif

