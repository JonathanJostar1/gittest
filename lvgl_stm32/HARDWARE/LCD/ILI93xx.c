#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
// ALIENTEK STM32F103������
// 2.4��/2.8��/3.5��/4.3��/7�� TFTҺ������
// ֧������IC�ͺŰ���:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
//                   SPFD5408/1505/B505/C505/NT35310/NT35510/SSD1963��
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// ��������:2010/7/4
// �汾��V3.0
// ��Ȩ���У�����ؾ���
// Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
// All rights reserved
//********************************************************************************
// V1.2�޸�˵��
// ֧����SPFD5408������,�����Һ��IDֱ�Ӵ�ӡ��HEX��ʽ.����鿴LCD����IC.
// V1.3
// �����˿���IO��֧��
// �޸��˱�����Ƶļ��ԣ�������V1.8���Ժ�Ŀ�����汾��
// ����1.8�汾֮ǰ(������1.8)��Һ��ģ��,���޸�LCD_Init������LCD_LED=1;ΪLCD_LED=1;
// V1.4
// �޸���LCD_ShowChar������ʹ�û��㹦�ܻ��ַ���
// �����˺�������ʾ��֧��
// V1.5 20110730
// 1,�޸���B505Һ������ɫ�����bug.
// 2,�޸��˿���IO�������������÷�ʽ.
// V1.6 20111116
// 1,�����LGDP4535Һ��������֧��
// V1.7 20120713
// 1,����LCD_RD_DATA����
// 2,���Ӷ�ILI9341��֧��
// 3,����ILI9325�Ķ�����������
// 4,����LCD_Scan_Dir����(����ʹ��)
// 6,�����޸��˲���ԭ���ĺ���,����Ӧ9341�Ĳ���
// V1.8 20120905
// 1,����LCD��Ҫ�������ýṹ��lcddev
// 2,����LCD_Display_Dir����,֧�����ߺ������л�
// V1.9 20120911
// 1,����RM68042������ID:6804��������6804��֧�ֺ�����ʾ����ԭ�򣺸ı�ɨ�跽ʽ��
// ����6804��������ʧЧ���Թ��ܶ෽�������У���ʱ�޽⡣
// V2.0 20120924
// �ڲ�Ӳ����λ�������,ILI9341��ID��ȡ�ᱻ�����9300,�޸�LCD_Init,���޷�ʶ��
// �����������IDΪ9300/�Ƿ�ID��,ǿ��ָ������ICΪILI9341��ִ��9341�ĳ�ʼ����
// V2.1 20120930
// ����ILI9325����ɫ��bug��
// V2.2 20121007
// ����LCD_Scan_Dir��bug��
// V2.3 20130120
// ����6804֧�ֺ�����ʾ
// V2.4 20131120
// 1,����NT35310��ID:5310����������֧��
// 2,����LCD_Set_Window����,�������ô���,�Կ������,�Ƚ�����,���Ǹú����ں���ʱ,��֧��6804.
// V2.5 20140211
// 1,����NT35510��ID:5510����������֧��
// V2.6 20140504
// 1,����ASCII 24*24�����֧��(���������û�������������)
// 2,�޸Ĳ��ֺ�������,��֧��MDK -O2�Ż�
// 3,���9341/35310/35510,дʱ������Ϊ���,�����ܵ�����ٶ�
// 4,ȥ����SSD1289��֧��,��Ϊ1289ʵ����̫����,������Ҫ1us...��ֱ����.���ʺ�F4ʹ��
// 5,����68042��C505��IC�Ķ���ɫ������bug.
// V2.7 20140710
// 1,����LCD_Color_Fill������һ��bug.
// 2,����LCD_Scan_Dir������һ��bug.
// V2.8 20140721
// 1,���MDKʹ��-O2�Ż�ʱLCD_ReadPoint��������ʧЧ������.
// 2,����LCD_Scan_Dir����ʱ���õ�ɨ�跽ʽ��ʾ��ȫ��bug.
// V2.9 20141130
// 1,������SSD1963 LCD��֧��.
// 2,����LCD_SSD_BackLightSet����
// 3,ȡ��ILI93XX��Rxx�Ĵ�������
// V3.0 20150423
// �޸�SSD1963 LCD������������.
//////////////////////////////////////////////////////////////////////////////////

// LCD�Ļ�����ɫ�ͱ���ɫ
uint16_t POINT_COLOR = 0x0000; // ������ɫ
uint16_t BACK_COLOR = 0xFFFF;  // ����ɫ

// ����LCD��Ҫ����
// Ĭ��Ϊ����
_lcd_dev lcddev;

