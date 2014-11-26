
#include "dialog.h"
#include "keypad.h"
#include "pic.h"
#include "stm32f10x_lib.h"
#include "ucos_ii.h"
#include "Terminal_Para.h"
#include "calendar.h"
#include "keypad.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gui.h"
#include "keypad.h"
#include "power_detect.h"
#include  "usb_pwr.h"
#include "app.h"
#include "lcd.h"
#include "YFBT07.h"

/* Private variables ---------------------------------------------------------*/
				
static unsigned char			menu_all;
static unsigned char			menu_pagecnt;			//当前窗体一页可以显示的菜单项
static 	unsigned int			user_zone_height;

static unsigned char			now_id;
static unsigned char			howrun;
static unsigned char			dlg_state;
//static TProcessBar				process_bar;				//进度条

static unsigned char			**pContent;
//static int						last_charge_state;			//上次的充电状态

static unsigned char			the_last_key;				//输入一个编辑框最后一个键值时的标记

static int							SCommand;
TbgTask						SbgTask;					// 后台任务回调函数
static int							SParam1;
static int							SParam2;
static void							*SParam3;
static void							*SParam4;
static unsigned char				SReturn;					// 任务的返回值
static int							SStatus;
static int							lowpower_detect_cnt;
static int							last_power_level;

typedef struct 
{
	unsigned char valid_flag;			//当前保存的信息是否有效标记
	unsigned char dlg_id;				//当前窗体的ID
	unsigned char menu_start;			//绘制当前窗体的起始菜单项
	unsigned char menu_current_item;	//当前选择的菜单项
}TDRAWCONTEXT;

static TDRAWCONTEXT		draw_context[2];	//最多有两级目录需要回退

unsigned char			display_menu_item_array[5];		//界面当前显示的菜单项
unsigned char			menu_current_item;				//当前菜单项
unsigned char			menu_start;	
unsigned char			icon_draw_flag;					//图标栏已经绘制了的标记 
/* Global variables -----------------------------------------------------------*/
unsigned char				g_editValue[MAX_EDIT][MAX_EDITNUM];//编辑框的全局变量

unsigned char				max_dlg;
unsigned char				next_dlg_id;
//unsigned char				content_font_size;	//显示窗体内容时所用的字体大小

TDialog						g_dlg;			
OS_EVENT					*pBGCommand;				//控制后台任务的命令
//unsigned char				auto_run_flag;				//控制窗体不需要获取到按键消息也可以进入按键处理的流程的标记。

//由于需要在窗口回调中修改中文属性的编辑框的首选输入法，所以需要将下面三个变量设置为全局的属性。
unsigned char				update_input_method_mode;	//更新输入法图标的模式
unsigned char				input_method;				//当前输入法
unsigned char				last_input_method;			//上一次使用的输入法
unsigned char				max_edit;					//当前窗体包含的可编辑编辑框的总数
unsigned char				all_edit;					//当前窗体包含的所有编辑框的总数
unsigned char				edit_res_rebuild_flag;		//表示当前窗体的编辑框资源链表需要在运行时动态创建，链表创建于内存中，需要动态创建编辑框资源链表的窗体
														//必须在窗体CREATE阶段的回调接口中标记。
//unsigned char				default_input_method_patch;	//编辑框的首选输入法是否需要另外指定
unsigned char				number_edit_patch;			//数字属性的编辑框中是否允许通过*号键输入小数点

unsigned char		same_key_times;				///连续按同一个按键的次数

//TEditRes					edit_res_rebuild[MAX_EDIT];	//动态创建的编辑框资源链表,在窗体CREATE阶段的回调接口中创建此链表。

unsigned char				tempx;
unsigned short				tempy;

unsigned char			update_icon_cnt;
unsigned short			edit_display_x_patch;

unsigned int			lowpower_flag;
int						bluetooth_match_cmd_cnt;
int						bluetooth_state_cnt;

		

//TDRAW_ITEM	draw_content[MAX_CASH_ITEM];	//有些窗体在DRAW阶段回调时绘制的窗体内容需要缓存起来的缓存空间，由于处理中文输入法时需要将窗体的内容刷掉,如果需要增加缓存的内容可以改变此缓存空间的大小
TRollDisplayItem	roll_display_table[3];	//需要滚动显示的内容
unsigned char		roll_display_item_cnt;	//需要滚动显示的个数
unsigned char		roll_update_cnt;
unsigned char		roll_data_buffer[22];

unsigned char		dlg_return_from_child;			//从子窗体返回此窗体运行的标记

/* External Variable -----------------------------------------------------------*/
extern const TActionList		dlg_actionlist[];
extern OS_EVENT					*pKeyOSQ;					// 按键消息
extern TkeyValue_Ascii			keyValue_Ascii_Tbl[];
extern TTerminalPara			g_param;			//全局终端参数
extern TypedefDateTime			currentDateTime;	
extern unsigned char			gFontSize;			// 字号
extern unsigned char			g_language;			// 文字类型
extern unsigned char			edit_state;			//编辑框状态
//extern unsigned char			current_operator;			// 当前登录到系统的操作员
extern unsigned int				input_update_flag;			//是否需要更新输入的标记
extern unsigned char			last_input_key;				//最近一次输入的按键
extern vu32 bDeviceState;
extern unsigned int				scan_start;
extern unsigned char			no_update_icon_flg;
extern unsigned char			usb_cable_state;
//extern unsigned char	input_valid_num;				//输入的有效位数
extern unsigned int				pos_state;
extern unsigned char			need_refresh_signal;
extern unsigned char			task_exit_flag;
extern int						bluetooth_module_state;

//extern unsigned char scanner_power_on_flag;
//extern unsigned int  scanner_timeout;
extern unsigned int	charge_state_cnt;

extern unsigned char			need_refresh_edit_index_tbl[];	

extern void EnterLowPowerMode(void);
extern void ExitLowPowerMode(void);
extern void	u_disk_proc(void);

static int exp10(unsigned char e)
{
	unsigned char i;
	int	res = 1;

	if (e>10)
	{
		return 1;
	}

	for (i = 0; i < e;i++)
	{
		res *= 10;
	}

	return res;
}
/**
* @brief 检查按键是否数字键和清除键
*/
int is_key_num(unsigned char key)
{
	if(key==KEY_NUM1 || key==KEY_NUM2 || key==KEY_NUM3 || 
		key==KEY_NUM4 || key==KEY_NUM5 || key==KEY_NUM6 || 
		key==KEY_NUM7 || key==KEY_NUM8 || key==KEY_NUM9 || 
		key==KEY_NUM0 ||key==KEY_ENTER || 
		key==KEY_UP || key==KEY_DOWN )
		return 0;
	return -1;
}

/**
* @brief 检查按键是否是键盘上的实际按键
*/
int is_key_keypad(unsigned char key)
{
	if(key==KEY_SCAN || key==KEY_UP || key==KEY_DOWN || key==KEY_LEFT || key == KEY_RIGHT
		|| key==KEY_LEFT_SHOTCUT || key==KEY_ESC || key==KEY_ENTER || key==KEY_RIGHT_SHOTCUT )
		return 0;
	return -1;
}

/**
* @brief 初始化对话框模块
*/
int dlg_init(void)
{
	//enote_tip_flag = 0;
	icon_draw_flag = 0;
	//content_font_size = 16;			//大部分窗体内容都是用16号字体，需要改变字体大小来显示窗体内容时，在串口创建时的回调函数中修改此变量即可。
	//auto_run_flag = 0;				//大部分的窗体都是正常的流程，在获取到按键消息之后才进入按键处理的流程
	lowpower_detect_cnt = 0;
	last_power_level = 4;
    charge_state_cnt = 0;
	pBGCommand						= OSSemCreate(0);
	return 0;
}

/**
* @brief 将数字转成键值
*/
unsigned char i2key(unsigned char i)
{
	if(i==0)	return KEY_NUM0;
	if(i==1)	return KEY_NUM1;
	if(i==2)	return KEY_NUM2;
	if(i==3)	return KEY_NUM3;
	if(i==4)	return KEY_NUM4;
	if(i==5)	return KEY_NUM5;
	if(i==6)	return KEY_NUM6;
	if(i==7)	return KEY_NUM7;
	if(i==8)	return KEY_NUM8;
	if(i==9)	return KEY_NUM9;
	if(i==11)	return KEY_NUM1;
	if(i==12)   return KEY_NUM2;
	if(i==13)   return KEY_NUM3;
	if(i==14)   return KEY_NUM4;
	return KEY_NUM0;			// 不应到这里哦
}

/**
* @brief 等待消息,可能是按键、定时器或其他系统消息
* @param[in] unsigned int type
* @return int 
*/
static unsigned char *dlg_getmessage(unsigned int type, int timeout)
{
	unsigned char					*pKey = (unsigned char*)0;
	unsigned char					err;

	pKey							= OSQPend(pKeyOSQ, timeout, &err);

	return pKey;

}


//清空消息队列里的消息
static void flush_message(void)
{
	OSQFlush(pKeyOSQ);
	return;
}


/**
* @brief 计算一个字符串的显示长度
* @param[in] unsigned char *str  需要计算的字符串指针
* @note 由于中文和英文的字模宽度比不在刚好是2:1，所以需要单独一个函数判断一个字符串的显示长度。
*/
int disp_len(unsigned char *str)
{
	return strlen(str)*6;
}

/**
* @brief 重新刷新窗体在DRAW阶段回调中绘制的窗体内容
*/
void refresh_draw_content(void)
{
#if 0 
	int i;
	for (i = 0; i<MAX_CASH_ITEM; i++)
	{
		if (draw_content[i].content[0] != 0)
		{
			gui_TextOut(draw_content[i].x,draw_content[i].y,draw_content[i].content,0,1);
		}
	}
#endif
}


/**
* @brief 初始化编辑框属性
* @param[in] TEdit *pEdit 要显示的Edit对柄
*/
void edit_init(TEdit *pEdit)
{
	unsigned char i = 0;
	unsigned char	j = 0;
	TEditRes	*pEditRes;

	pEditRes = pEdit->pRes;	
	max_edit = 0;
	all_edit = 0;

	//创建窗体时，编辑框都已经被清0了
	//memset(pEdit->active,0,MAX_EDIT);
	//memset(pEdit->max,0,MAX_EDIT);
	//memset(pEdit->now,0,MAX_EDIT);

	do 
	{
		if((pEditRes->style&0x0F) == EDIT_IP)				// ip地址定长为15   [0]123.456.789.abc[15]
		{
			pEdit->display_max[i]		= 15;
			pEdit->input_max[i]			= 15;
			pEdit->now[i]				= 0;
		}
		else if((pEditRes->style&0x0F) == EDIT_MONEY)			//			[8]1234567.2[0]
		{
			pEdit->display_max[i]		= 9;		//规定最多可以输入9位的交易金额
			pEdit->input_max[i]			= 9;
			pEdit->now[i]				= 0;
		}
		else													// 普通					[5]654321[0]
		{
			pEdit->display_max[i]  = pEditRes->display_max_num;
			pEdit->input_max[i]  = pEditRes->input_max_num;
			pEdit->now[i]	= 0;
		}

		if((pEditRes->style & EDIT_READONLY) == 0)
		{
			pEdit->active[i] = 1;
		}

		if((pEditRes->style & EDIT_READONLY) == 0)
		{
			max_edit++; //此窗体中包含的总的非只读编辑框的个数

			//找到第一个非只读的编辑框
			if ((j&0x80) != 0x80)
			{
				j = i;
				j |= 0x80;
			}
		}

		//编辑框中的字体默认为系统设置的字体大小及颜色
		pEdit->view[i].bg_color = gui_GetBGColor();
		pEdit->view[i].txt_color = gui_GetTextColor();
		pEdit->view[i].font_size = 12;

		pEdit->display_patch[i] = 0;

		pEditRes = pEditRes->next;							//指向下一个编辑框的资源
        i++;
	} while (pEditRes != 0);

	all_edit = i;
	pEdit->current_edit = (j&0x7f);	//默认第一个非只读编辑框为当前编辑框
	pEdit->blink				= 0;
	

	//pEdit->value				= &g_editValue[0];
	// edit清零
	memset(g_editValue, 0x00, MAX_EDITNUM*MAX_EDIT);
	last_input_key = 0;		//最近一次输入的按键清0
	input_update_flag = 0;
	edit_state = EDIT_STATE_INIT_STATUS;
	the_last_key = 0;
	input_method = _123_INPUT;
}
/**
* @brief 获取窗体当前编辑框的属性
* @param[in] TEdit *pEdit 要显示的Edit对柄
*/
static void edit_get_attribute (TEdit *pEdit,TEditAttribute * edit_attr)
{
	TEditRes	* pEditRes;
	unsigned char	i = 0;

	pEditRes = pEdit->pRes;

	do 
	{
		if (i == pEdit->current_edit)
		{
			break;
		}
		i ++;
		pEditRes = pEditRes->next;
	} while (pEditRes != 0);

	edit_attr->x = pEditRes->x;
	edit_attr->y = pEditRes->y;
	edit_attr->style = pEditRes->style;
	if (g_dlg.pRes->style & STYLE_CHILDWINDOW)		//注意子窗体中的编辑框的位置是相对于子窗体位置的相对坐标
	{
		edit_attr->x += DEFAULT_CHILDWINDOW_X; 
		edit_attr->y += DEFAULT_CHILDWINDOW_Y;
	}
}

/**
* @brief 将键值转换为输出的金额字符串
* @param[in] unsigned char *keyvalue 要转换的键值
* @note 先输入的键值先输出
*/
#if 0
static void edit_to_amount(unsigned char* keyvalue ,unsigned char * out,unsigned char len)
{
	unsigned char i,j = 0;
	if(len < 3)
	{
		out[0] = '0';
		out[1] = '.';
		for(i=0; i<len; i++)
		{
			out[2+i] = *key2ascii(keyvalue[i]);
		}

		if (len == 0)
		{
			out[2] = '0';
			out[3] = '0';
		}

		if (len == 1)
		{
			out[3] = out[2];
			out[2] = '0';
		}

		out[4] = 0;
	}
	else
	{
		for (i=0;i<len;i++)
		{
			if (i == len - 2)
			{
				j = 1;
				out[i] = '.';
			}
			out[i+j] = *key2ascii(keyvalue[i]);
		}
		out[len+1] = 0;
	}
}
#endif
/**
* @brief 设置某一个编辑框中显示的字体大小
* @param[in] unsigned char size			字体大小
* @param[in] unsigned char edit_index	编辑框索引
* @note 设置的字体大小只是在此窗体的生命期内有效,在窗体的DRAW阶段之后的回调接口中调用才会生效
*/
void edit_set_Font(TEdit *pEdit,unsigned char size,unsigned char edit_index)
{
	if ((size == 12)||(size == 16)||(size == 24))
	{
		pEdit->view[edit_index].font_size = size;
	}
	return;
}

