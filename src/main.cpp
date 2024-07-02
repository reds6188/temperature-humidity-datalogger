#include "main.h"

Led	Led1(LED_1, HS_DRIVER);		// LED1 - Lampeggia ad ogni comando ricevuto dal Display
Led	Led2(LED_2, HS_DRIVER);		// LED2 - Lampeggia ad ogni comando ricevuto dal Display
Led	Led3(LED_3, HS_DRIVER);		// LED3 - Lampeggia ad ogni comando ricevuto dall'LPC
Button Btn1(BUTTON_1, 80);
Button Btn2(BUTTON_2, 80);
TempUmidSensor sensor;

Timer TimerIdle, TimerCloud;

void sensor_callback(uint8_t *data, int size) {
	sensor.setRxBuffer(data, size);
}

void setup() {
	Led1.set();
	Led2.set();
	Led3.set();
	console.header("START INITIALIZATION", DOUBLE_DASHED, 80);
	printDeviceInfo();
	printResetReason();
	setupCloud();
	sensor.begin(sensor_callback);
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
	sensor.loop();

	if(TimerIdle.elapsedX100ms(2)) {
		TimerIdle.trigger();
		Led1.toggle();
	}

	if(TimerCloud.elapsedX1s(1)) {
		TimerCloud.trigger();
		Thing.createMessage();
		// Update Metrics ---------------------------------------------------------------------
		Thing.updateMetric(T5_S1_TEMP, sensor.getTemperature(1));
		Thing.updateMetric(T5_S1_HUMI, sensor.getHumidity(1));
		Thing.updateMetric(T5_S2_TEMP, sensor.getTemperature(2));
		Thing.updateMetric(T5_S2_HUMI, sensor.getHumidity(2));
		Thing.updateMetric(T5_S3_TEMP, sensor.getTemperature(3));
		Thing.updateMetric(T5_S3_HUMI, sensor.getHumidity(3));
		Thing.updateMetric(T5_S4_TEMP, sensor.getTemperature(4));
		Thing.updateMetric(T5_S4_HUMI, sensor.getHumidity(4));

		if(Thing.emptyMessage())
			Thing.deleteMessage();
		else {
			if(publishMqtt(DATA_INGESTION_REQ, Thing.getPayload()))
				console.success(MQTT_T, "Published on topic \"" + String(DATA_INGESTION_REQ) + "\"");
			else
				console.error(MQTT_T, "Failed to publish on topic \"" + String(DATA_INGESTION_REQ) + "\"");
		}
	}
}