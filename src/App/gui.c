/**
* @file gui.c
* @brief ˢ������Ŀ��UI����ģ��
*
* @version V0.0.1
* @author zhongyh
* @date 2009��8��28��
* @note
*      �ṩ�ǽ�ˢ��������ӿ�
*      320x240
*      ǰ������ɫѡ����GUIģ�����к������˲������л���
*      
*      
* @note �������˵����
*      
*      ��ģ�����ֲ���Ϊ16,24,32�����ֿ⣬��ѡ������󣬵����������������Ϊ��ѡ���塣
*      �������ʱ�򵥼�������Ŀ�ȣ����ಿ�ֻ��������ֱ���������ַ���ֹΪֹ��
*      �ֿ�����Data SDCard��
*     ����32������16*32��ASCII�����32*32����������
*     ������ʾ��Ϊ������ʾ
*
*
* @copy
*
* �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
* ����˾�������Ŀ����˾����һ��׷��Ȩ����
*
* <h1><center>&copy; COPYRIGHT 2009 netcom</center></h1>
*/


#include "gui.h"
#include "lcd.h"
#include "res_spi.h"
#include "AsciiLib_6x12.h"
#include "cpu.h"
#include "pic.h"

// ���� TFontInfo ��type˵��
// BIT 7 6 5 4 3 2 1 0
//     | | | |       +----  0 ROM�ֿ� 1��Դ���ֿ�
//     +-+-+-+------------  0 ��׼�ֿ� 1:FontMaker�ֿ� ������δ����
//



static unsigned short				bgColor;
static unsigned short				textColor;

unsigned char						gFontSize;			// �ֺ�
unsigned char				g_language;			// ��������

extern unsigned short			edit_display_x_patch;

#define MAX_FNT_CNT				FNT_CHN_12_12


/**
*	@brief  ˢ������Ŀ���õ���������Ϣ��
*/
TFontInfo FontList[MAX_FNT_CNT+1] = {
	{	0, 6,  12, 12, 0	},							// ascii 8x12
	{	0, 12, 12, 24, 0	},							// chn 12x12
};

unsigned int						dwPictureLBA;


/**
* @brief ��ʼ����ʾģ��
*/
void gui_init(void)
{
	Lcd_init();											// ��ʼ��LCDӲ��
	//Lcd_init();											// ��ʼ��LCDӲ��
	Lcd_Clear(BG_COLOR);

	FontList[FNT_ASC_6_12].address	= (unsigned int)AsciiLib_6x12;
	FontList[FNT_CHN_12_12].address	= 0;

	dwPictureLBA					= 0;

	gui_SetFont(12);
	gui_SetTextColor(TXT_COLOR);
}

