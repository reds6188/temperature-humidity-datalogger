#include "led.h"

Led::Led(int led_pin, led_driver_t led_driver_mode) {
	pin = led_pin;
	driver_mode = led_driver_mode;

	pinMode(pin, OUTPUT);
	digitalWrite(pin, (driver_mode == LS_DRIVER ? LOW : HIGH));
}

void Led::set(void) {
	digitalWrite(pin, (driver_mode == LS_DRIVER ? HIGH : LOW));
}

void Led::reset(void) {
	digitalWrite(pin, (driver_mode == LS_DRIVER ? LOW : HIGH));
}

void Led::toggle(void) {
	digitalWrite(pin, !digitalRead(pin));
}

void Led::setBlink(unsigned long time_on, unsigned long time_off) {
	time_blink[0] = time_on;
	time_blink[1] = time_off;

	blink_index = false;
	set();
}

void Led::loop(void) {
	
	if((millis() - timer_blink) > time_blink[blink_index]) {
		timer_blink = millis();
		blink_index = !blink_index;
		toggle();
	}
}