/**
* @brief 设置某一个编辑框中显示的字体的背景色
* @param[in] unsigned short color		颜色
* @param[in] unsigned char edit_index	编辑框索引
* @note 设置的字体大小只是在此窗体的生命期内有效,在窗体的DRAW阶段之后的回调接口中调用才会生效
*/
void edit_set_BgColor(TEdit *pEdit,unsigned short color,unsigned char edit_index)
{
	pEdit->view[edit_index].bg_color = color;
	return;
}

/**
* @brief 设置某一个编辑框中显示的字体的背景色
* @param[in] unsigned short color		颜色
* @param[in] unsigned char edit_index	编辑框索引
* @note 设置的字体大小只是在此窗体的生命期内有效,在窗体的DRAW阶段之后的回调接口中调用才会生效
*/
void edit_set_TextColor(TEdit *pEdit,unsigned short color,unsigned char edit_index)
{
	pEdit->view[edit_index].txt_color = color;
	return;
}
/**
* @brief edit刷新编辑框的显示
* @param[in] unsigned char mode  0:刷新当前窗体的所有的编辑框   1:刷新指定的编辑框
* @note 输入后，EditValue中填的是键值，而不是ascii，显示时需要转换
* @note
*/
void edit_refresh(TEdit *pEdit,unsigned char mode,unsigned char edit_index)
{
	int								i,j;
	TEditAttribute					edit_attr;			//当前编辑框的属性
	unsigned char					current_edit,edit_save;
	int								disp_len;
	TEditRes						* pEditRes;
	unsigned char					amount[10];
    unsigned char					str[38];
	unsigned int					x,y;
	unsigned char					offset;


	pEditRes = pEdit->pRes;
	edit_save = pEdit->current_edit;	//保存当前编辑框

	pEdit->current_edit = 0;

	do 
	{
		j = 0;
		current_edit = pEdit->current_edit;
		edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性
		//if ((edit_attr.style & 0x80) == EDIT_MUL_LINE)
		//{
		//	edit_display_x_patch = 0;
		//}

		//pEdit->display_patch[current_edit] = 0;

		if (edit_attr.y + 13*((pEdit->display_max[current_edit]-1)/20) > GUI_HEIGHT - SHOTCUT_HEIGNT - 12)
		{
			if(0 == mode)
			{
				break;
			}
			else
			{
				//编辑框的纵坐标大于显示区域
				edit_attr.y = ICON_HEIGHT + 18 + edit_attr.y - (GUI_HEIGHT - SHOTCUT_HEIGNT );
			}
		}


		if ((mode == 0) || (edit_index == current_edit))
		{ 
			if((edit_attr.style & 0x07) == EDIT_IP)				// 顺序排列 0.1.2.3.4.5.6
			{
				///STEP2:清除显示
				memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
				str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

				///STEP3:显示输出
				for(i=0; i<pEdit->now[current_edit]; i++)									/// 
				{
					if ((i==3) ||(i==6)||(i==9))
					{
						j = 12*(i/3);
					}
					gui_TextOut(edit_attr.x + i*gFontSize/2 + j, edit_attr.y, key2ascii(g_editValue[current_edit][i]), 0,1);
				}
				///STEP4:下划线
				if(edit_attr.style & EDIT_UNDERLINE)
				{
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
					gui_LineH(edit_attr.x + 4 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
					gui_LineH(edit_attr.x + 8 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
					gui_LineH(edit_attr.x + 12 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
				}

				//STEP5:显示分隔符
				gui_TextOut(edit_attr.x + 3*gFontSize/2, edit_attr.y, ".", 0,1);
				gui_TextOut(edit_attr.x + 7*gFontSize/2, edit_attr.y, ".", 0,1);
				gui_TextOut(edit_attr.x + 11*gFontSize/2, edit_attr.y, ".", 0,1);
			}
			else if(((edit_attr.style & 0x07) == EDIT_PSW)||((edit_attr.style & 0x07) == EDIT_PSW_S))		
			{
				///STEP2:清除显示
				memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
				str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);


				///STEP3:显示输出
				for(i=0; i<pEdit->now[current_edit]; i++)									/// * * * * * *
					gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, "*", 0,1);

				///STEP4:下划线
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR,1);
			}
			else if((edit_attr.style & 0x07) == EDIT_NUM)
			{
				/// STEP2:清除显示
				memset(str,' ',pEdit->display_max[current_edit]);
				str[pEdit->display_max[current_edit]] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, (((edit_attr.style & 0x10) == EDIT_NO_BLINK)?2:1),1);

				if ((edit_attr.style & 0x10) == EDIT_NO_BLINK)
				{
					gui_TextOut(4, edit_attr.y, "                    ", 2,1);
				}

				/// STEP3:输出字符
				offset = pEdit->now[current_edit]<pEdit->display_max[current_edit]?0:(pEdit->now[current_edit]-pEdit->display_max[current_edit]);
				for(i=0; i<(pEdit->now[current_edit]<pEdit->display_max[current_edit]?pEdit->now[current_edit]:pEdit->display_max[current_edit]); i++)
				{
					if((edit_attr.style & 0x10) == EDIT_NO_BLINK)
					{
						gui_TextOut(edit_attr.x + ((g_dlg.edit.display_max[current_edit]-1)-i)*gFontSize/2, edit_attr.y, key2ascii(g_editValue[current_edit][g_dlg.edit.now[current_edit] - i -1]), 2,1);
					}
					else
					{
						gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, key2ascii(g_editValue[current_edit][offset+i]), 0,1);
					}				
				}

				if(((edit_attr.style & 0x10) == EDIT_NO_BLINK)&&(pEdit->now[current_edit] == 0))
				{
					gui_TextOut(edit_attr.x+(g_dlg.edit.display_max[current_edit]-1)*gFontSize/2, edit_attr.y, "0", 2,1);
				}
				/// STEP4:下划线
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR,1);

			}
			else if((edit_attr.style & 0x07) == EDIT_MONEY)		// 倒序排列
			{
				// STEP2:清除显示
				memset(str,' ',pEdit->display_max[current_edit]);
				str[pEdit->display_max[current_edit]] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

				// STEP3显示输出

				// 输出字符,从最后一字节往前显示,注意加上小数点和货币符号
				if(edit_attr.style & EDIT_READONLY)
				{
					disp_len	= 0;
					while(g_editValue[current_edit][disp_len] != 0x00)
						disp_len++;
				}
				else
					disp_len				= pEdit->now[current_edit];

				//edit_to_amount(g_editValue[current_edit],amount,disp_len);
				//gui_TextOut(edit_attr.x + (pEdit->max[current_edit] - strlen((char const*)amount))*gFontSize/2,edit_attr.y,amount,0);
				//for(i=0; i<disp_len; i++)
				{
					gui_TextOut(edit_attr.x, edit_attr.y, g_editValue[current_edit], 0,1);			
				}

				// STEP4:下划线
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR,1);
			}
			else if ((edit_attr.style & 0x07) == EDIT_CHINESE || (edit_attr.style & 0x07) == EDIT_ALPHA)		
			{
				/// STEP2:清除显示
				memset(str,' ',pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6);
				str[pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);
                                

				//if((edit_attr.style & 0x80) == EDIT_MUL_LINE)
				//{
				//	gui_FillRect(0,edit_attr.y + gFontSize,GUI_WIDTH,gFontSize*4,BG_COLOR);	
				//}

				//for(i=0; i<pEdit->max[current_edit]; i++)
				//	gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, " ", 1);

				/// STEP3:输出字符,
				offset = pEdit->now[current_edit]<pEdit->display_max[current_edit]?0:(pEdit->now[current_edit]-pEdit->display_max[current_edit]);
				//判断是否将一个汉字的前半字节截断了
				for (i=0;i<offset;i++)
				{
					if (g_editValue[current_edit][i] > 0x80)
					{
						i++;
					}
				}

				if(i>offset)
				{
					offset++;
				}

				gui_TextOut(edit_attr.x, edit_attr.y, &g_editValue[current_edit][offset], 0,1);

				pEdit->display_patch[current_edit] = edit_display_x_patch;
				edit_display_x_patch = 0;

				/// STEP4:下划线
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2 + pEdit->display_patch[current_edit], TXT_COLOR,1);

			}
		}

		//if(pEdit->max[current_edit] == (pEdit->now[current_edit] + 1) || pEdit->max[current_edit] == pEdit->now[current_edit])
		{
			//清除可能还在闪烁的光标
			x = edit_attr.x + pEdit->display_max[current_edit]*gFontSize/2+pEdit->display_patch[current_edit];
			y = edit_attr.y;

			while(x >= GUI_WIDTH)
			{
				x -= GUI_WIDTH;
				y += 13;
			}

			gui_LineV(x,y+1,11,BG_COLOR);
		}

	

		if ((mode == 1)&&(edit_index == current_edit))
		{
			//只需要刷新了要刷新的编辑框即可，跳出
			break;
		}

		pEdit->current_edit++;
		pEditRes = pEditRes->next;
	} while (pEditRes != 0);

	pEdit->current_edit = edit_save;		//恢复当前的编辑框
	return;
}

/**
* @brief edit刷新编辑框的显示
* @param[in] unsigned char mode  0:刷新当前窗体的所有的编辑框   1:刷新指定的编辑框
* @param[out] unsigned char *buffer  将原来编辑框中的键值保存到虚拟键盘的缓冲区
* @note 输入后，EditValue中填的是键值，而不是ascii，显示时需要转换
* @note
*/
unsigned char edit_refresh_ext(TEdit *pEdit)
{
	int								i,j;
	TEditAttribute					edit_attr;			//当前编辑框的属性
	unsigned char					current_edit;
	int								disp_len;
	TEditRes						* pEditRes;
	unsigned char					amount[10];
	unsigned char					str[38];
	unsigned int					x,y;
	unsigned char					offset;
	unsigned char					virtual_keypad_type;


	pEditRes = pEdit->pRes;

	current_edit = pEdit->current_edit;
	edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性
	edit_attr.x = 0;
	edit_attr.y = 0;
	edit_attr.style &= ~EDIT_UNDERLINE;
	gui_SetTextColor(TXT_COLOR);

	{ 
		if((edit_attr.style & 0x07) == EDIT_IP)				// 顺序排列 0.1.2.3.4.5.6
		{
			///STEP2:清除显示
			memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
			str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

			///STEP3:显示输出
			for(i=0; i<pEdit->now[current_edit]; i++)									/// 
			{
				if ((i==3) ||(i==6)||(i==9))
				{
					j = 12*(i/3);
				}
				gui_TextOut(edit_attr.x + i*gFontSize/2 + j, edit_attr.y, key2ascii(g_editValue[current_edit][i]), 0,1);
			}
			///STEP4:下划线
			//if(edit_attr.style & EDIT_UNDERLINE)
			//{
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//	gui_LineH(edit_attr.x + 4 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//	gui_LineH(edit_attr.x + 8 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//	gui_LineH(edit_attr.x + 12 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//}

			//STEP5:显示分隔符
			gui_TextOut(edit_attr.x + 3*gFontSize/2, edit_attr.y, ".", 0,1);
			gui_TextOut(edit_attr.x + 7*gFontSize/2, edit_attr.y, ".", 0,1);
			gui_TextOut(edit_attr.x + 11*gFontSize/2, edit_attr.y, ".", 0,1);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_NUM;
		}
		else if(((edit_attr.style & 0x07) == EDIT_PSW)||((edit_attr.style & 0x07) == EDIT_PSW_S))		
		{
			///STEP2:清除显示
			memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
			str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);


			///STEP3:显示输出
			for(i=0; i<pEdit->now[current_edit]; i++)									/// * * * * * *
				gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, "*", 0,1);

			///STEP4:下划线
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_ALPHA;
		}
		else if((edit_attr.style & 0x07) == EDIT_NUM)
		{
			/// STEP2:清除显示
			memset(str,' ',pEdit->display_max[current_edit]);
			str[pEdit->display_max[current_edit]] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, (((edit_attr.style & 0x10) == EDIT_NO_BLINK)?2:1),1);

			if ((edit_attr.style & 0x10) == EDIT_NO_BLINK)
			{
				gui_TextOut(4, edit_attr.y, "                    ", 2,1);
			}

			/// STEP3:输出字符
			offset = pEdit->now[current_edit]<pEdit->display_max[current_edit]?0:(pEdit->now[current_edit]-pEdit->display_max[current_edit]);
			for(i=0; i<(pEdit->now[current_edit]<pEdit->display_max[current_edit]?pEdit->now[current_edit]:pEdit->display_max[current_edit]); i++)
			{
				if((edit_attr.style & 0x10) == EDIT_NO_BLINK)
				{
					gui_TextOut(edit_attr.x + ((g_dlg.edit.display_max[current_edit]-1)-i)*gFontSize/2, edit_attr.y, key2ascii(g_editValue[current_edit][g_dlg.edit.now[current_edit] - i -1]), 2,1);
				}
				else
				{
					gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, key2ascii(g_editValue[current_edit][offset+i]), 0,1);
				}				
			}

			if(((edit_attr.style & 0x10) == EDIT_NO_BLINK)&&(pEdit->now[current_edit] == 0))
			{
				gui_TextOut(edit_attr.x+(g_dlg.edit.display_max[current_edit]-1)*gFontSize/2, edit_attr.y, "0", 2,1);
			}
			/// STEP4:下划线
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_NUM;
		}
		else if((edit_attr.style & 0x07) == EDIT_MONEY)		// 倒序排列
		{
			// STEP2:清除显示
			memset(str,' ',pEdit->display_max[current_edit]);
			str[pEdit->display_max[current_edit]] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

			// STEP3显示输出

			// 输出字符,从最后一字节往前显示,注意加上小数点和货币符号
			if(edit_attr.style & EDIT_READONLY)
			{
				disp_len	= 0;
				while(g_editValue[current_edit][disp_len] != 0x00)
					disp_len++;
			}
			else
				disp_len				= pEdit->now[current_edit];

			//edit_to_amount(g_editValue[current_edit],amount,disp_len);
			//gui_TextOut(edit_attr.x + (pEdit->max[current_edit] - strlen((char const*)amount))*gFontSize/2,edit_attr.y,amount,0);
			//for(i=0; i<disp_len; i++)
			{
				gui_TextOut(edit_attr.x, edit_attr.y, g_editValue[current_edit], 0,1);			
			}

			// STEP4:下划线
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_NUM;
		}
		//else if ((edit_attr.style & 0x07) == EDIT_CHINESE || (edit_attr.style & 0x07) == EDIT_ALPHA)		
		else if ((edit_attr.style & 0x07) == EDIT_ALPHA)		
		{
			/// STEP2:清除显示
			memset(str,' ',pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6);
			str[pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);


			//if((edit_attr.style & 0x80) == EDIT_MUL_LINE)
			//{
			//	gui_FillRect(0,edit_attr.y + gFontSize,GUI_WIDTH,gFontSize*4,BG_COLOR);	
			//}

			//for(i=0; i<pEdit->max[current_edit]; i++)
			//	gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, " ", 1);

			/// STEP3:输出字符,
			offset = pEdit->now[current_edit]<pEdit->display_max[current_edit]?0:(pEdit->now[current_edit]-pEdit->display_max[current_edit]);
			//判断是否将一个汉字的前半字节截断了
			for (i=0;i<offset;i++)
			{
				if (g_editValue[current_edit][i] > 0x80)
				{
					i++;
				}
			}

			if(i>offset)
			{
				offset++;
			}

			gui_TextOut(edit_attr.x, edit_attr.y, &g_editValue[current_edit][offset], 0,1);

			pEdit->display_patch[current_edit] = edit_display_x_patch;
			edit_display_x_patch = 0;

			/// STEP4:下划线
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2 + pEdit->display_patch[current_edit], TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_ALPHA;
		}
	}

	//if(pEdit->max[current_edit] == (pEdit->now[current_edit] + 1) || pEdit->max[current_edit] == pEdit->now[current_edit])
	{
		//清除可能还在闪烁的光标
		x = edit_attr.x + pEdit->display_max[current_edit]*gFontSize/2+pEdit->display_patch[current_edit];
		y = edit_attr.y;

		while(x >= GUI_WIDTH)
		{
			x -= GUI_WIDTH;
			y += 13;
		}

		gui_LineV(x,y+1,11,BG_COLOR);
	}

	return virtual_keypad_type;
}


