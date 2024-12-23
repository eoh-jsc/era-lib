#ifndef INC_ERA_BLE_TRANSP_ESP32_HPP_
#define INC_ERA_BLE_TRANSP_ESP32_HPP_

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLE/ERaBLEConfig.hpp>
#include <BLE/ERaBLEVerify.hpp>
#include <BLE/ERaParse.hpp>
#include <ERa/ERaOTP.hpp>
#include <ERa/ERaTransp.hpp>
#include <ERa/ERaProtocol.hpp>
#include <Utility/MD5.hpp>
#include <Utility/ERacJSON.hpp>
#include <Utility/ERaQueue.hpp>
#include <Utility/ERaUtility.hpp>
#include <Encrypt/ERaEncryptMbedTLS.hpp>

#define SERVICE_UUID            "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TKN "cf153c1f-bc0d-11ed-afa1-0242ac120002"
#define CHARACTERISTIC_UUID_RSP "fba737c2-9f19-4779-9ef2-9446c29d0bf5"
#define CHARACTERISTIC_UUID_CMD "b32f3620-2846-4d1c-88aa-5dd06c0ad15e"

#define CHARACTERISTIC_UUID_AUT "6ee26cfb-b42f-43ae-9ecf-f70d84bbd64f"
#define CHARACTERISTIC_UUID_CTR "f0e7aa38-e466-42af-b4c7-5e6ec5c9a614"

