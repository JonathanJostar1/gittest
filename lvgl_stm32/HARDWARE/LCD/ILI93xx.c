#include "lcd.h"
#include "stdlib.h"
#include "font.h"
#include "usart.h"
#include "delay.h"
#include "stm32f10x.h"
//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F103开发板
// 2.4寸/2.8寸/3.5寸/4.3寸/7寸 TFT液晶驱动
// 支持驱动IC型号包括:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
//                   SPFD5408/1505/B505/C505/NT35310/NT35510/SSD1963等
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 创建日期:2010/7/4
// 版本：V3.0
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//********************************************************************************
// V1.2修改说明
// 支持了SPFD5408的驱动,另外把液晶ID直接打印成HEX格式.方便查看LCD驱动IC.
// V1.3
// 加入了快速IO的支持
// 修改了背光控制的极性（适用于V1.8及以后的开发板版本）
// 对于1.8版本之前(不包括1.8)的液晶模块,请修改LCD_Init函数的LCD_LED=1;为LCD_LED=1;
// V1.4
// 修改了LCD_ShowChar函数，使用画点功能画字符。
// 加入了横竖屏显示的支持
// V1.5 20110730
// 1,修改了B505液晶读颜色有误的bug.
// 2,修改了快速IO及横竖屏的设置方式.
// V1.6 20111116
// 1,加入对LGDP4535液晶的驱动支持
// V1.7 20120713
// 1,增加LCD_RD_DATA函数
// 2,增加对ILI9341的支持
// 3,增加ILI9325的独立驱动代码
// 4,增加LCD_Scan_Dir函数(慎重使用)
// 6,另外修改了部分原来的函数,以适应9341的操作
// V1.8 20120905
// 1,加入LCD重要参数设置结构体lcddev
// 2,加入LCD_Display_Dir函数,支持在线横竖屏切换
// V1.9 20120911
// 1,新增RM68042驱动（ID:6804），但是6804不支持横屏显示！！原因：改变扫描方式，
// 导致6804坐标设置失效，试过很多方法都不行，暂时无解。
// V2.0 20120924
// 在不硬件复位的情况下,ILI9341的ID读取会被误读成9300,修改LCD_Init,将无法识别
// 的情况（读到ID为9300/非法ID）,强制指定驱动IC为ILI9341，执行9341的初始化。
// V2.1 20120930
// 修正ILI9325读颜色的bug。
// V2.2 20121007
// 修正LCD_Scan_Dir的bug。
// V2.3 20130120
// 新增6804支持横屏显示
// V2.4 20131120
// 1,新增NT35310（ID:5310）驱动器的支持
// 2,新增LCD_Set_Window函数,用于设置窗口,对快速填充,比较有用,但是该函数在横屏时,不支持6804.
// V2.5 20140211
// 1,新增NT35510（ID:5510）驱动器的支持
// V2.6 20140504
// 1,新增ASCII 24*24字体的支持(更多字体用户可以自行添加)
// 2,修改部分函数参数,以支持MDK -O2优化
// 3,针对9341/35310/35510,写时间设置为最快,尽可能的提高速度
// 4,去掉了SSD1289的支持,因为1289实在是太慢了,读周期要1us...简直奇葩.不适合F4使用
// 5,修正68042及C505等IC的读颜色函数的bug.
// V2.7 20140710
// 1,修正LCD_Color_Fill函数的一个bug.
// 2,修正LCD_Scan_Dir函数的一个bug.
// V2.8 20140721
// 1,解决MDK使用-O2优化时LCD_ReadPoint函数读点失效的问题.
// 2,修正LCD_Scan_Dir横屏时设置的扫描方式显示不全的bug.
// V2.9 20141130
// 1,新增对SSD1963 LCD的支持.
// 2,新增LCD_SSD_BackLightSet函数
// 3,取消ILI93XX的Rxx寄存器定义
// V3.0 20150423
// 修改SSD1963 LCD屏的驱动参数.
//////////////////////////////////////////////////////////////////////////////////

