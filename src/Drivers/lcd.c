/**
 * @file lcd.c
 * @brief LCD����ģ�� 
 * @version 1.0
 * @author joe
 * @date 2013��10��23��
 * @note
 *   OLED������оƬ��SSD1306
 * 
 */

#include "stm32f10x_lib.h"
#include "lcd.h"

//#define WINDOW_SUPORT
///Ӳ����ص��Ӻ���
/**
*@def ������LCDģ���Ӳ����Դ�����ü�IO�����ĺ궨��
*/
#define LCD_RD  						GPIO_Pin_8				///PD.8
#define LCD_WR   						GPIO_Pin_9				///PD.9
#define LCD_RS  						GPIO_Pin_4				///PA.4
#define LCD_RST  						GPIO_Pin_0				///PB.0
#define LCD_CS  						GPIO_Pin_1				///PB.1
//#define LCD_BACKLIGHT 					GPIO_Pin_4				///PE.4

#define Set_LCD_RST			(*((volatile unsigned long *) 0x40010C10) = LCD_RST)
#define Clr_LCD_RST			(*((volatile unsigned long *) 0x40010C14) = LCD_RST)

#define Set_LCD_CS			(*((volatile unsigned long *) 0x40010C10) = LCD_CS)
#define Clr_LCD_CS			(*((volatile unsigned long *) 0x40010C14) = LCD_CS)

#define Set_LCD_WR			(*((volatile unsigned long *) 0x40011410) = LCD_WR)
#define Clr_LCD_WR			(*((volatile unsigned long *) 0x40011414) = LCD_WR)

#define Set_LCD_RD			(*((volatile unsigned long *) 0x40011410) = LCD_RD)
#define Clr_LCD_RD			(*((volatile unsigned long *) 0x40011414) = LCD_RD)

#define Set_LCD_RS			(*((volatile unsigned long *) 0x40010810) = LCD_RS)
#define Clr_LCD_RS			(*((volatile unsigned long *) 0x40010814) = LCD_RS)

//#define Lcd_Light_ON		GPIO_SetBits(GPIOE, LCD_BACKLIGHT)
//#define Lcd_Light_OFF		GPIO_ResetBits(GPIOE, LCD_BACKLIGHT)

//128*64 bit
unsigned char Lcd_RAM[8][128];		//��ʾ���棬ÿ��Bit��Ӧһ���㣬��ɫ��ʾ��1����ʾ�ĵ���ʾ��ɫ�� 0����ʾ�õ���ʾ��ɫ��
static unsigned char Lcd_RAM_x;
static unsigned char Lcd_RAM_y;


static void Lcd_WriteCommand(unsigned char cmd);
static void Lcd_WriteData(unsigned char data);
static void Lcd_Delay(unsigned short nCount);
static void Lcd_Init_Port(void);
static void Lcd_Reset(void);
static void Lcd_Delayms(vu16 nCount);

/**
***************************************************************************
*@brief	��ȡlcd֧�ֵĽӿ�
*@param[in] 
*@return LCD_SUPPORT_WIN : ֧��window���
*@warning
*@see	
*@note 
***************************************************************************
*/
unsigned int Lcd_CheckSupport(void)
{
	unsigned int					value = 0;
#ifdef WINDOW_SUPORT
	value							|= LCD_SUPPORT_WIN;
#endif

	return value;
}

/**
* @brief   ��ʱ
* @author joe
* @param[in] vu16 nCount   ��ʱֵ
* @return none
* @note  
*      ���÷�����Lcd_Delay(100);
*/
static void Lcd_Delayms(vu16 nCount)
{
	u32  i;
	for (i = 0; i < nCount*6000; i++) {
		;
	}
}

/**
 * @brief ��ʼ������LCDģ���Ӳ����Դ
 * @author joe
 * @param[in] none
 * @param[out] none
 * @return none
 * @note 
 *	**************************************************
 */
