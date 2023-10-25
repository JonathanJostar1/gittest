#include "timer.h"

void timer3_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = 71; // 设置时钟分频，使计数器频率为1kHz
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