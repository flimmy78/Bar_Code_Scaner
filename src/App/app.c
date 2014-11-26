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
static int					searched_rec_index;		//�������ļ�¼����

typedef struct tag_chk_info_context
{
	TCHECK_INFO_RECORD	check_info_node;			//�̵���Ϣ
	TGOODS_SPEC_RECORD	goods_node;					//��Ʒ��Ϣ
	unsigned int		chk_info_index;				//�̵���Ϣ�����嵥�е�λ������
	unsigned int		goods_info_index;			//��Ʒ��Ϣ�����嵥�е�λ������
	unsigned int		chkinf_need_update_flag;	//��ǰ������̵���Ϣ�Ƿ���Ҫ���µı��
	int					add_value;					//��ǰ������̵��������ۼ�ֵ
}CHK_INFO_CONTEXT;

static	CHK_INFO_CONTEXT			op_context;		//����������̵�ʱ�������Ļ���	
static	unsigned int	send_num;


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
extern int						bluetooth_module_state;
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
			gui_TextOut_ext(CENTER_ALIGN,42,"  ɨ��  ",1,0);
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
			gui_TextOut_ext(CENTER_ALIGN,42,"���ݴ���",1,0);
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
			gui_TextOut_ext(CENTER_ALIGN,42,"ϵͳ����",1,0);
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
			gui_TextOut_ext(CENTER_ALIGN,42,"ϵͳ��Ϣ",1,0);
		}
		break;
	default:
		break;
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
}