static void Lcd_Init_Port(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* EnableGPIOA, GPIOB, GPIOD,GPIOE*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
        
	//LCD_D0 -- LCD_D7 (PE.7 -- PE.14)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
		GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	//LCD_RD(PD.8),LCD_WR(PD.9)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	Set_LCD_RD;

	//LCD_RST(PB.0),LCD_CS(PB.1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//LCD_RS(PA.4)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
 * @brief   ��ʱ
 * @author joe
 * @param[in] unsigned short nCount   ��ʱֵ
 * @return none
 * @note  
 *      ���÷�����Lcd_Delay(100);
*/
static void Lcd_Delay(unsigned short nCount)
{
    unsigned int  i;
    for (i = 0; i < nCount*10; i++)
	{
        ;
    }
}


/**
 * @brief     ��λ ili9331 ������
 * @author joe
 * @param[in] none
 * @return none
 * @note  
 *      ���÷�����Lcd_Reset()
*/
static void Lcd_Reset(void)
{
	/***************************************
	**                                   **
	**  -------\______________/-------   **
	**         | <---Tres---> |          **
	**                                   **
	**  Tres: Min.10ms                    **
	***************************************/

	Set_LCD_RST;;
	Lcd_Delayms(1);
	Clr_LCD_RST;
	Lcd_Delayms(20);
	Set_LCD_RST;
	Lcd_Delayms(1);
	Set_LCD_RD;
}


/**
* @brief     ��LCD��80�ӿ����ߵ���������д������
* @author joe
* @return none
* @note  
*/
static inline void Lcd_WritePort(unsigned char data)
{	
		unsigned int				pd;
		pd = data;
		pd <<= 7;
		pd |= GPIOE->ODR & 0x807F;
		GPIOE->ODR = pd;
		//Lcd_Delay(10);
}


/**
 * @brief     д��������
 * @author joe
 * @param[in] unsigned char cmd    �Ĵ�����ַ
 * @return none
 * @note  ���ô˺���ʱҪ��֤CS�Ǳ����͵�״̬
 *  ************************************************************************
 *  **                                                                    **
 *  ** nCS       ----\__________________________________________/-------  **
 *  ** RS        ------\____________/-----------------------------------  **
 *  ** nRD       -------------------------------------------------------  **
 *  ** nWR       --------\_______/--------------------------------------  **
 *  ** DB[0:7]  -----------[CMD]-----------------------------------------  **
 *  **                                                                    **
 *  ************************************************************************
*/
static inline void Lcd_WriteCommand(unsigned char cmd)
{
	Clr_LCD_RS;
	Clr_LCD_WR;	
	//Set_LCD_RD;
	Lcd_WritePort(cmd);
	Set_LCD_WR;
	Set_LCD_RS; 
}


/**
 * @brief     дLCD��������GDRAMд����ʾ����
 * @author joe
 * @param[in] unsigned char data     ��ʾ���� 
 * @return none
 * @note  ���ô˺���ʱҪ��֤CS�Ǳ����͵�״̬
 */
static inline void Lcd_WriteData(unsigned char data)
{	
	Set_LCD_RS;
	Clr_LCD_WR;	
	//Set_LCD_RD;
	Lcd_WritePort(data);
	Set_LCD_WR; 
}


/**
 * @brief ����ʼ�� LCD ������(ili9331������)
 * @author joe
 * @param[in] none
 * @param[out] none
 * @return none
 * @note 
 *     ���÷�����Lcd_Initializtion()
*/
void Lcd_init(void)
{
	//unsigned int		i;

	//��ʼ��Ӳ���ӿ�
	Lcd_Init_Port();

	//��λ����IC
	Lcd_Delayms(50);
	Lcd_Reset();                                        // ��λ
	Lcd_Delayms(50);

	Clr_LCD_CS;

	//��������IC�ĳ�ʼ������״̬��Initial Code
	Lcd_WriteCommand(0xae);		//Trun off OLED panel
	Lcd_WriteCommand(0x00);		//Set Lower Column Start Address
	Lcd_WriteCommand(0x10);		//Set Higher Column Start Address	==>Column Start Address = 0
	Lcd_WriteCommand(0x40);		//Set Display Start line address
	Lcd_WriteCommand(0x81);		//Set contrast control register
	Lcd_WriteCommand(0xcf);
	Lcd_WriteCommand(0xa0);		
	Lcd_WriteCommand(0xa4);		//Disable entire display on
	Lcd_WriteCommand(0x20);		//Set Memory Addressing Mode
	Lcd_WriteCommand(00);		//Horizontal mode
	Lcd_WriteCommand(0xa6);		//Set normal display
	Lcd_WriteCommand(0xa8);		//Set multiplex ratio(1 to 64)
	Lcd_WriteCommand(0x3f);		//1/64 duty
	Lcd_WriteCommand(0xd3);		//set display offset
	Lcd_WriteCommand(0x00);		//not offset
	Lcd_WriteCommand(0xd5);		//set display clock divide ratio/oscillator frequency
	Lcd_WriteCommand(0x80);		//set divide ratio
	Lcd_WriteCommand(0xd9);		//set pre-charge priod
	Lcd_WriteCommand(0xf1);
	Lcd_WriteCommand(0xc0);		//Set normal display
	Lcd_WriteCommand(0xda);		//set com pins hardware configuration
	Lcd_WriteCommand(0x12);
	Lcd_WriteCommand(0xdb);		// set VCOMH
	Lcd_WriteCommand(0x40);
	Lcd_WriteCommand(0x8d);		//set Charge Pump Enable/Disable
	Lcd_WriteCommand(0x14);		// Enable
	Lcd_WriteCommand(0xaf);		//Turn on OLED panel

	Set_LCD_CS;

	Lcd_Delay(1000);
	Lcd_Clear(0xffff);
}



void Lcd_Enter_SleepMode(void)
{
	Clr_LCD_CS;
	Lcd_WriteCommand(0xae);		//Trun off OLED panel
	Set_LCD_CS;
}

void Lcd_Exit_SleepMode(void)
{
	Clr_LCD_CS;
	Lcd_WriteCommand(0xaf);		//Turn on OLED panel
	Set_LCD_CS;
}

/**
* @brief     ����Ļ����ָ������ɫ��������������� 0xffff
* @author joe
* @param[in] unsigned short Color   ������ɫֵ.
* @param[out]  none
* @return  none
* @note  
*     ���÷�����Lcd_Clear(0xffff);
*/
void Lcd_Clear(unsigned short Color)
{
	int i,j;
	unsigned char	data;

	//����ɫֵ����0x8000ʱ�������ɫ
	if (Color > 0x8000)
	{
		data = 0xff;
	}
	else
	{
		data = 0;
	}

	memset(Lcd_RAM,data,128*8);

	//����ΪHorizontal Address Mode
	Clr_LCD_CS;
        Lcd_WriteCommand(0x21);		//Set column Addr
	Lcd_WriteCommand(0);
	Lcd_WriteCommand(0x7f);

	Lcd_WriteCommand(0x22);		//Set page Addr
	Lcd_WriteCommand(0);
	Lcd_WriteCommand(0x07);
        
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			Lcd_WriteData(data);
		}
	}
	Set_LCD_CS;
}

