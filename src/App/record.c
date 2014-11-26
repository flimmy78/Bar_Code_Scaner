/**
 * @file record.c
 * @brief T1Gen项目交易记录管理模块
 * @version 2.0
 * @author joe
 * @date 2011年12月20日
 * @note 利用T卡保存记录
 *
 * @version 1.0
 * @author joe
 * @date 2011年03月30日
 * @note 利用通用的记录管理模块实现的
 * @copy
 *
 * 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
 * 本公司以外的项目。本司保留一切追究权利。
 *
 * <h1><center>&copy; COPYRIGHT 2011 netcom</center></h1>
 */

#include "record.h"
#include "crc32.h"
#include <string.h>
#include "data_transfer.h"
#include "TimeBase.h"
#include "calendar.h"
#include "Terminal_Para.h"
#include "ff.h"
#include "hash.h"
#include "res_upgrade.h"	
#include "JMemory.h"

unsigned char			rec_module_buffer[RECORD_MAX_SIZE];	//记录模块的公用buffer	


unsigned int current_node_offset;
unsigned int g_rec_offset;

static unsigned int	prev_node_offset;
static unsigned int	next_node_offset;


DJ_CONTEXT danju_context;

static unsigned int log_len;

FIL						file2,file3;
DIR						dir;							//文件夹


#ifdef T5_SD_DEBUG
FIL						debug_file;
unsigned int			debug_file_status;
#endif

extern FIL				file1;
extern FATFS			fs;
extern TTerminalPara	g_param;


/**
* @brief 初始化哈希表文件
*/
static int init_hash_table_file(FIL* file)
{
	unsigned char	buffer[512];
	unsigned int	r_w_byte_cnt,i;

	f_lseek(file,0);
	memset(buffer,0,512);
	for (i = 0; i< ((HASH_TABLE_SIZE*4)/512);i++)
	{
		if (f_write(file,buffer,512,&r_w_byte_cnt) != FR_OK)
		{
			return -1;
		}

		if (r_w_byte_cnt != 512)
		{
			return -1;
		}
	}

	if (f_truncate(file) != FR_OK)
	{
		return -1;
	}

	f_sync(file);

	return FR_OK;
}

/**
* @brief 初始化序列号信息文件
*/
static int init_data_info_file(FIL* file)
{
	unsigned char	buffer[12];
	unsigned int	r_w_byte_cnt,i;

	f_lseek(file,0);
	memcpy(buffer,"info",4);
	memset(buffer+4,0,8);
	

	if (f_write(file,buffer,12,&r_w_byte_cnt) != FR_OK)
	{
		return -1;
	}

	if (r_w_byte_cnt != 12)
	{
		return -1;
	}

	if (f_truncate(file) != FR_OK)
	{
		return -1;
	}

	f_sync(file);

	return FR_OK;
}

/**
 * @brief 系统的所有记录模块的初始化
 * @return 0: OK   else: 错误代码
 * @note: 返回值不能很好的定位到具体的错误发生的位置，后续有需要再修改
 */
int record_module_init(void)
{
	const unsigned char *tmp_dir[3];
	unsigned int		i,j;	
	unsigned char		dir_str[35];
	const unsigned char	*p_hash_table_file[3];

	//f_mount(0, &fs);										// 装载文件系统

	if( f_opendir(&dir,rec_file_dirctory) != FR_OK )
	{
		//打开记录文件失败或者无法访问SD卡,如果是该文件夹不存在，那么就创建一个新的文件夹
		if (f_mkdir(rec_file_dirctory) != FR_OK)
		{
			//无法访问SD卡
			return 1;	
		}

		if( f_opendir(&dir,rec_file_dirctory) != FR_OK )
		{
			//刚创建成功了还打不开，那就诡异了（会出现这个错误吗？？？）
			return 1;
		}
	}
	
	//到此为止，进入了T5_DB文件夹，检查保存各种记录的相应文件夹是否都存在
	
	tmp_dir[0] = goods_list_dirctory;
	tmp_dir[1] = check_dirctory;
	tmp_dir[2] = batch_dirctory;


	for (i = 0; i < 3;i++)
	{
		if( f_opendir(&dir,tmp_dir[i]) != FR_OK )
		{
			//如果是该文件夹不存在，那么就创建一个新的文件夹
			if (f_mkdir(tmp_dir[i]) != FR_OK)
			{
				//无法访问SD卡
				return 1;	
			}
		}

		if(i == 0 || i == 1)
		{
			//保存盘点信息的文件夹需要建立一个hash表文件
			p_hash_table_file[0] = barcode_hash_table_file;
			p_hash_table_file[1] = 0;
		}
		else
		{
			//保存脱机记录的文件夹需要建立一个INF文件
			p_hash_table_file[0] = batch_inf_file;
			p_hash_table_file[1] = 0;
		}

		j = 0;
		while (p_hash_table_file[j])
		{
			strcpy(dir_str,tmp_dir[i]);
			strcat(dir_str,p_hash_table_file[j]);

			if (f_open(&file1,(const char*)dir_str,FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
			{
				return 2;
			}

			if (i == 0 || i == 1)
			{
				if (file1.fsize != HASH_TABLE_SIZE*4)
				{
					if (init_hash_table_file(&file1) != FR_OK)
					{
						return 3;
					}
				}
			}

			if (i == 2)
			{
				if ((file1.fsize < 12)||(file1.fsize > 12 + 4*RECORD_MAX_SIZE))
				{
					if (init_data_info_file(&file1) != FR_OK)
					{
						return 3;
					}
				}
			}

			f_sync(&file1);
			j++;
		}
	}
	
	current_node_offset = 0;
	return 0;
}

/**
 * @brief 读指定记录，该记录必须存在，此函数中不作判断
 * @param[in] unsigned char rectype	 记录的类型
 * @param[in] int index 记录的索引(最老的记录的索引号是1)
 * @return 返回记录的地址
 */
unsigned char *record_module_read(unsigned char rectype, unsigned int index)
{
	unsigned char	dir_str[35];
	unsigned int	node_size;
	unsigned char	*pBuf;
	unsigned int	checkvalue;
	int		re_read_cnt = 3;		//如果一次读取失败，那么重读3次
	int		ret;

	if (index == 0)
	{
		return (unsigned char*)0;
	}
	
	pBuf = rec_module_buffer;
	if (REC_TYPE_GOODS_LIST == rectype)
	{
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,goods_list_file);
		node_size = sizeof(TGOODS_SPEC_RECORD);
	}
	else if (REC_TYPE_CHECK == rectype)
	{
		strcpy(dir_str,check_dirctory);
		strcat(dir_str,check_list_file);
		node_size = sizeof(TCHECK_INFO_RECORD);
	}
	else if (REC_TYPE_BATCH == rectype)
	{
		strcpy(dir_str,batch_dirctory);
		strcat(dir_str,batch_list_file);
		node_size = sizeof(TBATCH_NODE);
	}
	else
	{
		return (unsigned char*)0;
	}

	if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return (unsigned char*)0;
	}

	if (file1.fsize < node_size*index)
	{
		f_close(&file1);
		return (unsigned char*)0;
	}

	f_lseek(&file1,node_size*(index-1));

	if (f_read(&file1,pBuf,node_size,&node_size) != FR_OK)
	{
		f_close(&file1);
		return (unsigned char*)0;
	}

	checkvalue = crc32(0,pBuf+4,node_size-4);
	if (memcmp((unsigned char*)&checkvalue,pBuf,4))
	{
		//校验值不对
		f_close(&file1);
		return (unsigned char*)0;
	}

	f_close(&file1);
	return pBuf;
}

