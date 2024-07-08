#ifndef TEST_H_
#define TEST_H_

#include <Arduino.h>
#include <protocol.h>

static const uint8_t mock_temp[4][2] = {
    { 80, 2 },
    { 65, 2 },
    { 92, 2 },
    { 78, 2 }
};

static const uint8_t mock_humi[4][2] = {
    { 50, 3 },
    { 88, 2 },
    { 70, 2 },
    { 96, 3 }
};

uint8_t randomNumber(uint8_t avg, uint8_t var);
void createSensPacket(uint8_t * data, uint8_t index);

#endif  /* TEST_H_ */