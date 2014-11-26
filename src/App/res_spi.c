


/**
* @file res_spi.c
* @brief ˢ������Դ����ģ��,���ڹ����ֿ⣬ͼƬ���洢�����ȹ���
*        ��ģ�齫�ֿ����Դ�洢��SPI FLASH��
* @version V0.0.2
* @author joe
* @date 2010��2��5��
* @note
*     �޸ģ�
*			������Ŀ������Ҫ֧��GBK�ֿ⣬����������һ��12*12��GB2312�ֿ⣬SPI FLASH�������������Խ�12*12��
*			��GB2312�ֿ�ŵ�MCU FLASH�ڲ�ȥ�ˡ�
*
* @version V0.0.1
* @author zhongyh
* @date 2009��9��4��
* @note
*     ÿ�ֲ�ͬ����Դ����һ��������BPT�У�����BPT_DataType������
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
*	@brief	��Դ�ļ���Ϣ�ṹ��
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
*	@brief	ÿһ����Դ����Ϣ�ṹ��
*/
#pragma pack(1)
typedef struct {
	unsigned char				RIT_OEMName[16];    		// 0  
	unsigned char				RIT_ManName[16];    		// 10 
	unsigned char				RIT_ModuleName[16];			// 20 
	unsigned short				RIT_ProdID;					// 30 ��ƷID
	unsigned short				RIT_CodeVersion;    		// 32 ��������汾
	unsigned short				RIT_Date;            		// 34 ����ʱ��
	unsigned char				RIT_SN[10];            		// 36 ���к�
	unsigned int				RIT_TotSect;        		// 40 ��������ռ��������
	unsigned int				RIT_CodeSec;        		// 44 ��������ռ��������
	unsigned char				RIT_VerifyType;        		// 48 ��������У�鷽ʽ
	unsigned char				RIT_Endian;        			// 49 CPU Core
	unsigned short				RIT_DataType;         		// 4A �˷����ݵ����ͣ���������������룬��ָ�����������ͣ�
	unsigned int				RIT_StartAddr;        		// 4C ����������ram�д�ŵ���ʼ��ַ
	unsigned int				RIT_RunAddr;        		// 50 ���д���ʱPC��ת�ĵ�ַ
	unsigned char   			RIT_DataID;					// 54 �˷���Դ��ID��
	unsigned char				RIT_Reserved0;				// 55
	unsigned char				RIT_Align;					// 56 �˷���Դ����С���뵥λ��Ϊ2^n�ֽ�
	unsigned char				RIT_Reserved1;				// 57
	unsigned char				RIT_StrVer[13];				// 58  ��Դ�İ汾�ţ���ʽΪ20100610_0001
	unsigned char				RIT_Reserved[0x18B];		// 65 
	unsigned int				RIT_DataCRC32;				// 1F0 ��������CRC32��������һ�������𣬵�RIT_CodeSecֹ���������ݵ�CRC32ֵ��CRC���������xml�ļ��е����������ɡ�
	unsigned int				RIT_Next;            		// 1F4 ��һ��RIT�ڴ����еľ��Ե�ַ
	unsigned int				RIT_CheckSum;        		// 1F8 �˷���Դ��RIT��CRC32У��ֵ���� 
	unsigned int				RIT_TrailSig;       		// 1FC ������־(0x00,0x00,0x55,0xAB)
}TResInfoTable;

#pragma pack()

static unsigned char			res_buffer[512];

static FIL							resource_file;		//��Դ�ļ�ר��һ���ļ����
FATFS								res_fs;
#define res_file					"/resdata.bin"

/**
* @def ÿһ����Դ������BPT�У�BPT_DataType��Ӧ��ֵ
*/
#define RESTYPE_CODE		0x80					// ϵͳ��������
#define RESTYPE_FNT			0x10					// �ֿ���Դ��
#define RESTYPE_PIC			0x11					// ͼƬ��Դ��

/**
* @brief ��ָ����ַ��ʼ����BPT��������Ƿ���ͬ
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
* @brief ��Դģ���ʼ��
* @return 0:�ɹ���ʼ��
*        -1:��ʼ��SD��ʧ��
*        -2:SPI Flash���Ҳ�����Դ
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

	//�����ļ�ϵͳ
	f_mount(0, &res_fs);										//װ���ļ�ϵͳ

	
	ret = f_open(&resource_file, res_file, FA_OPEN_EXISTING | FA_READ ); 
	if(ret != FR_OK )
	{
		if (ret == FR_NO_FILESYSTEM)
		{
			//����ǻ�û�д����ļ�ϵͳ����ô�͸�ʽ���ļ�ϵͳ
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
	// 1. У����Դ�ļ�
	if( f_read(&resource_file, res_buffer, 512, &rd) != FR_OK )
	{
		f_close(&resource_file);
		return -2;
	}


	// У���ļ�ͷ
	if( res_buffer[0] != 'J' || res_buffer[1] != 'B' || res_buffer[2] != 'L' || res_buffer[3] != '3' )
	{
		f_close(&resource_file);
		return -2;
	}

	// װ����Դ
	dwLBA							= 0;
	do
	{
		// װ��һ����
		dwLBA += 1;
		if( res_loadres(dwLBA, &res_type, &res_id) != 0)
		{
			f_close(&resource_file);
			return -2;
		}

		// �ж���Դ����
		if(RESTYPE_FNT == res_type)
		{
			gui_setup_font(dwLBA+1, res_id);		// ��װ����
		}
		else if(RESTYPE_PIC == res_type)
		{
			gui_setup_picture(dwLBA + 1);
		}
		else
		{
			//return -2;
		}

		// ȡ��һ����Դ��ַ
		dwLBA						= *(unsigned int*)&res_buffer[0x1F4];
	}while(dwLBA != 0xFFFFFFFF);

	return 0;
}

/**
* @brief ��SPI FLASH�ж�ȡһ����������Դ��������res_buffer��
* @param[in] unsigned int dwLBAҪ��ȡ����Դ��ַ
* @return unsigned char *���ݴ���ڵ�ַ
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

	spi_flash_erase();	//�Ȳ�������SPI_FLASH

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

	spi_flash_erasesector(0);	//������һ��4K

	//��������һ��4Kʱ�Ƿ񽫵�5��SectorҲ������
	memset(buf,0,512);
	disk_read(1,buf,8,1);

	//��������һ��4K֮���Ƿ�����ٴα��
	memset(buf,0x50,512);		
	disk_write(1,buf,0,1);		//д��1��Sector

	memset(buf,0,512);
	disk_read(1,buf,0,1);

	memset(buf,0x57,512);
	disk_write(1,buf,7,1);		//д��8��sector		

	memset(buf,0,512);
	disk_read(1,buf,7,1);


	spi_flash_erasesector(0x1000);	//������2��4K

	//��������2��4Kʱ�Ƿ񽫵�5��SectorҲ������
	memset(buf,0,512);
	disk_read(1,buf,8,1);
}
#endif


