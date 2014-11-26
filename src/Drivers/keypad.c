/**
* @file keypad.c
* @brief 按键及LED驱动程序
* @note 
* 
* @version V0.0.1
* @author zhongyh
* @date 2009年8月28日
* @note
*      本模块为手持刷卡机按键处理模块
*      本项目为4x6矩阵键盘，有24个按键：
*    F1     F2    F3    F4
*    POWER  PgUp  PgDn  InPaper
*    '1'    '2'   '3'   Menu
*    '4'    '5'   '6'   Fun
*    '7'    '8'   '9'   Alpha
*    Esc    '0'   Clr   Enter
*    
*    由上至下，由左而右的键值顺序为：0,1,2,3,4,5...23
*
* @copy
*
* 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
* 本公司以外的项目。本司保留一切追究权利。
*
* <h1><center>&copy; COPYRIGHT 2009 netcom</center></h1>
*
*/
#include "stm32f10x_lib.h"
#include "ucos_ii.h"
#include <stdio.h>
#include "keypad.h"
#include "cpu.h"

//static unsigned int led_state;

static void keypad_initport(void);
static void Keypad_ScanLine(INT8U line);
//static int key_readcol(void);
static void BeepDelay(unsigned int delay);
/**
* @brief 键值表
*/
#if 0
const unsigned char KeyTable[] = 
{
	KEY_FUN1, KEY_FUN2, KEY_FUN3, KEY_FUN4,		// F1,F2,F3,F4
	KEY_POWER, KEY_UP, KEY_DOWN, KEY_INPAGE,	//PWR,PU,PD,InPaper
	
	KEY_NUM1, KEY_NUM2,KEY_NUM3,KEY_MENU,		// 1, 2, 3, Menu
	KEY_NUM4,KEY_NUM5,KEY_NUM6,KEY_FUN,			// 4, 5, 6, Fun
	KEY_NUM7,KEY_NUM8,KEY_NUM9,KEY_ALPHA,		// 7, 8, 9, Alpha
	KEY_ESC,KEY_NUM0,KEY_CLEAR,KEY_ENTER,		// ESC, 0, CLEAR, Enter
/*	
	24,25,26,27,	// 扩展按键(非程序使用)
	28,29,30,31,
	
	32,33,34,35,	// PN532事件
	36,37,38,39,
	
	40,41,42,43,	// GPRS事件
	44,45,46,47,
	
	48,49,50,51,	// 2.4G事件
	52,53,54,55,
	//	56,57,58,59,
*/		
};
#endif
const unsigned char KeyTable[] = 
{
	KEY_ESC, KEY_SCAN, KEY_LEFT_SHOTCUT, 
	KEY_ENTER, KEY_RIGHT_SHOTCUT, KEY_UP,
	KEY_LEFT, KEY_RIGHT,KEY_DOWN,KEY_POWER,
};