/**
* @brief ������ص�������task_ui��һ�����������Ľ���ص�����
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

		//�������ⰴ������: "KEY_ESC��LEFT��KEY_ESC��LEFT��KEY_ESC��LEFT��KEY_ESC" �������ع��ܲ˵�
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


//���촫����ַ���
static void build_transfer_str(TCHECK_INFO_RECORD * p_check,unsigned int num,unsigned char *str)
{
	unsigned char  temp[20];
	//���촫����ַ���
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

	strcat(str,"\x0d\x0a");		//���з�
}

//��⵱ǰ�������ӵ�״̬
//0:�Ͽ�״̬
//1:����״̬
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
		//U DISK�洢��ʽʱ�����û��ָ���洢��Ŀ���ļ�����Ϊ����״̬�Ͽ�
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
			return 0;	//���Ŀ���ļ�̫��Ҳ��Ϊ����״̬�ǶϿ���
		}
		return 1;
	}
	else
	{
		//�ѻ��洢��ʽʱ������ѻ��洢�ļ�¼�ﵽ�����ˣ���Ϊ����״̬�Ͽ�
		if (record_module_count(REC_TYPE_BATCH) == CHECK_LIST_MAX_CNT)
		{
			return 0;
		}

		return 1;
	}
}

//�ѻ����洫����ַ���
static int batch_save_barcode_str(unsigned char *str)
{
	TBATCH_NODE  batch_node;

	if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH && g_param.bluetooth_transfer_option == 0)
	{
		//�������������ѻ�����ѡ��ر�ʱ��������
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

//����һ���ַ���
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

		//memcpy(key_value_report,"\x00\x00\x28",3);	//����

		//SendData_To_PC(key_value_report, 3);
		//SendData_To_PC("\x00\x00\x00", 3);	//����
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
		
		f_lseek(&file2,file2.fsize);	//�ļ�ָ���ƶ��ļ�ĩβ
		
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
		//�ѻ�����ķ�ʽ
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


//�����ȡ����������Ϣ
static int transfer_barcode_proc(TCHECK_INFO_RECORD * p_check,unsigned int num)
{
	unsigned char  str[64];
	
	build_transfer_str(p_check,num,str);

	if (check_link_state(g_param.transfer_mode) == 0)
	{
		//���������ǶϿ�״̬
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

#define	ERR_TYPE_DUPLICATION	1		//�����ظ�����	
#define	ERR_TYPE_OVERALL		2		//����洢�Ѵ�����
#define	ERR_TYPE_STOREFAIL		3		//����洢ʧ��

//������ʾ
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
			gui_TextOut(0,39,"�����Ѵ���!",0,0);
			strcpy(line3_save,"�����Ѵ���!");
		}
		break;
	case ERR_TYPE_OVERALL:
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,39,"No More Space",0,0);
		}
		else
		{
			gui_TextOut(0,39,"�洢�Ѵ�����!",0,0);
		}
		break;
	case ERR_TYPE_STOREFAIL:
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,39,"Record Store fail!",0,0);
		}
		else
		{
			gui_TextOut(0,39,"��¼�洢ʧ��!",0,0);
		}
		break;
	}

	gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	return RUN_RERUN;
}


//��ʾ��Ʒ���
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
		return 0;	//û�п�����ʾ����Ʒ���
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
* @brief ɨ�蹦��
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
				//��������˲�ѯ���ݿ�ѡ���ô��Ҫ������ݿ��ļ��Ƿ����
				if (check_database())
				{
					gui_FillRect(0,0,GUI_WIDTH,GUI_HEIGHT,BG_COLOR,1);
					if (g_param.language == LANG_ENGLISH)
					{
						gui_TextOut_ext(CENTER_ALIGN,20,"Databse isn't exist!",0,1);
					}
					else
					{
						gui_TextOut_ext(CENTER_ALIGN,20,"���ݿ��ļ�������!",0,1);
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
				gui_TextOut(0,26,"�밴\"SCAN\"��ɨ��...",0,1);
			}

			scan_func_state = 0;
			memset((void*)&op_context,0,sizeof(CHK_INFO_CONTEXT));
			memset(last_BarCode,0,31);
			dlg_run_bgtask(ScanBarCode_task,0, 0, 0, 0);	//��̨�ȴ���������������
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
					sprintf(dis_str,"ȡ��    ȷ��% 8d",send_num);
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
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ȡ��            ȷ��",0,1);
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
				//��̨û��ɨ�赽�κ�������
				return RUN_RERUN;
			}

			//����Ǻ�̨ɨ�赽�����룬��ô��Ҫ����̨��ȡ������������ʾ����
			
			//��Ҫ���жϣ�����������������Ƿ���ϴε�һ��
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
					//������һ���̵���Ϣ�����ݿ�
					if(record_module_replace(REC_TYPE_CHECK,op_context.chk_info_index,(unsigned char*)&op_context.check_info_node,(TNODE_MODIFY_INFO*)0))
					{
						return err_tip(ERR_TYPE_STOREFAIL);
					}
				}
				p_check_info = (TCHECK_INFO_RECORD*)rec_searchby_tag(REC_TYPE_CHECK,TAG_BARCODE,BarCode,&searched_rec_index);
				if (p_check_info)
				{
					//�̵��嵥���Ѿ��и��������Ϣ
					if (g_param.avoid_dunplication_option)
					{
						return err_tip(ERR_TYPE_DUPLICATION);
					}
					memcpy((void*)&op_context.check_info_node,(void*)p_check_info,sizeof(TCHECK_INFO_RECORD));
					op_context.chk_info_index = searched_rec_index;
				}
				else
				{
					//�̵��嵥�л�û�и��������Ϣ
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
					//��Ҫ��ѯ���ݿ�
					p_goods_node = (TGOODS_SPEC_RECORD*)rec_searchby_tag(REC_TYPE_GOODS_LIST,TAG_BARCODE,BarCode,&searched_rec_index);
					if (p_goods_node)
					{
						//����Ʒ�嵥��������������
						memcpy((void*)&op_context.goods_node,(void*)p_goods_node,sizeof(TGOODS_SPEC_RECORD));
						op_context.goods_info_index = searched_rec_index;

						op_context.check_info_node.spec_rec_index = op_context.goods_info_index;
					}
				}

				op_context.add_value = 1;
			}
			else
			{
				//����һ�ε�������ͬ
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
				//�̵����������˱仯
				op_context.chkinf_need_update_flag = 1;
			}

			//�����Ҫ��ȡ�̵�ʱ�������
			if (g_param.add_time_option || g_param.add_date_option)
			{
				GetDateTime();
				Systime2BCD(op_context.check_info_node.check_time);
			}

			if (g_param.database_query_option)
			{
				//���������ݿ��ѯѡ���Ҫ��ʾ��Ʒ�Ĺ����Ϣ
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
						gui_TextOut(0,39,"**����Ʒ��Ϣ**",0,0);
						strcpy(line3_save,"**����Ʒ��Ϣ**");
					}
				}
			}

			if (g_param.barcode_statistic_option)
			{
				//������ͳ�ƹ���ѡ���Ҫ��ʾ�����������
				
				sprintf(dis_str,"<%d         >",op_context.check_info_node.check_num);
				
				gui_TextOut(0,39,dis_str,0,0);
				strcpy(line3_save,dis_str);
			}

			gui_refresh(0,0,GUI_WIDTH,GUI_HEIGHT);
			send_num = 1;

			if (g_param.transfer_confirm_option || g_param.quantity_enter_mode)
			{
				//�����˴���ȷ��ѡ����߿���������¼��
				if (g_param.quantity_enter_mode)
				{
					scan_func_state = 2;		//�ȴ�������������ȷ�Ϸ���
					if (g_param.language == LANG_ENGLISH)
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc     Enter      1",0,1);
					}
					else
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ȡ��    ȷ��       1",0,1);
					}
				}
				else
				{
					scan_func_state = 1;		//�ȴ�ȷ�Ϸ���
					if (g_param.language == LANG_ENGLISH)
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Esc            Enter",0,1);
					}
					else
					{
						gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ȡ��            ȷ��",0,1);
					}
				}	

				return RUN_RERUN;
			}
			else
			{
				//û�п�������ȷ����û�п���¼��������ѡ��ʱ��ɨ�赽������ֱ����ͼ���ͳ�ȥ
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
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ѡ��                ",0,1);
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
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ѡ��                ",0,1);
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
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ѡ��                ",0,1);
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
					gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ѡ��                ",0,1);
				}
				scan_func_state = 0;
				return RUN_RETURN;
			}
		}
		else
		{
			if ((scan_func_state & 0x10) == 0x10)
			{
				//��ʾ��Ʒ����״̬��ֻ��ӦKEY_UP����KEY_ESC��
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
					//������һ���̵���Ϣ�����ݿ�
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
* @brief ���ݴ��䷽ʽ��ѡ��
*/
static unsigned char dlgproc_data_transfer_func(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		//��ʾ��ǰ���õ����ݴ���ģʽ
		refresh_current_select_item(g_param.transfer_mode,1);

		if (menu_current_item == 1 || menu_current_item == 4 || menu_current_item == 5)
		{
			if (g_param.language == LANG_ENGLISH)
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"Option",0,1);
			}
			else
			{
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ѡ��",0,1);
			}
		}
	}

	if (DLGCB_RUN == type)
	{
		if (KEY_ENTER == *key || KEY_SCAN == *key)
		{
			if (menu_current_item == 6 || menu_current_item == 7)
			{
				//������Ʒ��Ϣ
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
				gui_TextOut(0,GUI_HEIGHT-SHOTCUT_HEIGNT,"ѡ��",0,1);
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


//�����ѻ����ݵ�����
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

		//delete_one_node(REC_TYPE_BATCH,i+1);	//�����ѻ���¼ɾ������ʾ�ѷ���
	}

	return i;
}

/**
* @brief �ѻ����ݴ���
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
				gui_TextOut(0,26,"��ȷ���豸��������״̬!",0,1);
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
					gui_TextOut(0,26,"û���ѻ�����",0,1);
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
					gui_TextOut(0,26,"���ڷ���,���Ժ�...",0,1);
				}

				cnt = batch_data_send_proc();

				if (g_param.language == LANG_ENGLISH)
				{
					sprintf(str,"Send %d barcodes!",cnt);
				}
				else
				{
					sprintf(str,"������%d������!",cnt);
				}
				gui_TextOut(0,39,str,0,1);
			}
			
		}
	}

	return RUN_CONTINUE;
}

/**
* @brief �ѻ����ݴ��䷽ʽ��ѡ��
*/
static unsigned char dlgproc_batch_transfer_mode(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		//��ʾ��ǰ���õ����ݴ���ģʽ
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
* @brief �ѻ��������
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
			gui_TextOut_ext(CENTER_ALIGN,20,"�ѻ����ݽ������",0,1);
			gui_TextOut_ext(CENTER_ALIGN,33,"�Ƿ����?",0,1);
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
					gui_TextOut_ext(CENTER_ALIGN,26,"�������ʧ��!",0,1);
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
					gui_TextOut_ext(CENTER_ALIGN,26,"��������ɹ�!",0,1);
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
* @brief ���ɨ���¼
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
			gui_TextOut_ext(CENTER_ALIGN,20,"ɨ���¼�������",0,1);
			gui_TextOut_ext(CENTER_ALIGN,33,"�Ƿ����?",0,1);
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
					gui_TextOut_ext(CENTER_ALIGN,26,"�������ʧ��!",0,1);
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
					gui_TextOut_ext(CENTER_ALIGN,26,"��������ɹ�!",0,1);
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
* @brief ɨ��ģʽѡ��
*/
static unsigned char dlgproc_scan_mode_menu(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		//��ʾ��ǰ���õ�ɨ��ģʽ
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

/**
* @brief ��������ģʽѡ��
*/
static unsigned char dlgproc_quantity_enter_menu(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.quantity_enter_mode);
}



/**
* @brief ���ʱ��ѡ��
*/
static unsigned char dlgproc_add_time_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.add_time_option);
}

