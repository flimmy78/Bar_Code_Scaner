/**
 * @file data_transfer.c
 * @brief 盘点机与PC工具之间数据传输应用层的实现
 *
 * @version V0.0.1
 * @author joe
 * @date 2011年05月11日
 * @note 
 *		请参考《盘点机与PC通讯规范.doc》
 * @copy
 *
 * 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
 * 本公司以外的项目。本司保留一切追究权利。
 *
 * <h1><center>&copy; COPYRIGHT 2011 netcom</center></h1>
 */

#include "data_transfer.h"
#include "tlv.H"
#include "PCUsart.h"
#include "hw_config.h"
#include "usb_lib.h"
#include <string.h>
#include "usb_pwr.h"
#include "Terminal_Para.h"
#include "calendar.h"
#include "record.h"


#if(IF_FILE_SIZE == IF_FILE_SIZE_2K)
	#define TRANS_DATA_MAX_LEN	2000			//透传数据的最大长度
	static unsigned char	g_comm_buf[2048];		//通用的buf
	static unsigned char	ack_buffer[2200];
#else
	#define TRANS_DATA_MAX_LEN	1000			//透传数据的最大长度
	static unsigned char	g_comm_buf[1024];		//通用的buf
	static unsigned char	ack_buffer[1100];
#endif

static unsigned char			need_verify_flag;


extern unsigned int		vcom_device_state;


//static unsigned int		inventory_rec_num;		//库存清单的记录数
//static unsigned int		check_rec_num;			//盘点清单的记录数
//static unsigned int		serialdata_rec_num;		//序列号采集清单的记录数
unsigned int		download_list_flag;		//开始下载清单的标记
//static unsigned char	expect_op_code;			//期望主机必须发送的命令

unsigned int			hh_lsh;					//会话流水号
unsigned char			hh_type;				//会话类型
unsigned short			op_code;				//接收到的操作码
unsigned char			data_trans_state;				//数据导入导出时的状态

//extern  TINVENTORY_RECORD			inventory_info;			//商品的库存信息
//extern  TGOODS_SPEC_RECORD		goods_spec;				//商品的规格信息
//extern	OP_REC_NODE				operate_rec;					//备份操作记录
//extern  unsigned char				current_operator;			// 当前登录到系统的操作员
extern  unsigned char				g_usb_type;
extern	TTerminalPara				g_param;							//Terminal Param
//extern  TINVENTORY_RECORD			*p_inventory_info;	//指向搜索到商品库存信息记录
extern  const unsigned char				*app_ver;

