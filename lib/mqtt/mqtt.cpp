#include "mqtt.h"

#include <LittleFS.h>

char device_id[48];
char client_id[64] = "things5-production-machine-";
char thing5_url[67] = "things5-production/v1/devices/";

const char * sub_topic_list[NUM_MQTT_TOPIC] = {
	READ_VAR_REQ,
	WRITE_VAR_REQ,
	LOG_INGESTION_RES,
	UPDATE_FIRMWARE_REQ,
	FIRMWARE_LIST_RES,
	FIRMWARE_DETAILS_RES,
};

Timer TimerReconnectMqtt;
bool mqtt_connected;

void (*cbOnMqttConnect)(void);
void (*cbOnMqttDisconnect)(void);
void (*cbOnMqttData)(char * topic, char * data, int data_length);

void onMqttConnect(void (*callback)(void)) {
	cbOnMqttConnect = callback;
}

void onMqttData(void (*callback)(char * topic, char * data, int data_length)) {
	cbOnMqttData = callback;
}

esp_mqtt_client_handle_t t5_mqtt_client;
char topic_list[NUM_MQTT_TOPIC][MQTT_TOPIC_SIZE];

void initMqttClient(void) {
	console.info(MQTT_T, "Reading certificates...");

	fs::SPIFFSFS CERT;

	// Mount CERT partition -----------------------------------------------------------------------
	if(!CERT.begin(false, "/spiffs", 4, "spiffs"))
        console.error(MQTT_T, "An Error has occurred while mounting CERT partition");
	else
		console.success(MQTT_T, "CERT partition has mounted");

	// Read "device_id.txt" file ----------------------------------------------
	const char * dev_id = getFileAsString(CERT, PATH_DEVICE_ID);
	if(dev_id != NULL) {
		strcat(client_id, dev_id);
		strcat(thing5_url, dev_id);
		strcpy(device_id, dev_id);
		console.info(MQTT_T, "Device ID: \"" + String(device_id) + "\"");
	}
	else
		console.error(MQTT_T, "Unknown Device ID");

	// Read "/device.ca.pem" file ---------------------------------------------
	char * cacert_str = getFileAsString(CERT, PATH_DEVICE_CA);
	if(cacert_str != NULL)
		console.success(MQTT_T, "\"" + String(PATH_DEVICE_CA) + "\" was loaded successfully");
	else
		console.error(MQTT_T, "\"" + String(PATH_DEVICE_CA) + "\" not found!");

	// Read "/device.certificate.pem" file ------------------------------------
	char * cert_str = getFileAsString(CERT, PATH_DEVICE_CERTIFICATE);
	if(cert_str != NULL)
		console.success(MQTT_T, "\"" + String(PATH_DEVICE_CERTIFICATE) + "\" was loaded successfully");
	else
		console.error(MQTT_T, "\"" + String(PATH_DEVICE_CERTIFICATE) + "\" not found!");

	// Read "/device.private.pem" file ------------------------------------
	char * private_str = getFileAsString(CERT, PATH_DEVICE_PRV_KEY);
	if(private_str != NULL)
		console.success(MQTT_T, "\"" + String(PATH_DEVICE_PRV_KEY) + "\" was loaded successfully");
	else
		console.error(MQTT_T, "\"" + String(PATH_DEVICE_PRV_KEY) + "\" not found!");

	CERT.end();

	// Init topic list --------------------------------------------------------
	for(uint8_t i=0 ; i<NUM_MQTT_TOPIC ; i++) {
		strcpy(topic_list[i], thing5_url);
		strcat(topic_list[i], sub_topic_list[i]);
	}

	const esp_mqtt_client_config_t mqttConf = {
            .uri = T5_BROKER_URL,
            .client_id = client_id,
			//.task_stack = 12000,
            .buffer_size = MQTT_BUFFER_SIZE,
            .cert_pem = cacert_str,
            .client_cert_pem = cert_str,
            .client_key_pem = private_str,
	};

	t5_mqtt_client = esp_mqtt_client_init(&mqttConf);
	if (t5_mqtt_client == NULL) {
		console.error(MQTT_T, "Error while creating MQTT client");
	}
	console.success(MQTT_T, "Client created");

	esp_mqtt_client_register_event(t5_mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, nullptr);
	//startMqttClient();
}

void deinitMqttCLient(void) {
	stopMqttClient();
	esp_mqtt_client_destroy(t5_mqtt_client);
}

