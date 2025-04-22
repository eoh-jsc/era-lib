#ifndef INC_ERA_SYNCER_PIN_HPP_
#define INC_ERA_SYNCER_PIN_HPP_

#include <vector>
#include <ERa/ERaApiHandler.hpp>
#include <ERa/ERaProtocol.hpp>
#include <Syncer/ERaSyncerPinDef.hpp>

class ERaSyncerPin
    : public ERaComponent
{
    typedef struct __PinState_t {
        __PinState_t()
            : pin(0)
            , value(0.0f)
        {}
        __PinState_t(uint16_t p, float v)
            : pin(p)
            , value(v)
        {}

        uint16_t pin;
        float value;
    } ERA_ATTR_PACKED PinState_t;

protected:
    const char* TAG = "SyncerPin";

public:
    ERaSyncerPin(ERaApiHandler& api,
                 unsigned long syncInterval = ERA_SYNCER_INTERVAL)
        : mApi(api)
        , mKey(ERA_SYNCER_PIN_KEY)
        , mPrefixPin(ERA_SYNCER_PIN_PREFIX)
        , mPrefixDown(ERA_SYNCER_PIN_DOWN_TOPIC)
        , mWritten(0)
        , mLastSyncTime(0)
        , mSyncInterval(syncInterval)
    {
        this->mApi.addERaComponent(this);
    }
    virtual ~ERaSyncerPin()
    {}

    void setSyncInterval(unsigned long interval) {
        this->mSyncInterval = interval;
    }

    void addPins(uint16_t pin) {
        this->mPins.push_back(pin);
    }

    template <typename... Args> 
    void addPins(uint16_t pin, Args... args) {
        this->addPins(pin);
        this->addPins(args...);
    }

protected:
    void begin() override {
        this->load();
        if (this->mSyncInterval) {
            this->mApi.addInterval(this->mSyncInterval, std::bind(&ERaSyncerPin::syncWithLog, this));
        }
    }

    void run() override {
        if (this->mSyncInterval) {
            return;
        }
        this->syncWithLog();
    }

    void message(const ERaDataBuff& topic, const char* payload) override {
        if (topic.size() != 3) {
            return;
        }
        if (topic.at(1) != this->mPrefixPin) {
            return;
        }

        const char* str = topic.at(2).getString();
        if (str == nullptr) {
            return;
        }
        uint16_t pin = ERA_DECODE_PIN_NAME(str);
        ERA_CHECK_PIN_RETURN(pin);
        if (!this->pinExists(pin)) {
            return;
        }

        cJSON* root = cJSON_Parse(payload);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            return;
        }
        cJSON* item = cJSON_GetObjectItem(root, "value");
        if (cJSON_IsNumber(item) ||
            cJSON_IsBool(item)) {
            this->save(pin, item->valuedouble);
        }
        cJSON_Delete(root);
        root = nullptr;
        item = nullptr;
    }

    void save(uint16_t pin, float value) {
        if (!this->pinExists(pin)) {
            return;
        }

        for (auto& state : this->mPinStates) {
            if (state.pin != pin) {
                continue;
            }
            state.value = value;
            return;
        }
        this->mPinStates.emplace_back(pin, value);
    }

    void load() {
        size_t size = this->mApi.getBytesSizeFromFlash(this->mKey);
        if (!size) {
            return;
        }
        if ((size % sizeof(PinState_t)) != 0) {
            return;
        }

        uint8_t* data = new uint8_t[size];
        this->mApi.readBytesFromFlash(this->mKey, data, size);
        for (size_t i = 0; i < size; i += sizeof(PinState_t)) {
            PinState_t* state = (PinState_t*)(data + i);
            this->save(state->pin, state->value);
            this->call(state);
        }
        delete[] data;
        data = nullptr;
    }

    void sync() {
        if (this->mPinStates.empty()) {
            return;
        }
        if (!this->isChanged()) {
            return;
        }
        this->mApi.writeBytesToFlash(this->mKey, this->mPinStates.data(),
                                     this->mPinStates.size() * sizeof(PinState_t));
        this->mWritten++;
    }

    void syncWithLog() {
        this->sync();
        ERA_LOG(TAG, ERA_PSTR("Synced: %d written"), this->mWritten);
    }

    bool isChanged() {
        size_t sizeSaved = this->mApi.getBytesSizeFromFlash(this->mKey);
        size_t sizeToSave = this->mPinStates.size() * sizeof(PinState_t);
        if (sizeSaved != sizeToSave) {
            return true;
        }

        bool changed {false};
        uint8_t* dataSaved = new uint8_t[sizeSaved];
        this->mApi.readBytesFromFlash(this->mKey, dataSaved, sizeSaved);
        for (size_t i = 0, j = 0; i < sizeSaved; i += sizeof(PinState_t), ++j) {
            PinState_t* stateSaved = (PinState_t*)(dataSaved + i);
            PinState_t* stateToSave = (PinState_t*)(this->mPinStates.data() + j);
            if ((stateSaved->pin != stateToSave->pin) ||
                (!ERaFloatCompare(stateSaved->value, stateToSave->value))) {
                changed = true;
                break;
            }
        }
        delete[] dataSaved;
        dataSaved = nullptr;
        return changed;
    }

    void call(const PinState_t* state) {
        if (!this->pinExists(state->pin)) {
            return;
        }
        if (this->mBaseTopic == nullptr) {
            return;
        }
        if (this->mCallback == nullptr) {
            return;
        }
        char topic[MAX_TOPIC_LENGTH] {0};
        FormatString(topic, this->mBaseTopic);
        FormatString(topic, this->mPrefixDown, state->pin);
        cJSON* object = cJSON_CreateObject();
        if (object == nullptr) {
            return;
        }
        cJSON_AddNumberWithDecimalToObject(object, "value", state->value, 2);
        char* payload = cJSON_PrintUnformatted(object);
        if (payload != nullptr) {
            this->mCallback(topic, payload);
            free(payload);
        }
        cJSON_Delete(object);
        object = nullptr;
        payload = nullptr;
    }

    bool pinExists(uint16_t pin) {
        if (this->mPins.empty()) {
            return true;
        }
        for (const auto& p : this->mPins) {
            if (p == pin) {
                return true;
            }
        }
        return false;
    }

    ERaApiHandler& mApi;
    const char* mKey;
    const char* mPrefixPin;
    const char* mPrefixDown;
    std::vector<PinState_t> mPinStates;
    std::vector<uint16_t> mPins;

    unsigned long mWritten;

    unsigned long mLastSyncTime;
    unsigned long mSyncInterval;
};

class ERaSyncerVirtual
    : public ERaSyncerPin
{
public:
    ERaSyncerVirtual(ERaApiHandler& api, unsigned long syncInterval)
        : ERaSyncerPin(api, syncInterval)
    {
        this->TAG = "SyncerVir";
        this->mKey = ERA_SYNCER_VIRTUAL_KEY;
        this->mPrefixPin = ERA_SYNCER_VIRTUAL_PREFIX;
        this->mPrefixDown = ERA_SYNCER_VIRTUAL_DOWN_TOPIC;
    }
};

#endif /* INC_ERA_SYNCER_PIN_HPP_ */
