
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
static unsigned char			menu_pagecnt;			//��ǰ����һҳ������ʾ�Ĳ˵���
static 	unsigned int			user_zone_height;

static unsigned char			now_id;
static unsigned char			howrun;
static unsigned char			dlg_state;
//static TProcessBar				process_bar;				//������

static unsigned char			**pContent;
//static int						last_charge_state;			//�ϴεĳ��״̬

static unsigned char			the_last_key;				//����һ���༭�����һ����ֵʱ�ı��

static int							SCommand;
TbgTask						SbgTask;					// ��̨����ص�����
static int							SParam1;
static int							SParam2;
static void							*SParam3;
static void							*SParam4;
static unsigned char				SReturn;					// ����ķ���ֵ
static int							SStatus;
static int							lowpower_detect_cnt;
static int							last_power_level;

typedef struct 
{
	unsigned char valid_flag;			//��ǰ�������Ϣ�Ƿ���Ч���
	unsigned char dlg_id;				//��ǰ�����ID
	unsigned char menu_start;			//���Ƶ�ǰ�������ʼ�˵���
	unsigned char menu_current_item;	//��ǰѡ��Ĳ˵���
}TDRAWCONTEXT;

static TDRAWCONTEXT		draw_context[2];	//���������Ŀ¼��Ҫ����

unsigned char			display_menu_item_array[5];		//���浱ǰ��ʾ�Ĳ˵���
unsigned char			menu_current_item;				//��ǰ�˵���
unsigned char			menu_start;	
unsigned char			icon_draw_flag;					//ͼ�����Ѿ������˵ı�� 
/* Global variables -----------------------------------------------------------*/
unsigned char				g_editValue[MAX_EDIT][MAX_EDITNUM];//�༭���ȫ�ֱ���

unsigned char				max_dlg;
unsigned char				next_dlg_id;
//unsigned char				content_font_size;	//��ʾ��������ʱ���õ������С

TDialog						g_dlg;			
OS_EVENT					*pBGCommand;				//���ƺ�̨���������
//unsigned char				auto_run_flag;				//���ƴ��岻��Ҫ��ȡ��������ϢҲ���Խ��밴����������̵ı�ǡ�

//������Ҫ�ڴ��ڻص����޸��������Եı༭�����ѡ���뷨��������Ҫ������������������Ϊȫ�ֵ����ԡ�
unsigned char				update_input_method_mode;	//�������뷨ͼ���ģʽ
unsigned char				input_method;				//��ǰ���뷨
unsigned char				last_input_method;			//��һ��ʹ�õ����뷨
unsigned char				max_edit;					//��ǰ��������Ŀɱ༭�༭�������
unsigned char				all_edit;					//��ǰ������������б༭�������
unsigned char				edit_res_rebuild_flag;		//��ʾ��ǰ����ı༭����Դ������Ҫ������ʱ��̬���������������ڴ��У���Ҫ��̬�����༭����Դ����Ĵ���
														//�����ڴ���CREATE�׶εĻص��ӿ��б�ǡ�
//unsigned char				default_input_method_patch;	//�༭�����ѡ���뷨�Ƿ���Ҫ����ָ��
unsigned char				number_edit_patch;			//�������Եı༭�����Ƿ�����ͨ��*�ż�����С����

unsigned char		same_key_times;				///������ͬһ�������Ĵ���

//TEditRes					edit_res_rebuild[MAX_EDIT];	//��̬�����ı༭����Դ����,�ڴ���CREATE�׶εĻص��ӿ��д���������

unsigned char				tempx;
unsigned short				tempy;

unsigned char			update_icon_cnt;
unsigned short			edit_display_x_patch;

unsigned int			lowpower_flag;
int						bluetooth_match_cmd_cnt;
int						bluetooth_state_cnt;

		

//TDRAW_ITEM	draw_content[MAX_CASH_ITEM];	//��Щ������DRAW�׶λص�ʱ���ƵĴ���������Ҫ���������Ļ���ռ䣬���ڴ����������뷨ʱ��Ҫ�����������ˢ��,�����Ҫ���ӻ�������ݿ��Ըı�˻���ռ�Ĵ�С
TRollDisplayItem	roll_display_table[3];	//��Ҫ������ʾ������
unsigned char		roll_display_item_cnt;	//��Ҫ������ʾ�ĸ���
unsigned char		roll_update_cnt;
unsigned char		roll_data_buffer[22];

unsigned char		dlg_return_from_child;			//���Ӵ��巵�ش˴������еı��

/* External Variable -----------------------------------------------------------*/
extern const TActionList		dlg_actionlist[];
extern OS_EVENT					*pKeyOSQ;					// ������Ϣ
extern TkeyValue_Ascii			keyValue_Ascii_Tbl[];
extern TTerminalPara			g_param;			//ȫ���ն˲���
extern TypedefDateTime			currentDateTime;	
extern unsigned char			gFontSize;			// �ֺ�
extern unsigned char			g_language;			// ��������
extern unsigned char			edit_state;			//�༭��״̬
//extern unsigned char			current_operator;			// ��ǰ��¼��ϵͳ�Ĳ���Ա
extern unsigned int				input_update_flag;			//�Ƿ���Ҫ��������ı��
extern unsigned char			last_input_key;				//���һ������İ���
extern vu32 bDeviceState;
extern unsigned int				scan_start;
extern unsigned char			no_update_icon_flg;
extern unsigned char			usb_cable_state;
//extern unsigned char	input_valid_num;				//�������Чλ��
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
* @brief ��鰴���Ƿ����ּ��������
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
* @brief ��鰴���Ƿ��Ǽ����ϵ�ʵ�ʰ���
*/
int is_key_keypad(unsigned char key)
{
	if(key==KEY_SCAN || key==KEY_UP || key==KEY_DOWN || key==KEY_LEFT || key == KEY_RIGHT
		|| key==KEY_LEFT_SHOTCUT || key==KEY_ESC || key==KEY_ENTER || key==KEY_RIGHT_SHOTCUT )
		return 0;
	return -1;
}

/**
* @brief ��ʼ���Ի���ģ��
*/
int dlg_init(void)
{
	//enote_tip_flag = 0;
	icon_draw_flag = 0;
	//content_font_size = 16;			//�󲿷ִ������ݶ�����16�����壬��Ҫ�ı������С����ʾ��������ʱ���ڴ��ڴ���ʱ�Ļص��������޸Ĵ˱������ɡ�
	//auto_run_flag = 0;				//�󲿷ֵĴ��嶼�����������̣��ڻ�ȡ��������Ϣ֮��Ž��밴�����������
	lowpower_detect_cnt = 0;
	last_power_level = 4;
    charge_state_cnt = 0;
	pBGCommand						= OSSemCreate(0);
	return 0;
}

/**
* @brief ������ת�ɼ�ֵ
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
	return KEY_NUM0;			// ��Ӧ������Ŷ
}

/**
* @brief �ȴ���Ϣ,�����ǰ�������ʱ��������ϵͳ��Ϣ
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


//�����Ϣ���������Ϣ
static void flush_message(void)
{
	OSQFlush(pKeyOSQ);
	return;
}


/**
* @brief ����һ���ַ�������ʾ����
* @param[in] unsigned char *str  ��Ҫ������ַ���ָ��
* @note �������ĺ�Ӣ�ĵ���ģ��ȱȲ��ڸպ���2:1��������Ҫ����һ�������ж�һ���ַ�������ʾ���ȡ�
*/
int disp_len(unsigned char *str)
{
	return strlen(str)*6;
}