/**
* @brief 虚拟键盘的处理函数
* @note 输入后，EditValue中填的是键值，而不是ascii，显示时需要转换
* @param[in] unsigned char type:虚拟键盘的类型  数字型  和 字母型
* @param[in] unsigned char key:键值 0:初始化键盘
*/
unsigned char virtual_keypad_proc(unsigned char type,unsigned char key)
{
	static unsigned char	x,y,start_y;
        unsigned char           i,j;
	unsigned char		str[2],tmp[2];
	unsigned char		key_array_1[2][5] = {{'0','1','2','3','4'},
											  {'5','6','7','8','9'}
											 };
	unsigned char		key_array_2[11][9] = {{'0','1','2','3','4','5','6','7','8'},
											{'9','A','B','C','D','E','F','G','H'},
											{'I','J','K','L','M','N','O','P','Q'},
											{'R','S','T','U','V','W','X','Y','Z'},
											{'a','b','c','d','e','f','g','h','i'},
											{'j','k','l','m','n','o','p','q','r'},
											{'s','t','u','v','w','x','y','z','!'},
											{'"','#','$','%','&','\'','(',')','*'},
											{'+',',','-','.','/',':',';','<','='},
											{'>','?','@','[','\\',']','^','_','`'},
											{'{','|','}','~',' ',' ',' ',' ',' '}
										   };

	if (key == 0)
	{
		x = 0;
		y = 0;
		start_y = 0;
	}
	else if (key == KEY_UP)
	{
		if (type == VIRTUAL_KEYPAD_TYPE_NUM)
		{
			y = (y == 1)?0:1;
		}
		else if(type == VIRTUAL_KEYPAD_TYPE_ALPHA)
		{
			if(x > 4)
			{
				if(y == 0)
					y = 9;
				else
					y--;
			}
			else
			{
				if(y == 0)
					y = 10;
				else
					y--;
			}

			if((start_y == 0)&&(y == 9 || y == 10))
			{
				start_y = 8;
			}
			else
			{
				if(y < start_y)  
					start_y --;
			}
		}
	}
	else if (key == KEY_DOWN)
	{
		if (type == VIRTUAL_KEYPAD_TYPE_NUM)
		{
			y = (y == 1)?0:1;
		}
		else if(type == VIRTUAL_KEYPAD_TYPE_ALPHA)
		{
			if(x > 4)
			{
				if(y == 9)
					y = 0;
				else
					y++;
			}
			else
			{
				if(y == 10)
					y = 0;
				else
					y++;
			}

			if(y == 0)
			{
				start_y = 0;
			}
			else
			{
				if((y - start_y)>2)  
					start_y ++;
			}
		}
	}
	else if (key == KEY_LEFT)
	{
		if (type == VIRTUAL_KEYPAD_TYPE_NUM)
		{
			if(x == 0)
			{
				y = (y == 1)?0:1;
				x = 4;
			}
			else
			{
				x--;
			}
		}
		else if(type == VIRTUAL_KEYPAD_TYPE_ALPHA)
		{
			if(x == 0)
			{
				if(y == 0)
				{
					y = 10;
					x = 4;
				}
				else
				{
					y--;
					x = 8;
				}
			}
			else
			{
				x--;
			}

			if((start_y == 0)&&(y == 10))
			{
				start_y = 8;
			}
			else
			{
				if(y < start_y)  
					start_y --;
			}
		}
	}
	else if (key == KEY_RIGHT)
	{
		if (type == VIRTUAL_KEYPAD_TYPE_NUM)
		{
			if(x == 4)
			{
				y = (y == 1)?0:1;
				x = 0;
			}
			else
			{
				x++;
			}
		}
		else if(type == VIRTUAL_KEYPAD_TYPE_ALPHA)
		{
			if((x == 8)&&(y<10))
			{
				x = 0;
				y++;
			}
			else
			{
				if((x == 4)&&(y == 10))
				{
					x = 0;
					y = 0;
				}
				else
				{
					x++;
				}
			}

			if(y == 0)
			{
				start_y = 0;
			}
			else
			{
				if((y - start_y)>2)  
					start_y ++;
			}
		}
	}
	else if (key == KEY_ENTER || key == KEY_SCAN)
	{
		if (type == VIRTUAL_KEYPAD_TYPE_NUM)
		{
			str[0] = key_array_1[y][x];
		}
		else if(type == VIRTUAL_KEYPAD_TYPE_ALPHA)
		{
			str[0] = key_array_2[y][x];
		}

		str[1] = 0;
		edit_stoe(str,1,tmp);
		return tmp[0];
	}

	if (type == VIRTUAL_KEYPAD_TYPE_NUM)
	{
		for (i = 0; i < 10;i++)
		{
			str[0] = key_array_1[i/5][i%5];
			str[1] = 0;
			if ((5*y+x) == i)
			{
				gui_SetTextColor(BG_COLOR);
			}
			else
			{
				gui_SetTextColor(TXT_COLOR);
			}
			gui_TextOut((i%5)*12+6,13*((i/5) + 1),str,0,0);
		}

		gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	}
	else if (type == VIRTUAL_KEYPAD_TYPE_ALPHA)
	{
		for(i = 0; i < 3;i++)
		{
			for(j = 0; j < 9;j++)
			{
				str[0] = key_array_2[start_y+i][j];
				str[1] = 0;
				if ((start_y+i) == y && j == x)
				{
					gui_SetTextColor(BG_COLOR);
				}
				else
				{
					gui_SetTextColor(TXT_COLOR);
				}
				gui_TextOut(j*12+6,13*(i + 1),str,0,0);
			}
		}

		gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	}
	return 0;
}
//
/**
* @brief edit处理一个按键消息
* @note 输入后，EditValue中填的是键值，而不是ascii，显示时需要转换
* @param[in] int key输入的键值,其中:
*            KEY_CLEAR 删除前一个输入字符
*            KEY_ENTER 刷新显示
*            KEY_NUM0..KEY_NUM9 输入
*/
static void edit_proc(TEdit *pEdit, unsigned char key,unsigned char virtual_keypad_flag)
{
	TEditAttribute					edit_attr;			//当前编辑框的属性
	unsigned char					current_edit;
	unsigned char					if_update_cursor;
	unsigned char					key_code[11];
	unsigned char					i;

	current_edit = pEdit->current_edit;
	

	edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性

	if((edit_attr.style & 0x07) == EDIT_IP)				/// 顺序排列 0.1.2.3.4.5.6
	{
		///STEP1:逻辑处理
		if((edit_attr.style & EDIT_READONLY)==0)
		{
			if(key==KEY_CLEAR)
			{
				if(pEdit->now[current_edit])
				{
					if (pEdit->now[current_edit] == 4 || pEdit->now[current_edit] ==8 || pEdit->now[current_edit] == 12)
					{
						pEdit->now[current_edit]--;
						g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
					}
					pEdit->now[current_edit]--;
					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
				}
			}
			else if (KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key)
			{

			}
			else if (KEY_FUN4 == key)
			{
				pEdit->now[current_edit] = 0;
				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
			}
			else
			{
				/// 允许输入
				if(pEdit->now[current_edit] < pEdit->input_max[current_edit])	
					g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
				if (pEdit->now[current_edit] == 3 || pEdit->now[current_edit] == 7 || pEdit->now[current_edit] == 11)
				{
					g_editValue[current_edit][pEdit->now[current_edit]++]	= KEY_juhao;
				}
			}
		}
	}
	else if(((edit_attr.style & 0x07) == EDIT_PSW)||((edit_attr.style & 0x07) == EDIT_PSW_S))
	{
		///STEP1:逻辑处理
		if((edit_attr.style & EDIT_READONLY)==0)
		{
			if(key==KEY_CLEAR)
			{
				if(pEdit->now[current_edit])
				{
					pEdit->now[current_edit]--;
					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
				}
			}
			else if (KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key)
			{

			}
			else if (KEY_FUN4 == key)
			{
				pEdit->now[current_edit] = 0;
				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
			}
			else
			{
				/// 允许输入
				if(pEdit->now[current_edit] < pEdit->input_max[current_edit])
					g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
			}
		}
	}
	else if((edit_attr.style & 0x07) == EDIT_NUM)		/// 倒序排列 6.5.4.3.2.1.0
	{
		/// STEP1:逻辑处理
		if((edit_attr.style & EDIT_READONLY)==0)
		{
			if(key==KEY_CLEAR)
			{
				if(pEdit->now[current_edit])
				{
					pEdit->now[current_edit]--;
					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
				}
			}
			else if (KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key)
			{

			}
			else if (KEY_FUN4 == key)
			{
				pEdit->now[current_edit] = 0;
				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
			}
			else
			{
				//if (number_edit_patch)
				//{
				//	if(KEY_xing == key)
				//	{
				//		key = KEY_juhao;
				//	}
				//	if(pEdit->now[current_edit] < pEdit->max[current_edit])
				//		g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
				//}
				//else
				{
					if ((KEY_xing != key) && (KEY_jing != key))
					{
						if ((KEY_NUM1 == key)&&(number_edit_patch))
						{
							if ((key == last_input_key)&&(input_update_flag < 3)&&(pEdit->now[current_edit]>1))
							{
								same_key_times++;
								if ((same_key_times%2) == 1)
								{
									if (pEdit->now[current_edit] > 1)
									{
										for(i = 0;i < pEdit->now[current_edit];i++)
										{
											if (g_editValue[current_edit][i] == KEY_juhao)
											{
												break;
											}
										}
									}

									if (i == pEdit->now[current_edit])
									{
										g_editValue[current_edit][pEdit->now[current_edit]-1]	= KEY_juhao;
									}
									else
									{
										same_key_times = 0;
										if(pEdit->now[current_edit] < pEdit->input_max[current_edit])
											g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
									}
								}
								else
								{
									g_editValue[current_edit][pEdit->now[current_edit]-1]	= KEY_NUM1;
								}
							}
							else
							{
								same_key_times = 0;
								if(pEdit->now[current_edit] < pEdit->input_max[current_edit])
									g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
							}
						}
						else
						{
                            same_key_times = 0;
							if(pEdit->now[current_edit] < pEdit->input_max[current_edit])
								g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
						}
						
					}
				}
				
			}
		}
	}
	else if((edit_attr.style & 0x07) == EDIT_MONEY)		// 倒序排列
	{
		// STEP1:逻辑处理
		if((edit_attr.style & EDIT_READONLY)==0)
		{
			if(key==KEY_CLEAR)			// $ 12345.67
			{
				if(pEdit->now[current_edit])
				{
					pEdit->now[current_edit]--;
					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
				}
			}
			else if (KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key)
			{

			}
			else if (KEY_FUN4 == key)
			{
				pEdit->now[current_edit] = 0;
				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
			}
			else
			{
				if((pEdit->now[current_edit] == 1) && (g_editValue[current_edit][0] == '0') && (key != KEY_xing))
				{
					// 如果编辑框的第一个是0，那么第二个输入的按键除了小数点之外，其余都需要取代第一个0
					g_editValue[current_edit][0] = *key2ascii(key);
				}
				else if (key == KEY_xing)
				{
					if (pEdit->now[current_edit] == 0)
					{
						g_editValue[current_edit][0] = *key2ascii(KEY_NUM0);
						g_editValue[current_edit][1] = *key2ascii(KEY_juhao);
						pEdit->now[current_edit] = 2;
					}
					else
					{
						for (i = 0;i < pEdit->now[current_edit];i++)
						{
							if (g_editValue[current_edit][i] == '.')
							{
								break;
							}
						}

						if (i == pEdit->now[current_edit])
						{
							//表明前面没有输入了小数点，可以继续输入小数点了
							if(pEdit->now[current_edit] < pEdit->input_max[current_edit])			// 扣除一位小数点,能存9位数字
								g_editValue[current_edit][pEdit->now[current_edit]++] = *key2ascii(KEY_juhao);
						}
					}
				}
				else
				{
					if(pEdit->now[current_edit] < pEdit->input_max[current_edit])			// 扣除一位小数点,能存9位数字
						g_editValue[current_edit][pEdit->now[current_edit]++] = *key2ascii(key);
				}
			}
		}
	}
	else if ((edit_attr.style & 0x07) == EDIT_ALPHA)	//定义为ALPHA属性的编辑框，可以使用三种输入法输入
	{
		if((edit_attr.style & EDIT_READONLY)==0)
		{
			if(key==KEY_CLEAR)
			{
				if(pEdit->now[current_edit])
				{
					pEdit->now[current_edit]--;
					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
				}
			}
			else if (KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key)
			{

			}
			else if (KEY_FUN4 == key)
			{
				pEdit->now[current_edit] = 0;
				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
			}
			else
			{
				/// 允许输入
				if((pEdit->now[current_edit] < pEdit->input_max[current_edit]) || (1 == the_last_key))
				{
					if_update_cursor = edit_alpha_proc(key,input_method,key_code);
					if (0 == if_update_cursor)
					{
						pEdit->now[current_edit] --;
					}
					else 
					{
						if(1 == the_last_key)
						{
							the_last_key = 0;
							return;
						}
					}
					strcpy((char*)&g_editValue[current_edit][pEdit->now[current_edit]],(char const*)key_code);
					pEdit->now[current_edit] ++;
					if (pEdit->now[current_edit] == pEdit->input_max[current_edit])
					{
						//输到最后一位了
						the_last_key = 1;		//标记输入了最后一位，处于等待按同一个键切换的状态
					}
				}
			}
		}
	}
	//else if ((edit_attr.style & 0x07) == EDIT_CHINESE)		///定义为CHINESE属性的编辑框，可以使用四种输入法输入
	//{
	//	if((edit_attr.style & EDIT_READONLY)==0)
	//	{
	//		//if ((edit_state == EDIT_STATE_ZD_NAME_SELECT)||(edit_state == EDIT_STATE_ZD_VALUE_SELECT))
	//		if (edit_state == EDIT_STATE_ZD_VALUE_SELECT)
	//		{
	//			//进入字典输入功能的处理
	//			edit_ZD_proc(key,key_code);
	//			if (edit_state == EDIT_STATE_CODE_GOT)
	//			{
	//				if (pEdit->now[current_edit] + strlen((char const*)key_code) <= pEdit->input_max[current_edit])
	//				{
	//					strcpy((char*)&g_editValue[current_edit][pEdit->now[current_edit]],(char const*)key_code);
	//					pEdit->now[current_edit] += strlen((char const*)key_code);
	//				}
 //                   input_method = PINYIN_INPUT;
	//			}
	//		}
	//		else
	//		{
	//			if ((EDIT_STATE_INIT_STATUS == edit_state || EDIT_STATE_CODE_GOT == edit_state)&&(KEY_CLEAR == key))
	//			{
	//				//在没有进入中文输入状态时按取消键，清除前面输入的键值
	//				if(pEdit->now[current_edit])
	//				{
	//					if (g_editValue[current_edit][pEdit->now[current_edit]-1] < 0x80)
	//					{
	//						//说明前面输入的是一个ASCII字符
	//						pEdit->now[current_edit]--;
	//					}
	//					else
	//					{
	//						//说明前面输入的是一个汉字，需要减去2个编码
	//						pEdit->now[current_edit] -= 2;
	//					}
	//					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
	//				}
	//			}
	//			else if ((EDIT_STATE_INIT_STATUS == edit_state || EDIT_STATE_CODE_GOT == edit_state)&&(KEY_FUN4 == key))
	//			{
	//				//在没有进入中文输入状态时按F4键，清除前面所有输入的键值
	//				pEdit->now[current_edit] = 0;
	//				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
	//			}
	//			else if ((EDIT_STATE_INIT_STATUS == edit_state)&&(KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key))
	//			{
	//				//非中文输入法状态时，KEY_ENTER、KEY_UP、KEY_DOWN、KEY_ESC都是不产生任何作用的
	//				//什么都不做
	//			}
	//			else
	//			{
	//				if ((input_method == PINYIN_INPUT)&&((pEdit->now[current_edit]+1) == pEdit->input_max[current_edit]))
	//				{
	//					//最后只剩下一个字符的位置时，不能再输入中文了
	//				}
	//				else if((pEdit->now[current_edit] < pEdit->input_max[current_edit]) || (key == KEY_CLEAR) || (1 == the_last_key))
	//				{
	//					if_update_cursor = edit_chinese_proc(key,input_method,key_code);
	//					if ((edit_state == EDIT_STATE_CODE_GOT)&&(key != KEY_CLEAR))
	//					{
	//						//如果已经获取到键值的编码,直接将键值对应的可显示编码COPY到保存键值的缓冲区
	//						if (0 == if_update_cursor)
	//						{
	//							pEdit->now[current_edit] -= strlen((char const*)key_code);
	//						}
	//						else 
	//						{
	//							if(1 == the_last_key)
	//							{
	//								the_last_key = 0;
	//								return;
	//							}
	//						}
	//						strcpy((char*)&g_editValue[current_edit][pEdit->now[current_edit]],(char const*)key_code);
	//						pEdit->now[current_edit] += strlen((char const*)key_code);

	//						if ((pEdit->now[current_edit] == pEdit->input_max[current_edit])&&(input_method != PINYIN_INPUT))
	//						{
	//							//输到最后一位了
	//							the_last_key = 1;		//标记输入了最后一位，处于等待按同一个键切换的状态
	//						}
	//					}

	//					if (input_method != PINYIN_INPUT)
	//					{
	//						edit_state = EDIT_STATE_INIT_STATUS;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}


	last_input_key = key;

	if ((edit_state != EDIT_STATE_PY_SELECT)&&(edit_state != EDIT_STATE_HZ_SELECT)&&(edit_state != EDIT_STATE_ZD_NAME_SELECT)&&(edit_state != EDIT_STATE_ZD_VALUE_SELECT))
	{
		//处于这两个状态下，不需要刷新编辑框
		if (virtual_keypad_flag)
		{
			edit_refresh_ext(pEdit);
		}
		else
		{
			edit_refresh(pEdit,1,current_edit);
		}
	}

}