void displaycom()
{
	int i,j;

	//����ΪHorizontal Address Mode
	Clr_LCD_CS;
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			Lcd_WriteData(0xcc);
		}
	}
	Set_LCD_CS;
}
void displaycom1()
{
	int i,j;

	//����ΪHorizontal Address Mode
	Clr_LCD_CS;
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	for(i=0;i<8;i++)
	{
		for(j=0;j<128;j++)
		{
			Lcd_WriteData(0x66);
		}
	}
	Set_LCD_CS;
}

void displayseg()
{
	int i,j;

	//����ΪHorizontal Address Mode
	Clr_LCD_CS;
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	for(i=0;i<8;i++)
	{
		for(j=0;j<64;j++)
		{
			Lcd_WriteData(0xff);
			Lcd_WriteData(0x00);
		}
	}
	Set_LCD_CS;
}
void displayseg1()
{
	int i,j;

	//����ΪHorizontal Address Mode
	Clr_LCD_CS;
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	for(i=0;i<8;i++)
	{
		for(j=0;j<64;j++)
		{
			Lcd_WriteData(0x00);
			Lcd_WriteData(0xff);
		}
	}
	Set_LCD_CS;
}

void displaysnow()
{
	int i,j;

	//����ΪHorizontal Address Mode
	Clr_LCD_CS;
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	for(i=0;i<8;i++)
	{
		for(j=0;j<64;j++)
		{
			Lcd_WriteData(0xcc);
			Lcd_WriteData(0x66);
		}
	}
	Set_LCD_CS;
}

