#ifndef INC_ERA_PIN_HPP_
#define INC_ERA_PIN_HPP_

#include <ERa/ERaDefine.hpp>
#include <Utility/ERaUtility.hpp>
#include <ERa/ERaReport.hpp>
#include <ERa/ERaData.hpp>
#include <ERa/ERaParam.hpp>

#if !defined(TOGGLE)
    #define TOGGLE              0x2
#endif
#if !defined(PWM)
    #define PWM                 0x27
#endif
#if !defined(RAW_PIN)
    #define RAW_PIN             0xFE
#endif
#if !defined(VIRTUAL)
    #define VIRTUAL             0xFF
#endif
#define ERA_VIRTUAL             (int)(-1)

template <class Report>
class ERaPin
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<int(uint8_t)> ReadPinHandler_t;
    typedef std::function<void(void*)> ReportPinCallback_t;
#else
    typedef int (*ReadPinHandler_t)(uint8_t);
    typedef void (*ReportPinCallback_t)(void*);
#endif

    const static int MAX_CHANNELS = 16;
    const static int MAX_PINS = ERA_MAX_GPIO_PIN;
    const static int MAX_VPINS = ERA_MAX_VIRTUAL_PIN;
    enum PinFlagT
        : uint8_t {
        PIN_ON_DELETE = 0x80
    };
    typedef struct __Pin_t {
        unsigned long prevMillis;
        unsigned long delay;
        ERaUInt_t configId;
        ERaPin::ReadPinHandler_t readPin;
        typename Report::iterator report;
        uint8_t pin;
        uint8_t pinMode;
        uint8_t channel; /* for pwm mode */
        bool enable;
        uint8_t called;
    } Pin_t;

public:
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(uint8_t, uint32_t)> WritePinHandler_t;
#else
    typedef void (*WritePinHandler_t)(uint8_t, uint32_t);
#endif
    enum VirtualTypeT
        : uint8_t {
        VIRTUAL_BASE = 0x00,
        VIRTUAL_NUMBER = 0x01,
        VIRTUAL_STRING = 0x02
    };

private:
    typedef struct __VPin_t {
        uint8_t pin;
        VirtualTypeT type;
        ERaUInt_t configId;
    } VPin_t;

