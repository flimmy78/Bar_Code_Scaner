
#ifndef _RECORD_H_
#define _RECORD_H_


//#define  T5_SD_DEBUG		//�����˺꣬��������T������DEBUG�Ĺ���	
//#define REC_DEBUG



//�ļ�ϵͳ���þ���·��,�����ļ�ϵͳ�ر���֧�ֳ��ļ����Ĺ��ܣ������ļ������ܳ���8���ֽ�
#define rec_file_dirctory			"/T6_DB"					//�������м�¼���ļ���
#define check_dirctory				"/T6_DB/check"				//�����̵���Ϣ��¼�����ļ���
#define batch_dirctory				"/T6_DB/batch"				//�����ѻ���¼�����ļ���
#define goods_list_dirctory			"/T6_DB/goods"				//����������Ʒ��Ϣ�����ļ���

//�����ļ���
#define goods_list_file					"/goods.lst"
#define check_list_file					"/check.lst"
#define barcode_hash_table_file			"/barhash.tbl"
#define name_hash_table_file			"/namehash.tbl"
#define batch_list_file					"/batch.lst"
#define batch_inf_file					"/batch.inf"

//#define zd_rec_file						"/ZD.rec"

#define log_file						"/T6_DB/log.tmp"		//��־�ļ� --- �ϵ籣������һЩ�ļ�����֮ǰ���Ƚ�һЩ�ؼ���Ϣд����ļ�


//����Լ�¼�ļ��Ĳ�������
#define OP_TYPE_ADD_NODE			1		//����һ���ڵ�
#define OP_TYPE_CLEAR_NODE			2		//������нڵ�
#define OP_TYPE_DEL_NODE			3		//ɾ��һ���ڵ�
#define OP_TYPE_REPLACE_NODE		4		//�滻�ڵ�
#define OP_TYPE_WRITE_BIN_FILE		5		//д͸���������ļ�



#define TAG_INDEX				0x00
#define TAG_BARCODE				0x01
//#define TAG_NAME				0x02



//���屣���¼��SD����״̬��
#define SD_STATUS_INIT				0		//��ʼ״̬����ȷ��SD�Ƿ���ڡ�Ҳ��ȷ���ļ�ϵͳ�Ƿ���ص�״̬
#define SD_STATUS_OPEN_DIR1			1		//�򿪵�һ����¼�ļ���T5_DB��״̬
#define SD_STATUS_OPEN_DIR2			2		//�򿪵ڶ�����¼�ļ��е�״̬
#define SD_STATUS_OPEN_FILE			3		//��ĳһ���ļ���״̬

#define GOODS_SPEC_CNT		5			//5�����

//����ÿ�������¼���ļ�����hash���ļ��Ĵ�С���ļ�ʵ�ʴ�СӦ����hash���С*4��
#define HASH_TABLE_SIZE		2048

#define RECORD_MAX_SIZE		512		//��ļ�¼��С


#pragma pack(1)
/**
* @brief	��Ʒ��Ϣ�Ĺ��,������Ʒ������
* @note ֧�����볤��30λ����Ʒ����20λ(10������)����Ʒ���20λ(10������)
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4�ֽ�	0	B		�˷ݽṹ��У��ֵ crc32
	unsigned char			bar_code[31];				// 31�ֽ�	4   N	
	unsigned char			name[21];					// 21�ֽ�	35	N		��Ʒ����
	unsigned char			spec[GOODS_SPEC_CNT][21];				// 105�ֽ�	56  N		��Ʒ���
	unsigned int			by_barcode_prev;			// 4�ֽ�	161
	unsigned int			by_barcode_next;			// 4�ֽ�	165			//total : 169�ֽ�
}TGOODS_SPEC_RECORD;


/**
* @brief	�̵���Ϣ��¼��ϸ
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4�ֽ�	0	B		�˷ݽṹ��У��ֵ crc32
	unsigned char			bar_code[31];				// 31�ֽ�	4   N		
	unsigned int			check_num;					// 4�ֽ�	35   B		�̵�����
	unsigned int			spec_rec_index;				// 4�ֽ�    39	B		����Ʒ����(��Ӧ��¼���ļ�¼����)
	unsigned char			check_time[7];				// 7�ֽ�	43 BCD		�̵�ʱ��
	unsigned char			rfu[6];						// 6�ֽ�	50          
	unsigned int			by_barcode_prev;			// 4�ֽ�	56
	unsigned int			by_barcode_next;			// 4�ֽ�    60
}TCHECK_INFO_RECORD;


/**
* @brief	�ѻ���¼��ϸ
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4�ֽ�	0	B		�˷ݽṹ��У��ֵ crc32        
	unsigned char			batch_str[66];				// 66�ֽ�	4				
	unsigned char			flag;						// 1�ֽ�	70          
	unsigned int			by_index_prev;				// 4�ֽ�	71
	unsigned int			by_index_next;				// 4�ֽ�    75
}TBATCH_NODE;




/**
* @brief	�ֵ�ļ�¼
*/
typedef struct  
{
	unsigned int			checkvalue;			//4�ֽ�		0	B		У��ֵ
	unsigned char			ZD_name[9];			//9�ֽ�		4	N		�ֵ��ֶ������ַ���			
	unsigned char			ZD_value[8][11];	//88�ֽ�	13	N		�ֵ����ݣ��ַ���			//��101�ֽ�			
}TZD_RECORD;


