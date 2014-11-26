/**
* @file main.c
* @brief 2.4GPOS项目主程序
*
* @version V0.0.1
* @author joe
* @date 2010年4月26日
* @note
*		none
*
* @copy
*
* 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
* 本公司以外的项目。本司保留一切追究权利。
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
// 0x20000000-0x20000010 此字节为主程序与BootCode间的参数传递区
__no_init unsigned int status_iap @ "VENDOR_RAM";
__no_init unsigned int nouse[3] @ "VENDOR_RAM";

/* Private functions ---------------------------------------------------------*/
static void Unconfigure_All(void);
static void GPIO_AllAinConfig(void);
static void task_background(void *pp);
static void task_keyboard(void *pp);
void u_disk_proc(void);

int bluetooth_module_state;		//蓝牙模块的状态

/* External variables -----------------------------------------------*/
extern	TTerminalPara			g_param;					//Terminal Param

/**
* @brief 上电后第一个运行起来的线程，首先完成硬件的初始化，系统的初始化，设置相应的状态机，然后创建其它线程，
*  可以运行其它线程创建的任务，并返回任务执行结果。
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

	// 初始化串口管理模块
	Comm_Init();

	// 数据串口(调试口)初始化
	data_uart_init();						

	//初始化时基函数
	TimeBase_Init();

	// 初始化键盘，蜂鸣器和LED口
	keypad_init();	

	//DTU_Init_Configuration();						//初始化配置

	//EM310_PowerOn();		//此处就将GPRS的电源打开
    //    trip(1);
	//长按开机
	OSTimeDlyHMSM(0,0,0,50);
	PowerOn();
	Beep(BEEP_DELAY);
	JMemory_init();

	//初始化POS机的状态机
	pos_state = 0;

	//factory_test_start_flag = 0;//初始化生产测试标志

#ifdef  BOOTCODE_EXIST
	// 检查ROM是否被读保护，没有则执行读保护操作
	if( FLASH_GetReadOutProtectionStatus() != SET )
	{
		FLASH_Unlock();
		FLASH_ReadOutProtection(ENABLE);
	}
#endif      

	//初始化RTC模块
	Calendar_Init();

	// 初始化GUI接口
	gui_init();
        
	//Lcd_test();
	// 显示Start Up
	//gui_TextOut(0, i, "System startup...",0,1); i+=13;

	//gui_TextOut(0, i, "Load resource...",0,1); i+= 13;

	//检查系统的资源是否已经装载OK
	if (res_init()!=0)		
	{
		//资源检查失败
		//gui_SetTextColor(BG_COLOR);
		gui_TextOut(0, 50, "Load resource failed",0,1); i+=13;

		goto sys_err_handle;
		//pos_state = STA_FACTORY_MODE;
	}

	gui_PictureOut(16,8,PIC_LOGO,1);


	// 初始化PN532接口
	//PN532_Port_Init();

	//窗体运行模块初始化
	dlg_init();

	OSTimeDlyHMSM(0,0,0,200);
#if 1
	//检查系统保存在SPI SD中的终端参数是否合法
	ret = ReadTerminalPara();
	if (ret)
	{
		//参数不合法，将系统恢复为出厂设置
		if(DefaultTerminalPara())
		{
			gui_clear(BG_COLOR);
			//gui_TextOut(16, 50, "系统参数创建失败",0);
			//gui_TextOutEx(CENTER_ALIGN, 26, "Load System Param",0,1);
			gui_TextOut(0, 50, "Load SysParam failed",0,1);
			goto sys_err_handle;
		}
	}
#endif

	//初始化存储记录的模块
	if(record_module_init())
	{
		gui_clear(BG_COLOR);
		//gui_TextOut_ext(CENTER_ALIGN,60 , "请检查是否插入",0); // y += get_dlg_fontsize();
		//gui_TextOut_ext(CENTER_ALIGN,73 , "存储卡",0);
		//gui_TextOutEx(CENTER_ALIGN,26 , "Record module Init",0,1);
		gui_TextOut(0,50 , "Record Init failed",0,1);
		goto sys_err_handle;
	}

	if (recover_record_by_logfile())
	{
		gui_clear(BG_COLOR);
		//gui_TextOut_ext(CENTER_ALIGN,60 , "记录恢复失败",0); // y += get_dlg_fontsize();
		//gui_TextOutEx(CENTER_ALIGN,26 , "Records recovery",0,1); // y += get_dlg_fontsize();
		gui_TextOut(0,50 , "Recovery failed",0,1);
		goto sys_err_handle;
	}
       
	//检测条形码阅读器
	UE988_Init(g_param.decoder_switch_map);

	//蓝牙模块初始化
	bluetooth_module_state = YFBT07_init();
        //YFBT07_Test();

	//初始化电池电压检测以及充电状态检测的模块
	InitPowerDetectModule();  
	
	////初始化
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
	// 进入显示主进程     
	// 创建进程间的信号量
	pKeyOSQ				= OSQCreate(&KeyPool[0], MAX_KEY_NUM);	// 建立按键消息队列
	pKeypressedEvent	= OSSemCreate(0);					//建立按键事件信号量
	Keypad_Int_Enable();									//使能键盘的中断功能


	// 显示子进程
	OSTaskCreateExt(task_ui,
		(void *)0,
		&task_ui_stk[STACK_SIZE_TASKUI-1],
		7,
		7,
		&task_ui_stk[0],
		STACK_SIZE_TASKUI,
		(void *)0,
		(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	// 键盘子进程
	OSTaskCreateExt(task_keyboard,
		(void *)0,
		&task_key_stk[STACK_SIZE_TASKKEY-1],
		8,
		8,
		&task_key_stk[0],
		STACK_SIZE_TASKKEY,
		(void *)0,
		(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));

	// 接收显示命令并显示
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
* @brief 刷新显示屏显示的任务
*/
static void task_keyboard(void *pp)
{
	unsigned char * key;
	unsigned char					err_code;

	for(;;){		

		OSSemPend(pKeypressedEvent, 0, &err_code);	//在此处检测是否有按键事件发生的信号量，此信号量在外部中断里面Post进来的
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
						OSSchedLock();		//不要再进行任务调度了
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
						//释放POWER键之后，先清屏再关机
						Lcd_Clear(BG_COLOR);
						PowerOff();      ///关机
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
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, IAP_SIZE);		//需要加密的 bootcode
#else	
	/* NVIC Configuration *******************************************************/
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
#endif
	memset(task_backgroud_stk, 0xAA, sizeof(task_backgroud_stk));
	memset(task_ui_stk, 0xAA, sizeof(task_ui_stk));
	memset(task_key_stk, 0xAA, sizeof(task_key_stk));

	status_iap = 0;		//初始化用于在应用程序和BootLoader之间传递参数的RAM变量

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
	// 这里要做延时，才能兼容某些比较差的晶体，以便顺利起震	
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
	
	//关闭外设
	led_g_ctrl(0);

	//先关闭所有外设的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ALL, DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL, DISABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ALL, DISABLE);

	//但是需要开启PWR模块的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); // Enable PWR clock


	//进入低功耗模式
	EXTI_ClearFlag(0xffff);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}

/************************************************
* Function Name  : ExitLowPowerMode()
************************************************/
void ExitLowPowerMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//重新配置时钟
	RCC_Configuration();

	//开启系统中使用到的模块的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD |
		RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1|
		RCC_APB2Periph_SPI1, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_SPI2 | RCC_APB1Periph_USB | RCC_APB1Periph_PWR, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_CRC, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	//if(CC1101_Init() == 0)
	//{
	//	//设置CC1101模块与基站匹配的频段
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

//进入U盘模式
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