/**
* @brief �������ѡ��
*/
static unsigned char dlgproc_add_date_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.add_date_option);
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
					//����Ʒ�嵥��������������
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
						strcpy(line3_save,"**����Ʒ��Ϣ**");
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
* @brief ���÷ָ���
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
* @brief �޸ķ�������
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
* @brief ȷ���޸ķ�������
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
			gui_TextOut_ext(CENTER_ALIGN,26,"ȷ���޸�����?",0,1);
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
* @brief ���ݿ��ѯѡ��
*/
static unsigned char dlgproc_database_query_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.database_query_option);
}

/**
* @brief �����빦�ܿ���ѡ��
*/
static unsigned char dlgproc_avoid_dunplication_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.avoid_dunplication_option);
}


/**
* @brief ����ͳ�ƹ��ܿ���ѡ��
*/
static unsigned char dlgproc_barcode_statistic_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.barcode_statistic_option);
}

/**
* @brief ����ȷ�Ϲ��ܿ���ѡ��
*/
static unsigned char dlgproc_transfer_confirm_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.transfer_confirm_option);
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
* @brief ��������ģʽѡ��
*/
static unsigned char dlgproc_transfer_bluetooth_option(unsigned char type,unsigned char *key)
{
	return proc_option_select(type,key,&g_param.bluetooth_transfer_option);
}

