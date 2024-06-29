#include "main.h"

Led	Led1(LED_1, HS_DRIVER);		// LED1 - Lampeggia ad ogni comando ricevuto dal Display
Led	Led2(LED_2, HS_DRIVER);		// LED2 - Lampeggia ad ogni comando ricevuto dal Display
Led	Led3(LED_3, HS_DRIVER);		// LED3 - Lampeggia ad ogni comando ricevuto dall'LPC
Button Btn1(BUTTON_1, 80);
Button Btn2(BUTTON_2, 80);

Timer TimerIdle;

void setup() {
	Led1.set();
	Led2.set();
	Led3.set();
	console.header("START INITIALIZATION", DOUBLE_DASHED, 80);
	printDeviceInfo();
	printResetReason();
	Btn1.onPress(softwareReset);
	Btn2.onPress(printDeviceInfo);
	console.header("END INITIALIZATION", DOUBLE_DASHED, 80);
	Led1.reset();
	Led2.reset();
	Led3.reset();
}

void loop() {
	Btn1.loop();
	Btn2.loop();

	if(TimerIdle.elapsedX100ms(2)) {
		TimerIdle.trigger();
		Led1.toggle();
	}
}