#ifndef INC_ERA_MODBUS_INTERNAL_HPP_
#define INC_ERA_MODBUS_INTERNAL_HPP_

#include <stdint.h>
#include <stddef.h>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusTransp.hpp>
#include <Utility/ERaUtility.hpp>

#define addRegister(...)    addModbusRegister(__VA_ARGS__)

class ERaModbusInternal
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<bool(int32_t&)> ReadCallback_t;
    typedef std::function<bool(int32_t)> WriteCallback_t;
#else
    typedef bool (*ReadCallback_t)(int32_t&);
    typedef bool (*WriteCallback_t)(int32_t);
#endif

    const static int MAX_REGISTERS = ERA_MAX_REGISTER;
    typedef struct __Register_t {
        uint8_t addr;
        uint8_t sa1;
        uint8_t sa2;
        ERaModbusInternal::ReadCallback_t read;
        ERaModbusInternal::WriteCallback_t write;
    } Register_t;

public:
    class iterator
    {
    public:
        iterator()
            : reg(nullptr)
            , pReg(nullptr)
        {}
        iterator(ERaModbusInternal* _reg, Register_t* _pReg)
            : reg(_reg)
            , pReg(_pReg)
        {}

        bool isValid() const {
            return ((this->reg != nullptr) && (this->pReg != nullptr));
        }

        operator Register_t*() const {
            return this->pReg;
        }

        operator bool() const {
            return this->isValid();
        }

        iterator& onRead(ERaModbusInternal::ReadCallback_t cb) {
            if (this->isValid()) {
                this->reg->onRead(this->pReg, cb);
            }
            return (*this);
        }

        iterator& onWrite(ERaModbusInternal::WriteCallback_t cb) {
            if (this->isValid()) {
                this->reg->onWrite(this->pReg, cb);
            }
            return (*this);
        }
        
    protected:
    private:
        void invalidate() {
            this->reg = nullptr;
            this->pReg = nullptr;
        }

        ERaModbusInternal* reg;
        Register_t* pReg;
    };

    ERaModbusInternal()
        : numRegister(0)
    {}
    ~ERaModbusInternal()
    {}

    iterator addModbusRegister(uint8_t addr, uint16_t reg) {
        return this->addModbusRegister(addr, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusRegister(uint8_t addr, uint8_t sa1, uint8_t sa2) {
        return iterator(this, this->setupRegister(addr, sa1, sa2));
    }

protected:
    bool handlerRead(ERaModbusRequest* request, ERaModbusResponse* response, bool& status);
    bool handlerWrite(ERaModbusRequest* request, ERaModbusResponse* response, bool& status);

private:
    bool handlerReadBits(ERaModbusRequest* request, ERaModbusResponse* response, bool& status);
    bool handlerReadBytes(ERaModbusRequest* request, ERaModbusResponse* response, bool& status);
    bool handlerWriteSingle(ERaModbusRequest* request, ERaModbusResponse* response, bool& status);
    bool handlerWriteMulti(ERaModbusRequest* request, ERaModbusResponse* response, bool& status);
    Register_t* setupRegister(uint8_t addr, uint8_t sa1, uint8_t sa2);
    bool onRead(Register_t* pReg, ERaModbusInternal::ReadCallback_t cb);
    bool onWrite(Register_t* pReg, ERaModbusInternal::WriteCallback_t cb);
    bool isRegisterFree();

    bool isValidRegister(const Register_t* pReg) {
        if (pReg == nullptr) {
            return false;
        }
        return true;
    }

    ERaList<Register_t*> ERaReg;
    unsigned int numRegister;
};

inline
bool ERaModbusInternal::handlerRead(ERaModbusRequest* request, ERaModbusResponse* response, bool& status) {
    if ((request == nullptr) ||
        (response == nullptr)) {
        return false;
    }

    bool found {false};
    switch (request->getFunction()) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            found = this->handlerReadBits(request, response, status);
            break;
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
            found = this->handlerReadBytes(request, response, status);
            break;
        default:
            break;
    }

    return found;
}

