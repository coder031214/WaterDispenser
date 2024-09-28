#ifndef __GET_SENSORS_DATATASK_h_
#define __GET_SENSORS_DATATASK_h_
#include "sys.h"
#define Range_Pump  2000
typedef struct
{	
	int Machine_ID;			//饮水机ID，1234显示位置
	char User_ID[32]; 		//用户ID身份
	int Machine_State;		//车锁 1-开，0-关    --0复位，1暂停 2运行
	int Temp_SetHot;	//工作温度	         --设定水温  
	int Temp_Normal;		//启动前温度         --水箱温度T1  DTH11
	int Temp_Hot;			//工作温度	         --热水水温T2 DTH11	
	int Pump;		//水泵	，，                 -- 水泵状态 M  LED
	int Water_Quantity;     //油门转速            --水流量F   旋转编码器	
	int V_Normal;			//启动前电压         --常温水电压V1  
	int V_Source;			//工作电压           --水源电压V2
	int TDS_Source;			//电压差值           --水源水质TDS1
	int TDS_Normal;			//温度差值           --水箱水质TDS2
	int Water_Hight;		//高水位              --高水位  Wh
	int Water_Low;			//低水位				 --低水位  Wl	
	
	int Light_Heat;			//热水加热器P否 1-有，0-无   --热水加热器P  LED
	int Light_Normal;		//亮灯否 1-亮，0-灭   --电磁阀S1  LED
	int Light_Hot;			//亮灯否 1-亮，0-灭   --电磁阀S2  LED
	
	int connect;			//网络否 1-连上了，0  --网络状态	
	
	int	ErrorAlarm;
	int	WaringAlarm;

    int Light_Sensor;
	int Hongwai_Sensor;

//	int GPS_position;			//设定水温	
//	u32 speed_Wheel;			//车轮速度
//	int I_Vehicle;			//水箱水质TDS2
}
Sensor_Data_t;

void sensor_app_init(void);
void RFID_User_ID(void);  //RFID
void Get_Sensors_Data_TASK(Sensor_Data_t *Sensor_Data);

//void Sensor_Flash(Sensor_Data_t *Last_Sensor_Data,Sensor_Data_t *Sensor_Data);
//void Send_DATA_Flash(Sensor_Data_t *Last_Sensor_Data);

#endif
