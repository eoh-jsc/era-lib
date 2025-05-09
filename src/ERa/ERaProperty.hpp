#ifndef INC_ERA_PROPERTY_HPP_
#define INC_ERA_PROPERTY_HPP_

#include <ERa/ERaDefine.hpp>
#include <Utility/ERaUtility.hpp>
#include <ERa/ERaReport.hpp>
#include <ERa/ERaParam.hpp>
#include "types/WrapperTypes.hpp"

#if !defined(VIRTUAL)
    #define VIRTUAL                     0xFF
#endif

#define ERA_PROPERTY_MIN_CHANGE         0.0f
#define ERA_PROPERTY_MIN_INTERVAL       500UL
#define ERA_PROPERTY_MAX_INTERVAL       60000UL

#define ERA_PROPERTY_MAX_ID_LENGTH      65

#define addProperty(p, ...)             addPropertyReal(ERA_F(#p), p, __VA_ARGS__)
#define addPropertyT(p, ...)            addPropertyRealT(ERA_F(#p), p, __VA_ARGS__)

enum PermissionT {
    PERMISSION_READ = 0x01,
    PERMISSION_WRITE = 0x02,
    PERMISSION_READ_WRITE = PERMISSION_READ | PERMISSION_WRITE,
    PERMISSION_CLOUD = 0x80,
    PERMISSION_CLOUD_READ_WRITE = PERMISSION_CLOUD | PERMISSION_READ_WRITE
};

template <class Api>
class ERaProperty
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> PropertyCallback_t;
    typedef std::function<void(void*)> PropertyCallback_p_t;
    typedef std::function<void(void*)> ReportPropertyCallback_t;
    typedef std::function<bool(void)> OnPublishCallback_t;
#else
    typedef void (*PropertyCallback_t)(void);
    typedef void (*PropertyCallback_p_t)(void*);
    typedef void (*ReportPropertyCallback_t)(void*);
    typedef bool (*OnPublishCallback_t)(void);
#endif

    const static int MAX_PROPERTIES = ERA_MAX_PROPERTIES;
    typedef struct __Property_t {
        ERaProperty::PropertyCallback_t callback;
        ERaProperty::PropertyCallback_p_t callback_p;
#if defined(ERA_ON_PUBLISH_PROPERTY)
        ERaProperty::OnPublishCallback_t onPublishCallback;
#endif
        ERaReport::iterator report;
        unsigned long prevMillis;
        PermissionT permission;
        void* param;
        void* allocPointer;
        WrapperBase* value;
        ERaParam id;
    } Property_t;

