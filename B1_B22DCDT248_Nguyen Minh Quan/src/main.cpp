#include <Arduino.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println("Doc DHT11 that bai");
    delay(2000);
    return;
  }
  Serial.print("Nhiet do: ");
  Serial.print(t);
  Serial.print(" °C  |  Do am: ");
  Serial.print(h);
  Serial.println(" %");
  delay(2000);
}
