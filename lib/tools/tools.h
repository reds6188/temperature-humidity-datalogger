#ifndef TOOLS_H_
#define TOOLS_H_

#include <Arduino.h>
#include "SPIFFS.h"
#include <console.h>

#define ESP32_T     "ESP32"
#define FILE_T      "FILE"
#define MISC_T      "MISC"

String byteToHex(uint8_t data);
String wordToHex(uint16_t data);
String DataToHexString(uint8_t *data, int data_size);
void softwareReset(void);
void printDeviceInfo(void);
void printPartitionsInfo(void);
String getResetReason(void);
void printResetReason(void);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
char* getFileAsString(fs::FS &fs, String path);

#endif  /* TOOLS_H_ */