void set_cursor(unsigned int x,unsigned int y)
{
	//���õ�ǰSRAM��ַ
	//note x Ӧ����4�ı��������򽫱���һΪ4�ı���
	if ((x > 127)||(y > 63))
	{
		return;
	}
	Lcd_RAM_x = x;
	Lcd_RAM_y = y/8;

	Lcd_WriteCommand(0x21);		//Set column Addr
	Lcd_WriteCommand(x);
	Lcd_WriteCommand(0x7f);

	Lcd_WriteCommand(0x22);		//Set page Addr
	Lcd_WriteCommand(Lcd_RAM_y);
	Lcd_WriteCommand(0x07);
}

void Lcd_test(void)
{

#if 0
	while(1)
	{
		//display_image1();

		Lcd_Clear(0);

		Lcd_PutCharLine(0,0,0x40,0xffff);
		Lcd_PutCharLine(0,1,0x40,0xffff);
		Lcd_PutCharLine(0,2,0x40,0xffff);
		Lcd_PutCharLine(0,3,0x40,0xffff);
		Lcd_PutCharLine(0,4,0x40,0xffff);
		Lcd_PutCharLine(0,5,0x00,0xffff);
		Lcd_PutCharLine(0,6,0x40,0xffff);
		Lcd_PutCharLine(0,7,0x00,0xffff);


		Lcd_PutCharLine(4,0,0x40,0);
		Lcd_PutCharLine(4,1,0x40,0);
		Lcd_PutCharLine(4,2,0x40,0);
		Lcd_PutCharLine(4,3,0x40,0);
		Lcd_PutCharLine(4,4,0x40,0);
		Lcd_PutCharLine(4,5,0x00,0);
		Lcd_PutCharLine(4,6,0x40,0);
		Lcd_PutCharLine(4,7,0x00,0);

		Lcd_PutCharLine(0,12,0xFF,0xffff);
		Lcd_PutCharLine(8,12,0xE0,0xffff);
		Lcd_PutCharLine(0,13,0x08,0xffff);
		Lcd_PutCharLine(8,13,0x00,0xffff);
		Lcd_PutCharLine(0,14,0x08,0xffff);
		Lcd_PutCharLine(8,14,0x00,0xffff);
		Lcd_PutCharLine(0,15,0x10,0xffff);
		Lcd_PutCharLine(8,15,0x00,0xffff);
		Lcd_PutCharLine(0,16,0x1F,0xffff);
		Lcd_PutCharLine(8,16,0x80,0xffff);
		Lcd_PutCharLine(0,17,0x00,0xffff);
		Lcd_PutCharLine(8,17,0x80,0xffff);
		Lcd_PutCharLine(0,18,0x00,0xffff);
		Lcd_PutCharLine(8,18,0x80,0xffff);
		Lcd_PutCharLine(0,19,0x00,0xffff);
		Lcd_PutCharLine(8,19,0x80,0xffff);
		Lcd_PutCharLine(0,20,0x00,0xffff);
		Lcd_PutCharLine(8,20,0x80,0xffff);
		Lcd_PutCharLine(0,21,0x0C,0xffff);
		Lcd_PutCharLine(8,21,0x80,0xffff);
		Lcd_PutCharLine(0,22,0x03,0xffff);
		Lcd_PutCharLine(8,22,0x00,0xffff);
		Lcd_PutCharLine(0,23,0x00,0xffff);
		Lcd_PutCharLine(8,23,0x00,0xffff);

		Lcd_Delayms(2000);
		Lcd_Clear(0xff);
		Lcd_Delayms(2000);
		Lcd_Clear(0xffff);

		displaycom();
		Lcd_Delayms(2000);

		displaycom1();
		Lcd_Delayms(2000);

		displayseg();
		Lcd_Delayms(2000);

		displayseg1();
		Lcd_Delayms(2000);

		displaysnow();
		Lcd_Delayms(2000);
	}
#endif

	displaycom();
	Lcd_Delayms(2000);

	displaycom1();
	Lcd_Delayms(2000);

	displayseg();
	Lcd_Delayms(2000);

	displayseg1();
	Lcd_Delayms(2000);

	displaysnow();
	Lcd_Delayms(2000);
}