/**
* @brief 构造记录文件列表
* @param[in] unsigned char  rec_type	记录类型
* @param[out] unsigned char *out_buf	存放记录明细数据
* @return 返回构造的记录明细数据的长度
* @note
*/
static unsigned int build_rec_file_list(unsigned char rec_type,unsigned char *out_buf)
{
	unsigned int		off = 0;
	int					file_size,i;
	unsigned char		dir_str[35];



	if (REC_TYPE_GOODS_LIST == rec_type)
	{
		//读取商品信息记录文件夹中的文件列表

		out_buf[off++] = 2;						//商品信息记录文件夹包含2个hash文件和1个记录文件，共3个文件
		out_buf[off++] = FILE_TYPE_GOODS_LIST;						//goods.lst
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,goods_list_file);
		file_size = get_file_size(dir_str);
		memcpy(out_buf+off,(void*)&file_size,4);off += 4;
		out_buf[off++] = FILE_TYPE_BAR_HASH_TBL;
		file_size = HASH_TABLE_SIZE*4;
		memcpy(out_buf+off,(void*)&file_size,4);off += 4;
	}
	//else if (REC_TYPE_SERIAL_LIST == rec_type)
	//{
	//	//读取序列号采集清单记录夹中的文件列表
	//	out_buf[off++] = 3;						//序列号采集记录文件夹包含1个hash文件、1个序列号信息文件和1个记录文件，共3个文件
	//	out_buf[off++] = FILE_TYPE_SERIAL_LIST;						//serial.lst
	//	strcpy(dir_str,serial_data_dirctory);
	//	strcat(dir_str,current_dj_dir);
	//	strcat(dir_str,serial_list_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;
	//	out_buf[off++] = FILE_TYPE_SERIAL_INF;						//serial.inf
	//	strcpy(dir_str,serial_data_dirctory);
	//	strcat(dir_str,current_dj_dir);
	//	strcat(dir_str,serial_data_info_file);
	//	file_size = get_file_size(dir_str);

	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;

	//	out_buf[off++] = FILE_TYPE_BAR_HASH_TBL;
	//	file_size = HASH_TABLE_SIZE*4;
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;

	//}
	//else if (REC_TYPE_CHECK_INFO == rec_type)
	//{
	//	//读取盘点信息记录文件夹中的文件列表
	//	out_buf[off++] = 2;						//盘点信息文件夹包含1个hash文件和1个记录文件，共2个文件

	//	out_buf[off++] = FILE_TYPE_CHECK_INF;			//"check.lst"
	//	strcpy(dir_str,check_info_dirctory);
	//	strcat(dir_str,check_info_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;

	//	out_buf[off++] = FILE_TYPE_BAR_HASH_TBL;
	//	file_size = HASH_TABLE_SIZE*4;
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;
	//}
	//else if (REC_TYPE_CHECK_MULTI == rec_type)
	//{
	//	//读取多单据盘点信息记录文件夹的文件列表
	//	out_buf[off++] = 2;						//盘点信息文件夹包含1个hash文件和1个记录文件，共2个文件

	//	out_buf[off++] = FILE_TYPE_CHECK_INF;			//"check.lst"
	//	strcpy(dir_str,multi_check_dirctory);
	//	strcat(dir_str,current_dj_dir);
	//	strcat(dir_str,check_info_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;

	//	out_buf[off++] = FILE_TYPE_BAR_HASH_TBL;
	//	file_size = HASH_TABLE_SIZE*4;
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;
	//}
	//else if (REC_TYPE_MULTCHK_LIST == rec_type)
	//{
	//	//读取多单据盘点中的单据列表
	//	out_buf[off++] = 2;						//多单据盘点文件夹包含1个inf文件和1个单据名节点文件，共2个文件

	//	out_buf[off++] = FILE_TYPE_MULTCHK_INF;			//"multchk.inf"
	//	strcpy(dir_str,multi_check_dirctory);
	//	strcat(dir_str,multchk_data_info_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;

	//	out_buf[off++] = FILE_TYPE_MULTCHK_LST;			//"multchk.lst"
	//	strcpy(dir_str,multi_check_dirctory);
	//	strcat(dir_str,multchk_list_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;
	//}
	//else if (REC_TYPE_MULTXLH_LIST == rec_type)
	//{
	//	//读取多单据序列号文件夹的单据列表
	//	out_buf[off++] = 2;						//盘点信息文件夹包含1个hash文件和1个记录文件，共2个文件

	//	out_buf[off++] = FILE_TYPE_MULTXLH_INF;			//"multxlh.inf"
	//	strcpy(dir_str,serial_data_dirctory);
	//	strcat(dir_str,multxlh_data_info_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;

	//	out_buf[off++] = FILE_TYPE_MULTXLH_LST;			//"multxlh.lst"
	//	strcpy(dir_str,serial_data_dirctory);
	//	strcat(dir_str,multxlh_list_file);
	//	file_size = get_file_size(dir_str);
	//	memcpy(out_buf+off,(void*)&file_size,4);off += 4;
	//}
	else
	{
		return 0;			//应该进不来这里
	}
	return off;
}

