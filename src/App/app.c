/**
* @file app.c
* @brief 2.4GPOS��ĿӦ��������
*
* @version V0.0.1
* @author joe
* @date 2010��04��28��
* @note
*		�ο����й��ƶ��ֻ�֧��ҵ���ֳ��ѻ�֧��POS�ն� �淶_v1.0.0.doc��
* 
* @copy
*
* �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
* ����˾�������Ŀ����˾����һ��׷��Ȩ����
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

const unsigned char *app_ver="V1.0.1";		//Ӧ�ð汾��

static unsigned char	BarCode[31];				//���������ĺ�̨������ǰ̨֮�䴫��������ı���
static unsigned char	last_BarCode[31];			//�ϴλ�ȡ����������
static unsigned char	BarCode_Type[20];			//��������
static unsigned char	BarCode_Len;				//���볤��
static unsigned char	dlg_refresh_flag;	//����ǿ��ˢ�µı��
static unsigned int		barcode_got_flag;				//��ȡ��������ı��

static unsigned char	scan_func_state;	

static unsigned char	line3_save[21];

static unsigned char	special_func_flag;




unsigned char			need_refresh_signal;			//��Ҫˢ�������ź�ǿ�ȵı��
unsigned char			no_update_icon_flg;
unsigned char			task_exit_flag;
unsigned char			usb_cable_state;
unsigned char			need_refresh_edit_index_tbl[4];	//��Ҫˢ�µı༭���б�
unsigned int					scan_start;						//��ʼɨ��ı��

extern TDialog					g_dlg;
extern unsigned char			g_editValue[MAX_EDIT][MAX_EDITNUM];//�༭���ȫ�ֱ���
extern unsigned char			menu_current_item;			//��ǰ�˵���
extern unsigned char			menu_start;
extern unsigned char			max_dlg;
extern unsigned char			next_dlg_id;
extern	TTerminalPara			g_param;							//Terminal Param
//extern TDRAW_ITEM				draw_content[];
extern unsigned char			gFontSize;			// �ֺ�
extern unsigned char			display_menu_item_array[5];
//extern int						bluetooth_module_state;
extern FIL						file2;
extern int						bluetooth_match_cmd_cnt;
extern int						bluetooth_state_cnt;
extern TypedefDateTime       currentDateTime;	
extern unsigned char			data_trans_state;				//���ݵ��뵼��ʱ��״̬
extern unsigned char			update_icon_cnt;
extern unsigned int				vcom_device_state;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


////////////////////////////���ڹ������Գ���///////////////////////////////////
#define  KEY_COUNT   10 //��������Ŀ
/**
*@brief ����ɨ��ʱ����ÿ����������Ļ�ϵ���ʾλ�ü��䰴��������Ļ�ϵĿ����Ϣ
*/
typedef struct  {
	unsigned char key;
	unsigned char x;
	unsigned char y;
	unsigned char width;
	int		      clear_flag;		//�Ƿ��Ѿ�ɨ��
} Tkeyscan_pos_width;

static int factory_test_task(int param1, int param2, void *param3, void *param4);
static unsigned char dlgproc_test_proc(unsigned char *key);
static int scan_aging_task(int param1, int param2, void *param3, void *param4);

static void test_LCD_display(void);
static void testKeypad(void);//��������
static void testScanerInit(void);//ɨ��ͷ����
static void testBluetooth(void);//��������
static void testUSB(void);//USB����
static void testRTC(void);//rtc����
static void testAD(void);

typedef void (*testfunc)(void);		//������Ժ���ָ��
typedef	void (*pFunction)(void);

const  testfunc testfunc_tbl[] = {
	test_LCD_display,			//������ʾ
	testKeypad,					//���Լ���
	testScanerInit,				//����ɨ��ͷ
	testUSB,					//USB����
	testRTC,					//RTC����
	testAD,				    //AD��������
	testBluetooth				//��������ģ��
};


