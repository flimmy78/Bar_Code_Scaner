/**
 * @file YFBT07.c
 * @brief 蓝牙模块的驱动 
 * @version 1.0
 * @author joe
 * @date 2013年10月25日
 * @note
 *   蓝牙模块YFBT07的驱动，SPI接口，通讯协议是基于《蓝牙键盘模块通讯协议.docx》
 * 
 */

#include "stm32f10x_lib.h"
#include "YFBT07.h"
#include "basic_fun.h"
#include "keypad.h"
#include "os_cpu.h"
#include "gui.h"

//LA 57648

/**
*@def 定义了与蓝牙模块连接的硬件资源的配置及IO操作的宏定义
*/

#define YFBT07_SPI_PORT					SPI1					//与蓝牙模块连接的是SPI1接口
#define YFBT07_SPI_PORT_CLK				RCC_APB2Periph_SPI1
#define ENABLE_YFBT07_SPI_PORT_CLK()	RCC_APB2PeriphClockCmd(YFBT07_SPI_PORT_CLK, ENABLE)

#define YFBT07_SPI_SCK		GPIO_Pin_5	//PA5
#define YFBT07_SPI_MISO		GPIO_Pin_6	//PA6
#define YFBT07_SPI_MOSI		GPIO_Pin_7	//PA7

#define YFBT07_SPI_INT		GPIO_Pin_13	//PD13		STM32需要与YFBT07模块进行通讯时就需要拉低此信号，一旦拉低此信号蓝牙模块就会输出SPI时钟信号，并发送数据过来
#define YFBT07_STATUS		GPIO_Pin_14	//PD14		此IO指示了蓝牙模块与主机的连接状态，未连接到蓝牙主机时输出低电平，连接到蓝牙主机时输出高电平
#define YFBT07_POWER_ENABLE	GPIO_Pin_9	//PC9		控制蓝牙模块电源的IO

#define YFBT07_SPI_INT_LOW()       GPIO_ResetBits(GPIOD, YFBT07_SPI_INT)
#define YFBT07_SPI_INT_HIGH()      GPIO_SetBits(GPIOD, YFBT07_SPI_INT)

#define YFBT07_POWER_ON()			GPIO_ResetBits(GPIOC, YFBT07_POWER_ENABLE)
#define YFBT07_POWER_OFF()			GPIO_SetBits(GPIOC, YFBT07_POWER_ENABLE)

#define LOW_LEVEL		0
#define HIGH_LEVEL		1

//#define GET_YFBT07_SPI_SCK_LEVEL()		GPIO_ReadInputDataBit(GPIOA,YFBT07_SPI_SCK)
//#define GET_YFBT07_SPI_MOSI_LEVEL()		GPIO_ReadInputDataBit(GPIOA,YFBT07_SPI_MOSI)

#define GET_YFBT07_SPI_SCK_LEVEL()		(((*(volatile unsigned long *) 0x40010808) & 0x0020) != 0)
#define GET_YFBT07_SPI_MOSI_LEVEL()		(((*(volatile unsigned long *) 0x40010808) & 0x0080) != 0)

//#define SET_YFBT07_SPI_MISO()			GPIO_SetBits(GPIOA,YFBT07_SPI_MISO)
//#define RESET_YFBT07_SPI_MISO()			GPIO_ResetBits(GPIOA,YFBT07_SPI_MISO)

#define SET_YFBT07_SPI_MISO()		(*((volatile unsigned long *) 0x40010810) = YFBT07_SPI_MISO)
#define RESET_YFBT07_SPI_MISO()		(*((volatile unsigned long *) 0x40010814) = YFBT07_SPI_MISO)

//检测蓝牙模块是否已经与蓝牙主机建立起连接
#define GET_YFBT07_CONNECT_STATE()		GPIO_ReadInputDataBit(GPIOD,YFBT07_STATUS)
/**
* @brief  初始化连接蓝牙模块的硬件IO资源 
* @param  None
* @retval : 0 成功
*          -1 失败
*/
static int YFBT07_HW_init(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* Enable SPI1 and GPIO clocks */
	//ENABLE_YFBT07_SPI_PORT_CLK();	//使能与蓝牙模块通讯的SPI接口的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	/* Configure SPI1 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure I/O for SPI_INT */
	GPIO_InitStructure.GPIO_Pin = YFBT07_SPI_INT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	YFBT07_SPI_INT_HIGH();

	/* Configure I/O for YFBT07_POWER_ENABLE */
	GPIO_InitStructure.GPIO_Pin = YFBT07_POWER_ENABLE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	YFBT07_POWER_OFF();

	/* Configure I/O for SPI_STATUS */
	GPIO_InitStructure.GPIO_Pin = YFBT07_STATUS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	/* SPI1 configuration */
	//从机模式
	//SPI_Cmd(YFBT07_SPI_PORT, DISABLE);
	//SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	//SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	//SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		
	//SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	//SPI_InitStructure.SPI_CRCPolynomial = 7;
	//SPI_Init(YFBT07_SPI_PORT, &SPI_InitStructure);

	//SPI_NSSInternalSoftwareConfig(YFBT07_SPI_PORT, SPI_NSSInternalSoft_Reset);
	///* Enable SPI1  */
	//SPI_Cmd(YFBT07_SPI_PORT, ENABLE);
	return 0;										// 成功
}

