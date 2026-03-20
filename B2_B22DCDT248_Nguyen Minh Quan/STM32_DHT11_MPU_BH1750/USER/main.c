#include "stm32f10x.h"
#include "tim2.h"
#include "uart.h"
#include "dht11.h"
#include "bh1750.h"
#include "mpu9265.h"
void MPU9265_Read_Print(void);
int main(void)
{
    USART1_Init(9600);
		Timer2_Init();
		DHT22_Init();
		//bh1750_init();
	 // MPU9265_SPI1_Init();
		while(1)
		{
			DHT22_Read();
			USART1_Send_String("abc\n");
//			uint16_t light = BH1750_ReadLight();
//			USART1_Send_String("Light: ");
//			USART1_Send_Number(light);
//			USART1_Send_String("\n");
//			MPU9265_Read_Print();
			GPIOC->ODR ^= GPIO_Pin_13;
			Delay_ms(1000);
		}
}

