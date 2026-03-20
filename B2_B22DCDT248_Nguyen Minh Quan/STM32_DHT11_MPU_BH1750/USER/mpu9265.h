#ifndef __MPU9265_H__
#define __MPU9265_H__

#include "stm32f10x.h"
#include <stdint.h>

typedef struct {
    int16_t ax, ay, az;
    int16_t temp;
    int16_t gx, gy, gz;
} MPU9265_Raw_t;
typedef struct
{
    float ax;
    float ay;
    float az;

    float gx;
    float gy;
    float gz;

    float temp;
} MPU9265_Data_t;
void     MPU9265_SPI1_Init(void);
uint8_t  MPU9265_WhoAmI(void);
int      MPU9265_Init(void);
int      MPU9265_ReadRaw(MPU9265_Raw_t *raw);
void MPU9265_Read_Print(void);
#endif