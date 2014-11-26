/**
* @file uart_drv.c
* @brief ������������
*	��ģ��Ϊ��������ģ�飬���й��������ڵ��շ�����
*
* @version V0.0.3
* @author xhh
* @date 2011��11��18��
* @note
*    fixed: ����bug--(TXE��־�ϵ��Ĭ��Ϊ1�����Կ���ʱ���ܴ�TXE���жϣ�����������
*    modify:���������ڳ�ʼ����Ĭ��TXE/TC �жϹرգ��򿪽����жϣ�Ӧ�õ��÷���ʱ���Ŵ�TXE/TC �жϣ������ر�

* @version V0.0.2
* @author zhongyh
* @date 2010��8��17��
* @note
*    ���϶�ģ�⴮�ڵ�֧�֣���Ҫ analog_uart.c
*    ɾ���˶Բ���ϵͳ������
*    ɾ����R3964Э�鴦����
*    ������ uart_device ���Ż��ٶ�
*
* @version V0.0.1
* @author zhongyh
* @date 2009��10��2��
* @note
*    first release,֧�ִ���1������2
*    
*
*
*/
#include "stm32f10x_lib.h"
#include "uart_drv.h"
#include <string.h>

/**
* @brief ���ڷ��͹���buffer
*/
typedef struct {
	unsigned char					*pTxbuffer;			// Ҫ���͵Ļ���
	int								TxbufferSize;		// Ҫ���͵����ݳ���
	int								tx_finish;			// ��ʶ�Ƿ�������������  1:���������  0��δ���
	CommIsrInByte					InByte;				// ���ڽ��յ�һ�ֽڵĻص�����
}COMM_RING_BUF;


static COMM_RING_BUF				Comm1Buf;
static COMM_RING_BUF				Comm2Buf;
static COMM_RING_BUF				Comm3Buf;
static COMM_RING_BUF				Comm4Buf;
static COMM_RING_BUF				Comm5Buf;
static COMM_RING_BUF				CommXBuf;			// ģ�⴮��

static COMM_RING_BUF * uart_device[] = 
{
   
	&Comm1Buf,
	&Comm2Buf,
	&Comm3Buf,
	&Comm4Buf,
	&Comm5Buf,
	&CommXBuf,
};



