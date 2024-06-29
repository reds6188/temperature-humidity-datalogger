#include "wi-fi.h"

long start_wifi_millis;

const char * ap_ssid = "Disenia AP";
const char * ap_password = "123456789";
const char * default_ssid = "Default_SSID";
const char * default_password = "DefaultPassword";

WiFiHandler::WiFiHandler(void) {
}

bool WiFiHandler::setCredentials(const char* ssid, const char* password, bool saveOnMemory)
{
    if(saveOnMemory) {
        if(!credentials.begin("credentials", false)) {
            console.error(WIFI_T, "Failed to write credentials in permanent memory");
            return false;
        }

        credentials.putString("ssid", ssid);
        credentials.putString("password", password);
        credentials.end();
        console.success(WIFI_T, "Credentials was set on memory!");
    }
	strcpy(this->ssid, ssid);
	strcpy(this->password, password);
    console.success(WIFI_T, "Credentials was set successfully!");
    console.info(WIFI_T, "New SSID is: " + String(ssid));
    return true;
}

void WiFiHandler::readCredentials(void)
{
    String ssid, password;

    if(credentials.begin("credentials", true)) {
        ssid = credentials.getString("ssid", String(default_ssid)); 
        password = credentials.getString("password", String(default_password));
        credentials.end();
    }
    else {
		console.error(WIFI_T, "Failed to read credentials from permanent memory");
		ssid = String(default_ssid);
		password = String(default_password);
	}

    if(ssid == "" || password == "") {
        console.error(WIFI_T, "No values saved for ssid or password");
    }

	strcpy(this->ssid, ssid.c_str());
	strcpy(this->password, password.c_str());

    console.success(WIFI_T, "Credentials was read successfully!");
    console.info(WIFI_T, "New SSID is: " + String(ssid));
	//console.info(WIFI_T, "New Password is: " + String(password));
}

void WiFiHandler::begin(wifi_mode_t mode) {
    console.info(WIFI_T, "Starting Wi-Fi configuration...");
	if(mode == WIFI_AP)
	{
		IPAddress localIP(192, 168, 4, 1);		// Set your Static IP address
		IPAddress gatewayAddr(192, 168, 4, 0);	// Set your Gateway IP address
		IPAddress subnetMask(255, 255, 255, 0);	// Set your Subnet Mask

		console.log(WIFI_T, "Setting AP (Access Point)...");
		WiFi.mode(WIFI_AP);

		if(!WiFi.softAP(ap_ssid, NULL, 1, 0, 1, false))
			console.error(WIFI_T, "AP failed to start");
        delay(100);
		if(!WiFi.softAPConfig(localIP, gatewayAddr, subnetMask))
			console.error(WIFI_T, "AP configuration failed");
		else
		{
			console.success(WIFI_T, "AP started!");
			console.info(WIFI_T, "Network SSID: " + String(ap_ssid));
			IPAddress IP = WiFi.softAPIP();
			console.info(WIFI_T, "AP IP address: " + IP.toString());
		}
	}
	else if(mode == WIFI_STA)
	{
		console.info(WIFI_T, "Setting STA (STAtion mode)...");
        readCredentials();
		WiFi.mode(WIFI_STA);
		WiFi.begin(ssid, password);
	}
	else
	{
		console.error(WIFI_T, "ERROR! Invalid Wi-Fi operating mode");
	}
}

wifi_mode_t WiFiHandler::getMode(void) {
	return WiFi.getMode();
}

void WiFiHandler::end(void) {
    WiFi.disconnect();
}

unsigned long tmr;

void WiFiHandler::loop(void) {
	wl_status_t curr_status = WiFi.status();

	if(scanning > SCAN_IDLE) {
		if(scanning == SCAN_START) {
			int16_t scan_res = WiFi.scanComplete();
			if(scan_res != WIFI_SCAN_RUNNING) {
				console.info(WIFI_T, "Start sanning for new networks...");
				int16_t res = WiFi.scanNetworks(true, false, false, 300, 0, NULL, NULL);
				if(res == WIFI_SCAN_RUNNING ) {
					console.success(WIFI_T, "Scan is running");
					scanning = SCAN_RUN;
				}
				else if(res == WIFI_SCAN_FAILED) {
					console.error(WIFI_T, "Failed to start running");
					scanning = SCAN_IDLE;
				}
				else {
					console.success(WIFI_T, "Scan completed");
					scanning = SCAN_IDLE;
				}
			}
			else
				scanning = SCAN_RUN;
		}
		else {	// scanning == SCAN_RUN
			if(millis() - wifi_timeout > 1000) {
				wifi_timeout = millis();
				int16_t scan_res = WiFi.scanComplete();
				if(scan_res != WIFI_SCAN_RUNNING) {
					if(scan_res == WIFI_SCAN_FAILED)
						console.error(WIFI_T, "Scan failed!");
					else
						console.success(WIFI_T, "Scan completed");
					scanning = SCAN_IDLE;
				}
				else {
					console.info(WIFI_T, "Scan is running...");
				}
			}
		}
	}
	else {
		if(curr_status != WL_CONNECTED)
		{
			if(millis() - wifi_timeout > WIFI_TIMEOUT) {
				//if(!scanning) {
					console.error(WIFI_T, "Timeout has expired.");
					WiFi.reconnect();
					wifi_timeout = millis();
					console.warning(WIFI_T, "Trying to connect... ");
				//}
			}
		}
	}
}


