#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "edit.h"
#include "gui.h"

/**
 * @brief �Ի���ص��ӿ�
 * type:�ص������ͣ�������"����ǰ�����ƺ�����ʱ������ǰ"�ĸ�ʱ����
 * key:�û�����İ���ֵ
 * pDlg:��ǰ�Ի���ľ��
 * return:���з�����ƣ�������"��������,��������, ��һ���Ի���, ��һ���Ի���"
 *        ��������:��ǰ��ֵ�Ѵ���ֱ�ӷ��ص�ѭ��ʼ��������ȡ��ֵ
 *        ��������:��ǰ��ֵ�Ѵ����δ������Ҫ�Ӹõ������������
 *        ��һ���Ի���:�����öԻ���������һ���Ի���(Enter)
 *        ��һ���Ի���:�����öԻ���������һ���Ի���(Esc)
 */
typedef unsigned char (*DialogCallback)(unsigned char type, unsigned char *key);

#define DLGCB_CREATE					0x00
#define DLGCB_DRAW						0x01
#define DLGCB_REFRESH					0x02
#define DLGCB_RUN						0x03
#define DLGCB_DESTORY					0x04
#define DLGCB_PAGECHANGE				0x05
#define DLGCB_BACK_FROM_VIRTUAL_KEYPAD	0x06	//����������˳�ʱִ�еĻص�

#define RUN_RERUN						0x00
#define RUN_CONTINUE					0x01
#define RUN_NEXT						0x02
#define RUN_PREV						0x03
#define RUN_RETURN						0x04
#define RUN_EDIT_PROC					0x05	//��ת�����������״̬


/** @note EDITͳһ�й�꣬������ʾΪ�Ǻ� */
typedef struct tagEditRes
{
	unsigned short		x;
	unsigned short		y;
	unsigned char		style;		// IP��ַ�����롢���֡����
	unsigned char		display_max_num;	// �����ʾ�ַ�����һ�������ַ�ռ���ֽ�
	unsigned char		input_max_num;	// ���ݔ���ַ�����һ�������ַ�ռ���ֽ�
	unsigned int		def;		// Ĭ��ֵ���˴�Ϊϵͳ������ID��,0��ʾû��
	struct tagEditRes	*next;		// ��һ���༭����Դ
}TEditRes;



typedef struct tagEditAttr
{
	unsigned short		x;
	unsigned short		y;
	unsigned char		style;		// IP��ַ�����롢���֡����
}TEditAttribute;


// ����style��Ķ���
#define EDIT_MUL_LINE	0x80	// ���б༭��		//ע��ĳһ���������ʹ���˶��б༭����ô�Ͳ�֧�ֶ�༭��
#define EDIT_UNDERLINE	0x40	// ���»���
#define EDIT_READONLY	0x20	// ֻ��
#define EDIT_NO_BLINK	0x10	// ����Ҫ��˸���
#define EDIT_IP			0x01	// IP��ַ��ʽ 192.168.001.234
#define EDIT_PSW		0x02	// �����ʽ�� ******
#define EDIT_NUM		0x03	// ���ָ�ʽ�� 123456
#define EDIT_MONEY		0x04	// ����ʽ�� 12.34
#define EDIT_CHINESE	0x05	// ������������
#define	EDIT_ALPHA		0x06	// ����������ĸ
#define EDIT_PSW_S		0x07	// �����ʽ����������볤�Ⱥ�Res�����õĳ���һ��
#define EDIT_ZD_INPUT	0x08	// ����ͨ���ֵ�ķ�ʽ����


#define MAX_CASH_ITEM	7



typedef struct tagDialogRes
{
	unsigned char							id;				// �Լ���id
	unsigned char							style;			// �Ի����ʽ��
	int								delay;			// ��ʱʱ��(����)
	TEditRes						*pEdit;			// 
	unsigned char					**pContent_en;	// �Ի�����������
	unsigned char					**pContent_scn;	// �Ի�����������
	unsigned char					**pContent_tcn;	// �Ի�����������			
}TDialogRes;

