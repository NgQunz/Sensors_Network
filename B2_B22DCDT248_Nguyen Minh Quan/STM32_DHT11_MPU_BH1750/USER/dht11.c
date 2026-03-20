#include "dht22.h"   // b?n có th? d?i tên file/h n?u mu?n
#include "uart.h"
#include "tim2.h"
#include "stm32f10x.h"

#define DHT_GPIO   GPIOB
#define DHT_PIN    GPIO_Pin_12

// ================== helper wait level ==================
static uint8_t DHT_WaitLevel(uint8_t level, uint16_t timeout_us)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < timeout_us) {
        if (GPIO_ReadInputDataBit(DHT_GPIO, DHT_PIN) == level) return 1;
    }
    return 0;
}

// ================== READ 1 BYTE (DHT22) ==================
static uint8_t DHT22_ReadByte(void)
{
    uint8_t byte = 0;

    for (int i = 0; i < 8; i++) {

        // 1) ch? bit b?t d?u: LOW ~50us
        if (!DHT_WaitLevel(0, 100)) break;

        // 2) ch? lên HIGH (b?t d?u pha HIGH)
        if (!DHT_WaitLevel(1, 100)) break;

        // 3) do d? r?ng HIGH d? phân bit
        TIM_SetCounter(TIM2, 0);
        while (GPIO_ReadInputDataBit(DHT_GPIO, DHT_PIN)) {
            if (TIM_GetCounter(TIM2) > 120) break; // safety
        }
        uint16_t high_us = TIM_GetCounter(TIM2);

        byte <<= 1;
        if (high_us > 45) byte |= 1;  // >45us => bit 1 (kho?ng 70us)
    }

    return byte;
}

// ================== INIT ==================
void DHT22_Init(void) // b?n có th? d?i tên thành DHT22_Init
{
    GPIO_InitTypeDef gpioInit;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    // LED DEBUG PC13 - OUTPUT
    gpioInit.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin   = GPIO_Pin_13;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);

    // DHT PB12 - OUTPUT OD (nh? ra HIGH du?c nh? pull-up)
    gpioInit.GPIO_Mode  = GPIO_Mode_Out_OD;
    gpioInit.GPIO_Pin   = DHT_PIN;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DHT_GPIO, &gpioInit);

    GPIO_SetBits(DHT_GPIO, DHT_PIN); // idle high
}

// ================== READ DHT22 ==================
void DHT22_Read(void) 
{
    uint8_t  u8Buff[5];
    uint8_t  sum;

    // ===== Start signal (DHT22) =====
    GPIO_ResetBits(DHT_GPIO, DHT_PIN);   // LOW
    Delay_ms(2);                         // DHT22: >= 1ms (2ms cho ch?c)
    GPIO_SetBits(DHT_GPIO, DHT_PIN);     // release -> HIGH

    // N?u b?n mu?n chu?n hon, chuy?n pin sang input pull-up lúc d?c:
    // (nhung b?n dang dùng OD + module có pull-up v?n d?c du?c)
    // GPIO_InitTypeDef gpio;
    // gpio.GPIO_Mode = GPIO_Mode_IPU;
    // gpio.GPIO_Pin = DHT_PIN;
    // gpio.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_Init(DHT_GPIO, &gpio);

    // ===== Sensor response: LOW 80us, HIGH 80us, then LOW =====
    if (!DHT_WaitLevel(0, 120)) { USART1_Send_String("DHT22 no response (LOW)\n"); return; }
    if (!DHT_WaitLevel(1, 120)) { USART1_Send_String("DHT22 no response (HIGH)\n"); return; }
    if (!DHT_WaitLevel(0, 120)) { USART1_Send_String("DHT22 no start data\n"); return; }

    // ===== Read 5 bytes =====
    for (int i = 0; i < 5; i++) {
        u8Buff[i] = DHT22_ReadByte();
    }

    // ===== checksum =====
    sum = (uint8_t)(u8Buff[0] + u8Buff[1] + u8Buff[2] + u8Buff[3]);

    // ===== Parse DHT22 =====
    uint16_t rh_x10   = ((uint16_t)u8Buff[0] << 8) | u8Buff[1];  // humidity *10
    uint16_t t_raw    = ((uint16_t)u8Buff[2] << 8) | u8Buff[3];  // sign + temp *10

    int16_t t_x10;
    if (t_raw & 0x8000) { // negative
        t_raw &= 0x7FFF;
        t_x10 = -(int16_t)t_raw;
    } else {
        t_x10 = (int16_t)t_raw;
    }

    // ===== Print UART (không dùng float) =====
    USART1_Send_String("Temperature: ");
    if (t_x10 < 0) { USART1_Send_String("-"); t_x10 = -t_x10; }
    USART1_Send_Number(t_x10 / 10);
    USART1_Send_String(".");
    USART1_Send_Number(t_x10 % 10);
    USART1_Send_String("*C\n");

    USART1_Send_String("Humidity: ");
    USART1_Send_Number(rh_x10 / 10);
    USART1_Send_String(".");
    USART1_Send_Number(rh_x10 % 10);
    USART1_Send_String("%\n");

    if (sum != u8Buff[4]) {
        USART1_Send_String("Checksum ERROR!\n");
    }
}