inline
bool ERaModbusInternal::handlerReadBits(ERaModbusRequest* request, ERaModbusResponse* response, bool& status) {
    status = true;
    bool found {false};
    const ERaList<Register_t*>::iterator* e = this->ERaReg.end();
    for (ERaList<Register_t*>::iterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (request->getSlaveAddress() != pReg->addr) {
            continue;
        }
        if ((request->getAddress() > BUILD_WORD(pReg->sa1, pReg->sa2)) ||
            ((request->getAddress() + request->getLength()) <= BUILD_WORD(pReg->sa1, pReg->sa2))) {
            continue;
        }
        uint8_t bytes {0};
        uint8_t bitIndex = (BUILD_WORD(pReg->sa1, pReg->sa2) - request->getAddress());
        switch (request->getFunction()) {
            case ModbusFunctionT::READ_COIL_STATUS:
            case ModbusFunctionT::READ_INPUT_STATUS:
                bytes = ((request->getLength() / 8) + 1);
                break;
            default:
                continue;
        }
        if (!found) {
            response->setDataIndex();
            response->appendBytes(bytes);
        }
        found = true;
        if (pReg->read == nullptr) {
            continue;
        }
        int32_t value {0};
        status &= pReg->read(value);
        response->setDataBit((bitIndex / 8), (bitIndex % 8),
                                        (value ? 0b1 : 0b0));
    }
    status &= found;
    if (found) {
        response->updateCRC();
        ERaLogHex("IB >>", request->getMessage(), request->getSize());
    }

    return found;
}

inline
bool ERaModbusInternal::handlerReadBytes(ERaModbusRequest* request, ERaModbusResponse* response, bool& status) {
    status = true;
    bool found {false};
    const ERaList<Register_t*>::iterator* e = this->ERaReg.end();
    for (ERaList<Register_t*>::iterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (request->getSlaveAddress() != pReg->addr) {
            continue;
        }
        if ((request->getAddress() > BUILD_WORD(pReg->sa1, pReg->sa2)) ||
            ((request->getAddress() + request->getLength()) <= BUILD_WORD(pReg->sa1, pReg->sa2))) {
            continue;
        }
        uint8_t bytes {0};
        uint8_t written {0};
        uint8_t byteIndex = (BUILD_WORD(pReg->sa1, pReg->sa2) - request->getAddress());
        switch (request->getFunction()) {
            case ModbusFunctionT::READ_HOLDING_REGISTERS:
                written = 2;
                byteIndex *= 2;
                bytes = (request->getLength() * 2);
                break;
            case ModbusFunctionT::READ_INPUT_REGISTERS:
                written = 4;
                byteIndex *= 2;
                bytes = (request->getLength() * 2);
                break;
            default:
                continue;
        }
        if (!found) {
            response->setDataIndex();
            response->appendBytes(bytes);
        }
        found = true;
        if (pReg->read == nullptr) {
            continue;
        }
        union {
            uint8_t value8[4];
            int32_t value {0};
        };
        status &= pReg->read(value);
        value = SWAP_UINT32_BE(value);
        response->setDataByte(byteIndex, (value8 + (4 - written)), written);
    }
    status &= found;
    if (found) {
        response->updateCRC();
        ERaLogHex("IB >>", request->getMessage(), request->getSize());
    }

    return found;
}

inline
bool ERaModbusInternal::handlerWrite(ERaModbusRequest* request, ERaModbusResponse* response, bool& status) {
    if ((request == nullptr) ||
        (response == nullptr)) {
        return false;
    }

    bool found {false};
    switch (request->getFunction()) {
        case ModbusFunctionT::FORCE_SINGLE_COIL:
        case ModbusFunctionT::PRESET_SINGLE_REGISTER:
            found = this->handlerWriteSingle(request, response, status);
            break;
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
            found = this->handlerWriteMulti(request, response, status);
            break;
        default:
            break;
    }

    return found;
}