// LCD的画笔颜色和背景色
uint16_t POINT_COLOR = 0x0000; // 画笔颜色
uint16_t BACK_COLOR = 0xFFFF;  // 背景色

// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;

// 写寄存器函数
// regval:寄存器值
void LCD_WR_REG(uint16_t regval)
{
    * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_CMD ) = regval;
    // LCD->LCD_REG = regval; // 写入要写的寄存器序号
}
// 写LCD数据
// data:要写入的值
void LCD_WR_DATA(uint16_t data)
{
    * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) = data;
    // LCD->LCD_RAM = data;
}
// 读LCD数据
// 返回值:读到的值
uint16_t LCD_RD_DATA(void)
{
    return ( * ( __IO uint16_t * ) ( FSMC_Addr_ILI9341_DATA ) );
    // __IO uint16_t ram; // 防止被优化
    // ram = LCD->LCD_RAM;
    // return ram;
}
// 写寄存器
// LCD_Reg:寄存器地址
// LCD_RegValue:要写入的数据
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
    // LCD->LCD_REG = LCD_Reg;      // 写入要写的寄存器序号
    // LCD->LCD_RAM = LCD_RegValue; // 写入数据
}
// 读寄存器
// LCD_Reg:寄存器地址
// 返回值:读到的数据
uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    LCD_WR_REG(LCD_Reg); // 写入要读的寄存器序号
    delay_us(5);
    return LCD_RD_DATA(); // 返回读到的值
}
// 开始写GRAM
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
    // LCD->LCD_REG = lcddev.wramcmd;
}
// LCD写GRAM
// RGB_Code:颜色值
void LCD_WriteRAM(uint16_t RGB_Code)
{
    LCD_WR_DATA(RGB_Code);
    // LCD->LCD_RAM = RGB_Code; // 写十六位GRAM
}
// 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
// 通过该函数转换
// c:GBR格式的颜色值
// 返回值：RGB格式的颜色值
uint16_t LCD_BGR2RGB(uint16_t c)
{
    uint16_t r, g, b, rgb;
    b = (c >> 0) & 0x1f;
    g = (c >> 5) & 0x3f;
    r = (c >> 11) & 0x1f;
    rgb = (b << 11) + (g << 5) + (r << 0);
    return (rgb);
}
// 当mdk -O1时间优化时需要设置
// 延时i
void opt_delay(uint8_t i)
{
    while (i--)
        ;
}
// 读取个某点的颜色值
// x,y:坐标
// 返回值:此点的颜色
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y)
{
    uint16_t r = 0, g = 0, b = 0;
    if (x >= lcddev.width || y >= lcddev.height)
        return 0; // 超过了范围,直接返回
    LCD_SetCursor(x, y);
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X1963)
        LCD_WR_REG(0X2E); // 9341/6804/3510/1963 发送读GRAM指令
    else if (lcddev.id == 0X5510)
        LCD_WR_REG(0X2E00); // 5510 发送读GRAM指令
    else
        LCD_WR_REG(0X22); // 其他IC发送读GRAM指令
    if (lcddev.id == 0X9320)
        opt_delay(2);  // FOR 9320,延时2us
    r = LCD_RD_DATA(); // dummy Read
    if (lcddev.id == 0X1963)
        return r; // 1963直接读就可以
    opt_delay(2);
    r = LCD_RD_DATA();                                                     // 实际坐标颜色
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510) // 9341/NT35310/NT35510要分2次读出
    {
        opt_delay(2);
        b = LCD_RD_DATA();
        g = r & 0XFF; // 对于9341/5310/5510,第一次读取的是RG的值,R在前,G在后,各占8位
        g <<= 8;
    }
    if (lcddev.id == 0X9325 || lcddev.id == 0X4535 || lcddev.id == 0X4531 || lcddev.id == 0XB505 || lcddev.id == 0XC505)
        return r; // 这几种IC直接返回颜色值
    else if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510)
        return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); // ILI9341/NT35310/NT35510需要公式转换一下
    else
        return LCD_BGR2RGB(r); // 其他IC
}
// LCD开启显示
void LCD_DisplayOn(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X1963)
        LCD_WR_REG(0X29); // 开启显示
    else if (lcddev.id == 0X5510)
        LCD_WR_REG(0X2900); // 开启显示
    else
        LCD_WriteReg(0X07, 0x0173); // 开启显示
}
// LCD关闭显示
void LCD_DisplayOff(void)
{
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X1963)
        LCD_WR_REG(0X28); // 关闭显示
    else if (lcddev.id == 0X5510)
        LCD_WR_REG(0X2800); // 关闭显示
    else
        LCD_WriteReg(0X07, 0x0); // 关闭显示
}
// 设置光标位置
// Xpos:横坐标
// Ypos:纵坐标
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
            Xpos = lcddev.width - 1 - Xpos; // 横屏时处理
        LCD_WR_REG(lcddev.setxcmd);
        LCD_WR_DATA(Xpos >> 8);
        LCD_WR_DATA(Xpos & 0XFF);
        LCD_WR_REG(lcddev.setycmd);
        LCD_WR_DATA(Ypos >> 8);
        LCD_WR_DATA(Ypos & 0XFF);
    }
    else if (lcddev.id == 0X1963)
    {
        if (lcddev.dir == 0) // x坐标需要变换
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
            Xpos = lcddev.width - 1 - Xpos; // 横屏其实就是调转x,y坐标
        LCD_WriteReg(lcddev.setxcmd, Xpos);
        LCD_WriteReg(lcddev.setycmd, Ypos);
    }
}
// 设置LCD的自动扫描方向
// 注意:其他函数可能会受到此函数设置的影响(尤其是9341/6804这两个奇葩),
// 所以,一般设置为L2R_U2D即可,如果设置为其他扫描方式,可能导致显示不正常.
// dir:0~7,代表8个方向(具体定义见lcd.h)
// 9320/9325/9328/4531/4535/1505/b505/5408/9341/5310/5510/1963等IC已经实际测试
void LCD_Scan_Dir(uint8_t dir)
{
    uint16_t regval = 0;
    uint16_t dirreg = 0;
    uint16_t temp;
    if ((lcddev.dir == 1 && lcddev.id != 0X6804 && lcddev.id != 0X1963) || (lcddev.dir == 0 && lcddev.id == 0X1963)) // 横屏时，对6804和1963不改变扫描方向！竖屏时1963改变方向
    {
        switch (dir) // 方向转换
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
    if (lcddev.id == 0x9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310 || lcddev.id == 0X5510 || lcddev.id == 0X1963) // 9341/6804/5310/5510/1963,特殊处理
    {
        switch (dir)
        {
        case L2R_U2D: // 从左到右,从上到下
            regval |= (0 << 7) | (0 << 6) | (0 << 5);
            break;
        case L2R_D2U: // 从左到右,从下到上
            regval |= (1 << 7) | (0 << 6) | (0 << 5);
            break;
        case R2L_U2D: // 从右到左,从上到下
            regval |= (0 << 7) | (1 << 6) | (0 << 5);
            break;
        case R2L_D2U: // 从右到左,从下到上
            regval |= (1 << 7) | (1 << 6) | (0 << 5);
            break;
        case U2D_L2R: // 从上到下,从左到右
            regval |= (0 << 7) | (0 << 6) | (1 << 5);
            break;
        case U2D_R2L: // 从上到下,从右到左
            regval |= (0 << 7) | (1 << 6) | (1 << 5);
            break;
        case D2U_L2R: // 从下到上,从左到右
            regval |= (1 << 7) | (0 << 6) | (1 << 5);
            break;
        case D2U_R2L: // 从下到上,从右到左
            regval |= (1 << 7) | (1 << 6) | (1 << 5);
            break;
        }
        if (lcddev.id == 0X5510)
            dirreg = 0X3600;
        else
            dirreg = 0X36;
        if ((lcddev.id != 0X5310) && (lcddev.id != 0X5510) && (lcddev.id != 0X1963))
            regval |= 0X08; // 5310/5510/1963不需要BGR
        if (lcddev.id == 0X6804)
            regval |= 0x02; // 6804的BIT6和9341的反了
        LCD_WriteReg(dirreg, regval);
        if (lcddev.id != 0X1963) // 1963不做坐标处理
        {
            if (regval & 0X20)
            {
                if (lcddev.width < lcddev.height) // 交换X,Y
                {
                    temp = lcddev.width;
                    lcddev.width = lcddev.height;
                    lcddev.height = temp;
                }
            }
            else
            {
                if (lcddev.width > lcddev.height) // 交换X,Y
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
        case L2R_U2D: // 从左到右,从上到下
            regval |= (1 << 5) | (1 << 4) | (0 << 3);
            break;
        case L2R_D2U: // 从左到右,从下到上
            regval |= (0 << 5) | (1 << 4) | (0 << 3);
            break;
        case R2L_U2D: // 从右到左,从上到下
            regval |= (1 << 5) | (0 << 4) | (0 << 3);
            break;
        case R2L_D2U: // 从右到左,从下到上
            regval |= (0 << 5) | (0 << 4) | (0 << 3);
            break;
        case U2D_L2R: // 从上到下,从左到右
            regval |= (1 << 5) | (1 << 4) | (1 << 3);
            break;
        case U2D_R2L: // 从上到下,从右到左
            regval |= (1 << 5) | (0 << 4) | (1 << 3);
            break;
        case D2U_L2R: // 从下到上,从左到右
            regval |= (0 << 5) | (1 << 4) | (1 << 3);
            break;
        case D2U_R2L: // 从下到上,从右到左
            regval |= (0 << 5) | (0 << 4) | (1 << 3);
            break;
        }
        dirreg = 0X03;
        regval |= 1 << 12;
        LCD_WriteReg(dirreg, regval);
    }
}
// 画点
// x,y:坐标
// POINT_COLOR:此点的颜色
void LCD_DrawPoint(uint16_t x, uint16_t y)
{
    LCD_SetCursor(x, y);    // 设置光标位置
    LCD_WriteRAM_Prepare(); // 开始写入GRAM
    LCD_WR_DATA(POINT_COLOR);
    // LCD->LCD_RAM = POINT_COLOR;
}
// 快速画点
// x,y:坐标
// color:颜色
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
            x = lcddev.width - 1 - x; // 横屏时处理
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
            x = lcddev.width - 1 - x; // 横屏其实就是调转x,y坐标
        LCD_WriteReg(lcddev.setxcmd, x);
        LCD_WriteReg(lcddev.setycmd, y);
    }
    LCD_WR_REG(lcddev.wramcmd);
    // LCD->LCD_REG = lcddev.wramcmd;
    LCD_WR_DATA(color);
    // LCD->LCD_RAM = color;
}
// SSD1963 背光设置
// pwm:背光等级,0~100.越大越亮.
void LCD_SSD_BackLightSet(uint8_t pwm)
{
    LCD_WR_REG(0xBE);        // 配置PWM输出
    LCD_WR_DATA(0x05);       // 1设置PWM频率
    LCD_WR_DATA(pwm * 2.55); // 2设置PWM占空比
    LCD_WR_DATA(0x01);       // 3设置C
    LCD_WR_DATA(0xFF);       // 4设置D
    LCD_WR_DATA(0x00);       // 5设置E
    LCD_WR_DATA(0x00);       // 6设置F
}

