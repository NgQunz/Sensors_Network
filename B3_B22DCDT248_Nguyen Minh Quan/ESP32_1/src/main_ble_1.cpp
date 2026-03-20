// // client - nhap chuoi tu ban phim
// #include <Arduino.h>
// #include "BluetoothSerial.h"

// BluetoothSerial SerialBT;

// bool connected = false;

// void setup()
// {
//     Serial.begin(115200);
//     Serial.println("Connecting...");

//     SerialBT.begin("ESP32_CLIENT", true); // master
//     delay(2000);

//     if (SerialBT.connect("ESP32_SERVER"))
//     {
//         Serial.println("Connected to server");
//         Serial.println("Nhap chuoi va nhan Enter de gui...");
//         connected = true;
//     }
//     else
//     {
//         Serial.println("Failed to connect");
//     }
// }

// void loop()
// {
//     if (connected)
//     {
//         // Nhan tu ESP2
//         if (SerialBT.available())
//         {
//             String msg = SerialBT.readStringUntil('\n');
//             msg.trim();
//             Serial.print("ESP1 Received: ");
//             Serial.println(msg);
//         }

//         // Gui tu ban phim
//         if (Serial.available())
//         {
//             String input = Serial.readStringUntil('\n');
//             input.trim();
//             if (input.length() > 0)
//             {
//                 SerialBT.println(input);
//                 Serial.print("ESP1 Send: ");
//                 Serial.println(input);
//             }
//         }
//     }
// }