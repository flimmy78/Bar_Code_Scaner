/**
* @file app.c
* @brief 2.4GPOS项目应用主程序
*
* @version V0.0.1
* @author joe
* @date 2010年04月28日
* @note
*		参考《中国移动手机支付业务现场脱机支付POS终端 规范_v1.0.0.doc》
* 
* @copy
*
* 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
* 本公司以外的项目。本司保留一切追究权利。
*
* <h1><center>&copy; COPYRIGHT 2010 netcom</center></h1>
*/

/* Private include -----------------------------------------------------------*/
#include "app.h"
#include "Dialog.h"
#include <string.h>
#include <stdlib.h>
#include "stm32f10x_lib.h"
#include "ucos_ii.h"
#include <stdio.h>
#include "lcd.h"
#include "data_uart.h"
#include "gui.h"
#include "pic.h"
#include "Terminal_Para.h"
#include "calendar.h"
#include "dlg.h"
#include "basic_fun.h"
#include "timeBase.h"
#include "usb_pwr.h"
#include "usb_istr.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "PCUsart.h"
#include "power_detect.h"
#include "JMemory.h"
#include "ff.h"
#include "uE988_scan.h"
#include "YFBT07.h"
#include "data_transfer.h"

const unsigned char *app_ver="V1.0.1";		//应用版本号

static unsigned char	BarCode[31];				//检测条形码的后台任务与前台之间传递条形码的变量
static unsigned char	last_BarCode[31];			//上次获取到的条形码
static unsigned char	BarCode_Type[20];			//条码类型
static unsigned char	BarCode_Len;				//条码长度
static unsigned char	dlg_refresh_flag;	//窗体强制刷新的标记
static unsigned int		barcode_got_flag;				//获取到条形码的标记
static int				current_home_item;
static unsigned char	scan_func_state;	
static unsigned char	valid_barcode_start_offset_tmp;
static unsigned char	valid_barcode_end_offset_tmp;
static unsigned char	line3_save[21];
static unsigned char	tmp_op_timeout;	
static unsigned char	special_func_flag;

//static TCHECK_INFO_RECORD	check_info;
static TCHECK_INFO_RECORD	*p_check_info;
static TGOODS_SPEC_RECORD   *p_goods_node;	
static int					searched_rec_index;		//搜索到的记录索引

typedef struct tag_chk_info_context
{
	TCHECK_INFO_RECORD	check_info_node;			//盘点信息
	TGOODS_SPEC_RECORD	goods_node;					//商品信息
	unsigned int		chk_info_index;				//盘点信息在其清单中的位置索引
	unsigned int		goods_info_index;			//商品信息在其清单中的位置索引
	unsigned int		chkinf_need_update_flag;	//当前条码的盘点信息是否需要更新的标记
	int					add_value;					//当前条码的盘点数量的累加值
}CHK_INFO_CONTEXT;

static	CHK_INFO_CONTEXT			op_context;		//对条码进行盘点时的上下文环境	
static	unsigned int	send_num;


unsigned char			need_refresh_signal;			//需要刷新无线信号强度的标记
unsigned char			no_update_icon_flg;
unsigned char			task_exit_flag;
unsigned char			usb_cable_state;
unsigned char			need_refresh_edit_index_tbl[4];	//需要刷新的编辑框列表
unsigned int					scan_start;						//开始扫描的标记

extern TDialog					g_dlg;
extern unsigned char			g_editValue[MAX_EDIT][MAX_EDITNUM];//编辑框的全局变量
extern unsigned char			menu_current_item;			//当前菜单项
extern unsigned char			menu_start;
extern unsigned char			max_dlg;
extern unsigned char			next_dlg_id;
extern	TTerminalPara			g_param;							//Terminal Param
//extern TDRAW_ITEM				draw_content[];
extern unsigned char			gFontSize;			// 字号
extern unsigned char			display_menu_item_array[5];
extern int						bluetooth_module_state;
extern FIL						file2;
extern int						bluetooth_match_cmd_cnt;
extern int						bluetooth_state_cnt;
extern TypedefDateTime       currentDateTime;	
extern unsigned char			data_trans_state;				//数据导入导出时的状态
extern unsigned char			update_icon_cnt;
extern unsigned int				vcom_device_state;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


////////////////////////////用于工厂测试程序///////////////////////////////////
#define  KEY_COUNT   10 //按键总数目
/**
*@brief 按键扫描时定义每个按键在屏幕上的显示位置及其按键名在屏幕上的宽度信息
*/
typedef struct  {
	unsigned char key;
	unsigned char x;
	unsigned char y;
	unsigned char width;
	int		      clear_flag;		//是否已经扫描
} Tkeyscan_pos_width;

static int factory_test_task(int param1, int param2, void *param3, void *param4);
static unsigned char dlgproc_test_proc(unsigned char *key);
static int scan_aging_task(int param1, int param2, void *param3, void *param4);

static void test_LCD_display(void);
static void testKeypad(void);//按键测试
static void testScanerInit(void);//扫描头测试
static void testBluetooth(void);//蓝牙测试
static void testUSB(void);//USB测试
static void testRTC(void);//rtc测试
static void testAD(void);

typedef void (*testfunc)(void);		//定义测试函数指针
typedef	void (*pFunction)(void);

const  testfunc testfunc_tbl[] = {
	test_LCD_display,			//测试显示
	testKeypad,					//测试键盘
	testScanerInit,				//测试扫描头
	testUSB,					//USB测试
	testRTC,					//RTC测试
	testAD,				    //AD采样测试
	testBluetooth				//测试蓝牙模块
};


/**
* @brief 这个表格是测试按键扫描时，按键在屏幕上的显示位置相对应的
* @see	每个按键在扫描时，对应按键名在屏幕上的显示如下：
POWER 
 -  ESC ENTER  -
LEFT UP RIGHT DOWN
      SCAN
*/
static Tkeyscan_pos_width keyscan_pos_width_tbl[] =
{
	{KEY_POWER,0,0,5,1},
	{KEY_LEFT_SHOTCUT,0,13,4,1},{KEY_ESC,6*4,13,4,1},{KEY_ENTER,6*8,13,6,1},{KEY_RIGHT_SHOTCUT,6*14,13,3,1},
	{KEY_LEFT,0,13*2,5,1},{KEY_UP,6*5,13*2,3,1},{KEY_RIGHT,6*8,13*2,6,1},{KEY_DOWN,6*14,13*2,4,1},
	{KEY_SCAN,6*6,13*3,4,1},

	{0xff,0,0,0,0}//表的结束标记
};

static int						testproc_first_enterkey;//测试程序里，按一次测试按键的标记
static  unsigned char			enter_scaner_test_flag;//进入扫描测试的标志

/**
***************************************************************************
*@brief 后台检测条形码的任务
*@param[in] int param1  指明是否受系统设置的条码位数的规则限制  0：受  1：不受
*@note 
***************************************************************************
*/
static int ScanBarCode_task(int param1, int param2, void *param3, void *param4)
{
	int								command;
	unsigned char					tmp[32],len;
	while(1)
	{
		if (0 == task_read_command_from_ui(&command,0))
		{
			if (TASK_EXIT == command)
			{
				if (barcode_got_flag)
				{
					//已经有条形码检测到或者输入了,跳转到询问是否需要更新库存信息的界面
					return KEY_EXT_2;
				}
				else
				{
					//还没有条形码检测到或者输入过，那么直接返回到主界面
					return KEY_EXT_3;
				}
			}

			if (TASK_RUN == command)
			{
				//用户按了扫描键
				//开始扫描商品条形码
				//scan_start = 1;
				memset(BarCode,0,31);
				memset(tmp,0,31);
				memset(BarCode_Type,0,20);
				BarCode_Len = 0;
				if (UE988_get_barcode(BarCode_Type, tmp,30, &BarCode_Len) == 0)
				//if (scanner_get_barcode(tmp,30,BarCode_Type,&BarCode_Len)  == 0)
				{
					//获取到条码
					//led_r_ctrl(1);
					Delay(5000);
					Beep(BEEP_DELAY);
					//led_r_ctrl(0);
					barcode_got_flag = 1;
				}
				//else
				//{
				//	//获取条形码失败
				//	//不需要处理，也不需要通知用户，就当做没有扫描到条码

				//	task_send_command_to_ui(KEY_EXT_1);
				//}

				if (param1 == 0)
				{
					//表示需要根据系统设置的录入条码位数来返回获取到的条码
					if ((g_param.valid_barcode_start_offset == 0xff)&&(g_param.valid_barcode_end_offset == 0xff))
					{
						memcpy(BarCode,tmp,BarCode_Len);
					}
					else
					{
						if ((g_param.valid_barcode_start_offset < 30)&&(g_param.valid_barcode_end_offset <= 30)&&(g_param.valid_barcode_start_offset < g_param.valid_barcode_end_offset)&&(g_param.valid_barcode_start_offset < BarCode_Len))
						{
							len = g_param.valid_barcode_end_offset - g_param.valid_barcode_start_offset;
							if (len > (BarCode_Len - g_param.valid_barcode_start_offset))
							{
								len = BarCode_Len - g_param.valid_barcode_start_offset;
							}
							memcpy(BarCode,tmp+g_param.valid_barcode_start_offset-1,len+1);
						}
						else
						{
							memcpy(BarCode,tmp,BarCode_Len);
						}

					}

				}
				else
				{
					memcpy(BarCode,tmp,BarCode_Len);
				}

				task_send_command_to_ui(KEY_EXT_1);
				//scan_start = 0;

				//模拟  
				//OSTimeDlyHMSM(0,0,0,10);
				//strcpy((char*)BarCode,"123456789123");
				//task_send_command_to_ui(KEY_EXT_1);
			}

		}
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}

/**
* @brief 检查输入的时间格式是否合法
* @note  输入的时间格式是YYYYMMDDHHMMSS
* return  0: 合法
*        -1: 非法
*/
static int check_time(unsigned char * input)
{
	unsigned short year,month,date,temp;
	year = ((*input)- 0x30)*1000 + (*(input + 1) - 0x30)*100 + (*(input + 2) - 0x30)*10 + (*(input + 3) - 0x30);

	if (year < 2000 ||year > 2030)//不允许输入2000~2030之外的数据
	{
		return -1;
	}

	//检查输入月份的合法性
	month = (*(input + 4) - 0x30)*10 + (*(input + 5) - 0x30);		//将月份转换为10进制
	if ((month > 12)||(month == 0))
	{
		return -1;
	}

	//检查输入日期的合法性
	date = (*(input + 6) - 0x30)*10 + (*(input + 7) - 0x30);		//将日期转换为10进制
	//月大，31天
	if ((month == 1)||(month == 3)||(month == 5) ||(month == 7) ||(month == 8) ||(month == 10) ||(month == 12))  
	{
		if ((date > 31)||(date == 0)) 
		{
			return -1;
		}
	}
	//2月份
	else if (month == 2) 
	{
		if ((year%4==0 && year%100!=0) || (year%400==0))		//闰年
		{
			if ((date > 29)||(date == 0)) 
			{
				return -1;
			}
		}
		else
		{
			if ((date > 28)||(date == 0)) 
			{
				return -1;
			}
		}
	}
	//月小
	else
	{
		if ((date > 30)||(date == 0)) 
		{
			return -1;
		}
	}


	//检查输入小时的合法性
	temp = (*(input + 8) - 0x30)*10 + (*(input + 9) - 0x30);		//将小时转换为10进制
	if (temp > 23) 
	{
		return -1;
	}
	//检查输入分钟的合法性
	temp = (*(input + 10) - 0x30)*10 + (*(input + 11) - 0x30);		//将分钟转换为10进制
	if (temp > 59) 
	{
		return -1;
	}
	//检查输入秒数的合法性
	temp = (*(input + 12) - 0x30)*10 + (*(input + 13) - 0x30);		//将秒数转换为10进制
	if (temp > 59) 
	{
		return -1;
	}

	//合法的输入,将用户输入的时间更新到当前时间里面
	currentDateTime.year = (unsigned char)(year - 2000);
	currentDateTime.month = (unsigned char)month;
	currentDateTime.day = (unsigned char)date;
	currentDateTime.hour = (*(input + 8) - 0x30)*10 + (*(input + 9) - 0x30);
	currentDateTime.min = (*(input + 10) - 0x30)*10 + (*(input + 11) - 0x30); 
	currentDateTime.sec = (*(input + 12) - 0x30)*10 + (*(input + 13) - 0x30); 
	return 0;		
}


static void refresh_home_item(void)
{
	gui_FillRect(0,16,GUI_WIDTH,24,BG_COLOR,0);
	switch (current_home_item)
	{
	case 1:
		gui_PictureOut(36,16,PIC_SCAN,0);
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"  Scan  ",1,0);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"  扫描  ",1,0);
		}
		break;
	case 2:
		gui_PictureOut(36,16,PIC_TRANS,0);
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"Transfer",1,0);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"数据传输",1,0);
		}
		break;
	case 3:
		gui_PictureOut(36,16,PIC_SETTING,0);
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"Setting",1,0);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"系统设置",1,0);
		}
		break;
	case 4:
		gui_PictureOut(36,16,PIC_INFO,0);
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"  Info  ",1,0);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,42,"系统信息",1,0);
		}
		break;
	default:
		break;
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
}