/**
* @brief �������ǲ��԰���ɨ��ʱ����������Ļ�ϵ���ʾλ�����Ӧ��
* @see	ÿ��������ɨ��ʱ����Ӧ����������Ļ�ϵ���ʾ���£�
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

	{0xff,0,0,0,0}//��Ľ������
};

static int						testproc_first_enterkey;//���Գ������һ�β��԰����ı��
static  unsigned char			enter_scaner_test_flag;//����ɨ����Եı�־

/**
***************************************************************************
*@brief ��̨��������������
*@param[in] int param1  ָ���Ƿ���ϵͳ���õ�����λ���Ĺ�������  0����  1������
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
					//�Ѿ����������⵽����������,��ת��ѯ���Ƿ���Ҫ���¿����Ϣ�Ľ���
					return KEY_EXT_2;
				}
				else
				{
					//��û���������⵽�������������ôֱ�ӷ��ص�������
					return KEY_EXT_3;
				}
			}

			if (TASK_RUN == command)
			{
				//�û�����ɨ���
				//��ʼɨ����Ʒ������
				//scan_start = 1;
				memset(BarCode,0,31);
				memset(tmp,0,31);
				memset(BarCode_Type,0,20);
				BarCode_Len = 0;
				if (UE988_get_barcode(BarCode_Type, tmp,30, &BarCode_Len) == 0)
					//if (scanner_get_barcode(tmp,30,BarCode_Type,&BarCode_Len)  == 0)
				{
					//��ȡ������
					//led_r_ctrl(1);
					Delay(5000);
					Beep(BEEP_DELAY);
					//led_r_ctrl(0);
					barcode_got_flag = 1;
				}
				//else
				//{
				//	//��ȡ������ʧ��
				//	//����Ҫ����Ҳ����Ҫ֪ͨ�û����͵���û��ɨ�赽����

				//	task_send_command_to_ui(KEY_EXT_1);
				//}

				if (param1 == 0)
				{
					//��ʾ��Ҫ����ϵͳ���õ�¼������λ�������ػ�ȡ��������
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

				//ģ��  
				//OSTimeDlyHMSM(0,0,0,10);
				//strcpy((char*)BarCode,"123456789123");
				//task_send_command_to_ui(KEY_EXT_1);
			}

		}
		OSTimeDlyHMSM(0, 0, 0, 300);
	}
}

/**
* @brief ��������ʱ���ʽ�Ƿ�Ϸ�
* @note  �����ʱ���ʽ��YYYYMMDDHHMMSS
* return  0: �Ϸ�
*        -1: �Ƿ�
*/
static int check_time(unsigned char * input)
{
	unsigned short year,month,date,temp;
	year = ((*input)- 0x30)*1000 + (*(input + 1) - 0x30)*100 + (*(input + 2) - 0x30)*10 + (*(input + 3) - 0x30);

	if (year < 2000 ||year > 2030)//����������2000~2030֮�������
	{
		return -1;
	}

	//��������·ݵĺϷ���
	month = (*(input + 4) - 0x30)*10 + (*(input + 5) - 0x30);		//���·�ת��Ϊ10����
	if ((month > 12)||(month == 0))
	{
		return -1;
	}

	//����������ڵĺϷ���
	date = (*(input + 6) - 0x30)*10 + (*(input + 7) - 0x30);		//������ת��Ϊ10����
	//�´�31��
	if ((month == 1)||(month == 3)||(month == 5) ||(month == 7) ||(month == 8) ||(month == 10) ||(month == 12))  
	{
		if ((date > 31)||(date == 0)) 
		{
			return -1;
		}
	}
	//2�·�
	else if (month == 2) 
	{
		if ((year%4==0 && year%100!=0) || (year%400==0))		//����
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
	//��С
	else
	{
		if ((date > 30)||(date == 0)) 
		{
			return -1;
		}
	}


	//�������Сʱ�ĺϷ���
	temp = (*(input + 8) - 0x30)*10 + (*(input + 9) - 0x30);		//��Сʱת��Ϊ10����
	if (temp > 23) 
	{
		return -1;
	}
	//���������ӵĺϷ���
	temp = (*(input + 10) - 0x30)*10 + (*(input + 11) - 0x30);		//������ת��Ϊ10����
	if (temp > 59) 
	{
		return -1;
	}
	//������������ĺϷ���
	temp = (*(input + 12) - 0x30)*10 + (*(input + 13) - 0x30);		//������ת��Ϊ10����
	if (temp > 59) 
	{
		return -1;
	}

	//�Ϸ�������,���û������ʱ����µ���ǰʱ������
	currentDateTime.year = (unsigned char)(year - 2000);
	currentDateTime.month = (unsigned char)month;
	currentDateTime.day = (unsigned char)date;
	currentDateTime.hour = (*(input + 8) - 0x30)*10 + (*(input + 9) - 0x30);
	currentDateTime.min = (*(input + 10) - 0x30)*10 + (*(input + 11) - 0x30); 
	currentDateTime.sec = (*(input + 12) - 0x30)*10 + (*(input + 13) - 0x30); 
	return 0;		
}


//������ʾ
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
		gui_TextOut_ext(CENTER_ALIGN,39,"����ƥ�����!",0,0);
		strcpy(line3_save,"����ƥ�����!");
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	OSTimeDlyHMSM(0,0,0,50);
	Beep(BEEP_DELAY*2);
	OSTimeDlyHMSM(0,0,0,50);
	Beep(BEEP_DELAY*2);
	return RUN_RERUN;
}


