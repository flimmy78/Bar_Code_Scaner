/**
* @file		calendar.c
* @brief	�ն�ʵʱʱ��ά����ģ��
* @author   joe
* @version  v1.00
* @date     2009.10.15
* @note    
*/

#include "stm32f10x_lib.h"
#include "sd2068.h"
#include "calendar.h"

/**
 * @def���ǵȴ�LSE����ĳ�ʱʱ��
 * @note ��ֵ�����㹻�󣬲��ܱ�֤�ȵ���/LSE����Ȼ����ܽ���RTC�����ó���.
 *		
 */
#define LSEStartUp_TimeOut   0xffffff   


#ifdef  TAMPER_DETECT_ENABLE
static void Tamper_IRQ_Enable(void);
static void Tamper_setup(void);
#endif

/**
 * @brief  ϵͳ�ĵ�ǰ����ʱ��
 */
TypedefDateTime       currentDateTime;	
	

/**
* @brief  ϵͳ�Ƿ����ֵı��
*/
#ifdef  TAMPER_DETECT_ENABLE
unsigned int	tamper_event_flag;	
#endif

/**
* @brief  ϵͳ��RTC�����Ƿ�OK�ı��
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
//        Tamper_setup();           //���¿��������ǵ�һ�ο���������Ĺ���
//        Tamper_IRQ_Enable();      //����������ж�
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
//		// ����ʱ��
//		SD2068A_SetTime((unsigned char*)&currentDateTime);		//���ó�ʼʱ��Ϊ2010.01.01 00 00 00
//
//
//		// ������ȡʱ��
//		SD2068A_GetTime((unsigned char*)&currentDateTime);
//
//		// ʱ�䲻�ԣ���ΪRTC����
//		if(currentDateTime.year + 2000 != 2010 )
//		{
//			RTC_OK_flag = 0;		//��λRTC���ϵı��
//		}
//
//
//		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//	}
//	else
	{
		
#ifdef  TAMPER_DETECT_ENABLE	 
		Tamper_IRQ_Enable();      //����������ж�
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
			// ����ʱ��
			SD2068A_SetTime((unsigned char*)&currentDateTime);		//���ó�ʼʱ��Ϊ2010.01.01 00 00 00


			// ������ȡʱ��
			SD2068A_GetTime((unsigned char*)&currentDateTime);

			// ʱ�䲻�ԣ���ΪRTC����
			if(currentDateTime.year + 2000 != 2010 )
			{
				RTC_OK_flag = 0;		//��λRTC���ϵı��
			}

		}
	}
}


/**
* @brief      �򵥵ļ��ϵͳ����ʱ��ĸ�ʽ
* @param[in]  none
* @param[out] none
* @return     none
* @note    û�м������ �´���С������     
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
* @brief      ���RTC��ȡ��ʱ���ʽ�����⣬����Dummyʱ��
* @param[in]  none
* @param[out] none
* @return     none
* @note    û�м������ �´���С������     
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
* @brief      ����ϵͳ����ʱ��
* @param[in]  unsigned char * timebuf  ������µ�ʱ�仺����
* @param[out] none
* @return     none
* @note       
*       ����������Ҫ��ʵ����ݵĻ����ϼ�ȥ2000
*/
void SetDateTime(void)
{
	if(SD2068A_SetTime((unsigned char*)&currentDateTime))
	{
		if (SD2068A_SetTime((unsigned char*)&currentDateTime))
		{
			RTC_OK_flag = 0;		//��λRTC�����쳣���
		}
	}
}

/**
* @brief     ��ȡϵͳ����ʱ��
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
* @brief     ϵͳʱ��ת��ΪBCD��
* @param[out] BYTE * time_bcd
* @return    none
* @note                    
*/
void Systime2BCD(unsigned char * time_bcd)
{
	//time_bcd[0] = (((currentDateTime.year + 2000)/1000) << 4) + ((currentDateTime.year + 2000)%1000)/100;	 //0x20
	//time_bcd[1] = ((((currentDateTime.year + 2000)%100)/10) << 4) + (currentDateTime.year + 2000)%10;	 //0x10

	//Ϊ������ٶȣ���������ݵĸ���λ�̶�Ϊ0x20�������ᵼ������ϵͳʱ�䳬��2100��ʱ��ϵͳʱ����ʾ����
	time_bcd[0] = 0x20;
	time_bcd[1] = ((currentDateTime.year/10) << 4) + currentDateTime.year%10;	 //0x10
	time_bcd[2] = ((currentDateTime.month/10) << 4) + currentDateTime.month%10;	 //0x05
	time_bcd[3] = ((currentDateTime.day/10) << 4) + currentDateTime.day%10;	 //0x11

	time_bcd[4] = ((currentDateTime.hour/10) << 4) + currentDateTime.hour%10;
	time_bcd[5] = ((currentDateTime.min/10) << 4) + currentDateTime.min%10;
	time_bcd[6] = ((currentDateTime.sec/10) << 4) + currentDateTime.sec%10;
}

