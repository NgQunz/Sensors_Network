#ifndef _BH1750_H_
#define _BH1750_H_

#include "stm32f10x.h"

void BH1750_Init(void);
float BH1750_ReadLux(void);

#endif