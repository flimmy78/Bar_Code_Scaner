#include "edit.h"
#include "gui.h"
#include<string.h>
#include "keypad.h"
#include "pic.h"
#include "record.h"
#include "basic_fun.h"
#include "calendar.h"
#include "Terminal_Para.h"

unsigned int				input_update_flag;			//�Ƿ���Ҫ��������ı��
unsigned char				last_input_key;				//���һ������İ���
unsigned char				edit_state;					//�༭��״̬

extern unsigned char		g_editValue[MAX_EDIT][MAX_EDITNUM];//�༭���ȫ�ֱ���
extern unsigned char		gFontSize;			// �ֺš�
//extern TZD_RECORD			ZD_rec;					//�ֵ��¼
extern	TTerminalPara		g_param;

static unsigned char		edit_buf[MAX_EDITNUM];
static unsigned char		edit_now;
static unsigned char		current_py;
static unsigned char		current_hz;
static unsigned char		same_key_times;				///������ͬһ�������Ĵ���
static unsigned char		last_keyvalue;				///���һ������ļ�ֵ
static unsigned char		py_match_num;				//��ȫƥ�䵽��ƴ��������
static unsigned char		last_py_match_num;			//���һ����ȫƥ�䵽��ƴ��������

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
* @brief ͬһ�������϶�Ӧ����ĸ֮����л�,��������͵�ǰʹ�õ����뷨���
* @param[in] unsigned char key			����İ���
* @param[in] unsigned char key_times	ͬһ���������������Ĵ���
* @param[in] unsigned char input_method	��ǰʹ�õ����뷨
* @����һ��ȫ�ֵ����������������һ�εļ�ֵ
* @note ֻ��ʹ��ABC����abc���뷨ʱ����Ҫ���ô˺��������л�
*		�˺����ľ���ʵ�����ֵ�Ķ����кܴ�����ԡ������Ҫ��ֲ��Ҫ����ʵ�ʵļ�ֵ���巽ʽ������Ӧ���޸ġ�
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
					//�����л���ĸ��Сд����ʱ�����л���ĸ�Ĺ���һ��
					keyvalue = last_keyvalue+0x80+1;
				}
			}
		}
		else
		{
			//��ĸ�л�����
			if (abc_INPUT == input_method)
			{
				//Сд��ĸ����ʱ
				keyvalue = last_keyvalue - 0x80 - 4;
			}
			else
			{
				//��д��ĸ����ʱ
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
					//�����л���ĸ��Сд����ʱ�����л���ĸ�Ĺ���һ��
					keyvalue = last_keyvalue+0x80+1;
				}
			}
		}
		else
		{
			//��ĸ�л�����
			if (abc_INPUT == input_method)
			{
				//Сд��ĸ����ʱ
				keyvalue = last_keyvalue - 0x80 - 3;
			}
			else
			{
				//��д��ĸ����ʱ
				keyvalue = last_keyvalue-3;
			}
		}
	}
	return keyvalue;
}

