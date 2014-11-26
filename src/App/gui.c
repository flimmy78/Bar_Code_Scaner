/**
* @file gui.c
* @brief 刷卡机项目的UI驱动模块
*
* @version V0.0.1
* @author zhongyh
* @date 2009年8月28日
* @note
*      提供非接刷卡器界面接口
*      320x240
*      前景背景色选定后，GUI模块所有函数按此参数进行绘制
*      
*      
* @note 文字输出说明：
*      
*      本模块文字部分为16,24,32三种字库，在选择字体后，调用文字输出函数即为所选字体。
*      文字输出时简单计算输出的宽度，多余部分换行输出。直到满屏或字符截止为止。
*      字库存放在Data SDCard里
*     增加32号字体16*32的ASCII字体和32*32的中文字体
*     竖屏显示改为横屏显示
*
*
* @copy
*
* 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
* 本公司以外的项目。本司保留一切追究权利。
*
* <h1><center>&copy; COPYRIGHT 2009 netcom</center></h1>
*/


#include "gui.h"
#include "lcd.h"
#include "res_spi.h"
#include "AsciiLib_6x12.h"
#include "cpu.h"
#include "pic.h"

// 关于 TFontInfo 的type说明
// BIT 7 6 5 4 3 2 1 0
//     | | | |       +----  0 ROM字库 1资源卡字库
//     +-+-+-+------------  0 标准字库 1:FontMaker字库 其他，未定义
//



static unsigned short				bgColor;
static unsigned short				textColor;

unsigned char						gFontSize;			// 字号
unsigned char				g_language;			// 文字类型

extern unsigned short			edit_display_x_patch;

#define MAX_FNT_CNT				FNT_CHN_12_12


/**
*	@brief  刷卡机项目中用到的字体信息表
*/
TFontInfo FontList[MAX_FNT_CNT+1] = {
	{	0, 6,  12, 12, 0	},							// ascii 8x12
	{	0, 12, 12, 24, 0	},							// chn 12x12
};

unsigned int						dwPictureLBA;


/**
* @brief 初始化显示模块
*/
void gui_init(void)
{
	Lcd_init();											// 初始化LCD硬件
	//Lcd_init();											// 初始化LCD硬件
	Lcd_Clear(BG_COLOR);

	FontList[FNT_ASC_6_12].address	= (unsigned int)AsciiLib_6x12;
	FontList[FNT_CHN_12_12].address	= 0;

	dwPictureLBA					= 0;

	gui_SetFont(12);
	gui_SetTextColor(TXT_COLOR);
}

/**
* @brief 装入字库信息
* @note 从SD卡中找到相应的字库
*       每个字库有一个独立的BPT
*/
#if 0
void gui_load_font(void)
{
	int				i;

	for(i=0; i<=MAX_FNT_CNT; i++)
	{
		if(FontList[i].type == 0)
		{
		}
		else
		{	// 从sd卡装载
		}
	}
}
#endif

/**
* @brief 安装中文字库
*/
int gui_setup_font(unsigned int dwLBA, unsigned char id)
{
	if( id == FNT_ASC_6_12 || id == FNT_CHN_12_12)
	{
		FontList[id].address = dwLBA;
	}
	return 0;
}


void gui_clear(unsigned short color)
{
	Lcd_Clear(color);
}

#if 0
/**
* @brief 取得ASCII字库信息
*/
static void 
GetAscFontData(
			   char* pBuffer, 
			   FONT* pFont,
			   char* pChar
			   )
{
	int								iOffset;

	iOffset							= pFont->iOffset + pFont->FontLibHeader.iSize 
		+ pFont->FontLibHeader.iAscOffset;

	iOffset							= iOffset+(*pChar) * pFont->FontLibHeader.iAscBytes;

	shm_read(pBuffer, iOffset, pFont->FontLibHeader.iAscBytes);
}

