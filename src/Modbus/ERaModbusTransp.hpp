#ifndef INC_ERA_MODBUS_TRANSPORT_HPP_
#define INC_ERA_MODBUS_TRANSPORT_HPP_

#include <cmath>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusMessage.hpp>

class ERaModbusResponse;

class ERaModbusRequest
    : public ERaModbusMessage
{
    friend class ERaModbusResponse;

public:
    ERaModbusRequest(uint8_t _transp, uint8_t _length)
        : ERaModbusMessage(_length)
        , transp(_transp)
        , packetId(0)
        , slaveAddr(0)
        , function(0)
        , addr(0)
        , len(0)
    {
        if (this->transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) {
            return;
        }
        if (++lastPacketId == 0) {
            lastPacketId = 1;
        }
        this->packetId = lastPacketId;
    }

    virtual uint8_t responseLength() = 0;

    uint16_t getPacketId() {
        return this->packetId;
    }

    uint8_t getFunction() {
        return this->function;
    }

    uint16_t getLength() {
        return this->len;
    }

protected:
    bool isRTU() {
        return (this->transp == ModbusTransportT::MODBUS_TRANSPORT_RTU);
    }

    uint8_t getSlaveAddress() {
        return this->slaveAddr;
    }

    uint16_t getAddress() {
        return this->addr;
    }

    uint16_t modbusCRC(uint8_t* buf, size_t len) {
        uint16_t crc = 0xFFFF;

        for (size_t pos = 0; pos < len; pos++) {
            crc ^= (uint16_t)buf[pos];

            for (int i = 8; i != 0; i--) {
                if ((crc & 0x0001) != 0) {
                    crc >>= 1;
                    crc ^= 0xA001;
                }
                else {
                    crc >>= 1;
                }
            }
        }

        return crc;
    }

    static uint16_t lastPacketId;
    uint8_t transp;
    uint16_t packetId;
    uint8_t slaveAddr;
    uint8_t function;
    uint16_t addr;
    uint16_t len;
};

uint16_t ERaModbusRequest::lastPacketId = 0;

class ERaModbusResponse
    : public ERaModbusMessage
{
public:
    ERaModbusResponse(ERaModbusRequest* _request,
                    uint8_t _length)
        : ERaModbusMessage(_length)
        , request(_request)
    {}

    bool isComplete() {
        if (this->request->isRTU()) {
            return this->isCompleteRTU();
        }
        return this->isCompleteTCP();
    }

    bool isSuccess() {
        if (this->request->isRTU()) {
            return this->isSuccessRTU();
        }
        return this->isSuccessTCP();
    }

    uint16_t getPacketId() {
        return BUILD_WORD(this->buffer[0], this->buffer[1]);
    }

    uint8_t getSlaveAddress() {
        if (this->request->isRTU()) {
            return this->buffer[0];
        }
        return this->buffer[6];
    }

    uint8_t getFunction() {
        if (this->request->isRTU()) {
            return this->buffer[1];
        }
        return this->buffer[7];
    }

    uint8_t getBytes() {
        if (this->request->isRTU()) {
            return this->buffer[2];
        }
        return this->buffer[8];
    }

    uint8_t* getData() {
        if (this->request->isRTU()) {
            return (this->buffer + 3);
        }
        return (this->buffer + 9);
    }

    bool checkCRC() {
        uint16_t crc = this->request->modbusCRC(this->buffer, this->length - 2);
        if ((this->buffer[this->length - 1] == HI_WORD(crc)) ||
            (this->buffer[this->length - 2] == LO_WORD(crc))) {
            return true;
        }
        return false;
    }

private:
    bool isCompleteRTU() {
        if ((this->buffer[1] > 0x80) &&
            (this->index == 5)) {
            return true;
        }
        if (this->index == this->request->responseLength()) {
            return true;
        }
        return false;
    }

    bool isSuccessRTU() {
        if (!this->isCompleteRTU()) {
            return false;
        }
        else if (this->buffer[1] > 0x80) {
            return false;
        }
        else if (!this->checkCRC()) {
            return false;
        }
        return ((this->getSlaveAddress() == this->request->getSlaveAddress()) &&
                (this->getFunction() == this->request->getFunction()));
    }

    bool isCompleteTCP() {
        if ((this->buffer[7] > 0x80) &&
            (this->index == 9)) {
            return true;
        }
        if (this->index == this->request->responseLength()) {
            return true;
        }
        return false;
    }

    bool isSuccessTCP() {
        if (!this->isCompleteTCP()) {
            return false;
        }
        else if (this->buffer[7] > 0x80) {
            return false;
        }
        return ((this->getPacketId() == this->request->getPacketId()) &&
                (this->getSlaveAddress() == this->request->getSlaveAddress()) &&
                (this->getFunction() == this->request->getFunction()));
    }

    ERaModbusRequest* request;
};