/**
* @brief 主界面回调函数，task_ui第一个运行起来的界面回调函数
*/
static unsigned char dlgproc_logo(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		special_func_flag = 0;
		refresh_home_item();
		//gui_PictureOut(36,16,PIC_SCAN+(current_home_item-1),1);
		//gui_PictureOut(118,ICON_HEIGHT+4,PIC_UP,1);
		//gui_PictureOut(118,ICON_HEIGHT+1+26,PIC_DOWN,1);
	}

	if (DLGCB_RUN == type)
	{
		if (*key == KEY_UP || *key == KEY_DOWN)
		{
			if (*key == KEY_UP)
			{
				if (current_home_item == 1)
				{
					current_home_item = 4;
				}
				else
				{
					current_home_item--;
				}
			}
			else
			{
				if (current_home_item == 4)
				{
					current_home_item = 1;
				}
				else
				{
					current_home_item++;
				}
			}

			refresh_home_item();
			return RUN_RERUN;
		}

		if (*key == KEY_LEFT_SHOTCUT || *key == KEY_ENTER || *key == KEY_SCAN)
		{
			*key = KEY_EXT_1 + current_home_item - 1;
			return RUN_NEXT;
		}

		//输入特殊按键序列: "KEY_ESC、LEFT、KEY_ESC、LEFT、KEY_ESC、LEFT、KEY_ESC" 进入隐藏功能菜单
		if ((KEY_ESC == *key)&&(special_func_flag == 0))
		{
			special_func_flag = 1;
			return RUN_RERUN;
		}

		if (1 == special_func_flag)
		{	
			if ( KEY_LEFT == *key)
			{
				special_func_flag = 2;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (2 == special_func_flag)
		{	
			if ( KEY_ESC == *key)
			{
				special_func_flag = 3;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (3 == special_func_flag)
		{	
			if ( KEY_LEFT == *key)
			{
				special_func_flag = 4;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (4 == special_func_flag)
		{	
			if ( KEY_ESC == *key)
			{
				special_func_flag = 5;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (5 == special_func_flag)
		{	
			if ( KEY_LEFT == *key)
			{
				special_func_flag = 6;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (6 == special_func_flag)
		{	
			if ( KEY_ESC == *key)
			{
				*key = KEY_EXT_9;
				return RUN_NEXT;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}
		
	}
	return RUN_CONTINUE;
}


//构造传输的字符串
static void build_transfer_str(TCHECK_INFO_RECORD * p_check,unsigned int num,unsigned char *str)
{
	unsigned char  temp[20];
	//构造传输的字符串
	strcpy(str,p_check->bar_code);
	if (g_param.add_time_option || g_param.add_date_option)
	{
		build_time_dis_str(p_check->check_time,temp,0);
		
		if (g_param.add_date_option)
		{
			strcat(str,g_param.transfer_seperator);
			strcat(str,temp);
			str[strlen(str) - 9] = 0;
		}

		if (g_param.add_time_option)
		{
			strcat(str,g_param.transfer_seperator);
			strcat(str,temp+11);
		}
	}

	if (g_param.quantity_enter_mode == QUANTITY_ENTER_MODE_NUM)
	{
		strcat(str,g_param.transfer_seperator);
		sprintf(temp,"%d",num);
		strcat(str,temp);
	}

	strcat(str,"\x0d\x0a");		//换行符
}

//检测当前传输连接的状态
//0:断开状态
//1:连接状态
static int check_link_state(unsigned char transfer_mode)
{
	if (transfer_mode == TRANSFER_MODE_BLUETOOTH)
	{
		return YFBT07_Connect_state();
	}
	else if (transfer_mode == TRANSFER_MODE_KEYBOARD || transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	{
		if (bDeviceState == CONFIGURED)
		{
			return 1;
		}

		return 0;
	}
	else if (transfer_mode == TRANSFER_MODE_U_DISK)
	{
		//U DISK存储方式时，如果没有指定存储的目标文件，认为连接状态断开
		if (g_param.u_disk_storage_file[0] == 0)
		{
			return 0;
		}

		if (f_open(&file2,g_param.u_disk_storage_file,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			return 0;
		}

		if (file2.fsize > 1024*1024)
		{
			return 0;	//如果目标文件太大也认为连接状态是断开的
		}
		return 1;
	}
	else
	{
		//脱机存储方式时，如果脱机存储的记录达到上限了，认为连接状态断开
		if (record_module_count(REC_TYPE_BATCH) == CHECK_LIST_MAX_CNT)
		{
			return 0;
		}

		return 1;
	}
}

//脱机保存传输的字符串
static int batch_save_barcode_str(unsigned char *str)
{
	TBATCH_NODE  batch_node;

	if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH && g_param.bluetooth_transfer_option == 0)
	{
		//蓝牙传输离线脱机保存选项关闭时，不保存
		return 0;
	}

	memset((void*)&batch_node,0,sizeof(TBATCH_NODE));
	strcpy(batch_node.batch_str,str);
	if(record_add(REC_TYPE_BATCH,(unsigned char*)&batch_node))
	{
		OSTimeDlyHMSM(0,0,0,200);
		if(record_add(REC_TYPE_BATCH,(unsigned char*)&batch_node))
		{
			return -1;
		}
	}

	return 0;
}

//传输一次字符串
static int transfer_barcode_str(unsigned char *str,unsigned char transfer_mode)
{
	TBATCH_NODE  batch_node;
	int			i,cnt;
	unsigned char key_value_report[8];

	if (transfer_mode == TRANSFER_MODE_BLUETOOTH)
	{
		YFBT07_SendKey(str,strlen(str));
	}
	else if (transfer_mode == TRANSFER_MODE_KEYBOARD)
	{
		OSSchedLock();
		for (i = 0; i < (strlen(str)-1); i++)
		{
			ascii_to_keyreport(str[i],key_value_report);

			SendData_To_PC(key_value_report, 3);
			SendData_To_PC("\x00\x00\x00", 3);
		}

		//memcpy(key_value_report,"\x00\x00\x28",3);	//换行

		//SendData_To_PC(key_value_report, 3);
		//SendData_To_PC("\x00\x00\x00", 3);	//弹起
		OSSchedUnlock();
	}
	else if(transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	{
		SendData_To_PC(str,strlen(str));
	}
	else if (transfer_mode == TRANSFER_MODE_U_DISK)
	{
		if (f_open(&file2,g_param.u_disk_storage_file,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			return -1;
		}
		
		f_lseek(&file2,file2.fsize);	//文件指针移动文件末尾
		
		if (f_write(&file2,(void*)str,strlen(str),&cnt) != FR_OK)
		{
			f_close(&file2);
			return -1;
		}

		if (cnt != strlen(str))
		{
			f_close(&file2);
			return -1;
		}

		f_close(&file2);
	}
	else
	{
		//脱机保存的方式
		memset((void*)&batch_node,0,sizeof(TBATCH_NODE));
		strcpy(batch_node.batch_str,str);
		if(record_add(REC_TYPE_BATCH,(unsigned char*)&batch_node))
		{
			OSTimeDlyHMSM(0,0,0,200);
			if(record_add(REC_TYPE_BATCH,(unsigned char*)&batch_node))
			{
				return -1;
			}
		}
	}

	return 0;
}


//传输获取到的条码信息
static int transfer_barcode_proc(TCHECK_INFO_RECORD * p_check,unsigned int num)
{
	unsigned char  str[64];
	
	build_transfer_str(p_check,num,str);

	if (check_link_state(g_param.transfer_mode) == 0)
	{
		//传输连接是断开状态
		Beep(BEEP_DELAY*2);
		if (g_param.quantity_enter_mode == QUANTITY_ENTER_MODE_REPEAT)
		{
			while (num)
			{
				batch_save_barcode_str(str);
				num--;
				OSTimeDlyHMSM(0,0,0,50);
			}
		}
		else
		{
			batch_save_barcode_str(str);
		}
	}
	else
	{
		if (g_param.quantity_enter_mode == QUANTITY_ENTER_MODE_REPEAT)
		{
			while (num)
			{
				transfer_barcode_str(str,g_param.transfer_mode);
				num--;
				OSTimeDlyHMSM(0,0,0,50);
			}
		}
		else
		{
			transfer_barcode_str(str,g_param.transfer_mode);
		}
	}
}

#define	ERR_TYPE_DUPLICATION	1		//条码重复错误	
#define	ERR_TYPE_OVERALL		2		//条码存储已达上限
#define	ERR_TYPE_STOREFAIL		3		//条码存储失败

//错误提示
static unsigned char err_tip(unsigned char err_type)
{
	Beep(BEEP_DELAY*2);
	switch (err_type)
	{
	case ERR_TYPE_DUPLICATION:
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,39,"Barcode duplication!",0,0);
			strcpy(line3_save,"Barcode duplication!");
		}
		else
		{
			gui_TextOut(0,39,"条码已存在!",0,0);
			strcpy(line3_save,"条码已存在!");
		}
		break;
	case ERR_TYPE_OVERALL:
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,39,"No More Space",0,0);
		}
		else
		{
			gui_TextOut(0,39,"存储已达上限!",0,0);
		}
		break;
	case ERR_TYPE_STOREFAIL:
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,39,"Record Store fail!",0,0);
		}
		else
		{
			gui_TextOut(0,39,"记录存储失败!",0,0);
		}
		break;
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	return RUN_RERUN;
}


//显示商品规格
int display_goods_spec(void)
{
	int i,j;

	for (i = 0; i < 5;i++)
	{
		if (op_context.goods_node.spec[i][0] != 0)
		{
			break;
		}
	}

	if (i == 5)
	{
		return 0;	//没有可以显示的商品规格
	}

	gui_FillRect(0,0,LCD_WIDTH,LCD_HEIGHT,BG_COLOR,0);

	for (i = 0,j = 0; i < 5;i++)
	{
		if (op_context.goods_node.spec[i][0] != 0)
		{
			gui_TextOut(0,13*j,op_context.goods_node.spec[i],0,0);
			j++;
		}
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	no_update_icon_flg = 1;

	return j;
}


/**
* @brief 扫描功能
*/
static unsigned char dlgproc_scan_func(unsigned char type,unsigned char *key)
{
	unsigned char  dis_str[13];
	if (DLGCB_CREATE == type)
	{
		if (dlg_refresh_flag == 0)
		{
			if (g_param.database_query_option)
			{
				//如果开启了查询数据库选项，那么需要检查数据库文件是否存在
				if (check_database())
				{
					gui_FillRect(0,0,GUI_WIDTH,GUI_HEIGHT,BG_COLOR,1);
					if (g_param.language == LANG_ENGLISH)
					{
						gui_TextOut_ext(CENTER_ALIGN,20,"Databse isn't exist!",0,1);
					}
					else
					{
						gui_TextOut_ext(CENTER_ALIGN,20,"数据库文件不存在!",0,1);
					}
					OSTimeDlyHMSM(0,0,1,0);
				}
			}
		}
	}

	if (DLGCB_DRAW == type)
	{
		if (dlg_refresh_flag == 0)
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,26,"Please press\"SCAN\"...",0,1);
			}
			else
			{
				gui_TextOut(0,26,"请按\"SCAN\"键扫描...",0,1);
			}

			scan_func_state = 0;
			memset((void*)&op_context,0,sizeof(CHK_INFO_CONTEXT));
			memset(last_BarCode,0,31);
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//后台等待检测条形码的任务
		}
		else
		{
			if (strlen(BarCode) > 20)
			{
				gui_TextOut(0,13,BarCode,0,1);
			}
			else
			{
				gui_TextOut(0,26,BarCode,0,1);
			}

			gui_TextOut(0,39,line3_save,0,1);

			if (scan_func_state == 2)
			{
				
				if (g_param.language == LANG_ENGLISH)
				{
					sprintf(dis_str,"Esc    Enter% 8d",send_num);
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,dis_str,0,1);
				}
				else
				{
					sprintf(dis_str,"取消    确认% 8d",send_num);
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,dis_str,0,1);
				}
			}
			else if (scan_func_state == 1)
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc            Enter",0,1);
				}
				else
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"取消            确认",0,1);
				}
			}	
		}

		dlg_refresh_flag = 0;
	}

	if (DLGCB_RUN == type)
	{
		if ((KEY_SCAN == *key)&&(scan_start == 0)&&(task_exit_flag == 0))
		{
			if (scan_func_state == 0)
			{
				scan_start = 1;
				ui_send_command_to_task(TASK_RUN);
				return RUN_RERUN;
			}
		}

		if (KEY_EXT_1 == *key)
		{
			scan_start = 0;

			if (BarCode[0] == 0)
			{
				//后台没有扫描到任何条形码
				return RUN_RERUN;
			}

			//如果是后台扫描到条形码，那么需要将后台获取到的条形码显示出来
			
			//需要先判断，本次输入的条形码是否和上次的一致
			if (strcmp(BarCode,last_BarCode))
			{
				gui_FillRect(0,13,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,0);
				if (strlen(BarCode) > 20)
				{
					gui_TextOut(0,13,BarCode,0,0);
				}
				else
				{
					gui_TextOut(0,26,BarCode,0,0);
				}
				gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);
				strcpy(last_BarCode,BarCode);

				if ((op_context.chkinf_need_update_flag)&&(op_context.chk_info_index))
				{
					//更新上一个盘点信息到数据库
					if(record_module_replace(REC_TYPE_CHECK,op_context.chk_info_index,(unsigned char*)&op_context.check_info_node,(TNODE_MODIFY_INFO*)0))
					{
						return err_tip(ERR_TYPE_STOREFAIL);
					}
				}
				p_check_info = (TCHECK_INFO_RECORD*)rec_searchby_tag(REC_TYPE_CHECK,TAG_BARCODE,BarCode,&searched_rec_index);
				if (p_check_info)
				{
					//盘点清单中已经有该条码的信息
					if (g_param.avoid_dunplication_option)
					{
						return err_tip(ERR_TYPE_DUPLICATION);
					}
					memcpy((void*)&op_context.check_info_node,(void*)p_check_info,sizeof(TCHECK_INFO_RECORD));
					op_context.chk_info_index = searched_rec_index;
				}
				else
				{
					//盘点清单中还没有该条码的信息
					memset((void*)&op_context.check_info_node,0,sizeof(TCHECK_INFO_RECORD));
					strcpy(op_context.check_info_node.bar_code,BarCode);

					if (record_module_count(REC_TYPE_CHECK) == CHECK_LIST_MAX_CNT )
					{
						return err_tip(ERR_TYPE_OVERALL);
					}

					if(record_add(REC_TYPE_CHECK,(unsigned char*)&op_context.check_info_node))
					{
						OSTimeDlyHMSM(0,0,0,200);
						if(record_add(REC_TYPE_CHECK,(unsigned char*)&op_context.check_info_node))
						{
							return err_tip(ERR_TYPE_STOREFAIL);
						}
					}

					op_context.chk_info_index = record_module_count(REC_TYPE_CHECK);
				}

				if (g_param.database_query_option)
				{
					//需要查询数据库
					p_goods_node = (TGOODS_SPEC_RECORD*)rec_searchby_tag(REC_TYPE_GOODS_LIST,TAG_BARCODE,BarCode,&searched_rec_index);
					if (p_goods_node)
					{
						//在商品清单中搜索到该条码
						memcpy((void*)&op_context.goods_node,(void*)p_goods_node,sizeof(TGOODS_SPEC_RECORD));
						op_context.goods_info_index = searched_rec_index;

						op_context.check_info_node.spec_rec_index = op_context.goods_info_index;
					}
				}

				op_context.add_value = 1;
			}
			else
			{
				//与上一次的条码相同
				if (g_param.avoid_dunplication_option)
				{
					return err_tip(ERR_TYPE_DUPLICATION);
				}

				if (op_context.check_info_node.check_num<999999999)
				{
					op_context.add_value = 1;
				}
			}

			op_context.check_info_node.check_num += op_context.add_value;

			if (op_context.add_value)
			{
				//盘点数量发生了变化
				op_context.chkinf_need_update_flag = 1;
			}

			//如果需要获取盘点时间或日期
			if (g_param.add_time_option || g_param.add_date_option)
			{
				GetDateTime();
				Systime2BCD(op_context.check_info_node.check_time);
			}

			if (g_param.database_query_option)
			{
				//开启了数据库查询选项，需要显示商品的规格信息
				if (op_context.goods_node.bar_code[0])
				{
					gui_TextOut(0,39,op_context.goods_node.name,0,1);
					strcpy(line3_save,op_context.goods_node.name);
				}
				else
				{
					if(LANG_ENGLISH == g_param.language)
					{
						gui_TextOut(0,39,"**NO SPEC**",0,0);
						strcpy(line3_save,"**NO SPEC**");
					}
					else
					{
						gui_TextOut(0,39,"**无商品信息**",0,0);
						strcpy(line3_save,"**无商品信息**");
					}
				}
			}

			if (g_param.barcode_statistic_option)
			{
				//开启了统计功能选项，需要显示该条码的数量
				
				sprintf(dis_str,"<%d         >",op_context.check_info_node.check_num);
				
				gui_TextOut(0,39,dis_str,0,0);
				strcpy(line3_save,dis_str);
			}

			gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);
			send_num = 1;

			if (g_param.transfer_confirm_option || g_param.quantity_enter_mode)
			{
				//开启了传输确认选项或者开启了数量录入
				if (g_param.quantity_enter_mode)
				{
					scan_func_state = 2;		//等待输入数量或者确认发送
					if (g_param.language == LANG_ENGLISH)
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc     Enter      1",0,1);
					}
					else
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"取消    确认       1",0,1);
					}
				}
				else
				{
					scan_func_state = 1;		//等待确认发送
					if (g_param.language == LANG_ENGLISH)
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc            Enter",0,1);
					}
					else
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"取消            确认",0,1);
					}
				}	

				return RUN_RERUN;
			}
			else
			{
				//没有开启传输确认且没有开启录入数量的选项时，扫描到的条码直接试图发送出去
				transfer_barcode_proc(&op_context.check_info_node,1);
				return RUN_RERUN;
			}
		}

		if (scan_func_state == 1)
		{
			if (*key == KEY_SCAN || *key == KEY_RIGHT_SHOTCUT || *key == KEY_ENTER)
			{
				transfer_barcode_proc(&op_context.check_info_node,1);
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Options             ",0,1);
				}
				else
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"选项                ",0,1);
				}
				scan_func_state = 0;
				return RUN_RERUN;
			}
			else if (*key == KEY_ESC || *key == KEY_LEFT_SHOTCUT)
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Options             ",0,1);
				}
				else
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"选项                ",0,1);
				}
				scan_func_state = 0;
				return RUN_RETURN;
			}
		}
		else if (scan_func_state == 2)
		{
			if (*key == KEY_RIGHT_SHOTCUT)
			{
				*key = KEY_EXT_9;
				return RUN_NEXT;
			}
			else if (*key == KEY_ENTER || *key == KEY_SCAN)
			{
				transfer_barcode_proc(&op_context.check_info_node,send_num);
				scan_func_state = 0;
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Options             ",0,1);
				}
				else
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"选项                ",0,1);
				}
				return RUN_RERUN;
			}
			else if (*key == KEY_ESC || *key == KEY_LEFT_SHOTCUT)
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Options             ",0,1);
				}
				else
				{
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"选项                ",0,1);
				}
				scan_func_state = 0;
				return RUN_RETURN;
			}
		}
		else
		{
			if ((scan_func_state & 0x10) == 0x10)
			{
				//显示商品规格的状态，只响应KEY_UP键和KEY_ESC键
				if(*key == KEY_UP || *key == KEY_ESC)
				{
					scan_func_state &= 0x0f;
					dlg_refresh_flag = 1;
					no_update_icon_flg = 0;
					update_icon_cnt = 5;
					gui_FillRect(0,0,GUI_WIDTH,ICON_HEIGHT, BG_COLOR,0);
					gui_LineH(0,ICON_HEIGHT,GUI_WIDTH,TXT_COLOR,0);
					dlg_update_icon(0,0);
					gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT/2);
					dlg_updatetime(52, 0);
					return RUN_RETURN;
				}
				else
				{
					return RUN_RERUN;
				}
			}

			if (*key == KEY_ESC)
			{
				if ((op_context.chkinf_need_update_flag)&&(op_context.chk_info_index))
				{
					//更新上一个盘点信息到数据库
					if(record_module_replace(REC_TYPE_CHECK,op_context.chk_info_index,(unsigned char*)&op_context.check_info_node,(TNODE_MODIFY_INFO*)0))
					{
						err_tip(ERR_TYPE_STOREFAIL);
						task_exit_flag = 1;
						OSTimeDlyHMSM(0,0,1,0);
						task_exit_flag = 0;
						return RUN_NEXT;
					}
				}
			}
		}

		if ((*key == KEY_DOWN)&&(g_param.database_query_option != 0)&&(op_context.goods_node.bar_code[0] != 0))
		{
			if (display_goods_spec())
			{
				scan_func_state |= 0x10;
			}
			return RUN_RERUN;
		}
	}

	return RUN_CONTINUE;
}


