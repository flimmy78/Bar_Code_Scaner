
#ifndef _RECORD_H_
#define _RECORD_H_


//#define  T5_SD_DEBUG		//开启此宏，打开了利用T卡进行DEBUG的功能	
//#define REC_DEBUG



//文件系统采用绝对路径,由于文件系统关闭了支持长文件名的功能，所以文件名不能超过8个字节
#define rec_file_dirctory			"/T6_DB"					//保存所有记录的文件夹
#define check_dirctory				"/T6_DB/check"				//保存盘点信息记录的子文件夹
#define batch_dirctory				"/T6_DB/batch"				//保存脱机记录的子文件夹
#define goods_list_dirctory			"/T6_DB/goods"				//保存所有商品信息的子文件夹

//定义文件名
#define goods_list_file					"/goods.lst"
#define check_list_file					"/check.lst"
#define barcode_hash_table_file			"/barhash.tbl"
#define name_hash_table_file			"/namehash.tbl"
#define batch_list_file					"/batch.lst"
#define batch_inf_file					"/batch.inf"

//#define zd_rec_file						"/ZD.rec"

#define log_file						"/T6_DB/log.tmp"		//日志文件 --- 断电保护，在一些文件操作之前会先将一些关键信息写入此文件


//定义对记录文件的操作类型
#define OP_TYPE_ADD_NODE			1		//增加一个节点
#define OP_TYPE_CLEAR_NODE			2		//清除所有节点
#define OP_TYPE_DEL_NODE			3		//删除一个节点
#define OP_TYPE_REPLACE_NODE		4		//替换节点
#define OP_TYPE_WRITE_BIN_FILE		5		//写透明二进制文件



#define TAG_INDEX				0x00
#define TAG_BARCODE				0x01
//#define TAG_NAME				0x02



//定义保存记录的SD卡的状态机
#define SD_STATUS_INIT				0		//初始状态，不确定SD是否存在、也不确定文件系统是否挂载的状态
#define SD_STATUS_OPEN_DIR1			1		//打开第一级记录文件夹T5_DB的状态
#define SD_STATUS_OPEN_DIR2			2		//打开第二级记录文件夹的状态
#define SD_STATUS_OPEN_FILE			3		//打开某一个文件的状态

#define GOODS_SPEC_CNT		5			//5个规格

//定义每个保存记录的文件夹中hash表文件的大小（文件实际大小应该是hash表大小*4）
#define HASH_TABLE_SIZE		2048

#define RECORD_MAX_SIZE		512		//最长的记录大小


#pragma pack(1)
/**
* @brief	商品信息的规格,包括商品的名称
* @note 支持条码长度30位，产品名称20位(10个汉字)，商品规格20位(10个汉字)
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4字节	0	B		此份结构的校验值 crc32
	unsigned char			bar_code[31];				// 31字节	4   N	
	unsigned char			name[21];					// 21字节	35	N		商品名称
	unsigned char			spec[GOODS_SPEC_CNT][21];				// 105字节	56  N		商品规格
	unsigned int			by_barcode_prev;			// 4字节	161
	unsigned int			by_barcode_next;			// 4字节	165			//total : 169字节
}TGOODS_SPEC_RECORD;


/**
* @brief	盘点信息记录明细
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4字节	0	B		此份结构的校验值 crc32
	unsigned char			bar_code[31];				// 31字节	4   N		
	unsigned int			check_num;					// 4字节	35   B		盘点数量
	unsigned int			spec_rec_index;				// 4字节    39	B		该商品索引(对应记录区的记录索引)
	unsigned char			check_time[7];				// 7字节	43 BCD		盘点时间
	unsigned char			rfu[6];						// 6字节	50          
	unsigned int			by_barcode_prev;			// 4字节	56
	unsigned int			by_barcode_next;			// 4字节    60
}TCHECK_INFO_RECORD;


/**
* @brief	脱机记录明细
*/
typedef struct  
{
	unsigned int			checkvalue;					// 4字节	0	B		此份结构的校验值 crc32        
	unsigned char			batch_str[66];				// 66字节	4				
	unsigned char			flag;						// 1字节	70          
	unsigned int			by_index_prev;				// 4字节	71
	unsigned int			by_index_next;				// 4字节    75
}TBATCH_NODE;




/**
* @brief	字典的记录
*/
typedef struct  
{
	unsigned int			checkvalue;			//4字节		0	B		校验值
	unsigned char			ZD_name[9];			//9字节		4	N		字典字段名，字符串			
	unsigned char			ZD_value[8][11];	//88字节	13	N		字典内容，字符串			//共101字节			
}TZD_RECORD;


/**
* @brief 节点修改时，需要构造的修改table元素的结构体
*/
typedef struct 
{
	unsigned char	keyspec_type;			//修改的关键字类型
	unsigned int	old_keyspec_hash;		//旧的关键字的hash值
	unsigned int	new_keyspec_hash;		//新的关键字的hash值
}TNODE_MODIFY_INFO;


/**
* @brief 保存log file文件时需要输入的保存信息数据结构
*/
typedef struct 
{
	unsigned int	op_type;			//操作类型
	unsigned int	rec_type;			//记录类型
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


extern unsigned int g_rec_offset;		//返回record_add之后，新增记录的物理偏移
extern unsigned char current_dj_dir[];

//定义各记录数的最大值
#define  GOODS_LIST_MAX_CNT		10000				//商品清单记录最大值
#define  CHECK_LIST_MAX_CNT		10000				//盘点清单记录的最大值
#define  BATCH_LIST_MAX_CNT		10000				//脱机清单记录的最大值

//定义记录类型
#define			REC_TYPE_GOODS_LIST				1			//商品记录				
#define			REC_TYPE_CHECK					2			//盘点记录
#define         REC_TYPE_BATCH					3			//脱机记录

//定义清单类型
#define LIST_TYPE_GOODS				1	//商品清单
#define LIST_TYPE_CHECK				2	//盘点清单


#define ABSOLUTE_SEARCH		0		//绝对搜索
#define RELATIVE_SEARCH		1		//相对搜索

#define SEARCH_MODE_FIRST	0		//搜索第一个
#define SEARCH_MODE_PREV	1		//搜索前一个
#define SEARCH_MODE_NEXT	2		//搜索后一个
				
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