/**
*	@brief 键值与ascii码对应表
*/
TkeyValue_Ascii keyValue_Ascii_Tbl[] = {
	{KEY_NUM1,"1"},{KEY_NUM2,"2"},{KEY_A,"A"},{KEY_B,"B"},{KEY_C,"C"},{KEY_a,"a"},{KEY_b,"b"},{KEY_c,"c"},
	{KEY_NUM3,"3"},{KEY_D,"D"},{KEY_E,"E"},{KEY_F,"F"},{KEY_d,"d"},{KEY_e,"e"},{KEY_f,"f"},{KEY_NUM4,"4"},
	{KEY_G,"G"},{KEY_H,"H"},{KEY_I,"I"},{KEY_g,"g"},{KEY_h,"h"},{KEY_i,"i"},{KEY_NUM5,"5"},{KEY_J,"J"},
	{KEY_K,"K"},{KEY_L,"L"},{KEY_j,"j"},{KEY_k,"k"},{KEY_l,"l"},{KEY_NUM6,"6"},{KEY_M,"M"},{KEY_N,"N"},
	{KEY_O,"O"},{KEY_m,"m"},{KEY_n,"n"},{KEY_o,"o"},{KEY_NUM7,"7"},{KEY_P,"P"},{KEY_Q,"Q"},{KEY_R,"R"},
	{KEY_S,"S"},{KEY_p,"p"},{KEY_q,"q"},{KEY_r,"r"},{KEY_s,"s"},{KEY_NUM8,"8"},{KEY_T,"T"},{KEY_U,"U"},{KEY_V,"V"},
	{KEY_t,"t"},{KEY_u,"u"},{KEY_v,"v"},{KEY_NUM9,"9"},{KEY_W,"W"},{KEY_X,"X"},{KEY_Y,"Y"},{KEY_Z,"Z"},
	{KEY_w,"w"},{KEY_x,"x"},{KEY_y,"y"},{KEY_z,"z"},{KEY_NUM0,"0"},{KEY_space," "},{KEY_douhao,","},{KEY_juhao,"."},{KEY_fenhao,";"},
	{KEY_xing,"*"},{KEY_jing,"#"},{KEY_lianjiehao,"_"},{KEY_zhonghuaxian,"-"},{KEY_zuo_kuohao,"("},{KEY_you_kuohao,")"},

        {KEY_gantanhao,"!"},{KEY_shuangyinhao,"\x22"},{KEY_meiyuanhao,"$"},{KEY_baifenhao,"%"},{KEY_yuhao,"&"},{KEY_danyinhao,"'"},
	{KEY_jiahao,"+"},{KEY_chuhao,"/"},{KEY_maohao,":"},{KEY_xiaoyuhao,"<"},{KEY_dengyuhao,"="},{KEY_dayuhao,">"},{KEY_wenhao,"?"},
	{KEY_athao,"@"},{KEY_zuozhongkuohao,"["},{KEY_xiehuaxianhao,"\\"},{KEY_youzhongkuohao,"]"},{KEY_xorhao,"^"},{KEY_piehao,"`"},
	{KEY_zuodakuohao,"{"},{KEY_huohao,"|"},{KEY_youdakuohao,"}"},{KEY_qufanhao,"~"},
	{KEY_FUN1, "F1"}, {KEY_FUN2, "F2"}, {KEY_FUN3, "F3"},{KEY_FUN4, "F4"}, {KEY_UP, "Up"}, {KEY_CLEAR, "Clear"},
	{KEY_ESC, "Cancel"}, {KEY_DOWN, "Down"},{KEY_LEFT, "Left"},{KEY_RIGHT, "Right"}, {KEY_ENTER, "Enter"},{KEY_POWER,"Power"},{0,0}
};
/**
* @brief   		Intialize the KeyBoard IO
* @param[in]   none
* @return      none
* @note   
*		键盘矩阵的硬件连接如下图所示：
*							Col1		Col2		Col3
*							(PD11)		(PD3)		(PD12)
*									
*		Line1(PC6)			F2			KEY_ENTER	   F1
*							
*		Line2(PC7)		    F3	        F4          KEY_UP
*
*		Line3(PD15)		    KEY_LEFT    KEY_RIGHT   KEY_DOWN
*/

/**
* @brief 将键值转换为ascii值
* @param[in] unsigned char key 内部扫描键值
* @return unsigned char * ascii键值的指针
*/
unsigned char *key2ascii(unsigned char key)
{
	unsigned int i = 0;

	if(key == 0)
		return 0;

	while (keyValue_Ascii_Tbl[i].asciivalue != 0) 
	{
		if (key == keyValue_Ascii_Tbl[i].keyvalue) 
		{
			return keyValue_Ascii_Tbl[i].asciivalue;
		}
		i++;
	}
	return 0;
}

static void keypad_initport(void)
{
	GPIO_InitTypeDef  gpio_init;
	EXTI_InitTypeDef EXTI_InitStructure;
    // NVIC_InitTypeDef	NVIC_InitStructure;
	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD |RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
        
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

	//POWER-ON  PA.1
	gpio_init.GPIO_Pin   = POWER_ON;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &gpio_init);
	//GPIO_ResetBits(GPIOA,POWER_ON);		


	//START  PC.3
	gpio_init.GPIO_Pin   = START;
	gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &gpio_init);
	
	//BEEP CS  PD.3
#ifdef HW_V1_01
	gpio_init.GPIO_Pin   = Beep_CS;
	gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &gpio_init);
	GPIO_ResetBits(GPIOD,Beep_CS);	
#endif
	//LED_G PD.1
	gpio_init.GPIO_Pin	= GPIO_Pin_1;
	gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &gpio_init);
	GPIO_ResetBits(GPIOD,GPIO_Pin_1);
	//led_state = 0;
	
	///列线置为输入模式，如果外面没有接上拉电阻，需要置为输入上拉模式
	//PD11 PD3 PD12		V1.0
#ifdef HW_V1_00
	gpio_init.GPIO_Pin   =  GPIO_Pin_3 | GPIO_Pin_11 | GPIO_Pin_12;
