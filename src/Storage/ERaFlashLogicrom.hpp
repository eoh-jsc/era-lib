#ifndef INC_ERA_FLASH_LOGICROM_HPP_
#define INC_ERA_FLASH_LOGICROM_HPP_

#include <param.h>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
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
};

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    if (buf == nullptr) {
        return 0;
    }
    param_load(key, buf, maxLen);
    return maxLen;
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    if (value == nullptr) {
        return 0;
    }
    param_save(key, (void*)value, len);
    return len;
}

#endif /* INC_ERA_FLASH_LOGICROM_HPP_ */
