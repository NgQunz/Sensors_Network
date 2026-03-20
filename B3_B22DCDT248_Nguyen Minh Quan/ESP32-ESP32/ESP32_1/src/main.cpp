// esp32_main.ino
#include <Arduino.h>

#define STM32_RX 16
#define STM32_TX 17
#define BAUD 115200

#define START_BYTE 0xAA
#define END_BYTE 0x55
#define CMD_TEXT 0x01
#define CMD_ACK 0x02

HardwareSerial STM32(2);

struct Message_t
{
  uint8_t start, cmd, len;
  uint8_t data[64];
  uint8_t checksum, end;
};

uint8_t calcCS(Message_t &m)
{
  uint8_t cs = m.cmd ^ m.len;
  for (int i = 0; i < m.len; i++)
    cs ^= m.data[i];
  return cs;
}

void sendMsg(uint8_t cmd, uint8_t *data, uint8_t len)
{
  Message_t m;
  m.start = START_BYTE;
  m.cmd = cmd;
  m.len = len;
  memcpy(m.data, data, len);
  m.checksum = calcCS(m);
  m.end = END_BYTE;

  STM32.write(m.start);
  STM32.write(m.cmd);
  STM32.write(m.len);
  STM32.write(m.data, m.len);
  STM32.write(m.checksum);
  STM32.write(m.end);
}

static uint8_t rx_buf[128];
static uint8_t rx_idx = 0;

bool recvMsg(Message_t &m)
{
  while (STM32.available())
  {
    uint8_t b = STM32.read();
    if (rx_idx == 0 && b != START_BYTE)
      continue;
    rx_buf[rx_idx++] = b;

    if (rx_idx >= 3)
    {
      uint8_t total = 3 + rx_buf[2] + 2;
      if (rx_idx == total)
      {
        m.start = rx_buf[0];
        m.cmd = rx_buf[1];
        m.len = rx_buf[2];
        memcpy(m.data, &rx_buf[3], m.len);
        m.checksum = rx_buf[3 + m.len];
        m.end = rx_buf[3 + m.len + 1];
        rx_idx = 0;
        return (m.end == END_BYTE && m.checksum == calcCS(m));
      }
    }
    if (rx_idx >= 128)
      rx_idx = 0;
  }
  return false;
}

void setup()
{
  Serial.begin(9600); // debug USB ESP32
  STM32.begin(BAUD, SERIAL_8N1, STM32_RX, STM32_TX);
  Serial.println("ESP32 ready. Nhap ban tin gui STM32:");
}

void loop()
{
  // ── Nhận từ STM32 ──
  Message_t m;
  if (recvMsg(m))
  {
    m.data[m.len] = '\0';
    if (m.cmd == CMD_TEXT)
    {
      Serial.printf("[STM32->ESP32] %s\n", (char *)m.data);
      // Gửi ACK
      uint8_t ack = 0x01;
      sendMsg(CMD_ACK, &ack, 1);
      Serial.println("[ESP32] Da gui ACK");
    }
    else if (m.cmd == CMD_ACK)
    {
      Serial.println("[ESP32] Nhan ACK tu STM32");
    }
  }

  // ── Nhập tay từ Serial Monitor ESP32, gửi tới STM32 ──
  if (Serial.available())
  {
    String s = Serial.readStringUntil('\n');
    s.trim();
    if (s.length() > 0)
    {
      Serial.printf("[ESP32->STM32] %s\n", s.c_str());
      sendMsg(CMD_TEXT, (uint8_t *)s.c_str(), s.length());
    }
  }
}