//��ʽ���������ʾ��ʽ
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
* @brief ɨ�蹦��
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
				gui_TextOut(0,13,"��������:",0,1);
			}

			format_barcode_disp_str(basical_barcode,dis_str);
			gui_TextOut_ext(CENTER_ALIGN,26,dis_str,0,1);

			scan_func_state = 0;
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//��̨�ȴ���������������
		}
		else
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,13,"Basical:",0,1);
			}
			else
			{
				gui_TextOut(0,13,"��������:",0,1);
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
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//��̨�ȴ���������������
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
				//��̨û��ɨ�赽�κ�������
				return RUN_RERUN;
			}

			//����Ǻ�̨ɨ�赽�����룬��ô��Ҫ����̨��ȡ������������ʾ����
			
			//��Ҫ���жϣ�����������������Ƿ�Ͳ�������һ��
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
* @brief �ֶ���������
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
* @brief ֻ��Ҫͨ�����·�ҳ��ѡ��˵���Ȼ��ͨ��ENTER����ת����һ���˵��Ĵ����ͨ�ô�����
* note   ע����Щ�������ת��ֵ������˵�������Ϊ��Ӧ��KEY_EXT_1,KEY_EXT_2,...
*/
static unsigned char proc_list_menu(unsigned char type,unsigned char *key)
{
	if (DLGCB_RUN == type)
	{
		if (KEY_SCAN == *key || KEY_ENTER == *key)
		{
			*key = KEY_EXT_1 + menu_current_item - 1;
			saveDrawContext();	//���浱ǰ�˵�����ת���Ӳ˵�ʱ��״̬
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
* @brief ɨ�蹦��ѡ��
*/
static unsigned char dlgproc_scan_option_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}

/**
* @brief ϵͳ��Ϣ
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
			saveDrawContext();	//���浱ǰ�˵�����ת���Ӳ˵�ʱ��״̬
			return RUN_NEXT;
		}

		if (KEY_RIGHT_SHOTCUT == *key)
		{
			*key = KEY_ESC;
			releaseDrawContext();
			return RUN_NEXT;
		}

		//�������ⰴ������: "LEFT_SHOTCUT��LEFT��RIGHT��LEFT_SHOTCUT��LEFT��RIGHT��LEFT_SHOTCUT" �������ع��ܲ˵�
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
* @brief	�ն����ò˵�
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
* @brief ѡ��ѡ���ͨ�ô�����
* ע��:ѡ�������0,1,2��.....(0��ʼ)
*/
static unsigned char proc_option_select(unsigned char type,unsigned char *key,unsigned char *param)
{
	if (DLGCB_DRAW == type)
	{
		//��ʾ��ǰ���õ�ѡ��
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
					//��������˷����룬��ô����Ҫ�ر�����ͳ�ƹ��ܺ����ݿ��ѯ����
					g_param.database_query_option = 0;
					g_param.barcode_statistic_option = 0;
				}
			}
			else if (param == &g_param.database_query_option)
			{
				if (g_param.database_query_option)
				{
					//������������ݿ��ѯ����ô����Ҫ�ر�����ͳ�ƹ��ܺͷ����빦��
					g_param.avoid_dunplication_option = 0;
					g_param.barcode_statistic_option = 0;
				}
			}
			else if (param == &g_param.barcode_statistic_option)
			{
				if (g_param.barcode_statistic_option)
				{
					//�������������ͳ�ƣ���ô����Ҫ�رշ����빦�ܺ����ݿ��ѯ����
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
				gui_TextOut(0,26,"�밴\"SCAN\"��ɨ��...",0,1);
			}
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//��̨�ȴ���������������
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
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ȡ��            ȷ��",0,1);
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
				//��̨û��ɨ�赽�κ�������
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
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ȡ��            ȷ��",0,1);
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
* @brief �ֶ���������
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
* @brief ����ѡ��
*/
static unsigned char dlgproc_language_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.language);
}


/**
* @brief ����������ѡ��
*/
static unsigned char dlgproc_beeper_vol_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.beeper_vol);
}


