#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.println("Wi-Fi connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.printf("IP: %s\n", WiFi.localIP().toString());
}