/**
* @brief 取得中文字字模信息
*/
static void 
GetChnFontData(
			   char* pBuffer, 
			   FONT* pFont,
			   char* pChar
			   )
{
	unsigned char					qh,wh;
	int								iOffset;

	//qu-wei code of chinese
	qh								= *(pChar)-0xa0;
	wh								= *(pChar+1)-0xa0;

	if(qh>=0x10)
	{
		iOffset						= pFont->iOffset + pFont->FontLibHeader.iSize 
			+ pFont->FontLibHeader.iChnOffset;
		iOffset						= iOffset 
			+ ((94*(qh-1)+(wh-1))-0x0582)* pFont->FontLibHeader.iChnBytes;
	}
	else{
		iOffset						= pFont->iOffset + pFont->FontLibHeader.iSize 
			+ pFont->FontLibHeader.iChnSymOffset;
		iOffset						= iOffset 
			+ (94*(qh-1)+(wh-1)) * pFont->FontLibHeader.iChnBytes;
	}

	shm_read(pBuffer, iOffset, pFont->FontLibHeader.iChnBytes);
}
#endif

/**
* @brief 输出一个英文字符
* @note  目前只支持一种字体的显示,且坐标必须为4的倍数
*/
static void gui_PutChar(unsigned short x,unsigned short y,unsigned char c,unsigned int refresh_flag)
{
	int								i;
	unsigned char		*pData;
	TFontInfo			*pInfo;
	unsigned short      lcd_x, lcd_y;

	lcd_x							= x;
	lcd_y							= y;

	if( gFontSize == 12)
	{	// 6x12号的ascii
		// 计算字模地址
		if (c >= 0x20)
		{
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * (c-0x20+3));	//在可见字符之前加了三个特殊字符
		}
		else if (c == 0x09)
		{
			//为了设置TAB键，用一个特殊的可见字符表示TAB键
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * 0);	
		}
		else if (c == 0x0a)
		{
			//为了设置换行键，用一个特殊的可见字符表示换行键
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * 1);	
		}
		else if (c == 0x0d)
		{
			//为了设置回车键，用一个特殊的可见字符表示回车键
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * 2);	
		}
		else
		{
			return;
		}
		
		pInfo			= &FontList[FNT_ASC_6_12];

		for(i=0; i<pInfo->height; i++, lcd_y++)
		{
			Lcd_PutCharLineExt(lcd_x, lcd_y, *pData++, textColor,6,refresh_flag);
		}
	}//12
	else if (gFontSize == 6)
	{
		if ((c >= 0x30)&&(c <= 0x39))
		{
			pData			= (unsigned char *)(num0_9_4x6 + 6 * (c-0x30));
			for(i=0; i<6; i++, lcd_y++)
			{
				Lcd_PutCharLineExt(lcd_x, lcd_y, *pData++, textColor,4,refresh_flag);
			}
		}
	}
}

/**
***************************************************************************
*@brief	输出一个中文字附（泛指非ascii字符）
*@param[in] x 横坐标
*@param[in] y 纵坐标
*@note  目前只支持一种字体的显示,且坐标必须为4的倍数
***************************************************************************
*/
static void gui_PutChn(unsigned short x,unsigned short y,unsigned char *c,unsigned int refresh_flag)
{
	int						i;
	TFontInfo				*pInfo;
	int						offset;
	unsigned char			*pData;
	unsigned char			*pOrg;

	if(gFontSize == 12)
	{

		/*
		GBK码对字库中偏移量的计算公式为：

		GBKindex = ((unsigned char)GBKword[0]-129)*190 +

		((unsigned char)GBKword[1]-64) - (unsigned char)GBKword[1]/128;

		*/
		offset				= ((c[0] - 0xA1) & 0x7F) * 94 + ((c[1]-0xA1)&0x7F);		//GB2312字符集偏移计算公式

		//offset				= (c[0] - 129) * 190 + (c[1]-64) - (c[1]/128);				//GBK字符集偏移计算公式
		pInfo				= &FontList[FNT_CHN_12_12];


		if(pInfo->address == 0)
			return;

		offset				*= pInfo->size;
		//pData				= (unsigned char*)(pInfo->address + offset);	// 指向存在STM32 Flash区域字库对应的该字模的起始地址
		pData				= read_resdata(pInfo->address + (offset/512));	// 计算所在的扇区
		pOrg				= pData;
		pData				+= offset & 0x1FF;								// 计算扇区内的偏移


		//显示字模数据
		for(i=0;i<pInfo->height; i++, y++)
		{
			Lcd_PutCharLine(x, y, *pData++, textColor,refresh_flag);

			Lcd_PutCharLineExt(x+8, y, *pData++, textColor,4,refresh_flag);
			//下一行的字模处于下一个扇区
			if( ((unsigned int)pOrg + 0x1FF) < (unsigned int)pData )
			{
                          y++;
                          i++;
				break;
			}
		}
		if (i < pInfo->height)
		{
			//读取位于下一个Sector的字模数据
			pData				= read_resdata(pInfo->address + (offset/512) + 1);

			//输出剩余的字模
			for(;i<pInfo->height; i++, y++)
			{
				Lcd_PutCharLine(x, y, *pData++, textColor,refresh_flag);
				Lcd_PutCharLineExt(x+8, y, *pData++, textColor,4,refresh_flag);
			}
		}
	}//12

}

