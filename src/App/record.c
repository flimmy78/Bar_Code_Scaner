/**
* @file record.c
* @version V0.0.1
* @author joe.zhou
* @date 2014年11月26日
* @note   本记录模块利用STM32的FLASH存储区的最后1K的空间保存一些需要掉电保存的信息
*          由于需要保存的信息比较简单，但是更新会比较频繁，所以采取如下策略：
*          基准条码  ---  更新频率不是很高，采取直接覆盖的方法更新
*          条码计数器 --- 更新频率很高，考虑到FLASH的寿命，采取增量更新的方式
*/


//@note:  注意大容量产品（256K-512K FLASH ROM SIZE）的Flash Page = 2K，一次擦除2K

#include <string.h>
#include "record.h"
#include "stm32f10x_flash_config.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_flash.h"

//掉电需要保存的信息
unsigned char  basical_barcode[32];		//基准条码,最后一个字节是XOR校验值
unsigned int   scan_count;				//条码计数器
static unsigned int	  count_page;				//条码计数器的页数

#define BASICAL_BARCODE_TAG		0x14725836

/**
* @brief 擦除MCU FLASH中的参数PAGE
* @param[in] int address 相对于参数区的起始地址
*/
static int erase_page(int page_addr)
{
	int i;
	if(FLASH_ErasePage(page_addr) != FLASH_COMPLETE)
	{
		for (i=0;i<1000;i++);
		if(FLASH_ErasePage(page_addr) != FLASH_COMPLETE)
		{
			return -1;
		}
	}
	return 0;
}


