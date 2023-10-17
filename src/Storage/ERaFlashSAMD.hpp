#ifndef INC_ERA_FLASH_SAMD_HPP_
#define INC_ERA_FLASH_SAMD_HPP_

#include <FlashStorage.h>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : flashSize(FLASH_NB_OF_PAGES)
        , flashMemory((void*)(FLASH_SIZE - FLASH_NB_OF_PAGES),
                    FLASH_NB_OF_PAGES)
        , flashAddress((void*)(FLASH_SIZE - FLASH_NB_OF_PAGES))
    {}
    ~ERaFlash()
    {}

    void begin() {};
    void end() {};
    void beginRead(const char ERA_UNUSED *filename) {};
    char* readLine() { return nullptr; };
    void endRead() {};
    void beginWrite(const char ERA_UNUSED *filename) {};
    void writeLine(const char ERA_UNUSED *buf) {};
    void endWrite() {};
    char* readFlash(const char ERA_UNUSED *filename) { return nullptr; };
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char ERA_UNUSED *filename, const char ERA_UNUSED *buf) {};
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
    size_t flashSize;
    FlashClass flashMemory;
    const volatile void* flashAddress;
};

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    if (buf == nullptr) {
        return 0;
    }
    if (maxLen > this->flashSize) {
        return 0;
    }
    flashMemory.write(this->flashAddress, buf, maxLen);
    ERA_FORCE_UNUSED(key);
    return maxLen;
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    if (value == nullptr) {
        return 0;
    }
    if (len > this->flashSize) {
        return 0;
    }
    flashMemory.erase(this->flashAddress, this->flashSize);
    flashMemory.write(this->flashAddress, value, len);
    ERA_FORCE_UNUSED(key);
    return len;
}

#endif /* INC_ERA_FLASH_SAMD_HPP_ */
