/**
* @file  uE988_scan.c
* @brief uE988 Scan Engine ����ɨ���ǵ�����ģ��
* @version 1.0
* @author yinzh
* @date 2011��01��24��
* @note
*/
#include <string.h>
#include "stm32f10x_lib.h"
#include "ucos_ii.h"
#include "uE988_scan.h"
#include "TimeBase.h"
#include "keypad.h"
//#include "gui.h"
#include "PCUsart.h"
#include "uart_drv.h"

//#define UE988_DEBUG

/**************	�������(Op_code)	**********************/
#define		BEEP						0xE6
#define		CMD_ACK						0xD0
#define		CMD_NAK						0xD1
#define		DECODE_DATA					0xF3
#define		EVENT						0xF6
#define		LED_OFF						0xE8
#define		LED_ON						0xE7
#define		PARAM_DEFAULTS				0xC8
#define		PARAM_REQUEST				0xC7
#define		PARAM_SEND					0xC6
#define		REPLY_REVISION				0xA4
#define		REQUEST_REVISION			0xA3
#define		SCAN_DISABLE				0xEA
#define		SCAN_ENABLE					0xE9
#define		SLEEP						0xE8
#define		START_DECODE				0xE4
#define		STOP_DECODE					0xE5
#define		CUSTOM_DEFAULTS				0x12

#define		RES_CHECKFAILURE			1
#define		RES_UNKOWN_MSG				2
#define		RESPONSE_SUCCESS			3
#define		RESPONSE_ACK				4
#define		RESPONSE_NAK				5

//�������ʹ������������ƵĶ�Ӧ�ṹ��
typedef struct  
{
	unsigned char type;
	unsigned char *name;
}TCodeTypeName;

static unsigned char	g_ack_enable;							//indicate whether ack/nck handshaking is enabled  1: enable; 0: disable
#define MAX_DECODE_DATA		50
//static unsigned char	g_decode_data[MAX_DECODE_DATA];

TUE988Command	g_resCmd;		//scan decoder -> host
unsigned char	*g_pReqCmd;		//host -> scan decoder

const	unsigned char	host_ack[6] = {0x04, CMD_ACK, 0x04, 0x0, 0xFF, 0x28};

static	unsigned char scaner_version[21];		//ɨ��ͷ�汾
static	unsigned int	wait_time_out;			//get_barcode����ĵȴ���ʱ����
static  unsigned int    scanner_baudrate;

TCodeTypeName code_type_name_tbl[] = {
	{0x01, "Code 39"}, {0x02, "Codabar"}, {0x03, "Code 128"},
	{0x04, "Discrete 2"}, {0x05, "IATA 2"}, {0x06, "Interleaved 2 of 5"},
	{0x07, "Code 93"}, {0x08, "UPC A"}, {0x48, "UPC A 2S"},
	{0x88, "UPC A 5S"}, {0x09, "UPC E0"}, {0x49, "UPC E0 2S"},
	{0x89, "UPC E0 5S"}, {0x0A, "EAN 8"}, {0x0B, "EAN 13"},
	{0x4B, "EAN 13 2S"}, {0x8B, "EAN 13 5S"}, {0x0E, "MSI"},
	{0x0F, "EAN 128"}, {0x10, "UPC E1"}, {0x50, "UPC E1 2S"},
	{0x90, "UPC E1 5S"}, {0x15, "Tp Code 39"}, {0x23, "RSS-Limit"},
	{0x24, "RSS-14"}, {0x25, "RSS-Expanded"},{0x13,"UK"},{0x11,"China Post"},
	{0x0c,"Code 11"},{0x0d,"Matrix 2 0f 5"}, {0, 0}
};

//TCodeTypeName code_type_name_tbl_OldVersion[] = {
//	{0x01, "Code 39"}, {0x02, "Codabar"}, {0x03, "Code 128"},
//	{0x04, "Discrete 2"}, {0x05, "IATA 2"}, {0x06, "Interleaved 2 of 5"},
//	{0x07, "Code 93"}, {0x08, "UPC A"}, {0x48, "UPC A 2S"},
//	{0x88, "UPC A 5S"}, {0x09, "UPC E0"}, {0x49, "UPC E0 2S"},
//	{0x89, "UPC E0 5S"}, {0x0A, "EAN 8"}, {0x0B, "EAN 13"},
//	{0x4B, "EAN 13 2S"}, {0x8B, "EAN 13 5S"}, {0x0E, "MSI"},
//	{0x0F, "EAN 128"}, {0x10, "UPC E1"}, {0x50, "UPC E1 2S"},
//	{0x90, "UPC E1 5S"}, {0x15, "Tp Code 39"}, {0x23, "RSS-Limit"},
//	{0x24, "RSS-14"}, {0x25, "RSS-Expanded"},{0x13,"UK"},{0x11,"China Post"},
//	{0x0c,"Code 11"},{0x0d,"Matrix 2 0f 5"}, {0, 0}
//};

