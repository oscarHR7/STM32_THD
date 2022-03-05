#include"STM32f10x_lib.h"
#include"init.h"

void RCC_Configuration(void)									
{										  
     ErrorStatus HSEStartUpStatus;
	 RCC_DeInit();
	 RCC_HSEConfig(RCC_HSE_ON);
	 HSEStartUpStatus = RCC_WaitForHSEStartUp();
	 if(HSEStartUpStatus == SUCCESS)
	 {
	     RCC_HCLKConfig(RCC_SYSCLK_Div1);
		 RCC_PCLK2Config(RCC_HCLK_Div1);
		 RCC_PCLK1Config(RCC_HCLK_Div2);
		 FLASH_SetLatency(FLASH_Latency_2);
		 FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		 RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
		 RCC_PLLCmd(ENABLE);
		 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY)==RESET)
		          ;
	     RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		 while(RCC_GetSYSCLKSource()!=0x08)
		     ;
         RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1|RCC_APB2Periph_GPIOB,ENABLE);
	
	  }
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
}

void USART_Configuration(void)
{
     USART_InitTypeDef USART_InitStructure;
	 USART_InitStructure.USART_BaudRate = 115200;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;
	 USART_InitStructure.USART_Parity = USART_Parity_No;
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	 USART_Init(USART1,&USART_InitStructure);
	 
	 USART_Cmd(USART1,ENABLE);
}

  void ADC_Configuration(void)
{
    ADC_InitTypeDef ADC_InitStructure;
	  RCC_ADCCLKConfig(RCC_PCLK2_Div4);
	  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfChannel = 1;
	  ADC_Init(ADC1,&ADC_InitStructure);
	  
	  ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_55Cycles5);
	  ADC_Cmd(ADC1,ENABLE);
	  ADC_ResetCalibration(ADC1);
	  while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
	  while(ADC_GetCalibrationStatus(ADC1));
	  ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	  
}



void system_init(void)
{
    RCC_Configuration();
    GPIO_Configuration();
	  ADC_Configuration();
	  USART_Configuration();
}	
