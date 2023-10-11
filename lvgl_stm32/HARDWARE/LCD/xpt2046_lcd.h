#ifndef XPT2046_H
#define XPT2046_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
/******************************* XPT2046 触摸屏触摸信号指示引脚定义(不使用中断) ***************************/
#define             XPT2046_PENIRQ_GPIO_CLK                      RCC_APB2Periph_GPIOE   
#define             XPT2046_PENIRQ_GPIO_PORT                     GPIOE
#define             XPT2046_PENIRQ_GPIO_PIN                      GPIO_Pin_4
//触屏信号有效电平
#define             XPT2046_PENIRQ_ActiveLevel                   0
#define             XPT2046_PENIRQ_Read()                        GPIO_ReadInputDataBit ( XPT2046_PENIRQ_GPIO_PORT, XPT2046_PENIRQ_GPIO_PIN )

/******************************* XPT2046 触摸屏模拟SPI引脚定义 ***************************/
#define             XPT2046_SPI_GPIO_CLK                         RCC_APB2Periph_GPIOE| RCC_APB2Periph_GPIOD
#define             XPT2046_SPI_CS_PIN		                     GPIO_Pin_13
#define             XPT2046_SPI_CS_PORT		                     GPIOD
#define	            XPT2046_SPI_CLK_PIN	                         GPIO_Pin_0
#define             XPT2046_SPI_CLK_PORT	                     GPIOE
#define	            XPT2046_SPI_MOSI_PIN	                     GPIO_Pin_2
#define	            XPT2046_SPI_MOSI_PORT	                     GPIOE
#define	            XPT2046_SPI_MISO_PIN	                     GPIO_Pin_3
#define	            XPT2046_SPI_MISO_PORT	                     GPIOE

#define             XPT2046_CS_ENABLE()                          GPIO_SetBits ( XPT2046_SPI_CS_PORT, XPT2046_SPI_CS_PIN )    
#define             XPT2046_CS_DISABLE()                         GPIO_ResetBits ( XPT2046_SPI_CS_PORT, XPT2046_SPI_CS_PIN )  

#define             XPT2046_CLK_HIGH()                           GPIO_SetBits ( XPT2046_SPI_CLK_PORT, XPT2046_SPI_CLK_PIN )    
#define             XPT2046_CLK_LOW()                            GPIO_ResetBits ( XPT2046_SPI_CLK_PORT, XPT2046_SPI_CLK_PIN ) 

#define             XPT2046_MOSI_1()                             GPIO_SetBits ( XPT2046_SPI_MOSI_PORT, XPT2046_SPI_MOSI_PIN ) 
#define             XPT2046_MOSI_0()                             GPIO_ResetBits ( XPT2046_SPI_MOSI_PORT, XPT2046_SPI_MOSI_PIN )
#define             XPT2046_MISO()                               GPIO_ReadInputDataBit ( XPT2046_SPI_MISO_PORT, XPT2046_SPI_MISO_PIN )

/******************************* XPT2046 触摸屏参数定义 ***************************/
//校准触摸屏时触摸坐标的AD值相差门限 
#define             XPT2046_THRESHOLD_CalDiff                    2               

#define	            XPT2046_CHANNEL_X 	                         0x90 	          //通道Y+的选择控制字	
#define	            XPT2046_CHANNEL_Y 	                         0xd0	          //通道X+的选择控制字

//触摸参数写到FLASH里的标志
#define				FLASH_TOUCH_PARA_FLAG_VALUE					0xA5

//触摸标志写到FLASH里的地址
#define 			FLASH_TOUCH_PARA_FLAG_ADDR			        (1*1024)

//触摸参数写到FLASH里的地址
#define 			FLASH_TOUCH_PARA_ADDR						(2*1024)

//触摸消抖阈值
#define DURIATION_TIME		    2

#define TOUCH_PRESSED 			1
#define TOUCH_RELEASE			0

typedef enum
{
   	XPT2046_STATE_RELEASE  = 0,	//触摸释放
	XPT2046_STATE_WAITING,		//触摸按下
	XPT2046_STATE_PRESSED,		//触摸按下 
}touch_state_t;

/**
  * @brief  XPT2046 初始化函数
  * @param  无
  * @retval 无
  */	
void XPT2046_Init(void);

/**
  * @brief  获取 XPT2046 触摸点（校准后）的坐标
  * @param  pDisplayCoordinate ：该指针存放获取到的触摸点坐标
  * @retval 获取情况
	*   该返回值为以下值之一：
  *     @arg 1 :获取成功
  *     @arg 0 :获取失败
  */
uint8_t XPT2046_Get_TouchedPoint(int16_t *x , int16_t *y);

/**
  * @brief  触摸屏检测状态机
  * @retval 触摸状态
	*   该返回值为以下值之一：
  *     @arg TOUCH_PRESSED :触摸按下
  *     @arg TOUCH_NOT_PRESSED :无触摸
  */
uint8_t XPT2046_TouchDetect(void);


/**
	* @brief   检测到触摸中断时调用的处理函数
	* @note    本函数需要在while循环里被调用，也可使用定时器定时调用。
	* @param   none
	* @retval  none
	*/
void XPT2046_TouchEvenHandler(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XPT2046_H */
