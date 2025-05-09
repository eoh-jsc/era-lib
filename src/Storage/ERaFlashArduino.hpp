#ifndef INC_ERA_FLASH_ARDUINO_HPP_
#define INC_ERA_FLASH_ARDUINO_HPP_

#include <Storage/ERaFlashSetup.hpp>

#if defined(ERA_HAS_PREFERENCES_H)

    #include <Storage/ERaFlashPreferences.hpp>

#elif defined(ERA_HAS_FLASH_DB_H)

    #include <Storage/ERaFlashDB.hpp>

#else

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
        size_t readFlash(const char ERA_UNUSED *key, void ERA_UNUSED *buf, size_t ERA_UNUSED maxLen) { return 0; }
        void writeFlash(const char ERA_UNUSED *filename, const char ERA_UNUSED *buf) {}
        size_t writeFlash(const char ERA_UNUSED *key, const void ERA_UNUSED *value, size_t ERA_UNUSED len) { return 0; }
        size_t getBytesSize(const char ERA_UNUSED *key) { return 0; }

    protected:
    private:
    };

#endif

#endif /* INC_ERA_FLASH_ARDUINO_HPP_ */
