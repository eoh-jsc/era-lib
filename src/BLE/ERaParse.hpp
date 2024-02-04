#ifndef INC_ERA_BLE_PARSE_HPP_
#define INC_ERA_BLE_PARSE_HPP_

#include <string.h>
#include <ERa/ERaDebug.hpp>
#include <ERa/ERaHelpersDef.hpp>
#include <Utility/ERaQueue.hpp>

#define LENGTH_SECRET_KEY   16

class ERaBluetooth
{
    enum ParseConfigT {
        PARSE_CONFIG_ID = 1,
        PARSE_CONFIG_ADDRESS = 2,
        PARSE_CONFIG_PASSWORD = 3,
        PARSE_CONFIG_SECRET_KEY = 4
    };

public:
    ERaBluetooth()
    {}
    ~ERaBluetooth()
    {}

    void parseConfig(const char* ptr);
    bool updateHashID(const char* hash);
    bool equals(const char* hash);

    bool operator == (const char* hash);
    bool operator != (const char* hash);

    unsigned int id;
    char hashID[37];
    char address[17];
    char password[17];
    uint8_t secretKey[LENGTH_SECRET_KEY];

    static ERaBluetooth*& instance() {
        static ERaBluetooth* _instance = nullptr;
        return _instance;
    }

    static ERaBluetooth* getInstance() {
        if (ERaBluetooth::instance() == nullptr) {
            ERaBluetooth::instance() = new ERaBluetooth();
        }
        return ERaBluetooth::instance();
    }

protected:
private:
    void processParseConfig(int part, const char* ptr, size_t len);
    void processParseConfigId(const char* ptr, size_t len);
    void processParseConfigAddress(const char* ptr, size_t len);
    void processParseConfigPassword(const char* ptr, size_t len);
    void processParseConfigSecretKey(const char* ptr, size_t len);
    void copyPasswordToSecretKey() {
        CopyToArray(this->password, this->secretKey);
    }
};

inline
void ERaBluetooth::parseConfig(const char* ptr) {
    if (ptr == nullptr) {
        return;
    }
    if (!strlen(ptr)) {
        return;
    }

    size_t len = strlen(ptr);
    int part = 0;
    size_t position = 0;
    for (size_t i = 0; i < len; ++i) {
        if (ptr[i] == ';') {
            part++;
            this->processParseConfig(part, ptr + position, i - position);
            position = i + 1;
        }
    }

    this->copyPasswordToSecretKey();
}

inline
bool ERaBluetooth::updateHashID(const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    if (strcmp(this->hashID, hash)) {
        snprintf(this->hashID, sizeof(this->hashID), hash);
        return true;
    }
    return false;
}

inline
bool ERaBluetooth::equals(const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    return !strcmp(this->hashID, hash);
}

inline
bool ERaBluetooth::operator == (const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    return !strcmp(this->hashID, hash);
}

inline
bool ERaBluetooth::operator != (const char* hash) {
    if (hash == nullptr) {
        return false;
    }
    return strcmp(this->hashID, hash);
}

inline
void ERaBluetooth::processParseConfig(int part, const char* ptr, size_t len) {
    switch(part) {
        case ParseConfigT::PARSE_CONFIG_ID:
            this->processParseConfigId(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_ADDRESS:
            this->processParseConfigAddress(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_PASSWORD:
            this->processParseConfigPassword(ptr, len);
            break;
        case ParseConfigT::PARSE_CONFIG_SECRET_KEY:
            this->processParseConfigSecretKey(ptr, len);
            break;
        default:
            break;
    }
}

inline
void ERaBluetooth::processParseConfigId(const char* ptr, size_t len) {
    LOC_BUFFER_PARSE

    memcpy(buf, ptr, len);
    this->id = atoi(buf);

    FREE_BUFFER_PARSE
}

inline
void ERaBluetooth::processParseConfigAddress(const char* ptr, size_t len) {
    if (len != 16) {
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        this->address[i] = ptr[i];
    }
    this->address[len] = 0;
}

inline
void ERaBluetooth::processParseConfigPassword(const char* ptr, size_t len) {
    if (len != 16) {
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        this->password[i] = ptr[i];
    }
    this->password[len] = 0;
}

inline
void ERaBluetooth::processParseConfigSecretKey(const char* ptr, size_t len) {
    if (len != 16) {
        return;
    }

    for (size_t i = 0; i < len; ++i) {
        this->secretKey[i] = (uint8_t)ptr[i];
    }
}

#endif /* INC_ERA_BLE_PARSE_HPP_ */
