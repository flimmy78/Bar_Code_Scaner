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

static unsigned char	scan_func_state;	

static unsigned char	line3_save[21];

static unsigned char	special_func_flag;




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
//extern int						bluetooth_module_state;
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


//错误提示
static unsigned char err_tip(void)
{
	Beep(BEEP_DELAY*2);

	if (g_param.language == LANG_ENGLISH)
	{
		gui_TextOut_ext(CENTER_ALIGN,39,"Barcode Mismatch!",0,0);
		strcpy(line3_save,"Barcode Mismatch!");
	}
	else
	{
		gui_TextOut_ext(CENTER_ALIGN,39,"条码匹配错误!",0,0);
		strcpy(line3_save,"条码匹配错误!");
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	OSTimeDlyHMSM(0,0,0,50);
	Beep(BEEP_DELAY*2);
	OSTimeDlyHMSM(0,0,0,50);
	Beep(BEEP_DELAY*2);
	return RUN_RERUN;
}


//格式化条码的显示格式
static void format_barcode_disp_str(unsigned char *barcode, unsigned char *disp_str)
{
	if (strlen((const char*)barcode) > 20)
	{
		disp_str[0] = ' ';
		disp_str[1] = ' ';
		memcpy(disp_str+2,barcode,7);
		disp_str[9]='.';
		disp_str[10]='.';
		disp_str[11]='.';
		memcpy(disp_str+12,barcode+strlen((const char*)barcode)-7,7);
		disp_str[19]=' ';
		disp_str[20]=0;
	}
	else
	{
		strcpy((char*)disp_str,(const char*)barcode);
	}
}

/**
* @brief 扫描功能
*/
static unsigned char dlgproc_scan_func(unsigned char type,unsigned char *key)
{
	unsigned char  dis_str[21];
	if (DLGCB_CREATE == type)
	{
		if (dlg_refresh_flag == 0)
		{
			//record_read(REC_TYPE_BASCIAL_BARCODE,last_BarCode);
			if (basical_barcode[0] == 0)
			{
				*key = KEY_EXT_1;
				return RUN_NEXT;
			}
		}
	}

	if (DLGCB_DRAW == type)
	{
		if (dlg_refresh_flag == 0)
		{
			sprintf(dis_str,"%d",scan_count);
			gui_TextOut(0,0,dis_str,0,1);
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,13,"Basical:",0,1);
			}
			else
			{
				gui_TextOut(0,13,"参照条码:",0,1);
			}

			format_barcode_disp_str(basical_barcode,dis_str);
			gui_TextOut_ext(CENTER_ALIGN,26,dis_str,0,1);

			scan_func_state = 0;
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//后台等待检测条形码的任务
		}
		else
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,13,"Basical:",0,1);
			}
			else
			{
				gui_TextOut(0,13,"参照条码:",0,1);
			}

			format_barcode_disp_str(basical_barcode,dis_str);
			gui_TextOut_ext(CENTER_ALIGN,26,dis_str,0,1);

			if (strcmp(BarCode,basical_barcode))
			{
				scan_func_state = 1;
				sprintf(dis_str,"%d",scan_count);
				gui_TextOut(0,0,dis_str,0,1);
				return err_tip();
			}
			else
			{
				record_write(REC_TYPE_COUNTER,(unsigned char*)0);
			}

			sprintf(dis_str,"%d",scan_count);
			gui_TextOut(0,0,dis_str,0,1);

			format_barcode_disp_str(BarCode,dis_str);
			gui_TextOut_ext(CENTER_ALIGN,39,dis_str,0,0);

			scan_func_state = 0;
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//后台等待检测条形码的任务
		}

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

			if (scan_func_state == 1)
			{
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
			
			//需要先判断，本次输入的条形码是否和参照条码一致
			if (strcmp(BarCode,basical_barcode))
			{
				scan_func_state = 1;
				return err_tip();
			}
			else
			{
				record_write(REC_TYPE_COUNTER,(unsigned char*)0);
			}

			sprintf(dis_str,"%d",scan_count);
			gui_TextOut(0,0,dis_str,0,0);

			format_barcode_disp_str(BarCode,dis_str);
			gui_TextOut_ext(CENTER_ALIGN,39,dis_str,0,0);
			gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);
                        return RUN_RERUN;
			
		}

		if(scan_func_state == 1)
		{
			gui_TextOut(0,39,"                    ",1,1);
			scan_func_state = 0;
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief 手动输入条码
*/
static unsigned char dlgproc_input_barcode_ext(unsigned char type,unsigned char *key)
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
			dlg_refresh_flag = 1;
			scan_func_state = 1;
			*key = KEY_EXT_1;
			return RUN_NEXT;
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


static unsigned char dlgproc_basical_barcode_setting(unsigned char type,unsigned char *key)
{

	if(DLGCB_DRAW == type)
	{
		if(dlg_refresh_flag == 0)
		{
			scan_func_state = 0;
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,26,"Please press\"SCAN\"...",0,1);
			}
			else
			{
				gui_TextOut(0,26,"请按\"SCAN\"键扫描...",0,1);
			}
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//后台等待检测条形码的任务
		}
		else
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

			scan_func_state = 1;
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc            Enter",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"取消            确认",0,1);
			}

			gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);
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
			else
			{
				*key = KEY_ENTER;
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

			gui_FillRect(0,13,GUI_WIDTH,USERZONE_HEIGHT,BG_COLOR,0);
			if (strlen(BarCode) > 20)
			{
				gui_TextOut(0,13,BarCode,0,0);
			}
			else
			{
				gui_TextOut(0,26,BarCode,0,0);
			}

			scan_func_state = 1;
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc            Enter",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"取消            确认",0,1);
			}

			gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);
		}

		if(*key == KEY_LEFT_SHOTCUT)
		{
			if(scan_func_state == 1)
			{
				*key = KEY_ESC;
			}
			return RUN_NEXT;
		}

		if(*key == KEY_RIGHT_SHOTCUT || *key == KEY_ENTER)
		{
			if(scan_func_state == 1)
			{
				record_write(REC_TYPE_BASCIAL_BARCODE,BarCode);
				*key = KEY_ESC;
				return RUN_NEXT;
			}
			else
			{
				return RUN_RERUN;
			}
		}
	}
        
        return RUN_CONTINUE;
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
			dlg_refresh_flag = 1;
			scan_func_state = 1;
			*key = KEY_EXT_1;
			return RUN_NEXT;
		}
	}
	return RUN_CONTINUE;
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
			record_clear();
			DefaultTerminalPara();

			*key = KEY_ESC;
			return RUN_NEXT;
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
* @brief 查看固件信息
*/
static unsigned char dlgproc_fw_update(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
		usb_device_init(USB_MASSSTORAGE);
		USB_Cable_Config(1);
		
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"Please Connect USB..",0,1);
		}
		else
		{
			gui_TextOut_ext(CENTER_ALIGN,26,"请连接USB...",0,1);
		}
		g_param.transfer_mode = TRANSFER_MODE_U_DISK;
	}

	if (DLGCB_DESTORY == type)
	{
		USB_Cable_Config(0);
		g_param.transfer_mode = 0;
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
	menu_current_item = 1;
	menu_start = 1;
	memset(BarCode,0,31);
	//memset((void*)draw_content,0,MAX_CASH_ITEM*sizeof(TDRAW_ITEM));	//将缓存的内容清掉

	//if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	//{
	//	bluetooth_match_cmd_cnt = 0;
	//	bluetooth_state_cnt = 0;
	//}

	InitDrawContext();
}

