#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "stm32f10x.h"

void DS18B20_Start(void);
void DS18B20_WriteByte(uint8_t data);
uint8_t DS18B20_ReadByte(void);

#endif