#include <vector>
#include <SD.h>

using namespace std;

extern const char *rootFolderName;
extern char resultsFileName[50];

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

void configSD(int sdCsPin) {
  Serial.println("SD configuration...");
  if (!SD.begin(sdCsPin)) {
    Serial.println("SD initialization error");
    while(1);
  }
  Serial.println("SD configured!");
}