public:
    class iterator
    {
        friend class ERaProperty;

    public:
        iterator()
            : prop(nullptr)
            , pProp(nullptr)
        {}
        iterator(ERaProperty* _prop, Property_t* _pProp)
            : prop(_prop)
            , pProp(_pProp)
        {}

        bool isValid() const {
            return ((this->prop != nullptr) && (this->pProp != nullptr));
        }

        operator Property_t*() const {
            return this->pProp;
        }

        operator bool() const {
            return this->isValid();
        }

        iterator& onUpdate(ERaProperty::PropertyCallback_t cb) {
            if (this->isValid()) {
                this->prop->onUpdate(this->pProp, cb);
            }
            return (*this);
        }

        iterator& onUpdate(ERaProperty::PropertyCallback_p_t cb, void* args) {
            if (this->isValid()) {
                this->prop->onUpdate(this->pProp, cb, args);
            }
            return (*this);
        }

#if defined(ERA_ON_PUBLISH_PROPERTY)
        iterator& onPublish(ERaProperty::OnPublishCallback_t cb) {
            if (this->isValid()) {
                this->prop->onPublish(this->pProp, cb);
            }
            return (*this);
        }
#endif

        iterator& publish(bool send = false) {
            if (this->isValid()) {
                this->prop->publish(this->pProp, send);
            }
            return (*this);
        }

        iterator& publishEvery(unsigned long interval = 1000UL) {
            if (this->isValid()) {
                this->prop->publishEvery(this->pProp, interval);
            }
            return (*this);
        }

        iterator& publishOnChange(float minChange = 1.0f,
                                unsigned long minInterval = 1000UL,
                                unsigned long maxInterval = 60000UL) {
            if (this->isValid()) {
                this->prop->publishOnChange(this->pProp, minChange, minInterval, maxInterval);
            }
            return (*this);
        }

        ERaReport::iterator* getReport() const {
            if (this->isValid()) {
                return &this->pProp->report;
            }
            return nullptr;
        }

        WrapperBase* getValue() const {
            if (this->isValid()) {
                return this->pProp->value;
            }
            return nullptr;
        }

    protected:
        iterator& allocatorPointer(void* ptr) {
            if (this->isValid()) {
                this->prop->allocatorPointer(this->pProp, ptr);
            }
            return (*this);
        }

        iterator& resetUpdate() {
            if (this->isValid()) {
                this->prop->resetUpdate(this->pProp);
            }
            return (*this);
        }

    private:
        void invalidate() {
            this->prop = nullptr;
            this->pProp = nullptr;
        }

        ERaProperty* prop;
        Property_t* pProp;
    };

    ERaProperty()
        : numProperty(0)
        , timeout(100L)
        , publishSettings {
            .minChange = ERA_PROPERTY_MIN_CHANGE,
            .minInterval = ERA_PROPERTY_MIN_INTERVAL,
            .maxInterval = ERA_PROPERTY_MAX_INTERVAL
        }
    {}
    ~ERaProperty()
    {}

    iterator getPropertyVirtual(uint16_t pin) {
        return iterator(this, this->isPropertyIdExist(pin));
    }

    iterator getPropertyReal(const char* id) {
        return iterator(this, this->isPropertyIdExist(id));
    }

    ERaReport& getPropertyReport() {
        return this->ERaPropRp;
    }

    template <typename T>
    iterator addPropertyVirtual(T& value, PermissionT const permission) {
        int pin = this->findVirtualPin();
        if (pin < 0) {
            return iterator();
        }
        return this->addPropertyVirtual(pin, value, permission);
    }

    iterator addPropertyVirtual(uint16_t pin, bool& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperBool(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, int& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperInt(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, unsigned int& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedInt(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperLong(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, unsigned long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedLong(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, long long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperLongLong(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, unsigned long long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedLongLong(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, float& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperFloat(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, double& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperDouble(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, ERaDataJson& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperObject(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, ERaString& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperString(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }

    iterator addPropertyVirtual(uint16_t pin, WrapperBase& value, PermissionT const permission) {
        return iterator(this, this->setupProperty(pin, &value, permission));
    }

#if defined(ERA_HAS_TYPE_TRAITS_H)
    template <typename T, typename = enable_if_t<!std::is_base_of<WrapperBase, T>::value>>
    iterator addPropertyVirtual(uint16_t pin, T& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperNumber<T>(value);
        return iterator(this, this->setupProperty(pin, wrapper, permission));
    }
#endif

    iterator addPropertyReal(const char* id, bool& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperBool(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, int& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperInt(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, unsigned int& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedInt(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, unsigned long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, long long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperLongLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, unsigned long long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedLongLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, float& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperFloat(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, double& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperDouble(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, ERaDataJson& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperObject(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, ERaString& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperString(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const char* id, WrapperBase& value, PermissionT const permission) {
        return iterator(this, this->setupProperty(id, &value, permission));
    }

#if defined(ERA_HAS_TYPE_TRAITS_H)
    template <typename T, typename = enable_if_t<!std::is_base_of<WrapperBase, T>::value>>
    iterator addPropertyReal(const char* id, T& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperNumber<T>(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }
#endif

#if defined(ERA_HAS_PROGMEM)
    iterator addPropertyReal(const __FlashStringHelper* id, bool& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperBool(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, int& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperInt(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, unsigned int& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedInt(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, unsigned long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, long long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperLongLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, unsigned long long& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperUnsignedLongLong(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, float& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperFloat(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, double& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperDouble(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, ERaDataJson& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperObject(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, ERaString& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperString(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }

    iterator addPropertyReal(const __FlashStringHelper* id, WrapperBase& value, PermissionT const permission) {
        return iterator(this, this->setupProperty(id, &value, permission));
    }

#if defined(ERA_HAS_TYPE_TRAITS_H)
    template <typename T, typename = enable_if_t<!std::is_base_of<WrapperBase, T>::value>>
    iterator addPropertyReal(const __FlashStringHelper* id, T& value, PermissionT const permission) {
        WrapperBase* wrapper = new WrapperNumber<T>(value);
        return iterator(this, this->setupProperty(id, wrapper, permission));
    }
#endif
#endif

    void setPropertyPublishSettings(float minChange = ERA_PROPERTY_MIN_CHANGE,
                                    unsigned long minInterval = ERA_PROPERTY_MIN_INTERVAL,
                                    unsigned long maxInterval = ERA_PROPERTY_MAX_INTERVAL) {
        if (minInterval < ERA_PROPERTY_MIN_INTERVAL) {
            minInterval = ERA_PROPERTY_MIN_INTERVAL;
        }
        if (maxInterval < ERA_PROPERTY_MAX_INTERVAL) {
            maxInterval = ERA_PROPERTY_MAX_INTERVAL;
        }
        this->publishSettings.minChange = minChange;
        this->publishSettings.minInterval = minInterval;
        this->publishSettings.maxInterval = maxInterval;
    }

protected:
    void run();
    void handler(uint16_t pin, const ERaParam& param);
    void handler(const char* id, const ERaParam& param);
    void updateProperty(const ERaPin<ERaReport>& pin);
    void publishAll(bool onlyNumber = false);

#if !defined(ERA_VIRTUAL_WRITE_LEGACY)
    template <typename T>
    void virtualWriteProperty(uint16_t pin, const T& value, bool send) {
        Property_t* pProp = this->isPropertyIdExist(pin);
        if (pProp == nullptr) {
        }
        else if (pProp->value->isNumber()) {
            (*pProp->value) = value;
            pProp->report.updateReport(value, false, false);
            if (send) {
                pProp->report();
            }
            return;
        }
        else {
            return;
        }
        double* pValue = this->create<double>(value);
        if (pValue == nullptr) {
            return;
        }
        WrapperBase* wrapper = new WrapperDouble(*pValue);
        if (wrapper == nullptr) {
            free(pValue);
            pValue = nullptr;
            return;
        }
        this->addPropertyVirtual(pin, *wrapper, PermissionT::PERMISSION_CLOUD_READ_WRITE).
                                 publishOnChange(this->publishSettings.minChange,
                                 this->publishSettings.minInterval, this->publishSettings.maxInterval).
                                 publish(false).allocatorPointer(pValue);
    }

    void virtualWriteProperty(uint16_t pin, const ERaParam& value, bool send) {
        if (value.isNumber()) {
            this->virtualWriteProperty(pin, value.getDouble(), send);
        }
        else if (value.isString()) {
            this->virtualWriteProperty(pin, value.getString(), send);
        }
        else if (value.isObject()) {
            this->virtualWriteProperty(pin, *value.getObject(), send);
        }
    }

    void virtualWriteProperty(uint16_t pin, const ERaDataJson& value, bool send) {
        if (!value.isSendJSON()) {
            this->virtualWriteProperty(pin, const_cast<ERaDataJson&>(value).getString(), send);
        }
        else if (value.isObject()) {
            this->virtualObjectProperty(pin, value, send);
        }
        else if (value.isArray()) {
            this->virtualArrayProperty(pin, value, send);
        }
    }

    void virtualWriteProperty(uint16_t pin, const ERaString& value, bool send) {
        this->virtualWriteProperty(pin, value.getString(), send);
    }

#if defined(ERA_STRING_WRITE_LEGACY)
    void virtualWriteProperty(uint16_t pin, const char* value, bool send) {
        this->thisApi().virtualWriteSingle(pin, value, false);
        ERA_FORCE_UNUSED(send);
    }
#else
    void virtualWriteProperty(uint16_t pin, const char* value, bool send) {
        Property_t* pProp = this->isPropertyIdExist(pin);
        if (pProp != nullptr) {
            (*pProp->value) = value;
            if (send) {
                pProp->report();
                pProp->value->updated();
            }
            return;
        }
        ERaString* pValue = this->create<ERaString>(value);
        if (pValue == nullptr) {
            return;
        }
        WrapperBase* wrapper = new WrapperString(*pValue);
        if (wrapper == nullptr) {
            pValue->~ERaString();
            free(pValue);
            pValue = nullptr;
            return;
        }
        wrapper->setOptions(0x01);
        this->addPropertyVirtual(pin, *wrapper, PermissionT::PERMISSION_CLOUD_READ_WRITE).
                                 publishOnChange(this->publishSettings.minChange,
                                 this->publishSettings.minInterval, this->publishSettings.maxInterval).
                                 publish(false).allocatorPointer(pValue).resetUpdate();
    }
#endif

    void virtualWriteProperty(uint16_t pin, char* value, bool send) {
        this->virtualWriteProperty(pin, (const char*)value, send);
    }

    void virtualObjectProperty(uint16_t pin, const ERaDataJson& value, bool send) {
        this->virtualJsonProperty<WrapperObject>(pin, value, send);
    }

    void virtualArrayProperty(uint16_t pin, const ERaDataJson& value, bool send) {
        this->virtualJsonProperty<WrapperArray>(pin, value, send);
    }

    template <typename T>
    void virtualJsonProperty(uint16_t pin, const ERaDataJson& value, bool send) {
        Property_t* pProp = this->isPropertyIdExist(pin);
        if (pProp != nullptr) {
            (*pProp->value) = const_cast<ERaDataJson&>(value).getString();
            if (send) {
                pProp->report();
                pProp->value->updated();
            }
            return;
        }
        ERaDataJson* pValue = this->create(value);
        if (pValue == nullptr) {
            return;
        }
        WrapperBase* wrapper = new T(*pValue);
        if (wrapper == nullptr) {
            pValue->~ERaDataJson();
            free(pValue);
            pValue = nullptr;
            return;
        }
        wrapper->setOptions(0x01);
        this->addPropertyVirtual(pin, *wrapper, PermissionT::PERMISSION_CLOUD_READ_WRITE).
                                 publishOnChange(this->publishSettings.minChange,
                                 this->publishSettings.minInterval, this->publishSettings.maxInterval).
                                 publish(false).allocatorPointer(pValue).resetUpdate();
    }
#endif

private:
    void updateValue(const Property_t* pProp);
    Property_t* setupProperty(uint16_t pin, WrapperBase* value, PermissionT const permission);
    Property_t* setupProperty(const char* id, WrapperBase* value, PermissionT const permission);
#if defined(ERA_HAS_PROGMEM)
    Property_t* setupProperty(const __FlashStringHelper* id, WrapperBase* value, PermissionT const permission);
#endif

    void getValue(Property_t* pProp, const ERaParam& param);
    bool onUpdate(Property_t* pProp, ERaProperty::PropertyCallback_t cb);
    bool onUpdate(Property_t* pProp, ERaProperty::PropertyCallback_p_t cb, void* args);
#if defined(ERA_ON_PUBLISH_PROPERTY)
    bool onPublish(Property_t* pProp, ERaProperty::OnPublishCallback_t cb);
#endif
    bool publish(Property_t* pProp, bool send = false);
    bool publishEvery(Property_t* pProp, unsigned long interval = 1000UL);
    bool publishOnChange(Property_t* pProp, float minChange = 1.0f,
                        unsigned long minInterval = 1000UL,
                        unsigned long maxInterval = 60000UL);
    bool publishProperty(Property_t* pProp, bool onlyNumber = false);
    bool allocatorPointer(Property_t* pProp, void* ptr);
    bool resetUpdate(Property_t* pProp);
    bool isPropertyFree();
    int findVirtualPin();

    template <typename T>
    Property_t* isPropertyIdExist(T pin);

    size_t splitString(char* strInput, const char* delims);

    void onCallbackVirtual(const Property_t* const pProp);
    void onCallbackReal(const Property_t* const pProp);
    void onCallbackProperty(const Property_t* const pProp, bool send = false);
    void onCallback(void* args);
#if !defined(ERA_HAS_FUNCTIONAL_H)
    static void _onCallback(void* args);
#endif

    template <typename T>
    T* create(const T& value) {
        T* ptr = (T*)malloc(sizeof(T));
        if (ptr != nullptr) {
            memset((void*)ptr, 0, sizeof(T));
            (*ptr) = value;
        }
        return ptr;
    }

    bool isValidProperty(const Property_t* pProp) {
        if (pProp == nullptr) {
            return false;
        }
        return true;
    }

    bool getFlag(uint8_t flags, uint8_t mask) {
        return (flags & mask) == mask;
    }

    inline
    const Api& thisApi() const {
        return static_cast<const Api&>(*this);
    }

    inline
    Api& thisApi() {
        return static_cast<Api&>(*this);
    }

#if defined(ERA_HAS_FUNCTIONAL_H)
    ReportPropertyCallback_t propertyCb = [&, this](void* args) {
        this->onCallback(args);
    };
#else
    ReportPropertyCallback_t propertyCb = [](void* args) {
        ERaProperty::_onCallback(args);
    };
#endif

    ERaList<Property_t*> ERaProp;
    ERaReport ERaPropRp;
    unsigned int numProperty;
    unsigned long timeout;

    ReportSettings_t publishSettings;

    using PropertyIterator = typename ERaList<Property_t*>::iterator;
};

template <class Api>
void ERaProperty<Api>::run() {
    unsigned long currentMillis = ERaMillis();
    const PropertyIterator* e = this->ERaProp.end();
    for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = it->getNext()) {
        Property_t* pProp = it->get();
        if (!this->isValidProperty(pProp)) {
            continue;
        }
        if ((currentMillis - pProp->prevMillis) < this->timeout) {
            continue;
        }
        unsigned long skipTimes = ((currentMillis - pProp->prevMillis) / this->timeout);
        // update time
        pProp->prevMillis += (this->timeout * skipTimes);
        // update value
        if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_READ)) {
            continue;
        }
        if (pProp->value == nullptr) {
            continue;
        }
        this->updateValue(pProp);
    }

    this->ERaPropRp.run();
}

template <class Api>
void ERaProperty<Api>::updateValue(const Property_t* pProp) {
    switch (pProp->value->getType()) {
        case WrapperTypeT::WRAPPER_TYPE_BOOL:
            pProp->report.updateReport(pProp->value->getBool(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_INT:
            pProp->report.updateReport(pProp->value->getInt(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT:
            pProp->report.updateReport(pProp->value->getUnsignedInt(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_LONG:
            pProp->report.updateReport(pProp->value->getLong(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG:
            pProp->report.updateReport(pProp->value->getUnsignedLong(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_LONG_LONG:
            pProp->report.updateReport(pProp->value->getLongLong(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG:
            pProp->report.updateReport(pProp->value->getUnsignedLongLong(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_FLOAT:
        case WrapperTypeT::WRAPPER_TYPE_DOUBLE:
        case WrapperTypeT::WRAPPER_TYPE_NUMBER:
            pProp->report.updateReport(pProp->value->getDouble(), false, false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_STRING:
        case WrapperTypeT::WRAPPER_TYPE_OBJECT:
        case WrapperTypeT::WRAPPER_TYPE_ARRAY:
            if (pProp->value->updated()) {
                pProp->report();
            }
            break;
        default:
            break;
    }
}

template <class Api>
void ERaProperty<Api>::handler(uint16_t pin, const ERaParam& param) {
    bool found {false};
    Property_t* pProp = nullptr;
    const PropertyIterator* e = this->ERaProp.end();
    for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = it->getNext()) {
        pProp = it->get();
        if (!this->isValidProperty(pProp)) {
            continue;
        }
        if (!pProp->id.isNumber()) {
            continue;
        }
        if (pProp->id == pin) {
            found = true;
            this->getValue(pProp, param);
            break;
        }
    }
#if !defined(ERA_VIRTUAL_WRITE_LEGACY)

#if defined(ERA_STRING_WRITE_LEGACY)
    if (!param.isNumber()) {
        return;
    }
#endif
    if (found) {
    }
    else if (param.hasNonObject()) {
        this->virtualWriteProperty(pin, param, false);
    }
    else if (param.isObject() && param.getObject()->isObject()) {
        this->virtualObjectProperty(pin, *param.getObject(), false);
    }
    else if (param.isObject() && param.getObject()->isArray()) {
        this->virtualArrayProperty(pin, *param.getObject(), false);
    }

#endif
    ERA_FORCE_UNUSED(found);
}

template <class Api>
void ERaProperty<Api>::handler(const char* id, const ERaParam& param) {
    bool found {false};
    Property_t* pProp = nullptr;
    const PropertyIterator* e = this->ERaProp.end();
    for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = it->getNext()) {
        pProp = it->get();
        if (!this->isValidProperty(pProp)) {
            continue;
        }
        if (!pProp->id.isString()) {
            continue;
        }
        if (pProp->id == id) {
            found = true;
            this->getValue(pProp, param);
            break;
        }
    }
}

#if defined(ERA_VIRTUAL_WRITE_LEGACY)
    template <class Api>
    void ERaProperty<Api>::updateProperty(const ERaPin<ERaReport>& pin) {
        ERA_FORCE_UNUSED(pin);
    }

    template <class Api>
    void ERaProperty<Api>::publishAll(bool onlyNumber) {
        ERA_FORCE_UNUSED(onlyNumber);
    }
#else
    template <class Api>
    void ERaProperty<Api>::updateProperty(const ERaPin<ERaReport>& pin) {
        Property_t* pProp = nullptr;
        PropertyIterator* next = nullptr;
        const PropertyIterator* e = this->ERaProp.end();
        for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = next) {
            next = it->getNext();
            pProp = it->get();
            if (!this->isValidProperty(pProp)) {
                continue;
            }
            if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_CLOUD)) {
                this->publishProperty(pProp, true);
                continue;
            }
            if (!pProp->id.isNumber()) {
                continue;
            }
            if (pProp->id.getInt() < 0) {
                continue;
            }
            if (pin.isVPinExist(pProp->id.getInt(), pProp->value)) {
                pProp->report.executeNow();
                continue;
            }
            else {
                if (pProp->value != nullptr) {
                    delete pProp->value;
                    pProp->value = nullptr;
                }
                if (pProp->allocPointer != nullptr) {
                    free(pProp->allocPointer);
                    pProp->allocPointer = nullptr;
                }
                pProp->report.deleteReport();
                delete pProp;
                pProp = nullptr;
                it->get() = nullptr;
                this->ERaProp.remove(it);
            }
        }

        this->ERaPropRp.run();
    }

    template <class Api>
    void ERaProperty<Api>::publishAll(bool onlyNumber) {
        const PropertyIterator* e = this->ERaProp.end();
        for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = it->getNext()) {
            Property_t* pProp = it->get();
            this->publishProperty(pProp, onlyNumber);
        }
    }
#endif

template <class Api>
void ERaProperty<Api>::getValue(Property_t* pProp, const ERaParam& param) {
    if (pProp == nullptr) {
        return;
    }
    if (pProp->value == nullptr) {
        return;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_WRITE)) {
        return;
    }

    switch (pProp->value->getType()) {
        case WrapperTypeT::WRAPPER_TYPE_BOOL:
        case WrapperTypeT::WRAPPER_TYPE_INT:
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT:
        case WrapperTypeT::WRAPPER_TYPE_LONG:
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG:
            if (param.isNumber()) {
                (*pProp->value) = param.getInt();
                pProp->report.updateReport(param.getInt());
            }
            break;
        case WrapperTypeT::WRAPPER_TYPE_LONG_LONG:
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG:
        case WrapperTypeT::WRAPPER_TYPE_FLOAT:
        case WrapperTypeT::WRAPPER_TYPE_DOUBLE:
        case WrapperTypeT::WRAPPER_TYPE_NUMBER:
            if (param.isNumber()) {
                (*pProp->value) = param.getDouble();
                pProp->report.updateReport(param.getDouble());
            }
            break;
        case WrapperTypeT::WRAPPER_TYPE_STRING:
        case WrapperTypeT::WRAPPER_TYPE_OBJECT:
        case WrapperTypeT::WRAPPER_TYPE_ARRAY:
            if (param.isString()) {
                (*pProp->value) = param.getString();
                if (pProp->value->updated()) {
#if defined(ERA_STRING_REPORT_ON_WRITE)
                    pProp->report();
#endif
                }
            }
            else if (param.isObject()) {
            }
            break;
        default:
            return;
    }
    if (pProp->callback_p != nullptr) {
        pProp->callback_p(pProp->param);
    }
    else if (pProp->callback != nullptr) {
        pProp->callback();
    }
}

template <class Api>
typename ERaProperty<Api>::Property_t* ERaProperty<Api>::setupProperty(uint16_t pin, WrapperBase* value,
                                                                    PermissionT const permission) {
    if (!this->isPropertyFree()) {
        return nullptr;
    }
    if (value == nullptr) {
        return nullptr;
    }

    Property_t* pProp = new Property_t();
    if (pProp == nullptr) {
        return nullptr;
    }

    pProp->id = pin;
    pProp->value = value;
    pProp->callback = nullptr;
    pProp->callback_p = nullptr;
#if defined(ERA_ON_PUBLISH_PROPERTY)
    pProp->onPublishCallback = nullptr;
#endif
    pProp->param = nullptr;
    pProp->allocPointer = nullptr;
    pProp->prevMillis = ERaMillis();
    pProp->permission = permission;
    pProp->report = ERaReport::iterator();
    this->publishOnChange(pProp);
    this->ERaProp.put(pProp);
    this->numProperty++;
    return pProp;
}

template <class Api>
typename ERaProperty<Api>::Property_t* ERaProperty<Api>::setupProperty(const char* id, WrapperBase* value,
                                                                    PermissionT const permission) {
    if (!this->isPropertyFree()) {
        return nullptr;
    }
    if (id == nullptr) {
        return nullptr;
    }
    if (value == nullptr) {
        return nullptr;
    }

    Property_t* pProp = new Property_t();
    if (pProp == nullptr) {
        return nullptr;
    }

    pProp->id = -1;
    pProp->id = id;
    pProp->value = value;
    pProp->callback = nullptr;
    pProp->callback_p = nullptr;
#if defined(ERA_ON_PUBLISH_PROPERTY)
    pProp->onPublishCallback = nullptr;
#endif
    pProp->param = nullptr;
    pProp->allocPointer = nullptr;
    pProp->prevMillis = ERaMillis();
    pProp->permission = permission;
    pProp->report = ERaReport::iterator();
    this->publishOnChange(pProp);
    this->ERaProp.put(pProp);
    this->numProperty++;
    return pProp;
}

#if defined(ERA_HAS_PROGMEM)
    template <class Api>
    typename ERaProperty<Api>::Property_t* ERaProperty<Api>::setupProperty(const __FlashStringHelper* id,
                                                                            WrapperBase* value, PermissionT const permission) {
        if (id == nullptr) {
            return nullptr;
        }
        PGM_P p = reinterpret_cast<PGM_P>(id);
        size_t size = strlen_P(p) + sizeof("");
        char str[size] {0};
        memcpy_P(str, p, size);
        return this->setupProperty(str, value, permission);
    }
#endif

template <class Api>
bool ERaProperty<Api>::onUpdate(Property_t* pProp, ERaProperty::PropertyCallback_t cb) {
    if (pProp == nullptr) {
        return false;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_WRITE)) {
        return false;
    }

    pProp->callback = cb;
    return true;
}

template <class Api>
bool ERaProperty<Api>::onUpdate(Property_t* pProp, ERaProperty::PropertyCallback_p_t cb, void* args) {
    if (pProp == nullptr) {
        return false;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_WRITE)) {
        return false;
    }

    pProp->callback_p = cb;
    pProp->param = args;
    return true;
}

#if defined(ERA_ON_PUBLISH_PROPERTY)
    template <class Api>
    bool ERaProperty<Api>::onPublish(Property_t* pProp, ERaProperty::OnPublishCallback_t cb) {
        if (pProp == nullptr) {
            return false;
        }
        if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_READ)) {
            return false;
        }

        pProp->onPublishCallback = cb;
        return true;
    }
#endif

template <class Api>
bool ERaProperty<Api>::publish(Property_t* pProp, bool send) {
    if (pProp == nullptr) {
        return false;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_READ)) {
        return false;
    }

    this->onCallbackProperty(pProp, send);
    return true;
}

template <class Api>
bool ERaProperty<Api>::publishEvery(Property_t* pProp, unsigned long interval) {
    if (pProp == nullptr) {
        return false;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_READ)) {
        return false;
    }

    if (pProp->report) {
        pProp->report.reportEvery(interval);
    }
    else {
        this->publishOnChange(pProp, 1.0f, 1000UL, interval);
    }
    return true;
}

template <class Api>
bool ERaProperty<Api>::publishOnChange(Property_t* pProp, float minChange,
                                        unsigned long minInterval,
                                        unsigned long maxInterval) {
    if (pProp == nullptr) {
        return false;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_READ)) {
        return false;
    }

    if (pProp->report) {
        pProp->report.changeReportableChange(minInterval, maxInterval, minChange);
    }
    else {
        pProp->report = this->ERaPropRp.setReporting(minInterval, maxInterval, minChange, this->propertyCb, pProp);
    }
    if (pProp->value->isNumber()) {
        this->updateValue(pProp);
    }
    else {
#if defined(ERA_STRING_REPORT_INTERVAL)
        pProp->report.updateReport(0.0f, false, false);
#endif
    }
    return true;
}

template <class Api>
bool ERaProperty<Api>::publishProperty(Property_t* pProp, bool onlyNumber) {
    if (pProp == nullptr) {
        return false;
    }
    if (!this->getFlag(pProp->permission, PermissionT::PERMISSION_READ)) {
        return false;
    }

    if (!pProp->id.isNumber()) {
        return false;
    }
    if (pProp->id.getInt() < 0) {
        return false;
    }
    if (onlyNumber && !pProp->value->isNumber()) {
        return false;
    }

    pProp->report.executeNow();
    return true;
}

template <class Api>
bool ERaProperty<Api>::allocatorPointer(Property_t* pProp, void* ptr) {
    if (pProp == nullptr) {
        return false;
    }

    pProp->allocPointer = ptr;
    return true;
}

template <class Api>
bool ERaProperty<Api>::resetUpdate(Property_t* pProp) {
    if (pProp == nullptr) {
        return false;
    }

    pProp->value->updated();
    return true;
}

template <class Api>
void ERaProperty<Api>::onCallbackVirtual(const Property_t* const pProp) {
    if (pProp->value->isNumber() && ERaIsSpN(pProp->value->getDouble())) {
        return;
    }

    switch (pProp->value->getType()) {
        case WrapperTypeT::WRAPPER_TYPE_BOOL:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getBool(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_INT:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getInt(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getUnsignedInt(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_LONG:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getLong(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getUnsignedLong(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_LONG_LONG:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getLongLong(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getUnsignedLongLong(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_FLOAT:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getFloat(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_DOUBLE:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getDouble(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_NUMBER:
            this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getNumber(), false);
            break;
        case WrapperTypeT::WRAPPER_TYPE_STRING:
            if (pProp->value->getString() != nullptr) {
                this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->getString(), false);
            }
            break;
        case WrapperTypeT::WRAPPER_TYPE_OBJECT:
        case WrapperTypeT::WRAPPER_TYPE_ARRAY:
            if (pProp->value->getString() != nullptr) {
                this->thisApi().virtualWriteSingle(pProp->id.getInt(), pProp->value->toJSON(),
                                                   pProp->value->isSendJSON() || ERA_API_JSON);
            }
            break;
        default:
            break;
    }
}

template <class Api>
void ERaProperty<Api>::onCallbackReal(const Property_t* const pProp) {
#if defined(ERA_SPECIFIC)
    if (pProp->value->getType() == WrapperTypeT::WRAPPER_TYPE_STRING) {
        return this->thisApi().specificDataWrite(pProp->id, pProp->value->getString(), true, true);
    }

    const char* id = pProp->id.getString();
    if (id == nullptr) {
        return;
    }

    char copy[65] {0};
    CopyToString(id, copy);
    ERaDataBuff arrayId(copy, strlen(copy) + 1, sizeof(copy));
    this->splitString(copy, ":");

    if (arrayId.size() < 2) {
        return;
    }

    cJSON* root = cJSON_CreateObject();
    if (root == nullptr) {
        return;
    }
    cJSON* dataItem = cJSON_CreateObject();
    if (dataItem == nullptr) {
        cJSON_Delete(root);
        root = nullptr;
        return;
    }
    char topic[65] {0};
    FormatString(topic, ERA_PUB_PREFIX_ZIGBEE_DATA_TOPIC, arrayId.at(0).getString());

    cJSON_AddStringToObject(root, "type", "device_data");
    cJSON_AddItemToObject(root, "data", dataItem);

    const PropertyIterator* e = this->ERaProp.end();
    for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = it->getNext()) {
        Property_t* property = it->get();
        if (!this->isValidProperty(property)) {
            continue;
        }
        if (!property->id.isString()) {
            continue;
        }
        if (property->id.getString() == nullptr) {
            continue;
        }
        if (strncmp(property->id.getString(),
                    arrayId.at(0).getString(),
                    strlen(arrayId.at(0).getString()))) {
            continue;
        }
        const char* ptrColon = strchr(property->id.getString(), ':');
        if (ptrColon == nullptr) {
            continue;
        }
        ptrColon++;

        if (property == pProp) {
            switch (property->value->getType()) {
                case WrapperTypeT::WRAPPER_TYPE_BOOL:
                    property->report.updateReport(property->value->getBool(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getBool());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_INT:
                    property->report.updateReport(property->value->getInt(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getInt());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT:
                    property->report.updateReport(property->value->getUnsignedInt(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getUnsignedInt());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_LONG:
                    property->report.updateReport(property->value->getLong(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getLong());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG:
                    property->report.updateReport(property->value->getUnsignedLong(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getUnsignedLong());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_LONG_LONG:
                    property->report.updateReport(property->value->getLongLong(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getLongLong());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG:
                    property->report.updateReport(property->value->getUnsignedLongLong(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getUnsignedLongLong());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_FLOAT:
                    property->report.updateReport(property->value->getFloat(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getFloat());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_DOUBLE:
                    property->report.updateReport(property->value->getDouble(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getDouble());
                    break;
                case WrapperTypeT::WRAPPER_TYPE_NUMBER:
                    property->report.updateReport(property->value->getNumber(), false, false);
                    cJSON_SetNumberToObject(dataItem, ptrColon, property->value->getNumber());
                    break;
                default:
                    break;
            }
            continue;
        }

        if (!property->report.isReported()) {
            continue;
        }

        switch (property->value->getType()) {
            case WrapperTypeT::WRAPPER_TYPE_BOOL:
                cJSON_SetNumberToObject(dataItem, ptrColon, (bool)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_INT:
                cJSON_SetNumberToObject(dataItem, ptrColon, (int)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_INT:
                cJSON_SetNumberToObject(dataItem, ptrColon, (unsigned int)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_LONG:
                cJSON_SetNumberToObject(dataItem, ptrColon, (long)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG:
                cJSON_SetNumberToObject(dataItem, ptrColon, (unsigned long)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_LONG_LONG:
                cJSON_SetNumberToObject(dataItem, ptrColon, (long long)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_UNSIGNED_LONG_LONG:
                cJSON_SetNumberToObject(dataItem, ptrColon, (unsigned long long)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_FLOAT:
                cJSON_SetNumberToObject(dataItem, ptrColon, (float)property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_DOUBLE:
                cJSON_SetNumberToObject(dataItem, ptrColon, property->report.getPreviousValue());
                break;
            case WrapperTypeT::WRAPPER_TYPE_NUMBER:
                cJSON_SetNumberToObject(dataItem, ptrColon, property->report.getPreviousValue());
                break;
            default:
                break;
        }

        if (property->report.isCalled()) {
            property->report.skipReport();
        }
    }

    this->thisApi().specificDataWrite(topic, root, true, true);
    cJSON_Delete(root);
    root = nullptr;
    dataItem = nullptr;
#endif
    // Update later
    ERA_FORCE_UNUSED(pProp);
}

template <class Api>
void ERaProperty<Api>::onCallbackProperty(const Property_t* const pProp, bool send) {
    if (pProp == nullptr) {
        return;
    }
    if (pProp->value == nullptr) {
        return;
    }

#if defined(ERA_ON_PUBLISH_PROPERTY)
    if (send) {
    }
    else if (pProp->onPublishCallback == nullptr) {
    }
    else if (!pProp->onPublishCallback()) {
        return;
    }
#else
    ERA_FORCE_UNUSED(send);
#endif

    if (pProp->id.isString()) {
        this->onCallbackReal(pProp);
    }
    else if (pProp->id.isNumber()) {
        this->onCallbackVirtual(pProp);
    }
}

template <class Api>
void ERaProperty<Api>::onCallback(void* args) {
    ERaProperty::Property_t* pProp = (ERaProperty::Property_t*)args;
    this->onCallbackProperty(pProp, false);
}

template <class Api>
bool ERaProperty<Api>::isPropertyFree() {
    if (this->numProperty >= MAX_PROPERTIES) {
        return false;
    }

    return true;
}

template <class Api>
int ERaProperty<Api>::findVirtualPin() {
    if (this->numProperty >= MAX_PROPERTIES) {
        return -1;
    }

    for (int i = 0; i < MAX_PROPERTIES; ++i) {
        if (this->isPropertyIdExist(i) != nullptr) {
            return i;
        }
    }
    return -1;
}

template <class Api>
template <typename T>
typename ERaProperty<Api>::Property_t* ERaProperty<Api>::isPropertyIdExist(T pin) {
    const PropertyIterator* e = this->ERaProp.end();
    for (PropertyIterator* it = this->ERaProp.begin(); it != e; it = it->getNext()) {
        Property_t* pProp = it->get();
        if (pProp->id == pin) {
            return pProp;
        }
    }
    return nullptr;
}

template <class Api>
size_t ERaProperty<Api>::splitString(char* strInput, const char* delims) {
    if ((strInput == nullptr) ||
        (delims == nullptr)) {
        return 0;
    }
    if (!strlen(strInput)) {
        return 0;
    }
    size_t size {0};
    char* token = strtok(strInput, delims);
    while (token != nullptr) {
        ++size;
        token = strtok(nullptr, delims);
    }
    return size;
}

template <class Api>
using PropertyEntry = typename ERaProperty<Api>::iterator;

#endif /* INC_ERA_PROPERTY_HPP_ */
