#ifndef INC_ERA_FLASH_ESP32_HPP_
#define INC_ERA_FLASH_ESP32_HPP_

#include <SPIFFS.h>
#include <Preferences.h>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
    {}
    ~ERaFlash()
    {}

    void begin();
    char* readFlash(const char* filename);
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char* filename, const char* buf);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
    Preferences preferences;
};

inline
void ERaFlash::begin() {
    SPIFFS.end();
    if (!SPIFFS.begin(true)) {
        ERA_LOG(TAG, ERA_PSTR("ERa flash init FAILED"));
    }
    this->preferences.end();
    if (!this->preferences.begin("era", false)) {
        ERA_LOG(TAG, ERA_PSTR("ERa flash init FAILED"));
    }
}

inline
char* ERaFlash::readFlash(const char* filename) {
    File file = SPIFFS.open(filename, FILE_READ);
    if (!file) {
        return nullptr;
    }
    size_t size = file.size();
    if (!size) {
        file.close();
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(size + 1);
    if (buf == nullptr) {
        file.close();
        return nullptr;
    }
    buf[size] = '\0';
    file.readBytes(buf, size);
    file.close();
    return buf;
}

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    return preferences.getBytes(key, buf, maxLen);
}

inline
void ERaFlash::writeFlash(const char* filename, const char* buf) {
    if (buf == nullptr) {
        return;
    }
    File file = SPIFFS.open(filename, FILE_WRITE);
    if (!file) {
        return;
    }
    file.print(buf);
    file.close();
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    return preferences.putBytes(key, value, len);
}

#endif /* INC_ERA_FLASH_ESP32_HPP_ */