/**
* @brief ������ĸ���Ա༭������뺯��
* @param[in] unsigned char key						����ļ�ֵ
* @param[in] unsigned char input_method				��ǰ�����뷨
* @param[out] unsigned char	*key_code				���ذ�����Ӧ�Ŀ���ʾASCII��
* @return unsigned char	�Ƿ���Ҫ���Ʊ༭����λ��	0:����Ҫ   1:��Ҫ	
* @note �˺������˷����˰�����Ӧ�Ŀ���ʾASCII�룬���������Ƿ���Ҫ���¹��λ�õ���Ϣ
*/
unsigned char edit_alpha_proc(unsigned char key,unsigned char input_method,unsigned char *key_code)
{
	unsigned char if_update_cursor = 1;
	unsigned char tmp_keyvalue;
	if (_123_INPUT == input_method)
	{
		//������л�Ϊ�������뷨����ôֻ����������
		//if (key == KEY_xing)
		//{
		//	key = KEY_juhao;
		//}
		key_code[0] = *key2ascii(key);
		last_keyvalue = key;
	}
	else if (ABC_INPUT == input_method || abc_INPUT == input_method || key == KEY_NUM1 || key == KEY_NUM0 || key == KEY_xing)
	{
		//����л�Ϊ��д��ĸ������
		if ((key == last_input_key)&&(input_update_flag < 3))
		{
			//ͬһ������Ӧ����ĸ֮���л�
			same_key_times++;
			tmp_keyvalue = keyvalue_switch(key,same_key_times,input_method);
			key_code[0] = *key2ascii(tmp_keyvalue);
			last_keyvalue = tmp_keyvalue;
			if_update_cursor = 0;			//����Ҫ���ƹ��
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
* @brief ƴ�����з���
* @����ÿ�ι̶�ƥ��16��ƴ�����У�����ƴ�����еĳ��Ȳ����ǹ̶��ģ�������ʾ��ʱ����Ҫ����Щƥ�䵽��ƴ�����з��顣
* @����Ŀǰ�������С�������Ҫ��5������ʾ��
* @return ����ƴ�����������Ŀ
* @note �˺�����ƥ�䵽��16��ƴ�����и���ʵ�ʳ��ȷ��飬�ֱ𷵻�ÿ�����������ʾ��ƴ�����и���
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
* @brief ��ʾ��������ƥ�䵽��ƴ�����м���Ӧ�ĺ�������
* @param[in] unsigned char py_offset   ��ǰѡ���ƴ������ƫ��
* @param[in] unsigned char hz_offset   ��ǰѡ��ĺ���ƫ��
* @note �˺����������õ�ȫ�ֵĵ�ǰ�༭��״̬�Լ������������
*/
static void edit_chinese_display(unsigned char py_offset,unsigned char hz_offset)
{
	unsigned char	py_display_array[5];		//�����Ա��ֳ�5��
	unsigned char	total_py_display_line;
	unsigned char	i,j,offset = 0;
	unsigned char	start_dis_offset = 0;
	unsigned short	dis_len = 0;
	unsigned int	total_hz_num,total_hz_line;
	unsigned char   hz_display_buf[3] ={0,0,0};
	unsigned char	font_size_save;

	total_py_display_line = edit_py_array(py_display_array);		//���ô˺���֮�󽫵�ǰƥ�䵽��ƴ�����з��飬ÿ�������ʾ��ƴ������������������
	

	//���ݵ�ǰѡ���ƴ����ʾ��Ӧ��ƴ������
	//��������ǰѡ���ƴ�����д����ĸ�������
	for (i = 0;i<total_py_display_line;i++)
	{
		offset += py_display_array[i];
		if (py_offset < offset)
		{
			break;
		}
		start_dis_offset += py_display_array[i];
	}

	//i��ʾ��ǰƴ�����д����ĸ���ʾ������

	gui_SetBGColor(CHINESE_EDIT_BG_COLOR);
	gui_SetTextColor(CHINESE_EDIT_TXT_COLOR);
	font_size_save = gFontSize;
	gui_SetFont(CHINESE_EDIT_FONT_SIZE);

	//����ɵ���ʾ����,�������·�ҳ��ͼ������
	gui_FillRect(0,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X+CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_H,BG_COLOR);
	gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);

	//��ʾ�÷�������е�ƴ������
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

	//���ݵ�ǰƴ��������ʾ���·�ҳͼ��
	if (EDIT_STATE_PY_SELECT == edit_state)
	{
		if (py_offset==0)
		{
			//��ǰ��ʾ���ǵ�һ��ƴ�����У�ֻ��Ҫ��ʾ�·�������
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4,PIC_DOWN);
		}
		else if (py_offset==(py_match_num-1))
		{
			//��ʾ�������һ��ƴ�����У�ֻ��Ҫ��ʾ�Ϸ���
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4,PIC_UP);
		}
		else
		{
			//���޷�ҳ������Ҫ��ʾ
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4,PIC_UP);
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4,PIC_DOWN);
		}
	}

	//��ʾ��ǰѡ���ƴ�����ж�Ӧ�ĺ���
	total_hz_num = strlen((char const*)cpt9PY_Mb[py_offset]->PY_mb)/2;		//һ����Ӧ�ĺ��ָ���
	total_hz_line = total_hz_num*CHINESE_EDIT_FONT_SIZE;
	total_hz_line += CHINESE_EDIT_ZONE_W - 1;
	total_hz_line /= CHINESE_EDIT_ZONE_W;
	//total_hz_line = ((total_hz_num*CHINESE_EDIT_FONT_SIZE) + (CHINESE_EDIT_ZONE_W - 1))/CHINESE_EDIT_ZONE_W;//һ��������ʾ���к���
	
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
	//i��ȡ����ǰ����λ���ĸ���ʾ����

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

	//���ݵ�ǰ������ʾ���飬�������·�ҳ������ʾ��ʽ
	if ((total_hz_line > 1)&&(EDIT_STATE_HZ_SELECT == edit_state))
	{
		//�ܹ���ֹһ�еĺ�����Ҫ��ʾ
		if (i==0)
		{
			//��ǰ��ʾ���ǵ�һ�����ַ��飬ֻ��Ҫ��ʾ�·�������
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
		}
		else if (i==(total_hz_line-1))
		{
			//���һ����ʾ���飬ֻ��Ҫ��ʾ�Ϸ���
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
		}
		else
		{
			//���޷�ҳ������Ҫ��ʾ
			gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
			gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
		}
	}


	if (EDIT_STATE_HZ_SELECT == edit_state)
	{
		//���ں���ѡ��״̬ʱ���ں����·���ʾһ�����֣������������ּ�����ѡ��
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
* @brief ����������ʱ��������༭��ļ�ֵ
* @param[in] unsigned char key						����ļ�ֵ
* @param[in] unsigned char input_method				��ǰ�����뷨
* @param[out] unsigned char *code			���������뷨�£������ɺ��ֵ����룬��ô���غ��ֵ�GB2312���룻���������뷨�£����ؼ�ֵ��Ӧ��ASCII��,ע����Ҫ�����ַ���
* @return unsigned char  �Ƿ���Ҫ���Ʊ༭����λ��	0:����Ҫ   1:��Ҫ	
* @note �˺���ִ��֮���Ӱ��ȫ�ֵı༭��״̬���༭��״̬������ʾ
*		 EDIT_STATE_INIT_STATUS		���ڷ��������뷨״̬
*        EDIT_STATE_PY_SELECT		����ƴ��ѡ��׶�
*        EDIT_STATE_HZ_SELECT		���ں���ѡ��׶�
*        EDIT_STATE_CODE_GOT		��ȡ������ļ�ֵ�ı���
*		������״ֻ̬��ͨ��KEY_ENTER�����л���
*		EDIT�ĳ�ʼ״̬���Ƿ��������뷨״̬�������������ּ��л�Ϊƴ��ѡ��״̬��ͨ��T9ƥ�䵽���ܵ�ƴ�������Լ���Ӧ�ĺ�����ʾ�����󣬿���ͨ�����¼�ѡ��ƴ�����У�
*		��KEY_ENTER��ƴ��ѡ��״̬�л�������ѡ��״̬���ں���ѡ��״̬�¿���ͨ�����·���ѡ����Ӧ�ĺ��֣�
*		�ں���ѡ��״̬�°�KEY_ENTER�����л�����ȡ��׼ȷ�ĺ��ֵ�״̬��������ȡ���ĺ��ֱ��뷵�ء�
*       ֻ�д˺����ķ���ֵΪEDIT_STATE_CODE_GOTʱ��code���صı�����Ƕ�Ӧ���ֵ�GB2312���롣
*/
unsigned char edit_chinese_proc(unsigned char key,unsigned char input_method,unsigned char *code)
{
	unsigned char	if_update_cursor = 1;

	//��������ж������ܾ��ᣬ��֪�����´��ܲ��ܿ���������
	//��ʵ��Ϊ�˴�����ƴ�����뷨״̬�£������ּ�1��������ŵ����⡣Ŀǰ��������ƴ�����뷨ʱ�����ּ�1ͬ�������ǵķ��ţ����Բ����ð���1�������������
	//�����ں���ѡ��״̬�£�����Ҫ�����ּ�1��������ѡ����Ӧ���֡����Ը���ôһ��������ж�������������
	//��ʵҪ�޸İ���1��Ӧ����ȫ�Ƿ��ŵĻ�����Ҫ�޸�T9���뷨�����ּ�1��Ӧ������ɡ�����Ҳ��Ҫ���⴦����Ϊ1��û�ж�Ӧ��ƴ�����п�����ʾ����
	if (((PINYIN_INPUT == input_method)&&(key != KEY_NUM1)&&(key != KEY_NUM0)&&(key != KEY_xing))||((key == KEY_NUM1) &&(edit_state == EDIT_STATE_HZ_SELECT)))
	{
		if (edit_state == EDIT_STATE_INIT_STATUS)
		{
			//�ڻ�û�д����������뷨״̬ʱ�����������ͽ�����ƴ��ѡ��״̬��
			edit_state = EDIT_STATE_PY_SELECT;	
			edit_now = 0;
			current_py = 0;

			//����һ��ר�ŵ�����������ʾƥ�䵽��ƴ�����м���Ӧ��������
			gui_LineH(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,COLOR_BLACK);
			gui_FillRect(CHINESE_EDIT_ZONE_X,CHINESE_EDIT_ZONE_Y,CHINESE_EDIT_ZONE_W,CHINESE_EDIT_ZONE_H,CHINESE_EDIT_BG_COLOR);
			//gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4,PIC_UP);
			//gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4,PIC_DOWN);
		}

		if (edit_state == EDIT_STATE_PY_SELECT)
		{
			//ƴ��ѡ��״̬�£����������ֵ
			if (KEY_ENTER == key)
			{
				edit_state = EDIT_STATE_HZ_SELECT;		//�༭��״̬�л�Ϊ����ѡ��״̬
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
					//���ּ�
					if(edit_now < 6)
					{
						//�������6����ĸ����֧���������
						edit_buf[edit_now++] = *key2ascii(key);
					}
				}
				edit_buf[edit_now] = 0;

				py_match_num = T9PY_Get_Match_PY_MB(edit_buf,cpt9PY_Mb);		//�˺�������������������ȫƥ���ƴ�����и���
				if (0 == py_match_num)
				{
					//����������ĸ����û��һ����ȫƥ���ƴ�����У���ô���������ĸ�ǲ��Ϸ���
					edit_now--;
					py_match_num = last_py_match_num;
				}

				last_py_match_num = py_match_num;
			}
		}
		else if (edit_state == EDIT_STATE_HZ_SELECT)
		{
			//���ں���ѡ��״̬ʱ������ͨ�����ּ�������ѡ���Ӧ�ĺ���
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
				edit_state = EDIT_STATE_PY_SELECT;		//�༭��״̬���ص�ƴ��ѡ��״̬
				current_hz = 0;
			}
			else if (KEY_ENTER == key)
			{
				//��ȡ�����ֱ���
				edit_state = EDIT_STATE_CODE_GOT;
				memcpy(code,cpt9PY_Mb[current_py]->PY_mb+2*current_hz,2);
				code[2] = 0;
				current_hz = 0;
				current_py = 0;
				return if_update_cursor;
			}
			else
			{
				//���ּ�
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

		//��ʾƥ�䵽��ƴ������
		edit_chinese_display(current_py,current_hz);
	}
	else
	{
		//���ڷ��������뷨״̬ʱ��KEY_UP��KEY_DOWN��KEY_CLEAR��KEY_ENTER�ǽ������˺�����
		 if_update_cursor = edit_alpha_proc(key,input_method,code);
		 edit_state = EDIT_STATE_CODE_GOT;
	}

	return if_update_cursor;
}

#if 0
/**
* @brief ��ʾϵͳ���õ��ֵ������б�
* @param[in] unsigned char start_display_item		��ʼ��ʾ����
* @param[in] unsigned char total_display_item		�ܹ���Ҫ��ʾ����
* @param[in] unsigned char current_item				��ǰ��
* @return unsigned char 	�Ѿ���ʾ������
* @note ����Ӧ��ʾ���ݵĿ��
*       �ֵ��б���Ҫ�̶��������������ֶΣ���ǰ���ڡ���ǰʱ��
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

	//����ɵ���ʾ����,�������·�ҳ��ͼ������
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
			pBuf = "��ǰ����";
		}
		else
		{
			pBuf = "��ǰʱ��";
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
			pBuf = "��ǰ����";
		}
		else
		{
			pBuf = "��ǰʱ��";
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
		//��ʾ�·�ҳ
		gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
	}

	if (current_item > 0)
	{
		//��ʾ�Ϸ�ҳ
		gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
	}

	gui_SetBGColor(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_SetFont(font_size_save);
	return (i - start_display_item);
}


/**
* @brief ��ʾĳһ�ֵ�����Ӧ���ֵ������б�
* @param[in] unsigned char index					��Ӧ���ֵ���
* @param[in] unsigned char start_display_item		��ʼ��ʾ����
* @param[in] unsigned char current_item				��ǰ��
* @return unsigned char 	�Ѿ���ʾ������
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

	//����ɵ���ʾ����,�������·�ҳ��ͼ������
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
		//��ʾ�·�ҳ
		gui_PictureOut(GUI_WIDTH-24,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_DOWN);
	}

	if (current_item > 0)
	{
		//��ʾ�Ϸ�ҳ
		gui_PictureOut(0,CHINESE_EDIT_ZONE_Y+4+DEFAULT_FONT_SIZE,PIC_UP);
	}


	gui_SetBGColor(BG_COLOR);
	gui_SetTextColor(TXT_COLOR);
	gui_SetFont(font_size_save);

	return (i - start_display_item);
}
#endif
/**
* @brief ���ֵ�����ʱ��������༭��ļ�ֵ
* @param[in] unsigned char key						����ļ�ֵ
* @param[out] unsigned char *code				
* @return
* @note �˺���ִ��֮���Ӱ��ȫ�ֵı༭��״̬���༭��״̬������ʾ
*		 EDIT_STATE_INIT_STATUS		���ڷ��������뷨״̬
*        EDIT_STATE_ZD_NAME_SELECT		����ƴ��ѡ��׶�
*        EDIT_STATE_ZD_VALUE_SELECT		���ں���ѡ��׶�
*        EDIT_STATE_CODE_GOT		��ȡ������ļ�ֵ�ı���
*		������״̬ͨ��KEY_ENTER������KEY_CLEAR�����л���
*		EDIT�ĳ�ʼ״̬���Ƿ��ֵ����뷨״̬������F3���л�Ϊ�ֵ�����״̬����ѯϵͳ���õ��ֵ����ƣ����ֵ�������ʾ�����󣬿���ͨ�����¼�ѡ���ֵ����ƣ�
*		��KEY_ENTER���ֵ�����ѡ��״̬�л����ֵ�ֵ��ѡ��״̬�����ֵ�ֵѡ��״̬�¿���ͨ�����·���ѡ����Ӧ��ֵ��
*		���ֵ�ֵѡ��״̬�°�KEY_ENTER���ͽ����ȡ������ļ�ֵ�ı��룬������ȡ�����ֵ�ֵ�ı��뷵�ء�
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
		//��ʾϵͳ���õ��ֵ��б�
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

			//�����ֵ�ֵ����
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
				//��ʾ���ֵ������
				display_item_yet = display_ZD_value_list(zd_name_valid_tbl[select_ZD],start_display_item,current_item,&total_value_item);
				if (total_value_item == 0)
				{
					edit_state = EDIT_STATE_CODE_GOT;
					//�����ֵ�ֵ����
					code[0] = 0;
					gui_FillRect(0,CHINESE_EDIT_ZONE_Y-1,GUI_WIDTH,CHINESE_EDIT_ZONE_H,BG_COLOR);
				}
			}
			else
			{
				//ѡ������������̶����ֵ��ֶΡ���ǰ���ڡ��͡���ǰʱ�䡱��ֻ����ϵͳʱ�䷵�ص��༭��
				edit_state = EDIT_STATE_CODE_GOT;
				GetDateTime();
				Systime2BCD(datetime_bcd);
				//�����ֵ�ֵ����
				if (current_item == total_name_item)
				{
					//ѡ����ǡ���ǰ���ڡ�
					bcd_field_to_str(datetime_bcd,4,datetime_str);
					memcpy(code,datetime_str,4);
					code[4] = '-';
					memcpy(code+5,datetime_str+4,2);
					code[7] = '-';
					strcpy(code+8,datetime_str+6);
				}
				else
				{
					//ѡ����ǡ���ǰʱ�䡱
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

			//��ʾϵͳ���õ��ֵ��б�
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