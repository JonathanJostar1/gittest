#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* 8位转16位 */
void u8_to_u16(uint8_t *u8_data, uint16_t *u16_data);

void u16_to_u8(uint32_t *u16_data, uint8_t *u8_data);

void u8_to_u32(uint8_t *u8_data, uint32_t *u32_data);

void u32_to_u8(uint32_t *u32_data, uint8_t *u8_data);

void u16_to_u32(uint16_t *u16_data, uint32_t *u32_data);

void u32_to_u16(uint32_t *u32_data, uint16_t *u16_data);