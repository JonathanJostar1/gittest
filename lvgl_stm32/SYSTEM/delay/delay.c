#include "delay.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

static uint8_t  fac_us=0;							//us��ʱ������			   
static uint16_t fac_ms=0;							//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��
	
	
#if SYSTEM_SUPPORT_OS
extern void xPortSysTickHandler(void);    //������Ҫ
/**
 * @brief       systick�жϷ�����,ʹ��OSʱ�õ�
 * @param       ticks: ��ʱ�Ľ�����
 * @retval      ��
 */
void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) /* OS��ʼ����,��ִ�������ĵ��ȴ��� */
    {
        xPortSysTickHandler();
    }

}
#endif

			  
void delay_init(void)
{
#if SYSTEM_SUPPORT_OS  					
	uint32_t reload;
#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	
	fac_us = SystemCoreClock/1000000;				  
#if SYSTEM_SUPPORT_OS  							
	reload = SystemCoreClock/8000000;				
	reload *= 1000000/configTICK_RATE_HZ;	
													
	fac_ms = 1000/configTICK_RATE_HZ;				   

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD = reload; 						//ÿ1/delay_ostickspersec���ж�һ��	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK    

#else
	fac_ms = (uint16_t)fac_us*1000;					// ��OS��,����ÿ��ms��Ҫ��systickʱ����   
#endif
}								    

#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(uint32_t us)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;			//LOAD��ֵ	    	 
	ticks = us*fac_us; 					    //��Ҫ�Ľ�����	  		 
	tcnt = 0;
	told = SysTick->VAL;        		    //�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if (tnow < told)
            {
                //����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
                tcnt += told-tnow;		
            }
			else 
            {
                tcnt += reload-tnow+told;
            }                
			told = tnow;
			if(tcnt >= ticks)
            {
                //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
                break;				
            }
		}  
	};								    
}
//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(uint16_t ms)
{	
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        //ϵͳ�Ѿ�����		
        if(ms >= fac_ms)						 
        { 
            //��ʱ��ʱ�����OS������ʱ������
            vTaskDelay(ms / fac_ms);	 		
        }
        ms %= fac_ms;						//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
    }
    delay_us((u32)(ms * 1000));				//��ͨ��ʽ��ʱ 
}
#else //����OSʱ
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//ʱ�����	  		 
	SysTick->VAL=0x00;        					//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL =0X00;      					 //��ռ�����	 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;				//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;							//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL =0X00;       					//��ռ�����	  	    
} 
#endif 