void task_ui(void* pp)
{
	app_init();
	//gui_clear(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_setlanguage(g_param.language);
	max_dlg				= DLG_MAX;
	next_dlg_id			= DLG_scan_func;	
	dlg_loop();
}

//根据设置对设备的状态进行初始化
void device_init_by_setting(void)
{
	//if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	//{
	//	USB_Cable_Config(0);		//断开USB设备的连接
	//	YFBT07_power_ctrl(1);		//开启蓝牙模块的电源
	//	bluetooth_match_cmd_cnt = 0;
	//	led_g_ctrl(1);
	//}
	//else if (g_param.transfer_mode == TRANSFER_MODE_KEYBOARD)
	//{
	//	USB_Cable_Config(1);
	//	usb_device_init(USB_KEYBOARD);
	//	YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
	//	led_g_ctrl(0);
	//}
	//else if (g_param.transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	//{
	//	USB_Cable_Config(1);
	//	usb_device_init(USB_VIRTUAL_PORT);
	//	YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
	//	led_g_ctrl(0);
	//}
	//else if (g_param.transfer_mode == TRANSFER_MODE_U_DISK)
	//{
	//	YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
	//	led_g_ctrl(0);
	//	g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
	//	usb_device_init(USB_MASSSTORAGE);
	//	USB_Cable_Config(1);
	//}
	//else
	//{
	//	if (g_param.batch_transfer_mode == TRANSFER_MODE_BLUETOOTH)
	//	{
	//		USB_Cable_Config(0);		//断开USB设备的连接
	//		YFBT07_power_ctrl(1);		//开启蓝牙模块的电源
	//		bluetooth_match_cmd_cnt = 0;
	//		led_g_ctrl(1);
	//	}
	//	else if (g_param.batch_transfer_mode == TRANSFER_MODE_KEYBOARD)
	//	{
	//		usb_device_init(USB_KEYBOARD);
	//		YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
	//		led_g_ctrl(0);
	//		USB_Cable_Config(1);
	//	}
	//	else
	//	{
	//		USB_Cable_Config(1);
	//		usb_device_init(USB_VIRTUAL_PORT);
	//		YFBT07_power_ctrl(0);	//关闭蓝牙模块的电源
	//		led_g_ctrl(0);
	//	}
	//}

	USB_Cable_Config(0);
	YFBT07_power_ctrl(0);
	g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
	usb_device_init(USB_MASSSTORAGE);
	led_g_ctrl(0);
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
			//enter_test_task(test_type);
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
			if (UE988_get_barcode(BarCode_Type, BarCode,30, &BarCode_Len) == 0)
				//if (scanner_get_barcode(tmp,30,BarCode_Type,&BarCode_Len)  == 0)
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
