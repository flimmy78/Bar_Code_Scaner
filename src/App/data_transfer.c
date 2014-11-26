/**
 * @file data_transfer.c
 * @brief �̵����PC����֮�����ݴ���Ӧ�ò��ʵ��
 *
 * @version V0.0.1
 * @author joe
 * @date 2011��05��11��
 * @note 
 *		��ο����̵����PCͨѶ�淶.doc��
 * @copy
 *
 * �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
 * ����˾�������Ŀ����˾����һ��׷��Ȩ����
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
	#define TRANS_DATA_MAX_LEN	2000			//͸�����ݵ���󳤶�
	static unsigned char	g_comm_buf[2048];		//ͨ�õ�buf
	static unsigned char	ack_buffer[2200];
#else
	#define TRANS_DATA_MAX_LEN	1000			//͸�����ݵ���󳤶�
	static unsigned char	g_comm_buf[1024];		//ͨ�õ�buf
	static unsigned char	ack_buffer[1100];
#endif

static unsigned char			need_verify_flag;


extern unsigned int		vcom_device_state;


//static unsigned int		inventory_rec_num;		//����嵥�ļ�¼��
//static unsigned int		check_rec_num;			//�̵��嵥�ļ�¼��
//static unsigned int		serialdata_rec_num;		//���кŲɼ��嵥�ļ�¼��
unsigned int		download_list_flag;		//��ʼ�����嵥�ı��
//static unsigned char	expect_op_code;			//�����������뷢�͵�����

unsigned int			hh_lsh;					//�Ự��ˮ��
unsigned char			hh_type;				//�Ự����
unsigned short			op_code;				//���յ��Ĳ�����
unsigned char			data_trans_state;				//���ݵ��뵼��ʱ��״̬

//extern  TINVENTORY_RECORD			inventory_info;			//��Ʒ�Ŀ����Ϣ
//extern  TGOODS_SPEC_RECORD		goods_spec;				//��Ʒ�Ĺ����Ϣ
//extern	OP_REC_NODE				operate_rec;					//���ݲ�����¼
//extern  unsigned char				current_operator;			// ��ǰ��¼��ϵͳ�Ĳ���Ա
extern  unsigned char				g_usb_type;
extern	TTerminalPara				g_param;							//Terminal Param
//extern  TINVENTORY_RECORD			*p_inventory_info;	//ָ����������Ʒ�����Ϣ��¼
extern  const unsigned char				*app_ver;

/**
* @brief �����¼�ļ��б�
* @param[in] unsigned char  rec_type	��¼����
* @param[out] unsigned char *out_buf	��ż�¼��ϸ����
* @return ���ع���ļ�¼��ϸ���ݵĳ���
* @note
*/
static unsigned int build_rec_file_list(unsigned char rec_type,unsigned char *out_buf)
{
	unsigned int		off = 0;
	int					file_size,i;
	unsigned char		dir_str[35];



	if (REC_TYPE_GOODS_LIST == rec_type)
	{
		//��ȡ��Ʒ��Ϣ��¼�ļ����е��ļ��б�

		out_buf[off++] = 2;						//��Ʒ��Ϣ��¼�ļ��а���2��hash�ļ���1����¼�ļ�����3���ļ�
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
	//	//��ȡ���кŲɼ��嵥��¼���е��ļ��б�
	//	out_buf[off++] = 3;						//���кŲɼ���¼�ļ��а���1��hash�ļ���1�����к���Ϣ�ļ���1����¼�ļ�����3���ļ�
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
	//	//��ȡ�̵���Ϣ��¼�ļ����е��ļ��б�
	//	out_buf[off++] = 2;						//�̵���Ϣ�ļ��а���1��hash�ļ���1����¼�ļ�����2���ļ�

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
	//	//��ȡ�൥���̵���Ϣ��¼�ļ��е��ļ��б�
	//	out_buf[off++] = 2;						//�̵���Ϣ�ļ��а���1��hash�ļ���1����¼�ļ�����2���ļ�

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
	//	//��ȡ�൥���̵��еĵ����б�
	//	out_buf[off++] = 2;						//�൥���̵��ļ��а���1��inf�ļ���1���������ڵ��ļ�����2���ļ�

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
	//	//��ȡ�൥�����к��ļ��еĵ����б�
	//	out_buf[off++] = 2;						//�̵���Ϣ�ļ��а���1��hash�ļ���1����¼�ļ�����2���ļ�

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
		return 0;			//Ӧ�ý���������
	}
	return off;
}

