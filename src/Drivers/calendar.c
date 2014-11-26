/**
* @file		calendar.c
* @brief	终端实时时钟维护的模块
* @author   joe
* @version  v1.00
* @date     2009.10.15
* @note    
*/

#include "stm32f10x_lib.h"
#include "sd2068.h"
#include "calendar.h"

/**
 * @def这是等待LSE起振的超时时限
 * @note 此值必须足够大，才能保证等到了/LSE起振，然后才能进入RTC的配置程序.
 *		
 */
#define LSEStartUp_TimeOut   0xffffff   


#ifdef  TAMPER_DETECT_ENABLE
static void Tamper_IRQ_Enable(void);
static void Tamper_setup(void);
#endif

/**
 * @brief  系统的当前日期时间
 */
TypedefDateTime       currentDateTime;	
	

/**
* @brief  系统是否被入侵的标记
*/
#ifdef  TAMPER_DETECT_ENABLE
unsigned int	tamper_event_flag;	
#endif

/**
* @brief  系统的RTC功能是否OK的标记
*/
unsigned int	RTC_OK_flag;

/**
* @brief      Initializes calendar application.
* @param[in]  none
* @param[out] none
* @return     none
* @note                    
*/
void Calendar_Init(void)
{
	/* PWR and BKP clocks selection --------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);

	RTC_OK_flag = 1;
	SD2068A_Initial();

//	if(BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
//	{
//#ifdef  TAMPER_DETECT_ENABLE
//		tamper_event_flag = 1;
//        Tamper_setup();           //重新开启或者是第一次开启侵入检测的功能
//        Tamper_IRQ_Enable();      //打开侵入检测的中断
//#endif
//		Beep(800);
//
//		currentDateTime.sec					= 0x00;
//		currentDateTime.min					= 0x00;
//		currentDateTime.hour					= 0x00;
//		currentDateTime.week					= 0x00;
//		currentDateTime.day						= 0x01;
//		currentDateTime.month					= 0x01;
//		currentDateTime.year					= 2010 - 2000;
//		// 设置时间
//		SD2068A_SetTime((unsigned char*)&currentDateTime);		//设置初始时间为2010.01.01 00 00 00
//
//
//		// 立即获取时间
//		SD2068A_GetTime((unsigned char*)&currentDateTime);
//
//		// 时间不对，即为RTC故障
//		if(currentDateTime.year + 2000 != 2010 )
//		{
//			RTC_OK_flag = 0;		//置位RTC故障的标记
//		}
//
//
//		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//	}
//	else
	{
		
#ifdef  TAMPER_DETECT_ENABLE	 
		Tamper_IRQ_Enable();      //打开侵入检测的中断
#endif

		SD2068A_GetTime((unsigned char*)&currentDateTime);
		if (currentDateTime.year + 2000 > 2200)
		{
			currentDateTime.sec					= 0x00;
			currentDateTime.min					= 0x00;
			currentDateTime.hour				= 0x00;
			currentDateTime.week				= 0x00;
			currentDateTime.day					= 0x01;
			currentDateTime.month				= 0x01;
			currentDateTime.year				= 2010 - 2000;
			// 设置时间
			SD2068A_SetTime((unsigned char*)&currentDateTime);		//设置初始时间为2010.01.01 00 00 00


			// 立即获取时间
			SD2068A_GetTime((unsigned char*)&currentDateTime);

			// 时间不对，即为RTC故障
			if(currentDateTime.year + 2000 != 2010 )
			{
				RTC_OK_flag = 0;		//置位RTC故障的标记
			}

		}
	}
}


/**
* @brief      简单的检查系统日期时间的格式
* @param[in]  none
* @param[out] none
* @return     none
* @note    没有检查闰年 月大月小的问题     
*/
static int check_time_format(void)
{
	if (currentDateTime.sec >= 60)
	{
		return -1;
	}

	if (currentDateTime.min >= 60)
	{
		return -1;
	}

	if (currentDateTime.hour >= 24)
	{
		return -1;
	}

	if (currentDateTime.day > 31)
	{
		return -1;
	}

	if (currentDateTime.month > 12)
	{
		return -1;
	}

	return 0;

}

/**
* @brief      如果RTC获取的时间格式有问题，构造Dummy时间
* @param[in]  none
* @param[out] none
* @return     none
* @note    没有检查闰年 月大月小的问题     
*/
static int dummy_time_format(void)
{
	while(currentDateTime.sec >= 60)
	{
		currentDateTime.sec -= 60;
		currentDateTime.min++;
	}

	while(currentDateTime.min >= 60)
	{
		currentDateTime.min -= 60;
		currentDateTime.hour++;
	}

	while(currentDateTime.hour >= 24)
	{
		currentDateTime.hour -= 24;
		currentDateTime.day++;
	}

	while(currentDateTime.day > 31)
	{
		currentDateTime.day -= 31;
		currentDateTime.month++;
	}

	while(currentDateTime.month > 12)
	{
		currentDateTime.month -= 12;
		currentDateTime.year++;
	}

	return 0;

}


/**
* @brief      设置系统日期时间
* @param[in]  unsigned char * timebuf  输入的新的时间缓冲区
* @param[out] none
* @return     none
* @note       
*       输入的年份需要在实际年份的基础上减去2000
*/
void SetDateTime(void)
{
	if(SD2068A_SetTime((unsigned char*)&currentDateTime))
	{
		if (SD2068A_SetTime((unsigned char*)&currentDateTime))
		{
			RTC_OK_flag = 0;		//置位RTC功能异常标记
		}
	}
}

/**
* @brief     获取系统日期时间
* @param[in]  none
* @param[out] none
* @return     none
* @note                    
*/
void GetDateTime(void)
{
	unsigned char cnt = 0;
	do 
	{
		if (SD2068A_GetTime((unsigned char*)&currentDateTime))
		{
			if (SD2068A_GetTime((unsigned char*)&currentDateTime))
			{
				RTC_OK_flag = 0;
			}
		}

		cnt++;
	} while (check_time_format() && (cnt < 4));


	if (cnt == 4)
	{
		dummy_time_format();
	}

}
/**
* @brief     系统时间转换为BCD码
* @param[out] BYTE * time_bcd
* @return    none
* @note                    
*/
void Systime2BCD(unsigned char * time_bcd)
{
	//time_bcd[0] = (((currentDateTime.year + 2000)/1000) << 4) + ((currentDateTime.year + 2000)%1000)/100;	 //0x20
	//time_bcd[1] = ((((currentDateTime.year + 2000)%100)/10) << 4) + (currentDateTime.year + 2000)%10;	 //0x10

	//为了提高速度，将上面年份的高两位固定为0x20，这样会导致设置系统时间超过2100年时，系统时间显示错误。
	time_bcd[0] = 0x20;
	time_bcd[1] = ((currentDateTime.year/10) << 4) + currentDateTime.year%10;	 //0x10
	time_bcd[2] = ((currentDateTime.month/10) << 4) + currentDateTime.month%10;	 //0x05
	time_bcd[3] = ((currentDateTime.day/10) << 4) + currentDateTime.day%10;	 //0x11

	time_bcd[4] = ((currentDateTime.hour/10) << 4) + currentDateTime.hour%10;
	time_bcd[5] = ((currentDateTime.min/10) << 4) + currentDateTime.min%10;
	time_bcd[6] = ((currentDateTime.sec/10) << 4) + currentDateTime.sec%10;
}

/**
* @brief	使能入侵检测的中断
* @param[in]  none
* @param[out] none
* @return     none
* @note                   
*/
#ifdef  TAMPER_DETECT_ENABLE
static void Tamper_IRQ_Enable(void)
{
    NVIC_InitTypeDef                  NVIC_InitStructure; 
    //设置Tamper Detect的中断向量
    NVIC_InitStructure.NVIC_IRQChannel = TAMPER_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority	= 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    
    //Enable the Tamper event interrupt                   
    BKP_ITConfig(ENABLE);                                 //BKP_CSR.TPIE = 1 允许Tamper引脚的侵入中断
}

/**
* @brief	入侵检测的中断服务函数
* @param[in]  none
* @param[out] none
* @return     none
* @note       处理系统在运行的状况下，出现侵入的情况            
*/
void TAMPER_IRQ_Handle(void)
{
    if(BKP_GetITStatus() != RESET )         //如果是产生了Tamper中断
    {
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);
        //Clear Tamper Event flag
        BKP_ClearFlag();                                      //BKP_CSR.TEF = 0  清除备份控制/状态寄存器的侵入事件标记
		
        //Clear Tamper interrupt falg
        BKP_ClearITPendingBit();                              //BKP_CSR.TIF = 0  清除备份控制/状态寄存器的侵入中断标记
		
        //Disable access to BKP Domain                
        //PWR_BackupAccessCmd(DISABLE);
		
        tamper_event_flag = 1;
		
        if ((BKP_ReadBackupRegister(BKP_DR1) == 0) &&
			(BKP_ReadBackupRegister(BKP_DR2) == 0))           //理论上产生了中断都会清掉数据备份区的数据的
        {
            //@to do ....
            //此处添加检测到侵入事件后的处理代码，比V如可以将侵入的时间记录到NM区域，擦除关键的数据
			
        }
        else    //如果备份数据没有清掉，或者备份数据还没有设置好，那么什么都不做
        {
			;
        }
	}
}

