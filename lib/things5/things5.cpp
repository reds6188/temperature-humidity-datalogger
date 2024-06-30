#include "things5.h"

Things5::Things5(bool timestamp_en) {
    doc.clear();
    timestamp_enabled = timestamp_en;
}

void Things5::setUUID(void) {
    // Generating UUID v4
    uint8_t uuid_array[16];
    ESPRandom::uuid4(uuid_array);

    doc.clear();    // Clear document
    doc["request_id"] = ESPRandom::uuidToString(uuid_array);
}

void Things5::setProperty(String key, String value) {
    doc[key] = value;
}

//=================================================================================================
//===================================   THINGS5 METRIC METHODS   ==================================
//=================================================================================================

// Define a new metric [label] of integer/float [type] ------------------------
void Things5::defMetric(const char * label, metrics_t type) {
    switch(type) {
        case METRICS_INT:
            if(num_metrics_int < T5_MAX_NUM_VAR) {
                strcpy(metrics_int[num_metrics_int].label, label);
                metrics_int[num_metrics_int].value = 0;
                num_metrics_int++;
            }
            break;
        case METRICS_FLT:
            if(num_metrics_flt < T5_MAX_NUM_VAR) {
                strcpy(metrics_flt[num_metrics_flt].label, label);
                metrics_flt[num_metrics_flt].value = 0;
                num_metrics_flt++;
            }
            break;
        case METRICS_STR:
            break;
        default:
            break;
    }
}

// Return the index of the metric [label] -------------------------------------
// return -1 if the metric doesn't exists -------------------------------------
int8_t Things5::findMetric(const char * label, metrics_t type) {
    int8_t index = -1;
    switch(type) {
        case METRICS_INT:
            for(uint8_t i=0 ; i<num_metrics_int ; i++) {
                if(!strcmp(metrics_int[i].label, label)) {
                    index = i;
                    break;
                }
            }
            break;
        case METRICS_FLT:
            for(uint8_t i=0 ; i<num_metrics_flt ; i++) {
                if(!strcmp(metrics_flt[i].label, label)) {
                    index = i;
                    break;
                }
            }
            break;
        default:
            break;
    }

    return index;
}

// Update metric [label] with integer [value] ---------------------------------
// return "true" if value has changed -----------------------------------------
bool Things5::updateMetric(const char * label, int32_t value) {
    int8_t index = findMetric(label, METRICS_INT);
    if(index > -1) {
        if(metrics_int[index].value != value) {
            metrics_int[index].value = value;
            #ifdef LOG_DEBUG_T5
            console.log(T5_T, "Metric \"" + String(label) + "\" has changed");
            #endif
            if(building_msg) {
                if(!init_metrics) {
                    init_metrics = true;
                    initMetrics(timestamp);
                }
                addMetric(String(label), value);
            }
            return true;
        }
        return false;
    }
    #ifdef LOG_DEBUG_T5
    console.error(T5_T, "Metric not found");
    #endif
    return false;
}

// Update metric [label] with float [value] -----------------------------------
// return "true" if value has changed -----------------------------------------
bool Things5::updateMetric(const char * label, float value) {
    int8_t index = findMetric(label, METRICS_FLT);
    float round_value = roundf(value * 10)/10;
    if(index > -1) {
        if(metrics_flt[index].value != round_value) {
            metrics_flt[index].value = round_value;
            #ifdef LOG_DEBUG_T5
            console.log(T5_T, "Metric \"" + String(label) + "\" has changed: " + String(round_value));
            #endif
            if(building_msg) {
                if(!init_metrics) {
                    init_metrics = true;
                    initMetrics(timestamp);
                }
                addMetric(String(label), round_value);
            }
            return true;
        }
        return false;
    }
    #ifdef LOG_DEBUG_T5
    console.error(T5_T, "Metric not found");
    #endif
    return false;
}

void Things5::initMetrics(unsigned long long time_stamp) {
    JsonArray metrics = doc.createNestedArray("metrics");
    JsonObject metrics_0 = metrics.createNestedObject();
    if(timestamp_enabled)
        metrics_0["timestamp"] = time_stamp;
}

void Things5::writeAllMetrics(void) {
    for(uint8_t i=0 ; i<num_metrics_int ; i++)
        addMetric(metrics_int[i].label, metrics_int[i].value);
    for(uint8_t i=0 ; i<num_metrics_flt ; i++)
        addMetric(metrics_flt[i].label, metrics_flt[i].value);
}

void Things5::writeAllMetrics(metrics_t type) {
    switch(type) {
        case METRICS_INT:
            for(uint8_t i=0 ; i<num_metrics_int ; i++)
                addMetric(metrics_int[i].label, metrics_int[i].value);
            break;
        case METRICS_FLT:
            for(uint8_t i=0 ; i<num_metrics_flt ; i++)
                addMetric(metrics_flt[i].label, metrics_flt[i].value);
            break;
        default:
            break;
    }
}

