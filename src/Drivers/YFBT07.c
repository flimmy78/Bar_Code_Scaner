/**
 * @file YFBT07.c
 * @brief ����ģ������� 
 * @version 1.0
 * @author joe
 * @date 2013��10��25��
 * @note
 *   ����ģ��YFBT07��������SPI�ӿڣ�ͨѶЭ���ǻ��ڡ���������ģ��ͨѶЭ��.docx��
 * 
 */

#include "stm32f10x_lib.h"
#include "YFBT07.h"
#include "basic_fun.h"
#include "keypad.h"
#include "os_cpu.h"
#include "gui.h"

//LA 57648

/**
*@def ������������ģ�����ӵ�Ӳ����Դ�����ü�IO�����ĺ궨��
*/

#define YFBT07_SPI_PORT					SPI1					//������ģ�����ӵ���SPI1�ӿ�
#define YFBT07_SPI_PORT_CLK				RCC_APB2Periph_SPI1
#define ENABLE_YFBT07_SPI_PORT_CLK()	RCC_APB2PeriphClockCmd(YFBT07_SPI_PORT_CLK, ENABLE)

#define YFBT07_SPI_SCK		GPIO_Pin_5	//PA5
#define YFBT07_SPI_MISO		GPIO_Pin_6	//PA6
#define YFBT07_SPI_MOSI		GPIO_Pin_7	//PA7

#define YFBT07_SPI_INT		GPIO_Pin_13	//PD13		STM32��Ҫ��YFBT07ģ�����ͨѶʱ����Ҫ���ʹ��źţ�һ�����ʹ��ź�����ģ��ͻ����SPIʱ���źţ����������ݹ���
#define YFBT07_STATUS		GPIO_Pin_14	//PD14		��IOָʾ������ģ��������������״̬��δ���ӵ���������ʱ����͵�ƽ�����ӵ���������ʱ����ߵ�ƽ
#define YFBT07_POWER_ENABLE	GPIO_Pin_9	//PC9		��������ģ���Դ��IO

#define YFBT07_SPI_INT_LOW()       GPIO_ResetBits(GPIOD, YFBT07_SPI_INT)
#define YFBT07_SPI_INT_HIGH()      GPIO_SetBits(GPIOD, YFBT07_SPI_INT)

#define YFBT07_POWER_ON()			GPIO_ResetBits(GPIOC, YFBT07_POWER_ENABLE)
#define YFBT07_POWER_OFF()			GPIO_SetBits(GPIOC, YFBT07_POWER_ENABLE)

#define LOW_LEVEL		0
#define HIGH_LEVEL		1

//#define GET_YFBT07_SPI_SCK_LEVEL()		GPIO_ReadInputDataBit(GPIOA,YFBT07_SPI_SCK)
//#define GET_YFBT07_SPI_MOSI_LEVEL()		GPIO_ReadInputDataBit(GPIOA,YFBT07_SPI_MOSI)

#define GET_YFBT07_SPI_SCK_LEVEL()		(((*(volatile unsigned long *) 0x40010808) & 0x0020) != 0)
#define GET_YFBT07_SPI_MOSI_LEVEL()		(((*(volatile unsigned long *) 0x40010808) & 0x0080) != 0)

//#define SET_YFBT07_SPI_MISO()			GPIO_SetBits(GPIOA,YFBT07_SPI_MISO)
//#define RESET_YFBT07_SPI_MISO()			GPIO_ResetBits(GPIOA,YFBT07_SPI_MISO)

#define SET_YFBT07_SPI_MISO()		(*((volatile unsigned long *) 0x40010810) = YFBT07_SPI_MISO)
#define RESET_YFBT07_SPI_MISO()		(*((volatile unsigned long *) 0x40010814) = YFBT07_SPI_MISO)

//�������ģ���Ƿ��Ѿ���������������������
#define GET_YFBT07_CONNECT_STATE()		GPIO_ReadInputDataBit(GPIOD,YFBT07_STATUS)
/**
* @brief  ��ʼ����������ģ���Ӳ��IO��Դ 
* @param  None
* @retval : 0 �ɹ�
*          -1 ʧ��
*/
static int YFBT07_HW_init(void)
{
	SPI_InitTypeDef		SPI_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;

	/* Enable SPI1 and GPIO clocks */
	//ENABLE_YFBT07_SPI_PORT_CLK();	//ʹ��������ģ��ͨѶ��SPI�ӿڵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	/* Configure SPI1 pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure I/O for SPI_INT */
	GPIO_InitStructure.GPIO_Pin = YFBT07_SPI_INT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	YFBT07_SPI_INT_HIGH();

	/* Configure I/O for YFBT07_POWER_ENABLE */
	GPIO_InitStructure.GPIO_Pin = YFBT07_POWER_ENABLE;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	YFBT07_POWER_OFF();

	/* Configure I/O for SPI_STATUS */
	GPIO_InitStructure.GPIO_Pin = YFBT07_STATUS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);


	/* SPI1 configuration */
	//�ӻ�ģʽ
	//SPI_Cmd(YFBT07_SPI_PORT, DISABLE);
	//SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	//SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
	//SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	//SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	//SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	//SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	//SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		
	//SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	//SPI_InitStructure.SPI_CRCPolynomial = 7;
	//SPI_Init(YFBT07_SPI_PORT, &SPI_InitStructure);

	//SPI_NSSInternalSoftwareConfig(YFBT07_SPI_PORT, SPI_NSSInternalSoft_Reset);
	///* Enable SPI1  */
	//SPI_Cmd(YFBT07_SPI_PORT, ENABLE);
	return 0;										// �ɹ�
}

