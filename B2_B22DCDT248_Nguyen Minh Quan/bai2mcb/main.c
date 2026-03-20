#include "stm32f10x.h"
#include <stdio.h>

// Nhúng thu vi?n c?m bi?n và giao ti?p
#include "ds18b20.h"
#include "bh1750.h"
#include "adxl345.h"
#include "uart.h"

static char uart_buf[128]; 

// ==========================================================
// HÀM DELAY T?O TR? (Gi? l?i d? module DS18B20 g?i extern)
// ==========================================================
void Delay_us(uint32_t us) {
    SysTick->LOAD = (72 * us) - 1; 
    SysTick->VAL = 0;
    SysTick->CTRL = 5;
    while (!(SysTick->CTRL & 0x10000));
    SysTick->CTRL = 0;
}

void Delay_ms(uint32_t ms) {
    while (ms--) { Delay_us(1000); }
}

// ==========================================================
// CHUONG TRÌNH CHÍNH
// ==========================================================
int main(void) {
    float lux = 0.0;
    uint8_t temp1, temp2;
    float temperature = 0.0;
    uint8_t adxl_data[6];
    int16_t x, y, z;

    // 1. Kh?i t?o ph?n c?ng Bare Metal
    USART1_Init();
    BH1750_Init();
    ADXL345_Init();
    

    Delay_ms(200);

    // 2. Vòng l?p l?y m?u liên t?c
    while (1) {
        
        // --- ÐO NHI?T Ð? (DS18B20) ---
        DS18B20_Start();
        DS18B20_WriteByte(0xCC); 
        DS18B20_WriteByte(0x44); 
        
        Delay_ms(800); 
        
        DS18B20_Start();
        DS18B20_WriteByte(0xCC); 
        DS18B20_WriteByte(0xBE); 
        temp1 = DS18B20_ReadByte(); 
        temp2 = DS18B20_ReadByte(); 
        temperature = (float)((temp2 << 8) | temp1) / 16.0f;

        // --- ÐO ÁNH SÁNG (BH1750) ---
        lux = BH1750_ReadLux();

        // --- ÐO GIA T?C (ADXL345) ---
        ADXL_ReadMulti(0x32, adxl_data, 6);
        x = (int16_t)((adxl_data[1] << 8) | adxl_data[0]);
        y = (int16_t)((adxl_data[3] << 8) | adxl_data[2]);
        z = (int16_t)((adxl_data[5] << 8) | adxl_data[4]);

        // --- XU?T D? LI?U QUA UART ---
        sprintf(uart_buf, "[Nhiet do]: %5.2f \xC2\xB0 C    [Sang]: %7.2f Lux  |  [Gia toc]: X:%5d Y:%5d Z:%5d\r\n", 
                (double)temperature,  (double)lux, x, y, z);
        UART_SendString(uart_buf);
        
        Delay_ms(200); 
    }
}