/**
* @brief ��ʱѡ��
*/
static unsigned char dlgproc_timeout_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.operate_timeout);
}




/**
* @brief ��������ѡ��
*/
static unsigned char dlgproc_barcode_option(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}

/**
* @brief ʱ���������ò˵�
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
			// ȡ�ñ༭��ֵ
			if(g_dlg.edit.now[0] != g_dlg.edit.input_max[0])
			{
				return RUN_RERUN;
			}

			//��ֵת��Ϊ�ַ���
			edit_etos(g_editValue[0],input_time,g_dlg.edit.now[0]);
			/** @note �ڴ˼�������ʱ���Ƿ񲻷��ϸ�ʽ		*/
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
				gui_TextOut_ext(CENTER_ALIGN,13,"����ʱ�䲻�Ϸ�!",0,0);
				gui_TextOut_ext(CENTER_ALIGN,16,"����������",0,0);
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
* @brief �ָ�Ĭ������
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
			gui_TextOut(0,13,"�ָ�Ĭ�����ý������������,�Ƿ����?",0,1);
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
* @brief �鿴�ڴ���Ϣ
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
				sprintf(str,"ʣ��%d�����ֽ�,��ռ��%d�ֽ�",cnt*512,8*1024*1024-(cnt*512));
			}

			gui_TextOut(0,13,str,0,1);
		}
	}
	return RUN_CONTINUE;
}


