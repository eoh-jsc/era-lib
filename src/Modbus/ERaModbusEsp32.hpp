#ifndef INC_ERA_MODBUS_ESP32_HPP_
#define INC_ERA_MODBUS_ESP32_HPP_

#include <Modbus/ERaModbus.hpp>

#if !defined(UART_MODBUS)
    #define UART_MODBUS UART_NUM_1
#endif

template <class Api>
void ERaModbus<Api>::configModbus() {
    if (this->stream != NULL) {
        return;
    }

    uart_config_t config = {
        .baud_rate = MODBUS_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_MODBUS, &config));
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_ERROR_CHECK(uart_set_pin(UART_MODBUS, MODBUS_TXD_Pin, MODBUS_RXD_Pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_MODBUS, MODBUS_BUFFER_SIZE, MODBUS_BUFFER_SIZE, 10, (QueueHandle_t*)(&this->messageHandle), 0));
    this->_streamDefault = true;
}

template <class Api>
void ERaModbus<Api>::setBaudRate(uint32_t baudrate) {
    ERaModbusBaudrate(baudrate);
    if (!this->streamDefault()) {
        return;
    }

    uart_flush(UART_MODBUS);
    uart_set_baudrate(UART_MODBUS, baudrate);
}

template <class Api>
bool ERaModbus<Api>::waitResponse(ERaModbusResponse* response) {
    if (response == nullptr) {
        return false;
    }

    int length {0};
    uart_event_t event;

    if (this->total++ > 99) {
        this->total = 1;
        this->failRead = 0;
        this->failWrite = 0;
    }

    MillisTime_t startMillis = ERaMillis();

    if (this->stream != NULL) {
        do {
            if (!this->stream->available()) {
#if defined(ERA_NO_RTOS)
                ERaOnWaiting();
                this->thisApi().run();
#endif
                if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                    break;
                }
                ERA_MODBUS_YIELD();
                continue;
            }

            do {
                response->add(this->stream->read());
            } while (this->stream->available());

            if (response->isComplete()) {
                ERaLogHex("MB <<", response->getMessage(), response->getPosition());
                return response->isSuccess();
            }
            ERA_MODBUS_YIELD();
        } while (ERaRemainingTime(startMillis, MAX_TIMEOUT_MODBUS));
        return false;
    }

    do {
        if (ERaOs::osMessageQueueGet((QueueHandle_t)(this->messageHandle), &event, NULL, 1) == osOK) {
            switch (event.type) {
                case uart_event_type_t::UART_DATA: {
                    ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_MODBUS, (size_t*)&length));
                    uint8_t receive[length] {0};
                    uart_read_bytes(UART_MODBUS, receive, length, portMAX_DELAY);
                    for (int i = 0; i < length; ++i) {
                        response->add(receive[i]);
                    }
                    if (response->isComplete()) {
                        ERaLogHex("MB <<", response->getMessage(), response->getPosition());
                        return response->isSuccess();
                    }
                }
                    break;
                case uart_event_type_t::UART_FIFO_OVF:
                case uart_event_type_t::UART_BUFFER_FULL:
                    uart_flush_input(UART_MODBUS);
                    ERaOs::osMessageQueueReset((QueueHandle_t)(this->messageHandle));
                    break;
                default:
                    break;
            }
        }
        else {
#if defined(ERA_NO_RTOS)
            ERaOnWaiting();
            this->thisApi().run();
#endif
            if (ModbusState::is(ModbusStateT::STATE_MB_PARSE)) {
                break;
            }
        }
        ERA_MODBUS_YIELD();
    } while (ERaRemainingTime(startMillis, MAX_TIMEOUT_MODBUS));
    return false;
}

template <class Api>
void ERaModbus<Api>::sendCommand(uint8_t* data, size_t size) {
    if (data == nullptr) {
        return;
    }

    ERaLogHex("MB >>", data, size);
    this->switchToTransmit();
    if (this->stream != NULL) {
        this->stream->write(data, size);
        this->stream->flush();
    }
    else {
        SEND_UART(UART_MODBUS, data, size);
        WAIT_SEND_UART_DONE(UART_MODBUS);
        FLUSH_UART(UART_MODBUS);
    }
    this->switchToReceive();
}

#endif /* INC_ERA_MODBUS_ESP32_HPP_ */
