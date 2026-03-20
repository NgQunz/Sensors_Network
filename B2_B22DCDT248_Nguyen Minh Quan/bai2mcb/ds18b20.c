#include "ds18b20.h"

// G?i nh? hàm Delay_us t? main.c sang
extern void Delay_us(uint32_t us);

static void DS18B20_PIN_OUT(void) {
    GPIOA->CRL &= ~(0x0FU << 4); 
    GPIOA->CRL |=  (0x03 << 4); // Push-Pull
}

static void DS18B20_PIN_IN(void) {
    GPIOA->CRL &= ~(0x0FU << 4); 
    GPIOA->CRL |=  (0x08 << 4); // Input Pull-up/down
    GPIOA->ODR |=  (1 << 1);    // Kéo Pull-up
}

void DS18B20_Start(void) {
    DS18B20_PIN_OUT();
    GPIOA->BRR = (1 << 1); 
    Delay_us(480);
    DS18B20_PIN_IN();      
    Delay_us(480);
}

void DS18B20_WriteByte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            DS18B20_PIN_OUT();
            GPIOA->BRR = (1 << 1); Delay_us(1);
            DS18B20_PIN_IN();      Delay_us(60);
        } else {
            DS18B20_PIN_OUT();
            GPIOA->BRR = (1 << 1); Delay_us(60);
            DS18B20_PIN_IN();
        }
    }
}

uint8_t DS18B20_ReadByte(void) {
    uint8_t value = 0;
    DS18B20_PIN_IN(); 
    for (uint8_t i = 0; i < 8; i++) {
        DS18B20_PIN_OUT();
        GPIOA->BRR = (1 << 1); Delay_us(2);
        DS18B20_PIN_IN();      Delay_us(10);
        
        if (GPIOA->IDR & (1 << 1)) value |= (1 << i);
        Delay_us(50); 
    }
    return value;
}