/**
* @brief 处理编辑框光标
*/
static void edit_blink(TEdit *pEdit,unsigned char virtual_keypad_flag)
{
	TEditAttribute					edit_attr;			//当前编辑框的属性
	unsigned char					current_edit;
	unsigned char cur_pos;
	unsigned short					i,x,y;
	//unsigned short		patch = 0;
	unsigned char		offset;

	current_edit = pEdit->current_edit;

	if (pEdit->now[current_edit] < pEdit->display_max[current_edit])
	{
		cur_pos = pEdit->now[current_edit];
	}
	else
	{
		cur_pos = pEdit->display_max[current_edit];	
		offset = pEdit->now[current_edit] - pEdit->display_max[current_edit];

		for (i=0;i<offset;i++)
		{
			if (g_editValue[current_edit][i] > 0x80)
			{
				i++;
			}
		}

		if (i>offset)
		{
			cur_pos--;
		}
	}
	
	
	edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性
	//if ((edit_attr.style & 0x80) == EDIT_MUL_LINE)
	//{
	//	patch = edit_display_x_patch;
	//}
	if (virtual_keypad_flag)
	{
		edit_attr.x = 0;
		edit_attr.y = 0;
	}
	x = edit_attr.x + cur_pos*gFontSize/2 + pEdit->display_patch[current_edit];
	y = edit_attr.y;


	if (edit_attr.y + 13 *((pEdit->display_max[current_edit]-1)/20) > GUI_HEIGHT - SHOTCUT_HEIGNT - 12)
	{
		//编辑框的纵坐标大于显示区域
		y = ICON_HEIGHT + 18 + edit_attr.y - (GUI_HEIGHT - SHOTCUT_HEIGNT );
	}

	while(x >= GUI_WIDTH)
	{
		x -= GUI_WIDTH;
		y += 13;
	}

	if(((edit_attr.style & 0x10) != EDIT_NO_BLINK)&&(edit_state == EDIT_STATE_CODE_GOT || edit_state == EDIT_STATE_INIT_STATUS))
	{
		//需要闪烁光标的编辑框
		if(pEdit->blink)
		{
			// 清光标
			pEdit->blink		= 0;
			//gui_FillRect(x,y,pEdit->view[current_edit].font_size/6,pEdit->view[current_edit].font_size-2,BG_COLOR);
			gui_LineV(x,y+1,11,BG_COLOR);
		}
		else
		{
			// 画光标
			pEdit->blink		= 1;
			//gui_FillRect(x,y,pEdit->view[current_edit].font_size/6,pEdit->view[current_edit].font_size-2,TXT_COLOR);
			gui_LineV(x,y+1,11,TXT_COLOR);
		}
	}
	
}


/**
* @brief 是否包含处于激活状态的编辑框
* @return 0:不包含   1:包含
*/
static int edit_is_active(TEdit *pEdit)
{
	unsigned char	i;
	for (i = 0; i < MAX_EDIT;i++)
	{
		if (pEdit->active[i] != 0)
		{
			return 1;
		}
	}

	return 0;
}

/**
* @brief 使能某一个编辑框
*/
void edit_enable(TEdit *pEdit,int index)
{
	if (index > 7)
	{
		return;
	}

	pEdit->active[index] = 1;
}

/**
* @brief 使某一个编辑框失效
*/
void edit_disable(TEdit *pEdit,int index)
{
	if (index > 7)
	{
		return;
	}

	pEdit->active[index] = 0;
}

/**
* @brief 输入法的切换
*/
static void input_method_switch(TEdit *pEdit)
{
	TEditAttribute					edit_attr;			//当前编辑框的属性

	edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性
	
	input_method++;

	if ((edit_attr.style & 0x07) == EDIT_ALPHA)
	{
		if (input_method > _123_INPUT)
		{
			input_method = ABC_INPUT;
		}
	}
	else if ((edit_attr.style & 0x07) == EDIT_CHINESE)
	{
		if (input_method > _123_INPUT)
		{
			if (LANG_ENGLISH == g_language)
			{
				input_method = ABC_INPUT;
			}
			else
			{
				input_method = PINYIN_INPUT;
			}
		}
	}
	else
	{
		//其余属性的编辑框，输入法不允许切换
		input_method--;
	}

	input_update_flag = 3;
	return;
}