/**
* @brief 构造目标记录文件的路径
* @param[in] unsigned char rectype	记录类型
* @param[in] unsigned char filetype 文件类型
* @param[out] unsigned char *p_dir  返回构造的文件路径
* @note
*/
static void	build_target_rec_dir(unsigned char rectype,unsigned char filetype,unsigned char *p_dir)
{
	const unsigned char *p_tmp_dir;
	const unsigned char *p_tmp_file;

	switch (rectype)
	{
	case REC_TYPE_GOODS_LIST:
		p_tmp_dir = goods_list_dirctory;
		break;
	//case REC_TYPE_CHECK_INFO:
	//	p_tmp_dir = check_info_dirctory;
	//	break;
	//case REC_TYPE_SERIAL_LIST:
	//	p_tmp_dir = serial_data_dirctory;
	//	break;
	//case REC_TYPE_CHECK_MULTI:
	//	p_tmp_dir = multi_check_dirctory;
	//	break;
	//case REC_TYPE_MULTCHK_LIST:
	//	p_tmp_dir = multi_check_dirctory;
	//	break;
	//case REC_TYPE_MULTXLH_LIST:
	//	p_tmp_dir = serial_data_dirctory;
	//	break;
	default:
		return;
	}

	switch (filetype)
	{
	case FILE_TYPE_GOODS_LIST:
		p_tmp_file = goods_list_file;
		break;
	case FILE_TYPE_BAR_HASH_TBL:
		p_tmp_file = barcode_hash_table_file;
		break;
	case FILE_TYPE_NAME_HASH_TBL:
		p_tmp_file = name_hash_table_file;
		break;
// 	case FILE_TYPE_SERIAL_INF:
// 		p_tmp_file = serial_data_info_file;
// 		break;
	//case FILE_TYPE_CHECK_INF:
	//	p_tmp_file = check_info_file;
	//	break;
// 	case FILE_TYPE_SERIAL_LIST:
// 		p_tmp_file = serial_list_file;
// 		break;
// 	case FILE_TYPE_MULTCHK_INF:
// 		p_tmp_file = multchk_data_info_file;
// 		break;
// 	case FILE_TYPE_MULTCHK_LST:
// 		p_tmp_file = multchk_list_file;
// 		break;
// 	case FILE_TYPE_MULTXLH_INF:
// 		p_tmp_file = multxlh_data_info_file;
// 		break;
// 	case FILE_TYPE_MULTXLH_LST:
// 		p_tmp_file = multxlh_list_file;
// 		break;
	default:
		return;
	}

	strcpy(p_dir,p_tmp_dir);
// 	if (REC_TYPE_CHECK_MULTI == rectype || REC_TYPE_SERIAL_LIST == rectype)
// 	{
// 		strcat(p_dir,current_dj_dir);
// 	}
	strcat(p_dir,p_tmp_file);

	return;
}

