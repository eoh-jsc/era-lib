#ifndef INC_ERA_FLASH_REALTEK_HPP_
#define INC_ERA_FLASH_REALTEK_HPP_

#include <Storage/ERaFlashSetup.hpp>

#if defined(ERA_HAS_PREFERENCES_H)

    #include <Storage/ERaFlashPreferences.hpp>

#elif defined(ERA_HAS_FLASH_DB_H)

    #include <Storage/ERaFlashDB.hpp>

#else

    #include <FlashMemory.h>

    class ERaFlash
    {
        const char* TAG = "Flash";
    public:
        ERaFlash()
        {}
        ~ERaFlash()
        {}

        void begin() {}
        void end() {}
        void beginRead(const char ERA_UNUSED *filename) {}
        char* readLine() { return nullptr; }
        void endRead() {}
        void beginWrite(const char ERA_UNUSED *filename) {}
        void writeLine(const char ERA_UNUSED *buf) {}
        void endWrite() {}
        char* readFlash(const char ERA_UNUSED *filename) { return nullptr; }
        size_t readFlash(const char* key, void* buf, size_t maxLen);
        void writeFlash(const char ERA_UNUSED *filename, const char ERA_UNUSED *buf) {}
        size_t writeFlash(const char* key, const void* value, size_t len);
        size_t getBytesSize(const char* key) { return 0; }

    protected:
    private:
    };

    inline
    size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
        if (buf == nullptr) {
            return 0;
        }
        if (maxLen > FlashMemory.buf_size) {
            return 0;
        }
        FlashMemory.read();
        memcpy(buf, FlashMemory.buf, maxLen);
        ERA_FORCE_UNUSED(key);
        return maxLen;
    }

    inline
    size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
        if (value == nullptr) {
            return 0;
        }
        if (len > FlashMemory.buf_size) {
            return 0;
        }
        memcpy(FlashMemory.buf, value, len);
        FlashMemory.update();
        ERA_FORCE_UNUSED(key);
        return len;
    }

#endif

#endif /* INC_ERA_FLASH_REALTEK_HPP_ */