/**
* @brief �鿴�̼���Ϣ
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
			gui_TextOut(0,13,"�̼��汾:V1.0.0",0,1);
			strcpy(str,"Ӧ�ð汾:");
			strcat(str,app_ver);
			gui_TextOut(0,26,str,0,1);
			gui_TextOut(0,39,"ɨ��ͷ:uE_2.3_1.5.29",0,1);
		}
	}
	return RUN_CONTINUE;
}

/**
* @brief �鿴�̼���Ϣ
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
			gui_TextOut_ext(CENTER_ALIGN,26,"������USB...",0,1);
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
* @brief ��ʼ��Ӧ�û���
*/
static void app_init(void)
{
	dlg_refresh_flag = 0;
	no_update_icon_flg = 0;
	need_refresh_signal = 0;
	menu_current_item = 1;
	menu_start = 1;
	memset(BarCode,0,31);
	//memset((void*)draw_content,0,MAX_CASH_ITEM*sizeof(TDRAW_ITEM));	//��������������

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

//�������ö��豸��״̬���г�ʼ��
void device_init_by_setting(void)
{
	//if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	//{
	//	USB_Cable_Config(0);		//�Ͽ�USB�豸������
	//	YFBT07_power_ctrl(1);		//��������ģ��ĵ�Դ
	//	bluetooth_match_cmd_cnt = 0;
	//	led_g_ctrl(1);
	//}
	//else if (g_param.transfer_mode == TRANSFER_MODE_KEYBOARD)
	//{
	//	USB_Cable_Config(1);
	//	usb_device_init(USB_KEYBOARD);
	//	YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
	//	led_g_ctrl(0);
	//}
	//else if (g_param.transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	//{
	//	USB_Cable_Config(1);
	//	usb_device_init(USB_VIRTUAL_PORT);
	//	YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
	//	led_g_ctrl(0);
	//}
	//else if (g_param.transfer_mode == TRANSFER_MODE_U_DISK)
	//{
	//	YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
	//	led_g_ctrl(0);
	//	g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
	//	usb_device_init(USB_MASSSTORAGE);
	//	USB_Cable_Config(1);
	//}
	//else
	//{
	//	if (g_param.batch_transfer_mode == TRANSFER_MODE_BLUETOOTH)
	//	{
	//		USB_Cable_Config(0);		//�Ͽ�USB�豸������
	//		YFBT07_power_ctrl(1);		//��������ģ��ĵ�Դ
	//		bluetooth_match_cmd_cnt = 0;
	//		led_g_ctrl(1);
	//	}
	//	else if (g_param.batch_transfer_mode == TRANSFER_MODE_KEYBOARD)
	//	{
	//		usb_device_init(USB_KEYBOARD);
	//		YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
	//		led_g_ctrl(0);
	//		USB_Cable_Config(1);
	//	}
	//	else
	//	{
	//		USB_Cable_Config(1);
	//		usb_device_init(USB_VIRTUAL_PORT);
	//		YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
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
		"\xe8\xa0\x9c\xed\x99\xa1\xe9\x9c\xf9\xe0\x90\xa7\xe4\x87\xe8\x93\xe6\xf5\x9c\xad",	//��Ʒ���ƣ����ݲɼ���
		"\xE8\xA0\x9C\xED\x8A\x97\xE0\x9F\xF9\xE0\x0E\x6C",									//��Ʒ�ͺţ�T6
		"\x93\xA0\xE8\xA0\xE9\xFD\x93\x96\xF9\xE0\x93\xB4\x81\x80\x90\x8a\xe7\xf7\xe8\xf2\x9b\xa0\xef\xbd\x8d\x89\x89\x8a\x95\x84\xe3\xf1\x91\xe4",	//�������̣������н������������޹�˾
		"\xEF\xBD\xE1\xEA\xF9\xE0\x6A\x6D\x6F\x6F\x77\x69\x68\x62\x69\x6B\x68\x69\x63",		//�绰��0755-32831239
		"\x92\xb7\xe6\xa4\x8c\xa2\x8d\xad\x92\xf2\xea\xbc\x92\xf2\x91\xa3\x89\x8a",	//�������Ȩ��Ȩ����
		"\x8b\x95\xe7\xa1\xef\x9b\xea\xbc\xf9\xf6\x94\xff\xed\xf2\xeb\x82\xe4\xe5\xf9\xfb",//�Ͻ����棬Υ���ؾ���
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
* @brief ������ص�������task_ui��һ�����������Ľ���ص�����
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
* @brief ���Բ˵�����
*/
static unsigned char dlgproc_test_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}


/**
* @brief �ϻ����Խ���
*/
static unsigned char dlgproc_age_test_menu(unsigned char type,unsigned char *key)
{
	unsigned char test_type;

	if (DLGCB_RUN == type)
	{
		if (KEY_SCAN == *key || KEY_ENTER == *key)
		{
			//menu_current_item = 1 ����ɨ��ͷ�ϻ���������
			//menu_current_item = 2 �������������ϻ���������
			//menu_current_item = 3 ����ɨ��+�����ϻ���������
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
* @brief ������Բ˵�����
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
		temp[i] = ADC_GetConversionValue(ADC1);		//�õ�ADת���Ľ��
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

	//ȡ20��ֵ֮��,ȥ����Сֵ������ܣ���ȡƽ��ֵ���򵥵�ƽ���˲�
	result -= min;
	result -= max;
	result /= 18; 

	return result;

}

/*
*@brief  LCD ���⼰LED���Կ��Ʋ���
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
*@brief  LCD ���⼰LED���Կ��Ʋ���
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
* @brief  ����������
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
*@brief: ��������
*/
static void testKeypad(void)
{
	unsigned char * key;
	unsigned char lastkey = 0xff;
	int i = 0;
	int keyscancnt = 0;

	for (keyscancnt = 0;keyscancnt < KEY_COUNT;keyscancnt++)
	{
		keyscan_pos_width_tbl[keyscancnt].clear_flag = 1;		//������
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
	OSSchedLock();		//��Ҫ�ٽ�����������ˣ���ֹ��ֵ������������
	while (1) 
	{
		key = keypad_getkey();

		if ((key)&&(lastkey != (*key))) 
		{
			lastkey = *key;
			Beep(BEEP_DELAY);
			i = 0;
			while (keyscan_pos_width_tbl[i].key != 0xff)	//�������
			{
				if ((*key == keyscan_pos_width_tbl[i].key)&&(keyscan_pos_width_tbl[i].clear_flag == 1))
				{
					keyscan_pos_width_tbl[i].clear_flag = 0;		//������
					gui_FillRect(keyscan_pos_width_tbl[i].x,keyscan_pos_width_tbl[i].y, 6*keyscan_pos_width_tbl[i].width, 13, BG_COLOR,1);
					keyscancnt ++;		//��ס�Ѿ�ɨ���˶��ٸ���
					break;
				}
				i++;
			}
		}
		if (keyscancnt == KEY_COUNT)		//����ɨ�������˳�//
		{
			break;
		}
	}
	OSSchedUnlock();		//�������
	return;
}


//ɨ��ͷ����
static void testScanerInit(void)
{
	unsigned char dis_str[5];
	//unsigned char tmp[31];
	int           ret = 0;
	int			  command;
	unsigned char scan_success_flag = 0;

	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,0);
	gui_TextOut(0,0,"����(S):",0,0);
	//gui_LineH(0,13,20*6,TXT_COLOR,0);
	gui_TextOut(0,26,"��������:",0,0);
	//gui_LineH(0,39,20*6,TXT_COLOR,0);
	gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);

	scan_success_flag = 0;
	while(1)
	{
		ret = task_read_command_from_ui(&command, 0); 
		if( !ret &&(TASK_CONTINUE == command))
		{
			//�û�����ɨ���
			//��ʼɨ����Ʒ������
			//scan_start = 1;
			memset(BarCode,0,31);
			//memset(tmp,0,31);
			memset(BarCode_Type,0,20);
			BarCode_Len = 0;
			if (UE988_get_barcode(BarCode_Type, BarCode,30, &BarCode_Len) == 0)
				//if (scanner_get_barcode(tmp,30,BarCode_Type,&BarCode_Len)  == 0)
			{
				//��ȡ������
				Delay(5000);
				Beep(BEEP_DELAY);
				barcode_got_flag = 1;
				scan_start = 0;

				//��ʾ����
				//gui_TextOut()
				//��̨ɨ����������� 
				//��ô��Ҫ����̨��ȡ������������ʾ����

				if ((BarCode[0] != 0)&&(BarCode_Len != 0))
				{
					gui_TextOut_ext(CENTER_ALIGN,13,BarCode,0,1);
					gui_TextOut_ext(CENTER_ALIGN,39,BarCode_Type,0,1);
					//hex_to_str(((BarCode_Len > 30)?30:BarCode_Len),10,0,dis_str);
					//gui_TextOut_ext(CENTER_ALIGN,LINE6,dis_str,1);
				}
				//gui_SetTextColor(TXT_COLOR);

				enter_scaner_test_flag  = 2;	//����ɨ����Եı�־���㣬�˳�ɨ�����
				scan_success_flag		= 1;	//������ʾ�Ѿ����Գɹ�

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


//����ģ�����
static void testBluetooth(void)
{
	OSSchedLock();		//��Ҫ�ٽ�����������ˣ���ֹ��ֵ������������
	YFBT07_Test();
	OSSchedUnlock();		//�������

}


//USB����
static void testUSB(void)//USB����
{
	unsigned char i = 0;

#if 1

	usb_device_init(USB_KEYBOARD);	//���豸��ʼ��Ϊһ��
	USB_Cable_Config(1);
	
	OSSchedLock();		//��Ҫ�ٽ�����������ˣ���ֹ��ֵ������������
	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,0);
	gui_TextOut_ext(CENTER_ALIGN,13,"����USB��!",0,0);
	gui_TextOut(40,26,"�ȴ���",0,0);
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
		//OSTimeDlyHMSM(0,0,0,500);//��ͣ0.5 s
		Delay(9000000);
	}

	gui_TextOut(0,26,"                   ",0,1);//������Waiting��
	gui_TextOut_ext(CENTER_ALIGN,26,"USB���ӳɹ�!",0,1);//usb���ӳɹ���
	Beep(BEEP_DELAY);
	OSSchedUnlock();		//��Ҫ�ٽ���������ˣ���ֹ��ֵ������������
	OSTimeDlyHMSM(0,0,1,500);//��ͣ1.5 s

#endif

}


//RTC����
static void testRTC(void)//rtc����
{
	volatile int  n;
	unsigned char sec_save;

	gui_FillRect(0, 0, GUI_WIDTH, GUI_HEIGHT-SHOTCUT_HEIGNT-6, BG_COLOR,1);
INIT_RTC:

	SD2068A_GetTime((unsigned char*)&currentDateTime);
	sec_save = currentDateTime.sec;
	OSTimeDlyHMSM(0,0,1,50);//��ʱ1.05s
	SD2068A_GetTime((unsigned char*)&currentDateTime);
	if ((currentDateTime.sec+60-sec_save)%60 >= 1 )
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"RTC ��������",0,1);
		//����CC1101ģ�����վƥ���Ƶ��
		OSTimeDlyHMSM(0,0,1,500);//��ͣ1.5 s
	}
	else
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"RTC ��������",0,1);
		//����CC1101ģ�����վƥ���Ƶ��
		OSTimeDlyHMSM(0,0,1,500);//��ͣ1.5 s
		goto  INIT_RTC;//
	}

}

