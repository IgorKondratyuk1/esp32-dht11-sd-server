#include "WebServer.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <vector>
#include <SD.h>
#include "SdStorageUtils.h"

using namespace std;

extern const char *rootFolderName;
extern AsyncWebServer server;

void setupServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/script.js", HTTP_GET, handleJS);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/list-files", HTTP_GET, handleFilesList);
  server.on("/file", HTTP_GET, handleCsvFileByName);
  server.begin();
  Serial.println("Server started");
}

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