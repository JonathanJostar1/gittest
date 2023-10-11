#include "xpt2046_lcd.h"
#include "spi_flash.h"

typedef struct         //У׼ϵ���ṹ�壨����ʹ�ã�
{
	 float dX_X, dX_Y, dX, dY_X, dY_Y, dY;
} strType_XPT2046_TouchPara;

/******************************* ���� XPT2046 ȫ�ֱ��� ***************************/
//Ĭ�ϴ�����������ͬ����Ļ���в��죬�����µ��ô���У׼������ȡ
strType_XPT2046_TouchPara strXPT2046_TouchPara[] = { 	
    -0.006464,   -0.073259,  280.358032,    0.074878,    0.002052,   -6.545977, //ɨ�跽ʽ0
    0.086314,    0.001891,  -12.836658,   -0.003722,   -0.065799,  254.715714,  //ɨ�跽ʽ1
    0.002782,    0.061522,  -11.595689,    0.083393,    0.005159,  -15.650089,  //ɨ�跽ʽ2
    0.089743,   -0.000289,  -20.612209,   -0.001374,    0.064451,  -16.054003,  //ɨ�跽ʽ3
    0.000767,   -0.068258,  250.891769,   -0.085559,   -0.000195,  334.747650,  //ɨ�跽ʽ4
    -0.084744,    0.000047,  323.163147,   -0.002109,   -0.066371,  260.985809, //ɨ�跽ʽ5
    -0.001848,    0.066984,  -12.807136,   -0.084858,   -0.000805,  333.395386, //ɨ�跽ʽ6
    -0.085470,   -0.000876,  334.023163,   -0.003390,    0.064725,   -6.211169, //ɨ�跽ʽ7
};

/**
  * @brief  ���� XPT2046 �ļ�΢�뼶��ʱ����
  * @param  nCount ����ʱ����ֵ����λΪ΢��
  * @retval ��
  */	
static void XPT2046_DelayUS(__IO uint32_t ulCount)
{
	uint32_t i;
	for ( i = 0; i < ulCount; i ++ )
	{
		uint8_t uc = 12;     //����ֵΪ12����Լ��1΢��  
		while(uc --);     //��1΢��	
	}
}

/**
  * @brief  XPT2046 ��д������
  * @param  ucCmd ������
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0x90 :ͨ��Y+��ѡ�������
  *     @arg 0xd0 :ͨ��X+��ѡ�������
  * @retval ��
  */
static void XPT2046_WriteCMD(uint8_t ucCmd) 
{
	XPT2046_MOSI_0();
	XPT2046_CLK_LOW();

	for (uint8_t i = 0; i < 8; i++) 
	{
		((ucCmd >> ( 7 - i )) & 0x01 ) ? XPT2046_MOSI_1() : XPT2046_MOSI_0();
		
        XPT2046_DelayUS(5);
        XPT2046_CLK_HIGH();
        XPT2046_DelayUS(5);
        XPT2046_CLK_LOW();
	}
}


/**
  * @brief  XPT2046 �Ķ�ȡ����
  * @param  ��
  * @retval ��ȡ��������
  */
static uint16_t XPT2046_ReadCMD(void) 
{
	uint16_t usBuf=0, usTemp;
	XPT2046_MOSI_0();
	XPT2046_CLK_HIGH();

	for (uint8_t i = 0; i < 12; i++) 
	{
		XPT2046_CLK_LOW();    
		usTemp = XPT2046_MISO();
		usBuf |= usTemp << (11 - i);
		XPT2046_CLK_HIGH();
	}
	return usBuf;
}

/**
  * @brief  �� XPT2046 ѡ��һ��ģ��ͨ��������ADC��������ADC�������
  * @param  ucChannel
  *   �ò���Ϊ����ֵ֮һ��
  *     @arg 0x90 :ͨ��Y+��ѡ�������
  *     @arg 0xd0 :ͨ��X+��ѡ�������
  * @retval ��ͨ����ADC�������
  */
static uint16_t XPT2046_ReadAdc ( uint8_t ucChannel )
{
    XPT2046_WriteCMD(ucChannel);
    return XPT2046_ReadCMD();
}