/**
* @brief     ��ָ�����껭��
* @author joe
* @param[in] unsigned short x         ������
* @param[in] unsigned short y         ������
* @param[in] unsigned char  color      ��ɫ   1����ɫ   0����ɫ
* @param[out]  none
* @return  none
* @note  
*     ���÷�����Lcd_SetPoint(10,10,1);
*/
inline void Lcd_SetPoint(unsigned int x,unsigned int y,unsigned short color,unsigned int refresh_flag)
{
	unsigned char offset;
	unsigned char data;

	set_cursor(x,y);
	offset = y%8;		//Ҫ�����ĵ�λ�ڸ��ֽڵĵڼ���bit
	if(color)
	{
		Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x] |= (0x01 << offset);
	}
	else
	{
		Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x] &= ~(0x01 << offset);
	}

	if (refresh_flag)
	{
		data = Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x];

		Lcd_WriteData(data);
	}
}

//ֻ�Ǹı��Դ������ֵ
void Lcd_SetRAMPoint(unsigned int x,unsigned int y,unsigned short color)
{
	unsigned int offset;
	//if (x > 127 || y > 63)
	//{
	//	return;
	//}

	Lcd_RAM_x = x;
	Lcd_RAM_y = y/8;
	offset = y%8;		//Ҫ�����ĵ�λ�ڸ��ֽڵĵڼ���bit
	if(color)
	{
		Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x] |= (0x01 << offset);
	}
	else
	{
		Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x] &= ~(0x01 << offset);
	}
}

unsigned int Lcd_GetRAMPoint(unsigned int x,unsigned int y)
{
	unsigned int offset;

	//if (x > 127 || y > 63)
	//{
	//	return 0;
	//}

	Lcd_RAM_x = x;
	Lcd_RAM_y = y/8;
	offset = y%8;		//Ҫ�����ĵ�λ�ڸ��ֽڵĵڼ���bit
	if(Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x] & (0x01 << offset))
	{
		return 1;
	}
	
	return 0;
}

//���Դ������ˢ�µ�LCD��ĳһ��ʾ����
void Lcd_refresh(unsigned int x, unsigned int y,unsigned int w, unsigned int h)
{
	unsigned int i,j,temph = 0;
	unsigned char *pdata;

	//������ʼ���꼰ˢ����������
	if ((x > 127)||(y > 63)||(w == 0)||(h == 0)||(w>128)||(h>64))
	{
		return;
	}
	Lcd_RAM_x = x;
	Lcd_RAM_y = y/8;

	y %= 8;
	if(y)
	{
		temph++;
		h -= (8 - y);
	}
	h = (h-1)/8;
	h += 1 + temph;

	Clr_LCD_CS;
	Lcd_WriteCommand(0x21);		//Set column Addr
	Lcd_WriteCommand(x);
	Lcd_WriteCommand(0x7f);

	Lcd_WriteCommand(0x22);		//Set page Addr
	Lcd_WriteCommand(Lcd_RAM_y);
	Lcd_WriteCommand(0x07);

	
	Lcd_WriteCommand(0x20);	//Set Memory Addressing Mode
	Lcd_WriteCommand(00);	//Horizontal mode
	pdata = &Lcd_RAM[Lcd_RAM_y][Lcd_RAM_x];
	for(i=0;i<h;i++)
	{
		for(j=0;j<w;j++)
		{
			Lcd_WriteData(*pdata);
			pdata++;
		}
	}
	Set_LCD_CS;

}

/**
* @brief ��lcd���һС����ģ
*/
void Lcd_PutCharLineBK(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor, unsigned short bkColor,unsigned int refresh_flag)
{
#if 0

#endif
}

/**
* @brief ���С��8�����ģ
* @param[in] int len ��ֵС�ڵ���8
*/
void Lcd_PutCharLineExtBK(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor ,unsigned short bkColor, unsigned int len,unsigned int refresh_flag)
{
#if 0

#endif
}

/**
* @brief ��lcd���һС����ģ
*/
void Lcd_PutCharLine(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor,unsigned int refresh_flag) 
{
	int				i;

	Clr_LCD_CS;
	if (charColor)
	{
		for(i=0; i<8; i++)
		{
			if(d & 0x80)
				Lcd_SetPoint(x++, y, 1,refresh_flag);		// �ַ���ɫ
			else
				Lcd_SetPoint(x++, y, 0,refresh_flag);
			d <<= 1;
		}
	}
	else
	{
		for(i=0; i<8; i++)
		{
			if(d & 0x80)
				Lcd_SetPoint(x++, y, 0,refresh_flag);		// �ַ���ɫ
			else
				Lcd_SetPoint(x++, y, 1,refresh_flag);
			d <<= 1;
		}
	}
	Set_LCD_CS;

}

