/**
 * @file YFBT07.h
 * @brief ����ģ��������ͷ�ļ� 
 * @version 1.0
 * @author joe
 * @date 2013��10��25��
 * @note
*/
#ifndef _YFBT07_H_
#define _YFBT07_H_


int YFBT07_init(void);
int YFBT07_check(void);
int YFBT07_Enter_Match_Mode(void);
int YFBT07_SendKey(unsigned char *key_buffer,unsigned int len);
int YFBT07_Connect_state(void);
void YFBT07_power_ctrl(unsigned char on_off);
void YFBT07_Test(void);

#endif