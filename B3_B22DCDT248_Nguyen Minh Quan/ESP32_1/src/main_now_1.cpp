#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <string.h>

uint8_t peerMAC[] = {0x10, 0x06, 0x1C, 0x85, 0xA6, 0x30};

#define LED_PIN 2
#define BH1750_ADDR 0x23
#define TEMP_THRESHOLD 24.0f

typedef struct
{
    char text[250];
} Data;

Data sendData;
Data recvData;

volatile bool got_reply = false;

void BH1750_Init()
{
    Wire.begin();
    Wire.beginTransmission(BH1750_ADDR);
    Wire.write(0x01);
    Wire.endTransmission();
    Wire.beginTransmission(BH1750_ADDR);
    Wire.write(0x10);
    Wire.endTransmission();
    delay(180);
}

uint16_t BH1750_ReadLux()
{
    uint16_t lux = 0;
    Wire.requestFrom(BH1750_ADDR, 2);
    if (Wire.available() >= 2)
    {
        lux = (uint16_t)Wire.read() << 8;
        lux |= Wire.read();
        lux = (uint16_t)(lux / 1.2f);
    }
    return lux;
}

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&recvData, incomingData, sizeof(recvData));
    Serial.println(recvData.text);

    const char *p = strstr(recvData.text, "T:");
    if (p)
    {
        float temp = atof(p + 2);
        if (temp > TEMP_THRESHOLD)
        {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("Nhiet do > 24C -> BAT LED D2");
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
            Serial.println("Nhiet do <= 24C -> TAT LED D2");
        }
    }

    got_reply = true;
}

void onSent(const uint8_t *mac, esp_now_send_status_t status)
{
    Serial.print("Trang thai gui:");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    BH1750_Init();

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK)
    {
        Serial.println("Loi ket noi");
        return;
    }

    esp_now_register_recv_cb(onReceive);
    esp_now_register_send_cb(onSent);

    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    peerInfo.ifidx = WIFI_IF_STA;

    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
        Serial.println("Loi ket noi");
        return;
    }
}

void loop()
{
    uint16_t lux = BH1750_ReadLux();
    Serial.print("Lux = ");
    Serial.println(lux);

    memset(sendData.text, 0, sizeof(sendData.text));
    snprintf(sendData.text, sizeof(sendData.text),
             "Nguyen Minh Quan - B22DCDT248 - Lux:%d", lux);

    got_reply = false;
    esp_now_send(peerMAC, (uint8_t *)&sendData, sizeof(sendData));
    Serial.println(sendData.text);

    uint32_t t = millis();
    while (!got_reply && (millis() - t) < 1000)
        delay(10);

    if (!got_reply)
        Serial.println("Khong nhan duoc");
    Serial.println();
    delay(2000);
}