public:
    class iterator
    {
    public:
        iterator()
            : Pin(nullptr)
            , pPin(nullptr)
        {}
        iterator(ERaPin* _Pin, Pin_t* _pPin)
            : Pin(_Pin)
            , pPin(_pPin)
        {}
        
        operator Pin_t*() const {
            return this->pPin;
        }

        operator bool() const {
            return this->isValid();
        }
        
        void operator() (void) const {
            if (!this->isValid()) {
                return;
            }
            this->Pin->executeNow(this->pPin);
        }

        bool isValid() const {
            return ((this->Pin != nullptr) && (this->pPin != nullptr));
        }

        Pin_t* getId() const {
            return this->pPin;
        }

        void restartPin() const {
            if (!this->isValid()) {
                return;
            }
            this->Pin->restartPin(this->pPin);
        }

        bool isEnable() const {
            if (!this->isValid()) {
                return false;
            }
            return this->Pin->isEnable(this->pPin);
        }

        void enable() const {
            if (!this->isValid()) {
                return;
            }
            this->Pin->enable(this->pPin);
        }

        void disable() const {
            if (!this->isValid()) {
                return;
            }
            this->Pin->disable(this->pPin);
        }

        bool changeInterval(unsigned long interval, unsigned long minInterval,
                            unsigned long maxInterval, float minChange) const {
            if (!this->isValid()) {
                return false;
            }
            return this->Pin->changeInterval(this->pPin, interval, minInterval, maxInterval, minChange);
        }

        void setScale(float min, float max, float rawMin, float rawMax) const {
            if (!this->isValid()) {
                return;
            }
            this->Pin->setScale(this->pPin, min, max, rawMin, rawMax);
        }

        typename Report::ScaleData_t* getScale() const {
            if (!this->isValid()) {
                return nullptr;
            }
            return this->Pin->getScale(this->pPin);
        }

        void deletePin() {
            if (!this->isValid()) {
                return;
            }
            this->Pin->deletePin(this->pPin);
            this->invalidate();
        }

    protected:
    private:
        void invalidate() {
            this->Pin = nullptr;
            this->pPin = nullptr;
        }

        ERaPin* Pin;
        Pin_t* pPin;
    };

    ERaPin(Report& _report)
        : report(_report)
        , numPin(0)
    {
        memset(this->hashID, 0, sizeof(this->hashID));
    }
    ~ERaPin()
    {}
    
    void run();
    bool updateHashID(const char* hash);

    iterator setPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval,
                        unsigned long maxInterval, float minChange,
                        ERaPin::ReportPinCallback_t cb) {
        return iterator(this, this->setupPinReport(p, pMode, readPin, interval, minInterval, maxInterval, minChange, cb));
    }

    iterator setPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval,
                        unsigned long maxInterval, float minChange,
                        ERaPin::ReportPinCallback_t cb,
                        ERaUInt_t configId) {
        return iterator(this, this->setupPinReport(p, pMode, readPin, interval, minInterval, maxInterval, minChange, cb, configId));
    }

    iterator setPinRaw(uint8_t p, ERaUInt_t configId) {
        return iterator(this, this->setupPinRaw(p, configId));
    }

    VPin_t* setPinVirtual(uint8_t p, ERaUInt_t configId,
                        VirtualTypeT type = VirtualTypeT::VIRTUAL_BASE) {
        return this->setupPinVirtual(p, configId, type);
    }

    iterator setPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel,
                            ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                            unsigned long minInterval, unsigned long maxInterval,
                            float minChange, ERaPin::ReportPinCallback_t cb) {
        return iterator(this, this->setupPWMPinReport(p, pMode, channel, readPin, interval, minInterval, maxInterval, minChange, cb));
    }

    iterator setPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel,
                            ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                            unsigned long minInterval, unsigned long maxInterval,
                            float minChange, ERaPin::ReportPinCallback_t cb,
                            ERaUInt_t configId) {
        return iterator(this, this->setupPWMPinReport(p, pMode, channel, readPin, interval, minInterval, maxInterval, minChange, cb, configId));
    }

    bool changeInterval(Pin_t* pPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval,
                        float minChange);
    void restartPin(Pin_t* pPin);
    void executeNow(Pin_t* pPin);
    void deletePin(Pin_t* pPin);
    void deleteWithPin(uint8_t p);
    void deleteAll();
    void writeAllPin(ERaPin::WritePinHandler_t writePin,
                    uint32_t value, uint8_t pMode, bool pwm = false);
    bool isEnable(Pin_t* pPin);
    void enable(Pin_t* pPin);
    void disable(Pin_t* pPin);
    void setScale(Pin_t* pPin, float min, float max, float rawMin, float rawMax);
    void enableAll();
    void disableAll();

    typename Report::ScaleData_t* getScale(Pin_t* pPin) const;
    typename Report::ScaleData_t* findScale(uint8_t p) const;
    typename Report::iterator* getReport(uint8_t p) const;
    int findPinMode(uint8_t p) const;
    int findChannelPWM(uint8_t p) const;
    ERaInt_t findConfigId(uint8_t p, const ERaParam& param) const;
    ERaInt_t findVPinConfigId(uint8_t p, const ERaParam& param) const;
    ERaInt_t findVPinConfigId(uint8_t p, const ERaDataJson::iterator& param) const;
    int findChannelFree() const;
    bool isVPinExist(uint8_t p, const WrapperBase* param) const;

protected:
private:
    Pin_t* setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval,
                        unsigned long maxInterval, float minChange,
                        ERaPin::ReportPinCallback_t cb);
    Pin_t* setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval,
                        unsigned long maxInterval, float minChange,
                        ERaPin::ReportPinCallback_t cb, ERaUInt_t configId);
    Pin_t* setupPinRaw(uint8_t p, ERaUInt_t configId);
    VPin_t* setupPinVirtual(uint8_t p, ERaUInt_t configId,
                            VirtualTypeT type = VirtualTypeT::VIRTUAL_BASE);
    Pin_t* setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel,
                            ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                            unsigned long minInterval, unsigned long maxInterval,
                            float minChange, ERaPin::ReportPinCallback_t cb);
    Pin_t* setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel,
                            ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                            unsigned long minInterval, unsigned long maxInterval,
                            float minChange, ERaPin::ReportPinCallback_t cb,
                            ERaUInt_t configId);

    bool isPinFree() const;
    bool isVPinFree() const;
    Pin_t* findPinExist(uint8_t p) const;
    VPin_t* findVPinExist(uint8_t p) const;
    Pin_t* findPinOfChannel(uint8_t channel) const;

    bool isValidPin(const Pin_t* pPin) const {
        if (pPin == nullptr) {
            return false;
        }
        return ((pPin->readPin != nullptr) ||
                (pPin->pinMode == PWM) ||
                (pPin->pinMode == RAW_PIN) ||
                (pPin->pinMode == VIRTUAL));
    }

    void setFlag(uint8_t& flags, uint8_t mask, bool value) {
        if (value) {
            flags |= mask;
        }
        else {
            flags &= ~mask;
        }
    }

    bool getFlag(uint8_t flags, uint8_t mask) {
        return (flags & mask) == mask;
    }

    Report& report;
    ERaList<Pin_t*> pin;
    ERaList<VPin_t*> vPin;
    unsigned int numPin;
    unsigned int numVPin;
    char hashID[37];

    using PinIterator = typename ERaList<Pin_t*>::iterator;
    using VPinIterator = typename ERaList<VPin_t*>::iterator;
};

