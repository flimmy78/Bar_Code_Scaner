#include "edit.h"
#include "gui.h"
#include<string.h>
#include "keypad.h"
#include "pic.h"
#include "record.h"
#include "basic_fun.h"
#include "calendar.h"
#include "Terminal_Para.h"

unsigned int				input_update_flag;			//是否需要更新输入的标记
unsigned char				last_input_key;				//最近一次输入的按键
unsigned char				edit_state;					//编辑框状态

extern unsigned char		g_editValue[MAX_EDIT][MAX_EDITNUM];//编辑框的全局变量
extern unsigned char		gFontSize;			// 字号、
//extern TZD_RECORD			ZD_rec;					//字典记录
extern	TTerminalPara		g_param;

static unsigned char		edit_buf[MAX_EDITNUM];
static unsigned char		edit_now;
static unsigned char		current_py;
static unsigned char		current_hz;
static unsigned char		same_key_times;				///连续按同一个按键的次数
static unsigned char		last_keyvalue;				///最近一次输入的键值
static unsigned char		py_match_num;				//完全匹配到的拼音序列数
static unsigned char		last_py_match_num;			//最近一次完全匹配到的拼音序列数

static	unsigned char total_name_item;
static  unsigned char zd_name_valid_tbl[8];
static  unsigned char zd_value_valid_tbl[8];
static unsigned char  total_value_item;
static	unsigned char display_item_yet;
static	unsigned char start_display_item;
static	unsigned char current_item,select_ZD;
static	unsigned char save_start_item;
static unsigned char  save_select_ZD;


/**
* @brief 同一个按键上对应的字母之间的切换,这个函数和当前使用的输入法相关
* @param[in] unsigned char key			输入的按键
* @param[in] unsigned char key_times	同一个按键被连续按的次数
* @param[in] unsigned char input_method	当前使用的输入法
* @还有一个全局的输入参数，就是上一次的键值
* @note 只有使用ABC或者abc输入法时才需要调用此函数进行切换
*		此函数的具体实现与键值的定义有很大相关性。如果需要移植需要根据实际的键值定义方式来做相应的修改。
*/
static unsigned char keyvalue_switch(unsigned char key,unsigned char key_times,unsigned char input_method)
{
	unsigned char keyvalue;

	if (KEY_NUM7 == key|| KEY_NUM9 == key) 
	{
		if ((key_times+1)%5 != 0) 
		{
			keyvalue = last_keyvalue+1;
			if (abc_INPUT == input_method)
			{
				if (key_times%5 == 0)
				{
					//数字切回字母，小写输入时数字切回字母的规则不一样
					keyvalue = last_keyvalue+0x80+1;
				}
			}
		}
		else
		{
			//字母切回数字
			if (abc_INPUT == input_method)
			{
				//小写字母输入时
				keyvalue = last_keyvalue - 0x80 - 4;
			}
			else
			{
				//大写字母输入时
				keyvalue = last_keyvalue-4;
			}
		}
	}
	else if (KEY_NUM1 == key)
	{
		if ((key_times+1)%31 != 0) 
		{
			keyvalue = last_keyvalue+1;
		}
		else
		{
			keyvalue = last_keyvalue-30;
		}
	}
	else if (KEY_NUM0 == key) 
	{
		if ((key_times+1)%2 != 0) 
		{
			keyvalue = last_keyvalue+1;
		}
		else
		{
			keyvalue = last_keyvalue-1;;
		}
	}
	else if (KEY_xing == key)
	{
		if ((key_times+1)%2 != 0) 
		{
			keyvalue = last_keyvalue-1;
		}
		else
		{
			keyvalue = last_keyvalue+1;;
		}
	}
	else
	{
		if ((key_times+1)%4 != 0) 
		{
			keyvalue = last_keyvalue+1;
			if (abc_INPUT == input_method)
			{
				if (key_times%4 == 0)
				{
					//数字切回字母，小写输入时数字切回字母的规则不一样
					keyvalue = last_keyvalue+0x80+1;
				}
			}
		}
		else
		{
			//字母切回数字
			if (abc_INPUT == input_method)
			{
				//小写字母输入时
				keyvalue = last_keyvalue - 0x80 - 3;
			}
			else
			{
				//大写字母输入时
				keyvalue = last_keyvalue-3;
			}
		}
	}
	return keyvalue;
}

