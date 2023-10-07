#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "stm32f10x.h"


// SPI�����ٶ����� 
#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

void SPIx_Init(void);			 //��ʼ��SPI��
uint8_t SPIx_ReadWriteByte(uint8_t TxData);//SPI���߶�дһ���ֽ�
		 
#endif

