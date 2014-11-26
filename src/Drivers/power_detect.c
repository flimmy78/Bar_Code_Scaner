/**
* @file power_detect.c
* @brief ��ص�ѹ����Լ����״̬��� 
* @version 1.0
* @author joe
* @date 2009��11��12��
* @note
*    1.����STM32 ��ADCģ�����ؾ���һ����ѹ��·֮��ķ�ѹ��  
*	  2.�����IC��״̬IO�����Ե�֪�Ƿ��ڳ���Լ�����Ƿ����
*/
#include "stm32f10x_lib.h" 
#include "power_detect.h"

static void ADC_Module_Init(void);
unsigned int	charge_state_cnt;
unsigned int	charge_detect_io_cnt;
unsigned int	last_charge_detect_io_cnt;

/**
* @brief	��ʼ�����ڵ�ص�ѹ����ADCģ��
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

	// ������ADC�Զ�У׼����������ִ��һ�Σ���֤����
	// Enable ADC1 reset calibaration register 
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC1));

	// Start ADC1 calibaration
	ADC_StartCalibration(ADC1);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));
	// ADC�Զ�У׼����---------------

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);			//��ʼת��
}

/**
* @brief	  ��ʼ������USB�������IO	 ���� (���״̬���IO) 
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
* @brief	  ��ʼ�������ص�ѹ����Լ����״̬����ģ��	  
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
* @brief	���ؼ�⵽�ĵ�ѹ�ļ���
* @param[in]  none
* @param[out] none
* @return     none
* @note     �ݶ�4������ϵͳ���صĵ�ѹ������0��ʱ�򣬱�ʾϵͳ�ĵ�ѹ����
*			��صĹ�����ѹ��Χ��: 4.7 -- 3.5
*			VREF+ - VREF- = 3.38V
*			�����ϵõ���ת��ֵ�����ֵ��: (8.4 * 110/(110 + 220))/(VREF+ - VREF-) * 0x0fff = 0xD40 (3392)
*			�����ϵõ���ת��ֵ����Сֵ��: (6.8 * 110/(110 + 220))/(VREF+ - VREF-) * 0x0fff = 0xABA (2746)
*			����ÿһ������������ǣ�(3392 - 2746)/ 4 = 161
*           �Ѿ��������ԣ�ʵ��ֵ������ֵ��ƫ��󣬿�������Ӧ������
*/
unsigned int GetPowerVoltageClass(void)
{ 
	unsigned int  i,result = 0;
	unsigned short  temp[20];
	unsigned short	min,max;

	for(i = 0;i < 20;i++)
	{
		temp[i] = ADC_GetConversionValue(ADC1);		//�õ�ADת���Ľ��
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

	//ȡ20��ֵ֮��,ȥ����Сֵ������ܣ���ȡƽ��ֵ���򵥵�ƽ���˲�
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
* @brief	���ؼ�⵽�ĵ�ѹ�İٷֱ�ֵ
* @param[in]  none
* @param[out] none
* @return     none
* @note     ����25%����ô����ֵ����25;
���⣬Ϊ�˱���ʹ�ø���������ȡ��������ķ�ʽ��15.5%ʱ������ֵ16
*/
int GetPowerVoltagePercent(void)
{ 
#if 0
	unsigned int  i,result = 0;
	unsigned short  temp[20];
	unsigned short	min,max;

	for(i = 0;i < 20;i++)
	{
		temp[i] = ADC_GetConversionValue(ADC1);		//�õ�ADת���Ľ��
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

	//ȡ20��ֵ֮��,ȥ����Сֵ������ܣ���ȡƽ��ֵ���򵥵�ƽ���˲�
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
* @brief	�����״̬
* @param[in]  none
* @param[out] none
* @return     1:  ������    0: ���ڳ��
* @note  ���ڳ��ʱ�������IO������壬����������͵�ƽ 
*        ��Ҫע����ǣ��˺���ֻ���ڵ�֪����Դ�Ѿ������ǰ���£���ȥ���������塣��Ϊû�в������Դʱ�����IOҲ�ǵ͵�ƽ�ġ�
*		����IO�ı����ж�����ɳ��״̬�ļ�⣬���һֱ�ڽ��жϣ���˵�����ڳ�磬�����ʾ������
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
* @brief	���USB�Ƿ����
* @param[in]  none
* @param[out] none
* @return     0:  ����    1: û�в���
* @note  �����Ҫ���ж�����ȥʵ�֣���ô����Ҫ�ڳ�ʼ��ʱ���˼��IO�����ⲿ�ж�
*		 ��������񼶲�ѯ����ô����֮�ʵ��ô˺���������Ƿ�������Դ                 
*/
unsigned int USBLink_Insert_Detect(void)
{
	unsigned int i;
	if(GPIO_ReadInputDataBit(GPIOC, USBLINK_INSERT_DETECT_PIN))
	{
		for (i=0;i < 2000;i++);		//��ʱһС��ʱ�䣬��ֹ����Ϊ������ɵ�
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



