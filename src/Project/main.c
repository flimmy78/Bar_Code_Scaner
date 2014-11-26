/**
* @file main.c
* @brief 2.4GPOS��Ŀ������
*
* @version V0.0.1
* @author joe
* @date 2010��4��26��
* @note
*		none
*
* @copy
*
* �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
* ����˾�������Ŀ����˾����һ��׷��Ȩ����
*
* <h1><center>&copy; COPYRIGHT 2009 netcom</center></h1>
*/

/* Private Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "ucos_ii.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "app.h"
#include "lcd.h"
#include "data_uart.h"
#include "gui.h"
#include "keypad.h"
#include "dialog.h"
#include "res_spi.h"
#include "res_upgrade.h"
#include "TimeBase.h"
#include "calendar.h"
#include "uart_drv.h"
#include "Terminal_para.h"
#include "power_detect.h"
#include "record.h"
#include "stm32f10x_flash_config.h"
#include "uE988_scan.h"
#include "pic.h"
#include "data_transfer.h"
#include "usb_lib.h"
#include "PCUsart.h"
//#include "http_protocol.h"
//#include "em310_driver.h"
#include "JMemory.h"
#include "usb_pwr.h"
/* Private define ------------------------------------------------------------*/
#define BOOTCODE_EXIST		//Enable this Macro for Release,Disable for Debug; 

// Cortex System Control register address
#define SCB_SysCtrl					((u32)0xE000ED10)
// SLEEPDEEP bit mask
#define SysCtrl_SLEEPDEEP_Set		((u32)0x00000004)

//define the stack size of each task
#define STACK_SIZE_TASKBG			512
#define STACK_SIZE_TASKUI			512
#define STACK_SIZE_TASKKEY			256

#define MAX_KEY_NUM		8


/* Global variables ---------------------------------------------------------*/
ErrorStatus			HSEStartUpStatus;							//Extern crystal OK Flag
unsigned int		pos_state;									//POS state flag
//unsigned char		factory_test_start_flag;


OS_STK				task_backgroud_stk[STACK_SIZE_TASKBG];		//the stack of the task_backgroud
OS_STK				task_ui_stk[STACK_SIZE_TASKUI];				//the stack of the task_ui
OS_STK				task_key_stk[STACK_SIZE_TASKKEY];			//the stack of the task_keyboard

void				*KeyPool[MAX_KEY_NUM];			// keyvalue message pool
OS_EVENT			*pKeyOSQ;						// get a key value Semaphore
OS_EVENT			*pKeypressedEvent;				//any key pressed  event pointer ---communication between ISR and task_keyboard
// 0x20000000-0x20000010 ���ֽ�Ϊ��������BootCode��Ĳ���������
__no_init unsigned int status_iap @ "VENDOR_RAM";
__no_init unsigned int nouse[3] @ "VENDOR_RAM";

/* Private functions ---------------------------------------------------------*/
static void Unconfigure_All(void);
static void GPIO_AllAinConfig(void);
static void task_background(void *pp);
static void task_keyboard(void *pp);
void u_disk_proc(void);

int bluetooth_module_state;		//����ģ���״̬

/* External variables -----------------------------------------------*/
extern	TTerminalPara			g_param;					//Terminal Param