//RTC����
static void testAD(void)//rtc����
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
		gui_TextOut_ext(CENTER_ALIGN,26,"AD��������",0,1);
		OSTimeDlyHMSM(0,0,1,500);//��ͣ1.5 s
	}
	else
	{
		gui_TextOut_ext(CENTER_ALIGN,26,"AD��������",0,1);
		Beep(BEEP_DELAY*2);
		OSTimeDlyHMSM(0,0,1,300);//��ͣ1.5 s
	}
}


/**
* @brief �������Եĺ�̨����
* @param[in] none                                 
*/
static int factory_test_task(int param1, int param2, void *param3, void *param4)
{	
	int								command;
	//������Ŀ��ţ��������ѯtestfunc_tbl���������ȡ��Ӧ�Ĳ��Ժ���
	int								testItemNum = 0;
	//int max = sizeof(testfunc_tbl);
	while(1)
	{
		while(1)
		{
			// �ȴ�UIָ��
			if( task_read_command_from_ui(&command, 0) == 0)
			{
				if(command == TASK_RUN)
				{
					break;			// ���Կ�ʼ
				}

				if(command == TASK_EXIT)
				{
					return KEY_EXT_2;			// �û�ȡ��
				}
			}
		}

		testfunc_tbl[testItemNum]();				//������Ӧ�Ĳ��Ժ�����ɸ���Ĳ���
		task_send_command_to_ui(KEY_EXT_1);			//ĳ��������	
		testItemNum ++;								//������Ŀ��ż�1��������һ��Ĳ���

		if (testItemNum == sizeof(testfunc_tbl)/sizeof(int))			//����ȫ����ɣ��˳��˺�̨����
		{
			break;
		}
	}
	return KEY_EXT_1;		//�������
}



