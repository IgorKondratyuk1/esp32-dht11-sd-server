#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <SD.h>
#include <DHT.h>
#include <WiFi.h>
#include <time.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "TimeUtils.h"
#include "secrets.h"

using namespace std;

#define SD_CS_PIN 5
#define DHT_PIN 21
#define DHT_TYPE DHT11

const char* rootFolderName = "/dht11-values";
const char* fileNamePattern = "/%s_dht11-values.csv";
char resultsFileName[50];
char todayDate[10];

AsyncWebServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

void setValuesFromDHT11(float *temperatureValue, float *humidityValue);
void writeValuesToCsvFile(const char *time, float temperatureValue, float humidityValue);
void readValuesFromCsvFile();
void initializeNewCsvFile(char *fileName);
void initializeRootFolder(const char *folderName);
void connectToWiFi();
void configSD();
void configDHT11Module();
vector<string> getFilesList();

void handleRoot(AsyncWebServerRequest *request) {
  if (!LittleFS.exists("/index.html")) {
    request->send(500, "text/plain", "Failed to open file");
    return;
  }
  request->send(LittleFS, "/index.html", "text/html");
}

void handleJS(AsyncWebServerRequest *request) {
  if (!LittleFS.exists("/script.js")) {
    request->send(500, "text/plain", "Failed to open JS file");
    return;
  }
  request->send(LittleFS, "/script.js", "application/javascript");
}

void handleCSS(AsyncWebServerRequest *request) {
  if (!LittleFS.exists("/style.css")) {
    request->send(500, "text/plain", "Failed to open CSS file");
    return;
  }
  request->send(LittleFS, "/style.css", "text/css");
}

void handleFilesList(AsyncWebServerRequest *request) {
  JsonDocument doc;
  JsonArray arr = doc["items"].to<JsonArray>();

  vector<string> files = getFilesList();

  for (int i = 0; i < files.size(); i++) {
    arr.add(files.at(i));
  }

  String json;
  serializeJson(doc, json);
  request->send(200, "application/json", json);
}

void handleCsvFileByName(AsyncWebServerRequest *request) {
  string fileName;
  if (request->hasParam("fileName")) {
    fileName = request->getParam("fileName")->value().c_str();
    Serial.printf("fileName: %s\n", fileName.c_str());
  }

  string fullFilePath = string(rootFolderName) + '/' + fileName;
  Serial.printf("fullFilePath: %s\n", fullFilePath.c_str());
  if (!SD.exists(fullFilePath.c_str())) {
    request->send(404, "text/plain", "CSV file not found");
    return;
  }

  request->send(SD, fullFilePath.c_str(), "text/plain");
}

vector<string> getFilesList() {
  vector<string> result = {};

  File espFilesFolder = SD.open(rootFolderName);
  if (!espFilesFolder || !espFilesFolder.isDirectory()) {
    return result;
  }

  File file = espFilesFolder.openNextFile();
  while (file) {
    if (!file.isDirectory()) {
      result.push_back(file.name());
    }
    file = espFilesFolder.openNextFile();
  }

  return result;
}

void setupServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/script.js", HTTP_GET, handleJS);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/list-files", HTTP_GET, handleFilesList);
  server.on("/file", HTTP_GET, handleCsvFileByName);
  server.begin();
  Serial.println("Server started");
}

void TaskMain(void * parameter) {
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

  if (!LittleFS.begin()) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  Serial.println("LittleFS mounted successfully");
  connectToWiFi();
  configLocalNtp();
  

  configSD();
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
    16384,               // стек (байти)
    NULL,               // параметри задачі
    1,                  // пріоритет
    NULL,               // хендл (не обов’язково)
    1                   // ядро ESP32 (0 або 1)
  );
}

void loop() {}

// Configs
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

void configSD() {
  Serial.println("SD configuration...");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD initialization error");
    while(1);
  }
  Serial.println("SD configured!");
}

void configDHT11Module() {
  Serial.println("DHT configuration...");
  dht.begin();
  // Timeout for DHT11 setup. Not necessary but recommended
  delay(2000);
  Serial.println("DHT configured!");
}

void initializeNewCsvFile(char *fileName) {
  File file = SD.open(fileName, FILE_WRITE);
  if (file) {
    file.println("date,temperature,humidity");
    file.close();
  } else {
    Serial.println("File not opened for write");
  }
  Serial.println("New file created");
}

void initializeRootFolder(const char *folderName) {
  SD.mkdir(folderName);
  Serial.println("New folder created");
}

void setValuesFromDHT11(float *temperatureValue, float *humidityValue) {
  *temperatureValue = dht.readTemperature();
  *humidityValue = dht.readHumidity();
}

void writeValuesToCsvFile(const char *time, float temperatureValue, float humidityValue) {
  Serial.printf("Writing to: %s\n", resultsFileName);
  File file = SD.open(resultsFileName, FILE_APPEND);
  if (file) {
    file.printf("%s,%.2f,%.2f\n", time, temperatureValue, humidityValue);
    file.close();
  } else {
    Serial.println("File not opened for write");
  }
  file.close();
}

void readValuesFromCsvFile() {
  File file = SD.open(resultsFileName, FILE_READ);
  if (file) {
    while (file.available()) {
      Serial.write(file.read());
    }
  } else {
    Serial.println("File not opened for read");
  }
  file.close();
}