void Things5::addMetric(String label, int32_t value) {
    JsonArray metrics_0_data;
    if(doc["metrics"][0]["data"].isNull())
        metrics_0_data = doc["metrics"][0].createNestedArray("data");
    else
        metrics_0_data = doc["metrics"][0]["data"];

    StaticJsonDocument<64> obj;
    obj["name"] = label;
    obj["value"] = String(value);
    metrics_0_data.add(obj);
    obj.clear();
    datapoint_counter.message++;
}

void Things5::addMetric(String label, uint32_t value) {
    JsonArray metrics_0_data;
    if(doc["metrics"][0]["data"].isNull())
        metrics_0_data = doc["metrics"][0].createNestedArray("data");
    else
        metrics_0_data = doc["metrics"][0]["data"];

    StaticJsonDocument<64> obj;
    obj["name"] = label;
    obj["value"] = String(value);
    metrics_0_data.add(obj);
    obj.clear();
    datapoint_counter.message++;
}

void Things5::addMetric(String label, float value) {
    JsonArray metrics_0_data;
    if(doc["metrics"][0]["data"].isNull())
        metrics_0_data = doc["metrics"][0].createNestedArray("data");
    else
        metrics_0_data = doc["metrics"][0]["data"];

    StaticJsonDocument<64> obj;
    obj["name"] = label;
    obj["value"] = String(value);
    metrics_0_data.add(obj);
    obj.clear();
    datapoint_counter.message++;
}

//=================================================================================================
//===================================   THINGS5 STATE METHODS   ===================================
//=================================================================================================

// Define a new state [label] -------------------------------------------------
void Things5::defState(const char * label) {
    if(num_states < T5_MAX_NUM_VAR) {
        strcpy(states[num_states].label, label);
        states[num_states].value = "";
        num_states++;
    }
}

// Return the index of the state [label] --------------------------------------
// return -1 if the state doesn't exists --------------------------------------
int8_t Things5::findState(const char * label) {
    int8_t index = -1;
    for(uint8_t i=0 ; i<num_states ; i++) {
        if(!strcmp(states[i].label, label)) {
            index = i;
            break;
        }
    }

    return index;
}

// Update state [label] with string [value] -----------------------------------
// return "true" if value has changed -----------------------------------------
bool Things5::updateState(const char * label, String value) {
    int8_t index = findState(label);
    if(index > -1) {
        if(states[index].value != value) {
            states[index].value = value;
            #ifdef LOG_DEBUG_T5
            console.log(T5_T, "State \"" + String(label) + "\" has changed");
            #endif
            if(building_msg) {
                if(!init_states) {
                    init_states = true;
                    initMetrics(timestamp);
                }
                addState(String(label), value);
            }
            return true;
        }
        return false;
    }
    #ifdef LOG_DEBUG_T5
    console.error(T5_T, "State not found");
    #endif
    return false;
}

void Things5::initStates(unsigned long long time_stamp) {
    JsonArray states = doc.createNestedArray("states");
    JsonObject states_0 = states.createNestedObject();
    if(timestamp_enabled)
        states_0["timestamp"] = time_stamp;
}

void Things5::writeAllStates(void) {
    for(uint8_t i=0 ; i<num_states ; i++) {
        addState(states[i].label, states[i].value);
    }
}

void Things5::addState(String label, String value) {
    JsonArray states_0_data;
    if(doc["states"][0]["data"].isNull())
        states_0_data = doc["states"][0].createNestedArray("data");
    else
        states_0_data = doc["states"][0]["data"];

    StaticJsonDocument<64> obj;
    obj["name"] = label;
    obj["value"] = String(value);
    states_0_data.add(obj);
    obj.clear();
    datapoint_counter.message++;
}

//=================================================================================================
//===================================   THINGS5 EVENT METHODS   ===================================
//=================================================================================================

// Define a new event [label] -------------------------------------------------
void Things5::defEvent(const char * label) {
    if(num_events < T5_MAX_NUM_VAR) {
        strcpy(events[num_events].label, label);
        states[num_events].value = false;
        num_events++;
    }
}

// Return the index of the event [label] --------------------------------------
// return -1 if the event doesn't exists --------------------------------------
int8_t Things5::findEvent(const char * label) {
    int8_t index = -1;
    for(uint8_t i=0 ; i<num_events ; i++) {
        if(!strcmp(events[i].label, label)) {
            index = i;
            break;
        }
    }

    return index;
}

// Update event [label] with boolean [value] ----------------------------------
// return "true" if value has changed -----------------------------------------
bool Things5::updateEvent(const char * label, bool value) {
    int8_t index = findEvent(label);
    if(index > -1) {
        if(events[index].value != value) {
            events[index].value = value;
            if(events[index].value) {
                #ifdef LOG_DEBUG_T5
                console.log(T5_T, "Event \"" + String(label) + "\" has triggered");
                #endif
                if(building_msg) {
                    if(!init_events) {
                        init_events = true;
                        initEvents(timestamp);
                    }
                    addEvent(String(label));
                }
            }
            else
                console.log(T5_T, "Event \"" + String(label) + "\" has reset");
            return true;
        }
        return false;
    }
    #ifdef LOG_DEBUG_T5
    console.error(T5_T, "Event not found");
    #endif
    return false;
}