/**
* @brief �������ԵĴ���ص����õĹ����ص�����
*/
static unsigned char dlgproc_test_proc(unsigned char *key)
{
	if (*key == KEY_ENTER || *key == KEY_SCAN || *key == KEY_RIGHT_SHOTCUT)
	{
		if (testproc_first_enterkey == 1) 
		{
			testproc_first_enterkey = 0;			//���������
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
				ui_send_command_to_task(TASK_RUN);		//����ʹɨ������˳�
				return RUN_RERUN;
			}
			else
			{
				return RUN_RERUN;
			}
		}

	}

	if (*key == KEY_EXT_1)		//��̨������һ����ԣ��ͻ᷵�������ֵ
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
	pos_state &= ~STA_FACTORY_MODE;		//�������ģʽ��״̬��
	ui_send_command_to_task(TASK_EXIT);
	return RUN_RERUN;
	}

	if (*key == KEY_EXT_2)		//��̨����ȡ��
	{
	*key = KEY_ESC;
	return RUN_NEXT;
	}
	*/ 

	return RUN_CONTINUE;
}

/*
*��ʾ���Խ���
*/
unsigned char dlgproc_test_lcd_display(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
		enter_scaner_test_flag  = 0;//����ɨ����Եı�־��ʼ��
		//��ʼ�ں�̨�����������Գ���
		dlg_run_bgtask(factory_test_task, 0, 0, 0, 0);
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}

	return RUN_CONTINUE;
}



