/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ��������ʵ��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-MINI STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
 
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"	
#include "./lcd/bsp_ili9341_lcd.h"
#include "./lcd/bsp_xpt2046_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include "./led/bsp_led.h" 
#include "palette.h"
#include <string.h>


extern uint16_t lcdid;

int main(void)
{		
	//LCD ��ʼ��
	ILI9341_Init();  
	
	//��������ʼ��
	XPT2046_Init();
	//��FLASH���ȡУ����������FLASH�޲�������ʹ��ģʽ3����У��
  if(lcdid == LCDID_ILI9341)
  {
    Calibrate_or_Get_TouchParaWithFlash(3,0); //����������ԣ���Ҫǿ������У׼���ѵڶ�������0�ĳ�1����
  }
  else if(lcdid == LCDID_ST7789V)
  {
    Calibrate_or_Get_TouchParaWithFlash(5,0); //����������ԣ���Ҫǿ������У׼���ѵڶ�������0�ĳ�1����
  }

	/* USART config */
	USART_Config();  
	LED_GPIO_Config();
	
	printf("\r\n ********** ����������� *********** \r\n"); 
	printf("\r\n ��������ʾ�����������Ķ������е�readme.txt�ļ�˵��������Ҫ���FLASH��ˢ��ģ����\r\n"); 
 
	//����0��3��5��6 ģʽ�ʺϴ���������ʾ���֣�
	//���Ƽ�ʹ������ģʽ��ʾ����	����ģʽ��ʾ���ֻ��о���Ч��			
	//���� 6 ģʽΪ�󲿷�Һ�����̵�Ĭ����ʾ����  
  if(lcdid == LCDID_ILI9341)
  {
    ILI9341_GramScan ( 3 );
  }
  else if(lcdid == LCDID_ST7789V)
  {
    ILI9341_GramScan ( 5 );
  }
	
	//���ƴ����������
	Palette_Init(LCD_SCAN_MODE);
	
	while ( 1 )
	{
		//������⺯��������������10ms����һ��
			XPT2046_TouchEvenHandler();
	}
		
}





/* ------------------------------------------end of file---------------------------------------- */