/**
* @brief 构造响应报文
* @param[in] unsigned short			op_code		命令代码
* @param[in] unsigned int			param1		参数1
* @param[in] unsigned int			param2		参数2
* @param[in] unsigned int			param3		参数3
* @return	返回响应数据的长度
* @note
*/
static unsigned int build_ack(unsigned short op_code,unsigned int param1,unsigned int param2,unsigned int param3,unsigned int param4)
{
	TTLVPacket				tlvpacket;
	unsigned int			data_field_len;
	unsigned char			tmp[8];
	unsigned char			dir_str[30];
	int						r_len;
	union
	{
		unsigned int	Dev_Serial_int[3];
		unsigned char	Dev_Serial_byte[12];
	} Dev_Serial;

	tlv_init(&tlvpacket,ack_buffer);	

	tlv_addtag(&tlvpacket, TAG_HH_LSH, (unsigned char*)&hh_lsh);

	if (OP_CODE_CONNECT == op_code)
	{
		//连接命令的响应报文
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tmp[0] = 0x00;
		tmp[1] = 0x02;		//终端返回数据帧最大长度,512
		tmp[2] = 0x00;
		tmp[3] = 0x02;		//终端可以接收的数据帧的最大长度,512
		tmp[4] = 0x00;
		tmp[5] = 0x00;		//锁定状态
		tmp[6] = tmp[7] = 0;
		tlv_addtag(&tlvpacket, TAG_IF_FEATURE, tmp);
		tlv_addtag(&tlvpacket, TAG_HH_TYPE, &hh_type);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x0f\x0e" );
	}
	else if (OP_CODE_DISCONNECT == op_code)
	{
		//断开命令的响应报文
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tlv_addtag(&tlvpacket, TAG_HH_RESULT, (unsigned char*)&param1);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x0f\x0f" );
	}
	else if (OP_CODE_INQ_APP_VERSION == op_code)
	{
		//查询应用版本号的响应报文
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tlv_addtag(&tlvpacket, TAG_APP_VERSION, (unsigned char*)app_ver);

		Dev_Serial.Dev_Serial_int[0] = *(vu32*)(0x1FFFF7E8);
		Dev_Serial.Dev_Serial_int[1] = *(vu32*)(0x1FFFF7EC);
		Dev_Serial.Dev_Serial_int[2] = *(vu32*)(0x1FFFF7F0);

		tlv_addtag(&tlvpacket, TAG_DEVICE_ID, Dev_Serial.Dev_Serial_byte);		//设备ID
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x00\x01" );
	}
	else if (OP_CODE_INQ_PARAM_VERSION == op_code)
	{
		//查询参数版本号的响应报文
		if (param1)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "01");		//认证失败
		}
		else
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "00");
		}
		
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x00\x03" );
	}
	//else if (OP_CODE_VERIFY == op_code)
	//{
	//	//认证
	//	tlv_addtag(&tlvpacket, TAG_ACK, "00");
	//	tlv_addtag(&tlvpacket, TAG_PARAM_VERSION, "V1.0.0");
	//	tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x00\x02" );
	//}
	else if (OP_CODE_INQ_REC_FILE_LIST == op_code)
	{
		//读取记录文件列表
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tlv_addtag(&tlvpacket, TAG_REC_TYPE, (unsigned char*)&param1);
// 		if (((unsigned char)param1 == REC_TYPE_CHECK_MULTI) || ((unsigned char)param1 == REC_TYPE_SERIAL_LIST))
// 		{
// 			tlv_addtag(&tlvpacket, TAG_DJ_DIR, current_dj_dir);
// 		}
		data_field_len = build_rec_file_list((unsigned char)param1,g_comm_buf);		//构造文件列表数据域
		tlv_addtag_ext(&tlvpacket,TAG_FILE_LIST,g_comm_buf,data_field_len);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x01\x01" );
	}
	else if (OP_CODE_READ_REC_FILE == op_code)
	{
		//读取记录文件
		build_target_rec_dir((unsigned char)param1,(unsigned char)param2,dir_str);

		if (param4 > TRANS_DATA_MAX_LEN)
		{
			param4 = TRANS_DATA_MAX_LEN;
		}
		r_len = read_rec_file(dir_str,param3,param4,g_comm_buf);
		if (r_len < 0)
		{
			//读取失败
			tlv_addtag(&tlvpacket, TAG_ACK, "04");		//记录读取失败
			tlv_addtag(&tlvpacket, TAG_REC_TYPE, (unsigned char*)&param1);
// 			if (((unsigned char)param1 == REC_TYPE_CHECK_MULTI) || ((unsigned char)param1 == REC_TYPE_SERIAL_LIST))
// 			{
// 				tlv_addtag(&tlvpacket, TAG_DJ_DIR, current_dj_dir);
// 			}
			tlv_addtag(&tlvpacket, TAG_FILE_TYPE, (unsigned char*)&param2);
		}
		else
		{
			//读取成功
			tlv_addtag(&tlvpacket, TAG_ACK, "00");		
			tlv_addtag(&tlvpacket, TAG_REC_TYPE, (unsigned char*)&param1);
// 			if (((unsigned char)param1 == REC_TYPE_CHECK_MULTI) || ((unsigned char)param1 == REC_TYPE_SERIAL_LIST))
// 			{
// 				tlv_addtag(&tlvpacket, TAG_DJ_DIR, current_dj_dir);
// 			}
			tlv_addtag(&tlvpacket, TAG_FILE_TYPE, (unsigned char*)&param2);
			tlv_addtag(&tlvpacket, TAG_DATA_LENGTH, (unsigned char*)&r_len);
			tlv_addtag_ext(&tlvpacket,TAG_TRNAS_DATA,g_comm_buf,r_len);
		}
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x02\x01" );
	}
	else if (OP_CODE_DOWNLOAD_GOODS_INFO == op_code)
	{
		//下载商品信息
		if (param2==1)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "01");		//参数错误
		}
		else if (param2==2)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "02");		//更新失败
		}
		else if (param2==3)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "03");		//记录不完整或者格式不正确
		}
		else
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "00");
		}
		tlv_addtag(&tlvpacket, TAG_REC_TYPE, (unsigned char*)&param1);
		tlv_addtag(&tlvpacket, TAG_FILE_TYPE, (unsigned char*)&param3);
		tlv_addtag(&tlvpacket, TAG_DATA_LENGTH, (unsigned char*)&param4);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE, "\x03\x01");

	}
	else if (OP_CODE_DOWNLOAD_APP == op_code || OP_CODE_DOWNLOAD_RESDATA == op_code )
	{
		//下载应用升级文件
		if (param2==1)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "01");		//参数错误
		}
		else if (param2==2)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "02");		//更新失败
		}
		else if (param2==3)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "03");		//记录不完整或者格式不正确
		}
		else
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "00");
		}
		tlv_addtag(&tlvpacket, TAG_FILE_TYPE, (unsigned char*)&param1);
		tlv_addtag(&tlvpacket, TAG_DATA_LENGTH, (unsigned char*)&param4);
		if (OP_CODE_DOWNLOAD_APP == op_code)
		{
			tlv_addtag(&tlvpacket, TAG_OPERATE_CODE, "\x04\x01");
		}
		else
		{
			tlv_addtag(&tlvpacket, TAG_OPERATE_CODE, "\x04\x02");
		}
	}
	else 
	{
		;		//应该来不了这儿的！！！
	}

	tlv_pack(&tlvpacket);

	return (tlvpacket.ptr);
}