/*
*���԰���
*/
unsigned char dlgproc_test_keypad(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
	}
	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;
}



/*
*����ɨ��ͷ
*/
unsigned char dlgproc_test_scaner_init(unsigned char type,unsigned char *key)
{
	if( DLGCB_DRAW == type)
	{
		scan_start				= 0;    //���Կ�ʼɨ��
		enter_scaner_test_flag  = 1;	//����ɨ����Եı�־
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
	}

	if (DLGCB_RUN  == type)
	{
		return dlgproc_test_proc(key);
	}

	if (DLGCB_DESTORY == type)
	{
		enter_scaner_test_flag  = 0;			//���������
	}

	return RUN_CONTINUE;

}

/*
*����cc1101
*/
unsigned char dlgproc_test_bluetooth(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*����usb
*/
unsigned char dlgproc_test_usb(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*����rtc
*/
unsigned char dlgproc_test_rtc(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
	}

	if (type == DLGCB_RUN) 
	{
		return dlgproc_test_proc(key);
	}
	return RUN_CONTINUE;

}

/*
*����rtc
*/
unsigned char dlgproc_test_AD(unsigned char type,unsigned char *key)
{
	if( type == DLGCB_DRAW )
	{
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������
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
		testproc_first_enterkey = 1;	//���ƴ���ʱ����λ������

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
