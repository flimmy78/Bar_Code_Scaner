

#ifndef _APP_H_
#define _APP_H_

#include "dialog.h"
#include "keypad.h"
#include "record.h"


#define HZ_TEST		0//���Ժ�����ģ


#define BARCODE_LEN_MIN		1		//�������С����


#define LIST_TYPE_GOODS			1	//��Ʒ�嵥
#define LIST_TYPE_CHECK			2	//�̵��嵥
#define LIST_TYPE_SERIAL_DATA	3	//���к��嵥



//��¼����
#define GOODS_LIST_TYPE		1
#define CHECK_LIST_TYPE		2





//������ϸʱ����Ҫ�����Ĳ�������
#define OPERATE_TYPE_IN			0x01
#define OPERATE_TYPE_OUT		0x02
#define OPERATE_TYPE_CHECK		0x04


/*
*�̵���Ĺ���ģʽ,pos_state��ȡֵ
*/

#define STA_FACTORY_MODE        0x01
#define STA_GPRS_EXIST			0x10
#define STA_NO_SIM				0x20




#define MODIFY_NUM_PER_VIEW					50		//ÿ�����������¼ʱ�������޸Ĳ�����¼�Ĵ���		//HEAP�ռ���Ҫ2048�����Ҫ���������������ô��Ҫ����HEAP�ռ�

#define MODIFY_NUM_GOODS_SPEC				10		//ÿ���������嵥�����̵��嵥ʱ�������޸���Ʒ���Ĵ�������������ˣ�ϵͳ���Զ���ʾ���¿����Ϣ��		//��Ҫռ��1800�ֽڵ��ڴ�ռ�

//typedef union 
//{
//	struct  
//	{
//		unsigned int	rec_index;
//		int				value_modify;
//		unsigned char   barcode[21];
//		unsigned char	searched;
//	}						modify_item[MODIFY_NUM_PER_VIEW];					//
//}TOPERATE_REC_MODIFY_TABLE;						//32*MODIFY_NUM_PER_VIEW




void task_ui(void*pp);
void app_init(void);
void device_init_by_setting(void);

#endif
