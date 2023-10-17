#ifndef INC_ERA_OTA_STORAGE_RP2040_HPP_
#define INC_ERA_OTA_STORAGE_RP2040_HPP_

#include <stddef.h>
#include <stdint.h>
#include <BlockDevice.h>
#include <FlashIAPBlockDevice.h>
#include <FATFileSystem.h>

#define ERA_MAX_SKETCH_SIZE    0x100000

using namespace mbed;

class ERaOTAStorage
{
public:
    ERaOTAStorage()
        : file(nullptr)
        , fs("ota")
        , bd(nullptr)
        , maxSketchSize(ERA_MAX_SKETCH_SIZE)
    {
        this->bd = new FlashIAPBlockDevice(XIP_BASE + 0xF00000, this->maxSketchSize);
    }
    ~ERaOTAStorage()
    {}

    bool open(int length) {
        if (length > this->maxSketchSize) {
            return false;
        }

        ERaWatchdogFeed();

        int err = this->bd->init();
        if (err < 0) {
            return false;
        }
        this->bd->erase(XIP_BASE + 0xF00000, this->maxSketchSize);

        ERaWatchdogFeed();

        err = this->fs.reformat(this->bd);
        if (err != 0) {
            return false;
        }

        ERaWatchdogFeed();

        this->file = fopen("/ota/UPDATE.BIN.LZSS", "wb");
        if (this->file == nullptr) {
            return false;
        }
        return true;
    }

    size_t write(uint8_t value) {
        if (this->file == nullptr) {
            return 0;
        }
        fwrite(&value, 1, sizeof(value), this->file);
    }

    void close() {
        if (this->file != nullptr) {
            fclose(this->file);
            this->file = nullptr;
        }
        fs.unmount();
    }

    void apply() {
        NVIC_SystemReset();
    }

    uint32_t maxSize() const {
        return this->maxSketchSize;
    }

    static ERaOTAStorage& instance() {
        static ERaOTAStorage _instance;
        return _instance;
    }

protected:
private:
    FILE* file;
    FATFileSystem fs;
    BlockDevice* bd;
    uint32_t maxSketchSize;
};

#define InternalStorage ERaOTAStorage::instance()

#endif /* INC_ERA_OTA_STORAGE_RP2040_HPP_ */
