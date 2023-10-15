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
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "config.h"
#include "calculate.h"
#include "led.h"
#include "timer.h"

typedef struct
{
    lv_obj_t *scr;
    lv_obj_t *equal_btn, *dot_btn, *btn_sub, *btn_div, *btn_mult, *btn_plus, *btn_C, *btn_delete;
    lv_obj_t *eq_label, *dot_label, *label_sub, *label_div, *label_mult, *label_plus, *label_C, *label_delete; 
    
    lv_obj_t *btn_0, *btn_1, *btn_2, *btn_3, *btn_4, *btn_5, *btn_6, *btn_7, *btn_8, *btn_9;
    lv_obj_t *label_0, *label_1, *label_2, *label_3, *label_4, *label_5, *label_6, *label_7, *label_8, *label_9;
    
    lv_obj_t *label_result;
    lv_obj_t *label_input;
    
    uint16_t btn_size_x;
    uint16_t btn_size_y;
    uint16_t btn_pos_x;  // 第一个按钮位置
    uint16_t btn_pos_y;  // 第一个按钮位置
}my_lv_obj_t;

static my_lv_obj_t my_obj = {
    .btn_size_x = 80,
    .btn_size_y = 35,
    .btn_pos_x = 0,
};

static void input_handle(char *operate)
{
    static uint8_t is_get_result = 0;
    if(operate == NULL || strlen(operate) == 0)
    {
         return;
    }
    
    if(is_get_result)
    {
         lv_label_set_text(my_obj.label_input, " ");
         is_get_result = 0;
    }
    
    if(strncmp(operate, "C", 2) == 0)
    {
        lv_label_set_text(my_obj.label_result, " ");
        lv_label_set_text(my_obj.label_input, " ");
        return;
    }
    
    if(strncmp(operate, "=", 2) == 0)
    {
        char *expression = lv_label_get_text(my_obj.label_input);
        int result = evaluateInfixExpression(expression);
        lv_label_set_text_fmt(my_obj.label_result, "%d", result);
        is_get_result = 1;
        return;
    }
    
    if(strncmp(operate, "<", 2) == 0)
    {
        char *expression = lv_label_get_text(my_obj.label_input);
        if(strlen(expression) > 0)
        {
            char short_express[64] = {0};
            strncpy(short_express, expression, strlen(expression) - 1);
            lv_label_set_text_fmt(my_obj.label_input, "%s", short_express); 
        }
        return;
    }
    
    /* 显示正常算式 */
    char *expression = lv_label_get_text(my_obj.label_input);
    if(strlen(expression) <= 18 && strlen(expression) > 0)
    {
        char output[64] = {0};
        snprintf(output, sizeof(output), "%s%s", expression, operate);
        printf("%s\r\n", output);
        lv_label_set_text_fmt(my_obj.label_input, "%s", output); 
    }

}


static void event_handler(lv_event_t * event)
{
    lv_event_code_t code = lv_event_get_code(event);
    
    if(code == LV_EVENT_CLICKED) 
    {
        lv_obj_t *obj = lv_event_get_target(event);
        lv_obj_t *btn_label = lv_obj_get_child(obj, 0);
        char *btn_text = lv_label_get_text(btn_label);
        printf("%s Clicked\r\n", btn_text);
        
        input_handle(btn_text);
    }
}

