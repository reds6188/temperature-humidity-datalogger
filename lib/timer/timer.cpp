#include "timer.h"

Timer::Timer(void) {

}

Timer::~Timer(void) {

}

void Timer::trigger() {
    timer_ms = millis();
}

unsigned long Timer::getCurrentTime(void) {
    return millis();
}

unsigned long Timer::getTriggerTime(void) {
    return timer_ms;
}

bool Timer::elapsedX1ms(unsigned long time_ms) {
    return (millis() - timer_ms) >= time_ms ? true : false;
}

bool Timer::elapsedX10ms(unsigned long time_10ms) {
    return (millis() - timer_ms) >= (time_10ms * 10) ? true : false;
}

bool Timer::elapsedX100ms(unsigned long time_100ms) {
    return (millis() - timer_ms) >= (time_100ms * 100) ? true : false;
}

bool Timer::elapsedX1s(unsigned long time_s) {
    return (millis() - timer_ms) >= (time_s * 1000) ? true : false;
}

bool Timer::elapsedX10s(unsigned long time_10s) {
	unsigned long current = millis();
	unsigned long elapsed_time;
	if(current > timer_ms)
		elapsed_time = current - timer_ms;
	else
		elapsed_time = timer_ms - current;
    return elapsed_time >= (time_10s * 10000) ? true : false;
}

bool Timer::elapsedX1m(unsigned long time_m) {
    return (millis() - timer_ms) >= (time_m * 60000) ? true : false;
}

bool Timer::elapsedX1h(unsigned long time_h) {
    return (millis() - timer_ms) >= (time_h * 3600000) ? true : false;
}

bool Timer::elapsedXtu(unsigned long time, time_unit_t time_unit) {
    return (millis() - timer_ms) >= (time * time_unit) ? true : false;
}

String Timer::getTimeStamp(void) {
    unsigned long tmp = millis();
    unsigned long time_unit[3] = { T_1h, T_1m, T_1s };
    uint8_t time[4];

    for(uint8_t i=0 ; i<3 ; i++) {
        time[i] = tmp / time_unit[i];
        tmp -= time[i] * time_unit[i];
    }
    time[3] = tmp;

    return String(time[0]) + ":" +String(time[1]) + ":" +String(time[2]) + "." +String(time[3]);
}
