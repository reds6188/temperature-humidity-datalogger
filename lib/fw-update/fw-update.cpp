#include "fw-update.h"

bool fwUpdating = false;
String update_id;
String firmware_url;
void(*cbOnLoop)(void);
void(*cbOnProgress)(String uuid, uint8_t progress);
void(*cbOnEnd)(String uuid, bool success);

void startFirmwareUpdate(const char* url, String uuid) {
    update_id = uuid;
    firmware_url = String(url);
    console.log(UPDATE_T, "Firwmare url: \"" + firmware_url + "\"");
    fwUpdating = true;
}

void onUpdateLoop(void(*callback)(void)) {
    cbOnLoop = callback;
}

void onUpdateProgress(void(*callback)(String uuid, uint8_t progress)) {
    cbOnProgress = callback;
}

void onUpdateEnd(void(*callback)(String uuid, bool success)) {
    cbOnEnd = callback;
}

bool isFwUpdating(void) {
    return fwUpdating;
}

bool handleFirmwareUpdate(void) {
    HTTPClient https;
    WiFiClient wf;
    size_t bin_size, rem_size;
    float flt_progress;
    uint8_t progress, progress_old;
    char str[60];
    const char * url = firmware_url.c_str();
    unsigned long update_refresh;

    https.setTimeout(50000);
    https.begin(url);
    //https.begin(firmware_url.c_str());
    
    // Send HTTP GET request
    console.log(UPDATE_T, "Requesting firmware binary with GET request...");
    int httpResponseCode = https.GET();

    if(httpResponseCode > 0) {
        console.log(UPDATE_T, "HTTP Response code: " + String(httpResponseCode));
        bin_size = https.getSize();
        rem_size = bin_size;
        console.log(UPDATE_T, "HTTP Payload size: " + String(bin_size));
        WiFiClient * stream = https.getStreamPtr();

        // create buffer for read
        //uint8_t buff[2048] = { 0 };

        if(!Update.begin(bin_size)) { //start with max available size
            //Update.printError(Serial);
            console.error(UPDATE_T, "Error: " + String(Update.errorString()));
            cbOnEnd(update_id, false);
            return false;
        }
        else {
            while(https.connected() && (rem_size > 0 || rem_size == -1))
            {
                cbOnLoop();

                // Get available data size
                size_t size = stream->available();

                if(size) {
                    // Read up to 2048 byte
                    // create buffer for read
                    uint8_t buff[2048] = { 0 };
                    //uint8_t * buff = (uint8_t*)calloc(2048, sizeof(uint8_t));
                    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                    int wc = Update.write(buff, c);
                    //free(buff);
                    //Serial.printf("Written %d (expected %d)\n", wc, c);
                    //sprintf(str, "Written %d (expected %d)", wc, c);
                    //console.log(UPDATE_T, String(str));
                    console.log(UPDATE_T, "Written " + String(wc) + " (expected " + String(c) + ")");

                    if(wc != c) {
                        //Update.printError(Serial);
                        console.error(UPDATE_T, "Error: " + String(Update.errorString()));
                        return false;
                    }
                    else {
                        flt_progress = (float)(bin_size - rem_size) / (float)(bin_size) * 100;
                        sprintf(str, "Remaining %d of %d (Progress = %3.1f %%)", rem_size, bin_size, flt_progress);
                        console.log(UPDATE_T, String(str));

                        // Send update notification every 1 second
                        if(millis() - update_refresh > 1000) {
                            update_refresh = millis();

                            progress = flt_progress;
                            if(progress != progress_old) {
                                progress_old = progress;
                                cbOnProgress(update_id, progress);
                            }
                        }
                    }

                    if(rem_size > 0) {
                        rem_size -= c;
                    }
                }
                delay(1);
            }
            
            if(Update.end(true))
                console.success(UPDATE_T, "Update was successfully completed!");
            else
                console.error(UPDATE_T, "Update ended with errors");
            
            cbOnEnd(update_id, true);
            fwUpdating = false;
        }
    }
    else {
        console.error(UPDATE_T, "HTTP Error code: " + String(httpResponseCode));
        https.end();
        cbOnEnd(update_id, false);
        fwUpdating = false;
        return false;
    }

    https.end();
    return true;
}

bool updateFirmware(const char* url, String uuid, void(*cbProgress)(String uuid, uint8_t progress), void(*cbSuccess)(String uuid, bool success))
{
    HTTPClient https;
    WiFiClient wf;
    size_t bin_size, rem_size;
    float flt_progress;
    uint8_t progress, progress_old;
    char str[60];

    https.setTimeout(50000);
    https.begin(url);
    
    // Send HTTP GET request
    console.log(UPDATE_T, "Requesting firmware binary with GET request...");
    int httpResponseCode = https.GET();

    if(httpResponseCode > 0) {
        console.log(UPDATE_T, "HTTP Response code: " + String(httpResponseCode));
        bin_size = https.getSize();
        rem_size = bin_size;
        console.log(UPDATE_T, "HTTP Payload size: " + String(bin_size));
        WiFiClient * stream = https.getStreamPtr();

        // create buffer for read
        //uint8_t buff[2048] = { 0 };

        if(!Update.begin(bin_size)) { //start with max available size
            //Update.printError(Serial);
            console.error(UPDATE_T, "Error: " + String(Update.errorString()));
            cbSuccess(uuid, false);
            return false;
        }
        else {
            while(https.connected() && (rem_size > 0 || rem_size == -1))
            {
                // Get available data size
                size_t size = stream->available();

                if(size) {
                    // Read up to 2048 byte
                    // create buffer for read
                    uint8_t buff[2048] = { 0 };
                    //uint8_t * buff = (uint8_t*)calloc(2048, sizeof(uint8_t));
                    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                    int wc = Update.write(buff, c);
                    //free(buff);
                    //Serial.printf("Written %d (expected %d)\n", wc, c);
                    //sprintf(str, "Written %d (expected %d)", wc, c);
                    //console.log(UPDATE_T, String(str));
                    console.log(UPDATE_T, "Written " + String(wc) + " (expected " + String(c) + ")");

                    if(wc != c) {
                        //Update.printError(Serial);
                        console.error(UPDATE_T, "Error: " + String(Update.errorString()));
                        return false;
                    }
                    else {
                        flt_progress = (float)(bin_size - rem_size) / (float)(bin_size) * 100;
                        sprintf(str, "Remaining %d of %d (Progress = %3.1f %%)", rem_size, bin_size, flt_progress);
                        console.log(UPDATE_T, String(str));
                        progress = flt_progress;
                        console.log(UPDATE_T, "Progress = " + String(progress));
                        if(progress != progress_old) {
                            console.log(UPDATE_T, "Notify progress");
                            progress_old = progress;
                            cbProgress(uuid, progress);
                        }
                    }

                    if(rem_size > 0) {
                        rem_size -= c;
                    }
                }
                delay(1);
            }
            
            if(Update.end(true))
                console.success(UPDATE_T, "Update was successfully completed!");
            else
                console.error(UPDATE_T, "Update ended with errors");
            
            cbSuccess(uuid, true);
        }
    }
    else {
        console.error(UPDATE_T, "HTTP Error code: " + String(httpResponseCode));
        https.end();
        cbSuccess(uuid, false);
        return false;
    }

    https.end();
    return true;
}