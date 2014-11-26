#ifndef _DUMMY_FAT16_DATA_H_
#define _DUMMY_FAT16_DATA_H_


//根据构造的FAT数据可以确定下来的LBA
#define MBR_LBA		0
#define DBR_LBA		0x29
#define FAT1_LBA	(DBR_LBA+6)	
#define FAT2_LBA	(DBR_LBA+239)
#define FDT_LBA		(DBR_LBA+472)
#define DB_FILE_LBA (DBR_LBA+504)

extern const unsigned char dummy_MBR[];
extern const unsigned char dummy_DBR[];
extern const unsigned char dummy_FAT[];
extern const unsigned char dummy_FDT[];

#endif