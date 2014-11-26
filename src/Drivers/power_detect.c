/**
* @file power_detect.c
* @brief 电池电压检测以及充电状态检测 
* @version 1.0
* @author joe
* @date 2009年11月12日
* @note
*    1.利用STM32 的ADC模块检测电池经过一个分压电路之后的分压。  
*	  2.检测充电IC的状态IO，可以得知是否在充电以及充电是否完成
*/
#include "stm32f10x_lib.h" 
#include "power_detect.h"

static void ADC_Module_Init(void);
unsigned int	charge_state_cnt;
unsigned int	charge_detect_io_cnt;
unsigned int	last_charge_detect_io_cnt;

/**
* @brief	初始化用于电池电压检测的ADC模块
* @param[in]  none
* @param[out] none
* @return     none
* @note                    
*/
static void ADC_Module_Init(void)
{
	ADC_InitTypeDef   adc_init;
	GPIO_InitTypeDef  gpio_init;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//PC.2   ADC-IN
	gpio_init.GPIO_Pin  = PV_DETECT_PIN;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &gpio_init);

	adc_init.ADC_Mode               = ADC_Mode_Independent;		//
	adc_init.ADC_ScanConvMode       = DISABLE;
	adc_init.ADC_ContinuousConvMode = ENABLE;
	adc_init.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
	adc_init.ADC_DataAlign          = ADC_DataAlign_Right;
	adc_init.ADC_NbrOfChannel       = 1;
	ADC_Init(ADC1, &adc_init);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_13Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	// 下面是ADC自动校准，开机后需执行一次，保证精度
	// Enable ADC1 reset calibaration register 
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC1));

	// Start ADC1 calibaration
	ADC_StartCalibration(ADC1);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));
	// ADC自动校准结束---------------

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);			//开始转换
}

/**
* @brief	  初始化用于USB插入检测的IO	 或者 (充电状态检测IO) 
* @param[in]  none
* @param[out] none
* @return     none
* @note                    
*/
static void USBLinkDetectIO_Init(void)
{	
	GPIO_InitTypeDef  gpio_init;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	//PC.1   ChargePower_Insert detect
	gpio_init.GPIO_Pin  = USBLINK_INSERT_DETECT_PIN;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &gpio_init);

	//PC.0   for debug  test StartDelay()
	//gpio_init.GPIO_Pin  = GPIO_Pin_0;
	//gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
	//GPIO_Init(GPIOC, &gpio_init);
	//GPIO_ResetBits(GPIOC,GPIO_Pin_0);

	/* Connect EXTI Line1 to PC.1 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);

	EXTI_ClearITPendingBit(EXTI_Line1);
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	EXTI_GenerateSWInterrupt(EXTI_Line1);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);      
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

	//PE.0   ChargeState detect
	gpio_init.GPIO_Pin  = CHARGE_DETECT_PIN;
	gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &gpio_init);

	/* Connect EXTI Line0 to PE.0 */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource0);

	EXTI_ClearITPendingBit(EXTI_Line0);
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	EXTI_GenerateSWInterrupt(EXTI_Line0);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);      
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	charge_detect_io_cnt = 0;
	last_charge_detect_io_cnt = 1;
}