/**
* @brief ����ˢ�´�����DRAW�׶λص��л��ƵĴ�������
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
* @brief ��ʼ���༭������
* @param[in] TEdit *pEdit Ҫ��ʾ��Edit�Ա�
*/
void edit_init(TEdit *pEdit)
{
	unsigned char i = 0;
	unsigned char	j = 0;
	TEditRes	*pEditRes;

	pEditRes = pEdit->pRes;	
	max_edit = 0;
	all_edit = 0;

	//��������ʱ���༭���Ѿ�����0��
	//memset(pEdit->active,0,MAX_EDIT);
	//memset(pEdit->max,0,MAX_EDIT);
	//memset(pEdit->now,0,MAX_EDIT);

	do 
	{
		if((pEditRes->style&0x0F) == EDIT_IP)				// ip��ַ����Ϊ15   [0]123.456.789.abc[15]
		{
			pEdit->display_max[i]		= 15;
			pEdit->input_max[i]			= 15;
			pEdit->now[i]				= 0;
		}
		else if((pEditRes->style&0x0F) == EDIT_MONEY)			//			[8]1234567.2[0]
		{
			pEdit->display_max[i]		= 9;		//�涨����������9λ�Ľ��׽��
			pEdit->input_max[i]			= 9;
			pEdit->now[i]				= 0;
		}
		else													// ��ͨ					[5]654321[0]
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
			max_edit++; //�˴����а������ܵķ�ֻ���༭��ĸ���

			//�ҵ���һ����ֻ���ı༭��
			if ((j&0x80) != 0x80)
			{
				j = i;
				j |= 0x80;
			}
		}

		//�༭���е�����Ĭ��Ϊϵͳ���õ������С����ɫ
		pEdit->view[i].bg_color = gui_GetBGColor();
		pEdit->view[i].txt_color = gui_GetTextColor();
		pEdit->view[i].font_size = 12;

		pEdit->display_patch[i] = 0;

		pEditRes = pEditRes->next;							//ָ����һ���༭�����Դ
        i++;
	} while (pEditRes != 0);

	all_edit = i;
	pEdit->current_edit = (j&0x7f);	//Ĭ�ϵ�һ����ֻ���༭��Ϊ��ǰ�༭��
	pEdit->blink				= 0;
	

	//pEdit->value				= &g_editValue[0];
	// edit����
	memset(g_editValue, 0x00, MAX_EDITNUM*MAX_EDIT);
	last_input_key = 0;		//���һ������İ�����0
	input_update_flag = 0;
	edit_state = EDIT_STATE_INIT_STATUS;
	the_last_key = 0;
	input_method = _123_INPUT;
}
/**
* @brief ��ȡ���嵱ǰ�༭�������
* @param[in] TEdit *pEdit Ҫ��ʾ��Edit�Ա�
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
	if (g_dlg.pRes->style & STYLE_CHILDWINDOW)		//ע���Ӵ����еı༭���λ����������Ӵ���λ�õ��������
	{
		edit_attr->x += DEFAULT_CHILDWINDOW_X; 
		edit_attr->y += DEFAULT_CHILDWINDOW_Y;
	}
}

/**
* @brief ����ֵת��Ϊ����Ľ���ַ���
* @param[in] unsigned char *keyvalue Ҫת���ļ�ֵ
* @note ������ļ�ֵ�����
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
* @brief ����ĳһ���༭������ʾ�������С
* @param[in] unsigned char size			�����С
* @param[in] unsigned char edit_index	�༭������
* @note ���õ������Сֻ���ڴ˴��������������Ч,�ڴ����DRAW�׶�֮��Ļص��ӿ��е��òŻ���Ч
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
* @brief ����ĳһ���༭������ʾ������ı���ɫ
* @param[in] unsigned short color		��ɫ
* @param[in] unsigned char edit_index	�༭������
* @note ���õ������Сֻ���ڴ˴��������������Ч,�ڴ����DRAW�׶�֮��Ļص��ӿ��е��òŻ���Ч
*/
void edit_set_BgColor(TEdit *pEdit,unsigned short color,unsigned char edit_index)
{
	pEdit->view[edit_index].bg_color = color;
	return;
}

/**
* @brief ����ĳһ���༭������ʾ������ı���ɫ
* @param[in] unsigned short color		��ɫ
* @param[in] unsigned char edit_index	�༭������
* @note ���õ������Сֻ���ڴ˴��������������Ч,�ڴ����DRAW�׶�֮��Ļص��ӿ��е��òŻ���Ч
*/
void edit_set_TextColor(TEdit *pEdit,unsigned short color,unsigned char edit_index)
{
	pEdit->view[edit_index].txt_color = color;
	return;
}
/**
* @brief editˢ�±༭�����ʾ
* @param[in] unsigned char mode  0:ˢ�µ�ǰ��������еı༭��   1:ˢ��ָ���ı༭��
* @note �����EditValue������Ǽ�ֵ��������ascii����ʾʱ��Ҫת��
* @note
*/
void edit_refresh(TEdit *pEdit,unsigned char mode,unsigned char edit_index)
{
	int								i,j;
	TEditAttribute					edit_attr;			//��ǰ�༭�������
	unsigned char					current_edit,edit_save;
	int								disp_len;
	TEditRes						* pEditRes;
	unsigned char					amount[10];
    unsigned char					str[38];
	unsigned int					x,y;
	unsigned char					offset;


	pEditRes = pEdit->pRes;
	edit_save = pEdit->current_edit;	//���浱ǰ�༭��

	pEdit->current_edit = 0;

	do 
	{
		j = 0;
		current_edit = pEdit->current_edit;
		edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������
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
				//�༭��������������ʾ����
				edit_attr.y = ICON_HEIGHT + 18 + edit_attr.y - (GUI_HEIGHT - SHOTCUT_HEIGNT );
			}
		}


		if ((mode == 0) || (edit_index == current_edit))
		{ 
			if((edit_attr.style & 0x07) == EDIT_IP)				// ˳������ 0.1.2.3.4.5.6
			{
				///STEP2:�����ʾ
				memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
				str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

				///STEP3:��ʾ���
				for(i=0; i<pEdit->now[current_edit]; i++)									/// 
				{
					if ((i==3) ||(i==6)||(i==9))
					{
						j = 12*(i/3);
					}
					gui_TextOut(edit_attr.x + i*gFontSize/2 + j, edit_attr.y, key2ascii(g_editValue[current_edit][i]), 0,1);
				}
				///STEP4:�»���
				if(edit_attr.style & EDIT_UNDERLINE)
				{
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
					gui_LineH(edit_attr.x + 4 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
					gui_LineH(edit_attr.x + 8 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
					gui_LineH(edit_attr.x + 12 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR,1);
				}

				//STEP5:��ʾ�ָ���
				gui_TextOut(edit_attr.x + 3*gFontSize/2, edit_attr.y, ".", 0,1);
				gui_TextOut(edit_attr.x + 7*gFontSize/2, edit_attr.y, ".", 0,1);
				gui_TextOut(edit_attr.x + 11*gFontSize/2, edit_attr.y, ".", 0,1);
			}
			else if(((edit_attr.style & 0x07) == EDIT_PSW)||((edit_attr.style & 0x07) == EDIT_PSW_S))		
			{
				///STEP2:�����ʾ
				memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
				str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);


				///STEP3:��ʾ���
				for(i=0; i<pEdit->now[current_edit]; i++)									/// * * * * * *
					gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, "*", 0,1);

				///STEP4:�»���
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR,1);
			}
			else if((edit_attr.style & 0x07) == EDIT_NUM)
			{
				/// STEP2:�����ʾ
				memset(str,' ',pEdit->display_max[current_edit]);
				str[pEdit->display_max[current_edit]] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, (((edit_attr.style & 0x10) == EDIT_NO_BLINK)?2:1),1);

				if ((edit_attr.style & 0x10) == EDIT_NO_BLINK)
				{
					gui_TextOut(4, edit_attr.y, "                    ", 2,1);
				}

				/// STEP3:����ַ�
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
				/// STEP4:�»���
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR,1);

			}
			else if((edit_attr.style & 0x07) == EDIT_MONEY)		// ��������
			{
				// STEP2:�����ʾ
				memset(str,' ',pEdit->display_max[current_edit]);
				str[pEdit->display_max[current_edit]] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

				// STEP3��ʾ���

				// ����ַ�,�����һ�ֽ���ǰ��ʾ,ע�����С����ͻ��ҷ���
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

				// STEP4:�»���
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR,1);
			}
			else if ((edit_attr.style & 0x07) == EDIT_CHINESE || (edit_attr.style & 0x07) == EDIT_ALPHA)		
			{
				/// STEP2:�����ʾ
				memset(str,' ',pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6);
				str[pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6] = 0;
				gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);
                                

				//if((edit_attr.style & 0x80) == EDIT_MUL_LINE)
				//{
				//	gui_FillRect(0,edit_attr.y + gFontSize,GUI_WIDTH,gFontSize*4,BG_COLOR);	
				//}

				//for(i=0; i<pEdit->max[current_edit]; i++)
				//	gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, " ", 1);

				/// STEP3:����ַ�,
				offset = pEdit->now[current_edit]<pEdit->display_max[current_edit]?0:(pEdit->now[current_edit]-pEdit->display_max[current_edit]);
				//�ж��Ƿ�һ�����ֵ�ǰ���ֽڽض���
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

				/// STEP4:�»���
				if(edit_attr.style & EDIT_UNDERLINE)
					gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2 + pEdit->display_patch[current_edit], TXT_COLOR,1);

			}
		}

		//if(pEdit->max[current_edit] == (pEdit->now[current_edit] + 1) || pEdit->max[current_edit] == pEdit->now[current_edit])
		{
			//������ܻ�����˸�Ĺ��
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
			//ֻ��Ҫˢ����Ҫˢ�µı༭�򼴿ɣ�����
			break;
		}

		pEdit->current_edit++;
		pEditRes = pEditRes->next;
	} while (pEditRes != 0);

	pEdit->current_edit = edit_save;		//�ָ���ǰ�ı༭��
	return;
}

