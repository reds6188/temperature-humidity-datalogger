#include "tools.h"

String byteToHex(uint8_t data) {
    char tmp[4];
    sprintf(tmp, "0x%.2X ", data);
    return String(tmp);
}

String wordToHex(uint16_t data) {
    char tmp[6];
    sprintf(tmp, "0x%.4X ", data);
    return String(tmp);
}

String DataToHexString(uint8_t *data, int data_size) {
	char hex_str[6];
	String output_str = "";

	if(data_size < 32) {
		for(uint8_t i = 0 ; i < data_size ; i++) {
			sprintf(hex_str, "0x%02X ", *(data + i));
			output_str += String(hex_str);
		}
	}
	else
		output_str = "Failed to print packet: length exceeds maximum value";

	return output_str;
}

void softwareReset(void) {
	console.warning(ESP32_T, "Restarting...");
	delay(1000);
	ESP.restart();
}

void printDeviceInfo(void) {
	console.info(ESP32_T, "ESP32 Chip Model: " + String(ESP.getChipModel()));
	console.info(ESP32_T, "ESP32 Flash Size: " + String(ESP.getFlashChipSize()));
	console.info(ESP32_T, "Compile Date: " + String(__DATE__));
	console.info(ESP32_T, "Compile Time: " + String(__TIME__));
}

void printPartitionsInfo(void) {
	char part_str[64];
	sprintf(part_str, "| Type | Sub |  Offset  |   Size   |       Label      |");
	console.info(ESP32_T, String(part_str));
	sprintf(part_str, "| ---- | --- | -------- | -------- | ---------------- |");
	console.info(ESP32_T, String(part_str));

	esp_partition_iterator_t pi = esp_partition_find(ESP_PARTITION_TYPE_ANY, ESP_PARTITION_SUBTYPE_ANY, NULL);
	if (pi != NULL) {
		do {
			const esp_partition_t* p = esp_partition_get(pi);
			sprintf(part_str, "|  %02x  | %02x  | 0x%06X | 0x%06X | %-16s |", 
			p->type, p->subtype, p->address, p->size, p->label);
			console.info(ESP32_T, String(part_str));
		} while (pi = (esp_partition_next(pi)));		
	}
}

String getResetReason(void) {
    esp_reset_reason_t rst_reason = esp_reset_reason();
    String reason;

    switch(rst_reason) {
        case ESP_RST_UNKNOWN:   reason = "Reset reason can not be determined";                     break;
        case ESP_RST_POWERON:   reason = "Reset due to power-on event";                            break;
        case ESP_RST_EXT:       reason = "Reset by external pin (not applicable for ESP32)";       break;
        case ESP_RST_SW:        reason = "Software reset via esp_restart";                         break;
        case ESP_RST_PANIC:     reason = "Software reset due to exception/panic";                  break;
        case ESP_RST_INT_WDT:   reason = "Reset (software or hardware) due to interrupt watchdog"; break;
        case ESP_RST_TASK_WDT:  reason = "Reset due to task watchdog";                             break;
        case ESP_RST_WDT:       reason = "Reset due to other watchdogs";                           break;
        case ESP_RST_DEEPSLEEP: reason = "Reset after exiting deep sleep mode";                    break;
        case ESP_RST_BROWNOUT:  reason = "Brownout reset (software or hardware)";                  break;
        case ESP_RST_SDIO:      reason = "Reset over SDIO";                                        break;
    }

    return reason;
}

void printResetReason(void) {
    console.info(ESP32_T, getResetReason());  
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
	//console.warning(FS_T, "Listing directory:" + String(dirname));
	File root = fs.open(dirname);
	if(!root){
		console.error(FILE_T, "Failed to open directory");
		return;
	}
	if(!root.isDirectory()){
		console.error(FILE_T, "Not a directory");
		return;
	}

	File file = root.openNextFile();
	while(file){
		if(file.isDirectory()){
			listDir(fs, file.path(), levels +1);
		}
		else {
			console.data(FILE_T, " FILE : " + String(dirname) + "/" + String(file.name()) + "  SIZE: " + String(file.size()));
		}
		file = root.openNextFile();
	}
}

char* getFileAsString(fs::FS &fs, String path) {
	if(fs.exists(path)) {
		File file = fs.open(path, FILE_READ);
		if(!file) {
			console.error(FILE_T, "Failed to open " + path);
			return NULL;
		}
		char* buffer = (char*)calloc(file.size() + 1, sizeof(char));
		file.readBytes(buffer, file.size());
		file.close();
		return buffer;
	}

	console.error(FILE_T, "File \"" + path + "\" does not exists!");
	return NULL;
}
