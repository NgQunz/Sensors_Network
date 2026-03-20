// #include <Arduino.h>
// #include <Wire.h>
// #include <SPI.h>
// #include <Adafruit_MPU6050.h>
// #include <Adafruit_Sensor.h>
// #include <MFRC522.h>
// #include "DHT.h"
// Adafruit_MPU6050 mpu;
// #define DHTPIN 4
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);
// #define SS_PIN 5
// #define RST_PIN 16
// MFRC522 rfid(SS_PIN, RST_PIN);

// void setup()
// {
//     Serial.begin(9600);
//     Wire.begin(21, 22);
//     if (!mpu.begin())
//     {
//         Serial.println("MPU6050 not found!");
//         while (1)
//             delay(10);
//     }
//     Serial.println("MPU6050 ready!");
//     mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
//     mpu.setGyroRange(MPU6050_RANGE_500_DEG);
//     mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

//     dht.begin();
//     Serial.println("DHT22 ready!");

//     SPI.begin(18, 19, 23, 5);
//     rfid.PCD_Init();
//     Serial.println("RFID ready");
// }

// void loop()
// {
//     sensors_event_t a, g, temp;
//     mpu.getEvent(&a, &g, &temp);

//     Serial.print("[MPU] Accel: ");
//     Serial.print(a.acceleration.x);
//     Serial.print(", ");
//     Serial.print(a.acceleration.y);
//     Serial.print(", ");
//     Serial.println(a.acceleration.z);

//     Serial.print("[MPU] Gyro: ");
//     Serial.print(g.gyro.x);
//     Serial.print(", ");
//     Serial.print(g.gyro.y);
//     Serial.print(", ");
//     Serial.println(g.gyro.z);
//     float humi = dht.readHumidity();
//     float temp_dht = dht.readTemperature();

//     if (!isnan(humi) && !isnan(temp_dht))
//     {
//         Serial.print("[DHT] Temp: ");
//         Serial.print(temp_dht);
//         Serial.print(" C | Humi: ");
//         Serial.print(humi);
//         Serial.println(" %");
//     }
//     else
//     {
//         Serial.println("[DHT] Read error");
//     }

//     if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial())
//     {
//         Serial.print("[RFID] UID: ");
//         for (byte i = 0; i < rfid.uid.size; i++)
//         {
//             if (rfid.uid.uidByte[i] < 0x10)
//                 Serial.print("0");
//             Serial.print(rfid.uid.uidByte[i], HEX);
//             Serial.print(" ");
//         }
//         Serial.println();
//         rfid.PICC_HaltA();
//     }

//     Serial.println("");
//     delay(1000);
// }