//unsigned int	scan_start;

static int write_cmd_to_scanner(const unsigned char *pData, unsigned short length);
static void pack_command(unsigned char cmd_code, unsigned char cmd_status, unsigned char *pCmddata, unsigned char data_len);


///* 
//* @brief: ����֧��/��֧��ĳ������
//* @param[in] codeType: ������ţ������ϵĺ궨��
//*				 ctrl: ֻ��ȡENABLE �� DISABLE
//* @note: 
//*/
//int UE988_codeType_ctrl(unsigned char codeType, unsigned char ctrl)
//{
//	unsigned char req_data[8] = {0};
//	unsigned char cur_pos = 0;
//
//	req_data[cur_pos++] = 0xFF;	//beep code
//	if (codeType == ChinaPost || codeType == RSS14 ||
//		codeType == RSSLimited || codeType == RSSExpanded)
//	{
//		req_data[cur_pos++] = 0xF0;
//	}
//	req_data[cur_pos++] = codeType;
//	req_data[cur_pos++] = ctrl;
//	pack_command(PARAM_SEND, 0x08, req_data, cur_pos);
//	if (write_cmd_to_scanner(g_pReqCmd, strlen(g_pReqCmd)) == 0)
//	{
//		return 0;
//	}
//
//	return -1;
//}


/* 
* @brief: ����/�ر�ĳ����������
* @param[in] codeType: �������ʹ��ţ����궨��
*				 ctrl: ֻ��ΪENABLE �� DISABLE
* @note: Ĭ������£� code 11  code93
China Post
Codabar
MSI
RSSϵ��(RSS-14 RSS-Limited RSS-Expanded)���ǹرյ�
*/
int UE988_codeType_ctrl(unsigned char codeType, unsigned char ctrl)
{
	unsigned char req_data[8] = {0};
	unsigned char cur_pos = 0;

	req_data[cur_pos++] = 0xFF;	//beep code
	if (codeType == ChinaPost || codeType == RSS14 ||
		codeType == RSSLimited || codeType == RSSExpanded)
	{
		req_data[cur_pos++] = 0xF0;
	}
	else if (codeType == Matrix20f5 || codeType == UK)
	{
		req_data[cur_pos++] = 0xF2;
	}

	req_data[cur_pos++] = codeType;
	req_data[cur_pos++] = ctrl;
	pack_command(PARAM_SEND, 0x08, req_data, cur_pos);
	if (write_cmd_to_scanner(g_pReqCmd, strlen(g_pReqCmd)) == 0)
	{
		return 0;
	}

	return -1;
}

/* 
* @brief: ��������Ĳ�������
* @param[in] unsigned char *param: �����õĲ������� 
*/
int UE988_set_codeParam(unsigned char *param, unsigned char param_len)
{
	unsigned char req_data[16] = {0};

	if (param_len > 15)
	{
		return -1;
	}

	req_data[0] = 0xFF;		//beep code
	memcpy(req_data+1, param, param_len);

	pack_command(PARAM_SEND, 0x08, req_data, param_len+1);
	if (write_cmd_to_scanner(g_pReqCmd, strlen(g_pReqCmd)) == 0)
	{
		return 0;
	}

	return -1;
}


/**
* @brief �������ʹ��Ż�ȡ��������
* @param[in] unsigned char type ���ʹ���
* @return unsigned char * name ��������
*/
static unsigned char *type2name(unsigned char type)
{
	unsigned int i = 0;

	if(type == 0)
		return 0;

	while (code_type_name_tbl[i].type != 0) 
	{
		if (type == code_type_name_tbl[i].type) 
		{
			return code_type_name_tbl[i].name;
		}
		i++;
	}
	return 0;
}
/*
 * @brief: ��ʼ��ģ��˿�
*/
static void UE988_GPIO_config(unsigned int baudrate)
{
	GPIO_InitTypeDef				GPIO_InitStructure;
	USART_InitTypeDef				USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	//Power
	GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_8;
        GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_8);

	// ʹ��UART3, PB10,PB11
	/* Configure USART3 Tx (PB.10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed			= GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure USART3 Rx (PB.11) as input floating				*/
	GPIO_InitStructure.GPIO_Pin				= GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode			= GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate		= baudrate;					
	USART_InitStructure.USART_WordLength	= USART_WordLength_8b;
	USART_InitStructure.USART_StopBits		= USART_StopBits_1;
	USART_InitStructure.USART_Parity		= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode			= USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);
}

static void UE988_NVIC_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel				=USART3_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority	= 0;
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority	= 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd			= ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

static void reset_resVar(void)
{
	g_resCmd.CmdPos = 0;
	g_resCmd.DataLength = 0;
	g_resCmd.status	 = 0;
}

