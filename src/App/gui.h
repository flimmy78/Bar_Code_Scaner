#ifndef _GUI_H__
#define _GUI_H__

#include "lcd.h"


typedef struct {
	unsigned char					type;				// 见下面说明
	unsigned char					width;				// 字宽
	unsigned char					height;				// 字高
	unsigned char					size;				// 一个字模占的字节数
	unsigned int					address;			// 如果是ROM字库，则为内存指针
	// 如果是资源字库，则为起始扇区
}TFontInfo;


// 颜色定义
#define TXT_COLOR		0xffff
#define BG_COLOR		0

#define COLOR_BLACK           TXT_COLOR
#define COLOR_WHITE           BG_COLOR


#define GUI_WIDTH	(LCD_WIDTH-2)
#define GUI_HEIGHT	LCD_HEIGHT

#define CLIENT_HEIGHT	GUI_HEIGHT

#define SHOTCUT_HEIGNT	12			//快捷栏的高度

#define USERZONE_HEIGHT		(GUI_HEIGHT - ICON_HEIGHT - SHOTCUT_HEIGNT)	//除去ICON栏、标题栏、下面快捷菜单栏之外，剩余的用户显示区域高度

#define DEFAULT_FONT_SIZE	12
#define ICON_HEIGHT			12
#define TITLE_HEIGHT            0

#define LANG_ENGLISH			0		// English
#define LANG_SCHINESE			1		// Simplified Chinese
#define LANG_TCHINESE			2		// Traditional Chinese


#define RIGHT_ALIGN			0
#define CENTER_ALIGN		1
#define LEFT_ALIGN			2



void gui_init(void);
void gui_clear(unsigned short color);

void gui_SetBGColor(unsigned short color);
void gui_SetTextColor(unsigned short color);

unsigned short gui_GetBGColor(void);
unsigned short gui_GetTextColor(void);

void gui_LineH(int x, int y, int width, unsigned short color,unsigned char refresh_flag);
void gui_LineV(int x, int y, int hight, unsigned short color);
void gui_FillRect(int x, int y, int w, int h, unsigned short color,unsigned int refresh_flag);
void gui_DrawRect(int x, int y, int w, int h, unsigned short color);

int gui_setup_font(unsigned int dwLBA, unsigned char id);
unsigned char gui_GetFont(void);
void gui_SetFont(int font);
void gui_setlanguage(unsigned char language);
//unsigned char gui_getlanguage(void);

void gui_TextOut(int x, int y, unsigned char *pText, int bk,unsigned int refresh_flag);
void gui_TextOut_ext(unsigned char mode,int y, unsigned char *pText, int bk,unsigned int refresh_flag);
void gui_TextOutEx(unsigned char mode,int y, unsigned char *pText, int bk,unsigned int refresh_flag);
void gui_TextOutEx2(int x,int y, unsigned char *pText, int bk,unsigned int refresh_flag);
void gui_CtrlBL(int on);

void gui_setup_picture(unsigned int dwLBA);
void gui_PictureOut(int x, int y, unsigned int PicID,unsigned int refresh_flag);
void gui_PictureArrayOut(int x, int y, unsigned char const *pPic);
void gui_CodePicture(int x, int y, unsigned char *pic);
void gui_refresh(unsigned int x,unsigned int y,unsigned int w,unsigned int h);
void gui_test(void);
#endif