//下载库存清单之后，将下载的数据增加到入库操作记录
//void add_op_record_after_trans_in(TINVENTORY_RECORD *p_inventory)
//{
//
//	//通过新导入的库存清单,增加入库操作记录
//	//@todo....
//
//	strcpy(operate_rec.goods_detail.code.bar_code,p_inventory->bar_code);
//	operate_rec.goods_detail.spec_rec_index = p_inventory->spec_rec_index;
//	operate_rec.goods_detail.num = p_inventory->total_num;
//
//	if(operate_rec.goods_detail.num)
//	{
//		//如果导入时库存数不为0才需要增加一条入库操作记录
//		if (record_add(g_param.current_goods_in_rec,(unsigned char*)&operate_rec))
//		{
//			record_add(g_param.current_goods_in_rec,(unsigned char*)&operate_rec);
//		}
//	}
//}

//检查下载的清单记录的格式是否正确
//static int check_list_rec_format(TINVENTORY_INFO * p_list_rec)
//{
//	int i;
//		 
//	for (i = 0; i < strlen(p_list_rec->bar_code);i++)
//	{
//		if (p_list_rec->bar_code[i] > 0x80)
//		{
//			return -1;		//条形码中包含了非法的字符 
//		}
//	}
//
//	if (strlen(p_list_rec->name) > 29)
//	{
//		//判断最后一个字节是否是汉字的一半还是一个可显示字符（汉字或者英文字符）的结束字符
//		i = 0;
//		while(1)
//		{
//			if (p_list_rec->name[i] > 0x80)
//			{
//				i+=2;
//			}
//			else
//			{
//				i++;
//			}
//
//			if (i>=29)
//			{
//				break;
//			}
//		}
//
//		if (i == 30)
//		{
//			p_list_rec->name[30] = 0;
//		}
//		else
//		{
//			p_list_rec->name[29] = 0;
//		}
//	}
//
//	//时间格式的校验
//	//@todo...
//
//	return 0;
//}

/**
 * @brief 处理接收到的命令
 * @param[in] unsigned char *cmd			指向接收到的命令
 * @return
 * @note
 */