/**
* @brief U�̴���ģʽѡ��
*/
static unsigned char dlgproc_transfer_u_disk_option(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}

/**
* @brief �ѻ����ݴ���ѡ��
*/
static unsigned char dlgproc_transfer_batch_option(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}


/**
***************************************************************************
*@brief ��ʾ�Ự���
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
			gui_TextOut_ext(CENTER_ALIGN,26,"��Ʒ��Ϣ����",0,0);
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
				gui_TextOut_ext(CENTER_ALIGN,39,"ʧ��",0,0);
			}
			else
			{
				gui_TextOut_ext(CENTER_ALIGN,39,"�ɹ�",0,0);
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
			gui_TextOut_ext(CENTER_ALIGN,26,"USB���ӶϿ���",0,0);
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
			gui_TextOut_ext(CENTER_ALIGN,26,"�������������",0,0);
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
			gui_TextOut_ext(CENTER_ALIGN,26,"������USB...",0,0);
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
			gui_TextOut_ext(CENTER_ALIGN,26,"�ȴ��������ʱ",0,0);
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
			gui_TextOut_ext(CENTER_ALIGN,26,"�����������ݿ�...",0,0);
		}

		gui_refresh(0,0,LCD_WIDTH,LCD_HEIGHT);
	}

	return;
}
/**
***************************************************************************
*@brief ���ݵ��뵼���ĺ�̨����
*@param[in] 
*@return 
*@warning
*@see	
*@note  �˺����޷��ж�Ӧ�ò�����Ҫ��ʵ�ʹ����Ƿ�ִ�гɹ�����Ϊ�������PC
*		��������ģ��Ƿ�ɹ�ֻ��PC�ϵ�Ӧ�ó����֪����������ҪPC�����Ķ�
*		�������з��ش˴εĻỰ�����
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
				//��û�п�ʼ���ݴ��䣬�û�����ȡ������Ҫ�˳�����
				//����Ѿ���ʼ�������������ݣ��������û���ȡ�����˳�����
				if (TASK_EXIT == command)
				{
					//�û�����ȡ����
					return KEY_EXT_1;
				}
			}
		}

		if (bDeviceState == CONFIGURED)
		{
			//�ն�ʵ�ֵ�USB�豸�Ѿ���PCö�ٳɹ�
			if (last_state != bDeviceState)
			{
				reset_command();						//USB�жϿ�ʼ����PC��������������
			}

			////��ʼ��ʱ������Ƿ����ʱ
			if (last_cmd_status != g_pcCommand.status)
			{
				//StartDelay(200*120);		//2���ӵ�����ȴ���ʱ
				delay_cnt = 8000;
			}

			if (g_pcCommand.status)
			{
				//���յ�������������������
				data_trans_start_flag = 1;		//�Ѿ���ʼ���ݴ�����
				no_update_icon_flg = 1;
				ret = data_transfer_proc(g_pcCommand.CmdBuffer,param1,data_trans_proc_callback); 
				no_update_icon_flg = 0;
				update_icon_cnt = 0;
				if (0 == ret)
				{
					data_trans_state = 1;
					reset_command();				//׼��������һ������
					last_cmd_status = 0xff;
					continue;
				}
				else if(1 == ret)
				{
					//���յ��������͵ĶϿ�������һỰ�ɹ�
					return KEY_GPRS_S1;		//�Ự�ɹ�
				}
				else if (2 == ret)
				{
					//���յ��������͵ĶϿ�������ǻỰʧ��
					return KEY_GPRS_S2;			//�Ựʧ��
				}
				else if (0x55aa == ret)
				{
					//��HOST��USB���ӶϿ�
					return KEY_GPRS_S3;		//USB���ӶϿ���
				}
				else   //if (ret)
				{
					//�������������ն�û����Ӧ
					return KEY_GPRS_S4;		//������������
				}	
			}
			else
			{
				delay_cnt--;
				//��û�н��յ�PC���͹���������
				if ((last_cmd_status != g_pcCommand.status)&&(data_trans_start_flag != 1))
				{
					task_send_command_to_ui(KEY_GPRS_S5);	
				}

				//if (DelayIsEnd() == 0)
				if (delay_cnt == 0)
				{
					//�ȴ��������ʱ
					return KEY_GPRS_S6;
				}
			}

			last_cmd_status = g_pcCommand.status;
			last_state = bDeviceState;
		}
		else
		{
			//�ն˻�û�н���������USB�ӿڻ��߻�û��ö�ٳɹ�
			if (last_state != bDeviceState)
			{
				if(data_trans_start_flag)
				{
					return KEY_GPRS_S3;		//���񷵻أ���ʾǰ̨��USB���ӱ��Ͽ��ˡ�
				}
				else
				{
					task_send_command_to_ui(KEY_GPRS_S7);	//֪ͨ����ˢ�½�����ʾ
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
*@brief ���ݵ��뵼�����ܵĻص�����
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
		usb_device_init(USB_MASSSTORAGE);	//���豸��ʼ��Ϊһ��Masstorage�豸
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
			//ˢ����ʾ
			gui_FillRect(0,ICON_HEIGHT+1,GUI_WIDTH,USERZONE_HEIGHT-1,BG_COLOR,0);
			if (LANG_ENGLISH == g_param.language)
			{
				gui_TextOut_ext(CENTER_ALIGN,26,"Waiting command...",0,0);
				//@English
			}
			else
			{
				gui_TextOut_ext(CENTER_ALIGN,26,"�ȴ���������...",0,0);
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
* @brief ����ϵͳ����
*/
static unsigned char dlgproc_set_system_password(unsigned char type,unsigned char *key)
{
	//todo.....

	return RUN_CONTINUE;
}

