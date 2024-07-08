#include "test.h"

uint8_t randomNumber(uint8_t avg, uint8_t var) {
    uint8_t sign = rand() % 2;  // 0 = minus, 1 = plus
    uint8_t variation = rand() % var;
    return avg + (sign ? -variation : variation);
}

void createSensPacket(uint8_t * data, uint8_t index) {
    uint8_t t_avg = mock_temp[index][0];
    uint8_t t_var = mock_temp[index][1];
    uint8_t h_avg = mock_humi[index][0];
    uint8_t h_var = mock_humi[index][1];
    *data       = START_SYMB;
    *(data + 1) = 5;
    *(data + 2) = 0xA0 + index;
    *(data + 3) = randomNumber(t_avg, t_var);
    *(data + 4) = randomNumber(h_avg, h_var);
    *(data + 5) = Check_nby(4, data+1);
}