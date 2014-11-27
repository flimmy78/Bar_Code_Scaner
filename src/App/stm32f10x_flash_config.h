/**
* @file stm32f10x_flash_config.h
* @brief 2.4GPOS��Ŀ�������ڲ�FLASH�����ʹ�ù滮���������
*        
* @version V0.0.1
* @author joe.zhou
* @date 2010��4��27��
* @note
*		STM32F103VET6 �ڲ���512K FLASH����
*     
*     ����滮���£�BootCode Area					1024*16		// BootLoader������AppCode���������߲���
*					AppCode  Area					
*					GB2312 12*12�ֿ⣨optinal��		
*                   Config Area						���1K�Ŀռ䱣��ϵͳ��һЩ������Ҫ�������Ϣ��������
*/

#ifndef _STM32F10X_FLASH_CONFIG_H_
#define _STM32F10X_FLASH_CONFIG_H_

	//����STM32 FLASH�Ļ�������
	#define		FLASH_START					0x08000000		// Flash��ʼ��ַ
	#define		PAGE_SIZE					1024			//STM32 Flash��Page Size
	#define		MCU_FLASH_SIZE				(300*PAGE_SIZE)	//STM32 Flash��Flash Size

	//STM32 FLASH��ʹ������
	#define		IAP_SIZE					(PAGE_SIZE*32)	//BootCode Size
	#define		CONFIG_AREA_SIZE			(PAGE_SIZE*1)		//1��Page������������ŵ�����Ҫ�������Ϣ������

	#define		CONFIG_ADDR_IN_MCUFLASH		(MCU_FLASH_SIZE - 4*CONFIG_AREA_SIZE)	//STM32F103VET6 ����512 Page��Flash�����1��Page��������һЩ����
	//#define		CHECK_VALUE_ADDR			CONFIG_ADDR_IN_MCUFLASH
	//#define		DEVICE_STATE_ADDR			(CHECK_VALUE_ADDR + 4)			//���1K����ĵ�һ���ֽڱ��棬�豸��״̬
	//#define		KEK_INDEX_ADDR				(DEVICE_STATE_ADDR + 4)			//��� kek��������Ч�ֽڣ�ʵ��ֻ�õ���λ��3���ֽ� kek_valid | kek_index(2Bytes) | reserved(1bytes)
	//#define		KEK_ADDR					(KEK_INDEX_ADDR + 4)			//���kek��ռ��48�ֽ�
	//#define		ADMIN_PSW_ADDR				(KEK_ADDR + 48)					//���ϵͳ����Ա���룬8�ֽ�
	//#define		NOTSEND_REC_NUM_ADDR		(ADMIN_PSW_ADDR + 8)			//���δ���ͽ��׼�¼��4�ֽ�
	//#define		TOTAL_CONSUME_INFO_ADDR		(NOTSEND_REC_NUM_ADDR + 4)		//��������ѽ�����Ϣ��8�ֽ�			ǰ6�ֽ��������ѽ��׽���2�ֽ��������ѱ���
	//#define		TOTAL_CANCLE_INFO_ADDR		(TOTAL_CONSUME_INFO_ADDR + 8)	//����ܳ���+�˻�������Ϣ��8�ֽ�	ǰ6�ֽ����ܽ��׽���2�ֽ����ܽ��ױ���
	//#define		TOTAL_RETURN_INFO_ADDR		(TOTAL_CANCLE_INFO_ADDR + 8)	//����ܳ���+�˻�������Ϣ��8�ֽ�	ǰ6�ֽ����ܽ��׽���2�ֽ����ܽ��ױ���
	//#define		TOTAL_LOAD_INFO_ADDR		(TOTAL_RETURN_INFO_ADDR + 8)	//����ܳ�ֵ������Ϣ��8�ֽ�			ǰ6�ֽ����ܳ�ֵ����2�ֽ����ܳ�ֵ���ױ���
	//#define		TOTAL_TRADE_INFO_ADDR		(TOTAL_LOAD_INFO_ADDR + 8)		//����ܽ��ױ�����4�ֽ�	
	//#define		JIESUAN_FAIL_FLAG_ADDR		(TOTAL_TRADE_INFO_ADDR + 4)		//������һ�ν����Ƿ�ʧ�ܵı�ǣ�4�ֽ�

	#define		BASICAL_BARCODE_ADDR				CONFIG_ADDR_IN_MCUFLASH				//��ſͻ����õĻ�׼���룬32�ֽ�
	#define		SCAN_COUNT_PAGE_ADDR			(BASICAL_BARCODE_ADDR+32)
    
	#define     SCAN_COUNT						    (CONFIG_ADDR_IN_MCUFLASH+PAGE_SIZE*2)	//��ż�������

#endif