//利用IO模拟SPI接口，发送一个字节到蓝牙模块
static unsigned char SendByte2YFBT07(unsigned char data)
{
	unsigned char  i,tmp = 0;
	unsigned int	cnt = 0;
	OS_CPU_SR  cpu_sr = 0;

	OS_ENTER_CRITICAL();
	SET_YFBT07_SPI_MISO();
	for(i = 0; i < 8;i++)
	{
		//while(GET_YFBT07_SPI_SCK_LEVEL() == HIGH_LEVEL);
		while(GET_YFBT07_SPI_SCK_LEVEL()&&(cnt<100000))
                {
                  cnt++;
                }
		if (data&(0x80>>i))
		{
			SET_YFBT07_SPI_MISO();
		}
		else
		{
			RESET_YFBT07_SPI_MISO();
		}
		//while(GET_YFBT07_SPI_SCK_LEVEL() == LOW_LEVEL);
		cnt = 0;
                while(!GET_YFBT07_SPI_SCK_LEVEL()&&(cnt<100000))
                {
                  cnt++;
                }
	}
	
	SET_YFBT07_SPI_MISO();

	OS_EXIT_CRITICAL();
        
    return tmp;
}

//发送数据到蓝牙模块
inline static unsigned char SendByteArray2YFBT07(unsigned char *data,unsigned int len)
{
	unsigned char i;
	volatile unsigned char tmp;

	YFBT07_SPI_INT_LOW();
	for(i = 0;i < len;i++)
	{
		//YFBT07_SPI_PORT->DR = data[i];		//此语句会清除TXE  TXE = 0；
		//while((YFBT07_SPI_PORT->SR && (1<<1))==0);	 //等待发送缓冲区空，理论上此处应该会检测SPI主机的时钟信号，在时钟信号的同步下将发送缓冲区的数据发送出去，发送完会置位TXE，使TXE = 1；
		//tmp = (unsigned char)YFBT07_SPI_PORT->DR;	 //清除RXNE标志，同时接收到主机发送过来的数据

		SendByte2YFBT07(data[i]);
	}
	YFBT07_SPI_INT_HIGH();
}


//测试YFBT07模块的SPI通讯是否ok，只要拉低INT信号，可以检测到YFBT07模块输出的时钟信号，即认为模块工作正常
int YFBT07_check(void)
{
	int cnt = 0;
#if 1
	RESET_YFBT07_SPI_MISO();
	YFBT07_SPI_INT_LOW();
	//while((GET_YFBT07_SPI_SCK_LEVEL() == HIGH_LEVEL)&&(cnt < 100000))		//检测时钟信号的到来
	while((GET_YFBT07_SPI_SCK_LEVEL())&&(cnt < 100000))		//检测时钟信号的到来
	{
		cnt++;
	}

	YFBT07_SPI_INT_HIGH();
        //SET_YFBT07_SPI_MISO();
        
	if (cnt == 100000)
	{
		return -1;
	}
#endif
	return 0;

}


/**
* @brief   延时
* @author joe
* @param[in] vu16 nCount   延时值
* @return none
* @note  
*      调用方法：Lcd_Delay(100);
*/
static void YFBT07_Delayms(vu16 nCount)
{
	u32  i;
	for (i = 0; i < nCount*6000; i++) {
		;
	}
}


#define READ_YFBT07_DUMMY_BYTE		0xFF

/**
* @brief  初始化蓝牙模块 
* @param  None
* @retval : 0 成功
*          -1 失败
*/
int YFBT07_init(void)
{
	unsigned char status;

	YFBT07_HW_init();		//初始化硬件资源
	YFBT07_POWER_ON();		//开启蓝牙模块的电源

	YFBT07_Delayms(2000);	//200ms 延时一段时间等待模块上电进入正常的工作状态，此等待时间待测试
	
	//测试蓝牙模块是否工作正常，其实只能检测SPI的通信是否正常
	if (YFBT07_check())
	{
		return -1;
	}
	return 0;
}

//控制蓝牙模块的电源
void YFBT07_power_ctrl(unsigned char on_off)
{
	if (on_off)
	{
		YFBT07_POWER_ON();
		YFBT07_SPI_INT_HIGH();
		SET_YFBT07_SPI_MISO();
	}
	else
	{
		YFBT07_POWER_OFF();	
		YFBT07_SPI_INT_LOW();
		RESET_YFBT07_SPI_MISO();
	}
	YFBT07_Delayms(500);
}

