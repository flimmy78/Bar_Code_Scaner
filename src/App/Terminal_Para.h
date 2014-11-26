/**
*  @file 	Terminal_Para.h
*  @brief  	�������ն���Ҫ������NVM����Ĳ��������ݽṹ
*  @note	
*/

#ifndef _TERMINAL_PARA_H_
#define _TERMINAL_PARA_H_

//ɨ��Ĵ�����ʽ
#define SCAN_TRIG_MODE_ONESHOT		1		//���δ���
#define SCAN_TRIG_MODE_KEEP			2		//��������
#define SCAN_TRIG_MODE_CONTINUE		3		//����ɨ��

//����¼�뷽ʽ
#define QUANTITY_ENTER_MODE_DISABLE		0		//��ֹ¼������
#define QUANTITY_ENTER_MODE_REPEAT		1		//�ظ��������
#define QUANTITY_ENTER_MODE_NUM			2		//�������

//���䷽ʽ�Ķ���
#define TRANSFER_MODE_BLUETOOTH		1
#define TRANSFER_MODE_KEYBOARD		2
#define TRANSFER_MODE_VIRTUAL_COMM	3
#define TRANSFER_MODE_U_DISK		4
#define TRANSFER_MODE_BATCH			5

/**
 *	�Ա�ģ���ж���Ĳ�����ÿ����������������������¼�Ҫ˵����
 *			B��		�����ƣ�������Ʊ�ʾ��ʵ�������Ƕ��پ��Ƕ���
 *			N��		��ֵ���ҿ�����λ��Ч����ǰ���㣬����ʾ��������2λΪ�Ƿ֣�ASCII�����BCD��
 *			ANS:	��ĸ�����ֻ�������ţ��󿿣��Ҷ��ಿ����ո�ASCII��
 */

/**
*@brief ������ͨ����Ա�Ľṹ������
*/
typedef struct  {
	unsigned char	id[2];				//���			N
	unsigned char	pwd[4];				//4λ����		N
	unsigned char	name[9];			//����			N
}TOperator;

/**
*@brief ����洢��SPI FLASH�ڲ��ն˲����Ľṹ����
*@ note	
*/
#pragma pack(1)

typedef struct  {
	unsigned int			checkvalue;					//4�ֽ�		0	B	�˷ݽṹ��У��ֵ crc32			
	unsigned char			struct_ver;					//1�ֽ�		4	B	�����汾�ţ���ʶ�˽ṹ�İ汾
	unsigned char			operate_timeout;			//1�ֽ�		5   B	������ʱʱ�� 0:�ر�  1:30S  2:1����  3:2����  4:5����
	unsigned char			language;					//1�ֽ�		6	B   ϵͳʹ�õ���������
	
	unsigned short			decoder_switch_map;			//2�ֽ�		7   B	����������

	unsigned char			valid_barcode_start_offset;	//1�ֽ�		9	B	��Ч¼����������ʼλ
	unsigned char			valid_barcode_end_offset;	//1�ֽ�		10	B	��Ч¼�������Ľ���λ

	unsigned char			scan_trig_mode;				//1�ֽ�		11  B	ɨ�谴��������ʽ 1:���ΰ�������  2:��������  3:����ɨ��

	unsigned char			quantity_enter_mode;		//1�ֽ�		12  B   ����¼�뷽ʽ  0:�ر�����¼��  1: �ظ��������  2:�����������

	unsigned char			add_time_option;			//1�ֽ�		13  B	�Ƿ����ɨ��ʱ��	0:�����  1:���
	unsigned char			add_date_option;			//1�ֽ�		14  B	�Ƿ����ɨ������	0:�����  1:���
	
	unsigned char			database_query_option;		//1�ֽ�		15  B	���ݿ��ѯ�Ƿ���	0:�ر�  1:����

	unsigned char			avoid_dunplication_option;	//1�ֽ�		16  B	�����빦���Ƿ���	0:�ر�  1:����

	unsigned char			barcode_statistic_option;	//1�ֽ�		17  B	����ͳ�ƹ����Ƿ���	0:�ر�  1:����

	unsigned char			transfer_confirm_option;	//1�ֽ�		18  B	����ȷ�Ϲ����Ƿ���	0:�ر�  1:����

	unsigned char			transfer_mode;				//1�ֽ�		19	B	���ݴ��䷽ʽ��ѡ��	1;����  2:USB����  3:USB���⴮�� 4:U��ģʽ 5:�ѻ����淽ʽ

	unsigned char			batch_transfer_mode;		//1�ֽ�		20	B	�ѻ����ݵĴ��䷽ʽ  1:����  2:USB����  3:USB���⴮��

	unsigned char			system_psw[8];				//8�ֽ�		21	B	ϵͳ����

	unsigned char			beeper_vol;					//1�ֽ�     29	B	����������  0:��  1:��
	
	unsigned char			bluetooth_transfer_option;	//1�ֽ�		30	B	��������ģʽѡ��	0: �ر��������ݴ洢   1:�����������ݴ洢

	unsigned char			u_disk_storage_file[14];	//14�ֽ�	31  N	U�̴���ģʽʱ�洢ɨ�����ݵ��ļ�

	unsigned char			batch_transfer_statistics;	//1�ֽ�		45  B	�ѻ����ݴ���ʱ������ͳ�ƹ���ѡ��  0:�ر�  1:����

	unsigned char			transfer_seperator[3];		//3�ֽ�		46  B	���ݴ���ʱ�����ݷָ���

	//unsigned char			rfu[13];						//13�ֽ� 49 B		
	unsigned char			endtag[2];					//0x55,0xAA  62      һ��64�ֽ�
} TTerminalPara;
#pragma pack()

int ReadTerminalPara(void);
int SaveTerminalPara(void);
int DefaultTerminalPara(void);
#endif
