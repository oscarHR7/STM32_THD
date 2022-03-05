#include"STM32f10x_lib.h"
#include"init.h"
#include "stm32_dsp.h"
#include "table_fft.h"
#include<math.h>
#include<stdio.h>

#define N  1024

long int data_in[N],data_out[N];
float Mag[N/2];


int fputc(int ch,FILE *f)
{
    USART_SendData(USART1,(u8)ch);
	  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
	  return ch;
}

void GetPowerMag()
 {
     signed short XR,XI;
     float X,Y,M;
     unsigned short k;
     for(k=0; k<N/2; k++)
     {
         XR  = (data_out[k] << 16) >> 16;
         XI  = (data_out[k] >> 16);
         X =   (float)XR;
         Y =   (float)XI;
         M = sqrt(X * X + Y * Y) ;
         if(k == 0)
             Mag[k] = M ;
         else
             Mag[k] = M *2;
     }
}

unsigned short GetMaxMag(void)
{
     unsigned short k,find_k;
	   float Max;
	   Max    = Mag[25];
	   find_k = 25;
	   for(k=25;k<N/2;k++)
	   {
		      if(Mag[k]>Max)
					{
					      Max = Mag[k];
						    find_k = k;
					}
		 }
		 return(find_k);
}

float GetNWave(int k)
{
    return sqrt( (Mag[k-1] * Mag[k-1] + Mag[k] * Mag[k] + Mag[k+1] * Mag[k+1] ))*3.3/4096;
}


void delay(unsigned short t)
{
   while(t>0)
		   t--;
}

/******************************************************************************
* ������  		: Systick_Configuration
* ��������    	: ����Systick��ʱ��,��װ��ʱ��Ϊ1s
* �������      : ��
* ������      : ��
* ����ֵ        : ��
******************************************************************************/
void Systick_Configuration(void)
{
    /* ʧ��Systick��ʱ�� */
    SysTick_CounterCmd(SysTick_Counter_Disable);
    /* ѡ��HCLKΪSystickʱ��Դ */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    /* ���Systick������ */
    SysTick_CounterCmd(SysTick_Counter_Clear);
    /* ��ƵΪ72/8MHz�����ü���ֵΪ9000 * 1000���Եõ�1s��ʱ���*/
    SysTick_SetReload(9000 * 1000);			
}

/******************************************************************************
* ������  		: Delay_Second
* ��������    	: 1s��ʱ
* �������      : ��
* ������      : ��
* ����ֵ        : ��
******************************************************************************/
void Delay(u32 time)
{
     /* ��ƵΪ72/8MHz�����ü���ֵΪ9000 * 1000���Եõ�1s��ʱ���*/
     SysTick_SetReload(time);
	  /* ����Systick���� */
    SysTick_CounterCmd(SysTick_Counter_Enable);
    /* �ȴ�Systick������0 */
    while(SysTick_GetFlagStatus(SysTick_FLAG_COUNT) == 0);
    /* ʧ��Systick��ʱ�� */
    SysTick_CounterCmd(SysTick_Counter_Disable);
    /* ���Systick������ */
    SysTick_CounterCmd(SysTick_Counter_Clear);
}

int main(void)
{
   		unsigned short n,k,old_k = 0;
	  float Vmax,old_Vmax,Vo;
	  system_init();
    printf(" \r\n\r\nFrequence Analysis starting...\r\n");
	  while(1)
		{
	      for(n=0;n<N;n++)
        {
		        Delay(412);
			      data_in[n] = ((signed short)ADC_GetConversionValue(ADC1))<<16;
	      }  
	      cr4_fft_1024_stm32(data_out, data_in, N);
        GetPowerMag();
		    k   = GetMaxMag();
				Vmax = Mag[k]*3.3/4096; 
			  Vo   = Mag[0]*3.3/4096;
				if( k!=old_k || fabs(Vmax - old_Vmax)/Vmax > 0.05)   //��⵽Ƶ���б仯��ˢ����ʾ����
	 		    printf(" The frequence of input signal is %d Hz. Vpp = %.2fV  Vo = %.2fV\r\n\r\n",k*20,2*Vmax,Vo);
				old_k = k;
				old_Vmax = Vmax;
			  Delay(9000*8000);
		}
}
