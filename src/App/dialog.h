#ifndef _DIALOG_H_
#define _DIALOG_H_

#include "edit.h"
#include "gui.h"

/**
 * @brief 对话框回调接口
 * type:回调的类型，可以是"创建前，绘制后，运行时，结束前"四个时机．
 * key:用户输入的按键值
 * pDlg:当前对话框的句柄
 * return:运行方向控制，可以是"重新运行,继续运行, 下一个对话框, 上一个对话框"
 *        重新运行:当前键值已处理，直接返回到循环始处，重新取键值
 *        继续运行:当前键值已处理或未处理，需要从该点继续往下运行
 *        下一个对话框:结束该对话框，运行下一个对话框(Enter)
 *        上一个对话框:结束该对话框，运行上一个对话框(Esc)
 */
typedef unsigned char (*DialogCallback)(unsigned char type, unsigned char *key);

#define DLGCB_CREATE					0x00
#define DLGCB_DRAW						0x01
#define DLGCB_REFRESH					0x02
#define DLGCB_RUN						0x03
#define DLGCB_DESTORY					0x04
#define DLGCB_PAGECHANGE				0x05
#define DLGCB_BACK_FROM_VIRTUAL_KEYPAD	0x06	//从虚拟键盘退出时执行的回调

#define RUN_RERUN						0x00
#define RUN_CONTINUE					0x01
#define RUN_NEXT						0x02
#define RUN_PREV						0x03
#define RUN_RETURN						0x04
#define RUN_EDIT_PROC					0x05	//跳转到处理输入的状态


/** @note EDIT统一有光标，密码显示为星号 */
typedef struct tagEditRes
{
	unsigned short		x;
	unsigned short		y;
	unsigned char		style;		// IP地址、密码、数字、金额
	unsigned char		display_max_num;	// 最大显示字符数，一个中文字符占两字节
	unsigned char		input_max_num;	// 最大輸入字符数，一个中文字符占两字节
	unsigned int		def;		// 默认值，此处为系统参数的ID号,0表示没有
	struct tagEditRes	*next;		// 下一个编辑框资源
}TEditRes;



typedef struct tagEditAttr
{
	unsigned short		x;
	unsigned short		y;
	unsigned char		style;		// IP地址、密码、数字、金额
}TEditAttribute;


// 其中style项的定义
#define EDIT_MUL_LINE	0x80	// 多行编辑框		//注意某一个界面如果使用了多行编辑框，那么就不支持多编辑框
#define EDIT_UNDERLINE	0x40	// 带下划线
#define EDIT_READONLY	0x20	// 只读
#define EDIT_NO_BLINK	0x10	// 不需要闪烁光标
#define EDIT_IP			0x01	// IP地址格式 192.168.001.234
#define EDIT_PSW		0x02	// 密码格式， ******
#define EDIT_NUM		0x03	// 数字格式， 123456
#define EDIT_MONEY		0x04	// 金额格式， 12.34
#define EDIT_CHINESE	0x05	// 可以输入中文
#define	EDIT_ALPHA		0x06	// 可以输入字母
#define EDIT_PSW_S		0x07	// 密码格式，输入的密码长度和Res中设置的长度一致
#define EDIT_ZD_INPUT	0x08	// 可以通过字典的方式输入


#define MAX_CASH_ITEM	7



typedef struct tagDialogRes
{
	unsigned char							id;				// 自己的id
	unsigned char							style;			// 对话框的式样
	int								delay;			// 超时时间(毫秒)
	TEditRes						*pEdit;			// 
	unsigned char					**pContent_en;	// 对话框文字内容
	unsigned char					**pContent_scn;	// 对话框文字内容
	unsigned char					**pContent_tcn;	// 对话框文字内容			
}TDialogRes;

typedef struct tagdisplayInfo
{
	unsigned short	bg_color;
	unsigned short	txt_color;
	unsigned char	font_size;
} TDisplayInfo;

typedef struct tagEdit
{
	TEditRes			*pRes;					// 指向当前编辑框的资源指针
	unsigned char		current_edit;			// 当前的编辑框
	unsigned char		active[MAX_EDIT];		// 标识当前编辑框是否处于激活状态,0:只读,1:可以编辑
	unsigned char		display_max[MAX_EDIT];	// 最大可显示字符数(包括小数点)
	unsigned char		input_max[MAX_EDIT];	// 最大可输入字符数(包括小数点)，默认等于最大可显示字符数
	unsigned char		now[MAX_EDIT];			// 当前光标指向的字符位
	unsigned char		display_patch[MAX_EDIT];// 碰到某一行最后无法显示一个汉字时需要换行显示的补齐值
	unsigned char		blink;					// 光标闪烁状态
	TDisplayInfo		view[MAX_EDIT];			// 每个编辑框显示的字体的属性
	//unsigned char		**value;				// 该edit指向的字符串
}TEdit;


typedef struct tagRollDisplayItem
{
	unsigned char	x;				//开始显示的坐标
	unsigned char	y;
	unsigned char	display_len;	//滚动显示区域的长度
	unsigned char	offset;			//开始显示的起始偏移
	unsigned char	*display_data;	//滚动显示的数据
}TRollDisplayItem;



/** 键值对应的对话框ID */
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
	unsigned int		id;			// 对话框的id号，根据id号来跳转到下一个
	TDialogRes			*dlgres;	// 运行前要绘制的介面
	DialogCallback		callback;	// 运行时的回调函数
	TJumpList			*jmplst;	// 返回时要调
	char				*name;		// 窗体名字
}TActionList;

typedef int (*TbgTask)(int param1, int param2, void *param3, void *param4);


typedef struct
{
	unsigned char	active;			//进度条激活的标志	0:Inactive   1:Active
	unsigned char	style;			//进度条的类型（不同的进度条的表现形态不一样）
	unsigned short	pos_x;			//进度条的坐标x
	unsigned short	pos_y;			//进度条的坐标y
	unsigned short	max_value;		//表示进度的最大值
	unsigned short	current_value;	//进度条的当前值
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

//虚拟键盘的类型
#define VIRTUAL_KEYPAD_TYPE_NUM		0x01
#define VIRTUAL_KEYPAD_TYPE_ALPHA		0x02


#define STYLE_NONE					0			// 全图片界面，无标题栏无LOGO栏
#define STYLE_ICON					0x01		// 包含图标栏
#define STYLE_TITLE					0x02		// 包含标题栏
#define STYLE_SHORTCUT				0x04		// 最底下的快捷键
#define STYLE_LIST					0x08		// 客户户为列表形式，否则则为普通文本形式
#define STYLE_AUTO					0x10		// 自动运行对话框
#define STYLE_TIP					0x20		// 提示窗体
#define STYLE_CHILDWINDOW			0x40		// 子窗口，父窗口不会被刷新


//定义子窗体的的默认位置、大小、标题栏背景色，前景色、按钮背景色，前景色、显示区域背景色、前景色 、字体大小等参数
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

// 前台对话框部分
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
// 后台任务部分
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
