/**
* @file  power_detect.h
* @brief  
* @version 1.0
* @author joe
* @date 2009年11月12日
* @note
*/

#ifndef	_POWER_DETECT_H_ 
#define _POWER_DETECT_H_


/**
* breif 定义 电池电压检测时电压级别的阀值
* @note 这些阀值的计算是根据电路设计时的元件参数来决定的。AD的输入端是电池电压经过一个分压电路之后的分压。所以
*       分压电路的准确性是影响这些阀值的定义的。同时参考电压的稳定也对此AD转换的稳定性有很大的影响。
*       当前电路设计中所用的分压电阻分别是30K | 30K ，VREF+ - VREF- = VDD = 3.3V
*       电池的工作电压范围是: 4.2 V -- 3.5 V 
*		理论上得到的转换值的最大值是: (4.2 * 30/(30 + 30))/(VREF+ - VREF-) * 0x0fff = 0xA2E (2605)
*		理论上得到的转换值的最小值是: (3.5 * 30/(30 + 30))/(VREF+ - VREF-) * 0x0fff = 0x87B (2172)
*       所以每一个级别的粒度是：(2606 - 2171)/ 4 = 109
*       实测时发现如果用的分压电阻太大(620K和30K)时，AIN输入的采样电压会被IO口拉低!!!
*/
//#define CLASS_MAX_VALUE       2605		//3880		 //最大的实际值		
//#define CLASS_MIN_VALUE       2172		//3388		//最小的实际值
//#define CLASS_PROTECTED		  2000		//3200		//保护级别，到了此级别就强制关机
//#define CLASS_NUM             4			//定义的级别数

//#define CLASS_GRANULARITY     (CLASS_MAX_VALUE - CLASS_MIN_VALUE)/CLASS_NUM    //电压级别的粒度


//#define CLASS1_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY)       //级别1  电压最高		约4.1V
//#define CLASS2_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY*2)     //级别2				约V				
//#define CLASS3_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY*3)     //级别3				约V
//#define CLASS4_TAP_VALUE      (CLASS_MAX_VALUE - CLASS_GRANULARITY*4)     //级别4				约3.5V

#define CLASS1_TAP_VALUE      3150       //级别1 			约2.5V
#define CLASS2_TAP_VALUE      2950	//2988     //级别2				约2.3V				
#define CLASS3_TAP_VALUE      2750	//2722     //级别3				约2.1V
#define CLASS4_TAP_VALUE      2600	//2600     //级别4				约2.0V

//要定义更多的电压级别，在此处添加宏定义即可


//充电电源插入检测IO
#define	USBLINK_INSERT_DETECT_PIN	GPIO_Pin_1			//PC.1  待定

#define	CHARGE_DETECT_PIN	GPIO_Pin_0			//PE.0 

//电池电压检测IO
#define PV_DETECT_PIN			GPIO_Pin_2			//PC.2

void InitPowerDetectModule(void);
unsigned int GetPowerVoltageClass(void);

unsigned int USBLink_Insert_Detect(void);

unsigned int  ChargeState_Detect(void);

int GetPowerVoltagePercent(void);
#endif