template <class Report>
void ERaPin<Report>::run() {
    unsigned long currentMillis = ERaMillis();
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if ((pPin->pinMode == RAW_PIN) ||
            (pPin->pinMode == VIRTUAL)) {
            continue;
        }
        if ((currentMillis - pPin->prevMillis) < pPin->delay) {
            continue;
        }
        unsigned long skipTimes = ((currentMillis - pPin->prevMillis) / pPin->delay);
        // update time
        pPin->prevMillis += (pPin->delay * skipTimes);
        // call update data
        if (!pPin->enable) {
            continue;
        }
        if (pPin->readPin == nullptr) {
            continue;
        }
        if (pPin->pinMode != PWM) {
            pPin->report.updateReport(pPin->readPin(pPin->pin));
        }
        else {
#if defined(ESP32) &&   \
    (ESP_IDF_VERSION_MAJOR > 4)
            pPin->report.updateReport(pPin->readPin(pPin->pin), true);
#else
            pPin->report.updateReport(pPin->readPin(pPin->channel), true);
#endif
        }
    }

    PinIterator* next = nullptr;
    for (PinIterator* it = this->pin.begin(); it != e; it = next) {
        next = it->getNext();
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if (!pPin->called) {
            continue;
        }
        if (this->getFlag(pPin->called, PinFlagT::PIN_ON_DELETE)) {
            pPin->report.deleteReport();
            delete pPin;
            pPin = nullptr;
            it->get() = nullptr;
            this->pin.remove(it);
            this->numPin--;
            continue;
        }
        pPin->called = 0;
    }

    this->report.run();
}

