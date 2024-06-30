#pragma once

#ifndef T5_VARIABLES_H_
#define T5_VARIABLES_H_

// WARNING: Limit variable name to 20 characters 

// METRICS ------------------------------------------------
static const char * T5_S1_TEMP =		    "s1_temperature";   // float
static const char * T5_S1_HUMI =		    "s1_humidity";      // float
static const char * T5_S2_TEMP =		    "s2_temperature";   // float
static const char * T5_S2_HUMI =		    "s2_humidity";      // float
static const char * T5_S3_TEMP =		    "s3_temperature";   // float
static const char * T5_S3_HUMI =		    "s3_humidity";      // float
static const char * T5_S4_TEMP =		    "s4_temperature";   // float
static const char * T5_S4_HUMI =		    "s4_humidity";      // float

// EVENTS -------------------------------------------------
static const char * T5_ALARM_COM_S1 =		"alarm_com_s1";
static const char * T5_ALARM_COM_S2 =		"alarm_com_s2";
static const char * T5_ALARM_COM_S3 =		"alarm_com_s3";
static const char * T5_ALARM_COM_S4 =       "alarm_com_s4";
static const char * T5_INFO_RESTART =		"info_restart";


#endif  /* T5_VARIABLES_H_ */