//����IOģ��SPI�ӿڣ�����һ���ֽڵ�����ģ��
static unsigned char SendByte2YFBT07(unsigned char data)
{
	unsigned char  i,tmp = 0;
	unsigned int	cnt = 0;
	OS_CPU_SR  cpu_sr = 0;

	OS_ENTER_CRITICAL();
	SET_YFBT07_SPI_MISO();
	for(i = 0; i < 8;i++)
	{
		//while(GET_YFBT07_SPI_SCK_LEVEL() == HIGH_LEVEL);
		while(GET_YFBT07_SPI_SCK_LEVEL()&&(cnt<100000))
                {
                  cnt++;
                }
		if (data&(0x80>>i))
		{
			SET_YFBT07_SPI_MISO();
		}
		else
		{
			RESET_YFBT07_SPI_MISO();
		}
		//while(GET_YFBT07_SPI_SCK_LEVEL() == LOW_LEVEL);
		cnt = 0;
                while(!GET_YFBT07_SPI_SCK_LEVEL()&&(cnt<100000))
                {
                  cnt++;
                }
	}
	
	SET_YFBT07_SPI_MISO();

	OS_EXIT_CRITICAL();
        
    return tmp;
}

//�������ݵ�����ģ��
inline static unsigned char SendByteArray2YFBT07(unsigned char *data,unsigned int len)
{
	unsigned char i;
	volatile unsigned char tmp;

	YFBT07_SPI_INT_LOW();
	for(i = 0;i < len;i++)
	{
		//YFBT07_SPI_PORT->DR = data[i];		//���������TXE  TXE = 0��
		//while((YFBT07_SPI_PORT->SR && (1<<1))==0);	 //�ȴ����ͻ������գ������ϴ˴�Ӧ�û���SPI������ʱ���źţ���ʱ���źŵ�ͬ���½����ͻ����������ݷ��ͳ�ȥ�����������λTXE��ʹTXE = 1��
		//tmp = (unsigned char)YFBT07_SPI_PORT->DR;	 //���RXNE��־��ͬʱ���յ��������͹���������

		SendByte2YFBT07(data[i]);
	}
	YFBT07_SPI_INT_HIGH();
}


//����YFBT07ģ���SPIͨѶ�Ƿ�ok��ֻҪ����INT�źţ����Լ�⵽YFBT07ģ�������ʱ���źţ�����Ϊģ�鹤������
int YFBT07_check(void)
{
	int cnt = 0;
#if 1
	RESET_YFBT07_SPI_MISO();
	YFBT07_SPI_INT_LOW();
	//while((GET_YFBT07_SPI_SCK_LEVEL() == HIGH_LEVEL)&&(cnt < 100000))		//���ʱ���źŵĵ���
	while((GET_YFBT07_SPI_SCK_LEVEL())&&(cnt < 100000))		//���ʱ���źŵĵ���
	{
		cnt++;
	}

	YFBT07_SPI_INT_HIGH();
        //SET_YFBT07_SPI_MISO();
        
	if (cnt == 100000)
	{
		return -1;
	}
#endif
	return 0;

}


/**
* @brief   ��ʱ
* @author joe
* @param[in] vu16 nCount   ��ʱֵ
* @return none
* @note  
*      ���÷�����Lcd_Delay(100);
*/
static void YFBT07_Delayms(vu16 nCount)
{
	u32  i;
	for (i = 0; i < nCount*6000; i++) {
		;
	}
}


#define READ_YFBT07_DUMMY_BYTE		0xFF

/**
* @brief  ��ʼ������ģ�� 
* @param  None
* @retval : 0 �ɹ�
*          -1 ʧ��
*/
int YFBT07_init(void)
{
	unsigned char status;

	YFBT07_HW_init();		//��ʼ��Ӳ����Դ
	YFBT07_POWER_ON();		//��������ģ��ĵ�Դ

	YFBT07_Delayms(2000);	//200ms ��ʱһ��ʱ��ȴ�ģ���ϵ���������Ĺ���״̬���˵ȴ�ʱ�������
	
	//��������ģ���Ƿ�����������ʵֻ�ܼ��SPI��ͨ���Ƿ�����
	if (YFBT07_check())
	{
		return -1;
	}
	return 0;
}

//��������ģ��ĵ�Դ
void YFBT07_power_ctrl(unsigned char on_off)
{
	if (on_off)
	{
		YFBT07_POWER_ON();
		YFBT07_SPI_INT_HIGH();
		SET_YFBT07_SPI_MISO();
	}
	else
	{
		YFBT07_POWER_OFF();	
		YFBT07_SPI_INT_LOW();
		RESET_YFBT07_SPI_MISO();
	}
	YFBT07_Delayms(500);
}

