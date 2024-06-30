#ifndef INC_ERA_MODBUS_SLAVE_HPP_
#define INC_ERA_MODBUS_SLAVE_HPP_

#include <stdint.h>
#include <stddef.h>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusTransp.hpp>
#include <Utility/ERaUtility.hpp>

#define addSlaveRegister(...)    addModbusSlaveRegister(__VA_ARGS__)

template <class Modbus>
class ERaModbusSlave
    : protected ERaModbusRequest
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(const ERaModbusRequest*)> HandlerCallback_t;
#else
    typedef void (*HandlerCallback_t)(const ERaModbusRequest*);
#endif

    const static int MAX_REGISTERS = ERA_MAX_REGISTERS;
    typedef struct __Register_t {
        uint8_t addr;
        uint8_t func;
        uint8_t sa1;
        uint8_t sa2;
        bool response;
        ERaModbusSlave::HandlerCallback_t callback;
    } Register_t;

public:
    class iterator
    {
    public:
        iterator()
            : reg(nullptr)
            , pReg(nullptr)
        {}
        iterator(ERaModbusSlave* _reg, Register_t* _pReg)
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

        iterator& onResponse() {
            if (this->isValid()) {
                this->reg->onResponse(this->pReg);
            }
            return (*this);
        }

        iterator& onHandler(ERaModbusSlave::HandlerCallback_t cb) {
            if (this->isValid()) {
                this->reg->onHandler(this->pReg, cb);
            }
            return (*this);
        }

    protected:
    private:
        void invalidate() {
            this->reg = nullptr;
            this->pReg = nullptr;
        }

        ERaModbusSlave* reg;
        Register_t* pReg;
    };

    ERaModbusSlave()
        : ERaModbusRequest(0, 12)
        , numRegister(0)
        , enabled(false)
        , callbacks(nullptr)
    {}
    ~ERaModbusSlave()
    {}

    iterator addModbusSlaveRegister(uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusSlaveRegister(addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusSlaveRegister(uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        return iterator(this, this->setupRegister(addr, func, sa1, sa2));
    }

    void onModbusSlaveHandlers(ERaModbusSlave::HandlerCallback_t cb) {
        this->enabled = true;
        this->callbacks = cb;
    }

protected:
    void setSlaveTransport(uint8_t transp);
    void setSlaveAddress(uint8_t addr);
    void setSlaveFunction(uint8_t func);
    bool handlerSlave();
    bool isComplete();
    bool isSuccess();
    bool isEnabled() const;

    uint8_t responseLength() override {
        return 0;
    }

private:
    bool handlerCallbacks();
    void sendResponse(Register_t* pReg);
    Register_t* setupRegister(uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2);
    bool onResponse(Register_t* pReg);
    bool onHandler(Register_t* pReg, ERaModbusSlave::HandlerCallback_t cb);
    bool isCompleteRTU();
    bool isSuccessRTU();
    bool isCompleteTCP();
    bool isSuccessTCP();
    bool isRegisterFree() const;

    bool isValidRegister(const Register_t* pReg) const {
        if (pReg == nullptr) {
            return false;
        }
        return true;
    }

    inline
    const Modbus& thisModbus() const {
        return static_cast<const Modbus&>(*this);
    }

    inline
    Modbus& thisModbus() {
        return static_cast<Modbus&>(*this);
    }

    ERaList<Register_t*> ERaReg;
    unsigned int numRegister;

    bool enabled;

    ERaModbusSlave::HandlerCallback_t callbacks;

    using ModbusSlaveIterator = typename ERaList<Register_t*>::iterator;
};

template <class Modbus>
void ERaModbusSlave<Modbus>::setSlaveTransport(uint8_t transp) {
    this->transp = transp;
}

template <class Modbus>
void ERaModbusSlave<Modbus>::setSlaveAddress(uint8_t addr) {
    this->slaveAddr = addr;
}

template <class Modbus>
void ERaModbusSlave<Modbus>::setSlaveFunction(uint8_t func) {
    this->function = func;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::handlerSlave() {
    if (!this->enabled) {
        return false;
    }

    bool status {false};

    switch (this->function) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
        case ModbusFunctionT::FORCE_SINGLE_COIL:
        case ModbusFunctionT::PRESET_SINGLE_REGISTER:
            status = this->handlerCallbacks();
            break;
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
        default:
            break;
    }

    return status;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::handlerCallbacks() {
    if (!this->isSuccess()) {
        return false;
    }

    ERaLogHex("SB <<", this->getMessage(), this->getPosition());

    const ModbusSlaveIterator* e = this->ERaReg.end();
    for (ModbusSlaveIterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (this->callbacks != nullptr) {
            this->callbacks(this);
        }
        if (pReg->addr && (this->getSlaveAddress() != pReg->addr)) {
            continue;
        }
        if (this->getFunction() != pReg->func) {
            continue;
        }
        if (this->getAddress() != BUILD_WORD(pReg->sa1, pReg->sa2)) {
            continue;
        }
        if (pReg->callback == nullptr) {
            continue;
        }
        pReg->callback(this);
        this->sendResponse(pReg);
        break;
    }

    return true;
}

template <class Modbus>
void ERaModbusSlave<Modbus>::sendResponse(Register_t* pReg) {
    if (pReg == nullptr) {
        return;
    }
    if (!pReg->response) {
        return;
    }

    switch (pReg->func) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS: {
            uint8_t data[5] {0};
            data[0] = this->getSlaveAddress();
            data[1] = (pReg->func | 0x80);
            data[2] = ModbusStatusT::MODBUS_STATUS_ILLEGAL_DATA_VALUE;
            uint16_t crc = this->modbusCRC(data, sizeof(data) - 2);
            data[3] = LO_WORD(crc);
            data[4] = HI_WORD(crc);
            ERaLogHex("SB >>", data, sizeof(data));
            this->thisModbus().sendCommand(data, sizeof(data));
        }
            break;
        case ModbusFunctionT::FORCE_SINGLE_COIL:
        case ModbusFunctionT::PRESET_SINGLE_REGISTER: {
            ERaLogHex("SB >>", this->getMessage(), this->getPosition());
            this->thisModbus().sendCommand(this->getMessage(), this->getPosition());
        }
            break;
        case ModbusFunctionT::FORCE_MULTIPLE_COILS:
        case ModbusFunctionT::PRESET_MULTIPLE_REGISTERS:
        default:
            break;
    }
}

template <class Modbus>
typename ERaModbusSlave<Modbus>::Register_t* ERaModbusSlave<Modbus>::setupRegister(uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
    if (!this->isRegisterFree()) {
        return nullptr;
    }

    Register_t* pReg = new Register_t();
    if (pReg == nullptr) {
        return nullptr;
    }

    this->enabled = true;

    pReg->addr = addr;
    pReg->func = func;
    pReg->sa1 = sa1;
    pReg->sa2 = sa2;
    pReg->response = false;
    pReg->callback = nullptr;
    this->ERaReg.put(pReg);
    this->numRegister++;
    return pReg;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::onResponse(Register_t* pReg) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->response = true;
    return true;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::onHandler(Register_t* pReg, ERaModbusSlave::HandlerCallback_t cb) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->callback = cb;
    return true;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isComplete() {
    if (this->isRTU()) {
        return this->isCompleteRTU();
    }
    return this->isCompleteTCP();
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isSuccess() {
    if (this->isRTU()) {
        return this->isSuccessRTU();
    }
    return this->isSuccessTCP();
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isCompleteRTU() {
    if (this->index < 8) {
        return false;
    }
    return true;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isSuccessRTU() {
    if (!this->isCompleteRTU()) {
        return false;
    }
    this->addr = BUILD_WORD(this->buffer[2], this->buffer[3]);
    this->len = BUILD_WORD(this->buffer[4], this->buffer[5]);
    uint16_t crc = this->modbusCRC(this->buffer, 6);
    return (crc == BUILD_WORD(this->buffer[7], this->buffer[6]));
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isCompleteTCP() {
    if (this->index < 12) {
        return false;
    }
    return true;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isSuccessTCP() {
    if (!this->isCompleteTCP()) {
        return false;
    }
    this->addr = BUILD_WORD(this->buffer[8], this->buffer[9]);
    this->len = BUILD_WORD(this->buffer[10], this->buffer[11]);
    return true;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isEnabled() const {
    return this->enabled;
}

template <class Modbus>
bool ERaModbusSlave<Modbus>::isRegisterFree() const {
    if (this->numRegister >= MAX_REGISTERS) {
        return false;
    }

    return true;
}

#endif /* INC_ERA_MODBUS_SLAVE_HPP_ */
