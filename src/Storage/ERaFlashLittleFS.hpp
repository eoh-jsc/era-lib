#ifndef INC_ERA_FLASH_LITTLE_FS_HPP_
#define INC_ERA_FLASH_LITTLE_FS_HPP_

#include <LittleFS.h>

#define SPIFFS LittleFS

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : mInitialized(false)
    {}
    ~ERaFlash()
    {}

    void begin();
    void end();
    void beginRead(const char* filename);
    char* readLine();
    void endRead();
    void beginWrite(const char* filename);
    void writeLine(const char* buf);
    void endWrite();
    char* readFlash(const char* filename);
    size_t readFlash(const char* key, void* buf, size_t maxLen);
    void writeFlash(const char* filename, const char* buf);
    size_t writeFlash(const char* key, const void* value, size_t len);

protected:
private:
    File mFile;
    bool mInitialized;
};

inline
void ERaFlash::begin() {
    if (this->mInitialized) {
        return;
    }
    this->end();
    if (!SPIFFS.begin()) {
        ERA_LOG_ERROR(TAG, ERA_PSTR("ERa flash init FAILED"));
        return;
    }
    this->mInitialized = true;
}

inline
void ERaFlash::end() {
    SPIFFS.end();
    this->mInitialized = false;
}

inline
void ERaFlash::beginRead(const char* filename) {
    if (!this->mInitialized) {
        return;
    }

    this->endRead();
    this->mFile = SPIFFS.open(filename, "r");
}

inline
char* ERaFlash::readLine() {
    if (!this->mInitialized) {
        return nullptr;
    }

    if (!this->mFile) {
        return nullptr;
    }
    int c {0};
    size_t pos {0};
    size_t size = 1024;
    char* buffer = (char*)ERA_MALLOC(size);
    if (buffer == nullptr) {
        return nullptr;
    }
    do {
        c = this->mFile.read();
        if ((c != EOF) && (c != '\n')) {
            buffer[pos++] = (char)c;
        }
        if (pos >= size - 1) {
            size += size;
            char* copy = (char*)ERA_REALLOC(buffer, size);
            if (copy == nullptr) {
                free(buffer);
                buffer = nullptr;
                return nullptr;
            }
            buffer = copy;
        }
    } while ((c != EOF) && (c != '\n'));
    buffer[pos] = 0;
    if (!pos) {
        free(buffer);
        buffer = nullptr;
    }
    return buffer;
}

inline
void ERaFlash::endRead() {
    if (!this->mInitialized) {
        return;
    }

    if (!this->mFile) {
        return;
    }
    this->mFile.close();
}

inline
void ERaFlash::beginWrite(const char* filename) {
    if (!this->mInitialized) {
        return;
    }

    this->endWrite();
    this->mFile = SPIFFS.open(filename, "w");
}

inline
void ERaFlash::writeLine(const char* buf) {
    if (!this->mInitialized) {
        return;
    }

    if (!this->mFile) {
        return;
    }
    this->mFile.printf("%s\n", buf);
}

inline
void ERaFlash::endWrite() {
    if (!this->mInitialized) {
        return;
    }

    if (!this->mFile) {
        return;
    }
    this->mFile.close();
}

inline
char* ERaFlash::readFlash(const char* filename) {
    if (!this->mInitialized) {
        return nullptr;
    }

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

inline
size_t ERaFlash::readFlash(const char* key, void* buf, size_t maxLen) {
    if (!this->mInitialized) {
        return 0;
    }

    if (buf == nullptr) {
        return 0;
    }
    File file = SPIFFS.open(key, "r");
    if (!file) {
        return 0;
    }
    size_t size = file.size();
    if (!size) {
        file.close();
        return 0;
    }
    size = ERaMin(size, maxLen);
    file.read((uint8_t*)buf, size);
    file.close();
    return size;
}

inline
void ERaFlash::writeFlash(const char* filename, const char* buf) {
    if (!this->mInitialized) {
        return;
    }

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

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    if (!this->mInitialized) {
        return 0;
    }

    if (value == nullptr) {
        return 0;
    }
    File file = SPIFFS.open(key, "w");
    if (!file) {
        return 0;
    }
    size_t size {0};
    size = file.write((uint8_t*)value, len);
    file.close();
    return size;
}

#endif /* INC_ERA_FLASH_LITTLE_FS_HPP_ */
