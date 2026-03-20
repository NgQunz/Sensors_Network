#include "stm32f10x.h"      // Device header
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_i2c.h"  // Keil::Device:StdPeriph Drivers:I2C
#include "stm32f10x_rcc.h"  // Keil::Device:StdPeriph Drivers:RCC
#include "bh1750.h"
#include "tim2.h"

#define BH1750_ADDRESS 0x46 // dia chi I2C cua BH1750

static void set_i2c_7bit_address(I2C_TypeDef * i2cx, uint16_t i2c_address, uint8_t direct_i2c);
static void start_i2c(I2C_TypeDef * i2cx, uint16_t i2c_address);
static void stop_i2c(I2C_TypeDef * i2cx);
static void i2c_standard_send_a_byte(I2C_TypeDef * i2cx, uint8_t serial_line);
static uint8_t i2c_receive_a_byte_ack(I2C_TypeDef * i2cx);
static uint8_t i2c_receive_a_byte_nack(I2C_TypeDef * i2cx);

void bh1750_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

  I2C_InitTypeDef I2C_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // Alternate Function - OD
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; // khi su dung standard mode
  I2C_InitStructure.I2C_OwnAddress1 = 0x00;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000; // standard mode
  I2C_Init(I2C1, &I2C_InitStructure);

  I2C_Cmd(I2C1, ENABLE);
}
uint16_t BH1750_ReadLight(void)
{
  uint16_t light = 0; // 16 bit du lieu cam bien
  uint8_t data[2];    // luu tru 2 byte du lieu
  // Send "Continuously H-resolution mode " instruction
  i2c_write_one_byte_no_reg(I2C1, BH1750_ADDRESS, 0x10); // master gui dia chi va yeu cau mode doc
                                                         /*0x10 Start measurement at 1lx resolution.
                                                         Continuously H-Resolution Mode
                                                         Measurement Time is typically 120ms */
                                                         // Wait to complete 1st H-resolution mode measurement.( max. 180ms. )
  Delay_ms(180);
  // Read measurement result
  i2c_read_multi_byte_no_reg(I2C1, BH1750_ADDRESS, 2, data);
  light = (data[0] << 8) | data[1];
  return light / 1.2; // BH1750 outputs data in 1.2x format
}

// I2C

void i2c_read_multi_byte_no_reg(I2C_TypeDef * i2cx, uint16_t address, uint8_t len,uint8_t *data){
  start_i2c(i2cx,address); // start condition
  set_i2c_7bit_address(i2cx,address,I2C_Direction_Receiver);// 7bit addr + bit 1 (read)
  for(int i=0;i<len;i++){
      if(i==len-1){
        *(data+i) = i2c_receive_a_byte_nack(i2cx);// last byte
      }
      else{
        *(data+i) = i2c_receive_a_byte_ack(i2cx) ; // first byte
      }
  }
  stop_i2c(i2cx);
}
void i2c_write_one_byte_no_reg(I2C_TypeDef * i2cx, uint16_t address,uint8_t data){
  start_i2c(i2cx,address);// start condition
  set_i2c_7bit_address(i2cx,address,I2C_Direction_Transmitter);// master gui dia chi den slave(DR)
  i2c_standard_send_a_byte(i2cx,data);// master ghi du lieu
  while(!I2C_CheckEvent(i2cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED)){} /// wait till transmitted last byte complete
  stop_i2c(i2cx);// stop condition
}
static void set_i2c_7bit_address(I2C_TypeDef * i2cx, uint16_t i2c_address, uint8_t direct_i2c){
  I2C_Send7bitAddress(i2cx,i2c_address,direct_i2c);//transmister ->(address) <=> (7bit + bit 0(wirte))

  switch(direct_i2c){
    case I2C_Direction_Transmitter:{ // truong hop write  
      while(!I2C_CheckEvent(i2cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){} /// wait till i2c -> master transmitter mode
      break;
    }
    case I2C_Direction_Receiver:{ // truong hop read  
      while(!I2C_CheckEvent(i2cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){} /// wait till i2c -> master receiver mode
      break;
    }
  }
}

static void start_i2c(I2C_TypeDef * i2cx, uint16_t i2c_address){ /// after that call, stm32 default turn to master mode. Slave use that to recognize a start serial communicate
  I2C_GenerateSTART(i2cx,ENABLE);/// bat dau chuyen i2c - CR1 = 0x0100
  while(!I2C_CheckEvent(i2cx,I2C_EVENT_MASTER_MODE_SELECT)){} /// i2c select to be master mode. EV5 (SB = 1)
}

static void stop_i2c(I2C_TypeDef * i2cx){ /// end i2c serial communicate, i2c turn to idle mode
  I2C_GenerateSTOP(i2cx,ENABLE); /// truyen tin hieu ket thuc
}
static void i2c_standard_send_a_byte(I2C_TypeDef * i2cx, uint8_t serial_line){ /// send as much as you want during transmit
  I2C_SendData(i2cx,serial_line); // DR = serial_line
  while(!I2C_CheckEvent(i2cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING)){} /// check if i2c master transmitted complete and receive ack
}

static uint8_t i2c_receive_a_byte_ack(I2C_TypeDef * i2cx)
{
	// Enable ACK of received data
	I2C_AcknowledgeConfig(i2cx, ENABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while (!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_RECEIVED));

	// Read and return data byte from I2C data register
	return I2C_ReceiveData(i2cx);
}

static uint8_t i2c_receive_a_byte_nack(I2C_TypeDef * i2cx)
{
	// Disable ACK of received data
	I2C_AcknowledgeConfig(i2cx, DISABLE);
	// Wait for I2C EV7
	// It means that the data has been received in I2C data register
	while (!I2C_CheckEvent(i2cx, I2C_EVENT_MASTER_BYTE_RECEIVED));//RXNE = 1

	// Read and return data byte from I2C data register
	return I2C_ReceiveData(i2cx);
}
/*end of file*/
