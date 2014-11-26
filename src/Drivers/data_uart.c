/**
 * @file data_uart.c
 * @brief 
 *			串口1的驱动，系统提供给外部的串口，开发阶段可以用于调试
 * @version V0.0.1
 * @author zhongyh
 * @date 2009年12月28日
 * @note
 *
 * @copy
 *
 * 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
 * 本公司以外的项目。本司保留一切追究权利。
 *
 * <h1><center>&copy; COPYRIGHT 2009 netcom</center></h1>
 */
/* Private include -----------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_uart.h"
#include "keypad.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//#define DATA_UART_EN         GPIO_Pin_7		// PB7,数据串口控制


#if 0
/**
 * @brief	打开数据串口
 * @note	因为外部串口和PN532串口实际上是公用一个串口的，只是利用一个IO控制
*			一个模拟开关来复用的，此处实际上是使能数据串口这一路的模拟开关
 */
void ENABLE_DATA_UART(void)
{
	PN532_SWITCH_OFF();
	GPIO_SetBits(GPIOB, DATA_UART_EN);
}

/**
* @brief	关闭数据串口
* @note	因为外部串口和PN532串口实际上是公用一个串口的，只是利用一个IO控制
*			一个模拟开关来复用的，此处实际上是关闭数据串口这一路的模拟开关
*/
void DISABLE_DATA_UART(void)
{
	GPIO_ResetBits(GPIOB, DATA_UART_EN);
}
#endif

/**
* @brief 初始化数据串口
*/
void data_uart_init(void)
{
	USART_InitTypeDef						USART_InitStructure;
	GPIO_InitTypeDef						GPIO_InitStructure;
	NVIC_InitTypeDef						NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel		= USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	
	// 目标板使用PB.7切换到数据口
	//GPIO_InitStructure.GPIO_Pin				= DATA_UART_EN;
	//GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_Out_PP;
	//GPIO_Init(GPIOB, &GPIO_InitStructure);
	//GPIO_ResetBits(GPIOB, DATA_UART_EN);

//#ifndef BOOTCODE_EXIST		//如果没有引导代码	
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 设置串口参数								*/
	USART_InitStructure.USART_BaudRate		= 115200;
	USART_InitStructure.USART_WordLength	= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;
	USART_InitStructure.USART_Parity		= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode			= USART_Mode_Tx | USART_Mode_Rx;
	//	USART_InitStructure.USART_Clock		= USART_Clock_Disable;
	//	USART_InitStructure.USART_CPOL			= USART_CPOL_Low;
	//	USART_InitStructure.USART_CPHA			= USART_CPHA_2Edge;
	//	USART_InitStructure.USART_LastBit		= USART_LastBit_Disable;
	USART_Init(USART1, &USART_InitStructure);
	
	
	// 接收和发送中断
	//	USART_ITConfig(USART1, USART_IT_TXE,  ENABLE);
	//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
//#endif	
	
	//ENABLE_DATA_UART();
	/* Enable USART1							*/
	
}

/**
 * @brief	发送一个字节
 */
void data_uart_sendbyte(unsigned char data)
{
	USART_SendData(USART1, (unsigned short)data);
}

/**
 * @brief	接收一个字节
 */
unsigned char uart_rec_byte(void)
{
	int	i = 0;
	while((USART_GetFlagStatus(USART1,USART_FLAG_RXNE)== RESET)&&(i<400000))
	{
		i++;
	}
	if (i == 400000) 
	{
		return 0x55;
	}
	return  USART_ReceiveData(USART1) & 0xFF;              /* Read one byte from the receive data register         */
}

/**
* @brief 实现此函数可以利用系统函数printf,方便调试时格式输出调试信息
*/
int fputc(int ch, FILE *f)
{
	//ENABLE_DATA_UART();
	/* Write a character to the USART */
	
	USART_SendData(USART1, (u8) ch);
	
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
	{
	}
	//	DISABLE_DATA_UART;
	return ch;        
}
