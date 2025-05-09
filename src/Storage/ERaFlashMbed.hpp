#ifndef INC_ERA_FLASH_MBED_HPP_
#define INC_ERA_FLASH_MBED_HPP_

#include <stddef.h>
#include <sys/stat.h>
#include <BlockDevice.h>
#include <FlashIAPBlockDevice.h>
#include <LittleFileSystem.h>

#if defined(ERA_FLASH_SIZE)
    #define ERA_MBED_FS_SIZE_KB         ERA_FLASH_SIZE
#else
    #define ERA_MBED_FS_SIZE_KB         64
#endif

#if defined(ARDUINO_ARCH_RP2040)
    #define ERA_MBED_FLASH_SIZE         (2 * 1024 * 1024)
#elif defined(ARDUINO_ARCH_NRF52840)
    #define ERA_MBED_FLASH_SIZE         (1 * 1024 * 1024)
#elif defined(ARDUINO_PORTENTA_H7_M7) || \
    defined(ARDUINO_PORTENTA_H7_M4)
    #define ERA_MBED_FLASH_SIZE         0xFFFFFFFF
#else
    #define ERA_MBED_FLASH_SIZE         0xFFFFFFFF
    #define ERA_MBED_BLOCK_DEVICE_DEFAULT
    #warning "Please set Block Device with method function 'setBlockDevice'"
#endif

#if !defined(XIP_BASE)
    #define XIP_BASE                    0
#endif

#define ERA_MBED_FS_LOCATION_END        ERA_MBED_FLASH_SIZE
#define ERA_MBED_FS_START               ERA_MBED_FS_LOCATION_END - (ERA_MBED_FS_SIZE_KB * 1024)

using namespace mbed;

class ERaFlash
{
    const char* TAG = "Flash";
public:
    ERaFlash()
        : mFile(nullptr)
        , mFs("fs")
        , mBd(nullptr)
        , mRemoved(false)
        , mInitialized(false)
    {
        this->mBd = this->createBlockDevice();
    }
    ~ERaFlash()
    {
#if !defined(ERA_MBED_BLOCK_DEVICE_DEFAULT)
        delete this->mBd;
#endif
    }

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
    size_t getBytesSize(const char* key);

    void setBlockDevice(BlockDevice& bd) {
#if !defined(ERA_MBED_BLOCK_DEVICE_DEFAULT)
        if (!this->mRemoved) {
            this->mRemoved = true;
            delete this->mBd;
        }
#endif
        this->mBd = &bd;
    }

protected:
private:
    BlockDevice* createBlockDevice() {
#if defined(ERA_MBED_BLOCK_DEVICE_DEFAULT)
        return BlockDevice::get_default_instance();
#elif defined(ARDUINO_PORTENTA_H7_M7) || defined(ARDUINO_PORTENTA_H7_M4)
        FlashIAP flash;
        int sectorSize {0};
        size_t flashSize {0};
        uint32_t startAddress {0};
        uint32_t availableSize {0};
        uint32_t flashStartAddress {0};
        if (flash.init()) {
            return BlockDevice::get_default_instance();
        }
        sectorSize = flash.get_sector_size(FLASHIAP_APP_ROM_END_ADDR);
        flashStartAddress = flash.get_flash_start();
        startAddress = [](uint32_t value, uint32_t size) -> uint32_t {
            return (((value - 1) / size) + 1) * size;
        } (FLASHIAP_APP_ROM_END_ADDR, sectorSize);
        flashSize = flash.get_flash_size();
        flash.deinit();
        availableSize = flashStartAddress + flashSize - startAddress;
        if (availableSize % (sectorSize * 2)) {
            availableSize = [](uint64_t value, uint64_t size) -> uint64_t {
                return (((value) / size)) * size;
            } (availableSize, sectorSize * 2);
        }
        if (availableSize > (ERA_MBED_FS_SIZE_KB * 1024)) {
            availableSize = ERA_MBED_FS_SIZE_KB * 1024;
        }
        return new FlashIAPBlockDevice(startAddress, availableSize);
#else
        return new FlashIAPBlockDevice(XIP_BASE + ERA_MBED_FS_START, (ERA_MBED_FS_SIZE_KB * 1024));
#endif
    }

    void mkdir(const char* path);

    FILE* mFile;
    LittleFileSystem mFs;
    BlockDevice* mBd;
    bool mRemoved;
    bool mInitialized;
};

