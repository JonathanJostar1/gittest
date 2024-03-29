#include "spi.h"
					  
void SPIx_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable SPI1 and GPIOA clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1|RCC_APB2Periph_AFIO, ENABLE);
	
	/* Configure SPI1 pins: NSS, SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//SPI1 NSS 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_SetBits(GPIOC, GPIO_Pin_4); 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4);   

	/* SPI1 configuration */ 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	                  
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;               
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	 		            
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;		              
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			              
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				  
	SPI_InitStructure.SPI_CRCPolynomial = 7;						  
	SPI_Init(SPI1, &SPI_InitStructure);
	/* Enable SPI1  */
	// SPI_Cmd(SPI1, ENABLE); 								
}   


uint8_t SPIx_ReadWriteByte(uint8_t TxData)
{		
	uint8_t retry=0;				 
	while((SPI1->SR&1<<1)==0)
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	 
	retry=0;
	while((SPI1->SR&1<<0)==0) 
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;         	    
}































