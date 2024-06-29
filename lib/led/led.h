#ifndef LED_H_
#define LED_H_

#include <Arduino.h>

typedef enum {
    LS_DRIVER = 0,      // Low-Side Driver
    HS_DRIVER = 1       // High-Side Driver
} led_driver_t;

class Led {
	private:
        int pin;
        led_driver_t driver_mode;
        unsigned long timer_blink;
        unsigned long time_blink[2];
        bool blink_index;
	public:
		Led(int led_pin, led_driver_t led_driver_mode);
        void set(void);
        void reset(void);
        void toggle(void);
        void setBlink(unsigned long time_on, unsigned long time_off);
        void loop(void);
};

#endif  /* LED_H_ */