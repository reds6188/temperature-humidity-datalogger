#ifndef WI_FI_H_
#define WI_FI_H_

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <console.h>
#include <esp_wifi.h>

#define WIFI_T          "WI-FI" // Tag for log

#define WIFI_TIMEOUT    10000   // in milliseconds

typedef struct {
    char ssid[32];
    int8_t rssi;
    bool encripted;
}network_t;

typedef enum {
    SCAN_IDLE,
    SCAN_START,
    SCAN_RUN
}wifi_scan_t;

class WiFiHandler {
    private:
        Preferences credentials;
        char ssid[32];
        char password[64];
        wl_status_t status;
        int16_t scan_status;
        unsigned long wifi_timeout;
        wifi_scan_t scanning;
        uint8_t wifi_list_size;
        network_t wifi_list[32];
        //void (*cbOnScanCompleted)(network_t * list, size_t list_size);
    public:
        WiFiHandler(void);
        bool setCredentials(const char* ssid, const char* password, bool saveOnMemory);
        void readCredentials(void);
        void begin(wifi_mode_t mode);
        wifi_mode_t getMode(void);
        void end(void);
        void loop(void);
        void onEvent(void cbOnEvent(WiFiEvent_t event, WiFiEventInfo_t info), WiFiEvent_t event);
        String getSSID(void);
        bool isEncrypted(void);
        int8_t getRSSI(void);
        String getLocalIP(void);
        String getMacAddress(wifi_interface_t interface);
        void printMacAddress(wifi_interface_t interface);
        bool connected(void);
        void scanNetworks(void);
        wifi_scan_t getScanStatus(void);
        void setScanDone(void);
        //bool scanCompleted(void);
        //bool scanRunning(void);
        void readNetworkList(void);
        network_t getNetworkInfo(uint8_t index);
        uint8_t getNetworkListSize(void);
        void startMDNS(const char* host_name);
        void stopMDNS(void);
};

#endif  /* WI_FI_H_ */