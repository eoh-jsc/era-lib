#ifndef INC_ERA_NIM_BLE_TRANSP_HPP_
#define INC_ERA_NIM_BLE_TRANSP_HPP_

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>
#include <BLE/ERaParse.hpp>
#include <ERa/ERaOTP.hpp>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaProtocol.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaQueue.hpp>
#include <Utility/ERaUtility.hpp>
#include <Encrypt/ERaEncryptMbedTLS.hpp>

#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TKN "cf153564-bc0d-11ed-afa1-0242ac120002"
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
    ERaBLETransp(bool _base64 = false)
        : ERaEncryptMbedTLS(_base64)
        , transpProp(ERaBluetooth::instance())
        , timeout(1000L)
        , _connected(false)
        , initialized(false)
        , topic(NULL)
        , callback(NULL)
    {
        ERa.setERaTransp(*this);
    }
    ~ERaBLETransp()
    {}

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

        pServer = NimBLEDevice::createServer();
        pServer->setCallbacks(this);

        pService = pServer->createService(SERVICE_UUID);

        pCharacteristicTKN = pService->createCharacteristic(
                                CHARACTERISTIC_UUID_TKN,
                                NIMBLE_PROPERTY::READ
                            );

        pCharacteristicTKN->setCallbacks(this);

        pCharacteristicRSP = pService->createCharacteristic(
                                CHARACTERISTIC_UUID_RSP,
                                NIMBLE_PROPERTY::NOTIFY
                            );

        pCharacteristicCMD = pService->createCharacteristic(
                                CHARACTERISTIC_UUID_CMD,
                                NIMBLE_PROPERTY::WRITE
                            );

        pCharacteristicCMD->setCallbacks(this);

        static NimBLESecurity* pSecurity = new NimBLESecurity();
        pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

        pService->start();

        pServer->getAdvertising()->addServiceUUID(pService->getUUID());
        pServer->getAdvertising()->setAppearance(0x00);
        pServer->getAdvertising()->setScanResponse(true);
        pServer->getAdvertising()->start();

        this->initialized = true;
    }

    void run() override {
        if (!this->connected()) {
            return;
        }
    }

    void setTopic(const char* _topic) override {
        this->topic = _topic;
    }

    void onMessage(MessageCallback_t cb) override {
        this->callback = cb;
    }

    int connect(IPAddress ip, uint16_t port) override {
        ERA_FORCE_UNUSED(ip);
        ERA_FORCE_UNUSED(port);
        this->rxBuffer.clear();
        this->_connected = true;
        return true;
    }

    int connect(const char* host, uint16_t port) override {
        ERA_FORCE_UNUSED(host);
        ERA_FORCE_UNUSED(port);
        this->rxBuffer.clear();
        this->_connected = true;
        return true;
    }

    void disconnect() {
        return this->stop();
    }

    size_t write(uint8_t value) override {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buf, size_t size) override {
        pCharacteristicRSP->setValue((uint8_t*)buf, size);
        pCharacteristicRSP->notify();
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
            if (this->available() < size) {
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

protected:
    static ERaBLETransp instance;

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
            ERaEncryptMbedTLS::decrypt(buf, size, data, dataLen);
            if (dataLen && (data != nullptr)) {
                this->progress((char*)data);
                ERA_LOG(TAG, ERA_PSTR("BLE data (%d): %s"), dataLen, data);
                free(data);
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
        if (ERaOTP::run(otp)) {
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
        item = cJSON_GetObjectItem(root, "command");
        if (!cJSON_IsString(item)) {
            return false;
        }

        bool status {false};
        char* payload = nullptr;
        char _topic[MAX_TOPIC_LENGTH] {0};
        FormatString(_topic, this->topic);
        // Now support only Modbus
        FormatString(_topic, "/down");

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
        payload = cJSON_PrintUnformatted(object);

        if (payload != nullptr) {
            status = true;
            this->callback(_topic, payload);
            free(payload);
        }

        cJSON_Delete(object);
        payload = nullptr;
        object = nullptr;
        return status;
    }

    ERaBluetooth*& transpProp;
    unsigned long timeout;
    bool _connected;
    bool initialized;
    const char* topic;
    MessageCallback_t callback;
    ERaQueue<uint8_t, ERA_MAX_READ_BYTES> rxBuffer;

    NimBLEServer* pServer;
    NimBLEService* pService;
    NimBLECharacteristic* pCharacteristicTKN;
    NimBLECharacteristic* pCharacteristicRSP;
    NimBLECharacteristic* pCharacteristicCMD;
};

#endif /* INC_ERA_NIM_BLE_TRANSP_HPP_ */
