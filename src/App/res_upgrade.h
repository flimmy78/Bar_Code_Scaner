#ifndef _RES_UPGRADE_H_
#define _RES_UPGRADE_H_

typedef struct  
{
	unsigned char					magic[4];
	unsigned int					xor;
	unsigned int					xor_data;
	unsigned int					length;
	unsigned char					OEMName[16];
	unsigned char					Version[16];
	unsigned char					Date[16];
}TPackHeader;


int res_upgrade(void);
//int res_card_insert(void);
#endif
