#ifndef INC_ERA_MODBUS_DATA_HPP_
#define INC_ERA_MODBUS_DATA_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaReport.hpp>
#include <Modbus/ERaParse.hpp>
#include <Modbus/ERaDefineModbus.hpp>
#include <Modbus/ERaModbusTransp.hpp>
#include <Utility/ERaUtility.hpp>
#include <ERa/types/WrapperTypes.hpp>

#define addDataRegister(...)    addModbusDataRegister(__VA_ARGS__)

class ERaModbusData
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> DataCallback_t;
    typedef std::function<void(void*)> ReportCallback_t;
#else
    typedef void (*DataCallback_t)(void);
    typedef void (*ReportCallback_t)(void*);
#endif

    const static int MAX_REGISTERS = ERA_MAX_REGISTERS;
    typedef struct __Register_t {
        uint8_t addr;
        uint8_t func;
        uint8_t sa1;
        uint8_t sa2;
        uint8_t len1;
        uint8_t len2;
        uint8_t type;
        uint8_t convert;
        ERaModbusData::DataCallback_t callback;
        ERaReport::iterator report;
        WrapperBase* value;
        ERaUInt_t configId;
        float f_value;
        float scale;
    } Register_t;

    const char* TAG = "DB";

public:
    class iterator
    {
    public:
        iterator()
            : reg(nullptr)
            , pReg(nullptr)
        {}
        iterator(ERaModbusData* _reg, Register_t* _pReg)
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

        iterator& publishOnChange(float minChange = 1.0f,
                                unsigned long minInterval = 1000UL,
                                unsigned long maxInterval = 60000UL) {
            if (this->isValid()) {
                this->reg->publishOnChange(this->pReg, minChange, minInterval, maxInterval);
            }
            return (*this);
        }

        iterator& setScale(float min, float max, float rawMin, float rawMax) {
            if (this->isValid()) {
                this->reg->setScale(this->pReg, min, max, rawMin, rawMax);
            }
            return (*this);
        }

        iterator& onUpdate(ERaModbusData::DataCallback_t cb) {
            if (this->isValid()) {
                this->reg->onUpdate(this->pReg, cb);
            }
            return (*this);
        }

        iterator& setLength(uint16_t length) {
            if (this->isValid()) {
                this->reg->setLength(this->pReg, length);
            }
            return (*this);
        }

        iterator& setType(uint8_t type) {
            if (this->isValid()) {
                this->reg->setType(this->pReg, type);
            }
            return (*this);
        }

        iterator& setConvert(uint8_t convert) {
            if (this->isValid()) {
                this->reg->setConvert(this->pReg, convert);
            }
            return (*this);
        }

        iterator& setScale(float scale) {
            if (this->isValid()) {
                this->reg->setScale(this->pReg, scale);
            }
            return (*this);
        }

        iterator& setBit() {
            return this->setType(ModbusDataT::MODBUS_BIT);
        }

        iterator& setInt16() {
            return this->setType(ModbusDataT::MODBUS_INT16);
        }

        iterator& setUint16() {
            return this->setType(ModbusDataT::MODBUS_UINT16);
        }

        iterator& setInt32() {
            return this->setType(ModbusDataT::MODBUS_INT32);
        }

        iterator& setUint32() {
            return this->setType(ModbusDataT::MODBUS_UINT32);
        }

        iterator& setFloat() {
            return this->setType(ModbusDataT::MODBUS_FLOAT);            
        }

        iterator& setBigEndian() {
            return this->setConvert(ModbusConvertT::MODBUS_BIG_ENDIAN);
        }

        iterator& setLittleEndian() {
            return this->setConvert(ModbusConvertT::MODBUS_LITTLE_ENDIAN);
        }

        iterator& setMidBigEndian() {
            return this->setConvert(ModbusConvertT::MODBUS_MID_BIG_ENDIAN);
        }

        iterator& setMidLittleEndian() {
            return this->setConvert(ModbusConvertT::MODBUS_MID_LITTLE_ENDIAN);
        }

    protected:
    private:
        void invalidate() {
            this->reg = nullptr;
            this->pReg = nullptr;
        }

        ERaModbusData* reg;
        Register_t* pReg;
    };

    ERaModbusData()
        : numRegister(0)
        , enableReport(false)
    {}
    ~ERaModbusData()
    {}

    iterator addModbusDataRegister(bool& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(int& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(unsigned int& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(long& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(unsigned long& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(long long& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(unsigned long long& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(float& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(double& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    template <typename T>
    iterator addModbusDataRegister(T& value, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addModbusDataRegister(value, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addModbusDataRegister(bool& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperBool(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(int& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperInt(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(unsigned int& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperUnsignedInt(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(long& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperLong(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(unsigned long& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperUnsignedLong(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(long long& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperLongLong(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(unsigned long long& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperUnsignedLongLong(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(float& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperFloat(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    iterator addModbusDataRegister(double& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperDouble(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

    template <typename T>
    iterator addModbusDataRegister(T& value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        WrapperBase* wrapper = new WrapperNumber<T>(value);
        return iterator(this, this->setupRegister(wrapper, addr, func, sa1, sa2));
    }

protected:
    bool isNewReport() const {
        return this->enableReport;
    }

    void removeConfigId();
    void updateRegister();
    void parseConfig(const void* ptr, bool json);
    void processParseParamConvertAi(const cJSON* const root, iterator& it);
    void processParseConfigSensorParam(const cJSON* const root, uint8_t addr);
    void processParseConfigSensorParams(const cJSON* const root, uint8_t addr);
    void processParseConfigSensor(const cJSON* const root);
    void processParseConfigSensors(const cJSON* const root);
    bool handler(ERaModbusRequest* request, ERaModbusResponse* response, bool success, bool skip = false);

    virtual void configIdModbusWrite(ERaInt_t configId, float value) {
        ERA_FORCE_UNUSED(configId);
        ERA_FORCE_UNUSED(value);
        ERA_LOG_WARNING(TAG, ERA_PSTR("configIdModbusWrite default."));
    }

    virtual void configIdModbusRemove(ERaInt_t configId) {
        ERA_FORCE_UNUSED(configId);
        ERA_LOG_WARNING(TAG, ERA_PSTR("configIdModbusRemove default."));
    }

private:
    iterator addInternalRegister(ERaInt_t configId, uint8_t addr, uint8_t func, uint16_t reg) {
        return this->addInternalRegister(configId, addr, func, HI_WORD(reg), LO_WORD(reg));
    }

    iterator addInternalRegister(ERaInt_t configId, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2) {
        return iterator(this, this->setupRegister(nullptr, addr, func, sa1, sa2, configId));
    }

    bool handlerBits(ERaModbusRequest* request, ERaModbusResponse* response, bool success);
    bool handlerBytes(ERaModbusRequest* request, ERaModbusResponse* response, bool success);
    Register_t* setupRegister(WrapperBase* value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2, ERaUInt_t configId = 0UL);
    bool publishOnChange(Register_t* pReg, float minChange = 1.0f,
                        unsigned long minInterval = 1000UL,
                        unsigned long maxInterval = 60000UL);
    bool setScale(Register_t* pReg, float min, float max, float rawMin, float rawMax);
    bool onUpdate(Register_t* pReg, ERaModbusData::DataCallback_t cb);
    bool setLength(Register_t* pReg, uint16_t length);
    bool setType(Register_t* pReg, uint8_t type);
    bool setConvert(Register_t* pReg, uint8_t convert);
    bool setScale(Register_t* pReg, float scale);
    bool isRegisterFree() const;

    void onCallback(void* args);
#if !defined(ERA_HAS_FUNCTIONAL_H)
    static void _onCallback(void* args);
#endif

    bool isValidRegister(const Register_t* pReg) const {
        if (pReg == nullptr) {
            return false;
        }
        return true;
    }

    bool getBit(uint8_t byte, uint8_t pos) {
        return ((byte >> pos) & 0x01);
    }

    ERaList<Register_t*> ERaReg;
    ERaList<ERaInt_t> ERaRegId;
    ERaReport ERaRegRp;
    unsigned int numRegister;

    bool enableReport;

#if defined(ERA_HAS_FUNCTIONAL_H)
    ReportCallback_t reportCallback = [&, this](void* args) {
        this->onCallback(args);
    };
#else
    ReportCallback_t reportCallback = [&, this](void* args) {
        ERaModbusData::_onCallback(args);
    };
#endif

    using ModbusDataIterator = typename ERaList<Register_t*>::iterator;
};

inline
ERaModbusData::Register_t* ERaModbusData::setupRegister(WrapperBase* value, uint8_t addr, uint8_t func, uint8_t sa1, uint8_t sa2, ERaUInt_t configId) {
    if (!this->isRegisterFree()) {
        return nullptr;
    }

    Register_t* pReg = new Register_t();
    if (pReg == nullptr) {
        return nullptr;
    }

    pReg->addr = addr;
    pReg->func = func;
    pReg->sa1 = sa1;
    pReg->sa2 = sa2;
    pReg->len1 = 0;
    pReg->len2 = 1;
    pReg->type = ModbusDataT::MODBUS_INT16;
    pReg->convert = ModbusConvertT::MODBUS_BIG_ENDIAN;
    pReg->callback = nullptr;
    pReg->report = ERaReport::iterator();

    if (value == nullptr) {
        pReg->f_value = 0.0f;
        value = new WrapperFloat(pReg->f_value);
    }
    if (value == nullptr) {
        delete pReg;
        return nullptr;
    }

    pReg->configId = configId;
    pReg->value = value;
    pReg->scale = 1.0f;
    this->ERaReg.put(pReg);
    this->numRegister++;
    return pReg;
}

inline
void ERaModbusData::updateRegister() {
    this->ERaRegId.clear();
    Register_t* pReg = nullptr;
    ModbusDataIterator* next = nullptr;
    const ModbusDataIterator* e = this->ERaReg.end();
    for (ModbusDataIterator* it = this->ERaReg.begin(); it != e; it = next) {
        next = it->getNext();
        pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (!pReg->configId) {
            continue;
        }
        this->ERaRegId.put(pReg->configId);
        if (pReg->value != nullptr) {
            delete pReg->value;
            pReg->value = nullptr;
        }
        pReg->report.deleteReport();
        delete pReg;
        pReg = nullptr;
        it->get() = nullptr;
        this->ERaReg.remove(it);
    }

    this->ERaRegRp.run();
}

inline
void ERaModbusData::processParseParamConvertAi(const cJSON* const root, ERaModbusData::iterator& it) {
    cJSON* min = cJSON_GetObjectItem(root, MODBUS_PARAM_AI_MIN_VALUE);
    if (!cJSON_IsNumber(min)) {
        return;
    }

    cJSON* max = cJSON_GetObjectItem(root, MODBUS_PARAM_AI_MAX_VALUE);
    if (!cJSON_IsNumber(max)) {
        return;
    }

    cJSON* rawMin = cJSON_GetObjectItem(root, MODBUS_PARAM_AI_RAW_MIN_VALUE);
    if (!cJSON_IsNumber(rawMin)) {
        return;
    }

    cJSON* rawMax = cJSON_GetObjectItem(root, MODBUS_PARAM_AI_RAW_MAX_VALUE);
    if (!cJSON_IsNumber(rawMax)) {
        return;
    }

    it.setScale(min->valuedouble, max->valuedouble, rawMin->valuedouble, rawMax->valuedouble);
}

inline
void ERaModbusData::processParseConfigSensorParam(const cJSON* const root, uint8_t addr) {
    cJSON* func = cJSON_GetObjectItem(root, MODBUS_PARAM_FUNCTION_KEY);
    if (!cJSON_IsNumber(func)) {
        return;
    }

    if (func->valueint > ModbusFunctionT::READ_INPUT_REGISTERS) {
        return;
    }

    cJSON* reg = cJSON_GetObjectItem(root, MODBUS_PARAM_REGISTER_KEY);
    if (!cJSON_IsNumber(reg)) {
        return;
    }

    cJSON* configId = cJSON_GetObjectItem(root, MODBUS_PARAM_CONFIG_ID_KEY);
    if (!cJSON_IsNumber(configId)) {
        return;
    }

    cJSON* scale = cJSON_GetObjectItem(root, MODBUS_PARAM_SCALE_KEY);
    if (!cJSON_IsNumber(scale)) {
        return;
    }

    cJSON* transformer = cJSON_GetObjectItem(root, MODBUS_PARAM_TRANSFORMER_KEY);
    if (!cJSON_IsString(transformer)) {
        return;
    }

    iterator it = this->addInternalRegister(
        configId->valueint, addr, func->valueint, reg->valueint
    ).setScale(scale->valuedouble).publishOnChange(1.0f, 1000UL, 60000UL);

    switch (func->valueint) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            if (ERaStrCmp(transformer->valuestring, "int_all")) {
                it.setBit();
            }
            break;
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
            if (ERaStrCmp(transformer->valuestring, "int_all")) {
                it.setInt16();
            }
            else if (ERaStrCmp(transformer->valuestring, "uint_32")) {
                it.setUint32().setMidLittleEndian().setLength(2);
            }
            else if (ERaStrCmp(transformer->valuestring, "int_first4") ||
                     ERaStrCmp(transformer->valuestring, "int_last4")) {
                it.setInt32().setLength(2);
            }
            else if (ERaStrCmp(transformer->valuestring, "float_abcd")) {
                it.setFloat().setBigEndian().setLength(2);
            }
            else if (ERaStrCmp(transformer->valuestring, "float_dcba")) {
                it.setFloat().setLittleEndian().setLength(2);
            }
            else if (ERaStrCmp(transformer->valuestring, "float_badc")) {
                it.setFloat().setMidBigEndian().setLength(2);
            }
            else if (ERaStrCmp(transformer->valuestring, "float_cdab") ||
                     ERaStrCmp(transformer->valuestring, "float_cdba")) {
                it.setFloat().setMidLittleEndian().setLength(2);
            }
            else if (ERaStrCmp(transformer->valuestring, "convert_ai")) {
                it.setUint16();
                this->processParseParamConvertAi(root, it);
            }
            break;
        default:
            break;
    }
}

inline
void ERaModbusData::processParseConfigSensorParams(const cJSON* const root, uint8_t addr) {
    size_t size = cJSON_GetArraySize(root);

    for (size_t i = 0; i < size; ++i) {
        cJSON* param = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(param)) {
            continue;
        }
        this->processParseConfigSensorParam(param, addr);
    }
}

inline
void ERaModbusData::processParseConfigSensor(const cJSON* const root) {
    cJSON* address = cJSON_GetObjectItem(root, MODBUS_SENSOR_ADDRESS_KEY);
    if (!cJSON_IsNumber(address)) {
        return;
    }

    cJSON* configs = cJSON_GetObjectItem(root, MODBUS_SENSOR_CONFIGS_KEY);
    if (!cJSON_IsArray(configs)) {
        return;
    }
    this->processParseConfigSensorParams(configs, address->valueint);
}

inline
void ERaModbusData::processParseConfigSensors(const cJSON* const root) {
    size_t size = cJSON_GetArraySize(root);

    for (size_t i = 0; i < size; ++i) {
        cJSON* sensor = cJSON_GetArrayItem(root, i);
        if (!cJSON_IsObject(sensor)) {
            continue;
        }
        this->processParseConfigSensor(sensor);
    }
}

inline
void ERaModbusData::parseConfig(const void* ptr, bool json) {
    this->enableReport = false;
    this->updateRegister();

    if (!json) {
        return;
    }
    if (ptr == nullptr) {
        return;
    }
    const cJSON* root = (const cJSON*)ptr;
    if (!cJSON_IsObject(root)) {
        return;
    }

    cJSON* version = cJSON_GetObjectItem(root, MODBUS_VERSION_KEY);
    if (!cJSON_IsNumber(version)) {
        return;
    }
    if (version->valueint < MODBUS_VERSION_SUPPORT_JSON) {
        return;
    }

    cJSON* sensors = cJSON_GetObjectItem(root, MODBUS_SENSORS_KEY);
    if (cJSON_IsArray(sensors)) {
        this->processParseConfigSensors(sensors);
    }

    this->enableReport = true;
}

inline
bool ERaModbusData::handler(ERaModbusRequest* request, ERaModbusResponse* response, bool success, bool skip) {
    if (skip) {
        return false;
    }
    if ((request == nullptr) ||
        (response == nullptr)) {
        return false;
    }

    bool found {false};
    switch (request->getFunction()) {
        case ModbusFunctionT::READ_COIL_STATUS:
        case ModbusFunctionT::READ_INPUT_STATUS:
            found = this->handlerBits(request, response, success);
            break;
        case ModbusFunctionT::READ_HOLDING_REGISTERS:
        case ModbusFunctionT::READ_INPUT_REGISTERS:
            found = this->handlerBytes(request, response, success);
            break;
        default:
            break;
    }

    this->ERaRegRp.run();

    return found;
}

inline
void ERaModbusData::removeConfigId() {
    const ERaList<ERaInt_t>::iterator* e = this->ERaRegId.end();
    for (ERaList<ERaInt_t>::iterator* it = this->ERaRegId.begin(); it != e; it = it->getNext()) {
        ERaInt_t configId = it->get();
        if (!configId) {
            continue;
        }
        this->configIdModbusRemove(configId);
    }
    this->ERaRegId.clear();
}

inline
bool ERaModbusData::handlerBits(ERaModbusRequest* request, ERaModbusResponse* response, bool success) {
    bool found {false};
    const ModbusDataIterator* e = this->ERaReg.end();
    for (ModbusDataIterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (request->getSlaveAddress() != pReg->addr) {
            continue;
        }
        if (request->getFunction() != pReg->func) {
            continue;
        }
        if ((request->getAddress() > BUILD_WORD(pReg->sa1, pReg->sa2)) ||
            ((request->getAddress() + request->getLength()) <= BUILD_WORD(pReg->sa1, pReg->sa2))) {
            continue;
        }
        if (!success) {
            pReg->report.disable();
            continue;
        }
        pReg->report.enable();

        uint8_t bitIndex = (BUILD_WORD(pReg->sa1, pReg->sa2) - request->getAddress());
        uint8_t byteIndex = (bitIndex / 8);
        switch (request->getFunction()) {
            case ModbusFunctionT::READ_COIL_STATUS:
            case ModbusFunctionT::READ_INPUT_STATUS:
                if (response->getBytes() > byteIndex) {
                    (*pReg->value) = this->getBit(response->getData()[byteIndex], bitIndex);
                    break;
                }
            default:
                continue;
        }
        found = true;
        if (pReg->callback != nullptr) {
            pReg->callback();
        }
        pReg->report.updateReport(pReg->value->getDouble());
#if defined(MODBUS_DATA_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Address: %d, Function: %d, Register: %d, Value: %s"),
        pReg->addr, pReg->func, BUILD_WORD(pReg->sa1, pReg->sa2), pReg->value->getBool() ? "true" : "false");
#endif
    }

    return found;
}

inline
bool ERaModbusData::handlerBytes(ERaModbusRequest* request, ERaModbusResponse* response, bool success) {
    bool found {false};
    const ModbusDataIterator* e = this->ERaReg.end();
    for (ModbusDataIterator* it = this->ERaReg.begin(); it != e; it = it->getNext()) {
        Register_t* pReg = it->get();
        if (!this->isValidRegister(pReg)) {
            continue;
        }
        if (request->getSlaveAddress() != pReg->addr) {
            continue;
        }
        if (request->getFunction() != pReg->func) {
            continue;
        }
        if ((request->getAddress() > BUILD_WORD(pReg->sa1, pReg->sa2)) ||
            ((request->getAddress() + request->getLength()) <= BUILD_WORD(pReg->sa1, pReg->sa2))) {
            continue;
        }
        if (!success) {
            pReg->report.disable();
            continue;
        }
        pReg->report.enable();

        uint8_t expectedLen {0};
        uint8_t byteIndex = (BUILD_WORD(pReg->sa1, pReg->sa2) - request->getAddress());
                byteIndex *= 2;
        uint8_t* pData = response->getData();
        switch (pReg->type) {
            case ModbusDataT::MODBUS_INT16:
            case ModbusDataT::MODBUS_UINT16:
                expectedLen = 2;
                break;
            case ModbusDataT::MODBUS_INT32:
            case ModbusDataT::MODBUS_UINT32:
            case ModbusDataT::MODBUS_FLOAT:
                expectedLen = 4;
                break;
            default:
                continue;
        }
        switch (request->getFunction()) {
            case ModbusFunctionT::READ_HOLDING_REGISTERS:
            case ModbusFunctionT::READ_INPUT_REGISTERS:
                if (response->getBytes() >= (byteIndex + expectedLen)) {
                    break;
                }
            default:
                continue;
        }
        switch (pReg->type) {
            case ModbusDataT::MODBUS_INT16:
                if (pReg->convert == ModbusConvertT::MODBUS_BIG_ENDIAN) {
                    (*pReg->value) = (int16_t)BUILD_UINT16_BE(pData[byteIndex]);
                }
                else {
                    (*pReg->value) = (int16_t)BUILD_UINT16_LE(pData[byteIndex]);
                }
                break;
            case ModbusDataT::MODBUS_UINT16:
                if (pReg->convert == ModbusConvertT::MODBUS_BIG_ENDIAN) {
                    (*pReg->value) = BUILD_UINT16_BE(pData[byteIndex]);
                }
                else {
                    (*pReg->value) = BUILD_UINT16_LE(pData[byteIndex]);
                }
                break;
            case ModbusDataT::MODBUS_INT32:
            case ModbusDataT::MODBUS_UINT32:
            case ModbusDataT::MODBUS_FLOAT: {
                union {
                    uint32_t value {0};
                    float f_value;
                };
                switch (pReg->convert) {
                    case ModbusConvertT::MODBUS_BIG_ENDIAN:
                        value = BUILD_BIG_ENDIAN(pData[byteIndex]);
                        break;
                    case ModbusConvertT::MODBUS_LITTLE_ENDIAN:
                        value = BUILD_LITTLE_ENDIAN(pData[byteIndex]);
                        break;
                    case ModbusConvertT::MODBUS_MID_BIG_ENDIAN:
                        value = BUILD_MID_BIG_ENDIAN(pData[byteIndex]);
                        break;
                    case ModbusConvertT::MODBUS_MID_LITTLE_ENDIAN:
                        value = BUILD_MID_LITTLE_ENDIAN(pData[byteIndex]);
                        break;
                    default:
                        continue;
                }
                if (pReg->type == ModbusDataT::MODBUS_FLOAT) {
                    (*pReg->value) = f_value;
                }
                else if (pReg->type == ModbusDataT::MODBUS_INT32) {
                    (*pReg->value) = (int32_t)value;
                }
                else {
                    (*pReg->value) = value;
                }
            }
                break;
            default:
                continue;
        }
        found = true;
        (*pReg->value) = (pReg->value->getFloat() * pReg->scale);
        if (pReg->callback != nullptr) {
            pReg->callback();
        }
        pReg->report.updateReport(pReg->value->getDouble());
#if defined(MODBUS_DATA_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Address: %d, Function: %d, Register: %d, Value: %.2f"),
        pReg->addr, pReg->func, BUILD_WORD(pReg->sa1, pReg->sa2), pReg->value->getDouble());
#endif
    }

    return found;
}

inline
void ERaModbusData::onCallback(void* args) {
    ERaModbusData::Register_t* pReg = (ERaModbusData::Register_t*)args;
    if (pReg == nullptr) {
        return;
    }
    if (!pReg->configId) {
        return;
    }
    this->configIdModbusWrite(pReg->configId, pReg->report.getValue());
}

inline
bool ERaModbusData::publishOnChange(Register_t* pReg, float minChange,
                                        unsigned long minInterval,
                                        unsigned long maxInterval) {
    if (pReg == nullptr) {
        return false;
    }

    minChange *= pReg->scale;
    if (pReg->report) {
        pReg->report.changeReportableChange(minInterval, maxInterval, minChange);
    }
    else {
        pReg->report = this->ERaRegRp.setReporting(minInterval, maxInterval, minChange, this->reportCallback, pReg);
    }
    return true;
}

inline
bool ERaModbusData::setScale(Register_t* pReg, float min, float max, float rawMin, float rawMax) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->report.setScale(min, max, rawMin, rawMax);
    return true;
}

inline
bool ERaModbusData::onUpdate(Register_t* pReg, ERaModbusData::DataCallback_t cb) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->callback = cb;
    return true;
}

inline
bool ERaModbusData::setLength(Register_t* pReg, uint16_t length) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->len1 = HI_WORD(length);
    pReg->len2 = LO_WORD(length);
    return true;
}

inline
bool ERaModbusData::setType(Register_t* pReg, uint8_t type) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->type = type;
    return true;
}

inline
bool ERaModbusData::setConvert(Register_t* pReg, uint8_t convert) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->convert = convert;
    return true;
}

inline
bool ERaModbusData::setScale(Register_t* pReg, float scale) {
    if (pReg == nullptr) {
        return false;
    }

    pReg->scale = scale;
    return true;
}

inline
bool ERaModbusData::isRegisterFree() const {
    if (this->numRegister >= MAX_REGISTERS) {
        return false;
    }

    return true;
}

#endif /* INC_ERA_MODBUS_DATA_HPP_ */