/**
* @brief 文字输出
*/
void gui_TextOut(int x, int y, unsigned char *pText, int bk,unsigned int refresh_flag)
{
	unsigned char					*pChar = pText;

        edit_display_x_patch = 0;
	while(*pChar)
	{
		if(( *pChar >= 0x20 && *pChar <= 0x80)||(*pChar == 0x09)||(*pChar == 0x0a)||(*pChar == 0x0d))
		{	// ASCII
			if(x > GUI_WIDTH-6)
			{
				x	= 0;
				if (y > GUI_HEIGHT-12)
				{
					return;
				}
				else
				{
					y	+= 13;
				}
			}

			gui_PutChar(x, y, *pChar,refresh_flag);
			x		+= gFontSize/2;
			pChar++;
		}
		else if(*pChar < 0x20)
		{
			return;
		} 
		else
		{		// 简体中文,中文字库在sd卡里
			if(x > GUI_WIDTH-12)
			{
				if (x == (GUI_WIDTH - 6))
				{
					//编辑框显示时，由于在一行的最后不能显示一个汉字，所以需要在换行的最后一个位置空出一个字符的位置来
					edit_display_x_patch += 6;
					gui_PutChar(x, y, ' ',refresh_flag);
				}
				x	= 0;
				if (y > GUI_HEIGHT-12)
				{
					return;
				}
				else
				{
					y	+= 13;
				}
			}
			gui_PutChn(x, y, pChar,refresh_flag);
			x		+= gFontSize;
			pChar	+= 2;
		}
	}

	if(bk != 2)
	{
		Lcd_DrawLineV(GUI_WIDTH,y,gFontSize,BG_COLOR,1);
		Lcd_DrawLineV(GUI_WIDTH+1,y,gFontSize,BG_COLOR,1);
	}
}

/**
* @brief 文字输出,x坐标指定为以下几种模式
*	0：左对齐
*   1：居中显示
*   2：右对齐
* @note  只能单行显示
*/
void gui_TextOut_ext(unsigned char mode,int y, unsigned char *pText, int bk,unsigned int refresh_flag)
{
	unsigned char					*pChar = pText;
	unsigned int					x  = 0;


	if (mode == LEFT_ALIGN)
	{
		x = 0;
	}
	else
	{
		while(*pChar)
		{
			if(( *pChar >= 0x20 && *pChar <= 0x80)||(*pChar == 0x09)||(*pChar == 0x0a)||(*pChar == 0x0d))
			{	// ASCII
				x += 6;   
				pChar++;
			}
			else
			{	
				x		+= 12;
				pChar	+= 2;
			}
		}
		if (x > GUI_WIDTH)
		{
			gui_TextOutEx2(0,y,pText,bk,refresh_flag);
			return;
		}

		if (mode == CENTER_ALIGN)
		{
			x = (GUI_WIDTH - x)/2;
		}
		else if (mode == RIGHT_ALIGN)
		{
			x = GUI_WIDTH - x;
		}
		else
		{
			return;
		}
	}

	pChar = pText;
	while(*pChar)
	{
		if(( *pChar >= 0x20 && *pChar <= 0x80)||(*pChar == 0x09)||(*pChar == 0x0a)||(*pChar == 0x0d))
		{	// ASCII
			gui_PutChar(x, y, *pChar,refresh_flag);
			x += 6;  
			pChar++;
		}
		else if(*pChar < 0x20)
		{
			return;
		} 
		else
		{		// 简体中文,中文字库在sd卡里
			gui_PutChn(x, y, pChar,refresh_flag);
			x		+= 12;
			pChar	+= 2;
		}
	}
}

