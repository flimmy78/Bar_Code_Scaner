#ifndef _TLV_H_
#define _TLV_H_

#define TLV_FRAME_SIZE		1024		//定义TLV数据结构帧的size		//注意不能超过用来传输数据的BUF的SIZE

#define TAG_HH_LSH					0x01			//会话流水号			4byte  B
#define TAG_ACK						0x0a			//应答码				2byte  N
#define TAG_APP_VERSION				0x0b			//应用程序版本号		8byte  N
#define TAG_PARAM_VERSION			0x0c			//参数版本号			8byte  N
#define TAG_REC_TYPE				0x0d			//记录类型				1byte  B
#define TAG_DEVICE_ID				0x0e			//终端设备ID			12byte  B
#define TAG_VERIFY_DATA				0x0f			//认证数据				12byte  B    
#define TAG_DJ_DIR					0x10			//单据的相对路径		4 byte N   
#define TAG_IF_FEATURE				0x11			//终端接口特征			8byte  B
#define TAG_HH_RESULT				0x12			//会话结果				1byte  B 
#define TAG_HH_TYPE					0x13			//会话类型				1byte  B
#define TAG_FILE_TYPE				0x14			//文件类型				1byte  B
#define TAG_FILE_OFFSET				0x15			//文件的偏移			4byte  B
#define TAG_DATA_LENGTH				0x16			//数据长度				4byte  B
#define TAG_FILE_LIST				0x17			//文件列表				x byte B	//变长
#define TAG_DOWNLOAD_COMPLETE_FLAG	0x18			//下载结束的标记		1byte  B
#define TAG_TRNAS_DATA				0x19			//透传数据				x byte B	//变长
#define TAG_OPERATE_CODE			0x60			//操作码				2byte  B


typedef struct
{
	int						ptr;				//当前在包内的指针位置
	unsigned char			*buffer;			//数据域的指针
}TTLVPacket;


typedef struct {
	unsigned char			tag;			// tag
	unsigned int			length;			// 该tag的长度
	unsigned char			type;			// tag的数据类型
}TTagInfo;


void tlv_init(TTLVPacket *tlvpacket,unsigned char * databuf);
void tlv_pack(TTLVPacket *tlvpacket);
void tlv_addtag(TTLVPacket *tlvpacket, unsigned char tag, unsigned char *indata);
void tlv_addtag_ext(TTLVPacket *tlvpacket, unsigned char tag,unsigned char *indata,unsigned int len);
int  tlv_verify(unsigned char *indata);
unsigned char *tlv_get_tagvalue(unsigned char tag, TTLVPacket *tlv_packet);
TTagInfo * tlv_get_taginfo(unsigned char tag);
#endif
