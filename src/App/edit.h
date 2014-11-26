#ifndef _EDIT_H_
#define _EDIT_H_



//定义每个窗体可以支持的编辑框个数
#define MAX_EDIT				2

//定义每个编辑框输入的最大长度
#define MAX_EDITNUM				31


//定义中文输入处理函数的状态
#define	EDIT_STATE_INIT_STATUS		0		//编辑框还没有开始进入中文输入的状态
#define EDIT_STATE_PY_SELECT		1		//编辑框已经进入了中文输入状态的拼音选择状态
#define EDIT_STATE_HZ_SELECT		2		//编辑框已经进入了中文输入状态的汉字选择状态
#define EDIT_STATE_CODE_GOT			3		//编辑框已经获取到输入键值的编码

#define EDIT_STATE_ZD_NAME_SELECT	4		//编辑框进入选择字典名的状态
#define EDIT_STATE_ZD_VALUE_SELECT	5		//编辑框进入选择字典值的状态


//输入法类型定义
#define PINYIN_INPUT	0x01
#define ABC_INPUT		0x02
#define abc_INPUT		0x03
#define _123_INPUT		0x04

//定义中文输入时绘制的显示区域的相关信息
#define CHINESE_EDIT_ZONE_X						24//X Pos
//#define CHINESE_EDIT_ZONE_Y						220//Y Pos
#define CHINESE_EDIT_ZONE_Y						222//Y Pos
#define CHINESE_EDIT_ZONE_W						24*8//宽度
//#define CHINESE_EDIT_ZONE_H						72//高度
#define CHINESE_EDIT_ZONE_H						70//高度

#define CHINESE_EDIT_TXT_COLOR					COLOR_BLUE		//文字颜色
#define CHINESE_EDIT_BG_COLOR					COLOR_GRAY		//背景颜色

#define CHINESE_EDIT_FONT_SIZE					24//字体大小

#define ZD_EDIT_FONT_SIZE						16


unsigned char edit_alpha_proc(unsigned char key,unsigned char input_method,unsigned char *key_code);
unsigned char edit_chinese_proc(unsigned char key,unsigned char input_method, unsigned char *code);
void edit_ZD_proc(unsigned char key,unsigned char *code);

#endif