/**
  ******************************************************************************
  * @file    USART/Printf/main.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    04/16/2010
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "delay.h"
#include <stdio.h>
#include "lvgl.h"
#include "usart.h"
#include "examples/porting/lv_port_disp.h"
#include "examples/porting/lv_port_indev.h"
#include "examples/lv_examples.h"


void timer3_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = SystemCoreClock / 1000 - 1; // 设置时钟分频，使计数器频率为1kHz
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
    TIM_InitStruct.TIM_Period = 1000 - 1; // 设置自动重装载寄存器ARR以获得1毫秒的中断
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_InitStruct);

    
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn; // TIM3更新中断
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // 设置中断优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 启用全局中断
    __enable_irq();

    
    TIM_Cmd(TIM3, ENABLE);
}


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
    delay_init();
    uart_init(115200);
    
    timer3_init();
    lv_init();
    lv_port_disp_init();
    
    lv_port_indev_init();
   
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_text(label, "Hello, LVGL!");
    lv_obj_center(label);
    lv_scr_load(scr);
    
    while(1)
    {   
          lv_timer_handler();
          delay_ms(10);
    }
    
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
int fputc(int ch, FILE* f)
{
    USART_SendData(USART1, (unsigned char) ch);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    {
    }
    return ch;
}


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
