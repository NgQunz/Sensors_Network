#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>

uint8_t peerMAC[] = {0x78, 0x1C, 0x3C, 0xF5, 0x89, 0xC8};

#define LED_PIN 2
#define DHT_PIN 4
#define DHT_TYPE DHT11
#define LUX_THRESHOLD 100

DHT dht(DHT_PIN, DHT_TYPE);

typedef struct
{
    char text[250];
} Data;

Data sendData;
Data recvData;

void onReceive(const uint8_t *mac, const uint8_t *incomingData, int len)
{
    memcpy(&recvData, incomingData, sizeof(recvData));
    Serial.println(recvData.text);

    const char *p = strstr(recvData.text, "Lux:");
    if (p)
    {
        int32_t lux = atoi(p + 4);
        Serial.print("Lux: ");
        Serial.println(lux);

        if (lux < LUX_THRESHOLD)
        {
            digitalWrite(LED_PIN, HIGH);
            Serial.println("Lux < 100 -> BAT LED2");
        }
        else
        {
            digitalWrite(LED_PIN, LOW);
            Serial.println("Lux >= 100 -> TAT LED2");
        }
    }
}

void onSent(const uint8_t *mac, esp_now_send_status_t status)
{
    Serial.print("Trang thai gui: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    dht.begin();

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
    int temp = (int)dht.readTemperature();
    int hum = (int)dht.readHumidity();

    if (isnan(temp) || isnan(hum))
    {
        Serial.println("DHT11 doc that bai");
        delay(2000);
        return;
    }

    Serial.print("T=");
    Serial.print(temp);
    Serial.print("C H=");
    Serial.print(hum);
    Serial.println("%");

    memset(sendData.text, 0, sizeof(sendData.text));
    snprintf(sendData.text, sizeof(sendData.text),
             "Phan hoi tu Node 2 ve Node 1: T:%dC H:%d%%", temp, hum);

    esp_now_send(peerMAC, (uint8_t *)&sendData, sizeof(sendData));
    Serial.println(sendData.text);
    Serial.println();
    delay(2000);
}