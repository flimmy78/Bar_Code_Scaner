

#ifndef _APP_H_
#define _APP_H_

#include "dialog.h"
#include "keypad.h"
#include "record.h"


#define HZ_TEST		0//测试汉字字模


#define BARCODE_LEN_MIN		1		//条码的最小长度


#define LIST_TYPE_GOODS			1	//商品清单
#define LIST_TYPE_CHECK			2	//盘点清单
#define LIST_TYPE_SERIAL_DATA	3	//序列号清单



//记录类型
#define GOODS_LIST_TYPE		1
#define CHECK_LIST_TYPE		2





//搜索明细时的需要搜索的操作类型
#define OPERATE_TYPE_IN			0x01
#define OPERATE_TYPE_OUT		0x02
#define OPERATE_TYPE_CHECK		0x04


/*
*盘点机的工作模式,pos_state的取值
*/

#define STA_FACTORY_MODE        0x01
#define STA_GPRS_EXIST			0x10
#define STA_NO_SIM				0x20





#pragma  pack(1)

typedef struct Tag_InOut_MX
{
	unsigned char operate;			//1:入库	2:出库   3:盘点		4:导入
	unsigned int  num;				//数量
	unsigned char date_time[7];		//时间
	unsigned char operator_num[2];	//操作员号码
	struct Tag_InOut_MX	*next;		//指向下一个明细
}TGOODS_INOUT_MX;


#pragma pack()


#define MODIFY_NUM_PER_VIEW					50		//每次浏览操作记录时，可以修改操作记录的次数		//HEAP空间需要2048，如果要增加这个次数，那么需要增加HEAP空间

#define MODIFY_NUM_GOODS_SPEC				10		//每次浏览库存清单或者盘点清单时，可以修改商品规格的次数，如果超过了，系统会自动提示更新库存信息。		//需要占用1800字节的内存空间

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


//被修改的操作记录
typedef struct  
{
	unsigned int	rec_index;
	int				value_modify;
	int				value_changed;
	unsigned char   barcode[21];
	unsigned char	searched;
}	TOPERATE_REC_MODIFY_ITEM;


//被修改的商品规格
typedef struct  
{
	unsigned int	rec_index;
	unsigned char   name[61];
	unsigned char	modify_spec[GOODS_SPEC_CNT][52];
	unsigned char   old_name[61];
}TGOODS_DETAIL_MODIFY_ITEM;

void task_ui(void*pp);
void app_init(void);
void device_init_by_setting(void);

#endif
