/**
* @file uart_drv.c
* @brief 串口驱动程序
*	本模块为串口驱动模块，集中管理多个串口的收发任务
*
* @version V0.0.3
* @author xhh
* @date 2011年11月18日
* @note
*    fixed: 部分bug--(TXE标志上电后，默认为1，所以开机时不能打开TXE的中断，否则会误进入
*    modify:开机，串口初始化后，默认TXE/TC 中断关闭，打开接收中断；应用调用发送时，才打开TXE/TC 中断，用完后关闭

* @version V0.0.2
* @author zhongyh
* @date 2010年8月17日
* @note
*    整合对模拟串口的支持，需要 analog_uart.c
*    删除了对操作系统的依赖
*    删除了R3964协议处理部分
*    增加了 uart_device 以优化速度
*
* @version V0.0.1
* @author zhongyh
* @date 2009年10月2日
* @note
*    first release,支持串口1及串口2
*    
*
*
*/
#include "stm32f10x_lib.h"
#include "uart_drv.h"
#include <string.h>

/**
* @brief 串口发送管理buffer
*/
typedef struct {
	unsigned char					*pTxbuffer;			// 要发送的缓冲
	int								TxbufferSize;		// 要发送的数据长度
	int								tx_finish;			// 标识是否发送完所有数据  1:发送已完成  0：未完成
	CommIsrInByte					InByte;				// 串口接收到一字节的回调函数
}COMM_RING_BUF;