/**
* @brief 处理字母属性编辑框的输入函数
* @param[in] unsigned char key						输入的键值
* @param[in] unsigned char input_method				当前的输入法
* @param[out] unsigned char	*key_code				返回按键对应的可显示ASCII码
* @return unsigned char	是否需要后移编辑框光标位置	0:不需要   1:需要	
* @note 此函数除了返回了按键对应的可显示ASCII码，还返回了是否需要更新光标位置的信息
*/
unsigned char edit_alpha_proc(unsigned char key,unsigned char input_method,unsigned char *key_code)
{
	unsigned char if_update_cursor = 1;
	unsigned char tmp_keyvalue;
	if (_123_INPUT == input_method)
	{
		//如果是切换为数字输入法，那么只能输入数字
		//if (key == KEY_xing)
		//{
		//	key = KEY_juhao;
		//}
		key_code[0] = *key2ascii(key);
		last_keyvalue = key;
	}
	else if (ABC_INPUT == input_method || abc_INPUT == input_method || key == KEY_NUM1 || key == KEY_NUM0 || key == KEY_xing)
	{
		//如果切换为大写字母的输入
		if ((key == last_input_key)&&(input_update_flag < 3))
		{
			//同一按键对应的字母之间切换
			same_key_times++;
			tmp_keyvalue = keyvalue_switch(key,same_key_times,input_method);
			key_code[0] = *key2ascii(tmp_keyvalue);
			last_keyvalue = tmp_keyvalue;
			if_update_cursor = 0;			//不需要后移光标
		}
		else
		{
			same_key_times = 0;
			if (key == KEY_xing)
			{
				key_code[0] = *key2ascii(key);
				last_keyvalue = key;
			}
			else
			{
				if ((ABC_INPUT == input_method)||(key == KEY_NUM1)||(key == KEY_NUM0))
				{
					key_code[0] = *key2ascii(key+1);
					last_keyvalue = key+1;
				}
				else
				{
					key_code[0] = *key2ascii(key+1+0x80);
					last_keyvalue = key+1+0x80;
				}
			}
		}
	}
	//last_input_key = key;
	key_code[1] = 0;

	return if_update_cursor;
}


/**
* @brief 拼音序列分组
* @由于每次固定匹配16个拼音序列，而且拼音序列的长度并不是固定的，所以显示的时候需要将这些匹配到的拼音序列分组。
* @按照目前的字体大小，最多需要分5组来显示。
* @return 返回拼音被分组的数目
* @note 此函数将匹配到的16个拼音序列根据实际长度分组，分别返回每个分组可以显示的拼音序列个数
*/
#if 0
static unsigned char edit_py_array(unsigned char* py_display_array)
{
	unsigned char i,j,tmp;
	unsigned short dis_len;

	j = tmp = 0;
	dis_len = 0;
        memset(py_display_array,0,5);
	for (i = 0;i<py_match_num;i++)
	{
		dis_len += strlen(cpt9PY_Mb[i]->PY)+1;
		if (dis_len*CHINESE_EDIT_FONT_SIZE/2 > CHINESE_EDIT_ZONE_W)
		{
			py_display_array[j++] = i - tmp;
			tmp = i;
			dis_len = 0;
			i--;
		}
	}

	py_display_array[j] = i - tmp;

	return (j+1);
}


