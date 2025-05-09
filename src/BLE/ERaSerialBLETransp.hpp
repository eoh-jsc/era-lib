#ifndef INC_ERA_SERIAL_BLE_TRANSP_HPP_
#define INC_ERA_SERIAL_BLE_TRANSP_HPP_

#include <BLE/ERaBLEConfig.hpp>
#include <BLE/ERaParse.hpp>
#include <ERa/ERaOTP.hpp>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaProtocol.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaQueue.hpp>
#include <Utility/ERaUtility.hpp>

class ERaBLETransp
    : public ERaTransp
{
    const char* TAG = "BLETransp";

public:
    ERaBLETransp(ERaCallbackSetter& setter,
                Stream& stream)
        : mStream(stream)
        , mTranspProp(ERaBluetooth::instance())
        , mApi(NULL)
        , mTimeout(1000L)
        , mConnected(false)
        , mInitialized(false)
#if defined(ERA_HAS_RTOS)
        , mBleTask(NULL)
#endif
    {
        setter.setERaTransp(this);
        ERaBLETransp::instance() = this;
    }
    ~ERaBLETransp()
    {}

    void setAPI(ERaApiHandler* api) {
        this->mApi = api;
    }

    void setAPI(ERaApiHandler& api) {
        this->mApi = &api;
    }

    void setTranspProperty(void* args) {
        if (args == NULL) {
            return;
        }
        char* ptr = (char*)args;
        this->mTranspProp->getInstance();
        this->mTranspProp->parseConfig(ptr);
    }

    void begin(void* args = NULL) override {
        if (args == NULL) {
            return;
        }
        this->setTranspProperty(args);
        if (!strlen(this->mTranspProp->address)) {
            return;
        }

        this->onConfig();

        if (this->mInitialized) {
            return;
        }
#if defined(ERA_HAS_RTOS) && !defined(ERA_NO_RTOS)
        #if !defined(ERA_MCU_CORE)
            #define ERA_MCU_CORE            0
        #endif
        #if !defined(ERA_BLE_TASK_SIZE)
            #define ERA_BLE_TASK_SIZE       (1024 * 5)
        #endif
        #if !defined(ERA_BLE_TASK_PRIORITY)
            #define ERA_BLE_TASK_PRIORITY   (configMAX_PRIORITIES - 4)
        #endif
        this->mBleTask = ERaOs::osThreadNew(this->bleTask, "bleTask", ERA_BLE_TASK_SIZE,
                                            this, ERA_BLE_TASK_PRIORITY, ERA_MCU_CORE);
#endif
        this->mConnected = true;
        this->mInitialized = true;
    }

    void run() override {
        if (!this->connected()) {
            return;
        }

#if !defined(ERA_HAS_RTOS) || defined(ERA_NO_RTOS)
        this->progressData();
#endif
    }

    int connect(IPAddress ip, uint16_t port) override {
        ERA_FORCE_UNUSED(ip);
        ERA_FORCE_UNUSED(port);
        this->mConnected = true;
        return 1;
    }

    int connect(const char* host, uint16_t port) override {
        ERA_FORCE_UNUSED(host);
        ERA_FORCE_UNUSED(port);
        this->mConnected = true;
        return 1;
    }

    void disconnect() {
        return this->stop();
    }

    size_t write(uint8_t value) override {
        return this->mStream.write(value);
    }

    size_t write(const uint8_t* buf, size_t size) override {
        return this->mStream.write(buf, size);
    }

    size_t write(const char* buf) {
        return this->write((const uint8_t*)buf, strlen(buf));
    }

    int available() override {
        return this->mStream.available();
    }

    int read() override {
        return this->mStream.read();
    }

    int read(uint8_t* buf, size_t size) override {
        uint8_t* begin = buf;
        uint8_t* end = buf + size;
        MillisTime_t startMillis = ERaMillis();
        while ((begin < end) && ((ERaMillis() - startMillis) < this->mTimeout)) {
            int c = this->mStream.read();
            if (c < 0) {
                continue;
            }
            *begin++ = (uint8_t)c;
        }
        return ((int)(begin - buf));
    }

    int timedRead() {
        uint8_t c {0};
        if (this->read(&c, 1) <= 0) {
            return -1;
        }
        return (int)c;
    }

    size_t readBytesUntil(uint8_t* buf, size_t size, char terminator) {
        if (buf == nullptr) {
            return 0;
        }
        if (!size) {
            return 0;
        }
        size_t index {0};
        while (index < size) {
            int c = this->timedRead();
            if ((c < 0) || (c == terminator)) {
                buf[index] = 0;
                break;
            }
            *buf++ = (uint8_t)c;
            index++;
        }
        return index;
    }

    size_t readBytesUntil(char* buf, size_t size, char terminator) {
        return this->readBytesUntil((uint8_t*)buf, size, terminator);
    }

    int peek() override {
        return this->mStream.peek();
    }

    void flush() override {
        this->mStream.flush();
    }

    void stop() override {
        this->mConnected = false;
    }

    uint8_t connected() override {
        return this->mConnected;
    }

    operator bool() override {
        return this->mConnected;
    }

    static ERaBLETransp* getInstance() {
        return ERaBLETransp::instance();
    }

protected:
#if defined(ERA_HAS_RTOS)
    static void bleTask(void* args) {
        if (args == NULL) {
            ERaOs::osThreadDelete(NULL);
        }
        ERaBLETransp* ble = (ERaBLETransp*)args;
        for (;;) {
            ERA_BLE_YIELD();
            ble->progressData();
        }
        ERaOs::osThreadDelete(NULL);
    }
#endif

    void progressData() {
        if (!this->connected()) {
            return;
        }

        size_t len = this->available();
        if (!len) {
            return;
        }

        size_t size {0};
        uint8_t locBuf[256] {0};
        uint8_t* buf = locBuf;

        do {
            size += len;
            if (size >= sizeof(locBuf)) {
                if (buf == locBuf) {
                    buf = (uint8_t*)ERA_MALLOC(size + 1);
                    memcpy(buf, locBuf, size);
                }
                else {
                    uint8_t* copy = (uint8_t*)ERA_REALLOC(buf, size + 1);
                    if (copy == nullptr) {
                        free(buf);
                        buf = nullptr;
                        return;
                    }
                    buf = copy;
                }
                if (buf == nullptr) {
                    return;
                }
                buf[size] = 0;
            }
            this->read(buf + size - len, len);
            MillisTime_t startMillis = ERaMillis();
            do {
                ERaDelay(1);
                len = this->available();
            } while(!len && ((ERaMillis() - startMillis) < ERA_BLE_YIELD_MS));
        } while (len);

        this->progress((char*)buf);

        if (buf != locBuf) {
            free(buf);
        }
        buf = nullptr;
    }

    static ERaBLETransp*& instance() {
        static ERaBLETransp* _instance = nullptr;
        return _instance;
    }

private:
    void onConfig() {
        cJSON* root = cJSON_CreateObject();
        if (root == nullptr) {
            return;
        }

        char secretKey[17] {0};
        memcpy(secretKey, this->mTranspProp->secretKey, sizeof(this->mTranspProp->secretKey));

        cJSON_AddStringToObject(root, "type", "config");
        cJSON_AddStringToObject(root, "user_name", this->mTranspProp->address);
        cJSON_AddStringToObject(root, "password", this->mTranspProp->password);
        cJSON_AddStringToObject(root, "secret_key", secretKey);

        char* config = cJSON_PrintUnformatted(root);

        if (config != nullptr) {
            this->write(config);
        }

        cJSON_Delete(root);
        free(config);
        root = nullptr;
        config = nullptr;
    }

    void progress(const char* message) {
        cJSON* root = cJSON_Parse(message);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            this->response("failed");
            return;
        }

        if (this->onCallback(root)) {
            this->response("ok");
        }
        else {
            this->response("failed");
        }

        cJSON_Delete(root);
        root = nullptr;
    }

    void response(const char* status) {
        cJSON* root = cJSON_CreateObject();
        if (root == nullptr) {
            return;
        }

        cJSON_AddStringToObject(root, "type", "command");
        cJSON_AddStringToObject(root, "status", status);

        char* rsp = cJSON_PrintUnformatted(root);

        if (rsp != nullptr) {
            this->write(rsp);
        }

        cJSON_Delete(root);
        free(rsp);
        root = nullptr;
        rsp = nullptr;
    }

    bool onCallback(cJSON* const root) {
        if ((this->baseTopic == nullptr) ||
            (this->callback == nullptr)) {
            return false;
        }

        unsigned int userID {0};

        cJSON* item = cJSON_GetObjectItem(root, "type");
        if (!cJSON_IsString(item)) {
            return false;
        }
        if (!ERaStrCmp(item->valuestring, "command")) {
            return false;
        }
        item = cJSON_GetObjectItem(root, "user");
        if (cJSON_IsNumber(item)) {
            userID = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "alias");
        if (!cJSON_IsString(item)) {
            return false;
        }
        if (strlen(item->valuestring) != 36) {
            return false;
        }

        bool status {false};
        char* payload = nullptr;
        char dTopic[MAX_TOPIC_LENGTH] {0};
        FormatString(dTopic, this->baseTopic);
        // Now support only Modbus
        FormatString(dTopic, ERA_PUB_PREFIX_DOWN_TOPIC);

        cJSON* object = cJSON_CreateObject();
        if (object == nullptr) {
            return false;
        }
        cJSON* subObject = cJSON_CreateObject();
        if (subObject == nullptr) {
            cJSON_Delete(object);
            object = nullptr;
            return false;
        }
        cJSON_AddStringToObject(object, "action", "send_command");
        cJSON_AddItemToObject(object, "data", subObject);
        cJSON* array = cJSON_CreateArray();
        if (array == nullptr) {
            cJSON_Delete(object);
            object = nullptr;
            return false;
        }

        cJSON_AddItemToArray(array, cJSON_CreateString(item->valuestring));
        cJSON_AddItemToObject(subObject, "commands", array);

        cJSON* paramItem = cJSON_GetObjectItem(root, "value");
        if (paramItem != nullptr) {
            cJSON_AddItemToObject(subObject, "value",
                    cJSON_Duplicate(paramItem, true));
        }

        payload = cJSON_PrintUnformatted(object);

        if (payload != nullptr) {
            status = true;
            this->callback(dTopic, payload);
            free(payload);
        }

        /* Publish action log */
        this->publishActionLog(userID, item->valuestring);

        cJSON_Delete(object);
        payload = nullptr;
        object = nullptr;
        return status;
    }

    void publishActionLog(unsigned int userID, const char* alias) {
        if (!userID) {
            return;
        }
        if (alias == nullptr) {
            return;
        }
        if (this->mApi == nullptr) {
            return;
        }

        char message[256] {0};
        FormatString(message, MESSAGE_BLE_ACTION_LOG, userID, alias);
#if defined(ERA_SPECIFIC)
        this->mApi->specificDataWrite(TOPIC_BLE_ACTION_LOG, message, true, false);
#endif
    }

    Stream& mStream;
    ERaBluetooth*& mTranspProp;
    ERaApiHandler* mApi;
    unsigned long mTimeout;
    bool mConnected;
    bool mInitialized;
#if defined(ERA_HAS_RTOS)
    TaskHandle_t mBleTask;
#endif
};

using ERaBLEPeripheral = ERaBLETransp;

#endif /* INC_ERA_SERIAL_BLE_TRANSP_HPP_ */
