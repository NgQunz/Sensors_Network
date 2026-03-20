#include "uart.h"

// C?u hình UART chu?n 9600 Baud cho xung nh?p 72MHz
void USART1_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;
    
    // Chân PA9 (TX) - Alternate function output Push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE9 | GPIO_CRH_CNF9_1);
    
    USART1->BRR = 0x1D4C; // 72MHz / 9600
    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE;
}

// Hàm d?y chu?i ký t?
void UART_SendString(char *s) {
    while (*s) {
        while (!(USART1->SR & USART_SR_TXE));
        USART1->DR = *s++;
    }
}