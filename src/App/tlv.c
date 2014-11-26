/**
 * @file tlv.c
 * @brief �̵����PCͨѶ֮��������·���ʵ��
 *
 * @version V0.0.1
 * @author joe
 * @date 2011��05��11��
 * @note
 *    ͨ�����⴮����PC֮��ͨѶ��ͨѶ������֡��ʽ��
 *		STX(1Byte) | LEN(4Byte) | DATA��TLV���ݽṹ��| ETX��1Byte�� | CRC��4Byte��
 *    LEN:��λ��ǰ����λ�ں󣬱���0x01 0x10��ʾ272byte
 *		TLV �ṹ˵���� TAG��1Byte�� | LEN(1-4Byte) | VALUE(LEN Byte)  
 *
 * @copy
 *
 * �˴���Ϊ���ڽ������������޹�˾��Ŀ���룬�κ��˼���˾δ����ɲ��ø��ƴ�����������
 * ����˾�������Ŀ����˾����һ��׷��Ȩ����
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
 * @brief ��ʼ��tlv����֡
 * @param[in] TTLVPacket *tlvpacket    ָ��tlv����֡��ָ��
 * @param[in] BYTE * databuf  ���tlv����֡��ʵ��λ��
 * @return
 * @note databufָ��Ŀռ�����㹻�󣬿��Դ��һ֡tlv����֡
 */
void tlv_init(TTLVPacket *tlvpacket,unsigned char * databuf)
{
	memset((unsigned char*)tlvpacket, 0x00, sizeof(TTLVPacket));
	memset(databuf, 0, TLV_FRAME_SIZE);

	tlvpacket->buffer			= databuf;
	tlvpacket->buffer[0]		= 0x02;		// ��ʼλ
	tlvpacket->ptr				= 5;			// ����4�ֽڳ���	
}

/**
 * @brief �������������֡���������֡ʣ��Ľṹ������������ֹͣλ��CRC
 * @param[in] WORD	att_len
 * @return
 * @note  ���tlvpacket->ptrʵ���Ͼ������tlv����֡�ĳ���   
 */
void tlv_pack(TTLVPacket *tlvpacket)
{
	unsigned int				datalen;
	unsigned long				crc;

	datalen = tlvpacket->ptr - 5;

	// ��䳤��
	tlvpacket->buffer[1]				= (datalen>>24) & 0xFF;
	tlvpacket->buffer[2]				= (datalen>>16) & 0xFF;
	tlvpacket->buffer[3]				= (datalen>>8) & 0xFF;
	tlvpacket->buffer[4]				= datalen & 0xFF;



	// ֹͣλ
	tlvpacket->buffer[tlvpacket->ptr++]	= 0x03;

	// CRC
	crc = crc32(0, &tlvpacket->buffer[5], datalen);
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 24 ) & 0xFF;
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 16 ) & 0xFF;
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 8  ) & 0xFF;
	tlvpacket->buffer[tlvpacket->ptr++] = (crc >> 0  ) & 0xFF;
}

/**
 * @brief �ҵ���tag����Ϣ�ṹָ��
 * @param[in]
 * @return
 * @note
 */
TTagInfo * tlv_get_taginfo(unsigned char tag)
{
	int			i,begin,end;
	
	// @note taginfo������tag�Ѿ��ǰ����������еģ����Կ��ö��ַ�����
	
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
 * @brief ����һ������tag��tlv���ݽṹ֡��
 * @param[out] TTLVPacket *tlvpacket Ҫ���뵽��tlv���ݽṹ
 * @param[in] unsigned char tag Ҫ�����tag��
 * @param[in] unsigned char *indata ��tag������
 * @return
 * @note   TAG��Ӧ�����ݳ��Ȳ��ܳ���0xffffff
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
			//̫���ˣ��޷���ӵ�TLV�Ľṹ����
			return;
		}
		
		// 3 VAL
		memcpy(buf, indata, tag_info->length);
	}
}
/**
* @brief ����䳤Tag��tlv���ݽṹ֡��
* @param[out] TTLVPacket *tlvpacket Ҫ���뵽��tlv���ݽṹ
* @param[in] unsigned char tag	  Ҫ�����������tag
* @param[in] unsigned char *indata ��tag������
* @param[in] unsigned int len	   �䳤������ĳ���
* @return
* @note TAG��Ӧ�����ݳ��Ȳ��ܳ���0xffffff
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
		//̫���ˣ��޷���ӵ�TLV�Ľṹ����
		return;
	}

	// 4 VAL
	memcpy(buf, indata, len);
}


/**
 * @brief У�������Ƿ���Ϲ淶�����������·��������֡�ṹ  STX(1Byte) | LEN(4Byte) | DATA��TLV���ݽṹ��| ETX��1Byte�� | CRC��4Byte��
 * @param[in] unsigned char *indata  ҪУ�������ָ��
 * @return 0������ -1����ʼλ����  -2���ܳ������  -3������λ����  -4��CRCУ�����
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
 * @brief ��tlv����֡���ҵ���Ӧtag�򣬲�����������ָ��
 * @note  ����������ֻ��֧��TAG��Ӧ�ĳ����ֽ�Ϊ1���ֽڵģ������Ժܲ�
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
			//˵����ʾ��ֹһ���ֽ�������ʾ����
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
* @brief ���tag���Ӧ���������Ƿ�Ϸ�
* @param[in] BYTE tag	tag����
* @param[in] BYTE * pvalue  ��tag��������ָ��
* @return  -1: ��������ָ��Ϊ0
*          -2: �������д���
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
* @brief ����������ʾ��TLV���ݽṹ����һ��������������ṹ��TLV���ݰ��н��ָ��TAG����Ӧ�����������
* @param[in] BYTE tag			tag����
* @param[in] BYTE * inbuf		��Ҫ���������ݰ�
* @param[in] unsigned int in_len		��Ҫ���������ݰ��ĳ���
* @param[out] unsigned int *data_len	��TAG��Ӧ������ĳ���
* @return	���ظ�TAG��Ӧ���������е�ƫ��
	��6F��	FCIģ��											M
			��84��	DF��									M
			��A5��	FCI����ר��ģ��							M
				��BF0C��	�������Զ������ݵ�FCI			M
					��61��	Ŀ¼���						M
						��4F��	DF��(AID)					M
						��50��	Ӧ�ñ�ǩ					O
						��87��	Ӧ������ָʾ��				C
* @note ������������ֽ�TAG�����ں��油��0x00
*		�˺���Ŀǰ�ľ��������ڣ��������������ͬ��TAG����ôֻ���ҵ�ǰ���TAG����Ӧ��������
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
			//���TAG��һ���ֽڵĵ�5λȫ��1����ôTAG��2���ֽ���ɣ�����TAGֻ��1���ֽ�
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
			//�����ֽڵ����λ�����1�Ļ�����ô����ֽڵĵͰ��ֽڵ�ֵ�ͱ�ʾ���������ٸ��ֽ�������ʾ���ȡ�����0x82 0x01 0x02 �ͱ�ʾ0x82���滹����2���ֽ�������ʾ������ĳ��ȵġ���0x0201 ��λ��ǰ
			if ((len_byte & 0x0f) > 4)
			{
				//��ʾ���ȵ��ֽڲ����ܴ���4���ֽ�
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
				//TAG�ĵ�һ���ֽڵ�bit6����0��ʾ����������û��������
				offset += value_len;
				continue;
			}
			else
			{
				//��ǰ�������л�������������ôҪ�����������
				k = tlv_search_tagvalue(tag,inbuf+offset,value_len,&len);
				if (0 == k)
				{
					//���û����������TAG����ô������������
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

