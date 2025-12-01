#pragma once

extern const char* ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;

void configLocalNtp();
const char* getDateString();
const char* getTimeString();