class ERaModbusRequest01
    : public ERaModbusRequest
{
public:
    ERaModbusRequest01(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _len)
        : ERaModbusRequest(_transp, ((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 8 : 12))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::READ_COIL_STATUS;
        this->addr = _addr;
        this->len = _len;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(0x00);
            this->add(0x06);
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 6);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return ((this->isRTU() ? 5 : 9) +
                std::ceil(static_cast<float>(this->len) / 8));
    }
};

class ERaModbusRequest02
    : public ERaModbusRequest
{
public:
    ERaModbusRequest02(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _len)
        : ERaModbusRequest(_transp, ((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 8 : 12))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::READ_INPUT_STATUS;
        this->addr = _addr;
        this->len = _len;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(0x00);
            this->add(0x06);
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 6);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return ((this->isRTU() ? 5 : 9) +
                std::ceil(static_cast<float>(this->len) / 8));
    }
};

class ERaModbusRequest03
    : public ERaModbusRequest
{
public:
    ERaModbusRequest03(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _len)
        : ERaModbusRequest(_transp, ((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 8 : 12))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::READ_HOLDING_REGISTERS;
        this->addr = _addr;
        this->len = _len;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(0x00);
            this->add(0x06);
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 6);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return ((this->isRTU() ? 5 : 9) +
                (this->len * 2));
    }
};

class ERaModbusRequest04
    : public ERaModbusRequest
{
public:
    ERaModbusRequest04(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _len)
        : ERaModbusRequest(_transp, ((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 8 : 12))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::READ_INPUT_REGISTERS;
        this->addr = _addr;
        this->len = _len;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(0x00);
            this->add(0x06);
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 6);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return ((this->isRTU() ? 5 : 9) +
                (this->len * 2));
    }
};

class ERaModbusRequest05
    : public ERaModbusRequest
{
public:
    ERaModbusRequest05(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _data)
        : ERaModbusRequest(_transp, ((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 8 : 12))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::FORCE_SINGLE_COIL;
        this->addr = _addr;
        this->len = _data;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(0x00);
            this->add(0x06);
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 6);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return (this->isRTU() ? 8 : 12);
    }
};

class ERaModbusRequest06
    : public ERaModbusRequest
{
public:
    ERaModbusRequest06(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _data)
        : ERaModbusRequest(_transp, ((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 8 : 12))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::PRESET_SINGLE_REGISTER;
        this->addr = _addr;
        this->len = _data;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(0x00);
            this->add(0x06);
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 6);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return (this->isRTU() ? 8 : 12);
    }
};

class ERaModbusRequest0F
    : public ERaModbusRequest
{
public:
    ERaModbusRequest0F(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _len,
                    const uint8_t* data)
        : ERaModbusRequest(_transp, (((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 9 : 13) +
                                    std::ceil(static_cast<float>(_len) / 8)))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::FORCE_MULTIPLE_COILS;
        this->addr = _addr;
        this->len = _len;
        _len = std::ceil(static_cast<float>(_len) / 8);
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(HI_WORD(0x07 + _len));
            this->add(LO_WORD(0x07 + _len));
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        this->add(_len);
        for (size_t i = 0; (i < _len) && (data != nullptr); ++i) {
            this->add(data[i]);
        }
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 7 + _len);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return (this->isRTU() ? 8 : 12);
    }
};