/**
* @brief 只需要通过上下翻页键选择菜单，然后通过ENTER键跳转到下一级菜单的窗体的通用处理函数
* note   注意这些窗体的跳转键值必须与菜单项设置为相应的KEY_EXT_1,KEY_EXT_2,...
*/
static unsigned char proc_list_menu(unsigned char type,unsigned char *key)
{
	if (DLGCB_RUN == type)
	{
		if (KEY_SCAN == *key || KEY_ENTER == *key)
		{
			*key = KEY_EXT_1 + menu_current_item - 1;
			saveDrawContext();	//保存当前菜单在跳转到子菜单时的状态
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			releaseDrawContext();
			return RUN_NEXT;
		}

	}
	return RUN_CONTINUE;
}

/**
* @brief 扫描功能选项
*/
static unsigned char dlgproc_scan_option_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}

/**
* @brief 系统信息
*/
static unsigned char dlgproc_system_info(unsigned char type,unsigned char *key)
{
	//return proc_list_menu(type,key);

	if (DLGCB_CREATE == type)
	{
		special_func_flag = 0;
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_SCAN == *key || KEY_ENTER == *key)
		{
			*key = KEY_EXT_1 + menu_current_item - 1;
			saveDrawContext();	//保存当前菜单在跳转到子菜单时的状态
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			releaseDrawContext();
			return RUN_NEXT;
		}

		//输入特殊按键序列: "LEFT_SHOTCUT、LEFT、RIGHT、LEFT_SHOTCUT、LEFT、RIGHT、LEFT_SHOTCUT" 进入隐藏功能菜单
		if ((KEY_LEFT_SHOTCUT == *key)&&(special_func_flag == 0))
		{
			special_func_flag = 1;
			return RUN_RERUN;
		}

		if (1 == special_func_flag)
		{	
			if ( KEY_LEFT == *key)
			{
				special_func_flag = 2;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (2 == special_func_flag)
		{	
			if ( KEY_RIGHT == *key)
			{
				special_func_flag = 3;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (3 == special_func_flag)
		{	
			if ( KEY_LEFT_SHOTCUT == *key)
			{
				special_func_flag = 4;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (4 == special_func_flag)
		{	
			if ( KEY_LEFT == *key)
			{
				special_func_flag = 5;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (5 == special_func_flag)
		{	
			if ( KEY_RIGHT == *key)
			{
				special_func_flag = 6;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}

		if (6 == special_func_flag)
		{	
			if ( KEY_LEFT_SHOTCUT == *key)
			{
				*key = KEY_EXT_9;
				return RUN_NEXT;
			}
			else
			{
				special_func_flag = 0;
			}
			return RUN_RERUN;
		}


	}
	return RUN_CONTINUE;
}



/**
* @brief	终端设置菜单
*/
static unsigned char dlgproc_terminal_setting(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}


static void refresh_current_select_item(unsigned char current_set,unsigned char refresh_flag)
{
	unsigned char i = 0;

	while (display_menu_item_array[i])
	{
		if (display_menu_item_array[i] == current_set)
		{
			gui_PictureOut(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,PIC_GOU,refresh_flag);
			//break;
		}
		else
		{
			gui_FillRect(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,24,13,BG_COLOR,refresh_flag);
		}
        i++;
	}
}

/**
* @brief 数据传输方式的选择
*/
static unsigned char dlgproc_data_transfer_func(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		//显示当前设置的数据传输模式
		refresh_current_select_item(g_param.transfer_mode,1);

		if (menu_current_item == 1 || menu_current_item == 4 || menu_current_item == 5)
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Option",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"选项",0,1);
			}
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ENTER == *key || KEY_SCAN == *key)
		{
			if (menu_current_item == 6 || menu_current_item == 7)
			{
				//导入商品信息
				*key = KEY_EXT_1 + menu_current_item - 1;
				saveDrawContext();
				return RUN_NEXT;
			}
			else
			{
				g_param.transfer_mode = menu_current_item;
				refresh_current_select_item(g_param.transfer_mode,1);
				device_init_by_setting();
				return RUN_RERUN;
			}
		}

		if (KEY_LEFT_SHOTCUT == *key)
		{
			if (menu_current_item == 1 || menu_current_item == 4 || menu_current_item == 5)
			{
				*key = KEY_EXT_1+menu_current_item - 1;
				saveDrawContext();
				return RUN_NEXT;
			}
			else
			{
				return RUN_RERUN;
			}
		}

		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			SaveTerminalPara();
			releaseDrawContext();
			return RUN_NEXT;
		}
	}

	if (DLGCB_PAGECHANGE == type)
	{
		refresh_current_select_item(g_param.transfer_mode,1);
		if (menu_current_item == 1 || menu_current_item == 4 || menu_current_item == 5)
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Option",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"选项",0,1);
			}
		}
		else
		{
			gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"    ",0,1);
		}
	}

	if (DLGCB_DESTORY == type)
	{
		//device_init_by_setting();
	}

	return RUN_CONTINUE;
}


//发送脱机数据的任务
unsigned int batch_data_send_proc(void)
{
	unsigned int cnt,i;
	TBATCH_NODE * pbatch_data;

	cnt = record_module_count(REC_TYPE_BATCH);
	for(i = 0; i < cnt;i++)
	{
		if (check_link_state(g_param.batch_transfer_mode) == 0)
		{
			break;
		}

		pbatch_data = (TBATCH_NODE*)record_module_read(REC_TYPE_BATCH,i+1);
		if (pbatch_data)
		{
			transfer_barcode_str(pbatch_data->batch_str,g_param.batch_transfer_mode);
		}

		//delete_one_node(REC_TYPE_BATCH,i+1);	//将该脱机记录删除，表示已发送
	}

	return i;
}

/**
* @brief 脱机数据传输
*/
static unsigned char dlgproc_batch_data_send(unsigned char type,unsigned char *key)
{
	unsigned int cnt;
	unsigned char str[21];

	if (DLGCB_DRAW == type)
	{
		if (check_link_state(g_param.batch_transfer_mode) == 0)
		{
			Beep(BEEP_DELAY*2);
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,26,"Check Device is Connected!",0,1);
			}
			else
			{
				gui_TextOut(0,26,"请确认设备处于连接状态!",0,1);
			}
		}
		else
		{
			if (record_module_count(REC_TYPE_BATCH) == 0)
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,26,"No Batch Data!",0,1);
				}
				else
				{
					gui_TextOut(0,26,"没有脱机数据",0,1);
				}
			}
			else
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,26,"Sending,Wait a minute",0,1);
				}
				else
				{
					gui_TextOut(0,26,"正在发送,请稍候...",0,1);
				}

				cnt = batch_data_send_proc();

				if (g_param.language == LANG_ENGLISH)
				{
					sprintf(str,"Send %d barcodes!",cnt);
				}
				else
				{
					sprintf(str,"发送了%d条数据!",cnt);
				}
				gui_TextOut(0,39,str,0,1);
			}
			
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 脱机数据传输方式的选择
*/
static unsigned char dlgproc_batch_transfer_mode(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		//显示当前设置的数据传输模式
		refresh_current_select_item(g_param.batch_transfer_mode,1);
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ENTER == *key || KEY_SCAN == *key)
		{

			g_param.batch_transfer_mode = menu_current_item;
			refresh_current_select_item(g_param.batch_transfer_mode,1);
			return RUN_RERUN;
		}

		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			SaveTerminalPara();
			return RUN_NEXT;
		}
	}

	//if (DLGCB_PAGECHANGE == type)
	//{
	//	refresh_current_select_item(g_param.batch_transfer_mode,1);
	//}

	if (DLGCB_DESTORY == type)
	{
		device_init_by_setting();
	}

	return RUN_CONTINUE;
}


