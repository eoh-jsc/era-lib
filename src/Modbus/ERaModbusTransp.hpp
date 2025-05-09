#ifndef INC_ERA_MODBUS_TRANSPORT_HPP_
#define INC_ERA_MODBUS_TRANSPORT_HPP_

#include <math.h>
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
        this->packetId = ERaModbusRequest::nextPacketId();
    }

    virtual uint8_t responseLength() = 0;

    uint16_t getPacketId() const {
        return this->packetId;
    }

    uint8_t getSlaveAddress() const {
        return this->slaveAddr;
    }

    uint8_t getFunction() const {
        return this->function;
    }

    uint16_t getAddress() const {
        return this->addr;
    }

    uint16_t getLength() const {
        return this->len;
    }

    /* Start: for ModbusInternal */
    uint8_t getExtraBit(uint8_t byteIndex, uint8_t bitIndex) {
        if (this->function != ModbusFunctionT::FORCE_MULTIPLE_COILS) {
            return 0;
        }
        if (this->isRTU() &&
            ((7 + byteIndex) >= this->length)) {
            return 0;
        }
        else if (!this->isRTU() &&
            ((13 + byteIndex) >= this->length)) {
            return 0;
        }

        if (this->isRTU()) {
            return ((this->buffer[7 + byteIndex] >> bitIndex) & 0b1);
        }
        else {
            return ((this->buffer[13 + byteIndex] >> bitIndex) & 0b1);
        }
    }

    uint16_t getExtraWord(uint8_t regIndex) {
        if (this->function != ModbusFunctionT::PRESET_MULTIPLE_REGISTERS) {
            return 0;
        }
        if (this->isRTU() &&
            ((7 + regIndex + 1) >= this->length)) {
            return 0;
        }
        else if (!this->isRTU() &&
            ((13 + regIndex + 1) >= this->length)) {
            return 0;
        }

        if (this->isRTU()) {
            return BUILD_WORD(this->buffer[7 + regIndex], this->buffer[7 + regIndex + 1]);
        }
        else {
            return BUILD_WORD(this->buffer[13 + regIndex], this->buffer[13 + regIndex + 1]);
        }
    }
    /* End: for ModbusInternal */

protected:
    bool isRTU() {
        return (this->transp == ModbusTransportT::MODBUS_TRANSPORT_RTU);
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

    static uint16_t nextPacketId() {
        static uint16_t lastPacketId = 0;
        if (++lastPacketId == 0) {
            lastPacketId = 1;
        }
        return lastPacketId;
    }

    uint8_t transp;
    uint16_t packetId;
    uint8_t slaveAddr;
    uint8_t function;
    uint16_t addr;
    uint16_t len;
};

