#include "timer.h"

void timer3_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    TIM_InitStruct.TIM_Prescaler = 71; // ����ʱ�ӷ�Ƶ��ʹ������Ƶ��Ϊ1kHz
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
    TIM_InitStruct.TIM_Period = 1000 - 1; // �����Զ���װ�ؼĴ���ARR�Ի��1������ж�
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_InitStruct);

    
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn; // TIM3�����ж�
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0; // �����ж����ȼ�
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // ����ȫ���ж�
    __enable_irq();
    
    TIM_Cmd(TIM3, ENABLE);
}