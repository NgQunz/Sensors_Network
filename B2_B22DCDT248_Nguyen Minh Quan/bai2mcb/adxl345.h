#ifndef _ADXL345_H_
#define _ADXL345_H_

#include "stm32f10x.h"

void ADXL345_Init(void);
void ADXL_ReadMulti(uint8_t reg, uint8_t *buffer, uint8_t length);

#endif