/*
*brief: Ӳ������ɨ����
*/
#if 0
static void hardware_wakeup(void)
{
	int i = 0;

	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	for (i = 0; i < 1000; i++)
	{
		;
	}

	GPIO_SetBits(GPIOA, GPIO_Pin_6);
}
#endif
/**
* @brief  �����ݸ�������ɨ����
* @param[in] unsigned char *pData Ҫ���͵�����
* @param[in] int length Ҫ�������ݵĳ���
*/
static void send_data_to_scanner(const unsigned char *pData, unsigned short length)
{
	while(length--)
	{
		USART_SendData(USART3, *pData++);
		while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		{
		}
	}
	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET){};
}

/**
* @brief  �������������ɨ����
* @param[in] unsigned char *pData Ҫ���͵�����
* @param[in] int length Ҫ�������ݵĳ���
* @param[out]	0: �ɹ�
*				-1: ʧ��
*/
static int write_cmd_to_scanner(const unsigned char *pData, unsigned short length)
{
	unsigned char	retry_times		= 2;

	UE988_wakeup();		//�Ȼ���ģ��

loop:
	retry_times--;
	send_data_to_scanner(pData, length);

	if (g_ack_enable == 0)	//ack/nak handshaking disabled
	{
		return 0;
	}

	//wait host ack
	reset_resVar();
	StartDelay(200);			/* ��ʱ1S		*/
	while (DelayIsEnd() != 0)
	{
		if (g_resCmd.status == RESPONSE_ACK)
		{
			return 0;
		}
		else if (g_resCmd.status == RESPONSE_NAK)
		{
			return -1;
		}
	}
	if (retry_times == 0)
	{
		return -1;
	}
	goto	loop;
}

static unsigned short calc_checksum(unsigned char *pData, unsigned char data_len)
{
	unsigned int i = 0;
	unsigned short checksum = 0;

	for (i = 0; i < data_len; i++)
	{
		checksum	+= pData[i];
	}

	checksum	= ~checksum;
	checksum	+= 1;

	return checksum;
}

/**
* @brief  �������(host->scanner)
* @param[in] unsigned char cmd_code �������
* @param[in] unsigned char cmd_status 
* @param[in] unsigned char *pCmddata ��������
* @param[in] unsigned char data_len �������ݳ���
* @note: �����ʽ
*	---------------------------------------------------------------
*  | length | Op-code | Message Source | Status | Data | Checksum |
*	---------------------------------------------------------------
*/
static void pack_command(unsigned char cmd_code, unsigned char cmd_status, unsigned char *pCmddata, unsigned char data_len)
{
	unsigned short cur_pos	= 0;
	unsigned short checksum = 0;
        
        memset(g_pReqCmd,0,50);

	g_pReqCmd[cur_pos]	= 4 + data_len;		//length
	cur_pos++;
	g_pReqCmd[cur_pos] = cmd_code;			//Op_code
	cur_pos++;
	g_pReqCmd[cur_pos] = 0x04;				// Message Source
	cur_pos++;
	g_pReqCmd[cur_pos]	= cmd_status;		//Status
	cur_pos++;

	if (data_len > 0 && pCmddata != 0)
	{
		memcpy(g_pReqCmd+cur_pos, pCmddata, data_len);		//Data
		cur_pos		+=	data_len;
	}

	checksum = calc_checksum(g_pReqCmd, cur_pos);
	g_pReqCmd[cur_pos] = (checksum>>8)&0xFF;
	cur_pos++;
	g_pReqCmd[cur_pos] = checksum&0xFF;
}

static void	send_ack_to_scanner(void)
{
	send_data_to_scanner(host_ack, 6);
}

/*
* @param[in] cause	RES_CHECKFAILURE			
*					RES_UNKOWN_MSG
*/
static void send_nak_to_sanner(unsigned char cause)
{
	unsigned short checksum;
	unsigned char	host_nak[7] = {0x05, CMD_NAK, 0x04, 0x0, 0x0, 0x0, 0x0};

	host_nak[4]	= cause;
	checksum = calc_checksum(host_nak, 5);
	host_nak[5]	= (checksum>>8)&0xFF;
	host_nak[6] = (checksum&0xFF);
	send_data_to_scanner(host_nak, 7);
}


