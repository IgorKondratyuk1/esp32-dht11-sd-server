#pragma once
#include <vector>
#include <string>

using namespace std;

vector<string> getFilesList();
void readValuesFromCsvFile();
void writeValuesToCsvFile(const char *time, float temperatureValue, float humidityValue);
void initializeNewCsvFile(char *fileName);
void initializeRootFolder(const char *folderName);
void configSD(int sdCsPin);