// д�Ĵ�������
// regval:�Ĵ���ֵ
void LCD_WR_REG(uint16_t regval)
{
    * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = regval;
    // LCD->LCD_REG = regval; // д��Ҫд�ļĴ������
}
// дLCD����
// data:Ҫд���ֵ
void LCD_WR_DATA(uint16_t data)
{
    * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = data;
    // LCD->LCD_RAM = data;
}
// ��LCD����
// ����ֵ:������ֵ
uint16_t LCD_RD_DATA(void)
{
    return ( * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) );
    // __IO uint16_t ram; // ��ֹ���Ż�
    // ram = LCD->LCD_RAM;
    // return ram;
}
// д�Ĵ���
// LCD_Reg:�Ĵ�����ַ
// LCD_RegValue:Ҫд�������
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
    // LCD->LCD_REG = LCD_Reg;      // д��Ҫд�ļĴ������
    // LCD->LCD_RAM = LCD_RegValue; // д������
}
// ���Ĵ���
// LCD_Reg:�Ĵ�����ַ
// ����ֵ:����������
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg); // д��Ҫ���ļĴ������
    delay_us(5);
    return LCD_RD_DATA(); // ���ض�����ֵ
}
// ��ʼдGRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
    // LCD->LCD_REG = lcddev.wramcmd;
}
// LCDдGRAM
// RGB_Code:��ɫֵ
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD_WR_DATA(RGB_Code);
    // LCD->LCD_RAM = RGB_Code; // дʮ��λGRAM
}
// ��ILI93xx����������ΪGBR��ʽ��������д���ʱ��ΪRGB��ʽ��
// ͨ���ú���ת��
// c:GBR��ʽ����ɫֵ
// ����ֵ��RGB��ʽ����ɫֵ
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}
// ��mdk -O1ʱ���Ż�ʱ��Ҫ����
// ��ʱi
void opt_delay(uint8_t i)
{
    while (i--)
        ;
}
// ��ȡ��ĳ�����ɫֵ
// x,y:����
// ����ֵ:�˵����ɫ
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;
    if (x >= lcddev.width || y >= lcddev.height)
        return 0; // �����˷�Χ,ֱ�ӷ���
    LCD_SetCursor(x, y);
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X1963)
        LCD_WR_REG(0X2E); // 9341/6804/3510/1963 ���Ͷ�GRAMָ��
    else if (lcddev.id == 0X5510)
        LCD_WR_REG(0X2E00); // 5510 ���Ͷ�GRAMָ��
    else
        LCD_WR_REG(0X22); // ����IC���Ͷ�GRAMָ��
    if (lcddev.id == 0X9320)
        opt_delay(2);  // FOR 9320,��ʱ2us
    r = LCD_RD_DATA(); // dummy Read
    if (lcddev.id == 0X1963)
        return r; // 1963ֱ�Ӷ��Ϳ���
    opt_delay(2);
    r = LCD_RD_DATA();                                                     // ʵ��������ɫ
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510) // 9341/NT35310/NT35510Ҫ��2�ζ���
    {
        opt_delay(2);
        b = LCD_RD_DATA();
        g = r & 0XFF; // ����9341/5310/5510,��һ�ζ�ȡ����RG��ֵ,R��ǰ,G�ں�,��ռ8λ
        g <<= 8;
    }
    if (lcddev.id == 0X9325 || lcddev.id == 0X4535 || lcddev.id == 0X4531 || lcddev.id == 0XB505 || lcddev.id == 0XC505)
        return r; // �⼸��ICֱ�ӷ�����ɫֵ
    else if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510)
        return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); // ILI9341/NT35310/NT35510��Ҫ��ʽת��һ��
    else
        return LCD_BGR2RGB(r); // ����IC
}
// LCD������ʾ
void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X1963)
        LCD_WR_REG(0X29); // ������ʾ
    else if (lcddev.id == 0X5510)
        LCD_WR_REG(0X2900); // ������ʾ
    else
        LCD_WriteReg(0X07, 0x0173); // ������ʾ
}
// LCD�ر���ʾ
void LCD_DisplayOff(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X1963)
        LCD_WR_REG(0X28); // �ر���ʾ
    else if (lcddev.id == 0X5510)
        LCD_WR_REG(0X2800); // �ر���ʾ
    else
        LCD_WriteReg(0X07, 0x0); // �ر���ʾ
}
// ���ù��λ��
// Xpos:������
// Ypos:������
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else if (lcddev.id == 0X6804)
    {
        if (lcddev.dir == 1)
            Xpos = lcddev.width - 1 - Xpos; // ����ʱ����
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0) // x������Ҫ�任
        {
            Xpos = lcddev.width - 1 - Xpos;
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
        }
        else
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(Xpos >> 8);
            LCD_WR_DATA(Xpos & 0XFF);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
        }
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
        LCD_WR_DATA((lcddev.height - 1) >> 8);
        LCD_WR_DATA((lcddev.height - 1) & 0XFF);
    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else
    {
        if (lcddev.dir == 1)
            Xpos = lcddev.width - 1 - Xpos; // ������ʵ���ǵ�תx,y����
        LCD_WriteReg(lcddev.setxcmd, Xpos);
        LCD_WriteReg(lcddev.setycmd, Ypos);
    }
}
// ����LCD���Զ�ɨ�跽��
// ע��:�����������ܻ��ܵ��˺������õ�Ӱ��(������9341/6804����������),
// ����,һ������ΪL2R_U2D����,�������Ϊ����ɨ�跽ʽ,���ܵ�����ʾ������.
// dir:0~7,����8������(���嶨���lcd.h)
// 9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963��IC�Ѿ�ʵ�ʲ���
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;
    if ((lcddev.dir == 1 && lcddev.id != 0X6804 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963)) // ����ʱ����6804��1963���ı�ɨ�跽������ʱ1963�ı䷽��
    {
        switch (dir) // ����ת��
        {
        case 0:
            dir = 6;
            break;
        case 1:
            dir = 7;
            break;
        case 2:
            dir = 4;
            break;
        case 3:
            dir = 5;
            break;
        case 4:
            dir = 1;
            break;
        case 5:
            dir = 0;
            break;
        case 6:
            dir = 3;
            break;
        case 7:
            dir = 2;
            break;
        }
    }
    if (lcddev.id == 0x9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X5510 || lcddev.id == 0X1963) // 9341/6804/5310/5510/1963,���⴦��
    {
        switch (dir)
        {
        case L2R_U2D: // ������,���ϵ���
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U: // ������,���µ���
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D: // ���ҵ���,���ϵ���
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U: // ���ҵ���,���µ���
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R: // ���ϵ���,������
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L: // ���ϵ���,���ҵ���
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R: // ���µ���,������
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L: // ���µ���,���ҵ���
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
        }
        if (lcddev.id == 0X5510)
            dirreg = 0X3600;
        else
            dirreg = 0X36;
        if ((lcddev.id != 0X5310) && (lcddev.id != 0X5510) && (lcddev.id != 0X1963))
            regval |= 0X08; // 5310/5510/1963����ҪBGR
        if (lcddev.id == 0X6804)
            regval |= 0x02; // 6804��BIT6��9341�ķ���
        LCD_WriteReg(dirreg, regval);
        if (lcddev.id != 0X1963) // 1963�������괦��
        {
            if (regval & 0X20)
            {
                if (lcddev.width < lcddev.height) // ����X,Y
                {
                    temp = lcddev.width;
                    lcddev.width = lcddev.height;
                    lcddev.height = temp;
                }
            }
            else
            {
                if (lcddev.width > lcddev.height) // ����X,Y
                {
                    temp = lcddev.width;
                    lcddev.width = lcddev.height;
                    lcddev.height = temp;
                }
            }
        }
        if (lcddev.id == 0X5510)
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setxcmd + 1);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setxcmd + 2);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_REG(lcddev.setxcmd + 3);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
            LCD_WR_REG(lcddev.setycmd);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setycmd + 1);
            LCD_WR_DATA(0);
            LCD_WR_REG(lcddev.setycmd + 2);
            LCD_WR_DATA((lcddev.height - 1) >> 8);
            LCD_WR_REG(lcddev.setycmd + 3);
            LCD_WR_DATA((lcddev.height - 1) & 0XFF);
        }
        else
        {
            LCD_WR_REG(lcddev.setxcmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA((lcddev.width - 1) >> 8);
            LCD_WR_DATA((lcddev.width - 1) & 0XFF);
            LCD_WR_REG(lcddev.setycmd);
            LCD_WR_DATA(0);
            LCD_WR_DATA(0);
            LCD_WR_DATA((lcddev.height - 1) >> 8);
            LCD_WR_DATA((lcddev.height - 1) & 0XFF);
        }
    }
    else
    {
        switch (dir)
        {
        case L2R_U2D: // ������,���ϵ���
            regval |= (1 << 5) | (1 << 4) | (0 << 3);
            break;
        case L2R_D2U: // ������,���µ���
            regval |= (0 << 5) | (1 << 4) | (0 << 3);
            break;
        case R2L_U2D: // ���ҵ���,���ϵ���
            regval |= (1 << 5) | (0 << 4) | (0 << 3);
            break;
        case R2L_D2U: // ���ҵ���,���µ���
            regval |= (0 << 5) | (0 << 4) | (0 << 3);
            break;
        case U2D_L2R: // ���ϵ���,������
            regval |= (1 << 5) | (1 << 4) | (1 << 3);
            break;
        case U2D_R2L: // ���ϵ���,���ҵ���
            regval |= (1 << 5) | (0 << 4) | (1 << 3);
            break;
        case D2U_L2R: // ���µ���,������
            regval |= (0 << 5) | (1 << 4) | (1 << 3);
            break;
        case D2U_R2L: // ���µ���,���ҵ���
            regval |= (0 << 5) | (0 << 4) | (1 << 3);
            break;
        }
        dirreg = 0X03;
        regval |= 1 << 12;
        LCD_WriteReg(dirreg, regval);
    }
}
// ����
// x,y:����
// POINT_COLOR:�˵����ɫ
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_SetCursor(x, y);    // ���ù��λ��
    LCD_WriteRAM_Prepare(); // ��ʼд��GRAM
    LCD_WR_DATA(POINT_COLOR);
    // LCD->LCD_RAM = POINT_COLOR;
}
// ���ٻ���
// x,y:����
// color:��ɫ
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(y & 0XFF);
    }
    else if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0)
            x = lcddev.width - 1 - x;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else if (lcddev.id == 0X6804)
    {
        if (lcddev.dir == 1)
            x = lcddev.width - 1 - x; // ����ʱ����
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(x >> 8);
        LCD_WR_DATA(x & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(y >> 8);
        LCD_WR_DATA(y & 0XFF);
    }
    else
    {
        if (lcddev.dir == 1)
            x = lcddev.width - 1 - x; // ������ʵ���ǵ�תx,y����
        LCD_WriteReg(lcddev.setxcmd, x);
        LCD_WriteReg(lcddev.setycmd, y);
    }
    LCD_WR_REG(lcddev.wramcmd);
    // LCD->LCD_REG = lcddev.wramcmd;
    LCD_WR_DATA(color);
    // LCD->LCD_RAM = color;
}
// SSD1963 ��������
// pwm:����ȼ�,0~100.Խ��Խ��.
void LCD_SSD_BackLightSet(uint8_t pwm)
{
    LCD_WR_REG(0xBE);        // ����PWM���
    LCD_WR_DATA(0x05);       // 1����PWMƵ��
    LCD_WR_DATA(pwm * 2.55); // 2����PWMռ�ձ�
    LCD_WR_DATA(0x01);       // 3����C
    LCD_WR_DATA(0xFF);       // 4����D
    LCD_WR_DATA(0x00);       // 5����E
    LCD_WR_DATA(0x00);       // 6����F
}

