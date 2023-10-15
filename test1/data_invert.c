#include "data_invert.h"

// 高为在前，低位在后
void u8_to_u16(uint8_t *u8_data, uint16_t *u16_data)
{
    *u16_data = (u8_data[0] << 8) | u8_data[1];
}

void u16_to_u8(uint32_t *u16_data, uint8_t *u8_data)
{
    u8_data[0] = (*u16_data >> 8) & 0xff;
    u8_data[1] = *u16_data & 0xff;
}

void u8_to_u32(uint8_t *u8_data, uint32_t *u32_data)
{
    *u32_data = (u8_data[0] << 24) | (u8_data[1] << 16) | (u8_data[2] << 8) | u8_data[3];
}

void u32_to_u8(uint32_t *u32_data, uint8_t *u8_data)
{
    u8_data[0] = (*u32_data >> 24) & 0xff;
    u8_data[1] = (*u32_data >> 16) & 0xff;
    u8_data[2] = (*u32_data >> 8) & 0xff;
    u8_data[3] = *u32_data & 0xff;
}

void u16_to_u32(uint16_t *u16_data, uint32_t *u32_data)
{
    *u32_data = (u16_data[0] << 16) | u16_data[1];
}

void u32_to_u16(uint32_t *u32_data, uint16_t *u16_data)
{
    u16_data[0] = (*u32_data >> 16) & 0xffff; //高16位
    u16_data[1] = *u32_data & 0xffff;         //低16位
}

int main(void)
{
    uint8_t a = 0xf0;
    uint16_t b = 0;
    u16_to_u8(&a, &b);
    printf("0x%04x\n", b);

    system("pause");
    return 0;
}