#else
	//PD11 PD4 PD12		V1.01
	gpio_init.GPIO_Pin   =  GPIO_Pin_4 | GPIO_Pin_11 | GPIO_Pin_12;
#endif

	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &gpio_init);

	///* Connect EXTI Line4 to PA.4 */
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);

	//EXTI_ClearITPendingBit(EXTI_Line4);
	//EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	//EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	//EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	//EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	//EXTI_Init(&EXTI_InitStructure); 
	//EXTI_GenerateSWInterrupt(EXTI_Line4);
	
	
#ifdef HW_V1_00
	/* Connect EXTI Line3 to PD.3 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource3);
	/* Connect EXTI Line11 to PD.11 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
	/* Connect EXTI Line11 to PD.12 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);

	/* Configure EXTI LineX to generate an interrupt on falling edge */
	EXTI_ClearITPendingBit(EXTI_Line3| EXTI_Line11 | EXTI_Line12);
	EXTI_InitStructure.EXTI_Line = EXTI_Line3 | EXTI_Line11 | EXTI_Line12;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line3 | EXTI_Line11 | EXTI_Line12);
#else
	/* Connect EXTI Line3 to PC.3 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource3);

	EXTI_ClearITPendingBit(EXTI_Line3);
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	EXTI_GenerateSWInterrupt(EXTI_Line3);

	/* Connect EXTI Line4 to PD.4 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4);
	/* Connect EXTI Line11 to PD.11 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource11);
	/* Connect EXTI Line11 to PD.12 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);

	/* Configure EXTI LineX to generate an interrupt on falling edge */
	EXTI_ClearITPendingBit(EXTI_Line4| EXTI_Line11 | EXTI_Line12);
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line11 | EXTI_Line12;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	EXTI_GenerateSWInterrupt(EXTI_Line4 | EXTI_Line11 | EXTI_Line12);
#endif



	
	///行线置为输出模式（PD.15）
	gpio_init.GPIO_Pin   = GPIO_Pin_15;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &gpio_init);	

	///PC.6, PC.7
	gpio_init.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &gpio_init);

	///每一根行线都输出低电平
	Keypad_ScanLine(0);
}

/**
* @brief   	Scan key pad lines
* @param[in]   none
* @return      none
*/
void Keypad_Int_Enable(void)
{
	NVIC_InitTypeDef	NVIC_InitStructure;
#ifdef HW_V1_00
	EXTI_ClearITPendingBit(EXTI_Line3);
#else
	EXTI_ClearITPendingBit(EXTI_Line3);
	EXTI_ClearITPendingBit(EXTI_Line4);
#endif
	EXTI_ClearITPendingBit(EXTI_Line11);
	EXTI_ClearITPendingBit(EXTI_Line12);
	

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    // 抢占式优先级别
#ifdef HW_V1_00
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
#else
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQChannel;
#endif
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/**
* @brief   	Scan key pad lines
* @param[in]   none
* @return      none
*/
static void Keypad_ScanLine(INT8U line)
{
	switch(line)
	{
	case 0:     //pull low  every line
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
		GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7);
		break;
	case 1:
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
		break;
	case  2:
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
		GPIO_SetBits(GPIOC,GPIO_Pin_6);
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
		break;
	case  3:
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
		GPIO_SetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_12);
		break;
	default:
		break;    
	}
}


/**
* @brief 读取键盘列值
*/
int key_readcol(void)
{
	int			value = 0;
	
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12))
		value	|= 0x04; 
#ifdef HW_V1_00
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3))
#else
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4))
#endif
		value	|= 0x02;
	if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_11))
		value	|= 0x01;

	return value;
}




//void led_r_ctrl(int on)
//{
//	if(on)
//	{
//		if (led_state & 0x10)
//		{
//			GPIO_SetBits(GPIOD, GPIO_Pin_1);	//先关闭蓝灯
//		}
//		GPIO_ResetBits(GPIOD, GPIO_Pin_0);
//		led_state |= 0x01;
//	}
//	else
//	{
//		if (led_state & 0x10)
//		{
//			GPIO_ResetBits(GPIOD, GPIO_Pin_1);	//再打开蓝灯
//		}
//		GPIO_SetBits(GPIOD, GPIO_Pin_0);
//		led_state &= ~0x01;
//	}
//}

