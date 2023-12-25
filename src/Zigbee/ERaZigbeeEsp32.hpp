#if !defined(INC_ERA_ZIGBEE_ESP32_HPP_) && \
    defined(ERA_ZIGBEE)
#define INC_ERA_ZIGBEE_ESP32_HPP_

#include <Zigbee/ERaZigbee.hpp>

#if !defined(UART_ZIGBEE)
    #if (SOC_UART_NUM > 2) || \
        (ESP_IDF_VERSION_MAJOR < 4)
        #define UART_ZIGBEE         UART_NUM_2
    #else
        #define UART_ZIGBEE         UART_NUM_1
    #endif
#endif

template <class Api>
void ERaZigbee<Api>::configZigbee() {
    if (this->initialized) {
        return;
    }

#if defined(ERA_MODBUS)
    #if ESP_IDF_VERSION_MAJOR < 4
        if (UART_ZIGBEE == UART_MODBUS) {
    #else
        if (uart_is_driver_installed(UART_ZIGBEE)) {
    #endif
        ZigbeeState::set(ZigbeeStateT::STATE_ZB_IGNORE);
        ERA_LOG(TAG, ERA_PSTR("[Warning] UART %d installed, please setup another UART for Zigbee!"), UART_ZIGBEE);
        ERA_LOG(TAG, ERA_PSTR("[Warning] Or disable Modbus first!"));
        ERA_LOG(TAG, ERA_PSTR("[Warning] Exclude header file ERa.hpp and Include specific header file ERaSimpleZBEsp32.hpp"));
        return;
    }
#endif
    uart_config_t config = {
        .baud_rate = ZIGBEE_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_ZIGBEE, &config));
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_ERROR_CHECK(uart_set_pin(UART_ZIGBEE, ZIGBEE_TXD_Pin, ZIGBEE_RXD_Pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_ZIGBEE, 2 * ZIGBEE_BUFFER_SIZE, ZIGBEE_BUFFER_SIZE, 50, (QueueHandle_t*)(&this->messageHandle), 0));
    this->initialized = true;
}

template <class Api>
void ERaZigbee<Api>::serialEnd() {
    if (!this->initialized) {
        return;
    }
#if (ESP_IDF_VERSION_MAJOR >= 4)
    if (!uart_is_driver_installed(UART_ZIGBEE)) {
        return;
    }
#endif
    uart_flush(UART_ZIGBEE);
    ESP_ERROR_CHECK(uart_driver_delete(UART_ZIGBEE));
    this->initialized = false;
}

template <class Api>
void ERaZigbee<Api>::handleZigbeeData() {
    uart_event_t event;
    if (ERaOs::osMessageQueueGet((QueueHandle_t)(this->messageHandle), &event, NULL, 1) != osOK) {
        return;
    }
    int length {0};
    uint8_t index {0};
    uint8_t payload[256] {0};
    do {
        if (index) {
            if (ERaOs::osMessageQueueGet((QueueHandle_t)(this->messageHandle), &event, NULL, DEFAULT_TIMEOUT) != osOK) {
                break;
            }
        }
        switch (event.type) {
            case uart_event_type_t::UART_DATA: {
                ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_ZIGBEE, (size_t*)&length));
                uint8_t receive[(length < 256) ? 256 : length] {0};
                uart_read_bytes(UART_ZIGBEE, receive, length, portMAX_DELAY);
                this->processZigbee(receive, length, 256, payload, index, (!index ? 0 : payload[this->PositionDataLength]));
            }
                break;
            case uart_event_type_t::UART_FIFO_OVF:
            case uart_event_type_t::UART_BUFFER_FULL:
                uart_flush_input(UART_ZIGBEE);
                ERaOs::osMessageQueueReset((QueueHandle_t)(this->messageHandle));
                break;
            default:
                break;
        }
    } while (index);
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::waitResponse(Response_t rspWait, void* value) {
    int length {0};
    uart_event_t event;
    
    if(!rspWait.timeout || rspWait.timeout > MAX_TIMEOUT) {
        rspWait.timeout = MAX_TIMEOUT;
    }
    uint8_t cmdStatus = ZnpCommandStatusT::INVALID_PARAM;

    MillisTime_t startMillis = ERaMillis();

    do {
        if (this->thisZigbee().isResponse()) {
            Response_t& rsp = this->thisZigbee().getResponse();
            if (CheckAFdata_t(rsp, rspWait)) {
                cmdStatus = rsp.cmdStatus;
            }
            if (CompareRsp_t(rsp, rspWait)) {
                return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_SUCCESSFUL);
            }
            if (CheckRsp_t(rsp, rspWait)) {
                // sync
                if (this->thisZigbee().queueRsp.writeable()) {
                    this->thisZigbee().queueRsp += rsp;
                }
            }
        }
        uint8_t index {0};
        uint8_t payload[256] {0};
        do {
            if (ERaOs::osMessageQueueGet((QueueHandle_t)(this->thisZigbee().messageHandle), &event, NULL, (!index ? 1 : MAX_TIMEOUT)) == osOK) {
                switch (event.type) {
                    case uart_event_type_t::UART_DATA: {
                        ESP_ERROR_CHECK(uart_get_buffered_data_len(UART_ZIGBEE, (size_t*)&length));
                        uint8_t receive[(length < 256) ? 256 : length] {0};
                        uart_read_bytes(UART_ZIGBEE, receive, length, portMAX_DELAY);
                        if (this->thisZigbee().processZigbee(receive, length, 256, payload, index,
                            (!index ? 0 : payload[this->thisZigbee().PositionDataLength]), &cmdStatus, &rspWait, value)) {
                            return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_SUCCESSFUL);
                        }
                    }
                        break;
                    case uart_event_type_t::UART_FIFO_OVF:
                    case uart_event_type_t::UART_BUFFER_FULL:
                        uart_flush_input(UART_ZIGBEE);
                        ERaOs::osMessageQueueReset((QueueHandle_t)(this->thisZigbee().messageHandle));
                        break;
                    default:
                        break;
                }
            }
            else {
                break;
            }
        } while (index);
        ERA_ZIGBEE_YIELD();
    } while (ERaRemainingTime(startMillis, rspWait.timeout));
    return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_TIMEOUT);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendByte(uint8_t byte) {
    ERaGuardLock(this->mutex);
    ERaLogHex("ZB >>", &byte, 1);
    SEND_UART(UART_ZIGBEE, &byte, 1);
    WAIT_SEND_UART_DONE(UART_ZIGBEE);
    ERaGuardUnlock(this->mutex);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendCommand(const vector<uint8_t>& data) {
    ERaGuardLock(this->mutex);
    ERaLogHex("ZB >>", data.data(), data.size());
    SEND_UART(UART_ZIGBEE, const_cast<uint8_t*>(data.data()), data.size());
    WAIT_SEND_UART_DONE(UART_ZIGBEE);
    ERaGuardUnlock(this->mutex);
}

#endif /* INC_ERA_ZIGBEE_ESP32_HPP_ */