/**
* @brief �ڵ��޸�ʱ����Ҫ������޸�tableԪ�صĽṹ��
*/
typedef struct 
{
	unsigned char	keyspec_type;			//�޸ĵĹؼ�������
	unsigned int	old_keyspec_hash;		//�ɵĹؼ��ֵ�hashֵ
	unsigned int	new_keyspec_hash;		//�µĹؼ��ֵ�hashֵ
}TNODE_MODIFY_INFO;


/**
* @brief ����log file�ļ�ʱ��Ҫ����ı�����Ϣ���ݽṹ
*/
typedef struct 
{
	unsigned int	op_type;			//��������
	unsigned int	rec_type;			//��¼����
	unsigned int	param1;
	unsigned int	param2;
	void			*param3;
	unsigned int	param4;
	void			*param5;
}TLOG_FILE_PARAM;

typedef struct t_danju_context
{
	unsigned char	current_offset;
	unsigned char	comm_switch;
}DJ_CONTEXT;

#pragma pack()


extern unsigned int g_rec_offset;		//����record_add֮��������¼������ƫ��
extern unsigned char current_dj_dir[];

//�������¼�������ֵ
#define  GOODS_LIST_MAX_CNT		10000				//��Ʒ�嵥��¼���ֵ
#define  CHECK_LIST_MAX_CNT		10000				//�̵��嵥��¼�����ֵ
#define  BATCH_LIST_MAX_CNT		10000				//�ѻ��嵥��¼�����ֵ

//�����¼����
#define			REC_TYPE_GOODS_LIST				1			//��Ʒ��¼				
#define			REC_TYPE_CHECK					2			//�̵��¼
#define         REC_TYPE_BATCH					3			//�ѻ���¼

//�����嵥����
#define LIST_TYPE_GOODS				1	//��Ʒ�嵥
#define LIST_TYPE_CHECK				2	//�̵��嵥


#define ABSOLUTE_SEARCH		0		//��������
#define RELATIVE_SEARCH		1		//�������

#define SEARCH_MODE_FIRST	0		//������һ��
#define SEARCH_MODE_PREV	1		//����ǰһ��
#define SEARCH_MODE_NEXT	2		//������һ��
				
int record_module_init(void);
unsigned char *record_module_read(unsigned char rectype, unsigned int index);
int record_add(unsigned char rectype,unsigned char *precord);
int record_add_ext(unsigned char rectype,unsigned char *precord,unsigned char open_mode,unsigned char close_mode);
int record_module_count(unsigned char rectype);
int record_clear(unsigned char rectype);
unsigned char *rec_searchby_tag(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag,int *index);


int record_module_replace(unsigned char rectype,int index,unsigned char *p_new_record,TNODE_MODIFY_INFO *modify_info_tbl);
unsigned char *get_goodsnode(int index);
unsigned char *rec_searchby_tag_ext(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag, unsigned int start,unsigned char direct,int *index);
int delete_one_node(unsigned char rectype,unsigned int index);
unsigned char* get_node(unsigned char rectype,unsigned int mode,unsigned int offset);
int get_file_size(const unsigned char *dir);
int read_rec_file(const unsigned char *dir,unsigned int offset,unsigned int len,unsigned char *pdata);
int write_rec_file(const unsigned char *dir,unsigned int offset,unsigned int len,unsigned char *pdata);
int check_record_dir(unsigned char rectype);
int check_updatefile(void);
int check_resdatafile(void);
int del_resdata_bin(void);
int del_update_bin(void);
int recover_record_by_logfile(void);
unsigned int serail_data_PA_to_LA(unsigned int physical_index);
int clear_log_file(int mode);
int check_serial_list_file(void);
unsigned char danju_search(unsigned char mode,unsigned char* key_str,unsigned char *result_offset_buf);
unsigned char *rec_serachby_name(unsigned char rec_type,unsigned char *intag,unsigned int *r_index);
int check_database(void);

#ifdef T5_SD_DEBUG 
void debug_out(unsigned char* out,unsigned int len,unsigned char format);
void delete_debug_file(void);
void close_debug_file(void);

extern unsigned int debug_file_status;
#endif

#endif

