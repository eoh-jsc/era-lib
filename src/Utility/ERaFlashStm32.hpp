#ifndef INC_ERA_FLASH_STM32_HPP_
#define INC_ERA_FLASH_STM32_HPP_

#include <EEPROM.h>

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : position(0)
    {}
    ~ERaFlash()
    {}

    void begin() {
        EEPROM.begin();
#if !defined(DATA_EEPROM_BASE)
        eeprom_buffer_fill();
#endif
    }

    char* readFlash(const char* filename);

    template <typename T>
    size_t readFlash(const char* key, T& buffer);

    void writeFlash(const char* filename, const char* buffer);

    template <typename T>
    size_t writeFlash(const char* key, const T& value);

protected:
private:
#if !defined(DATA_EEPROM_BASE)
    bool readFlashUntil(const char c, char* buffer = nullptr);
    bool clearFlashUntil(const char c);
    void flashMove(size_t dst, size_t src);
    size_t getSizeFromPosition();
    void setPosition(size_t pos) {
        this->position = pos;
    }
#endif

    size_t getSize(const char* filename);

    size_t position;
};

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
    uint16_t length {0};
    uint16_t address = atof(filename);
    do {
        buf[length] = EEPROM.read(address + length);
    } while(--size && ++length);
    return buf;
#else
    this->setPosition(0);
    while (this->readFlashUntil('/')) {
        char* buf = (char*)ERA_MALLOC(this->getSizeFromPosition() + 1);
		if (buf == nullptr) {
			return nullptr;
		}
        this->readFlashUntil('\0', buf);
        if (strcmp(filename, buf)) {
            this->readFlashUntil('\0');
            free(buf);
            continue;
        }
        free(buf);
        buf = (char*)ERA_MALLOC(this->getSizeFromPosition() + 1);
		if (buf == nullptr) {
			return nullptr;
		}
        this->readFlashUntil('\0', buf);
        return buf;
    }
    return nullptr;
#endif
}

template <typename T>
size_t ERaFlash::readFlash(const char* key, T& buffer) {
    if (sizeof(buffer) > EEPROM.length()) {
        return 0;
    }
    uint16_t address = atof(key);
    EEPROM.get(address, buffer);
    return sizeof(buffer);
}

void ERaFlash::writeFlash(const char* filename, const char* buffer) {
    if (buffer == nullptr) {
        return;
    }
    if (strlen(buffer) > EEPROM.length()) {
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
        char* buf = (char*)ERA_MALLOC(this->getSizeFromPosition() + 1);
		if (buf == nullptr) {
			return;
		}
		this->readFlashUntil('\0', buf);
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
    size_t length {0};
    eeprom_buffered_write_byte(this->position++, '/');
    for (size_t i = 0; i < strlen(filename); ++i) {
        eeprom_buffered_write_byte(this->position++, filename[i]);
    }
    eeprom_buffered_write_byte(this->position++, 0);
    for (size_t i = this->position; i < EEPROM.length(); ++i) {
        eeprom_buffered_write_byte(i, buffer[length++]);
        if (length == strlen(buffer)) {
            this->position += length + 1;
            break;
        }
    }
    eeprom_buffer_flush();
#endif
}

template <typename T>
size_t ERaFlash::writeFlash(const char* key, const T& value) {
    if (sizeof(value) > EEPROM.length()) {
        return 0;
    }
    uint16_t address = atof(key);
    EEPROM.put(address, value);
    return sizeof(value);
}

#if !defined(DATA_EEPROM_BASE)

    bool ERaFlash::readFlashUntil(const char c, char* buffer) {
        size_t length {0};
        for (size_t i = this->position; i < EEPROM.length(); ++i) {
            if (buffer != nullptr) {
                buffer[length++] = eeprom_buffered_read_byte(i);
            }
            if (eeprom_buffered_read_byte(i) == c) {
                this->position = i + 1;
                return true;
            }
        }
        return false;
    }

    bool ERaFlash::clearFlashUntil(const char c) {
        for (size_t i = this->position; i < EEPROM.length(); ++i) {
            if (eeprom_buffered_read_byte(i) == c) {
                this->position = i;
                return true;
            }
            eeprom_buffered_write_byte(i, 0);
        }
        return false;
    }

    void ERaFlash::flashMove(size_t dst, size_t src) {
        size_t length = src - dst;
        size_t e = EEPROM.length() - length - 1;
        for (size_t i = dst; i < e; ++i) {
            eeprom_buffered_write_byte(i, eeprom_buffered_read_byte(length + i + 1));
        }
        for (size_t i = e; i < EEPROM.length(); ++i) {
            eeprom_buffered_write_byte(i, 0);
        }
    }

    size_t ERaFlash::getSizeFromPosition() {
        char c {0};
        size_t size {0};
        do {
            c = eeprom_buffered_read_byte(this->position + size);
        } while (c && c != 0xFF && ++size);
        return size;
    }

#endif

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