/**
* @brief 光标在编辑框之间的切换
*/
void edit_switch(TEdit *pEdit,unsigned char key)
{
	unsigned char					i;
	unsigned char					current_edit;
	TEditAttribute					edit_attr;			//当前编辑框的属性
	unsigned char cur_pos,offset;

	current_edit = pEdit->current_edit;
	if (pEdit->now[current_edit] < pEdit->display_max[current_edit])
	{
		cur_pos = pEdit->now[current_edit];
	}
	else
	{
		cur_pos = pEdit->display_max[current_edit];	
		offset = pEdit->now[current_edit] - pEdit->display_max[current_edit];

		for (i=0;i<offset;i++)
		{
			if (g_editValue[current_edit][i] > 0x80)
			{
				i++;
			}
		}


		if (i>offset)
		{
			cur_pos--;
		}
	}

	//需要先清除原来的光标
	edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性

	if((edit_attr.style & 0x07) == EDIT_MONEY)
	{
		cur_pos = pEdit->display_max[current_edit]-1;
	}

	if((edit_attr.style & 0x10) != EDIT_NO_BLINK)
	{
		// 清光标
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-3 , gFontSize/2, BG_COLOR);
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-2 , gFontSize/2, BG_COLOR);
		//gui_FillRect(edit_attr.x + cur_pos*gFontSize/2,edit_attr.y,gFontSize/6,gFontSize-2,BG_COLOR);
		//gui_ClearLineV(edit_attr.x+cur_pos*8,edit_attr.y+1,11,TXT_COLOR);
		if (edit_attr.y + 13 * ((pEdit->display_max[current_edit] - 1)/20) > GUI_HEIGHT - SHOTCUT_HEIGNT - 12)
		{
			//编辑框的纵坐标大于显示区域
			edit_attr.y = ICON_HEIGHT + 18 + edit_attr.y - (GUI_HEIGHT - SHOTCUT_HEIGNT );
		}
                  
         gui_LineV(edit_attr.x+cur_pos*gFontSize/2+pEdit->display_patch[current_edit],edit_attr.y+1,11,BG_COLOR);
	}


	if (KEY_UP == key)
	{
		//上翻键
		for(i = 0;i<all_edit;i++)
		{
			if (current_edit == 0)
			{
				current_edit = all_edit - 1; 
			}
			else
			{
				current_edit--;
			}

			if (pEdit->active[current_edit] != 1)
			{
				if (current_edit == 0)
				{
					current_edit = all_edit - 1; 
				}
				else
				{
					current_edit--;
				}

				if (pEdit->active[current_edit] == 1)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	if (KEY_DOWN == key)
	{
		//下翻键
		for(i = 0;i<all_edit;i++)
		{
			if (current_edit == all_edit - 1)
			{
				current_edit = 0; 
			}
			else
			{
				current_edit++;
			}

			if (pEdit->active[current_edit] != 1)
			{
				if (current_edit == all_edit - 1)
				{
					current_edit = 0; 
				}
				else
				{
					current_edit++;
				}

				if (pEdit->active[current_edit] == 1)
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	pEdit->current_edit = current_edit;
	pEdit->blink = 0;		//立即绘制新编辑框的光标
	last_input_key = 0;		//最近输入的按键清0
	return;
}

/**
* @brief 光标直接跳转到指定的编辑框
*/
void edit_switch_ext(TEdit *pEdit,unsigned char edit_index)
{
	unsigned char					i;
	unsigned char					current_edit;
	TEditAttribute					edit_attr;			//当前编辑框的属性
	unsigned char cur_pos,offset;

	current_edit = pEdit->current_edit;

	if (pEdit->now[current_edit] < pEdit->display_max[current_edit])
	{
		cur_pos = pEdit->now[current_edit];
	}
	else
	{
		cur_pos = pEdit->display_max[current_edit];	
		offset = pEdit->now[current_edit] - pEdit->display_max[current_edit];
		for (i=0;i<offset;i++)
		{
			for (i=0;i<offset;i++)
			{
				if (g_editValue[current_edit][i] > 0x80)
				{
					i++;
				}
			}

			if(i>offset)
			{
				offset++;
			}
		}

		if (i>offset)
		{
			cur_pos--;
		}
	}
	
	//需要先清除原来的光标
	edit_get_attribute(pEdit,&edit_attr);		//获取当前编辑框的相关属性

	if((edit_attr.style & 0x07) == EDIT_MONEY)
	{
		cur_pos = pEdit->display_max[current_edit]-1;
	}

	if((edit_attr.style & 0x10) != EDIT_NO_BLINK)
	{
		// 清光标
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-3 , gFontSize/2, BG_COLOR);
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-2 , gFontSize/2, BG_COLOR);
		//gui_FillRect(edit_attr.x + cur_pos*gFontSize/2,edit_attr.y,gFontSize/6,gFontSize-2,BG_COLOR);
		gui_LineV(edit_attr.x+cur_pos*gFontSize/2,edit_attr.y+1,11,BG_COLOR);
	}

	pEdit->current_edit = edit_index;
	pEdit->blink = 0;		//立即绘制新编辑框的光标
	last_input_key = 0;		//最近输入的按键清0
	return;
}
/**
* @brief edit设置显示的字符,将pValue设置到g_editValue中
*/
void edit_setvalue(TEdit *pEdit, unsigned char *pValue,unsigned char edit_index)
{
	int						i;
	if (edit_index > (MAX_EDITNUM - 1))
	{
		return;
	}
	pEdit->now[edit_index] = 0;
	memset(g_editValue[edit_index],0,MAX_EDITNUM);

	for(i=0; i<pEdit->input_max[edit_index] && pValue[i]!=0x00; i++)
	{
		g_editValue[edit_index][i]		= pValue[i];
		pEdit->now[edit_index]++;
	}
	g_editValue[edit_index][i]			= 0x00;
}

/**
* @brief 将edit中的值转成整型
* @note 注意edit中的值，是键值而不是ascii
*/
int edit_atoi(unsigned char *pValue)
{
	int						i;
	int						value = 0;

	for(i=0; ;i++)
	{
		if( *pValue == 0x00 )
			return value;
		value				*= 10;
		value				+= *(key2ascii(*pValue)) - '0';
		pValue ++;
	}
	//return value;
}


/**
* @brief 将edit中的值转成放大100倍之后的整数，为了支持小数点后2位
* @note 注意edit中的值，是键值而不是ascii
*/
int edit_atoi_100X(unsigned char *pValue)
{
	int						i;
	int						value = 0;
	unsigned char			dot_pos = 0;

	for(i=0; ;i++)
	{
		if ((i == dot_pos + 3)&&(dot_pos != 0))
		{
			return value;
		}
		if( *pValue == 0x00 )
		{
			if ((i == dot_pos + 2)&&(dot_pos != 0))
			{	
				return value*10;
			}
			return value*100;
		}

		if (*pValue == KEY_juhao)
		{
			dot_pos = i;
			pValue++;
			continue;
		}

		value				*= 10;
		value				+= *(key2ascii(*pValue)) - '0';
		pValue ++;
	}
	//return value;
}

/**
* @brief 将edit中的值转成浮点数
* @note 注意edit中的值，是键值而不是ascii
*/
float edit_atof(unsigned char *pValue)
{
#if 0
	int						i;
	float					value = 0;
	float					q;
	unsigned char			got_dot_flag = 0;

	//input_valid_num = 4;		//默认保留小数点后4位
	for(i=0; ;i++)
	{
		if( *pValue == 0x00 )
		{
			return value;
		}
		if (*pValue != KEY_juhao)
		{
			if (got_dot_flag == 0)
			{
				value				*= 10;
				value				+= *(key2ascii(*pValue)) - '0';
			}
			else
			{
				value += (*(key2ascii(*pValue)) - '0')*q;
				q *= 0.1;
			}
			
		}
		else
		{
			got_dot_flag = 1;
			q = 0.1;
		}
		pValue ++;
	}
#endif
	
	int						i;
	int						zs = 0;
	int						xs = 0;
	float					value = 0;
	unsigned char			xs_len = 0;
	unsigned char			got_dot_flag = 0;


	for(i=0; ;i++)
	{
		if( *pValue == 0x00 )
		{
			while (xs_len)
			{
				value += (float)(xs%10)/exp10(xs_len);
				xs /= 10;
				xs_len--; 
			}
			
			return (value+zs);
		}

		if (*pValue != KEY_juhao)
		{
			if (got_dot_flag)
			{
				xs *= 10;
				xs += *(key2ascii(*pValue)) - '0';
				xs_len++;
			}
			else
			{
				zs *= 10;
				zs += *(key2ascii(*pValue)) - '0';
			}
		}
		else
		{
			got_dot_flag = 1;
		}
		pValue ++;
	}
}

/**
* @brief 将整型转成键值，放到编辑框里
*/
unsigned char edit_itoa(unsigned int value, unsigned char *pValue)
{
	unsigned char						i,j;
	unsigned char						tmp[10];

	if( 0 == value)
	{
		pValue[0] = i2key(0);
		pValue[1] = 0;
		return 1;
	}
	else
	{
		for(i=0; value;i++)
		{
			tmp[i]			= i2key( value % 10);
			value				/= 10;
		}

		for (j = 0;j < i;j++)
		{
			pValue[j] = tmp[i-1-j];
		}

		pValue[j] = 0;
	}

	return i;
}

/**
* @brief 将被放大100倍的整型转成键值，放到编辑框里
*/
unsigned char edit_itoa_100X(unsigned int value, unsigned char *pValue)
{
	unsigned char						i,j;
	unsigned char						tmp[10],buf[10];

	if( 0 == value)
	{
		pValue[0] = i2key(0);
		pValue[1] = 0;
		return 1;
	}
	else
	{
		if (value%100)
		{
			for(i=0; value;i++)
			{
				tmp[i]			= i2key( value % 10);
				value				/= 10;
			}

			if (i == 1)
			{
				tmp[i++] = KEY_NUM0;
				tmp[i++] = KEY_juhao;
				tmp[i++] = KEY_NUM0;
			}
			else if (i == 2)
			{
				tmp[i++] = KEY_juhao;
				tmp[i++] = KEY_NUM0;
			}
			else
			{
				memcpy(buf,tmp+2,8);
				tmp[2] = KEY_juhao;
				memcpy(tmp+3,buf,7);

				i += 1;
			}
		}
		else
		{
			value /=100;
			for(i=0; value;i++)
			{
				tmp[i]			= i2key( value % 10);
				value				/= 10;
			}
		}

		for (j = 0;j < i;j++)
		{
			pValue[j] = tmp[i-1-j];
		}

		pValue[j] = 0;
	}

	return i;
}

/**
* @brief 将浮点数转成键值，放到编辑框里
*/
//unsigned char edit_ftoa(float value, unsigned char *pValue)
//{
//
//}
/**
* @brief 将字符串，转为edit中的值（键值）
*/
void edit_stoe(unsigned char *indata, unsigned int inlen,unsigned char *outdata)
{
	unsigned int						i;
	unsigned int						j;

	for(i=0; i<inlen; i++)
	{
		j = 0;
		while (keyValue_Ascii_Tbl[j].asciivalue != 0) 
		{
			if (indata[i] == *(keyValue_Ascii_Tbl[j].asciivalue)) 
			{
				outdata[i] = keyValue_Ascii_Tbl[j].keyvalue;
				break;
			}
			j++;
		}
	}
	outdata[inlen]			= 0;

}

/**
* @brief 将edit中的转为字符串
*/
void edit_etos(unsigned char *indata, unsigned char *outdata, int len)
{
	//	unsigned char			c;
	int						i;

	for(i=0; i<len; i++)
	{       
		outdata[i] = *(key2ascii(indata[i]));
	}
	outdata[len] = 0x00;		// 结束符
}

/**
* @brief 从edit中取得现有值
*/
void edit_getvalue(TEdit *pEdit, unsigned char *pValue)
{
	int						i;

	for(i=0; i<pEdit->now[pEdit->current_edit]; i++)
	{
		pValue[i]			= g_editValue[pEdit->current_edit][i];
	}
	pValue[i]				= 0x00;
}
/**
***************************************************************************
*@brief	获取当前语言的内容字符串,unicode编码
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
static unsigned char **get_content(void)
{
	TDialogRes						* res;
	res								= g_dlg.pRes;

	if(g_language == 0)
		return res->pContent_en;
	else if(g_language == 1)
		return res->pContent_scn;
	else
		return res->pContent_tcn;
}
/**
* @brief 更新显示时间,每隔一秒由中断调用
* @param[in] xpos
* @param[in] ypos
* @note 时间的显示颜色反显了，所以显示时间的位置必须以深色为背景的区域
* @return none
*/
void dlg_updatetime(unsigned short xpos,unsigned short ypos)
{
	unsigned char		str[6];
	GetDateTime();
	sprintf((char*)str,"%02d:%02d",currentDateTime.hour,currentDateTime.min);
	if (need_refresh_signal)
	{
		gui_TextOut_ext(CENTER_ALIGN, ypos,str,0,1);
	}
	else
	{
		gui_TextOut(xpos, ypos,str,0,1);
	}
}
/**
* @brief 注册进度条
* @param[in] unsigned char style	进度条的类型
* @param[in] unsigned short x		进度条的坐标x
* @param[in] unsigned short y		进度条的坐标y
* @param[in] unsigned short max		进度条的最大值
*/
void process_bar_regist(unsigned char style,unsigned short x,unsigned short y,unsigned short max)
{
#if 0
	process_bar.style = style;
	process_bar.pos_x = x;
	process_bar.pos_y = y;
	process_bar.max_value = max;
	process_bar.current_value = 0;
	process_bar.active = 1;
#endif
}

/**
* @brief 注销进度条
*/
void process_bar_cancle(void)
{
	//process_bar.active = 0;
}

/**
* @brief 更新进度条
*/
void process_bar_update(void)
{
#if 0
	int i;

	if (process_bar.current_value < 0xFFFF)
	{
		process_bar.current_value ++;
	}
	else
	{
		process_bar.current_value =  0;
	}
	
	if (process_bar.style == PROCESS_BAR_STYLE_1)
	{
		//形态1的进度条,同最大值无关
		//for (i = 0;i < 4;i++)
		//{
		//	gui_DrawRect(process_bar.pos_x+i*12,process_bar.pos_y,4,8,TXT_COLOR);
		//}

		i = process_bar.current_value%5;
		if (i == 0)
		{
			gui_FillRect(process_bar.pos_x,process_bar.pos_y,4*10,4,BG_COLOR);
		}
		else
		{
			gui_FillRect(process_bar.pos_x+(i-1)*8,process_bar.pos_y,4,4,TXT_COLOR);
		}
	}
	else if (process_bar.style == PROCESS_BAR_STYLE_2)
	{
		//@todo...
	}
#endif 
}

/**
* @brief 更新图标,GPRS信号强度，电池电量，充电提示，通信状态
* @param[in] int mode 0:不需要取实际的网络信号强度    1:需要取实际的网络信号强度
* @return 0:不需要重新绘制原界面  1:低电事件    2:重新绘制原界面
* @note 因为在显示开机画面时如果也需要取实际的网络信号强度的话，可能因为GPRS模块的响应比较慢，导致开机显示主界面时
*		 会有一个明显的停顿感
*/
int dlg_update_icon(int mode,unsigned char refresh_flag)
{
	int	powerlevel;
	int csq;
	int i,needpoweroff = 0;
	unsigned char * key;
	unsigned int	lowpower_cnt = 0;

	//if (need_refresh_signal)
	//{
	//	
	//}
	if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	{

		gui_PictureOut(24,0,PIC_BLUETOOTH,refresh_flag);

		if (YFBT07_Connect_state())
		{	
			//连接状态
			bluetooth_match_cmd_cnt = 0;
			gui_PictureOut(0,0,PIC_ANT5,refresh_flag);
		}
		else
		{
			gui_PictureOut(0,0,PIC_ANT1,refresh_flag);
			//如果是处于未连接状态，那么就间隔2分钟发送一次匹配命令
			if (bluetooth_module_state == 0)
			{
				bluetooth_state_cnt = 0;
				if (bluetooth_match_cmd_cnt == 60)
				{
					bluetooth_match_cmd_cnt = 0;
					YFBT07_Enter_Match_Mode();
				}
				if (bluetooth_match_cmd_cnt%8 == 0)
				{
					YFBT07_SendKey("\x0d",1);
				}
				bluetooth_match_cmd_cnt ++;
			}
			else
			{
				if(bluetooth_state_cnt == 20)
				{
					bluetooth_state_cnt = 0;
					bluetooth_module_state = YFBT07_check();
				}
				bluetooth_state_cnt++;
			}
		}
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_KEYBOARD)
	{
		if (bDeviceState == CONFIGURED)
		{
			gui_PictureOut(0,0,PIC_KEYPAD_C,refresh_flag);
		}
		else
		{
			gui_PictureOut(0,0,PIC_KEYPAD_D,refresh_flag);
		}
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	{
		if (bDeviceState == CONFIGURED)
		{
			gui_PictureOut(0,0,PIC_COMM_C,refresh_flag);
		}
		else
		{
			gui_PictureOut(0,0,PIC_COMM_D,refresh_flag);
		}
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_U_DISK)
	{
		gui_PictureOut(0,0,PIC_UDISK,refresh_flag);
	}


	// 显示电池电量
	if (USBLink_Insert_Detect())
	{		
		//没有插入外接USB电源
		powerlevel					= GetPowerVoltageClass();
		if (powerlevel == (last_power_level+1))
		{
			powerlevel = last_power_level;
		}
		last_power_level = powerlevel;
		//powerlevel = 4;
		if (powerlevel >= 1)
		{
			lowpower_detect_cnt = 0;
			lowpower_cnt = 0;
			lowpower_flag = 0;
		}
		switch(powerlevel)
		{
		case 4:	gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT0,refresh_flag);	break;
		case 3: gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT1,refresh_flag);	break;
		case 2: gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT2,refresh_flag);	break;
		case 1: gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT3,refresh_flag);	break;
		default:	//电池电量低
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4,refresh_flag);
			lowpower_detect_cnt++;
		}

		if ((lowpower_detect_cnt > 3)&&(lowpower_detect_cnt<8))
		{	
			//gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4);
			//Beep(BEEP_DELAY*2);
			//gui_FillRect(0, ICON_HEIGHT+1, GUI_WIDTH, USERZONE_HEIGHT, BG_COLOR);
			//gui_TextOut(16, 60,"系统电量不足...",0);
			//gui_TextOut(18, 72,"请保存当前数据!",0);
			//OSTimeDlyHMSM(0,0,1,0);
			lowpower_flag ++;
			return 1;
		}

		if (lowpower_detect_cnt >= 8)
		{
			OSSchedLock();		//不要再进行任务调度了
			while (USBLink_Insert_Detect())
			{
				lowpower_cnt++;
				gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4,refresh_flag);
				Beep(BEEP_DELAY*2);
				for (i = 0;i < 1000000;i++);		//延时一小段时间
				gui_FillRect(0, ICON_HEIGHT+1, GUI_WIDTH, USERZONE_HEIGHT, BG_COLOR,refresh_flag);
				gui_TextOut_ext(CENTER_ALIGN, 26,"系统电量不足...",0,refresh_flag);
				//@English
				gui_TextOut_ext(CENTER_ALIGN, 39,"请插入外部电源!",0,refresh_flag);
				//@English
				key = keypad_getkey();
				if ((*key) == KEY_POWER) 
				{
					needpoweroff++;	
				}
				else
				{
					needpoweroff = 0;
				}

				if (needpoweroff > 3) 
				{
					Lcd_Clear(BG_COLOR);
					gui_TextOut_ext(CENTER_ALIGN, 26, "POWER OFF",0,refresh_flag);                         
					while(*(keypad_getkey()) == KEY_POWER)
					{
						;
					}
					//释放POWER键之后，先清屏再关机
					Lcd_Clear(BG_COLOR);
					PowerOff();      ///关机
				}

				if (lowpower_cnt > 100)
				{
					//如果连续低电报警100次，还没有插入外接电源或者关机，那么就强制关机。
					Lcd_Clear(BG_COLOR);
					PowerOff();      ///关机
				}
			}
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT_SHAN,refresh_flag);	//显示充电图标
			OSSchedUnlock();		//任务调度解锁
			return 2;


		}
	}
	else
	{
		lowpower_flag = 0;
		//插入了USB外接电源
		if (ChargeState_Detect())		//充电完成
		{
                        charge_state_cnt = 0;
			gui_PictureOut(GUI_WIDTH-24,0, PIC_CHARGE,refresh_flag);		//显示外接电源图标 	
		}
		else		//正在充电
		{
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4,refresh_flag);
			OSTimeDlyHMSM(0,0,0,200);
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT_SHAN,refresh_flag);	//显示充电图标
		}
	}

	return 0;
}

