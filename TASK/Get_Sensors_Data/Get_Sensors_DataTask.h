#ifndef __GET_SENSORS_DATATASK_h_
#define __GET_SENSORS_DATATASK_h_
#include "sys.h"
#define Range_Pump  2000
typedef struct
{	
	int Machine_ID;			//��ˮ��ID��1234��ʾλ��
	char User_ID[32]; 		//�û�ID���
	int Machine_State;		//���� 1-����0-��    --0��λ��1��ͣ 2����
	int Temp_SetHot;	//�����¶�	         --�趨ˮ��  
	int Temp_Normal;		//����ǰ�¶�         --ˮ���¶�T1  DTH11
	int Temp_Hot;			//�����¶�	         --��ˮˮ��T2 DTH11	
	int Pump;		//ˮ��	����                 -- ˮ��״̬ M  LED
	int Water_Quantity;     //����ת��            --ˮ����F   ��ת������	
	int V_Normal;			//����ǰ��ѹ         --����ˮ��ѹV1  
	int V_Source;			//������ѹ           --ˮԴ��ѹV2
	int TDS_Source;			//��ѹ��ֵ           --ˮԴˮ��TDS1
	int TDS_Normal;			//�¶Ȳ�ֵ           --ˮ��ˮ��TDS2
	int Water_Hight;		//��ˮλ              --��ˮλ  Wh
	int Water_Low;			//��ˮλ				 --��ˮλ  Wl	
	
	int Light_Heat;			//��ˮ������P�� 1-�У�0-��   --��ˮ������P  LED
	int Light_Normal;		//���Ʒ� 1-����0-��   --��ŷ�S1  LED
	int Light_Hot;			//���Ʒ� 1-����0-��   --��ŷ�S2  LED
	
	int connect;			//����� 1-�����ˣ�0  --����״̬	
	
	int	ErrorAlarm;
	int	WaringAlarm;

    int Light_Sensor;
	int Hongwai_Sensor;

//	int GPS_position;			//�趨ˮ��	
//	u32 speed_Wheel;			//�����ٶ�
//	int I_Vehicle;			//ˮ��ˮ��TDS2
}
Sensor_Data_t;

void sensor_app_init(void);
void RFID_User_ID(void);  //RFID
void Get_Sensors_Data_TASK(Sensor_Data_t *Sensor_Data);

//void Sensor_Flash(Sensor_Data_t *Last_Sensor_Data,Sensor_Data_t *Sensor_Data);
//void Send_DATA_Flash(Sensor_Data_t *Last_Sensor_Data);

#endif