void lvgl_task(void *pvParameters)
{
    my_obj.btn_pos_y = MY_DISP_HOR_RES - my_obj.btn_size_y * 6;
    
    timer3_init();
    lv_init();
    
    #if 1
    lv_port_disp_init();
    lv_port_indev_init();    
   
    my_obj.scr = lv_obj_create(NULL);                                     
    my_obj.equal_btn = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.equal_btn, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.equal_btn, my_obj.btn_pos_x + my_obj.btn_size_x * 2, my_obj.btn_pos_y + my_obj.btn_size_y * 5);
    lv_obj_add_event_cb(my_obj.equal_btn, event_handler, LV_EVENT_ALL, NULL);
    my_obj.eq_label = lv_label_create(my_obj.equal_btn);
    lv_label_set_text(my_obj.eq_label, "=");
    lv_obj_align(my_obj.eq_label, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_0 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_0, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_0, my_obj.btn_pos_x + my_obj.btn_size_x * 1, my_obj.btn_pos_y + my_obj.btn_size_y * 5);
    lv_obj_add_event_cb(my_obj.btn_0, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_0 = lv_label_create(my_obj.btn_0);
    lv_label_set_text(my_obj.label_0, "0");
    lv_obj_align(my_obj.label_0, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.dot_btn = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.dot_btn, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.dot_btn, my_obj.btn_pos_x + my_obj.btn_size_x * 0, my_obj.btn_pos_y + my_obj.btn_size_y * 5);
    lv_obj_add_event_cb(my_obj.dot_btn, event_handler, LV_EVENT_ALL, NULL);
    my_obj.dot_label = lv_label_create(my_obj.dot_btn);
    lv_label_set_text(my_obj.dot_label, ".");
    lv_obj_align(my_obj.dot_label, LV_ALIGN_CENTER, 0, 0);
    

    my_obj.btn_3 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_3, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_3, my_obj.btn_pos_x + my_obj.btn_size_x * 2, my_obj.btn_pos_y + my_obj.btn_size_y * 4);
    lv_obj_add_event_cb(my_obj.btn_3, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_3 = lv_label_create(my_obj.btn_3);
    lv_label_set_text(my_obj.label_3, "3");
    lv_obj_align(my_obj.label_3, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_2 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_2, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_2, my_obj.btn_pos_x + my_obj.btn_size_x * 1, my_obj.btn_pos_y + my_obj.btn_size_y * 4);;
    lv_obj_add_event_cb(my_obj.btn_2, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_2 = lv_label_create(my_obj.btn_2);
    lv_label_set_text(my_obj.label_2, "2");
    lv_obj_align(my_obj.label_2, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_1 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_1, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_1, my_obj.btn_pos_x + my_obj.btn_size_x * 0, my_obj.btn_pos_y + my_obj.btn_size_y * 4);
    lv_obj_add_event_cb(my_obj.btn_1, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_1 = lv_label_create(my_obj.btn_1);
    lv_label_set_text(my_obj.label_1, "1");
    lv_obj_align(my_obj.label_1, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_6 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_6, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_6, my_obj.btn_pos_x + my_obj.btn_size_x * 2, my_obj.btn_pos_y + my_obj.btn_size_y * 3);
    lv_obj_add_event_cb(my_obj.btn_6, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_6 = lv_label_create(my_obj.btn_6);
    lv_label_set_text(my_obj.label_6, "6");
    lv_obj_align(my_obj.label_6, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_5 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_5, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_5, my_obj.btn_pos_x + my_obj.btn_size_x * 1, my_obj.btn_pos_y + my_obj.btn_size_y * 3);
    lv_obj_add_event_cb(my_obj.btn_5, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_5 = lv_label_create(my_obj.btn_5);
    lv_label_set_text(my_obj.label_5, "5");
    lv_obj_align(my_obj.label_5, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_4 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_4, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_4, my_obj.btn_pos_x + my_obj.btn_size_x * 0, my_obj.btn_pos_y + my_obj.btn_size_y * 3);
    lv_obj_add_event_cb(my_obj.btn_4, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_4 = lv_label_create(my_obj.btn_4);
    lv_label_set_text(my_obj.label_4, "4");
    lv_obj_align(my_obj.label_4, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_9 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_9, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_9, my_obj.btn_pos_x + my_obj.btn_size_x * 2, my_obj.btn_pos_y + my_obj.btn_size_y * 2);
    lv_obj_add_event_cb(my_obj.btn_9, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_9 = lv_label_create(my_obj.btn_9);
    lv_label_set_text(my_obj.label_9, "9");
    lv_obj_align(my_obj.label_9, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_8 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_8, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_8, my_obj.btn_pos_x + my_obj.btn_size_x * 1, my_obj.btn_pos_y + my_obj.btn_size_y * 2);
    lv_obj_add_event_cb(my_obj.btn_8, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_8 = lv_label_create(my_obj.btn_8);
    lv_label_set_text(my_obj.label_8, "8");
    lv_obj_align(my_obj.label_8, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_7 = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_7, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_7, my_obj.btn_pos_x + my_obj.btn_size_x * 0, my_obj.btn_pos_y + my_obj.btn_size_y * 2);
    lv_obj_add_event_cb(my_obj.btn_7, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_7 = lv_label_create(my_obj.btn_7);
    lv_label_set_text(my_obj.label_7, "7");
    lv_obj_align(my_obj.label_7, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_sub = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_sub, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_sub, my_obj.btn_pos_x + my_obj.btn_size_x * 0, my_obj.btn_pos_y + my_obj.btn_size_y * 1);
    lv_obj_add_event_cb(my_obj.btn_sub, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_sub = lv_label_create(my_obj.btn_sub);
    lv_label_set_text(my_obj.label_sub, "-");
    lv_obj_align(my_obj.label_sub, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_div = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_div, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_div, my_obj.btn_pos_x + my_obj.btn_size_x * 1, my_obj.btn_pos_y + my_obj.btn_size_y * 1);
    lv_obj_add_event_cb(my_obj.btn_div, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_div = lv_label_create(my_obj.btn_div);
    lv_label_set_text(my_obj.label_div, "/");
    lv_obj_align(my_obj.label_div, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_mult = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_mult, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_mult, my_obj.btn_pos_x + my_obj.btn_size_x * 2, my_obj.btn_pos_y + my_obj.btn_size_y * 1);
    lv_obj_add_event_cb(my_obj.btn_mult, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_mult = lv_label_create(my_obj.btn_mult);
    lv_label_set_text(my_obj.label_mult, "*");
    lv_obj_align(my_obj.label_mult, LV_ALIGN_CENTER, 0, 0);

    my_obj.btn_plus = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_plus, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_plus, my_obj.btn_pos_x + my_obj.btn_size_x * 0, my_obj.btn_pos_y + my_obj.btn_size_y * 0);
    lv_obj_add_event_cb(my_obj.btn_plus, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_plus = lv_label_create(my_obj.btn_plus);
    lv_label_set_text(my_obj.label_plus, "+");
    lv_obj_align(my_obj.label_plus, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_C = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_C, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_C, my_obj.btn_pos_x + my_obj.btn_size_x * 1, my_obj.btn_pos_y + my_obj.btn_size_y * 0);
    lv_obj_add_event_cb(my_obj.btn_C, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_C = lv_label_create(my_obj.btn_C);
    lv_label_set_text(my_obj.label_C, "C");
    lv_obj_align(my_obj.label_C, LV_ALIGN_CENTER, 0, 0);
    
    my_obj.btn_delete = lv_btn_create(my_obj.scr);
    lv_obj_set_size(my_obj.btn_delete, my_obj.btn_size_x, my_obj.btn_size_y);
    lv_obj_set_pos(my_obj.btn_delete, my_obj.btn_pos_x + my_obj.btn_size_x * 2, my_obj.btn_pos_y + my_obj.btn_size_y * 0);
    lv_obj_add_event_cb(my_obj.btn_delete, event_handler, LV_EVENT_ALL, NULL);
    my_obj.label_delete = lv_label_create(my_obj.btn_delete);
    lv_label_set_text(my_obj.label_delete, "<");
    lv_obj_align(my_obj.label_delete, LV_ALIGN_CENTER, 0, 0);

    /* 结果输出 */
    my_obj.label_result = lv_label_create(my_obj.scr);
    lv_obj_set_size(my_obj.label_result, 240, 30);
    lv_obj_set_pos(my_obj.label_result, 0, 15);
    lv_label_set_text(my_obj.label_result, "123456789");
    lv_label_set_long_mode(my_obj.label_result, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(my_obj.label_result, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_style_text_font(my_obj.label_result, &lv_font_montserrat_28, 0);
    
    /* 算数输入 */
    my_obj.label_input = lv_label_create(my_obj.scr);
    lv_obj_set_size(my_obj.label_input, 240, 25);
    lv_obj_set_pos(my_obj.label_input, 0, 75);
    lv_label_set_text(my_obj.label_input, "987654321");
    lv_label_set_long_mode(my_obj.label_input, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(my_obj.label_input, LV_TEXT_ALIGN_RIGHT, 0);

    lv_scr_load(my_obj.scr);
    #endif
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

static TaskHandle_t  lvgl_task_handle;

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
 
    // lvgl_task(NULL);
    
	xTaskCreate(lvgl_task, "lvgl_task", 4*1024, NULL, 5, &lvgl_task_handle); 
    
    vTaskStartScheduler(); 

    return 0;
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