/**
* @brief 显示对话框中编辑框的输入法ICON
*/
void show_input_method_icon(unsigned short x,unsigned short y,TEdit *pEdit,unsigned char mode)
{
	TEditAttribute		edit_attr;
	unsigned short		save_txt_color;


	save_txt_color =  gui_GetTextColor();
	if(0 == mode)
	{
		//第一次创建的编辑框时
		//gui_FillRect(x,y,12*6,16,BG_COLOR);
		last_input_method = 0;

		//2013-03-12，决定将所有编辑框的默认首选输入法都修改为数字输入法。

		//获取当前编辑框的属性
		//edit_get_attribute(pEdit,&edit_attr);
		//if((edit_attr.style & 0x07) == EDIT_CHINESE)
		//{
		//	//允许输入中文的编辑框需要切换输入法，pingyin、ABC、abc、123
		//	if (LANG_ENGLISH == g_language)
		//	{
		//		input_method = ABC_INPUT;
		//		if ((default_input_method_patch)&&(default_input_method_patch != PINYIN_INPUT))
		//		{
		//			input_method = default_input_method_patch;
		//		}
		//	}
		//	else
		//	{
		//		input_method = PINYIN_INPUT;
		//		if (default_input_method_patch)
		//		{
		//			input_method = default_input_method_patch;
		//		}
		//	}
		//}
		//else if((edit_attr.style & 0x07) == EDIT_ALPHA)
		//{
		//	//允许输入字母的编辑框需要切换输入法，ABC、abc、123
		//	input_method = ABC_INPUT;
		//	if (default_input_method_patch)
		//	{
		//		input_method = default_input_method_patch;
		//	}
		//}
		//else
		//{
		//	input_method = _123_INPUT;
		//}

		input_method = _123_INPUT;
	}


	if (last_input_method != input_method)
	{
		//if (PINYIN_INPUT == last_input_method)
		{
			gui_FillRect(x,y,25,12,BG_COLOR,1);
		}
		//gui_SetTextColor(BG_COLOR);
		//只有当上一次使用的输入法与当前输入法不同时才需要重新刷新输入法ICON
		switch (input_method)
		{
		case _123_INPUT:
			gui_TextOut(x,y,"123",1,1);
			break;
		case ABC_INPUT:
			gui_TextOut(x,y,"ABC",1,1);
			break;
		case abc_INPUT:
			gui_TextOut(x,y,"abc",1,1);
			break;
		case PINYIN_INPUT:
			//gui_TextOut(x,y,"PY",1);
			gui_TextOut(x,y,"拼音",1,1);
			break;
		default:
			break;
		}
		
		gui_SetTextColor(save_txt_color);
		last_input_method = input_method;
	}
	return;
}

/**
* @brief 将对话框pRes所属的菜单,从第index项开始显示出来
* @param[in] unsigned char highlight_current_item_flag  是否需要高亮当前选择项的标记
* @note 本函数不对指针进行检查
*/
void show_menu(unsigned char highlight_current_item_flag)
{
	TDialogRes					*pRes;
	unsigned char						i,tmp_line = 0;
    unsigned short                                    y;
	unsigned char			dis_str[17];	
        

	pRes						= g_dlg.pRes;
	pContent					= get_content(); //pRes->pContent;

	// 清空剩余空间
	tempy					= 0;
	if(g_dlg.pRes->style & STYLE_ICON || g_dlg.pRes->style & STYLE_TITLE)
	{
		tempy				+= ICON_HEIGHT+1;
	}

	y = tempy;

	gui_FillRect(0,tempy,GUI_WIDTH-24,user_zone_height, BG_COLOR,0);


	/** @note 要注意，内容只能在客户区域中输出，受CLIENT_HEIGHT和字体高度限制 */
	pContent					+= menu_start+1;		// 跳过第一行和第二行内容，第一行是标题，第二行是快捷键

	i = 0;
	memset((void*)display_menu_item_array,0,sizeof(display_menu_item_array));
	while(pContent[tmp_line] && (tempy + gFontSize<= CLIENT_HEIGHT - SHOTCUT_HEIGNT) )
	{
		memset(dis_str,0x20,16);
		dis_str[16] = 0;
		memcpy(dis_str,pContent[tmp_line],strlen(pContent[tmp_line]));
		if(((menu_current_item - menu_start) == tmp_line)&&(highlight_current_item_flag == 1))
		{
			gui_SetTextColor(BG_COLOR);		//当前选中的菜单反色显示
		}
		else
		{
			gui_SetTextColor(TXT_COLOR);
		}
		gui_TextOut(0, tempy, dis_str,0,0);
		display_menu_item_array[i++] = menu_start+tmp_line;
		tmp_line++;
		tempy					+= gFontSize+1;		//行距为1
	}

	//if (page_change_tag)
	//{
	//	//不考虑某一个界面需要显示两个箭头的情况，也就是说不支持某一个窗体需要分三屏显示
	//	if (menu_start == 2)
	//	{
	//		//需要显示下翻页的图标
	//		gui_PictureOut(56,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_DOWN);
	//	}
	//	else
	//	{
	//		//需要显示上翻页的图标
	//		gui_PictureOut(56,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_UP);
	//	}
	//}
	gui_SetTextColor(TXT_COLOR);
	gui_refresh(0,0,GUI_WIDTH+2,GUI_HEIGHT);
}

