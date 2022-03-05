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
* 函数名  		: Systick_Configuration
* 函数描述    	: 设置Systick定时器,重装载时间为1s
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
******************************************************************************/
void Systick_Configuration(void)
{
    /* 失能Systick定时器 */
    SysTick_CounterCmd(SysTick_Counter_Disable);
    /* 选择HCLK为Systick时钟源 */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    /* 清除Systick计数器 */
    SysTick_CounterCmd(SysTick_Counter_Clear);
    /* 主频为72/8MHz，配置计数值为9000 * 1000可以得到1s定时间隔*/
    SysTick_SetReload(9000 * 1000);			
}

/******************************************************************************
* 函数名  		: Delay_Second
* 函数描述    	: 1s定时
* 输入参数      : 无
* 输出结果      : 无
* 返回值        : 无
******************************************************************************/
void Delay(u32 time)
{
     /* 主频为72/8MHz，配置计数值为9000 * 1000可以得到1s定时间隔*/
     SysTick_SetReload(time);
	  /* 启动Systick计数 */
    SysTick_CounterCmd(SysTick_Counter_Enable);
    /* 等待Systick计数至0 */
    while(SysTick_GetFlagStatus(SysTick_FLAG_COUNT) == 0);
    /* 失能Systick定时器 */
    SysTick_CounterCmd(SysTick_Counter_Disable);
    /* 清除Systick计数器 */
    SysTick_CounterCmd(SysTick_Counter_Clear);
}

int main(void)
{
   	unsigned short  n,base_k,old_k = 0;
	  int k;
	  float Vmax,old_Vmax;
	  float Vn,Vbase = 0,Vntotal = 0;
	  system_init();

  	//printf(" \r\n\r\nFrequence Analysis starting...\r\n");
	  while(1)
		{
	      for(n=0;n<N;n++)
        {
		        Delay(412);        //频率分辨力接近20Hz
			      data_in[n] = ((signed short)ADC_GetConversionValue(ADC1))<<16;
	      }  
	      cr4_fft_1024_stm32(data_out, data_in, N);
        GetPowerMag();
				
		    base_k   = GetMaxMag();
				Vmax = Mag[base_k]*3.3/4096; 
				
			  if( base_k!=old_k || fabs(Vmax - old_Vmax)/Vmax > 0.05)  
			  {
	 		     printf("FFT Result\r\n");
					 for(k=N/2;k>=0;k--)
					 {
						 printf("k=%d:%.2f\r\n",k,Mag[k]*3.3/4096);
					 }
					 
					n = 1;
					Vntotal = 0;
					printf("\r\n The frequence of input signal is %dHz \r\n\r\n",base_k*20);
					for(k=base_k;k<N/2;k=k+base_k)
					{
						printf(" %d Wave :  %.3f \r\n\r\n",n,GetNWave(k));
						if(k==base_k)
							Vbase = GetNWave(k);
						else
						{
						  Vn =  GetNWave(k);
							Vntotal +=  Vn*Vn;
						}
					   n++;
				  }
				  printf("\r\n Vbase = %.3f Vntotal = %.3f ,失真度为: %.3f%%\r\n\r\n",Vbase,sqrt(Vntotal),sqrt(Vntotal)*100/Vbase);   
				}
				old_k = base_k;
				old_Vmax = Vmax;
			  Delay(9000*1000);
		}
}
