#ifndef _DS2068_H_
#define _DS2068_H_

/*
typedef struct		//�����ʱ�����ͽṹ��,ȫ����Ϊʮ����
{
	unsigned char Second;
	unsigned char Minute;
	unsigned char Hour;
	unsigned char Week;
	unsigned char Day;
	unsigned char Month;
	unsigned char Year;
}TSystemTimeStr;
*/

void  SD2068A_Initial(void);
int  SD2068A_GetTime(unsigned char *TimeBuff);
int  SD2068A_SetTime(unsigned char *TimeBuff);

#endif

