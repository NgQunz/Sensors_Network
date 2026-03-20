#include "mpu9265.h"
#include "tim2.h"

/* ===== MPU regs (MPU-92xx family) ===== */
#define REG_WHO_AM_I        0x75
#define REG_PWR_MGMT_1      0x6B
#define REG_SMPLRT_DIV      0x19
#define REG_CONFIG          0x1A
#define REG_GYRO_CONFIG     0x1B
#define REG_ACCEL_CONFIG    0x1C
#define REG_ACCEL_XOUT_H    0x3B

#define SPI_READ_BIT        0x80

/* ===== CS pin: PA4 (d?i n?u b?n dùng pin khác) ===== */
static inline void CS_LOW(void)  { GPIOA->BRR  = (1U << 4); }
static inline void CS_HIGH(void) { GPIOA->BSRR = (1U << 4); }

/* ===== SPI1 TxRx 1 byte ===== */
static uint8_t SPI1_TxRx(uint8_t data)
{
    while(!(SPI1->SR & SPI_SR_TXE));
    *(volatile uint8_t*)&SPI1->DR = data;
    while(!(SPI1->SR & SPI_SR_RXNE));
    return *(volatile uint8_t*)&SPI1->DR;
}

/* ===== low-level (static, n?i b? file) ===== */
static void mpu_write_u8(uint8_t reg, uint8_t val)
{
    CS_LOW();
    SPI1_TxRx(reg & 0x7F);
    SPI1_TxRx(val);
    CS_HIGH();
}

static uint8_t mpu_read_u8(uint8_t reg)
{
    uint8_t v;
    CS_LOW();
    SPI1_TxRx((reg & 0x7F) | SPI_READ_BIT);
    v = SPI1_TxRx(0xFF);
    CS_HIGH();
    return v;
}

static void mpu_read_buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
    CS_LOW();
    SPI1_TxRx((reg & 0x7F) | SPI_READ_BIT);
    for(uint8_t i=0;i<len;i++) buf[i] = SPI1_TxRx(0xFF);
    CS_HIGH();
}

/* ===== Public APIs ===== */
void MPU9265_SPI1_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_SPI1EN;

    /* PA4 CS output push-pull 50MHz */
    GPIOA->CRL &= ~(0xFU << (4 * 4));
    GPIOA->CRL |=  (0x3U << (4 * 4));
    CS_HIGH();

    /* PA5 SCK AF PP 50MHz */
    GPIOA->CRL &= ~(0xFU << (5 * 4));
    GPIOA->CRL |=  (0xBU << (5 * 4));

    /* PA7 MOSI AF PP 50MHz */
    GPIOA->CRL &= ~(0xFU << (7 * 4));
    GPIOA->CRL |=  (0xBU << (7 * 4));

    /* PA6 MISO input floating */
    GPIOA->CRL &= ~(0xFU << (6 * 4));
    GPIOA->CRL |=  (0x4U << (6 * 4));

    /* SPI1: master, software NSS, start slow, Mode0 */
    SPI1->CR1 = 0;
    SPI1->CR1 |= SPI_CR1_MSTR;
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;  // /64
    SPI1->CR1 |= (SPI_CR1_CPOL | SPI_CR1_CPHA);

    SPI1->CR1 |= SPI_CR1_SPE;

    Delay_ms(10);
}

uint8_t MPU9265_WhoAmI(void)
{
    return mpu_read_u8(REG_WHO_AM_I);
}

int MPU9265_Init(void)
{
    /* reset */
    mpu_write_u8(REG_PWR_MGMT_1, 0x80);
    Delay_ms(100);

    /* wake, select clock */
    mpu_write_u8(REG_PWR_MGMT_1, 0x01);
    Delay_ms(10);

    /* basic config */
    mpu_write_u8(REG_SMPLRT_DIV, 0x07);
    mpu_write_u8(REG_CONFIG,     0x03);
    mpu_write_u8(REG_GYRO_CONFIG,  0x00); // ±250 dps
    mpu_write_u8(REG_ACCEL_CONFIG, 0x00); // ±2g

    /* Check WHO_AM_I just for debug */
    uint8_t who = MPU9265_WhoAmI();
    if (who == 0x00 || who == 0xFF) return -1; // g?n nhu ch?c SPI l?i

    return 0;
}

int MPU9265_ReadRaw(MPU9265_Raw_t *raw)
{
    uint8_t b[14];
    mpu_read_buf(REG_ACCEL_XOUT_H, b, 14);

    raw->ax   = (int16_t)((b[0] << 8) | b[1]);
    raw->ay   = (int16_t)((b[2] << 8) | b[3]);
    raw->az   = (int16_t)((b[4] << 8) | b[5]);
    raw->temp = (int16_t)((b[6] << 8) | b[7]);
    raw->gx   = (int16_t)((b[8] << 8) | b[9]);
    raw->gy   = (int16_t)((b[10] << 8) | b[11]);
    raw->gz   = (int16_t)((b[12] << 8) | b[13]);

    return 0;
}
void MPU9265_Read_Print(void)
{
    MPU9265_Raw_t m;

    MPU9265_ReadRaw(&m);

    USART1_Send_String("AX: ");
    USART1_Send_Number(m.ax);
    USART1_Send_String("  AY: ");
    USART1_Send_Number(m.ay);
    USART1_Send_String("  AZ: ");
    USART1_Send_Number(m.az);
    USART1_Send_String("\r\n");

    USART1_Send_String("GX: ");
    USART1_Send_Number(m.gx);
    USART1_Send_String("  GY: ");
    USART1_Send_Number(m.gy);
    USART1_Send_String("  GZ: ");
    USART1_Send_Number(m.gz);
    USART1_Send_String("\r\n");

    USART1_Send_String("TEMP: ");
    USART1_Send_Number(m.temp);
    USART1_Send_String("\r\n\r\n");
}