//计算校验值并填回去
static calc_check_value(unsigned char rectype,unsigned char *precord)
{
	unsigned int		checkvalue;

	//计算此份记录的校验值，放到记录的最前面4个字节
	if (REC_TYPE_GOODS_LIST == rectype)
	{
		checkvalue = crc32(0,precord + 4,sizeof(TGOODS_SPEC_RECORD) - 4);
	}
	else if (REC_TYPE_CHECK == rectype )
	{
		checkvalue = crc32(0,precord + 4,sizeof(TCHECK_INFO_RECORD) - 4);
	}
	else if (REC_TYPE_BATCH == rectype)
	{
		checkvalue = crc32(0,precord + 4,sizeof(TBATCH_NODE) - 4);
	}

	memcpy(precord,(unsigned char*)&checkvalue,4);
}
/**
* @brief 增加一个节点到记录文件之后更新记录文件的链表
* @param[in] FIL *file						记录文件指针,已经打开的文件
* @param[in] unsigned char	*p_node			要新增的节点指针
* @param[in] unsigned char	node_type		节点类型		
* @param[in] unsigned char	key_type		关键字类型		1:barcode  2:name (序列号节点的属性1) 
* @param[in] unsigned int	header			链表首地址
* @param[in] unsigned int	node_offset		要增加节点的偏移
* @return 0:成功
*        else:失败
* @note 只有商品信息节点是双向链表，所以增加节点时需要维护双向链表，其余节点都是单向链表
*/
static int  update_link_info_after_addNode(FIL *file,unsigned char*p_node, unsigned char node_type,unsigned char key_type,unsigned int header,unsigned int node_offset)
{
	unsigned int	rec_size,next_node_offset,current_offset,tmp;
	unsigned char	buffer[512];		//如果一个节点的大小超过了512，那么还需要增加此临时空间

	if (REC_TYPE_GOODS_LIST == node_type)
	{
		rec_size = sizeof(TGOODS_SPEC_RECORD);
	}
	else if (REC_TYPE_CHECK == node_type)
	{
		rec_size = sizeof(TCHECK_INFO_RECORD);
	}
	else if (REC_TYPE_BATCH == node_type)
	{	
		rec_size = sizeof(TBATCH_NODE);
	}
	else
	{
		return -1;
	}

	next_node_offset = header;
	while (next_node_offset)
	{
		current_offset = next_node_offset;
		f_lseek(file,(next_node_offset-1)*rec_size);		//文件指针定位到链表首记录的偏移处，注意链表中记录的偏移是该节点结束的偏移
		if (f_read(file,buffer,rec_size,&tmp) != FR_OK)
		{
			return -2;
		}

		if(key_type != 1)
		{
			return -1;
		}

		if (REC_TYPE_GOODS_LIST == node_type)
		{
			//商品节点
			next_node_offset = ((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next;
			if (next_node_offset == 0)
			{
				((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next = node_offset;		//将链表最后一个节点的next指针指向当前节点
				((TGOODS_SPEC_RECORD*)p_node)->by_barcode_next = current_offset;	//将当前节点的prev指针指向链表的最后一个节点
				((TGOODS_SPEC_RECORD*)p_node)->by_barcode_next = 0;					//将当前节点的next指针清0
			}
		}
		else if(REC_TYPE_CHECK == node_type)
		{
			//盘点信息节点
			next_node_offset = ((TCHECK_INFO_RECORD*)buffer)->by_barcode_next;
			if (next_node_offset == 0)
			{
				((TCHECK_INFO_RECORD*)buffer)->by_barcode_next = node_offset;
				((TCHECK_INFO_RECORD*)p_node)->by_barcode_prev = current_offset;
				((TCHECK_INFO_RECORD*)p_node)->by_barcode_next = 0;
			}
		}
		else
		{
			next_node_offset = ((TBATCH_NODE*)buffer)->by_index_next;
			if (next_node_offset == 0)
			{
				((TBATCH_NODE*)buffer)->by_index_next = node_offset;
				((TBATCH_NODE*)p_node)->by_index_prev = current_offset;
				((TBATCH_NODE*)p_node)->by_index_next = 0;
			}
		}
	}

	calc_check_value(node_type,buffer);
	f_lseek(file,(current_offset-1)*rec_size);		//文件指针重新定位到链表最后一个节点处
	if (f_write(file,buffer,rec_size,&tmp) != FR_OK)
	{
		return -3;
	}

	if (tmp != rec_size)
	{
		return -3;
	}

	f_sync(file);
	
	return 0;
}


//将准备进行的文件操作的相关信息保存到日志文件
//note : 对于每一种操作来说参数列表中的参数的含义都不相同
//       op_type == OP_TYPE_ADD_NODE 时,		param1 表示记录类型, param2 表示新增节点的偏移  param3 表示新增节点的长度  param4 指向新增节点数据
//		 op_type == OP_TYPE_CLEAR_NODE 时,		param1 表示记录类型, param2 无意义					param3 无意义 param4 无意义
//		 op_type == OP_TYPE_DEL_NODE 时,		param1 表示记录类型, param2 表示要删除节点的偏移	param3 无意义 param4 无意义
//		 op_type == OP_TYPE_REPLACE_NODE 时,	param1 表示记录类型, param2 表示要替换节点的偏移	param3 表示替换节点的长度  param4 指向新替换节点数据
//		 op_type == OP_TYPE_WRITE_BIN_FILE 时,	param1 表示记录类型, param2 表示文件类型	param3 无意义  param4 无意义
static int save_log_file(void *data,unsigned int len)
{
	unsigned int tmp;
	unsigned int llen;
	unsigned char tmp_buffer[530];
	//将该操作保存到日志文件中
	if (f_open(&file3,log_file,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -1;
	}

	if (log_len == 0)
	{
		llen = len+4;
	}
	else
	{
		llen = log_len+len;
	}
	
	f_lseek(&file3,file3.fsize-log_len);

	if (log_len)
	{
		if (f_read(&file3,(void*)&tmp_buffer,log_len,&tmp) != FR_OK)
		{
			f_close(&file3);
			return -1;
		}

		if (tmp != log_len)
		{		
			f_close(&file3);
			return -1;
		}
	}
	memcpy((void*)tmp_buffer,(void*)&llen,4);
	if (log_len)
	{
		memcpy((void*)(tmp_buffer+log_len),data,len);
	}
	else
	{
		memcpy((void*)(tmp_buffer+4),data,len);
	}
	
	f_lseek(&file3,file3.fsize-log_len);
	if(f_write(&file3,tmp_buffer,llen,&tmp) != FR_OK)
	{
		f_close(&file3);
		return -1;
	}

	if (tmp != llen)
	{
		f_close(&file3);
		return -1;
	}
	f_close(&file3);		//关闭日志文件
	log_len = llen;
        return 0;
}


//清除日志文件最新添加的日志
int clear_log_file(int mode)
{
	if (f_open(&file3,log_file,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -1;
	}

	if (mode)
	{
		f_lseek(&file3,0);
	}
	else
	{
		if (file3.fsize >= log_len)
		{
			f_lseek(&file3,file3.fsize-log_len);
		}
	}
	f_truncate(&file3);

	f_close(&file3);
	return 0;
}

/**
* @brief 增加一条记录
* @param[in] unsigned char *precord 记录指针
* @return 0:成功
*        else:失败
* @note 此函数已经增加断电保护
*/
int record_add(unsigned char rectype,unsigned char *precord)
{
	unsigned char	dir_str[35];
	unsigned char	dir_tmp[35];
	unsigned char	inf_file_str[35];
	unsigned int	rec_offset,i;
	unsigned int	node_size,tmp;
	unsigned long	hash_value[10];
	const unsigned char	*p_hash_table_file[11];
	unsigned int		link_end;		//链表的尾地址
	const unsigned char	*target_dir;
	unsigned int		target_max_cnt;
	unsigned int		invalid_node_offset = 0;
	unsigned int		temp;
	int					err_code = 0;
	unsigned int		log_data[3];

	log_len = 0;

	if (REC_TYPE_GOODS_LIST == rectype)
	{
		target_dir = goods_list_dirctory;
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,goods_list_file);
		node_size = sizeof(TGOODS_SPEC_RECORD);
		target_max_cnt = GOODS_LIST_MAX_CNT;

		hash_value[0] = HashString(((TGOODS_SPEC_RECORD*)precord)->bar_code,0);	//计算该商品条码的hash值
		//hash_value[1] = HashString(((TGOODS_SPEC_RECORD*)precord)->name,0);		//计算该商品名称的hash值
		p_hash_table_file[0] = barcode_hash_table_file;
		//p_hash_table_file[1] = name_hash_table_file;
		p_hash_table_file[1] = 0;
	}
	else if (REC_TYPE_CHECK == rectype)
	{
		target_dir = check_dirctory;
		strcpy(dir_str,check_dirctory);
		strcat(dir_str,check_list_file);
		node_size = sizeof(TCHECK_INFO_RECORD);
		hash_value[0] = HashString(((TCHECK_INFO_RECORD*)precord)->bar_code,0);	//计算该商品条码的hash值
		p_hash_table_file[0] = barcode_hash_table_file;
		p_hash_table_file[1] = 0;
		target_max_cnt = CHECK_LIST_MAX_CNT;		
	}
	else if (REC_TYPE_BATCH == rectype)
	{
		target_dir = batch_dirctory;
		strcpy(dir_str,batch_dirctory);
		strcat(dir_str,batch_list_file);
		node_size = sizeof(TBATCH_NODE);
		hash_value[0] = 2;	//计算该商品条码的hash值
		p_hash_table_file[0] = batch_inf_file;
		p_hash_table_file[1] = 0;
		target_max_cnt = BATCH_LIST_MAX_CNT;		
	}
	else
	{
		return -1;
	}

	if (REC_TYPE_BATCH == rectype)
	{

		strcpy(inf_file_str,batch_dirctory);
		strcat(inf_file_str,batch_inf_file);

		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
		{
			return -2;
		}

		if (file1.fsize > 12)
		{
			f_lseek(&file1,file1.fsize-4);
			if ((f_read(&file1,(void*)&invalid_node_offset,4,&tmp) != FR_OK) ||(tmp != 4))		//获取保存在序列号信息文件中的某一个已经被删除节点的偏移
			{
				f_close(&file1);
				return -4;
			}
		}

		f_close(&file1);
	}

	//打开保存相应记录的节点文件
	if (f_open(&file1,(const char*)dir_str,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -2;
	}

	if (invalid_node_offset)
	{
		rec_offset = invalid_node_offset;		//如果是新增序列号节点，并且记录文件中存在已经被删除的节点，那么新增的节点
	}
	else
	{
		rec_offset = file1.fsize/node_size;		//获取记录文件当前已经保存的记录数
		if (rec_offset < target_max_cnt)
		{
			rec_offset += 1;	
		}
		else
		{
			//rec_offset = 1;		//如果记录数达到了上限值，那么无法增加新记录
			f_close(&file1);
			return -20;
		}
	}

	g_rec_offset = rec_offset;
#ifdef REC_DEBUG
	debug_out("\x0d\x0a",2,1);
	debug_out("Add Node:",strlen("Add Node:"),1);
	debug_out((unsigned char*)&rec_offset,4,0);
#endif

	log_data[0] = OP_TYPE_ADD_NODE;
	log_data[1] = rectype;
	log_data[2] = rec_offset;


	if (save_log_file((void*)log_data,12))		//状态0，LOG文件中只保存了操作类型、记录类型、目标记录偏移
	{
		err_code = -10;
		goto err_handle;
	}

	//计算此记录需要计算的关键字的hash值
	i = 0;
	while (p_hash_table_file[i])
	{
		strcpy(dir_str,target_dir);
		strcat(dir_str,p_hash_table_file[i]);

		//打开相应的hash_table文件
		if (f_open(&file2,(const char*)dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			err_code = -4;
			goto err_handle;
		}

		f_lseek(&file2,4*(hash_value[i]%HASH_TABLE_SIZE));		//定位到该hash值在hash表中对应的偏移处

		//读取此hash值对应的链表的尾地址
		if (f_read(&file2,(void*)&link_end,4,&tmp) != FR_OK)
		{
			err_code = -5;
			goto err_handle;
		}
#ifdef REC_DEBUG
		debug_out(" LinkEnd=",strlen(" LinkEnd="),1);
		debug_out((unsigned char*)&link_end,4,0);
#endif
		log_data[0] = hash_value[i];
		log_data[1] = link_end;
		if(save_log_file((void*)log_data,8))		//状态1或者状态2
		{
			err_code = -10;
			goto err_handle;
		}
		
		f_lseek(&file2,4*(hash_value[i]%HASH_TABLE_SIZE));		//定位到该hash值在hash表中对应的偏移处

		//更新该链表的尾节点
		if (f_write(&file2,(void*)&rec_offset,4,&tmp) != FR_OK)
		{
			err_code = -6;
			goto err_handle;
		}

		if (tmp != 4)
		{
			err_code = -6;
			goto err_handle;
		}

		f_sync(&file2);

		if (REC_TYPE_BATCH == rectype)
		{
			f_lseek(&file2,4);
			if (f_read(&file2,(void*)&temp,4,&tmp) != FR_OK)
			{
				err_code = -7;
				goto err_handle;
			}

			if (temp == 0)
			{
				//如果链表的首节点还是0，那么需要更新链表的首节点
				f_lseek(&file2,4);
				if (f_write(&file2,(void*)&rec_offset,4,&tmp) != FR_OK)
				{
					err_code = -8;
					goto err_handle;
				}

				if (tmp != 4)
				{
					err_code = -8;
					goto err_handle;
				}
			}
		}

		f_close(&file2);	//hash表文件可以关闭了

		if (link_end)
		{
			//如果链表不是空的，那么需要更新链表的信息(如果是商品信息节点，还需要更新当前节点的信息，因为商品信息链表是双向链表)
			if (update_link_info_after_addNode(&file1,precord,rectype,(i+1),link_end,rec_offset))
			{
				err_code = -9;
				goto err_handle;
			}
		}

		i++;
	}

	calc_check_value(rectype,precord);

	if (save_log_file((void*)precord,node_size))
	{
		err_code = -8;
		goto err_handle;
	}

	f_lseek(&file1,(rec_offset-1)*node_size);		//将文件指针移到文件末尾
	if (f_write(&file1,precord,node_size,&tmp) != FR_OK)
	{
		err_code = -3;	 
		goto err_handle;
	}

	if (tmp != node_size)
	{
		err_code = -3;	 
		goto err_handle;
	}

	//保存节点的文件可以关闭了
	f_close(&file1);	

	if (invalid_node_offset)
	{
		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
		{
			return -2;
		}

		f_lseek(&file1,file1.fsize-4);
		f_truncate(&file1);		//将记录在信息文件中的已经删除的节点偏移清掉。
		f_close(&file1);
	}
	
	clear_log_file(0);
	return 0;

err_handle:

	f_close(&file1);
	return err_code;

}

/**
* @brief 增加一条记录
* @param[in] unsigned char *precord 记录指针
* @param[in] unsigned char open_mode 是都需要重新打开文件		0:需要  else:不需要
* @param[in] unsigned char close_mode 是否需要关闭操作文件		0:需要  else:不需要
* @return 0:成功
*        -1:失败
* @note 此函数暂时没有考虑断电保护，后续需要增加断电保护功能
*/
int record_add_ext(unsigned char rectype,unsigned char *precord,unsigned char open_mode,unsigned char close_mode)
{
	//
}

/**
* @brief 将一个商品信息节点添加到相应链表中
* @param[in] FIL* node_file							该节点所在的文件（已经处于open的状态）
* @param[in] unsigned char link_type				链表类型	1:条码链表	2:名称链表 
* @param[in] TGOODS_SPEC_RECORD *p_TGOODS_SPEC_RECORD				该商品节点的指针
* @param[in] unsigned int key_hashvalue				该节点相应关键字的hash值	
* @return 0:成功
*        -1:失败
* @note  此函数只支持添加商品信息节点
*	     node_file 已经处于打开的状态
*/
static int add_goods_node(FIL* node_file,unsigned char link_type,TGOODS_SPEC_RECORD *p_goods_node,unsigned int key_hashvalue,unsigned int index)
{
	unsigned char dir_str[35];
	const unsigned char * target_hash_file;
	unsigned int		  target_offset,link_end,tmp;

	strcpy(dir_str,goods_list_dirctory);
	//if ((link_type == 1)||(link_type == 2))
	if (link_type == 1)
	{
		target_hash_file = barcode_hash_table_file;
		target_offset = 4*(key_hashvalue%HASH_TABLE_SIZE);
	}
	else
	{
		return -1;
	}

	strcat(dir_str,target_hash_file);
	if (f_open(&file2,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
	{
		return -2;
	}

	f_lseek(&file2,target_offset);
	//获取链表的尾地址
	if (f_read(&file2,(void*)&link_end,4,&tmp) != FR_OK)
	{
		return -3;
	}

	//更新链表的尾地址
	f_lseek(&file2,target_offset);
	if (f_write(&file2,(void*)&index,4,&tmp) != FR_OK)
	{
		return -4;
	}

	if (tmp != 4)
	{
		return -4;
	}

	f_sync(&file2);

	if (link_end == 0)
	{
		//如果链表的尾地址为0
		p_goods_node->by_barcode_next = 0;
		p_goods_node->by_barcode_prev = 0;
		f_close(&file2);
	}
	else
	{
		f_close(&file2);
		if (link_end != index)
		{
			if(update_link_info_after_addNode(node_file,(unsigned char*)p_goods_node,1,link_type,link_end,index))
			{
				return -5;
			}
		}
	}

	return 0;
}
/**
* @brief 将一个商品信息节点从某一个链表中删除
* @param[in] FIL* node_file							该节点所在的文件（已经处于open的状态）
* @param[in] unsigned char link_type				链表类型	1:条码链表	2:名称链表
* @param[in] TGOODS_SPEC_RECORD *p_TGOODS_SPEC_RECORD				该商品节点的指针
* @param[in] unsigned int key_hashvalue				该节点相应关键字的hash值	
* @return 0:成功
*        -1:失败
* @note  此函数只支持删除商品信息链表
*	     node_file 已经处于打开的状态
*/
static int delete_goods_node(FIL* node_file,unsigned char link_type,TGOODS_SPEC_RECORD *p_goods_node,unsigned int key_hashvalue)
{
	const unsigned char *target_hash_file = 0;
	unsigned char		tmp_buffer[512];
	unsigned char		dir_str[35];
	unsigned int		new_link_end,target_offset,i,tmp;


	//if ((link_type == 1)||(link_type == 2))
	if (link_type == 1)
	{
		if (p_goods_node->by_barcode_next == 0)
		{
			//说明该节点是相应规格链表的尾节点，那么需要更新相应规格的hash_table文件
			target_hash_file = barcode_hash_table_file;
			new_link_end = p_goods_node->by_barcode_prev;	//新的链表尾地址指针
			p_goods_node->by_barcode_prev = 0;					//将当前节点的next指针清0
			target_offset = 4*(key_hashvalue%HASH_TABLE_SIZE);
		}
		else
		{
			f_lseek(node_file,(p_goods_node->by_barcode_next - 1)*sizeof(TGOODS_SPEC_RECORD));

			if (f_read(node_file,tmp_buffer,sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
			{
				return -6;
			}

			((TGOODS_SPEC_RECORD*)tmp_buffer)->by_barcode_prev = p_goods_node->by_barcode_prev;
			calc_check_value(REC_TYPE_GOODS_LIST,tmp_buffer);

			f_lseek(node_file,(p_goods_node->by_barcode_next - 1)*sizeof(TGOODS_SPEC_RECORD));

			//更新了该节点的后一个节点的prev指针
			if (f_write(node_file,tmp_buffer,sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
			{
				return -7;
			}

			if (tmp != sizeof(TGOODS_SPEC_RECORD))
			{
				return -7;
			}

			f_sync(node_file);
		}

		if (p_goods_node->by_barcode_prev)
		{
			//如果该节点还有前一个节点，那么它的next节点也需要更新
			f_lseek(node_file,(p_goods_node->by_barcode_prev - 1)*sizeof(TGOODS_SPEC_RECORD));

			if (f_read(node_file,tmp_buffer,sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
			{
				return -8;
			}

			((TGOODS_SPEC_RECORD*)tmp_buffer)->by_barcode_next = p_goods_node->by_barcode_next;
			calc_check_value(REC_TYPE_GOODS_LIST,tmp_buffer);

			f_lseek(node_file,(p_goods_node->by_barcode_prev - 1)*sizeof(TGOODS_SPEC_RECORD));

			//更新了该节点的前一个节点的next指针
			if (f_write(node_file,tmp_buffer,sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
			{
				return -9;
			}

			if (tmp != sizeof(TGOODS_SPEC_RECORD))
			{
				return -9;
			}

			f_sync(node_file);
		}

		//至此为止，可以将该节点自身的前后节点指针清除掉
		p_goods_node->by_barcode_prev = 0;
		p_goods_node->by_barcode_next = 0;
	}
	else
	{
		return -1;
	}

	if (target_hash_file)
	{
		//需要更新相应的hash_table文件
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,target_hash_file);

		if (f_open(&file2,dir_str,FA_OPEN_EXISTING | FA_WRITE) != FR_OK)
		{
			return -2;
		}

		f_lseek(&file2,target_offset);
		if (f_write(&file2,(void*)&new_link_end,4,&tmp) != FR_OK)
		{
			return -5;
		}

		if (tmp != 4)
		{
			return -5;
		}

		f_close(&file2);
	}

	return 0;
}

/**
* @brief 取代某一条记录
* @param[in] unsigned char rectype					记录类型
* @param[in] int			index					记录索引
* @param[in] unsigned char *p_new_record			新记录
* @param[in] unsigned char *key_value_change_tbl	记录替换时，关键字发生变化的列表
* @return 0:成功
*        -1:失败
* @note  如果替换记录（节点）时，新的节点的关键字发生了变化，那么需要将已经修改的关键字的相应编号生成一张表传进来，以0结束
*		 关键的编号如下：1: 表示条形码关键字（实际上这个关键字不可能发生变化）;
*						 2: 名称关键字
*/
int record_module_replace(unsigned char rectype,int index,unsigned char *p_new_record,TNODE_MODIFY_INFO *modify_info_tbl)
{
	unsigned char		dir_str[35];
	unsigned int		node_size,tmp;
	unsigned int		i;
	unsigned int		log_data[6];

	if (index == 0)
	{
		return -1;
	}
	if (REC_TYPE_GOODS_LIST == rectype)
	{
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,goods_list_file);
		node_size = sizeof(TGOODS_SPEC_RECORD);
	}
	else if (REC_TYPE_CHECK == rectype)
	{
		strcpy(dir_str,check_dirctory);
		strcat(dir_str,check_list_file);
		node_size = sizeof(TCHECK_INFO_RECORD);
	}
	else
	{
		return -1;
	}

	if (f_open(&file1,(const char*)dir_str,FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
	{
		return -2;
	}
#if 0
	if (REC_TYPE_GOODS_LIST == rectype)
	{
		if ((modify_info_tbl)&&(modify_info_tbl[0].keyspec_type))
		{
			//有关键字发生变化，那么需要更新该节点所在链表的信息
			i = 0;
			while(modify_info_tbl[i].keyspec_type)
			{
				switch (modify_info_tbl[i].keyspec_type)
				{
				case TAG_NAME:
					//从原来的链表中删除，增加到新的链表中

					//保存日志文件
					log_len = 0;
					log_data[0] = OP_TYPE_REPLACE_NODE;
					log_data[1] = rectype;
					log_data[2] = index;
					log_data[3] = modify_info_tbl[i].old_keyspec_hash;
					log_data[4] = modify_info_tbl[i].new_keyspec_hash;
					
					strcpy(dir_str,goods_list_dirctory);
					strcat(dir_str,name_hash_table_file);
					if (f_open(&file2,dir_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
					{
						return -6;
					}

					f_lseek(&file2,4*(modify_info_tbl[i].new_keyspec_hash%HASH_TABLE_SIZE));
					//获取链表的尾地址
					if (f_read(&file2,(void*)&log_data[5],4,&tmp) != FR_OK)
					{
						return -6;
					}

					f_close(&file2);
					
					if(save_log_file((void*)log_data,24))
					{
						return -6;
					}

					if(save_log_file((void*)p_new_record,node_size))
					{
						return -6;
					}

					if(delete_goods_node(&file1,modify_info_tbl[i].keyspec_type,(TGOODS_SPEC_RECORD*)p_new_record,modify_info_tbl[i].old_keyspec_hash))
					{
						f_close(&file1);
						return -4;
					}

					if (add_goods_node(&file1,modify_info_tbl[i].keyspec_type,(TGOODS_SPEC_RECORD*)p_new_record,modify_info_tbl[i].new_keyspec_hash,index))
					{
						f_close(&file1);
						return -5;
					}
					break;
				default:
					break;
				}

				i++;
			}

		}
	}
#endif

	calc_check_value(rectype,p_new_record);
	f_lseek(&file1,(index - 1)*node_size);

	if (f_write(&file1,(void*)p_new_record,node_size,&tmp) != FR_OK)
	{
		f_close(&file1);
		return -3;
	}

	if (tmp != node_size)
	{
		f_close(&file1);
		return -3;
	}
	f_close(&file1);

	if ((REC_TYPE_GOODS_LIST == rectype)&&(modify_info_tbl)&&(modify_info_tbl[0].keyspec_type))
	{
		clear_log_file(0);		//清除日志
	}
	
	return 0;
}


/**
* @brief 得到记录总条数
* @return 0...LOGIC_RECORD_BLOCK_SIZE
*/
int record_module_count(unsigned char rectype)
{
	unsigned char dir_str[35];
	unsigned char inf_file_str[35];
	int		cnt,rec_size;
	if (rectype == REC_TYPE_GOODS_LIST)
	{
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,goods_list_file);
		rec_size = sizeof(TGOODS_SPEC_RECORD);
	}
	else if (rectype == REC_TYPE_CHECK)
	{
		strcpy(dir_str,check_dirctory);
		strcat(dir_str,check_list_file);
		rec_size = sizeof(TCHECK_INFO_RECORD);
	}
	else if (rectype == REC_TYPE_BATCH)
	{
		strcpy(dir_str,batch_dirctory);
		strcat(dir_str,batch_list_file);
		rec_size = sizeof(TBATCH_NODE);
	}
	else
	{
		return 0;
	}

	if (f_open(&file1,(const char*)dir_str,FA_OPEN_ALWAYS | FA_READ) != FR_OK)
	{
		return -1;
	}

	cnt = file1.fsize / rec_size;
	f_close(&file1);

	if (REC_TYPE_BATCH == rectype)
	{
		strcpy(inf_file_str,batch_dirctory);
		strcat(inf_file_str,batch_inf_file);
	
		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
		{
			return -1;
		}

		if (cnt >= (file1.fsize - 12)/4)
		{
			cnt -= (file1.fsize - 12)/4;		//减去无效的节点，理论上无效的节点的偏移都记录在信息文件中，所以直接减去在信息文件中记录的无效节点个数，即表示在节点文件中还存在的节点个数。
			f_close(&file1);
		}
		else
		{
			//理论上应该不会出现这种情况！万一真出现了呢？尼玛，那只能来杀手锏了，直接将记录文件全部初始化
			f_close(&file1);
			record_clear(rectype);
			cnt = 0;
		}
	}
	return cnt;
}

/**
* @brief 清除所有记录
* @return 0：成功  -1：失败
*/
int record_clear(unsigned char rectype)
{
	unsigned char	dir_str[35];
	unsigned char	dir_tmp[35];
	const unsigned char	*p_hash_table_file[3];
	const unsigned char	*target_dir;
	int i;
	unsigned int		log_data[2];

	if (rectype == REC_TYPE_GOODS_LIST)
	{
		target_dir = goods_list_dirctory;
		strcpy(dir_str,goods_list_dirctory);
		strcat(dir_str,goods_list_file);

		p_hash_table_file[0] = barcode_hash_table_file;
		p_hash_table_file[1] = 0;
	}
	else if (rectype == REC_TYPE_CHECK)
	{
		target_dir = check_dirctory;
		strcpy(dir_str,check_dirctory);
		strcat(dir_str,check_list_file);
		p_hash_table_file[0] = barcode_hash_table_file;
		p_hash_table_file[1] = 0;
	}
	else if (rectype == REC_TYPE_BATCH)
	{
		target_dir = batch_dirctory;
		strcpy(dir_str,batch_dirctory);
		strcat(dir_str,batch_list_file);
		p_hash_table_file[0] = batch_inf_file;
		p_hash_table_file[1] = 0;
	}
	else
	{
		return -1;
	}

	if (f_open(&file1,(const char*)dir_str,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -1;
	}

	f_lseek(&file1,0);		//文件指针移动到文件开始

	//将文件截断到文件开始
	if (f_truncate(&file1) != FR_OK)
	{
		f_close(&file1);
		return -2;
	}

	f_close(&file1);



	log_data[0] = OP_TYPE_CLEAR_NODE;
	log_data[1] = rectype;
	log_len = 0;
	if (save_log_file((void*)log_data,8))		//状态0，LOG文件中只保存了操作类型、记录类型
	{
		return -4;
	}

	i = 0;
	while (p_hash_table_file[i])
	{
		strcpy(dir_str,target_dir);
		strcat(dir_str,p_hash_table_file[i]);

		//打开相应的hash_table文件
		if (f_open(&file2,(const char*)dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			return -3;
		}

		if (rectype == REC_TYPE_BATCH)
		{
			init_data_info_file(&file2);	
		}
		else
		{
			init_hash_table_file(&file2);	
		}
		f_close(&file2);
        i++;
	}

	clear_log_file(0);		//清除日志
	return 0;
}

/**
* @brief 根据某一个关键字查询交易记录，返回记录节点的偏移
* @param[in] unsigned char rectype				要搜索的记录区
* @param[in] unsigned char search_tag_type		要匹配的关键字类型
* @param[in] unsigned char *in_tag				要匹配的关键字字符串
* @param[out] unsigned char *outbuf				搜索到的记录缓冲区
* @return 搜索结果	=0:没有搜索到该交易记录  > 0:搜索到的记录的偏移   < 0: 错误类型
* @note  
*/
static int rec_search(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag,unsigned char *outbuf)
{
	unsigned int hash_value,link_end,tmp;
	unsigned char dir_str[35];
	unsigned char	dir_tmp[35];
	unsigned char	buffer[512];		//如果一个节点的大小超过了512，那么还需要增加此临时空间
	const unsigned char	*	target_index_file;
	const unsigned char	*	target_directory;
	const unsigned char	*	target_node_file;
	unsigned int	node_size;				//节点大小
	unsigned int	target_offset;			//节点中要与关键字匹配的字符串位于节点结构体中的偏移
	unsigned int	target_prev_offset;		//该节点对应关键字的链表指针在GOOD_NODE结构体中相对于by_barcode_next的偏移
	//注意：上述两个值与结构体中分配给每个数据域空间的大小密切相关，所以如果结构体修改，此函数中这些地方要做相应的修改	

	if (REC_TYPE_GOODS_LIST == rectype)
	{
		target_directory = goods_list_dirctory;
		target_node_file = goods_list_file;
		node_size = sizeof(TGOODS_SPEC_RECORD);
		switch (search_tag_type)
		{
		case TAG_BARCODE:
			target_index_file = barcode_hash_table_file;
			target_offset = 4;
			target_prev_offset = 0;
			break;
		//case TAG_NAME:
		//	target_index_file = name_hash_table_file;
		//	target_offset = 4+31;												
		//	target_prev_offset = 1;
		//	break;
		default:
			return -1;		//参数错误
		}
	}
	else if (REC_TYPE_CHECK == rectype)
	{
		node_size = sizeof(TCHECK_INFO_RECORD);
		target_node_file = check_list_file;
		target_directory = check_dirctory;

		if (search_tag_type == TAG_BARCODE)
		{
			target_index_file = barcode_hash_table_file;
			target_offset = 4;						//条形码位于操作记录节点记录起始位置的偏移4字节处											
			target_prev_offset = 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	memset(outbuf,0,node_size);		//清掉返回数据的缓冲区
	strcpy(dir_str,target_directory);


	//如果要搜索的关键字类型是条码

	//step1: 计算关键字的hash值
	hash_value = HashString(in_tag,0);	

	//step2: 根据hash值查找到与该关键字具有相同hash值模值的链表尾地址
	strcat(dir_str,target_index_file);		
	if (f_open(&file1,dir_str,FA_OPEN_ALWAYS | FA_READ) != FR_OK)
	{
		return -1;
	}

	f_lseek(&file1,4*(hash_value%HASH_TABLE_SIZE));

	if (f_read(&file1,(void*)&link_end,4,&tmp) != FR_OK)
	{
		return -2;
	}	

	f_close(&file1);

	//根据读取出来的链表首地址查找到第一个与关键字完全匹配的记录，如果需要将完全匹配关键字的所有记录全部都读取出来，需要另外的接口来实现。

	if (link_end)
	{	
		strcpy(dir_str,target_directory);
		strcat(dir_str,target_node_file);

		//打开商品清单文件
		if (f_open(&file1,dir_str,FA_OPEN_ALWAYS | FA_READ) != FR_OK)
		{
			return -3;
		}

		while (link_end)
		{
			f_lseek(&file1,(link_end-1)*node_size);		//文件指针指向该节点的起始位置

			if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK) || (node_size != tmp))
			{
				return -4;
			}

			if (strcmp(in_tag,buffer+target_offset) == 0)
			{
				memcpy(outbuf,buffer,node_size);
				return link_end;		//搜索到链表中第一个与关键字匹配的节点
			}

			if (REC_TYPE_GOODS_LIST == rectype)
			{
				link_end = ((TGOODS_SPEC_RECORD*)buffer)->by_barcode_prev;
			}
			else if (REC_TYPE_CHECK == rectype)
			{
				link_end = ((TCHECK_INFO_RECORD*)buffer)->by_barcode_prev;
			}
		}
	}

	return 0;		//没有搜索到与关键字匹配的记录
}

/**
* @brief 根据关键字在记录中搜索与之相匹配的记录索引
* @param[in] unsigned char rec_type		记录类型
* @param[in] unsigned char tag_typa		搜索的关键字类型
* @param[in] unsigned char * intag		搜索的关键字字符串
* @param[in] unsigned char	inlen		关键字长度
* @return 返回搜索到的交易记录的索引号  -1 ：表示没有找到相应的交易记录   else: 搜索到的交易记录的索引号
* @note  字符串比较
*/
unsigned char *rec_serachby_name(unsigned char rec_type,unsigned char *intag,unsigned int *r_index)
{
#if 0
	unsigned char					*pBuf;
	unsigned int					index = 1;
	int								count;
	unsigned int					target_spec_rec_index;
	*r_index = 0;
	unsigned char					buffer[sizeof(TGOODS_SPEC_RECORD)];

	if (rec_type != REC_TYPE_CHECK_INFO)
	{
		return (unsigned char*)0;		//参数错误
	}

	pBuf = rec_searchby_tag(REC_TYPE_GOODS_LIST,TAG_NAME,intag,&target_spec_rec_index);
	if (pBuf == 0)
	{
		return (unsigned char*)0;
	}

	memcpy(buffer,pBuf,sizeof(TGOODS_SPEC_RECORD));

	return rec_searchby_tag(REC_TYPE_CHECK_INFO,TAG_BARCODE,((TGOODS_SPEC_RECORD*)buffer)->bar_code,r_index);
#endif
}

//在知道链表首地址的前提下，按照某一个关键字搜索链表的节点
//@note 链表的首地址不在搜索范围之内。
//      操作记录不支持反向搜索
static int rec_search_ext(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag,unsigned int start,unsigned char direct,unsigned char *outbuf)
{
	unsigned int  tmp;
	unsigned char dir_str[35];
	unsigned char dir_tmp[35];
	unsigned char	buffer[512];		//如果一个节点的大小超过了512，那么还需要增加此临时空间
	const unsigned char	*	target_directory;
	const unsigned char	*	target_node_file;
	unsigned int	node_size;				//节点大小
	unsigned int	target_offset;			//节点中要与关键字匹配的字符串位于节点结构体中的偏移
	unsigned int	target_next_offset;		//该节点对应关键字的链表指针在GOOD_NODE结构体中相对于by_barcode_next的偏移
	//注意：上述两个值与结构体中分配给每个数据域空间的大小密切相关，所以如果结构体修改，此函数中这些地方要做相应的修改	

	if (REC_TYPE_GOODS_LIST == rectype)
	{
		target_directory = goods_list_dirctory;
		target_node_file = goods_list_file;
		node_size = sizeof(TGOODS_SPEC_RECORD);
		switch (search_tag_type)
		{
		case TAG_BARCODE:
			target_offset = 4;
			target_next_offset = 0;
			break;
		//case TAG_NAME:
		//	target_offset = 4+31;												
		//	target_next_offset = 1;
		//	break;
		default:
			return -1;		//参数错误
		}
	}
	else if (REC_TYPE_CHECK== rectype)
	{
		node_size = sizeof(TCHECK_INFO_RECORD);
		target_node_file = check_list_file;
		target_directory = check_dirctory;

		if (search_tag_type == TAG_BARCODE)
		{
			target_offset = 4;						//条形码位于操作记录节点记录起始位置的偏移4字节处											
			target_next_offset = 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	//memset(outbuf,0,node_size);		//清掉返回数据的缓冲区
	//strcpy(dir_str,target_directory);

	if (start)
	{	
		strcpy(dir_str,target_directory);
		strcat(dir_str,target_node_file);

		//打开商品清单文件
		if (f_open(&file1,dir_str,FA_OPEN_ALWAYS | FA_READ) != FR_OK)
		{
			return -3;
		}

		f_lseek(&file1,(start-1)*node_size);		//文件指针指向该节点的起始位置

		if (f_read(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			return -4;
		}

		while (start)
		{
			if (REC_TYPE_GOODS_LIST == rectype)
			{
				if (direct == 0)
				{
					start = ((TGOODS_SPEC_RECORD*)buffer)->by_barcode_prev;
				}
				else
				{
					start = ((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next;
				}
			}
			else if (REC_TYPE_CHECK == rectype)
			{
				if (direct == 0)
				{
					start = ((TCHECK_INFO_RECORD*)buffer)->by_barcode_prev;
				}
				else
				{
					start = ((TCHECK_INFO_RECORD*)buffer)->by_barcode_next;
				}
			}

			if (start)
			{
				f_lseek(&file1,(start-1)*node_size);		//文件指针指向该节点的起始位置

				if (f_read(&file1,buffer,node_size,&tmp) != FR_OK)
				{
					return -4;
				}

				if (strcmp(in_tag,buffer+target_offset) == 0)
				{
					memcpy(outbuf,buffer,node_size);
					return start;		//搜索到链表中第一个与关键字匹配的节点
				}
			}
		}
	}

	return 0;		//没有搜索到与关键字匹配的记录
}


/**
* @brief 根据某一个关键字查询交易记录，返回记录指针
* @param[in] unsigned char rectype				要搜索的记录区
* @param[in] unsigned char search_tag_type		要匹配的关键字类型
* @param[in] unsigned char *in_tag				要匹配的关键字字符串
* @param[out] int	*index							返回该记录的索引值
* @return 搜索结果	0 没有搜索到记录		else 记录指针
* @note  实现的时候必须进行字符串比较
*/
unsigned char *rec_searchby_tag(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag, int *index)
{
	unsigned char *pBuf;

	pBuf = rec_module_buffer;
	*index = rec_search(rectype,search_tag_type,in_tag,pBuf);
	if (*index > 0)
	{
		return pBuf;
	}

	return (unsigned char *)0;
}

/**
* @brief 根据某一个关键字查询交易记录，返回记录指针
* @param[in] unsigned char rectype				要搜索的记录区
* @param[in] unsigned char search_tag_type		要匹配的关键字类型
* @param[in] unsigned char *in_tag				要匹配的关键字字符串
* @param[out] int	*index							返回该记录的索引值
* @return 搜索结果	0 没有搜索到记录		else 记录指针
* @note  实现的时候必须进行字符串比较
*/
unsigned char *rec_searchby_tag_ext(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag, unsigned int start,unsigned char direct,int *index)
{
	unsigned char *pBuf;

	pBuf = rec_module_buffer;
	*index = rec_search_ext(rectype,search_tag_type,in_tag,start,direct,pBuf);
	if (*index > 0)
	{
		return pBuf;
	}

	return (unsigned char *)0;
}


/**
* @brief 获取商品信息记录区中的某一个节点
* @param[in] unsigned char mode				ABSOLUTE_SEARCH:绝对位移搜索	RELATIVE_SEARCH:相对位移搜索
* @param[in] unsigned int index				记录索引	1:表示第一个节点 
* @note
*/
unsigned char *get_goodsnode(int index)
{
	unsigned char	dir_str[35];
	unsigned char	*pBuf;
	unsigned int	tmp;
	unsigned int	rec_cnt;

	if (index == 0)
	{
		return (unsigned char *)0;
	}

	pBuf = rec_module_buffer;
	rec_cnt = record_module_count(REC_TYPE_GOODS_LIST);

	if (index > rec_cnt)
	{
		return (unsigned char *)0;
	}

	strcpy(dir_str,goods_list_dirctory);
	strcat(dir_str,goods_list_file);
	if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return (unsigned char *)0;
	}

	f_lseek(&file1,(index-1)*sizeof(TGOODS_SPEC_RECORD));
	if (f_read(&file1,(void*)pBuf,512,&tmp) != FR_OK)
	{
		f_close(&file1);
		return (unsigned char *)0;
	}
	f_close(&file1);
	return pBuf;

	return (unsigned char *)0;

}

/**
* @brief 删除序列号采集记录文件或者无线采集条码记录文件中的某一个节点
* @param[in] unsigned int index	 物理偏移（索引）
*/
int delete_one_node(unsigned char rectype,unsigned int index)
{
	unsigned char	buffer[sizeof(TBATCH_NODE)];
	unsigned int	tmp,temp;
	unsigned short	prev,next;
	unsigned int	barcode_hash_prev,barcode_hash_next;
	unsigned int	hash_value,link_end;
	unsigned char	inf_file_str[35];
	unsigned int	node_size;
	unsigned int	log_data[8];

	//step1:先将该节点标记为无效节点
	if (rectype == REC_TYPE_BATCH)
	{
		strcpy(inf_file_str,batch_dirctory);
		strcat(inf_file_str,batch_list_file);
		node_size = sizeof(TBATCH_NODE);
	}
	else
	{
		return -1;
	}

	if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
	{
		return -2;
	}

	f_lseek(&file1,(index-1)*node_size);
	if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
	{
		f_close(&file1);
		return -3;
	}


	((TBATCH_NODE*)buffer)->flag = 0x55;

	prev = ((TBATCH_NODE*)buffer)->by_index_prev;
	next = ((TBATCH_NODE*)buffer)->by_index_next;

	barcode_hash_prev = 0;
	barcode_hash_next = 0;
	hash_value = 0;

	f_lseek(&file1,(index-1)*node_size);
	if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
	{
		f_close(&file1);
		return -4;
	}

	if (tmp != node_size)
	{
		f_close(&file1);
		return -5;
	}

	f_sync(&file1);

	log_data[0] = OP_TYPE_DEL_NODE;
	log_data[1] = rectype;
	log_data[2] = index;
	log_data[3] = prev;
	log_data[4] = next;
	log_data[5] = barcode_hash_prev;
	log_data[6] = barcode_hash_next;
	log_data[7] = hash_value;
	log_len = 0;
	if (save_log_file((void*)log_data,32))
	{
		return -6;
	}

	//step2:更新该节点前一个节点和后一个节点的链表信息
	if(prev)
	{
		f_lseek(&file1,(prev-1)*node_size);
		if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
		{
			f_close(&file1);
			return -7;
		}


		((TBATCH_NODE*)buffer)->by_index_next = next;

		calc_check_value(rectype,buffer);

		f_lseek(&file1,(prev-1)*node_size);
		if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -8;
		}

		if (tmp != node_size)
		{
			f_close(&file1);
			return -9;
		}

		f_sync(&file1);
	}

	if (next)
	{
		f_lseek(&file1,(next-1)*node_size);
		if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
		{
			f_close(&file1);
			return -10;
		}


		((TBATCH_NODE*)buffer)->by_index_prev = prev;

		calc_check_value(rectype,buffer);
		f_lseek(&file1,(next-1)*node_size);
		if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -11;
		}

		if (tmp != node_size)
		{
			f_close(&file1);
			return -12;
		}

		f_sync(&file1);
	}

	//可以将节点文件关闭了
	f_close(&file1);

	//step3:将删除的节点的偏移记录在信息文件中

	strcpy(inf_file_str,batch_dirctory);
	strcat(inf_file_str,batch_inf_file);

	if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
	{
		return -19;
	}

	f_lseek(&file1,file1.fsize);
	if (f_write(&file1,(void*)&index,4,&tmp) != FR_OK)
	{
		f_close(&file1);
		return -20;
	}

	if (tmp != 4)
	{
		f_close(&file1);
		return -21;
	}

	f_sync(&file1);

	//如果删除的节点是链表的首节点，那么还需要更新保存的链表首节点信息
	f_lseek(&file1,4);
	if ((f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)||(tmp != 4))
	{
		f_close(&file1);
		return -22;
	}

	if (temp == index)
	{
		f_lseek(&file1,4);
		temp = next;
		if (f_write(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -23;
		}

		if (tmp != 4)
		{
			f_close(&file1);
			return -24;
		}

		f_sync(&file1);
	}

	//如果删除的节点是链表的尾节点，那么还需要更新保存的链表尾节点信息
	f_lseek(&file1,8);
	if ((f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)||(tmp != 4))
	{
		f_close(&file1);
		return -25;
	}

	if (temp == index)
	{
		f_lseek(&file1,8);
		temp = prev;
		if (f_write(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -26;
		}

		if (tmp != 4)
		{
			f_close(&file1);
			return -27;
		}
	}

	f_close(&file1);

	clear_log_file(0);		//清除日志

	return 0;
}

#if 0
/**
* @brief 读取序列号节点
* @param[in] unsigned int mode  0:第一个有效节点   1;前一个有效节点   2:后一个有效节点  3:指定节点偏移
* @param[in] unsigned int offset  只有在mode = 3时才有用
*/
unsigned char* get_node(unsigned char rectype,unsigned int mode,unsigned int offset)
{
	unsigned int	node_offset,tmp;
	unsigned char	*pRec = 0;
	unsigned char   inf_file_str[35];
	if (mode == 0)
	{
		current_node_offset = 0;
		if (rectype == REC_TYPE_SERIAL_LIST)
		{
			strcpy(inf_file_str,serial_data_dirctory);
			strcat(inf_file_str,current_dj_dir);
			strcat(inf_file_str,serial_data_info_file);
		}
		else if (rectype == REC_TYPE_MULTCHK_LIST)
		{
			strcpy(inf_file_str,multi_check_dirctory);
			strcat(inf_file_str,multchk_data_info_file);
		}
		else if (rectype == REC_TYPE_MULTXLH_LIST)
		{
			strcpy(inf_file_str,serial_data_dirctory);
			strcat(inf_file_str,multxlh_data_info_file);
		}
		else
		{
			strcpy(inf_file_str,wireless_bargun_dirctory);
			strcat(inf_file_str,bargun_data_info_file);
		}

		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
		{
			return (unsigned char*)0;
		}

		f_lseek(&file1,8);
		if ((f_read(&file1,(void*)&node_offset,4,&tmp) != FR_OK)||(tmp != 4))
		{
			f_close(&file1);
			return (unsigned char*)0;
		}

		f_close(&file1);
	}
	else if (1 == mode)
	{
		node_offset = next_node_offset;
	}
	else if (2 == mode)
	{
		node_offset = prev_node_offset;
	}
	else if(3 == mode)
	{
		node_offset = offset;
	}
	else
	{
		return (unsigned char*)0;
	}

	if (node_offset)
	{
		current_node_offset = node_offset;
		if (rectype == REC_TYPE_SERIAL_LIST)
		{
			pRec = record_module_read(REC_TYPE_SERIAL_LIST,current_node_offset);
			prev_node_offset = ((SERIAL_DATA_NODE*)pRec)->by_index_prev;
			next_node_offset = ((SERIAL_DATA_NODE*)pRec)->by_index_next;
		}
		else if (rectype == REC_TYPE_MULTCHK_LIST)
		{
			pRec = record_module_read(REC_TYPE_MULTCHK_LIST,current_node_offset);
			prev_node_offset = ((MULTCHK_NAME*)pRec)->prev;
			next_node_offset = ((MULTCHK_NAME*)pRec)->next;
		}
		else if (rectype == REC_TYPE_MULTXLH_LIST)
		{
			pRec = record_module_read(REC_TYPE_MULTXLH_LIST,current_node_offset);
			prev_node_offset = ((MULTXLH_NAME*)pRec)->prev;
			next_node_offset = ((MULTXLH_NAME*)pRec)->next;
		}
		else
		{
			pRec = record_module_read(REC_TYPE_BARGUN,current_node_offset);
			prev_node_offset = ((TBARGUN_DATA_NODE*)pRec)->by_index_prev;
			next_node_offset = ((TBARGUN_DATA_NODE*)pRec)->by_index_next;
		}

	}

	return pRec;
}

/**
* @brief 获取序列号节点文件中物理偏移对应的逻辑偏移
* @param[in] unsigned int	physical_index
*/
unsigned int serail_data_PA_to_LA(unsigned int physical_index)
{
	unsigned int	node_offset,tmp;
	unsigned char   inf_file_str[35];
	unsigned char	*pRec;
	unsigned int	logical_index = 1;

	strcpy(inf_file_str,serial_data_dirctory);
	strcat(inf_file_str,current_dj_dir);
	strcat(inf_file_str,serial_data_info_file);


	if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}

	f_lseek(&file1,4);
	if ((f_read(&file1,(void*)&node_offset,4,&tmp) != FR_OK)||(tmp != 4))
	{
		f_close(&file1);
		return 0;
	}

	f_close(&file1);

	while(node_offset != physical_index)
	{
		pRec = record_module_read(REC_TYPE_SERIAL_LIST,node_offset);
		if (pRec)
		{
			node_offset = ((SERIAL_DATA_NODE*)pRec)->by_index_next;
			if (node_offset == 0)
			{
				return 0;
			}
		}
		else
		{
			return 0;
		}

		logical_index++;
	}

	return logical_index;
}
#endif
/**
* @brief 获取某一个文件的大小
* @param[in] const unsigned char *dir		文件的路径
*/
int get_file_size(const unsigned char *dir)
{
	int size;
	if (f_open(&file1,dir,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}

	size = file1.fsize;

	f_close(&file1);

	return size;
}

/**
* @brief 读取记录文件夹中的某一个文件
* @param[in] const unsigned char *dir		文件的路径
* @param[in] unsigned int	 offset			文件偏移
* @param[in] unsigned int	 len			数据长度
* @param[in] unsigned char *pdata			数据指针
* @return  < 0		读取文件失败
*          >= 0		读取成功,返回读取数据的实际长度
*/
int read_rec_file(const unsigned char *dir,unsigned int offset,unsigned int len,unsigned char *pdata)
{
	int	tmp;

	if (f_open(&file1,dir,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return -1;
	}

	if(file1.fsize < offset)
	{
		f_close(&file1);
		return -2;
	}

	f_lseek(&file1,offset);
	if (f_read(&file1,pdata,len,&tmp) != FR_OK)
	{
		f_close(&file1);
		return -3;
	}

	f_close(&file1);
	return tmp;
}

/**
* @brief 往记录文件夹中的某一个文件写入数据
* @param[in] const unsigned char *dir		文件的路径
* @param[in] unsigned int	 offset			文件偏移
* @param[in] unsigned int	 len			数据长度
* @param[in] unsigned char *pdata			数据指针
* @return  < 0		写入文件失败
*          >= 0		写入成功,返回写入数据的实际长度
*/
int write_rec_file(const unsigned char *dir,unsigned int offset,unsigned int len,unsigned char *pdata)
{
	int	tmp;

	if (f_open(&file1,dir,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -1;
	}

	if(file1.fsize < offset)
	{
		f_close(&file1);
		return -2;
	}

	f_lseek(&file1,offset);
	if (f_write(&file1,pdata,len,&tmp) != FR_OK)
	{
		f_close(&file1);
		return -3;
	}

	if (tmp != len)
	{
		f_close(&file1);
		return -3;
	}

	f_close(&file1);
	return tmp;
}

/**
* @brief 检查某一个记录的文件夹中的记录文件是否完整或者合法
* @param[in] const unsigned char rectype	记录类型
*/
int check_record_dir(unsigned char rectype)
{
	const unsigned char	*p_rec_file[4];
	const unsigned char *p_rec_dir;
	unsigned char dir_str[35];
	unsigned int	i,tmp;
	int				ret = 0;
	unsigned char   tmp_buffer[4];

	if (REC_TYPE_GOODS_LIST == rectype)
	{
		//检查商品信息文件夹中的记录文件
		p_rec_dir = goods_list_dirctory;

		p_rec_file[0] = barcode_hash_table_file;
		p_rec_file[1] = goods_list_file;
		p_rec_file[2] = 0;
	}
	else if (REC_TYPE_CHECK == rectype)
	{
		p_rec_dir = check_dirctory;
		p_rec_file[0] = barcode_hash_table_file;
		p_rec_file[1] = check_list_file;
		p_rec_file[2] = 0;
	}
	else
	{
		return 0;
	}

	i = 0;
	while(p_rec_file[i])
	{
		strcpy(dir_str,p_rec_dir);
		strcat(dir_str,p_rec_file[i]);

		if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
		{
			//记录文件不存在
			return -2;
		}

		if (REC_TYPE_GOODS_LIST == rectype)
		{
			if (i==0)
			{
				if (file1.fsize != 4*HASH_TABLE_SIZE)
				{
					ret = -3;		//hash表文件大小错误
				}
			}

			if(i == 1)
			{
				if ((file1.fsize % sizeof(TGOODS_SPEC_RECORD)) != 0)
				{
					ret = -5;		//商品信息记录文件不能被节点大小整除
				}
			}
		}
		else
		{
			if (i == 0)
			{
				if (file1.fsize != 4*HASH_TABLE_SIZE)
				{
					ret = -3;		//hash表文件大小错误
				}
			}

			if (i == 1)
			{
				if ((file1.fsize % sizeof(TCHECK_INFO_RECORD)) != 0) 
				{
					ret = -5;
				}
			}
		}

		f_close(&file1);

		if (ret)
		{
			return ret;
		}
		i++;
	}

	//上面检查完了记录文件夹中记录文件的存在以及每个记录文件的大小是否正确
	//还需要检查记录文件与各hash文件之间的关联性是否正确，以及记录文件内部链表的建立是否正确。
	//@todo.....

	return 0;
}


/**
***************************************************************************
*@brief	校验下载的升级文件是否正确
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
int check_updatefile(void)
{
	UINT							rd;
	int								i;
	int								j;
	unsigned int					code_sector,code_xor,xor;

	if( f_open(&file1, "/update.bin", FA_OPEN_EXISTING | FA_READ ) != FR_OK )
	{
		return -1;
	}

	if( f_read(&file1, rec_module_buffer, 512, &rd) != FR_OK )
	{
		f_close(&file1);
		return -1;
	}

	if( rec_module_buffer[0] != 'J' || rec_module_buffer[1] != 'B' || rec_module_buffer[2] != 'L' )
	{
		f_close(&file1);
		return -1;
	}

	code_sector						= ((TPackHeader*)rec_module_buffer)->length;
	code_xor						= ((TPackHeader*)rec_module_buffer)->xor_data;

	code_sector						/= 512;
	xor								= 0;
	for(i=0; i<code_sector; i++)
	{
		if( f_read(&file1, rec_module_buffer, 512, &rd) != FR_OK )
		{
			f_close(&file1);
			return -1;
		}

		for(j=0; j<128; j++)
		{
			xor						^= *((unsigned int*)&rec_module_buffer[j*4]);
		}
	}

	f_close(&file1);

	if(xor != code_xor)
	{
		return -1;
	}

	return 0;
}


/**
***************************************************************************
*@brief	校验下载的资源文件是否正确
*@param[in] 
*@return 
*@warning
*@see	
*@note 
***************************************************************************
*/
int check_resdatafile(void)
{
	UINT							rd;
//	int								i;
//	int								j;
//	unsigned int					code_sector,code_xor,xor;

	if( f_open(&file1, "/resdata.tmp", FA_OPEN_EXISTING | FA_READ ) != FR_OK )
	{
		return -1;
	}

	if( f_read(&file1, rec_module_buffer, 512, &rd) != FR_OK )
	{
		f_close(&file1);
		return -1;
	}

	if( rec_module_buffer[0] != 'J' || rec_module_buffer[1] != 'B' || rec_module_buffer[2] != 'L' || rec_module_buffer[3] != '3')
	{
		f_close(&file1);
		return -1;
	}

	//code_sector					= ((TPackHeader*)rec_module_buffer)->length;
	//code_xor						= ((TPackHeader*)rec_module_buffer)->xor_data;

	// 资源文件不能超过8M字节
	//if( code_sector >= 8 * 1024 * 1024 )
	//{
	//	f_close(&file1);
	//	return -3;
	//}

	//code_sector						/= 512;
	//xor								= 0;
	//for(i=0; i<code_sector; i++)
	//{
	//	if( f_read(&file1, rec_module_buffer, 512, &rd) != FR_OK )
	//	{
	//		f_close(&file1);
	//		return -1;
	//	}

	//	for(j=0; j<128; j++)
	//	{
	//		xor						^= *((unsigned int*)&rec_module_buffer[j*4]);
	//	}
	//}

	f_close(&file1);

	//if(xor != code_xor)
	//{
	//	return -1;
	//}

	return 0;
}


/**
***************************************************************************
*@brief 判断是否有应用升级文件的存在，如果有升级文件存在就删除应用升级文件
*@param[in] 
*@return 0 删除成功  else; 删除失败
*@warning
*@see	
*@note 
***************************************************************************
*/
int del_update_bin(void)
{
	if( f_open(&file1, "/update.bin", FA_OPEN_EXISTING | FA_READ ) != FR_OK )
	{
		return 0;
	}

	f_close(&file1);

	//应用升级文件存在，将升级文件删除
	if (f_unlink("/update.bin") != FR_OK)
	{
		return -1;	//删除旧的资源文件失败
	}

	return 0;
}

/**
***************************************************************************
*@brief 判断是否有资源升级文件的存在，如果有升级文件存在就删除资源升级文件
*@param[in] 
*@return 0 删除成功  else; 删除失败
*@warning
*@see	
*@note 
***************************************************************************
*/
int del_resdata_bin(void)
{
	if( f_open(&file1, "/resdata.tmp", FA_OPEN_EXISTING | FA_READ ) != FR_OK )
	{
		return 0;
	}

	f_close(&file1);

	//应用升级文件存在，将升级文件删除
	if (f_unlink("/resdata.tmp") != FR_OK)
	{
		return -1;	//删除旧的资源文件失败
	}

	return 0;
}


/**
* @brief ]恢复hash表的修改，包括恢复相应的节点文件的修改
* @param[in]
* @return 0:成功
*        -1:失败
*/
static int recover_hash_table_modify(unsigned int rectype,unsigned char hash_type,unsigned int hash_value,unsigned int linkend)
{
		unsigned char	dir_str[35],node_size;
		unsigned int	tmp;
		unsigned char   buffer[256];

		if (REC_TYPE_GOODS_LIST == rectype)
		{
			if (hash_type == 0)
			{
				strcpy(dir_str,goods_list_dirctory);
				strcat(dir_str,barcode_hash_table_file);
			}
			//else if (hash_type == 1)
			//{
			//	strcpy(dir_str,goods_list_dirctory);
			//	strcat(dir_str,name_hash_table_file);
			//}
			else
			{
				return -1;
			}
		}
		else if (REC_TYPE_CHECK == rectype)
		{
			if (hash_type == 0)
			{
				strcpy(dir_str,check_dirctory);
				strcat(dir_str,barcode_hash_table_file);
			}
			else
			{
				return -1;
			}
		}
		else if (REC_TYPE_BATCH == rectype)
		{
			if (hash_type == 0)
			{
				strcpy(dir_str,batch_dirctory);
				strcat(dir_str,batch_inf_file);
			}
			else
			{
				return -1;
			}
		}
		else 
		{
			return -1;
		}


		//打开HashTable文件
		if (f_open(&file2,dir_str,FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
		{
			return -3;
		}

		f_lseek(&file2,4*(hash_value%HASH_TABLE_SIZE));

		if (f_write(&file2,(void*)&linkend,4,&tmp) != FR_OK)
		{
			f_close(&file2);
			return -4;
		}

		if (tmp != 4)
		{
			f_close(&file2);
			return -4;
		}

		f_close(&file2);

		if(linkend)
		{
			//打开节点文件
			if (REC_TYPE_GOODS_LIST == rectype)
			{
				strcpy(dir_str,goods_list_dirctory);
				strcat(dir_str,goods_list_file);
				node_size = sizeof(TGOODS_SPEC_RECORD);
			}
			else if (REC_TYPE_CHECK == rectype)
			{
				strcpy(dir_str,check_dirctory);
				strcat(dir_str,check_list_file);
				node_size = sizeof(TCHECK_INFO_RECORD);
			}
			else if (REC_TYPE_BATCH == rectype)
			{
				strcpy(dir_str,batch_dirctory);
				strcat(dir_str,batch_list_file);
				node_size = sizeof(TBATCH_NODE);
			}
			

			if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
			{
				return -3;
			}

			if(file1.fsize == 0)
			{
				f_close(&file1);
				return 0;
			}

			f_lseek(&file1,(linkend-1)*node_size);

			if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
			{
				f_close(&file1);
				return -4;
			}

			if (REC_TYPE_GOODS_LIST == rectype)
			{
				if (((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next)
				{
					((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next = 0;
				}
			}
			else if ((REC_TYPE_CHECK == rectype))
			{
				if (((TCHECK_INFO_RECORD*)buffer)->by_barcode_next)
				{
					((TCHECK_INFO_RECORD*)buffer)->by_barcode_next = 0;
				}

			}
			else if ((REC_TYPE_BATCH == rectype))
			{
				if (((TBATCH_NODE*)buffer)->by_index_next)
				{
					((TBATCH_NODE*)buffer)->by_index_next = 0;
				}

			}
			calc_check_value((unsigned char)rectype,buffer);

			f_lseek(&file1,(linkend-1)*node_size);
			if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
			{
				f_close(&file1);
				return -5;
			}

			if (tmp != node_size)
			{
				f_close(&file1);
				return -5;
			}

			f_close(&file1);
		}

		return 0;
}



/**
* @brief 增加记录操作的恢复
* @param[in] 
* @return 0:成功
*        -1:失败
*/
static int record_add_recover(unsigned int rectype,unsigned int rec_offset,unsigned char *log_data,unsigned int log_data_len)
{
	unsigned char	dir_str[35];
	unsigned int	tmp;
	unsigned int	hash_value,linkend,check_value;
	if (log_data_len < 8)
	{
		return -1;
	}

	if (log_data_len == 8)
	{
		//只是记录了Barcode HashValue和相应链表的尾节点，那么Hashtable和节点文件都有可能已经改变了，将已经改变的恢复为原来的状况
		hash_value = *((unsigned int*)(log_data)); 
		linkend = *((unsigned int*)(log_data+4));
		if (recover_hash_table_modify(rectype,0,hash_value,linkend))
		{
			return -2;
		}
		return 0;
	}
	else
	{
		if(REC_TYPE_GOODS_LIST == rectype)
		{
			if (log_data_len == 16)
			{
				//恢复为原来的状况
				hash_value = *((unsigned int*)(log_data)); 
				linkend = *((unsigned int*)(log_data+4));
				if (recover_hash_table_modify(rectype,0,hash_value,linkend))
				{
					return -2;
				}

				hash_value = *((unsigned int*)(log_data+8)); 
				linkend = *((unsigned int*)(log_data+12));
				if (recover_hash_table_modify(rectype,1,hash_value,linkend))
				{
					return -3;
				}

				return 0;
			}
			else
			{
				if (REC_TYPE_GOODS_LIST == rectype)
				{
					if (log_data_len == 16+sizeof(TGOODS_SPEC_RECORD))
					{
						//可能已经写入了节点数据
						strcpy(dir_str,goods_list_dirctory);
						strcat(dir_str,goods_list_file);
						if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
						{
							return -4;
						}

						if (file1.fsize != rec_offset*sizeof(TGOODS_SPEC_RECORD))
						{
							//说明实际上没有写入节点
							f_lseek(&file1,file1.fsize);
							if (f_write(&file1,(void*)(log_data+16),sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
							{
								f_close(&file1);
								return -5;
							}

							if (tmp != sizeof(TGOODS_SPEC_RECORD))
							{	
								f_close(&file1);
								return -5;
							}
						}
						f_close(&file1);
						return 0;
					}
					else
					{	
						return -2;			//日志数据长度错误
					}
				}
			}
		}
		else if (rectype == REC_TYPE_CHECK)
		{
			if (log_data_len == 8+sizeof(TCHECK_INFO_RECORD))
			{
				//可能已经写入了节点数据

				strcpy(dir_str,check_dirctory);
				strcat(dir_str,check_list_file);
				if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
				{
					return -4;
				}

				if (file1.fsize != rec_offset*sizeof(TCHECK_INFO_RECORD))
				{
					//说明实际上没有写入节点
					f_lseek(&file1,file1.fsize);
					if (f_write(&file1,(void*)(log_data+8),sizeof(TCHECK_INFO_RECORD),&tmp) != FR_OK)
					{
						f_close(&file1);
						return -5;
					}

					if (tmp != sizeof(TCHECK_INFO_RECORD))
					{	
						f_close(&file1);
						return -5;
					}
				}

				f_close(&file1);
				return 0;
			}
			else
			{
				return -2;		//日志数据长度错误
			}
		}
		else if (rectype == REC_TYPE_BATCH)
		{
			if (log_data_len == 8+sizeof(TBATCH_NODE))
			{
				//可能已经写入了节点数据

				strcpy(dir_str,batch_dirctory);
				strcat(dir_str,batch_list_file);
				if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
				{
					return -4;
				}

				if (file1.fsize != rec_offset*sizeof(TBATCH_NODE))
				{
					//说明实际上没有写入节点
					f_lseek(&file1,file1.fsize);
					if (f_write(&file1,(void*)(log_data+8),sizeof(TBATCH_NODE),&tmp) != FR_OK)
					{
						f_close(&file1);
						return -5;
					}

					if (tmp != sizeof(TBATCH_NODE))
					{	
						f_close(&file1);
						return -5;
					}
				}

				f_close(&file1);
				return 0;
			}
			else
			{
				return -2;		//日志数据长度错误
			}
		}
	}

	return -1;
}



/**
* @brief 恢复被中断的清除所有记录操作
* @return 0：成功  -1：失败
*/
int record_clear_recover(unsigned int rectype)
{
	unsigned char dir_str[35];
	unsigned char dir_tmp[35];
	const unsigned char	*p_hash_table_file[3];
	const unsigned char	*target_dir;
	int i;

	if (rectype == REC_TYPE_GOODS_LIST)
	{
		target_dir = goods_list_dirctory;

		p_hash_table_file[0] = barcode_hash_table_file;
		p_hash_table_file[1] = 0;
	}
	else if (rectype == REC_TYPE_CHECK)
	{
		target_dir = check_dirctory;
		p_hash_table_file[0] = barcode_hash_table_file;
		p_hash_table_file[1] = 0;
	}
	else if (rectype == REC_TYPE_BATCH)
	{
		target_dir = batch_dirctory;
		p_hash_table_file[0] = batch_inf_file;
		p_hash_table_file[1] = 0;
	}
	else
	{
		return 0;
	}

	i = 0;
	while (p_hash_table_file[i])
	{
		strcpy(dir_str,target_dir);
		strcat(dir_str,p_hash_table_file[i]);

		//打开相应的hash_table文件
		if (f_open(&file2,(const char*)dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			return -3;
		}
		if (rectype == REC_TYPE_BATCH)
		{
			init_data_info_file(&file2);	
		}
		else
		{
			init_hash_table_file(&file2);	
		}
		

		f_close(&file2);
		i++;
	}
	return 0;
}

#if 0
/**
* @brief 恢复被中断的删除某一个节点的操作
* @return 0：成功  -1：失败
*/
static int del_one_node_recover(unsigned int rectype,unsigned int index,unsigned int prev,unsigned int next,unsigned int barcode_hash_prev,unsigned int barcode_hash_next,unsigned int hash_value)
{
	unsigned char	buffer[sizeof(SERIAL_DATA_NODE)];
	unsigned int	tmp,temp;
	unsigned char	inf_file_str[35];
	unsigned int	node_size;

	if (rectype == REC_TYPE_SERIAL_LIST)
	{
		strcpy(inf_file_str,serial_data_dirctory);
		strcat(inf_file_str,current_dj_dir);
		strcat(inf_file_str,serial_list_file);
		node_size = sizeof(SERIAL_DATA_NODE);
	}
	else if (rectype == REC_TYPE_BARGUN)
	{
		strcpy(inf_file_str,wireless_bargun_dirctory);
		strcat(inf_file_str,bargun_data_file);
		node_size = sizeof(TBARGUN_DATA_NODE);
	}
	else if (rectype == REC_TYPE_MULTCHK_LIST)
	{
		strcpy(inf_file_str,multi_check_dirctory);
		strcat(inf_file_str,multchk_list_file);
		node_size = sizeof(MULTCHK_NAME);
	}
	else if (rectype == REC_TYPE_MULTCHK_LIST)
	{
		strcpy(inf_file_str,serial_data_dirctory);
		strcat(inf_file_str,multxlh_list_file);
		node_size = sizeof(MULTXLH_NAME);
	}
	else
	{
		return -1;
	}

	//step2:更新该节点前一个节点和后一个节点的链表信息
	if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
	{
		return -2;
	}

	if(prev)
	{
		f_lseek(&file1,(prev-1)*node_size);
		if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
		{
			f_close(&file1);
			return -4;
		}

		if (rectype == REC_TYPE_SERIAL_LIST)
		{
			((SERIAL_DATA_NODE*)buffer)->by_index_next = next;
		}
		else if(rectype == REC_TYPE_MULTCHK_LIST)
		{
			((MULTCHK_NAME*)buffer)->next = next;
		}
		else if(rectype == REC_TYPE_MULTXLH_LIST)
		{
			((MULTXLH_NAME*)buffer)->next = next;
		}
		else //if(rectype == REC_TYPE_BARGUN)
		{
			((TBARGUN_DATA_NODE*)buffer)->by_index_next = next;
		}
		


		calc_check_value(rectype,buffer);

		f_lseek(&file1,(prev-1)*node_size);
		if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -5;
		}

		if (tmp != node_size)
		{
			f_close(&file1);
			return -5;
		}
	}

	if (next)
	{
		f_lseek(&file1,(next-1)*node_size);
		if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
		{
			f_close(&file1);
			return -6;
		}


		if (REC_TYPE_SERIAL_LIST == rectype)
		{
			((SERIAL_DATA_NODE*)buffer)->by_index_prev = prev;
		}
		else if(REC_TYPE_MULTCHK_LIST == rectype)
		{
			((MULTCHK_NAME*)buffer)->prev = prev;
		}
		else if(REC_TYPE_MULTXLH_LIST == rectype)
		{
			((MULTXLH_NAME*)buffer)->prev = prev;
		}
		else //if(rectype == REC_TYPE_BARGUN)
		{
			((TBARGUN_DATA_NODE*)buffer)->by_index_prev = prev;
		}
		

		calc_check_value(rectype,buffer);
		f_lseek(&file1,(next-1)*node_size);
		if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -7;
		}

		if (tmp != node_size)
		{
			f_close(&file1);
			return -7;
		}
	}

	if(barcode_hash_prev)
	{
		f_lseek(&file1,(barcode_hash_prev-1)*node_size);
		if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
		{
			f_close(&file1);
			return -4;
		}

		((SERIAL_DATA_NODE*)buffer)->by_keyproperty_next[0] = barcode_hash_next;

		calc_check_value(rectype,buffer);

		f_lseek(&file1,(barcode_hash_prev-1)*node_size);
		if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -5;
		}

		if (tmp != node_size)
		{
			f_close(&file1);
			return -5;
		}

		f_sync(&file1);
	}


	if (barcode_hash_next)
	{
		f_lseek(&file1,(barcode_hash_next-1)*node_size);
		if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK)||(tmp != node_size))
		{
			f_close(&file1);
			return -6;
		}

		((SERIAL_DATA_NODE*)buffer)->by_keyproperty_prev[0] = barcode_hash_prev;


		calc_check_value(rectype,buffer);
		f_lseek(&file1,(barcode_hash_next-1)*node_size);
		if (f_write(&file1,buffer,node_size,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -7;
		}

		if (tmp != node_size)
		{
			f_close(&file1);
			return -7;
		}
	}


	//可以将节点文件关闭了
	f_close(&file1);

	//step3:将删除的节点的偏移记录在信息文件中
	if (rectype == REC_TYPE_SERIAL_LIST)
	{
		strcpy(inf_file_str,serial_data_dirctory);
		strcat(inf_file_str,current_dj_dir);
		strcat(inf_file_str,serial_data_info_file);
	}
	else if (rectype == REC_TYPE_MULTCHK_LIST)
	{
		strcpy(inf_file_str,multi_check_dirctory);
		strcat(inf_file_str,multchk_data_info_file);
	}
	else if (rectype == REC_TYPE_MULTXLH_LIST)
	{
		strcpy(inf_file_str,serial_data_dirctory);
		strcat(inf_file_str,multxlh_data_info_file);
	}
	else //if(rectype == REC_TYPE_BARGUN)
	{ 
		strcpy(inf_file_str,wireless_bargun_dirctory);
		strcat(inf_file_str,bargun_data_info_file);
	}


	if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
	{
		return -8;
	}

	if (file1.fsize > 12)
	{
		f_lseek(&file1,file1.fsize-4);
		if (f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -9;
		}

		if (temp == index)
		{
			goto check_next;		//说明该删除的节点偏移已经保存在INF文件中了，不需要再保存一次
		}
	}

	f_lseek(&file1,file1.fsize);
	if (f_write(&file1,(void*)&index,4,&tmp) != FR_OK)
	{
		f_close(&file1);
		return -9;
	}

	if (tmp != 4)
	{
		f_close(&file1);
		return -9;
	}

	f_sync(&file1);

check_next:

	//如果删除的节点是链表的首节点，那么还需要更新保存的链表首节点信息
	f_lseek(&file1,4);
	if ((f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)||(tmp != 4))
	{
		f_close(&file1);
		return -10;
	}

	if (temp == index)
	{
		f_lseek(&file1,4);
		temp = next;
		if (f_write(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -11;
		}

		if (tmp != 4)
		{
			f_close(&file1);
			return -11;
		}

		f_sync(&file1);
	}

	//如果删除的节点是链表的尾节点，那么还需要更新保存的链表尾节点信息
	f_lseek(&file1,8);
	if ((f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)||(tmp != 4))
	{
		f_close(&file1);
		return -10;
	}

	if (temp == index)
	{
		f_lseek(&file1,8);
		temp = prev;
		if (f_write(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -11;
		}

		if (tmp != 4)
		{
			f_close(&file1);
			return -11;
		}
	}

	f_close(&file1);

	if (rectype == REC_TYPE_SERIAL_LIST)
	{
		//打开hash表文件，确定该节点是否是对应链表的尾节点，如果是尾节点，还需要更新hash表文件
		strcpy(inf_file_str,serial_data_dirctory);
		strcat(inf_file_str,current_dj_dir);
		strcat(inf_file_str,barcode_hash_table_file);

		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			return -12;
		}

		f_lseek(&file1,4*(hash_value%HASH_TABLE_SIZE));
		//获取链表的尾地址
		if (f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -13;
		}

		if (temp == index)
		{
			//如果该节点刚好是对应链表的尾节点，那么需要更新该链表对应的尾节点
			f_lseek(&file1,4*(hash_value%HASH_TABLE_SIZE));

			if (f_write(&file1,(void*)barcode_hash_prev,4,&tmp) != FR_OK)
			{
				f_close(&file1);
				return -14;
			}
		}

		f_close(&file1);
	}

	return 0;
}
#endif

static int add_goods_node_ext(FIL* node_file,TGOODS_SPEC_RECORD *p_goods_node,unsigned int key_hashvalue,unsigned int index,unsigned int saved_linkend)
{
#if 0
	unsigned char dir_str[35];
	unsigned int		  target_offset,link_end,tmp;

	strcpy(dir_str,goods_list_dirctory);
	strcat(dir_str,name_hash_table_file);
	target_offset = 4*(key_hashvalue%HASH_TABLE_SIZE);

	if (f_open(&file2,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
	{
		return -2;
	}

	f_lseek(&file2,target_offset);
	//获取链表的尾地址
	if (f_read(&file2,(void*)&link_end,4,&tmp) != FR_OK)
	{
		return -3;
	}

	//更新链表的尾地址
	f_lseek(&file2,target_offset);
	if (f_write(&file2,(void*)&index,4,&tmp) != FR_OK)
	{
		return -4;
	}

	if (tmp != 4)
	{
		return -4;
	}

	f_sync(&file2);

	if (link_end == 0)
	{
		//如果链表的尾地址为0
		p_goods_node->by_keyspec_next[1] = 0;
		p_goods_node->by_keyspec_prev[1] = 0;
		f_close(&file2);
	}
	else
	{
		f_close(&file2);
		if (link_end != index)
		{
			if(update_link_info_after_addNode(node_file,(unsigned char*)p_goods_node,1,2,link_end,index))
			{
				return -5;
			}
		}
		else
		{
			if(update_link_info_after_addNode(node_file,(unsigned char*)p_goods_node,1,2,saved_linkend,index))
			{
				return -5;
			}
		}
	}

	return 0;
#endif
}

/**
* @brief 恢复被中断的替换商品信息节点的操作
* @return 0：成功  -1：失败
*/
static int goods_node_replace_recover(unsigned int index,unsigned int old_hashvalue,unsigned int new_hashvalue,unsigned int saved_linkend,void* p_node)
{
	unsigned char		dir_str[35];
	unsigned int		tmp;

	strcpy(dir_str,goods_list_dirctory);
	strcat(dir_str,goods_list_file);

	if (f_open(&file1,(const char*)dir_str,FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
	{
		return -1;
	}

	if(delete_goods_node(&file1,2,(TGOODS_SPEC_RECORD*)p_node,old_hashvalue))
	{
		f_close(&file1);
		return -2;
	}

	if (add_goods_node_ext(&file1,(TGOODS_SPEC_RECORD*)p_node,new_hashvalue,index,saved_linkend))
	{
		f_close(&file1);
		return -3;
	}

	calc_check_value(REC_TYPE_GOODS_LIST,p_node);
	f_lseek(&file1,(index - 1)*sizeof(TGOODS_SPEC_RECORD));

	if (f_write(&file1,p_node,sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
	{
		f_close(&file1);
		return -3;
	}

	if (tmp != sizeof(TGOODS_SPEC_RECORD))
	{
		f_close(&file1);
		return -3;
	}

	f_close(&file1);
}

/**
* @brief 记录操作的恢复
* @param[in] unsigned char *precord 记录指针
* @return 0:成功
*        -1:失败
*/
static int record_recover(unsigned char *log_data,unsigned int log_data_len)
{
	unsigned int	op_type,rec_type,rec_offset,prev,next,saved_linkend;
	unsigned int	barcode_hash_prev,barcode_hash_next;

	op_type = *((unsigned int*)log_data);

	if (OP_TYPE_ADD_NODE == op_type)
	{
		if (log_data_len < 12)
		{
			return -2;		//日志数据长度错误
		}

		rec_type = *((unsigned int*)(log_data+4));
		rec_offset = *((unsigned int*)(log_data+8));

		if (log_data_len == 12)
		{
			//如果日志数据只有12字节，那么说明在增加记录时还没有对原来的记录文件做任何的变动，所以不需要任何的处理
			return 0;
		}

		//说明在记录增加过程中可能已经改变了部分记录数据，需要采取恢复措施
		if (record_add_recover(rec_type,rec_offset,log_data+12,log_data_len-12))
		{
			return -3;
		}
	}
	else if (OP_TYPE_CLEAR_NODE == op_type)
	{
		if ((log_data_len == 8)||(log_data_len == 12))
		{
			rec_type = *((unsigned int*)(log_data+4));

			if (record_clear_recover(rec_type))
			{
				return -3;
			}
		}
		else
		{
			return -2;	//日志数据长度错误
		}
	}
	else if (OP_TYPE_DEL_NODE == op_type)
	{
		//if ((log_data_len == 32)||(log_data_len == 36))
		//{
		//	rec_type = *((unsigned int*)(log_data+4));
		//	rec_offset = *((unsigned int*)(log_data+8));
		//	prev = *((unsigned int*)(log_data+12));
		//	next = *((unsigned int*)(log_data+16));
		//	barcode_hash_prev = *((unsigned int*)(log_data+20));
		//	barcode_hash_next = *((unsigned int*)(log_data+24));
		//	saved_linkend = *((unsigned int*)(log_data+28));
		//	if (del_one_node_recover(rec_type,rec_offset,prev,next,barcode_hash_prev,barcode_hash_next,saved_linkend))
		//	{
		//		return -3;
		//	}
		//}
		//else
		//{
		//	return -2;	//日志数据长度错误
		//}

	}
	else if (OP_TYPE_REPLACE_NODE == op_type)
	{
		if (log_data_len == 24+sizeof(TGOODS_SPEC_RECORD))
		{
			rec_type = *((unsigned int*)(log_data+4));
			rec_offset = *((unsigned int*)(log_data+8));
			prev = *((unsigned int*)(log_data+12));
			next = *((unsigned int*)(log_data+16));
			saved_linkend = *((unsigned int*)(log_data+20));
			
			if (rec_type != REC_TYPE_GOODS_LIST)
			{
				return -2;				//日志数据错误
			}

			if (goods_node_replace_recover(rec_offset,prev,next,saved_linkend,(void*)(log_data+24)))
			{
				return -3;
			}
		}
		else if (log_data_len == 24)
		{
			return 0;
		}
		else
		{
			return -2;	//日志数据长度错误
		}
	}
	//else
	//{
	//	return -1;
	//}

	return 0;
}


/**
***************************************************************************
*@brief 判断是否存在日志文件，如果有日志文件试图根据日志文件恢复被中断的操作或者回滚被中断的操作
*@param[in] 
*@return 0 成功  else; 失败
***************************************************************************
*/
int recover_record_by_logfile(void)
{
	unsigned int	logfile_len,offset,logitem_len;
	int				tmp;
	int				err_code = 0;
	unsigned char	log_bufffer[256];

	if (f_open(&file3,log_file,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -1;
	}
	
	logfile_len = file3.fsize;	//日志文件的长度
	offset = 0;
	while(logfile_len)
	{
		f_lseek(&file3,offset);
		if (f_read(&file3,(void*)&logitem_len,4,&tmp) != FR_OK)
		{
			err_code = -2;
			break;
		}

		if (tmp != 4)
		{
			err_code = -2;
			break;
		}

		//if ((logitem_len <= 4)||(logitem_len > 256))
		//{
		//	err_code = -3;
		//	if (logitem_len == 0)	//碰到一种情况，log文件本身大小为24字节，但是内容全部为0，怀疑是写log文件自身时断电造成的
		//	{
		//		err_code = 0;		
		//	}
		//	break;
		//}

		if (logitem_len != logfile_len)
		{
			err_code = 0;		//出现这种情况认为是在写LOG文件本身断电造成的
			break;
		}

		if (f_read(&file3,(void*)log_bufffer,logitem_len-4,&tmp) != FR_OK)
		{
			err_code = -4;
			break;
		}

		if (tmp != logitem_len-4)
		{
			err_code = -4;
			break;
		}

		if (record_recover(log_bufffer,logitem_len-4))
		{
			err_code = -5;
			break;
		}

		offset += logitem_len;
		logfile_len -= logitem_len;
	}

	if (err_code == 0)
	{
		f_lseek(&file3,0);
		f_truncate(&file3);		//清除日志文件
	}

	f_close(&file3);
	return err_code;
}

/**
***************************************************************************
*@brief 检查数据库文件是否存在
***************************************************************************
*/
int check_database(void)
{
	unsigned char		dir_str[35];

	strcpy(dir_str,goods_list_dirctory);
	strcat(dir_str,goods_list_file);

	if (f_open(&file2,dir_str,FA_READ | FA_OPEN_EXISTING) != FR_OK)
	{
		return -1;
	}

	if (file2.fsize == 0)
	{
		return -2;
	}

	f_close(&file2);

	strcpy(dir_str,goods_list_dirctory);
	strcat(dir_str,barcode_hash_table_file);

	if (f_open(&file2,dir_str,FA_READ | FA_OPEN_EXISTING) != FR_OK)
	{
		return -3;
	}

	if (file2.fsize !=  HASH_TABLE_SIZE*4)
	{
		return -4;
	}

	f_close(&file2);

	return 0;
}

/**
***************************************************************************
*@brief 单据搜索功能
*@param[in] unsigned char mode   低半字节  0: 完全匹配且只要匹配到一个就返回    1:部分匹配，且需要将匹配的单据索引返回
*								 高半字节  0: 表示搜索的是盘点单据				1:表示搜索的是序列号的单据
*@param[in] unsigned char *key_str  需要匹配的字符串
*@param[out] unsigned char *result_offset_buf  搜索到的单据偏移都按照搜到的先后顺序存放在此Buffer中，调用此函数者负责提供MULTI_DJ_MAX_CNT个字节的空间
*@return 返回匹配到记录数
@note mode = 0 时，实际上是检测一下是否有相同的单据，并不关心有几个相同的，相同的单据的偏移，所以传进来的result_offset_buf可以为0
      mode = 1时，需要搜索所有的可以匹配的单据，所以需要知道有几个匹配的单据，以及单据的偏移
***************************************************************************
*/
#if 0
unsigned char danju_search(unsigned char mode,unsigned char* key_str,unsigned char *result_offset_buf)
{
	unsigned char dir_str[35];
	unsigned char tmp_buffer[sizeof(MULTXLH_NAME)];		//sizeof(MULTXLH_NAME)  > sizeof(MULTCHK_NAME)
	unsigned int	tmp,link_header;
	unsigned char  cnt = 0;
	unsigned int  target_node_size;

	if ((mode & 0xf0) == 0)
	{
		strcpy(dir_str,multi_check_dirctory);
		strcat(dir_str,multchk_data_info_file);
		target_node_size = sizeof(MULTCHK_NAME);
	}
	else
	{
		strcpy(dir_str,serial_data_dirctory);
		strcat(dir_str,multxlh_data_info_file);
		target_node_size = sizeof(MULTXLH_NAME);
	}

	//step1:从信息文件中找到节点文件中节点链表的首地址
	if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}

	if (file1.fsize<12)
	{
		f_close(&file1);
		return 0;
	}

	f_lseek(&file1,8);
	if (f_read(&file1,(void*)&link_header,4,&tmp) != FR_OK)
	{
		f_close(&file1);
		return 0;
	}

	if (tmp != 4)
	{
		f_close(&file1);
		return 0;
	}

	f_close(&file1);

	if (link_header == 0)
	{
		return 0;
	}


	//step2: 打开节点文件，按照链表的顺序进行搜索
	if ((mode & 0xf0) == 0)
	{
		strcpy(dir_str,multi_check_dirctory);
		strcat(dir_str,multchk_list_file);
	}
	else
	{
		strcpy(dir_str,serial_data_dirctory);
		strcat(dir_str,multxlh_list_file);
	}

	if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return 0;
	}

	do 
	{
		f_lseek(&file1,(link_header-1)*target_node_size);
		if (f_read(&file1,(void*)&tmp_buffer,target_node_size,&tmp) != FR_OK)
		{
			break;
		}

		if (tmp != target_node_size)
		{
			break;
		}

		if (mode&0x0f)
		{
			if ((mode & 0xf0) == 0)
			{
				if (memcmp(key_str,((MULTCHK_NAME*)tmp_buffer)->listname,strlen(key_str)) == 0)
				{
					result_offset_buf[cnt++] = link_header;
				}
			}
			else
			{
				if (memcmp(key_str,((MULTXLH_NAME*)tmp_buffer)->listname,strlen(key_str)) == 0)
				{
					result_offset_buf[cnt++] = link_header;
				}
			}
		}
		else
		{
			if ((mode & 0xf0) == 0)
			{
				if (strcmp(key_str,((MULTCHK_NAME*)tmp_buffer)->listname) == 0)
				{
					//result_offset_buf[cnt++] = link_header;
					return 1;		//检测到相同的单据
				}
			}
			else
			{
				if (strcmp(key_str,((MULTXLH_NAME*)tmp_buffer)->listname) == 0)
				{
					//result_offset_buf[cnt++] = link_header;
					return 1;		//检测到相同的单据
				}
			}
		}


		if ((mode & 0xf0) == 0)
		{
			link_header = ((MULTCHK_NAME*)tmp_buffer)->prev;
		}
		else
		{
			link_header = ((MULTXLH_NAME*)tmp_buffer)->prev;
		}
	} while (link_header);

	f_close(&file1);

	return cnt;
}
#endif

#ifdef T5_SD_DEBUG

void debug_out(unsigned char* out,unsigned int len,unsigned char format)
{
	unsigned int write_num;
	unsigned char *pBuf;
	unsigned char data;

	if (0 == debug_file_status)
	{
		//创建DEBUG结果的输出文件
		if (f_open(&debug_file,"Debug.out",FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
		{
			return;
		}

		f_lseek(&debug_file,debug_file.fsize);
		debug_file_status = 1;
	}


	if (1 == debug_file_status)
	{
		if (format == 0)
		{
			//表明打印的数据是16进制数，需要转换为ASCII格式
			pBuf = (unsigned char*)Jmalloc(len*3);
			if (pBuf == NULL)
			{
				return;
			}

			for (write_num = 0;write_num < len;write_num++)
			{
				data = out[write_num];
				pBuf[write_num*3] = HexToAscii(data >> 4);
				pBuf[write_num*3+1] = HexToAscii(out[write_num]&0x0f);
				pBuf[write_num*3+2] = ' ';
			}

			if (f_write(&debug_file,pBuf,len*3,&write_num) != FR_OK)
			{
				Jfree(pBuf);
				f_close(&debug_file);
				debug_file_status = 0;
			}

			Jfree(pBuf);
		}
		else
		{
			//表明打印的数据是ASCII格式，不需要转换
			if (f_write(&debug_file,out,len,&write_num) != FR_OK)
			{
				f_close(&debug_file);
				debug_file_status = 0;
			}
		}

		f_sync(&debug_file);
	}
}


void delete_debug_file(void)
{
	debug_file_status = 0;
	if (f_open(&debug_file,"Debug.out",FA_OPEN_EXISTING | FA_WRITE) != FR_OK)
	{
		return;
	}
	
	f_lseek(&debug_file,0);
	f_truncate(&debug_file);

	f_close(&debug_file);
}

void close_debug_file(void)
{
	if (debug_file_status == 1)
	{
		f_close(&debug_file);
	}
}

#endif

#if 0
int check_serial_list_file(void)
{
#if 0
	unsigned char buf1[200],buf2[200];
	unsigned int tmp;

	if (f_open(&file3,"/T5_DB/serial/serial.lst",FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return -1;
	}

	if (file3.fsize >= 2*sizeof(SERIAL_DATA_NODE))
	{
		f_lseek(&file3,file3.fsize - 2*sizeof(SERIAL_DATA_NODE));
		if (f_read(&file3,(void*)buf1,sizeof(SERIAL_DATA_NODE),&tmp) != FR_OK)
		{
			f_close(&file3);
			return -1;
		}

		if (tmp != sizeof(SERIAL_DATA_NODE))
		{
			f_close(&file3);
			return -1;
		}

		if (f_read(&file3,(void*)buf2,sizeof(SERIAL_DATA_NODE),&tmp) != FR_OK)
		{
			f_close(&file3);
			return -1;
		}

		if (tmp != sizeof(SERIAL_DATA_NODE))
		{
			f_close(&file3);
			return -1;
		}

		if (memcmp(buf1,buf2,sizeof(SERIAL_DATA_NODE)) == 0)
		{
			f_close(&file3);
			return 1;
		}
	}

		f_close(&file3);

#endif

	unsigned int index;
	unsigned char *ptmp;

	if (f_open(&file3,"/T5_DB/serial/serial.lst",FA_OPEN_EXISTING | FA_READ) != FR_OK)
	{
		return -1;
	}

	index = file3.fsize/sizeof(SERIAL_DATA_NODE);
	f_close(&file3);

	ptmp = record_module_read(REC_TYPE_SERIAL_LIST,index);
	if (ptmp == 0)
	{

		return 1;
	}

	return 0;
}
#endif