/**
* @brief 显示按键序列匹配到的拼音序列及对应的汉字序列
* @param[in] unsigned char py_offset   当前选择的拼音序列偏移
* @param[in] unsigned char hz_offset   当前选择的汉字偏移
* @note 此函数还会利用到全局的当前编辑框状态以及搜索到的码表
*/
static void edit_chinese_display(unsigned char py_offset,unsigned char hz_offset)
{
	unsigned char	py_display_array[5];		//最多可以被分成5组
	unsigned char	total_py_display_line;
	unsigned char	i,j,offset = 0;
	unsigned char	start_dis_offset = 0;
	unsigned short	dis_len = 0;
	unsigned int	total_hz_num,total_hz_line;
	unsigned char   hz_display_buf[3] ={0,0,0};
	unsigned char	font_size_save;

	total_py_display_line = edit_py_array(py_display_array);		//调用此函数之后将当前匹配到的拼音序列分组，每组可以显示的拼音序列数发到数组中
	

	//根据当前选择的拼音显示相应的拼音序列
	//搜索到当前选择的拼音序列处于哪个分组中
	for (i = 0;i<total_py_display_line;i++)
	{
		offset += py_display_array[i];
		if (py_offset < offset)
		{
			break;
		}
		start_dis_offset += py_display_array[i];
	}

	//i表示当前拼音序列处于哪个显示分组中

	gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
	gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
	font_size_save = gFontSize;
	gui_SetFont(CHINESE_EDIT_FONT_SIZE);

	//清除旧的显示区域,包括上下翻页的图标区域
	gui_FillRect(0,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);

	//显示该分组的所有的拼音序列
	for (j = 0;j<py_display_array[i];j++)
	{
		if (EDIT_STATE_PY_SELECT == edit_state)
		{
			if (j+start_dis_offset == py_offset)
			{
				gui_SetBGColor(TXT_COLOR);
				gui_SetTextColor(BG_COLOR);
			}
		}
		gui_TextOut(CHINESE_EDIT_ZONE_X+dis_len,CHINESE_EDIT_ZONE_Y+4,(unsigned char*)cpt9PY_Mb[start_dis_offset+j]->PY,1);
		dis_len += (strlen(cpt9PY_Mb[start_dis_offset+j]->PY)+1)*CHINESE_EDIT_FONT_SIZE/2;
		if (EDIT_STATE_PY_SELECT == edit_state)
		{
			if (j+start_dis_offset == py_offset)
			{
				gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
				gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
			}
		}
	}	

	//根据当前拼音序列显示上下翻页图标
	if (EDIT_STATE_PY_SELECT == edit_state)
	{
		if (py_offset==0)
		{
			//当前显示的是第一个拼音序列，只需要显示下翻键即可
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4,PIC_DOWN);
		}
		else if (py_offset==(py_match_num-1))
		{
			//显示的是最后一个拼音序列，只需要显示上翻键
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4,PIC_UP);
		}
		else
		{
			//上限翻页键都需要显示
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4,PIC_UP);
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4,PIC_DOWN);
		}
	}

	//显示当前选择的拼音序列对应的汉字
	total_hz_num = strlen((char const*)cpt9PY_Mb[py_offset]->PY_mb)/2;		//一共对应的汉字个数
	total_hz_line = total_hz_num*CHINESE_EDIT_FONT_SIZE;
	total_hz_line += CHINESE_EDIT_ZONE_W - 1;
	total_hz_line /= CHINESE_EDIT_ZONE_W;
	//total_hz_line = ((total_hz_num*CHINESE_EDIT_FONT_SIZE) + (CHINESE_EDIT_ZONE_W - 1))/CHINESE_EDIT_ZONE_W;//一共可以显示几行汉字
	
	offset = 0;
	start_dis_offset = 0;
	for (i = 0;i<total_hz_line;i++)
	{
		offset += 8;
		if (hz_offset < offset)
		{
			break;
		}
		start_dis_offset += 8;
	}
	//i获取到当前汉字位于哪个显示分组

	for (j = 0;j<8;j++)
	{
		if (start_dis_offset+j == total_hz_num)
		{
			break;
		}

		if (EDIT_STATE_HZ_SELECT == edit_state)
		{
			if (j+start_dis_offset == hz_offset)
			{
				gui_SetBGColor(TXT_COLOR);
				gui_SetTextColor(BG_COLOR);
			}
		}

		memcpy(hz_display_buf,cpt9PY_Mb[py_offset]->PY_mb+2*(start_dis_offset+j),2);
		gui_TextOut(CHINESE_EDIT_ZONE_X+j*CHINESE_EDIT_FONT_SIZE,CHINESE_EDIT_ZONE_Y+32,hz_display_buf,1);

		if (EDIT_STATE_HZ_SELECT == edit_state)
		{
			if (j+start_dis_offset == hz_offset)
			{
				gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
				gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
			}
		}
	}

	//根据当前汉字显示分组，决定上下翻页键的显示方式
	if ((total_hz_line > 1)&&(EDIT_STATE_HZ_SELECT == edit_state))
	{
		//总共不止一行的汉字需要显示
		if (i==0)
		{
			//当前显示的是第一个汉字分组，只需要显示下翻键即可
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
		}
		else if (i==(total_hz_line-1))
		{
			//最后一个显示分组，只需要显示上翻键
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
		}
		else
		{
			//上限翻页键都需要显示
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
		}
	}


	if (EDIT_STATE_HZ_SELECT == edit_state)
	{
		//处于汉字选择状态时，在汉字下方显示一排数字，可以利用数字键快速选字
		gui_SetFont(12);
		hz_display_buf[1] = 0;
		for (j=0;j<8;j++)
		{
			if (start_dis_offset+j == total_hz_num)
			{
				break;
			}

			hz_display_buf[0] = '1'+j;
			gui_TextOut(CHINESE_EDIT_ZONE_X+8+24*j,CHINESE_EDIT_ZONE_Y+CHINESE_EDIT_ZONE_H-16,hz_display_buf,1);
		}
		gui_SetFont(DEFAULT_FONT_SIZE);
	}

	gui_SetBGColor(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_SetFont(font_size_save);
}