/**
* @brief 文字输出,x坐标指定为以下几种模式
*	0：左对齐
*   1：居中显示
*   2：右对齐
* @note  只能单行显示
*/
void gui_TextOutEx2(int x,int y, unsigned char *pText, int bk,unsigned int refresh_flag)
{
	unsigned char					*pChar = pText;

	pChar = pText;
	while(*pChar)
	{
		if(( *pChar >= 0x20 && *pChar <= 0x80)||(*pChar == 0x09)||(*pChar == 0x0a)||(*pChar == 0x0d))
		{	// ASCII
			if (x+6>GUI_WIDTH)
			{
				return;
			}
			gui_PutChar(x, y, *pChar,refresh_flag);
			x += 6;  
			pChar++;
		}
		else if(*pChar < 0x20)
		{
			return;
		} 
		else
		{		// 简体中文,中文字库在sd卡里
			if (x+12>GUI_WIDTH)
			{
				return;
			}
			gui_PutChn(x, y, pChar,refresh_flag);
			x		+= 12;
			pChar	+= 2;
		}
	}
}


/**
* @brief 文字输出,x坐标指定为以下几种模式
*	0：左对齐
*   1：居中显示
*   2：右对齐
* @note  支持自动换行显示
*/
void gui_TextOutEx(unsigned char mode,int y, unsigned char *pText, int bk,unsigned int refresh_flag)
{
	unsigned char					*pChar = pText;
	unsigned int					x  = 0;
	unsigned char					disp_str[21],i = 0;
	unsigned char					line = 0;



	while(*pChar)
	{
		if( *pChar >= 0x20 && *pChar <= 0x80)
		{	// ASCII
			x += 6; 
			if (x > GUI_WIDTH)
			{
				disp_str[i] = 0;
				gui_TextOut_ext(LEFT_ALIGN,y+line*(gFontSize+2),disp_str,bk,refresh_flag);
				line++;
				i = 0;
                                x = 0;
			}
			else
			{
				disp_str[i++] = *pChar;
				pChar++;
			}
		}
		else if ((*pChar == 0x0d)&&(*(pChar+1) == 0x0a))
		{
			disp_str[i] = 0;
			gui_TextOut_ext(mode,y+line*(gFontSize+2),disp_str,bk,refresh_flag);
			line++;
			i = 0;

			pChar += 2;
                        x = 0;
		}
		else
		{	
			x		+= 12;
			if (x > GUI_WIDTH)
			{
				disp_str[i] = 0;
				gui_TextOut_ext(LEFT_ALIGN,y+line*(gFontSize+2),disp_str,bk,refresh_flag);
				line++;
				i = 0;
                                x = 0;
			}
			else
			{
				disp_str[i++] = *pChar;
				disp_str[i++] = *(pChar+1);
				pChar	+= 2;
			}
		}
	}

	disp_str[i] = 0;

	gui_TextOut_ext(mode,y+line*(gFontSize+2),disp_str,bk,refresh_flag);
}

/**
* @brief 画水平线
* @param[in] int x, y 线起始点
* @param[in] int width 线长
*/
void gui_LineH(int x, int y, int width, unsigned short color,unsigned char refresh_flag)
{
	//Lcd_DrawLineH(x, y, width,color);

	int w;
	w = GUI_WIDTH - x;
	if (w >= width)
	{
		Lcd_DrawLineH(x, y, width, color,refresh_flag);
		return ;
	}
	else
	{
		Lcd_DrawLineH(x, y, w, color,refresh_flag);
		w = width - w;		//剩余线长
		while(1)
		{
			y += 13;
			if (w > GUI_WIDTH)
			{
				Lcd_DrawLineH(0, y, GUI_WIDTH, color,refresh_flag);
				w -= GUI_WIDTH;
			}
			else
			{
				Lcd_DrawLineH(0, y, w, color,refresh_flag);
				break;
			}
		}
	}
}

/**
* @brief 画垂直线
* @param[in] int x,y 线起始点
@ @param[in] int hight 线高
*/
void gui_LineV(int x, int y, int hight, unsigned short color)
{
	while(x > GUI_WIDTH)
	{
		y += 13;
		x -= GUI_WIDTH;
	}
	Lcd_DrawLineV(x, y, hight,color,1);
}

