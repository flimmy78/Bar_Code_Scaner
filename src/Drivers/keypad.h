#ifndef _KEYPAD_H_
#define _KEYPAD_H_

//#define HW_V1_00			//��һ��ĵ��԰���Ҫ�򿪴˺�
#define HW_V1_01			//V1.01���Ӳ����Ҫ�򿪴˺�



// ���̲��ֲο�keypad.c���ļ�ͷ˵��
#define KEY_LEFT_SHOTCUT		76
#define KEY_ESC					1
#define KEY_ENTER				2
#define KEY_RIGHT_SHOTCUT		3

#define KEY_POWER		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_LEFT		7

#define KEY_RIGHT		8
//����2���ܵļ�ֵ
#define KEY_NUM2		9
#define KEY_A			10
#define KEY_B			11
#define KEY_C			12

#define KEY_a			(0x80+KEY_A)	//128+10 = 138
#define KEY_b			(0x80+KEY_B)	//128+11 = 139
#define KEY_c			(0x80+KEY_C)	//128+12 = 140

//����3���ܵļ�ֵ
#define KEY_NUM3		13
#define KEY_D			14
#define KEY_E			15
#define KEY_F			16

#define KEY_d			(0x80+KEY_D)	
#define KEY_e			(0x80+KEY_E)	
#define KEY_f			(0x80+KEY_F)	


//����4���ܵļ�ֵ
#define KEY_NUM4		17
#define KEY_G			18
#define KEY_H			19
#define KEY_I			20

#define KEY_g			(0x80+KEY_G)	
#define KEY_h			(0x80+KEY_H)	
#define KEY_i			(0x80+KEY_I)	

//����5���ܵļ�ֵ
#define KEY_NUM5		21
#define KEY_J			22
#define KEY_K			23
#define KEY_L			24

#define KEY_j			(0x80+KEY_J)	
#define KEY_k			(0x80+KEY_K)	
#define KEY_l			(0x80+KEY_L)

//����6���ܵļ�ֵ
#define KEY_NUM6		25
#define KEY_M			26
#define KEY_N			27
#define KEY_O			28

#define KEY_m			(0x80+KEY_M)	
#define KEY_n			(0x80+KEY_N)	
#define KEY_o			(0x80+KEY_O)

//����7���ܵļ�ֵ
#define KEY_NUM7		29
#define KEY_P			30
#define KEY_Q			31
#define KEY_R			32
#define KEY_S			33


#define KEY_p			(0x80+KEY_P)	
#define KEY_q			(0x80+KEY_Q)	
#define KEY_r			(0x80+KEY_R)
#define KEY_s			(0x80+KEY_S)	

//����8���ܵļ�ֵ
#define KEY_NUM8		34
#define KEY_T			35
#define KEY_U			36
#define KEY_V			37

#define KEY_t			(0x80+KEY_T)	
#define KEY_u			(0x80+KEY_U)	
#define KEY_v			(0x80+KEY_V)

//����9���ܵļ�ֵ
#define KEY_NUM9		38
#define KEY_W			39
#define KEY_X			40
#define KEY_Y			41
#define KEY_Z			42

#define KEY_w			(0x80+KEY_W)	
#define KEY_x			(0x80+KEY_X)	
#define KEY_y			(0x80+KEY_Y)
#define KEY_z			(0x80+KEY_Z)	//128+42 = 170

//����0���ܵļ�ֵ
#define KEY_NUM0			43
#define KEY_space			44

//����1��Ӧ�ķ��ż�,�������
#define KEY_NUM1			45
#define KEY_douhao			46		//","
#define KEY_juhao			47		//"."
#define KEY_fenhao			48		//;
#define KEY_lianjiehao		49		//_
#define KEY_zhonghuaxian	50		//-
#define KEY_zuo_kuohao		51		//(
#define KEY_you_kuohao		52		//)

#define KEY_gantanhao		53		//!
#define KEY_shuangyinhao	54		//"
#define KEY_meiyuanhao		55		//$
#define KEY_baifenhao		56		//%
#define KEY_yuhao			57		//&
#define KEY_danyinhao		58		//'
#define KEY_jiahao			59		//+
#define KEY_chuhao			60		// /
#define KEY_maohao			61		//:
#define KEY_xiaoyuhao		62		//<
#define KEY_dengyuhao		63		//=
#define KEY_dayuhao			64		//>
#define KEY_wenhao			65		//?
#define KEY_athao			66		//@
#define KEY_zuozhongkuohao  67		//[
#define KEY_xiehuaxianhao	68		/// \;
#define KEY_youzhongkuohao	69		//]
#define KEY_xorhao			70		//^
#define KEY_piehao			71		//`
#define KEY_zuodakuohao		72		//{
#define KEY_huohao			73		//|
#define KEY_youdakuohao		74		//}
#define KEY_qufanhao		75		//~


//ע���ֵ76��KEY_FUN1ռ����



//���ܼ�
#define KEY_SCAN		77
#define KEY_CLEAR		(KEY_SCAN+1)
#define KEY_FUN1		(KEY_CLEAR+1)
#define KEY_FUN2		(KEY_FUN1+1)
#define KEY_FUN3		(KEY_FUN2+1)
#define KEY_FUN4		(KEY_FUN3+1)
#define KEY_xing		(KEY_FUN4+1)		//'*'
#define KEY_jing		(KEY_xing+1)		//'#'