class ERaModbusResponse
    : public ERaModbusMessage
{
public:
    ERaModbusResponse(ERaModbusRequest* _request,
                    uint8_t _length)
        : ERaModbusMessage(_length)
        , request(_request)
        , status(ModbusStatusT::MODBUS_STATUS_OK)
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

    ModbusStatusT getStatusCode() {
        return (ModbusStatusT)this->status;
    }

    void setBytes(uint8_t bytes) {
        if (this->request->isRTU()) {
            this->buffer[2] = bytes;
        }
        else {
            this->buffer[8] = bytes;
        }
    }

    void appendBytes(uint8_t bytes) {
        if (this->request->isRTU()) {
            this->buffer[2] += bytes;
        }
        else {
            this->buffer[8] += bytes;
        }
    }

    uint8_t* getData() {
        if (this->request->isRTU()) {
            return (this->buffer + 3);
        }
        return (this->buffer + 9);
    }

    /* Start: for ModbusInternal */
    void setDataIndex(uint8_t shift = 0) {
        if (shift) {
            this->index += shift;
            return;
        }
        if (this->request->isRTU()) {
            this->index = 3;
            memcpy(this->buffer, this->request->getMessage(), 2);
        }
        else {
            this->index = 9;
            memcpy(this->buffer, this->request->getMessage(), 8);
        }
    }

    void setDataBit(uint8_t byteIndex, uint8_t bitIndex, uint8_t bit) {
        if (this->request->isRTU() &&
            ((3 + byteIndex) >= this->length)) {
            return;
        }
        else if (!this->request->isRTU() &&
            ((9 + byteIndex) >= this->length)) {
            return;
        }

        if (this->request->isRTU()) {
            this->buffer[3 + byteIndex] |= ((bit ? 0b1 : 0b0) << bitIndex);
        }
        else {
            this->buffer[9 + byteIndex] |= ((bit ? 0b1 : 0b0) << bitIndex);
        }
    }

    void setDataByte(uint8_t byteIndex, const uint8_t* pData, uint8_t pDataLen) {
        if (this->request->isRTU() &&
            (((3 + byteIndex) + pDataLen) >= this->length)) {
            return;
        }
        else if (!this->request->isRTU() &&
            (((9 + byteIndex) + pDataLen) >= this->length)) {
            return;
        }

        if (this->request->isRTU()) {
            memcpy(((this->buffer + 3) + byteIndex), pData, pDataLen);
        }
        else {
            memcpy(((this->buffer + 9) + byteIndex), pData, pDataLen);
        }
    }

    void setData(const uint8_t* pData, uint8_t pDataLen) {
        for (size_t i = 0; i < pDataLen; ++i) {
            this->add(pData[i]);
        }
    }

    void updateCRC() {
        if (!this->request->isRTU()) {
            return;
        }
        uint16_t crc = this->request->modbusCRC(this->buffer, this->length - 2);
        this->buffer[this->length - 1] = HI_WORD(crc);
        this->buffer[this->length - 2] = LO_WORD(crc);
    }
    /* End: for ModbusInternal */

    ModbusBuffer_t* getDataBuffer() {
        if (this->request->isRTU()) {
            return reinterpret_cast<ModbusBuffer_t*>(&this->buffer[2]);
        }
        return reinterpret_cast<ModbusBuffer_t*>(&this->buffer[8]);
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
        if (((this->buffer[1] & 0x80) == 0x80) &&
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
        else if ((this->buffer[1] & 0x80) == 0x80) {
            this->setStatusCode(this->buffer[2]);
            return false;
        }
        else if (!this->checkCRC()) {
            return false;
        }
        return ((this->getSlaveAddress() == this->request->getSlaveAddress()) &&
                (this->getFunction() == this->request->getFunction()));
    }

    bool isCompleteTCP() {
        if (((this->buffer[7] & 0x80) == 0x80) &&
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
        else if ((this->buffer[7] & 0x80) == 0x80) {
            this->setStatusCode(this->buffer[8]);
            return false;
        }
        return ((this->getPacketId() == this->request->getPacketId()) &&
                (this->getSlaveAddress() == this->request->getSlaveAddress()) &&
                (this->getFunction() == this->request->getFunction()));
    }

    void setStatusCode(uint8_t code) {
        if ((this->getFunction() & 0x1F) != this->request->getFunction()) {
            this->status = ModbusStatusT::MODBUS_STATUS_MAX;
        }
        else {
            this->status = code;
        }
    }

    ERaModbusRequest* request;
    uint8_t status;
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

    uint8_t responseLength() override {
        return ((this->isRTU() ? 5 : 9) +
                ::ceil(static_cast<float>(this->len) / 8));
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

    uint8_t responseLength() override {
        return ((this->isRTU() ? 5 : 9) +
                ::ceil(static_cast<float>(this->len) / 8));
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

    uint8_t responseLength() override {
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

    uint8_t responseLength() override {
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

    uint8_t responseLength() override {
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

    uint8_t responseLength() override {
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
                                    ::ceil(static_cast<float>(_len) / 8)))
    {
        this->slaveAddr = _slaveAddr;
        this->function = ModbusFunctionT::FORCE_MULTIPLE_COILS;
        this->addr = _addr;
        this->len = _len;
        _len = ::ceil(static_cast<float>(_len) / 8);
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

    uint8_t responseLength() override {
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

    uint8_t responseLength() override {
        return (this->isRTU() ? 8 : 12);
    }
};

#include <Modbus/ERaModbusInternal.hpp>
#include <Modbus/ERaModbusData.hpp>
#include <Modbus/ERaModbusSlave.hpp>

template <class Modbus>
class ERaModbusTransp
    : public ERaModbusInternal
    , public ERaModbusData
{
    const char* TAG = "Modbus";

    typedef bool (ERaModbusTransp::*ProcessFunc)(
        ERaModbusRequest*, ERaModbusResponse*, ModbusConfig_t&, bool, bool
    );

protected:
    ERaModbusTransp()
        : currentRequest(nullptr)
        , currentResponse(nullptr)
        , lastRequestTime(0UL)
    {}
    ~ERaModbusTransp()
    {}

    bool readCoilStatus(const uint8_t transp, ModbusConfig_t& param) {
        this->createCurrentRequest<ERaModbusRequest01>(transp, param);
        return this->beforeProcess(&ERaModbusTransp::processRead, param);
    }

    bool readInputStatus(const uint8_t transp, ModbusConfig_t& param) {
        this->createCurrentRequest<ERaModbusRequest02>(transp, param);
        return this->beforeProcess(&ERaModbusTransp::processRead, param);
    }

    bool readHoldingRegisters(const uint8_t transp, ModbusConfig_t& param) {
        this->createCurrentRequest<ERaModbusRequest03>(transp, param);
        return this->beforeProcess(&ERaModbusTransp::processRead, param);
    }

    bool readInputRegisters(const uint8_t transp, ModbusConfig_t& param) {
        this->createCurrentRequest<ERaModbusRequest04>(transp, param);
        return this->beforeProcess(&ERaModbusTransp::processRead, param);
    }

    bool beforeProcess(ProcessFunc func, ModbusConfig_t& param) {
        bool status {false};
        this->createCurrentResponse();

        status = (this->*func)(
            this->currentRequest, this->currentResponse, param, !this->lastRequestTime, false
        );

        this->deleteCurrentRequest();
        this->deleteCurrentResponse();
        return status;
    }

    bool readCoilStatusLegacy(const uint8_t transp, ModbusConfig_t& param, bool skip = false) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest01(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->beforeProcessLegacy(&ERaModbusTransp::processRead, request, param, skip);
    }

    bool readInputStatusLegacy(const uint8_t transp, ModbusConfig_t& param, bool skip = false) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest02(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->beforeProcessLegacy(&ERaModbusTransp::processRead, request, param, skip);
    }

    bool readHoldingRegistersLegacy(const uint8_t transp, ModbusConfig_t& param, bool skip = false) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest03(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->beforeProcessLegacy(&ERaModbusTransp::processRead, request, param, skip);
    }

    bool readInputRegistersLegacy(const uint8_t transp, ModbusConfig_t& param, bool skip = false) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest04(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->beforeProcessLegacy(&ERaModbusTransp::processRead, request, param, skip);
    }

    bool forceSingleCoil(const uint8_t transp, ModbusConfig_t& param) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest05(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->beforeProcessLegacy(&ERaModbusTransp::processWrite, request, param);
    }

    bool presetSingleRegister(const uint8_t transp, ModbusConfig_t& param) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest06(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2));
        return this->beforeProcessLegacy(&ERaModbusTransp::processWrite, request, param);
    }

    bool forceMultipleCoils(const uint8_t transp, ModbusConfig_t& param) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest0F(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2), param.extra);
        return this->beforeProcessLegacy(&ERaModbusTransp::processWrite, request, param);
    }

    bool presetMultipleRegisters(const uint8_t transp, ModbusConfig_t& param) {
        ERaModbusRequest* request = new_modbus ERaModbusRequest10(transp, param.addr,
                                    BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2), param.extra);
        return this->beforeProcessLegacy(&ERaModbusTransp::processWrite, request, param);
    }

    bool beforeProcessLegacy(ProcessFunc func, ERaModbusRequest* request,
                             ModbusConfig_t& param, bool skip = false) {
        bool status {false};
        ERA_ASSERT_NULL(request, false)
        ERaModbusResponse* response = new_modbus ERaModbusResponse(request, request->responseLength());
        ERA_ASSERT_NULL_DEL(response, false, request)

        status = (this->*func)(request, response, param, false, skip);

        delete request;
        delete response;
        request = nullptr;
        response = nullptr;
        return status;
    }

    bool processRead(ERaModbusRequest* request, ERaModbusResponse* response,
                     ModbusConfig_t& param, bool sendOnly = false, bool skip = false) {
        if ((request == nullptr) || (response == nullptr)) {
            return false;
        }

        bool status {false};

        if (!this->lastRequestTime) {
        }
        else if (this->thisModbus().availableBytes()) {
        }
        else if ((ERaMillis() - this->lastRequestTime) >= this->thisModbus().timeout) {
            this->lastRequestTime = 0UL;
            param.totalFail++;
            return false;
        }
        else {
            return false;
        }

        if (ERaModbusInternal::handlerRead(request, response, status)) {
            this->thisModbus().updateTotalRead();
            ERaLogHex("IB <<", response->getMessage(), response->getSize());
        }
        else if (this->thisModbus().failCounter.min && !skip &&
                (param.totalFail >= this->thisModbus().failCounter.min)) {
            /* Skip send command */
            ERA_LOG_WARNING(TAG, ERA_PSTR("Skip send command id: %d, counter: %d"),
                            request->getSlaveAddress(), param.totalFail);
        }
        else {
            status = this->sendRequestAndWaitResponse(request, response, sendOnly);
            if (sendOnly) {
                return status;
            }
        }
        if (status) {
            param.totalFail = 0;
            ERaModbusData::handler(request, response, true, skip);
            this->thisModbus().onData(request, response, skip);
        }
        else {
            param.totalFail++;
            ERaModbusData::handler(request, response, false, skip);
            this->thisModbus().onError(request, skip);
        }
        if (param.totalFail >= this->thisModbus().failCounter.max) {
            param.totalFail = this->thisModbus().failCounter.start;
        }
        this->lastRequestTime = 0UL;
        return status;
    }

    bool processWrite(ERaModbusRequest* request, ERaModbusResponse* response,
                      ModbusConfig_t& param, bool sendOnly = false, bool skip = false) {
        bool status {false};
        this->thisModbus().updateTotalWrite();
        if (ERaModbusInternal::handlerWrite(request, response, status)) {
            ERaLogHex("IB <<", request->getMessage(), request->getSize());
        }
        else {
            for (size_t i = 0; i < 2; ++i) {
                this->thisModbus().sendRequest(request->getMessage(), request->getSize());
                if (this->thisModbus().waitResponse(request, response)) {
                    status = true;
                    break;
                }
            }
        }
        ERA_FORCE_UNUSED(param);
        ERA_FORCE_UNUSED(sendOnly);
        ERA_FORCE_UNUSED(skip);
        return status;
    }

    bool canNextRequest() const {
        return (this->lastRequestTime == 0UL);
    }

    void reset() {
        this->deleteCurrentRequest();
        this->deleteCurrentResponse();
        this->lastRequestTime = 0UL;
    }

private:
    bool sendRequestAndWaitResponse(ERaModbusRequest* request, ERaModbusResponse* response, bool sendOnly) {
        bool status {false};
        if (!this->lastRequestTime) {
            this->thisModbus().updateTotalRead();
            this->thisModbus().sendRequest(request->getMessage(), request->getSize());
            this->lastRequestTime = ERaMillis();
            status = true;
        }
        if (!sendOnly) {
            status = this->thisModbus().waitResponse(request, response);
        }
        return status;
    }

    template <class T>
    void createCurrentRequest(const uint8_t transp, const ModbusConfig_t& param) {
        if (this->currentRequest != nullptr) {
            return;
        }
        this->currentRequest = new_modbus T(
            transp, param.addr, BUILD_WORD(param.sa1, param.sa2), BUILD_WORD(param.len1, param.len2)
        );
    }

    void deleteCurrentRequest() {
        if (this->lastRequestTime) {
            return;
        }
        if (this->currentRequest == nullptr) {
            return;
        }
        delete this->currentRequest;
        this->currentRequest = nullptr;
    }

    void createCurrentResponse() {
        if (this->currentResponse != nullptr) {
            return;
        }
        ERA_ASSERT_NULL(this->currentRequest, this->dummy())
        this->currentResponse = new_modbus ERaModbusResponse(
            this->currentRequest, this->currentRequest->responseLength()
        );
        ERA_ASSERT_NULL_DEL(this->currentResponse, this->dummy(), this->currentRequest)
    }

    void deleteCurrentResponse() {
        if (this->lastRequestTime) {
            return;
        }
        if (this->currentResponse == nullptr) {
            return;
        }
        delete this->currentResponse;
        this->currentResponse = nullptr;
    }

    void dummy() {
    }

    inline
    const Modbus& thisModbus() const {
        return static_cast<const Modbus&>(*this);
    }

    inline
    Modbus& thisModbus() {
        return static_cast<Modbus&>(*this);
    }

    ERaModbusRequest* currentRequest;
    ERaModbusResponse* currentResponse;
    unsigned int lastRequestTime;
};

#endif /* INC_ERA_MODBUS_TRANSPORT_HPP_ */