static COMM_RING_BUF				Comm1Buf;
static COMM_RING_BUF				Comm2Buf;
static COMM_RING_BUF				Comm3Buf;
static COMM_RING_BUF				Comm4Buf;
static COMM_RING_BUF				Comm5Buf;
static COMM_RING_BUF				CommXBuf;			// 模拟串口

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
* @brief		：	USART中断逐个字节发送 
* @param[in]	：	none				
* @return		：	
* @note			：	此函数在中断中调用 
****************************************************
*/
void Comm_IsrSendBytes(unsigned char ch)
{
	unsigned char		dat;
	unsigned char		err;

	dat			=	Comm_IsrGetTxChar(ch, &err);				//从发送缓冲中取出一个数据并发送出去

	switch(ch)
	{
	case COMM1:
		if( COMM_NO_ERR == err )									//是否发送完毕
		{
			USART_SendData(USART1, dat);
		}
		else
		{
			
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM2:
		if( COMM_NO_ERR  == err )									//是否发送完毕
		{
			USART_SendData(USART2, dat);	
		}
		else
		{
			
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM3:
		if( COMM_NO_ERR  == err )									//是否发送完毕
		{
			USART_SendData(USART3, dat);
		}
		else
		{
			//关闭TXE中断的使能
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM4:
		if( COMM_NO_ERR  == err )									//是否发送完毕
		{
			USART_SendData(UART4, dat);	
		}
		else
		{	
			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
		}
		break;

	case COMM5:
		//if( COMM_NO_ERR  == err )									//是否发送完毕
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
* @brief 中断程序调用此函数，从发送缓冲中取一个字符发送出去
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
*@brief	串口管理模块初始化
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
* @brief 应用程序调用，判断发送缓冲是否为空
* @param[in] 
* @return 0:空
*         1:满
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
		empty	=	0;					// 发送已完成 。空
	}
	else
	{	
		empty	=	1;					// 发送未完成。非空
	}
	return empty;
}


/**
* @brief 中断程序调用此函数，将一个接收到的字符放到缓冲中

*/
void Comm_IsrPutRxChar (unsigned char ch, unsigned char c)
{
	COMM_RING_BUF					*pbuf;

	// 此函数为优化，没有对参数的安全检查!!!

	pbuf							= uart_device[ch-1];

	if (pbuf->InByte)
		(pbuf->InByte)(c);
}


/**
* @brief 为指定的串口中断设置串口中断接收函数
* @param[in] unsigned char ch:COMM1,COMM2,COMM3,COMM4,COMM5
* @param[in] CommIsrInByte inbyte 中断处理函数
note:(1) USART_IT_TXE  可以先不打开，使用的时候再打开，使用完关闭,其复位值默认为1,会误入中断；
		 需要发送的时候再打开，顺序是先写入DR，然后USART_ITConfig(USART2, USART_IT_TXE, ENABLE);参考《stm32参考文档》
     (2) USART_ClearITPendingBit(USART2, USART_IT_TXE);此句无作用
	 (3) USART_ClearITPendingBit(USART3, USART_IT_RXNE)最好用(void)USART_ReceiveData(USART3)代替---参考《stm32参考文档》
	  
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
	{	//智能卡
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
		// 模拟串口，不用做任何事情
	}
}

/**
* @brief 应用程序调用此函数，通过串口发送数据
* @param[in] unsigned char ch 串口的通道，可以是COMM1,COMM2,COMM3,COMM4,COMM5
* @param[in] unsigned char *buffer 要发送的数据缓冲
* @param[in] int len 要发送的数据长度
* @param[in] CommIsrInByte inbyte 接收数据的回调函数
*/
void Comm_AppSendBuffer(unsigned char ch, unsigned char *buffer, int len)
{
	COMM_RING_BUF					*pbuf;

	// 必须有字符要发送
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

	// 填充发送结构
	pbuf->pTxbuffer				= buffer+1;
	pbuf->TxbufferSize			= len-1;
	pbuf->tx_finish				= 0;

	// 发送第一个字符，并打开发送中断，然后返回。
	// 在中断程序中会将剩余的数据送到串口
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
		//不打开接收中断,等进入TC中断后打开
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
		// todo 模拟串口
		// 启动发送第一个字节，并启动接收中断
//		analoguart_sendbyte(*buffer);
		break;

	default:
		return;
	}
}

/**
***************************************************************************
*@brief	初始化串口，并打开串口。具体的中断在设置回调的函数中打开
*@param[in] parity:0  无校验 no
                   1  奇校验 odd
				   2  偶校验 even
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
		usart_wordlength =USART_WordLength_9b;//奇校验
		usart_parity=USART_Parity_Odd;
		break;
	case 2:
		usart_wordlength =USART_WordLength_9b;//偶校验
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
          // 初始化中断
		NVIC_InitStructure.NVIC_IRQChannel		= USART1_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//先占优先级
		//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;				//子优先级

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

		// 初始化串口
		USART_InitStructure.USART_BaudRate		= baud;										// 
		USART_InitStructure.USART_WordLength	= usart_wordlength;						 
		USART_InitStructure.USART_StopBits		= USART_StopBits_1;							//停止位
		USART_InitStructure.USART_Parity		= usart_parity;							 
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//没有硬件流控制
		USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;		

		USART_Init(USART1, &USART_InitStructure);

		

		 
		USART_Cmd(USART1, ENABLE);
		return 0;
	}
	else if( ch == COMM2 )
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

		// 初始化IO
		// TX
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Configure USART1 Rx (PA.10) as input floating				*/
		GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;

		GPIO_Init(GPIOA, &GPIO_InitStructure);

		// 初始化串口
		USART_InitStructure.USART_BaudRate		= baud;						// 
		USART_InitStructure.USART_WordLength	= usart_wordlength;				//8bit数据位模式
		USART_InitStructure.USART_StopBits		= USART_StopBits_1;						//停止位
		USART_InitStructure.USART_Parity		= usart_parity;							//没有奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//没有硬件流控制
		USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;		
		
		USART_Init(USART2, &USART_InitStructure);

		// 初始化中断
		NVIC_InitStructure.NVIC_IRQChannel		= USART2_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;				//子优先级
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

		// 初始化IO
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

		// 初始化串口
		USART_InitStructure.USART_BaudRate		= baud;						// 
		USART_InitStructure.USART_WordLength	= usart_wordlength;				 
		USART_InitStructure.USART_StopBits		= USART_StopBits_1;						//停止位
		USART_InitStructure.USART_Parity		= usart_parity;							 
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;			//没有硬件流控制
		USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;		

		USART_Init(UART4, &USART_InitStructure);

		// 初始化中断
		NVIC_InitStructure.NVIC_IRQChannel		= UART4_IRQChannel;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;				//子优先级
		NVIC_InitStructure.NVIC_IRQChannelCmd	= ENABLE;
		NVIC_Init(&NVIC_InitStructure);

		USART_Cmd(UART4, ENABLE);

		return 0;
	}
    else if( ch == COMM5 )
	{
  //        // 初始化中断
		//NVIC_InitStructure.NVIC_IRQChannel		= UART5_IRQChannel;
		//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;				//先占优先级
	
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

		//// 初始化串口
		//USART_InitStructure.USART_BaudRate		= baud;										// 
		//USART_InitStructure.USART_WordLength	= usart_wordlength;						 
		//USART_InitStructure.USART_StopBits		= USART_StopBits_1;							//停止位
		//USART_InitStructure.USART_Parity		= usart_parity;							 
		//USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//没有硬件流控制
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