static int set_UE_param_default(void)
{
	unsigned char cmd_buf[] = {0x04, 0xD8, 0x04, 0x00, 0xFF, 0x20};

	if (write_cmd_to_scanner(cmd_buf, 6) == 0)
	{
		return 0;
	}

	return -1;
}
/* 
* @brief: ��ģ��Trigger Mode��Ϊ Hostģʽ
* @note: �����ڵ�һ��ʹ��ģ��ʱ���ã��Ժ����Զ����
*/
static int switch_to_Host_Mode(void)
{
	unsigned char param_num = 0x8A;
	unsigned char cmd_buf[] = {0x07, 0xC6, 0x04, 0x08, 0x00, 0x8A, 0x08, 0xFE, 0x95};
	int cur_mode = 0;

	/* step 1: �Ȳ�ѯtrigger mode, ���Ϊ0x08, ������Ѿ���Host ģʽ, ����, ���޸� */
	cur_mode = UE988_get_curParam(param_num);
	if (cur_mode == -1 || cur_mode == -2)
	{
		return cur_mode;    //���ش����룬 -2����ʾ��Ӧ��ʱ�������ǲ����ʴ��󣬻���ɨ��ͷ�𻵣�δ��װ
	}

	if (cur_mode == 0x08)	//�Ѿ���Hostģʽ
	{
		return 0;
	}

	//����������ΪMDĬ�ϲ���
	OSTimeDlyHMSM(0,0,0,500);
	set_UE_param_default();
	OSTimeDlyHMSM(0,0,0,500);

	/* step 2:  ��ģʽ�޸ĳ�ΪHostģʽ			*/
	if (write_cmd_to_scanner(cmd_buf, 9) == 0)
	{
		return 0;
	}

	return -1;
}

/* 
* @brief: ��decode data packet format ����ΪPacked Data, Ĭ��ΪData onlyģʽ
* @note: �����ڵ�һ��ʹ��ģ��ʱ���ã��Ժ����Զ����
*/
static int set_decode_data_format(void)
{
	unsigned char param_num = 0xEE;
	unsigned char cmd_buf[] = {0x07, 0xC6, 0x04, 0x08, 0x00, 0xEE, 0x01, 0xFE, 0x38};
	int cur_mode = 0;

	/* step 1: �Ȳ�ѯtrigger mode, ���Ϊ0x08, ������Ѿ���Host ģʽ, ����, ���޸� */
	cur_mode = UE988_get_curParam(param_num);
	if (cur_mode == -1)
	{
		return -1;
	}
	if (cur_mode == 0x01)	//�Ѿ���packed data
	{
		return 0;
	}

	/* step 2:  ��ģʽ�޸ĳ�ΪHostģʽ			*/
	OSTimeDlyHMSM(0,0,0,500);
	if (write_cmd_to_scanner(cmd_buf, 9) == 0)
	{
		return 0;
	}

	return -1;
}

/*
* @breif: ��ʼ��һЩ����
*/
static int UE988_Init_param(void)
{
	int   ret;
	//��trigger mode ��Ϊ host
	ret = switch_to_Host_Mode();
	if(ret == -2)
	{
		//��Ӧ��ʱ�����Գ����л������Ĳ����ʣ��ٴγ���һ��
		UE988_GPIO_config(115200);

		ret = switch_to_Host_Mode();
		if(ret)
		{
			return ret;
		}
		else
		{
			scanner_baudrate = 115200;
		}
	}
	else
	{
		scanner_baudrate = 9600;
	}

	//����Decode Data Packet Format ΪPacked Data
	set_decode_data_format();

	//��Ĭ�ϴ򿪵�ɨ�赽����֮��ķ������ܹر�
	UE988_set_curParam(0x38,0,1);

	if(scanner_baudrate == 115200)
	{
	  UE988_set_curParam(0x9C,0x06,1);    //��ɨ��ͷ�Ĳ���������Ϊ9600 
	 UE988_GPIO_config(9600);
	}

	return 0;
}

