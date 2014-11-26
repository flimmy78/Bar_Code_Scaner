/**
 * @file record.c
 * @brief T1Gen��Ŀ���׼�¼����ģ��
 * @version 2.0
 * @author joe
 * @date 2011��12��20��
 * @note ����T�������¼
 *
 * @version 1.0
 * @author joe
 * @date 2011��03��30��
 * @note ����ͨ�õļ�¼����ģ��ʵ�ֵ�
 * @copy
 *
 * �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
 * ����˾�������Ŀ����˾����һ��׷��Ȩ����
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

unsigned char			rec_module_buffer[RECORD_MAX_SIZE];	//��¼ģ��Ĺ���buffer	


unsigned int current_node_offset;
unsigned int g_rec_offset;

static unsigned int	prev_node_offset;
static unsigned int	next_node_offset;


DJ_CONTEXT danju_context;

static unsigned int log_len;

FIL						file2,file3;
DIR						dir;							//�ļ���


#ifdef T5_SD_DEBUG
FIL						debug_file;
unsigned int			debug_file_status;
#endif

extern FIL				file1;
extern FATFS			fs;
extern TTerminalPara	g_param;


/**
* @brief ��ʼ����ϣ���ļ�
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
* @brief ��ʼ�����к���Ϣ�ļ�
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
 * @brief ϵͳ�����м�¼ģ��ĳ�ʼ��
 * @return 0: OK   else: �������
 * @note: ����ֵ���ܺܺõĶ�λ������Ĵ�������λ�ã���������Ҫ���޸�
 */