//记录的初始化
int record_init(void)
{
	unsigned int i,ff_flag=1;
	unsigned char *p;
	unsigned char tmp,check = 0;
	unsigned short n;

	memset(basical_barcode,0,32);
	scan_count = 0;

    p = basical_barcode;
	for (i = 0;i < 32;i++)
	{
		*p = *((unsigned char*)(FLASH_START + BASICAL_BARCODE_ADDR + i));
		check ^= *p;
		if (*p!=0xff)
		{
			ff_flag = 0;
		}
		p++;
	}

	if (ff_flag)
	{
		memset(basical_barcode,0,32);
	}

	if (check != 0)
	{
		memset(basical_barcode,0,32);
		if (ff_flag == 0)
		{
			if (erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
			{
				if (erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
				{
					return -1;
				}
			}
		}
	}

	count_page = *((unsigned int*)(FLASH_START + SCAN_COUNT_PAGE_ADDR));

	if (count_page == 0xffffffff)
	{
		count_page = 0;
	}
	scan_count = count_page*1024;

	for (i = 0 ;i < 1024;i++)
	{
		n = *((unsigned short*)(FLASH_START + SCAN_COUNT+2*i));
		if (n == i+1)
		{
			scan_count++;
		}
		else
		{
			if (n != 0xffff)
			{
				scan_count = count_page*1024;
				if (erase_page(FLASH_START+SCAN_COUNT))
				{
					if (erase_page(FLASH_START+SCAN_COUNT))
					{
						return -1;
					}
				}
			}
		}
	}

	return 0;
}


//读取记录
int record_read(unsigned char rectype, unsigned char *pBuf)
{
	if (REC_TYPE_BASCIAL_BARCODE == rectype)
	{
		memcpy(pBuf,basical_barcode,31);
	}
	else if (REC_TYPE_COUNTER == rectype)
	{
		*((unsigned int*)pBuf) = scan_count;
	}
	else
	{
		return -1;
	}

	return 0;
}


//写记录
//对于REC_TYPE_COUNTER记录来说，每调用一次此函数，表示计数器+1
int record_write(unsigned char rectype, unsigned char *pRec)
{
	unsigned char check=0;
	unsigned int i,n;
	unsigned char *p;
	unsigned short cnt;

	if (REC_TYPE_BASCIAL_BARCODE == rectype)
	{
		if (strlen((const char*)pRec)>30)
		{
			return -1;
		}

		memset(basical_barcode,0,32);
		strcpy((char*)basical_barcode,(const char*)pRec);
		for (i = 0;i<30;i++)
		{
			check ^= basical_barcode[i];
		}

		basical_barcode[31] = check;

		FLASH_Unlock();
		//擦除MCU FLASH的参数区
		if(erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
		{
			if(erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
			{
				FLASH_Lock();
				return -2;
			}
		}

		p = basical_barcode;
		for (i=0;i<8;i++)
		{
			((unsigned char*)&n)[0] = *(p+i*4);
			((unsigned char*)&n)[1] = *(p+i*4+1);
			((unsigned char*)&n)[2] = *(p+i*4+2);
			((unsigned char*)&n)[3] = *(p+i*4+3);
			if(FLASH_ProgramWord(FLASH_START + BASICAL_BARCODE_ADDR + i*4, n) != FLASH_COMPLETE)
			{
				if(FLASH_ProgramWord(FLASH_START + BASICAL_BARCODE_ADDR + i*4, n) != FLASH_COMPLETE)
				{
					FLASH_Lock();
					return -3;
				}
			}
		}

		scan_count = 0;
		if(erase_page(FLASH_START+SCAN_COUNT))
		{
			if(erase_page(FLASH_START+SCAN_COUNT))
			{
				FLASH_Lock();
				return -2;
			}
		}

		FLASH_Lock();
		
	}
	else if (REC_TYPE_COUNTER == rectype)
	{
                scan_count++;
		cnt = scan_count%1024;
		if (cnt == 0)
		{
			FLASH_Unlock();

			//擦除MCU FLASH的参数区
			if(erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
			{
				if(erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
				{
					FLASH_Lock();
					return -2;
				}
			}

			p = basical_barcode;
			for (i=0;i<8;i++)
			{
				((unsigned char*)&n)[0] = *(p+i*4);
				((unsigned char*)&n)[1] = *(p+i*4+1);
				((unsigned char*)&n)[2] = *(p+i*4+2);
				((unsigned char*)&n)[3] = *(p+i*4+3);
				if(FLASH_ProgramWord(FLASH_START + BASICAL_BARCODE_ADDR + i*4, n)!= FLASH_COMPLETE)
				{
					if(FLASH_ProgramWord(FLASH_START + BASICAL_BARCODE_ADDR + i*4, n)!= FLASH_COMPLETE)
					{
						FLASH_Lock();
						return -3;
					}
				}
			}

			count_page++;
			if(FLASH_ProgramWord(FLASH_START + SCAN_COUNT_PAGE_ADDR, count_page)!=FLASH_COMPLETE)
			{
				if(FLASH_ProgramWord(FLASH_START + SCAN_COUNT_PAGE_ADDR, count_page)!=FLASH_COMPLETE)
				{
					FLASH_Lock();
					return -4;
				}

			}

			if(erase_page(FLASH_START+SCAN_COUNT))
			{
				if(erase_page(FLASH_START+SCAN_COUNT))
				{
					FLASH_Lock();
					return -5;
				}
			}

			FLASH_Lock();
		}
		else
		{
			for (i = 0 ;i < 1024;i++)
			{
				n = *((unsigned short*)(FLASH_START + SCAN_COUNT+2*i));
				if (n == 0xffff)
				{
					if (cnt == i+1)
					{
						FLASH_Unlock();
						if (FLASH_ProgramHalfWord(FLASH_START + SCAN_COUNT + i*2, cnt)!= FLASH_COMPLETE)
						{
							if (FLASH_ProgramHalfWord(FLASH_START + SCAN_COUNT + i*2, cnt)!= FLASH_COMPLETE)
							{
								FLASH_Lock();
								return -2;
							}
						}
						
						FLASH_Lock();
                                                return 0;
					}
					else
					{
						return -3;
					}
				}
			}
		}
	}
	else
	{
		return -1;
	}
	return 0;
}


int record_clear(void)
{
	memset(basical_barcode,0,32);
	scan_count = 0;

	FLASH_Unlock();
	if (erase_page(FLASH_START+BASICAL_BARCODE_ADDR))
	{
		erase_page(FLASH_START+BASICAL_BARCODE_ADDR);
	}

	if (erase_page(FLASH_START+SCAN_COUNT))
	{
		erase_page(FLASH_START+SCAN_COUNT);
	}

	FLASH_Lock();
}


