#ifndef INC_ERA_ZIGBEE_LINUX_HPP_
#define INC_ERA_ZIGBEE_LINUX_HPP_

#include <Zigbee/ERaZigbee.hpp>

template <class Api>
void ERaZigbee<Api>::configZigbee() {
    if (this->fd < 0) {
        this->fd = serialOpen("/dev/ttyACM0", ZIGBEE_BAUDRATE);
    }
}

template <class Api>
void ERaZigbee<Api>::handleZigbeeData() {
    if (this->fd < 0) {
        return;
    }

    if (!serialDataAvail(this->fd)) {
        return;
    }
    int remain {0};
    uint8_t index {0};
    uint8_t payload[256] {0};
    MillisTime_t startMillis = ERaMillis();
    do {
        if (!serialDataAvail(this->fd)) {
            ERA_ZIGBEE_YIELD();
            continue;
        }
        int position {0};
        int length = serialDataAvail(this->fd);
        uint8_t receive[(length < 256) ? 256 : length] {0};
        ERaGuardLock(this->mutexData);
        do {
            receive[position] = serialGetchar(this->fd);
            if (index && !position) {
                length = remain;
            }
            else if (!index && (position == this->PositionDataLength)) {
                if (receive[this->PositionSOF] == this->SOF) {
                    length = receive[this->PositionDataLength] + this->MinMessageLength;
                }
                else {
                    length = 0;
                }
            }
        } while ((++position < length) && ERaRemainingTime(startMillis, DEFAULT_TIMEOUT));
        ERaGuardUnlock(this->mutexData);
        this->processZigbee(receive, length, 256, payload, index, (!index ? 0 : payload[this->PositionDataLength]));
        if (index) {
            if (!length) {
                index = 0;
            }
            remain = length - index;
            ERA_ZIGBEE_YIELD();
        }
    } while (index && ERaRemainingTime(startMillis, DEFAULT_TIMEOUT));
}

template <class Zigbee>
ResultT ERaToZigbee<Zigbee>::waitResponse(Response_t rspWait, void* value) {
    if (this->thisZigbee().fd < 0) {
        return ResultT::RESULT_FAIL;
    }
    
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
        if (!serialDataAvail(this->thisZigbee().fd)) {
            ERA_ZIGBEE_YIELD();
            continue;
        }
        int remain {0};
        uint8_t index {0};
        uint8_t payload[256] {0};
        do {
            if (!serialDataAvail(this->thisZigbee().fd)) {
                ERA_ZIGBEE_YIELD();
                continue;
            }
            int position = 0;
            int length = serialDataAvail(this->thisZigbee().fd);
            uint8_t receive[(length < 256) ? 256 : length] {0};
            ERaGuardLock(this->thisZigbee().mutexData);
            do {
                receive[position] = serialGetchar(this->thisZigbee().fd);
                if (index && !position) {
                    length = remain;
                }
                else if (!index && (position == this->thisZigbee().PositionDataLength)) {
                    if (receive[this->thisZigbee().PositionSOF] == this->thisZigbee().SOF) {
                        length = receive[this->thisZigbee().PositionDataLength] + this->thisZigbee().MinMessageLength;
                    }
                    else {
                        length = 0;
                    }
                }
            } while ((++position < length) && ERaRemainingTime(startMillis, rspWait.timeout));
            ERaGuardUnlock(this->thisZigbee().mutexData);
            if (this->thisZigbee().processZigbee(receive, length, 256, payload, index,
                (!index ? 0 : payload[this->thisZigbee().PositionDataLength]), &cmdStatus, &rspWait, value)) {
                return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_SUCCESSFUL);
            }
            if (index) {
                if (!length) {
                    index = 0;
                }
                remain = length - index;
                ERA_ZIGBEE_YIELD();
            }
        } while (index && ERaRemainingTime(startMillis, rspWait.timeout));
        ERA_ZIGBEE_YIELD();
    } while (ERaRemainingTime(startMillis, rspWait.timeout));
    return ((cmdStatus != ZnpCommandStatusT::INVALID_PARAM) ? static_cast<ResultT>(cmdStatus) : ResultT::RESULT_TIMEOUT);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendByte(uint8_t byte) {
    if (this->thisZigbee().fd < 0) {
        return;
    }

    ERaGuardLock(this->mutex);
    ERaLogHex("ZB >>", &byte, 1);
    serialPutchar(this->thisZigbee().fd, byte);
    serialFlush(this->thisZigbee().fd);
    ERaGuardUnlock(this->mutex);
}

template <class Zigbee>
void ERaToZigbee<Zigbee>::sendCommand(const vector<uint8_t>& data) {
    if (this->thisZigbee().fd < 0) {
        return;
    }

    ERaGuardLock(this->mutex);
    ERaLogHex("ZB >>", data.data(), data.size());
    for (const auto& var : data) {
        serialPutchar(this->thisZigbee().fd, var);
    }
    serialFlush(this->thisZigbee().fd);
    ERaGuardUnlock(this->mutex);
}

#endif /* INC_ERA_ZIGBEE_LINUX_HPP_ */