class ERaModbusRequest10
    : public ERaModbusRequest
{
public:
    ERaModbusRequest10(uint8_t _transp,
                    uint8_t _slaveAddr,
                    uint16_t _addr,
                    uint16_t _len,
                    const uint8_t* data)
        : ERaModbusRequest(_transp, (((_transp == ModbusTransportT::MODBUS_TRANSPORT_RTU) ? 9 : 13) +
                                    (_len * 2)))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::PRESET_MULTIPLE_REGISTERS;
        this->addr = _addr;
        this->len = _len;
        _len = _len * 2;
        if (!this->isRTU()) {
            this->add(HI_WORD(this->packetId));
            this->add(LO_WORD(this->packetId));
            this->add(0x00);
            this->add(0x00);
            this->add(HI_WORD(0x07 + _len));
            this->add(LO_WORD(0x07 + _len));
        }
        this->add(this->slaveAddr);
        this->add(this->function);
        this->add(HI_WORD(this->addr));
        this->add(LO_WORD(this->addr));
        this->add(HI_WORD(this->len));
        this->add(LO_WORD(this->len));
        this->add(_len);
        for (size_t i = 0; (i < _len) && (data != nullptr); ++i) {
            this->add(data[i]);
        }
        if (this->isRTU()) {
            uint16_t crc = this->modbusCRC(this->buffer, 7 + _len);
            this->add(LO_WORD(crc));
            this->add(HI_WORD(crc));
        }
    }

    uint8_t responseLength() {
        return (this->isRTU() ? 8 : 12);
    }
};

template <class Modbus>
class ERaModbusTransp
{
public:
    ERaModbusTransp()
    {}
    ~ERaModbusTransp()
    {}

    bool readCoilStatus(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest01(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->processRead(request);
    }

    bool readInputStatus(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest02(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->processRead(request);
    }

    bool readHoldingRegisters(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest03(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->processRead(request);
    }

    bool readInputRegisters(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest04(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->processRead(request);
    }

    bool forceSingleCoil(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest05(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->processWrite(request);
    }

    bool presetSingleRegister(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest06(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->processWrite(request);
    }

    bool forceMultipleCoils(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest0F(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2), param.extra);
        return this->processWrite(request);
    }

    bool presetMultipleRegisters(const uint8_t transp, const ModbusConfig_t& param) {
        ERaModbusRequest* request = new ERaModbusRequest10(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2), param.extra);
        return this->processWrite(request);
    }

    bool processRead(ERaModbusRequest* request) {
        bool status {false};
        ERaModbusResponse* response = new ERaModbusResponse(request, request->responseLength());
        this->thisModbus().sendCommand(request->getMessage(), request->getSize());
        status = this->thisModbus().waitResponse(response);
        if (status) {
            this->thisModbus().onData(request, response);
        }
        else {
            this->thisModbus().onError(request);
        }
        delete request;
        delete response;
        return status;
    }

    bool processWrite(ERaModbusRequest* request) {
        bool status {false};
        ERaModbusResponse* response = new ERaModbusResponse(request, request->responseLength());
        for (size_t i = 0; i < 2; ++i) {
            this->thisModbus().sendCommand(request->getMessage(), request->getSize());
            if (this->thisModbus().waitResponse(response)) {
                status = true;
                break;
            }
        }
        delete request;
        delete response;
        return status;
    }

private:
	inline
	const Modbus& thisModbus() const {
		return static_cast<const Modbus&>(*this);
	}

	inline
	Modbus& thisModbus() {
		return static_cast<Modbus&>(*this);
	}

};

#endif /* INC_ERA_MODBUS_TRANSPORT_HPP_ */