class ERaBLETransp
    : public ERaTransp
    , public ERaOTP
    , public ERaBLEVerify
    , public ERaEncryptMbedTLS
    , public BLEServerCallbacks
    , public BLECharacteristicCallbacks
{
    const char* TAG = "BLETransp";

public:
    ERaBLETransp(ERaCallbackSetter& setter,
                bool base64 = false,
                bool encrypt = true)
        : ERaEncryptMbedTLS(base64)
        , mTranspProp(ERaBluetooth::instance())
        , mApi(NULL)
        , mTimeout(1000L)
        , mConnected(false)
        , mInitialized(false)
        , mUseEncrypt(encrypt)
        , mUserDeviceName(NULL)
        , mServer(NULL)
        , mService(NULL)
        , mCharacteristicTKN(NULL)
        , mCharacteristicRSP(NULL)
        , mCharacteristicCMD(NULL)
        , mNewService(NULL)
        , mCharacteristicAUT(NULL)
        , mCharacteristicCTR(NULL)
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

    void setEncrypt(bool encrypt = true) {
        this->mUseEncrypt = encrypt;
    }

    void setTranspProperty(void* args) {
        if (args == NULL) {
            return;
        }
        char* ptr = (char*)args;
        this->mTranspProp->getInstance();
        this->mTranspProp->parseConfig(ptr);
    }

    void begin(const char* address, const char* password, const char* name = NULL) {
        this->mTranspProp->getInstance();
        this->mTranspProp->setAddress(address);
        this->mTranspProp->setPassword(password);
        this->mUserDeviceName = name;
        this->begin((void*)"");
    }

    void begin(void* args = NULL) override {
        if (args == NULL) {
            return;
        }
        this->setTranspProperty(args);
        if (!strlen(this->mTranspProp->address)) {
            return;
        }

        ERaEncryptMbedTLS::begin(this->mTranspProp->secretKey);

        if (this->mInitialized) {
            esp_ble_gap_set_device_name(this->getDeviceName());
            return;
        }

        if (!BLEDevice::getInitialized()) {
            /* BLEDevice::deinit(); */
            BLEDevice::init(this->getDeviceName());
        }

        this->mServer = BLEDevice::createServer();
        this->mServer->setCallbacks(this);

        this->mService = this->mServer->createService(SERVICE_UUID);

        this->mCharacteristicTKN = this->mService->createCharacteristic(
                                CHARACTERISTIC_UUID_TKN,
                                BLECharacteristic::PROPERTY_READ
                            );

        this->mCharacteristicTKN->setCallbacks(this);

        this->mCharacteristicRSP = this->mService->createCharacteristic(
                                CHARACTERISTIC_UUID_RSP,
                                BLECharacteristic::PROPERTY_NOTIFY
                            );

        this->mCharacteristicRSP->addDescriptor(new BLE2902());

        this->mCharacteristicCMD = this->mService->createCharacteristic(
                                CHARACTERISTIC_UUID_CMD,
                                BLECharacteristic::PROPERTY_WRITE
                            );

        this->mCharacteristicCMD->setCallbacks(this);

        if (this->isNewService()) {
            this->mNewService = this->mServer->createService(this->mTranspProp->address);

            this->mCharacteristicAUT = this->mNewService->createCharacteristic(
                                    CHARACTERISTIC_UUID_AUT,
                                    BLECharacteristic::PROPERTY_READ |
                                    BLECharacteristic::PROPERTY_WRITE |
                                    BLECharacteristic::PROPERTY_WRITE_NR
                                );

            this->mCharacteristicAUT->setCallbacks(this);

            this->mCharacteristicCTR = this->mNewService->createCharacteristic(
                                    CHARACTERISTIC_UUID_CTR,
                                    BLECharacteristic::PROPERTY_WRITE
                                );

            this->mCharacteristicCTR->setCallbacks(this);
        }

        static BLESecurity* pSecurity = new BLESecurity();
        pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);

        if (this->isNewService()) {
            this->mNewService->start();
            this->mServer->getAdvertising()->addServiceUUID(this->mNewService->getUUID());
        }

        this->mService->start();
        this->mServer->getAdvertising()->addServiceUUID(this->mService->getUUID());

        this->mServer->getAdvertising()->setAppearance(0x00);
        this->mServer->getAdvertising()->setScanResponse(true);
        this->mServer->getAdvertising()->start();

        this->mInitialized = true;
    }

    void run() override {
        if (!this->connected()) {
            return;
        }

        if (!ERaBLEVerify::hasVerify()) {
            return;
        }
        ERaInt_t connID = ERaBLEVerify::connTimeout();
        if (connID >= 0) {
            this->mServer->disconnect(connID);
        }
    }

    int connect(IPAddress ip, uint16_t port) override {
        ERA_FORCE_UNUSED(ip);
        ERA_FORCE_UNUSED(port);
        this->mRxBuffer.clear();
        this->mConnected = true;
        return 1;
    }

    int connect(const char* host, uint16_t port) override {
        ERA_FORCE_UNUSED(host);
        ERA_FORCE_UNUSED(port);
        this->mRxBuffer.clear();
        this->mConnected = true;
        return 1;
    }

    void disconnect() {
        return this->stop();
    }

    size_t write(uint8_t value) override {
        return this->write(&value, 1);
    }

    size_t write(const uint8_t* buf, size_t size) override {
        if (this->mCharacteristicRSP == NULL) {
            return 0;
        }
        this->mCharacteristicRSP->setValue((uint8_t*)buf, size);
        this->mCharacteristicRSP->notify();
        return size;
    }

    size_t write(const char* buf) {
        return this->write((const uint8_t*)buf, strlen(buf));
    }

    int available() override {
        return this->mRxBuffer.size();
    }

    int read() override {
        if (this->mRxBuffer.isEmpty()) {
            return -1;
        }
        return this->mRxBuffer.get();
    }

    int read(uint8_t* buf, size_t size) override {
        MillisTime_t startMillis = ERaMillis();
        while (ERaMillis() - startMillis < this->mTimeout) {
            if (this->available() < (int)size) {
                ERaDelay(1);
            }
            else {
                break;
            }
        }
        return this->mRxBuffer.get(buf, size);
    }

    int peek() override {
        if (this->mRxBuffer.isEmpty()) {
            return -1;
        }
        return this->mRxBuffer.peek();
    }

    void flush() override {
        this->mRxBuffer.clear();
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
    static ERaBLETransp*& instance() {
        static ERaBLETransp* _instance = nullptr;
        return _instance;
    }

private:
    size_t write(BLECharacteristic* pCharacteristic, const char* buf) {
        pCharacteristic->setValue((uint8_t*)buf, strlen(buf));
        return strlen(buf);
    }

    void onConnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) override {
        ERA_LOG(TAG, ERA_PSTR("BLE id %d connect"), param->connect.conn_id);
        if (ERaBLEVerify::getVerifyCode(param->connect.conn_id, false) < 0) {
            pServer->disconnect(param->connect.conn_id);
        }
#if defined(ERA_BT_MULTI_CONNECT)
        pServer->getAdvertising()->start();
#endif
        this->mConnected = true;
    }

    void onDisconnect(BLEServer* pServer, esp_ble_gatts_cb_param_t* param) override {
        ERaBLEVerify::removeVerify(param->disconnect.conn_id);
        if (pServer->getConnectedCount() > 1) {
            ERA_LOG(TAG, ERA_PSTR("BLE id %d disconnect"), param->disconnect.conn_id);
            return;
        }
        ERA_LOG(TAG, ERA_PSTR("BLE disconnect all"));
        pServer->getAdvertising()->start();
        this->mConnected = false;
    }

    void onRead(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) override {
        if (!pCharacteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_UUID_TKN))) {
            return;
        }

        int otp = ERaOTP::createOTP();
        if (otp >= 0) {
            pCharacteristic->setValue(otp);
        }
        else {
            this->mServer->disconnect(param->read.conn_id);
        }
    }

    void onWrite(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) override {
        size_t size = pCharacteristic->getLength();
        const uint8_t* rxValue = pCharacteristic->getData();

        if (size) {
            size_t dataLen {0};
            uint8_t* data = nullptr;
            if (this->mUseEncrypt) {
                ERaEncryptMbedTLS::decrypt(rxValue, size, data, dataLen);
            }
            else {
                data = (uint8_t*)rxValue;
                dataLen = size;
            }
            if (dataLen && (data != nullptr)) {
                if (pCharacteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_UUID_CMD))) {
                    this->progress((char*)data);
                }
                else {
                    this->progress((char*)data, pCharacteristic, param);
                }
                ERA_LOG(TAG, ERA_PSTR("BLE data (%d): %s"), dataLen, data);
                if (data != rxValue) {
                    free(data);
                }
                data = nullptr;
            }
            else {
                this->response("FAILED");
            }
            this->mRxBuffer.put(rxValue, size);
        }
    }

    void progress(const char* message) {
        cJSON* root = cJSON_Parse(message);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            this->response("FAILED");
            return;
        }

        ERaInt_t otp {0};
        bool status {false};
        cJSON* item = cJSON_GetObjectItem(root, "token");
        if (cJSON_IsNumber(item)) {
            otp = item->valueint;
        }
        if (ERaOTP::run(otp) ||
            !this->mUseEncrypt) {
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

    void progress(const char* message, BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
        if (pCharacteristic->getUUID().equals(BLEUUID(CHARACTERISTIC_UUID_AUT))) {
            this->progressAuth(message, pCharacteristic, param);
        }
        else {
            this->progressControl(message, param);
        }
    }

    void progressAuth(const char* message, BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
        if (ERaStrCmp(message, "start")) {
            this->startAuthorization(pCharacteristic, param);
        }
        else if (ERaStrNCmp(message, "confirm")) {
            this->confirmAuthorization(message + 7, pCharacteristic, param);
        }
    }

    void progressControl(const char* message, esp_ble_gatts_cb_param_t* param) {
        if ((this->baseTopic == nullptr) ||
            (this->callback == nullptr)) {
            return;
        }
        if (!ERaBLEVerify::isVerified(param->write.conn_id)) {
            this->mServer->disconnect(param->write.conn_id);
            ERaBLEVerify::removeVerify(param->write.conn_id);
            return;
        }
        if (strchr(message, ';') == nullptr) {
            return;
        }

        char* copy = ERaStrdup(message);
        if (copy == nullptr) {
            return;
        }
        ERaDataBuff data(copy, strlen(copy) + 1);
        data.split(";");

        char payload[30] {0};
        char aTopic[MAX_TOPIC_LENGTH] {0};
        FormatString(aTopic, this->baseTopic);
        FormatString(aTopic, ERA_PUB_PREFIX_ARDUINO_DOWN_TOPIC, data.at(0).getInt());
        FormatString(payload, R"json({"value":%.2f})json", data.at(1).getDouble());

        this->callback(aTopic, payload);
        free(copy);
        copy = nullptr;
    }

    void startAuthorization(BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
        if (ERaBLEVerify::isVerified(param->write.conn_id)) {
            return;
        }
        ERaInt_t verifyCode = ERaBLEVerify::getVerifyCode(param->write.conn_id, false);
        if (verifyCode < 0) {
            this->mServer->disconnect(param->write.conn_id);
            return;
        }
        char code[20] {0};
        FormatString(code, "%08d", verifyCode);
        this->write(pCharacteristic, code);
#if defined(ERA_BLE_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify code: %s"), param->write.conn_id, code);
#endif
    }

    void confirmAuthorization(const char* message, BLECharacteristic* pCharacteristic, esp_ble_gatts_cb_param_t* param) {
        ERaInt_t verifyCode = ERaBLEVerify::getVerifyCode(param->write.conn_id, true);
        if (verifyCode < 0) {
            this->mServer->disconnect(param->write.conn_id);
            return;
        }
        char code[20] {0};
        char verify[50] {0};
        FormatString(code, "%08d", verifyCode);
        ERaStrConcat(verify, this->mTranspProp->password);
        ERaStrConcat(verify, code);

#if defined(ERA_BLE_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify data: %s"), param->write.conn_id, verify);
#endif

        MD5 md5;
        md5.begin();
        md5.update((const char*)verify);
        const char* encrypted = md5.finalize();
        if (encrypted == nullptr) {
        }
        else if (ERaStrCmp(message, encrypted)) {
            this->write(pCharacteristic, "confirmed");
            ERaBLEVerify::setVerified(param->write.conn_id, true);
#if defined(ERA_BLE_DEBUG)
            ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify match: %s"), param->write.conn_id, encrypted);
#endif
            return;
        }
        this->mServer->disconnect(param->write.conn_id);
        ERaBLEVerify::removeVerify(param->write.conn_id);
#if defined(ERA_BLE_DEBUG)
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connection id: %d, No MD5 match: Local = %s, Target = %s"),
                                    param->write.conn_id, encrypted, message);
#endif
    }

    void response(const char* status) {
        this->write(status);
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
        item = cJSON_GetObjectItem(root, "password");
        if (!cJSON_IsString(item)) {
            return false;
        }
        if (!ERaStrCmp(item->valuestring, this->mTranspProp->password)) {
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

    const char* getDeviceName() {
        if (this->mUserDeviceName != NULL) {
            return this->mUserDeviceName;
        }
        if (strlen(this->mTranspProp->address) == 16) {
            return this->mTranspProp->address;
        }

        uint32_t hash = ERaHash(this->mTranspProp->address);
        ClearArray(this->mDeviceName);
        FormatString(this->mDeviceName, "ERa-%08x", hash);
        return this->mDeviceName;
    }

    bool isNewService() {
        return (strlen(this->mTranspProp->address) == 36);
    }

    ERaBluetooth*& mTranspProp;
    ERaApiHandler* mApi;
    unsigned long mTimeout;
    bool mConnected;
    bool mInitialized;
    bool mUseEncrypt;
    ERaQueue<uint8_t, ERA_MAX_READ_BYTES> mRxBuffer;
    char mDeviceName[33] {0};
    const char* mUserDeviceName;

    BLEServer* mServer;
    BLEService* mService;
    BLECharacteristic* mCharacteristicTKN;
    BLECharacteristic* mCharacteristicRSP;
    BLECharacteristic* mCharacteristicCMD;

    BLEService* mNewService;
    BLECharacteristic* mCharacteristicAUT;
    BLECharacteristic* mCharacteristicCTR;
};

using ERaBLEPeripheral = ERaBLETransp;

#endif /* INC_ERA_BLE_TRANSP_ESP32_HPP_ */
