#include "iot.h"
#include "credentials.h"

WiFiHandler wifi_handler;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
    uint8_t reason;
	switch (event) {
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
			//ntp_client.begin();
			//initModbus();
			console.success(WIFI_T, "EVENT - Wi-Fi connection was established with network \"" + wifi_handler.getSSID() + "\"");
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
			reason = info.wifi_sta_disconnected.reason;
			console.warning(WIFI_T, "EVENT - Wi-Fi was disconnected, reason code: " + String(reason) + " (" + String(WiFi.disconnectReasonName((wifi_err_reason_t)reason)) + ")");
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

void setupCloud(void) {
    wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_CONNECTED);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_START);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
	wifi_handler.onEvent(onWiFiEvent, ARDUINO_EVENT_WIFI_SCAN_DONE);
	wifi_handler.setCredentials(test_wifi_ssid, test_wifi_password, true);
	wifi_handler.begin(WIFI_STA);
	wifi_handler.printMacAddress(WIFI_IF_STA);
	wifi_handler.printMacAddress(WIFI_IF_AP);
	//initMqttClient();
	//onMqttConnect(mqtt_connection_callback);
	//onMqttData(mqtt_data_callback);
}