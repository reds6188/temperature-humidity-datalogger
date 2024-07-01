#include "serial.h"

Uart::Uart() {
}

Uart::Uart(uart_port_t uart_num) {
    this->uart_num = uart_num;
}

void Uart::setNum(uart_port_t uart_num) {
    this->uart_num = uart_num;
}

esp_err_t Uart::begin(uart_pin_t uart_pin, uart_config_t uart_config) {
    String uTag = uart_tag[uart_num];

    if(uart_param_config(uart_num, &uart_config) != ESP_OK) {
        console.log(uTag, "Failed to configure UART" + String(uart_num));
        return ESP_FAIL;
    }

    //uart_set_mode(uart_num, UART_MODE_RS485_HALF_DUPLEX);
    
    if(uart_set_pin(uart_num, uart_pin.tx_pin, uart_pin.rx_pin, uart_pin.rts_pin, uart_pin.cts_pin) != ESP_OK) {
        console.log(uTag, "Failed to set pins for UART" + String(uart_num));
        return ESP_FAIL;
    }

    if(uart_driver_install(uart_num, BUF_SIZE, BUF_SIZE, QUEUE_SIZE, &uart_queue[uart_num], 0) != ESP_OK) {
        console.log(uTag, "Failed to install drivers for UART" + String(uart_num));
        return ESP_FAIL;
    }

    if(uart_pin.en_pin != UART_PIN_NO_CHANGE) {
        en_pin = uart_pin.en_pin;
        pinMode(uart_pin.en_pin, OUTPUT);       // Pin per il driver esterno
        digitalWrite(uart_pin.en_pin, HIGH);    // Driver in ricezione
    }

    return ESP_OK;
}

void Uart::end(void) {
    String uTag = uart_tag[uart_num];

    if(uart_wait_tx_done(uart_num, 100) == ESP_ERR_TIMEOUT)
        console.error(uTag, "UART TX FIFO Empty timeout error");

    if(uart_driver_delete(uart_num) != ESP_OK) {
        console.log(uTag, "Failed to delete drivers for UART" + String(uart_num));
    }
    else {
        console.log(uTag, "Drivers for UART" + String(uart_num) + " were deleted");
        vTaskDelete(xHandle);
    }
}

esp_err_t Uart::setHandler(void(*callback)(uint8_t *data, int data_size)) {

    BaseType_t result = pdPASS;
    this->handler = callback;
    xHandle = NULL;
    if(xTaskCreate(UartIrqHandler, "UART_ISR_ROUTINE", 2048, this, 12, &xHandle) == pdPASS) {
        return ESP_OK;
    }

    return ESP_FAIL;
}

void Uart::UartIrqHandler(void *pvParameters) {
    Uart *pThis = (Uart *) pvParameters;
    uart_event_t event;
    uint8_t *uart_data = (uint8_t*) malloc(RX_BUFFER_SIZE);
    uart_port_t uart_n = pThis->uart_num;
    //console.log(uart_tag[uart_n], "Rx handler was set");

	bool exit_condition = false;

    while(1)
    {
        if(xQueueReceive(uart_queue[uart_n], (void * )&event, (portTickType)portMAX_DELAY))
        {
            int uart_data_length = 0;
            bzero(uart_data, RX_BUFFER_SIZE);
            switch(event.type)
            {
                // Event of UART receving data
                case UART_DATA:
                    //int uart_data_length = 0;
                    uart_get_buffered_data_len(uart_n, (size_t*)&uart_data_length);
                    //Serial.printf("uart length: %d\n", uart_data_length);
                    if(uart_data_length > RX_BUFFER_SIZE) {
                        Serial.println("Length exceeds the maximum allowed");
                        uart_flush_input(uart_n);
                    }
                    else {
                        uart_data_length = uart_read_bytes(uart_n, uart_data, uart_data_length, 100);
                        if(pThis->handler != nullptr) {
                            (*(pThis->handler))(uart_data, uart_data_length);
                        }
                        else {
                            Serial.println("Pointer is null");
                        }
                    }
                    break;
                // Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(uart_n);
                    xQueueReset(uart_queue[uart_n]);
                    break;
                // Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    // If buffer full happened, you should consider increasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(uart_n);
                    xQueueReset(uart_queue[uart_n]);
                    break;
                // Event of UART RX break detected
                case UART_BREAK:
                    break;
                // Event of UART parity check error
                case UART_PARITY_ERR:
                    //pThis->uart_error.parity_err++;
                    break;
                // Event of UART frame error
                case UART_FRAME_ERR:
                    //pThis->uart_error.frame_err++;
                    break;
                case UART_PATTERN_DET:
                    break;
                default:
                    break;
            }



        }
        //If you want to break out of the loop due to certain conditions, set exit condition to true
        if(exit_condition) {
            break;
        }
    }

    //Out side of loop now. Task needs to clean up and self terminate before returning
    free(uart_data);
    uart_data = NULL;
    vTaskDelete(NULL);
}

void Uart::writeData(uint8_t *data, int data_size) {
    if(en_pin != UART_PIN_NO_CHANGE)
        digitalWrite(en_pin, HIGH);	// Driver interno in ricezione
	uart_write_bytes(uart_num, (const char*)data, data_size);
	uart_wait_tx_done(uart_num,100);	// Attendo che il buffer di trasmissione sia vuoto
    if(en_pin != UART_PIN_NO_CHANGE)
	    digitalWrite(en_pin, LOW);	// Driver esterno in ricezione
}

void Uart::printHexData(uint8_t *data, int data_size) {
	char hex_str[6];
	String output_str = ">>> ";

	if(data_size < RX_BUFFER_SIZE) {
		for(uint8_t i = 0 ; i < data_size ; i++) {
			sprintf(hex_str, "0x%02X ", *(data + i));
			output_str += String(hex_str);
		}
	}
	else
		output_str = "Failed to print packet: length exceeds maximum value";


	console.log(UART_T, output_str);
}

void Uart::printRawData(uint8_t *data, int data_size) {
	char chr_str[6];
	String output_str = ">>> ";

	if(data_size < RX_BUFFER_SIZE) {
		for(uint8_t i = 0 ; i < data_size ; i++) {
			sprintf(chr_str, "%c", *(data + i));
			output_str += String(chr_str);
		}
	}
	else
		output_str = "Failed to print packet: length exceeds maximum value";


	console.log(UART_T, output_str);
}

String Uart::getRawString(uint8_t *data, int data_size) {
	char chr_str[2];
	String output_str = ">>> ";

	if(data_size < RX_BUFFER_SIZE) {
		for(uint8_t i = 0 ; i < data_size ; i++) {
			sprintf(chr_str, "%c", *(data + i));
			output_str += String(chr_str);
		}
	}
	else
		output_str = "Failed to print packet: length exceeds maximum value";


	return output_str;
}