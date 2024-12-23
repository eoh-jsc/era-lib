#ifndef INC_ERA_BLE_TRANSP_ARDUINO_HPP_
#define INC_ERA_BLE_TRANSP_ARDUINO_HPP_

#include <ArduinoBLE.h>
#include <ArduinoBearSSL.h>
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
#include <Encrypt/ERaEncryptBearSSL.hpp>

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
    , public ERaEncryptBearSSL
{
    const char* TAG = "BLETransp";

public:
    ERaBLETransp(ERaCallbackSetter& setter,
                bool base64 = false,
                bool encrypt = true)
        : ERaEncryptBearSSL(base64)
        , mTranspProp(ERaBluetooth::instance())
        , mApi(NULL)
        , mTimeout(1000L)
        , mConnected(false)
        , mInitialized(false)
        , mUseEncrypt(encrypt)
        , mUserDeviceName(NULL)
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

        ERaEncryptBearSSL::begin(this->mTranspProp->secretKey);

        /* Set the local name peripheral advertises */
        BLE.setLocalName(this->getDeviceName());

        if (this->mInitialized) {
            return;
        }

        if (!BLE.begin()) {
            return;
        }

        this->mService = new BLEService(SERVICE_UUID);

        /* Add the characteristic to the service */
        /* Assign event handlers for characteristic */
        this->mCharacteristicTKN = new BLEByteCharacteristic(CHARACTERISTIC_UUID_TKN, BLEProperty::BLERead);
        this->mCharacteristicTKN->setEventHandler((BLECharacteristicEvent)BLEProperty::BLERead, _onRead);
        this->mService->addCharacteristic(*this->mCharacteristicTKN);

        this->mCharacteristicRSP = new BLEByteCharacteristic(CHARACTERISTIC_UUID_RSP, BLEProperty::BLENotify);
        this->mService->addCharacteristic(*this->mCharacteristicRSP);

        this->mCharacteristicCMD = new BLEByteCharacteristic(CHARACTERISTIC_UUID_CMD, BLEProperty::BLEWrite);
        this->mCharacteristicCMD->setEventHandler(BLECharacteristicEvent::BLEWritten, _onWrite);
        this->mService->addCharacteristic(*this->mCharacteristicCMD);

        if (this->isNewService()) {
            this->mNewService = new BLEService(SERVICE_UUID);

            /* Add the characteristic to the service */
            /* Assign event handlers for characteristic */
            this->mCharacteristicAUT = new BLEByteCharacteristic(CHARACTERISTIC_UUID_AUT, BLEProperty::BLERead |
                                                                                          BLEProperty::BLEWrite |
                                                                                          BLEProperty::BLEWriteWithoutResponse);
            this->mCharacteristicAUT->setEventHandler((BLECharacteristicEvent)BLEProperty::BLERead, _onRead);
            this->mCharacteristicAUT->setEventHandler(BLECharacteristicEvent::BLEWritten, _onWrite);
            this->mNewService->addCharacteristic(*this->mCharacteristicAUT);

            this->mCharacteristicCTR = new BLEByteCharacteristic(CHARACTERISTIC_UUID_CTR, BLEProperty::BLEWrite);
            this->mCharacteristicCTR->setEventHandler(BLECharacteristicEvent::BLEWritten, _onWrite);
            this->mNewService->addCharacteristic(*this->mCharacteristicCTR);
        }

        if (this->isNewService()) {
            /* Add service */
            BLE.addService(*this->mNewService);
        }
        /* Add service */
        BLE.addService(*this->mService);

        if (this->isNewService()) {
            /* Set the UUID for the service this peripheral advertises */
            BLE.setAdvertisedService(*this->mNewService);
        }
        else {
            /* Set the UUID for the service this peripheral advertises */
            BLE.setAdvertisedService(*this->mService);
        }

        /* Assign event handlers for connected, disconnected */
        BLE.setEventHandler(BLEDeviceEvent::BLEConnected, _onConnect);
        BLE.setEventHandler(BLEDeviceEvent::BLEDisconnected, _onDisconnect);

        /* Start advertising */
        BLE.advertise();

        this->mInitialized = true;
    }

    void run() override {
        BLE.poll();
        if (!this->connected()) {
            return;
        }

        this->progressData();
        if (!ERaBLEVerify::hasVerify()) {
            return;
        }
        ERaInt_t connID = ERaBLEVerify::connTimeout();
        if (connID >= 0) {
            BLEDevice peripheral = BLE.available();
            if (peripheral) {
                peripheral.disconnect();
            }
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
        this->mCharacteristicRSP->writeValue((uint8_t*)buf, size);
        this->mCharacteristicRSP->written();
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

    int timedRead() {
        uint8_t c {0};
        this->read(&c, 1);
        return (int)c;
    }

    bool readBytesUntil(uint8_t* buf, size_t size, char terminator) {
        if (buf == nullptr) {
            return false;
        }
        if (!size) {
            return false;
        }
        size_t index {0};
        bool found {false};
        while (index < size) {
            int c = this->timedRead();
            if (c < 0) {
                break;
            }
            if (c == terminator) {
                found = true;
                break;
            }
            *buf++ = (uint8_t)c;
            index++;
        }
        return found;
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
            } while(!len && (ERaMillis() - startMillis < ERA_BLE_YIELD_MS));
        } while (len);

        ERaDataBuff splitBuf(buf, size + 1);

        if (splitBuf.size() >= 3) {
            const char* uuid = splitBuf.at(0).getString();
            uint16_t connID = splitBuf.at(1).getInt();
            this->processMessage((uint8_t*)splitBuf.at(2).getString(),
                                splitBuf.at(2).length(), uuid, connID);
        }

        if (buf != locBuf) {
            free(buf);
        }
        buf = nullptr;
    }

    void processMessage(uint8_t* buf, size_t size, const char* uuid, uint16_t connID) {
        size_t dataLen {0};
        uint8_t* data = nullptr;
        if (this->mUseEncrypt) {
            ERaEncryptBearSSL::decrypt(buf, size, data, dataLen);
        }
        else {
            data = buf;
            dataLen = size;
        }
        if (dataLen && (data != nullptr)) {
            if (ERaStrCmp(uuid, CHARACTERISTIC_UUID_CMD)) {
                this->progress((char*)data);
            }
            else {
                this->progress((char*)data, uuid, connID);
            }
            ERA_LOG(TAG, ERA_PSTR("BLE data (%d): %s"), dataLen, data);
            if (data != buf) {
                free(data);
            }
            data = nullptr;
        }
        else {
            this->response("FAILED");
        }
    }

    static ERaBLETransp*& instance() {
        static ERaBLETransp* _instance = nullptr;
        return _instance;
    }

