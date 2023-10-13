#include "delay.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

static uint8_t  fac_us=0;							//us延时倍乘数			   
static uint16_t fac_ms=0;							//ms延时倍乘数,在ucos下,代表每个节拍的ms数
	
	
#if SYSTEM_SUPPORT_OS
extern void xPortSysTickHandler(void);    //这句很重要
/**
 * @brief       systick中断服务函数,使用OS时用到
 * @param       ticks: 延时的节拍数
 * @retval      无
 */
void SysTick_Handler(void)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) /* OS开始跑了,才执行正常的调度处理 */
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

	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD = reload; 						//每1/delay_ostickspersec秒中断一次	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK    

#else
	fac_ms = (uint16_t)fac_us*1000;					// 非OS下,代表每个ms需要的systick时钟数   
#endif
}								    

#if SYSTEM_SUPPORT_OS  							//如果需要支持OS.
//延时nus
//nus为要延时的us数.		    								   
void delay_us(uint32_t us)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;			//LOAD的值	    	 
	ticks = us*fac_us; 					    //需要的节拍数	  		 
	tcnt = 0;
	told = SysTick->VAL;        		    //刚进入时的计数器值
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if (tnow < told)
            {
                //这里注意一下SYSTICK是一个递减的计数器就可以了.
                tcnt += told-tnow;		
            }
			else 
            {
                tcnt += reload-tnow+told;
            }                
			told = tnow;
			if(tcnt >= ticks)
            {
                //时间超过/等于要延迟的时间,则退出.
                break;				
            }
		}  
	};								    
}
//延时nms
//nms:要延时的ms数
void delay_ms(uint16_t ms)
{	
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        //系统已经运行		
        if(ms >= fac_ms)						 
        { 
            //延时的时间大于OS的最少时间周期
            vTaskDelay(ms / fac_ms);	 		
        }
        ms %= fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时    
    }
    delay_us((u32)(ms * 1000));				//普通方式延时 
}
#else //不用OS时
//延时nus
//nus为要延时的us数.		    								   
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//时间加载	  		 
	SysTick->VAL=0x00;        					//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;      					 //清空计数器	 
}
//延时nms
//注意nms的范围
//SysTick->LOAD为24位寄存器,所以,最大延时为:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK单位为Hz,nms单位为ms
//对72M条件下,nms<=1864 
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;							//清空计数器
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//等待时间到达   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//关闭计数器
	SysTick->VAL =0X00;       					//清空计数器	  	    
} 
#endif 