/**
* @brief 在中文输入时处理输入编辑框的键值
* @param[in] unsigned char key						输入的键值
* @param[in] unsigned char input_method				当前的输入法
* @param[out] unsigned char *code			在中文输入法下，如果完成汉字的输入，那么返回汉字的GB2312编码；在其它输入法下，返回键值对应的ASCII码,注意需要返回字符串
* @return unsigned char  是否需要后移编辑框光标位置	0:不需要   1:需要	
* @note 此函数执行之后会影响全局的编辑框状态，编辑框状态如下所示
*		 EDIT_STATE_INIT_STATUS		处于非中文输入法状态
*        EDIT_STATE_PY_SELECT		处于拼音选择阶段
*        EDIT_STATE_HZ_SELECT		处于汉字选择阶段
*        EDIT_STATE_CODE_GOT		获取到输入的键值的编码
*		这三个状态只能通过KEY_ENTER键来切换，
*		EDIT的初始状态就是非中文输入法状态，输入任意数字键切换为拼音选择状态，通过T9匹配到可能的拼音序列以及相应的汉字显示出来后，可以通过上下键选择拼音序列，
*		按KEY_ENTER将拼音选择状态切换到汉字选择状态，在汉字选择状态下可以通过上下翻键选择相应的汉字；
*		在汉字选择状态下按KEY_ENTER，就切换到获取到准确的汉字的状态，并将获取到的汉字编码返回。
*       只有此函数的返回值为EDIT_STATE_CODE_GOT时，code返回的编码才是对应汉字的GB2312编码。
*/
unsigned char edit_chinese_proc(unsigned char key,unsigned char input_method,unsigned char *code)
{
	unsigned char	if_update_cursor = 1;

	//下面这个判断条件很纠结，不知道我下次能不能看懂！！！
	//其实是为了处理在拼音输入法状态下，按数字键1输入标点符号的问题。目前的做法是拼音输入法时按数字键1同样输入半角的符号，所以不能让按键1进入下面的流程
	//但是在汉字选择状态下，又需要按数字键1可以用来选择相应汉字。所以搞这么一个纠结的判断条件。。。。
	//其实要修改按键1对应输入全角符号的话，需要修改T9输入法中数字键1对应的码表即可。但是也需要特殊处理，因为1键没有对应的拼音序列可以显示出来
	if (((PINYIN_INPUT == input_method)&&(key != KEY_NUM1)&&(key != KEY_NUM0)&&(key != KEY_xing))||((key == KEY_NUM1) &&(edit_state == EDIT_STATE_HZ_SELECT)))
	{
		if (edit_state == EDIT_STATE_INIT_STATUS)
		{
			//在还没有处于中文输入法状态时按键进来，就进入了拼音选择状态了
			edit_state = EDIT_STATE_PY_SELECT;	
			edit_now = 0;
			current_py = 0;

			//绘制一个专门的区域用来显示匹配到的拼音序列及对应汉字序列
			gui_LineH(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,COLOR_BLACK);
			gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);
			//gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4,PIC_UP);
			//gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4,PIC_DOWN);
		}

		if (edit_state == EDIT_STATE_PY_SELECT)
		{
			//拼音选择状态下，允许输入键值
			if (KEY_ENTER == key)
			{
				edit_state = EDIT_STATE_HZ_SELECT;		//编辑框状态切换为汉字选择状态
				current_hz = 0;
			}
			else if (KEY_UP == key || KEY_DOWN == key)
			{
				if (KEY_DOWN == key)
				{
					if (current_py < (py_match_num - 1))
					{
						current_py++;
					}
				}
				else
				{
					if (current_py > 0)
					{
						current_py--;
					}
				}

				strcpy(edit_buf,cpt9PY_Mb[current_py]->t9PY_T9);
				edit_now = strlen(cpt9PY_Mb[current_py]->t9PY_T9);
			}
			else
			{
				if (KEY_CLEAR == key)
				{
					edit_now--;
					if (edit_now == 0)
					{
						current_hz = 0;
						edit_state = EDIT_STATE_CODE_GOT;
                        return if_update_cursor;
					}
				}
				else
				{
					//数字键
					if(edit_now < 6)
					{
						//最多输入6个字母，不支持组词输入
						edit_buf[edit_now++] = *key2ascii(key);
					}
				}
				edit_buf[edit_now] = 0;

				py_match_num = T9PY_Get_Match_PY_MB(edit_buf,cpt9PY_Mb);		//此函数返回与输入序列完全匹配的拼音序列个数
				if (0 == py_match_num)
				{
					//如果输入的字母序列没有一个完全匹配的拼音序列，那么刚输入的字母是不合法的
					edit_now--;
					py_match_num = last_py_match_num;
				}

				last_py_match_num = py_match_num;
			}
		}
		else if (edit_state == EDIT_STATE_HZ_SELECT)
		{
			//处于汉字选择状态时，可以通过数字键来快速选择对应的汉字
			if (KEY_UP == key || KEY_DOWN == key)
			{
				if (KEY_DOWN == key)
				{
					current_hz += 8;
					if (current_hz >= strlen((char const*)cpt9PY_Mb[current_py]->PY_mb)/2)
					{
						current_hz -= 8;
					}
				}
				else
				{
					//current_hz = strlen((char const*)cpt9PY_Mb[current_py]->PY_mb)/2;
					if (current_hz >= 8)
					{
						current_hz -= 8;
					}
				}
			}
			else if (KEY_CLEAR == key)
			{
				edit_state = EDIT_STATE_PY_SELECT;		//编辑框状态返回到拼音选择状态
				current_hz = 0;
			}
			else if (KEY_ENTER == key)
			{
				//获取到汉字编码
				edit_state = EDIT_STATE_CODE_GOT;
				memcpy(code,cpt9PY_Mb[current_py]->PY_mb+2*current_hz,2);
				code[2] = 0;
				current_hz = 0;
				current_py = 0;
				return if_update_cursor;
			}
			else
			{
				//数字键
				if(KEY_NUM1 == key || KEY_NUM2 == key || KEY_NUM3 == key || 
					KEY_NUM4 == key || KEY_NUM5 == key || KEY_NUM6 == key || 
					KEY_NUM7 == key || KEY_NUM8 == key)
				{
					current_hz += (*key2ascii(key) - 0x31);
					edit_state = EDIT_STATE_CODE_GOT;
					memcpy(code,cpt9PY_Mb[current_py]->PY_mb+2*current_hz,2);
					code[2] = 0;
					current_hz = 0;
					current_py = 0;
					return if_update_cursor;
				}
			}
		}

		//显示匹配到的拼音序列
		edit_chinese_display(current_py,current_hz);
	}
	else
	{
		//处于非中文输入法状态时，KEY_UP、KEY_DOWN、KEY_CLEAR、KEY_ENTER是进不来此函数的
		 if_update_cursor = edit_alpha_proc(key,input_method,code);
		 edit_state = EDIT_STATE_CODE_GOT;
	}

	return if_update_cursor;
}

