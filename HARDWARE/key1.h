#ifndef __KEY1_H_
#define __KEY1_H_
enum keyValueType
{
	s101=0,s102,s103,s104,s105,s106,s107,s108,s109,s110,s111,s112,s113,
	s114,s115,s116,s117,s118,s119,s120,s121,s122,s123,s124,s125,noneKey
};

typedef enum keyValueType keyType;




void key_init(void);//按键初始化
keyType keyScanInd(void);//独立按键扫描
keyType KEY_ScanTriggerOnce(void);//独立按键扫描，按下触发，仅一次有效

/*******************键盘模块接线*********************/
#define K_PF4_PIN_PORT		GPIOF
#define K_PF4_PIN_PORT_CLK	RCC_APB2Periph_GPIOF
#define K_PF4_PIN			GPIO_Pin_4
#define K_PF4_IN_Init		K_PF4_PIN_PORT_CLK,\
							K_PF4_PIN_PORT,\
							K_PF4_PIN,GPIO_Speed_10MHz,GPIO_Mode_IPU
#define K_PF4				PFin(4)

#define K_PF5_PIN_PORT		GPIOF
#define K_PF5_PIN_PORT_CLK	RCC_APB2Periph_GPIOF
#define K_PF5_PIN			GPIO_Pin_5
#define K_PF5_IN_Init		K_PF5_PIN_PORT_CLK,\
							K_PF5_PIN_PORT,\
							K_PF5_PIN,GPIO_Speed_10MHz,GPIO_Mode_IPU
#define K_PF5				PFin(5)

#define K_PF6_PIN_PORT		GPIOF
#define K_PF6_PIN_PORT_CLK	RCC_APB2Periph_GPIOF
#define K_PF6_PIN			GPIO_Pin_6
#define K_PF6_IN_Init		K_PF6_PIN_PORT_CLK,\
							K_PF6_PIN_PORT,\
							K_PF6_PIN,GPIO_Speed_10MHz,GPIO_Mode_IPU
#define K_PF6				PFin(6)

#define K_PF7_PIN_PORT		GPIOF
#define K_PF7_PIN_PORT_CLK	RCC_APB2Periph_GPIOF
#define K_PF7_PIN			GPIO_Pin_7
#define K_PF7_IN_Init		K_PF7_PIN_PORT_CLK,\
							K_PF7_PIN_PORT,\
							K_PF7_PIN,GPIO_Speed_10MHz,GPIO_Mode_IPU
#define K_PF7				PFin(7)

#endif



