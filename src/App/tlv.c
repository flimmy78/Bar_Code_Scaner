/**
 * @file tlv.c
 * @brief 盘点机与PC通讯之间数据链路层的实现
 *
 * @version V0.0.1
 * @author joe
 * @date 2011年05月11日
 * @note
 *    通过虚拟串口与PC之间通讯，通讯的数据帧格式：
 *		STX(1Byte) | LEN(4Byte) | DATA（TLV数据结构）| ETX（1Byte） | CRC（4Byte）
 *    LEN:高位在前，低位在后，比如0x01 0x10表示272byte
 *		TLV 结构说明： TAG（1Byte） | LEN(1-4Byte) | VALUE(LEN Byte)  
 *
 * @copy
 *
 * 此代码为深圳江波龙电子有限公司项目代码，任何人及公司未经许可不得复制传播，或用于
 * 本公司以外的项目。本司保留一切追究权利。
 *
 * <h1><center>&copy; COPYRIGHT 2009 netcom</center></h1>
 */
/* Private include -----------------------------------------------------------*/
#include "tlv.h"
#include <string.h>
#include "crc32.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define TAG_N							0
#define TAG_ANS							1
#define TAG_B							2


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const TTagInfo	taginfo[] = {
	{TAG_HH_LSH,4,TAG_B},{TAG_ACK,	2,	TAG_N  },{TAG_APP_VERSION,	8,	TAG_N  },{TAG_PARAM_VERSION,	8,	TAG_N  },
	{TAG_REC_TYPE,	1,	TAG_B  },{TAG_DEVICE_ID,	12,TAG_B},{TAG_VERIFY_DATA,	16,TAG_B},{TAG_DJ_DIR,	4,TAG_N},{TAG_IF_FEATURE,	8,TAG_B},
	{TAG_HH_RESULT,	1,TAG_B},{TAG_HH_TYPE,	1,TAG_B},{TAG_FILE_TYPE,1,TAG_B},{TAG_FILE_OFFSET,4,TAG_B},{TAG_DATA_LENGTH,4,TAG_B},
	{TAG_FILE_LIST,0,TAG_B},{TAG_DOWNLOAD_COMPLETE_FLAG,1,TAG_B},{TAG_TRNAS_DATA,0,TAG_B},{TAG_OPERATE_CODE,	2,	TAG_N  },
};


/* Global variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
 * @brief 初始化tlv数据帧
 * @param[in] TTLVPacket *tlvpacket    指向tlv数据帧的指针
 * @param[in] BYTE * databuf  存放tlv数据帧的实际位置
 * @return
 * @note databuf指向的空间必须足够大，可以存放一帧tlv数据帧
 */
void tlv_init(TTLVPacket *tlvpacket,unsigned char * databuf)
{
	memset((unsigned char*)tlvpacket, 0x00, sizeof(TTLVPacket));
	memset(databuf, 0, TLV_FRAME_SIZE);

	tlvpacket->buffer			= databuf;
	tlvpacket->buffer[0]		= 0x02;		// 开始位
	tlvpacket->ptr				= 5;			// 保留4字节长度	
}

/**
 * @brief 组成完整的数据帧，填充数据帧剩余的结构，包括长度域、停止位，CRC
 * @param[in] WORD	att_len
 * @return
 * @note  最后tlvpacket->ptr实际上就是这个tlv数据帧的长度   
 */
void tlv_pack(TTLVPacket *tlvpacket)
{
	unsigned int				datalen;
	unsigned long				crc;

	datalen = tlvpacket->ptr - 5;

	// 填充长度
	tlvpacket->buffer[1]				= (datalen>>24) & 0xFF;
	tlvpacket->buffer[2]				= (datalen>>16) & 0xFF;
	tlvpacket->buffer[3]				= (datalen>>8) & 0xFF;
	tlvpacket->buffer[4]				= datalen & 0xFF;



	// 停止位
	tlvpacket->buffer[tlvpacket->ptr++]	= 0x03;

	// CRC
	crc = crc32(0, &tlvpacket->buffer[5], datalen);
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 24 ) & 0xFF;
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 16 ) & 0xFF;
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 8  ) & 0xFF;
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 0  ) & 0xFF;
}

