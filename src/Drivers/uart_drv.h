
#ifndef _UART_DRV_H_
#define _UART_DRV_H_

// ==========================================================================================================
// 串口驱动管理接口

#define COMM1			0x01
#define COMM2			0x02
#define COMM3			0x03
#define COMM4			0x04
#define COMM5			0x05
#define COMMX			0x06		// 模拟串口
#define COMM_MAX		0x07

#define COMM_NO_ERR		0x00
#define COMM_TX_FULL	0xFE
#define COMM_BAD_CH		0xFF
#define COMM_TX_EMPTY	0xFD

typedef unsigned char (*CommIsrInByte)(unsigned char c);

void Comm_Init (void);																			//串口管理模块初始化
void Comm_SetReceiveProc(unsigned char ch, CommIsrInByte inbyte);								//设置接收回调函数
int Comm_InitPort(int ch, int baud,unsigned short parity);                                      //初始化串口，并打开中断
int Comm_AppIsFull(unsigned char ch);  															//应用程序调用，判断发送缓冲是否为空	
void Comm_AppSendBuffer(unsigned char ch, unsigned char *buffer, int len);						//应用程序调用此函数，通过串口发送数据
void Comm_IsrSendBytes(unsigned char ch);														//中断中调用，发送数据
void Comm_IsrPutRxChar (unsigned char ch, unsigned char c);										//中断程序调用此函数，将一个接收到的字符放到缓冲中

unsigned char Comm_IsrGetTxChar (unsigned char ch, unsigned char *err);                         //中断程序调用此函数，从发送缓冲中取一个字符发送出去


#endif




/******************* (C) COPYRIGHT 2010 Netcom *****END OF FILE****************/