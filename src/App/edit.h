#ifndef _EDIT_H_
#define _EDIT_H_



//����ÿ���������֧�ֵı༭�����
#define MAX_EDIT				2

//����ÿ���༭���������󳤶�
#define MAX_EDITNUM				31


//�����������봦������״̬
#define	EDIT_STATE_INIT_STATUS		0		//�༭��û�п�ʼ�������������״̬
#define EDIT_STATE_PY_SELECT		1		//�༭���Ѿ���������������״̬��ƴ��ѡ��״̬
#define EDIT_STATE_HZ_SELECT		2		//�༭���Ѿ���������������״̬�ĺ���ѡ��״̬
#define EDIT_STATE_CODE_GOT			3		//�༭���Ѿ���ȡ�������ֵ�ı���

#define EDIT_STATE_ZD_NAME_SELECT	4		//�༭�����ѡ���ֵ�����״̬
#define EDIT_STATE_ZD_VALUE_SELECT	5		//�༭�����ѡ���ֵ�ֵ��״̬


//���뷨���Ͷ���
#define PINYIN_INPUT	0x01
#define ABC_INPUT		0x02
#define abc_INPUT		0x03
#define _123_INPUT		0x04

//������������ʱ���Ƶ���ʾ����������Ϣ
#define CHINESE_EDIT_ZONE_X						24//X Pos
//#define CHINESE_EDIT_ZONE_Y						220//Y Pos
#define CHINESE_EDIT_ZONE_Y						222//Y Pos
#define CHINESE_EDIT_ZONE_W						24*8//���
//#define CHINESE_EDIT_ZONE_H						72//�߶�
#define CHINESE_EDIT_ZONE_H						70//�߶�

#define CHINESE_EDIT_TXT_COLOR					COLOR_BLUE		//������ɫ
#define CHINESE_EDIT_BG_COLOR					COLOR_GRAY		//������ɫ

#define CHINESE_EDIT_FONT_SIZE					24//�����С

#define ZD_EDIT_FONT_SIZE						16


unsigned char edit_alpha_proc(unsigned char key,unsigned char input_method,unsigned char *key_code);
unsigned char edit_chinese_proc(unsigned char key,unsigned char input_method, unsigned char *code);
void edit_ZD_proc(unsigned char key,unsigned char *code);

#endif