inline
bool ERaModbusInternal::handlerWriteSingle(ERaModbusRequest* request, ERaModbusResponse* response, bool& status) {
    status = true;
    bool found {false};
    const ERaList<Register_t*>::iterator* e = this->ERaReg.end();
    for (ERaList<Register_t*>::iterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (request->getSlaveAddress() != pReg->addr) {
            continue;
        }
        if (request->getAddress() != BUILD_WORD(pReg->sa1, pReg->sa2)) {
            continue;
        }
        int32_t value {0};
        switch (request->getFunction()) {
            case ModbusFunctionT::FORCE_SINGLE_COIL:
                if (request->getLength() == MODBUS_SINGLE_COIL_ON) {
                    value = 1;
                }
                else if (request->getLength() == MODBUS_SINGLE_COIL_OFF) {
                    value = 0;
                }
                else {
                    continue;
                }
                break;
            case ModbusFunctionT::PRESET_SINGLE_REGISTER:
                value = request->getLength();
                break;
            default:
                continue;
        }
        found = true;
        if (pReg->write != nullptr) {
            status &= pReg->write(value);
        }
    }
    status &= found;
    if (found) {
        ERaLogHex("IB >>", request->getMessage(), request->getSize());
    }

    ERA_FORCE_UNUSED(response);
    return found;
}

inline
bool ERaModbusInternal::handlerWriteMulti(ERaModbusRequest* request, ERaModbusResponse* response, bool& status) {
    status = true;
    bool found {false};
    const ERaList<Register_t*>::iterator* e = this->ERaReg.end();
    for (ERaList<Register_t*>::iterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (request->getSlaveAddress() != pReg->addr) {
            continue;
        }
        if ((request->getAddress() > BUILD_WORD(pReg->sa1, pReg->sa2)) ||
            ((request->getAddress() + request->getLength()) <= BUILD_WORD(pReg->sa1, pReg->sa2))) {
            continue;
        }
        int32_t value {0};
        uint8_t regIndex = (BUILD_WORD(pReg->sa1, pReg->sa2) - request->getAddress());
        switch (request->getFunction()) {
            case ModbusFunctionT::FORCE_MULTIPLE_COILS:
                value = request->getExtraBit((regIndex / 8), (regIndex % 8));
                break;
            case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
                value = request->getExtraWord(regIndex);
                break;
            default:
                continue;
        }
        found = true;
        if (pReg->write != nullptr) {
            status &= pReg->write(value);
        }
    }
    status &= found;
    if (found) {
        ERaLogHex("IB >>", request->getMessage(), request->getSize());
    }

    ERA_FORCE_UNUSED(response);
    return found;
}

inline
ERaModbusInternal::Register_t* ERaModbusInternal::setupRegister(uint8_t addr, uint8_t sa1, uint8_t sa2) {
    if (!this->isRegisterFree()) {
        return nullptr;
    }

    Register_t* pReg = new Register_t();
    if (pReg == nullptr) {
        return nullptr;
    }

    pReg->addr = addr;
    pReg->sa1 = sa1;
    pReg->sa2 = sa2;
    pReg->read = nullptr;
    pReg->write = nullptr;
    this->ERaReg.put(pReg);
    this->numRegister++;
    return pReg;
}

inline
bool ERaModbusInternal::onRead(Register_t* pReg, ERaModbusInternal::ReadCallback_t cb) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->read = cb;
    return true;
}

inline
bool ERaModbusInternal::onWrite(Register_t* pReg, ERaModbusInternal::WriteCallback_t cb) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->write = cb;
    return true;
}

inline
bool ERaModbusInternal::isRegisterFree() {
    if (this->numRegister >= MAX_REGISTERS) {
        return false;
    }

    return true;
}

using RegisterEntry = ERaModbusInternal::iterator;

#endif /* INC_ERA_MODBUS_INTERNAL_HPP_ */
