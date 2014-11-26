/**
***************************************************************************
* @file res_upgrade.c
* @brief 资源升级
***************************************************************************
*
* @version V0.0.1
* @author zhongyh
* @date 2010年12月02日
* @note
*      1.移植自hbs boot_master.c
*
***************************************************************************
*
* @copy
*
* 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
* 本公司以外的项目。本司保留一切追究权利。
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
* @brief 判断是否插入了资源卡
* @return 0: 没有插入资源卡		1:插入了资源卡
*/
//int res_card_insert(void)
//{
//	f_mount(0, &fs);										// 装载文件系统

//	if( f_open(&file1, resupdate_file, FA_OPEN_EXISTING | FA_READ ) != FR_OK )
//	{
//		return 0;
//	}
//	return 1;
//}
/**
***************************************************************************
*@brief 判断是否有资源升级文件的存在，如果有升级文件存在就进行资源升级
*@param[in] 
*@return 0 升级成功  else; 失败
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

	//挂载文件系统
	f_mount(0, &fs);										// 装载文件系统


	if (check_resdatafile())
	{
		//资源升级文件不存在或者升级文件错误
		return -1;
	}

	//资源升级文件存在，将升级的资源文件替换掉原来的资源文件
	//先将原来的资源文件删除，然后将资源升级文件的文件名修改即可
	if (f_unlink("/resdata.bin") != FR_OK)
	{
		return -2;	//删除旧的资源文件失败
	}

	if (f_rename("/resdata.tmp","/resdata.bin") != FR_OK)
	{
		return -3;	//修改升级资源文件名失败
	}

	return 0;
}



