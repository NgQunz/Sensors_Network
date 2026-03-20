#ifndef __BH1750_H
#define __BH1750_H

#include "stm32f10x.h"                  // Device header
//#include "My_i2c.h"

void bh1750_init(void);
uint16_t BH1750_ReadLight(void);
void i2c_read_multi_byte_no_reg(I2C_TypeDef * i2cx, uint16_t address, uint8_t len,uint8_t *data);
void i2c_write_one_byte_no_reg(I2C_TypeDef * i2cx, uint16_t address,uint8_t data);
#endif /* __BH1750_H */