// ����LCD��ʾ����
// dir:0,������1,����
void LCD_Display_Dir(uint8_t dir)
{
    if (dir == 0) // ����
    {
        lcddev.dir = 0; // ����
        lcddev.width = 240;
        lcddev.height = 320;
        if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310)
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
            if (lcddev.id == 0X6804 || lcddev.id == 0X5310)
            {
                lcddev.width = 320;
                lcddev.height = 480;
            }
        }
        else if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 480;
            lcddev.height = 800;
        }
        else if (lcddev.id == 0X1963)
        {
            lcddev.wramcmd = 0X2C; // ����д��GRAM��ָ��
            lcddev.setxcmd = 0X2B; // ����дX����ָ��
            lcddev.setycmd = 0X2A; // ����дY����ָ��
            lcddev.width = 480;    // ���ÿ���480
            lcddev.height = 800;   // ���ø߶�800
        }
        else
        {
            lcddev.wramcmd = 0X22;
            lcddev.setxcmd = 0X20;
            lcddev.setycmd = 0X21;
        }
    }
    else // ����
    {
        lcddev.dir = 1; // ����
        lcddev.width = 320;
        lcddev.height = 240;
        if (lcddev.id == 0X9341 || lcddev.id == 0X5310)
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2A;
            lcddev.setycmd = 0X2B;
        }
        else if (lcddev.id == 0X6804)
        {
            lcddev.wramcmd = 0X2C;
            lcddev.setxcmd = 0X2B;
            lcddev.setycmd = 0X2A;
        }
        else if (lcddev.id == 0x5510)
        {
            lcddev.wramcmd = 0X2C00;
            lcddev.setxcmd = 0X2A00;
            lcddev.setycmd = 0X2B00;
            lcddev.width = 800;
            lcddev.height = 480;
        }
        else if (lcddev.id == 0X1963)
        {
            lcddev.wramcmd = 0X2C; // ����д��GRAM��ָ��
            lcddev.setxcmd = 0X2A; // ����дX����ָ��
            lcddev.setycmd = 0X2B; // ����дY����ָ��
            lcddev.width = 800;    // ���ÿ���800
            lcddev.height = 480;   // ���ø߶�480
        }
        else
        {
            lcddev.wramcmd = 0X22;
            lcddev.setxcmd = 0X21;
            lcddev.setycmd = 0X20;
        }
        if (lcddev.id == 0X6804 || lcddev.id == 0X5310)
        {
            lcddev.width = 480;
            lcddev.height = 320;
        }
    }
    LCD_Scan_Dir(DFT_SCAN_DIR); // Ĭ��ɨ�跽��
}
// ���ô���,���Զ����û������굽�������Ͻ�(sx,sy).
// sx,sy:������ʼ����(���Ͻ�)
// width,height:���ڿ��Ⱥ͸߶�,�������0!!
// �����С:width*height.
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height)
{
    uint8_t hsareg, heareg, vsareg, veareg;
    uint16_t hsaval, heaval, vsaval, veaval;
    uint16_t twidth, theight;
    twidth = sx + width - 1;
    theight = sy + height - 1;
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X6804 || (lcddev.dir == 1 && lcddev.id == 0X1963))
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_DATA(theight & 0XFF);
    }
    else if (lcddev.id == 0X1963) // 1963�������⴦��
    {
        sx = lcddev.width - width - sx;
        height = sy + height - 1;
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_DATA((sx + width - 1) >> 8);
        LCD_WR_DATA((sx + width - 1) & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_DATA(height >> 8);
        LCD_WR_DATA(height & 0XFF);
    }
    else if (lcddev.id == 0X5510)
    {
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(sx >> 8);
        LCD_WR_REG(lcddev.setxcmd + 1);
        LCD_WR_DATA(sx & 0XFF);
        LCD_WR_REG(lcddev.setxcmd + 2);
        LCD_WR_DATA(twidth >> 8);
        LCD_WR_REG(lcddev.setxcmd + 3);
        LCD_WR_DATA(twidth & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(sy >> 8);
        LCD_WR_REG(lcddev.setycmd + 1);
        LCD_WR_DATA(sy & 0XFF);
        LCD_WR_REG(lcddev.setycmd + 2);
        LCD_WR_DATA(theight >> 8);
        LCD_WR_REG(lcddev.setycmd + 3);
        LCD_WR_DATA(theight & 0XFF);
    }
    else // ��������IC
    {
        if (lcddev.dir == 1) // ����
        {
            // ����ֵ
            hsaval = sy;
            heaval = theight;
            vsaval = lcddev.width - twidth - 1;
            veaval = lcddev.width - sx - 1;
        }
        else
        {
            hsaval = sx;
            heaval = twidth;
            vsaval = sy;
            veaval = theight;
        }
        hsareg = 0X50;
        heareg = 0X51; // ˮƽ���򴰿ڼĴ���
        vsareg = 0X52;
        veareg = 0X53; // ��ֱ���򴰿ڼĴ���
        // ���üĴ���ֵ
        LCD_WriteReg(hsareg, hsaval);
        LCD_WriteReg(heareg, heaval);
        LCD_WriteReg(vsareg, vsaval);
        LCD_WriteReg(veareg, veaval);
        LCD_SetCursor(sx, sy); // ���ù��λ��
    }
}

/**
  * @brief  ��ʼ��ILI9341��IO����
  * @param  ��
  * @retval ��
  */
static void ILI9341_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ʹ��FSMC��Ӧ��Ӧ�ܽ�ʱ��*/
	RCC_APB2PeriphClockCmd (/*�����ź�*/
                            ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
                            ILI9341_RD_CLK	|ILI9341_BK_CLK|ILI9341_RST_CLK|
                            /*�����ź�*/
                            ILI9341_D0_CLK|ILI9341_D1_CLK|	ILI9341_D2_CLK | 
                            ILI9341_D3_CLK | ILI9341_D4_CLK|ILI9341_D5_CLK|
                            ILI9341_D6_CLK | ILI9341_D7_CLK|ILI9341_D8_CLK|
                            ILI9341_D9_CLK | ILI9341_D10_CLK|ILI9341_D11_CLK|
                            ILI9341_D12_CLK | ILI9341_D13_CLK|ILI9341_D14_CLK|
                            ILI9341_D15_CLK	, ENABLE );	

		
	
	/* ����FSMC���Ӧ��������,FSMC-D0~D15 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D0_PIN;
	GPIO_Init ( ILI9341_D0_PORT, & GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = ILI9341_D1_PIN;
	GPIO_Init ( ILI9341_D1_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D2_PIN;
	GPIO_Init ( ILI9341_D2_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D3_PIN;
	GPIO_Init ( ILI9341_D3_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D4_PIN;
	GPIO_Init ( ILI9341_D4_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D5_PIN;
	GPIO_Init ( ILI9341_D5_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D6_PIN;
	GPIO_Init ( ILI9341_D6_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D7_PIN;
	GPIO_Init ( ILI9341_D7_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D8_PIN;
	GPIO_Init ( ILI9341_D8_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D9_PIN;
	GPIO_Init ( ILI9341_D9_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D10_PIN;
	GPIO_Init ( ILI9341_D10_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D11_PIN;
	GPIO_Init ( ILI9341_D11_PORT, & GPIO_InitStructure );

	GPIO_InitStructure.GPIO_Pin = ILI9341_D12_PIN;
	GPIO_Init ( ILI9341_D12_PORT, & GPIO_InitStructure );	
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D13_PIN;
	GPIO_Init ( ILI9341_D13_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D14_PIN;
	GPIO_Init ( ILI9341_D14_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_D15_PIN;
	GPIO_Init ( ILI9341_D15_PORT, & GPIO_InitStructure );
	

	
	/* ����FSMC���Ӧ�Ŀ�����
	 * FSMC_NOE   :LCD-RD
	 * FSMC_NWE   :LCD-WR
	 * FSMC_NE1   :LCD-CS
	 * FSMC_A16  	:LCD-DC
	 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RD_PIN; 
	GPIO_Init (ILI9341_RD_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_WR_PIN; 
	GPIO_Init (ILI9341_WR_PORT, & GPIO_InitStructure );
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_CS_PIN; 
	GPIO_Init ( ILI9341_CS_PORT, & GPIO_InitStructure );  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_DC_PIN; 
	GPIO_Init ( ILI9341_DC_PORT, & GPIO_InitStructure );
	

  /* ����LCD��λRST���ƹܽ�*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN; 
	GPIO_Init ( ILI9341_RST_PORT, & GPIO_InitStructure );
	
	
	/* ����LCD������ƹܽ�BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN; 
	GPIO_Init ( ILI9341_BK_PORT, & GPIO_InitStructure );
}


/**
  * @brief  LCD  FSMC ģʽ����
  * @param  ��
  * @retval ��
  */
static void ILI9341_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure={0};
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming={0}; 	
	
	/* ʹ��FSMCʱ��*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

	//��ַ����ʱ�䣨ADDSET��Ϊ1��HCLK 2/72M=28ns
	readWriteTiming.FSMC_AddressSetupTime      = 0x01;	 //��ַ����ʱ��
	//���ݱ���ʱ�䣨DATAST��+ 1��HCLK = 5/72M=70ns	
	readWriteTiming.FSMC_DataSetupTime         = 0x04;	 //���ݽ���ʱ��
	//ѡ����Ƶ�ģʽ
	//ģʽB,�첽NOR FLASHģʽ����ILI9341��8080ʱ��ƥ��
	readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B;	
	
	/*����������ģʽB�޹�*/
	//��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
	readWriteTiming.FSMC_AddressHoldTime       = 0x00;	 //��ַ����ʱ��
	//��������ת�����ڣ������ڸ���ģʽ��NOR����
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	//����ʱ�ӷ�Ƶ��������ͬ�����͵Ĵ洢��
	readWriteTiming.FSMC_CLKDivision           = 0x00;
	//���ݱ���ʱ�䣬������ͬ���͵�NOR	
	readWriteTiming.FSMC_DataLatency           = 0x00;	

	
	FSMC_NORSRAMInitStructure.FSMC_Bank                  = FSMC_Bank1_NORSRAMx;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux        = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType            = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth       = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode       = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity    = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode              = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive      = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation        = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal            = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode          = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst            = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct     = &readWriteTiming;  
	
	FSMC_NORSRAMInit ( & FSMC_NORSRAMInitStructure ); 
	
	
	/* ʹ�� FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );  
		
		
}


/**
  * @brief  ���� ILI9341 ����ʱ����
  * @param  nCount ����ʱ����ֵ
  * @retval ��
  */	
static void ILI9341_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

/**
 * @brief  ILI9341 ������λ
 * @param  ��
 * @retval ��
 */
void ILI9341_Rst ( void )
{			
	GPIO_ResetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );	 //�͵�ƽ��λ

	ILI9341_Delay ( 0xAFF ); 					   

	GPIO_SetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );		 	 

	ILI9341_Delay ( 0xAFF ); 	
	
}

