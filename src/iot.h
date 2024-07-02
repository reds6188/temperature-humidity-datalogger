#ifndef IOT_H_
#define IOT_H_

#include <wi-fi.h>
#include <mqtt.h>
#include <things5.h>
#include <t5_variables.h>

extern Things5	Thing;

void onWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info);
void mqtt_connection_callback(void);
void mqtt_data_callback(char * topic, char * data, int data_length);
void setupCloud(void);

#endif  /* IOT_H_ */