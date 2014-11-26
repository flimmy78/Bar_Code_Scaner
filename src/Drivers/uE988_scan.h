#ifndef UE988_SCAN_H
#define UE988_SCAN_H

#define UE988_FRAME_MAX_LEN		257

#define UE988_LED_OFF		1
#define UE988_LED_ON		2
#define UE988_SCAN_DISABLE	3
#define UE988_SCAN_ENABLE	4
#define UE988_START_DECODE	5
#define UE988_STOP_DECODE	6


#define ENABLE		1
#define DISABLE		0


////Code Type
//#define Code39		0x00
//#define UPCA		0x01
//#define UPCE		0x02
//#define EAN13		0x03
//#define EAN8		0x04
//#define IL2of5		0x06	//Interleaved 2 of 5
//#define Codabar		0x07
//#define Code128		0x08
//#define Code93		0x09
//#define Code11		0x0A
//#define MSI			0x0B
//#define UCCEAN128	0x0E
//
//#define ChinaPost	0x98
//#define RSS14		0x52
//#define RSSLimited	0x53
//#define RSSExpanded 0x54

//Code Type
#define Code39		0x00
#define UPCA		0x01
#define UPCE		0x02
#define UPCE1		0x0C
#define EAN13		0x03
#define EAN8		0x04
#define InDus2of5	0x05
#define IL2of5		0x06	//Interleaved 2 of 5
#define Codabar		0x07
#define Code128		0x08
#define Code93		0x09
#define Code11		0x0A
#define MSI			0x0B
#define TriopCode39	0x0D
#define UCCEAN128	0x0E
#define ISBT128		0x54

//F0
#define ChinaPost	0x98
#define RSS14		0x52
#define RSSLimited	0x53
#define RSSExpanded 0x54

//F2
#define Matrix20f5	0x60
#define UK			0x90


/**
* @brief uE988 ÃüÁî¶¨Òå  decoder->host
*/
typedef struct {
	unsigned short			CmdPos;
	unsigned short			DataLength;
	unsigned char			status;
	unsigned char			*CmdBuffer;
}TUE988Command;

void UE988_Init(unsigned short switch_map);
int UE988_RxISRHandler(unsigned char c);
void UE988_beep(unsigned char beep_code);
void UE988_led_ctrl(unsigned char ctrl_type);
int UE988_get_curParam(unsigned char param_num);
int UE988_set_curParam(unsigned char param_num, unsigned char param_value, unsigned char set_type);
void UE988_scan_ctrl(unsigned char ctrl_type);
void UE988_enter_sleep(void);
void UE988_wakeup(void);
void UE988_start_stop_decode(unsigned char ctrl_type);
int UE988_get_barcode(unsigned char *code_type, unsigned char *code_buf, unsigned char inbuf_size,unsigned char *code_len);
int UE988_get_softVersion(unsigned char *softVer, unsigned char *plen);
//void UE988_scan_task(void);
void UE988_set_decoder_switch(unsigned short switch_map);
#endif	//UE988_SCAN_H