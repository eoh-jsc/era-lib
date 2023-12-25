#ifndef INC_ERA_NIM_BLE_TRANSP_HPP_
#define INC_ERA_NIM_BLE_TRANSP_HPP_

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <BLE/ERaBLEConfig.hpp>
#include <BLE/ERaParse.hpp>
#include <ERa/ERaOTP.hpp>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaProtocol.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaQueue.hpp>
#include <Utility/ERaUtility.hpp>
#include <Encrypt/ERaEncryptMbedTLS.hpp>

#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TKN "cf153c1f-bc0d-11ed-afa1-0242ac120002"
#define CHARACTERISTIC_UUID_RSP "fba737c2-9f19-4779-9ef2-9446c29d0bf5"
#define CHARACTERISTIC_UUID_CMD "b32f3620-2846-4d1c-88aa-5dd06c0ad15e"

class ERaBLETransp
    : public ERaTransp
    , public ERaOTP
    , public ERaEncryptMbedTLS
    , public NimBLEServerCallbacks
    , public NimBLECharacteristicCallbacks
{
    const char* TAG = "BLETransp";

public:
    ERaBLETransp(ERaCallbacksHelper& helper,
                bool _base64 = false,
                bool _encrypt = true)
        : ERaEncryptMbedTLS(_base64)
        , transpProp(ERaBluetooth::instance())
        , api(NULL)
        , timeout(1000L)
        , _connected(false)
        , initialized(false)
        , useEncrypt(_encrypt)
        , pServer(NULL)
        , pService(NULL)
        , pCharacteristicTKN(NULL)
        , pCharacteristicRSP(NULL)
        , pCharacteristicCMD(NULL)
    {
        helper.setERaTransp(this);
        ERaBLETransp::instance() = this;
    }
    ~ERaBLETransp()
    {}

    void setAPI(ERaApiHandler* api) {
        this->api = api;
    }

    void setAPI(ERaApiHandler& api) {
        this->api = &api;
    }

    void setEncrypt(bool _encrypt = true) {
        this->useEncrypt = _encrypt;
    }

    void setTranspProperty(void* args) {
        if (args == NULL) {
            return;
        }
        char* ptr = (char*)args;
        this->transpProp->getInstance();
        this->transpProp->parseConfig(ptr);
    }

    void begin(void* args = NULL) override {
        if (args == NULL) {
            return;
        }
        this->setTranspProperty(args);
        if (!strlen(this->transpProp->address)) {
            return;
        }

        ERaEncryptMbedTLS::begin(this->transpProp->secretKey);

        if (this->initialized) {
            NimBLEDevice::setDeviceName(this->transpProp->address);
            return;
        }

        if (NimBLEDevice::getInitialized()) {
            NimBLEDevice::deinit();
        }
        NimBLEDevice::init(this->transpProp->address);

        this->pServer = NimBLEDevice::createServer();
        this->pServer->setCallbacks(this);

        this->pService = this->pServer->createService(SERVICE_UUID);

        this->pCharacteristicTKN = this->pService->createCharacteristic(
                                CHARACTERISTIC_UUID_TKN,
                                NIMBLE_PROPERTY::READ
                            );

        this->pCharacteristicTKN->setCallbacks(this);

        this->pCharacteristicRSP = this->pService->createCharacteristic(
                                CHARACTERISTIC_UUID_RSP,
                                NIMBLE_PROPERTY::NOTIFY
                            );

        this->pCharacteristicCMD = this->pService->createCharacteristic(
                                CHARACTERISTIC_UUID_CMD,
                                NIMBLE_PROPERTY::WRITE
                            );

        this->pCharacteristicCMD->setCallbacks(this);

        static NimBLESecurity* pSecurity = new NimBLESecurity();
        pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

        this->pService->start();

        this->pServer->getAdvertising()->addServiceUUID(this->pService->getUUID());
        this->pServer->getAdvertising()->setAppearance(0x00);
        this->pServer->getAdvertising()->setScanResponse(true);
        this->pServer->getAdvertising()->start();

        this->initialized = true;
    }

    void run() override {
        if (!this->connected()) {
            return;
        }
    }

    int connect(IPAddress ip, uint16_t port) override {
        ERA_FORCE_UNUSED(ip);
        ERA_FORCE_UNUSED(port);
        this->rxBuffer.clear();
        this->_connected = true;
        return 1;
    }

    int connect(const char* host, uint16_t port) override {
        ERA_FORCE_UNUSED(host);
        ERA_FORCE_UNUSED(port);
        this->rxBuffer.clear();
        this->_connected = true;
        return 1;
    }

    void disconnect() {
        return this->stop();
    }

    size_t write(uint8_t value) override {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buf, size_t size) override {
        if (this->pCharacteristicRSP == NULL) {
            return 0;
        }
        this->pCharacteristicRSP->setValue((uint8_t*)buf, size);
        this->pCharacteristicRSP->notify();
        return size;
    }

    size_t write(const char* buf) {
        return this->write((const uint8_t*)buf, strlen(buf));
    }

    int available() override {
        return this->rxBuffer.size();
    }

    int read() override {
        if (this->rxBuffer.isEmpty()) {
            return -1;
        }
        return this->rxBuffer.get();
    }

    int read(uint8_t* buf, size_t size) override {
        MillisTime_t startMillis = ERaMillis();
        while (ERaMillis() - startMillis < this->timeout) {
            if (this->available() < (int)size) {
                ERaDelay(1);
            }
            else {
                break;
            }
        }
        return this->rxBuffer.get(buf, size);
    }

    int peek() override {
        if (this->rxBuffer.isEmpty()) {
            return -1;
        }
        return this->rxBuffer.peek();
    }

    void flush() override {
        this->rxBuffer.clear();
    }

    void stop() override {
        this->_connected = false;
    }

    uint8_t connected() override {
        return this->_connected;
    }

    operator bool() override {
        return this->_connected;
    }

    static ERaBLETransp* getInstance() {
        return ERaBLETransp::instance();
    }

protected:
    static ERaBLETransp*& instance() {
        static ERaBLETransp* _instance = nullptr;
        return _instance;
    }

private:
    void onConnect(NimBLEServer* pServer) override {
        ERA_LOG(TAG, ERA_PSTR("BLE id %d connect"), pServer->getPeerDevices().back());
#if defined(ERA_BT_MULTI_CONNECT)
        pServer->startAdvertising();
#endif
        this->_connected = true;
    }

    void onDisconnect(NimBLEServer* pServer) override {
        if (pServer->getConnectedCount()) {
            ERA_LOG(TAG, ERA_PSTR("BLE disconnect"));
            return;
        }
        ERA_LOG(TAG, ERA_PSTR("BLE disconnect all"));
        pServer->getAdvertising()->start();
        this->_connected = false;
    }

    void onRead(NimBLECharacteristic* pCharacteristic) override {
        int otp = ERaOTP::createOTP();
        if (otp >= 0) {
            pCharacteristic->setValue(otp);
        }
    }

    void onWrite(NimBLECharacteristic* pCharacteristic) override {
        size_t size = pCharacteristic->getDataLength();
        const uint8_t* rxValue = pCharacteristic->getValue();

        uint8_t locBuf[256] {0};
        uint8_t* buf = locBuf;
        if (size >= sizeof(locBuf)) {
            buf = (uint8_t*)ERA_MALLOC(size + 1);
            if (buf == nullptr) {
                return;
            }
        }
        memcpy(buf, rxValue, size);
        buf[size] = 0;

        if (size) {
            size_t dataLen {0};
            uint8_t* data = nullptr;
            if (this->useEncrypt) {
                ERaEncryptMbedTLS::decrypt(buf, size, data, dataLen);
            }
            else {
                data = buf;
                dataLen = size;
            }
            if (dataLen && (data != nullptr)) {
                this->progress((char*)data);
                ERA_LOG(TAG, ERA_PSTR("BLE data (%d): %s"), dataLen, data);
                if (data != buf) {
                    free(data);
                }
                data = nullptr;
            }
            else {
                this->response("FAILED");
            }
            rxBuffer.put(buf, size);
        }

        if (buf != locBuf) {
            free(buf);
        }
        buf = nullptr;
    }

    void progress(const char* message) {
        cJSON* root = cJSON_Parse(message);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            this->response("FAILED");
            return;
        }

        int otp {0};
        bool status {false};
        cJSON* item = cJSON_GetObjectItem(root, "token");
        if (cJSON_IsNumber(item)) {
            otp = item->valueint;
        }
        if (ERaOTP::run(otp) ||
            !this->useEncrypt) {
            status = this->onCallback(root);
        }

        if (status) {
            this->response("OK");
        }
        else {
            this->response("FAILED");
        }

        cJSON_Delete(root);
        root = nullptr;
    }

    void response(const char* status) {
        this->write(status);
    }

    bool onCallback(cJSON* const root) {
        if ((this->topic == nullptr) ||
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
        item = cJSON_GetObjectItem(root, "password");
        if (!cJSON_IsString(item)) {
            return false;
        }
        if (!ERaStrCmp(item->valuestring, this->transpProp->password)) {
            return false;
        }
        item = cJSON_GetObjectItem(root, "user");
        if (cJSON_IsNumber(item)) {
            userID = item->valueint;
        }
        item = cJSON_GetObjectItem(root, "command");
        if (!cJSON_IsString(item)) {
            return false;
        }
        if (strlen(item->valuestring) != 36) {
            return false;
        }

        bool status {false};
        char* payload = nullptr;
        char _topic[MAX_TOPIC_LENGTH] {0};
        FormatString(_topic, this->topic);
        // Now support only Modbus
        FormatString(_topic, ERA_PUB_PREFIX_DOWN_TOPIC);

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
            this->callback(_topic, payload);
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
        if (this->api == nullptr) {
            return;
        }

        char message[256] {0};
        FormatString(message, MESSAGE_BLE_ACTION_LOG, userID, alias);
#if defined(ERA_SPECIFIC)
        this->api->specificDataWrite(TOPIC_BLE_ACTION_LOG, message, true, false);
#endif
    }

    ERaBluetooth*& transpProp;
    ERaApiHandler* api;
    unsigned long timeout;
    bool _connected;
    bool initialized;
    bool useEncrypt;
    ERaQueue<uint8_t, ERA_MAX_READ_BYTES> rxBuffer;

    NimBLEServer* pServer;
    NimBLEService* pService;
    NimBLECharacteristic* pCharacteristicTKN;
    NimBLECharacteristic* pCharacteristicRSP;
    NimBLECharacteristic* pCharacteristicCMD;
};

#endif /* INC_ERA_NIM_BLE_TRANSP_HPP_ */