/**
* @brief editˢ�±༭�����ʾ
* @param[in] unsigned char mode  0:ˢ�µ�ǰ��������еı༭��   1:ˢ��ָ���ı༭��
* @param[out] unsigned char *buffer  ��ԭ���༭���еļ�ֵ���浽������̵Ļ�����
* @note �����EditValue������Ǽ�ֵ��������ascii����ʾʱ��Ҫת��
* @note
*/
unsigned char edit_refresh_ext(TEdit *pEdit)
{
	int								i,j;
	TEditAttribute					edit_attr;			//��ǰ�༭�������
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
	edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������
	edit_attr.x = 0;
	edit_attr.y = 0;
	edit_attr.style &= ~EDIT_UNDERLINE;
	gui_SetTextColor(TXT_COLOR);

	{ 
		if((edit_attr.style & 0x07) == EDIT_IP)				// ˳������ 0.1.2.3.4.5.6
		{
			///STEP2:�����ʾ
			memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
			str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

			///STEP3:��ʾ���
			for(i=0; i<pEdit->now[current_edit]; i++)									/// 
			{
				if ((i==3) ||(i==6)||(i==9))
				{
					j = 12*(i/3);
				}
				gui_TextOut(edit_attr.x + i*gFontSize/2 + j, edit_attr.y, key2ascii(g_editValue[current_edit][i]), 0,1);
			}
			///STEP4:�»���
			//if(edit_attr.style & EDIT_UNDERLINE)
			//{
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//	gui_LineH(edit_attr.x + 4 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//	gui_LineH(edit_attr.x + 8 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//	gui_LineH(edit_attr.x + 12 * gFontSize/2, edit_attr.y + gFontSize-1, 3 * gFontSize/2, TXT_COLOR);
			//}

			//STEP5:��ʾ�ָ���
			gui_TextOut(edit_attr.x + 3*gFontSize/2, edit_attr.y, ".", 0,1);
			gui_TextOut(edit_attr.x + 7*gFontSize/2, edit_attr.y, ".", 0,1);
			gui_TextOut(edit_attr.x + 11*gFontSize/2, edit_attr.y, ".", 0,1);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_NUM;
		}
		else if(((edit_attr.style & 0x07) == EDIT_PSW)||((edit_attr.style & 0x07) == EDIT_PSW_S))		
		{
			///STEP2:�����ʾ
			memset(str,' ',(GUI_WIDTH - edit_attr.x)/(gFontSize/2));
			str[(GUI_WIDTH - edit_attr.x)/(gFontSize/2)] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);


			///STEP3:��ʾ���
			for(i=0; i<pEdit->now[current_edit]; i++)									/// * * * * * *
				gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, "*", 0,1);

			///STEP4:�»���
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize-1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_ALPHA;
		}
		else if((edit_attr.style & 0x07) == EDIT_NUM)
		{
			/// STEP2:�����ʾ
			memset(str,' ',pEdit->display_max[current_edit]);
			str[pEdit->display_max[current_edit]] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, (((edit_attr.style & 0x10) == EDIT_NO_BLINK)?2:1),1);

			if ((edit_attr.style & 0x10) == EDIT_NO_BLINK)
			{
				gui_TextOut(4, edit_attr.y, "                    ", 2,1);
			}

			/// STEP3:����ַ�
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
			/// STEP4:�»���
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_NUM;
		}
		else if((edit_attr.style & 0x07) == EDIT_MONEY)		// ��������
		{
			// STEP2:�����ʾ
			memset(str,' ',pEdit->display_max[current_edit]);
			str[pEdit->display_max[current_edit]] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);

			// STEP3��ʾ���

			// ����ַ�,�����һ�ֽ���ǰ��ʾ,ע�����С����ͻ��ҷ���
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

			// STEP4:�»���
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2, TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_NUM;
		}
		//else if ((edit_attr.style & 0x07) == EDIT_CHINESE || (edit_attr.style & 0x07) == EDIT_ALPHA)		
		else if ((edit_attr.style & 0x07) == EDIT_ALPHA)		
		{
			/// STEP2:�����ʾ
			memset(str,' ',pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6);
			str[pEdit->display_max[current_edit]+pEdit->display_patch[current_edit]/6] = 0;
			gui_TextOut(edit_attr.x, edit_attr.y, str, 1,1);


			//if((edit_attr.style & 0x80) == EDIT_MUL_LINE)
			//{
			//	gui_FillRect(0,edit_attr.y + gFontSize,GUI_WIDTH,gFontSize*4,BG_COLOR);	
			//}

			//for(i=0; i<pEdit->max[current_edit]; i++)
			//	gui_TextOut(edit_attr.x + i*gFontSize/2, edit_attr.y, " ", 1);

			/// STEP3:����ַ�,
			offset = pEdit->now[current_edit]<pEdit->display_max[current_edit]?0:(pEdit->now[current_edit]-pEdit->display_max[current_edit]);
			//�ж��Ƿ�һ�����ֵ�ǰ���ֽڽض���
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

			/// STEP4:�»���
			//if(edit_attr.style & EDIT_UNDERLINE)
			//	gui_LineH(edit_attr.x, edit_attr.y + gFontSize+1, pEdit->display_max[current_edit] * gFontSize/2 + pEdit->display_patch[current_edit], TXT_COLOR);
			virtual_keypad_type = VIRTUAL_KEYPAD_TYPE_ALPHA;
		}
	}

	//if(pEdit->max[current_edit] == (pEdit->now[current_edit] + 1) || pEdit->max[current_edit] == pEdit->now[current_edit])
	{
		//������ܻ�����˸�Ĺ��
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
* @brief ������̵Ĵ�����
* @note �����EditValue������Ǽ�ֵ��������ascii����ʾʱ��Ҫת��
* @param[in] unsigned char type:������̵�����  ������  �� ��ĸ��
* @param[in] unsigned char key:��ֵ 0:��ʼ������
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
* @brief edit����һ��������Ϣ
* @note �����EditValue������Ǽ�ֵ��������ascii����ʾʱ��Ҫת��
* @param[in] int key����ļ�ֵ,����:
*            KEY_CLEAR ɾ��ǰһ�������ַ�
*            KEY_ENTER ˢ����ʾ
*            KEY_NUM0..KEY_NUM9 ����
*/
static void edit_proc(TEdit *pEdit, unsigned char key,unsigned char virtual_keypad_flag)
{
	TEditAttribute					edit_attr;			//��ǰ�༭�������
	unsigned char					current_edit;
	unsigned char					if_update_cursor;
	unsigned char					key_code[11];
	unsigned char					i;

	current_edit = pEdit->current_edit;
	

	edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������

	if((edit_attr.style & 0x07) == EDIT_IP)				/// ˳������ 0.1.2.3.4.5.6
	{
		///STEP1:�߼�����
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
				/// ��������
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
		///STEP1:�߼�����
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
				/// ��������
				if(pEdit->now[current_edit] < pEdit->input_max[current_edit])
					g_editValue[current_edit][pEdit->now[current_edit]++]	= key;
			}
		}
	}
	else if((edit_attr.style & 0x07) == EDIT_NUM)		/// �������� 6.5.4.3.2.1.0
	{
		/// STEP1:�߼�����
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
	else if((edit_attr.style & 0x07) == EDIT_MONEY)		// ��������
	{
		// STEP1:�߼�����
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
					// ����༭��ĵ�һ����0����ô�ڶ�������İ�������С����֮�⣬���඼��Ҫȡ����һ��0
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
							//����ǰ��û��������С���㣬���Լ�������С������
							if(pEdit->now[current_edit] < pEdit->input_max[current_edit])			// �۳�һλС����,�ܴ�9λ����
								g_editValue[current_edit][pEdit->now[current_edit]++] = *key2ascii(KEY_juhao);
						}
					}
				}
				else
				{
					if(pEdit->now[current_edit] < pEdit->input_max[current_edit])			// �۳�һλС����,�ܴ�9λ����
						g_editValue[current_edit][pEdit->now[current_edit]++] = *key2ascii(key);
				}
			}
		}
	}
	else if ((edit_attr.style & 0x07) == EDIT_ALPHA)	//����ΪALPHA���Եı༭�򣬿���ʹ���������뷨����
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
				/// ��������
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
						//�䵽���һλ��
						the_last_key = 1;		//������������һλ�����ڵȴ���ͬһ�����л���״̬
					}
				}
			}
		}
	}
	//else if ((edit_attr.style & 0x07) == EDIT_CHINESE)		///����ΪCHINESE���Եı༭�򣬿���ʹ���������뷨����
	//{
	//	if((edit_attr.style & EDIT_READONLY)==0)
	//	{
	//		//if ((edit_state == EDIT_STATE_ZD_NAME_SELECT)||(edit_state == EDIT_STATE_ZD_VALUE_SELECT))
	//		if (edit_state == EDIT_STATE_ZD_VALUE_SELECT)
	//		{
	//			//�����ֵ����빦�ܵĴ���
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
	//				//��û�н�����������״̬ʱ��ȡ���������ǰ������ļ�ֵ
	//				if(pEdit->now[current_edit])
	//				{
	//					if (g_editValue[current_edit][pEdit->now[current_edit]-1] < 0x80)
	//					{
	//						//˵��ǰ���������һ��ASCII�ַ�
	//						pEdit->now[current_edit]--;
	//					}
	//					else
	//					{
	//						//˵��ǰ���������һ�����֣���Ҫ��ȥ2������
	//						pEdit->now[current_edit] -= 2;
	//					}
	//					g_editValue[current_edit][pEdit->now[current_edit]] = 0x00;
	//				}
	//			}
	//			else if ((EDIT_STATE_INIT_STATUS == edit_state || EDIT_STATE_CODE_GOT == edit_state)&&(KEY_FUN4 == key))
	//			{
	//				//��û�н�����������״̬ʱ��F4�������ǰ����������ļ�ֵ
	//				pEdit->now[current_edit] = 0;
	//				memset((void*)g_editValue[current_edit],0,MAX_EDITNUM);
	//			}
	//			else if ((EDIT_STATE_INIT_STATUS == edit_state)&&(KEY_ENTER == key || KEY_UP == key || KEY_DOWN == key))
	//			{
	//				//���������뷨״̬ʱ��KEY_ENTER��KEY_UP��KEY_DOWN��KEY_ESC���ǲ������κ����õ�
	//				//ʲô������
	//			}
	//			else
	//			{
	//				if ((input_method == PINYIN_INPUT)&&((pEdit->now[current_edit]+1) == pEdit->input_max[current_edit]))
	//				{
	//					//���ֻʣ��һ���ַ���λ��ʱ������������������
	//				}
	//				else if((pEdit->now[current_edit] < pEdit->input_max[current_edit]) || (key == KEY_CLEAR) || (1 == the_last_key))
	//				{
	//					if_update_cursor = edit_chinese_proc(key,input_method,key_code);
	//					if ((edit_state == EDIT_STATE_CODE_GOT)&&(key != KEY_CLEAR))
	//					{
	//						//����Ѿ���ȡ����ֵ�ı���,ֱ�ӽ���ֵ��Ӧ�Ŀ���ʾ����COPY�������ֵ�Ļ�����
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
	//							//�䵽���һλ��
	//							the_last_key = 1;		//������������һλ�����ڵȴ���ͬһ�����л���״̬
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
		//����������״̬�£�����Ҫˢ�±༭��
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
* @brief ����༭����
*/
static void edit_blink(TEdit *pEdit,unsigned char virtual_keypad_flag)
{
	TEditAttribute					edit_attr;			//��ǰ�༭�������
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
	
	
	edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������
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
		//�༭��������������ʾ����
		y = ICON_HEIGHT + 18 + edit_attr.y - (GUI_HEIGHT - SHOTCUT_HEIGNT );
	}

	while(x >= GUI_WIDTH)
	{
		x -= GUI_WIDTH;
		y += 13;
	}

	if(((edit_attr.style & 0x10) != EDIT_NO_BLINK)&&(edit_state == EDIT_STATE_CODE_GOT || edit_state == EDIT_STATE_INIT_STATUS))
	{
		//��Ҫ��˸���ı༭��
		if(pEdit->blink)
		{
			// ����
			pEdit->blink		= 0;
			//gui_FillRect(x,y,pEdit->view[current_edit].font_size/6,pEdit->view[current_edit].font_size-2,BG_COLOR);
			gui_LineV(x,y+1,11,BG_COLOR);
		}
		else
		{
			// �����
			pEdit->blink		= 1;
			//gui_FillRect(x,y,pEdit->view[current_edit].font_size/6,pEdit->view[current_edit].font_size-2,TXT_COLOR);
			gui_LineV(x,y+1,11,TXT_COLOR);
		}
	}
	
}