//����ɨ��ͷ�Ľ���������
void UE988_set_decoder_switch(unsigned short switch_map)
{
	UE988_codeType_ctrl(EAN8,((switch_map&(0x0001<<0))?ENABLE:DISABLE));
	UE988_codeType_ctrl(EAN13,((switch_map&(0x0001<<0))?ENABLE:DISABLE));
	UE988_codeType_ctrl(UCCEAN128,((switch_map&(0x0001<<0))?ENABLE:DISABLE));

	UE988_codeType_ctrl(UPCA,((switch_map&(0x0001<<1))?ENABLE:DISABLE));
	UE988_codeType_ctrl(UPCE,((switch_map&(0x0001<<1))?ENABLE:DISABLE));
	UE988_codeType_ctrl(UPCE1,((switch_map&(0x0001<<1))?ENABLE:DISABLE));

	UE988_codeType_ctrl(Code39,((switch_map&(0x0001<<2))?ENABLE:DISABLE));

	UE988_codeType_ctrl(Code93,((switch_map&(0x0001<<3))?ENABLE:DISABLE));

	UE988_codeType_ctrl(Code11,((switch_map&(0x0001<<4))?ENABLE:DISABLE));

	UE988_codeType_ctrl(Code128,((switch_map&(0x0001<<5))?ENABLE:DISABLE));

	UE988_codeType_ctrl(IL2of5,((switch_map&(0x0001<<6))?ENABLE:DISABLE));

	UE988_codeType_ctrl(Matrix20f5,((switch_map&(0x0001<<7))?ENABLE:DISABLE));

	UE988_codeType_ctrl(InDus2of5,((switch_map&(0x0001<<8))?ENABLE:DISABLE));

	UE988_codeType_ctrl(MSI,((switch_map&(0x0001<<9))?ENABLE:DISABLE));

	UE988_codeType_ctrl(RSS14,((switch_map&(0x0001<<10))?ENABLE:DISABLE));
	UE988_codeType_ctrl(RSSLimited,((switch_map&(0x0001<<10))?ENABLE:DISABLE));
	UE988_codeType_ctrl(RSSExpanded,((switch_map&(0x0001<<10))?ENABLE:DISABLE));

	UE988_codeType_ctrl(ChinaPost,((switch_map&(0x0001<<11))?ENABLE:DISABLE));
}
/*
* @brief: ģ���ʼ��
*/
void UE988_Init(unsigned short switch_map)
{
	unsigned int	tmp;
	//unsigned char IL2of5_len[5] = {0x16, 0x01, 0x17, 0x20};
	//unsigned char Indus2of5_len[5] = {0x14, 0x01, 0x15, 0x20};
	//unsigned char MSI_len[5] = {0x1E, 0x01, 0x1F, 0x20};
	//unsigned char MSI_checkTrans[3] = {0x2E, 0x01};
	

	UE988_GPIO_config(9600);
	UE988_NVIC_config();
	
	g_pReqCmd	= g_send_buff;
	g_resCmd.CmdBuffer	= g_receive_buff;

	reset_resVar();
	g_ack_enable	= 1;


	//��ʼ����������
	Comm_SetReceiveProc(COMM3, (CommIsrInByte)UE988_RxISRHandler);						//���ô��ڻص�����

#ifndef UE988_DEBUG
	if(UE988_Init_param())
	{
		return;
	}

	//code 11
	//China Post
	//Codabar
//MSI
//RSSϵ��(RSS-14 RSS-Limited RSS-Expanded)
	//��Щ�������͵�֧��Ĭ���ǹرյģ���Ҫ�򿪶����ǵ�֧��
#if 1

	UE988_set_decoder_switch(switch_map);
	//UE988_codeType_ctrl(Code11,ENABLE);
	//UE988_codeType_ctrl(ChinaPost,ENABLE);
	//UE988_codeType_ctrl(Codabar,ENABLE);
	//UE988_codeType_ctrl(MSI,ENABLE);
	//UE988_codeType_ctrl(RSS14,ENABLE);
	//UE988_codeType_ctrl(RSSLimited,ENABLE);
	//UE988_codeType_ctrl(RSSExpanded,ENABLE);
	//UE988_codeType_ctrl(Code93,ENABLE);
	//UE988_codeType_ctrl(IL2of5,ENABLE);
	//UE988_codeType_ctrl(TriopCode39, ENABLE);

	//UE988_codeType_ctrl(InDus2of5, ENABLE);
	//UE988_codeType_ctrl(UPCE1, ENABLE);

	UE988_set_codeParam("\x16\x01\x17\x20", 4);				//UE988_set_codeParam(IL2of5_len, 4);
	UE988_set_codeParam("\x14\x01\x15\x20", 4);				//UE988_set_codeParam(Indus2of5_len, 4);
	//UE988_set_codeParam("\x1e\x01\x1f\x20", 4);				//UE988_set_codeParam(MSI_len, 4);
	//UE988_set_codeParam("\x2e\x01", 2);						//UE988_set_codeParam(MSI_checkTrans, 2);

	write_cmd_to_scanner("\x07\xC6\x04\x08\x00\x34\x00\xFE\xF3", 9);

#endif

	//Comm_InitPort(COMM3,9600, 0);									//���ô��ڲ���  ���ڲ����Ѿ�����

	//wait_time_out = 29;
	//if(0 == UE988_get_softVersion(scaner_version,(unsigned char*)&tmp))
	//{
	//	if (memcmp(scaner_version,"uE_HW",5) == 0)
	//	{
	//		//���µ�ɨ��ͷ�汾���ǣ�uE_HW2.3_SW1.5.29_HY
	//		//����һ��ɨ��ͷ�İ汾����:NBRSYPAC(D)...
	//		wait_time_out = 48;
	//	}
	//}

	wait_time_out = 48;
#endif
	//scan_start = 0;
}


