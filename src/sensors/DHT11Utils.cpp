#include <Arduino.h>
#include <DHT.h>

extern DHT dht;

void configDHT11Module() {
  Serial.println("DHT configuration...");
  dht.begin();
  // Timeout for DHT11 setup. Not necessary but recommended
  delay(2000);
  Serial.println("DHT configured!");
}

void setValuesFromDHT11(float *temperatureValue, float *humidityValue) {
  *temperatureValue = dht.readTemperature();
  *humidityValue = dht.readHumidity();
}