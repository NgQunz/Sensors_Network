#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f10x.h"
#include <stdint.h>

typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t temp;
} MPU6050_Raw_t;

typedef struct {
    float ax_g, ay_g, az_g;      // g
    float gx_dps, gy_dps, gz_dps; // deg/s
    float temp_c;               // °C
} MPU6050_Scaled_t;

void     MPU6050_I2C1_Init(void);
uint8_t  MPU6050_Init(void);
uint8_t  MPU6050_ReadRaw(MPU6050_Raw_t *raw);
void MPU6050_PrintData(void);

#endif