// #include "painlessMesh.h"

// #define MESH_PREFIX "ESP_MESH"
// #define MESH_PASSWORD "12345678"
// #define MESH_PORT 5555

// painlessMesh mesh;

// void receivedCallback(uint32_t from, String &msg)
// {
//   Serial.printf("Received from %u: %s\n", from, msg.c_str());
// }

// void setup()
// {
//   Serial.begin(115200);

//   mesh.setDebugMsgTypes(ERROR | STARTUP);
//   mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
//   mesh.onReceive(&receivedCallback);

//   Serial.println("MESH STARTED");
//   Serial.println("Nhap chuoi va nhan Enter de gui...");
// }

// void loop()
// {
//   mesh.update();

//   if (Serial.available())
//   {
//     String input = Serial.readStringUntil('\n');
//     input.trim();
//     if (input.length() > 0)
//     {
//       mesh.sendBroadcast(input);
//       Serial.print("ESP1 Send: ");
//       Serial.println(input);
//     }
//   }
// }