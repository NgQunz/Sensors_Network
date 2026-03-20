#ifndef __DHT22_H
#define __DHT22_H

#include "stm32f10x.h"
#include <stdint.h>

typedef struct {
    int16_t  temperature_x10;   // ví d? 253 = 25.3°C, -12 = -1.2°C
    uint16_t humidity_x10;      // ví d? 575 = 57.5%
    uint8_t  checksum_ok;       // 1: ok, 0: fail
} DHT22_Data_t;

void DHT22_Init(void);
void DHT22_Read(void) ; // return 1 n?u d?c du?c frame, 0 n?u timeout/format l?i

#endif