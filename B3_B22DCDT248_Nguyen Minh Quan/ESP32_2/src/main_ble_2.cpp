// // server - nhap chuoi tu ban phim
// #include "BluetoothSerial.h"

// BluetoothSerial SerialBT;

// void setup()
// {
//     Serial.begin(115200);
//     SerialBT.begin("ESP32_SERVER");
//     Serial.println("Bluetooth Server Ready");
//     Serial.println("Nhap chuoi va nhan Enter de gui...");
// }

// void loop()
// {
//     // Nhan tu ESP1
//     if (SerialBT.available())
//     {
//         String msg = SerialBT.readStringUntil('\n');
//         msg.trim();
//         Serial.print("ESP2 Received: ");
//         Serial.println(msg);
//     }

//     // Gui tu ban phim
//     if (Serial.available())
//     {
//         String input = Serial.readStringUntil('\n');
//         input.trim();
//         if (input.length() > 0)
//         {
//             SerialBT.println(input);
//             Serial.print("ESP2 Send: ");
//             Serial.println(input);
//         }
//     }
// }