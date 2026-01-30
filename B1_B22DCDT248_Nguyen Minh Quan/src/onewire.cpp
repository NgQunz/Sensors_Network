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
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  if (isnan(humi) || isnan(temp))
  {
    Serial.println("Error");
    delay(2000);
    return;
  }
  Serial.print("Nhiet do: ");
  Serial.print(temp);
  Serial.print(" °C | Do am: ");
  Serial.print(humi);
  Serial.println(" %");
  delay(2000);
}
