/**
*  @file 	Terminal_Para.h
*  @brief  	定义了终端需要保存在NVM区域的参数的数据结构
*  @note	
*/

#ifndef _TERMINAL_PARA_H_
#define _TERMINAL_PARA_H_

//扫描的触发方式
#define SCAN_TRIG_MODE_ONESHOT		1		//单次触发
#define SCAN_TRIG_MODE_KEEP			2		//按键保持
#define SCAN_TRIG_MODE_CONTINUE		3		//连续扫描

//数量录入方式
#define QUANTITY_ENTER_MODE_DISABLE		0		//禁止录入数量
#define QUANTITY_ENTER_MODE_REPEAT		1		//重复输出条码
#define QUANTITY_ENTER_MODE_NUM			2		//输出数量

//传输方式的定义
#define TRANSFER_MODE_BLUETOOTH		1
#define TRANSFER_MODE_KEYBOARD		2
#define TRANSFER_MODE_VIRTUAL_COMM	3
#define TRANSFER_MODE_U_DISK		4
#define TRANSFER_MODE_BATCH			5

/**
 *	对本模块中定义的参数的每个数据域的数据类型做如下简要说明：
 *			B：		二进制，其二进制表示的实际数据是多少就是多少
 *			N：		数值，右靠，首位有效数字前充零，若表示金额，则最右2位为角分，ASCII码或者BCD码
 *			ANS:	字母、数字或特殊符号，左靠，右多余部分填空格。ASCII码
 */

/**
*@brief 定义普通操作员的结构体类型
*/
typedef struct  {
	unsigned char	id[2];				//编号			N
	unsigned char	pwd[4];				//4位密码		N
	unsigned char	name[9];			//姓名			N
}TOperator;

/**
*@brief 定义存储在SPI FLASH内部终端参数的结构类型
*@ note	
*/
#pragma pack(1)

typedef struct  {
	unsigned int			checkvalue;					//4字节		0	B	此份结构的校验值 crc32			
	unsigned char			struct_ver;					//1字节		4	B	参数版本号，标识此结构的版本
	unsigned char			operate_timeout;			//1字节		5   B	操作超时时间 0:关闭  1:30S  2:1分钟  3:2分钟  4:5分钟
	unsigned char			language;					//1字节		6	B   系统使用的语言类型
	
	unsigned short			decoder_switch_map;			//2字节		7   B	解码器开关

	unsigned char			valid_barcode_start_offset;	//1字节		9	B	有效录入的条码的起始位
	unsigned char			valid_barcode_end_offset;	//1字节		10	B	有效录入的条码的结束位

	unsigned char			scan_trig_mode;				//1字节		11  B	扫描按键触发方式 1:单次按键触发  2:按键保持  3:连续扫描

	unsigned char			quantity_enter_mode;		//1字节		12  B   数量录入方式  0:关闭数量录入  1: 重复输出条码  2:输出条码数量

	unsigned char			add_time_option;			//1字节		13  B	是否添加扫描时间	0:不添加  1:添加
	unsigned char			add_date_option;			//1字节		14  B	是否添加扫描日期	0:不添加  1:添加
	
	unsigned char			database_query_option;		//1字节		15  B	数据库查询是否开启	0:关闭  1:开启

	unsigned char			avoid_dunplication_option;	//1字节		16  B	防重码功能是否开启	0:关闭  1:开启

	unsigned char			barcode_statistic_option;	//1字节		17  B	条码统计功能是否开启	0:关闭  1:开启

	unsigned char			transfer_confirm_option;	//1字节		18  B	传输确认功能是否开启	0:关闭  1:开启

	unsigned char			transfer_mode;				//1字节		19	B	数据传输方式的选择	1;蓝牙  2:USB键盘  3:USB虚拟串口 4:U盘模式 5:脱机保存方式

	unsigned char			batch_transfer_mode;		//1字节		20	B	脱机数据的传输方式  1:蓝牙  2:USB键盘  3:USB虚拟串口

	unsigned char			system_psw[8];				//8字节		21	B	系统密码

	unsigned char			beeper_vol;					//1字节     29	B	按键音开关  0:关  1:开
	
	unsigned char			bluetooth_transfer_option;	//1字节		30	B	蓝牙传输模式选项	0: 关闭离线数据存储   1:开启离线数据存储

	unsigned char			u_disk_storage_file[14];	//14字节	31  N	U盘传输模式时存储扫描数据的文件

	unsigned char			batch_transfer_statistics;	//1字节		45  B	脱机数据传输时的数据统计功能选项  0:关闭  1:开启

	unsigned char			transfer_seperator[3];		//3字节		46  B	数据传输时的数据分隔符

	//unsigned char			rfu[13];						//13字节 49 B		
	unsigned char			endtag[2];					//0x55,0xAA  62      一共64字节
} TTerminalPara;
#pragma pack()

int ReadTerminalPara(void);
int SaveTerminalPara(void);
int DefaultTerminalPara(void);
#endif