// 设置LCD显示方向
// dir:0,竖屏；1,横屏
void LCD_Display_Dir(uint8_t dir)
{
    if (dir == 0) // 竖屏
    {
        lcddev.dir = 0; // 竖屏
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
            lcddev.wramcmd = 0X2C; // 设置写入GRAM的指令
            lcddev.setxcmd = 0X2B; // 设置写X坐标指令
            lcddev.setycmd = 0X2A; // 设置写Y坐标指令
            lcddev.width = 480;    // 设置宽度480
            lcddev.height = 800;   // 设置高度800
        }
        else
        {
            lcddev.wramcmd = 0X22;
            lcddev.setxcmd = 0X20;
            lcddev.setycmd = 0X21;
        }
    }
    else // 横屏
    {
        lcddev.dir = 1; // 横屏
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
            lcddev.wramcmd = 0X2C; // 设置写入GRAM的指令
            lcddev.setxcmd = 0X2A; // 设置写X坐标指令
            lcddev.setycmd = 0X2B; // 设置写Y坐标指令
            lcddev.width = 800;    // 设置宽度800
            lcddev.height = 480;   // 设置高度480
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
    LCD_Scan_Dir(DFT_SCAN_DIR); // 默认扫描方向
}
// 设置窗口,并自动设置画点坐标到窗口左上角(sx,sy).
// sx,sy:窗口起始坐标(左上角)
// width,height:窗口宽度和高度,必须大于0!!
// 窗体大小:width*height.
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
    else if (lcddev.id == 0X1963) // 1963竖屏特殊处理
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
    else // 其他驱动IC
    {
        if (lcddev.dir == 1) // 横屏
        {
            // 窗口值
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
        heareg = 0X51; // 水平方向窗口寄存器
        vsareg = 0X52;
        veareg = 0X53; // 垂直方向窗口寄存器
        // 设置寄存器值
        LCD_WriteReg(hsareg, hsaval);
        LCD_WriteReg(heareg, heaval);
        LCD_WriteReg(vsareg, vsaval);
        LCD_WriteReg(veareg, veaval);
        LCD_SetCursor(sx, sy); // 设置光标位置
    }
}