/**
  * @brief  ��ȡ XPT2046 ��Xͨ����Yͨ����ADֵ��12 bit�������4096��
  * @param  sX_Ad �����Xͨ��ADֵ�ĵ�ַ
  * @param  sY_Ad �����Yͨ��ADֵ�ĵ�ַ
  * @retval ��
  */
static void XPT2046_ReadAdc_XY (int16_t * sX_Ad, int16_t * sY_Ad)  
{ 
	int16_t sX_Ad_Temp, sY_Ad_Temp; 
	
	sX_Ad_Temp = XPT2046_ReadAdc (XPT2046_CHANNEL_X);

	XPT2046_DelayUS (1); 

	sY_Ad_Temp = XPT2046_ReadAdc (XPT2046_CHANNEL_Y); 
	
	*sX_Ad = sX_Ad_Temp; 
	*sY_Ad = sY_Ad_Temp; 
}


/**
  * @brief  XPT2046 ��ʼ������
  * @param  ��
  * @retval ��
  */	
void XPT2046_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;


    /* ����GPIOʱ�� */
    RCC_APB2PeriphClockCmd (XPT2046_SPI_GPIO_CLK | XPT2046_PENIRQ_GPIO_CLK, ENABLE);

    /* ģ��SPI GPIO��ʼ�� */          
    GPIO_InitStructure.GPIO_Pin=XPT2046_SPI_CLK_PIN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN;
    GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
    GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
    GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure); 

    /* ����Ƭѡ��ѡ��XPT2046 */
    XPT2046_CS_DISABLE();		
                                
    //�����������ź�ָʾ���ţ���ʹ���ж�
    GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
    GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);	
}

/**
  * @brief  ���������״̬��
  * @retval ����״̬
	*   �÷���ֵΪ����ֵ֮һ��
  *     @arg TOUCH_PRESSED :��������
  *     @arg TOUCH_NOT_PRESSED :�޴���
  */
uint8_t XPT2046_TouchDetect(void)
{
    static touch_state_t touch_state = XPT2046_STATE_RELEASE;
    static uint16_t wait_time = 0;   // �����ȴ�ʱ��
    uint8_t result = TOUCH_RELEASE;
    
    switch(touch_state)
	{
        case XPT2046_STATE_RELEASE:
        {
            if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) //��һ�γ��ִ����ź�
            {
                touch_state = XPT2046_STATE_WAITING;
                result = TOUCH_RELEASE;
            }
            else
            {
                touch_state = XPT2046_STATE_RELEASE;
                result = TOUCH_RELEASE;
            }
        }
        break;
        
        case XPT2046_STATE_WAITING:
        if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) //��һ�γ��ִ����ź�
        {
            /* ���� */
            if(wait_time++ > DURIATION_TIME)
            {
                wait_time = 0;
                touch_state = XPT2046_STATE_PRESSED;
                result = TOUCH_PRESSED;
            }

        }
        else
        {
            wait_time = 0;
            touch_state = XPT2046_STATE_RELEASE;
            result = TOUCH_RELEASE;
        }
        break;
        
        case XPT2046_STATE_PRESSED:
        {
            if(XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) //��һ�γ��ִ����ź�
            {
                touch_state = XPT2046_STATE_PRESSED;
                result = TOUCH_PRESSED;
            }
            else
            {
                /* �����ͷ� */
                touch_state = XPT2046_STATE_RELEASE;
                result = TOUCH_RELEASE;
            }
        }
        break;
        
        default:
        {
            touch_state = XPT2046_STATE_RELEASE;
            result = TOUCH_RELEASE;            
        }
        break;
        
    }
    
    return result;
}

