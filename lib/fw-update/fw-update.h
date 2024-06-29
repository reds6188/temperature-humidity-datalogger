#ifndef	FW_UPDATE_H_
#define FW_UPDATE_H_

#include <Arduino.h>
#include <HTTPClient.h>
#include <Update.h>
#include <console.h>

#define UPDATE_T    "UPDATE"    // Tag for log

void startFirmwareUpdate(const char* url, String uuid);
void onUpdateLoop(void(*callback)(void));
void onUpdateProgress(void(*callback)(String uuid, uint8_t progress));
void onUpdateEnd(void(*callback)(String uuid, bool success));
bool isFwUpdating(void);
bool handleFirmwareUpdate(void);
bool updateFirmware(const char* url, String uuid, void(*cbProgress)(String uuid, uint8_t progress), void(*cbSuccess)(String uuid, bool success));

#endif  /* FW_UPDATE_H_ */