/**
* @brief	入侵检测功能设定
* @param[in]  none
* @param[out] none
* @return     none
* @note                  
*/
static void Tamper_setup(void)
{
	/* PWR and BKP clocks selection ------------------------------------------*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	
	/* Allow access to BKP Domain */
	PWR_BackupAccessCmd(ENABLE);
	
	//Set Tamper Event Low Level active 
	BKP_TamperPinLevelConfig(BKP_TamperPinLevel_High);     //BKP_CR.TPAL = 0  Tamper引脚上的高电平会清除所有数据备份寄存器
	//BKP_TamperPinLevelConfig(BKP_TamperPinLevel_Low);      //BKP_CR.TPAL = 1  Tamper引脚上的低电平会清除所有数据备份寄存器
	
	//Enalbe Tamper detect function
	BKP_TamperPinCmd(ENABLE);                             //BKP_CR.TPE = 1  开启Tamper引脚的侵入检测功能
	
	//Clear Tamper Event flag
	BKP_ClearFlag();                                      //BKP_CSR.TEF = 0  清除备份控制/状态寄存器的侵入事件标记
	
	//Clear Tamper interrupt falg
	BKP_ClearITPendingBit();                              //BKP_CSR.TIF = 0  清除备份控制/状态寄存器的侵入中断标记
	
	//在备份寄存器中设一个标记，如果检测到侵入，那么BKP_DR1会被清掉的
	//BKP_WriteBackupRegister(BKP_DR1,0xa5a5);        //这个放到RTC的初始化函数中去做了          
	
	// @to do .....
	//在此处对系统的关键数据进行初始化，如果检测到侵入，那么系统开机时会在此处将保存在NVM中的关键参数初始化了。
}
#endif