/**
* @brief װ���ֿ���Ϣ
* @note ��SD�����ҵ���Ӧ���ֿ�
*       ÿ���ֿ���һ��������BPT
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
		{	// ��sd��װ��
		}
	}
}
#endif

/**
* @brief ��װ�����ֿ�
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
* @brief ȡ��ASCII�ֿ���Ϣ
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
* @brief ȡ����������ģ��Ϣ
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
* @brief ���һ��Ӣ���ַ�
* @note  Ŀǰֻ֧��һ���������ʾ,���������Ϊ4�ı���
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
	{	// 6x12�ŵ�ascii
		// ������ģ��ַ
		if (c >= 0x20)
		{
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * (c-0x20+3));	//�ڿɼ��ַ�֮ǰ�������������ַ�
		}
		else if (c == 0x09)
		{
			//Ϊ������TAB������һ������Ŀɼ��ַ���ʾTAB��
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * 0);	
		}
		else if (c == 0x0a)
		{
			//Ϊ�����û��м�����һ������Ŀɼ��ַ���ʾ���м�
			pData			= (unsigned char *)(FontList[FNT_ASC_6_12].address + FontList[FNT_ASC_6_12].size * 1);	
		}
		else if (c == 0x0d)
		{
			//Ϊ�����ûس�������һ������Ŀɼ��ַ���ʾ�س���
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
*@brief	���һ�������ָ�����ָ��ascii�ַ���
*@param[in] x ������
*@param[in] y ������
*@note  Ŀǰֻ֧��һ���������ʾ,���������Ϊ4�ı���
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
		GBK����ֿ���ƫ�����ļ��㹫ʽΪ��

		GBKindex = ((unsigned char)GBKword[0]-129)*190 +

		((unsigned char)GBKword[1]-64) - (unsigned char)GBKword[1]/128;

		*/
		offset				= ((c[0] - 0xA1) & 0x7F) * 94 + ((c[1]-0xA1)&0x7F);		//GB2312�ַ���ƫ�Ƽ��㹫ʽ

		//offset				= (c[0] - 129) * 190 + (c[1]-64) - (c[1]/128);				//GBK�ַ���ƫ�Ƽ��㹫ʽ
		pInfo				= &FontList[FNT_CHN_12_12];


		if(pInfo->address == 0)
			return;

		offset				*= pInfo->size;
		//pData				= (unsigned char*)(pInfo->address + offset);	// ָ�����STM32 Flash�����ֿ��Ӧ�ĸ���ģ����ʼ��ַ
		pData				= read_resdata(pInfo->address + (offset/512));	// �������ڵ�����
		pOrg				= pData;
		pData				+= offset & 0x1FF;								// ���������ڵ�ƫ��


		//��ʾ��ģ����
		for(i=0;i<pInfo->height; i++, y++)
		{
			Lcd_PutCharLine(x, y, *pData++, textColor,refresh_flag);

			Lcd_PutCharLineExt(x+8, y, *pData++, textColor,4,refresh_flag);
			//��һ�е���ģ������һ������
			if( ((unsigned int)pOrg + 0x1FF) < (unsigned int)pData )
			{
                          y++;
                          i++;
				break;
			}
		}
		if (i < pInfo->height)
		{
			//��ȡλ����һ��Sector����ģ����
			pData				= read_resdata(pInfo->address + (offset/512) + 1);

			//���ʣ�����ģ
			for(;i<pInfo->height; i++, y++)
			{
				Lcd_PutCharLine(x, y, *pData++, textColor,refresh_flag);
				Lcd_PutCharLineExt(x+8, y, *pData++, textColor,4,refresh_flag);
			}
		}
	}//12

}

/**
* @brief �������
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
		{		// ��������,�����ֿ���sd����
			if(x > GUI_WIDTH-12)
			{
				if (x == (GUI_WIDTH - 6))
				{
					//�༭����ʾʱ��������һ�е��������ʾһ�����֣�������Ҫ�ڻ��е����һ��λ�ÿճ�һ���ַ���λ����
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
* @brief �������,x����ָ��Ϊ���¼���ģʽ
*	0�������
*   1��������ʾ
*   2���Ҷ���
* @note  ֻ�ܵ�����ʾ
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
		{		// ��������,�����ֿ���sd����
			gui_PutChn(x, y, pChar,refresh_flag);
			x		+= 12;
			pChar	+= 2;
		}
	}
}

/**
* @brief �������,x����ָ��Ϊ���¼���ģʽ
*	0�������
*   1��������ʾ
*   2���Ҷ���
* @note  ֻ�ܵ�����ʾ
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
		{		// ��������,�����ֿ���sd����
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
* @brief �������,x����ָ��Ϊ���¼���ģʽ
*	0�������
*   1��������ʾ
*   2���Ҷ���
* @note  ֧���Զ�������ʾ
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
* @brief ��ˮƽ��
* @param[in] int x, y ����ʼ��
* @param[in] int width �߳�
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
		w = width - w;		//ʣ���߳�
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
* @brief ����ֱ��
* @param[in] int x,y ����ʼ��
@ @param[in] int hight �߸�
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
* @brief ��ǰ��ɫ��һ������
* @param[in] int x,y ���Ͻ�����
* @param[out] int w,h ���
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
* @brief �ҵ���ͼƬ��Դ������ID���õ���ַ
*/
void gui_setup_picture(unsigned int dwLBA)
{
	dwPictureLBA					= dwLBA;
}