/**
* @brief �Ƿ�������ڼ���״̬�ı༭��
* @return 0:������   1:����
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
* @brief ʹ��ĳһ���༭��
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
* @brief ʹĳһ���༭��ʧЧ
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
* @brief ���뷨���л�
*/
static void input_method_switch(TEdit *pEdit)
{
	TEditAttribute					edit_attr;			//��ǰ�༭�������

	edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������
	
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
		//�������Եı༭�����뷨�������л�
		input_method--;
	}

	input_update_flag = 3;
	return;
}

/**
* @brief ����ڱ༭��֮����л�
*/
void edit_switch(TEdit *pEdit,unsigned char key)
{
	unsigned char					i;
	unsigned char					current_edit;
	TEditAttribute					edit_attr;			//��ǰ�༭�������
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

	//��Ҫ�����ԭ���Ĺ��
	edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������

	if((edit_attr.style & 0x07) == EDIT_MONEY)
	{
		cur_pos = pEdit->display_max[current_edit]-1;
	}

	if((edit_attr.style & 0x10) != EDIT_NO_BLINK)
	{
		// ����
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-3 , gFontSize/2, BG_COLOR);
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-2 , gFontSize/2, BG_COLOR);
		//gui_FillRect(edit_attr.x + cur_pos*gFontSize/2,edit_attr.y,gFontSize/6,gFontSize-2,BG_COLOR);
		//gui_ClearLineV(edit_attr.x+cur_pos*8,edit_attr.y+1,11,TXT_COLOR);
		if (edit_attr.y + 13 * ((pEdit->display_max[current_edit] - 1)/20) > GUI_HEIGHT - SHOTCUT_HEIGNT - 12)
		{
			//�༭��������������ʾ����
			edit_attr.y = ICON_HEIGHT + 18 + edit_attr.y - (GUI_HEIGHT - SHOTCUT_HEIGNT );
		}
                  
         gui_LineV(edit_attr.x+cur_pos*gFontSize/2+pEdit->display_patch[current_edit],edit_attr.y+1,11,BG_COLOR);
	}


	if (KEY_UP == key)
	{
		//�Ϸ���
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
		//�·���
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
	pEdit->blink = 0;		//���������±༭��Ĺ��
	last_input_key = 0;		//�������İ�����0
	return;
}

