/**
 * @file lcd.h
 * @brief LCD驱动模块的头文件 
 * @version 1.0
 * @author joe
 * @date 2013年10月25日
 * @note
*/
#ifndef _LCD_H_
#define _LCD_H_

#define LCD_WIDTH		128
#define LCD_HEIGHT		64

//#define LCD_SUPPORT_WIN		0x0001

#define Horizontal     0x00
#define Vertical       0x01

void Lcd_init(void);
void Lcd_Clear(unsigned short Color);
void Lcd_SetCursor(unsigned int x,unsigned int y);
inline void Lcd_SetPoint(unsigned int x,unsigned int y,unsigned short color,unsigned int refresh_flag);

void Lcd_PutCharLine(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor,unsigned int refresh_flag);
void Lcd_PutCharLineExt(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor, unsigned int len,unsigned int refresh_flag);
void Lcd_PutCharLineBK(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor, unsigned short bkColor,unsigned int refresh_flag);
void Lcd_PutCharLineExtBK(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor ,unsigned short bkColor, unsigned int len,unsigned int refresh_flag);

void Lcd_DrawLineV(unsigned int x, unsigned int y, unsigned int h,unsigned short color,unsigned int refresh_flag);
void Lcd_DrawLineH(unsigned int x, unsigned int y, unsigned int w, unsigned short c,unsigned int refresh_flag);
void Lcd_DrawPicture(unsigned int StartX,unsigned int StartY,unsigned int EndX,unsigned int EndY,unsigned short *pic,unsigned int refresh_flag);

unsigned int Lcd_CheckSupport(void);
void Lcd_refresh(unsigned int x, unsigned int y,unsigned int w, unsigned int h);
void Lcd_test(void);

void displaycom();
void displaycom1();
void displayseg();
void displayseg1();
void displaysnow();
#endif