/**
***************************************************************************
*@brief	创建一个对话框
*@param[in] 
*@return 0 正确
*       -1 错误
*@warning
*@see	
*@note 
***************************************************************************
*/
static void dlg_create(void)
{
	if( now_id > max_dlg )
	{
		// 出错了，正常情况下id不可能超过 DLG_MAX
		// 这里添加错误时的提示代码
		while(1);

	}

	g_dlg.jmplst				= dlg_actionlist[now_id-1].jmplst;
	g_dlg.pRes					= dlg_actionlist[now_id-1].dlgres;
	g_dlg.callback				= dlg_actionlist[now_id-1].callback;
	g_dlg.active				= 1;			// 此域暂时没用
	g_dlg.name					= dlg_actionlist[now_id-1].name;

	memset((unsigned char*)&g_dlg.edit, 0, sizeof(TEdit));
}
/**
***************************************************************************
*@brief	检查是否需要跳转到其他状态
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
static int check_and_jump(unsigned char key)
{
	unsigned char					i;

	for( i=0; ;i++ )
	{
		if( g_dlg.jmplst[i].key == 0x00 )
		{
			return -1;
		}

		if( g_dlg.jmplst[i].key == key )
		{
			if( g_dlg.callback )
			{
				g_dlg.callback(DLGCB_DESTORY,&key);
			}
			return key;
		}
	}
}

/**
* @brief 运行一个对话框，并处理对话框绘制及按键等
* @param[in] TDialog *pDlg 要运行的对话框句柄
* @param[in] unsigned int mode
*        bit0   0:normal startup
*               1:startup without refresh
* @return 该对话框退出时返回的按键
*/
static unsigned char dlg_run(unsigned char mode)
{
	int						starty;
	int						ret;
	unsigned char			i,j,k;
	unsigned int			update_cursor_cnt = 0;
	unsigned short			op_timeout;		//背光关闭或者操作的延时
	unsigned char			*pKey, key;
	unsigned short			tmp_y;
	unsigned char			tmp_str[5];
	unsigned char			highlight_flag;	//显示菜单时是否需要高亮当前选择项的标志
	unsigned char			virtual_keypad_type;
	//unsigned char			virtual_keypad_buffer[31];
	unsigned char			return_from_edit_proc_state_flag;	//标记当前是从虚拟键盘跳回的状态标志
	static unsigned int			time_out;

	dlg_state						= DLG_INIT;
	//default_input_method_patch		= 0;
	number_edit_patch				= 0;
	highlight_flag	= 1;		//默认需要高亮，只有包含了编辑框的窗体不需要
	return_from_edit_proc_state_flag = 0;
	
	switch(g_param.operate_timeout)
	{
	case 1:	//30S
		time_out = 119; 
		break;
	case 2:	//1分钟
		time_out = 238;
		break;
	case 3: //2分钟
		time_out = 476;
		break;
	case 4: //5分钟
		time_out = 1190;
		break;
	default:	//10分钟
		time_out = 2380;
	}

	while(1)
	{
		switch(dlg_state)
		{
			/* *************************************************************************** */
			/* 对话框初始化部分 */
			/* *************************************************************************** */
		case DLG_INIT:

			// ************回调接口一************
			// 处理窗口创建前的事情
			// 如有些窗体经状态机跳转，则可以在此处直接返回
			update_icon_cnt = 0;
			task_exit_flag = 0;
			scan_start = 0;
			roll_display_item_cnt = 0;
			roll_update_cnt = 0;
			gui_SetTextColor(TXT_COLOR);
			if(g_dlg.callback)
			{
				// 此处执行窗口创建回调
				howrun				= (g_dlg.callback)(DLGCB_CREATE,&key);

				// 此处可能返回
				if(howrun == RUN_NEXT)
					return key;

				if(howrun == RUN_PREV)
					return KEY_ESC;

				if (howrun == RUN_EDIT_PROC)
				{
					dlg_state = DLG_EDIT_PROC1;
					break;
				}
			}
	
			starty					= 0;			// 输出的y坐标
			edit_state = EDIT_STATE_INIT_STATUS;	//初始化编辑框状态 
			user_zone_height = GUI_HEIGHT-SHOTCUT_HEIGNT;

			//如果窗体需要延时返回，那么此处标记
			if( g_dlg.pRes->delay )
			{
				g_dlg.delay				= g_dlg.pRes->delay;
			}
			else
			{
				g_dlg.delay				= -1;
			}

			op_timeout = 0;

			// ==========================================================================================================
			// 处理图标栏，固定长度
			if( g_dlg.pRes->style & STYLE_ICON )
			{
				starty				+= ICON_HEIGHT+1;
				user_zone_height -= ICON_HEIGHT+1;


				//如果前面已经有窗体绘制了图标栏就不需要刷新了
				if (0 == icon_draw_flag)
				{
					// fill title
					gui_FillRect(0,0,GUI_WIDTH,ICON_HEIGHT, BG_COLOR,0);
					gui_LineH(0,ICON_HEIGHT,GUI_WIDTH,TXT_COLOR,0);
					//Update ICON
					dlg_update_icon(0,0);
					//gui_refresh(0,0,GUI_WIDTH,ICON_HEIGHT);
					icon_draw_flag = 1;
				}

				//显示位于ICON栏的标题，标题固定可以显示4个汉字
				//pContent			= get_content();
				//gui_FillRect(0,0,12*5,12, BG_COLOR,1);
				//gui_TextOut(0, 0, pContent[0],0,1);			//标题栏的内容是窗体第一行内容
			}
			else
			{
				//没有图标栏的窗体，图标栏被清掉了
				icon_draw_flag = 0;

				if( g_dlg.pRes->style & STYLE_TITLE )
				{
					starty				+= ICON_HEIGHT+1;
					user_zone_height -= ICON_HEIGHT+1;
					gui_FillRect(0,0,GUI_WIDTH,ICON_HEIGHT, BG_COLOR,0);
					gui_LineH(0,ICON_HEIGHT,GUI_WIDTH,TXT_COLOR,0);
					pContent			= get_content();
					gui_TextOut(0, 0, pContent[0],0,0);			//标题栏的内容是窗体第一行内容
				}
			}

			if (0 == mode)
			{
				//如果是新创建的窗体,先清屏
				gui_FillRect(0,starty,GUI_WIDTH,user_zone_height, BG_COLOR,0);
			}

			//初始化窗体中的编辑框
			if((g_dlg.pRes->pEdit)&&(return_from_edit_proc_state_flag == 0))
			{
				//如果有的窗体的编辑框资源链表在编译阶段是确定不了的，只有在运行时才能确定下来，那么可以在窗体CREATE阶段的回调函数中设置一个标记，表明
				//此窗体的编辑框资源需要在运行时动态创建，而不是从编译时的资源链表中装载。
				if (edit_res_rebuild_flag)
				{
					//g_dlg.edit.pRes = edit_res_rebuild;			//指向动态创建的编辑框资源链表
				}
				else
				{
					g_dlg.edit.pRes		= g_dlg.pRes->pEdit;		// 告诉edit，他的资源属性是什么,此处指向的编辑框链表是在编译时已经确定的
				}

				edit_init(&g_dlg.edit);
				update_input_method_mode = 0;
			}

			//处理需要在界面最下面一栏显示快捷菜单

			pContent			= get_content();
			gui_FillRect(0,CLIENT_HEIGHT-SHOTCUT_HEIGNT,GUI_WIDTH,SHOTCUT_HEIGNT, BG_COLOR,0);
			if( g_dlg.pRes->style & STYLE_SHORTCUT )
			{	
				gui_TextOut(0, CLIENT_HEIGHT-SHOTCUT_HEIGNT, pContent[1],0,0);			//快捷栏的内容是窗体第二行内容
			}


			
			tmp_y = starty;
			dlg_state				= DLGCB_DRAW;		// 对话框进入绘制窗体状态（不包括绘制图标及标题栏）

			break;

		case DLGCB_DRAW:
			// ==========================================================================================================
			// 显示对话框内容
			//
            pContent			= get_content();
			starty					= tmp_y;
			menu_all				= 0;

			tempx					= 2;
			menu_pagecnt			= 0;

			gui_SetTextColor(TXT_COLOR);

			if ((draw_context[0].valid_flag == 1) && (draw_context[0].dlg_id == g_dlg.pRes->id))
			{
				menu_start		  = draw_context[0].menu_start;
				menu_current_item = draw_context[0].menu_current_item;
			}
			else if ((draw_context[1].valid_flag == 1) && (draw_context[1].dlg_id == g_dlg.pRes->id))
			{
				menu_start		  = draw_context[1].menu_start;
				menu_current_item = draw_context[1].menu_current_item;
			}
			else
			{
				menu_start		  = 1;
				menu_current_item = 1;
			}
			
			while(pContent[tempx])
			{
				// 统计每页的菜单项数
				if( starty + gFontSize <= CLIENT_HEIGHT - SHOTCUT_HEIGNT)
				{
					starty			+= gFontSize+1;
					menu_pagecnt	++;
				}

				// 统计菜单总项数
				menu_all			++;							// 菜单项加一
				tempx				++;
			}

			if(g_dlg.pRes->pEdit)
			{
				highlight_flag = 0;
			}

			// 显示菜单
			if( pContent[1] )
			{
				show_menu(highlight_flag);
			}

			// ==========================================================================================================
			// ************ 回调接口二 ************
			// 此处处理标准绘制所绘制不到的元素
			if((g_dlg.callback)&&(return_from_edit_proc_state_flag == 0))
			{
				// 执行补绘回调
				howrun				= (g_dlg.callback)(DLGCB_DRAW,&key);
				// 此处可能返回
				if(howrun == RUN_NEXT)
					return key;

				if(howrun == RUN_PREV)
					return KEY_ESC;
			}

			//检查是否是提示窗体，这种窗体没有回调函数,蜂鸣器响声
			if(g_dlg.pRes->style & STYLE_TIP)
			{
				Beep(BEEP_DELAY);
			}

			// 编辑框刷新显示
			if(g_dlg.pRes->pEdit)
			{
				edit_refresh(&g_dlg.edit,0,0);//刷新所有编辑框
				//if (edit_is_active(&g_dlg.edit))
				//{
				//	//如果包含了可以编辑的编辑框，显示当前编辑框的默认输入法状态
				//	show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,update_input_method_mode);
				//}
			}

			dlg_return_from_child = 0;
			flush_message();		//将界面完整画出来之前按下的按键都清掉

			// 显示按扭
			dlg_state				= DLG_PROC;		// 对话框进入循环状态

			break;

			/* *************************************************************************** */
			/* 对话框运行部分 */
			/* *************************************************************************** */
		case DLG_PROC:
			// 取得按键，或等待其他消息
			if(g_dlg.edit.active ||  (g_dlg.pRes->style & STYLE_ICON) != 0 || g_dlg.delay != -1)
			{
				pKey				= dlg_getmessage(0, 25);	//有编辑框的界面，需要更新图标的界面
			}
			else
			{
				//不需要处理非按键事物的窗体，就堵在此处一直等待按键事件的发生
				pKey				= dlg_getmessage(0, 0);
			}


			if( pKey == 0 )
			{
				if((g_dlg.pRes->style & STYLE_ICON)&&(no_update_icon_flg == 0))
				{
					//包含了ICON的界面需要更新ICON和界面时间
					// 更新时间
					dlg_updatetime(52, 0);
					// 更新图标
					if(++update_icon_cnt >= 5)
					{
						update_icon_cnt	= 0;
						ret = dlg_update_icon(1,1);
						if (ret == 1)
						{
							//dlg_state				= DLG_INIT;			//重新绘制此窗体
							//break;
							key = KEY_LOW_POWER;      //相当于按下了退出键
							goto dlg_handle_key;
						}
						else if (ret == 2)
						{
							dlg_state				= DLG_INIT;			//重新绘制此窗体
							break;
						}
					}
				}

				if ((USBLink_Insert_Detect() == 0)&&(g_param.transfer_mode == TRANSFER_MODE_U_DISK)&&(bDeviceState == CONFIGURED))
				{
					u_disk_proc();
				}

				//2012.02.15 joe.zhou增加
				//如果窗体注册了一个进度条，那么需要更新进度条
				//if (process_bar.active)
				//{
				//	//如果进度条已经被激活，那么需要更新进度条
				//	process_bar_update();
				//}

				//包含了非只读属性的编辑框的窗体需要闪烁光标
				if(edit_is_active(&g_dlg.edit))
				{
					input_update_flag++;
					if ((input_update_flag >= 3)&&(the_last_key == 1))
					{
						the_last_key = 0;//标记最后一个按键输入结束
					}
					if(++update_cursor_cnt >= 3)
					{
						update_cursor_cnt = 0;
						edit_blink(&g_dlg.edit,0);
					}	
				}

				//2013.01.31 joe.zhou增加
				//包含了需要滚动显示的窗体需要对显示内容进行滚动显示
				roll_update_cnt++;
				if (roll_update_cnt>2)
				{
					roll_update_cnt = 0;
					for (i=0;i<roll_display_item_cnt;i++)
					{
						if ((roll_display_table[i].display_data[roll_display_table[i].offset] > 0x80)&&(roll_display_table[i].offset < strlen(roll_display_table[i].display_data)))
						{
							roll_display_table[i].offset += 2;
						}
						else
						{
							roll_display_table[i].offset++;
						}

						if ( roll_display_table[i].offset >= (strlen(roll_display_table[i].display_data)+roll_display_table[i].display_len) )
						{
							roll_display_table[i].offset = 0;			
						}

						if ( roll_display_table[i].offset > strlen(roll_display_table[i].display_data))
						{
							memset(roll_data_buffer,0x20,roll_display_table[i].display_len);
							k = roll_display_table[i].display_len-roll_display_table[i].offset+strlen(roll_display_table[i].display_data);
							memcpy(roll_data_buffer+k,roll_display_table[i].display_data,roll_display_table[i].offset-strlen(roll_display_table[i].display_data));
							roll_data_buffer[roll_display_table[i].display_len] = 0;
							for (j = k;j < roll_display_table[i].display_len;j++)
							{
								if (roll_data_buffer[j] > 0x80)
								{
									j++;
								}
							}

							if (j > roll_display_table[i].display_len)
							{
								roll_data_buffer[roll_display_table[i].display_len-1] = 0x20;
							}
						}
						else
						{
							if (strlen(roll_display_table[i].display_data+roll_display_table[i].offset) > roll_display_table[i].display_len)
							{
								memcpy(roll_data_buffer,roll_display_table[i].display_data+roll_display_table[i].offset,roll_display_table[i].display_len);
								roll_data_buffer[roll_display_table[i].display_len] = 0;
								for (j = 0;j < roll_display_table[i].display_len;j++)
								{
									if (roll_data_buffer[j] > 0x80)
									{
										j++;
									}
								}

								if (j > roll_display_table[i].display_len)
								{
									roll_data_buffer[roll_display_table[i].display_len-1] = 0;
								}
							}
							else
							{
								strcpy(roll_data_buffer,roll_display_table[i].display_data+roll_display_table[i].offset);
								memset(roll_data_buffer+strlen(roll_display_table[i].display_data+roll_display_table[i].offset),0x20,roll_display_table[i].display_len-strlen(roll_display_table[i].display_data+roll_display_table[i].offset));
								roll_data_buffer[roll_display_table[i].display_len] = 0;
							}
						}

						gui_TextOut(roll_display_table[i].x,roll_display_table[i].y,roll_data_buffer,0,1);
					}
				}

				//设定了延时的窗体需要处理窗口延时自动返回上一级窗体
				if( g_dlg.pRes->delay)
				{
					if(g_dlg.pRes->style & STYLE_TIP)
					{
						if( op_timeout == 4)
						{
							op_timeout = 0;
							if(g_dlg.callback)
							{
								// 执行退出回调
								howrun			= (g_dlg.callback)(DLGCB_DESTORY,&key);
							}
							return  KEY_ESC;		//相当于按下了退出键
						}
						else
						{
							op_timeout++;
						}
					}
					else
					{
						if (g_param.operate_timeout)
						{
							//设定了延时时间的界面，根据界面设定的延时，延时时间到之后，相当于按下了ESC键
							if( op_timeout == (g_param.operate_timeout*200 + g_param.operate_timeout*80/4) )  //2800
							{
								op_timeout = 0;
								key = KEY_ESC;      //相当于按下了退出键
								goto dlg_handle_key;		
							}
							else
							{
								op_timeout++;
							}
						}
					}
				}
				else
				{
					//没有设定延时的界面，需要在延时间到之后关闭屏幕背光，进入低功耗模式
					if ((g_param.operate_timeout)&&(USBLink_Insert_Detect() == 1))	//外部供电时，设备不会进入低功耗模式
					//if (g_param.operate_timeout)
					{
						if(op_timeout > time_out)		//600//延时时间到，需要关闭背光，主控进入低功耗模式
						{
							Lcd_Enter_SleepMode();
							//gui_CtrlBL(0);
							// enable Debug in Stop mode
							DBGMCU->CR |= DBGMCU_CR_DBG_STOP;	//调试时打开，否则进入低功耗时会断开与JLINK的连接

							//if (usb_cable_state)
							//{
							//	USB_Cable_Config(0);
							//}
							EnterLowPowerMode();
							ExitLowPowerMode();
							Lcd_Exit_SleepMode();
							Calendar_Init();

							//if (usb_cable_state)
							//{
							//	USB_Cable_Config(1);
							//}
                            goto dlg_handle_key;
						}
						else
						{
							op_timeout++;
						}
					}
				}

				//此处提供一个接口可以让某些时候窗体没有获取到按键，也可以进入一次处理按键的流程里面去
				//if (auto_run_flag)
				//{
				//	auto_run_flag = 0;
				//	goto dlg_handle_key;
				//}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//此段代码于20110922添加，增加一个接口给窗体，可以使窗体在DRAW完之后，没有任何按键按下之前
				//循环检测一些系统事件（比如USB设备是否被主机枚举成功、电池电量），并作出相应的反应。
				if(g_dlg.callback)
				{
					howrun				= (g_dlg.callback)(DLGCB_REFRESH,&key);
					if (howrun == RUN_NEXT)
					{
						return key;
					}
				}
				/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

				break;
			}

			//有获取到按键消息
			key						= *pKey;

			if (((scan_start)&&(is_key_keypad(key) == 0))||(lowpower_flag > 0))
			{
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//此段代码于20110922添加，由于需求将KEY_CLEAR键与KEY_ESC键的功能合二为一，
			//当前窗体没有编辑框时，按KEY_CLEAR键相当于KEY_ESC
			//如果当前窗体有编辑框时，按下KEY_CLEAR键清除输入时，如果清除到没有输入值时相当于按下了KEY_ESC.
			//if (key == KEY_CLEAR)
			//{
			//	if ((edit_state != EDIT_STATE_PY_SELECT)&&(edit_state != EDIT_STATE_HZ_SELECT))
			//	{
			//		if ((edit_is_active(&g_dlg.edit) == 0)||(g_dlg.edit.now[g_dlg.edit.current_edit] == 0))
			//		{
			//			key = KEY_ESC;
			//		}
			//	}
			//}
			//////////////////////////////////////////////////////////////////////////

dlg_handle_key:
			//if( 0 == g_dlg.pRes->delay)
			//{
			//	if (op_timeout > (g_param.operate_timeout*200 + g_param.operate_timeout*80/5))		//如果是背光已经被关闭的状态
			//	{				
			//		gui_CtrlBL(1);		//重新开背光
			//		op_timeout		= 0;
			//		break;					//跳出，那么第一次按按键只是将关闭的背光打开
			//	}
			//}
			op_timeout		= 0;

			// ************ 回调接口三 ************
			// 将消息，丢给用户处理：系统消息，按键消息，定时器消息
			if(g_dlg.callback)
			{
				// 将系统消息分发给应用处理,msg为入口消息，也为返回消息
				//if (((KEY_ENTER == key  || KEY_UP == key || KEY_DOWN == key)&&(edit_state != EDIT_STATE_HZ_SELECT)&&(edit_state != EDIT_STATE_PY_SELECT)&&
				//	(edit_state != EDIT_STATE_ZD_NAME_SELECT)&&(edit_state != EDIT_STATE_ZD_VALUE_SELECT))||((key != KEY_ENTER)&&(key != KEY_UP)&&(key != KEY_DOWN)))
				{
					howrun				= (g_dlg.callback)(DLGCB_RUN,&key);
					// 重新运行
					if(howrun == RUN_RERUN)
					{
						break;
					}

					if (howrun == RUN_RETURN)
					{
						dlg_state				= DLG_DRAW;		// 对话框重新绘制
						break;
					}

					if (howrun == RUN_EDIT_PROC)
					{
						dlg_state = DLG_EDIT_PROC1;
						break;
					}

					// 要结束当前对话框
					if(howrun == RUN_PREV || howrun == RUN_NEXT)
					{
						// ************ 回调接口四 ************
						if(g_dlg.callback)
						{
							// 执行退出回调
							howrun			= (g_dlg.callback)(DLGCB_DESTORY,&key);
						}
						return key;
					}
				}
			}

			//如果是数字键，则交给edit处理
			//if( (is_key_num(key) == 0) && (edit_is_active(&g_dlg.edit)))
			//{
			//	edit_proc(&g_dlg.edit, key);
			//	input_update_flag = 0;
			//}


			//此按键在包含了可编辑的编辑框的窗体中用来切换输入法的
#if 0
			if ((KEY_jing == key)&&(edit_is_active(&g_dlg.edit)))
			{
				input_method_switch(&g_dlg.edit);	
				show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,1);//显示当前编辑框的输入法ICON
				//如果当前编辑框状态处于中文输入法状态，而又将输入法切换为非中文输入法时，需要清除专门为中文输入法绘制的区域并重新绘制界面
				if ((edit_state != EDIT_STATE_INIT_STATUS)&&(input_method != PINYIN_INPUT))
				{
					gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H+1,BG_COLOR);
					gui_FillRect(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,16,12,TXT_COLOR);
					gui_FillRect(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,16,12,TXT_COLOR);
					update_input_method_mode = 1;
                    edit_state = EDIT_STATE_INIT_STATUS;
					show_menu();
					refresh_draw_content();
					if (need_refresh_edit_index_tbl[0] == 0)
					{
						edit_refresh(&g_dlg.edit,0,0);//刷新所有编辑框
					}
					else
					{
						for (i= 0;i < 4;i++)
						{
							edit_refresh(&g_dlg.edit,1,need_refresh_edit_index_tbl[i]-1);//刷新需要刷新的编辑框
						}

						//刷新上下翻页键
						gui_PictureOut(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_UP);		//显示下翻页的图标
						gui_PictureOut(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_DOWN);		//显示下翻页的图标
					}
					
					show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,0);
					break;
				}
			}

			//如果处于中文输入法时，编辑框已经获取到一组数字键对应的中文编码，将中文编辑框清掉（实际上可以在此处实现联想输入功能）
			if ((input_method == PINYIN_INPUT)&&(edit_state == EDIT_STATE_CODE_GOT))
			{
				//清掉拼音输入法的显示区域
				gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H+1,BG_COLOR);
				gui_FillRect(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,16,12,TXT_COLOR);
				gui_FillRect(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,16,12,TXT_COLOR);
				update_input_method_mode = 1;
				edit_state = EDIT_STATE_INIT_STATUS;
				show_menu();
				refresh_draw_content();
				if (need_refresh_edit_index_tbl[0] == 0)
				{
					edit_refresh(&g_dlg.edit,0,0);//刷新所有编辑框
				}
				else
				{
					for (i= 0;i < 4;i++)
					{
						edit_refresh(&g_dlg.edit,1,need_refresh_edit_index_tbl[i]-1);//刷新需要刷新的编辑框
					}
					gui_PictureOut(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_UP);		//显示下翻页的图标
					gui_PictureOut(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_DOWN);		//显示下翻页的图标
				}
				show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,1);
				break;

			}

			if (EDIT_STATE_CODE_GOT == edit_state)
			{
				edit_state = EDIT_STATE_INIT_STATUS;
				break;			//20120521 增加
			}

