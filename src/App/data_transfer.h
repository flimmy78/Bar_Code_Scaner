#ifndef _DATA_TRANSFER_H_
#define _DATA_TRANSFER_H_

#include "record.h"


//文件类型的宏定义
#define FILE_TYPE_GOODS_LIST		0x01			//商品信息的节点文件
#define FILE_TYPE_BAR_HASH_TBL		0x02			//按照条码进行HASH分算的HASH表文件
#define FILE_TYPE_NAME_HASH_TBL		0x03			//按照名称进行HASH分算的HASH表文件
#define FILE_TYPE_CHECK_INF			0x04			//盘点信息的节点文件
#define FILE_TYPE_SERIAL_INF		0x05			//序列号信息节点文件中被删除的节点信息文件
#define FILE_TYPE_SERIAL_LIST		0x06			//序列号信息的节点文件
//#define FILE_TYPE_CHK_EXT_INF		0x07			//空盘点信息的节点文件
#define FILE_TYPE_MULTCHK_INF		0x08			//盘点单据列表文件中被删除节点的信息文件
#define FILE_TYPE_MULTCHK_LST		0x09			//盘点单据列表的节点文件
#define FILE_TYPE_MULTXLH_INF		0x0a			//序列号单据列表文件中被删除节点的信息文件
#define FILE_TYPE_MULTXLH_LST		0x0b			//序列号单据列表的节点文件

#define FILE_TYPE_UPDATE_BIN		0x11			//应用升级文件
#define FILE_TYPE_RESDATA_BIN		0x12			//资源文件


//操作代码的宏定义
#define OP_CODE_CONNECT						0x0f0e			//连接
#define OP_CODE_CONNECT2					0x8f8e			//连接2（API 方式访问，为了区别旧版本的工具）
#define OP_CODE_DISCONNECT					0x0f0f			//断开
#define OP_CODE_INQ_APP_VERSION				0x0001			//查询应用版本号
#define OP_CODE_INQ_PARAM_VERSION			0x0002			//查询参数版本号
#define OP_CODE_VERIFY						0x0003			//认证
#define OP_CODE_INQ_REC_FILE_LIST			0x0101			//查询记录的文件列表
#define OP_CODE_READ_REC_FILE				0x0201			//读取记录文件
#define OP_CODE_DOWNLOAD_GOODS_INFO			0x0301			//下载商品信息
#define OP_CODE_DOWNLOAD_APP				0x0401			//下载应用升级文件
#define OP_CODE_DOWNLOAD_RESDATA			0x0402			//下载资源升级文件


//会话类型
#define TRANS_TYPE_GOODS_INFO_OUT		0x01		//导出商品信息
#define TRANS_TYPE_CHECK_INFO_OUT		0x02		//导出盘点信息
#define TRANS_TYPE_SERIALDATA_OUT		0x03		//导出序列号采集清单
//#define TRANS_TYPE_CHECK_EXT_INFO_OUT	0x05		//导出空盘点信息

#define TRANS_TYPE_GOODS_INFO_IN		0x11		//导入商品信息
#define TRANS_TYPE_UPDATE_IN			0x21		//导入应用升级文件
#define TRANS_TYPE_RESDATA_IN			0x22		//导入资源升级文件

#pragma pack(1)
#if 0
/**
* @brief	下载或者上传的库存商品数据结构
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4字节	0	B		此份结构的校验值 crc32
	int						total_num;					// 4字节	4   B		库存数量
	unsigned char			bar_code[21];				// 21字节	8	N		商品条形码
	unsigned char			name[31];					// 31字节	29	N		商品名称
	unsigned char			spec[GOODS_SPEC_CNT][22];	// 132字节	60  N		商品规格
	unsigned char			last_in_datetime[7];		// 7字节	192 BCD		最近入库时间
	unsigned char			last_out_datetime[7];		// 7字节	199 BCD		最近出库时间		//共206字节
}TINVENTORY_INFO;

/**
* @brief	下载或者上传的盘点商品数据结构
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4字节	0	B		此份结构的校验值 crc32
	int						check_num;					// 4字节	4   B		盘点数量
	unsigned char			bar_code[21];				// 21字节	8	N		商品条形码
	unsigned char			name[31];					// 31字节	29	N		商品名称
	unsigned char			spec[GOODS_SPEC_CNT][22];	// 132字节  60  N		规格字段名(8字节):规格描述(12字节)	
	unsigned char			last_check_datetime[7];		// 7字节	192 BCD		最近入库时间
	unsigned char			last_check_oprator_num[2];	// 2字节	199 BCD		最近盘点操作员号码
	unsigned char			rfu[5];						// 5字节	201	B		保留，				凑成206字节，保证和库存清单的记录一样大小
}TCHECK_INFO;

/**
* @brief	上传给后台时产品明细的数据结构
*/
typedef struct  
{
	unsigned int			num;						// 4字节	0	B		出入库数量
	unsigned int			auto_a_d_num;				// 4字节	4	B		自动扣除或者自动增加的数量
	unsigned char			name[31];					// 31字节	8	N		商品名称
	unsigned char			bar_code[21];				// 21字节   39   N		商品代码   //共60字节
}TGOODS_DETAIL;

/**
* @brief	上传给后台时操作记录的数据结构
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4字节	0	B		此份结构的校验值 crc32
	unsigned char			operator_num[2];			// 2字节	4	N		操作员号码
	unsigned char			operator_name[8];			// 8字节	6   N		操作员姓名
	unsigned char			datetime[7];				// 7字节	14	BCD		出入库时间
	TGOODS_DETAIL			goods_detail;				// 60字节	21			入库的商品信息（商品代码和入库的数量）
	unsigned int			orignal_num;				// 4字节	81	B		最近一次修改之前的值,如果此值不为0，表明此操作记录被修改过
	unsigned char			trans_in_flag;				// 1字节	85	B		数据导入的标记，表明改记录是导入清单时增加的			//共86字节
}TGOODS_OP_INFO;

#pragma pack()
#endif

extern unsigned char			hh_type;				//会话类型
extern unsigned short			op_code;				//接收到的操作码 	

typedef void (*datatrans_callback)(void);

void usb_device_init(unsigned char device_type);
int data_transfer_proc(unsigned char *cmd,int mode,datatrans_callback pcallback);
//void add_op_record_after_trans_in(TINVENTORY_RECORD *p_inventory);

#endif