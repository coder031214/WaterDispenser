#include "sg90.h"
#include "usart.h"

//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;              //定义GPIO结构体 需要用结构体类型参数时候一定要先在前面定义
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;    //定义TIMx定时器结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;            //定义定时器脉宽调制结构体
    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4,ENABLE);          //使能TIM3、4时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOA时钟和GPIOB时钟
    
//    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);                     //TIM3部分重映射 TIM3_CH2->PB5
	
    //设置该引脚为复用输出功能,输出TIM3、4 的PWM脉冲波形	GPIOB.0、1、6、7、8、9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_9;// GPIO_Pin_8|                              
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                         //复用输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;                       //配置输出速率
    GPIO_Init(GPIOB,&GPIO_InitStructure);                                   //初始化GPIOB
	
	 //设置该引脚为复用输出功能,输出TIM3 的PWM脉冲波形	GPIOA 6、7
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;                               
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                         //复用输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;                       //配置输出速率
    GPIO_Init(GPIOA,&GPIO_InitStructure);                                   //初始化GPIOA
	

	
	//初始化TIM3
    TIM_TimeBaseStructure.TIM_Period = arr;                                 //设置自动重装载寄存器周期的值 arr=value-1
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                              //设置预分频值 psc=value-1
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;             //TIM向上计数模式
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);                          //初始化TIMx时间基数
    
	//初始化TIM4
    TIM_TimeBaseStructure.TIM_Period = arr;                                 //设置自动重装载寄存器周期的值 arr=value-1
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                              //设置预分频值 psc=value-1
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                            //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;             //TIM向上计数模式
    TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);                          //初始化TIMx时间基数
	
	//初始化TIM3 Channel2 PWM模式     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                       //选择定时器模式:TIM脉冲宽度调制模式1（改变占空比）
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;           //使能比较输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;               //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM3 OC1
    TIM_OC2Init(TIM3,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC3Init(TIM3,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM3 OC2
	TIM_OC4Init(TIM3,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM3 OC2
	
	
    //初始化TIM4 Channel2 PWM模式     
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;                       //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;           //使能比较输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;               //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM4,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM4 OC1
    TIM_OC2Init(TIM4,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM4 OC2
	TIM_OC3Init(TIM4,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM4 OC2
	TIM_OC4Init(TIM4,&TIM_OCInitStructure);                                 //根据T指定的参数初始化外设TIM4 OC2

 
//    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);                        //使能TIM3在CCR2上的预装载寄存器
    TIM_Cmd(TIM3, ENABLE);                                                  //使能TIM3
	TIM_Cmd(TIM4, ENABLE);                                                  //使能TIM4
}


//定时器初始化
//#include"timer.h"
#include"sys.h"
 
void TIM3_PWM_Init(u16 crr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStucture;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitTypeStucture;
	TIM_OCInitTypeDef TIM_OCInitTypeStucture;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOB的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能定时器3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO ,ENABLE);//开启复用时钟
	
	GPIO_InitStucture.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStucture.GPIO_Pin=GPIO_Pin_5;
	GPIO_InitStucture.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStucture);//初始化GPIO口
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//将B5部分重映射到定时器3
	
	TIM_TimeBaseInitTypeStucture.TIM_ClockDivision=TIM_CKD_DIV1;//时钟分割
	TIM_TimeBaseInitTypeStucture.TIM_CounterMode=TIM_CounterMode_Up;//向上计数模式
	TIM_TimeBaseInitTypeStucture.TIM_Period=crr;//重装载值
	TIM_TimeBaseInitTypeStucture.TIM_Prescaler=psc;//预分频系数
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitTypeStucture);
	
	TIM_OCInitTypeStucture.TIM_OCMode=TIM_OCMode_PWM1;//设置pwm模式位2，即计数器的值大于设定值有效
	TIM_OCInitTypeStucture.TIM_OCPolarity=TIM_OCPolarity_High;//若判定有效，有效为高电平，无效则为低电平
	TIM_OCInitTypeStucture.TIM_OutputState=TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitTypeStucture.TIM_Pulse=0;
	TIM_OC2Init(TIM3,&TIM_OCInitTypeStucture);
	
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能预装载寄存器
	
	TIM_Cmd(TIM3,ENABLE);//使能定时器3
}
//主函数
#include "sys.h"
#include "delay.h"
#include "led.h"
//#include"timer.h"
 u16 pwm_vlue=0;
	u8 Dir=1;