/**
* @brief ����ɫ��BMPͼƬ�����lcd��
* @note Ϊ�˼��Ч�ʣ��˲���ֱ�Ӳ���lcdӲ���ӿ�
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

	// pOrg   res_buffer��ʼ��ַ
	// pData  ����ָ��
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
			return;		//��ЩBMPͼƬ�ļ��ı�־�ֽڲ�����BM
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
			return;		//��ЩBMPͼƬ�ļ��ı�־�ֽڲ�����BM
		}

		data_offset = *(unsigned short*)(pData+10);	//������λͼ���ݵ���ʼ��ַ

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
		w						= i;		// �ָ����
		if((w/8)%4)
		{
			pData += 4 - ((w/8)%4);		//����BMPһ��������4�ֽڶ��룬������Ҫ����  
		}
		lcd_x						= j;		// �ص�x���
	}
}

/**
* @brief ����ɫ��BMPͼƬ�����lcd��
* @note Ϊ�˼��Ч�ʣ��˲���ֱ�Ӳ���lcdӲ���ӿ�
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
		return;		//��ЩBMPͼƬ�ļ��ı�־�ֽڲ�����BM
	}

	data_offset = *(unsigned short*)(pData+10);	//������λͼ���ݵ���ʼ��ַ

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
		w						= i;		// �ָ����
		if((w/8)%4)
		{
			pData += 4 - ((w/8)%4);		//����BMPһ��������4�ֽڶ��룬������Ҫ����  
		}
		lcd_x						= j;		// �ص�x���
	}
}

/**
* @brief ����ͼƬID����ָ��λ������ʾͼƬ
* @param[in] int x,y ���������
* @param[in] unsigned int PicID ͼƬID
*/
void gui_PictureOut(int x, int y, unsigned int PicID,unsigned int refresh_flag)
{
	unsigned int					*pIDArray;
	unsigned int					pic_address, pic_sector;
	//volatile int	temp;


	if(dwPictureLBA == 0)
	{
		// ͼƬװ��ʧ��
		return;
	}

	// װ��ID����,ÿ��IDռ4�ֽ�,����ͨ����ַdwPictureLBA,����ID��ƫ�Ƶõ���ID��ַ
	// ���ڵ�����
	if (PicID != PIC_USBLINK)
	{
		pIDArray						= (unsigned int*)read_resdata(dwPictureLBA + (PicID*4)/512);
		if(pIDArray)
		{
			pIDArray					+= PicID % 128;
		}

		// ȡ�õ�ַ
		pic_address						= *pIDArray;
		if (pic_address)
		{
			pic_sector						= dwPictureLBA + (pic_address / 512);	// �������ڵ�
			pic_address						&= (512-1);								// �����ڵ�ƫ��ֵ

			gui_PictureStreem(x, y, pic_sector, pic_address,refresh_flag);
		}
	}
	else
	{
		gui_PictureStreem_ext(x, y, (unsigned char*)pic_usblink,refresh_flag);
	}

}

/**
* @brief ��ͼƬ�������lcd��
* @pPic: ����ͼƬ�ĵ�ַ
* @note Ϊ�˼��Ч�ʣ��˲���ֱ�Ӳ���lcdӲ���ӿ�
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
*@brief	����ϵͳʹ�õ�����
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
*@brief	��ȡϵͳ��ǰʹ�õ���������
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


//ˢ����ʾ
void gui_refresh(unsigned int x,unsigned int y,unsigned int w,unsigned int h)
{
	Lcd_refresh(x,y,w,h);
}