static uint8_t XPT2046_ReadAdc_Smooth_XY (int16_t *x, int16_t *y)
{
	uint8_t ucCount = 0, i;
	
	int16_t sAD_X, sAD_Y;
	int16_t sBufferArray[2][10] = {{0},{0}};  //����X��Y���ж�β���
	
	//�洢�����е���Сֵ�����ֵ
	int32_t lX_Min, lX_Max, lY_Min, lY_Max;


	/* ѭ������10�� */ 
	do					       				
	{		  
		XPT2046_ReadAdc_XY(&sAD_X, &sAD_Y);  
		
		sBufferArray[0][ucCount] = sAD_X;  
		sBufferArray[1][ucCount] = sAD_Y;
		
		ucCount++;  
		
	}	while ((XPT2046_PENIRQ_Read() == XPT2046_PENIRQ_ActiveLevel) && (ucCount < 10));//�û����������ʱ��TP_INT_IN�ź�Ϊ�� ���� ucCount<10
	
	/*����ɹ�����10������*/
	if ( ucCount ==10 )		 					
	{
		lX_Max = lX_Min = sBufferArray [0][0];
		lY_Max = lY_Min = sBufferArray [1][0];       
		
		for (i = 1; i < 10; i++)
		{
			if (sBufferArray[0][i] < lX_Min)
				lX_Min = sBufferArray [0][i];
			
			else if (sBufferArray [0][i] > lX_Max )
				lX_Max = sBufferArray[0][i];

		}
		
		for ( i = 1; i < 10; i ++ )
		{
			if (sBufferArray[1][i] < lY_Min)
            {
                lY_Min = sBufferArray [1][i];
            }
			else if (sBufferArray[1][i] > lY_Max)
            {
                 lY_Max = sBufferArray[1][i];
            }
		}
		
		/*ȥ����Сֵ�����ֵ֮����ƽ��ֵ*/
		*x = (sBufferArray[0][0] + sBufferArray[0][1] + sBufferArray[0][2] + sBufferArray[0][3] + sBufferArray[0][4] + 
		                           sBufferArray[0][5] + sBufferArray[0][6] + sBufferArray[0][7] + sBufferArray[0][8] + sBufferArray[0][9] - lX_Min-lX_Max) >> 3;
		
		*y = (sBufferArray[1][0] + sBufferArray[1][1] + sBufferArray[1][2] + sBufferArray[1][3] + sBufferArray[1][4] + 
		                           sBufferArray[1][5] + sBufferArray[1][6] + sBufferArray[1][7] + sBufferArray[1][8] + sBufferArray[1][9] - lY_Min-lY_Max) >> 3; 
        
		return 1;		
	}   	
	return 0;    	
}

/**
  * @brief  ��ȡ XPT2046 �����㣨У׼�󣩵�����
  * @param  pDisplayCoordinate ����ָ���Ż�ȡ���Ĵ���������
  * @param  pTouchPara������У׼ϵ��
  * @retval ��ȡ���
	*   �÷���ֵΪ����ֵ֮һ��
  *     @arg 1 :��ȡ�ɹ�
  *     @arg 0 :��ȡʧ��
  */
#define LCD_MODE    3
uint8_t XPT2046_Get_TouchedPoint(int16_t *x , int16_t *y)
{
    uint8_t ucRet = 1;           //���������򷵻�0
    int16_t X, Y;
    if (XPT2046_ReadAdc_Smooth_XY(&X, &Y))
    {    
        *x = ((strXPT2046_TouchPara[LCD_MODE].dX_X * X) + (strXPT2046_TouchPara[LCD_MODE].dX_Y * Y) + strXPT2046_TouchPara[LCD_MODE].dX);        
        *y = ((strXPT2046_TouchPara[LCD_MODE].dY_X * X) + (strXPT2046_TouchPara[LCD_MODE].dY_Y * Y) + strXPT2046_TouchPara[LCD_MODE].dY);

    }
     
    else ucRet = 0;            //�����ȡ�Ĵ�����Ϣ�����򷵻�0
    return ucRet;
} 


/**
	* @brief   ��⵽�����ж�ʱ���õĴ�����
	* @note    ��������Ҫ��whileѭ���ﱻ���ã�Ҳ��ʹ�ö�ʱ����ʱ����
	*            ���磬����ÿ��5ms����һ�Σ�������ֵ��DURIATION_TIME������Ϊ2������ÿ�������Լ��100���㡣
	* @param   none
	* @retval  none
	*/
void XPT2046_TouchEvenHandler(void)
{

    if(XPT2046_TouchDetect() == TOUCH_PRESSED)
    {
        //��ȡ��������
        // XPT2046_Get_TouchedPoint(&cinfo,strXPT2046_TouchPara);
        /*���´�����Ϣ��pre xy*/
        // cinfo.pre_x = cinfo.x; cinfo.pre_y = cinfo.y;  

    }
    else
    {
        /*�����ͷţ��� xy ����Ϊ��*/
//        cinfo.x = -1;
//        cinfo.y = -1; 
//        cinfo.pre_x = -1;
//        cinfo.pre_y = -1;
    }

}
