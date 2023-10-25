#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "stm32f10x.h"
#include <stdio.h>

//#define  W25X16_ID              0xEF3015   //W25X16
//#define  W25Q16_ID              0xEF4015	 //W25Q16
//#define  W25Q128_ID             0XEF4018   //W25Q128
#define  W25Q64_ID              0XEF4017    //W25Q64

#define SPI_FLASH_PageSize              256
#define SPI_FLASH_PerWritePageSize      256
#define SPI_FLASH_SECTOR_SIZE           4096

/*命令定义-开头*******************************/
#define W25X_WriteEnable		      0x06 
#define W25X_WriteDisable		      0x04 
#define W25X_ReadStatusReg		      0x05 
#define W25X_WriteStatusReg		      0x01 
#define W25X_ReadData			      0x03 
#define W25X_FastReadData		      0x0B 
#define W25X_FastReadDual		      0x3B 
#define W25X_PageProgram		      0x02 
#define W25X_BlockErase			      0xD8 
#define W25X_SectorErase		      0x20 
#define W25X_ChipErase			      0xC7 
#define W25X_PowerDown			      0xB9 
#define W25X_ReleasePowerDown	      0xAB 
#define W25X_DeviceID			      0xAB 
#define W25X_ManufactDeviceID   	  0x90 
#define W25X_JedecDeviceID		      0x9F

/* WIP(busy)标志，FLASH内部正在写入 */
#define WIP_Flag                  0x01
#define Dummy_Byte                0xFF
/*命令定义-结尾*******************************/


/*SPI接口定义-开头****************************/
#define      FLASH_SPIx                        SPI1
#define      FLASH_SPI_APBxClock_FUN          RCC_APB2PeriphClockCmd
#define      FLASH_SPI_CLK                     RCC_APB2Periph_SPI1

//CS(NSS)引脚 片选选普通GPIO即可
#define      FLASH_SPI_CS_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define      FLASH_SPI_CS_CLK                  RCC_APB2Periph_GPIOC    
#define      FLASH_SPI_CS_PORT                 GPIOC
#define      FLASH_SPI_CS_PIN                  GPIO_Pin_0

//SCK引脚
#define      FLASH_SPI_SCK_APBxClock_FUN      RCC_APB2PeriphClockCmd
#define      FLASH_SPI_SCK_CLK                 RCC_APB2Periph_GPIOA   
#define      FLASH_SPI_SCK_PORT                GPIOA   
#define      FLASH_SPI_SCK_PIN                 GPIO_Pin_5
//MISO引脚
#define      FLASH_SPI_MISO_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MISO_CLK                RCC_APB2Periph_GPIOA    
#define      FLASH_SPI_MISO_PORT               GPIOA 
#define      FLASH_SPI_MISO_PIN                GPIO_Pin_6
//MOSI引脚
#define      FLASH_SPI_MOSI_APBxClock_FUN     RCC_APB2PeriphClockCmd
#define      FLASH_SPI_MOSI_CLK                RCC_APB2Periph_GPIOA    
#define      FLASH_SPI_MOSI_PORT               GPIOA 
#define      FLASH_SPI_MOSI_PIN                GPIO_Pin_7

#define  		SPI_FLASH_CS_LOW()     						GPIO_ResetBits( FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN )
#define  		SPI_FLASH_CS_HIGH()    						GPIO_SetBits( FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN )

/*SPI接口定义-结尾****************************/

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

/**
 * @brief  SPI_FLASH初始化
 * @param  无
 * @retval 无
 */
void SPI_FLASH_Init(void);

/**
 * @brief  擦除FLASH扇区
 * @param  SectorAddr：要擦除的扇区地址
 * @retval 无
 */
void SPI_FLASH_SectorErase(uint32_t SectorAddr);

/**
 * @brief  擦除FLASH扇区，整片擦除
 * @param  无
 * @retval 无
 */
void SPI_FLASH_BulkErase(void);

/**
 * @brief  对FLASH按页写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度，必须小于等于SPI_FLASH_PerWritePageSize
 * @retval 无
 */
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

/**
 * @brief  对FLASH写入数据，调用本函数写入数据前需要先擦除扇区
 * @param	pBuffer，要写入数据的指针
 * @param  WriteAddr，写入地址
 * @param  NumByteToWrite，写入数据长度
 * @retval 无
 */
void SPI_FLASH_BufferWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);

/**
 * @brief  读取FLASH数据
 * @param 	pBuffer，存储读出数据的指针
 * @param   ReadAddr，读取地址
 * @param   NumByteToRead，读取数据长度
 * @retval 无
 */
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);

/**
 * @brief  读取FLASH ID
 * @param 	无
 * @retval FLASH ID
 */
uint32_t SPI_FLASH_ReadID(void);

/**
 * @brief  读取FLASH Device ID
 * @param 	无
 * @retval FLASH Device ID
 */
uint32_t SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(uint32_t ReadAddr);

// 进入掉电模式
void SPI_Flash_PowerDown(void);

// 唤醒
void SPI_Flash_WAKEUP(void);

/**
 * @brief  使用SPI读取一个字节的数据
 * @param  无
 * @retval 返回接收到的数据
 */
uint8_t SPI_FLASH_ReadByte(void);

/**
 * @brief  使用SPI发送一个字节的数据
 * @param  byte：要发送的数据
 * @retval 返回接收到的数据
 */
uint8_t SPI_FLASH_SendByte(uint8_t byte);

/**
 * @brief  使用SPI发送两个字节的数据
 * @param  byte：要发送的数据
 * @retval 返回接收到的数据
 */
uint16_t SPI_FLASH_SendHalfWord(uint16_t HalfWord);

/**
 * @brief  向FLASH发送 写使能 命令
 * @param  none
 * @retval none
 */
void SPI_FLASH_WriteEnable(void);

/**
 * @brief  等待WIP(BUSY)标志被置0，即等待到FLASH内部数据写入完毕
 * @param  none
 * @retval none
 */
void SPI_FLASH_WaitForWriteEnd(void);
																					


#endif /* __SPI_FLASH_H */


