/**
* @file stm32f10x_flash_config.h
* @brief 2.4GPOS项目中主控内部FLASH区域的使用规划及相关配置
*        
* @version V0.0.1
* @author joe.zhou
* @date 2010年4月27日
* @note
*		STM32F103VET6 内部有512K FLASH区域
*     
*     具体规划如下：BootCode Area					1024*16		// BootLoader，用于AppCode的升级或者擦除
*					AppCode  Area					
*					GB2312 12*12字库（optinal）		
*                   Config Area						最后1K的空间保存系统的一些掉电需要保存的信息或者配置
*/

#ifndef _STM32F10X_FLASH_CONFIG_H_
#define _STM32F10X_FLASH_CONFIG_H_

	//定义STM32 FLASH的基本特性
	#define		FLASH_START					0x08000000		// Flash起始地址
	#define		PAGE_SIZE					1024			//STM32 Flash的Page Size
	#define		MCU_FLASH_SIZE				(300*PAGE_SIZE)	//STM32 Flash的Flash Size

	//STM32 FLASH的使用配置
	#define		IAP_SIZE					(PAGE_SIZE*32)	//BootCode Size
	#define		CONFIG_AREA_SIZE			(PAGE_SIZE*1)		//1个Page的区域用来存放掉电需要保存的信息或配置

	#define		CONFIG_ADDR_IN_MCUFLASH		(MCU_FLASH_SIZE - 4*CONFIG_AREA_SIZE)	//STM32F103VET6 共有512 Page的Flash，最后1个Page用来保存一些参数
	//#define		CHECK_VALUE_ADDR			CONFIG_ADDR_IN_MCUFLASH
	//#define		DEVICE_STATE_ADDR			(CHECK_VALUE_ADDR + 4)			//最后1K区域的第一个字节保存，设备的状态
	//#define		KEK_INDEX_ADDR				(DEVICE_STATE_ADDR + 4)			//存放 kek索引和有效字节，实际只用到低位的3个字节 kek_valid | kek_index(2Bytes) | reserved(1bytes)
	//#define		KEK_ADDR					(KEK_INDEX_ADDR + 4)			//存放kek，占用48字节
	//#define		ADMIN_PSW_ADDR				(KEK_ADDR + 48)					//存放系统管理员密码，8字节
	//#define		NOTSEND_REC_NUM_ADDR		(ADMIN_PSW_ADDR + 8)			//存放未上送交易记录，4字节
	//#define		TOTAL_CONSUME_INFO_ADDR		(NOTSEND_REC_NUM_ADDR + 4)		//存放总消费交易信息，8字节			前6字节是总消费交易金额，后2字节是总消费笔数
	//#define		TOTAL_CANCLE_INFO_ADDR		(TOTAL_CONSUME_INFO_ADDR + 8)	//存放总撤销+退货交易信息，8字节	前6字节是总交易金额，后2字节是总交易笔数
	//#define		TOTAL_RETURN_INFO_ADDR		(TOTAL_CANCLE_INFO_ADDR + 8)	//存放总撤销+退货交易信息，8字节	前6字节是总交易金额，后2字节是总交易笔数
	//#define		TOTAL_LOAD_INFO_ADDR		(TOTAL_RETURN_INFO_ADDR + 8)	//存放总充值交易信息，8字节			前6字节是总充值金额，后2字节是总充值交易笔数
	//#define		TOTAL_TRADE_INFO_ADDR		(TOTAL_LOAD_INFO_ADDR + 8)		//存放总交易笔数，4字节	
	//#define		JIESUAN_FAIL_FLAG_ADDR		(TOTAL_TRADE_INFO_ADDR + 4)		//存放最近一次结算是否失败的标记，4字节

	#define		BASICAL_BARCODE_ADDR				CONFIG_ADDR_IN_MCUFLASH				//存放客户设置的基准条码，32字节
	#define		SCAN_COUNT_PAGE_ADDR			(BASICAL_BARCODE_ADDR+32)
    
	#define     SCAN_COUNT						    (CONFIG_ADDR_IN_MCUFLASH+PAGE_SIZE*2)	//存放计数次数

#endif