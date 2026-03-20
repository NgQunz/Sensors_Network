#include "adxl345.h"

#define CS_LOW()  (GPIOA->BRR = GPIO_BRR_BR4)
#define CS_HIGH() (GPIOA->BSRR = GPIO_BSRR_BS4)

static void SPI1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN;

    GPIOA->CRL &= ~(0xFFFFU << 16); 
    GPIOA->CRL |= (0x3U << 16) | (0xAU << 20) | (0x8U << 24) | (0xAU << 28);
    GPIOA->ODR |= (1 << 6); 

    CS_HIGH(); 
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA | 
                SPI_CR1_BR_1 | SPI_CR1_BR_0 | SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_SPE; 
}

static uint8_t SPI_Transfer(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE)); 
    SPI1->DR = data; 
    while (!(SPI1->SR & SPI_SR_RXNE)); 
    return (uint8_t)SPI1->DR;
}

static void ADXL_WriteReg(uint8_t reg, uint8_t value) {
    CS_LOW();
    SPI_Transfer(reg & 0x7F); 
    SPI_Transfer(value);
    CS_HIGH();
}

void ADXL_ReadMulti(uint8_t reg, uint8_t *buffer, uint8_t length) {
    CS_LOW();
    SPI_Transfer(reg | 0xC0); 
    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = SPI_Transfer(0xFF); 
    }
    CS_HIGH();
}

void ADXL345_Init(void) {
    SPI1_Init();
    ADXL_WriteReg(0x31, 0x0B); 
    ADXL_WriteReg(0x2D, 0x08); 
}