#ifndef INC_MVP_FLASH_ESP8266_HPP_
#define INC_MVP_FLASH_ESP8266_HPP_

#include <LittleFS.h>
#include <Preferences.h>

#define SPIFFS LittleFS

class MVPFlash
{
    const char* TAG = "Flash";
public:
    MVPFlash()
    {}
    ~MVPFlash()
    {}

    void begin();
    char* readFlash(const char* filename);
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char *filename, const char* buf);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
    Preferences preferences;
};

void MVPFlash::begin() {
    SPIFFS.end();
    if (!SPIFFS.begin()) {
        MVP_LOG(TAG, "MVP flash init FAILED");
    }
    this->preferences.end();
    if (!this->preferences.begin("mvp", false)) {
        MVP_LOG(TAG, "MVP flash init FAILED");
    }
}

char* MVPFlash::readFlash(const char* filename) {
    File file = SPIFFS.open(filename, "r");
    if (!file) {
        return nullptr;
    }
    size_t size = file.size();
    if (!size) {
        file.close();
        return nullptr;
    }
    char* buf = (char*)MVP_MALLOC(size + 1);
    if (buf == nullptr) {
        file.close();
        return nullptr;
    }
    buf[size] = '\0';
    file.readBytes(buf, size);
    file.close();
    return buf;
}

size_t MVPFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    return preferences.getBytes(key, buf, maxLen);
}

void MVPFlash::writeFlash(const char *filename, const char* buf) {
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

size_t MVPFlash::writeFlash(const char* key, const void* value, size_t len) {
    return preferences.putBytes(key, value, len);
}

#endif /* INC_MVP_FLASH_ESP8266_HPP_ */