/**
* @brief ����host�յ�scanner������
* @param[in] unsigned char c ������ַ�
* @return 0:success put in buffer
*        -1:fail
*/
int UE988_RxISRHandler(unsigned char c)
{
	unsigned short checksum = 0;

		g_resCmd.CmdBuffer[g_resCmd.CmdPos++] = c;

#ifdef UE988_DEBUG

		if (g_resCmd.CmdPos == 20)
		{
			g_resCmd.CmdPos = 0;
			g_resCmd.status	= RESPONSE_SUCCESS;
		}
#else
		/*-------------------------------------------------------*/
		/*���´��������ݰ���ʽ�Ƿ���ȷ                      */
		/*-------------------------------------------------------*/
		if (g_resCmd.CmdPos == 1)
		{
			g_resCmd.DataLength = g_resCmd.CmdBuffer[0];
			if (g_resCmd.DataLength == 0)
			{
				g_resCmd.CmdPos = 0;
				g_resCmd.status = RES_UNKOWN_MSG;
				return -1;
			}
		}

		if (g_resCmd.CmdPos == 3)
		{
			if (g_resCmd.CmdBuffer[2] != 0)	//Message Source, must be 0 
			{
				g_resCmd.CmdPos = 0;
				g_resCmd.status = RES_UNKOWN_MSG;
				return -1;
			}
		}

		if (g_resCmd.CmdPos == g_resCmd.DataLength+2)	//�������
		{
			checksum = calc_checksum(g_resCmd.CmdBuffer, g_resCmd.DataLength);
			if (((checksum>>8)&0xFF) != g_resCmd.CmdBuffer[g_resCmd.DataLength] ||
				 (checksum&0xFF) != g_resCmd.CmdBuffer[g_resCmd.DataLength+1]
				)
			{
				g_resCmd.CmdPos = 0;
				g_resCmd.status = RES_CHECKFAILURE;
				return -1;
			}
			else
			{
				if (g_resCmd.CmdBuffer[1] == CMD_ACK)
				{
					g_resCmd.status = RESPONSE_ACK;
				}
				else if (g_resCmd.CmdBuffer[1] == CMD_NAK)
				{
					g_resCmd.status = RESPONSE_NAK;
				}
				else
				{
					g_resCmd.status	= RESPONSE_SUCCESS;
				}
			}
		}
		
		if (g_resCmd.CmdPos > g_resCmd.DataLength+2)
		{
			reset_resVar();
			g_resCmd.status = RES_UNKOWN_MSG;
			return -1;
		}
#endif
	return 0;
}

/*
* @brief: host����scanner��beep
* @param[in] beep_code: ȡֵ��Χ 0x00-0x19
*/
void UE988_beep(unsigned char beep_code)
{
	unsigned char op_code = BEEP;
	unsigned char status = 0x0;
	unsigned char cmd_buf[2] = {0};

	cmd_buf[0] = beep_code;
	pack_command(op_code, status, cmd_buf, 1);
	write_cmd_to_scanner(g_pReqCmd, 7);
}

/*
* @brief: host����scanner��LED
* @param[in] ctrl_type: UE988_LED_OFF	1
*						UE988_LED_ON	2
*/
void UE988_led_ctrl(unsigned char ctrl_type)
{
	unsigned char led_select[2] = {0x01};

	if (ctrl_type == UE988_LED_OFF)
	{
		pack_command(LED_OFF, 0, led_select, 1);
	}
	else if (ctrl_type == UE988_LED_ON)
	{
		pack_command(LED_ON, 0, led_select, 1);
	}
	else
	{
		return;
	}

	write_cmd_to_scanner(g_pReqCmd, 7);
}

/*
* @brief: reset the decoders parameter settings to the factory default values
*/
void UE988_reset_param(void)
{
	pack_command(PARAM_DEFAULTS, 0, 0, 0);
	write_cmd_to_scanner(g_pReqCmd, 6);
}

/*
* @brief: ��ȡdecoder��ǰ�Ĳ�����Ϣ
* @param[in] param_num: ��������
* @ret:		-1: ��ȡʧ��
*			����: ����ֵ
*/
int UE988_get_curParam(unsigned char param_num)
{
	unsigned char req_data[2] = {0};

	req_data[0] = param_num;
	pack_command(PARAM_REQUEST, 0, req_data, 1);
	g_ack_enable	= 0;	//the response to this command is PARAM_SEND, not ACK
	write_cmd_to_scanner(g_pReqCmd, 7);

	//wait response (PARAM_SEND)
	reset_resVar();
	StartDelay(400);			/* ��ʱ2S		*/
	while (DelayIsEnd() != 0)
	{
		if (g_resCmd.status == RESPONSE_SUCCESS) //�ɹ��յ���Ӧ
		{
			if (g_resCmd.CmdBuffer[1] != PARAM_SEND ||
				g_resCmd.CmdBuffer[5] != param_num)	//��Ӧ���ݴ���
			{
				g_ack_enable	= 1;
				OSTimeDlyHMSM(0,0,0,500);
				send_nak_to_sanner(RES_UNKOWN_MSG);
				return -1;
			}
			else
			{
				g_ack_enable	= 1;
				OSTimeDlyHMSM(0,0,0,500);
				send_ack_to_scanner();
				return	g_resCmd.CmdBuffer[6];
			}
		}//�ɹ��յ���Ӧ
		else if (g_resCmd.status == RESPONSE_NAK)	//��Ӧʧ��
		{
			g_ack_enable	= 1;
			return -1;
		}
		else if (g_resCmd.status == RES_CHECKFAILURE)
		{
			g_ack_enable	= 1;
			OSTimeDlyHMSM(0,0,0,500);
			send_nak_to_sanner(RES_CHECKFAILURE);
			return -1;
		}
		else if (g_resCmd.status == RES_UNKOWN_MSG)
		{
			g_ack_enable	= 1;
			OSTimeDlyHMSM(0,0,0,500);
			send_nak_to_sanner(RES_UNKOWN_MSG);
			return -1;
		}
	}//��ʱ

	//��ʱ
	g_ack_enable	= 1;
	return -2;
}

