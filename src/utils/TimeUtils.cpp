#include <Arduino.h>
#include <time.h>
#include "TimeUtils.h"

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 2 * 3600;   // GMT+2
const int daylightOffset_sec = 3600;  // +1 година для літнього часу (можна 0)

void configLocalNtp() {
  Serial.println("NTP configuration...");
  // Config ESP32 RTC with NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Timeout for time setup. Not necessary but recommended
  delay(2000);

  // Отримання часу
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Ntp error. Failed to obtain time");
    delay(1000);
    configLocalNtp();
  }
  Serial.println(&timeinfo, "Date: %d-%m-%Y  Time: %H:%M:%S");
  Serial.println("NTP configured!");
}

// Return date "DD-MM-YYYY"
const char* getDateString() {
  static char buffer[20]; // статичний буфер
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) return NULL;

  strftime(buffer, sizeof(buffer), "%d-%m-%Y", &timeinfo);
  return buffer;
}

// Return time "HH:MM:SS"
const char* getTimeString() {
  static char buffer[9];
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)) return NULL;

  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);
  return buffer;
}