/**
* @brief ����Ŀ���¼�ļ���·��
* @param[in] unsigned char rectype	��¼����
* @param[in] unsigned char filetype �ļ�����
* @param[out] unsigned char *p_dir  ���ع�����ļ�·��
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
* @brief ������Ӧ����
* @param[in] unsigned short			op_code		�������
* @param[in] unsigned int			param1		����1
* @param[in] unsigned int			param2		����2
* @param[in] unsigned int			param3		����3
* @return	������Ӧ���ݵĳ���
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
		//�����������Ӧ����
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tmp[0] = 0x00;
		tmp[1] = 0x02;		//�ն˷�������֡��󳤶�,512
		tmp[2] = 0x00;
		tmp[3] = 0x02;		//�ն˿��Խ��յ�����֡����󳤶�,512
		tmp[4] = 0x00;
		tmp[5] = 0x00;		//����״̬
		tmp[6] = tmp[7] = 0;
		tlv_addtag(&tlvpacket, TAG_IF_FEATURE, tmp);
		tlv_addtag(&tlvpacket, TAG_HH_TYPE, &hh_type);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x0f\x0e" );
	}
	else if (OP_CODE_DISCONNECT == op_code)
	{
		//�Ͽ��������Ӧ����
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tlv_addtag(&tlvpacket, TAG_HH_RESULT, (unsigned char*)&param1);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x0f\x0f" );
	}
	else if (OP_CODE_INQ_APP_VERSION == op_code)
	{
		//��ѯӦ�ð汾�ŵ���Ӧ����
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tlv_addtag(&tlvpacket, TAG_APP_VERSION, (unsigned char*)app_ver);

		Dev_Serial.Dev_Serial_int[0] = *(vu32*)(0x1FFFF7E8);
		Dev_Serial.Dev_Serial_int[1] = *(vu32*)(0x1FFFF7EC);
		Dev_Serial.Dev_Serial_int[2] = *(vu32*)(0x1FFFF7F0);

		tlv_addtag(&tlvpacket, TAG_DEVICE_ID, Dev_Serial.Dev_Serial_byte);		//�豸ID
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x00\x01" );
	}
	else if (OP_CODE_INQ_PARAM_VERSION == op_code)
	{
		//��ѯ�����汾�ŵ���Ӧ����
		if (param1)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "01");		//��֤ʧ��
		}
		else
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "00");
		}
		
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x00\x03" );
	}
	//else if (OP_CODE_VERIFY == op_code)
	//{
	//	//��֤
	//	tlv_addtag(&tlvpacket, TAG_ACK, "00");
	//	tlv_addtag(&tlvpacket, TAG_PARAM_VERSION, "V1.0.0");
	//	tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x00\x02" );
	//}
	else if (OP_CODE_INQ_REC_FILE_LIST == op_code)
	{
		//��ȡ��¼�ļ��б�
		tlv_addtag(&tlvpacket, TAG_ACK, "00");
		tlv_addtag(&tlvpacket, TAG_REC_TYPE, (unsigned char*)&param1);
// 		if (((unsigned char)param1 == REC_TYPE_CHECK_MULTI) || ((unsigned char)param1 == REC_TYPE_SERIAL_LIST))
// 		{
// 			tlv_addtag(&tlvpacket, TAG_DJ_DIR, current_dj_dir);
// 		}
		data_field_len = build_rec_file_list((unsigned char)param1,g_comm_buf);		//�����ļ��б�������
		tlv_addtag_ext(&tlvpacket,TAG_FILE_LIST,g_comm_buf,data_field_len);
		tlv_addtag(&tlvpacket, TAG_OPERATE_CODE,"\x01\x01" );
	}
	else if (OP_CODE_READ_REC_FILE == op_code)
	{
		//��ȡ��¼�ļ�
		build_target_rec_dir((unsigned char)param1,(unsigned char)param2,dir_str);

		if (param4 > TRANS_DATA_MAX_LEN)
		{
			param4 = TRANS_DATA_MAX_LEN;
		}
		r_len = read_rec_file(dir_str,param3,param4,g_comm_buf);
		if (r_len < 0)
		{
			//��ȡʧ��
			tlv_addtag(&tlvpacket, TAG_ACK, "04");		//��¼��ȡʧ��
			tlv_addtag(&tlvpacket, TAG_REC_TYPE, (unsigned char*)&param1);
// 			if (((unsigned char)param1 == REC_TYPE_CHECK_MULTI) || ((unsigned char)param1 == REC_TYPE_SERIAL_LIST))
// 			{
// 				tlv_addtag(&tlvpacket, TAG_DJ_DIR, current_dj_dir);
// 			}
			tlv_addtag(&tlvpacket, TAG_FILE_TYPE, (unsigned char*)&param2);
		}
		else
		{
			//��ȡ�ɹ�
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
		//������Ʒ��Ϣ
		if (param2==1)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "01");		//��������
		}
		else if (param2==2)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "02");		//����ʧ��
		}
		else if (param2==3)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "03");		//��¼���������߸�ʽ����ȷ
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
		//����Ӧ�������ļ�
		if (param2==1)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "01");		//��������
		}
		else if (param2==2)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "02");		//����ʧ��
		}
		else if (param2==3)
		{
			tlv_addtag(&tlvpacket, TAG_ACK, "03");		//��¼���������߸�ʽ����ȷ
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
		;		//Ӧ������������ģ�����
	}

	tlv_pack(&tlvpacket);

	return (tlvpacket.ptr);
}


//���ؿ���嵥֮�󣬽����ص��������ӵ���������¼
//void add_op_record_after_trans_in(TINVENTORY_RECORD *p_inventory)
//{
//
//	//ͨ���µ���Ŀ���嵥,������������¼
//	//@todo....
//
//	strcpy(operate_rec.goods_detail.code.bar_code,p_inventory->bar_code);
//	operate_rec.goods_detail.spec_rec_index = p_inventory->spec_rec_index;
//	operate_rec.goods_detail.num = p_inventory->total_num;
//
//	if(operate_rec.goods_detail.num)
//	{
//		//�������ʱ�������Ϊ0����Ҫ����һ����������¼
//		if (record_add(g_param.current_goods_in_rec,(unsigned char*)&operate_rec))
//		{
//			record_add(g_param.current_goods_in_rec,(unsigned char*)&operate_rec);
//		}
//	}
//}

//������ص��嵥��¼�ĸ�ʽ�Ƿ���ȷ
//static int check_list_rec_format(TINVENTORY_INFO * p_list_rec)
//{
//	int i;
//		 
//	for (i = 0; i < strlen(p_list_rec->bar_code);i++)
//	{
//		if (p_list_rec->bar_code[i] > 0x80)
//		{
//			return -1;		//�������а����˷Ƿ����ַ� 
//		}
//	}
//
//	if (strlen(p_list_rec->name) > 29)
//	{
//		//�ж����һ���ֽ��Ƿ��Ǻ��ֵ�һ�뻹��һ������ʾ�ַ������ֻ���Ӣ���ַ����Ľ����ַ�
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
//	//ʱ���ʽ��У��
//	//@todo...
//
//	return 0;
//}

/**
 * @brief ������յ�������
 * @param[in] unsigned char *cmd			ָ����յ�������
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

	//1.�������֡�ṹ������֡У���Ƿ���ȷ
	res = tlv_verify(cmd);
	if (res)
	{
		return res;		//-1 -2 -3 -4
	}

	tlvpacket.ptr = 0;
	tlvpacket.buffer = cmd;

	//��������ĻỰ��ˮ��
	pvalue	= tlv_get_tagvalue(TAG_HH_LSH, &tlvpacket);
	if (pvalue == 0)
	{
		return -6;				//����������ȱʧ
	}

	hh_lsh =  pvalue[3];
	hh_lsh <<= 8;
	hh_lsh |= pvalue[2];
	hh_lsh <<= 8;
	hh_lsh |= pvalue[1];
	hh_lsh <<= 8;
	hh_lsh |= pvalue[0];

	//��������Ĳ�������
	pvalue	= tlv_get_tagvalue(TAG_OPERATE_CODE, &tlvpacket);
	if (pvalue == 0)
	{
		return -6;				//����������ȱʧ
	}
	op_code = pvalue[0];
	op_code <<= 8;
	op_code |= pvalue[1];


	if (OP_CODE_CONNECT == op_code)
	{
		pvalue = tlv_get_tagvalue(TAG_HH_TYPE,&tlvpacket);
		if (pvalue == 0)
		{
			return -6;		//����������ȱʧ
		}

		if (mode == 0)
		{
			/*if ((pvalue[0] != TRANS_TYPE_GOODS_INFO_OUT)&&(pvalue[0] != TRANS_TYPE_SERIALDATA_OUT)&&
				(pvalue[0] != TRANS_TYPE_CHECK_INFO_OUT)&&
				(pvalue[0] != TRANS_TYPE_GOODS_INFO_IN))*/
			if (pvalue[0] != TRANS_TYPE_GOODS_INFO_IN)
			{
				//�Ự�������������
				return -6;
			}

		}
		else
		{
			if ((pvalue[0] != TRANS_TYPE_UPDATE_IN)&&(pvalue[0] != TRANS_TYPE_RESDATA_IN))
			{
				//�Ự�������������
				return -6;
			}

		}

		hh_type = pvalue[0];			//PCҪ����еĻỰ����

		//����ʱ�̶�������Ӧ��������
		vcom_device_state = 1;			//���յ�����������

		//��ʼ�������嵥�ı��
		download_list_flag = 0;

		//expect_op_code = 0;

		//���������������Ӧ����
		res_len = build_ack(op_code,0,0,0,0);

		need_verify_flag = 0;
	}
	else
	{
		//��������
		if (0 == vcom_device_state)
		{
			//��û�н��յ���������֮ǰ�������������Ӧ
			return -5;		//δ����״̬
		}
		else
		{
			//�Ѿ����յ����������������Ӧ��������
			if (OP_CODE_DISCONNECT == op_code)
			{
				//�Ͽ�����
				vcom_device_state = 0;			//���յ��˶Ͽ�����

				pvalue = tlv_get_tagvalue(TAG_HH_RESULT,&tlvpacket);
				if (pvalue == 0)
				{
					return -6;		//����������ȱʧ
				}
				
				hh_result = pvalue[0];				//�Ự���
				res_len = build_ack(op_code,hh_result,0,0,0);
			}
			else if(OP_CODE_INQ_APP_VERSION == op_code || OP_CODE_INQ_PARAM_VERSION == op_code)
			{
				//��ѯӦ�ð汾�� ���� �����汾��
				res_len = build_ack(op_code,0,0,0,0);
			}
			else
			{

				//�������������֤�ɹ�������Ӧ

				if (OP_CODE_INQ_REC_FILE_LIST == op_code)
				{
					//��ѯ��¼�ļ��б�
					pvalue	= tlv_get_tagvalue(TAG_REC_TYPE, &tlvpacket);		//��ȡ��Ҫ��ȡ���嵥��¼����
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}
					rec_type = pvalue[0];
// 					if ((pvalue[0] == REC_TYPE_CHECK_MULTI) || (pvalue[0] == REC_TYPE_SERIAL_LIST))
// 					{
// 						pvalue	= tlv_get_tagvalue(TAG_DJ_DIR, &tlvpacket);		//��ȡ��Ҫ��ȡ�ĵ��ݵ����·��
// 						if (pvalue == 0)
// 						{
// 							return -6;				//����������ȱʧ
// 						}
// 
// 						memcpy(current_dj_dir,pvalue,4);		//���õ�ǰ�ĵ���·��
// 					}
					res_len = build_ack(op_code,(unsigned int)rec_type,0,0,0);
				}
				else if (OP_CODE_READ_REC_FILE == op_code)
				{
					//��ȡ��¼�ļ�
					pvalue	= tlv_get_tagvalue(TAG_REC_TYPE, &tlvpacket);		//��ȡ��Ҫ��ȡ���嵥��¼����
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}
					rec_type = pvalue[0];
// 					if ((pvalue[0] == REC_TYPE_CHECK_MULTI) || (pvalue[0] == REC_TYPE_SERIAL_LIST))
// 					{
// 						pvalue	= tlv_get_tagvalue(TAG_DJ_DIR, &tlvpacket);		//��ȡ��Ҫ��ȡ�ĵ��ݵ����·��
// 						if (pvalue == 0)
// 						{
// 							return -6;				//����������ȱʧ
// 						}
// 
// 						memcpy(current_dj_dir,pvalue,4);		//���õ�ǰ�ĵ���·��
// 					}

					pvalue	= tlv_get_tagvalue(TAG_FILE_TYPE, &tlvpacket);		//��ȡ��Ҫ��ȡ���ļ�����
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}

					file_type = pvalue[0];

					pvalue	= tlv_get_tagvalue(TAG_FILE_OFFSET, &tlvpacket);		//��ȡ��Ҫ��ʼ��ȡ���ļ�ƫ��
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}

					((unsigned char*)&file_offset)[0] = pvalue[3];
					((unsigned char*)&file_offset)[1] = pvalue[2];
					((unsigned char*)&file_offset)[2] = pvalue[1];
					((unsigned char*)&file_offset)[3] = pvalue[0];

					pvalue	= tlv_get_tagvalue(TAG_DATA_LENGTH, &tlvpacket);		//��ȡ��Ҫ��ȡ�����ݳ���
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}

					((unsigned char*)&data_len)[0] = pvalue[3];
					((unsigned char*)&data_len)[1] = pvalue[2];
					((unsigned char*)&data_len)[2] = pvalue[1];
					((unsigned char*)&data_len)[3] = pvalue[0];

					res_len = build_ack(op_code,(unsigned int)rec_type,(unsigned int)file_type,file_offset,data_len);

				}
				else if (OP_CODE_DOWNLOAD_GOODS_INFO == op_code)
				{
					//������Ʒ��Ϣ
					pvalue	= tlv_get_tagvalue(TAG_REC_TYPE, &tlvpacket);		//��ȡ��Ҫ���ص��嵥��¼����
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}
					if (pvalue[0] != REC_TYPE_GOODS_LIST)
					{
						res_len = build_ack(op_code,pvalue[0],1,0,0);		//����������������Ӧ����
					}
					else
					{
						rec_type = pvalue[0];

						pvalue	= tlv_get_tagvalue(TAG_FILE_TYPE, &tlvpacket);		//��ȡ��Ҫд����ļ�����
						if (pvalue == 0)
						{
							return -6;				//����������ȱʧ
						}

						file_type = pvalue[0];
						if ((file_type != FILE_TYPE_GOODS_LIST)&&(file_type != FILE_TYPE_BAR_HASH_TBL)&&(file_type != FILE_TYPE_NAME_HASH_TBL)&&(file_type != FILE_TYPE_CHECK_INF))
						{
							res_len = build_ack(op_code,rec_type,1,(unsigned int)file_type,0);			//����������������Ӧ����
						}
						else
						{
							pvalue	= tlv_get_tagvalue(TAG_FILE_OFFSET,&tlvpacket);		//��ȡ��Ҫд����ļ�ƫ��
							if (pvalue == 0)
							{
								return -6;				//����������ȱʧ
							}

							((unsigned char*)&file_offset)[0] = pvalue[3];
							((unsigned char*)&file_offset)[1] = pvalue[2];
							((unsigned char*)&file_offset)[2] = pvalue[1];
							((unsigned char*)&file_offset)[3] = pvalue[0];

							pvalue	= tlv_get_tagvalue(TAG_DATA_LENGTH,&tlvpacket);		//��ȡ��Ҫд������ݳ���
							if (pvalue == 0)
							{
								return -6;				//����������ȱʧ
							}

							((unsigned char*)&data_len)[0] = pvalue[3];
							((unsigned char*)&data_len)[1] = pvalue[2];
							((unsigned char*)&data_len)[2] = pvalue[1];
							((unsigned char*)&data_len)[3] = pvalue[0];


							pvalue	= tlv_get_tagvalue(TAG_DOWNLOAD_COMPLETE_FLAG, &tlvpacket);		//��ȡ��Ʒ��Ϣ���ؽ������
							if (pvalue == 0)
							{
								return -6;				//����������ȱʧ
							}
							download_end_flag = pvalue[0];

							pvalue	= tlv_get_tagvalue(TAG_TRNAS_DATA, &tlvpacket);		//��ȡ����������͸������
							if (pvalue == 0)
							{
								return -6;				//����������ȱʧ
							}
							//������������͸������д����Ӧ���ļ�
							if (0 == download_list_flag)
							{
								download_list_flag = 1;

								//������Ʒ��Ϣ�Ὣ
								if (hh_type == TRANS_TYPE_GOODS_INFO_IN)
								{
									record_clear(REC_TYPE_GOODS_LIST);
									record_clear(REC_TYPE_CHECK);
								}
								else if (hh_type == TRANS_TYPE_UPDATE_IN)
								{
									//ɾ���ɵ�Ӧ�������ļ�
									del_update_bin();
								}
								else if (hh_type == TRANS_TYPE_RESDATA_IN)
								{
									//ɾ���ɵ���Դ�����ļ�
									del_resdata_bin();
								}
							}

							//����Ŀ���ļ���·��
							build_target_rec_dir(rec_type,file_type,dir_str);

							w_len = write_rec_file(dir_str,file_offset,data_len,pvalue);
							if (w_len < 0)
							{
								res_len = build_ack(op_code,rec_type,2,(unsigned int)file_type,0);			//�������ʧ�ܵ���Ӧ����
							}
							else
							{
								//���³ɹ��������Ʒ��Ϣ������ɱ�־�Ƿ���λ�����������ɣ���Ҫ������صļ�¼��ʽ�Ƿ���ȷ
								if (download_end_flag)
								{
									//������ص���Ʒ��Ϣ�Ƿ���������ʽ�Ƿ���ȷ
									if (check_record_dir(REC_TYPE_GOODS_LIST))
									{
										res_len = build_ack(op_code,rec_type,3,(unsigned int)file_type,0);			//�����¼���������߸�ʽ����ȷ����Ӧ����
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
					//����Ӧ�������ļ�
					pvalue	= tlv_get_tagvalue(TAG_FILE_TYPE, &tlvpacket);		//��ȡ��Ҫ���ص��ļ�����
					if (pvalue == 0)
					{
						return -6;				//����������ȱʧ
					}
					if (((pvalue[0] != FILE_TYPE_UPDATE_BIN)&&(op_code == OP_CODE_DOWNLOAD_APP))||((pvalue[0] != FILE_TYPE_RESDATA_BIN)&&(op_code == OP_CODE_DOWNLOAD_RESDATA)))
					{
						res_len = build_ack(op_code,(unsigned int)pvalue[0],1,0,0);		//����������������Ӧ����
					}
					else
					{
						file_type = pvalue[0];
						pvalue	= tlv_get_tagvalue(TAG_FILE_OFFSET,&tlvpacket);		//��ȡ��Ҫд����ļ�ƫ��
						if (pvalue == 0)
						{
							return -6;				//����������ȱʧ
						}

						((unsigned char*)&file_offset)[0] = pvalue[3];
						((unsigned char*)&file_offset)[1] = pvalue[2];
						((unsigned char*)&file_offset)[2] = pvalue[1];
						((unsigned char*)&file_offset)[3] = pvalue[0];

						pvalue	= tlv_get_tagvalue(TAG_DATA_LENGTH,&tlvpacket);		//��ȡ��Ҫд������ݳ���
						if (pvalue == 0)
						{
							return -6;				//����������ȱʧ
						}

						((unsigned char*)&data_len)[0] = pvalue[3];
						((unsigned char*)&data_len)[1] = pvalue[2];
						((unsigned char*)&data_len)[2] = pvalue[1];
						((unsigned char*)&data_len)[3] = pvalue[0];


						pvalue	= tlv_get_tagvalue(TAG_DOWNLOAD_COMPLETE_FLAG, &tlvpacket);		//��ȡ��Ʒ��Ϣ���ؽ������
						if (pvalue == 0)
						{
							return -6;				//����������ȱʧ
						}
						download_end_flag = pvalue[0];

						pvalue	= tlv_get_tagvalue(TAG_TRNAS_DATA, &tlvpacket);		//��ȡ����������͸������
						if (pvalue == 0)
						{
							return -6;				//����������ȱʧ
						}
						//������������͸������д����Ӧ���ļ�
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
							res_len = build_ack(op_code,(unsigned int)file_type,2,0,0);			//�������ʧ�ܵ���Ӧ����
						}
						else
						{
							//���³ɹ��������Ʒ��Ϣ������ɱ�־�Ƿ���λ�����������ɣ���Ҫ������صļ�¼��ʽ�Ƿ���ȷ
							if (download_end_flag)
							{
								//������ص��ļ��Ƿ�OK����ʽ�Ƿ���ȷ
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
									res_len = build_ack(op_code,(unsigned int)file_type,3,0,0);			//�����¼���������߸�ʽ����ȷ����Ӧ����
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
					//��������룬������Ӧ 
					return -7;
				}
			}
		}
	}

	if ((pcallback)&&(data_trans_state == 0))
	{
		//�ڻظ���Ӧ֮ǰ��Ҫִ��һЩ�ص�����
		pcallback();
	}

	//�ظ���Ӧ
	SendData_To_PC(ack_buffer,res_len);

	if (bDeviceState == CONFIGURED)
	{
		if (op_code == OP_CODE_DISCONNECT)
		{
			return (1+hh_result);		//�˴λỰ����,1:�Ự�ɹ�		2:�Ựʧ��
		}
		else
		{
			return 0;		//�˴λỰ��û�н���
		}
	}
	else
	{
		//��USB���ӶϿ���
		return 0x55aa;
	}
	
}