/*
* @brief: ����decoder��ǰ�Ĳ�����Ϣ
* @param[in] param_num: ��������
* @param[in] param_value: ����ֵ
* @param[in] set_type: ���÷�ʽ 0: ��ʱ���ã��ϵ��ָ���ԭֵ; 1: ��������
* @ret:		-1: ����ʧ��
*			0: ���óɹ�
*/
int UE988_set_curParam(unsigned char param_num, unsigned char param_value, unsigned char set_type)
{
	unsigned char status;
	unsigned char param_data[3] = {0};
	int ret;

	param_data[0] = 0xFF;	//Beep Code, ignore
	param_data[1] = param_num;
	param_data[2] = param_value;

	status = (set_type == 0)? 0x0 : 0x08;

	pack_command(PARAM_SEND, status, param_data, 3);
	ret = write_cmd_to_scanner(g_pReqCmd, 9);

	return ret;
}

/*
* @brief: ����decoder��ɨ��״̬
* @param[in] ctrl_type: UE988_SCAN_DISABLE  UE988_SCAN_ENABLE
*/
void UE988_scan_ctrl(unsigned char ctrl_type)
{
	if (ctrl_type == UE988_SCAN_DISABLE)
	{
		pack_command(SCAN_DISABLE, 0, 0, 0);
	}
	else if (ctrl_type == UE988_SCAN_ENABLE)
	{
		pack_command(SCAN_ENABLE, 0, 0, 0);
	}
	else
	{
		return;
	}

	write_cmd_to_scanner(g_pReqCmd, 6);
}

/*
* @brief: ��������״̬
* @note:
*	�����ʼ��ģ��ʱ�����˵͹���ģʽ����ôģ����Զ�����͹��ģ�����Ҫ���ô˺���
*	Ĭ��״̬�£�ģ�鿪���͹���ģ��
*/
void UE988_enter_sleep(void)
{
	pack_command(SLEEP, 0, 0, 0);
	write_cmd_to_scanner(g_pReqCmd, 6);
}

/*
* @brief: ����ģ��
*/
void UE988_wakeup(void)
{
	unsigned char cmd[2] = {0};

	send_data_to_scanner(cmd, 1);

	StartDelay(10);			/* ��ʱ50ms		*/
	while (DelayIsEnd() != 0)
	{
		;
	}
}

/*
* @breif:  ��ʼ �� ֹͣɨ������
* @param[in]: ctrl_type: UE988_START_DECODE  UE988_STOP_DECODE
*/
void UE988_start_stop_decode(unsigned char ctrl_type)
{
	if (ctrl_type == UE988_START_DECODE)
	{
		pack_command(START_DECODE, 0, 0, 0);
	}
	else if (ctrl_type == UE988_STOP_DECODE)
	{
		pack_command(STOP_DECODE, 0, 0, 0);
	}
	else
	{
		return;
	}

	write_cmd_to_scanner(g_pReqCmd, 6);
}

