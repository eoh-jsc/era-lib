#ifndef INC_ERA_ZIGBEE_ESP32_HPP_
#define INC_ERA_ZIGBEE_ESP32_HPP_

#include <Zigbee/ERaZigbee.hpp>

#define UART_ZIGBEE         UART_NUM_2

template <class Api>
void ERaZigbee<Api>::configZigbee() {
    uart_config_t config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_ZIGBEE, &config));
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_ERROR_CHECK(uart_set_pin(UART_ZIGBEE, ZIGBEE_TXD_Pin, ZIGBEE_RXD_Pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_ZIGBEE, 2 * ZIGBEE_BUFFER_SIZE, ZIGBEE_BUFFER_SIZE, 50, (QueueHandle_t*)(&this->messageHandle), 0));
}

template <class Api>
void ERaZigbee<Api>::handleZigbeeData() {
    int length {0};
    uart_event_t event;
    if (osMessageQueueGet((QueueHandle_t)(this->messageHandle), &event, NULL, 1) != osOK) {
        return;
    }
    switch (event.type) {
        case uart_event_type_t::UART_DATA: {
            ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_ZIGBEE, (size_t*)&length));
			uint8_t receive[length < 256 ? 256 : length] {0};
			uint8_t payload[length < 256 ? 256 : length] {0};
            uart_read_bytes(UART_ZIGBEE, receive, length, portMAX_DELAY);
            this->processZigbee(receive, length, (length < 256 ? 256 : length), payload, 0, 0);
        }
            break;
        case uart_event_type_t::UART_FIFO_OVF:
        case uart_event_type_t::UART_BUFFER_FULL:
            uart_flush_input(UART_ZIGBEE);
            osMessageQueueReset((QueueHandle_t)(this->messageHandle));
            break;
        default:
            break;
    }
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::waitResponse(Response_t rspWait, void* value) {
    int length {0};
    uart_event_t event;
    Response_t rsp {
        .nwkAddr = NO_NWK_ADDR,
        .type = TypeT::ERR,
        .subSystem = 0,
        .command = 0,
        .zclId = ClusterIDT::NO_CLUSTER_ID,
        .transId = 0,
        .transIdZcl = 0,
        .cmdStatus = ZnpCommandStatusT::INVALID_PARAM
    };
    
	if(!rspWait.timeout || rspWait.timeout > MAX_TIMEOUT) {
		rspWait.timeout = MAX_TIMEOUT;
    }
    uint8_t cmdStatus = ZnpCommandStatusT::INVALID_PARAM;

    unsigned long startMillis = ERaMillis();

    do {
        if (osMessageQueueGet((QueueHandle_t)(static_cast<Zigbee*>(this)->messageHandle), &event, NULL, 1) == osOK) {
            switch (event.type) {
                case uart_event_type_t::UART_DATA: {
                    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_ZIGBEE, (size_t*)&length));
					uint8_t receive[length < 256 ? 256 : length] {0};
					uint8_t payload[length < 256 ? 256 : length] {0};
                    uart_read_bytes(UART_ZIGBEE, receive, length, portMAX_DELAY);
                    if (static_cast<Zigbee*>(this)->processZigbee(receive, length, (length < 256 ? 256 : length), payload, 0, 0, &cmdStatus, &rspWait, value)) {
                        return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_SUCCESSFUL);
                    }
                }
                    break;
                case uart_event_type_t::UART_FIFO_OVF:
                case uart_event_type_t::UART_BUFFER_FULL:
                    uart_flush_input(UART_ZIGBEE);
                    osMessageQueueReset((QueueHandle_t)(static_cast<Zigbee*>(this)->messageHandle));
                    break;
                default:
                    break;
            }
        }
        ERaDelay(10);
    } while (ERaRemainingTime(startMillis, rspWait.timeout));
    return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_TIMEOUT);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendByte(uint8_t byte) {
    ERaGuardLock(this->mutex);
	SEND_UART(UART_ZIGBEE, &byte, 1);
	WAIT_SEND_UART_DONE(UART_ZIGBEE);
    ERaGuardUnlock(this->mutex);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendCommand(const vector<uint8_t>& data) {
    ERaGuardLock(this->mutex);
	SEND_UART(UART_ZIGBEE, const_cast<uint8_t*>(data.data()), data.size());
	WAIT_SEND_UART_DONE(UART_ZIGBEE);
    ERaGuardUnlock(this->mutex);
}

#endif /* INC_ERA_ZIGBEE_ESP32_HPP_ */
