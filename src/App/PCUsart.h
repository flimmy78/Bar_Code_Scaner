#ifndef PCUSART_H_
#define PCUSART_H_


#define IF_FILE_SIZE_1K			1024//导入导出工具利用1K接口文件大小访问设备
#define IF_FILE_SIZE_2K			2048//导入导出工具利用2K接口文件大小访问设备

#define IF_FILE_SIZE	IF_FILE_SIZE_1K		//定义导入导出工具访问设备时使用的接口文件的大小
//#define IF_FILE_SIZE	IF_FILE_SIZE_2K		//定义导入导出工具访问设备时使用的接口文件的大小

#if(IF_FILE_SIZE == IF_FILE_SIZE_2K)
	#define G_SEND_BUF_LENGTH			2048
	#define G_RECEIV_BUF_LENGTH			2048
#else
	#define G_SEND_BUF_LENGTH			1024
	#define G_RECEIV_BUF_LENGTH			1024
#endif

#define MASSTORAGE_DEVICE_TYPE_DUMMY_FAT		0	//虚拟文件系统设备
#define MASSTORAGE_DEVICE_TYPE_MSD				1	//MicroSD卡
#define MASSTORAGE_DEVICE_TYPE_SPI_FLASH		2	//SPI_FLASH

typedef struct {
	int						CmdPos;
	unsigned int			DataLength;
	unsigned int			status;
	unsigned char			*CmdBuffer;
	//unsigned char			ResBuffer[PC_COMMAND_LENTH];
}TPCCommand;

extern unsigned char		g_send_buff[G_SEND_BUF_LENGTH];
extern unsigned char		g_receive_buff[G_RECEIV_BUF_LENGTH];
extern TPCCommand			g_pcCommand;
extern unsigned char		g_mass_storage_device_type;

void PCUsart_Init(void);
int PCUsart_InByte(unsigned char c);
int PCUsart_process(void);
//void PCUsart_process(void *pp);
void reset_command(void);
//void init_card_test(void);
void USB_Cable_Config (unsigned char NewState);
void usb_device_init(unsigned char device_type);

#endif //PCUSART_H_