/*
* @breif:  ��ȡ������
* @param[out]: unsigned char *code_type: ����������		10���ֽ�
* @param[out]: unsigned char *code_buf: �洢������Ļ���, code Type + decode data
* @param[in]:  unsigned char inbuf_size: �������������decode_data��buf��С
* @param[out]  unsigned char *code_len:	 ʵ�ʻ�ȡ��������ĳ��ȣ����ʵ�ʻ�ȡ�ĳ��ȱȴ�������buf����ôֻ���ش�������buf��С������
*/
int UE988_get_barcode(unsigned char *code_type, unsigned char *code_buf, unsigned char inbuf_size,unsigned char *code_len)
{
	unsigned char *code_name;
	int		i = 0;

	//scan_start = 1;
	//enable scan
#ifndef UE988_DEBUG
	//UE988_scan_ctrl(UE988_SCAN_ENABLE);

	//start decode
	UE988_start_stop_decode(UE988_START_DECODE);
#endif
	//get code data
	reset_resVar();
	//StartDelay(800);			/* ��ʱ4S		*/    //���µ�ɨ��ͷ�ĳ�ʱ��4S
	//for(i = 0; i < 48; i++)
    for(i = 0; i < wait_time_out; i++)   //��ɨ��ͷ�ĳ�ʱֻ��3S����
	{
		if (g_resCmd.status == RESPONSE_SUCCESS) //�ɹ��յ���Ӧ
		{
#ifdef UE988_DEBUG
			*code_len = 20;
			memcpy(code_buf,g_resCmd.CmdBuffer,20);
            return 0;
#else
			send_ack_to_scanner();
		
			if (g_resCmd.CmdBuffer[1] == DECODE_DATA)
			{
				*code_len	= g_resCmd.CmdBuffer[0]-5;
				memcpy(code_buf, &g_resCmd.CmdBuffer[5], ((*code_len > inbuf_size)?inbuf_size:*code_len));
				code_name	= type2name(g_resCmd.CmdBuffer[4]);
				if ((code_name != 0)&&(code_type != 0))
				{
					strcpy(code_type, code_name);
				}
				//Beep(400);
				//scan_start = 0;
				return 0;
			}
#endif	                        
		
		}//�ɹ��յ���Ӧ
		else if (g_resCmd.status == RESPONSE_NAK)	//��Ӧʧ��
		{
			//scan_start = 0;
			return -1;
		}
		else if (g_resCmd.status == RES_CHECKFAILURE)
		{
#ifndef UE988_DEBUG
			send_nak_to_sanner(RES_CHECKFAILURE);
#endif			
			//scan_start = 0;
			return -1;
		}
		else if (g_resCmd.status == RES_UNKOWN_MSG)
		{
#ifndef UE988_DEBUG
			send_nak_to_sanner(RES_UNKOWN_MSG);
#endif			
			//scan_start = 0;
			return -1;
		}

		OSTimeDlyHMSM(0, 0, 0, 100);
	}//��ʱ

	//scan_start = 0;
	return -1;
}

/*
* @brief: ��ȡ ɨ��ͷ����汾��
* @note unsigned char *softVer 20�ֽڵĻ�����
*/
int UE988_get_softVersion(unsigned char *softVer, unsigned char *plen)
{
	unsigned int cnt;

	pack_command(REQUEST_REVISION, 0, 0, 0);
	g_ack_enable = 0;
	if(write_cmd_to_scanner(g_pReqCmd, 6))
	{
               g_ack_enable = 1;
		return -1;
	}

	//wait response (PARAM_SEND)
	reset_resVar();
	cnt = 180;			/* ��ʱ2S		*/
	while (cnt--)
	{
		if (g_resCmd.status == RESPONSE_SUCCESS) //�ɹ��յ���Ӧ
		{
			if (g_resCmd.CmdBuffer[1] != REPLY_REVISION)	//��Ӧ���ݴ���
			{
				send_nak_to_sanner(RES_UNKOWN_MSG);
				g_ack_enable = 1;
                                return -1;
			}
			else
			{
				send_ack_to_scanner();
				(*plen) = g_resCmd.CmdBuffer[0] - 4;
				memcpy(softVer, g_resCmd.CmdBuffer+4, ((*plen)>20)?20:(*plen));
				if(*plen > 20)
				{
					*plen = 20;
				}
				g_ack_enable = 1;
				return	0;
			}
		}//�ɹ��յ���Ӧ
		else if (g_resCmd.status == RESPONSE_NAK)	//��Ӧʧ��
		{
                  g_ack_enable = 1;
			return -1;
		}
		else if (g_resCmd.status == RES_CHECKFAILURE)
		{
			send_nak_to_sanner(RES_CHECKFAILURE);
			g_ack_enable = 1;
                        return -1;
		}
		else if (g_resCmd.status == RES_UNKOWN_MSG)
		{
			send_nak_to_sanner(RES_UNKOWN_MSG);
			g_ack_enable = 1;
                        return -1;
		}

		//OSTimeDlyHMSM(0,0,0,10);
                Delay(5000);
	}//��ʱ

	//��ʱ
	g_ack_enable = 1;
        return -1;
}


#if 0
void UE988_scan_task(void)
{
	unsigned char cur_key;
	unsigned char code_type[20];
	unsigned char code_len;

	while (1)
	{
		memset(g_decode_data, 0, MAX_DECODE_DATA);
		if (UE988_get_barcode(code_type, g_decode_data, &code_len) == 0)
		{
			gui_TextOut(30, 178, &g_decode_data[0], 1);
			StartDelay(200);			/* ��ʱ1S		*/
			while (DelayIsEnd() != 0)
			{}
		}
		cur_key = *keypad_getkey();
		if (cur_key != KEY_FUN1)
		{
			break;
		}
	}

	gui_TextOut(30, 178, "                  ", 0);
}
#endif