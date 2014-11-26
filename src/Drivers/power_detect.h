/**
* @file  power_detect.h
* @brief  
* @version 1.0
* @author joe
* @date 2009��11��12��
* @note
*/

#ifndef	_POWER_DETECT_H_ 
#define _POWER_DETECT_H_


/**
* breif ���� ��ص�ѹ���ʱ��ѹ����ķ�ֵ
* @note ��Щ��ֵ�ļ����Ǹ��ݵ�·���ʱ��Ԫ�������������ġ�AD��������ǵ�ص�ѹ����һ����ѹ��·֮��ķ�ѹ������
*       ��ѹ��·��׼ȷ����Ӱ����Щ��ֵ�Ķ���ġ�ͬʱ�ο���ѹ���ȶ�Ҳ�Դ�ADת�����ȶ����кܴ��Ӱ�졣
*       ��ǰ��·��������õķ�ѹ����ֱ���30K | 30K ��VREF+ - VREF- = VDD = 3.3V
*       ��صĹ�����ѹ��Χ��: 4.2 V -- 3.5 V 
*		�����ϵõ���ת��ֵ�����ֵ��: (4.2 * 30/(30 + 30))/(VREF+ - VREF-) * 0x0fff = 0xA2E (2605)
*		�����ϵõ���ת��ֵ����Сֵ��: (3.5 * 30/(30 + 30))/(VREF+ - VREF-) * 0x0fff = 0x87B (2172)
*       ����ÿһ������������ǣ�(2606 - 2171)/ 4 = 109
*       ʵ��ʱ��������õķ�ѹ����̫��(620K��30K)ʱ��AIN����Ĳ�����ѹ�ᱻIO������!!!
*/
//#define CLASS_MAX_VALUE       2605		//3880		 //����ʵ��ֵ		
//#define CLASS_MIN_VALUE       2172		//3388		//��С��ʵ��ֵ
//#define CLASS_PROTECTED		  2000		//3200		//�������𣬵��˴˼����ǿ�ƹػ�
//#define CLASS_NUM             4			//����ļ�����

//#define CLASS_GRANULARITY     (CLASS_MAX_VALUE - CLASS_MIN_VALUE)/CLASS_NUM    //��ѹ���������


//#define CLASS1_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY)       //����1  ��ѹ���		Լ4.1V
//#define CLASS2_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY*2)     //����2				ԼV				
//#define CLASS3_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY*3)     //����3				ԼV
//#define CLASS4_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY*4)     //����4				Լ3.5V

#define CLASS1_TAP_VALUE      3150       //����1 			Լ2.5V
#define CLASS2_TAP_VALUE      2950	//2988     //����2				Լ2.3V				
#define CLASS3_TAP_VALUE      2750	//2722     //����3				Լ2.1V
#define CLASS4_TAP_VALUE      2600	//2600     //����4				Լ2.0V

//Ҫ�������ĵ�ѹ�����ڴ˴���Ӻ궨�弴��


//����Դ������IO
#define	USBLINK_INSERT_DETECT_PIN	GPIO_Pin_1			//PC.1  ����

#define	CHARGE_DETECT_PIN	GPIO_Pin_0			//PE.0 

//��ص�ѹ���IO
#define PV_DETECT_PIN			GPIO_Pin_2			//PC.2

void InitPowerDetectModule(void);
unsigned int GetPowerVoltageClass(void);

unsigned int USBLink_Insert_Detect(void);

unsigned int  ChargeState_Detect(void);

int GetPowerVoltagePercent(void);
#endif