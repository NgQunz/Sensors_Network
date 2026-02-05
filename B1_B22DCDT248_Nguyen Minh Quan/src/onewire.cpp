#include <Arduino.h>
#include "DHT.h"

DHT Dht11(16, DHT11);

void setup()
{
  Serial.begin(9600);
  Dht11.begin();
}

void loop()
{
  float do_am = Dht11.readHumidity();
  float nhiet_do = Dht11.readTemperature();
  if (isnan(do_am) || isnan(nhiet_do))
  {
    Serial.println("Error");
    delay(2000);
    return;
  }
  Serial.print("Nhiet do: ");
  Serial.print(nhiet_do);
  Serial.print(" °C | Do am: ");
  Serial.print(do_am);
  Serial.println(" %");
  delay(2000);
}