/**
* @brief	ʹ�����ּ����ж�
* @param[in]  none
* @param[out] none
* @return     none
* @note                   
*/
#ifdef  TAMPER_DETECT_ENABLE
static void Tamper_IRQ_Enable(void)
{
    NVIC_InitTypeDef                  NVIC_InitStructure; 
    //����Tamper Detect���ж�����
    NVIC_InitStructure.NVIC_IRQChannel = TAMPER_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority	= 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    
    //Enable the Tamper event interrupt                   
    BKP_ITConfig(ENABLE);                                 //BKP_CSR.TPIE = 1 ����Tamper���ŵ������ж�
}

/**
* @brief	���ּ����жϷ�����
* @param[in]  none
* @param[out] none
* @return     none
* @note       ����ϵͳ�����е�״���£�������������            
*/
void TAMPER_IRQ_Handle(void)
{
    if(BKP_GetITStatus() != RESET )         //����ǲ�����Tamper�ж�
    {
        /* Allow access to BKP Domain */
        PWR_BackupAccessCmd(ENABLE);
        //Clear Tamper Event flag
        BKP_ClearFlag();                                      //BKP_CSR.TEF = 0  ������ݿ���/״̬�Ĵ����������¼����
		
        //Clear Tamper interrupt falg
        BKP_ClearITPendingBit();                              //BKP_CSR.TIF = 0  ������ݿ���/״̬�Ĵ����������жϱ��
		
        //Disable access to BKP Domain                
        //PWR_BackupAccessCmd(DISABLE);
		
        tamper_event_flag = 1;
		
        if ((BKP_ReadBackupRegister(BKP_DR1) == 0) &&
			(BKP_ReadBackupRegister(BKP_DR2) == 0))           //�����ϲ������ж϶���������ݱ����������ݵ�
        {
            //@to do ....
            //�˴���Ӽ�⵽�����¼���Ĵ�����룬��V����Խ������ʱ���¼��NM���򣬲����ؼ�������
			
        }
        else    //�����������û����������߱������ݻ�û�����úã���ôʲô������
        {
			;
        }
	}
}

/**
* @brief	���ּ�⹦���趨
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
	BKP_TamperPinLevelConfig(BKP_TamperPinLevel_High);     //BKP_CR.TPAL = 0  Tamper�����ϵĸߵ�ƽ������������ݱ��ݼĴ���
	//BKP_TamperPinLevelConfig(BKP_TamperPinLevel_Low);      //BKP_CR.TPAL = 1  Tamper�����ϵĵ͵�ƽ������������ݱ��ݼĴ���
	
	//Enalbe Tamper detect function
	BKP_TamperPinCmd(ENABLE);                             //BKP_CR.TPE = 1  ����Tamper���ŵ������⹦��
	
	//Clear Tamper Event flag
	BKP_ClearFlag();                                      //BKP_CSR.TEF = 0  ������ݿ���/״̬�Ĵ����������¼����
	
	//Clear Tamper interrupt falg
	BKP_ClearITPendingBit();                              //BKP_CSR.TIF = 0  ������ݿ���/״̬�Ĵ����������жϱ��
	
	//�ڱ��ݼĴ�������һ����ǣ������⵽���룬��ôBKP_DR1�ᱻ�����
	//BKP_WriteBackupRegister(BKP_DR1,0xa5a5);        //����ŵ�RTC�ĳ�ʼ��������ȥ����          
	
	// @to do .....
	//�ڴ˴���ϵͳ�Ĺؼ����ݽ��г�ʼ���������⵽���룬��ôϵͳ����ʱ���ڴ˴���������NVM�еĹؼ�������ʼ���ˡ�
}
#endif