template <class Report>
bool ERaPin<Report>::updateHashID(const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    if (strcmp(this->hashID, hash)) {
        snprintf(this->hashID, sizeof(this->hashID), hash);
        return true;
    }
    return false;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin,
                                                            unsigned long interval, unsigned long minInterval,
                                                            unsigned long maxInterval, float minChange,
                                                            ERaPin::ReportPinCallback_t cb) {
    if (!interval) {
        interval = 1;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Pin_t* pPin = this->findPinExist(p);
    if (pPin == nullptr) {
        if (!this->isPinFree()) {
            return nullptr;
        }
        pPin = new Pin_t();
        if (pPin == nullptr) {
            return nullptr;
        }
        this->pin.put(pPin);
        this->numPin++;
    }
    
    pPin->prevMillis = ERaMillis();
    pPin->delay = interval;
    pPin->readPin = readPin;
    if (pPin->report) {
        pPin->report.setScale(0, 0, 0, 0);
        pPin->report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    else {
        pPin->report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    pPin->configId = 0;
    pPin->pin = p;
    pPin->pinMode = pMode;
    pPin->channel = 0;
    pPin->enable = true;
    pPin->called = 0;
    return pPin;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin,
                                                            unsigned long interval, unsigned long minInterval,
                                                            unsigned long maxInterval, float minChange,
                                                            ERaPin::ReportPinCallback_t cb, ERaUInt_t configId) {
    if (!interval) {
        interval = 1;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Pin_t* pPin = this->findPinExist(p);
    if (pPin == nullptr) {
        if (!this->isPinFree()) {
            return nullptr;
        }
        pPin = new Pin_t();
        if (pPin == nullptr) {
            return nullptr;
        }
        this->pin.put(pPin);
        this->numPin++;
    }

    pPin->prevMillis = ERaMillis();
    pPin->delay = interval;
    pPin->readPin = readPin;
    if (pPin->report) {
        pPin->report.setScale(0, 0, 0, 0);
        pPin->report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    else {
        pPin->report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    pPin->configId = configId;
    pPin->pin = p;
    pPin->pinMode = pMode;
    pPin->channel = 0;
    pPin->enable = true;
    pPin->called = 0;
    return pPin;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::setupPinRaw(uint8_t p, ERaUInt_t configId) {
    Pin_t* pPin = this->findPinExist(p);
    if (pPin == nullptr) {
        if (!this->isPinFree()) {
            return nullptr;
        }
        pPin = new Pin_t();
        if (pPin == nullptr) {
            return nullptr;
        }
        this->pin.put(pPin);
        this->numPin++;
    }

    pPin->prevMillis = ERaMillis();
    pPin->delay = 0;
    pPin->readPin = nullptr;
    pPin->report = typename Report::iterator();
    pPin->configId = configId;
    pPin->pin = p;
    pPin->pinMode = RAW_PIN;
    pPin->channel = 0;
    pPin->enable = true;
    pPin->called = 0;
    return pPin;
}

template <class Report>
typename ERaPin<Report>::VPin_t* ERaPin<Report>::setupPinVirtual(uint8_t p, ERaUInt_t configId, VirtualTypeT type) {
    VPin_t* pVPin = this->findVPinExist(p);
    if (pVPin == nullptr) {
        if (!this->isVPinFree()) {
            return nullptr;
        }
        pVPin = new VPin_t();
        if (pVPin == nullptr) {
            return nullptr;
        }
        this->vPin.put(pVPin);
        this->numVPin++;
    }
    
    pVPin->pin = p;
    pVPin->type = type;
    pVPin->configId = configId;
    return pVPin;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel,
                                                                ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                                                                unsigned long minInterval, unsigned long maxInterval,
                                                                float minChange, ERaPin::ReportPinCallback_t cb) {
    if (!interval) {
        interval = 1;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Pin_t* pPin = this->findPinExist(p);
    if (pPin == nullptr) {
        if (!this->isPinFree()) {
            return nullptr;
        }
        pPin = new Pin_t();
        if (pPin == nullptr) {
            return nullptr;
        }
        this->pin.put(pPin);
        this->numPin++;
    }

    pPin->prevMillis = ERaMillis();
    pPin->delay = interval;
    pPin->readPin = readPin;
    if (pPin->report) {
        pPin->report.setScale(0, 0, 0, 0);
        pPin->report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    else {
        pPin->report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    pPin->configId = 0;
    pPin->pin = p;
    pPin->pinMode = pMode;
    pPin->channel = channel;
    pPin->enable = true;
    pPin->called = 0;
    return pPin;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel,
                                                                ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                                                                unsigned long minInterval, unsigned long maxInterval,
                                                                float minChange, ERaPin::ReportPinCallback_t cb,
                                                                ERaUInt_t configId) {
    if (!interval) {
        interval = 1;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Pin_t* pPin = this->findPinExist(p);
    if (pPin == nullptr) {
        if (!this->isPinFree()) {
            return nullptr;
        }
        pPin = new Pin_t();
        if (pPin == nullptr) {
            return nullptr;
        }
        this->pin.put(pPin);
        this->numPin++;
    }
    
    pPin->prevMillis = ERaMillis();
    pPin->delay = interval;
    pPin->readPin = readPin;
    if (pPin->report) {
        pPin->report.setScale(0, 0, 0, 0);
        pPin->report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    else {
        pPin->report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    pPin->configId = configId;
    pPin->pin = p;
    pPin->pinMode = pMode;
    pPin->channel = channel;
    pPin->enable = true;
    pPin->called = 0;
    return pPin;
}

template <class Report>
bool ERaPin<Report>::changeInterval(Pin_t* pPin, unsigned long interval,
                                    unsigned long minInterval, unsigned long maxInterval,
                                    float minChange) {
    if (!interval) {
        interval = 1;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }
    if (!this->isValidPin(pPin)) {
        return false;
    }

    pPin->delay = interval;
    pPin->report.changeReportableChange(minInterval, maxInterval, minChange);
    return true;
}

template <class Report>
void ERaPin<Report>::restartPin(Pin_t* pPin) {
    if (!this->isValidPin(pPin)) {
        return;
    }

    pPin->report.restartReport();
    pPin->prevMillis = ERaMillis();
}

template <class Report>
void ERaPin<Report>::executeNow(Pin_t* pPin) {
    if (!this->isValidPin(pPin)) {
        return;
    }

    pPin->prevMillis = ERaMillis() - pPin->delay;
    pPin->report();
}

template <class Report>
void ERaPin<Report>::deletePin(Pin_t* pPin) {
    if (!this->numPin) {
        return;
    }

    if (this->isValidPin(pPin)) {
        this->setFlag(pPin->called, PinFlagT::PIN_ON_DELETE, true);
    }
}

template <class Report>
void ERaPin<Report>::deleteWithPin(uint8_t p) {
    Pin_t* pPin = this->findPinExist(p);
    if (pPin == nullptr) {
        return;
    }

    this->deletePin(pPin);
}

template <class Report>
void ERaPin<Report>::deleteAll() {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (pPin == nullptr) {
            continue;
        }
        pPin->report.deleteReport();
        delete pPin;
        pPin = nullptr;
        it->get() = nullptr;
    }
    this->pin.clear();
    this->numPin = 0;

    this->report.run();

    const VPinIterator* eVPin = this->vPin.end();
    for (VPinIterator* it = this->vPin.begin(); it != eVPin; it = it->getNext()) {
        VPin_t* pVPin = it->get();
        if (pVPin == nullptr) {
            continue;
        }
        delete pVPin;
        pVPin = nullptr;
        it->get() = nullptr;
    }
    this->vPin.clear();
    this->numVPin = 0;
}

template <class Report>
void ERaPin<Report>::writeAllPin(ERaPin::WritePinHandler_t writePin,
                                uint32_t value, uint8_t pMode, bool pwm) {
    if (writePin == nullptr) {
        return;
    }
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if (pPin->pinMode == pMode) {
#if defined(ESP32) &&   \
    (ESP_IDF_VERSION_MAJOR > 4)
            writePin(pPin->pin, value);
            ERA_FORCE_UNUSED(pwm);
#else
            writePin((pwm ? pPin->channel : pPin->pin), value);
#endif
        }
    }
}

template <class Report>
bool ERaPin<Report>::isEnable(Pin_t* pPin) {
    if (this->isValidPin(pPin)) {
        return pPin->enable;
    }
    else {
        return false;
    }
}

template <class Report>
void ERaPin<Report>::enable(Pin_t* pPin) {
    if (this->isValidPin(pPin)) {
        pPin->enable = true;
        pPin->report.enable();
    }
}

template <class Report>
void ERaPin<Report>::disable(Pin_t* pPin) {
    if (this->isValidPin(pPin)) {
        pPin->enable = false;
        pPin->report.disable();
    }
}

template <class Report>
void ERaPin<Report>::setScale(Pin_t* pPin, float min, float max, float rawMin, float rawMax) {
    if (this->isValidPin(pPin)) {
        pPin->report.setScale(min, max, rawMin, rawMax);
    }
}

template <class Report>
void ERaPin<Report>::enableAll() {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (this->isValidPin(pPin)) {
            pPin->enable = true;
            pPin->report.enable();
        }
    }
}

template <class Report>
void ERaPin<Report>::disableAll() {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (this->isValidPin(pPin)) {
            pPin->enable = false;
            pPin->report.disable();
        }
    }
}

template <class Report>
typename Report::ScaleData_t* ERaPin<Report>::getScale(Pin_t* pPin) const {
    if (this->isValidPin(pPin)) {
        return pPin->report.getScale();
    }
    else {
        return nullptr;
    }
}

template <class Report>
typename Report::ScaleData_t* ERaPin<Report>::findScale(uint8_t p) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if (pPin->pin == p) {
            return pPin->report.getScale();
        }
    }

    return nullptr;
}

template <class Report>
typename Report::iterator* ERaPin<Report>::getReport(uint8_t p) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if (pPin->pin == p) {
            return &pPin->report;
        }
    }

    return nullptr;
}

template <class Report>
bool ERaPin<Report>::isPinFree() const {
    if (this->numPin >= MAX_PINS) {
        return false;
    }
    
    return true;
}

template <class Report>
bool ERaPin<Report>::isVPinFree() const {
    if (this->numVPin >= MAX_VPINS) {
        return false;
    }

    return true;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::findPinExist(uint8_t p) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if (pPin->pin == p) {
            return pPin;
        }
    }

    return nullptr;
}

template <class Report>
typename ERaPin<Report>::VPin_t* ERaPin<Report>::findVPinExist(uint8_t p) const {
    const VPinIterator* e = this->vPin.end();
    for (VPinIterator* it = this->vPin.begin(); it != e; it = it->getNext()) {
        VPin_t* pVPin = it->get();
        if (pVPin == nullptr) {
            continue;
        }
        if (pVPin->pin == p) {
            return pVPin;
        }
    }

    return nullptr;
}

template <class Report>
int ERaPin<Report>::findPinMode(uint8_t p) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if (pPin->pin == p) {
            return pPin->pinMode;
        }
    }

    return -1;
}

template <class Report>
int ERaPin<Report>::findChannelPWM(uint8_t p) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if ((pPin->pin == p) &&
            (pPin->pinMode == PWM)) {
            return pPin->channel;
        }
    }

    return -1;
}

