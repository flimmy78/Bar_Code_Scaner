#ifndef _KEYPAD_H_
#define _KEYPAD_H_

//#define HW_V1_00			//第一版的调试板需要打开此宏
#define HW_V1_01			//V1.01板的硬件需要打开此宏



// 键盘布局参看keypad.c的文件头说明
#define KEY_LEFT_SHOTCUT		76
#define KEY_ESC					1
#define KEY_ENTER				2
#define KEY_RIGHT_SHOTCUT		3

#define KEY_POWER		4
#define KEY_UP			5
#define KEY_DOWN		6
#define KEY_LEFT		7

#define KEY_RIGHT		8
//按键2可能的键值
#define KEY_NUM2		9
#define KEY_A			10
#define KEY_B			11
#define KEY_C			12

#define KEY_a			(0x80+KEY_A)	//128+10 = 138
#define KEY_b			(0x80+KEY_B)	//128+11 = 139
#define KEY_c			(0x80+KEY_C)	//128+12 = 140

//按键3可能的键值
#define KEY_NUM3		13
#define KEY_D			14
#define KEY_E			15
#define KEY_F			16

#define KEY_d			(0x80+KEY_D)	
#define KEY_e			(0x80+KEY_E)	
#define KEY_f			(0x80+KEY_F)	


//按键4可能的键值
#define KEY_NUM4		17
#define KEY_G			18
#define KEY_H			19
#define KEY_I			20

#define KEY_g			(0x80+KEY_G)	
#define KEY_h			(0x80+KEY_H)	
#define KEY_i			(0x80+KEY_I)	

//按键5可能的键值
#define KEY_NUM5		21
#define KEY_J			22
#define KEY_K			23
#define KEY_L			24

#define KEY_j			(0x80+KEY_J)	
#define KEY_k			(0x80+KEY_K)	
#define KEY_l			(0x80+KEY_L)

//按键6可能的键值
#define KEY_NUM6		25
#define KEY_M			26
#define KEY_N			27
#define KEY_O			28

#define KEY_m			(0x80+KEY_M)	
#define KEY_n			(0x80+KEY_N)	
#define KEY_o			(0x80+KEY_O)

//按键7可能的键值
#define KEY_NUM7		29
#define KEY_P			30
#define KEY_Q			31
#define KEY_R			32
#define KEY_S			33


#define KEY_p			(0x80+KEY_P)	
#define KEY_q			(0x80+KEY_Q)	
#define KEY_r			(0x80+KEY_R)
#define KEY_s			(0x80+KEY_S)	

//按键8可能的键值
#define KEY_NUM8		34
#define KEY_T			35
#define KEY_U			36
#define KEY_V			37

#define KEY_t			(0x80+KEY_T)	
#define KEY_u			(0x80+KEY_U)	
#define KEY_v			(0x80+KEY_V)

//按键9可能的键值
#define KEY_NUM9		38
#define KEY_W			39
#define KEY_X			40
#define KEY_Y			41
#define KEY_Z			42

#define KEY_w			(0x80+KEY_W)	
#define KEY_x			(0x80+KEY_X)	
#define KEY_y			(0x80+KEY_Y)
#define KEY_z			(0x80+KEY_Z)	//128+42 = 170

//按键0可能的键值
#define KEY_NUM0			43
#define KEY_space			44

//按键1对应的符号集,可以添加
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


//注意键值76被KEY_FUN1占用了



//功能键
#define KEY_SCAN		77
#define KEY_CLEAR		(KEY_SCAN+1)
#define KEY_FUN1		(KEY_CLEAR+1)
#define KEY_FUN2		(KEY_FUN1+1)
#define KEY_FUN3		(KEY_FUN2+1)
#define KEY_FUN4		(KEY_FUN3+1)
#define KEY_xing		(KEY_FUN4+1)		//'*'
#define KEY_jing		(KEY_xing+1)		//'#'


// 扩展的按键值
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

// PN532的事件
#define KEY_PN532_S1	(KEY_EXT_9+1)		// 66 PN532检测到卡
#define KEY_PN532_S2	(KEY_PN532_S1+1)	// 
#define KEY_PN532_S3	(KEY_PN532_S2+1)
#define KEY_PN532_S4	(KEY_PN532_S3+1)
#define KEY_PN532_S5	(KEY_PN532_S4+1)
#define KEY_PN532_S6	(KEY_PN532_S5+1)	// 用户卡错误
#define KEY_PN532_S7	(KEY_PN532_S6+1)	// 用户取消
#define KEY_PN532_S8	(KEY_PN532_S7+1)	// 完成

// GPRS的事件
#define KEY_GPRS_S1		(KEY_PN532_S8+1)	// 74 GPRS模块没有响应
#define KEY_GPRS_S2		(KEY_GPRS_S1+1)         //GPRS 模块没有SIM卡
#define KEY_GPRS_S3		(KEY_GPRS_S2+1)         //GPRS模块没有网络服务
#define KEY_GPRS_S4		(KEY_GPRS_S3+1)         //未知错误
#define KEY_GPRS_S5		(KEY_GPRS_S4+1)         //交易成功
#define KEY_GPRS_S6		(KEY_GPRS_S5+1)         //交易失败
#define KEY_GPRS_S7		(KEY_GPRS_S6+1)         //连接服务器失败
#define KEY_GPRS_S8		(KEY_GPRS_S7+1)         //发送数据失败
#define KEY_GPRS_S9		(KEY_GPRS_S8+1)         //接收数据失败，服务器没有响应
#define KEY_GPRS_S10    (KEY_GPRS_S9+1)			 //GPRS模块拨号失败
#define KEY_GPRS_S11	(KEY_GPRS_S10+1)         //GPRS模块拨号失败
#define KEY_GPRS_S12	(KEY_GPRS_S11+1)         //接收到的数据不合法

#define KEY_LOW_POWER	(KEY_GPRS_S12+1)		//86

#if 0
// 打印机的事件
#define KEY_PRINTER_S1		(KEY_24G_S8+1)			//93 //打印机没有响应
#define KEY_PRINTER_S2		(KEY_PRINTER_S1+1)		//卡纸
#define KEY_PRINTER_S3		(KEY_PRINTER_S2+1)		//纸将用完
#define KEY_PRINTER_S4		(KEY_PRINTER_S3+1)		//纸用完
#define KEY_PRINTER_S5		(KEY_PRINTER_S4+1)		//机构故障
#define KEY_PRINTER_S6		(KEY_PRINTER_S5+1)		//94  //错误的打印数据
#endif

#define KEY_CREATE		0xFF		// 对话框创建消息
#define KEY_DESTORY		0xFE		// 对话框消毁消息
#define KEY_DRAW		0xFD		// 窗体重绘消息


//根据具体应用的按键布局定义左右快捷键
#define	 KEY_SHOTCUT_A		KEY_ALPHA		//左快捷键
#define  KEY_SHOTCUT_B		KEY_ESC			//右快捷键


#define	TRIP_PIN  GPIO_Pin_0		//PC.0
#define POWER_ON  GPIO_Pin_1		//PA.1
#define START     GPIO_Pin_3		//PC.3
	
#define  BEEP_DELAY			200

#define POWER_KEY_PRESSED   (GPIO_ReadInputDataBit(GPIOC, START))

#define Beep_CS  GPIO_Pin_3		//PD.3
		


//键值与ascii对应结构体
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
