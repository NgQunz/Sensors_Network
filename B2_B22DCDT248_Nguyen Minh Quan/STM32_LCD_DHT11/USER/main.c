#include "stm32f10x.h"
#include "tim2.h"
#include "uart.h"
#include "dht22.h"
#include "bh1750.h"
#include "mpu9265.h"
#include "mpu6050.h"

int main(void)
{
    USART1_Init(9600);
		Timer2_Init();
		DHT22_Init();
		bh1750_init();
	  MPU9265_SPI1_Init();
	  MPU6050_I2C1_Init();
	  if(!MPU6050_Init())
    {
        USART1_Send_String("MPU6050 init FAIL\r\n");
        while(1);
    }
	  USART1_Send_String("All sensor init ok\r\n");
		for (uint8_t i=0;i<=100;i++)
		{
			DHT22_Read();
			MPU6050_PrintData();
//			uint16_t light = BH1750_ReadLight();
//			USART1_Send_String("Light: ");
//			USART1_Send_Number(light);
//			USART1_Send_String("\n");
			MPU9265_Read_Print();
			GPIOC->ODR ^= GPIO_Pin_13;
			Delay_ms(1000);
		}
}

