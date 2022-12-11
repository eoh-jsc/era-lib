#ifndef INC_ERA_FLASH_LITTLEFS_HPP_
#define INC_ERA_FLASH_LITTLEFS_HPP_

#include <LittleFS.h>

#define SPIFFS LittleFS

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
};

void ERaFlash::begin() {
    SPIFFS.end();
    if (!SPIFFS.begin()) {
        ERA_LOG(TAG, ERA_PSTR("ERa flash init FAILED"));
    }
}

char* ERaFlash::readFlash(const char* filename) {
    File file = SPIFFS.open(filename, "r");
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

size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    ERA_FORCE_UNUSED(key);
    ERA_FORCE_UNUSED(buf);
    ERA_FORCE_UNUSED(maxLen);
    return 0;
}

void ERaFlash::writeFlash(const char* filename, const char* buf) {
    if (buf == nullptr) {
        return;
    }
    File file = SPIFFS.open(filename, "w");
    if (!file) {
        return;
    }
    file.print(buf);
    file.close();
}

size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    ERA_FORCE_UNUSED(key);
    ERA_FORCE_UNUSED(value);
    ERA_FORCE_UNUSED(len);
    return 0;
}

#endif /* INC_ERA_FLASH_LITTLEFS_HPP_ */