typedef struct tagdisplayInfo
{
	unsigned short	bg_color;
	unsigned short	txt_color;
	unsigned char	font_size;
} TDisplayInfo;

typedef struct tagEdit
{
	TEditRes			*pRes;					// ָ��ǰ�༭�����Դָ��
	unsigned char		current_edit;			// ��ǰ�ı༭��
	unsigned char		active[MAX_EDIT];		// ��ʶ��ǰ�༭���Ƿ��ڼ���״̬,0:ֻ��,1:���Ա༭
	unsigned char		display_max[MAX_EDIT];	// ������ʾ�ַ���(����С����)
	unsigned char		input_max[MAX_EDIT];	// ���������ַ���(����С����)��Ĭ�ϵ���������ʾ�ַ���
	unsigned char		now[MAX_EDIT];			// ��ǰ���ָ����ַ�λ
	unsigned char		display_patch[MAX_EDIT];// ����ĳһ������޷���ʾһ������ʱ��Ҫ������ʾ�Ĳ���ֵ
	unsigned char		blink;					// �����˸״̬
	TDisplayInfo		view[MAX_EDIT];			// ÿ���༭����ʾ�����������
	//unsigned char		**value;				// ��editָ����ַ���
}TEdit;


typedef struct tagRollDisplayItem
{
	unsigned char	x;				//��ʼ��ʾ������
	unsigned char	y;
	unsigned char	display_len;	//������ʾ����ĳ���
	unsigned char	offset;			//��ʼ��ʾ����ʼƫ��
	unsigned char	*display_data;	//������ʾ������
}TRollDisplayItem;



/** ��ֵ��Ӧ�ĶԻ���ID */
typedef struct tagJumpList
{
	unsigned char		key;
	unsigned char		dlgid;
}TJumpList;

typedef struct tagDialog
{
	unsigned char		active;
	TDialogRes			*pRes;
	TEdit				edit;
	TJumpList			*jmplst;
	DialogCallback		callback;
	char				*name;
	int					delay;
}TDialog;

typedef struct tagActionList
{
	unsigned int		id;			// �Ի����id�ţ�����id������ת����һ��
	TDialogRes			*dlgres;	// ����ǰҪ���ƵĽ���
	DialogCallback		callback;	// ����ʱ�Ļص�����
	TJumpList			*jmplst;	// ����ʱҪ��
	char				*name;		// ��������
}TActionList;

typedef int (*TbgTask)(int param1, int param2, void *param3, void *param4);


typedef struct
{
	unsigned char	active;			//����������ı�־	0:Inactive   1:Active
	unsigned char	style;			//�����������ͣ���ͬ�Ľ������ı�����̬��һ����
	unsigned short	pos_x;			//������������x
	unsigned short	pos_y;			//������������y
	unsigned short	max_value;		//��ʾ���ȵ����ֵ
	unsigned short	current_value;	//�������ĵ�ǰֵ
}TProcessBar;


typedef struct
{
	unsigned char x;
	unsigned char y;
	unsigned char content[22];
}TDRAW_ITEM;


#define PROCESS_BAR_STYLE_1			1
#define PROCESS_BAR_STYLE_2			2

#define DLG_INIT			0x00
#define DLG_DRAW			0x01
#define DLG_PROC			0x02
#define DLG_PAGECHANGE		0x03
#define DLG_EDIT_PROC1		0x04
#define DLG_EDIT_PROC2		0x05
#define DLG_RETURN			0xFF

#define TASK_RUN			0x00
#define TASK_EXIT			0x01
#define TASK_CONTINUE		0x02

//������̵�����
#define VIRTUAL_KEYPAD_TYPE_NUM		0x01
#define VIRTUAL_KEYPAD_TYPE_ALPHA		0x02


