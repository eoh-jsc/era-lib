#ifndef INC_ERA_MODBUS_ACTION_HPP_
#define INC_ERA_MODBUS_ACTION_HPP_

#include <math.h>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaModbusConfig.hpp>
#include <Modbus/ERaDefineModbus.hpp>

class ERaModbusAction
{
public:
    ERaModbusAction()
    {}
    ~ERaModbusAction()
    {}

    bool addModbusActionRaw(const uint8_t* pData, size_t pDataLen,
                        uint16_t prevMs = 0U, uint16_t postMs = 0U, IPSlave_t* ip = nullptr) {
        if (pData == nullptr) {
            return false;
        }
        if (pDataLen < 6UL) {
            return false;
        }
        return this->addModbusActionRaw(pData[0], pData[1], pData[2], pData[3], pData[4], pData[5],
                                        (pDataLen > 6UL) ? (pData + 6) : nullptr,
                                        (pDataLen > 6UL) ? (pDataLen - 6) : 0UL, prevMs, postMs, ip);
    }

    bool addModbusActionRaw(uint8_t addr, uint8_t func, uint16_t reg, uint16_t len,
                            const uint8_t* pExtra, size_t pExtraLen,
                            uint16_t prevMs = 0U, uint16_t postMs = 0U, IPSlave_t* ip = nullptr) {
        return this->addModbusActionRaw(addr, func, HI_WORD(reg), LO_WORD(reg), HI_WORD(len),
                                        LO_WORD(len), pExtra, pExtraLen, prevMs, postMs, ip);
    }

    bool addModbusActionRaw(uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2, uint8_t len1,
                            uint8_t len2, const uint8_t* pExtra, size_t pExtraLen,
                            uint16_t prevMs = 0U, uint16_t postMs = 0U, IPSlave_t* ip = nullptr);

protected:
    bool addModbusAction(const char* ptr) {
        return this->addModbusAction(ptr, ModbusActionTypeT::MODBUS_ACTION_DEFAULT, 0);
    }

    bool addModbusAction(const char* ptr, uint16_t param) {
        return this->addModbusAction(ptr, ModbusActionTypeT::MODBUS_ACTION_PARAMS, param);
    }

    bool addModbusAction(const char* ptr, uint8_t type, uint16_t param);

    bool isRequest() {
        return this->queue.readable();
    }

    ModbusAction_t& getRequest() {
        return this->queue;
    }

    bool isEmptyRequest() {
        return this->queue.isEmpty();
    }

    bool isRawRequest() {
        return this->queueRaw.readable();
    }

    ModbusActionRaw_t& getRawRequest() {
        return this->queueRaw;
    }

    bool isEmptyRawRequest() {
        return this->queueRaw.isEmpty();
    }

private:
    ERaQueue<ModbusAction_t, MODBUS_MAX_ACTION> queue;
    ERaQueue<ModbusActionRaw_t, MODBUS_MAX_ACTION> queueRaw;
};

inline
bool ERaModbusAction::addModbusActionRaw(uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2, uint8_t len1,
                                         uint8_t len2, const uint8_t* pExtra, size_t pExtraLen,
                                         uint16_t prevMs, uint16_t postMs, IPSlave_t* ip) {
    if (!this->queueRaw.writeable()) {
        return false;
    }
    ModbusActionRaw_t req;
    req.addr = addr;
    req.func = func;
    req.sa1 = sa1;
    req.sa2 = sa2;
    req.len1 = len1;
    req.len2 = len2;
    if (pExtraLen && (pExtra != nullptr)) {
        req.pExtra = (uint8_t*)malloc(pExtraLen);
    }
    else {
        req.pExtra = nullptr;
    }
    if (req.pExtra != nullptr) {
        req.pExtraLen = pExtraLen;
        memcpy(req.pExtra, pExtra, pExtraLen);
    }
    else {
        req.pExtraLen = 0UL;
    }
    if (ip != nullptr) {
        req.ip.port = ip->port;
        req.ip.ip.dword = ip->ip.dword;
    }
    else {
        req.ip.port = 0;
        req.ip.ip.dword = 0UL;
    }
    req.prevdelay = prevMs;
    req.postdelay = postMs;
    this->queueRaw += req;
    return true;
}

inline
bool ERaModbusAction::addModbusAction(const char* ptr, uint8_t type, uint16_t param) {
    if (ptr == nullptr) {
        return false;
    }
    if (strlen(ptr) > 36) {
        return false;
    }
    if (!this->queue.writeable()) {
        return false;
    }
    size_t size = strlen(ptr) + 1;
    char* buf = (char*)ERA_MALLOC(size);
    if (buf == nullptr) {
        return false;
    }
    ModbusAction_t req;
    req.key = buf;
    req.type = type;
    req.param = param;
    memset(req.key, 0, size);
    strcpy(req.key, ptr);
    this->queue += req;
    return true;
}

#endif /* INC_ERA_MODBUS_ACTION_HPP_ */