/**
* @brief	  初始化处理电池电压检测以及充电状态检测的模块	  
* @param[in]  none
* @param[out] none
* @return     none
* @note                    
*/
void InitPowerDetectModule(void)
{
	USBLinkDetectIO_Init();
	ADC_Module_Init();
}
/**
* @brief	返回检测到的电压的级别
* @param[in]  none
* @param[out] none
* @return     none
* @note     暂定4级，当系统返回的电压级别是0的时候，表示系统的电压低了
*			电池的工作电压范围是: 4.7 -- 3.5
*			VREF+ - VREF- = 3.38V
*			理论上得到的转换值的最大值是: (8.4 * 110/(110 + 220))/(VREF+ - VREF-) * 0x0fff = 0xD40 (3392)
*			理论上得到的转换值的最小值是: (6.8 * 110/(110 + 220))/(VREF+ - VREF-) * 0x0fff = 0xABA (2746)
*			所以每一个级别的粒度是：(3392 - 2746)/ 4 = 161
*           已经经过测试，实测值和理论值的偏差不大，可以满足应用需求
*/
unsigned int GetPowerVoltageClass(void)
{ 
	unsigned int  i,result = 0;
	unsigned short  temp[20];
	unsigned short	min,max;

	for(i = 0;i < 20;i++)
	{
		temp[i] = ADC_GetConversionValue(ADC1);		//得到AD转换的结果
		result += temp[i];
		if (i == 0)
		{
			min = temp[i];
			max = temp[i];
		}

		if (temp[i] < min)
		{
			min = temp[i];
		}

		if (temp[i] > max)
		{
			max = temp[i];
		}
	}

	//取20次值之后,去掉最小值和最大周，再取平均值，简单的平滑滤波
	result -= min;
	result -= max;
	result /= 18; 

	if (result > CLASS1_TAP_VALUE) 
	{
		return 4;
	}
	else if (result > CLASS2_TAP_VALUE) 
	{
		return 3;
	}
	else if (result > CLASS3_TAP_VALUE) 
	{
		return 2;
	}
	else if (result > CLASS4_TAP_VALUE) 
	{
		return 1;
	}
	else
		return 0;
}
/**
* @brief	返回检测到的电压的百分比值
* @param[in]  none
* @param[out] none
* @return     none
* @note     比如25%，那么返回值就是25;
另外，为了避免使用浮点数，采取四舍五入的方式，15.5%时，返回值16
*/
int GetPowerVoltagePercent(void)
{ 
#if 0
	unsigned int  i,result = 0;
	unsigned short  temp[20];
	unsigned short	min,max;

	for(i = 0;i < 20;i++)
	{
		temp[i] = ADC_GetConversionValue(ADC1);		//得到AD转换的结果
		result += temp[i];
		if (i == 0)
		{
			min = temp[i];
			max = temp[i];
		}

		if (temp[i] < min)
		{
			min = temp[i];
		}

		if (temp[i] > max)
		{
			max = temp[i];
		}
	}

	//取20次值之后,去掉最小值和最大周，再取平均值，简单的平滑滤波
	result -= min;
	result -= max;
	result /= 18; 

	if (result < CLASS_PROTECTED)
	{
		return -1;
	}
	if (result > CLASS_MAX_VALUE)
	{
		result = CLASS_MAX_VALUE;
	}
	if (result < CLASS_MIN_VALUE)
	{
		result = CLASS_MIN_VALUE;
	}
	result = (result - CLASS_MIN_VALUE)*100;

	i = result/(CLASS_MAX_VALUE - CLASS_MIN_VALUE);

	if (result%(CLASS_MAX_VALUE - CLASS_MIN_VALUE) > ((CLASS_MAX_VALUE - CLASS_MIN_VALUE)/2)) 
	{
		i += 1;
	}

	if ((80<i)&&(i < 88))
	{
		i += 5;
	}
	if ((15 < i)&&(i<35))
	{
		i -= 8;
	}

	return i;
#endif
	return 0;

}



/**
* @brief	检测充电状态
* @param[in]  none
* @param[out] none
* @return     1:  充电完成    0: 正在充电
* @note  正在充电时，充电检测IO输出脉冲，充满后输出低电平 
*        需要注意的是：此函数只有在得知充电电源已经插入的前提下，再去检测才有意义。因为没有插入充电电源时，检测IO也是低电平的。
*		利用IO的边沿中断来完成充电状态的检测，如果一直在进中断，那说明正在充电，否则表示充电完成
*/
unsigned int  ChargeState_Detect(void)
{
	if (charge_detect_io_cnt != last_charge_detect_io_cnt)
	{
		charge_state_cnt = 50000;
		last_charge_detect_io_cnt = charge_detect_io_cnt;
		return 0;
	}
	else
	{
		if (charge_state_cnt--)
		{
			return 0;
		}
	}


	return 1;
}

/**
* @brief	检测USB是否插入
* @param[in]  none
* @param[out] none
* @return     0:  插入    1: 没有插入
* @note  如果需要在中断里面去实现，那么还需要在初始化时给此检测IO分配外部中断
*		 如果在任务级查询，那么可以之际调用此函数来检测是否插入充电电源                 
*/
unsigned int USBLink_Insert_Detect(void)
{
	unsigned int i;
	if(GPIO_ReadInputDataBit(GPIOC, USBLINK_INSERT_DETECT_PIN))
	{
		for (i=0;i < 2000;i++);		//延时一小段时间，防止是因为抖动造成的
		if(GPIO_ReadInputDataBit(GPIOC, USBLINK_INSERT_DETECT_PIN))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
		return 1;
}



