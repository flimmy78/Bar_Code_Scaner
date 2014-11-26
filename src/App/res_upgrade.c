/**
***************************************************************************
* @file res_upgrade.c
* @brief ��Դ����
***************************************************************************
*
* @version V0.0.1
* @author zhongyh
* @date 2010��12��02��
* @note
*      1.��ֲ��hbs boot_master.c
*
***************************************************************************
*
* @copy
*
* �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
* ����˾�������Ŀ����˾����һ��׷��Ȩ����
*
* <h1><center>&copy; COPYRIGHT 2010 netcom</center></h1>
*/
#include "res_upgrade.h"
#include "res_spi.h"
#include "ff.h"
#include <stdio.h>
#include <string.h>
#include "record.h"
#include "spi_sd.h"

#define resupdate_file				"/resdata.bin"


extern FATFS						fs;
extern FIL				file1;


/**
* @brief �ж��Ƿ��������Դ��
* @return 0: û�в�����Դ��		1:��������Դ��
*/
//int res_card_insert(void)
//{
//	f_mount(0, &fs);										// װ���ļ�ϵͳ

//	if( f_open(&file1, resupdate_file, FA_OPEN_EXISTING | FA_READ ) != FR_OK )
//	{
//		return 0;
//	}
//	return 1;
//}
/**
***************************************************************************
*@brief �ж��Ƿ�����Դ�����ļ��Ĵ��ڣ�����������ļ����ھͽ�����Դ����
*@param[in] 
*@return 0 �����ɹ�  else; ʧ��
*@warning
*@see	
*@note 
***************************************************************************
*/
int res_upgrade(void)
{

	if (SPI_SD_Init() != SD_RESPONSE_NO_ERROR)
	{
		return -1;
	}

	//�����ļ�ϵͳ
	f_mount(0, &fs);										// װ���ļ�ϵͳ


	if (check_resdatafile())
	{
		//��Դ�����ļ������ڻ��������ļ�����
		return -1;
	}

	//��Դ�����ļ����ڣ�����������Դ�ļ��滻��ԭ������Դ�ļ�
	//�Ƚ�ԭ������Դ�ļ�ɾ����Ȼ����Դ�����ļ����ļ����޸ļ���
	if (f_unlink("/resdata.bin") != FR_OK)
	{
		return -2;	//ɾ���ɵ���Դ�ļ�ʧ��
	}

	if (f_rename("/resdata.tmp","/resdata.bin") != FR_OK)
	{
		return -3;	//�޸�������Դ�ļ���ʧ��
	}

	return 0;
}