/**
* @brief  使蓝牙模块进入待配对状态 
* @param  None
* @retval : 0 成功
*          -1 失败
*/
int YFBT07_Enter_Match_Mode(void)
{
	unsigned char status;
	unsigned int	timeout = 0;
	OS_CPU_SR  cpu_sr = 0;

	OS_ENTER_CRITICAL();
	//发送使蓝牙模块进入待配对状态的命令
	YFBT07_SPI_INT_LOW();
	SendByte2YFBT07(0x5a);
	SendByte2YFBT07(0xa5);
	YFBT07_SPI_INT_HIGH();
	OS_EXIT_CRITICAL();
	return 0;
}

static unsigned char calc_check_value(unsigned char *buf,unsigned int len)
{
	unsigned char i;
	unsigned char xor = 0;

	for(i = 0;i < len;i++)
	{
		xor ^= buf[i];
	}

	return xor;
}

/**
* @brief  利用蓝牙模块是否与蓝牙主机建立了连接
* @param  None
* @retval : 0		断开状态
*           1		连接状态
*/
int YFBT07_Connect_state(void)
{
	return (int)GET_YFBT07_CONNECT_STATE();
}

/**
* @brief  利用蓝牙模块发送键值 
* @param  None
* @retval : 0 成功
*          -1 失败
*/
int YFBT07_SendKey(unsigned char *key_buffer,unsigned int len)
{
	unsigned char report_buf[11];
	unsigned char send_num = 0;

	report_buf[0] = 0x55;
	while(len)
	{
		send_num = ascii_to_keyreport2_ext(key_buffer,len,report_buf+1);	//已构造的键值报表
		
		report_buf[10] = calc_check_value(report_buf,10);

        SendByteArray2YFBT07(report_buf,11);

		//发送释放键
        //memset(report_buf+1,0,9);
	//	report_buf[10] = 0x55;
      
	//	SendByteArray2YFBT07(report_buf,11);

		YFBT07_Delayms(20);
                
        len -= send_num;
        key_buffer += send_num;
	}
	return 0;
}


//测试蓝牙模块
void YFBT07_Test(void)
{
	int  i,timeout = 0;
	unsigned char *key;
	unsigned char buf[50];
	unsigned char offset = 0;
	unsigned char test_state = 0;

	gui_clear(BG_COLOR);
	gui_TextOut(0,0,"蓝牙测试",0,1);

	while(1)
	{
		if (YFBT07_Connect_state() == 0)
		{
            gui_FillRect(0,13,GUI_WIDTH,GUI_HEIGHT-13,BG_COLOR,1);            
			gui_TextOut(0,13,"待匹配状态...",0,1);

			//如果蓝牙模块还没有与蓝牙注解建立起连接
			//发送命令使模块进入配对状态，等待蓝牙主机与其连接
            //可以先等待5S，什么都不做，如果5S之后仍旧没有连接到主机，再发送配对命令
			while(YFBT07_Connect_state() == 0)
			{
				if (timeout < 30000)
				{
					timeout++;
				}

				if (timeout == 30000)
				{
					timeout = 0;
					break;	//超时时间内还没有与蓝牙主机建立起连接，退出此循环，重新发送配对命令
				}

				YFBT07_Delayms(1);
			}

			YFBT07_Enter_Match_Mode();
		}
		else
		{
			if (test_state == 0)
			{
				gui_TextOut(0,13,"已连接状态...",0,1);
				gui_TextOut(0,26,"按[SCAN]键发送键值",0,1);
				//gui_TextOut(0,39,"此次发送键值:",0,1);
				key = keypad_getkey();
				if (*key == KEY_SCAN)
				{
					Beep(BEEP_DELAY);
					for (i = 0;i < 10;i++)
					{
						buf[i] = 0x3a+i;
					}
					buf[10] = 0;

					//gui_TextOut(0,52,"                    ",1,1);
					//gui_TextOut(0,52,buf,0,1);
					YFBT07_SendKey(buf,10);
					YFBT07_SendKey("\x0d",1);
					test_state = 1;
					gui_FillRect(0,13,GUI_WIDTH,GUI_HEIGHT-13,BG_COLOR,1);
				}
			}

			if (test_state == 1)
			{
				gui_TextOut(0,26,"蓝牙模块测试OK",0,1);
				gui_TextOut(0,39,"按[SCAN]键继续测试",0,1);
				if (*key == KEY_SCAN)
				{
					Beep(BEEP_DELAY);
					return;
				}
			}
			
			/*else if (*key == KEY_DOWN)
			{
				for (i = 0; i < 47; i++)
				{
					buf[i] = 0x20+i;
				}
				YFBT07_SendKey(buf,47);
			}
			else if (*key == KEY_UP)
			{
				for (i = 0; i < 48; i++)
				{
					buf[i] = 0x20+47+i;
				}
				YFBT07_SendKey(buf,48);
			}*/
		}
		YFBT07_Delayms(100);
	}
	
}