//ˢ�µ�ǰ��ʾ�Ľ��������صĿ������
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
* @brief ���ý�����
*/
static unsigned char dlgproc_decoder_setting(unsigned char type,unsigned char *key)
{	
	static unsigned short map_save;

	if (DLGCB_DRAW == type)
	{
		//��ʾ��ǰ���õĽ�����ѡ��
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
* @brief ���ý�����Чλ��
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
							gui_TextOut_ext(CENTER_ALIGN,13,"������Ч",0,1);
							gui_TextOut_ext(CENTER_ALIGN,26,"����������!",0,1);
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
* @brief �鿴������Ϣ
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
			gui_TextOut(0,13,"������������:LSBTK001",0,1);
			gui_TextOut(0,26,"����Э��汾:V2.0",0,1);
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
* @brief �������ļ�
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
					gui_TextOut_ext(CENTER_ALIGN,26,"�ļ�����ʧ��!",0,0);
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
					gui_TextOut_ext(CENTER_ALIGN,26,"�ļ������ɹ�!",0,0);
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
			//����Ƿ�����˷Ƿ��ַ�
			strcpy(str,g_editValue[0]);
			if ((strstr(str,"*") != NULL)||(strstr(str,"\\") != NULL)||(strstr(str,"|") != NULL)||(strstr(str,"\"") != NULL)\
                            ||(strstr(str,"/") != NULL)||(strstr(str,"?") != NULL)||(strstr(str,">") != NULL)||(strstr(str,"<") != NULL)\
                             ||(strstr(str,".") != NULL))
			{
				memset(g_editValue[0],0,MAX_EDITNUM);
				gui_FillRect(0,0,GUI_WIDTH,LCD_HEIGHT,BG_COLOR,0);
				Beep(BEEP_DELAY*2);
				gui_TextOut(0,26,"����������Ƿ��ַ�!",0,0);
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

//ɨ���Ŀ¼�µ�TXT�ļ�,���֧��20��TXT�ļ�
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

//ˢ����ʾ�ļ��б����ص�ǰѡ����ļ�����
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
		// �·�ҳ
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
	{	// �Ϸ�ҳ
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
* @brief ��ʾ�ļ��б�
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
* @brief ��ʽ��U DISK
*/
static unsigned char dlgproc_u_disk_format(unsigned char type,unsigned char *key)
{
	if (DLGCB_DRAW == type)
	{
		if (g_param.language == LANG_ENGLISH)
		{
			gui_TextOut(0,13,"All the data will be cleared,Continue��",0,1);
		}
		else
		{
			gui_TextOut(0,13,"�������ݽ���ʧ�����ɻָ�,�Ƿ����?",0,1);
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
				gui_TextOut(0,26,"���ڸ�ʽ��,���Ժ�...",0,1);
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
* @brief �ļ�����ѡ��
*/
static unsigned char dlgproc_file_options_menu(unsigned char type,unsigned char *key)
{
	return proc_list_menu(type,key);
}
/**
* @brief ����ļ�
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
			gui_TextOut(0,26,"ȷ����մ��ļ�?",0,1);
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
					gui_TextOut(0,26,"�ļ����ʧ��!",0,1);
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
* @brief ɾ���ļ�
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
				gui_TextOut(0,26,"����ɾ��Ŀ��洢�ļ�",0,1);
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
			gui_TextOut(0,26,"ȷ��ɾ�����ļ�?",0,1);
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
					gui_TextOut(0,26,"�ļ�ɾ��ʧ��!",0,1);
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
* @brief ��ʼ��Ӧ�û���
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
	//memset((void*)draw_content,0,MAX_CASH_ITEM*sizeof(TDRAW_ITEM));	//��������������

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

//�������ö��豸��״̬���г�ʼ��
void device_init_by_setting(void)
{
	if (g_param.transfer_mode == TRANSFER_MODE_BLUETOOTH)
	{
		USB_Cable_Config(0);		//�Ͽ�USB�豸������
		YFBT07_power_ctrl(1);		//��������ģ��ĵ�Դ
		bluetooth_match_cmd_cnt = 0;
		led_g_ctrl(1);
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_KEYBOARD)
	{
		USB_Cable_Config(1);
		usb_device_init(USB_KEYBOARD);
		YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
		led_g_ctrl(0);
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_VIRTUAL_COMM)
	{
		USB_Cable_Config(1);
		usb_device_init(USB_VIRTUAL_PORT);
		YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
		led_g_ctrl(0);
	}
	else if (g_param.transfer_mode == TRANSFER_MODE_U_DISK)
	{
		YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
		led_g_ctrl(0);
		g_mass_storage_device_type = MASSTORAGE_DEVICE_TYPE_SPI_FLASH;
		usb_device_init(USB_MASSSTORAGE);
		USB_Cable_Config(1);
	}
	else
	{
		if (g_param.batch_transfer_mode == TRANSFER_MODE_BLUETOOTH)
		{
			USB_Cable_Config(0);		//�Ͽ�USB�豸������
			YFBT07_power_ctrl(1);		//��������ģ��ĵ�Դ
			bluetooth_match_cmd_cnt = 0;
			led_g_ctrl(1);
		}
		else if (g_param.batch_transfer_mode == TRANSFER_MODE_KEYBOARD)
		{
			usb_device_init(USB_KEYBOARD);
			YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
			led_g_ctrl(0);
			USB_Cable_Config(1);
		}
		else
		{
			USB_Cable_Config(1);
			usb_device_init(USB_VIRTUAL_PORT);
			YFBT07_power_ctrl(0);	//�ر�����ģ��ĵ�Դ
			led_g_ctrl(0);
		}
	}
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


//��������
static void enter_test_task(unsigned char test_type)
{
	unsigned int  i,success_cnt,fail_cnt;
	unsigned char *key = NULL;
	unsigned char code_value[31];
	unsigned char str[11];
	unsigned char transfer_mode_save;


	//OSSchedLock();	//��Ҫ�ٽ����������
	gui_clear(0);	//����

	if (test_type == 1)
	{
		gui_TextOut(0,0,"����(S):",0,1);
		gui_TextOut(0,26,"�ɹ�����:",0,1);
		gui_TextOut(0,39,"�������:",0,1);
		success_cnt = fail_cnt = 0;
		memset(code_value,0,31);
	}
	else if (test_type == 2)
	{
		//������ģ��ǿ�ƴ�
		transfer_mode_save = g_param.transfer_mode;
		g_param.transfer_mode = TRANSFER_MODE_BLUETOOTH;
		device_init_by_setting();
		gui_TextOut_ext(CENTER_ALIGN,13,"����ģ���ѿ���",0,1);
		gui_TextOut_ext(CENTER_ALIGN,26,"�ȴ�����������...",0,1);
		success_cnt = 2;
		fail_cnt = 0x20;
		i = 1;
	}
	else if (test_type == 3)
	{
		if(check_link_state(g_param.transfer_mode) == 0)
		{
			gui_TextOut_ext(CENTER_ALIGN,13,"ȷ���Ƿ�����������?",0,1);
		}
		else
		{
			gui_TextOut(0,0,"����(S):",0,1);
			gui_TextOut(0,26,"ɨ�����:",0,1);
			gui_TextOut(0,39,"�������:",0,1);
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
				//��ȡ������
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
				//����״̬
				bluetooth_match_cmd_cnt = 0;
				gui_TextOut(0,0,"���ӳɹ�!",0,1);
				if (i == 1)
				{
					gui_TextOut(0,13,"��[scan]�������ݴ���",0,1);
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

				gui_TextOut(0,13,"���ڽ������ݴ���...",0,1);
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
				//����Ǵ���δ����״̬����ô�ͼ��2���ӷ���һ��ƥ������
				if(i == 2)
				{
					gui_TextOut(0,0,"���ӶϿ�!",0,0);
					gui_TextOut(0,13,"���ݴ��䱻�ж�!     ",0,0);
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
				gui_TextOut_ext(CENTER_ALIGN,26,"�����ж�!",0,0);
			}
			else
			{
				memset(BarCode,0,31);
				memset(BarCode_Type,0,20);
				BarCode_Len = 0;
				if (UE988_get_barcode(BarCode_Type, BarCode,30, &BarCode_Len) == 0)
				{
					//��ȡ������
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

		//Delay(400000);	//Լ200ms
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
			if (UE988_get_barcode(BarCode_Type, BarCode,20, &BarCode_Len) == 0)
				//if (scanner_get_barcode(BarCode,30,BarCode_Type,&BarCode_Len)  == 0)
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
