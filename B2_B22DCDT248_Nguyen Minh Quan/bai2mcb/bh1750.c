#include "bh1750.h"

#define BH1750_WRITE 0x46 
#define BH1750_READ  0x47 

// Mu?n hàm Delay t? main.c sang d? ch? c?m bi?n t?nh ng?
extern void Delay_ms(uint32_t ms);

static void I2C1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    GPIOB->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6 | GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
    GPIOB->CRL |= (GPIO_CRL_MODE6 | GPIO_CRL_CNF6_1 | GPIO_CRL_CNF6_0 | 
                   GPIO_CRL_MODE7 | GPIO_CRL_CNF7_1 | GPIO_CRL_CNF7_0);

    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    // C?u hình chu?n 36MHz (Vì chip dang ch?y 72MHz)
    I2C1->CR2 = 36;         
    I2C1->CCR = 180;        
    I2C1->TRISE = 37;       
    
    I2C1->CR1 |= I2C_CR1_PE; 
}

static void I2C_Start(void) {
    I2C1->CR1 |= I2C_CR1_START;
    while (!(I2C1->SR1 & I2C_SR1_SB));
}

static void BH1750_WriteCmd(uint8_t cmd) {
    I2C_Start();
    I2C1->DR = BH1750_WRITE;
    while (!(I2C1->SR1 & I2C_SR1_ADDR));
    (void)I2C1->SR1; (void)I2C1->SR2;
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = cmd; 
    while (!(I2C1->SR1 & I2C_SR1_BTF));
    I2C1->CR1 |= I2C_CR1_STOP;
}

void BH1750_Init(void) {
    I2C1_Init();
    
    // --- B?T Ð?U FIX: ÉP C?M BI?N TH?C D?Y T? T? ---
    Delay_ms(100);             // Ch? ?n d?nh ngu?n
    BH1750_WriteCmd(0x01);     // L?nh POWER ON
    Delay_ms(20);              // Ch? m?ch logic bên trong c?m bi?n kh?i d?ng
    BH1750_WriteCmd(0x03);     // L?nh RESET
    Delay_ms(20);              // Ch? xóa b? d?m cu
    BH1750_WriteCmd(0x10);     // B?t d?u do phân gi?i cao liên t?c
    Delay_ms(180);             // L?n do d?u tiên c?n t?i da 180ms d? hoàn thành
    // --- K?T THÚC FIX ---
}
float BH1750_ReadLux(void) {
    uint8_t bh_high = 0, bh_low = 0;
    
    I2C_Start();
    I2C1->DR = BH1750_READ; 
    while (!(I2C1->SR1 & I2C_SR1_ADDR)); 

    // --- CÁCH Ð?C TH? CÔNG T?NG BYTE (An toàn tuy?t d?i) ---
    
    // 1. B?t ACK d? s?n sàng nh?n Byte 1
    I2C1->CR1 |= I2C_CR1_ACK;         
    (void)I2C1->SR1; (void)I2C1->SR2; // Xóa c? ADDR d? b?t d?u hút d? li?u

    // 2. Ch? và hút Byte 1
    while (!(I2C1->SR1 & I2C_SR1_RXNE)); // Ch? Byte 1 nh?y vào b? d?m
    bh_high = (uint8_t)I2C1->DR;         // Rút Byte 1 ra (ACK s? t? d?ng du?c g?i di)

    // 3. Chu?n b? ép NACK cho Byte 2
    I2C1->CR1 &= ~I2C_CR1_ACK;        // T?t ACK (Báo cho c?m bi?n: "G?i n?t byte này là ngh? nhé")
    I2C1->CR1 |= I2C_CR1_STOP;        // Ð?t s?n c? STOP

    // 4. Ch? và hút Byte 2
    while (!(I2C1->SR1 & I2C_SR1_RXNE)); // Ch? Byte 2 nh?y vào
    bh_low = (uint8_t)I2C1->DR;          // Rút Byte 2 ra (Lúc này STOP dã du?c b?n di)

    // 5. Ph?c h?i tr?ng thái cho l?n d?c ti?p theo
    I2C1->CR1 |= I2C_CR1_ACK; 
    
    return (float)((bh_high << 8) | bh_low) / 1.2f;
}