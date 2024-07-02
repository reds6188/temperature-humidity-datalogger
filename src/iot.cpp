#include "iot.h"
#include "credentials.h"

WiFiHandler wifi_handler;
Things5		Thing(false);

bool restartFlag = true;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    uint8_t reason;
	switch (event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
			//ntp_client.begin();
			//initModbus();
			console.success(WIFI_T, "EVENT - Wi-Fi connection was established with network \"" + wifi_handler.getSSID() + "\"");
            startMqttClient();
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			reason = info.wifi_sta_disconnected.reason;
			console.warning(WIFI_T, "EVENT - Wi-Fi was disconnected, reason code: " + String(reason) + " (" + String(WiFi.disconnectReasonName((wifi_err_reason_t)reason)) + ")");
            stopMqttClient();
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
			console.success(WIFI_T, "EVENT - Wi-Fi ready, waiting for connection...");
            break;
		case ARDUINO_EVENT_WIFI_STA_GOT_IP:
			console.success(WIFI_T, "EVENT - Local IP address: " + wifi_handler.getLocalIP());
			break;
		case ARDUINO_EVENT_WIFI_SCAN_DONE:
			console.success(WIFI_T, "EVENT - Wi-Fi scan completed");
			wifi_handler.setScanDone();
			wifi_handler.readNetworkList();
			//wifi_scan_completed();
			break;
        default:
			console.warning(WIFI_T, "Unknown Wi-Fi event");
			break;
    }	
}

void initVarT5(void)
{
	Thing.defMetric(T5_S1_TEMP, METRICS_INT);
	Thing.defMetric(T5_S1_HUMI, METRICS_INT);
	Thing.defMetric(T5_S2_TEMP, METRICS_INT);
	Thing.defMetric(T5_S2_HUMI, METRICS_INT);
	Thing.defMetric(T5_S3_TEMP, METRICS_INT);
	Thing.defMetric(T5_S3_HUMI, METRICS_INT);
	Thing.defMetric(T5_S4_TEMP, METRICS_INT);
	Thing.defMetric(T5_S4_HUMI, METRICS_INT);
	//Thing.printVarList();
}

void mqtt_connection_callback(void)
{
	if(restartFlag) {
		restartFlag = false;

		// Send Firmware Version ----------------------------------------------
		Thing.setUUID();
		Thing.setProperty("version", String(FW_VERSION));
		if(publishMqtt(FIRMWARE_VERSION_REQ, Thing.getPayload()))
			console.success(MQTT_T, "Published on topic \"" + String(FIRMWARE_VERSION_REQ) + "\"");
		else
			console.error(MQTT_T, "Failed to publish on topic \"" + String(FIRMWARE_VERSION_REQ) + "\"");

		// Send event "Info Gateway Restart" and all data ---------------------
		Thing.createMessage();
		Thing.addEvent(T5_INFO_RESTART, getResetReason());
		Thing.writeAllMetrics();
		Thing.writeAllStates();
		if(publishMqtt(DATA_INGESTION_REQ, Thing.getPayload()))
			console.success(MQTT_T, "Published on topic \"" + String(DATA_INGESTION_REQ) + "\"");
		else
			console.error(MQTT_T, "Failed to publish on topic \"" + String(DATA_INGESTION_REQ) + "\"");
		console.info(MQTT_T, "Gateway was restarted");
		printResetReason();
	}
}

void mqtt_data_callback(char * topic, char * data, int data_length)
{
}

void setupCloud(void) {
    initMqttClient();
	onMqttConnect(mqtt_connection_callback);
	onMqttData(mqtt_data_callback);
    wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_CONNECTED);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_START);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
	wifi_handler.setCredentials(test_wifi_ssid, test_wifi_password, true);
	wifi_handler.begin(WIFI_STA);
	wifi_handler.printMacAddress(WIFI_IF_STA);
	wifi_handler.printMacAddress(WIFI_IF_AP);
}