#endif
			// 按到上下翻页
			if((KEY_UP == key|| KEY_DOWN == key) && (edit_state == EDIT_STATE_INIT_STATUS))
			{
				//如果包含了非只读编辑框的窗体,且没有处于中文输入法状态时
				if(edit_is_active(&g_dlg.edit))
				{
					//光标跳转到下一个非只读编辑框
					edit_switch(&g_dlg.edit,key);
					//show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,0);
					update_cursor_cnt = 0x100;		//保证切换之后的编辑框的光标立即更新
				}
				else
				{
					if (menu_all > 1)
					{
						dlg_state			= DLG_PAGECHANGE;
						break;
					}
				}
			}
			// 如果是返回的按键，则返回退出
			if(((KEY_ENTER == key) &&(edit_state == EDIT_STATE_INIT_STATUS)) || KEY_ESC == key)
			{
				// ************ 回调接口四 ************
				if(g_dlg.callback)
				{
					// 执行退出回调
					howrun			= (g_dlg.callback)(DLGCB_DESTORY,&key);
				}

				// 直接返回
				return key;
			}

			// 查表，下一个？
			ret						= check_and_jump(key);
			if( ret != -1 )
			{
				return key;
			}
			break;

			/* *************************************************************************** */
			/* 对话框处理翻页功能 */
			/* *************************************************************************** */
			// 菜单改变了
		case DLG_PAGECHANGE:
			if( KEY_DOWN == key)
			{	
				// 下翻页
				if (menu_current_item == menu_all)
				{
					menu_current_item = 1;
					menu_start = 1;
				}
				else
				{
					menu_current_item++;
					if (menu_current_item == (menu_start+menu_pagecnt))
					{
						menu_start ++;
					}
				}
			}
			else
			{	// 上翻页
				if (menu_current_item == 1)
				{
					menu_current_item = menu_all;
					menu_start = menu_all - menu_pagecnt + 1;
				}
				else
				{
					menu_current_item--;
					if (menu_current_item < menu_start)
					{
						menu_start--;
					}
				}
			}

			show_menu(highlight_flag);
			if(g_dlg.callback)
			{
				// 执行回调
				howrun			= (g_dlg.callback)(DLGCB_PAGECHANGE,&key);
			}

			dlg_state				= DLG_PROC;
			break;

		case DLG_EDIT_PROC1:
			//跳转到处理虚拟键盘进行输入的状态
			gui_FillRect(0,0,GUI_WIDTH,GUI_HEIGHT,BG_COLOR,1);
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-ICON_HEIGHT,"Enter    Select   Del",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-ICON_HEIGHT,"确认    选择     删除",0,1);
			}

			//显示可能已经设置的编辑框的值
			//strcpy(virtual_keypad_buffer,g_editValue[current_edit]);//将进入虚拟键盘之前的键值保存起来
			virtual_keypad_type = edit_refresh_ext(&g_dlg.edit);

			//显示虚拟键盘
			virtual_keypad_proc(virtual_keypad_type,0);
			dlg_state				= DLG_EDIT_PROC2;
			break;

		case DLG_EDIT_PROC2:
			pKey				= dlg_getmessage(0, 25);
			if (pKey == 0)
			{
				input_update_flag++;
				if ((input_update_flag >= 3)&&(the_last_key == 1))
				{
					the_last_key = 0;//标记最后一个按键输入结束
				}
				if(++update_cursor_cnt >= 3)
				{
					update_cursor_cnt = 0;
					edit_blink(&g_dlg.edit,1);
				}

				break;
			}

			//有获取到按键消息
			key						= *pKey;

			if (key == KEY_ESC)
			{
				//丢弃虚拟键盘接收到的输入
				if(g_dlg.callback)
				{
					// 执行从虚拟键盘返回时的回调
					howrun			= (g_dlg.callback)(DLGCB_BACK_FROM_VIRTUAL_KEYPAD,&key);
					if(howrun == RUN_NEXT)
					{
						return key;
					}
				}
				gui_FillRect(0,0,GUI_WIDTH,GUI_HEIGHT,BG_COLOR,0);
				dlg_state				= DLGCB_CREATE;
				break;
			}
			else if (key == KEY_LEFT_SHOTCUT)
			{
				//将虚拟键盘接收到的输入放回原来窗体的编辑框中
				if(g_dlg.callback)
				{
					// 执行从虚拟键盘返回时的回调
					howrun			= (g_dlg.callback)(DLGCB_BACK_FROM_VIRTUAL_KEYPAD,&key);
					if(howrun == RUN_NEXT)
					{
						return key;
					}
				}

				gui_FillRect(0,0,GUI_WIDTH,GUI_HEIGHT,BG_COLOR,0);
				return_from_edit_proc_state_flag = 1;
				dlg_state				= DLGCB_CREATE;
				break;
			}
			else if (key == KEY_RIGHT_SHOTCUT)
			{
				key = KEY_CLEAR;
			}
			else
			{
				key = virtual_keypad_proc(virtual_keypad_type,key);
			}

			if(key)
			{
				edit_proc(&g_dlg.edit, key,1);
				input_update_flag = 0;
			}

			break;
		}
	}
	//return 0;
}


/**
***************************************************************************
*@brief	窗体运行主循环
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
void dlg_loop(void)
{

	unsigned char						ret;
        
	now_id						= 0xFF;	

	for(;;)
	{
		if(now_id != next_dlg_id)
		{
			now_id				= next_dlg_id;
			dlg_create();
			ret					= dlg_run(0);		// run new dialog
		}
		else
		{
			ret					= dlg_run(1);		// no switch
		}

		tempx					= 0;
		while(g_dlg.jmplst[tempx].key)
		{
			if(g_dlg.jmplst[tempx].key == ret)
			{
				next_dlg_id			= g_dlg.jmplst[tempx].dlgid;
				break;
			}
			tempx++;
		}
	}
}

/**
* @brief 由task1调用，检测是否有后台任务需要运行，有则运行任务。并将运行结果返回。
* @note 任务如：PN532刷卡，GPRS通信等
*/
void dlg_run_task(void)
{
	unsigned char					err_code;
	int								ret_code;

	OSSemPend(pBGCommand, 0, &err_code);

	if(err_code == OS_NO_ERR)
	{
		// 如果要启动一个任务
		if( SCommand == TASK_RUN )
		{
			SStatus						= 1;			// busy

			if( SbgTask )
			{
				// 直接返回键值
				ret_code				= SbgTask(SParam1, SParam2, SParam3, SParam4);
			}
			else
			{
				ret_code				= -1;
			}
			SStatus						= 0;

			// 发送命令给界面，告知程序结束
			task_send_command_to_ui(ret_code);
		}
	}
}



/**
* @brief 请求运行一个后台任务
* @return 0:申请成功递交
*        -1:有任务在运行，请求失败
*/
int dlg_run_bgtask(TbgTask callback, int param1, int param2, void *param3, void *param4)
{
	if( (dlg_read_bgtask_status() != 0) && callback)
	{
		if (SbgTask != callback)
		{
			//新请求的任务与后台正在运行的任务不同，那么就将后台正在运行的任务结束
			SCommand						= TASK_EXIT;

			OSSemPost(pBGCommand);
			while(SStatus)				//等待当前后台任务结束
			{
				OSTimeDlyHMSM(0,0,0,10);
			}
		}
		else
		{
			return -1;
		}
	}

	SbgTask							= callback;
	SParam1							= param1;
	SParam2							= param2;
	SParam3							= param3;
	SParam4							= param4;
	SStatus							= 0;

	SCommand						= TASK_RUN;

	OSSemPost(pBGCommand);

	return 0;
}

/**
* @brief 给UI调用，检查后台程序运行状态
*/
int dlg_read_bgtask_status(void)
{
	return SStatus;
}

/**
* @brief 界面发送一个命令给任务
* @return 0:命令发送成功
*        -1:命令发送失败
*/
int ui_send_command_to_task(int command)
{
	SCommand						= command;
	if( OSSemPost(pBGCommand) == OS_ERR_NONE)
		return 0;
	else
		return -1;
}

/**
* @brief 读一个来自UI的命令
* @param[in] int *command 读取一个来自主机的命令
* @return 0:成功
*        -1:无命令
*/
int task_read_command_from_ui(int *command, int waitend)
{
	unsigned char						err_code;
	int									timeout;

	if(waitend == 0)
		timeout							= 3;
	else
		timeout							= waitend;


	do{
		OSSemPend(pBGCommand, timeout, &err_code);	// 是否有来自前台的命令？
		if(err_code == OS_ERR_NONE )
		{
			*command					= SCommand;
			return 0;
		}
	}while(waitend);

	return -1;
}


/**
* @brief 任务发送一个命令(键值)给前台ui
* @return 0:发送成功
*        -1:发送失败
*/
int task_send_command_to_ui(int command)
{
	unsigned char					err_code;
	SReturn							= command;

	err_code						= OSQPost(pKeyOSQ, (void*)&SReturn);
	if( err_code == OS_ERR_NONE)
		return 0;
	else
		return err_code;
}


//保存某窗体跳转时一些需要保存的信息
void saveDrawContext(void)
{
	if ((draw_context[0].valid_flag  == 1)&&(draw_context[0].dlg_id != g_dlg.pRes->id))
	{
		draw_context[1].valid_flag = 1;
		draw_context[1].dlg_id = g_dlg.pRes->id;
		draw_context[1].menu_start = menu_start;
		draw_context[1].menu_current_item = menu_current_item;
	}
	else
	{
		draw_context[0].valid_flag = 1;
		draw_context[0].dlg_id = g_dlg.pRes->id;
		draw_context[0].menu_start = menu_start;
		draw_context[0].menu_current_item = menu_current_item;
	}
}

//保存某窗体跳转时一些需要保存的信息
void releaseDrawContext(void)
{
	if (draw_context[1].valid_flag  == 1)
	{
		if (draw_context[1].dlg_id == g_dlg.pRes->id)
		{
			draw_context[1].valid_flag = 0;
		}
	}
	else
	{
		if (draw_context[0].dlg_id == g_dlg.pRes->id)
		{
			draw_context[0].valid_flag = 0;
		}
	}
}

void InitDrawContext(void)
{
	draw_context[0].valid_flag = 0;
	draw_context[1].valid_flag = 0;
}