/**
* @brief ���С��8�����ģ
* @param[in] int len ��ֵС�ڵ���8
*/
void Lcd_PutCharLineExt(unsigned int x, unsigned int y, unsigned char d, unsigned short charColor,unsigned int len,unsigned int refresh_flag) 
{
#if 0
	int				i;
	unsigned char	data = 0;
	unsigned char   h_byte;

	set_cursor(x,y);

	h_byte = (d >> 4)&0x0f;

	if (charColor > 0x8000)
	{
		for(i=0; i<4; i++)
		{
			if(h_byte & 0x01)
			{
				data |= 0x03;
			}
			h_byte >>= 1;
			if(i < 3)
			{
				data <<= 2;  
			}
		}
		write_data(data);
	}
	else
	{
		for(i=0; i<4; i++)
		{
			if((h_byte & 0x01) == 0)
			{
				data |= 0x03;
			}
			h_byte >>= 1;
			if(i < 3)
			{
				data <<= 2;  
			}
		}
		write_data(data);
	}
#endif

	Clr_LCD_CS;
	if (charColor)
	{
		while(len--)
		{
			if(d & 0x80)
			{
				Lcd_SetPoint(x++, y, 1,refresh_flag);		// ��ɫ�ĵ�
			}
			else
			{
				Lcd_SetPoint(x++, y, 0,refresh_flag);		// ��ɫ�ĵ�	
			}
			d <<= 1;
		}
	}
	else
	{
		while(len--)
		{
			if(d & 0x80)
			{
				Lcd_SetPoint(x++, y, 0,refresh_flag);		// ��ɫ�ĵ�
			}
			else
			{
				Lcd_SetPoint(x++, y, 1,refresh_flag);		// ��ɫ�ĵ�	
			}
			d <<= 1;
		}
	}
	Set_LCD_CS;
}



/**
* @brief ��ˮƽ��
* @note x:����һΪ4�ı���	w:����һΪ4�ı���
*/
void Lcd_DrawLineH(unsigned int x, unsigned int y, unsigned int w,unsigned short color,unsigned int refresh_flag)
{
	unsigned int width;
	int i;
	width = 128 - x;
	if (width < w)
	{
		w = width;
	}

	Clr_LCD_CS;
	for (i = 0; i < w;i++)
	{
		Lcd_SetPoint(x+i,y,color,refresh_flag);
	}
	Set_LCD_CS;
}

/**
* @brief ����ֱ��
*/
void Lcd_DrawLineV(unsigned int x, unsigned int y, unsigned int h,unsigned short color,unsigned int refresh_flag)
{
	unsigned int height;
	int i;
	height = 128 - y;
	if (height < h)
	{
		h = height;
	}

	Clr_LCD_CS;
	for(i = 0; i < h;i++)
	{
		Lcd_SetPoint(x,y+i,color,refresh_flag);
	}
	Set_LCD_CS;
}

/**
* @brief     ��ָ�����귶Χ��ʾһ��ͼƬ
* @author joe
* @param[in] unsigned short StartX         ����ʼ����
* @param[in] unsigned short StartY         ����ʼ����
* @param[in] unsigned short EndX   ��    ��������
* @param[in] unsigned short EndY         �н�������
* @param[in] unsigned short* pic         ͼƬͷָ��
* @param[out]  none
* @return  none
* @note  
*     ���÷�����Lcd_DrawPicture(0,0,100,100,(unsigned short*)demo);
*/
void Lcd_DrawPicture(unsigned int StartX,unsigned int StartY,unsigned int EndX,unsigned int EndY,unsigned short *pic,unsigned int refresh_flag)
{
#if 0
	int								i;

	Lcd_SetWindows(StartX,StartY,EndX,EndY);
	Lcd_SetCursor(StartX,StartY);

	//Clr_LCD_CS;
	i = EndX * EndY;

	Lcd_WriteIndex(0x0022);
	//	for (i=0; i<(EndX*EndY); i++)
	while(i--)
	{
		Lcd_WriteData(*pic++);
		//Clr_LCD_WR;
		//Lcd_Delay(1);
		//Set_LCD_WR;
	}

	//Set_LCD_CS;
#endif
}