int record_module_init(void)
{
	const unsigned char *tmp_dir[3];
	unsigned int		i,j;	
	unsigned char		dir_str[35];
	const unsigned char	*p_hash_table_file[3];

	//f_mount(0, &fs);										// װ���ļ�ϵͳ

	if( f_opendir(&dir,rec_file_dirctory) != FR_OK )
	{
		//�򿪼�¼�ļ�ʧ�ܻ����޷�����SD��,����Ǹ��ļ��в����ڣ���ô�ʹ���һ���µ��ļ���
		if (f_mkdir(rec_file_dirctory) != FR_OK)
		{
			//�޷�����SD��
			return 1;	
		}

		if( f_opendir(&dir,rec_file_dirctory) != FR_OK )
		{
			//�մ����ɹ��˻��򲻿����Ǿ͹����ˣ��������������𣿣�����
			return 1;
		}
	}
	
	//����Ϊֹ��������T5_DB�ļ��У���鱣����ּ�¼����Ӧ�ļ����Ƿ񶼴���
	
	tmp_dir[0] = goods_list_dirctory;
	tmp_dir[1] = check_dirctory;
	tmp_dir[2] = batch_dirctory;


	for (i = 0; i < 3;i++)
	{
		if( f_opendir(&dir,tmp_dir[i]) != FR_OK )
		{
			//����Ǹ��ļ��в����ڣ���ô�ʹ���һ���µ��ļ���
			if (f_mkdir(tmp_dir[i]) != FR_OK)
			{
				//�޷�����SD��
				return 1;	
			}
		}

		if(i == 0 || i == 1)
		{
			//�����̵���Ϣ���ļ�����Ҫ����һ��hash���ļ�
			p_hash_table_file[0] = barcode_hash_table_file;
			p_hash_table_file[1] = 0;
		}
		else
		{
			//�����ѻ���¼���ļ�����Ҫ����һ��INF�ļ�
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
 * @brief ��ָ����¼���ü�¼������ڣ��˺����в����ж�
 * @param[in] unsigned char rectype	 ��¼������
 * @param[in] int index ��¼������(���ϵļ�¼����������1)
 * @return ���ؼ�¼�ĵ�ַ
 */
unsigned char *record_module_read(unsigned char rectype, unsigned int index)
{
	unsigned char	dir_str[35];
	unsigned int	node_size;
	unsigned char	*pBuf;
	unsigned int	checkvalue;
	int		re_read_cnt = 3;		//���һ�ζ�ȡʧ�ܣ���ô�ض�3��
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
		//У��ֵ����
		f_close(&file1);
		return (unsigned char*)0;
	}

	f_close(&file1);
	return pBuf;
}

//����У��ֵ�����ȥ
static calc_check_value(unsigned char rectype,unsigned char *precord)
{
	unsigned int		checkvalue;

	//����˷ݼ�¼��У��ֵ���ŵ���¼����ǰ��4���ֽ�
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
* @brief ����һ���ڵ㵽��¼�ļ�֮����¼�¼�ļ�������
* @param[in] FIL *file						��¼�ļ�ָ��,�Ѿ��򿪵��ļ�
* @param[in] unsigned char	*p_node			Ҫ�����Ľڵ�ָ��
* @param[in] unsigned char	node_type		�ڵ�����		
* @param[in] unsigned char	key_type		�ؼ�������		1:barcode  2:name (���кŽڵ������1) 
* @param[in] unsigned int	header			�����׵�ַ
* @param[in] unsigned int	node_offset		Ҫ���ӽڵ��ƫ��
* @return 0:�ɹ�
*        else:ʧ��
* @note ֻ����Ʒ��Ϣ�ڵ���˫�������������ӽڵ�ʱ��Ҫά��˫����������ڵ㶼�ǵ�������
*/
static int  update_link_info_after_addNode(FIL *file,unsigned char*p_node, unsigned char node_type,unsigned char key_type,unsigned int header,unsigned int node_offset)
{
	unsigned int	rec_size,next_node_offset,current_offset,tmp;
	unsigned char	buffer[512];		//���һ���ڵ�Ĵ�С������512����ô����Ҫ���Ӵ���ʱ�ռ�

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
		f_lseek(file,(next_node_offset-1)*rec_size);		//�ļ�ָ�붨λ�������׼�¼��ƫ�ƴ���ע�������м�¼��ƫ���Ǹýڵ������ƫ��
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
			//��Ʒ�ڵ�
			next_node_offset = ((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next;
			if (next_node_offset == 0)
			{
				((TGOODS_SPEC_RECORD*)buffer)->by_barcode_next = node_offset;		//���������һ���ڵ��nextָ��ָ��ǰ�ڵ�
				((TGOODS_SPEC_RECORD*)p_node)->by_barcode_next = current_offset;	//����ǰ�ڵ��prevָ��ָ����������һ���ڵ�
				((TGOODS_SPEC_RECORD*)p_node)->by_barcode_next = 0;					//����ǰ�ڵ��nextָ����0
			}
		}
		else if(REC_TYPE_CHECK == node_type)
		{
			//�̵���Ϣ�ڵ�
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
	f_lseek(file,(current_offset-1)*rec_size);		//�ļ�ָ�����¶�λ���������һ���ڵ㴦
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


//��׼�����е��ļ������������Ϣ���浽��־�ļ�
//note : ����ÿһ�ֲ�����˵�����б��еĲ����ĺ��嶼����ͬ
//       op_type == OP_TYPE_ADD_NODE ʱ,		param1 ��ʾ��¼����, param2 ��ʾ�����ڵ��ƫ��  param3 ��ʾ�����ڵ�ĳ���  param4 ָ�������ڵ�����
//		 op_type == OP_TYPE_CLEAR_NODE ʱ,		param1 ��ʾ��¼����, param2 ������					param3 ������ param4 ������
//		 op_type == OP_TYPE_DEL_NODE ʱ,		param1 ��ʾ��¼����, param2 ��ʾҪɾ���ڵ��ƫ��	param3 ������ param4 ������
//		 op_type == OP_TYPE_REPLACE_NODE ʱ,	param1 ��ʾ��¼����, param2 ��ʾҪ�滻�ڵ��ƫ��	param3 ��ʾ�滻�ڵ�ĳ���  param4 ָ�����滻�ڵ�����
//		 op_type == OP_TYPE_WRITE_BIN_FILE ʱ,	param1 ��ʾ��¼����, param2 ��ʾ�ļ�����	param3 ������  param4 ������
static int save_log_file(void *data,unsigned int len)
{
	unsigned int tmp;
	unsigned int llen;
	unsigned char tmp_buffer[530];
	//���ò������浽��־�ļ���
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
	f_close(&file3);		//�ر���־�ļ�
	log_len = llen;
        return 0;
}


//�����־�ļ�������ӵ���־
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
* @brief ����һ����¼
* @param[in] unsigned char *precord ��¼ָ��
* @return 0:�ɹ�
*        else:ʧ��
* @note �˺����Ѿ����Ӷϵ籣��
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
	unsigned int		link_end;		//�����β��ַ
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

		hash_value[0] = HashString(((TGOODS_SPEC_RECORD*)precord)->bar_code,0);	//�������Ʒ�����hashֵ
		//hash_value[1] = HashString(((TGOODS_SPEC_RECORD*)precord)->name,0);		//�������Ʒ���Ƶ�hashֵ
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
		hash_value[0] = HashString(((TCHECK_INFO_RECORD*)precord)->bar_code,0);	//�������Ʒ�����hashֵ
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
		hash_value[0] = 2;	//�������Ʒ�����hashֵ
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
			if ((f_read(&file1,(void*)&invalid_node_offset,4,&tmp) != FR_OK) ||(tmp != 4))		//��ȡ���������к���Ϣ�ļ��е�ĳһ���Ѿ���ɾ���ڵ��ƫ��
			{
				f_close(&file1);
				return -4;
			}
		}

		f_close(&file1);
	}

	//�򿪱�����Ӧ��¼�Ľڵ��ļ�
	if (f_open(&file1,(const char*)dir_str,FA_OPEN_ALWAYS | FA_READ | FA_WRITE) != FR_OK)
	{
		return -2;
	}

	if (invalid_node_offset)
	{
		rec_offset = invalid_node_offset;		//������������кŽڵ㣬���Ҽ�¼�ļ��д����Ѿ���ɾ���Ľڵ㣬��ô�����Ľڵ�
	}
	else
	{
		rec_offset = file1.fsize/node_size;		//��ȡ��¼�ļ���ǰ�Ѿ�����ļ�¼��
		if (rec_offset < target_max_cnt)
		{
			rec_offset += 1;	
		}
		else
		{
			//rec_offset = 1;		//�����¼���ﵽ������ֵ����ô�޷������¼�¼
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


	if (save_log_file((void*)log_data,12))		//״̬0��LOG�ļ���ֻ�����˲������͡���¼���͡�Ŀ���¼ƫ��
	{
		err_code = -10;
		goto err_handle;
	}

	//����˼�¼��Ҫ����Ĺؼ��ֵ�hashֵ
	i = 0;
	while (p_hash_table_file[i])
	{
		strcpy(dir_str,target_dir);
		strcat(dir_str,p_hash_table_file[i]);

		//����Ӧ��hash_table�ļ�
		if (f_open(&file2,(const char*)dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			err_code = -4;
			goto err_handle;
		}

		f_lseek(&file2,4*(hash_value[i]%HASH_TABLE_SIZE));		//��λ����hashֵ��hash���ж�Ӧ��ƫ�ƴ�

		//��ȡ��hashֵ��Ӧ�������β��ַ
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
		if(save_log_file((void*)log_data,8))		//״̬1����״̬2
		{
			err_code = -10;
			goto err_handle;
		}
		
		f_lseek(&file2,4*(hash_value[i]%HASH_TABLE_SIZE));		//��λ����hashֵ��hash���ж�Ӧ��ƫ�ƴ�

		//���¸������β�ڵ�
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
				//���������׽ڵ㻹��0����ô��Ҫ����������׽ڵ�
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

		f_close(&file2);	//hash���ļ����Թر���

		if (link_end)
		{
			//��������ǿյģ���ô��Ҫ�����������Ϣ(�������Ʒ��Ϣ�ڵ㣬����Ҫ���µ�ǰ�ڵ����Ϣ����Ϊ��Ʒ��Ϣ������˫������)
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

	f_lseek(&file1,(rec_offset-1)*node_size);		//���ļ�ָ���Ƶ��ļ�ĩβ
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

	//����ڵ���ļ����Թر���
	f_close(&file1);	

	if (invalid_node_offset)
	{
		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_WRITE | FA_READ) != FR_OK)
		{
			return -2;
		}

		f_lseek(&file1,file1.fsize-4);
		f_truncate(&file1);		//����¼����Ϣ�ļ��е��Ѿ�ɾ���Ľڵ�ƫ�������
		f_close(&file1);
	}
	
	clear_log_file(0);
	return 0;

err_handle:

	f_close(&file1);
	return err_code;

}

/**
* @brief ����һ����¼
* @param[in] unsigned char *precord ��¼ָ��
* @param[in] unsigned char open_mode �Ƕ���Ҫ���´��ļ�		0:��Ҫ  else:����Ҫ
* @param[in] unsigned char close_mode �Ƿ���Ҫ�رղ����ļ�		0:��Ҫ  else:����Ҫ
* @return 0:�ɹ�
*        -1:ʧ��
* @note �˺�����ʱû�п��Ƕϵ籣����������Ҫ���Ӷϵ籣������
*/
int record_add_ext(unsigned char rectype,unsigned char *precord,unsigned char open_mode,unsigned char close_mode)
{
	//
}

/**
* @brief ��һ����Ʒ��Ϣ�ڵ���ӵ���Ӧ������
* @param[in] FIL* node_file							�ýڵ����ڵ��ļ����Ѿ�����open��״̬��
* @param[in] unsigned char link_type				��������	1:��������	2:�������� 
* @param[in] TGOODS_SPEC_RECORD *p_TGOODS_SPEC_RECORD				����Ʒ�ڵ��ָ��
* @param[in] unsigned int key_hashvalue				�ýڵ���Ӧ�ؼ��ֵ�hashֵ	
* @return 0:�ɹ�
*        -1:ʧ��
* @note  �˺���ֻ֧�������Ʒ��Ϣ�ڵ�
*	     node_file �Ѿ����ڴ򿪵�״̬
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
	//��ȡ�����β��ַ
	if (f_read(&file2,(void*)&link_end,4,&tmp) != FR_OK)
	{
		return -3;
	}

	//���������β��ַ
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
		//��������β��ַΪ0
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
* @brief ��һ����Ʒ��Ϣ�ڵ��ĳһ��������ɾ��
* @param[in] FIL* node_file							�ýڵ����ڵ��ļ����Ѿ�����open��״̬��
* @param[in] unsigned char link_type				��������	1:��������	2:��������
* @param[in] TGOODS_SPEC_RECORD *p_TGOODS_SPEC_RECORD				����Ʒ�ڵ��ָ��
* @param[in] unsigned int key_hashvalue				�ýڵ���Ӧ�ؼ��ֵ�hashֵ	
* @return 0:�ɹ�
*        -1:ʧ��
* @note  �˺���ֻ֧��ɾ����Ʒ��Ϣ����
*	     node_file �Ѿ����ڴ򿪵�״̬
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
			//˵���ýڵ�����Ӧ��������β�ڵ㣬��ô��Ҫ������Ӧ����hash_table�ļ�
			target_hash_file = barcode_hash_table_file;
			new_link_end = p_goods_node->by_barcode_prev;	//�µ�����β��ַָ��
			p_goods_node->by_barcode_prev = 0;					//����ǰ�ڵ��nextָ����0
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

			//�����˸ýڵ�ĺ�һ���ڵ��prevָ��
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
			//����ýڵ㻹��ǰһ���ڵ㣬��ô����next�ڵ�Ҳ��Ҫ����
			f_lseek(node_file,(p_goods_node->by_barcode_prev - 1)*sizeof(TGOODS_SPEC_RECORD));

			if (f_read(node_file,tmp_buffer,sizeof(TGOODS_SPEC_RECORD),&tmp) != FR_OK)
			{
				return -8;
			}

			((TGOODS_SPEC_RECORD*)tmp_buffer)->by_barcode_next = p_goods_node->by_barcode_next;
			calc_check_value(REC_TYPE_GOODS_LIST,tmp_buffer);

			f_lseek(node_file,(p_goods_node->by_barcode_prev - 1)*sizeof(TGOODS_SPEC_RECORD));

			//�����˸ýڵ��ǰһ���ڵ��nextָ��
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

		//����Ϊֹ�����Խ��ýڵ������ǰ��ڵ�ָ�������
		p_goods_node->by_barcode_prev = 0;
		p_goods_node->by_barcode_next = 0;
	}
	else
	{
		return -1;
	}

	if (target_hash_file)
	{
		//��Ҫ������Ӧ��hash_table�ļ�
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
* @brief ȡ��ĳһ����¼
* @param[in] unsigned char rectype					��¼����
* @param[in] int			index					��¼����
* @param[in] unsigned char *p_new_record			�¼�¼
* @param[in] unsigned char *key_value_change_tbl	��¼�滻ʱ���ؼ��ַ����仯���б�
* @return 0:�ɹ�
*        -1:ʧ��
* @note  ����滻��¼���ڵ㣩ʱ���µĽڵ�Ĺؼ��ַ����˱仯����ô��Ҫ���Ѿ��޸ĵĹؼ��ֵ���Ӧ�������һ�ű���������0����
*		 �ؼ��ı�����£�1: ��ʾ������ؼ��֣�ʵ��������ؼ��ֲ����ܷ����仯��;
*						 2: ���ƹؼ���
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
			//�йؼ��ַ����仯����ô��Ҫ���¸ýڵ������������Ϣ
			i = 0;
			while(modify_info_tbl[i].keyspec_type)
			{
				switch (modify_info_tbl[i].keyspec_type)
				{
				case TAG_NAME:
					//��ԭ����������ɾ�������ӵ��µ�������

					//������־�ļ�
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
					//��ȡ�����β��ַ
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
		clear_log_file(0);		//�����־
	}
	
	return 0;
}


/**
* @brief �õ���¼������
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
			cnt -= (file1.fsize - 12)/4;		//��ȥ��Ч�Ľڵ㣬��������Ч�Ľڵ��ƫ�ƶ���¼����Ϣ�ļ��У�����ֱ�Ӽ�ȥ����Ϣ�ļ��м�¼����Ч�ڵ����������ʾ�ڽڵ��ļ��л����ڵĽڵ������
			f_close(&file1);
		}
		else
		{
			//������Ӧ�ò�����������������һ��������أ����꣬��ֻ����ɱ����ˣ�ֱ�ӽ���¼�ļ�ȫ����ʼ��
			f_close(&file1);
			record_clear(rectype);
			cnt = 0;
		}
	}
	return cnt;
}

/**
* @brief ������м�¼
* @return 0���ɹ�  -1��ʧ��
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

	f_lseek(&file1,0);		//�ļ�ָ���ƶ����ļ���ʼ

	//���ļ��ضϵ��ļ���ʼ
	if (f_truncate(&file1) != FR_OK)
	{
		f_close(&file1);
		return -2;
	}

	f_close(&file1);



	log_data[0] = OP_TYPE_CLEAR_NODE;
	log_data[1] = rectype;
	log_len = 0;
	if (save_log_file((void*)log_data,8))		//״̬0��LOG�ļ���ֻ�����˲������͡���¼����
	{
		return -4;
	}

	i = 0;
	while (p_hash_table_file[i])
	{
		strcpy(dir_str,target_dir);
		strcat(dir_str,p_hash_table_file[i]);

		//����Ӧ��hash_table�ļ�
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

	clear_log_file(0);		//�����־
	return 0;
}

/**
* @brief ����ĳһ���ؼ��ֲ�ѯ���׼�¼�����ؼ�¼�ڵ��ƫ��
* @param[in] unsigned char rectype				Ҫ�����ļ�¼��
* @param[in] unsigned char search_tag_type		Ҫƥ��Ĺؼ�������
* @param[in] unsigned char *in_tag				Ҫƥ��Ĺؼ����ַ���
* @param[out] unsigned char *outbuf				�������ļ�¼������
* @return �������	=0:û���������ý��׼�¼  > 0:�������ļ�¼��ƫ��   < 0: ��������
* @note  
*/
static int rec_search(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag,unsigned char *outbuf)
{
	unsigned int hash_value,link_end,tmp;
	unsigned char dir_str[35];
	unsigned char	dir_tmp[35];
	unsigned char	buffer[512];		//���һ���ڵ�Ĵ�С������512����ô����Ҫ���Ӵ���ʱ�ռ�
	const unsigned char	*	target_index_file;
	const unsigned char	*	target_directory;
	const unsigned char	*	target_node_file;
	unsigned int	node_size;				//�ڵ��С
	unsigned int	target_offset;			//�ڵ���Ҫ��ؼ���ƥ����ַ���λ�ڽڵ�ṹ���е�ƫ��
	unsigned int	target_prev_offset;		//�ýڵ��Ӧ�ؼ��ֵ�����ָ����GOOD_NODE�ṹ���������by_barcode_next��ƫ��
	//ע�⣺��������ֵ��ṹ���з����ÿ��������ռ�Ĵ�С������أ���������ṹ���޸ģ��˺�������Щ�ط�Ҫ����Ӧ���޸�	

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
			return -1;		//��������
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
			target_offset = 4;						//������λ�ڲ�����¼�ڵ��¼��ʼλ�õ�ƫ��4�ֽڴ�											
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

	memset(outbuf,0,node_size);		//����������ݵĻ�����
	strcpy(dir_str,target_directory);


	//���Ҫ�����Ĺؼ�������������

	//step1: ����ؼ��ֵ�hashֵ
	hash_value = HashString(in_tag,0);	

	//step2: ����hashֵ���ҵ���ùؼ��־�����ͬhashֵģֵ������β��ַ
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

	//���ݶ�ȡ�����������׵�ַ���ҵ���һ����ؼ�����ȫƥ��ļ�¼�������Ҫ����ȫƥ��ؼ��ֵ����м�¼ȫ������ȡ��������Ҫ����Ľӿ���ʵ�֡�

	if (link_end)
	{	
		strcpy(dir_str,target_directory);
		strcat(dir_str,target_node_file);

		//����Ʒ�嵥�ļ�
		if (f_open(&file1,dir_str,FA_OPEN_ALWAYS | FA_READ) != FR_OK)
		{
			return -3;
		}

		while (link_end)
		{
			f_lseek(&file1,(link_end-1)*node_size);		//�ļ�ָ��ָ��ýڵ����ʼλ��

			if ((f_read(&file1,buffer,node_size,&tmp) != FR_OK) || (node_size != tmp))
			{
				return -4;
			}

			if (strcmp(in_tag,buffer+target_offset) == 0)
			{
				memcpy(outbuf,buffer,node_size);
				return link_end;		//�����������е�һ����ؼ���ƥ��Ľڵ�
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

	return 0;		//û����������ؼ���ƥ��ļ�¼
}

/**
* @brief ���ݹؼ����ڼ�¼��������֮��ƥ��ļ�¼����
* @param[in] unsigned char rec_type		��¼����
* @param[in] unsigned char tag_typa		�����Ĺؼ�������
* @param[in] unsigned char * intag		�����Ĺؼ����ַ���
* @param[in] unsigned char	inlen		�ؼ��ֳ���
* @return �����������Ľ��׼�¼��������  -1 ����ʾû���ҵ���Ӧ�Ľ��׼�¼   else: �������Ľ��׼�¼��������
* @note  �ַ����Ƚ�
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
		return (unsigned char*)0;		//��������
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

//��֪�������׵�ַ��ǰ���£�����ĳһ���ؼ�����������Ľڵ�
//@note ������׵�ַ����������Χ֮�ڡ�
//      ������¼��֧�ַ�������
static int rec_search_ext(unsigned char rectype,unsigned char search_tag_type,unsigned char *in_tag,unsigned int start,unsigned char direct,unsigned char *outbuf)
{
	unsigned int  tmp;
	unsigned char dir_str[35];
	unsigned char dir_tmp[35];
	unsigned char	buffer[512];		//���һ���ڵ�Ĵ�С������512����ô����Ҫ���Ӵ���ʱ�ռ�
	const unsigned char	*	target_directory;
	const unsigned char	*	target_node_file;
	unsigned int	node_size;				//�ڵ��С
	unsigned int	target_offset;			//�ڵ���Ҫ��ؼ���ƥ����ַ���λ�ڽڵ�ṹ���е�ƫ��
	unsigned int	target_next_offset;		//�ýڵ��Ӧ�ؼ��ֵ�����ָ����GOOD_NODE�ṹ���������by_barcode_next��ƫ��
	//ע�⣺��������ֵ��ṹ���з����ÿ��������ռ�Ĵ�С������أ���������ṹ���޸ģ��˺�������Щ�ط�Ҫ����Ӧ���޸�	

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
			return -1;		//��������
		}
	}
	else if (REC_TYPE_CHECK== rectype)
	{
		node_size = sizeof(TCHECK_INFO_RECORD);
		target_node_file = check_list_file;
		target_directory = check_dirctory;

		if (search_tag_type == TAG_BARCODE)
		{
			target_offset = 4;						//������λ�ڲ�����¼�ڵ��¼��ʼλ�õ�ƫ��4�ֽڴ�											
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

	//memset(outbuf,0,node_size);		//����������ݵĻ�����
	//strcpy(dir_str,target_directory);

	if (start)
	{	
		strcpy(dir_str,target_directory);
		strcat(dir_str,target_node_file);

		//����Ʒ�嵥�ļ�
		if (f_open(&file1,dir_str,FA_OPEN_ALWAYS | FA_READ) != FR_OK)
		{
			return -3;
		}

		f_lseek(&file1,(start-1)*node_size);		//�ļ�ָ��ָ��ýڵ����ʼλ��

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
				f_lseek(&file1,(start-1)*node_size);		//�ļ�ָ��ָ��ýڵ����ʼλ��

				if (f_read(&file1,buffer,node_size,&tmp) != FR_OK)
				{
					return -4;
				}

				if (strcmp(in_tag,buffer+target_offset) == 0)
				{
					memcpy(outbuf,buffer,node_size);
					return start;		//�����������е�һ����ؼ���ƥ��Ľڵ�
				}
			}
		}
	}

	return 0;		//û����������ؼ���ƥ��ļ�¼
}


/**
* @brief ����ĳһ���ؼ��ֲ�ѯ���׼�¼�����ؼ�¼ָ��
* @param[in] unsigned char rectype				Ҫ�����ļ�¼��
* @param[in] unsigned char search_tag_type		Ҫƥ��Ĺؼ�������
* @param[in] unsigned char *in_tag				Ҫƥ��Ĺؼ����ַ���
* @param[out] int	*index							���ظü�¼������ֵ
* @return �������	0 û����������¼		else ��¼ָ��
* @note  ʵ�ֵ�ʱ���������ַ����Ƚ�
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
* @brief ����ĳһ���ؼ��ֲ�ѯ���׼�¼�����ؼ�¼ָ��
* @param[in] unsigned char rectype				Ҫ�����ļ�¼��
* @param[in] unsigned char search_tag_type		Ҫƥ��Ĺؼ�������
* @param[in] unsigned char *in_tag				Ҫƥ��Ĺؼ����ַ���
* @param[out] int	*index							���ظü�¼������ֵ
* @return �������	0 û����������¼		else ��¼ָ��
* @note  ʵ�ֵ�ʱ���������ַ����Ƚ�
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
* @brief ��ȡ��Ʒ��Ϣ��¼���е�ĳһ���ڵ�
* @param[in] unsigned char mode				ABSOLUTE_SEARCH:����λ������	RELATIVE_SEARCH:���λ������
* @param[in] unsigned int index				��¼����	1:��ʾ��һ���ڵ� 
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
* @brief ɾ�����кŲɼ���¼�ļ��������߲ɼ������¼�ļ��е�ĳһ���ڵ�
* @param[in] unsigned int index	 ����ƫ�ƣ�������
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

	//step1:�Ƚ��ýڵ���Ϊ��Ч�ڵ�
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

	//step2:���¸ýڵ�ǰһ���ڵ�ͺ�һ���ڵ��������Ϣ
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

	//���Խ��ڵ��ļ��ر���
	f_close(&file1);

	//step3:��ɾ���Ľڵ��ƫ�Ƽ�¼����Ϣ�ļ���

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

	//���ɾ���Ľڵ���������׽ڵ㣬��ô����Ҫ���±���������׽ڵ���Ϣ
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

	//���ɾ���Ľڵ��������β�ڵ㣬��ô����Ҫ���±��������β�ڵ���Ϣ
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

	clear_log_file(0);		//�����־

	return 0;
}

#if 0
/**
* @brief ��ȡ���кŽڵ�
* @param[in] unsigned int mode  0:��һ����Ч�ڵ�   1;ǰһ����Ч�ڵ�   2:��һ����Ч�ڵ�  3:ָ���ڵ�ƫ��
* @param[in] unsigned int offset  ֻ����mode = 3ʱ������
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
* @brief ��ȡ���кŽڵ��ļ�������ƫ�ƶ�Ӧ���߼�ƫ��
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
* @brief ��ȡĳһ���ļ��Ĵ�С
* @param[in] const unsigned char *dir		�ļ���·��
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
* @brief ��ȡ��¼�ļ����е�ĳһ���ļ�
* @param[in] const unsigned char *dir		�ļ���·��
* @param[in] unsigned int	 offset			�ļ�ƫ��
* @param[in] unsigned int	 len			���ݳ���
* @param[in] unsigned char *pdata			����ָ��
* @return  < 0		��ȡ�ļ�ʧ��
*          >= 0		��ȡ�ɹ�,���ض�ȡ���ݵ�ʵ�ʳ���
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
* @brief ����¼�ļ����е�ĳһ���ļ�д������
* @param[in] const unsigned char *dir		�ļ���·��
* @param[in] unsigned int	 offset			�ļ�ƫ��
* @param[in] unsigned int	 len			���ݳ���
* @param[in] unsigned char *pdata			����ָ��
* @return  < 0		д���ļ�ʧ��
*          >= 0		д��ɹ�,����д�����ݵ�ʵ�ʳ���
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
* @brief ���ĳһ����¼���ļ����еļ�¼�ļ��Ƿ��������ߺϷ�
* @param[in] const unsigned char rectype	��¼����
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
		//�����Ʒ��Ϣ�ļ����еļ�¼�ļ�
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
			//��¼�ļ�������
			return -2;
		}

		if (REC_TYPE_GOODS_LIST == rectype)
		{
			if (i==0)
			{
				if (file1.fsize != 4*HASH_TABLE_SIZE)
				{
					ret = -3;		//hash���ļ���С����
				}
			}

			if(i == 1)
			{
				if ((file1.fsize % sizeof(TGOODS_SPEC_RECORD)) != 0)
				{
					ret = -5;		//��Ʒ��Ϣ��¼�ļ����ܱ��ڵ��С����
				}
			}
		}
		else
		{
			if (i == 0)
			{
				if (file1.fsize != 4*HASH_TABLE_SIZE)
				{
					ret = -3;		//hash���ļ���С����
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

	//���������˼�¼�ļ����м�¼�ļ��Ĵ����Լ�ÿ����¼�ļ��Ĵ�С�Ƿ���ȷ
	//����Ҫ����¼�ļ����hash�ļ�֮��Ĺ������Ƿ���ȷ���Լ���¼�ļ��ڲ�����Ľ����Ƿ���ȷ��
	//@todo.....

	return 0;
}


/**
***************************************************************************
*@brief	У�����ص������ļ��Ƿ���ȷ
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
*@brief	У�����ص���Դ�ļ��Ƿ���ȷ
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

	// ��Դ�ļ����ܳ���8M�ֽ�
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
*@brief �ж��Ƿ���Ӧ�������ļ��Ĵ��ڣ�����������ļ����ھ�ɾ��Ӧ�������ļ�
*@param[in] 
*@return 0 ɾ���ɹ�  else; ɾ��ʧ��
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

	//Ӧ�������ļ����ڣ��������ļ�ɾ��
	if (f_unlink("/update.bin") != FR_OK)
	{
		return -1;	//ɾ���ɵ���Դ�ļ�ʧ��
	}

	return 0;
}

/**
***************************************************************************
*@brief �ж��Ƿ�����Դ�����ļ��Ĵ��ڣ�����������ļ����ھ�ɾ����Դ�����ļ�
*@param[in] 
*@return 0 ɾ���ɹ�  else; ɾ��ʧ��
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

	//Ӧ�������ļ����ڣ��������ļ�ɾ��
	if (f_unlink("/resdata.tmp") != FR_OK)
	{
		return -1;	//ɾ���ɵ���Դ�ļ�ʧ��
	}

	return 0;
}


/**
* @brief ]�ָ�hash����޸ģ������ָ���Ӧ�Ľڵ��ļ����޸�
* @param[in]
* @return 0:�ɹ�
*        -1:ʧ��
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


		//��HashTable�ļ�
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
			//�򿪽ڵ��ļ�
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
* @brief ���Ӽ�¼�����Ļָ�
* @param[in] 
* @return 0:�ɹ�
*        -1:ʧ��
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
		//ֻ�Ǽ�¼��Barcode HashValue����Ӧ�����β�ڵ㣬��ôHashtable�ͽڵ��ļ����п����Ѿ��ı��ˣ����Ѿ��ı�Ļָ�Ϊԭ����״��
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
				//�ָ�Ϊԭ����״��
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
						//�����Ѿ�д���˽ڵ�����
						strcpy(dir_str,goods_list_dirctory);
						strcat(dir_str,goods_list_file);
						if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
						{
							return -4;
						}

						if (file1.fsize != rec_offset*sizeof(TGOODS_SPEC_RECORD))
						{
							//˵��ʵ����û��д��ڵ�
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
						return -2;			//��־���ݳ��ȴ���
					}
				}
			}
		}
		else if (rectype == REC_TYPE_CHECK)
		{
			if (log_data_len == 8+sizeof(TCHECK_INFO_RECORD))
			{
				//�����Ѿ�д���˽ڵ�����

				strcpy(dir_str,check_dirctory);
				strcat(dir_str,check_list_file);
				if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
				{
					return -4;
				}

				if (file1.fsize != rec_offset*sizeof(TCHECK_INFO_RECORD))
				{
					//˵��ʵ����û��д��ڵ�
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
				return -2;		//��־���ݳ��ȴ���
			}
		}
		else if (rectype == REC_TYPE_BATCH)
		{
			if (log_data_len == 8+sizeof(TBATCH_NODE))
			{
				//�����Ѿ�д���˽ڵ�����

				strcpy(dir_str,batch_dirctory);
				strcat(dir_str,batch_list_file);
				if (f_open(&file1,dir_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
				{
					return -4;
				}

				if (file1.fsize != rec_offset*sizeof(TBATCH_NODE))
				{
					//˵��ʵ����û��д��ڵ�
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
				return -2;		//��־���ݳ��ȴ���
			}
		}
	}

	return -1;
}



/**
* @brief �ָ����жϵ�������м�¼����
* @return 0���ɹ�  -1��ʧ��
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

		//����Ӧ��hash_table�ļ�
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
* @brief �ָ����жϵ�ɾ��ĳһ���ڵ�Ĳ���
* @return 0���ɹ�  -1��ʧ��
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

	//step2:���¸ýڵ�ǰһ���ڵ�ͺ�һ���ڵ��������Ϣ
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


	//���Խ��ڵ��ļ��ر���
	f_close(&file1);

	//step3:��ɾ���Ľڵ��ƫ�Ƽ�¼����Ϣ�ļ���
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
			goto check_next;		//˵����ɾ���Ľڵ�ƫ���Ѿ�������INF�ļ����ˣ�����Ҫ�ٱ���һ��
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

	//���ɾ���Ľڵ���������׽ڵ㣬��ô����Ҫ���±���������׽ڵ���Ϣ
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

	//���ɾ���Ľڵ��������β�ڵ㣬��ô����Ҫ���±��������β�ڵ���Ϣ
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
		//��hash���ļ���ȷ���ýڵ��Ƿ��Ƕ�Ӧ�����β�ڵ㣬�����β�ڵ㣬����Ҫ����hash���ļ�
		strcpy(inf_file_str,serial_data_dirctory);
		strcat(inf_file_str,current_dj_dir);
		strcat(inf_file_str,barcode_hash_table_file);

		if (f_open(&file1,inf_file_str,FA_OPEN_EXISTING | FA_READ | FA_WRITE) != FR_OK)
		{
			return -12;
		}

		f_lseek(&file1,4*(hash_value%HASH_TABLE_SIZE));
		//��ȡ�����β��ַ
		if (f_read(&file1,(void*)&temp,4,&tmp) != FR_OK)
		{
			f_close(&file1);
			return -13;
		}

		if (temp == index)
		{
			//����ýڵ�պ��Ƕ�Ӧ�����β�ڵ㣬��ô��Ҫ���¸������Ӧ��β�ڵ�
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
	//��ȡ�����β��ַ
	if (f_read(&file2,(void*)&link_end,4,&tmp) != FR_OK)
	{
		return -3;
	}

	//���������β��ַ
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
		//��������β��ַΪ0
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
* @brief �ָ����жϵ��滻��Ʒ��Ϣ�ڵ�Ĳ���
* @return 0���ɹ�  -1��ʧ��
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
* @brief ��¼�����Ļָ�
* @param[in] unsigned char *precord ��¼ָ��
* @return 0:�ɹ�
*        -1:ʧ��
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
			return -2;		//��־���ݳ��ȴ���
		}

		rec_type = *((unsigned int*)(log_data+4));
		rec_offset = *((unsigned int*)(log_data+8));

		if (log_data_len == 12)
		{
			//�����־����ֻ��12�ֽڣ���ô˵�������Ӽ�¼ʱ��û�ж�ԭ���ļ�¼�ļ����κεı䶯�����Բ���Ҫ�κεĴ���
			return 0;
		}

		//˵���ڼ�¼���ӹ����п����Ѿ��ı��˲��ּ�¼���ݣ���Ҫ��ȡ�ָ���ʩ
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
			return -2;	//��־���ݳ��ȴ���
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
		//	return -2;	//��־���ݳ��ȴ���
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
				return -2;				//��־���ݴ���
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
			return -2;	//��־���ݳ��ȴ���
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
*@brief �ж��Ƿ������־�ļ����������־�ļ���ͼ������־�ļ��ָ����жϵĲ������߻ع����жϵĲ���
*@param[in] 
*@return 0 �ɹ�  else; ʧ��
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
	
	logfile_len = file3.fsize;	//��־�ļ��ĳ���
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
		//	if (logitem_len == 0)	//����һ�������log�ļ������СΪ24�ֽڣ���������ȫ��Ϊ0��������дlog�ļ�����ʱ�ϵ���ɵ�
		//	{
		//		err_code = 0;		
		//	}
		//	break;
		//}

		if (logitem_len != logfile_len)
		{
			err_code = 0;		//�������������Ϊ����дLOG�ļ�����ϵ���ɵ�
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
		f_truncate(&file3);		//�����־�ļ�
	}

	f_close(&file3);
	return err_code;
}

/**
***************************************************************************
*@brief ������ݿ��ļ��Ƿ����
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
*@brief ������������
*@param[in] unsigned char mode   �Ͱ��ֽ�  0: ��ȫƥ����ֻҪƥ�䵽һ���ͷ���    1:����ƥ�䣬����Ҫ��ƥ��ĵ�����������
*								 �߰��ֽ�  0: ��ʾ���������̵㵥��				1:��ʾ�����������кŵĵ���
*@param[in] unsigned char *key_str  ��Ҫƥ����ַ���
*@param[out] unsigned char *result_offset_buf  �������ĵ���ƫ�ƶ������ѵ����Ⱥ�˳�����ڴ�Buffer�У����ô˺����߸����ṩMULTI_DJ_MAX_CNT���ֽڵĿռ�
*@return ����ƥ�䵽��¼��
@note mode = 0 ʱ��ʵ�����Ǽ��һ���Ƿ�����ͬ�ĵ��ݣ����������м�����ͬ�ģ���ͬ�ĵ��ݵ�ƫ�ƣ����Դ�������result_offset_buf����Ϊ0
      mode = 1ʱ����Ҫ�������еĿ���ƥ��ĵ��ݣ�������Ҫ֪���м���ƥ��ĵ��ݣ��Լ����ݵ�ƫ��
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

	//step1:����Ϣ�ļ����ҵ��ڵ��ļ��нڵ�������׵�ַ
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


	//step2: �򿪽ڵ��ļ������������˳���������
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
					return 1;		//��⵽��ͬ�ĵ���
				}
			}
			else
			{
				if (strcmp(key_str,((MULTXLH_NAME*)tmp_buffer)->listname) == 0)
				{
					//result_offset_buf[cnt++] = link_header;
					return 1;		//��⵽��ͬ�ĵ���
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
		//����DEBUG���������ļ�
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
			//������ӡ��������16����������Ҫת��ΪASCII��ʽ
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
			//������ӡ��������ASCII��ʽ������Ҫת��
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