/**
  * @brief  初始化ILI9341的IO引脚
  * @param  无
  * @retval 无
  */
static void ILI9341_GPIO_Config ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能FSMC对应相应管脚时钟*/
	RCC_APB2PeriphClockCmd (/*控制信号*/
                            ILI9341_CS_CLK|ILI9341_DC_CLK|ILI9341_WR_CLK|
                            ILI9341_RD_CLK	|ILI9341_BK_CLK|ILI9341_RST_CLK|
                            /*数据信号*/
                            ILI9341_D0_CLK|ILI9341_D1_CLK|	ILI9341_D2_CLK | 
                            ILI9341_D3_CLK | ILI9341_D4_CLK|ILI9341_D5_CLK|
                            ILI9341_D6_CLK | ILI9341_D7_CLK|ILI9341_D8_CLK|
                            ILI9341_D9_CLK | ILI9341_D10_CLK|ILI9341_D11_CLK|
                            ILI9341_D12_CLK | ILI9341_D13_CLK|ILI9341_D14_CLK|
                            ILI9341_D15_CLK	, ENABLE );	

		
	
	/* 配置FSMC相对应的数据线,FSMC-D0~D15 */	
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
	

	
	/* 配置FSMC相对应的控制线
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
	

  /* 配置LCD复位RST控制管脚*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_RST_PIN; 
	GPIO_Init ( ILI9341_RST_PORT, & GPIO_InitStructure );
	
	
	/* 配置LCD背光控制管脚BK*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	
	GPIO_InitStructure.GPIO_Pin = ILI9341_BK_PIN; 
	GPIO_Init ( ILI9341_BK_PORT, & GPIO_InitStructure );
}


/**
  * @brief  LCD  FSMC 模式配置
  * @param  无
  * @retval 无
  */