#if 0
/**
* @brief 显示系统设置的字典名称列表
* @param[in] unsigned char start_display_item		开始显示的项
* @param[in] unsigned char total_display_item		总共需要显示的项
* @param[in] unsigned char current_item				当前项
* @return unsigned char 	已经显示的项数
* @note 自适应显示内容的宽度
*       字典列表需要固定增加两个常用字段：当前日期、当前时间
*/
static unsigned char display_ZD_name_list(unsigned char start_display_item,unsigned char total_display_item,unsigned char current_item)
{
	unsigned char i;
	unsigned char *pBuf;
	unsigned short x,y;
	unsigned char	font_size_save;


	//if (total_display_item == 0)
	//{
	//	return 0;
	//}

	if (start_display_item > (total_display_item+2))
	{
		return 0;
	}

	if ((current_item < start_display_item)||(current_item >= (total_display_item+2)))
	{
		return 0;
	}

	x = CHINESE_EDIT_ZONE_X;
	y = CHINESE_EDIT_ZONE_Y;

	font_size_save = gFontSize;
	gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
	gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
	gui_SetFont(ZD_EDIT_FONT_SIZE);

	//清除旧的显示区域,包括上下翻页的图标区域
	gui_FillRect(0,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);


	for (i = start_display_item;i < (total_display_item+2);i++)
	{
		if (i < total_display_item)
		{
			pBuf = record_module_read(REC_TYPE_ZD,zd_name_valid_tbl[i]);
			pBuf = ((TZD_RECORD*)pBuf)->ZD_name;
		}
		else if (i == total_display_item)
		{
			pBuf = "当前日期";
		}
		else
		{
			pBuf = "当前时间";
		}


		if ((x+(strlen(pBuf)+2)*ZD_EDIT_FONT_SIZE/2) > (CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W+ZD_EDIT_FONT_SIZE))
		{
			if ((y + ZD_EDIT_FONT_SIZE + 4 + ZD_EDIT_FONT_SIZE) <= (CHINESE_EDIT_ZONE_Y + CHINESE_EDIT_ZONE_H + 4))
			{
				x = CHINESE_EDIT_ZONE_X;
				y += (ZD_EDIT_FONT_SIZE + 4);
			}
			else
			{
				break;
			}
		}
                x += ((strlen(pBuf)+2)*ZD_EDIT_FONT_SIZE/2);
	}

	if (i <= current_item)
	{
		start_display_item += (current_item+1-i);
	}


    x = CHINESE_EDIT_ZONE_X;
	y = CHINESE_EDIT_ZONE_Y;
	for (i = start_display_item;i < (total_display_item+2);i++)
	{
		if (i < total_display_item)
		{
			pBuf = record_module_read(REC_TYPE_ZD,zd_name_valid_tbl[i]);
			pBuf = ((TZD_RECORD*)pBuf)->ZD_name;
		}
		else if (i == total_display_item)
		{
			pBuf = "当前日期";
		}
		else
		{
			pBuf = "当前时间";
		}
		

		if ((x+(strlen(pBuf)+2)*ZD_EDIT_FONT_SIZE/2) > (CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W+ZD_EDIT_FONT_SIZE))
		{
			if ((y + ZD_EDIT_FONT_SIZE + 4 + ZD_EDIT_FONT_SIZE) <= (CHINESE_EDIT_ZONE_Y + CHINESE_EDIT_ZONE_H + 4))
			{
				x = CHINESE_EDIT_ZONE_X;
				y += (ZD_EDIT_FONT_SIZE + 4);
			}
			else
			{
				break;
			}
		}

		if (i == current_item)
		{
			gui_SetBGColor(TXT_COLOR);
			gui_SetTextColor(BG_COLOR);
		}

		gui_TextOut(x,y,pBuf,1);
		x += ((strlen(pBuf)+2)*ZD_EDIT_FONT_SIZE/2);

		if (i == current_item)
		{
			gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
			gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
		}
	}

	if ((current_item+1) < (total_display_item+2))
	{
		//显示下翻页
		gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
	}

	if (current_item > 0)
	{
		//显示上翻页
		gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
	}

	gui_SetBGColor(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_SetFont(font_size_save);
	return (i - start_display_item);
}


/**
* @brief 显示某一字典名对应的字典内容列表
* @param[in] unsigned char index					对应的字典名
* @param[in] unsigned char start_display_item		开始显示的项
* @param[in] unsigned char current_item				当前项
* @return unsigned char 	已经显示的项数
*/
static unsigned char display_ZD_value_list(unsigned char index,unsigned char start_display_item,unsigned char current_item,unsigned char *total_item)
{
	unsigned char i;
	unsigned char *pBuf;
	unsigned short x,y;
	unsigned char	font_size_save;

	pBuf = record_module_read(REC_TYPE_ZD,index);

	*total_item = 0;
	memset(zd_value_valid_tbl,0,8);
	for (i = 0;i < 8;i++)
	{
		if (((TZD_RECORD*)pBuf)->ZD_value[i][0] != 0)
		{
			zd_value_valid_tbl[(*total_item)++] = i;
		}
	}

	if (*total_item == 0)
	{
		return 0;
	}
	if (start_display_item > *total_item)
	{
		return 0;
	}

	if ((current_item < start_display_item)||(current_item >= *total_item))
	{
		return 0;
	}

	memcpy((void*)&ZD_rec,(void*)pBuf,sizeof(TZD_RECORD));

	//清除旧的显示区域,包括上下翻页的图标区域
	gui_FillRect(0,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);

	x = CHINESE_EDIT_ZONE_X;
	y = CHINESE_EDIT_ZONE_Y;

	font_size_save = gFontSize;
	gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
	gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
	gui_SetFont(ZD_EDIT_FONT_SIZE);

	for (i = start_display_item;i < *total_item;i++)
	{
		if ((x+(strlen(ZD_rec.ZD_value[zd_value_valid_tbl[i]])+2)*ZD_EDIT_FONT_SIZE/2) > (CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W+ZD_EDIT_FONT_SIZE))
		{
			if ((y + ZD_EDIT_FONT_SIZE + 4 + ZD_EDIT_FONT_SIZE) <= (CHINESE_EDIT_ZONE_Y + CHINESE_EDIT_ZONE_H))
			{
				x = CHINESE_EDIT_ZONE_X;
				y += (ZD_EDIT_FONT_SIZE + 4);
			}
			else
			{
				break;
			}
		}

		x += ((strlen(ZD_rec.ZD_value[zd_value_valid_tbl[i]])+2)*ZD_EDIT_FONT_SIZE/2);
	}

	if (i <= current_item)
	{
		start_display_item += (current_item+1-i);
	}


	x = CHINESE_EDIT_ZONE_X;
	y = CHINESE_EDIT_ZONE_Y;
	for (i = start_display_item;i < *total_item;i++)
	{
		if ((x+(strlen(ZD_rec.ZD_value[zd_value_valid_tbl[i]])+2)*ZD_EDIT_FONT_SIZE/2) > (CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W+ZD_EDIT_FONT_SIZE))
		{
			if ((y + ZD_EDIT_FONT_SIZE + 4 + ZD_EDIT_FONT_SIZE) <= (CHINESE_EDIT_ZONE_Y + CHINESE_EDIT_ZONE_H))
			{
				x = CHINESE_EDIT_ZONE_X;
				y += (ZD_EDIT_FONT_SIZE + 4);
			}
			else
			{
				break;
			}
		}

		if (i == current_item)
		{
			gui_SetBGColor(TXT_COLOR);
			gui_SetTextColor(BG_COLOR);
		}

		gui_TextOut(x,y,ZD_rec.ZD_value[zd_value_valid_tbl[i]],1);
		x += ((strlen(ZD_rec.ZD_value[zd_value_valid_tbl[i]])+2)*ZD_EDIT_FONT_SIZE/2);

		if (i == current_item)
		{
			gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
			gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
		}
	}


	if ((current_item+1) < *total_item)
	{
		//显示下翻页
		gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
	}

	if (current_item > 0)
	{
		//显示上翻页
		gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
	}


	gui_SetBGColor(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_SetFont(font_size_save);

	return (i - start_display_item);
}
#endif
/**
* @brief 在字典输入时处理输入编辑框的键值
* @param[in] unsigned char key						输入的键值
* @param[out] unsigned char *code				
* @return
* @note 此函数执行之后会影响全局的编辑框状态，编辑框状态如下所示
*		 EDIT_STATE_INIT_STATUS		处于非中文输入法状态
*        EDIT_STATE_ZD_NAME_SELECT		处于拼音选择阶段
*        EDIT_STATE_ZD_VALUE_SELECT		处于汉字选择阶段
*        EDIT_STATE_CODE_GOT		获取到输入的键值的编码
*		这三个状态通过KEY_ENTER键或者KEY_CLEAR键来切换，
*		EDIT的初始状态就是非字典输入法状态，输入F3键切换为字典输入状态，查询系统设置的字典名称，将字典名称显示出来后，可以通过上下键选择字典名称，
*		按KEY_ENTER将字典名称选择状态切换到字典值的选择状态，在字典值选择状态下可以通过上下翻键选择相应的值；
*		在字典值选择状态下按KEY_ENTER，就进入获取到输入的键值的编码，并将获取到的字典值的编码返回。
*/
void edit_ZD_proc(unsigned char key,unsigned char *code)
{
#if 0
	unsigned char datetime_bcd[7];
	unsigned char datetime_str[11];
	unsigned int	i,j;
	unsigned char	*pBuf;
	switch (key)
	{
	case KEY_FUN3:
		//显示系统设置的字典列表
		total_name_item = 0;
		j = record_module_count(REC_TYPE_ZD);
		memset(zd_name_valid_tbl,0,8);
		for (i = 0;i < j;i++)
		{
			pBuf = record_module_read(REC_TYPE_ZD,i);
			pBuf = ((TZD_RECORD*)pBuf)->ZD_name;
			if (*pBuf != 0)
			{
				zd_name_valid_tbl[total_name_item++] = i;
			}
		}
		start_display_item = 0;
		current_item = 0;
		gui_LineH(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,COLOR_BLACK);
		gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);

		display_item_yet = display_ZD_name_list(start_display_item,total_name_item,current_item);
		break;
	case KEY_ENTER:
		if (EDIT_STATE_ZD_VALUE_SELECT == edit_state)
		{
			edit_state = EDIT_STATE_CODE_GOT;

			//将该字典值返回
			strcpy(code,ZD_rec.ZD_value[zd_value_valid_tbl[current_item]]);

			gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H,BG_COLOR);
		}
		else if (EDIT_STATE_ZD_NAME_SELECT == edit_state)
		{
			if (current_item < total_name_item)
			{
				edit_state = EDIT_STATE_ZD_VALUE_SELECT;
				select_ZD = current_item;

				save_start_item = start_display_item;
				save_select_ZD = select_ZD;

				start_display_item = 0;
				current_item = 0;
				//显示该字典的内容
				display_item_yet = display_ZD_value_list(zd_name_valid_tbl[select_ZD],start_display_item,current_item,&total_value_item);
				if (total_value_item == 0)
				{
					edit_state = EDIT_STATE_CODE_GOT;
					//将该字典值返回
					code[0] = 0;
					gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H,BG_COLOR);
				}
			}
			else
			{
				//选择了最后两个固定的字典字段“当前日期”和“当前时间”，只将将系统时间返回到编辑框
				edit_state = EDIT_STATE_CODE_GOT;
				GetDateTime();
				Systime2BCD(datetime_bcd);
				//将该字典值返回
				if (current_item == total_name_item)
				{
					//选择的是“当前日期”
					bcd_field_to_str(datetime_bcd,4,datetime_str);
					memcpy(code,datetime_str,4);
					code[4] = '-';
					memcpy(code+5,datetime_str+4,2);
					code[7] = '-';
					strcpy(code+8,datetime_str+6);
				}
				else
				{
					//选择的是“当前时间”
					bcd_field_to_str(datetime_bcd+4,3,datetime_str);
					memcpy(code,datetime_str,2);
					code[2] = ':';
					memcpy(code+3,datetime_str+2,2);
					code[5] = ':';
					strcpy(code+6,datetime_str+4);
				}
				
				gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H,BG_COLOR);
			}

		}
		break;
	case KEY_UP:
		if (current_item > 0)
		{
			current_item --;
			if (current_item < start_display_item)
			{
				start_display_item --;
			}
			if (EDIT_STATE_ZD_VALUE_SELECT == edit_state)
			{
				display_item_yet = display_ZD_value_list(zd_name_valid_tbl[select_ZD],start_display_item,current_item,&total_value_item);
			}
			
			if (EDIT_STATE_ZD_NAME_SELECT == edit_state)
			{
				display_item_yet = display_ZD_name_list(start_display_item,total_name_item,current_item);
			}
			
		}
		break;
	case KEY_DOWN:
			if (EDIT_STATE_ZD_VALUE_SELECT == edit_state)
			{
				if ((current_item+1) < total_value_item)
				{
					current_item++;
					if (current_item >= display_item_yet)
					{
						start_display_item++;				
					}
					display_item_yet = display_ZD_value_list(zd_name_valid_tbl[select_ZD],start_display_item,current_item,&total_value_item);
				}
			}

			if (EDIT_STATE_ZD_NAME_SELECT == edit_state)
			{
				if ((current_item+1) < (total_name_item+2))
				{
					current_item++;
					if (current_item >= display_item_yet)
					{
						start_display_item++;				
					}
					display_item_yet = display_ZD_name_list(start_display_item,total_name_item,current_item);
				}
			}
            
		
		break;
	case KEY_CLEAR:
		if (EDIT_STATE_ZD_NAME_SELECT == edit_state)
		{
			edit_state = EDIT_STATE_INIT_STATUS;
			code[0] = 0;

			gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H,BG_COLOR);
		}
		else if (EDIT_STATE_ZD_VALUE_SELECT == edit_state)
		{
			edit_state = EDIT_STATE_ZD_NAME_SELECT;

			//显示系统设置的字典列表
			start_display_item = save_start_item;
			current_item = save_select_ZD;
			display_item_yet = display_ZD_name_list(start_display_item,total_name_item,current_item);
		}
		break;
	default:
		break;
	}
#endif
}
#endif