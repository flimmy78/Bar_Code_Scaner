#ifndef PCUSART_H_
#define PCUSART_H_


#define IF_FILE_SIZE_1K			1024//���뵼����������1K�ӿ��ļ���С�����豸
#define IF_FILE_SIZE_2K			2048//���뵼����������2K�ӿ��ļ���С�����豸

#define IF_FILE_SIZE	IF_FILE_SIZE_1K		//���嵼�뵼�����߷����豸ʱʹ�õĽӿ��ļ��Ĵ�С
//#define IF_FILE_SIZE	IF_FILE_SIZE_2K		//���嵼�뵼�����߷����豸ʱʹ�õĽӿ��ļ��Ĵ�С

#if(IF_FILE_SIZE == IF_FILE_SIZE_2K)
	#define G_SEND_BUF_LENGTH			2048
	#define G_RECEIV_BUF_LENGTH			2048
#else
	#define G_SEND_BUF_LENGTH			1024
	#define G_RECEIV_BUF_LENGTH			1024
#endif

#define MASSTORAGE_DEVICE_TYPE_DUMMY_FAT		0	//�����ļ�ϵͳ�豸
#define MASSTORAGE_DEVICE_TYPE_MSD				1	//MicroSD��
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