static void ILI9341_FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure={0};
	FSMC_NORSRAMTimingInitTypeDef  readWriteTiming={0}; 	
	
	/* 使能FSMC时钟*/
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

	//地址建立时间（ADDSET）为1个HCLK 2/72M=28ns
	readWriteTiming.FSMC_AddressSetupTime      = 0x01;	 //地址建立时间
	//数据保持时间（DATAST）+ 1个HCLK = 5/72M=70ns	
	readWriteTiming.FSMC_DataSetupTime         = 0x04;	 //数据建立时间
	//选择控制的模式
	//模式B,异步NOR FLASH模式，与ILI9341的8080时序匹配
	readWriteTiming.FSMC_AccessMode            = FSMC_AccessMode_B;	
	
	/*以下配置与模式B无关*/
	//地址保持时间（ADDHLD）模式A未用到
	readWriteTiming.FSMC_AddressHoldTime       = 0x00;	 //地址保持时间
	//设置总线转换周期，仅用于复用模式的NOR操作
	readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
	//设置时钟分频，仅用于同步类型的存储器
	readWriteTiming.FSMC_CLKDivision           = 0x00;
	//数据保持时间，仅用于同步型的NOR	
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
	
	
	/* 使能 FSMC_Bank1_NORSRAM4 */
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAMx, ENABLE );  
		
		
}