/**
* @brief ���ֱ����ת��ָ���ı༭��
*/
void edit_switch_ext(TEdit *pEdit,unsigned char edit_index)
{
	unsigned char					i;
	unsigned char					current_edit;
	TEditAttribute					edit_attr;			//��ǰ�༭�������
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
	
	//��Ҫ�����ԭ���Ĺ��
	edit_get_attribute(pEdit,&edit_attr);		//��ȡ��ǰ�༭����������

	if((edit_attr.style & 0x07) == EDIT_MONEY)
	{
		cur_pos = pEdit->display_max[current_edit]-1;
	}

	if((edit_attr.style & 0x10) != EDIT_NO_BLINK)
	{
		// ����
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-3 , gFontSize/2, BG_COLOR);
		//gui_LineH(edit_attr.x + cur_pos*gFontSize/2, edit_attr.y+gFontSize-2 , gFontSize/2, BG_COLOR);
		//gui_FillRect(edit_attr.x + cur_pos*gFontSize/2,edit_attr.y,gFontSize/6,gFontSize-2,BG_COLOR);
		gui_LineV(edit_attr.x+cur_pos*gFontSize/2,edit_attr.y+1,11,BG_COLOR);
	}

	pEdit->current_edit = edit_index;
	pEdit->blink = 0;		//���������±༭��Ĺ��
	last_input_key = 0;		//�������İ�����0
	return;
}
/**
* @brief edit������ʾ���ַ�,��pValue���õ�g_editValue��
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
* @brief ��edit�е�ֵת������
* @note ע��edit�е�ֵ���Ǽ�ֵ������ascii
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
* @brief ��edit�е�ֵת�ɷŴ�100��֮���������Ϊ��֧��С�����2λ
* @note ע��edit�е�ֵ���Ǽ�ֵ������ascii
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
* @brief ��edit�е�ֵת�ɸ�����
* @note ע��edit�е�ֵ���Ǽ�ֵ������ascii
*/
float edit_atof(unsigned char *pValue)
{
#if 0
	int						i;
	float					value = 0;
	float					q;
	unsigned char			got_dot_flag = 0;

	//input_valid_num = 4;		//Ĭ�ϱ���С�����4λ
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
* @brief ������ת�ɼ�ֵ���ŵ��༭����
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
* @brief �����Ŵ�100��������ת�ɼ�ֵ���ŵ��༭����
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
* @brief ��������ת�ɼ�ֵ���ŵ��༭����
*/
//unsigned char edit_ftoa(float value, unsigned char *pValue)
//{
//
//}
/**
* @brief ���ַ�����תΪedit�е�ֵ����ֵ��
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
* @brief ��edit�е�תΪ�ַ���
*/
void edit_etos(unsigned char *indata, unsigned char *outdata, int len)
{
	//	unsigned char			c;
	int						i;

	for(i=0; i<len; i++)
	{       
		outdata[i] = *(key2ascii(indata[i]));
	}
	outdata[len] = 0x00;		// ������
}

/**
* @brief ��edit��ȡ������ֵ
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
*@brief	��ȡ��ǰ���Ե������ַ���,unicode����
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
* @brief ������ʾʱ��,ÿ��һ�����жϵ���
* @param[in] xpos
* @param[in] ypos
* @note ʱ�����ʾ��ɫ�����ˣ�������ʾʱ���λ�ñ�������ɫΪ����������
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
* @brief ע�������
* @param[in] unsigned char style	������������
* @param[in] unsigned short x		������������x
* @param[in] unsigned short y		������������y
* @param[in] unsigned short max		�����������ֵ
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
* @brief ע��������
*/
void process_bar_cancle(void)
{
	//process_bar.active = 0;
}

/**
* @brief ���½�����
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
		//��̬1�Ľ�����,ͬ���ֵ�޹�
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
* @brief ����ͼ��,GPRS�ź�ǿ�ȣ���ص����������ʾ��ͨ��״̬
* @param[in] int mode 0:����Ҫȡʵ�ʵ������ź�ǿ��    1:��Ҫȡʵ�ʵ������ź�ǿ��
* @return 0:����Ҫ���»���ԭ����  1:�͵��¼�    2:���»���ԭ����
* @note ��Ϊ����ʾ��������ʱ���Ҳ��Ҫȡʵ�ʵ������ź�ǿ�ȵĻ���������ΪGPRSģ�����Ӧ�Ƚ��������¿�����ʾ������ʱ
*		 ����һ�����Ե�ͣ�ٸ�
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
			//����״̬
			bluetooth_match_cmd_cnt = 0;
			gui_PictureOut(0,0,PIC_ANT5,refresh_flag);
		}
		else
		{
			gui_PictureOut(0,0,PIC_ANT1,refresh_flag);
			//����Ǵ���δ����״̬����ô�ͼ��2���ӷ���һ��ƥ������
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


	// ��ʾ��ص���
	if (USBLink_Insert_Detect())
	{		
		//û�в������USB��Դ
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
		default:	//��ص�����
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4,refresh_flag);
			lowpower_detect_cnt++;
		}

		if ((lowpower_detect_cnt > 3)&&(lowpower_detect_cnt<8))
		{	
			//gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4);
			//Beep(BEEP_DELAY*2);
			//gui_FillRect(0, ICON_HEIGHT+1, GUI_WIDTH, USERZONE_HEIGHT, BG_COLOR);
			//gui_TextOut(16, 60,"ϵͳ��������...",0);
			//gui_TextOut(18, 72,"�뱣�浱ǰ����!",0);
			//OSTimeDlyHMSM(0,0,1,0);
			lowpower_flag ++;
			return 1;
		}

		if (lowpower_detect_cnt >= 8)
		{
			OSSchedLock();		//��Ҫ�ٽ������������
			while (USBLink_Insert_Detect())
			{
				lowpower_cnt++;
				gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4,refresh_flag);
				Beep(BEEP_DELAY*2);
				for (i = 0;i < 1000000;i++);		//��ʱһС��ʱ��
				gui_FillRect(0, ICON_HEIGHT+1, GUI_WIDTH, USERZONE_HEIGHT, BG_COLOR,refresh_flag);
				gui_TextOut_ext(CENTER_ALIGN, 26,"ϵͳ��������...",0,refresh_flag);
				//@English
				gui_TextOut_ext(CENTER_ALIGN, 39,"������ⲿ��Դ!",0,refresh_flag);
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
					//�ͷ�POWER��֮���������ٹػ�
					Lcd_Clear(BG_COLOR);
					PowerOff();      ///�ػ�
				}

				if (lowpower_cnt > 100)
				{
					//��������͵籨��100�Σ���û�в�����ӵ�Դ���߹ػ�����ô��ǿ�ƹػ���
					Lcd_Clear(BG_COLOR);
					PowerOff();      ///�ػ�
				}
			}
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT_SHAN,refresh_flag);	//��ʾ���ͼ��
			OSSchedUnlock();		//������Ƚ���
			return 2;


		}
	}
	else
	{
		lowpower_flag = 0;
		//������USB��ӵ�Դ
		if (ChargeState_Detect())		//������
		{
                        charge_state_cnt = 0;
			gui_PictureOut(GUI_WIDTH-24,0, PIC_CHARGE,refresh_flag);		//��ʾ��ӵ�Դͼ�� 	
		}
		else		//���ڳ��
		{
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT4,refresh_flag);
			OSTimeDlyHMSM(0,0,0,200);
			gui_PictureOut(GUI_WIDTH-24,0, PIC_BAT_SHAN,refresh_flag);	//��ʾ���ͼ��
		}
	}

	return 0;
}

/**
* @brief ��ʾ�Ի����б༭������뷨ICON
*/
void show_input_method_icon(unsigned short x,unsigned short y,TEdit *pEdit,unsigned char mode)
{
	TEditAttribute		edit_attr;
	unsigned short		save_txt_color;


	save_txt_color =  gui_GetTextColor();
	if(0 == mode)
	{
		//��һ�δ����ı༭��ʱ
		//gui_FillRect(x,y,12*6,16,BG_COLOR);
		last_input_method = 0;

		//2013-03-12�����������б༭���Ĭ����ѡ���뷨���޸�Ϊ�������뷨��

		//��ȡ��ǰ�༭�������
		//edit_get_attribute(pEdit,&edit_attr);
		//if((edit_attr.style & 0x07) == EDIT_CHINESE)
		//{
		//	//�����������ĵı༭����Ҫ�л����뷨��pingyin��ABC��abc��123
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
		//	//����������ĸ�ı༭����Ҫ�л����뷨��ABC��abc��123
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
		//ֻ�е���һ��ʹ�õ����뷨�뵱ǰ���뷨��ͬʱ����Ҫ����ˢ�����뷨ICON
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
			gui_TextOut(x,y,"ƴ��",1,1);
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
* @brief ���Ի���pRes�����Ĳ˵�,�ӵ�index�ʼ��ʾ����
* @param[in] unsigned char highlight_current_item_flag  �Ƿ���Ҫ������ǰѡ����ı��
* @note ����������ָ����м��
*/
void show_menu(unsigned char highlight_current_item_flag)
{
	TDialogRes					*pRes;
	unsigned char						i,tmp_line = 0;
    unsigned short                                    y;
	unsigned char			dis_str[17];	
        

	pRes						= g_dlg.pRes;
	pContent					= get_content(); //pRes->pContent;

	// ���ʣ��ռ�
	tempy					= 0;
	if(g_dlg.pRes->style & STYLE_ICON || g_dlg.pRes->style & STYLE_TITLE)
	{
		tempy				+= ICON_HEIGHT+1;
	}

	y = tempy;

	gui_FillRect(0,tempy,GUI_WIDTH-24,user_zone_height, BG_COLOR,0);


	/** @note Ҫע�⣬����ֻ���ڿͻ��������������CLIENT_HEIGHT������߶����� */
	pContent					+= menu_start+1;		// ������һ�к͵ڶ������ݣ���һ���Ǳ��⣬�ڶ����ǿ�ݼ�

	i = 0;
	memset((void*)display_menu_item_array,0,sizeof(display_menu_item_array));
	while(pContent[tmp_line] && (tempy + gFontSize<= CLIENT_HEIGHT - SHOTCUT_HEIGNT) )
	{
		memset(dis_str,0x20,16);
		dis_str[16] = 0;
		memcpy(dis_str,pContent[tmp_line],strlen(pContent[tmp_line]));
		if(((menu_current_item - menu_start) == tmp_line)&&(highlight_current_item_flag == 1))
		{
			gui_SetTextColor(BG_COLOR);		//��ǰѡ�еĲ˵���ɫ��ʾ
		}
		else
		{
			gui_SetTextColor(TXT_COLOR);
		}
		gui_TextOut(0, tempy, dis_str,0,0);
		display_menu_item_array[i++] = menu_start+tmp_line;
		tmp_line++;
		tempy					+= gFontSize+1;		//�о�Ϊ1
	}

	//if (page_change_tag)
	//{
	//	//������ĳһ��������Ҫ��ʾ������ͷ�������Ҳ����˵��֧��ĳһ��������Ҫ��������ʾ
	//	if (menu_start == 2)
	//	{
	//		//��Ҫ��ʾ�·�ҳ��ͼ��
	//		gui_PictureOut(56,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_DOWN);
	//	}
	//	else
	//	{
	//		//��Ҫ��ʾ�Ϸ�ҳ��ͼ��
	//		gui_PictureOut(56,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_UP);
	//	}
	//}
	gui_SetTextColor(TXT_COLOR);
	gui_refresh(0,0,GUI_WIDTH+2,GUI_HEIGHT);
}

/**
***************************************************************************
*@brief	����һ���Ի���
*@param[in] 
*@return 0 ��ȷ
*       -1 ����
*@warning
*@see	
*@note 
***************************************************************************
*/
static void dlg_create(void)
{
	if( now_id > max_dlg )
	{
		// �����ˣ����������id�����ܳ��� DLG_MAX
		// ������Ӵ���ʱ����ʾ����
		while(1);

	}

	g_dlg.jmplst				= dlg_actionlist[now_id-1].jmplst;
	g_dlg.pRes					= dlg_actionlist[now_id-1].dlgres;
	g_dlg.callback				= dlg_actionlist[now_id-1].callback;
	g_dlg.active				= 1;			// ������ʱû��
	g_dlg.name					= dlg_actionlist[now_id-1].name;

	memset((unsigned char*)&g_dlg.edit, 0, sizeof(TEdit));
}
/**
***************************************************************************
*@brief	����Ƿ���Ҫ��ת������״̬
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
* @brief ����һ���Ի��򣬲�����Ի�����Ƽ�������
* @param[in] TDialog *pDlg Ҫ���еĶԻ�����
* @param[in] unsigned int mode
*        bit0   0:normal startup
*               1:startup without refresh
* @return �öԻ����˳�ʱ���صİ���
*/
static unsigned char dlg_run(unsigned char mode)
{
	int						starty;
	int						ret;
	unsigned char			i,j,k;
	unsigned int			update_cursor_cnt = 0;
	unsigned short			op_timeout;		//����رջ��߲�������ʱ
	unsigned char			*pKey, key;
	unsigned short			tmp_y;
	unsigned char			tmp_str[5];
	unsigned char			highlight_flag;	//��ʾ�˵�ʱ�Ƿ���Ҫ������ǰѡ����ı�־
	unsigned char			virtual_keypad_type;
	//unsigned char			virtual_keypad_buffer[31];
	unsigned char			return_from_edit_proc_state_flag;	//��ǵ�ǰ�Ǵ�����������ص�״̬��־
	static unsigned int			time_out;

	dlg_state						= DLG_INIT;
	//default_input_method_patch		= 0;
	number_edit_patch				= 0;
	highlight_flag	= 1;		//Ĭ����Ҫ������ֻ�а����˱༭��Ĵ��岻��Ҫ
	return_from_edit_proc_state_flag = 0;
	
	switch(g_param.operate_timeout)
	{
	case 1:	//30S
		time_out = 119; 
		break;
	case 2:	//1����
		time_out = 238;
		break;
	case 3: //2����
		time_out = 476;
		break;
	case 4: //5����
		time_out = 1190;
		break;
	default:	//10����
		time_out = 2380;
	}

	while(1)
	{
		switch(dlg_state)
		{
			/* *************************************************************************** */
			/* �Ի����ʼ������ */
			/* *************************************************************************** */
		case DLG_INIT:

			// ************�ص��ӿ�һ************
			// �����ڴ���ǰ������
			// ����Щ���徭״̬����ת��������ڴ˴�ֱ�ӷ���
			update_icon_cnt = 0;
			task_exit_flag = 0;
			scan_start = 0;
			roll_display_item_cnt = 0;
			roll_update_cnt = 0;
			gui_SetTextColor(TXT_COLOR);
			if(g_dlg.callback)
			{
				// �˴�ִ�д��ڴ����ص�
				howrun				= (g_dlg.callback)(DLGCB_CREATE,&key);

				// �˴����ܷ���
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
	
			starty					= 0;			// �����y����
			edit_state = EDIT_STATE_INIT_STATUS;	//��ʼ���༭��״̬ 
			user_zone_height = GUI_HEIGHT-SHOTCUT_HEIGNT;

			//���������Ҫ��ʱ���أ���ô�˴����
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
			// ����ͼ�������̶�����
			if( g_dlg.pRes->style & STYLE_ICON )
			{
				starty				+= ICON_HEIGHT+1;
				user_zone_height -= ICON_HEIGHT+1;


				//���ǰ���Ѿ��д��������ͼ�����Ͳ���Ҫˢ����
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

				//��ʾλ��ICON���ı��⣬����̶�������ʾ4������
				//pContent			= get_content();
				//gui_FillRect(0,0,12*5,12, BG_COLOR,1);
				//gui_TextOut(0, 0, pContent[0],0,1);			//�������������Ǵ����һ������
			}
			else
			{
				//û��ͼ�����Ĵ��壬ͼ�����������
				icon_draw_flag = 0;

				if( g_dlg.pRes->style & STYLE_TITLE )
				{
					starty				+= ICON_HEIGHT+1;
					user_zone_height -= ICON_HEIGHT+1;
					gui_FillRect(0,0,GUI_WIDTH,ICON_HEIGHT, BG_COLOR,0);
					gui_LineH(0,ICON_HEIGHT,GUI_WIDTH,TXT_COLOR,0);
					pContent			= get_content();
					gui_TextOut(0, 0, pContent[0],0,0);			//�������������Ǵ����һ������
				}
			}

			if (0 == mode)
			{
				//������´����Ĵ���,������
				gui_FillRect(0,starty,GUI_WIDTH,user_zone_height, BG_COLOR,0);
			}

			//��ʼ�������еı༭��
			if((g_dlg.pRes->pEdit)&&(return_from_edit_proc_state_flag == 0))
			{
				//����еĴ���ı༭����Դ�����ڱ���׶���ȷ�����˵ģ�ֻ��������ʱ����ȷ����������ô�����ڴ���CREATE�׶εĻص�����������һ����ǣ�����
				//�˴���ı༭����Դ��Ҫ������ʱ��̬�����������Ǵӱ���ʱ����Դ������װ�ء�
				if (edit_res_rebuild_flag)
				{
					//g_dlg.edit.pRes = edit_res_rebuild;			//ָ��̬�����ı༭����Դ����
				}
				else
				{
					g_dlg.edit.pRes		= g_dlg.pRes->pEdit;		// ����edit��������Դ������ʲô,�˴�ָ��ı༭���������ڱ���ʱ�Ѿ�ȷ����
				}

				edit_init(&g_dlg.edit);
				update_input_method_mode = 0;
			}

			//������Ҫ�ڽ���������һ����ʾ��ݲ˵�

			pContent			= get_content();
			gui_FillRect(0,CLIENT_HEIGHT-SHOTCUT_HEIGNT,GUI_WIDTH,SHOTCUT_HEIGNT, BG_COLOR,0);
			if( g_dlg.pRes->style & STYLE_SHORTCUT )
			{	
				gui_TextOut(0, CLIENT_HEIGHT-SHOTCUT_HEIGNT, pContent[1],0,0);			//������������Ǵ���ڶ�������
			}


			
			tmp_y = starty;
			dlg_state				= DLGCB_DRAW;		// �Ի��������ƴ���״̬������������ͼ�꼰��������

			break;

		case DLGCB_DRAW:
			// ==========================================================================================================
			// ��ʾ�Ի�������
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
				// ͳ��ÿҳ�Ĳ˵�����
				if( starty + gFontSize <= CLIENT_HEIGHT - SHOTCUT_HEIGNT)
				{
					starty			+= gFontSize+1;
					menu_pagecnt	++;
				}

				// ͳ�Ʋ˵�������
				menu_all			++;							// �˵����һ
				tempx				++;
			}

			if(g_dlg.pRes->pEdit)
			{
				highlight_flag = 0;
			}

			// ��ʾ�˵�
			if( pContent[1] )
			{
				show_menu(highlight_flag);
			}

			// ==========================================================================================================
			// ************ �ص��ӿڶ� ************
			// �˴������׼���������Ʋ�����Ԫ��
			if((g_dlg.callback)&&(return_from_edit_proc_state_flag == 0))
			{
				// ִ�в���ص�
				howrun				= (g_dlg.callback)(DLGCB_DRAW,&key);
				// �˴����ܷ���
				if(howrun == RUN_NEXT)
					return key;

				if(howrun == RUN_PREV)
					return KEY_ESC;
			}

			//����Ƿ�����ʾ���壬���ִ���û�лص�����,����������
			if(g_dlg.pRes->style & STYLE_TIP)
			{
				Beep(BEEP_DELAY);
			}

			// �༭��ˢ����ʾ
			if(g_dlg.pRes->pEdit)
			{
				edit_refresh(&g_dlg.edit,0,0);//ˢ�����б༭��
				//if (edit_is_active(&g_dlg.edit))
				//{
				//	//��������˿��Ա༭�ı༭����ʾ��ǰ�༭���Ĭ�����뷨״̬
				//	show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,update_input_method_mode);
				//}
			}

			dlg_return_from_child = 0;
			flush_message();		//����������������֮ǰ���µİ��������

			// ��ʾ��Ť
			dlg_state				= DLG_PROC;		// �Ի������ѭ��״̬

			break;

			/* *************************************************************************** */
			/* �Ի������в��� */
			/* *************************************************************************** */
		case DLG_PROC:
			// ȡ�ð�������ȴ�������Ϣ
			if(g_dlg.edit.active ||  (g_dlg.pRes->style & STYLE_ICON) != 0 || g_dlg.delay != -1)
			{
				pKey				= dlg_getmessage(0, 25);	//�б༭��Ľ��棬��Ҫ����ͼ��Ľ���
			}
			else
			{
				//����Ҫ����ǰ�������Ĵ��壬�Ͷ��ڴ˴�һֱ�ȴ������¼��ķ���
				pKey				= dlg_getmessage(0, 0);
			}


			if( pKey == 0 )
			{
				if((g_dlg.pRes->style & STYLE_ICON)&&(no_update_icon_flg == 0))
				{
					//������ICON�Ľ�����Ҫ����ICON�ͽ���ʱ��
					// ����ʱ��
					dlg_updatetime(52, 0);
					// ����ͼ��
					if(++update_icon_cnt >= 5)
					{
						update_icon_cnt	= 0;
						ret = dlg_update_icon(1,1);
						if (ret == 1)
						{
							//dlg_state				= DLG_INIT;			//���»��ƴ˴���
							//break;
							key = KEY_LOW_POWER;      //�൱�ڰ������˳���
							goto dlg_handle_key;
						}
						else if (ret == 2)
						{
							dlg_state				= DLG_INIT;			//���»��ƴ˴���
							break;
						}
					}
				}

				if ((USBLink_Insert_Detect() == 0)&&(g_param.transfer_mode == TRANSFER_MODE_U_DISK)&&(bDeviceState == CONFIGURED))
				{
					u_disk_proc();
				}

				//2012.02.15 joe.zhou����
				//�������ע����һ������������ô��Ҫ���½�����
				//if (process_bar.active)
				//{
				//	//����������Ѿ��������ô��Ҫ���½�����
				//	process_bar_update();
				//}

				//�����˷�ֻ�����Եı༭��Ĵ�����Ҫ��˸���
				if(edit_is_active(&g_dlg.edit))
				{
					input_update_flag++;
					if ((input_update_flag >= 3)&&(the_last_key == 1))
					{
						the_last_key = 0;//������һ�������������
					}
					if(++update_cursor_cnt >= 3)
					{
						update_cursor_cnt = 0;
						edit_blink(&g_dlg.edit,0);
					}	
				}

				//2013.01.31 joe.zhou����
				//��������Ҫ������ʾ�Ĵ�����Ҫ����ʾ���ݽ��й�����ʾ
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

				//�趨����ʱ�Ĵ�����Ҫ��������ʱ�Զ�������һ������
				if( g_dlg.pRes->delay)
				{
					if(g_dlg.pRes->style & STYLE_TIP)
					{
						if( op_timeout == 4)
						{
							op_timeout = 0;
							if(g_dlg.callback)
							{
								// ִ���˳��ص�
								howrun			= (g_dlg.callback)(DLGCB_DESTORY,&key);
							}
							return  KEY_ESC;		//�൱�ڰ������˳���
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
							//�趨����ʱʱ��Ľ��棬���ݽ����趨����ʱ����ʱʱ�䵽֮���൱�ڰ�����ESC��
							if( op_timeout == (g_param.operate_timeout*200 + g_param.operate_timeout*80/4) )  //2800
							{
								op_timeout = 0;
								key = KEY_ESC;      //�൱�ڰ������˳���
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
					//û���趨��ʱ�Ľ��棬��Ҫ����ʱ�䵽֮��ر���Ļ���⣬����͹���ģʽ
					if ((g_param.operate_timeout)&&(USBLink_Insert_Detect() == 1))	//�ⲿ����ʱ���豸�������͹���ģʽ
					//if (g_param.operate_timeout)
					{
						if(op_timeout > time_out)		//600//��ʱʱ�䵽����Ҫ�رձ��⣬���ؽ���͹���ģʽ
						{
							Lcd_Enter_SleepMode();
							//gui_CtrlBL(0);
							// enable Debug in Stop mode
							DBGMCU->CR |= DBGMCU_CR_DBG_STOP;	//����ʱ�򿪣��������͹���ʱ��Ͽ���JLINK������

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

				//�˴��ṩһ���ӿڿ�����ĳЩʱ����û�л�ȡ��������Ҳ���Խ���һ�δ���������������ȥ
				//if (auto_run_flag)
				//{
				//	auto_run_flag = 0;
				//	goto dlg_handle_key;
				//}
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//�˶δ�����20110922��ӣ�����һ���ӿڸ����壬����ʹ������DRAW��֮��û���κΰ�������֮ǰ
				//ѭ�����һЩϵͳ�¼�������USB�豸�Ƿ�����ö�ٳɹ�����ص���������������Ӧ�ķ�Ӧ��
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

			//�л�ȡ��������Ϣ
			key						= *pKey;

			if (((scan_start)&&(is_key_keypad(key) == 0))||(lowpower_flag > 0))
			{
				break;
			}

			//////////////////////////////////////////////////////////////////////////
			//�˶δ�����20110922��ӣ���������KEY_CLEAR����KEY_ESC���Ĺ��ܺ϶�Ϊһ��
			//��ǰ����û�б༭��ʱ����KEY_CLEAR���൱��KEY_ESC
			//�����ǰ�����б༭��ʱ������KEY_CLEAR���������ʱ����������û������ֵʱ�൱�ڰ�����KEY_ESC.
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
			//	if (op_timeout > (g_param.operate_timeout*200 + g_param.operate_timeout*80/5))		//����Ǳ����Ѿ����رյ�״̬
			//	{				
			//		gui_CtrlBL(1);		//���¿�����
			//		op_timeout		= 0;
			//		break;					//��������ô��һ�ΰ�����ֻ�ǽ��رյı����
			//	}
			//}
			op_timeout		= 0;

			// ************ �ص��ӿ��� ************
			// ����Ϣ�������û�����ϵͳ��Ϣ��������Ϣ����ʱ����Ϣ
			if(g_dlg.callback)
			{
				// ��ϵͳ��Ϣ�ַ���Ӧ�ô���,msgΪ�����Ϣ��ҲΪ������Ϣ
				//if (((KEY_ENTER == key  || KEY_UP == key || KEY_DOWN == key)&&(edit_state != EDIT_STATE_HZ_SELECT)&&(edit_state != EDIT_STATE_PY_SELECT)&&
				//	(edit_state != EDIT_STATE_ZD_NAME_SELECT)&&(edit_state != EDIT_STATE_ZD_VALUE_SELECT))||((key != KEY_ENTER)&&(key != KEY_UP)&&(key != KEY_DOWN)))
				{
					howrun				= (g_dlg.callback)(DLGCB_RUN,&key);
					// ��������
					if(howrun == RUN_RERUN)
					{
						break;
					}

					if (howrun == RUN_RETURN)
					{
						dlg_state				= DLG_DRAW;		// �Ի������»���
						break;
					}

					if (howrun == RUN_EDIT_PROC)
					{
						dlg_state = DLG_EDIT_PROC1;
						break;
					}

					// Ҫ������ǰ�Ի���
					if(howrun == RUN_PREV || howrun == RUN_NEXT)
					{
						// ************ �ص��ӿ��� ************
						if(g_dlg.callback)
						{
							// ִ���˳��ص�
							howrun			= (g_dlg.callback)(DLGCB_DESTORY,&key);
						}
						return key;
					}
				}
			}

			//��������ּ����򽻸�edit����
			//if( (is_key_num(key) == 0) && (edit_is_active(&g_dlg.edit)))
			//{
			//	edit_proc(&g_dlg.edit, key);
			//	input_update_flag = 0;
			//}


			//�˰����ڰ����˿ɱ༭�ı༭��Ĵ����������л����뷨��
#if 0
			if ((KEY_jing == key)&&(edit_is_active(&g_dlg.edit)))
			{
				input_method_switch(&g_dlg.edit);	
				show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,1);//��ʾ��ǰ�༭������뷨ICON
				//�����ǰ�༭��״̬�����������뷨״̬�����ֽ����뷨�л�Ϊ���������뷨ʱ����Ҫ���ר��Ϊ�������뷨���Ƶ��������»��ƽ���
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
						edit_refresh(&g_dlg.edit,0,0);//ˢ�����б༭��
					}
					else
					{
						for (i= 0;i < 4;i++)
						{
							edit_refresh(&g_dlg.edit,1,need_refresh_edit_index_tbl[i]-1);//ˢ����Ҫˢ�µı༭��
						}

						//ˢ�����·�ҳ��
						gui_PictureOut(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_UP);		//��ʾ�·�ҳ��ͼ��
						gui_PictureOut(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_DOWN);		//��ʾ�·�ҳ��ͼ��
					}
					
					show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,0);
					break;
				}
			}

			//��������������뷨ʱ���༭���Ѿ���ȡ��һ�����ּ���Ӧ�����ı��룬�����ı༭�������ʵ���Ͽ����ڴ˴�ʵ���������빦�ܣ�
			if ((input_method == PINYIN_INPUT)&&(edit_state == EDIT_STATE_CODE_GOT))
			{
				//���ƴ�����뷨����ʾ����
				gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H+1,BG_COLOR);
				gui_FillRect(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,16,12,TXT_COLOR);
				gui_FillRect(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,16,12,TXT_COLOR);
				update_input_method_mode = 1;
				edit_state = EDIT_STATE_INIT_STATUS;
				show_menu();
				refresh_draw_content();
				if (need_refresh_edit_index_tbl[0] == 0)
				{
					edit_refresh(&g_dlg.edit,0,0);//ˢ�����б༭��
				}
				else
				{
					for (i= 0;i < 4;i++)
					{
						edit_refresh(&g_dlg.edit,1,need_refresh_edit_index_tbl[i]-1);//ˢ����Ҫˢ�µı༭��
					}
					gui_PictureOut(32,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_UP);		//��ʾ�·�ҳ��ͼ��
					gui_PictureOut(80,CLIENT_HEIGHT-SHOTCUT_HEIGNT,PIC_DOWN);		//��ʾ�·�ҳ��ͼ��
				}
				show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,1);
				break;

			}

			if (EDIT_STATE_CODE_GOT == edit_state)
			{
				edit_state = EDIT_STATE_INIT_STATUS;
				break;			//20120521 ����
			}

#endif
			// �������·�ҳ
			if((KEY_UP == key|| KEY_DOWN == key) && (edit_state == EDIT_STATE_INIT_STATUS))
			{
				//��������˷�ֻ���༭��Ĵ���,��û�д����������뷨״̬ʱ
				if(edit_is_active(&g_dlg.edit))
				{
					//�����ת����һ����ֻ���༭��
					edit_switch(&g_dlg.edit,key);
					//show_input_method_icon(54,GUI_HEIGHT-SHOTCUT_HEIGNT,&g_dlg.edit,0);
					update_cursor_cnt = 0x100;		//��֤�л�֮��ı༭��Ĺ����������
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
			// ����Ƿ��صİ������򷵻��˳�
			if(((KEY_ENTER == key) &&(edit_state == EDIT_STATE_INIT_STATUS)) || KEY_ESC == key)
			{
				// ************ �ص��ӿ��� ************
				if(g_dlg.callback)
				{
					// ִ���˳��ص�
					howrun			= (g_dlg.callback)(DLGCB_DESTORY,&key);
				}

				// ֱ�ӷ���
				return key;
			}

			// �����һ����
			ret						= check_and_jump(key);
			if( ret != -1 )
			{
				return key;
			}
			break;

			/* *************************************************************************** */
			/* �Ի�����ҳ���� */
			/* *************************************************************************** */
			// �˵��ı���
		case DLG_PAGECHANGE:
			if( KEY_DOWN == key)
			{	
				// �·�ҳ
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
			{	// �Ϸ�ҳ
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
				// ִ�лص�
				howrun			= (g_dlg.callback)(DLGCB_PAGECHANGE,&key);
			}

			dlg_state				= DLG_PROC;
			break;

		case DLG_EDIT_PROC1:
			//��ת������������̽��������״̬
			gui_FillRect(0,0,GUI_WIDTH,GUI_HEIGHT,BG_COLOR,1);
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-ICON_HEIGHT,"Enter    Select   Del",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-ICON_HEIGHT,"ȷ��    ѡ��     ɾ��",0,1);
			}

			//��ʾ�����Ѿ����õı༭���ֵ
			//strcpy(virtual_keypad_buffer,g_editValue[current_edit]);//�������������֮ǰ�ļ�ֵ��������
			virtual_keypad_type = edit_refresh_ext(&g_dlg.edit);

			//��ʾ�������
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
					the_last_key = 0;//������һ�������������
				}
				if(++update_cursor_cnt >= 3)
				{
					update_cursor_cnt = 0;
					edit_blink(&g_dlg.edit,1);
				}

				break;
			}

			//�л�ȡ��������Ϣ
			key						= *pKey;

			if (key == KEY_ESC)
			{
				//����������̽��յ�������
				if(g_dlg.callback)
				{
					// ִ�д�������̷���ʱ�Ļص�
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
				//��������̽��յ�������Ż�ԭ������ı༭����
				if(g_dlg.callback)
				{
					// ִ�д�������̷���ʱ�Ļص�
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
*@brief	����������ѭ��
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
* @brief ��task1���ã�����Ƿ��к�̨������Ҫ���У������������񡣲������н�����ء�
* @note �����磺PN532ˢ����GPRSͨ�ŵ�
*/
void dlg_run_task(void)
{
	unsigned char					err_code;
	int								ret_code;

	OSSemPend(pBGCommand, 0, &err_code);

	if(err_code == OS_NO_ERR)
	{
		// ���Ҫ����һ������
		if( SCommand == TASK_RUN )
		{
			SStatus						= 1;			// busy

			if( SbgTask )
			{
				// ֱ�ӷ��ؼ�ֵ
				ret_code				= SbgTask(SParam1, SParam2, SParam3, SParam4);
			}
			else
			{
				ret_code				= -1;
			}
			SStatus						= 0;

			// ������������棬��֪�������
			task_send_command_to_ui(ret_code);
		}
	}
}



/**
* @brief ��������һ����̨����
* @return 0:����ɹ��ݽ�
*        -1:�����������У�����ʧ��
*/
int dlg_run_bgtask(TbgTask callback, int param1, int param2, void *param3, void *param4)
{
	if( (dlg_read_bgtask_status() != 0) && callback)
	{
		if (SbgTask != callback)
		{
			//��������������̨�������е�����ͬ����ô�ͽ���̨�������е��������
			SCommand						= TASK_EXIT;

			OSSemPost(pBGCommand);
			while(SStatus)				//�ȴ���ǰ��̨�������
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
* @brief ��UI���ã�����̨��������״̬
*/
int dlg_read_bgtask_status(void)
{
	return SStatus;
}

/**
* @brief ���淢��һ�����������
* @return 0:����ͳɹ�
*        -1:�����ʧ��
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
* @brief ��һ������UI������
* @param[in] int *command ��ȡһ����������������
* @return 0:�ɹ�
*        -1:������
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
		OSSemPend(pBGCommand, timeout, &err_code);	// �Ƿ�������ǰ̨�����
		if(err_code == OS_ERR_NONE )
		{
			*command					= SCommand;
			return 0;
		}
	}while(waitend);

	return -1;
}


/**
* @brief ������һ������(��ֵ)��ǰ̨ui
* @return 0:���ͳɹ�
*        -1:����ʧ��
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


//����ĳ������תʱһЩ��Ҫ�������Ϣ
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

//����ĳ������תʱһЩ��Ҫ�������Ϣ
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