/**
* @brief 脱机数据清除
*/
static unsigned char dlgproc_batch_data_clear(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,20,"Batch data will be delete,Continue?",0,1);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,20,"脱机数据将被清除",0,1);
			gui_TextOut_ext(CENTER_ALIGN,33,"是否继续?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if ((KEY_RIGHT_SHOTCUT == *key)&&(task_exit_flag == 0))
		{
			if(record_clear(REC_TYPE_BATCH))
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"Data delete fail!",0,1);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"数据清除失败!",0,1);
				}
			}
			else
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"Data delete success!",0,1);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"数据清除成功!",0,1);
				}
			}

			task_exit_flag = 1;
			OSTimeDlyHMSM(0,0,1,0);
			task_exit_flag = 0;
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}

	return RUN_CONTINUE;
}




/**
* @brief 清除扫描记录
*/
static unsigned char dlgproc_clear_scan_record(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,20,"Scan record will be delete,Continue?",0,1);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,20,"扫描记录将被清除",0,1);
			gui_TextOut_ext(CENTER_ALIGN,33,"是否继续?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if ((KEY_RIGHT_SHOTCUT == *key)&&(task_exit_flag == 0))
		{
			if(record_clear(REC_TYPE_CHECK))
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"Data delete fail!",0,1);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"数据清除失败!",0,1);
				}
			}
			else
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"Data delete success!",0,1);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"数据清除成功!",0,1);
				}
			}

			task_exit_flag = 1;
			OSTimeDlyHMSM(0,0,1,0);
			task_exit_flag = 0;
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 扫描模式选择
*/
static unsigned char dlgproc_scan_mode_menu(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		//显示当前设置的扫描模式
		refresh_current_select_item(g_param.scan_trig_mode,1);
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ENTER == *key || KEY_SCAN == *key)
		{
			g_param.scan_trig_mode = menu_current_item;
			refresh_current_select_item(g_param.scan_trig_mode,1);
		}

		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			SaveTerminalPara();
			return RUN_NEXT;
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 选项选择的通用处理函数
* 注意:选项必须是0,1,2，.....(0开始)
*/
static unsigned char proc_option_select(unsigned char type,unsigned char *key,unsigned char *param)
{
	if (DLGCB_DRAW == type)
	{
		//显示当前设置的选项
		refresh_current_select_item(*param+1,1);
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ENTER == *key || KEY_SCAN == *key)
		{
			*param = menu_current_item-1;
			refresh_current_select_item(*param+1,1);
		}

		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			if (param == &g_param.language)
			{
				gui_setlanguage(g_param.language);
			}
			else if (param == &g_param.avoid_dunplication_option)
			{
				if (g_param.avoid_dunplication_option)
				{
					//如果开启了防重码，那么就需要关闭数据统计功能和数据库查询功能
					g_param.database_query_option = 0;
					g_param.barcode_statistic_option = 0;
				}
			}
			else if (param == &g_param.database_query_option)
			{
				if (g_param.database_query_option)
				{
					//如果开启了数据库查询，那么就需要关闭数据统计功能和防重码功能
					g_param.avoid_dunplication_option = 0;
					g_param.barcode_statistic_option = 0;
				}
			}
			else if (param == &g_param.barcode_statistic_option)
			{
				if (g_param.barcode_statistic_option)
				{
					//如果开启了数据统计，那么就需要关闭防重码功能和数据库查询功能
					g_param.database_query_option = 0;
					g_param.avoid_dunplication_option = 0;
				}
			}
			SaveTerminalPara();
			return RUN_NEXT;
		}

		if (DLGCB_PAGECHANGE == type)
		{
			refresh_current_select_item(*param+1,1);
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 数据输入模式选择
*/
static unsigned char dlgproc_quantity_enter_menu(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.quantity_enter_mode);
}



/**
* @brief 添加时间选择
*/
static unsigned char dlgproc_add_time_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.add_time_option);
}

/**
* @brief 添加日期选择
*/
static unsigned char dlgproc_add_date_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.add_date_option);
}


/**
* @brief 手动输入条码
*/
static unsigned char dlgproc_input_barcode(unsigned char type,unsigned char *key)
{
	if (DLGCB_CREATE == type)
	{
		g_dlg.edit.pRes		= g_dlg.pRes->pEdit;
		edit_init(&g_dlg.edit);
		return RUN_EDIT_PROC;
	}

	if (DLGCB_BACK_FROM_VIRTUAL_KEYPAD == type)
	{
		if (KEY_ESC == *key)
		{
			memset(BarCode,0,31);
			return RUN_NEXT;
		}

		if (KEY_LEFT_SHOTCUT == *key)
		{
			strcpy(BarCode,g_editValue[0]);
			send_num = 1;
			strcpy(op_context.check_info_node.bar_code,BarCode);
			dlg_refresh_flag = 1;
			if (g_param.quantity_enter_mode)
			{
				scan_func_state = 2;
			}
			else
			{
				//if (g_param.transfer_confirm_option)
				//{
					scan_func_state = 1;
				//}
			}

			if (g_param.add_time_option || g_param.add_date_option)
			{
				GetDateTime();
				Systime2BCD(op_context.check_info_node.check_time);
			}

			if (g_param.database_query_option)
			{
				p_goods_node = (TGOODS_SPEC_RECORD*)rec_searchby_tag(REC_TYPE_GOODS_LIST,TAG_BARCODE,BarCode,&searched_rec_index);
				if (p_goods_node)
				{
					//在商品清单中搜索到该条码
					memcpy((void*)&op_context.goods_node,(void*)p_goods_node,sizeof(TGOODS_SPEC_RECORD));
					op_context.goods_info_index = searched_rec_index;

					op_context.check_info_node.spec_rec_index = op_context.goods_info_index;
					strcpy(line3_save,op_context.goods_node.name);
				}
				else
				{
					if(LANG_ENGLISH == g_param.language)
					{
						strcpy(line3_save,"**NO SPEC**");
					}
					else
					{
						strcpy(line3_save,"**无商品信息**");
					}
				}
			}


			*key = KEY_EXT_1;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}


/**
* @brief 设置分隔符
*/
static unsigned char dlgproc_seperator_setting(unsigned char type,unsigned char *key)
{
	if (DLGCB_CREATE == type)
	{
		g_dlg.edit.pRes		= g_dlg.pRes->pEdit;
		edit_init(&g_dlg.edit);
		edit_setvalue(&g_dlg.edit,g_param.transfer_seperator,0);
		edit_refresh_ext(&g_dlg.edit);
		return RUN_EDIT_PROC;
	}

	if (DLGCB_BACK_FROM_VIRTUAL_KEYPAD == type)
	{
		if (KEY_LEFT_SHOTCUT == *key)
		{
			strcpy(g_param.transfer_seperator,g_editValue[0]);
			SaveTerminalPara();
			*key = KEY_ESC;
			return RUN_NEXT;
		}

		if (KEY_ESC == *key)
		{
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}


/**
* @brief 修改发送数量
*/
static unsigned char dlgproc_input_send_num(unsigned char type,unsigned char *key)
{
	unsigned char str[11];

	if (DLGCB_CREATE == type)
	{
		g_dlg.edit.pRes		= g_dlg.pRes->pEdit;
		edit_init(&g_dlg.edit);
		edit_itoa(send_num,str);
		edit_setvalue(&g_dlg.edit,str,0);
		return RUN_EDIT_PROC;
	}

	if (DLGCB_BACK_FROM_VIRTUAL_KEYPAD == type)
	{
		if (KEY_ESC == *key)
		{
			return RUN_NEXT;
		}

		if (KEY_LEFT_SHOTCUT == *key)
		{
			send_num = edit_atoi(g_editValue[0]);
			*key = KEY_EXT_1;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}

/**
* @brief 确认修改发送数量
*/
static unsigned char dlgproc_modify_num_ack(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Modify the number?",0,1);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"确定修改数量?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_RIGHT_SHOTCUT == *key || KEY_ESC == *key)
		{
			*key = KEY_ESC;
			send_num = 1;
			dlg_refresh_flag = 1;
			return RUN_NEXT;
		}

		if (KEY_LEFT_SHOTCUT == *key || KEY_ENTER == *key)
		{
			*key = KEY_ENTER;
			dlg_refresh_flag = 1;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}


/**
* @brief 数据库查询选择
*/
static unsigned char dlgproc_database_query_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.database_query_option);
}

/**
* @brief 防重码功能开启选择
*/
static unsigned char dlgproc_avoid_dunplication_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.avoid_dunplication_option);
}


/**
* @brief 条码统计功能开启选择
*/
static unsigned char dlgproc_barcode_statistic_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.barcode_statistic_option);
}

/**
* @brief 传输确认功能开启选择
*/
static unsigned char dlgproc_transfer_confirm_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.transfer_confirm_option);
}

/**
* @brief 语言选择
*/
static unsigned char dlgproc_language_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.language);
}


/**
* @brief 按键音开关选择
*/
static unsigned char dlgproc_beeper_vol_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.beeper_vol);
}


/**
* @brief 超时选择
*/
static unsigned char dlgproc_timeout_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.operate_timeout);
}

/**
* @brief 蓝牙传输模式选项
*/
static unsigned char dlgproc_transfer_bluetooth_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.bluetooth_transfer_option);
}

/**
* @brief U盘传输模式选项
*/
static unsigned char dlgproc_transfer_u_disk_option(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}

/**
* @brief 脱机数据处理选项
*/
static unsigned char dlgproc_transfer_batch_option(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}


/**
***************************************************************************
*@brief 显示会话结果
*@param[in] 
*@return 
*@warning
*@see	
***************************************************************************
*/
static void display_hh_result(int trans_type,unsigned char key)
{
	gui_FillRect(0,ICON_HEIGHT+1,GUI_WIDTH,USERZONE_HEIGHT-1,BG_COLOR,0);

	if ((key == KEY_GPRS_S1)||(key == KEY_GPRS_S2))
	{
		if (LANG_ENGLISH == g_param.language)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Import Goods Info",0,0);
			//@English
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"商品信息导入",0,0);
		}

		if (LANG_ENGLISH == g_param.language)
		{
			if (key == KEY_GPRS_S2)
			{
				gui_TextOut_ext(CENTER_ALIGN,39,"Fail",0,0);
				//@English
			}
			else
			{
				gui_TextOut_ext(CENTER_ALIGN,39,"Success",0,0);
				//@English
			}
		}
		else
		{
			if (key == KEY_GPRS_S2)
			{
				gui_TextOut_ext(CENTER_ALIGN,39,"失败",0,0);
			}
			else
			{
				gui_TextOut_ext(CENTER_ALIGN,39,"成功",0,0);
			}
		}
	}
	else if (key == KEY_GPRS_S3)
	{
		if (LANG_ENGLISH == g_param.language)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"USB link disconnected",0,0);
			//@English
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"USB连接断开了",0,0);
		}
	}
	else if(key == KEY_GPRS_S4)
	{
		if (LANG_ENGLISH == g_param.language)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Command Error",0,0);
			//@English
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"命令数据域错误",0,0);
		}
	}
	else if(key == KEY_GPRS_S7)
	{
		if (LANG_ENGLISH == g_param.language)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Connect USB link...",0,0);
			//@English
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"请连接USB...",0,0);
		}
	}
	else if (key == KEY_GPRS_S6)
	{
		if (LANG_ENGLISH == g_param.language)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Command Timeout!",0,0);
			//@English
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"等待主机命令超时",0,0);
		}
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);

	OSTimeDlyHMSM(0,0,1,500);
}

