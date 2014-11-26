


/**
* @file res_spi.c
* @brief 刷卡机资源管理模块,用于管理字库，图片，存储参数等功能
*        本模块将字库等资源存储在SPI FLASH中
* @version V0.0.2
* @author joe
* @date 2010年2月5日
* @note
*     修改：
*			由于项目需求需要支持GBK字库，而且增加了一个12*12的GB2312字库，SPI FLASH容量不够，所以将12*12的
*			的GB2312字库放到MCU FLASH内部去了。
*
* @version V0.0.1
* @author zhongyh
* @date 2009年9月4日
* @note
*     每种不同的资源，在一个独立的BPT中，利用BPT_DataType来区分
*
* 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x_lib.h"
#include "gui.h"
#include "res_spi.h"
#include "ff.h"
#include "diskio.h"
/**
*	@brief	资源文件信息结构体
*/
typedef struct  
{
	unsigned char					magic[4];
	unsigned int					xor;
	unsigned int					xor_data;
	unsigned int					length;
	unsigned char					OEMName[16];
	unsigned char					Version[16];
	unsigned char					Date[16];
}TPackHeader;

/**
*	@brief	每一份资源的信息结构体
*/
#pragma pack(1)
typedef struct {
	unsigned char				RIT_OEMName[16];    		// 0  
	unsigned char				RIT_ManName[16];    		// 10 
	unsigned char				RIT_ModuleName[16];			// 20 
	unsigned short				RIT_ProdID;					// 30 产品ID
	unsigned short				RIT_CodeVersion;    		// 32 引导代码版本
	unsigned short				RIT_Date;            		// 34 更新时间
	unsigned char				RIT_SN[10];            		// 36 序列号
	unsigned int				RIT_TotSect;        		// 40 引导分区占的扇区数
	unsigned int				RIT_CodeSec;        		// 44 引导代码占的扇区数
	unsigned char				RIT_VerifyType;        		// 48 引导代码校验方式
	unsigned char				RIT_Endian;        			// 49 CPU Core
	unsigned short				RIT_DataType;         		// 4A 此份数据的类型（如果不是引导代码，则指引导数据类型）
	unsigned int				RIT_StartAddr;        		// 4C 启动代码在ram中存放的起始地址
	unsigned int				RIT_RunAddr;        		// 50 运行代码时PC跳转的地址
	unsigned char   			RIT_DataID;					// 54 此份资源的ID号
	unsigned char				RIT_Reserved0;				// 55
	unsigned char				RIT_Align;					// 56 此份资源的最小对齐单位，为2^n字节
	unsigned char				RIT_Reserved1;				// 57
	unsigned char				RIT_StrVer[13];				// 58  资源的版本号，格式为20100610_0001
	unsigned char				RIT_Reserved[0x18B];		// 65 
	unsigned int				RIT_DataCRC32;				// 1F0 数据区的CRC32，即从下一个扇区起，到RIT_CodeSec止的所有数据的CRC32值或CRC，此项根据xml文件中的配置来生成。
	unsigned int				RIT_Next;            		// 1F4 下一份RIT在磁盘中的绝对地址
	unsigned int				RIT_CheckSum;        		// 1F8 此份资源表RIT的CRC32校验值。， 
	unsigned int				RIT_TrailSig;       		// 1FC 结束标志(0x00,0x00,0x55,0xAB)
}TResInfoTable;

#pragma pack()

static unsigned char			res_buffer[512];

static FIL							resource_file;		//资源文件专享一个文件句柄
FATFS								res_fs;
#define res_file					"/resdata.bin"

/**
* @def 每一份资源类型在BPT中，BPT_DataType对应的值
*/
#define RESTYPE_CODE		0x80					// 系统升级代码
#define RESTYPE_FNT			0x10					// 字库资源号
#define RESTYPE_PIC			0x11					// 图片资源号

/**
* @brief 从指定地址开始载入BPT，并检查是否相同
*/
static int res_loadres(unsigned int dwLBA, unsigned char *pType, unsigned char *pID)
{

	TResInfoTable			*pResTable;
	unsigned int			rd;

	if (f_lseek(&resource_file,dwLBA*512) != FR_OK)
	{
		return -1;
	}

	if (f_read(&resource_file,res_buffer,512,&rd) != FR_OK)
	{
		return -1;
	}


	pResTable	= (TResInfoTable*)res_buffer;

	if( res_buffer[510] != 0x55 ||
		res_buffer[511] != 0xAB)
	{
		return -1;
	}

	*pType					= pResTable->RIT_DataType;
	*pID					= pResTable->RIT_DataID;

	return 0;
}