/**
 * @brief 找到该tag的信息结构指针
 * @param[in]
 * @return
 * @note
 */
TTagInfo * tlv_get_taginfo(unsigned char tag)
{
	int			i,begin,end;
	
	// @note taginfo这个表的tag已经是按照升序排列的，所以可用二分法搜索
	
	begin = 0;
	end   = sizeof(taginfo)/sizeof(TTagInfo) - 1;
	i = (begin + end)/2;
	while(taginfo[i].tag != tag)
	{
		if(taginfo[i].tag > tag)
				end = i-1;
		else
				begin = i+1;
				
		if(begin > end)
				return (TTagInfo*)0;		
		
		i = (begin + end)/2;				
	}
	
	return (TTagInfo *)&taginfo[i];
}

/**
 * @brief 加入一个定长tag到tlv数据结构帧中
 * @param[out] TTLVPacket *tlvpacket 要加入到的tlv数据结构
 * @param[in] unsigned char tag 要加入的tag号
 * @param[in] unsigned char *indata 该tag的数据
 * @return
 * @note   TAG对应的数据长度不能超过0xffffff
 */
void tlv_addtag(TTLVPacket *tlvpacket, unsigned char tag, unsigned char *indata)
{
	TTagInfo					*tag_info;
	unsigned char			*buf;

	tag_info					= tlv_get_taginfo(tag);

	if(tag_info==(TTagInfo*)0)
	{
		return;
	}
	
	if(tag_info->length)
	{
		buf					= &tlvpacket->buffer[tlvpacket->ptr];

		// 1 TAG
		*buf				= tag;
		buf++;

		// 2 LEN
		if (tag_info->length < 0x80)
		{
			*buf				= tag_info->length;
			buf++;

			tlvpacket->ptr			+= tag_info->length+2;
		}
		else if (tag_info->length < 0x100)
		{
			*buf = 0x81;
			buf++;
			*buf = tag_info->length;
			buf++;

			tlvpacket->ptr			+= tag_info->length+3;
		}
		else if (tag_info->length < 0x10000)
		{
			*buf = 0x82;
			buf++;
			*buf = (tag_info->length >> 8) & 0xff;
			buf++;
			*buf = tag_info->length & 0xff;
			buf++;
			tlvpacket->ptr			+= tag_info->length+4;
		}
		else if (tag_info->length < 0x1000000)
		{
			*buf = 0x83;
			buf++;
			*buf = (tag_info->length >> 16) & 0xff;
			buf++;
			*buf = (tag_info->length >> 8) & 0xff;
			buf++;
			*buf = tag_info->length & 0xff;
			buf++;
			tlvpacket->ptr			+= tag_info->length+5;
		}
		else
		{
			//太长了，无法添加到TLV的结构中来
			return;
		}
		
		// 3 VAL
		memcpy(buf, indata, tag_info->length);
	}
}
/**
* @brief 加入变长Tag到tlv数据结构帧中
* @param[out] TTLVPacket *tlvpacket 要加入到的tlv数据结构
* @param[in] unsigned char tag	  要加入数据域的tag
* @param[in] unsigned char *indata 该tag的数据
* @param[in] unsigned int len	   变长数据域的长度
* @return
* @note TAG对应的数据长度不能超过0xffffff
*/
void tlv_addtag_ext(TTLVPacket *tlvpacket, unsigned char tag,unsigned char *indata,unsigned int len)
{
	unsigned char				*buf;

	buf					= &tlvpacket->buffer[tlvpacket->ptr];

	// 1 Add TAG
	*buf				= tag;
	buf++;

	if (len == 0)
	{
		return;
	}

	// 2 LEN
	if (len < 0x80)
	{
		*buf				= len;
		buf++;

		tlvpacket->ptr			+= len+2;
	}
	else if (len < 0x100)
	{
		*buf = 0x81;
		buf++;
		*buf = len;
		buf++;

		tlvpacket->ptr			+= len+3;
	}
	else if (len < 0x10000)
	{
		*buf = 0x82;
		buf++;
		*buf = (len >> 8) & 0xff;
		buf++;
		*buf = len & 0xff;
		buf++;
		tlvpacket->ptr			+= len+4;
	}
	else if (len < 0x1000000)
	{
		*buf = 0x83;
		buf++;
		*buf = (len >> 16) & 0xff;
		buf++;
		*buf = (len >> 8) & 0xff;
		buf++;
		*buf = len & 0xff;
		buf++;
		tlvpacket->ptr			+= len+5;
	}
	else
	{
		//太长了，无法添加到TLV的结构中来
		return;
	}

	// 4 VAL
	memcpy(buf, indata, len);
}


