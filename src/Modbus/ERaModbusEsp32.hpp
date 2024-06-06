#if !defined(INC_ERA_MODBUS_ESP32_HPP_) && \
    defined(ERA_MODBUS)
#define INC_ERA_MODBUS_ESP32_HPP_

#include <Modbus/ERaModbus.hpp>

#if !defined(UART_MODBUS)
    #define UART_MODBUS UART_NUM_1
#endif

#if !defined(UART_MODBUS_DATA_BITS)
    #define UART_MODBUS_DATA_BITS   UART_DATA_8_BITS
#endif

#if !defined(UART_MODBUS_STOP_BITS)
    #define UART_MODBUS_STOP_BITS   UART_STOP_BITS_1
#endif

#if !defined(UART_MODBUS_PARITY)
    #define UART_MODBUS_PARITY      UART_PARITY_DISABLE
#endif

inline
void ERaModbusStream::begin() {
    if (this->stream != NULL) {
        return;
    }
    if (this->initialized) {
        return;
    }

#if (ESP_IDF_VERSION_MAJOR >= 4)
    if (uart_is_driver_installed(UART_MODBUS)) {
        ERA_LOG_WARNING(TAG, ERA_PSTR("[Warning] UART %d installed, please setup another UART for Modbus!"), UART_MODBUS);
        return;
    }
#endif

    uart_config_t config = {
        .baud_rate = MODBUS_BAUDRATE,
        .data_bits = UART_MODBUS_DATA_BITS,
        .parity = UART_MODBUS_PARITY,
        .stop_bits = UART_MODBUS_STOP_BITS,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_param_config(UART_MODBUS, &config));
    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_ERROR_CHECK(uart_set_pin(UART_MODBUS, MODBUS_TXD_Pin, MODBUS_RXD_Pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_MODBUS, MODBUS_BUFFER_SIZE, MODBUS_BUFFER_SIZE, 10, (QueueHandle_t*)(&this->messageHandle), 0));
    this->initialized = true;
}

inline
void ERaModbusStream::end() {
    if (!this->initialized) {
        return;
    }

#if (ESP_IDF_VERSION_MAJOR >= 4)
    if (!uart_is_driver_installed(UART_MODBUS)) {
        return;
    }
#endif
    uart_flush(UART_MODBUS);
    ESP_ERROR_CHECK(uart_driver_delete(UART_MODBUS));
    this->initialized = false;
}

inline
void ERaModbusStream::setBaudRate(uint32_t baudrate) {
    ERaModbusBaudrate(baudrate);
    if (!this->initialized) {
        return;
    }

    uart_flush(UART_MODBUS);
    uart_set_baudrate(UART_MODBUS, baudrate);
}

inline
void ERaModbusStream::flushBytes() {
    if (this->stream != NULL) {
        return this->stream->flush();
    }

    uart_flush(UART_MODBUS);
}

inline
int ERaModbusStream::availableBytes() {
    if (this->stream != NULL) {
        return this->stream->available();
    }

    int length {0};
    uart_get_buffered_data_len(UART_MODBUS, (size_t*)&length);
    return length;
}

inline
void ERaModbusStream::sendByte(uint8_t byte) {
    ERaGuardLock(this->mutex);
    if (this->stream != NULL) {
        this->stream->write(byte);
    }
    else {
        SEND_UART(UART_MODBUS, &byte, 1);
        WAIT_SEND_UART_DONE(UART_MODBUS);
    }
    ERaGuardUnlock(this->mutex);
}

inline
void ERaModbusStream::sendBytes(const uint8_t* pData, size_t pDataLen) {
    ERaGuardLock(this->mutex);
    if (this->stream != NULL) {
        this->stream->write(pData, pDataLen);
    }
    else {
        SEND_UART(UART_MODBUS, const_cast<uint8_t*>(pData), pDataLen);
        WAIT_SEND_UART_DONE(UART_MODBUS);
    }
    ERaGuardUnlock(this->mutex);
}

inline
int ERaModbusStream::readByte() {
    if (this->stream != NULL) {
        return this->stream->read();
    }

    uint8_t byte {0};
    if (uart_read_bytes(UART_MODBUS, &byte, 1, this->timeout) <= 0) {
        return -1;
    }
    return byte;
}

inline
int ERaModbusStream::readBytes(uint8_t* buf, size_t size) {
    uint8_t* begin = buf;

    if (this->stream != NULL) {
        uint8_t* end = buf + size;
        MillisTime_t startMillis = ERaMillis();
        while ((begin < end) && ((ERaMillis() - startMillis) < this->timeout)) {
            int c = this->stream->read();
            if (c < 0) {
                continue;
            }
            *begin++ = (uint8_t)c;
        }
        return ((int)(begin - buf));
    }

    int32_t dataRead = 0;
    int32_t dataToRead = size;

    unsigned long startMillis = ERaMillis();
    while (dataToRead) {
        dataRead = uart_read_bytes(UART_MODBUS, begin, dataToRead, this->timeout);

        if (dataRead < 0) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Uart read error!!!"));
            return -1;
        }
        else if (dataRead > 0) {
            dataToRead -= dataRead;
            begin += dataRead;
        }
        else if (!ERaRemainingTime(startMillis, MODBUS_STREAM_TIMEOUT)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Uart read timeout!!!"));
            return -1;
        }
    }

    return size;
}

#endif /* INC_ERA_MODBUS_ESP32_HPP_ */