/**
* @brief 资源模块初始化
* @return 0:成功初始化
*        -1:初始化SD卡失败
*        -2:SPI Flash中找不到资源
*/
int res_init(void)
{

	unsigned int					dwLBA;
	unsigned char					res_type, res_id;
	unsigned int					rd;

	FRESULT				ret;

	//if (SPI_SD_Init() != SD_RESPONSE_NO_ERROR)
	//{
	//	return -1;
	//}

	//挂载文件系统
	f_mount(0, &res_fs);										//装载文件系统

	
	ret = f_open(&resource_file, res_file, FA_OPEN_EXISTING | FA_READ ); 
	if(ret != FR_OK )
	{
		if (ret == FR_NO_FILESYSTEM)
		{
			//如果是还没有创建文件系统，那么就格式化文件系统
			ret = f_mkfs(0,1,512);

			if (ret != FR_OK)
			{
				return -1;
			}
		}             
       return -1;
	}

	f_lseek(&resource_file,0);

	// ==========================================================================================================
	// 1. 校验资源文件
	if( f_read(&resource_file, res_buffer, 512, &rd) != FR_OK )
	{
		f_close(&resource_file);
		return -2;
	}


	// 校验文件头
	if( res_buffer[0] != 'J' || res_buffer[1] != 'B' || res_buffer[2] != 'L' || res_buffer[3] != '3' )
	{
		f_close(&resource_file);
		return -2;
	}

	// 装入资源
	dwLBA							= 0;
	do
	{
		// 装入一个表
		dwLBA += 1;
		if( res_loadres(dwLBA, &res_type, &res_id) != 0)
		{
			f_close(&resource_file);
			return -2;
		}

		// 判断资源类型
		if(RESTYPE_FNT == res_type)
		{
			gui_setup_font(dwLBA+1, res_id);		// 安装字体
		}
		else if(RESTYPE_PIC == res_type)
		{
			gui_setup_picture(dwLBA + 1);
		}
		else
		{
			//return -2;
		}

		// 取下一份资源地址
		dwLBA						= *(unsigned int*)&res_buffer[0x1F4];
	}while(dwLBA != 0xFFFFFFFF);

	return 0;
}

/**
* @brief 从SPI FLASH中读取一个扇区的资源，并放在res_buffer中
* @param[in] unsigned int dwLBA要读取的资源地址
* @return unsigned char *数据存放在地址
*/
unsigned char *read_resdata(unsigned int dwLBA)
{
	unsigned int	rd;
	unsigned int	retry_cnt = 2; 

	if (resource_file.flag & FA__ERROR)
	{
		resource_file.flag &= ~FA__ERROR;
	}

	while(retry_cnt)
	{
		if (f_lseek(&resource_file,dwLBA*512) != FR_OK)
		{
			retry_cnt--;
			f_mount(1,&res_fs);
			if(f_open(&resource_file,res_file,FA_OPEN_EXISTING | FA_READ) != FR_OK)
			{
				return (unsigned char *) 0; 
			}
			else
			{
				continue;
			}
		}

		if (f_read(&resource_file,res_buffer,512,&rd) != FR_OK)
		{
			retry_cnt--;
			f_mount(1,&res_fs);
			if(f_open(&resource_file,res_file,FA_OPEN_EXISTING | FA_READ) != FR_OK)
			{
				return (unsigned char *) 0; 
			}
			else
			{
				continue;
			}
		}

		return res_buffer;
	}

	return (unsigned char *) 0;
}


#if 0
void spi_flash_drv_test(void)
{
	unsigned char buf[512];

	spi_flash_init();

	spi_flash_erase();	//先擦除整个SPI_FLASH

	memset(buf,0x50,512);
	disk_write(1,buf,0,1);

	memset(buf,0,512);
	disk_read(1,buf,0,1);

	memset(buf,0x51,512);
	disk_write(1,buf,1,1);

	memset(buf,0,512);
	disk_read(1,buf,1,1);

	memset(buf,0x58,512);
	disk_write(1,buf,8,1);

	memset(buf,0,512);
	disk_read(1,buf,8,1);

	spi_flash_erasesector(0);	//擦除第一个4K

	//检查擦除第一个4K时是否将第5个Sector也擦除了
	memset(buf,0,512);
	disk_read(1,buf,8,1);

	//检查擦除第一个4K之后，是否可以再次编程
	memset(buf,0x50,512);		
	disk_write(1,buf,0,1);		//写第1个Sector

	memset(buf,0,512);
	disk_read(1,buf,0,1);

	memset(buf,0x57,512);
	disk_write(1,buf,7,1);		//写第8个sector		

	memset(buf,0,512);
	disk_read(1,buf,7,1);


	spi_flash_erasesector(0x1000);	//擦除第2个4K

	//检查擦除第2个4K时是否将第5个Sector也擦除了
	memset(buf,0,512);
	disk_read(1,buf,8,1);
}
#endif