template <class Report>
ERaInt_t ERaPin<Report>::findConfigId(uint8_t p, const ERaParam& param) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if ((pPin->pin != p) ||
            !pPin->configId) {
            continue;
        }
        if (param.isNumber()) {
            return pPin->configId;
        }
        else {
            break;
        }
    }

    return -1;
}

template <class Report>
ERaInt_t ERaPin<Report>::findVPinConfigId(uint8_t p, const ERaParam& param) const {
    const VPinIterator* e = this->vPin.end();
    for (VPinIterator* it = this->vPin.begin(); it != e; it = it->getNext()) {
        VPin_t* pVPin = it->get();
        if (pVPin == nullptr) {
            continue;
        }
        if ((pVPin->pin != p) ||
            !pVPin->configId) {
            continue;
        }
        switch (pVPin->type) {
            case VirtualTypeT::VIRTUAL_NUMBER:
                if (param.isNumber()) {
                    return pVPin->configId;
                }
                break;
            case VirtualTypeT::VIRTUAL_STRING:
                if (param.isString() || param.isObject()) {
                    return pVPin->configId;
                }
                break;
            case VirtualTypeT::VIRTUAL_BASE:
            default:
                return pVPin->configId;
        }
    }

    return -1;
}

template <class Report>
ERaInt_t ERaPin<Report>::findVPinConfigId(uint8_t p, const ERaDataJson::iterator& param) const {
    const VPinIterator* e = this->vPin.end();
    for (VPinIterator* it = this->vPin.begin(); it != e; it = it->getNext()) {
        VPin_t* pVPin = it->get();
        if (pVPin == nullptr) {
            continue;
        }
        if ((pVPin->pin != p) ||
            !pVPin->configId) {
            continue;
        }
        switch (pVPin->type) {
            case VirtualTypeT::VIRTUAL_NUMBER:
                if (param.isNumber() || param.isBool()) {
                    return pVPin->configId;
                }
                break;
            case VirtualTypeT::VIRTUAL_STRING:
                if (param.isString() || param.isNull()) {
                    return pVPin->configId;
                }
                break;
            case VirtualTypeT::VIRTUAL_BASE:
            default:
                return pVPin->configId;
        }
    }

    return -1;
}