/**
  * @brief  用于 ILI9341 简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void ILI9341_Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}

/**
 * @brief  ILI9341 软件复位
 * @param  无
 * @retval 无
 */
void ILI9341_Rst ( void )
{			
	GPIO_ResetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );	 //低电平复位

	ILI9341_Delay ( 0xAFF ); 					   

	GPIO_SetBits ( ILI9341_RST_PORT, ILI9341_RST_PIN );		 	 

	ILI9341_Delay ( 0xAFF ); 	
	
}

// 初始化lcd
// 该初始化函数可以初始化各种ILI93XX液晶,但是其他函数是基于ILI9320的!!!
// 在其他型号的驱动芯片上没有测试!
void LCD_Init(void)
{
    ILI9341_GPIO_Config();
    ILI9341_FSMC_Config();
    
    GPIO_ResetBits (ILI9341_BK_PORT, ILI9341_BK_PIN);	
    
    ILI9341_Rst();
    LCD_WR_REG(0xD3);
    lcddev.id = LCD_RD_DATA(); // dummy read
    lcddev.id = LCD_RD_DATA(); // 读到0X00
    lcddev.id = LCD_RD_DATA(); // 读取93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA(); // 读取41
    
    printf(" LCD ID:%x\r\n", lcddev.id); // 打印LCD ID
    if (lcddev.id == 0X9341)             // 9341初始化
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
    LCD_Display_Dir(1);	
    LCD_Clear(WHITE);
}
// 清屏函数
// color:要清屏的填充色
void LCD_Clear(uint16_t color)
{
    uint32_t index = 0;
    uint32_t totalpoint = lcddev.width;
    totalpoint *= lcddev.height;                    // 得到总点数
    if ((lcddev.id == 0X6804) && (lcddev.dir == 1)) // 6804横屏的时候特殊处理
    {
        lcddev.dir = 0;
        lcddev.setxcmd = 0X2A;
        lcddev.setycmd = 0X2B;
        LCD_SetCursor(0x00, 0x0000); // 设置光标位置
        lcddev.dir = 1;
        lcddev.setxcmd = 0X2B;
        lcddev.setycmd = 0X2A;
    }
    else
        LCD_SetCursor(0x00, 0x0000); // 设置光标位置
    LCD_WriteRAM_Prepare();          // 开始写入GRAM
    for (index = 0; index < totalpoint; index++)
    {
        LCD_WR_DATA(color);
        // LCD->LCD_RAM = color;
    }
}
// 在指定区域内填充单个颜色
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
// color:要填充的颜色
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    uint16_t i, j;
    uint16_t xlen = 0;
    uint16_t temp;
    if ((lcddev.id == 0X6804) && (lcddev.dir == 1)) // 6804横屏的时候特殊处理
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
            LCD_SetCursor(sx, i);   // 设置光标位置
            LCD_WriteRAM_Prepare(); // 开始写入GRAM
            for (j = 0; j < xlen; j++)
                LCD_WR_DATA(color);
                // LCD->LCD_RAM = color; // 显示颜色
        }
    }
}
// 在指定区域内填充指定颜色块
//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
// color:要填充的颜色
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color)
{
    uint16_t height, width;
    uint16_t i, j;
    width = ex - sx + 1;  // 得到填充的宽度
    height = ey - sy + 1; // 高度
    for (i = 0; i < height; i++)
    {
        LCD_SetCursor(sx, sy + i); // 设置光标位置
        LCD_WriteRAM_Prepare();    // 开始写入GRAM
        for (j = 0; j < width; j++)
            LCD_WR_DATA(color[i * width + j]);
            // LCD->LCD_RAM = color[i * width + j]; // 写入数据
    }
}
// 画线
// x1,y1:起点坐标
// x2,y2:终点坐标
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint16_t t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1; // 计算坐标增量
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1; // 设置单步方向
    else if (delta_x == 0)
        incx = 0; // 垂直线
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0; // 水平线
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y)
        distance = delta_x; // 选取基本增量坐标轴
    else
        distance = delta_y;
    for (t = 0; t <= distance + 1; t++) // 画线输出
    {
        LCD_DrawPoint(uRow, uCol); // 画点
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
// 画矩形
//(x1,y1),(x2,y2):矩形的对角坐标
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}
// 在指定位置画一个指定大小的圆
//(x,y):中心点
// r    :半径
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1); // 判断下个点位置的标志
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
        // 使用Bresenham算法画圆
        if (di < 0)
            di += 4 * a + 6;
        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }
    }
}
// 在指定位置显示一个字符
// x,y:起始坐标
// num:要显示的字符:" "--->"~"
// size:字体大小 12/16/24
// mode:叠加方式(1)还是非叠加方式(0)
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp, t1, t;
    uint16_t y0 = y;
    uint8_t csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2); // 得到字体一个字符对应点阵集所占的字节数
    num = num - ' ';                                           // 得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
    for (t = 0; t < csize; t++)
    {
        if (size == 12)
            temp = asc2_1206[num][t]; // 调用1206字体
        else if (size == 16)
            temp = asc2_1608[num][t]; // 调用1608字体
        else if (size == 24)
            temp = asc2_2412[num][t]; // 调用2412字体
        else
            return; // 没有的字库
        for (t1 = 0; t1 < 8; t1++)
        {
            if (temp & 0x80)
                LCD_Fast_DrawPoint(x, y, POINT_COLOR);
            else if (mode == 0)
                LCD_Fast_DrawPoint(x, y, BACK_COLOR);
            temp <<= 1;
            y++;
            if (y >= lcddev.height)
                return; // 超区域了
            if ((y - y0) == size)
            {
                y = y0;
                x++;
                if (x >= lcddev.width)
                    return; // 超区域了
                break;
            }
        }
    }
}
// m^n函数
// 返回值:m^n次方.
uint32_t LCD_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while (n--)
        result *= m;
    return result;
}
// 显示数字,高位为0,则不显示
// x,y :起点坐标
// len :数字的位数
// size:字体大小
// color:颜色
// num:数值(0~4294967295);
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
// 显示数字,高位为0,还是显示
// x,y:起点坐标
// num:数值(0~999999999);
// len:长度(即要显示的位数)
// size:字体大小
// mode:
//[7]:0,不填充;1,填充0.
//[6:1]:保留
//[0]:0,非叠加显示;1,叠加显示.
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
// 显示字符串
// x,y:起点坐标
// width,height:区域大小
// size:字体大小
//*p:字符串起始地址
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p)
{
    uint8_t x0 = x;
    width += x;
    height += y;
    while ((*p <= '~') && (*p >= ' ')) // 判断是不是非法字符!
    {
        if (x >= width)
        {
            x = x0;
            y += size;
        }
        if (y >= height)
            break; // 退出
        LCD_ShowChar(x, y, *p, size, 0);
        x += size / 2;
        p++;
    }
}