/**
* @brief �ϵ���һ�������������̣߳��������Ӳ���ĳ�ʼ����ϵͳ�ĳ�ʼ����������Ӧ��״̬����Ȼ�󴴽������̣߳�
*  �������������̴߳��������񣬲���������ִ�н����
*/
static void task_background(void *pp)
{
	int				ret,i = 0;
	int				time_out = 0;
	//unsigned char   *key = NULL;

	//unsigned char vrh,bt,vcoml,vcomh,vmf;
	//unsigned char dis_str[21];
	//unsigned char *var;


	OS_CPU_SysTickInit();

	// Clear SLEEPDEEP bit of Cortex System Control Register
	*(vu32 *) SCB_SysCtrl &= ~SysCtrl_SLEEPDEEP_Set;

	Unconfigure_All();

	// ��ʼ�����ڹ���ģ��
	Comm_Init();

	// ���ݴ���(���Կ�)��ʼ��
	data_uart_init();						

	//��ʼ��ʱ������
	TimeBase_Init();

	// ��ʼ�����̣���������LED��
	keypad_init();	

	//DTU_Init_Configuration();						//��ʼ������

	//EM310_PowerOn();		//�˴��ͽ�GPRS�ĵ�Դ��
    //    trip(1);
	//��������
	OSTimeDlyHMSM(0,0,0,50);
	PowerOn();
	Beep(BEEP_DELAY);
	JMemory_init();

	//��ʼ��POS����״̬��
	pos_state = 0;

	//factory_test_start_flag = 0;//��ʼ���������Ա�־

#ifdef  BOOTCODE_EXIST
	// ���ROM�Ƿ񱻶�������û����ִ�ж���������
	if( FLASH_GetReadOutProtectionStatus() != SET )
	{
		FLASH_Unlock();
		FLASH_ReadOutProtection(ENABLE);
	}
#endif      

	//��ʼ��RTCģ��
	Calendar_Init();

	// ��ʼ��GUI�ӿ�
	gui_init();
        
	//Lcd_test();
	// ��ʾStart Up
	//gui_TextOut(0, i, "System startup...",0,1); i+=13;

	//gui_TextOut(0, i, "Load resource...",0,1); i+= 13;

	//���ϵͳ����Դ�Ƿ��Ѿ�װ��OK
	if (res_init()!=0)		
	{
		//��Դ���ʧ��
		//gui_SetTextColor(BG_COLOR);
		gui_TextOut(0, 50, "Load resource failed",0,1); i+=13;

		goto sys_err_handle;
		//pos_state = STA_FACTORY_MODE;
	}

	gui_PictureOut(16,8,PIC_LOGO,1);


	// ��ʼ��PN532�ӿ�
	//PN532_Port_Init();

	//��������ģ���ʼ��
	dlg_init();

	OSTimeDlyHMSM(0,0,0,200);
#if 1
	//���ϵͳ������SPI SD�е��ն˲����Ƿ�Ϸ�
	ret = ReadTerminalPara();
	if (ret)
	{
		//�������Ϸ�����ϵͳ�ָ�Ϊ��������
		if(DefaultTerminalPara())
		{
			gui_clear(BG_COLOR);
			//gui_TextOut(16, 50, "ϵͳ��������ʧ��",0);
			//gui_TextOutEx(CENTER_ALIGN, 26, "Load System Param",0,1);
			gui_TextOut(0, 50, "Load SysParam failed",0,1);
			goto sys_err_handle;
		}
	}
#endif

	//��ʼ���洢��¼��ģ��
	if(record_module_init())
	{
		gui_clear(BG_COLOR);
		//gui_TextOut_ext(CENTER_ALIGN,60 , "�����Ƿ����",0); // y += get_dlg_fontsize();
		//gui_TextOut_ext(CENTER_ALIGN,73 , "�洢��",0);
		//gui_TextOutEx(CENTER_ALIGN,26 , "Record module Init",0,1);
		gui_TextOut(0,50 , "Record Init failed",0,1);
		goto sys_err_handle;
	}

	if (recover_record_by_logfile())
	{
		gui_clear(BG_COLOR);
		//gui_TextOut_ext(CENTER_ALIGN,60 , "��¼�ָ�ʧ��",0); // y += get_dlg_fontsize();
		//gui_TextOutEx(CENTER_ALIGN,26 , "Records recovery",0,1); // y += get_dlg_fontsize();
		gui_TextOut(0,50 , "Recovery failed",0,1);
		goto sys_err_handle;
	}
       
	//����������Ķ���
	UE988_Init(g_param.decoder_switch_map);

	//����ģ���ʼ��
	bluetooth_module_state = YFBT07_init();
        //YFBT07_Test();

	//��ʼ����ص�ѹ����Լ����״̬����ģ��
	InitPowerDetectModule();  
	
	////��ʼ��
	device_init_by_setting();
	if ((USBLink_Insert_Detect() == 0) && (g_param.transfer_mode == TRANSFER_MODE_U_DISK))
	{
		while((bDeviceState != CONFIGURED)&&(time_out < 15))
		{
			time_out++;
			OSTimeDlyHMSM(0,0,0,100);
		}

		if (bDeviceState == CONFIGURED)
		{
			u_disk_proc();
		}
	}
	OSTimeDlyHMSM(0,0,0,100);
	// ������ʾ������     
	// �������̼���ź���
	pKeyOSQ				= OSQCreate(&KeyPool[0], MAX_KEY_NUM);	// ����������Ϣ����
	pKeypressedEvent	= OSSemCreate(0);					//���������¼��ź���
	Keypad_Int_Enable();									//ʹ�ܼ��̵��жϹ���


	// ��ʾ�ӽ���
	OSTaskCreateExt(task_ui,
		(void *)0,
		&task_ui_stk[STACK_SIZE_TASKUI-1],
		7,
		7,
		&task_ui_stk[0],
		STACK_SIZE_TASKUI,
		(void *)0,
		(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	// �����ӽ���
	OSTaskCreateExt(task_keyboard,
		(void *)0,
		&task_key_stk[STACK_SIZE_TASKKEY-1],
		8,
		8,
		&task_key_stk[0],
		STACK_SIZE_TASKKEY,
		(void *)0,
		(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	// ������ʾ�����ʾ
	for(;;)
	{
		dlg_run_task();
		OSTimeDlyHMSM(0,0,0,100);
	}


sys_err_handle:

	OSSchedLock();

	USB_Cable_Config(1);
	g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
	usb_device_init(USB_MASSSTORAGE);

	while (1)
	{
		if( *keypad_getkey() == KEY_POWER)
		{
			Beep(BEEP_DELAY);
			USB_Cable_Config(0);
			//bDeviceState = UNCONNECTED;
			OSSchedUnlock();
			NVIC_SETFAULTMASK();
			NVIC_GenerateSystemReset();
		}

		Delay(50000);
	}
}

/**
* @brief ˢ����ʾ����ʾ������
*/
static void task_keyboard(void *pp)
{
	unsigned char * key;
	unsigned char					err_code;

	for(;;){		

		OSSemPend(pKeypressedEvent, 0, &err_code);	//�ڴ˴�����Ƿ��а����¼��������ź��������ź������ⲿ�ж�����Post������
		if(err_code == OS_NO_ERR)
		{
			key = keypad_getkey();
			if(key)
			{
				OSQPost(pKeyOSQ, (void*)key);               
				if(*key == KEY_POWER)
				{
					StartDelay(150);
					while((DelayIsEnd()!= 0)&&(*(keypad_getkey()) == KEY_POWER));
					if(DelayIsEnd() == 0)
					{   
						OSSchedLock();		//��Ҫ�ٽ������������
						Beep(BEEP_DELAY);
						Lcd_Clear(BG_COLOR);
						//gui_FillRect(0, 31, 240, GUI_HEIGHT/2-30, BG_COLOR);
						//gui_SetTextColor(0);
						//gui_SetFont(48);
						gui_TextOut_ext(CENTER_ALIGN, 26, "POWER OFF",0,1);                         
						while(*(keypad_getkey()) == KEY_POWER)
						{
							;
						}
						//�ͷ�POWER��֮���������ٹػ�
						Lcd_Clear(BG_COLOR);
						PowerOff();      ///�ػ�
					}
				}
				else                  
				{    
					if ((g_param.beeper_vol)&&(*key != KEY_SCAN))
					{
						Beep(BEEP_DELAY);                    
					}
					keypad_release();
				}
			}
			else if ((USBLink_Insert_Detect() == 0)&&(g_param.transfer_mode == TRANSFER_MODE_U_DISK)&&(bDeviceState == CONFIGURED))
			{
				u_disk_proc();
			}
		}
	}
}
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	/* System Clocks Configuration **********************************************/
	RCC_Configuration(); 
#ifdef BOOTCODE_EXIST
	/* NVIC Configuration *******************************************************/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, IAP_SIZE);		//��Ҫ���ܵ� bootcode
#else	
	/* NVIC Configuration *******************************************************/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
#endif
	memset(task_backgroud_stk, 0xAA, sizeof(task_backgroud_stk));
	memset(task_ui_stk, 0xAA, sizeof(task_ui_stk));
	memset(task_key_stk, 0xAA, sizeof(task_key_stk));

	status_iap = 0;		//��ʼ��������Ӧ�ó����BootLoader֮�䴫�ݲ�����RAM����

	OSInit();

	OSDebugInit();

	OSTaskCreateExt(task_background,
		(void *)0,
		&task_backgroud_stk[STACK_SIZE_TASKBG-1],
		6,
		6,
		&task_backgroud_stk[0],
		STACK_SIZE_TASKBG,
		(void *)0,
		(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	OSStart();
}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{   
	vu32 i=0;
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSE							*/
	RCC_HSEConfig(RCC_HSE_ON);
	// ����Ҫ����ʱ�����ܼ���ĳЩ�Ƚϲ�ľ��壬�Ա�˳������	
	for(i=0; i<200000; i++);

	/* Wait till HSE is ready			*/
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{
		/* Enable Prefetch Buffer		*/
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		/* Flash 2 wait state			*/
		FLASH_SetLatency(FLASH_Latency_2);

		/* HCLK = SYSCLK					*/
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 

		/* PCLK2 = HCLK					*/
		RCC_PCLK2Config(RCC_HCLK_Div1); 

		/* PCLK1 = HCLK/2					*/
		RCC_PCLK1Config(RCC_HCLK_Div2);

		/* PLLCLK = 12MHz * 6 = 72 MHz	*/
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);

		/* PLLCLK = 8MHz * 8 = 64 MHz	*/
		//RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul9);

		/* Enable PLL						*/
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready		*/
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
}

/*******************************************************************************
* Function Name  : Unconfigure_All
* Description    : set all the RCC data to the default values 
*                  configure all the GPIO as input
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Unconfigure_All(void)
{
	//RCC_DeInit();

	/* RCC configuration */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);

	GPIO_AllAinConfig();
}


/*******************************************************************************
* Function Name  : GPIO_AllAinConfig
* Description    : Configure all GPIO port pins in Analog Input mode 
*                  (floating input trigger OFF)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void GPIO_AllAinConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************
* Function Name  : EnterLowPowerMode()
************************************************/
void EnterLowPowerMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//CC1101_DeInitport();
	
	//�ر�����
	led_g_ctrl(0);

	//�ȹر����������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ALL, DISABLE);

	//������Ҫ����PWRģ���ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // Enable PWR clock


	//����͹���ģʽ
	EXTI_ClearFlag(0xffff);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}

/************************************************
* Function Name  : ExitLowPowerMode()
************************************************/
void ExitLowPowerMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//��������ʱ��
	RCC_Configuration();

	//����ϵͳ��ʹ�õ���ģ���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |
		RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1|
		RCC_APB2Periph_SPI1, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_SPI2 | RCC_APB1Periph_USB | RCC_APB1Periph_PWR, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_CRC, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	//if(CC1101_Init() == 0)
	//{
	//	//����CC1101ģ�����վƥ���Ƶ��
	//	CC1101_Set_Frequency(g_param.cc1101_freq);
	//	CC1101_Module_Close();
	//}

	if ((g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)||(g_param.batch_transfer_mode == TRANSFER_MODE_BLUETOOTH))
	{
		led_g_ctrl(1);
	}

}

extern unsigned char Lcd_RAM[8][128];
extern vu32 bDeviceState;

//����U��ģʽ
void u_disk_proc(void)
{
	unsigned char  *key;
	unsigned char	*p_lcd_ram_save;
	OSSchedLock();
	
	p_lcd_ram_save = Jmalloc(1024);
	if (p_lcd_ram_save)
	{
		memcpy(p_lcd_ram_save,Lcd_RAM,1024);
	}
	
	gui_FillRect(0,0,LCD_WIDTH,LCD_HEIGHT,BG_COLOR,0);
	gui_PictureOut(48,14,PIC_USBLINK,0);
	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	
	while(USBLink_Insert_Detect() == 0)
	{	
		if(bDeviceState != CONFIGURED)
		{
			break;
		}

		Delay(2000);
	}

	OSSchedUnlock();

	if (p_lcd_ram_save)
	{
		memcpy(Lcd_RAM,p_lcd_ram_save,1024);
		Jfree(p_lcd_ram_save);
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
}

/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/

//void assert_failed(u8* file, u32 line)
//{ 
/* User can add his own implementation to report the file name and line number,
ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

/* Infinite loop */
//while (1)
//{
//}
//}
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

