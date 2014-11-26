/**
 * @file  Terminal_Para.c
 * @brief T1Gen��Ŀ���ն˲�������ģ��
 * @version 1.0
 * @author joe
 * @date 2011��03��30��
 * @note
*/  
/* Private include -----------------------------------------------------------*/
#include "Terminal_Para.h"
#include <string.h>
#include "crc32.h"
#include "ff.h"
#include "record.h"
#include "gui.h"

extern FATFS fs;
FIL	 file1;

#define PARAM_FILE	"/device.cfg"


/* private Variable -----------------------------------------------*/
/* external Variable -----------------------------------------------*/

/* Global Variable -----------------------------------------------*/
//�����ն˵�ϵͳ����
TTerminalPara		g_param;				//�ն˲���


/* Private function prototypes -----------------------------------------------*/
/* Private function  -----------------------------------------------*/

/**
* @brief	�ӱ����ն�ϵͳ�����Ĵ洢ģ���ж�ȡ�������浽ȫ�ֱ�����
* @param[in]  none				
* @param[out] �洢�ն˲�����ȫ�ֱ���
* @return     unsigned char  0  :SUCCESS   else : �������
* @note                  
*/
int ReadTerminalPara(void)
{
	unsigned char			*pDst;
	unsigned char			i,j;
	unsigned long			checkvalue;
	int				rd;

	pDst				= (unsigned char *)&g_param;	//ָ��ȫ�ֲ����洢��	
	if( f_open(&file1, PARAM_FILE, FA_OPEN_EXISTING | FA_READ ) != FR_OK )
	{
		return -1;
	}

	if(f_lseek(&file1,0) != FR_OK)
	{
		f_close(&file1);
		return -1;
	}
	
	if (f_read(&file1,(void*)pDst,512,&rd) != FR_OK)
	{
		f_close(&file1);
		return -1;
	}

#if 1
	//����У��ֵ�Ƿ���ȷ
	checkvalue = crc32(0,&g_param.struct_ver,sizeof(TTerminalPara) - 4);
	if (g_param.checkvalue != checkvalue)
	{
		//������У��ֵ����
		return 2;
	}

	// �������Ƿ���ȷ
	if ((g_param.endtag[0] != 0x55)||(g_param.endtag[1] != 0xAA)||(g_param.struct_ver != 2))
	{
		//�����Ľ�����ǲ���
		return 3;
	}

	if (g_param.transfer_mode != TRANSFER_MODE_BLUETOOTH && g_param.transfer_mode != TRANSFER_MODE_KEYBOARD && g_param.transfer_mode != TRANSFER_MODE_VIRTUAL_COMM && g_param.transfer_mode != TRANSFER_MODE_U_DISK && g_param.transfer_mode != TRANSFER_MODE_BATCH)
	{
		return 4;
	}


	if (g_param.batch_transfer_mode != TRANSFER_MODE_BLUETOOTH && g_param.batch_transfer_mode != TRANSFER_MODE_KEYBOARD && g_param.batch_transfer_mode != TRANSFER_MODE_VIRTUAL_COMM)
	{
		return 5;
	}

	if (g_param.scan_trig_mode != SCAN_TRIG_MODE_ONESHOT && g_param.scan_trig_mode != SCAN_TRIG_MODE_KEEP && g_param.scan_trig_mode != SCAN_TRIG_MODE_CONTINUE)
	{
		return 6;
	}

	if (g_param.quantity_enter_mode != QUANTITY_ENTER_MODE_DISABLE && g_param.quantity_enter_mode != QUANTITY_ENTER_MODE_NUM && g_param.quantity_enter_mode != QUANTITY_ENTER_MODE_REPEAT)
	{
		return 7;
	}

	if (g_param.operate_timeout != 0 && g_param.operate_timeout != 1 && g_param.operate_timeout != 2 && g_param.operate_timeout != 3 && g_param.operate_timeout != 4)
	{
		return 8;
	}

	if ((g_param.valid_barcode_start_offset != 0xff)||(g_param.valid_barcode_end_offset != 0xff))
	{
		if (g_param.valid_barcode_start_offset >= g_param.valid_barcode_end_offset)
		{
			//��ͬʱ����0xffʱ�������ʼλ���ڽ���λ����ʾ�����쳣
			return 19;
		}
	}

	//�����������Ƿ���ȷ
	//@todo....
#endif
	return 0;	
}

/**
* @brief	���û��������ն�ϵͳ�������浽�洢ģ��
* @param[in]   none				
* @param[in] �洢�ն˲�����ȫ�ֱ���
* @return     none
* @note   �˺�����ʵ�ֵ��ǽ��������浽PSAM����                
*/
int SaveTerminalPara(void)
{
	unsigned char		*pSrc;
	unsigned int		wr;

	pSrc				= (unsigned char *)&g_param;

	// ���¼���У��        
	g_param.checkvalue = crc32(0,&g_param.struct_ver, sizeof(TTerminalPara)-4);

	if( f_open(&file1, PARAM_FILE, FA_OPEN_ALWAYS | FA_WRITE ) != FR_OK )
	{
		return -1;
	}

	if(f_lseek(&file1,0) != FR_OK)
	{
		f_close(&file1);
		return -1;
	}

	if (f_write(&file1,(void*)pSrc,sizeof(TTerminalPara),&wr) != FR_OK)
	{
		f_close(&file1);
		return -1;
	}

	if (wr != sizeof(TTerminalPara))
	{
		f_close(&file1);
		return -1;
	}
	 
	f_close(&file1);
	return 0;
}


/**
* @brief ����Ĭ�ϵĲ��Բ���������
*/
int DefaultTerminalPara(void)
{
	unsigned char		*pSrc;
	//unsigned char		i,tmp[2];

	pSrc				= (unsigned char *)&g_param;
	//��������в���
	memset(pSrc,0,sizeof(TTerminalPara));
	//���ý������
	g_param.endtag[0]		= 0x55;
	g_param.endtag[1]		= 0xAA;
	//���ò����ṹ�汾��
	g_param.struct_ver		= 0x02;

	
	//����Ĭ�ϵ��ն˲���
	g_param.operate_timeout = 0;		//120S

	//Ĭ�ϵ�ϵͳ����:12345678
	memcpy(g_param.system_psw,"12345678",8);

	g_param.decoder_switch_map = 0xffff;	//Ĭ�����н���������
	g_param.valid_barcode_start_offset = 0xff;
	g_param.valid_barcode_end_offset = 0xff;
	g_param.language = LANG_SCHINESE;
	g_param.scan_trig_mode = 1;
	g_param.quantity_enter_mode = 0;
	g_param.add_time_option = 0;
	g_param.add_date_option = 0;
	g_param.database_query_option = 0;
	g_param.avoid_dunplication_option = 0;
	g_param.barcode_statistic_option = 0;
	g_param.transfer_confirm_option = 0;
	g_param.transfer_mode = 1;
	g_param.batch_transfer_mode = 1;
	g_param.beeper_vol = 1;
	strcpy(g_param.transfer_seperator,",");	//Ĭ�ϵķָ����Ƕ���
	strcpy(g_param.u_disk_storage_file,"Scan.TXT");//U��ģʽ��Ĭ�ϴ洢�ļ�
	
	return SaveTerminalPara();
}

