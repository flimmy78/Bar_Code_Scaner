#ifndef _RES_SPI_H_
#define _RES_SPI_H_

//#define  RES_UPDATE_BY_UART     
//#define  RES_UPDATE_BY_SDIO

// ×ÖµÄID¶¨Òå
#define FNT_ASC_6_12				0
#define FNT_CHN_12_12				1


int res_init(void);
unsigned char *read_resdata(unsigned int dwLBA);
void spi_flash_drv_test(void);

#endif
