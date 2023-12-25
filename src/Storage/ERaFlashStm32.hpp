#ifndef INC_ERA_FLASH_STM32_HPP_
#define INC_ERA_FLASH_STM32_HPP_

#include <stddef.h>
#include <EEPROM.h>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : position(0)
        , initialized(false)
        , maxLength(EEPROM.length() - 1024UL)
    {}
    ~ERaFlash()
    {}

    void begin();
    void end();
    void beginRead(const char* filename);
    char* readLine();
    void endRead() {};
    void beginWrite(const char* filename);
    void beginWriteWithMode(const char* filename, bool append = false);
    void writeLine(const char* buf);
    void endWrite();
    char* readFlash(const char* filename);
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char* filename, const char* buffer);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
#if !defined(DATA_EEPROM_BASE)
    bool readFlashUntil(const char c, char* buffer = nullptr, size_t size = 0);
    bool clearFlashUntil(const char c);
    void flashMove(size_t dst, size_t src);
    size_t getSizeFromPosition();
    void setPosition(size_t pos) {
        this->position = pos;
    }
#endif

    size_t getSize(const char* filename);

    size_t position;
    bool initialized;
    const size_t maxLength;
};

inline
void ERaFlash::begin() {
    if (this->initialized) {
        return;
    }
    this->end();
    EEPROM.begin();
#if !defined(DATA_EEPROM_BASE)
    eeprom_buffer_fill();
#endif
    this->initialized = true;
}

inline
void ERaFlash::end() {
    EEPROM.end();
    this->initialized = false;
}

inline
void ERaFlash::beginRead(const char* filename) {
#if defined(DATA_EEPROM_BASE)
    ERA_FORCE_UNUSED(filename);
#else
    this->setPosition(0);
    while (this->readFlashUntil('/')) {
        size_t size = this->getSizeFromPosition() + 1;
        char* buf = (char*)ERA_MALLOC(size);
        if (buf == nullptr) {
            return;
        }
        this->readFlashUntil('\0', buf, size);
        if (strcmp(filename, buf)) {
            this->readFlashUntil('\0');
            free(buf);
            continue;
        }
        free(buf);
        break;
    }
#endif
}

inline
char* ERaFlash::readLine() {
#if defined(DATA_EEPROM_BASE)
    return nullptr;
#else
    size_t size = this->getSizeFromPosition() + 1;
    char* buf = (char*)ERA_MALLOC(size);
    if (buf == nullptr) {
        return nullptr;
    }
    this->readFlashUntil('\0', buf, size);
    if (!strlen(buf)) {
        free(buf);
        return nullptr;
    }
    return buf;
#endif
}

inline
void ERaFlash::beginWrite(const char* filename) {
    this->beginWriteWithMode(filename, false);
}

inline
void ERaFlash::beginWriteWithMode(const char* filename, bool append) {
#if defined(DATA_EEPROM_BASE)
    ERA_FORCE_UNUSED(filename);
#else
    size_t dst {0};
    bool found {false};
    this->setPosition(0);
    while (this->readFlashUntil('/')) {
        size_t size = this->getSizeFromPosition() + 1;
        char* buf = (char*)ERA_MALLOC(size);
        if (buf == nullptr) {
            return;
        }
        this->readFlashUntil('\0', buf, size);
        if (strcmp(filename, buf)) {
            this->readFlashUntil('\0');
            free(buf);
            continue;
        }
        if (!append) {
            this->setPosition(this->position - strlen(buf) - 2);
            dst = this->position;
            this->clearFlashUntil('\0');
            this->setPosition(++this->position);
            this->clearFlashUntil('\0');
        }
        free(buf);
        found = true;
        break;
    }
    if (found) {
        if (!append) {
            this->flashMove(dst, this->position);
            this->setPosition(0);
        }
        else {
            this->readFlashUntil('\0');
            this->position--;
            return;
        }
    }
    if (!append) {
        while (this->readFlashUntil('/')) {
            this->readFlashUntil('\0');
        }
        if (found && this->position) {
            this->readFlashUntil('\0');
        }
    }
    eeprom_buffered_write_byte(this->position++, '/');
    for (size_t i = 0; i < strlen(filename); ++i) {
        eeprom_buffered_write_byte(this->position++, filename[i]);
    }
    eeprom_buffered_write_byte(this->position++, 0);
#endif
}