/**
* @brief 用前景色画一个矩型
* @param[in] int x,y 左上角坐标
* @param[out] int w,h 宽高
*/
void gui_FillRect(int x, int y, int w, int h, unsigned short color,unsigned int refresh_flag)
{
	while(h--)
		Lcd_DrawLineH(x, y++, w,color,refresh_flag); 
}

void gui_DrawRect(int x, int y, int w, int h, unsigned short color)
{
	Lcd_DrawLineV(x, y, h+1,color,1);
	Lcd_DrawLineV(x+w, y, h+1,color,1);
	Lcd_DrawLineH(x, y, w,color,1);
	Lcd_DrawLineH(x, y+h, w,color,1);
}

/**
* @brief 找到了图片资源，根据ID，得到地址
*/
void gui_setup_picture(unsigned int dwLBA)
{
	dwPictureLBA					= dwLBA;
}

/**
* @brief 将单色的BMP图片输出到lcd上
* @note 为了兼顾效率，此部分直接操作lcd硬件接口
*/
static void gui_PictureStreem(int x, int y, unsigned int dwLBA, unsigned int offset,unsigned int refresh_flag)
{
	unsigned char					*pData, *pOrg;
	unsigned short					w, h;
	int								i, j;
	unsigned short					lcd_x,lcd_y;
	unsigned int					data_offset,remain;
	unsigned char					tmp[26];


	lcd_x							= x;
	lcd_y							= y;

	// pOrg   res_buffer起始地址
	// pData  工作指针
	pOrg							= read_resdata(dwLBA);

	if(pOrg == (unsigned char*)0)
		return;

	pData						= pOrg + offset;
#if 0	
	if ((pData+10) > (pOrg + 0x200))
	{
		remain = pOrg + 0x200 - pData;
		data_offset = pData + 10 - pOrg - 0x200;
		dwLBA++;
		pOrg							= read_resdata(dwLBA);
		pData = pOrg;
		data_offset = *(unsigned short*)(pData+10-remain);

		w							= *(unsigned int*)(pData+18 - remain);
		h							= *(unsigned int*)(pData+22 - remain);

		pData						+= data_offset - remain;
	}
#endif
	if ((pData+26) >= (pOrg + 0x200))
	{
		remain = pOrg + 0x200 - pData;
		memcpy(tmp,pData,remain);
		
		dwLBA++;
		pOrg							= read_resdata(dwLBA);
		pData = pOrg;
		memcpy(tmp+remain,pData,26-remain);

		if ((*tmp != 'B')||(*(tmp+1) != 'M'))
		{
			return;		//有些BMP图片文件的标志字节并不是BM
		}

		data_offset = *(unsigned short*)(tmp+10);

		w							= *(unsigned int*)(tmp+18);
		h							= *(unsigned int*)(tmp+22);

		pData						+= data_offset - remain;
	}

	else
	{
		if ((*pData != 'B')||(*(pData+1) != 'M'))
		{
			return;		//有些BMP图片文件的标志字节并不是BM
		}

		data_offset = *(unsigned short*)(pData+10);	//真正的位图数据的起始地址

		w							= *(unsigned int*)(pData+18);
		h							= *(unsigned int*)(pData+22);

		pData						+= data_offset;
	}

	pOrg						+= 0x200;
	if (pData > pOrg)
	{
		data_offset = pData - pOrg;
		dwLBA ++;
		pOrg							= read_resdata(dwLBA);
		pData = pOrg + data_offset;
		pOrg += 0x200;
	}
	i							= w;
	j							= lcd_x;
lop:
	for(;h>0; h--)
	{
		while(w>0)
		{
			Lcd_PutCharLine(lcd_x, lcd_y, *pData,0xffff,refresh_flag);
			pData				++;
			w -= 8;
			lcd_x += 8;
			if(pData >= pOrg)
			{
				dwLBA++;
				pData			= read_resdata(dwLBA);
				goto lop;
			}
		}
		lcd_y++;
		w						= i;		// 恢复宽度
		if((w/8)%4)
		{
			pData += 4 - ((w/8)%4);		//由于BMP一行数据是4字节对齐，所以需要处理  
		}
		lcd_x						= j;		// 回到x起点
	}
}

