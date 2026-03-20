#ifndef _UART_H_
#define _UART_H_

#include "stm32f10x.h"

// Khai báo nguyên m?u hàm (B? ch? static)
void USART1_Init(void);
void UART_SendString(char *s);

#endif