inline
void ERaFlash::writeLine(const char* buf) {
#if defined(DATA_EEPROM_BASE)
    ERA_FORCE_UNUSED(buf);
#else
    if (buf == nullptr) {
        return;
    }
    size_t len = strlen(buf);
    if (!len || (len >= (this->maxLength - 1))) {
        return;
    }
    size_t end = this->position + len;
    for (size_t i = (this->maxLength - 1); i > end; --i) {
        eeprom_buffered_write_byte(i, eeprom_buffered_read_byte(i - len - 1));
    }
    for (size_t i = 0; i < len + 1; ++i) {
        eeprom_buffered_write_byte(this->position + i, 0);
    }
    size_t index {0};
    for (size_t i = this->position; i < (this->maxLength - 2); ++i) {
        eeprom_buffered_write_byte(i, buf[index++]);
        if (index == len) {
            this->position += index;
            break;
        }
    }
    eeprom_buffered_write_byte(this->position++, '\n');
#endif
}

inline
void ERaFlash::endWrite() {
#if !defined(DATA_EEPROM_BASE)
    eeprom_buffered_write_byte(this->position++, 0);
    eeprom_buffer_flush();
#endif
}

inline
char* ERaFlash::readFlash(const char* filename) {
#if defined(DATA_EEPROM_BASE)
    size_t size = this->getSize(filename);
    if (!size) {
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(size + 1);
    if (buf == nullptr) {
        return nullptr;
    }
    buf[size] = '\0';
    uint16_t index {0};
    uint16_t address = atof(filename);
    do {
        buf[index] = EEPROM.read(address + index);
    } while(--size && ++index);
    return buf;
#else
    this->setPosition(0);
    while (this->readFlashUntil('/')) {
        size_t size = this->getSizeFromPosition() + 1;
        char* buf = (char*)ERA_MALLOC(size);
        if (buf == nullptr) {
            return nullptr;
        }
        this->readFlashUntil('\0', buf, size);
        if (strcmp(filename, buf)) {
            this->readFlashUntil('\0');
            free(buf);
            continue;
        }
        free(buf);
        size = this->getSizeFromPosition() + 1;
        buf = (char*)ERA_MALLOC(size);
        if (buf == nullptr) {
            return nullptr;
        }
        this->readFlashUntil('\0', buf, size);
        return buf;
    }
    return nullptr;
#endif
}

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
#if defined(DATA_EEPROM_BASE)
    uint16_t address = atof(key);
    if ((address + maxLen) > EEPROM.length()) {
        return 0;
    }
    EEPROM.get(address, buf);
#else
    if (maxLen > 1024) {
        return 0;
    }
    uint8_t* buffer = (uint8_t*)buf;
    uint32_t address = EEPROM.length() - 1024;
    for (size_t i = 0; i < maxLen; ++i) {
        buffer[i] = eeprom_buffered_read_byte(address + i);
    }
    ERA_FORCE_UNUSED(key);
#endif
    return maxLen;
}

