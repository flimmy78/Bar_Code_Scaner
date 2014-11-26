
#ifndef _UART_DRV_H_
#define _UART_DRV_H_

// ==========================================================================================================
// ������������ӿ�

#define COMM1			0x01
#define COMM2			0x02
#define COMM3			0x03
#define COMM4			0x04
#define COMM5			0x05
#define COMMX			0x06		// ģ�⴮��
#define COMM_MAX		0x07

#define COMM_NO_ERR		0x00
#define COMM_TX_FULL	0xFE
#define COMM_BAD_CH		0xFF
#define COMM_TX_EMPTY	0xFD

typedef unsigned char (*CommIsrInByte)(unsigned char c);

void Comm_Init (void);																			//���ڹ���ģ���ʼ��
void Comm_SetReceiveProc(unsigned char ch, CommIsrInByte inbyte);								//���ý��ջص�����
int Comm_InitPort(int ch, int baud,unsigned short parity);                                      //��ʼ�����ڣ������ж�
int Comm_AppIsFull(unsigned char ch);  															//Ӧ�ó�����ã��жϷ��ͻ����Ƿ�Ϊ��	
void Comm_AppSendBuffer(unsigned char ch, unsigned char *buffer, int len);						//Ӧ�ó�����ô˺�����ͨ�����ڷ�������
void Comm_IsrSendBytes(unsigned char ch);														//�ж��е��ã���������
void Comm_IsrPutRxChar (unsigned char ch, unsigned char c);										//�жϳ�����ô˺�������һ�����յ����ַ��ŵ�������

unsigned char Comm_IsrGetTxChar (unsigned char ch, unsigned char *err);                         //�жϳ�����ô˺������ӷ��ͻ�����ȡһ���ַ����ͳ�ȥ


#endif




/******************* (C) COPYRIGHT 2010 Netcom *****END OF FILE****************/