/**
* @brief  ʹ����ģ���������״̬ 
* @param  None
* @retval : 0 �ɹ�
*          -1 ʧ��
*/
int YFBT07_Enter_Match_Mode(void)
{
	unsigned char status;
	unsigned int	timeout = 0;
	OS_CPU_SR  cpu_sr = 0;

	OS_ENTER_CRITICAL();
	//����ʹ����ģ���������״̬������
	YFBT07_SPI_INT_LOW();
	SendByte2YFBT07(0x5a);
	SendByte2YFBT07(0xa5);
	YFBT07_SPI_INT_HIGH();
	OS_EXIT_CRITICAL();
	return 0;
}

static unsigned char calc_check_value(unsigned char *buf,unsigned int len)
{
	unsigned char i;
	unsigned char xor = 0;

	for(i = 0;i < len;i++)
	{
		xor ^= buf[i];
	}

	return xor;
}

/**
* @brief  ��������ģ���Ƿ���������������������
* @param  None
* @retval : 0		�Ͽ�״̬
*           1		����״̬
*/
int YFBT07_Connect_state(void)
{
	return (int)GET_YFBT07_CONNECT_STATE();
}

/**
* @brief  ��������ģ�鷢�ͼ�ֵ 
* @param  None
* @retval : 0 �ɹ�
*          -1 ʧ��
*/
int YFBT07_SendKey(unsigned char *key_buffer,unsigned int len)
{
	unsigned char report_buf[11];
	unsigned char send_num = 0;

	report_buf[0] = 0x55;
	while(len)
	{
		send_num = ascii_to_keyreport2_ext(key_buffer,len,report_buf+1);	//�ѹ���ļ�ֵ����
		
		report_buf[10] = calc_check_value(report_buf,10);

        SendByteArray2YFBT07(report_buf,11);

		//�����ͷż�
        //memset(report_buf+1,0,9);
	//	report_buf[10] = 0x55;
      
	//	SendByteArray2YFBT07(report_buf,11);

		YFBT07_Delayms(20);
                
        len -= send_num;
        key_buffer += send_num;
	}
	return 0;
}


//��������ģ��
void YFBT07_Test(void)
{
	int  i,timeout = 0;
	unsigned char *key;
	unsigned char buf[50];
	unsigned char offset = 0;
	unsigned char test_state = 0;

	gui_clear(BG_COLOR);
	gui_TextOut(0,0,"��������",0,1);

	while(1)
	{
		if (YFBT07_Connect_state() == 0)
		{
            gui_FillRect(0,13,GUI_WIDTH,GUI_HEIGHT-13,BG_COLOR,1);            
			gui_TextOut(0,13,"��ƥ��״̬...",0,1);

			//�������ģ�黹û��������ע�⽨��������
			//��������ʹģ��������״̬���ȴ�����������������
            //�����ȵȴ�5S��ʲô�����������5S֮���Ծ�û�����ӵ��������ٷ����������
			while(YFBT07_Connect_state() == 0)
			{
				if (timeout < 30000)
				{
					timeout++;
				}

				if (timeout == 30000)
				{
					timeout = 0;
					break;	//��ʱʱ���ڻ�û���������������������ӣ��˳���ѭ�������·����������
				}

				YFBT07_Delayms(1);
			}

			YFBT07_Enter_Match_Mode();
		}
		else
		{
			if (test_state == 0)
			{
				gui_TextOut(0,13,"������״̬...",0,1);
				gui_TextOut(0,26,"��[SCAN]�����ͼ�ֵ",0,1);
				//gui_TextOut(0,39,"�˴η��ͼ�ֵ:",0,1);
				key = keypad_getkey();
				if (*key == KEY_SCAN)
				{
					Beep(BEEP_DELAY);
					for (i = 0;i < 10;i++)
					{
						buf[i] = 0x3a+i;
					}
					buf[10] = 0;

					//gui_TextOut(0,52,"                    ",1,1);
					//gui_TextOut(0,52,buf,0,1);
					YFBT07_SendKey(buf,10);
					YFBT07_SendKey("\x0d",1);
					test_state = 1;
					gui_FillRect(0,13,GUI_WIDTH,GUI_HEIGHT-13,BG_COLOR,1);
				}
			}

			if (test_state == 1)
			{
				gui_TextOut(0,26,"����ģ�����OK",0,1);
				gui_TextOut(0,39,"��[SCAN]����������",0,1);
				if (*key == KEY_SCAN)
				{
					Beep(BEEP_DELAY);
					return;
				}
			}
			
			/*else if (*key == KEY_DOWN)
			{
				for (i = 0; i < 47; i++)
				{
					buf[i] = 0x20+i;
				}
				YFBT07_SendKey(buf,47);
			}
			else if (*key == KEY_UP)
			{
				for (i = 0; i < 48; i++)
				{
					buf[i] = 0x20+47+i;
				}
				YFBT07_SendKey(buf,48);
			}*/
		}
		YFBT07_Delayms(100);
	}
	
}