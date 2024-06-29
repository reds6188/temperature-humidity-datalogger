#ifndef MQTT_H_
#define MQTT_H_

#include <Arduino.h>
#include <console.h>
#include <timer.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
#include <ESPRandom.h>
#include <fw-update.h>
#include <tools.h>

#include <mqtt_client.h>

#define MQTT_T						"MQTT"	// Tag for log

#define SSL_PORT					8883
#define MQTT_PORT					1883

#define MQTT_BUFFER_SIZE			1280//1024//4096//2048
#define MQTT_TOPIC_SIZE				100
#define MQTT_SHORT_TOPIC_SIZE		48
#define NUM_MQTT_TOPIC				6//4

#define T5_BROKER_URL				"mqtts://mqtt.things5.digital:8883"

// Things5 Short Topics -------------------------------------------------------
#define DATA_INGESTION_REQ			"/data_req"
#define DATA_INGESTION_RES			"/data_res"
#define READ_VAR_REQ				"/cmd/read_variables_req"
#define READ_VAR_RES				"/cmd/read_variables_res"
#define WRITE_VAR_REQ				"/cmd/write_variables_req"
#define WRITE_VAR_RES				"/cmd/write_variables_res"
#define UPDATE_FIRMWARE_REQ			"/cmd/update_firmware_req"
#define UPDATE_FIRMWARE_RES			"/cmd/update_firmware_res"
#define UPDATE_FIRMWARE_PROGRESS	"/cmd/update_firmware_progress"
#define FIRMWARE_VERSION_REQ		"/event/firmware_version_req"
#define FIRMWARE_LIST_REQ			"/req/firmwares_list_req"
#define FIRMWARE_LIST_RES			"/req/firmwares_list_res"
#define FIRMWARE_DETAILS_REQ		"/req/firmware_details_req"
#define FIRMWARE_DETAILS_RES		"/req/firmware_details_res"
#define LOG_INGESTION_REQ			"/logs/ingest_req"
#define LOG_INGESTION_RES			"/logs/ingest_res"

#define PATH_DEVICE_ID				"/device_id.txt"
#define PATH_DEVICE_CA				"/device.ca.pem"
#define PATH_DEVICE_CERTIFICATE		"/device.certificate.pem"
#define PATH_DEVICE_PRV_KEY			"/device.private.pem"

typedef struct {
	char payload[MQTT_BUFFER_SIZE];
	char topic[MQTT_TOPIC_SIZE];
}mqtt_msg_t;

void onMqttConnect(void (*callback)(void));
void onMqttData(void (*callback)(char * topic, char * data, int data_length));
void initMqttClient(void);
void deinitMqttCLient(void);
void startMqttClient(void);
void stopMqttClient(void);
void reconnectMqttClient(void);
//void loopMqttClient(void);
String getMqttDeviceID(void);
bool isMqttConnected(void);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
bool publishMqtt(const char * topic, String payload);
void unsubscribeMqtt(const char * topic);

//void mqttCallback(char* topic, byte* message, unsigned int length);

#endif  /* MQTT_H_ */