// ��չ�İ���ֵ
#define KEY_EXT_1		(KEY_jing+1)		// 58
#define KEY_EXT_2		(KEY_EXT_1+1)
#define KEY_EXT_3		(KEY_EXT_2+1)
#define KEY_EXT_4		(KEY_EXT_3+1)
#define KEY_EXT_5		(KEY_EXT_4+1)
#define KEY_EXT_6		(KEY_EXT_5+1)
#define KEY_EXT_7		(KEY_EXT_6+1)
#define KEY_EXT_8		(KEY_EXT_7+1)
#define KEY_EXT_9		(KEY_EXT_8+1)
#define KEY_EXT_10		(KEY_EXT_9+1)

// PN532���¼�
#define KEY_PN532_S1	(KEY_EXT_9+1)		// 66 PN532��⵽��
#define KEY_PN532_S2	(KEY_PN532_S1+1)	// 
#define KEY_PN532_S3	(KEY_PN532_S2+1)
#define KEY_PN532_S4	(KEY_PN532_S3+1)
#define KEY_PN532_S5	(KEY_PN532_S4+1)
#define KEY_PN532_S6	(KEY_PN532_S5+1)	// �û�������
#define KEY_PN532_S7	(KEY_PN532_S6+1)	// �û�ȡ��
#define KEY_PN532_S8	(KEY_PN532_S7+1)	// ���

// GPRS���¼�
#define KEY_GPRS_S1		(KEY_PN532_S8+1)	// 74 GPRSģ��û����Ӧ
#define KEY_GPRS_S2		(KEY_GPRS_S1+1)         //GPRS ģ��û��SIM��
#define KEY_GPRS_S3		(KEY_GPRS_S2+1)         //GPRSģ��û���������
#define KEY_GPRS_S4		(KEY_GPRS_S3+1)         //δ֪����
#define KEY_GPRS_S5		(KEY_GPRS_S4+1)         //���׳ɹ�
#define KEY_GPRS_S6		(KEY_GPRS_S5+1)         //����ʧ��
#define KEY_GPRS_S7		(KEY_GPRS_S6+1)         //���ӷ�����ʧ��
#define KEY_GPRS_S8		(KEY_GPRS_S7+1)         //��������ʧ��
#define KEY_GPRS_S9		(KEY_GPRS_S8+1)         //��������ʧ�ܣ�������û����Ӧ
#define KEY_GPRS_S10    (KEY_GPRS_S9+1)			 //GPRSģ�鲦��ʧ��
#define KEY_GPRS_S11	(KEY_GPRS_S10+1)         //GPRSģ�鲦��ʧ��
#define KEY_GPRS_S12	(KEY_GPRS_S11+1)         //���յ������ݲ��Ϸ�

#define KEY_LOW_POWER	(KEY_GPRS_S12+1)		//86

#if 0
// ��ӡ�����¼�
#define KEY_PRINTER_S1		(KEY_24G_S8+1)			//93 //��ӡ��û����Ӧ
#define KEY_PRINTER_S2		(KEY_PRINTER_S1+1)		//��ֽ
#define KEY_PRINTER_S3		(KEY_PRINTER_S2+1)		//ֽ������
#define KEY_PRINTER_S4		(KEY_PRINTER_S3+1)		//ֽ����
#define KEY_PRINTER_S5		(KEY_PRINTER_S4+1)		//��������
#define KEY_PRINTER_S6		(KEY_PRINTER_S5+1)		//94  //����Ĵ�ӡ����
#endif

#define KEY_CREATE		0xFF		// �Ի��򴴽���Ϣ
#define KEY_DESTORY		0xFE		// �Ի���������Ϣ
#define KEY_DRAW		0xFD		// �����ػ���Ϣ


//���ݾ���Ӧ�õİ������ֶ������ҿ�ݼ�
#define	 KEY_SHOTCUT_A		KEY_ALPHA		//���ݼ�
#define  KEY_SHOTCUT_B		KEY_ESC			//�ҿ�ݼ�


#define	TRIP_PIN  GPIO_Pin_0		//PC.0
#define POWER_ON  GPIO_Pin_1		//PA.1
#define START     GPIO_Pin_3		//PC.3
	
#define  BEEP_DELAY			200

#define POWER_KEY_PRESSED   (GPIO_ReadInputDataBit(GPIOC, START))

#define Beep_CS  GPIO_Pin_3		//PD.3
		


//��ֵ��ascii��Ӧ�ṹ��
typedef struct {
	unsigned char keyvalue;
	unsigned char* asciivalue;
} TkeyValue_Ascii;


void keypad_init(void);
unsigned char *keypad_getkey(void);
int keypad_isexit(int key);
int keypad_isnum(int key);
void keypad_release(void);
int key_readcol(void);

unsigned char *key2ascii(unsigned char key);

//void led_r_ctrl(int on);
void led_g_ctrl(int on);
void PowerOn(void);
void PowerOff(void);
void Beep(int cnt);
void Keypad_Int_Enable(void);

void trip(int cnt);
#endif
