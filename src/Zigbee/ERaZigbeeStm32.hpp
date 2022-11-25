#ifndef INC_ERA_ZIGBEE_STM32_HPP_
#define INC_ERA_ZIGBEE_STM32_HPP_

#include <Zigbee/ERaZigbee.hpp>

HardwareSerial SerialZB(PB11, PB10);

template <class Api>
void ERaZigbee<Api>::configZigbee() {
    if (this->stream != NULL) {
        return;
    }

    this->stream = &SerialZB;
    SerialZB.begin(9600);
}

template <class Api>
void ERaZigbee<Api>::handleZigbeeData() {
    if (this->stream == NULL) {
        return;
    }

    if (!this->stream->available()) {
        return;
    }
    uint8_t index {0};
    uint8_t payload[256] {0};
    MillisTime_t startMillis = ERaMillis();
    do {
        int position {0};
        int length = this->stream->available();
        uint8_t receive[(length < 256) ? 256 : length] {0};
        ERaGuardLock(this->mutexData);
        do {
            receive[position] = this->stream->read();
            if (position) {
                if (receive[this->PositionSOF] == this->SOF) {
                    length = receive[this->PositionDataLength] + this->MinMessageLength;
                }
                else {
                    length = 0;
                    break;
                }
            }
        } while ((++position < length) && ERaRemainingTime(startMillis, DEFAULT_TIMEOUT));
        ERaGuardUnlock(this->mutexData);
        this->processZigbee(receive, length, 256, payload, index, (!index ? 0 : payload[this->PositionDataLength]));
        if (index) {
            ERA_ZIGBEE_YIELD();
        }
    } while (index && this->stream->available());
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::waitResponse(Response_t rspWait, void* value) {
    if (static_cast<Zigbee*>(this)->stream == NULL) {
        return ResultT::RESULT_FAIL;
    }
    
	if(!rspWait.timeout || rspWait.timeout > MAX_TIMEOUT) {
		rspWait.timeout = MAX_TIMEOUT;
    }
    uint8_t cmdStatus = ZnpCommandStatusT::INVALID_PARAM;

    MillisTime_t startMillis = ERaMillis();

    do {
        if (static_cast<Zigbee*>(this)->isResponse()) {
            Response_t& rsp = static_cast<Zigbee*>(this)->getResponse();
            if (CheckAFdata_t(rsp, rspWait)) {
                cmdStatus = rsp.cmdStatus;
            }
            if (CompareRsp_t(rsp, rspWait)) {
                return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_SUCCESSFUL);
            }
            if (CheckRsp_t(rsp, rspWait)) {
                // sync
                if (static_cast<Zigbee*>(this)->queueRsp.writeable()) {
                    static_cast<Zigbee*>(this)->queueRsp += rsp;
                }
            }
        }
        if (!static_cast<Zigbee*>(this)->stream->available()) {
            ERA_ZIGBEE_YIELD();
            continue;
        }
        uint8_t index {0};
        uint8_t payload[256] {0};
        do {
            int position = 0;
            int length = static_cast<Zigbee*>(this)->stream->available();
            uint8_t receive[(length < 256) ? 256 : length] {0};
            ERaGuardLock(static_cast<Zigbee*>(this)->mutexData);
            do {
                receive[position] = static_cast<Zigbee*>(this)->stream->read();
                if (position) {
                    if (receive[static_cast<Zigbee*>(this)->PositionSOF] == static_cast<Zigbee*>(this)->SOF) {
                        length = receive[static_cast<Zigbee*>(this)->PositionDataLength] + static_cast<Zigbee*>(this)->MinMessageLength;
                    }
                    else {
                        length = 0;
                        break;
                    }
                }
            } while ((++position < length) && ERaRemainingTime(startMillis, rspWait.timeout));
            ERaGuardUnlock(static_cast<Zigbee*>(this)->mutexData);
            if (static_cast<Zigbee*>(this)->processZigbee(receive, length, 256, payload, index, (!index ? 0 : payload[static_cast<Zigbee*>(this)->PositionDataLength]), &cmdStatus, &rspWait, value)) {
                return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_SUCCESSFUL);
            }
            if (index) {
                ERA_ZIGBEE_YIELD();
            }
        } while (index && static_cast<Zigbee*>(this)->stream->available());
        ERA_ZIGBEE_YIELD();
    } while (ERaRemainingTime(startMillis, rspWait.timeout));
    return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_TIMEOUT);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendByte(uint8_t byte) {
    if (static_cast<Zigbee*>(this)->stream == NULL) {
        return;
    }

    ERaGuardLock(this->mutex);
    ERaLogHex("ZB >>", &byte, 1);
    static_cast<Zigbee*>(this)->stream->write(byte);
    static_cast<Zigbee*>(this)->stream->flush();
    ERaGuardUnlock(this->mutex);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendCommand(const vector<uint8_t>& data) {
    if (static_cast<Zigbee*>(this)->stream == NULL) {
        return;
    }

    ERaGuardLock(this->mutex);
    ERaLogHex("ZB >>", data.data(), data.size());
    static_cast<Zigbee*>(this)->stream->write(data.data(), data.size());
    static_cast<Zigbee*>(this)->stream->flush();
    ERaGuardUnlock(this->mutex);
}

#endif /* INC_ERA_ZIGBEE_STM32_HPP_ */
