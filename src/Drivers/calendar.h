/**
* @file		calendar.h
* @brief	日历模块的头文件
* @author   joe
* @version  v1.00
* @date     2009.10.15
* @note    
*/
#ifndef _CALENDAR_H_
#define _CALENDAR_H_


//#define TAMPER_DETECT_ENABLE

/**
* @brief 定义时间的数据结构                     
*/
typedef struct 
{
	unsigned char	sec;
	unsigned char	min;
	unsigned char	hour;
	unsigned char   week;
	unsigned char   day;
	unsigned char   month;
	unsigned char   year;
} TypedefDateTime;


void Calendar_Init(void);
void SetDateTime(void);
void GetDateTime(void);
void Systime2BCD(unsigned char * time_bcd);
#ifdef  TAMPER_DETECT_ENABLE
void TAMPER_IRQ_Handle(void);
#endif
#endif