void led_g_ctrl(int on)
{
	if(on)
	{
		//if (led_state & 0x01)
		//{
		//	GPIO_SetBits(GPIOD, GPIO_Pin_0);	//先关闭红灯
		//}
		GPIO_SetBits(GPIOD, GPIO_Pin_1);
		//led_state |= 0x10;
	}
	else
	{
		//if (led_state & 0x01)
		//{
		//	GPIO_ResetBits(GPIOD, GPIO_Pin_0);	//再打开红灯
		//}
		GPIO_ResetBits(GPIOD, GPIO_Pin_1);
		//led_state &= ~0x10;
	}
}

/**
* @brief 初始化按键
*/
void keypad_init(void)
{
	keypad_initport();
	Keypad_ScanLine(1);
	Keypad_ScanLine(2);
	Keypad_ScanLine(3);
	Keypad_ScanLine(0);
}


/**
* @brief 读取键盘值
*/
unsigned char *keypad_getkey(void)
{
	int i, value, value_new,n;

	if(POWER_KEY_PRESSED)
	{
		return (unsigned char*)&KeyTable[9];
	}
       
	value = key_readcol();
	
	if( value == 0x07)
		return 0;			// 无按键
	
	if(value != 0x03 && value!= 0x05 && value!=0x06)
		return 0;			// 多列的键被按下
	
	for(i=0; i<3; i++)
	{
		Keypad_ScanLine(i+1);
		//OSTimeDly(1);
		for(n = 0;n < 5000; n++);
		
		value_new = key_readcol();
		if(value == value_new)
		{
			Keypad_ScanLine(0);
			
			value &= 0x07;
			
			if(value == 0x06)
				return (unsigned char*)&KeyTable[i*3];
			if(value == 0x05)
				return (unsigned char*)&KeyTable[1+i*3];
			if(value == 0x03)
				return (unsigned char*)&KeyTable[2+i*3];
			return 0;
		}
	}
	Keypad_ScanLine(0);
	return 0;
	
}



void keypad_release(void)
{
	while( (key_readcol() & 0x07) != 0x07)
		OSTimeDly(2);
}

/**
* @brief 检查按键是否属于退出类型，如"Menu, Enter,Esc, Fun"
* @return 0:是退出类型
*        -1:不是退出类型
*/
int keypad_isexit(int key)
{
	if(key == KEY_ESC || key==KEY_ENTER)
	{
		return 0;
	}
	return -1;
}

/**
* @brief 检查按键是否数字键和清除键
*/
int keypad_isnum(int key)
{
	if(key==KEY_NUM1 || key==KEY_NUM2 || key==KEY_NUM3 || 
		key==KEY_NUM4 || key==KEY_NUM5 || key==KEY_NUM6 || 
		key==KEY_NUM7 || key==KEY_NUM8 || key==KEY_NUM9 || 
		key==KEY_NUM0 || key==KEY_CLEAR)
		return 0;
	return -1;
}

/**
* @brief 开机
*/
void PowerOn(void)
{
	//GPIO_ResetBits(GPIOA,POWER_ON);		//old HW Version
    GPIO_SetBits(GPIOA,POWER_ON);
	GPIO_SetBits(GPIOA,POWER_ON);
}

/**
* @brief 关机
*/
void PowerOff(void)
{
    while (1)
    {
		GPIO_ResetBits(GPIOA,POWER_ON);
    }
}

/**
* @brief 开蜂鸣器
* @param[in]  int cnt   蜂鸣器驱动脉冲的过数
* @note 延时是纯粹的软件延时，如果参数cnt太小，蜂鸣器会表现为没有声音发出来，建议cnt大于50000
*/
void Beep(int cnt)
{
	CPU_SR         cpu_sr;
	CPU_CRITICAL_ENTER();
    while(cnt)
    {
		GPIO_SetBits(GPIOD,Beep_CS);
		BeepDelay(3000);
		//GPIO_ResetBits(GPIOD,Beep_CS);
		//BeepDelay(1600);
		cnt--;
    }
	GPIO_ResetBits(GPIOD,Beep_CS);
	//BeepDelay(1600);
	CPU_CRITICAL_EXIT();
}

/**
* @brief 产生蜂鸣器驱动脉冲的延时函数  2.7KHZ
* @param[in]  int delay   
* @note 延时是纯粹的软件延时
*/
static void BeepDelay(unsigned int delay)
{
	do{
	}while(delay--);
}

//输入脉冲
void trip(int cnt)
{
	while(cnt--)
	{
		GPIO_SetBits(GPIOC,TRIP_PIN);
		BeepDelay(1);
		GPIO_ResetBits(GPIOC,TRIP_PIN);
		BeepDelay(1);
	}
}