private:
    size_t write(const char* uuid, const char* buf) {
        if (ERaStrCmp(uuid, CHARACTERISTIC_UUID_AUT)) {
            this->mCharacteristicAUT->setValue((const uint8_t*)buf, strlen(buf));
        }
        return strlen(buf);
    }

    static inline
    void _onConnect(BLEDevice device) {
        ERaBLETransp::instance()->onConnect(device);
    }

    static inline
    void _onDisconnect(BLEDevice device) {
        ERaBLETransp::instance()->onDisconnect(device);
    }

    static inline
    void _onRead(BLEDevice device, BLECharacteristic characteristic) {
        ERaBLETransp::instance()->onRead(device, characteristic);
    }

    static inline
    void _onWrite(BLEDevice device, BLECharacteristic characteristic) {
        ERaBLETransp::instance()->onWrite(device, characteristic, 0U);
    }

    void onConnect(BLEDevice& device) {
        ERA_LOG(TAG, ERA_PSTR("BLE address %s connect"), device.address().c_str());
        ERaBLEVerify::getVerifyCode(0U, false);
        this->mConnected = true;
    }

    void onDisconnect(BLEDevice& device) {
        ERaBLEVerify::removeVerify(0U);
        ERA_LOG(TAG, ERA_PSTR("BLE address %s disconnect"), device.address().c_str());
        this->mConnected = BLE.connected();
    }

    void onRead(BLEDevice& device, BLECharacteristic& characteristic) {
        if (!ERaStrCmp(characteristic.uuid(), CHARACTERISTIC_UUID_TKN)) {
            return;
        }

        ERaInt_t otp = ERaOTP::createOTP();
        if (otp >= 0) {
            this->mCharacteristicTKN->writeValue(reinterpret_cast<uint8_t*>(&otp), sizeof(otp));
        }
        ERA_FORCE_UNUSED(device);
    }

    void onWrite(BLEDevice& device, BLECharacteristic& characteristic, uint16_t connID) {
        size_t size = characteristic.valueLength();
        const uint8_t* rxValue = characteristic.value();

        if (size) {
            const char* uuid = characteristic.uuid();
            this->mRxBuffer.put((const uint8_t*)uuid, strlen(uuid));
            this->mRxBuffer.put(0x00);

            char str[2 + 8 * sizeof(int)] {0};
            FormatString(str, "%u", connID);
            this->mRxBuffer.put((const uint8_t*)str, strlen(str));
            this->mRxBuffer.put(0x00);

            this->mRxBuffer.put(rxValue, size);
        }
        ERA_FORCE_UNUSED(device);
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

    void progress(const char* message, const char* uuid, uint16_t connID) {
        if (ERaStrCmp(uuid, CHARACTERISTIC_UUID_AUT)) {
            this->progressAuth(message, uuid, connID);
        }
        else {
            this->progressControl(message, connID);
        }
    }

    void progressAuth(const char* message, const char* uuid, uint16_t connID) {
        if (ERaStrCmp(message, "start")) {
            this->startAuthorization(uuid, connID);
        }
        else if (ERaStrNCmp(message, "confirm")) {
            this->confirmAuthorization(message + 7, uuid, connID);
        }
    }

    void progressControl(const char* message, uint16_t connID) {
        if ((this->baseTopic == nullptr) ||
            (this->callback == nullptr)) {
            return;
        }
        if (!ERaBLEVerify::isVerified(connID)) {
            ERaBLEVerify::removeVerify(connID);
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

    void startAuthorization(const char* uuid, uint16_t connID) {
        if (ERaBLEVerify::isVerified(connID)) {
            return;
        }
        ERaInt_t verifyCode = ERaBLEVerify::getVerifyCode(connID, false);
        if (verifyCode < 0) {
            return;
        }
        char code[20] {0};
        FormatString(code, "%08d", verifyCode);
        this->write(uuid, code);
#if defined(ERA_BLE_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify code: %s"), connID, code);
#endif
    }

    void confirmAuthorization(const char* message, const char* uuid, uint16_t connID) {
        ERaInt_t verifyCode = ERaBLEVerify::getVerifyCode(connID, false);
        if (verifyCode < 0) {
            return;
        }
        char code[20] {0};
        char verify[50] {0};
        FormatString(code, "%08d", verifyCode);
        ERaStrConcat(verify, this->mTranspProp->password);
        ERaStrConcat(verify, code);

#if defined(ERA_BLE_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify data: %s"), connID, verify);
#endif

        MD5 md5;
        md5.begin();
        md5.update((const char*)verify);
        const char* encrypted = md5.finalize();
        if (encrypted == nullptr) {
        }
        else if (ERaStrCmp(message, encrypted)) {
            this->write(uuid, "confirmed");
            ERaBLEVerify::setVerified(connID, true);
#if defined(ERA_BLE_DEBUG)
            ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify match: %s"), connID, encrypted);
#endif
            return;
        }
        ERaBLEVerify::removeVerify(connID);
#if defined(ERA_BLE_DEBUG)
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connection id: %d, No MD5 match: Local = %s, Target = %s"),
                                    connID, encrypted, message);
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

    BLEService* mService;
    BLECharacteristic* mCharacteristicTKN;
    BLECharacteristic* mCharacteristicRSP;
    BLECharacteristic* mCharacteristicCMD;

    BLEService* mNewService;
    BLECharacteristic* mCharacteristicAUT;
    BLECharacteristic* mCharacteristicCTR;
};

using ERaBLEPeripheral = ERaBLETransp;

#endif /* INC_ERA_BLE_TRANSP_ARDUINO_HPP_ */
