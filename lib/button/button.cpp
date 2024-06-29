#include "button.h"

void fNULL(void) {}

Button::Button(uint8_t buttonPin, unsigned long debounce) {
    pin = buttonPin;
    debounceDelay = debounce;
    pinMode(pin, INPUT_PULLUP);
    onPressFunc = fNULL;
    onReleaseFunc = fNULL;
}

Button::~Button(void) {
}

void Button::onPress(void (*func)(void)) {
    onPressFunc = func;
}

void Button::onRelease(void (*func)(void)) {
    onReleaseFunc = func;
}

void Button::loop(void) {
    uint8_t reading = digitalRead(pin);

    if(reading != lastState) {
        debounceTimer = millis();
    }

    if((millis() - debounceTimer) > debounceDelay) {
        // if the button state has changed:
        if(reading != currState) {
            currState = reading;

            if(currState == LOW)
                (*onPressFunc)();
            else
                (*onReleaseFunc)();
        }
    }

    lastState = reading;
}