inline
void ERaFlash::writeFlash(const char* filename, const char* buffer) {
    if (buffer == nullptr) {
        return;
    }
    if (strlen(buffer) >= (this->maxLength - 1)) {
        return;
    }
#if defined(DATA_EEPROM_BASE)
    uint16_t address = atof(filename);
    size_t size = strlen(buffer);
    for (size_t i = 0; i < size; ++i) {
        EEPROM.write(address + i, buffer[i]);
    }
#else
    size_t dst {0};
    bool found {false};
    this->setPosition(0);
    while (this->readFlashUntil('/')) {
        size_t size = this->getSizeFromPosition() + 1;
        char* buf = (char*)ERA_MALLOC(size);
        if (buf == nullptr) {
            return;
        }
        this->readFlashUntil('\0', buf, size);
        if (strcmp(filename, buf)) {
            this->readFlashUntil('\0');
            free(buf);
            continue;
        }
        this->setPosition(this->position - strlen(buf) - 2);
        dst = this->position;
        this->clearFlashUntil('\0');
        this->setPosition(++this->position);
        this->clearFlashUntil('\0');
        free(buf);
        found = true;
        break;
    }
    if (found) {
        this->flashMove(dst, this->position);
        this->setPosition(0);
    }
    if (!strlen(buffer)) {
        return;
    }
    while (this->readFlashUntil('/')) {
        this->readFlashUntil('\0');
    }
    if (found && this->position) {
        this->readFlashUntil('\0');
    }
    size_t index {0};
    eeprom_buffered_write_byte(this->position++, '/');
    for (size_t i = 0; i < strlen(filename); ++i) {
        eeprom_buffered_write_byte(this->position++, filename[i]);
    }
    eeprom_buffered_write_byte(this->position++, 0);
    for (size_t i = this->position; i < (this->maxLength - 1); ++i) {
        eeprom_buffered_write_byte(i, buffer[index++]);
        if (index == strlen(buffer)) {
            this->position += index;
            break;
        }
    }
    eeprom_buffered_write_byte(this->position++, 0);
    eeprom_buffer_flush();
#endif
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
#if defined(DATA_EEPROM_BASE)
    if (len > EEPROM.length()) {
        return 0;
    }
    uint16_t address = atof(key);
    EEPROM.put(address, value);
#else
    if (len > 1024) {
        return 0;
    }
    const uint8_t* buffer = (const uint8_t*)value;
    uint32_t address = EEPROM.length() - 1024;
    for (size_t i = 0; i < len; ++i) {
        eeprom_buffered_write_byte(address + i, buffer[i]);
    }
    eeprom_buffer_flush();
    ERA_FORCE_UNUSED(key);
#endif
    return len;
}

#if !defined(DATA_EEPROM_BASE)

    inline
    bool ERaFlash::readFlashUntil(const char c, char* buffer, size_t size) {
        size_t index {0};
        for (size_t i = this->position; i < this->maxLength; ++i) {
            char ch = eeprom_buffered_read_byte(i);
            if (ch == c) {
                if (buffer != nullptr) {
                    buffer[index] = 0;
                }
                this->position = i + 1;
                return true;
            }
            if (buffer != nullptr) {
                buffer[index++] = ch;
                if (index >= size) {
                    break;
                }
            }
        }
        return false;
    }

    inline
    bool ERaFlash::clearFlashUntil(const char c) {
        for (size_t i = this->position; i < this->maxLength; ++i) {
            if (eeprom_buffered_read_byte(i) == c) {
                this->position = i;
                return true;
            }
            eeprom_buffered_write_byte(i, 0);
        }
        return false;
    }

    inline
    void ERaFlash::flashMove(size_t dst, size_t src) {
        size_t length = src - dst;
        size_t e = this->maxLength - length - 1;
        for (size_t i = dst; i < e; ++i) {
            eeprom_buffered_write_byte(i, eeprom_buffered_read_byte(length + i + 1));
        }
        for (size_t i = e; i < this->maxLength; ++i) {
            eeprom_buffered_write_byte(i, 0xFF);
        }
    }

    inline
    size_t ERaFlash::getSizeFromPosition() {
        char c {0};
        size_t size {0};
        do {
            c = eeprom_buffered_read_byte(this->position + size);
        } while (c && c != 0xFF && ++size);
        return size;
    }

#endif

inline
size_t ERaFlash::getSize(const char* filename) {
    char c {0};
    size_t size {0};
    uint16_t address = atof(filename);
    do {
        c = EEPROM.read(address + size);
    } while (c && c != 0xFF && ++size);
    return size;
}

#endif /* INC_ERA_FLASH_STM32_HPP_ */