void WiFiHandler::onEvent(void cbOnEvent(WiFiEvent_t event, WiFiEventInfo_t info), WiFiEvent_t event) {
	WiFi.onEvent(cbOnEvent, event);
}

String WiFiHandler::getSSID(void) {
	return String(ssid);
}

bool WiFiHandler::isEncrypted(void) {
	return (strlen(password) != 0);
}

int8_t WiFiHandler::getRSSI(void) {
	return WiFi.RSSI();
}

String WiFiHandler::getLocalIP(void) {
    IPAddress IP = WiFi.localIP();
	return IP.toString();
}

String WiFiHandler::getMacAddress(wifi_interface_t interface) {
    uint8_t mac[6];
    char macStr[18] = { 0 };
	if(esp_wifi_get_mac(interface, mac) == ESP_OK)
    	sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return String(macStr);
}

void WiFiHandler::printMacAddress(wifi_interface_t interface) {

	if(interface == WIFI_IF_STA)
		console.info(WIFI_T, "Wi-Fi MAC Address (STA): " + getMacAddress(interface));
	else if(interface == WIFI_IF_AP)
		console.info(WIFI_T, "Wi-Fi MAC Address (STA): " + getMacAddress(interface));
	else
		console.error(WIFI_T, "Unknown Wi-Fi interface");
}

bool WiFiHandler::connected(void) {
	if(WiFi.status() == WL_CONNECTED)
		return true;

	return false;
}

/*
void WiFiHandler::scanNetworks(void)
{
    console.info(WIFI_T, "Scanning for new networks...");
	scanning = true;
	if(WiFi.status() != WL_CONNECTED)
		WiFi.disconnect();
	int16_t n = WiFi.scanNetworks(true, false, false, 300, 0, NULL, NULL);
}
*/

void WiFiHandler::scanNetworks(void) {
	console.info(WIFI_T, "Starting new scannning...");
    if(scanning == SCAN_IDLE)
		scanning = SCAN_START;
}

void WiFiHandler::setScanDone(void) {
	scanning = SCAN_IDLE;
}

wifi_scan_t WiFiHandler::getScanStatus(void) {
	return scanning;
}

/*
bool WiFiHandler::scanCompleted(void)
{
	return (WiFi.scanComplete() >= 0);
}
*/

/*
bool WiFiHandler::scanRunning(void)
{
	return (WiFi.scanComplete() == WIFI_SCAN_RUNNING);
}
*/

void WiFiHandler::readNetworkList(void) {
	wifi_list_size = WiFi.scanComplete();
	if(!wifi_list_size)
		console.error(WIFI_T, "No network found!");
	else {
		console.info(WIFI_T, String(wifi_list_size) + " network(s) found:");
		for(int16_t i=0 ; i<wifi_list_size ; i++)
		{
			String encriptionType = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*";
			String rssi = String(WiFi.RSSI(i));
			console.data(WIFI_T, String(i) + ": " + WiFi.SSID(i) + " (" + rssi + " dBm) " + encriptionType);
			strcpy(wifi_list[i].ssid, WiFi.SSID(i).c_str());
			wifi_list[i].rssi = WiFi.RSSI(i);
			wifi_list[i].encripted = !(WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
		}
		console.info(WIFI_T, "Note: networks with \"*\" char are encrypted");
	}
}

network_t WiFiHandler::getNetworkInfo(uint8_t index) {
	return wifi_list[index];
}

uint8_t WiFiHandler::getNetworkListSize(void) {
	return wifi_list_size;
}

void WiFiHandler::startMDNS(const char* host_name) {
    if(MDNS.begin(host_name))
    {
        MDNS.addService("http", "tcp", 80);
        console.success(WIFI_T, "MDNS start, open the browser and type \"http://" + String(host_name) + ".local\"");
    }
    else
        console.error(WIFI_T, "MDNS failed to start");
}

void WiFiHandler::stopMDNS(void) {
    MDNS.end();
    console.warning(WIFI_T, "MDNS has stopped");
}