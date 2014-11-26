/**
 * @file  Terminal_Para.c
 * @brief T1Gen项目的终端参数管理模块
 * @version 1.0
 * @author joe
 * @date 2011年03月30日
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
//定义终端的系统参数
TTerminalPara		g_param;				//终端参数


/* Private function prototypes -----------------------------------------------*/
/* Private function  -----------------------------------------------*/

/**
* @brief	从保存终端系统参数的存储模块中读取参数保存到全局变量中
* @param[in]  none				
* @param[out] 存储终端参数的全局变量
* @return     unsigned char  0  :SUCCESS   else : 错误代码
* @note                  
*/
int ReadTerminalPara(void)
{
	unsigned char			*pDst;
	unsigned char			i,j;
	unsigned long			checkvalue;
	int				rd;

	pDst				= (unsigned char *)&g_param;	//指向全局参数存储区	
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
	//计算校验值是否正确
	checkvalue = crc32(0,&g_param.struct_ver,sizeof(TTerminalPara) - 4);
	if (g_param.checkvalue != checkvalue)
	{
		//参数的校验值不对
		return 2;
	}

	// 检查参数是否正确
	if ((g_param.endtag[0] != 0x55)||(g_param.endtag[1] != 0xAA)||(g_param.struct_ver != 2))
	{
		//参数的结束标记不对
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
			//不同时等于0xff时，如果起始位大于结束位，表示参数异常
			return 19;
		}
	}

	//检查其余参数是否正确
	//@todo....
#endif
	return 0;	
}

/**
* @brief	将用户变更后的终端系统参数保存到存储模块
* @param[in]   none				
* @param[in] 存储终端参数的全局变量
* @return     none
* @note   此函数中实现的是将参数保存到PSAM卡中                
*/
int SaveTerminalPara(void)
{
	unsigned char		*pSrc;
	unsigned int		wr;

	pSrc				= (unsigned char *)&g_param;

	// 重新计算校验        
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
* @brief 建立默认的测试参数并保存
*/
int DefaultTerminalPara(void)
{
	unsigned char		*pSrc;
	//unsigned char		i,tmp[2];

	pSrc				= (unsigned char *)&g_param;
	//先清空所有参数
	memset(pSrc,0,sizeof(TTerminalPara));
	//设置结束标记
	g_param.endtag[0]		= 0x55;
	g_param.endtag[1]		= 0xAA;
	//设置参数结构版本号
	g_param.struct_ver		= 0x02;

	
	//构造默认的终端参数
	g_param.operate_timeout = 0;		//120S

	//默认的系统密码:12345678
	memcpy(g_param.system_psw,"12345678",8);

	g_param.decoder_switch_map = 0xffff;	//默认所有解码器都打开
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
	strcpy(g_param.transfer_seperator,",");	//默认的分隔符是逗号
	strcpy(g_param.u_disk_storage_file,"Scan.TXT");//U盘模式的默认存储文件
	
	return SaveTerminalPara();
}