void data_trans_proc_callback(void)
{
	//int		key;//command; 

	if (OP_CODE_CONNECT == op_code)
	{
		gui_FillRect(0,ICON_HEIGHT+1,GUI_WIDTH,USERZONE_HEIGHT-1,BG_COLOR,0);
		if (LANG_ENGLISH == g_param.language)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Download database...",0,0);
			//@English
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"正在下载数据库...",0,0);
		}

		gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	}

	return;
}
/**
***************************************************************************
*@brief 数据导入导出的后台任务
*@param[in] 
*@return 
*@warning
*@see	
*@note  此函数无法判断应用层所需要的实际功能是否执行成功，因为命令都是由PC
*		主动发起的，是否成功只有PC上的应用程序才知道，所以需要PC在最后的断
*		开命令中返回此次的会话结果。
***************************************************************************
*/
static int data_trans_task(int param1, int param2, void *param3, void *param4)
{
	int				command,ret;
	int				data_trans_start_flag = 0;
	unsigned int	last_state = 0xff;
	unsigned int	last_cmd_status = 0xff;
	unsigned int	delay_cnt;

	data_trans_state	= 0;
	while(1)
	{
		if(0 == task_read_command_from_ui(&command,0))
		{
			if (0 == data_trans_start_flag)
			{
				//还没有开始数据传输，用户按了取消键需要退出任务
				//如果已经开始与主机传输数据，不允许用户按取消键退出任务
				if (TASK_EXIT == command)
				{
					//用户按了取消键
					return KEY_EXT_1;
				}
			}
		}

		if (bDeviceState == CONFIGURED)
		{
			//终端实现的USB设备已经被PC枚举成功
			if (last_state != bDeviceState)
			{
				reset_command();						//USB中断开始接收PC发送下来的命令
			}

			////开始计时，检测是否命令超时
			if (last_cmd_status != g_pcCommand.status)
			{
				//StartDelay(200*120);		//2分钟的命令等待超时
				delay_cnt = 8000;
			}

			if (g_pcCommand.status)
			{
				//接收到主机发送下来的命令
				data_trans_start_flag = 1;		//已经开始数据传输了
				no_update_icon_flg = 1;
				ret = data_transfer_proc(g_pcCommand.CmdBuffer,param1,data_trans_proc_callback); 
				no_update_icon_flg = 0;
				update_icon_cnt = 0;
				if (0 == ret)
				{
					data_trans_state = 1;
					reset_command();				//准备接收下一个命令
					last_cmd_status = 0xff;
					continue;
				}
				else if(1 == ret)
				{
					//接收到主机发送的断开命令，并且会话成功
					return KEY_GPRS_S1;		//会话成功
				}
				else if (2 == ret)
				{
					//接收到主机发送的断开命令，但是会话失败
					return KEY_GPRS_S2;			//会话失败
				}
				else if (0x55aa == ret)
				{
					//与HOST的USB连接断开
					return KEY_GPRS_S3;		//USB连接断开了
				}
				else   //if (ret)
				{
					//命令数据有误，终端没有响应
					return KEY_GPRS_S4;		//命令数据有误
				}	
			}
			else
			{
				delay_cnt--;
				//还没有接收到PC发送过来的命令
				if ((last_cmd_status != g_pcCommand.status)&&(data_trans_start_flag != 1))
				{
					task_send_command_to_ui(KEY_GPRS_S5);	
				}

				//if (DelayIsEnd() == 0)
				if (delay_cnt == 0)
				{
					//等待主机命令超时
					return KEY_GPRS_S6;
				}
			}

			last_cmd_status = g_pcCommand.status;
			last_state = bDeviceState;
		}
		else
		{
			//终端还没有接入主机的USB接口或者还没有枚举成功
			if (last_state != bDeviceState)
			{
				if(data_trans_start_flag)
				{
					return KEY_GPRS_S3;		//任务返回，提示前台“USB连接被断开了”
				}
				else
				{
					task_send_command_to_ui(KEY_GPRS_S7);	//通知主机刷新界面显示
				}
			}
			last_state = bDeviceState;

			last_cmd_status = 0xff;
			USB_Cable_Config(0);
			Delay(5000);
			USB_Cable_Config(1);
			usb_cable_state = 1;
			OSTimeDlyHMSM(0,0,1,500);
		}
		OSTimeDlyHMSM(0,0,0,1);
	}
}


/**
***************************************************************************
*@brief 数据导入导出功能的回调函数
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
static unsigned char dlgproc_import_database(unsigned char type,unsigned char *key)
{
	if (DLGCB_CREATE == type)
	{
		g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_DUMMY_FAT;
		usb_device_init(USB_MASSSTORAGE);	//将设备初始化为一个Masstorage设备
		USB_Cable_Config(1);
		usb_cable_state = 1;
		vcom_device_state = 0;

		tmp_op_timeout = g_param.operate_timeout;
		g_param.operate_timeout = 0xFF;
		scan_start = 0;
	}

	if (DLGCB_DRAW == type)
	{
		dlg_run_bgtask(data_trans_task,0,0,0,0);
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			ui_send_command_to_task(TASK_EXIT);
			return RUN_RERUN;
		}

		if ((KEY_GPRS_S6 == *key) || (KEY_GPRS_S7 == *key) || (KEY_GPRS_S4 == *key) || (KEY_GPRS_S2 == *key) || (KEY_GPRS_S3 == *key))
		{
			display_hh_result(hh_type,*key);
			if (*key == KEY_GPRS_S7)
			{
				return RUN_RERUN;
			}
			*key = KEY_ESC;
			return RUN_NEXT;
		}
		else if (KEY_EXT_1 == *key)
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}
		else if (KEY_ENTER == *key)
		{
			return RUN_RERUN;
		}
		else if (KEY_GPRS_S5 == *key)
		{
			//刷新显示
			gui_FillRect(0,ICON_HEIGHT+1,GUI_WIDTH,USERZONE_HEIGHT-1,BG_COLOR,0);
			if (LANG_ENGLISH == g_param.language)
			{
				gui_TextOut_ext(CENTER_ALIGN,26,"Waiting command...",0,0);
				//@English
			}
			else
			{
				gui_TextOut_ext(CENTER_ALIGN,26,"等待主机命令...",0,0);
			}

			gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
			return RUN_RERUN;
		}
		else if (KEY_GPRS_S1 == *key)
		{
			display_hh_result(hh_type,KEY_GPRS_S1);
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}

	if (DLGCB_DESTORY == type)
	{
		g_param.operate_timeout = tmp_op_timeout;
		device_init_by_setting();
	}

	return RUN_CONTINUE;
}


/**
* @brief 条码设置选项
*/
static unsigned char dlgproc_barcode_option(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}

/**
* @brief 时间日期设置菜单
*/
static unsigned char dlgproc_time_date_setting(unsigned char type,unsigned char *key)
{
	unsigned char input_time[15];

	if (DLGCB_RUN == type)
	{
		if (KEY_LEFT_SHOTCUT == *key)
		{
			return RUN_EDIT_PROC;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			// 取得编辑框值
			if(g_dlg.edit.now[0] != g_dlg.edit.input_max[0])
			{
				return RUN_RERUN;
			}

			//键值转换为字符串
			edit_etos(g_editValue[0],input_time,g_dlg.edit.now[0]);
			/** @note 在此检查输入的时间是否不符合格式		*/
			if(check_time(input_time) == 0)
			{
				SetDateTime();
				*key = KEY_ESC;
				return					RUN_NEXT;
			}
			gui_FillRect(0,ICON_HEIGHT+1,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,0);
			if (LANG_ENGLISH == g_param.language)
			{
				gui_TextOut_ext(CENTER_ALIGN,13,"Invalid Time!",0,0);
				gui_TextOut_ext(CENTER_ALIGN,26,"Re-input!",0,0);
				//@English
			}
			else
			{
				gui_TextOut_ext(CENTER_ALIGN,13,"输入时间不合法!",0,0);
				gui_TextOut_ext(CENTER_ALIGN,16,"请重新输入",0,0);
			}
                        gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
			task_exit_flag = 1;
			OSTimeDlyHMSM(0,0,1,500);
			task_exit_flag = 0;
			return RUN_RETURN;
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 恢复默认设置
*/
static unsigned char dlgproc_default_setting(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,13,"The operate will clear all the data,Continue?",0,1);
		}
		else
		{
			gui_TextOut(0,13,"恢复默认设置将清除所有数据,是否继续?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_LEFT_SHOTCUT == *key || KEY_ESC == *key)
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			record_clear(REC_TYPE_CHECK);
			record_clear(REC_TYPE_BATCH);
			DefaultTerminalPara();

			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 设置系统密码
*/
static unsigned char dlgproc_set_system_password(unsigned char type,unsigned char *key)
{
	//todo.....

	return RUN_CONTINUE;
}

//刷新当前显示的解码器开关的开关情况
static void refresh_decoder_switch_map(unsigned char mode,unsigned char refresh_flag)
{
	unsigned char i = 0;

	while (display_menu_item_array[i])
	{
		if (mode)
		{
			if (display_menu_item_array[i] == menu_current_item)
			{
				if (g_param.decoder_switch_map & (0x0001 << (display_menu_item_array[i] - 1)))
				{
					gui_PictureOut(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,PIC_GOU,1);
				}
				else
				{
					gui_FillRect(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,24,13,BG_COLOR,1);
				}
                                break;
			}
		}
		else
		{
			if (g_param.decoder_switch_map & (0x0001 << (display_menu_item_array[i] - 1)))
			{
				gui_PictureOut(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,PIC_GOU,1);
			}
			else
			{
				gui_FillRect(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,24,13,BG_COLOR,1);
			}
		}
		i++;
	}
}

/**
* @brief 设置解码器
*/
static unsigned char dlgproc_decoder_setting(unsigned char type,unsigned char *key)
{	
	static unsigned short map_save;

	if (DLGCB_DRAW == type)
	{
		//显示当前设置的解码器选项
		refresh_decoder_switch_map(0,1);
		map_save = g_param.decoder_switch_map;
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ENTER == *key || KEY_SCAN == *key)
		{
			g_param.decoder_switch_map ^= (0x0001 << menu_current_item - 1);
			refresh_decoder_switch_map(1,1);
			return RUN_RERUN;
		}
	}

	if (DLGCB_PAGECHANGE == type)
	{
		refresh_decoder_switch_map(0,1);
	}

	if (DLGCB_DESTORY == type)
	{
		if (map_save != g_param.decoder_switch_map)
		{
			SaveTerminalPara();
			UE988_set_decoder_switch(g_param.decoder_switch_map);
		}	
	}

	return RUN_CONTINUE;
}

/**
* @brief 设置解码有效位数
*/
static unsigned char dlgproc_invalid_offset_setting(unsigned char type,unsigned char *key)
{
	unsigned char str[4];

	if (DLGCB_CREATE == type)
	{
		valid_barcode_start_offset_tmp = g_param.valid_barcode_start_offset;
		valid_barcode_end_offset_tmp   = g_param.valid_barcode_end_offset;

	}
	if (DLGCB_DRAW == type)
	{
		if ((g_param.valid_barcode_start_offset != 0xff)||(g_param.valid_barcode_end_offset != 0xff))
		{
			edit_itoa(valid_barcode_start_offset_tmp,str);
			edit_setvalue(&g_dlg.edit,str,0);	

			edit_itoa(valid_barcode_end_offset_tmp,str);
			edit_setvalue(&g_dlg.edit,str,1);	
		}
	}

	if ((DLGCB_RUN == type)&&(task_exit_flag == 0))
	{
		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			if ((g_dlg.edit.now[0] == 0) && (g_dlg.edit.now[1] == 0))
			{
				g_param.valid_barcode_end_offset	= 0xff;
				g_param.valid_barcode_start_offset  = 0xff;
			}
			else
			{
				if((g_dlg.edit.now[0] == 0) || (g_dlg.edit.now[1] == 0))
				{
					return RUN_RERUN;
				}
				else
				{
					valid_barcode_start_offset_tmp =   edit_atoi(g_editValue[0]);
					valid_barcode_end_offset_tmp   =   edit_atoi(g_editValue[1]);

					if ((valid_barcode_end_offset_tmp > 30) || (valid_barcode_start_offset_tmp > 30)
						|| (valid_barcode_end_offset_tmp <= valid_barcode_start_offset_tmp)
						|| (valid_barcode_start_offset_tmp < 1) )
					{
						gui_FillRect(0,ICON_HEIGHT+1,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,1);	

						if (LANG_ENGLISH == g_param.language)
						{
							gui_TextOut_ext(CENTER_ALIGN,13,"Invalid setting",0,1);
							gui_TextOut_ext(CENTER_ALIGN,26,"Re-input!",0,1);
							//@English
						}
						else
						{
							gui_TextOut_ext(CENTER_ALIGN,13,"设置无效",0,1);
							gui_TextOut_ext(CENTER_ALIGN,26,"请重新输入!",0,1);
						}
						task_exit_flag = 1;
						OSTimeDlyHMSM(0,0,1,0);
						task_exit_flag = 0;
						return RUN_RETURN;
					}
					g_param.valid_barcode_start_offset = valid_barcode_start_offset_tmp;
					g_param.valid_barcode_end_offset   = valid_barcode_end_offset_tmp;

				}

			}
			*key = KEY_ESC;
			SaveTerminalPara();
			return RUN_NEXT;
		}

		if (KEY_LEFT_SHOTCUT == *key)
		{
			return RUN_EDIT_PROC;
		}

	}
	return RUN_CONTINUE;
}


/**
* @brief 查看内存信息
*/
static unsigned char dlgproc_view_memery(unsigned char type,unsigned char *key)
{
	unsigned int cnt;
	unsigned char str[51];
	FATFS		*fs;

	if (DLGCB_DRAW == type)
	{
		if(f_getfree("/",&cnt,&fs) == FR_OK)
		{
			if (g_param.language == LANG_ENGLISH)
			{
				sprintf(str,"Remain %d free bytes,used %d bytes",cnt*512,8*1024*1024-(cnt*512));
			}
			else
			{
				sprintf(str,"剩余%d可用字节,已占用%d字节",cnt*512,8*1024*1024-(cnt*512));
			}

			gui_TextOut(0,13,str,0,1);
		}
	}
	return RUN_CONTINUE;
}


/**
* @brief 查看蓝牙信息
*/
static unsigned char dlgproc_bluetooth_info(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,13,"KeyboardName:LSBTK001",0,1);
			gui_TextOut(0,26,"ProtocolVer:V2.0",0,1);
		}
		else
		{
			gui_TextOut(0,13,"蓝牙键盘名称:LSBTK001",0,1);
			gui_TextOut(0,26,"蓝牙协议版本:V2.0",0,1);
		}
	}
	return RUN_CONTINUE;
}

