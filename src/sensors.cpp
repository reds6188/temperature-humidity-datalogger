#include "sensors.h"

Uart uart_232;

uart_pin_t UART_232_pin {
	.tx_pin = UART_232_TX,
	.rx_pin = UART_232_RX,
	.rts_pin = UART_PIN_NO_CHANGE,
	.cts_pin = UART_PIN_NO_CHANGE,
	.en_pin = UART_PIN_NO_CHANGE,
};

uart_config_t UART_232_config = {
	.baud_rate = 9600,
	.data_bits = UART_DATA_8_BITS,
	.parity = UART_PARITY_EVEN,
	.stop_bits = UART_STOP_BITS_1,
	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	.rx_flow_ctrl_thresh = 122,
};

TempUmidSensor::TempUmidSensor(void) {
}

TempUmidSensor::~TempUmidSensor(void) {
}

void TempUmidSensor::begin(void(*callback)(uint8_t *data, int size))  {
	uart_232.setNum(UART_232);
	if(uart_232.begin(UART_232_pin, UART_232_config) != ESP_OK) {
		console.error(SENS_T, "Error on UART initialization");
	}
	else {
		console.success(SENS_T, "UART was initialized successfully");
		uart_232.setHandler(callback);
	}
}

void TempUmidSensor::scan(void) {
	scanning = true;
}

void TempUmidSensor::setRxBuffer(uint8_t *data, int size) {
	if(size < 0) {
		console.error(SENS_T, "Wrong buffer size");
		return;
	}

	for(uint8_t i = 0 ; i < size ; i++)
		rxData[i] = *(data + i);

	receivedFlag = true;
}

void TempUmidSensor::parseRxBuffer(void) {
	uint8_t CRCrx;

	if(receivedFlag) {
		CRCrx = Check_nby(*(rxData+1)-1, rxData+1);

		if(CRCrx == *(rxData + *(rxData+1))) {
			uint8_t rxAddress = *(rxData+2);
			if(isAddress(rxAddress)) {
				uint8_t index = getIndex(rxAddress);
				setTemperature(index, *(rxData+3));
				setHumidity(index, *(rxData+4));
				//Led2.toggle();
			}
			else
				console.error(SENS_T, "Unknown Device code");
		}
		else
			console.error(SENS_T, "CRC error");
		
		receivedFlag = false;
		comError = false;
		TimerCom.trigger();
	}
}

void TempUmidSensor::refresh(void) {
	// Aggiorno la tastiera ogni 100 ms -------------------
	if(TimerRefresh.elapsedX100ms(1))
	{
		TimerRefresh.trigger();
		if(scanning) {

		}
		else {
			next();
			sendRequest(sens_list[sens_index].address);
		}
	}
}

void TempUmidSensor::sendRequest(uint8_t address)
{
	txData[0] = START_SYMB;
	txData[1] = 3;
	txData[2] = address;
	txData[3] = Check_nby(txData[1]-1, txData+1);	//non considero i byte di start

	uart_232.writeData(txData, 4);
}

void TempUmidSensor::next(void)
{
	if(sens_index < sens_num)
		sens_index++;
	else
		sens_index = 0;
}

void TempUmidSensor::loop(void) {
	refresh();
	parseRxBuffer();

	if(!comError && TimerCom.elapsedX1s(30)) {
		comError = true;
		console.error(SENS_T, "Communication broken with sensors");
	}
}

bool TempUmidSensor::isAddress(uint8_t value) {
	for(uint8_t i=0 ; i<SENSORS_NUM_MAX ; i++) {
		if(value == (SENSOR_BASE_ADDR + i))
			return true;
	}
	return false;
}

sensor_t TempUmidSensor::getSensor(uint8_t address) {
	for(uint8_t i=0 ; i<sens_num ; i++) {
		if(address == sens_list[i].address)
			return sens_list[i];
	}
}

uint8_t TempUmidSensor::getIndex(uint8_t address) {
	for(uint8_t i=0 ; i<sens_num ; i++) {
		if(address == sens_list[i].address)
			return i;
	}
}

void TempUmidSensor::setTemperature(uint8_t index, uint8_t value) {
	sens_list[index].temperature = value;
}

uint8_t TempUmidSensor::getTemperature(uint8_t index) {
	return sens_list[index].temperature;
}

void TempUmidSensor::setHumidity(uint8_t index, uint8_t value) {
	sens_list[index].humidity = value;
}

uint8_t TempUmidSensor::getHumidity(uint8_t index) {
	return sens_list[index].humidity;
}