/**
 * @brief 校验数据是否符合规范定义的数据链路增的数据帧结构  STX(1Byte) | LEN(4Byte) | DATA（TLV数据结构）| ETX（1Byte） | CRC（4Byte）
 * @param[in] unsigned char *indata  要校验的数据指针
 * @return 0：符合 -1：起始位错误  -2：总长度溢出  -3：结束位错误  -4：CRC校验错误
 */
int tlv_verify(unsigned char *indata)
{
	unsigned int			len,tmp;
	unsigned long			crc;

	
	if( indata[0] != 0x02)
	{
		return -1;
	}

	len = indata[1];
	len <<= 24;
	tmp = indata[2];
	tmp <<= 16;
	len |= tmp;
	tmp = indata[3];
	tmp <<= 8;
	len |= tmp;
	len |= indata[4];

	if (len > (TLV_FRAME_SIZE - 10))
	{
		return -2;
	}
	

	if(indata[len+5] != 0x03)
	{
		return -3;
	}
	crc = crc32(0, &indata[5], len);

	if( indata[len+6] != ((crc>>24)&0xFF) ||
		indata[len+7] != ((crc>>16)&0xFF) ||
		indata[len+8] != ((crc>> 8)&0xFF) ||
		indata[len+9] != (crc & 0xFF))
	{
		return -4;
	}

	return 0;
}

/**
 * @brief 从tlv数据帧中找到相应tag域，并返回数据域指针
 * @note  此搜索函数只是支持TAG对应的长度字节为1个字节的，兼容性很差
 */
unsigned char *tlv_get_tagvalue(unsigned char tag, TTLVPacket *tlv_packet)
{
	unsigned char					*pindata;
	int								len, pos,tmp_len,tmp;

	pindata	= &tlv_packet->buffer[5];

	len	= tlv_packet->buffer[1];
	len	<<= 24;
	tmp_len	= tlv_packet->buffer[2];
	tmp_len	<<= 16;
	len |= tmp_len;
	tmp_len	= tlv_packet->buffer[3];
	tmp_len	<<= 8;
	len |= tmp_len;
	len	|= tlv_packet->buffer[4];

	pos								= 0;

	while(pos < len)
	{
		if ((pindata[1] & 0x80) == 0x80)
		{
			//说明表示不止一个字节用来表示长度
			if ((pindata[1] & 0x0f) == 0x01)
			{
				tmp_len = pindata[2];
				tmp = 2;
			}
			else if ((pindata[1] & 0x0f) == 0x02)
			{
				tmp_len = pindata[2];
				tmp_len <<= 8;
				tmp_len += pindata[3];

				tmp = 3;
			}
			else if ((pindata[1] & 0x0f) == 0x03)
			{
				tmp_len = pindata[2];
				tmp_len <<= 16;
				tmp = pindata[3];
				tmp <<= 8;
				tmp_len |= tmp;
				tmp_len |= pindata[4];

				tmp = 4;
			}
			else
			{		
				return (unsigned char *)0;
			}
		}
		else
		{
			tmp_len = pindata[1];
			tmp = 1;
		}

		if(pindata[0] == tag)
		{
			return pindata+1+tmp; 
		}

		pos							+= tmp_len + 1 + tmp;
		pindata						+= tmp_len + 1 + tmp;

	}
	return (unsigned char *)0;
}