/**
* @brief 查看固件信息
*/
static unsigned char dlgproc_firmware_info(unsigned char type,unsigned char *key)
{
	unsigned char str[21];

	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,13,"Firmware:V1.0.0",0,1);
			strcpy(str,"App Ver:");
			strcat(str,app_ver);
			gui_TextOut(0,26,str,0,1);
			gui_TextOut(0,39,"Scanner:uE_2.3_1.5.29",0,1);
		}
		else
		{
			gui_TextOut(0,13,"固件版本:V1.0.0",0,1);
			strcpy(str,"应用版本:");
			strcat(str,app_ver);
			gui_TextOut(0,26,str,0,1);
			gui_TextOut(0,39,"扫描头:uE_2.3_1.5.29",0,1);
		}
	}
	return RUN_CONTINUE;
}


/**
* @brief 创建新文件
*/
static unsigned char dlgproc_create_new_file(unsigned char type,unsigned char *key)
{
	unsigned char str[13];

	if (DLGCB_RUN == type)
	{
		if ((KEY_LEFT_SHOTCUT == *key)&&(task_exit_flag == 0))
		{
			return RUN_EDIT_PROC;
		}

		if ((KEY_ENTER == *key || KEY_SCAN == *key)&&(task_exit_flag == 0))
		{
			if(g_dlg.edit.now[0] == 0)
			{
				return RUN_RERUN;
			}
			
			strcpy(str,g_editValue[0]);
			strcat(str,".txt");
			
			gui_FillRect(0,13,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,0);

			if (f_open(&file2,(const unsigned char*)str,FA_CREATE_NEW) != FR_OK)
			{
				
				Beep(BEEP_DELAY*2);
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"Create file fail!",0,0);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"文件创建失败!",0,0);
				}
			}
			else
			{
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"Create file success!",0,0);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN,26,"文件创建成功!",0,0);
				}
				f_close(&file2);
			}
			gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
			task_exit_flag = 1;
			OSTimeDlyHMSM(0,0,1,0);
			task_exit_flag = 0;
			*key = KEY_ENTER;
			return RUN_NEXT;
		}

		if ((KEY_RIGHT_SHOTCUT == *key || KEY_ESC == *key)&&(task_exit_flag == 0))
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}

	if (DLGCB_BACK_FROM_VIRTUAL_KEYPAD == type)
	{
		if (KEY_LEFT_SHOTCUT == *key)
		{
			//检查是否包含了非法字符
			strcpy(str,g_editValue[0]);
			if ((strstr(str,"*") != NULL)||(strstr(str,"\\") != NULL)||(strstr(str,"|") != NULL)||(strstr(str,"\"") != NULL)\
                            ||(strstr(str,"/") != NULL)||(strstr(str,"?") != NULL)||(strstr(str,">") != NULL)||(strstr(str,"<") != NULL)\
                             ||(strstr(str,".") != NULL))
			{
				memset(g_editValue[0],0,MAX_EDITNUM);
				gui_FillRect(0,0,GUI_WIDTH,LCD_HEIGHT,BG_COLOR,0);
				Beep(BEEP_DELAY*2);
				gui_TextOut(0,26,"不允许包含非法字符!",0,0);
				gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
				OSTimeDlyHMSM(0,0,1,0);
				gui_TextOut(0,26,"                    ",0,1);
				*key = KEY_EXT_1;
				return RUN_NEXT;
			}
		}
	}

	return RUN_CONTINUE;
}

//扫描根目录下的TXT文件,最多支持20个TXT文件
static unsigned char  scan_files(unsigned char *pfn_List)
{
	FRESULT res;
	FILINFO fno;
	DIR dir;
	unsigned char i = 0;

	res = f_opendir(&dir, "/");
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) break;
			if (fno.fname[0] == '.') continue;
			if (strstr(fno.fname,".txt") == NULL && strstr(fno.fname,".TXT") == NULL)	continue;
			if (fno.fattrib & AM_DIR) continue;
			
			strcpy(pfn_List+13*i,fno.fname);
			i++;

			if (i == 21)
			{
				break;
			}
		}
	}
	return i;
}

static unsigned char current_select_file[13];

//刷新显示文件列表，返回当前选择的文件索引
static void refresh_file_list(unsigned char *plist,unsigned char list_cnt,unsigned char key)
{
	static unsigned char start_display_offset;
	static unsigned char current_offset;
	unsigned char i;

	if (list_cnt == 0)
	{
		return;
	}

	if (key == 0)
	{
		start_display_offset = 0;
		current_offset = 0;
	}
	else if( KEY_DOWN == key)
	{	
		// 下翻页
		if (current_offset == (list_cnt-1))
		{
			current_offset = 0;
			start_display_offset = 0;
		}
		else
		{
			current_offset++;
			if (current_offset == (start_display_offset+3))
			{
				start_display_offset ++;
			}
		}
	}
	else if(KEY_UP == key)
	{	// 上翻页
		if (current_offset == 0)
		{
			current_offset = list_cnt-1;
			start_display_offset = list_cnt - 3;
		}
		else
		{
			current_offset--;
			if (current_offset < start_display_offset)
			{
				start_display_offset--;
			}
		}
	}
	else if (KEY_ENTER == key || KEY_SCAN == key)
	{
		strcpy(g_param.u_disk_storage_file,plist+(start_display_offset+current_offset)*13);
	}

	gui_FillRect(0,13,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,0);
	for (i = 0; i < 3;i++)
	{
		if ((start_display_offset+i) == list_cnt)
		{
			break;
		}

		if (current_offset == start_display_offset+i)
		{
			gui_SetTextColor(BG_COLOR);
			strcpy(current_select_file,plist+(start_display_offset+current_offset)*13);
		}
		else
		{
			gui_SetTextColor(TXT_COLOR);
		}

		gui_TextOut(0,13+13*i,plist+(start_display_offset+i)*13,0,0);

		if (strcmp(g_param.u_disk_storage_file,plist+(start_display_offset+i)*13) == 0)
		{
			gui_PictureOut(GUI_WIDTH - 24,ICON_HEIGHT+1+i*13,PIC_GOU,0);
		}
	}
	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	gui_SetTextColor(TXT_COLOR);
}
/**
* @brief 显示文件列表
*/
static unsigned char dlgproc_file_list(unsigned char type,unsigned char *key)
{
	static unsigned char *pList;
	static unsigned char file_cnt = 0;

	if (DLGCB_CREATE == type)
	{
		pList = (unsigned char*)Jmalloc(13*21);
		if (pList)
		{
			file_cnt = scan_files(pList);
		}
	}

	if (DLGCB_DRAW == type)
	{
		refresh_file_list(pList,file_cnt,0);
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_UP == *key || KEY_DOWN == *key || KEY_ENTER == *key || KEY_SCAN == *key)
		{
			refresh_file_list(pList,file_cnt,*key);
			return RUN_RERUN;
		}

		if (KEY_ESC == *key || KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			SaveTerminalPara();
			return RUN_NEXT;
		}

		if (KEY_LEFT_SHOTCUT == *key)
		{
			if (file_cnt == 0)
			{
				return RUN_RERUN;
			}

			return RUN_NEXT;
		}
	}

	if (DLGCB_DESTORY == type)
	{
		Jfree(pList);
	}

	return RUN_CONTINUE;
}


/**
* @brief 格式化U DISK
*/
static unsigned char dlgproc_u_disk_format(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,13,"All the data will be cleared,Continue？",0,1);
		}
		else
		{
			gui_TextOut(0,13,"所有数据将丢失并不可恢复,是否继续?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_LEFT_SHOTCUT == *key)
		{	
			gui_FillRect(0,13,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,1);
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,26,"Formatting...",0,1);
			}
			else
			{
				gui_TextOut(0,26,"正在格式化,请稍候...",0,1);
			}
			f_mkfs(0,1,512);

			*key = KEY_ENTER;
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}

/**
* @brief 文件操作选项
*/
static unsigned char dlgproc_file_options_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}
/**
* @brief 清空文件
*/
static unsigned char dlgproc_clear_file_ack(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,26,"Clear this file?",0,1);
		}
		else
		{
			gui_TextOut(0,26,"确定清空此文件?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_LEFT_SHOTCUT == *key)
		{	
			if (f_open(&file2,current_select_file,FA_OPEN_EXISTING | FA_WRITE) != FR_OK)
			{
				gui_TextOut(0,26,"                    ",0,1);
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,26,"Clear fail!",0,1);
				}
				else
				{
					gui_TextOut(0,26,"文件清空失败!",0,1);
				}
				Beep(BEEP_DELAY*2);
			}

			f_lseek(&file2,0);

			f_truncate(&file2);

			f_close(&file2);
                        *key = KEY_ENTER;
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}
/**
* @brief 删除文件
*/
static unsigned char dlgproc_delete_file_ack(unsigned char type,unsigned char *key)
{

	if (DLGCB_DRAW == type)
	{
		if(strcmp(g_param.u_disk_storage_file,current_select_file) == 0)
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,26,"Can't Del target file",0,1);
			}
			else
			{
				gui_TextOut(0,26,"不能删除目标存储文件",0,1);
			}
			Beep(BEEP_DELAY*2);
			OSTimeDlyHMSM(0,0,1,0);

			*key = KEY_ESC;
			return RUN_NEXT;
		}

		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,26,"Delete this file?",0,1);
		}
		else
		{
			gui_TextOut(0,26,"确定删除此文件?",0,1);
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_LEFT_SHOTCUT == *key)
		{	
			if (f_unlink(current_select_file) != FR_OK)
			{
				gui_TextOut(0,26,"                    ",0,1);
				if (g_param.language == LANG_ENGLISH)
				{
					gui_TextOut(0,26,"Delete fail!",0,1);
				}
				else
				{
					gui_TextOut(0,26,"文件删除失败!",0,1);
				}
				Beep(BEEP_DELAY*2);
			}
                        *key = KEY_ENTER;
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
}

/**
* @brief 初始化应用环境
*/
static void app_init(void)
{
	dlg_refresh_flag = 0;
	no_update_icon_flg = 0;
	need_refresh_signal = 0;
	current_home_item = 1;
	menu_current_item = 1;
	menu_start = 1;
	memset(BarCode,0,31);
	//memset((void*)draw_content,0,MAX_CASH_ITEM*sizeof(TDRAW_ITEM));	//将缓存的内容清掉

	if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	{
		bluetooth_match_cmd_cnt = 0;
		bluetooth_state_cnt = 0;
	}

	InitDrawContext();
}

void task_ui(void* pp)
{
	app_init();
	//gui_clear(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_setlanguage(g_param.language);
	max_dlg				= DLG_MAX;
	next_dlg_id			= DLG_logo;	
	dlg_loop();
}