void Things5::initEvents(unsigned long long time_stamp) {
    JsonArray events = doc.createNestedArray("events");
    JsonObject events_0 = events.createNestedObject();
    if(timestamp_enabled)
        events_0["timestamp"] = time_stamp;
}

void Things5::initEvents(void) {
    initEvents(timestamp);
}

void Things5::addEvent(String label) {
    JsonArray events_0_data;
    if(doc["events"][0]["data"].isNull())
        events_0_data = doc["events"][0].createNestedArray("data");
    else
        events_0_data = doc["events"][0]["data"];

    StaticJsonDocument<64> obj;
    obj["name"] = label;
    events_0_data.add(obj);
    obj.clear();
    datapoint_counter.message++;
}

void Things5::addEvent(String label, String description) {
    JsonArray events_0_data;
    if(doc["events"][0]["data"].isNull())
        events_0_data = doc["events"][0].createNestedArray("data");
    else
        events_0_data = doc["events"][0]["data"];

    StaticJsonDocument<192> obj;
    obj["name"] = label;
    obj["metadata"]["description"] = description;
    events_0_data.add(obj);
    obj.clear();
    datapoint_counter.message++;
}

//=================================================================================================
//====================================   THINGS5 LOG METHODS   ====================================
//=================================================================================================

void Things5::initLogs(void) {
  JsonArray logs = doc.createNestedArray("logs");
}

void Things5::addLog(unsigned long long time_stamp, String log_line) {
    JsonArray log = doc["logs"].createNestedArray();
    log.add(String(time_stamp));
    log.add(log_line);
}

//=================================================================================================
//==============================   THINGS5 FIRMWARE UPDATE METHODS   ==============================
//=================================================================================================

void Things5::updateProgress(String uuid, uint8_t progress) {
    doc["request_id"] = uuid;
    doc["progress_percentage"] = progress;
}

void Things5::updateResult(String uuid, bool success) {
    doc["request_id"] = uuid;
    doc["success"] = success;
}

void Things5::addParamToArray(JsonArray arr, const char * name, uint8_t value) {
	JsonObject obj = arr.createNestedObject();
	obj["name"] = name;
	obj["value"] = String(value);
	obj["type"] = "integer";
}

void Things5::addParamToArray(JsonArray arr, const char * name, String value) {
	JsonObject obj = arr.createNestedObject();
	obj["name"] = name;
	obj["value"] = value;
	obj["type"] = "string";
}

String Things5::getPayload(void) {
    String payload;
    serializeJson(doc, payload);
    doc.clear();
    building_msg = false;
    return payload;
}

bool Things5::emptyMessage(void) {
    return !(init_metrics | init_states | init_events);
}

void Things5::createMessage(unsigned long long time_stamp) {
    setUUID();
    timestamp = time_stamp;
    init_metrics = false;
    init_states = false;
    init_events = false;
    building_msg = true;
    datapoint_counter.message = 0;
}

void Things5::deleteMessage(void) {
    init_metrics = false;
    init_states = false;
    init_events = false;
    building_msg = false;
    doc.clear();
}

void Things5::updateDatapointCounter(void) {
    console.info(T5_T, "Sent message with " + String(datapoint_counter.message) + " datapoint(s)");
    datapoint_counter.daily += datapoint_counter.message;
    datapoint_counter.message = 0;
}

void Things5::resetDatapointCounter(void) {
    datapoint_counter.daily = 0;
    datapoint_counter.message = 0;
}

uint32_t Things5::getDatapointCounter(void) {
    return datapoint_counter.daily;
}

void Things5::printVarList(void) {
    console.header("THINGS5 VARIABLE LIST", HASH, 80);
    for(uint8_t i=0 ; i<num_metrics_int ; i++) {
        String label = metrics_int[i].label;
        int value = metrics_int[i].value;
        console.info(T5_T, "[METRICS] Label = \"" + label + "\" ; Value = " + String(value) + " ; Type = INTEGER");
    }
    for(uint8_t i=0 ; i<num_metrics_flt ; i++) {
        String label = metrics_flt[i].label;
        int value = metrics_flt[i].value;
        console.info(T5_T, "[METRICS] Label = \"" + label + "\" ; Value = " + String(value) + " ; Type = FLOAT");
    }
    for(uint8_t i=0 ; i<num_states ; i++) {
        String label = states[i].label;
        String value = states[i].value;
        console.info(T5_T, "[STATES] Label = \"" + label + "\" ; Value = " + String(value));
    }
    for(uint8_t i=0 ; i<num_events ; i++) {
        String label = events[i].label;
        bool value = events[i].value;
        console.info(T5_T, "[EVENTS] Label = \"" + label + "\" ; Value = " + String(value));
    }
}




    