/**
* @brief 检查tag相对应的数据域是否合法
* @param[in] BYTE tag	tag类型
* @param[in] BYTE * pvalue  该tag的数据域指针
* @return  -1: 该数据域指针为0
*          -2: 数据域有错误
*           0: OK
* @note
*/
/*
int tlv_check_value(BYTE tag,BYTE * pvalue)
{
	//@todo...
	return 0;
}
*/


/*
* @brief 解析如下所示的TLV数据结构，从一包符合类似下面结构的TLV数据包中解出指定TAG所对应的数据域出来
* @param[in] BYTE tag			tag类型
* @param[in] BYTE * inbuf		需要解析的数据包
* @param[in] unsigned int in_len		需要解析的数据包的长度
* @param[out] unsigned int *data_len	该TAG对应数据域的长度
* @return	返回该TAG对应的数据域中的偏移
	‘6F’	FCI模板											M
			‘84’	DF名									M
			‘A5’	FCI数据专用模板							M
				‘BF0C’	发卡方自定义数据的FCI			M
					‘61’	目录入口						M
						‘4F’	DF名(AID)					M
						‘50’	应用标签					O
						‘87’	应用优先指示器				C
* @note 如果是搜索单字节TAG，先在后面补齐0x00
*		此函数目前的局限性在于，假如包含两个相同的TAG，那么只能找到前面的TAG所对应的数据域
*/
/*
WORD tlv_search_tagvalue(WORD tag, BYTE* inbuf,WORD in_len,WORD *data_len) reentrant
{
	WORD	offset = 0;
	BYTE	len_byte,i;
	WORD	value_len,k,len;
	WORD	current_tag;

	while (offset < in_len)
	{
		if (0x1f == (inbuf[offset] & 0x1f)) 
		{
			//如果TAG第一个字节的低5位全是1，那么TAG由2个字节组成，否则，TAG只有1个字节
			len_byte = inbuf[2+offset];
			current_tag = (inbuf[offset] << 8) + inbuf[offset+1];
			offset += 2;
		}
		else
		{
			len_byte = inbuf[offset+1];
			current_tag = inbuf[offset] << 8;
			offset += 1;
		}

		if (len_byte & 0x80)
		{
			//长度字节的最高位如果是1的话，那么这个字节的低半字节的值就表示后面会跟多少个字节用来表示长度。比如0x82 0x01 0x02 就表示0x82后面还会有2个字节用来表示数据域的长度的。即0x0201 低位在前
			if ((len_byte & 0x0f) > 4)
			{
				//表示长度的字节不可能大于4个字节
				value_len = 0;
				return 0;
			}
			value_len = 0;
			k = 1;
			for (i = 0; i < (len_byte & 0x0f);i++)
			{
				value_len += inbuf[offset+1+i] * k;
				k *= 256;
			}
			offset += (len_byte & 0x0f) + 1;
		}
		else
		{
			value_len = len_byte;
			offset += 1;
		}

		if (tag == current_tag)
		{
			*data_len = value_len;
			return offset;
		}
		else 
		{
			if (((current_tag >> 8) & 0x20) == 0)
			{
				//TAG的第一个字节的bit6等于0表示其数据域再没有子域了
				offset += value_len;
				continue;
			}
			else
			{
				//当前数据域中还包含了子域，那么要进行深度搜索
				k = tlv_search_tagvalue(tag,inbuf+offset,value_len,&len);
				if (0 == k)
				{
					//如果没有搜索到该TAG，那么继续往后搜索
					offset += value_len;
					continue;
				}
				else
				{
					*data_len = len;
					return offset+k;
				}
			}
		}
	}
	return 0;
}
*/

