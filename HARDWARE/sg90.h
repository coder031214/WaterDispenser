#ifndef __SG90_H_
#define __SG90_H_
#include "sys.h"

void PWM_Init(u16 arr,u16 psc);
void TIM3_PWM_Init(u16 crr,u16 psc);//
int pwm(void);
#endif