//根据设置对设备的状态进行初始化
void device_init_by_setting(void)
{
	if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	{
		USB_Cable_Config(0);		//断开USB设备的连接
		YFBT07_power_ctrl(1);		//开启蓝牙模块的电源
		bluetooth_match_cmd_cnt = 0;
		led_g_ctrl(1);
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_KEYBOARD)
	{
		USB_Cable_Config(1);
		usb_device_init(USB_KEYBOARD);
		YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
		led_g_ctrl(0);
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	{
		USB_Cable_Config(1);
		usb_device_init(USB_VIRTUAL_PORT);
		YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
		led_g_ctrl(0);
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_U_DISK)
	{
		YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
		led_g_ctrl(0);
		g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
		usb_device_init(USB_MASSSTORAGE);
		USB_Cable_Config(1);
	}
	else
	{
		if (g_param.batch_transfer_mode == TRANSFER_MODE_BLUETOOTH)
		{
			USB_Cable_Config(0);		//断开USB设备的连接
			YFBT07_power_ctrl(1);		//开启蓝牙模块的电源
			bluetooth_match_cmd_cnt = 0;
			led_g_ctrl(1);
		}
		else if (g_param.batch_transfer_mode == TRANSFER_MODE_KEYBOARD)
		{
			usb_device_init(USB_KEYBOARD);
			YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
			led_g_ctrl(0);
			USB_Cable_Config(1);
		}
		else
		{
			USB_Cable_Config(1);
			usb_device_init(USB_VIRTUAL_PORT);
			YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
			led_g_ctrl(0);
		}
	}
}

void display_copyright_info(unsigned char state)
{
	unsigned char *str[6]={
		"\xe8\xa0\x9c\xed\x99\xa1\xe9\x9c\xf9\xe0\x90\xa7\xe4\x87\xe8\x93\xe6\xf5\x9c\xad",	//产品名称：数据采集器
		"\xE8\xA0\x9C\xED\x8A\x97\xE0\x9F\xF9\xE0\x0E\x6C",									//产品型号：T6
		"\x93\xA0\xE8\xA0\xE9\xFD\x93\x96\xF9\xE0\x93\xB4\x81\x80\x90\x8a\xe7\xf7\xe8\xf2\x9b\xa0\xef\xbd\x8d\x89\x89\x8a\x95\x84\xe3\xf1\x91\xe4",	//生产厂商：深圳市江波龙电子有限公司
		"\xEF\xBD\xE1\xEA\xF9\xE0\x6A\x6D\x6F\x6F\x77\x69\x68\x62\x69\x6B\x68\x69\x63",		//电话：0755-32831239
		"\x92\xb7\xe6\xa4\x8c\xa2\x8d\xad\x92\xf2\xea\xbc\x92\xf2\x91\xa3\x89\x8a",	//软件著作权版权所有
		"\x8b\x95\xe7\xa1\xef\x9b\xea\xbc\xf9\xf6\x94\xff\xed\xf2\xeb\x82\xe4\xe5\xf9\xfb",//严禁盗版，违法必究！
	};

	unsigned char i,buffer[41];

	gui_clear(0);
	if (state == 0)
	{	
		memset(buffer,0,41);
		for (i = 0;i < strlen(str[0]);i++)
		{
			buffer[i] = (*(str[0]+i))^0x5a;
		}
		gui_TextOut(0,0,buffer,0,0);

		memset(buffer,0,41);
		for (i = 0;i < strlen(str[1]);i++)
		{
			buffer[i] = (*(str[1]+i))^0x5a;
		}
		gui_TextOut(0,13,buffer,0,0);

		memset(buffer,0,41);
		for (i = 0;i < strlen(str[2]);i++)
		{
			buffer[i] = (*(str[2]+i))^0x5a;
		}
		gui_TextOut(0,26,buffer,0,0);

		memset(buffer,0,41);
		for (i = 0;i < strlen(str[3]);i++)
		{
			buffer[i] = (*(str[3]+i))^0x5a;
		}
		gui_TextOut(0,52,buffer,0,0);
	}

	if (state == 1)
	{
		memset(buffer,0,41);
		for (i = 0;i < strlen(str[4]);i++)
		{
			buffer[i] = (*(str[4]+i))^0x5a;
		}
		gui_TextOut(0,0,buffer,0,0);

		memset(buffer,0,41);
		for (i = 0;i < strlen(str[5]);i++)
		{
			buffer[i] = (*(str[5]+i))^0x5a;
		}
		gui_TextOut(0,13,buffer,0,0);
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
}

/**
* @brief 主界面回调函数，task_ui第一个运行起来的界面回调函数
*/
static unsigned char dlgproc_about_copyright(unsigned char type,unsigned char *key)
{
	static unsigned char dlg_state = 0;

	if (DLGCB_DRAW == type)
	{
		display_copyright_info(dlg_state);
	}

	if (DLGCB_RUN == type)
	{
		if ((*key == KEY_UP)&&(dlg_state == 1))
		{
			dlg_state = 0;
			display_copyright_info(dlg_state);
			return RUN_RERUN;
		}

		if ((*key == KEY_DOWN)&&(dlg_state == 0))
		{
			dlg_state = 1;
			display_copyright_info(dlg_state);
			return RUN_RERUN;
		}
	}

	return RUN_CONTINUE;
}


//测试任务
static void enter_test_task(unsigned char test_type)
{
	unsigned int  i,success_cnt,fail_cnt;
	unsigned char *key = NULL;
	unsigned char code_value[31];
	unsigned char str[11];
	unsigned char transfer_mode_save;


	//OSSchedLock();	//不要再进行任务调度
	gui_clear(0);	//清屏

	if (test_type == 1)
	{
		gui_TextOut(0,0,"条码(S):",0,1);
		gui_TextOut(0,26,"成功次数:",0,1);
		gui_TextOut(0,39,"错误次数:",0,1);
		success_cnt = fail_cnt = 0;
		memset(code_value,0,31);
	}
	else if (test_type == 2)
	{
		//将蓝牙模块强制打开
		transfer_mode_save = g_param.transfer_mode;
		g_param.transfer_mode = TRANSFER_MODE_BLUETOOTH;
		device_init_by_setting();
		gui_TextOut_ext(CENTER_ALIGN,13,"蓝牙模块已开启",0,1);
		gui_TextOut_ext(CENTER_ALIGN,26,"等待主机连接中...",0,1);
		success_cnt = 2;
		fail_cnt = 0x20;
		i = 1;
	}
	else if (test_type == 3)
	{
		if(check_link_state(g_param.transfer_mode) == 0)
		{
			gui_TextOut_ext(CENTER_ALIGN,13,"确认是否已连接主机?",0,1);
		}
		else
		{
			gui_TextOut(0,0,"条码(S):",0,1);
			gui_TextOut(0,26,"扫描次数:",0,1);
			gui_TextOut(0,39,"传输次数:",0,1);
		}
		success_cnt = fail_cnt = 0;
	}

	while (1)
	{
		if (test_type == 1)
		{
			memset(BarCode,0,31);
			memset(BarCode_Type,0,20);
			BarCode_Len = 0;
			if (UE988_get_barcode(BarCode_Type, BarCode,30, &BarCode_Len) == 0)
			{
				//获取到条码
				//led_r_ctrl(1);
				Delay(5000);
				//Beep(BEEP_DELAY);
				//led_r_ctrl(0);

				gui_TextOut(48, 0,  "                                ", 0,0);
				if (strlen(BarCode) > 20)
				{
					gui_TextOut(48, 0,  BarCode, 0,0);
				}
				else
				{
					gui_TextOut_ext(CENTER_ALIGN, 13,  BarCode, 0,0);
				}

				if ((success_cnt == 0) && (fail_cnt == 0))
				{
					strcpy(code_value,BarCode);
					success_cnt++;
				}
				else
				{
					if (strcmp(code_value,BarCode))
					{
						fail_cnt++;
					}
					else
					{
						success_cnt++;
					}
				}

				sprintf((char*)str,"%d",success_cnt);
				gui_TextOut(9*6, 26,str, 0,0);
				sprintf((char*)str,"%d",fail_cnt);
				gui_TextOut(9*6, 39,str,0,0);
			}
		}
		else if (test_type == 2)
		{
			if (YFBT07_Connect_state())
			{	
				//连接状态
				bluetooth_match_cmd_cnt = 0;
				gui_TextOut(0,0,"连接成功!",0,1);
				if (i == 1)
				{
					gui_TextOut(0,13,"按[scan]进行数据传输",0,1);
					while(1)
					{
						key = keypad_getkey();
						if((key) && (KEY_SCAN == *key)) 
						{
							Beep(BEEP_DELAY);
							break;
						}
					}
				}

				gui_TextOut(0,13,"正在进行数据传输...",0,1);
				i = 2;
				memset(code_value,0,31);
				for (i = 0; i < success_cnt;i++)
				{
					if (fail_cnt > 0x7e)
					{
						fail_cnt = 0x20;
					}
					code_value[i] = fail_cnt++;
				}
				success_cnt++;
				if (success_cnt > 30)
				{
					success_cnt = 2;
				}

				
				YFBT07_SendKey(code_value,strlen(code_value));
				YFBT07_SendKey("\x0d",1);

				gui_TextOut(0,26+(success_cnt%3)*13,"                    ",0,0);
				gui_TextOut(0,26+(success_cnt%3)*13,code_value,0,0);

			}
			else 
			{
				//如果是处于未连接状态，那么就间隔2分钟发送一次匹配命令
				if(i == 2)
				{
					gui_TextOut(0,0,"连接断开!",0,0);
					gui_TextOut(0,13,"数据传输被中断!     ",0,0);
				}
				
				if (bluetooth_module_state == 0)
				{
					bluetooth_state_cnt = 0;
					if (bluetooth_match_cmd_cnt == 600)
					{
						bluetooth_match_cmd_cnt = 0;
						YFBT07_Enter_Match_Mode();
					}
					if (bluetooth_match_cmd_cnt%80 == 0)
					{
						YFBT07_SendKey("\x0d",1);
					}
					bluetooth_match_cmd_cnt ++;
				}
				else
				{
					if(bluetooth_state_cnt == 200)
					{
						bluetooth_state_cnt = 0;
						bluetooth_module_state = YFBT07_check();
					}
					bluetooth_state_cnt++;
				}
			}
		}
		else if (test_type == 3)
		{
			if(check_link_state(g_param.transfer_mode) == 0)
			{
				gui_FillRect(0,0,LCD_WIDTH,13*4,0,0);
				gui_TextOut_ext(CENTER_ALIGN,26,"连接中断!",0,0);
			}
			else
			{
				memset(BarCode,0,31);
				memset(BarCode_Type,0,20);
				BarCode_Len = 0;
				if (UE988_get_barcode(BarCode_Type, BarCode,30, &BarCode_Len) == 0)
				{
					//获取到条码
					//led_r_ctrl(1);
					Delay(5000);
					//Beep(BEEP_DELAY);
					//led_r_ctrl(0);

					gui_TextOut(48, 0,  "                                ", 0,0);
					if (strlen(BarCode) > 20)
					{
						gui_TextOut(48, 0,  BarCode, 0,0);
					}
					else
					{
						gui_TextOut_ext(CENTER_ALIGN, 13,  BarCode, 0,0);
					}

					success_cnt++;
					transfer_barcode_str(BarCode,g_param.transfer_mode);
					transfer_barcode_str("\x0d",g_param.transfer_mode);
					fail_cnt++;

					sprintf((char*)str,"%d",success_cnt);
					gui_TextOut(9*6, 26,str, 0,0);
					sprintf((char*)str,"%d",fail_cnt);
					gui_TextOut(9*6, 39,str,0,0);

				}
			}
		}

		gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);

		key = keypad_getkey();
		if((key) && (KEY_ESC == *key)) 
		{
			Beep(BEEP_DELAY);
			break;
		}

		//Delay(400000);	//约200ms
		OSTimeDlyHMSM(0,0,0,200);
	}

	if (test_type == 2)
	{
		g_param.transfer_mode = transfer_mode_save;
		device_init_by_setting();
	}
	//OSSchedUnlock();
}

/**
* @brief 测试菜单界面
*/
static unsigned char dlgproc_test_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}


/**
* @brief 老化测试界面
*/
static unsigned char dlgproc_age_test_menu(unsigned char type,unsigned char *key)
{
	unsigned char test_type;

	if (DLGCB_RUN == type)
	{
		if (KEY_SCAN == *key || KEY_ENTER == *key)
		{
			//menu_current_item = 1 进入扫描头老化测试任务
			//menu_current_item = 2 进入蓝牙传输老化测试任务
			//menu_current_item = 3 进入扫描+传输老化测试任务
			test_type = menu_current_item;
			enter_test_task(test_type);
			*key = KEY_EXT_1;
			return RUN_NEXT;
		}

		if (KEY_LEFT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			return RUN_NEXT;
		}

	}
	return RUN_CONTINUE;
}

/**
* @brief 单项测试菜单界面
*/
static unsigned char dlgproc_single_test_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}


/*
*
*
*/
int  GetPowerVoltage_test_mode(void)
{
	unsigned int  i,result = 0;
	unsigned short  temp[20];
	unsigned short	min,max;

	for(i = 0;i < 20;i++)
	{
		temp[i] = ADC_GetConversionValue(ADC1);		//得到AD转换的结果
		result += temp[i];
		if (i == 0)
		{
			min = temp[i];
			max = temp[i];
		}

		if (temp[i] < min)
		{
			min = temp[i];
		}

		if (temp[i] > max)
		{
			max = temp[i];
		}
	}

	//取20次值之后,去掉最小值和最大周，再取平均值，简单的平滑滤波
	result -= min;
	result -= max;
	result /= 18; 

	return result;

}

/*
*@brief  LCD 背光及LED测试控制测试
*/
void test_check_key(void)
{
	unsigned char *key = NULL;
	volatile unsigned int  delay_cnt;    
	key = keypad_getkey();
	for(delay_cnt = 0;delay_cnt < 20000*190;delay_cnt++);
	while (1) 
	{
		for(delay_cnt = 0;delay_cnt < 20000*10;delay_cnt++);
		//OSTimeDlyHMSM(0,0,0,200);
		key = keypad_getkey();
		if ((key) && ((KEY_ENTER == (*key))||(KEY_RIGHT_SHOTCUT == (*key))||(KEY_SCAN == (*key)))) 
		{
			Beep(BEEP_DELAY);
			break;
		}
	}
}

