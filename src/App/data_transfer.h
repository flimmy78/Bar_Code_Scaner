#ifndef _DATA_TRANSFER_H_
#define _DATA_TRANSFER_H_

#include "record.h"


//�ļ����͵ĺ궨��
#define FILE_TYPE_GOODS_LIST		0x01			//��Ʒ��Ϣ�Ľڵ��ļ�
#define FILE_TYPE_BAR_HASH_TBL		0x02			//�����������HASH�����HASH���ļ�
#define FILE_TYPE_NAME_HASH_TBL		0x03			//�������ƽ���HASH�����HASH���ļ�
#define FILE_TYPE_CHECK_INF			0x04			//�̵���Ϣ�Ľڵ��ļ�
#define FILE_TYPE_SERIAL_INF		0x05			//���к���Ϣ�ڵ��ļ��б�ɾ���Ľڵ���Ϣ�ļ�
#define FILE_TYPE_SERIAL_LIST		0x06			//���к���Ϣ�Ľڵ��ļ�
//#define FILE_TYPE_CHK_EXT_INF		0x07			//���̵���Ϣ�Ľڵ��ļ�
#define FILE_TYPE_MULTCHK_INF		0x08			//�̵㵥���б��ļ��б�ɾ���ڵ����Ϣ�ļ�
#define FILE_TYPE_MULTCHK_LST		0x09			//�̵㵥���б�Ľڵ��ļ�
#define FILE_TYPE_MULTXLH_INF		0x0a			//���кŵ����б��ļ��б�ɾ���ڵ����Ϣ�ļ�
#define FILE_TYPE_MULTXLH_LST		0x0b			//���кŵ����б�Ľڵ��ļ�

#define FILE_TYPE_UPDATE_BIN		0x11			//Ӧ�������ļ�
#define FILE_TYPE_RESDATA_BIN		0x12			//��Դ�ļ�


//��������ĺ궨��
#define OP_CODE_CONNECT						0x0f0e			//����
#define OP_CODE_CONNECT2					0x8f8e			//����2��API ��ʽ���ʣ�Ϊ������ɰ汾�Ĺ��ߣ�
#define OP_CODE_DISCONNECT					0x0f0f			//�Ͽ�
#define OP_CODE_INQ_APP_VERSION				0x0001			//��ѯӦ�ð汾��
#define OP_CODE_INQ_PARAM_VERSION			0x0002			//��ѯ�����汾��
#define OP_CODE_VERIFY						0x0003			//��֤
#define OP_CODE_INQ_REC_FILE_LIST			0x0101			//��ѯ��¼���ļ��б�
#define OP_CODE_READ_REC_FILE				0x0201			//��ȡ��¼�ļ�
#define OP_CODE_DOWNLOAD_GOODS_INFO			0x0301			//������Ʒ��Ϣ
#define OP_CODE_DOWNLOAD_APP				0x0401			//����Ӧ�������ļ�
#define OP_CODE_DOWNLOAD_RESDATA			0x0402			//������Դ�����ļ�


//�Ự����
#define TRANS_TYPE_GOODS_INFO_OUT		0x01		//������Ʒ��Ϣ
#define TRANS_TYPE_CHECK_INFO_OUT		0x02		//�����̵���Ϣ
#define TRANS_TYPE_SERIALDATA_OUT		0x03		//�������кŲɼ��嵥
//#define TRANS_TYPE_CHECK_EXT_INFO_OUT	0x05		//�������̵���Ϣ

#define TRANS_TYPE_GOODS_INFO_IN		0x11		//������Ʒ��Ϣ
#define TRANS_TYPE_UPDATE_IN			0x21		//����Ӧ�������ļ�
#define TRANS_TYPE_RESDATA_IN			0x22		//������Դ�����ļ�

#pragma pack(1)
#if 0
/**
* @brief	���ػ����ϴ��Ŀ����Ʒ���ݽṹ
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4�ֽ�	0	B		�˷ݽṹ��У��ֵ crc32
	int						total_num;					// 4�ֽ�	4   B		�������
	unsigned char			bar_code[21];				// 21�ֽ�	8	N		��Ʒ������
	unsigned char			name[31];					// 31�ֽ�	29	N		��Ʒ����
	unsigned char			spec[GOODS_SPEC_CNT][22];	// 132�ֽ�	60  N		��Ʒ���
	unsigned char			last_in_datetime[7];		// 7�ֽ�	192 BCD		������ʱ��
	unsigned char			last_out_datetime[7];		// 7�ֽ�	199 BCD		�������ʱ��		//��206�ֽ�
}TINVENTORY_INFO;

/**
* @brief	���ػ����ϴ����̵���Ʒ���ݽṹ
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4�ֽ�	0	B		�˷ݽṹ��У��ֵ crc32
	int						check_num;					// 4�ֽ�	4   B		�̵�����
	unsigned char			bar_code[21];				// 21�ֽ�	8	N		��Ʒ������
	unsigned char			name[31];					// 31�ֽ�	29	N		��Ʒ����
	unsigned char			spec[GOODS_SPEC_CNT][22];	// 132�ֽ�  60  N		����ֶ���(8�ֽ�):�������(12�ֽ�)	
	unsigned char			last_check_datetime[7];		// 7�ֽ�	192 BCD		������ʱ��
	unsigned char			last_check_oprator_num[2];	// 2�ֽ�	199 BCD		����̵����Ա����
	unsigned char			rfu[5];						// 5�ֽ�	201	B		������				�ճ�206�ֽڣ���֤�Ϳ���嵥�ļ�¼һ����С
}TCHECK_INFO;

/**
* @brief	�ϴ�����̨ʱ��Ʒ��ϸ�����ݽṹ
*/
typedef struct  
{
	unsigned int			num;						// 4�ֽ�	0	B		���������
	unsigned int			auto_a_d_num;				// 4�ֽ�	4	B		�Զ��۳������Զ����ӵ�����
	unsigned char			name[31];					// 31�ֽ�	8	N		��Ʒ����
	unsigned char			bar_code[21];				// 21�ֽ�   39   N		��Ʒ����   //��60�ֽ�
}TGOODS_DETAIL;

/**
* @brief	�ϴ�����̨ʱ������¼�����ݽṹ
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4�ֽ�	0	B		�˷ݽṹ��У��ֵ crc32
	unsigned char			operator_num[2];			// 2�ֽ�	4	N		����Ա����
	unsigned char			operator_name[8];			// 8�ֽ�	6   N		����Ա����
	unsigned char			datetime[7];				// 7�ֽ�	14	BCD		�����ʱ��
	TGOODS_DETAIL			goods_detail;				// 60�ֽ�	21			������Ʒ��Ϣ����Ʒ���������������
	unsigned int			orignal_num;				// 4�ֽ�	81	B		���һ���޸�֮ǰ��ֵ,�����ֵ��Ϊ0�������˲�����¼���޸Ĺ�
	unsigned char			trans_in_flag;				// 1�ֽ�	85	B		���ݵ���ı�ǣ������ļ�¼�ǵ����嵥ʱ���ӵ�			//��86�ֽ�
}TGOODS_OP_INFO;

#pragma pack()
#endif

extern unsigned char			hh_type;				//�Ự����
extern unsigned short			op_code;				//���յ��Ĳ����� 	

typedef void (*datatrans_callback)(void);

void usb_device_init(unsigned char device_type);
int data_transfer_proc(unsigned char *cmd,int mode,datatrans_callback pcallback);
//void add_op_record_after_trans_in(TINVENTORY_RECORD *p_inventory);

#endif