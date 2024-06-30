#ifndef THINGS5_H_
#define THINGS5_H_

#include <Arduino.h>
#include <ESPRandom.h>
#include <ArduinoJson.h>
#include <console.h>

#define T5_T 				"T5"

#define JSON_SIZE           2048	// JSON maximum size
#define T5_LABEL_MAX_LENGTH	20		// Label maximum length
#define T5_MAX_NUM_VAR		20		// Metrics/State/Events array maximum length

typedef enum {
	METRICS_INT = 0,
	METRICS_FLT,
	METRICS_STR
}metrics_t;

typedef struct {
	char label[T5_LABEL_MAX_LENGTH];
	int32_t value;
}metrics_int_t;

typedef struct {
	char label[T5_LABEL_MAX_LENGTH];
	float value;
}metrics_flt_t;

typedef struct {
	char label[T5_LABEL_MAX_LENGTH];
	String value;
}t5_states_t;

typedef struct {
	char label[T5_LABEL_MAX_LENGTH];
	bool value;
}t5_events_t;

typedef struct {
	uint8_t message;
	uint32_t daily;
}dp_counter_t;

class Things5 {
	private:
		//StaticJsonDocument<JSON_SIZE> doc;
		//DynamicJsonDocument doc = DynamicJsonDocument(1024);
		bool timestamp_enabled;
		DynamicJsonDocument doc = DynamicJsonDocument(JSON_SIZE);
		unsigned long long timestamp;
		//-----------------------
		metrics_int_t metrics_int[T5_MAX_NUM_VAR];
		metrics_flt_t metrics_flt[T5_MAX_NUM_VAR];
		t5_states_t states[T5_MAX_NUM_VAR];
		t5_events_t events[T5_MAX_NUM_VAR];
		uint8_t num_metrics_int;
		uint8_t num_metrics_flt;
		uint8_t num_states;
		uint8_t num_events;
		bool building_msg;
		bool init_metrics;
		bool init_states;
		bool init_events;
		dp_counter_t datapoint_counter;
	public:
		Things5(bool timestamp_en = true);
		void setUUID(void);
		void setProperty(String key, String value);
		// Things5 Metrics methods ------------------------------------------------------
		void defMetric(const char * label, metrics_t type);
		int8_t findMetric(const char * label, metrics_t type);
		bool updateMetric(const char * label, int32_t value);
		bool updateMetric(const char * label, float value);
		void initMetrics(unsigned long long time_stamp);
		void writeAllMetrics(void);
		void writeAllMetrics(metrics_t type);
		void addMetric(String label, int32_t value);
		void addMetric(String label, uint32_t value);
		void addMetric(String label, float value);
		// Things5 States methods -------------------------------------------------------
		void defState(const char * label);
		int8_t findState(const char * label);
		bool updateState(const char * label, String value);
		void initStates(unsigned long long time_stamp);
		void writeAllStates(void);
		void addState(String label, String value);
		// Things5 Events methods -------------------------------------------------------
		void defEvent(const char * label);
		int8_t findEvent(const char * label);
		bool updateEvent(const char * label, bool value);
		void initEvents(unsigned long long time_stamp);
		void initEvents(void);
		void addEvent(String label);
		void addEvent(String label, String description);
		// Things5 Logs methods ---------------------------------------------------------
		void initLogs(void);
		void addLog(unsigned long long time_stamp, String log_line);
		void updateProgress(String uuid, uint8_t progress);
		void updateResult(String uuid, bool success);
		void addParamToArray(JsonArray arr, const char * name, uint8_t value);
		void addParamToArray(JsonArray arr, const char * name, String value);
		String getPayload(void);
		bool emptyMessage(void);
		void createMessage(unsigned long long time_stamp);
		void deleteMessage(void);
		void updateDatapointCounter(void);
		void resetDatapointCounter(void);
		uint32_t getDatapointCounter(void);
		void printVarList(void);
};

#endif	// THINGS5_H_