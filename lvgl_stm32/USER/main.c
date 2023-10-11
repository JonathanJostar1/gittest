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
#include "xpt2046_lcd.h"


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

/* ����LED���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
// R-��ɫ
#define LED1_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define LED1_GPIO_PIN		GPIO_Pin_5			        /* ���ӵ�SCLʱ���ߵ�GPIO */

// G-��ɫ
#define LED2_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define LED2_GPIO_PIN		GPIO_Pin_0			        /* ���ӵ�SCLʱ���ߵ�GPIO */

// B-��ɫ
#define LED3_GPIO_PORT    	GPIOB			            /* GPIO�˿� */
#define LED3_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define LED3_GPIO_PIN		GPIO_Pin_1			        /* ���ӵ�SCLʱ���ߵ�GPIO */
#define digitalToggle(p,i)  {p->ODR ^=i;}                //�����ת״̬
#define	digitalHi(p,i)		{p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		{p->BRR=i;}	 //����͵�ƽ
/* �������IO�ĺ� */
#define LED1_TOGGLE		 digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_OFF		   digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED1_ON			   digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)

void LED_GPIO_Config(void)
{		
		/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*����LED��ص�GPIO����ʱ��*/
		RCC_APB2PeriphClockCmd( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;	

		/*��������ģʽΪͨ���������*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*������������Ϊ50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
		
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

		/*���ÿ⺯������ʼ��GPIO*/
		GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
		
		/*ѡ��Ҫ���Ƶ�GPIO����*/
		GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

		/*���ÿ⺯������ʼ��GPIOF*/
		GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);

		/* �ر�����led��	*/
		GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
		
		/* �ر�����led��	*/
		GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);	 
    
    /* �ر�����led��	*/
		GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked\r\n");
    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled\r\n");
    }
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
    
    LED_GPIO_Config();
    
    timer3_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
   
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_size(btn1, 100, 40);
    lv_obj_set_pos(btn1, 0, 0);
    // lv_obj_set_align(btn1, LV_ALIGN_CENTER);
    lv_obj_add_event_cb(btn1, event_handler, LV_EVENT_ALL, NULL);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_CHECKABLE);
    
    
    lv_obj_t *label = lv_label_create(btn1);
    lv_label_set_text(label, "Toggle");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    
//    lv_obj_t *label1 = lv_label_create(scr);
//    lv_label_set_text(label1, "0");
//    lv_obj_set_pos(label1, 0, 0);
//    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    lv_scr_load(scr);  
    
    LED1_ON;
    while(1)
    {   
        lv_timer_handler();
        delay_ms(10);
        
        if(TOUCH_PRESSED == XPT2046_TouchDetect())
        {
             LED1_ON;
        }
        else
        {
             LED1_OFF;
        }
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
