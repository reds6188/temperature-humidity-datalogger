#ifndef TIMER_H_
#define TIMER_H_

#include <Arduino.h>

typedef enum {
	T_1ms	= 1,
	T_10ms	= 10,
	T_100ms	= 100,
	T_1s	= 1000,
	T_10s	= 10000,
	T_1m	= 60000,
	T_1h	= 3600000
} time_unit_t;

class Timer {
	private:
		volatile unsigned long timer_ms;
	public:
		Timer(void);
		~Timer();
		void trigger(void);
		unsigned long getCurrentTime(void);
		unsigned long getTriggerTime(void);
		bool elapsedX1ms(unsigned long time_ms);
		bool elapsedX10ms(unsigned long time_10ms);
		bool elapsedX100ms(unsigned long time_100ms);
		bool elapsedX1s(unsigned long time_s);
		bool elapsedX10s(unsigned long time_10s);
		bool elapsedX1m(unsigned long time_m);
		bool elapsedX1h(unsigned long time_h);
		bool elapsedXtu(unsigned long time, time_unit_t time_unit);
		String getTimeStamp(void);
};

#endif  /* TIMER_H_ */