/*
*@brief  LCD 背光及LED测试控制测试
*/
static void test_LCD_display(void)
{
	OSSchedLock();
	Lcd_Clear(0xffff);
	//test_check_key();
	//Lcd_Clear(0);
	//test_check_key();
	//displaycom();
	//test_check_key();
	//displaycom1();
	//test_check_key();
	//displayseg();
	//test_check_key();
	//displayseg1();
	test_check_key();
	displaysnow();
	test_check_key();
	Lcd_Clear(0);
	OSSchedUnlock();
}

/**
* @brief  蜂鸣器测试
*/
#if 0
static void testbuzzer(void)
{
	Beep(6*BEEP_DELAY);
	OSTimeDlyHMSM(0,0,1,0);
	Beep(BEEP_DELAY);
	OSTimeDlyHMSM(0,0,1,0);
	Beep(BEEP_DELAY);
	OSTimeDlyHMSM(0,0,1,0);
}
#endif



/*
*@brief: 按键测试
*/
static void testKeypad(void)
{
	unsigned char * key;
	unsigned char lastkey = 0xff;
	int i = 0;
	int keyscancnt = 0;

	for (keyscancnt = 0;keyscancnt < KEY_COUNT;keyscancnt++)
	{
		keyscan_pos_width_tbl[keyscancnt].clear_flag = 1;		//清掉标记
	}

	keyscancnt = 0;

	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,0);
	//gui_FillRect(0, ICON_HEIGHT+1, 240, 250, BG_COLOR);
	 
	gui_TextOut(0, 0,      "POWER", 0,0);
	gui_TextOut(0, 13,   "[-] ESC ENTER [-]", 0,0);
	gui_TextOut(0, 26, "LEFT UP RIGHT DOWN", 0,0);
	gui_TextOut(0, 39, "      SCAN", 0,0);
	gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);

	OSTimeDlyHMSM(0,0,1,0);
	OSSchedLock();		//不要再进行任务调度了，防止键值触发其它任务
	while (1) 
	{
		key = keypad_getkey();

		if ((key)&&(lastkey != (*key))) 
		{
			lastkey = *key;
			Beep(BEEP_DELAY);
			i = 0;
			while (keyscan_pos_width_tbl[i].key != 0xff)	//搜索表格
			{
				if ((*key == keyscan_pos_width_tbl[i].key)&&(keyscan_pos_width_tbl[i].clear_flag == 1))
				{
					keyscan_pos_width_tbl[i].clear_flag = 0;		//清掉标记
					gui_FillRect(keyscan_pos_width_tbl[i].x,keyscan_pos_width_tbl[i].y, 6*keyscan_pos_width_tbl[i].width, 13, BG_COLOR,1);
					keyscancnt ++;		//记住已经扫描了多少个键
					break;
				}
				i++;
			}
		}
		if (keyscancnt == KEY_COUNT)		//按键扫描完了退出//
		{
			break;
		}
	}
	OSSchedUnlock();		//任务解锁
	return;
}


//扫描头测试
static void testScanerInit(void)
{
	unsigned char dis_str[5];
	//unsigned char tmp[31];
	int           ret = 0;
	int			  command;
	unsigned char scan_success_flag = 0;

	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,0);
	gui_TextOut(0,0,"条码(S):",0,0);
	//gui_LineH(0,13,20*6,TXT_COLOR,0);
	gui_TextOut(0,26,"条码类型:",0,0);
	//gui_LineH(0,39,20*6,TXT_COLOR,0);
	gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);

	scan_success_flag = 0;
	while(1)
	{
		ret = task_read_command_from_ui(&command, 0); 
		if( !ret &&(TASK_CONTINUE == command))
		{
			//用户按了扫描键
			//开始扫描商品条形码
			//scan_start = 1;
			memset(BarCode,0,31);
			//memset(tmp,0,31);
			memset(BarCode_Type,0,20);
			BarCode_Len = 0;
			if (UE988_get_barcode(BarCode_Type, BarCode,20, &BarCode_Len) == 0)
				//if (scanner_get_barcode(BarCode,30,BarCode_Type,&BarCode_Len)  == 0)
			{
				//获取到条码
				Delay(5000);
				Beep(BEEP_DELAY);
				barcode_got_flag = 1;
				scan_start = 0;

				//显示条码
				//gui_TextOut()
				//后台扫描条形码结束 
				//那么需要将后台获取到的条形码显示出来

				if ((BarCode[0] != 0)&&(BarCode_Len != 0))
				{
					gui_TextOut_ext(CENTER_ALIGN,13,BarCode,0,1);
					gui_TextOut_ext(CENTER_ALIGN,39,BarCode_Type,0,1);
					//hex_to_str(((BarCode_Len > 30)?30:BarCode_Len),10,0,dis_str);
					//gui_TextOut_ext(CENTER_ALIGN,LINE6,dis_str,1);
				}
				//gui_SetTextColor(TXT_COLOR);

				enter_scaner_test_flag  = 2;	//进入扫描测试的标志清零，退出扫描测试
				scan_success_flag		= 1;	//用来表示已经测试成功

				//gui_TextOut(4,240,"Press [OK] key to begin next ",0);
				//gui_TextOut(4,240+18,"test!",0);
				//gui_TextOut(4,240+18+20,"Press [A] key to continue ",0);
				//gui_TextOut(4,240+18+20+18,"scanning!",0);
				//gui_SetTextColor(TXT_COLOR);

			}
			scan_start = 0;
		}
		else if( !ret && (TASK_RUN == command) && scan_success_flag)
		{
			break;
		}

	}

}


//蓝牙模块测试
static void testBluetooth(void)
{
	OSSchedLock();		//不要再进行任务调度了，防止键值触发其它任务
	YFBT07_Test();
	OSSchedUnlock();		//任务解锁

}


//USB测试
static void testUSB(void)//USB测试
{
	unsigned char i = 0;

#if 1

	usb_device_init(USB_KEYBOARD);	//将设备初始化为一个
	USB_Cable_Config(1);
	
	OSSchedLock();		//不要再进行任务调度了，防止键值触发其它任务
	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,0);
	gui_TextOut_ext(CENTER_ALIGN,13,"连接USB线!",0,0);
	gui_TextOut(40,26,"等待中",0,0);
	gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);

	while (CONFIGURED != bDeviceState)
	{
		switch ( i = (i+1)%5)
		{
		case 0:
			gui_TextOut(40+12*3,26,"...",0,1);
			break;
		case 2:
			gui_TextOut(40+12*3,26,".  ",0,1);
			break;
		case 3:
			gui_TextOut(40+12*3,26,"   ",0,1);
			break;
		case 1:
		case 4:
			gui_TextOut(40+12*3,26,".. ",0,1);
		}
		//OSTimeDlyHMSM(0,0,0,500);//暂停0.5 s
		Delay(9000000);
	}

	gui_TextOut(0,26,"                   ",0,1);//擦除“Waiting”
	gui_TextOut_ext(CENTER_ALIGN,26,"USB连接成功!",0,1);//usb连接成功！
	Beep(BEEP_DELAY);
	OSSchedUnlock();		//不要再进任务调度了，防止键值触发其它任务
	OSTimeDlyHMSM(0,0,1,500);//暂停1.5 s

#endif

}


//RTC测试
static void testRTC(void)//rtc测试
{
	volatile int  n;
	unsigned char sec_save;

	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,1);
INIT_RTC:

	SD2068A_GetTime((unsigned char*)&currentDateTime);
	sec_save = currentDateTime.sec;
	OSTimeDlyHMSM(0,0,1,50);//延时1.05s
	SD2068A_GetTime((unsigned char*)&currentDateTime);
	if ((currentDateTime.sec+60-sec_save)%60 >= 1 )
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"RTC 工作正常",0,1);
		//设置CC1101模块与基站匹配的频段
		OSTimeDlyHMSM(0,0,1,500);//暂停1.5 s
	}
	else
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"RTC 工作错误",0,1);
		//设置CC1101模块与基站匹配的频段
		OSTimeDlyHMSM(0,0,1,500);//暂停1.5 s
		goto  INIT_RTC;//
	}

}

//RTC测试
static void testAD(void)//rtc测试
{
	int i,vol_value,val_value_aver,vol_value_final;

	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,1);

	val_value_aver = 0;
	for (i = 0;i < 10;i++)
	{
		vol_value = GetPowerVoltage_test_mode();
		val_value_aver += vol_value; 
		OSTimeDlyHMSM(0,0,0,50);
	}

	val_value_aver = val_value_aver/10;
	vol_value_final = (val_value_aver*7705)/10000;

	if ((vol_value_final >= 2300)&&(vol_value_final <=2500))
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"AD采样正常",0,1);
		OSTimeDlyHMSM(0,0,1,500);//暂停1.5 s
	}
	else
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"AD采样错误",0,1);
		Beep(BEEP_DELAY*2);
		OSTimeDlyHMSM(0,0,1,300);//暂停1.5 s
	}
}


/**
* @brief 工厂测试的后台任务
* @param[in] none                                 
*/
static int factory_test_task(int param1, int param2, void *param3, void *param4)
{	
	int								command;
	//测试项目标号，根据其查询testfunc_tbl这个表来获取相应的测试函数
	int								testItemNum = 0;
	//int max = sizeof(testfunc_tbl);
	while(1)
	{
		while(1)
		{
			// 等待UI指令
			if( task_read_command_from_ui(&command, 0) == 0)
			{
				if(command == TASK_RUN)
				{
					break;			// 测试开始
				}

				if(command == TASK_EXIT)
				{
					return KEY_EXT_2;			// 用户取消
				}
			}
		}

		testfunc_tbl[testItemNum]();				//调用相应的测试函数完成该项的测试
		task_send_command_to_ui(KEY_EXT_1);			//某项测试完成	
		testItemNum ++;								//测试项目标号加1，进行下一项的测试

		if (testItemNum == sizeof(testfunc_tbl)/sizeof(int))			//测试全部完成，退出此后台任务
		{
			break;
		}
	}
	return KEY_EXT_1;		//测试完成
}



/**
* @brief 生产测试的窗体回调调用的公共回调函数
*/
static unsigned char dlgproc_test_proc(unsigned char *key)
{
	if (*key == KEY_ENTER || *key == KEY_SCAN || *key == KEY_RIGHT_SHOTCUT)
	{
		if (testproc_first_enterkey == 1) 
		{
			testproc_first_enterkey = 0;			//清掉这个标记
			ui_send_command_to_task(TASK_RUN);
			return RUN_RERUN;
		}
		else 
		{
			if (1 == enter_scaner_test_flag)
			{
				ui_send_command_to_task(TASK_CONTINUE);
				return RUN_RERUN;
			}
			else if(2 == enter_scaner_test_flag)
			{
				ui_send_command_to_task(TASK_RUN);		//用意使扫描测试退出
				return RUN_RERUN;
			}
			else
			{
				return RUN_RERUN;
			}
		}

	}

	if (*key == KEY_EXT_1)		//后台运行完一项测试，就会返回这个键值
	{
		*key = KEY_ENTER;
		return RUN_NEXT;
	}

	if (*key == KEY_ESC) 
	{
		return RUN_RERUN;
	}

	/*
	if (*key == KEY_ESC) 
	{
	pos_state &= ~STA_FACTORY_MODE;		//清掉测试模式的状态机
	ui_send_command_to_task(TASK_EXIT);
	return RUN_RERUN;
	}

	if (*key == KEY_EXT_2)		//后台任务被取消
	{
	*key = KEY_ESC;
	return RUN_NEXT;
	}
	*/ 

	return RUN_CONTINUE;
}

/*
*显示测试界面
*/
unsigned char dlgproc_test_lcd_display(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
		enter_scaner_test_flag  = 0;//进入扫描测试的标志初始化
		//开始在后台运行生产测试程序
		dlg_run_bgtask(factory_test_task, 0, 0, 0, 0);
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}

	return RUN_CONTINUE;
}



/*
*测试按键
*/
unsigned char dlgproc_test_keypad(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
	}
	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;
}



/*
*测试扫描头
*/
unsigned char dlgproc_test_scaner_init(unsigned char type,unsigned char *key)
{
	if( DLGCB_DRAW == type)
	{
		scan_start				= 0;    //可以开始扫描
		enter_scaner_test_flag  = 1;	//进入扫描测试的标志
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
	}

	if (DLGCB_RUN  == type)
	{
		return dlgproc_test_proc(key);
	}

	if (DLGCB_DESTORY == type)
	{
		enter_scaner_test_flag  = 0;			//清掉这个标记
	}

	return RUN_CONTINUE;

}

/*
*测试cc1101
*/
unsigned char dlgproc_test_bluetooth(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*测试usb
*/
unsigned char dlgproc_test_usb(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*测试rtc
*/
unsigned char dlgproc_test_rtc(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*测试rtc
*/
unsigned char dlgproc_test_AD(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*
*/
unsigned char dlgproc_test_complete(unsigned char type,unsigned char *key)
{

	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//绘制窗体时给置位这个标记

		// pos_run_mode = ;
		//OSTimeDlyHMSM(0,0,1,0);
		//gui_CtrlBL(0);
		/* Request to enter STOP mode with regulator ON */
		//PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
		//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

		RCC_Configuration();
		/* Allow access to BKP Domain */
		//PWR_BackupAccessCmd(ENABLE);
		Calendar_Init();

		USB_Cable_Config(0);
		OSTimeDlyHMSM(0,0,0,300);
		USB_Cable_Config(1);

	}
	return RUN_CONTINUE;
}