template <class Report>
int ERaPin<Report>::findChannelFree() const {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (this->findPinOfChannel(i) == nullptr) {
            return i;
        }
    }

    return -1;
}

template <class Report>
bool ERaPin<Report>::isVPinExist(uint8_t p, const WrapperBase* param) const {
    VPin_t* pVPin = this->findVPinExist(p);
    if (pVPin == nullptr) {
        return false;
    }
    if (param == nullptr) {
        return true;
    }

    switch (pVPin->type) {
        case VirtualTypeT::VIRTUAL_NUMBER:
            if (param->isNumber()) {
                return true;
            }
            break;
        case VirtualTypeT::VIRTUAL_STRING:
            if (param->isString()) {
                return true;
            }
            break;
        case VirtualTypeT::VIRTUAL_BASE:
        default:
            return true;
    }

    return false;
}

template <class Report>
typename ERaPin<Report>::Pin_t* ERaPin<Report>::findPinOfChannel(uint8_t channel) const {
    const PinIterator* e = this->pin.end();
    for (PinIterator* it = this->pin.begin(); it != e; it = it->getNext()) {
        Pin_t* pPin = it->get();
        if (!this->isValidPin(pPin)) {
            continue;
        }
        if ((pPin->channel == channel) &&
            (pPin->pinMode == PWM)) {
            return pPin;
        }
    }

    return nullptr;
}

using PinEntry = ERaPin<ERaReport>::iterator;

#endif /* INC_ERA_PIN_HPP_ */