/**
* @brief 将单色的BMP图片输出到lcd上
* @note 为了兼顾效率，此部分直接操作lcd硬件接口
*/
static void gui_PictureStreem_ext(int x, int y, unsigned char* pData,unsigned int refresh_flag)
{
	unsigned short					w, h;
	int								i, j;
	unsigned short					lcd_x,lcd_y;
	unsigned int					data_offset;


	lcd_x							= x;
	lcd_y							= y;



	if ((*pData != 'B')||(*(pData+1) != 'M'))
	{
		return;		//有些BMP图片文件的标志字节并不是BM
	}

	data_offset = *(unsigned short*)(pData+10);	//真正的位图数据的起始地址

	w							= *(unsigned int*)(pData+18);
	h							= *(unsigned int*)(pData+22);

	pData						+= data_offset;

	i							= w;
	j							= lcd_x;
	for(;h>0; h--)
	{
		while(w>0)
		{
			Lcd_PutCharLine(lcd_x, lcd_y, *pData,0xffff,refresh_flag);
			pData				++;
			w -= 8;
			lcd_x += 8;
		}
		lcd_y++;
		w						= i;		// 恢复宽度
		if((w/8)%4)
		{
			pData += 4 - ((w/8)%4);		//由于BMP一行数据是4字节对齐，所以需要处理  
		}
		lcd_x						= j;		// 回到x起点
	}
}

/**
* @brief 根据图片ID，在指定位置上显示图片
* @param[in] int x,y 输出的坐标
* @param[in] unsigned int PicID 图片ID
*/
void gui_PictureOut(int x, int y, unsigned int PicID,unsigned int refresh_flag)
{
	unsigned int					*pIDArray;
	unsigned int					pic_address, pic_sector;
	//volatile int	temp;


	if(dwPictureLBA == 0)
	{
		// 图片装载失败
		return;
	}

	// 装入ID数组,每个ID占4字节,所以通过基址dwPictureLBA,加上ID的偏移得到该ID地址
	// 所在的扇区
	if (PicID != PIC_USBLINK)
	{
		pIDArray						= (unsigned int*)read_resdata(dwPictureLBA + (PicID*4)/512);
		if(pIDArray)
		{
			pIDArray					+= PicID % 128;
		}

		// 取得地址
		pic_address						= *pIDArray;
		if (pic_address)
		{
			pic_sector						= dwPictureLBA + (pic_address / 512);	// 数据所在的
			pic_address						&= (512-1);								// 扇区内的偏移值

			gui_PictureStreem(x, y, pic_sector, pic_address,refresh_flag);
		}
	}
	else
	{
		gui_PictureStreem_ext(x, y, (unsigned char*)pic_usblink,refresh_flag);
	}

}

/**
* @brief 将图片流输出到lcd上
* @pPic: 保存图片的地址
* @note 为了兼顾效率，此部分直接操作lcd硬件接口
*/
void gui_PictureArrayOut(int x, int y, unsigned char const *pPic)
{

}

void gui_SetBGColor(unsigned short color)
{
	bgColor			= color;
}

unsigned short gui_GetBGColor(void)
{
	return bgColor;
}


void gui_SetTextColor(unsigned short color)
{
	textColor		= color;
}

unsigned short gui_GetTextColor(void)
{
	return textColor;
}


void gui_SetFont(int font)
{
	if ((font != 12) && (font != 16) && (font != 6))
	{
		gFontSize = 12;
	}
	gFontSize		= font;
}

//unsigned char gui_GetFont(void)
//{
//	return gFontSize;
//}

void gui_CtrlBL(int on)
{
	//Lcd_BackLight(on);
}


/**
***************************************************************************
*@brief	设置系统使用的语言
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
void gui_setlanguage(unsigned char language)
{
	if(language < 3)
		g_language					= language;
	else
		g_language					= 0;

}

/**
***************************************************************************
*@brief	获取系统当前使用的语言类型
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
#if 0
unsigned char gui_getlanguage(void)
{
	return g_language;
}
#endif


void gui_test(void)
{
	int i,j;
	for (i = 0; i < 10;i++)
	{
		for (j = 0; j < 10; j++)
		{
			gui_PutChar(j*8,i*12,0x21+j+(10*i),1);
		}
	}
}


//刷新显示
void gui_refresh(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
	Lcd_refresh(x,y,w,h);
}