int data_transfer_proc(unsigned char *cmd,int mode,datatrans_callback pcallback)
{
	int						res;
	TTLVPacket				tlvpacket;
	unsigned char			*pvalue;
	unsigned char			rec_type,hh_result,file_type;
	unsigned int			file_offset,data_len,res_len;
	unsigned char			download_end_flag;
	int						w_len;

	unsigned char			dir_str[30];

	//1.检查数据帧结构、数据帧校验是否正确
	res = tlv_verify(cmd);
	if (res)
	{
		return res;		//-1 -2 -3 -4
	}

	tlvpacket.ptr = 0;
	tlvpacket.buffer = cmd;

	//解析命令的会话流水号
	pvalue	= tlv_get_tagvalue(TAG_HH_LSH, &tlvpacket);
	if (pvalue == 0)
	{
		return -6;				//命令数据域缺失
	}

	hh_lsh =  pvalue[3];
	hh_lsh <<= 8;
	hh_lsh |= pvalue[2];
	hh_lsh <<= 8;
	hh_lsh |= pvalue[1];
	hh_lsh <<= 8;
	hh_lsh |= pvalue[0];

	//解析命令的操作代码
	pvalue	= tlv_get_tagvalue(TAG_OPERATE_CODE, &tlvpacket);
	if (pvalue == 0)
	{
		return -6;				//命令数据域缺失
	}
	op_code = pvalue[0];
	op_code <<= 8;
	op_code |= pvalue[1];


	if (OP_CODE_CONNECT == op_code)
	{
		pvalue = tlv_get_tagvalue(TAG_HH_TYPE,&tlvpacket);
		if (pvalue == 0)
		{
			return -6;		//命令数据域缺失
		}

		if (mode == 0)
		{
			/*if ((pvalue[0] != TRANS_TYPE_GOODS_INFO_OUT)&&(pvalue[0] != TRANS_TYPE_SERIALDATA_OUT)&&
				(pvalue[0] != TRANS_TYPE_CHECK_INFO_OUT)&&
				(pvalue[0] != TRANS_TYPE_GOODS_INFO_IN))*/
			if (pvalue[0] != TRANS_TYPE_GOODS_INFO_IN)
			{
				//会话类型数据域错误
				return -6;
			}

		}
		else
		{
			if ((pvalue[0] != TRANS_TYPE_UPDATE_IN)&&(pvalue[0] != TRANS_TYPE_RESDATA_IN))
			{
				//会话类型数据域错误
				return -6;
			}

		}

		hh_type = pvalue[0];			//PC要求进行的会话类型

		//任意时刻都可以响应连接命令
		vcom_device_state = 1;			//接收到了连接命令

		//初始化下载清单的标记
		download_list_flag = 0;

		//expect_op_code = 0;

		//构造连接命令的响应报文
		res_len = build_ack(op_code,0,0,0,0);

		need_verify_flag = 0;
	}
	else
	{
		//其余命令
		if (0 == vcom_device_state)
		{
			//还没有接收到连接命令之前，其余命令都不响应
			return -5;		//未连接状态
		}
		else
		{
			//已经接收到了连接命令，可以响应其余命令
			if (OP_CODE_DISCONNECT == op_code)
			{
				//断开命令
				vcom_device_state = 0;			//接收到了断开命令

				pvalue = tlv_get_tagvalue(TAG_HH_RESULT,&tlvpacket);
				if (pvalue == 0)
				{
					return -6;		//命令数据域缺失
				}
				
				hh_result = pvalue[0];				//会话结果
				res_len = build_ack(op_code,hh_result,0,0,0);
			}
			else if(OP_CODE_INQ_APP_VERSION == op_code || OP_CODE_INQ_PARAM_VERSION == op_code)
			{
				//查询应用版本号 或者 参数版本号
				res_len = build_ack(op_code,0,0,0,0);
			}
			else
			{

				//如下命令必须认证成功才能响应

				if (OP_CODE_INQ_REC_FILE_LIST == op_code)
				{
					//查询记录文件列表
					pvalue	= tlv_get_tagvalue(TAG_REC_TYPE, &tlvpacket);		//获取需要读取的清单记录类型
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}
					rec_type = pvalue[0];
// 					if ((pvalue[0] == REC_TYPE_CHECK_MULTI) || (pvalue[0] == REC_TYPE_SERIAL_LIST))
// 					{
// 						pvalue	= tlv_get_tagvalue(TAG_DJ_DIR, &tlvpacket);		//获取需要读取的单据的相对路径
// 						if (pvalue == 0)
// 						{
// 							return -6;				//命令数据域缺失
// 						}
// 
// 						memcpy(current_dj_dir,pvalue,4);		//设置当前的单据路径
// 					}
					res_len = build_ack(op_code,(unsigned int)rec_type,0,0,0);
				}
				else if (OP_CODE_READ_REC_FILE == op_code)
				{
					//读取记录文件
					pvalue	= tlv_get_tagvalue(TAG_REC_TYPE, &tlvpacket);		//获取需要读取的清单记录类型
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}
					rec_type = pvalue[0];
// 					if ((pvalue[0] == REC_TYPE_CHECK_MULTI) || (pvalue[0] == REC_TYPE_SERIAL_LIST))
// 					{
// 						pvalue	= tlv_get_tagvalue(TAG_DJ_DIR, &tlvpacket);		//获取需要读取的单据的相对路径
// 						if (pvalue == 0)
// 						{
// 							return -6;				//命令数据域缺失
// 						}
// 
// 						memcpy(current_dj_dir,pvalue,4);		//设置当前的单据路径
// 					}

					pvalue	= tlv_get_tagvalue(TAG_FILE_TYPE, &tlvpacket);		//获取需要读取的文件类型
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}

					file_type = pvalue[0];

					pvalue	= tlv_get_tagvalue(TAG_FILE_OFFSET, &tlvpacket);		//获取需要开始读取的文件偏移
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}

					((unsigned char*)&file_offset)[0] = pvalue[3];
					((unsigned char*)&file_offset)[1] = pvalue[2];
					((unsigned char*)&file_offset)[2] = pvalue[1];
					((unsigned char*)&file_offset)[3] = pvalue[0];

					pvalue	= tlv_get_tagvalue(TAG_DATA_LENGTH, &tlvpacket);		//获取需要读取的数据长度
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}

					((unsigned char*)&data_len)[0] = pvalue[3];
					((unsigned char*)&data_len)[1] = pvalue[2];
					((unsigned char*)&data_len)[2] = pvalue[1];
					((unsigned char*)&data_len)[3] = pvalue[0];

					res_len = build_ack(op_code,(unsigned int)rec_type,(unsigned int)file_type,file_offset,data_len);

				}
				else if (OP_CODE_DOWNLOAD_GOODS_INFO == op_code)
				{
					//下载商品信息
					pvalue	= tlv_get_tagvalue(TAG_REC_TYPE, &tlvpacket);		//获取需要下载的清单记录类型
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}
					if (pvalue[0] != REC_TYPE_GOODS_LIST)
					{
						res_len = build_ack(op_code,pvalue[0],1,0,0);		//构造数据域错误的响应报文
					}
					else
					{
						rec_type = pvalue[0];

						pvalue	= tlv_get_tagvalue(TAG_FILE_TYPE, &tlvpacket);		//获取需要写入的文件类型
						if (pvalue == 0)
						{
							return -6;				//命令数据域缺失
						}

						file_type = pvalue[0];
						if ((file_type != FILE_TYPE_GOODS_LIST)&&(file_type != FILE_TYPE_BAR_HASH_TBL)&&(file_type != FILE_TYPE_NAME_HASH_TBL)&&(file_type != FILE_TYPE_CHECK_INF))
						{
							res_len = build_ack(op_code,rec_type,1,(unsigned int)file_type,0);			//构造数据域错误的响应报文
						}
						else
						{
							pvalue	= tlv_get_tagvalue(TAG_FILE_OFFSET,&tlvpacket);		//获取需要写入的文件偏移
							if (pvalue == 0)
							{
								return -6;				//命令数据域缺失
							}

							((unsigned char*)&file_offset)[0] = pvalue[3];
							((unsigned char*)&file_offset)[1] = pvalue[2];
							((unsigned char*)&file_offset)[2] = pvalue[1];
							((unsigned char*)&file_offset)[3] = pvalue[0];

							pvalue	= tlv_get_tagvalue(TAG_DATA_LENGTH,&tlvpacket);		//获取需要写入的数据长度
							if (pvalue == 0)
							{
								return -6;				//命令数据域缺失
							}

							((unsigned char*)&data_len)[0] = pvalue[3];
							((unsigned char*)&data_len)[1] = pvalue[2];
							((unsigned char*)&data_len)[2] = pvalue[1];
							((unsigned char*)&data_len)[3] = pvalue[0];


							pvalue	= tlv_get_tagvalue(TAG_DOWNLOAD_COMPLETE_FLAG, &tlvpacket);		//获取商品信息下载结束标记
							if (pvalue == 0)
							{
								return -6;				//命令数据域缺失
							}
							download_end_flag = pvalue[0];

							pvalue	= tlv_get_tagvalue(TAG_TRNAS_DATA, &tlvpacket);		//获取下载下来的透传数据
							if (pvalue == 0)
							{
								return -6;				//命令数据域缺失
							}
							//将下载下来的透传数据写入相应的文件
							if (0 == download_list_flag)
							{
								download_list_flag = 1;

								//下载商品信息会将
								if (hh_type == TRANS_TYPE_GOODS_INFO_IN)
								{
									record_clear(REC_TYPE_GOODS_LIST);
									record_clear(REC_TYPE_CHECK);
								}
								else if (hh_type == TRANS_TYPE_UPDATE_IN)
								{
									//删除旧的应用升级文件
									del_update_bin();
								}
								else if (hh_type == TRANS_TYPE_RESDATA_IN)
								{
									//删除旧的资源升级文件
									del_resdata_bin();
								}
							}

							//构造目标文件的路径
							build_target_rec_dir(rec_type,file_type,dir_str);

							w_len = write_rec_file(dir_str,file_offset,data_len,pvalue);
							if (w_len < 0)
							{
								res_len = build_ack(op_code,rec_type,2,(unsigned int)file_type,0);			//构造更新失败的响应报文
							}
							else
							{
								//更新成功，检查商品信息下载完成标志是否置位，如果下载完成，需要检查下载的记录格式是否正确
								if (download_end_flag)
								{
									//检查下载的商品信息是否完整，格式是否正确
									if (check_record_dir(REC_TYPE_GOODS_LIST))
									{
										res_len = build_ack(op_code,rec_type,3,(unsigned int)file_type,0);			//构造记录不完整或者格式不正确的响应报文
									}
									else
									{
										res_len = build_ack(op_code,rec_type,0,(unsigned int)file_type,w_len);
									}
								}
								else
								{
									res_len = build_ack(op_code,rec_type,0,(unsigned int)file_type,w_len);
								}
							}
						}
					}
				}
				else if ((OP_CODE_DOWNLOAD_APP == op_code) || (OP_CODE_DOWNLOAD_RESDATA == op_code))
				{
					//下载应用升级文件
					pvalue	= tlv_get_tagvalue(TAG_FILE_TYPE, &tlvpacket);		//获取需要下载的文件类型
					if (pvalue == 0)
					{
						return -6;				//命令数据域缺失
					}
					if (((pvalue[0] != FILE_TYPE_UPDATE_BIN)&&(op_code == OP_CODE_DOWNLOAD_APP))||((pvalue[0] != FILE_TYPE_RESDATA_BIN)&&(op_code == OP_CODE_DOWNLOAD_RESDATA)))
					{
						res_len = build_ack(op_code,(unsigned int)pvalue[0],1,0,0);		//构造数据域错误的响应报文
					}
					else
					{
						file_type = pvalue[0];
						pvalue	= tlv_get_tagvalue(TAG_FILE_OFFSET,&tlvpacket);		//获取需要写入的文件偏移
						if (pvalue == 0)
						{
							return -6;				//命令数据域缺失
						}

						((unsigned char*)&file_offset)[0] = pvalue[3];
						((unsigned char*)&file_offset)[1] = pvalue[2];
						((unsigned char*)&file_offset)[2] = pvalue[1];
						((unsigned char*)&file_offset)[3] = pvalue[0];

						pvalue	= tlv_get_tagvalue(TAG_DATA_LENGTH,&tlvpacket);		//获取需要写入的数据长度
						if (pvalue == 0)
						{
							return -6;				//命令数据域缺失
						}

						((unsigned char*)&data_len)[0] = pvalue[3];
						((unsigned char*)&data_len)[1] = pvalue[2];
						((unsigned char*)&data_len)[2] = pvalue[1];
						((unsigned char*)&data_len)[3] = pvalue[0];


						pvalue	= tlv_get_tagvalue(TAG_DOWNLOAD_COMPLETE_FLAG, &tlvpacket);		//获取商品信息下载结束标记
						if (pvalue == 0)
						{
							return -6;				//命令数据域缺失
						}
						download_end_flag = pvalue[0];

						pvalue	= tlv_get_tagvalue(TAG_TRNAS_DATA, &tlvpacket);		//获取下载下来的透传数据
						if (pvalue == 0)
						{
							return -6;				//命令数据域缺失
						}
						//将下载下来的透传数据写入相应的文件
						if (OP_CODE_DOWNLOAD_APP == op_code)
						{
							w_len = write_rec_file("/update.bin",file_offset,data_len,pvalue);
						}
						else
						{
							w_len = write_rec_file("/resdata.tmp",file_offset,data_len,pvalue);
						}

						if (w_len < 0)
						{
							res_len = build_ack(op_code,(unsigned int)file_type,2,0,0);			//构造更新失败的响应报文
						}
						else
						{
							//更新成功，检查商品信息下载完成标志是否置位，如果下载完成，需要检查下载的记录格式是否正确
							if (download_end_flag)
							{
								//检查下载的文件是否OK，格式是否正确
								if (OP_CODE_DOWNLOAD_APP == op_code)
								{
									res = check_updatefile();
								}
								else
								{
									res = check_resdatafile();
								}

								if (res)
								{
									res_len = build_ack(op_code,(unsigned int)file_type,3,0,0);			//构造记录不完整或者格式不正确的响应报文
								}
								else
								{
									res_len = build_ack(op_code,(unsigned int)file_type,0,0,w_len);
								}
							}
							else
							{
								res_len = build_ack(op_code,(unsigned int)file_type,0,0,w_len);
							}
						}
					}
				}
				else
				{
					//其余操作码，都不响应 
					return -7;
				}
			}
		}
	}

	if ((pcallback)&&(data_trans_state == 0))
	{
		//在回复响应之前需要执行一些回调函数
		pcallback();
	}

	//回复响应
	SendData_To_PC(ack_buffer,res_len);

	if (bDeviceState == CONFIGURED)
	{
		if (op_code == OP_CODE_DISCONNECT)
		{
			return (1+hh_result);		//此次会话结束,1:会话成功		2:会话失败
		}
		else
		{
			return 0;		//此次会话还没有结束
		}
	}
	else
	{
		//与USB连接断开了
		return 0x55aa;
	}
	
}