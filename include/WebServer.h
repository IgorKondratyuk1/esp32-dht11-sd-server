#pragma once
#include <ESPAsyncWebServer.h>

void setupServer();

void handleRoot(AsyncWebServerRequest *request);

void handleJS(AsyncWebServerRequest *request);

void handleCSS(AsyncWebServerRequest *request);

void handleFilesList(AsyncWebServerRequest *request);

void handleCsvFileByName(AsyncWebServerRequest *request);