/**
****************************************************
* @brief		��	USART�ж�����ֽڷ��� 
* @param[in]	��	none				
* @return		��	
* @note			��	�˺������ж��е��� 
****************************************************
*/
void Comm_IsrSendBytes(unsigned char ch)
{
	unsigned char		dat;
	unsigned char		err;

	dat			=	Comm_IsrGetTxChar(ch, &err);				//�ӷ��ͻ�����ȡ��һ�����ݲ����ͳ�ȥ

	switch(ch)
	{
	case COMM1:
		if( COMM_NO_ERR == err )									//�Ƿ������
		{
			USART_SendData(USART1, dat);
		}
		else
		{
			
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM2:
		if( COMM_NO_ERR  == err )									//�Ƿ������
		{
			USART_SendData(USART2, dat);	
		}
		else
		{
			
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM3:
		if( COMM_NO_ERR  == err )									//�Ƿ������
		{
			USART_SendData(USART3, dat);
		}
		else
		{
			//�ر�TXE�жϵ�ʹ��
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM4:
		if( COMM_NO_ERR  == err )									//�Ƿ������
		{
			USART_SendData(UART4, dat);	
		}
		else
		{	
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM5:
		//if( COMM_NO_ERR  == err )									//�Ƿ������
		//{
		//	USART_SendData(UART5, dat);
		//}
		//else
		//{
		//	USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
		//}
		break;
	default:
		break;
	}
}



/**
*********************************************************************************************************  
* @brief �жϳ�����ô˺������ӷ��ͻ�����ȡһ���ַ����ͳ�ȥ
* Arguments   : 'ch'    is the COMM port channel number and can either be:  
*                           COMM1,COMM2,COMM3,COMM4,COMM5
*               'err'   is a pointer to where an error code will be deposited:  
*                           *err is set to COMM_NO_ERR         if at least one character was left in the  
*                                                              buffer.  
*                           *err is set to COMM_TX_EMPTY       if the Tx buffer is empty.  
*                           *err is set to COMM_BAD_CH         if you have specified an incorrect channel  
* @return     : The next character in the Tx buffer or NUL if the buffer is empty.  
*********************************************************************************************************  
*/
unsigned char Comm_IsrGetTxChar (unsigned char ch, unsigned char *err)
{
	unsigned char					c;
	COMM_RING_BUF					*pbuf;

	*err							= COMM_NO_ERR;

	if( ch >= COMM_MAX )
	{
		*err						= COMM_BAD_CH;
		return 0;
	}

	pbuf							= uart_device[ch-1];

	if(pbuf->pTxbuffer != (unsigned char*)0 )
	{

		if (pbuf->TxbufferSize>0)
		{
			c						= *pbuf->pTxbuffer++;
			*err					= COMM_NO_ERR;
			pbuf->TxbufferSize --;
			return c;
		}
		else
		{
			if( pbuf->TxbufferSize == 0)
				pbuf->TxbufferSize	= -1;
			pbuf->tx_finish			= 1;
			*err					= COMM_TX_EMPTY;
			return 0;
		}
	}
	*err							= COMM_TX_EMPTY;
	return 0;
}

/**
***************************************************************************
*@brief	���ڹ���ģ���ʼ��
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
void Comm_Init (void)
{
	memset(&Comm1Buf,0,sizeof(Comm1Buf));
	memset(&Comm2Buf,0,sizeof(Comm2Buf));
	memset(&Comm3Buf,0,sizeof(Comm3Buf));
	memset(&Comm4Buf,0,sizeof(Comm4Buf));
	memset(&Comm5Buf,0,sizeof(Comm5Buf));
	memset(&CommXBuf,0,sizeof(CommXBuf));

	Comm1Buf.tx_finish =1;
	Comm2Buf.tx_finish =1;
	Comm3Buf.tx_finish =1;
	Comm4Buf.tx_finish =1;
	Comm5Buf.tx_finish =1;
	CommXBuf.tx_finish =1;
	
}

/**
* @brief Ӧ�ó�����ã��жϷ��ͻ����Ƿ�Ϊ��
* @param[in] 
* @return 0:��
*         1:��
* @note 
*/
int Comm_AppIsFull (unsigned char ch)
{
	int								empty;
	COMM_RING_BUF					*pbuf;

	if( ch >= COMM_MAX )
	{
		return 0;
	}
	pbuf		=	uart_device[ch-1];

	if( pbuf->tx_finish )
	{	
		empty	=	0;					// ��������� ����
	}
	else
	{	
		empty	=	1;					// ����δ��ɡ��ǿ�
	}
	return empty;
}


/**
* @brief �жϳ�����ô˺�������һ�����յ����ַ��ŵ�������

*/
void Comm_IsrPutRxChar (unsigned char ch, unsigned char c)
{
	COMM_RING_BUF					*pbuf;

	// �˺���Ϊ�Ż���û�жԲ����İ�ȫ���!!!

	pbuf							= uart_device[ch-1];

	if (pbuf->InByte)
		(pbuf->InByte)(c);
}


/**
* @brief Ϊָ���Ĵ����ж����ô����жϽ��պ���
* @param[in] unsigned char ch:COMM1,COMM2,COMM3,COMM4,COMM5
* @param[in] CommIsrInByte inbyte �жϴ�����
note:(1) USART_IT_TXE  �����Ȳ��򿪣�ʹ�õ�ʱ���ٴ򿪣�ʹ����ر�,�临λֵĬ��Ϊ1,�������жϣ�
		 ��Ҫ���͵�ʱ���ٴ򿪣�˳������д��DR��Ȼ��USART_ITConfig(USART2, USART_IT_TXE, ENABLE);�ο���stm32�ο��ĵ���
     (2) USART_ClearITPendingBit(USART2, USART_IT_TXE);�˾�������
	 (3) USART_ClearITPendingBit(USART3, USART_IT_RXNE)�����(void)USART_ReceiveData(USART3)����---�ο���stm32�ο��ĵ���
	  
*/
void Comm_SetReceiveProc(unsigned char ch, CommIsrInByte inbyte)
{
	if( ch == COMM1 )
	{
		Comm1Buf.TxbufferSize		= 0;
		Comm1Buf.InByte				= inbyte;
		(void)USART_ReceiveData(USART1);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	}
	else if(ch == COMM2)
	{
		Comm2Buf.TxbufferSize		= 0;
		Comm2Buf.InByte				= inbyte;
		(void)USART_ReceiveData(USART2);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
	else if(ch == COMM3)
	{	//���ܿ�
		Comm3Buf.TxbufferSize		= 0;
		Comm3Buf.InByte				= inbyte;
		
		(void)USART_ReceiveData(USART3);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	}
	else if(ch == COMM4)
	{	// COMM4
		Comm4Buf.TxbufferSize		= 0;
		Comm4Buf.InByte				= inbyte;
		
		(void)USART_ReceiveData(UART4);
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	}
	else if(ch == COMM5)
	{	// COMM5
		Comm5Buf.TxbufferSize		= 0;
		Comm5Buf.InByte				= inbyte;
		//(void)USART_ReceiveData(UART5);
		//USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
	}
	else
	{
		// ģ�⴮�ڣ��������κ�����
	}
}

/**
* @brief Ӧ�ó�����ô˺�����ͨ�����ڷ�������
* @param[in] unsigned char ch ���ڵ�ͨ����������COMM1,COMM2,COMM3,COMM4,COMM5
* @param[in] unsigned char *buffer Ҫ���͵����ݻ���
* @param[in] int len Ҫ���͵����ݳ���
* @param[in] CommIsrInByte inbyte �������ݵĻص�����
*/
void Comm_AppSendBuffer(unsigned char ch, unsigned char *buffer, int len)
{
	COMM_RING_BUF					*pbuf;

	// �������ַ�Ҫ����
	if ( len <= 0 )
	{
		return;
	}

	switch (ch)
	{ /* Obtain pointer to communications channel */
	case COMM1:
		pbuf = &Comm1Buf;
		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		break;
	case COMM2:
		pbuf = &Comm2Buf;
 		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
 		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		break;
	case COMM3:
		pbuf = &Comm3Buf;
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);	
		break;
	case COMM4:
		pbuf = &Comm4Buf;
		USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
		USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
		break;
	//case COMM5:
	//	pbuf = &Comm1Buf;
	//	USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
	//	USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
		//break;
	case COMMX:
		pbuf = &CommXBuf;
		break;

	default:
		return;
	}

	// ��䷢�ͽṹ
	pbuf->pTxbuffer				= buffer+1;
	pbuf->TxbufferSize			= len-1;
	pbuf->tx_finish				= 0;

	// ���͵�һ���ַ������򿪷����жϣ�Ȼ�󷵻ء�
	// ���жϳ����лὫʣ��������͵�����
	switch(ch)
	{
	case COMM1:
		USART_SendData(USART1, *buffer);
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		(void)USART_ReceiveData(USART1);
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		break;
	case COMM2:
		USART_SendData(USART2, *buffer);
 		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		(void)USART_ReceiveData(USART2);
 		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		break;

	case COMM3:
		//���򿪽����ж�,�Ƚ���TC�жϺ��
		USART_SendData(USART3, *buffer);
		USART_ClearITPendingBit(USART3, USART_IT_TC);
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
		break;
	case COMM4:
		USART_SendData(UART4, *buffer);
		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
		(void)USART_ReceiveData(UART4);
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
		break;

	case COMM5:
		//USART_SendData(UART5, *buffer);
		//USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
		//(void)USART_ReceiveData(UART5);
		//USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
		break;

	case COMMX:
		// todo ģ�⴮��
		// �������͵�һ���ֽڣ������������ж�
//		analoguart_sendbyte(*buffer);
		break;

	default:
		return;
	}
}

/**
***************************************************************************
*@brief	��ʼ�����ڣ����򿪴��ڡ�������ж������ûص��ĺ����д�
*@param[in] parity:0  ��У�� no
                   1  ��У�� odd
				   2  żУ�� even
*@return 
*@warning
*@see	
*@note COMMX 
***************************************************************************
*/
int Comm_InitPort(int ch, int baud,unsigned short parity)
{
	USART_InitTypeDef				USART_InitStructure;
	NVIC_InitTypeDef				NVIC_InitStructure;
	GPIO_InitTypeDef				GPIO_InitStructure; 

	unsigned short usart_parity;
	unsigned short usart_wordlength;
	switch (parity)
	{
	case 1:
		usart_wordlength =USART_WordLength_9b;//��У��
		usart_parity=USART_Parity_Odd;
		break;
	case 2:
		usart_wordlength =USART_WordLength_9b;//żУ��
		usart_parity=USART_Parity_Even;
		break;
	default:
		usart_wordlength =USART_WordLength_8b;
		usart_parity=USART_Parity_No;
		break;
	}

	/* 1 bit for pre-emption priority, 3 bits for subpriority */
//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	if( ch == COMM1 )
	{
          // ��ʼ���ж�
		NVIC_InitStructure.NVIC_IRQChannel		= USART1_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//��ռ���ȼ�
		//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;				//�����ȼ�

		NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
		NVIC_Init(&NVIC_InitStructure);
                
                
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure USART1 Rx (PA.10) as input floating */
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		// ��ʼ������
		USART_InitStructure.USART_BaudRate		= baud;										// 
		USART_InitStructure.USART_WordLength	= usart_wordlength;						 
		USART_InitStructure.USART_StopBits		= USART_StopBits_1;							//ֹͣλ
		USART_InitStructure.USART_Parity		= usart_parity;							 
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//û��Ӳ��������
		USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;		

		USART_Init(USART1, &USART_InitStructure);

		

		 
		USART_Cmd(USART1, ENABLE);
		return 0;
	}
	else if( ch == COMM2 )
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

		// ��ʼ��IO
		// TX
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure USART1 Rx (PA.10) as input floating				*/
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;

		GPIO_Init(GPIOA, &GPIO_InitStructure);

		// ��ʼ������
		USART_InitStructure.USART_BaudRate		= baud;						// 
		USART_InitStructure.USART_WordLength	= usart_wordlength;				//8bit����λģʽ
		USART_InitStructure.USART_StopBits		= USART_StopBits_1;						//ֹͣλ
		USART_InitStructure.USART_Parity		= usart_parity;							//û����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//û��Ӳ��������
		USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;		
		
		USART_Init(USART2, &USART_InitStructure);

		// ��ʼ���ж�
		NVIC_InitStructure.NVIC_IRQChannel		= USART2_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;				//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		
		 
		USART_Cmd(USART2, ENABLE);

		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

		return 0;
	}
	
	else if( ch == COMM4 )
	{
		//PC10:tx  PC11:rx
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);

		// ��ʼ��IO
		// TX
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOC, &GPIO_InitStructure);

		/* Configure USART4 Rx (PC.11) as input floating				*/
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
		//		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;

		GPIO_Init(GPIOC, &GPIO_InitStructure);

		// ��ʼ������
		USART_InitStructure.USART_BaudRate		= baud;						// 
		USART_InitStructure.USART_WordLength	= usart_wordlength;				 
		USART_InitStructure.USART_StopBits		= USART_StopBits_1;						//ֹͣλ
		USART_InitStructure.USART_Parity		= usart_parity;							 
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//û��Ӳ��������
		USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;		

		USART_Init(UART4, &USART_InitStructure);

		// ��ʼ���ж�
		NVIC_InitStructure.NVIC_IRQChannel		= UART4_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;				//�����ȼ�
		NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_Cmd(UART4, ENABLE);

		return 0;
	}
    else if( ch == COMM5 )
	{
  //        // ��ʼ���ж�
		//NVIC_InitStructure.NVIC_IRQChannel		= UART5_IRQChannel;
		//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;				//��ռ���ȼ�
	
		//NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
		//NVIC_Init(&NVIC_InitStructure);
  //              
  //      RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 , ENABLE);        
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOD  | RCC_APB2Periph_AFIO, ENABLE);

		///* Configure UART5 Tx (PC.12) as alternate function push-pull */
		//GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_12;
		//GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
		//GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
		//GPIO_Init(GPIOC, &GPIO_InitStructure);

		///* Configure UART5 Rx (PD.02) as input floating */
		//GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_2;
		//GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
		//GPIO_Init(GPIOD, &GPIO_InitStructure);

		//// ��ʼ������
		//USART_InitStructure.USART_BaudRate		= baud;										// 
		//USART_InitStructure.USART_WordLength	= usart_wordlength;						 
		//USART_InitStructure.USART_StopBits		= USART_StopBits_1;							//ֹͣλ
		//USART_InitStructure.USART_Parity		= usart_parity;							 
		//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//û��Ӳ��������
		//USART_InitStructure.USART_Mode			= USART_Mode_Tx;//USART_Mode_Rx |		

		//USART_Init(UART5, &USART_InitStructure);

		//USART_Cmd(UART5, ENABLE);
        
		return 0;
	}
	else if( ch == COMMX )
	{
//		init_analoguart_port(baud);
		return -1;
	}
	return 0;
}




/******************* (C) COPYRIGHT 2010 Netcom *****END OF FILE****************/