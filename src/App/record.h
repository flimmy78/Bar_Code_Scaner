
#ifndef _RECORD_H_
#define _RECORD_H_


#define REC_TYPE_BASCIAL_BARCODE		1
#define REC_TYPE_COUNTER				2

extern unsigned char  basical_barcode[32];		//基准条码,最后一个字节是XOR校验值
extern unsigned int   scan_count;				//条码计数器

int record_init(void);
int record_read(unsigned char rectype, unsigned char *pBuf);
int record_write(unsigned char rectype, unsigned char *pRec);
int record_clear(void);

#endif