// ��ʼ��lcd
// �ó�ʼ���������Գ�ʼ������ILI93XXҺ��,�������������ǻ���ILI9320��!!!
// �������ͺŵ�����оƬ��û�в���!
void LCD_Init(void)
{
    ILI9341_GPIO_Config();
    ILI9341_FSMC_Config();
    
    GPIO_ResetBits (ILI9341_BK_PORT, ILI9341_BK_PIN);	
    
    ILI9341_Rst();
    LCD_WR_REG(0xD3);
    lcddev.id = LCD_RD_DATA(); // dummy read
    lcddev.id = LCD_RD_DATA(); // ����0X00
    lcddev.id = LCD_RD_DATA(); // ��ȡ93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA(); // ��ȡ41
    
    printf(" LCD ID:%x\r\n", lcddev.id); // ��ӡLCD ID
    if (lcddev.id == 0X9341)             // 9341��ʼ��
    {
        /*  Power control B (CFh)  */
        LCD_WR_REG(0xCF);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x81);
        LCD_WR_DATA(0X30);
        delay_ms(5);
        
        /*  Power on sequence control (EDh) */
        LCD_WR_REG(0xED);
        LCD_WR_DATA(0x64);
        LCD_WR_DATA(0x03);
        LCD_WR_DATA(0X12);
        LCD_WR_DATA(0X81);
        delay_ms(5);
        
        /*  Driver timing control A (E8h) */
        LCD_WR_REG(0xE8);
        LCD_WR_DATA(0x85);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x78);
        delay_ms(5);
        
        /*  Power control A (CBh) */
        LCD_WR_REG(0xCB);
        LCD_WR_DATA(0x39);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x34);
        LCD_WR_DATA(0x02);
        delay_ms(5);
        
        /* Pump ratio control (F7h) */
        LCD_WR_REG(0xF7);
        LCD_WR_DATA(0x20);  
        delay_ms(5);  
        
        /* Driver timing control B */
        LCD_WR_REG(0xEA);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        delay_ms(5);
        
        /* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
        LCD_WR_REG ( 0xB1 );
        LCD_WR_DATA ( 0x00 );
        LCD_WR_DATA ( 0x1B );
        delay_ms(5);
        
        /*  Display Function Control (B6h) */
        LCD_WR_REG ( 0xB6 );
        LCD_WR_DATA ( 0x0A );
        LCD_WR_DATA ( 0xA2 );
        delay_ms(5);
        
        /* Power Control 1 (C0h) */
        LCD_WR_REG(0xC0);  // Power control
        LCD_WR_DATA(0x35); // VRH[5:0]
        delay_ms(5);
        
        /* Power Control 2 (C1h) */
        LCD_WR_REG(0xC1);  // Power control
        LCD_WR_DATA(0x11); // SAP[2:0];BT[3:0]
        delay_ms(5);
        
        /* VCOM Control 1 (C5h) */
        LCD_WR_REG(0xC5);  // VCM control
        LCD_WR_DATA(0x45); // 3F
        LCD_WR_DATA(0x45); // 3C
        delay_ms(5);
        
        /*  VCOM Control 2 (C7h)  */
        LCD_WR_REG(0xC7);  // VCM control2
        LCD_WR_DATA(0xA2);
        delay_ms(5);
        
        /* Enable 3G (F2h) */
        LCD_WR_REG ( 0xF2 );
        LCD_WR_DATA ( 0x00 );
        delay_ms(5);
        
        /* Gamma Set (26h) */
        LCD_WR_REG ( 0x26 );
        LCD_WR_DATA ( 0x01 );
        delay_ms(5);
        
        /* Positive Gamma Correction */
        LCD_WR_REG(0xE0); // Set Gamma
        LCD_WR_DATA(0x0F);
        LCD_WR_DATA ( 0x26 );
        LCD_WR_DATA ( 0x24 );
        LCD_WR_DATA ( 0x0B );
        LCD_WR_DATA ( 0x0E );
        LCD_WR_DATA ( 0x09 );
        LCD_WR_DATA ( 0x54 );
        LCD_WR_DATA ( 0xA8 );
        LCD_WR_DATA ( 0x46 );
        LCD_WR_DATA ( 0x0C );
        LCD_WR_DATA ( 0x17 );
        LCD_WR_DATA ( 0x09 );
        LCD_WR_DATA ( 0x0F );
        LCD_WR_DATA ( 0x07 );
        LCD_WR_DATA ( 0x00 );
        delay_ms(5);
    
        /* Negative Gamma Correction (E1h) */
        LCD_WR_REG(0XE1); // Set Gamma
        LCD_WR_DATA ( 0x00 );
        LCD_WR_DATA ( 0x19 );
        LCD_WR_DATA ( 0x1B );
        LCD_WR_DATA ( 0x04 );
        LCD_WR_DATA ( 0x10 );
        LCD_WR_DATA ( 0x07 );
        LCD_WR_DATA ( 0x2A );
        LCD_WR_DATA ( 0x47 );
        LCD_WR_DATA ( 0x39 );
        LCD_WR_DATA ( 0x03 );
        LCD_WR_DATA ( 0x06 );
        LCD_WR_DATA ( 0x06 );
        LCD_WR_DATA ( 0x30 );
        LCD_WR_DATA ( 0x38 );
        LCD_WR_DATA ( 0x0F );
        delay_ms(5);
        
        /* memory access control set */
        LCD_WR_REG(0x36); // Memory Access Control
        LCD_WR_DATA(0xC8);
        delay_ms(5);
        
         /* column address control set */
        LCD_WR_REG(0x2A);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0xEF);
        delay_ms(5);
        
        /* page address control set */
        LCD_WR_REG(0x2B);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x01);
        LCD_WR_DATA(0x3f);
        delay_ms(5);
        
        /*  Pixel Format Set (3Ah)  */
        LCD_WR_REG(0x3A);
        LCD_WR_DATA(0x55);
        
        /* Sleep Out (11h)  */
        LCD_WR_REG(0x11); // Exit Sleep
        delay_ms(120);
        
        LCD_WR_REG(0x29); // display on
    }
    LCD_Display_Dir(1); // Ĭ��Ϊ����	
    LCD_Clear(WHITE);
}
// ��������
// color:Ҫ���������ɫ
void LCD_Clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height;                    // �õ��ܵ���
    if ((lcddev.id == 0X6804) && (lcddev.dir == 1)) // 6804������ʱ�����⴦��
    {
        lcddev.dir = 0;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
        LCD_SetCursor(0x00, 0x0000); // ���ù��λ��
        lcddev.dir = 1;
        lcddev.setxcmd = 0X2B;
        lcddev.setycmd = 0X2A;
    }
    else
        LCD_SetCursor(0x00, 0x0000); // ���ù��λ��
    LCD_WriteRAM_Prepare();          // ��ʼд��GRAM
    for (index = 0; index < totalpoint; index++)
    {
        LCD_WR_DATA(color);
        // LCD->LCD_RAM = color;
    }
}
// ��ָ����������䵥����ɫ
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
// color:Ҫ������ɫ
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    uint16_t temp;
    if ((lcddev.id == 0X6804) && (lcddev.dir == 1)) // 6804������ʱ�����⴦��
    {
        temp = sx;
        sx = sy;
        sy = lcddev.width - ex - 1;
        ex = ey;
        ey = lcddev.width - temp - 1;
        lcddev.dir = 0;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
        LCD_Fill(sx, sy, ex, ey, color);
        lcddev.dir = 1;
        lcddev.setxcmd = 0X2B;
        lcddev.setycmd = 0X2A;
    }
    else
    {
        xlen = ex - sx + 1;
        for (i = sy; i <= ey; i++)
        {
            LCD_SetCursor(sx, i);   // ���ù��λ��
            LCD_WriteRAM_Prepare(); // ��ʼд��GRAM
            for (j = 0; j < xlen; j++)
                LCD_WR_DATA(color);
                // LCD->LCD_RAM = color; // ��ʾ��ɫ
        }
    }
}
// ��ָ�����������ָ����ɫ��
//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)
// color:Ҫ������ɫ
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;
    width = ex - sx + 1;  // �õ����Ŀ���
    height = ey - sy + 1; // �߶�
    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i); // ���ù��λ��
        LCD_WriteRAM_Prepare();    // ��ʼд��GRAM
        for (j = 0; j < width; j++)
            LCD_WR_DATA(color[i * width + j]);
            // LCD->LCD_RAM = color[i * width + j]; // д������
    }
}
// ����
// x1,y1:�������
// x2,y2:�յ�����
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // ������������
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1; // ���õ�������
    else if (delta_x == 0)
        incx = 0; // ��ֱ��
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; // ˮƽ��
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; // ѡȡ��������������
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++) // �������
    {
        LCD_DrawPoint(uRow, uCol); // ����
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}
// ������
//(x1,y1),(x2,y2):���εĶԽ�����
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}
// ��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
// r    :�뾶
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1); // �ж��¸���λ�õı�־
    while (a <= b)
    {
        LCD_DrawPoint(x0 + a, y0 - b); // 5
        LCD_DrawPoint(x0 + b, y0 - a); // 0
        LCD_DrawPoint(x0 + b, y0 + a); // 4
        LCD_DrawPoint(x0 + a, y0 + b); // 6
        LCD_DrawPoint(x0 - a, y0 + b); // 1
        LCD_DrawPoint(x0 - b, y0 + a);
        LCD_DrawPoint(x0 - a, y0 - b); // 2
        LCD_DrawPoint(x0 - b, y0 - a); // 7
        a++;
        // ʹ��Bresenham�㷨��Բ
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
// ��ָ��λ����ʾһ���ַ�
// x,y:��ʼ����
// num:Ҫ��ʾ���ַ�:" "--->"~"
// size:�����С 12/16/24
// mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // �õ�����һ���ַ���Ӧ������ռ���ֽ���
    num = num - ' ';                                           // �õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
            temp = asc2_1206[num][t]; // ����1206����
        else if (size == 16)
            temp = asc2_1608[num][t]; // ����1608����
        else if (size == 24)
            temp = asc2_2412[num][t]; // ����2412����
        else
            return; // û�е��ֿ�
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if (y >= lcddev.height)
                return; // ��������
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= lcddev.width)
                    return; // ��������
                break;
            }
        }
    }
}
// m^n����
// ����ֵ:m^n�η�.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}
// ��ʾ����,��λΪ0,����ʾ
// x,y :�������
// len :���ֵ�λ��
// size:�����С
// color:��ɫ
// num:��ֵ(0~4294967295);
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + (size / 2) * t, y, ' ', size, 0);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, 0);
    }
}
// ��ʾ����,��λΪ0,������ʾ
// x,y:�������
// num:��ֵ(0~999999999);
// len:����(��Ҫ��ʾ��λ��)
// size:�����С
// mode:
//[7]:0,�����;1,���0.
//[6:1]:����
//[0]:0,�ǵ�����ʾ;1,������ʾ.
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp;
    uint8_t enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                if (mode & 0X80)
                    LCD_ShowChar(x + (size / 2) * t, y, '0', size, mode & 0X01);
                else
                    LCD_ShowChar(x + (size / 2) * t, y, ' ', size, mode & 0X01);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + (size / 2) * t, y, temp + '0', size, mode & 0X01);
    }
}
// ��ʾ�ַ���
// x,y:�������
// width,height:�����С
// size:�����С
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' ')) // �ж��ǲ��ǷǷ��ַ�!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)
            break; // �˳�
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}