#define STYLE_NONE					0			// ȫͼƬ���棬�ޱ�������LOGO��
#define STYLE_ICON					0x01		// ����ͼ����
#define STYLE_TITLE					0x02		// ����������
#define STYLE_SHORTCUT				0x04		// ����µĿ�ݼ�
#define STYLE_LIST					0x08		// �ͻ���Ϊ�б���ʽ��������Ϊ��ͨ�ı���ʽ
#define STYLE_AUTO					0x10		// �Զ����жԻ���
#define STYLE_TIP					0x20		// ��ʾ����
#define STYLE_CHILDWINDOW			0x40		// �Ӵ��ڣ������ڲ��ᱻˢ��


//�����Ӵ���ĵ�Ĭ��λ�á���С������������ɫ��ǰ��ɫ����ť����ɫ��ǰ��ɫ����ʾ���򱳾�ɫ��ǰ��ɫ �������С�Ȳ���
#define	DEFAULT_CHILDWINDOW_X			12
#define	DEFAULT_CHILDWINDOW_Y			16
#define	DEFAULT_CHILDWINDOW_W			96
#define	DEFAULT_CHILDWINDOW_H			42
#define DEFAULT_CHILDWINDOW_TITLE_BG	TXT_COLOR
#define DEFAULT_CHILDWINDOW_TITLE_TXT	BG_COLOR
#define DEFAULT_CHILDWINDOW_BUTTON_BG	TXT_COLOR
#define DEFAULT_CHILDWINDOW_BUTTON_TXT	BG_COLOR
#define DEFAULT_CHILDWINDOW_DISP_BG		BG_COLOR
#define DEFAULT_CHILDWINDOW_DISP_TXT	TXT_COLOR
#define DEFAULT_CHILDWINDOW_FONT_SIZE	12

// ǰ̨�Ի��򲿷�
int dlg_init(void);
void dlg_updatetime(unsigned short xpos,unsigned short ypos);
int dlg_update_icon(int mode,unsigned char refresh_flag);
void edit_getvalue(TEdit *pEdit, unsigned char *pValue);
int edit_atoi(unsigned char *pValue);
float edit_atof(unsigned char *pValue);
int edit_atoi_100X(unsigned char *pValue);
unsigned char edit_itoa_100X(unsigned int value, unsigned char *pValue);
unsigned char edit_itoa(unsigned int value, unsigned char *pValue);
void edit_setvalue(TEdit *pEdit, unsigned char *pValue,unsigned char edit_index);
void edit_etos(unsigned char *indata, unsigned char *outdata, int len);
void edit_stoe(unsigned char *indata, unsigned int inlen,unsigned char *outdata);
int edit_tostring(unsigned char *pValue);
void show_menu(unsigned char highlight_current_item_flag);
void show_input_method_icon(unsigned short x,unsigned short y,TEdit *pEdit,unsigned char mode);
void edit_refresh(TEdit *pEdit,unsigned char mode,unsigned char edit_index);
unsigned char edit_refresh_ext(TEdit *pEdit);
void edit_set_Font(TEdit *pEdit,unsigned char size,unsigned char edit_index);
void edit_set_BgColor(TEdit *pEdit,unsigned short color,unsigned char edit_index);
void edit_set_TextColor(TEdit *pEdit,unsigned short color,unsigned char edit_index);
void edit_switch(TEdit *pEdit,unsigned char key);
void edit_switch_ext(TEdit *pEdit,unsigned char edit_index);
unsigned char i2key(unsigned char i);
void edit_init(TEdit *pEdit);
// ��̨���񲿷�
void dlg_run_task(void);
int dlg_run_bgtask(TbgTask callback, int param1, int param2, void *param3, void *param4);
int dlg_read_bgtask_status(void);
int ui_send_command_to_task(int command);
int task_read_command_from_ui(int *command,int waitend);
int task_send_command_to_ui(int command);
void dlg_loop(void);
void process_bar_regist(unsigned char style,unsigned short x,unsigned short y,unsigned short max);
void process_bar_cancle(void);
void process_bar_update(void);
void edit_enable(TEdit *pEdit,int index);
void edit_disable(TEdit *pEdit,int index);

int disp_len(unsigned char *str);
void refresh_draw_content(void);
void saveDrawContext(void);
void releaseDrawContext(void);
void InitDrawContext(void);
extern void RCC_Configuration(void);
#endif