inline
void ERaFlash::begin() {
    if (this->mInitialized) {
        return;
    }
    this->end();
    int err = this->mFs.mount(this->mBd);
    if (err) {
        err = this->mFs.reformat(this->mBd);
        if (err) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("ERa flash init FAILED"));
            return;
        }
    }
    this->mInitialized = true;
}

inline
void ERaFlash::end() {
    this->mFs.unmount();
    this->mInitialized = false;
}

inline
void ERaFlash::beginRead(const char* filename) {
    if (!this->mInitialized) {
        return;
    }

    this->endRead();
    this->mFile = fopen(filename, "r");
}

inline
char* ERaFlash::readLine() {
    if (!this->mInitialized) {
        return nullptr;
    }

    if (this->mFile == nullptr) {
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
        c = fgetc(this->mFile);
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

    if (this->mFile == nullptr) {
        return;
    }
    fclose(this->mFile);
    this->mFile = nullptr;
}

inline
void ERaFlash::beginWrite(const char* filename) {
    if (!this->mInitialized) {
        return;
    }

    this->endWrite();
    this->mFile = fopen(filename, "w");
    if (this->mFile == nullptr) {
        this->mkdir(filename);
        this->mFile = fopen(filename, "w");
    }
}

inline
void ERaFlash::writeLine(const char* buf) {
    if (!this->mInitialized) {
        return;
    }

    if (buf == nullptr) {
        return;
    }
    if (this->mFile == nullptr) {
        return;
    }
    fprintf(this->mFile, "%s\n", buf);
}

inline
void ERaFlash::endWrite() {
    if (!this->mInitialized) {
        return;
    }

    if (this->mFile == nullptr) {
        return;
    }
    fclose(this->mFile);
    this->mFile = nullptr;
}

inline
char* ERaFlash::readFlash(const char* filename) {
    if (!this->mInitialized) {
        return nullptr;
    }

    FILE* file = fopen(filename, "r");
    if (file == nullptr) {
        return nullptr;
    }
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    if (size <= 0) {
        fclose(file);
        return nullptr;
    }
    char* buf = (char*)ERA_MALLOC(size + 1);
    if (buf == nullptr) {
        fclose(file);
        return nullptr;
    }
    buf[size] = '\0';
    fgets(buf, size + 1, file);
    fclose(file);
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
    FILE* file = fopen(key, "rb");
    if (file == nullptr) {
        return 0;
    }
    fread(buf, maxLen, 1, file);
    fclose(file);
    return maxLen;
}

inline
void ERaFlash::writeFlash(const char* filename, const char* buf) {
    if (!this->mInitialized) {
        return;
    }

    if (buf == nullptr) {
        return;
    }
    FILE* file = fopen(filename, "w");
    if (file == nullptr) {
        this->mkdir(filename);
        file = fopen(filename, "w");
        if (file == nullptr) {
            return;
        }
    }
    fprintf(file, buf);
    fclose(file);
}

inline
size_t ERaFlash::writeFlash(const char* key, const void* value, size_t len) {
    if (!this->mInitialized) {
        return 0;
    }

    if (value == nullptr) {
        return 0;
    }
    FILE* file = fopen(key, "wb");
    if (file == nullptr) {
        this->mkdir(key);
        file = fopen(key, "wb");
        if (file == nullptr) {
            return 0;
        }
    }
    fwrite(value, len, 1, file);
    fclose(file);
    return len;
}

inline
size_t ERaFlash::getBytesSize(const char* key) {
    if (!this->mInitialized) {
        return 0;
    }

    FILE* file = fopen(key, "rb");
    if (file == nullptr) {
        return 0;
    }
    fseek(file, 0L, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return ((size >= 0) ? (size_t)size : 0);
}

inline
void ERaFlash::mkdir(const char* path) {
    if (path == nullptr) {
        return;
    }
    size_t len {0};
    char dir[256] {0};
    char* p = nullptr;
    struct stat st {0};

    snprintf(dir, sizeof(dir), "%s", path);
    len = strlen(dir);
    if (dir[len - 1] == '/') {
        dir[len - 1] = 0;
    }
    for (p = dir + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            if (stat(dir, &st) == -1) {
                ::mkdir(dir, 0755);
            }
            *p = '/';
        }
    }
}

#endif /* INC_ERA_FLASH_MBED_HPP_ */
