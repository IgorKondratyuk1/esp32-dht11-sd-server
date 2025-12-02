#include <Arduino.h>
#include <SD.h>
#include <time.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <DHT.h>
#include "WebServer.h"
#include "SdStorageUtils.h"
#include "TimeUtils.h"
#include "DHT11Utils.h"
#include "WifiUtils.h"

using namespace std;

#define SD_CS_PIN 5
#define DHT_PIN 21
#define DHT_TYPE DHT11

const char* rootFolderName = "/dht11-values";
const char* fileNamePattern = "/%s_dht11-values.csv";
char resultsFileName[50];
char todayDate[10];

void configLittleFS();

AsyncWebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

void TaskMain(void *parameter) {
  for(;;) {
    Serial.println("TaskMain start");

    float tempTemperature, tempHumidity;
    setValuesFromDHT11(&tempTemperature, &tempHumidity);
    writeValuesToCsvFile(getTimeString(), tempTemperature, tempHumidity);
    readValuesFromCsvFile(); // Test print

    vTaskDelay(20000 / portTICK_PERIOD_MS);  // Every 20 seconds
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Started");

  configLittleFS();
  connectToWiFi();
  configLocalNtp();
  configSD(SD_CS_PIN);
  configDHT11Module();

  strcpy(todayDate, getDateString());
  Serial.printf("Today date: %s\n", todayDate);

  char temp[100];
  sprintf(resultsFileName, fileNamePattern, todayDate);
  Serial.printf("File name 1: %s\n", resultsFileName);
  snprintf(temp, sizeof(temp), "%s%s", rootFolderName, resultsFileName);
  strcpy(resultsFileName, temp);
  Serial.printf("File name 2: %s\n", resultsFileName);

  bool isRootFolderExist = SD.exists(rootFolderName);
  Serial.printf("Is root folder exist: %d\n", isRootFolderExist);
  if (!isRootFolderExist) {
    initializeRootFolder(rootFolderName);
  }

  bool isFileExist = SD.exists(resultsFileName);
  Serial.printf("Is results file exist: %d\n", isFileExist);
  if (!isFileExist) {
    initializeNewCsvFile(resultsFileName);
  }

  setupServer();

  xTaskCreatePinnedToCore(
    TaskMain,           // функція задачі
    "TaskMain",         // ім'я задачі
    16384,              // стек (байти)
    NULL,               // параметри задачі
    1,                  // пріоритет
    NULL,               // хендл (не обов’язково)
    1                   // ядро ESP32 (0 або 1)
  );
}

void loop() {}

void configLittleFS() {
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    while(1);
  }
  Serial.println("LittleFS mounted successfully");
}