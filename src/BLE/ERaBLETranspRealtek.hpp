#ifndef INC_ERA_BLE_TRANSP_REALTEK_HPP_
#define INC_ERA_BLE_TRANSP_REALTEK_HPP_

#include <BLEDevice.h>
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
        , mService(NULL)
        , mCharacteristicTKN(NULL)
        , mCharacteristicRSP(NULL)
        , mCharacteristicCMD(NULL)
        , mNewService(NULL)
        , mCharacteristicAUT(NULL)
        , mCharacteristicCTR(NULL)
        , mAdvertData(NULL)
        , mScanData(NULL)
        , mBleTask(NULL)
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

        if (this->mAdvertData == NULL) {
            this->mAdvertData = new BLEAdvertData();
        }

        this->mAdvertData->clear();
        this->mAdvertData->addFlags(GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED);
        this->mAdvertData->addCompleteName(this->getDeviceName());
        if (this->isNewService()) {
            this->mAdvertData->addCompleteServices(BLEUUID(this->mTranspProp->address));
        }
        this->mAdvertData->addCompleteServices(BLEUUID(SERVICE_UUID));

        if (this->mInitialized) {
            BLE.configAdvert()->setAdvData(*this->mAdvertData);
            BLE.configAdvert()->updateAdvertParams();
            return;
        }

        this->mScanData = new BLEAdvertData();
        if (this->isNewService()) {
            this->mScanData->addCompleteServices(BLEUUID(this->mTranspProp->address));
        }
        this->mScanData->addCompleteServices(BLEUUID(SERVICE_UUID));

        this->mCharacteristicTKN = new BLECharacteristic(CHARACTERISTIC_UUID_TKN);
        this->mCharacteristicTKN->setReadProperty(true);
        this->mCharacteristicTKN->setReadPermissions(GATT_PERM_READ);
        this->mCharacteristicTKN->setReadCallback(_onReadTKN);
        this->mCharacteristicTKN->setBufferLen(CHAR_VALUE_MAX_LEN);

        this->mCharacteristicRSP = new BLECharacteristic(CHARACTERISTIC_UUID_RSP);
        this->mCharacteristicRSP->setNotifyProperty(true);
        this->mCharacteristicRSP->setBufferLen(CHAR_VALUE_MAX_LEN);

        this->mCharacteristicCMD = new BLECharacteristic(CHARACTERISTIC_UUID_CMD);
        this->mCharacteristicCMD->setWriteProperty(true);
        this->mCharacteristicCMD->setWritePermissions(GATT_PERM_WRITE);
        this->mCharacteristicCMD->setWriteCallback(_onWriteCMD);
        this->mCharacteristicCMD->setBufferLen(CHAR_VALUE_MAX_LEN);

        this->mService = new BLEService(SERVICE_UUID);
        this->mService->addCharacteristic(*this->mCharacteristicTKN);
        this->mService->addCharacteristic(*this->mCharacteristicRSP);
        this->mService->addCharacteristic(*this->mCharacteristicCMD);

        if (this->isNewService()) {
            this->mCharacteristicAUT = new BLECharacteristic(CHARACTERISTIC_UUID_AUT);
            this->mCharacteristicAUT->setReadProperty(true);
            this->mCharacteristicAUT->setReadPermissions(GATT_PERM_READ);
            this->mCharacteristicAUT->setReadCallback(_onReadAUT);
            this->mCharacteristicAUT->setWriteProperty(true);
            this->mCharacteristicAUT->setWriteNRProperty(true);
            this->mCharacteristicAUT->setWritePermissions(GATT_PERM_WRITE);
            this->mCharacteristicAUT->setWriteCallback(_onWriteAUT);
            this->mCharacteristicAUT->setBufferLen(CHAR_VALUE_MAX_LEN);

            this->mCharacteristicCTR = new BLECharacteristic(CHARACTERISTIC_UUID_CTR);
            this->mCharacteristicCTR->setWriteProperty(true);
            this->mCharacteristicCTR->setWritePermissions(GATT_PERM_WRITE);
            this->mCharacteristicCTR->setWriteCallback(_onWriteCTR);
            this->mCharacteristicCTR->setBufferLen(CHAR_VALUE_MAX_LEN);

            this->mNewService = new BLEService(this->mTranspProp->address);
            this->mNewService->addCharacteristic(*this->mCharacteristicAUT);
            this->mNewService->addCharacteristic(*this->mCharacteristicCTR);
        }

        BLE.init();
        BLE.configAdvert()->setAdvData(*this->mAdvertData);
        BLE.configAdvert()->setScanRspData(*this->mScanData);
        BLE.configSecurity()->setPairable(true);
        BLE.configSecurity()->setAuthFlags(GAP_AUTHEN_BIT_BONDING_FLAG);
        if (this->isNewService()) {
            BLE.configServer(2);
            BLE.addService(*this->mNewService);
            BLE.addService(*this->mService);
        }
        else {
            BLE.configServer(1);
            BLE.addService(*this->mService);
        }

        BLE.beginPeripheral();

        this->mBleTask = ERaOs::osThreadNew(this->bleTask, "bleTask", 1024 * 5, this,
                                            configMAX_PRIORITIES - 4);

        this->mConnected = true;
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
            BLE.configConnection()->disconnect(connID);
        }
        ERaBLEVerify& verifies = (*this);
        for (const auto& verify : verifies) {
            connID = verify.getConnID();
            if (connID < 0) {
                continue;
            }
            if (BLE.connected(connID)) {
                continue;
            }
            ERaBLEVerify::removeVerify(connID);
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
        return this->write(buf, size, 0);
    }

    size_t write(const uint8_t* buf, size_t size, uint8_t connID) {
        if (this->mCharacteristicRSP == NULL) {
            return 0;
        }
        this->mCharacteristicRSP->setData((uint8_t*)buf, size);
        if (BLE.connected(connID)) {
            this->mCharacteristicRSP->notify(connID);
        }
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
            BLEUUID uuid(splitBuf.at(0).getString());
            uint8_t connID = splitBuf.at(1).getInt();
            this->processMessage((uint8_t*)splitBuf.at(2).getString(),
                                splitBuf.at(2).length(), uuid, connID);
        }

        if (buf != locBuf) {
            free(buf);
        }
        buf = nullptr;
    }

    void processMessage(uint8_t* buf, size_t size, BLEUUID& uuid, uint8_t connID) {
        size_t dataLen {0};
        uint8_t* data = nullptr;
        if (this->mUseEncrypt) {
            ERaEncryptMbedTLS::decrypt(buf, size, data, dataLen);
        }
        else {
            data = buf;
            dataLen = size;
        }
        if (dataLen && (data != nullptr)) {
            if (uuid == BLEUUID(CHARACTERISTIC_UUID_CMD)) {
                this->progress((char*)data, connID);
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
            this->response("FAILED", connID);
        }
    }

    static ERaBLETransp*& instance() {
        static ERaBLETransp* _instance = nullptr;
        return _instance;
    }

private:
    size_t write(BLEUUID& uuid, const char* buf) {
        if (uuid == BLEUUID(CHARACTERISTIC_UUID_AUT)) {
            this->mCharacteristicAUT->setData((uint8_t*)buf, strlen(buf));
        }
        return strlen(buf);
    }

    static inline
    void _onReadTKN(BLECharacteristic* pCharacteristic, uint8_t connID) {
        ERaBLETransp::instance()->onRead(pCharacteristic, CHARACTERISTIC_UUID_TKN, connID);
    }

    static inline
    void _onReadAUT(BLECharacteristic* pCharacteristic, uint8_t connID) {
        ERaBLETransp::instance()->onRead(pCharacteristic, CHARACTERISTIC_UUID_AUT, connID);
    }

    static inline
    void _onWriteCMD(BLECharacteristic* pCharacteristic, uint8_t connID) {
        ERaBLETransp::instance()->onWrite(pCharacteristic, CHARACTERISTIC_UUID_CMD, connID);
    }

    static inline
    void _onWriteAUT(BLECharacteristic* pCharacteristic, uint8_t connID) {
        ERaBLETransp::instance()->onWrite(pCharacteristic, CHARACTERISTIC_UUID_AUT, connID);
    }

    static inline
    void _onWriteCTR(BLECharacteristic* pCharacteristic, uint8_t connID) {
        ERaBLETransp::instance()->onWrite(pCharacteristic, CHARACTERISTIC_UUID_CTR, connID);
    }

    void onRead(BLECharacteristic* pCharacteristic, const char* uuid, uint8_t connID) {
        if (!ERaStrCmp(uuid, CHARACTERISTIC_UUID_TKN)) {
            return;
        }

        ERaInt_t otp = ERaOTP::createOTP();
        if (otp >= 0) {
            pCharacteristic->setData(reinterpret_cast<uint8_t*>(&otp), sizeof(otp));
        }
        else {
            BLE.configConnection()->disconnect(connID);
        }
    }

    void onWrite(BLECharacteristic* pCharacteristic, const char* uuid, uint8_t connID) {
        size_t size = pCharacteristic->getDataLen();
        const uint8_t* rxValue = pCharacteristic->getDataBuff();

        if (size) {
            this->mRxBuffer.put((const uint8_t*)uuid, strlen(uuid));
            this->mRxBuffer.put(0x00);

            char str[2 + 8 * sizeof(int)] {0};
            FormatString(str, "%u", connID);
            this->mRxBuffer.put((const uint8_t*)str, strlen(str));
            this->mRxBuffer.put(0x00);

            this->mRxBuffer.put(rxValue, size);
        }
    }

    void progress(const char* message, uint8_t connID) {
        cJSON* root = cJSON_Parse(message);
        if (!cJSON_IsObject(root)) {
            cJSON_Delete(root);
            root = nullptr;
            this->response("FAILED", connID);
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
            this->response("OK", connID);
        }
        else {
            this->response("FAILED", connID);
        }

        cJSON_Delete(root);
        root = nullptr;
    }

    void progress(const char* message, BLEUUID& uuid, uint8_t connID) {
        if (uuid == BLEUUID(CHARACTERISTIC_UUID_AUT)) {
            this->progressAuth(message, uuid, connID);
        }
        else {
            this->progressControl(message, connID);
        }
    }

    void progressAuth(const char* message, BLEUUID& uuid, uint8_t connID) {
        if (ERaStrCmp(message, "start")) {
            this->startAuthorization(uuid, connID);
        }
        else if (ERaStrNCmp(message, "confirm")) {
            this->confirmAuthorization(message + 7, uuid, connID);
        }
    }

    void progressControl(const char* message, uint8_t connID) {
        if ((this->baseTopic == nullptr) ||
            (this->callback == nullptr)) {
            return;
        }
        if (!ERaBLEVerify::isVerified(connID)) {
            BLE.configConnection()->disconnect(connID);
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

    void startAuthorization(BLEUUID& uuid, uint8_t connID) {
        if (ERaBLEVerify::isVerified(connID)) {
            return;
        }
        ERaInt_t verifyCode = ERaBLEVerify::getVerifyCode(connID, false);
        if (verifyCode < 0) {
            BLE.configConnection()->disconnect(connID);
            return;
        }
        char code[20] {0};
        FormatString(code, "%08d", verifyCode);
        this->write(uuid, code);
#if defined(ERA_BLE_DEBUG)
        ERA_LOG(TAG, ERA_PSTR("Connection id: %d, Verify code: %s"), connID, code);
#endif
    }

    void confirmAuthorization(const char* message, BLEUUID& uuid, uint8_t connID) {
        ERaInt_t verifyCode = ERaBLEVerify::getVerifyCode(connID, true);
        if (verifyCode < 0) {
            BLE.configConnection()->disconnect(connID);
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
        BLE.configConnection()->disconnect(connID);
        ERaBLEVerify::removeVerify(connID);
#if defined(ERA_BLE_DEBUG)
        ERA_LOG_ERROR(TAG, ERA_PSTR("Connection id: %d, No MD5 match: Local = %s, Target = %s"),
                                    connID, encrypted, message);
#endif
    }

    void response(const char* status, uint8_t connID) {
        this->write((const uint8_t*)status, strlen(status), connID);
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

    BLEAdvertData* mAdvertData;
    BLEAdvertData* mScanData;
    TaskHandle_t mBleTask;
};

using ERaBLEPeripheral = ERaBLETransp;

#endif /* INC_ERA_BLE_TRANSP_REALTEK_HPP_ */
