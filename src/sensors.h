#ifndef SENSORS_H_
#define SENSORS_H_

#include <Arduino.h>
#include <console.h>
#include <serial.h>
#include <protocol.h>
#include <timer.h>
#include "def.h"

#define SENS_T              "SENS"

#define SENSORS_NUM_MAX     9
#define SENSOR_BASE_ADDR    0xA0

typedef struct {
    uint8_t address;
    uint8_t temperature;
    uint8_t humidity;
}sensor_t;

class TempUmidSensor {
    private:
        sensor_t sens_list[SENSORS_NUM_MAX];
        uint8_t sens_num;
        uint8_t sens_index;
        uint8_t txData[32];
		uint8_t rxData[32];
        bool scanning;
		bool receivedFlag;
        bool comError;
        Timer TimerRefresh, TimerCom;
    public:
        TempUmidSensor();
        ~TempUmidSensor(void);
        void begin(void(*callback)(uint8_t *data, int size));
        void scan(void);
        void setRxBuffer(uint8_t *data, int size);
        void parseRxBuffer(void);
        void refresh(void);
        void sendRequest(uint8_t address);
        void next(void);
        void loop(void);
        bool isAddress(uint8_t value);
        sensor_t getSensor(uint8_t address);
        uint8_t getIndex(uint8_t address);
        void setTemperature(uint8_t index, uint8_t value);
        uint8_t getTemperature(uint8_t index);
        void setHumidity(uint8_t index, uint8_t value);
        uint8_t getHumidity(uint8_t index);
};

#endif  /* SENSORS_H_ */