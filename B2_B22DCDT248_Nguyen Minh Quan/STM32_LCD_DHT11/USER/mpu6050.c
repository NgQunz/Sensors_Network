#include "mpu6050.h"

// ======================= CONFIG =======================
#define MPU6050_ADDR_7BIT     0x68
#define MPU6050_ADDR_WRITE    (MPU6050_ADDR_7BIT << 1)       // 0xD0
#define MPU6050_ADDR_READ     ((MPU6050_ADDR_7BIT << 1) | 1) // 0xD1

// Registers
#define REG_WHO_AM_I          0x75
#define REG_PWR_MGMT_1        0x6B
#define REG_SMPLRT_DIV        0x19
#define REG_CONFIG            0x1A
#define REG_GYRO_CONFIG       0x1B
#define REG_ACCEL_CONFIG      0x1C
#define REG_ACCEL_XOUT_H      0x3B

// Full-scale settings (we set these)
#define ACCEL_FS_SEL_2G       0x00  // +/-2g
#define GYRO_FS_SEL_250DPS    0x00  // +/-250 dps

// Timeouts (simple)
#define I2C_TIMEOUT           60000

static uint8_t i2c_wait(volatile uint32_t *flag_reg, uint32_t flag_mask, uint8_t set)
{
    uint32_t to = I2C_TIMEOUT;
    while (to--) {
        uint32_t v = *flag_reg;
        if (set) { if (v & flag_mask) return 1; }
        else     { if (!(v & flag_mask)) return 1; }
    }
    return 0;
}

static uint8_t MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    // START
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_SB, 1)) return 0;
    (void)I2C1->SR1;

    // Address + Write
    I2C_Send7bitAddress(I2C1, MPU6050_ADDR_WRITE, I2C_Direction_Transmitter);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_ADDR, 1)) return 0;
    (void)I2C1->SR1; (void)I2C1->SR2;

    // Reg
    I2C_SendData(I2C1, reg);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_TXE, 1)) return 0;

    // Data
    I2C_SendData(I2C1, data);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_BTF, 1)) return 0;

    // STOP
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 1;
}

static uint8_t MPU6050_ReadRegs(uint8_t reg, uint8_t *buf, uint8_t len)
{
    if (!buf || len == 0) return 0;

    // START
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_SB, 1)) return 0;
    (void)I2C1->SR1;

    // Address + Write
    I2C_Send7bitAddress(I2C1, MPU6050_ADDR_WRITE, I2C_Direction_Transmitter);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_ADDR, 1)) return 0;
    (void)I2C1->SR1; (void)I2C1->SR2;

    // Reg pointer
    I2C_SendData(I2C1, reg);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_TXE, 1)) return 0;

    // Re-START
    I2C_GenerateSTART(I2C1, ENABLE);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_SB, 1)) return 0;
    (void)I2C1->SR1;

    // Address + Read
    I2C_Send7bitAddress(I2C1, MPU6050_ADDR_READ, I2C_Direction_Receiver);
    if (!i2c_wait(&I2C1->SR1, I2C_SR1_ADDR, 1)) return 0;

    if (len == 1) {
        // single byte read: NACK + STOP
        I2C_AcknowledgeConfig(I2C1, DISABLE);
        (void)I2C1->SR1; (void)I2C1->SR2;
        I2C_GenerateSTOP(I2C1, ENABLE);

        if (!i2c_wait(&I2C1->SR1, I2C_SR1_RXNE, 1)) return 0;
        buf[0] = I2C_ReceiveData(I2C1);

        I2C_AcknowledgeConfig(I2C1, ENABLE);
        return 1;
    }

    // multi-byte read
    (void)I2C1->SR1; (void)I2C1->SR2;
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    for (uint8_t i = 0; i < len; i++) {
        if (i == (len - 1)) {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        if (!i2c_wait(&I2C1->SR1, I2C_SR1_RXNE, 1)) return 0;
        buf[i] = I2C_ReceiveData(I2C1);
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);
    return 1;
}

// ======================= PUBLIC =======================
void MPU6050_I2C1_Init(void)
{
    // GPIOB + I2C1 clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // PB6/PB7 as AF Open-Drain
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &gpio);

    // I2C1 config (100kHz)
    I2C_DeInit(I2C1);

    I2C_InitTypeDef i2c;
    i2c.I2C_Mode                = I2C_Mode_I2C;
    i2c.I2C_DutyCycle           = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1         = 0x00;
    i2c.I2C_Ack                 = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c.I2C_ClockSpeed          = 100000;
    I2C_Init(I2C1, &i2c);

    I2C_Cmd(I2C1, ENABLE);
}

uint8_t MPU6050_Init(void)
{
    uint8_t who = 0;

    // wake up
    if (!MPU6050_WriteReg(REG_PWR_MGMT_1, 0x00)) return 0;

    // optional basic config
    // SampleRate = GyroRate/(1+SMPLRT_DIV). GyroRate = 1kHz when DLPF enabled
    if (!MPU6050_WriteReg(REG_SMPLRT_DIV, 0x07)) return 0; // ~125 Hz
    if (!MPU6050_WriteReg(REG_CONFIG, 0x03)) return 0;     // DLPF ~44Hz
    if (!MPU6050_WriteReg(REG_GYRO_CONFIG,  GYRO_FS_SEL_250DPS)) return 0;
    if (!MPU6050_WriteReg(REG_ACCEL_CONFIG, ACCEL_FS_SEL_2G))    return 0;

    // WHO_AM_I should be 0x68
    if (!MPU6050_ReadRegs(REG_WHO_AM_I, &who, 1)) return 0;
    return (who == 0x68) ? 1 : 0;
}

uint8_t MPU6050_ReadRaw(MPU6050_Raw_t *raw)
{
    if (!raw) return 0;

    uint8_t b[14];
    if (!MPU6050_ReadRegs(REG_ACCEL_XOUT_H, b, 14)) return 0;

    raw->ax   = (int16_t)((b[0] << 8) | b[1]);
    raw->ay   = (int16_t)((b[2] << 8) | b[3]);
    raw->az   = (int16_t)((b[4] << 8) | b[5]);
    raw->temp = (int16_t)((b[6] << 8) | b[7]);
    raw->gx   = (int16_t)((b[8] << 8) | b[9]);
    raw->gy   = (int16_t)((b[10] << 8) | b[11]);
    raw->gz   = (int16_t)((b[12] << 8) | b[13]);

    return 1;
}

void MPU6050_PrintData(void)
{
    MPU6050_Raw_t raw;
		MPU6050_ReadRaw(&raw);
    USART1_Send_String("AX: ");
    USART1_Send_Number(raw.ax);
    USART1_Send_String("  AY: ");
    USART1_Send_Number(raw.ay);
    USART1_Send_String("  AZ: ");
    USART1_Send_Number(raw.az);
    USART1_Send_String("\r\n");

    USART1_Send_String("GX: ");
    USART1_Send_Number(raw.gx);
    USART1_Send_String("  GY: ");
    USART1_Send_Number(raw.gy);
    USART1_Send_String("  GZ: ");
    USART1_Send_Number(raw.gz);
    USART1_Send_String("\r\n");

    USART1_Send_String("TEMP: ");
    USART1_Send_Number(raw.temp);
    USART1_Send_String("\r\n\r\n");
}