void startMqttClient(void) {
	esp_err_t err = esp_mqtt_client_start(t5_mqtt_client);
	if(err)
		console.error(MQTT_T, "Error while starting MQTT client :" + String(err));
	else
		console.success(MQTT_T, "Client started");
}

void stopMqttClient(void) {
	esp_err_t err = esp_mqtt_client_stop(t5_mqtt_client);
	if(err)
		console.error(MQTT_T, "Error while stopping MQTT client :" + String(err));
	else
		console.warning(MQTT_T, "Client stopped");
}

void reconnectMqttClient(void) {
	esp_err_t err = esp_mqtt_client_reconnect(t5_mqtt_client);
	if(err)
		console.error(MQTT_T, "Error while reconnecting MQTT client :" + String(err));
	else
		console.warning(MQTT_T, "Client reconnecting...");
}

/*
void loopMqttClient(void) {
	if(!mqtt_connected && TimerReconnectMqtt.elapsedX1s(1)) {
		console.info(MQTT_T, "Restarting MQTT Client...");
		startMqttClient();
	}
}
*/

String getMqttDeviceID(void) {
	console.log(MQTT_T, "Device id = " + String(device_id));
	return String(device_id);
}

bool isMqttConnected(void) {
	return mqtt_connected;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
	auto event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
	char * short_topic;

	switch ((esp_mqtt_event_id_t) event_id) {
		case MQTT_EVENT_DISCONNECTED:
			mqtt_connected = false;
			console.warning(MQTT_T, "EVENT - Client disconnected!");
			//reconnectMqttClient();
			//TimerReconnectMqtt.trigger();
			break;
		case MQTT_EVENT_CONNECTED:
			mqtt_connected = true;
			console.success(MQTT_T, "EVENT - Client connected!");
			for(uint8_t i=0 ; i<NUM_MQTT_TOPIC ; i++) {
				if(esp_mqtt_client_subscribe(client, (const char *)topic_list[i], 1));
					console.success(MQTT_T, "+++ Subscribe successfully done on topic \"" + String(sub_topic_list[i]) + "\"");
			}
			cbOnMqttConnect();
			break;
		case MQTT_EVENT_ERROR:
            console.error(MQTT_T, "EVENT - Error");
			if(event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
 				console.error(MQTT_T, "Last error code reported from esp-tls: " + String(event->error_handle->esp_tls_last_esp_err));
				console.error(MQTT_T, "Last tls stack error number: " + String(event->error_handle->esp_tls_stack_err));
				console.error(MQTT_T, "Last captured errno: " + String(event->error_handle->esp_transport_sock_errno) + " (" + strerror(event->error_handle->esp_transport_sock_errno) + ")");

            } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
				console.error(MQTT_T, "Connection refused error: " + String(event->error_handle->connect_return_code));
            } else {
				console.error(MQTT_T, "Unknown error type: " + String(event->error_handle->error_type));
            }
            break;
		case MQTT_EVENT_DATA:
			console.info(MQTT_T, "Event data received");
			if(event->topic == NULL) {
				console.warning(MQTT_T, "Topic is NULL");
			}
			else {
				short_topic = (char*)calloc(MQTT_SHORT_TOPIC_SIZE, sizeof(char));
				const char * topic = strstr((const char *)event->topic, device_id);
				strcpy(short_topic, topic + strlen(device_id));
				cbOnMqttData(short_topic, event->data, event->data_len);
				free(short_topic);
			}
			break;
		default:
			break;
	}
}

bool publishMqtt(const char * topic, String payload) {
	char * url = (char*)calloc(strlen(thing5_url) + strlen(topic) + 1, sizeof(char));
	strcpy(url, thing5_url);
	strcat(url, topic);
	//int result = esp_mqtt_client_publish(t5_mqtt_client, url, payload.c_str(), payload.length(), 1, 0);
	int result = esp_mqtt_client_enqueue(t5_mqtt_client, url, payload.c_str(), payload.length(), 1, 0, true);
	free(url);
	if(result == -2)
		console.error(MQTT_T, "Outbox is full");
	return (result >= 0);
}

void unsubscribeMqtt(const char * topic) {
	char * url = (char*)calloc(strlen(thing5_url) + strlen(topic) + 1, sizeof(char));
	strcpy(url, thing5_url);
	strcat(url, topic);
	if(esp_mqtt_client_unsubscribe(t5_mqtt_client, url) == -1)
		console.error(MQTT_T, "Failed to unsubscribe topic \"" + String(topic) + "\"");
	else
		console.success(MQTT_T, "--- Unsubscribe successfully done on topic \"" + String(topic) + "\"");
}
