#ifndef INC_ERA_OTA_STORAGE_STM32_HPP_
#define INC_ERA_OTA_STORAGE_STM32_HPP_

#include <stddef.h>
#include <stdint.h>

#if defined(STM32F0xx)
    #include <stm32f0xx_hal_flash_ex.h>
#elif defined(STM32F1xx)
    #include <stm32f1xx_hal_flash_ex.h>
#elif defined(STM32F2xx)
    #include <stm32f2xx.h>
#elif defined(STM32F3xx)
    #include <stm32f3xx_hal_flash_ex.h>
#elif defined(STM32F4xx)
    #include <stm32f4xx.h>
#elif defined(STM32F7xx)
    #include <stm32f7xx.h>
    #define SMALL_SECTOR_SIZE       0x8000
    #define LARGE_SECTOR_SIZE       0x40000
#endif

#if defined(STM32F2xx) || defined(STM32F4xx)
    #define SMALL_SECTOR_SIZE       0x4000
    #define LARGE_SECTOR_SIZE       0x20000
#endif

#if !defined(ERA_OTA_STM32_SECTOR)
    #define ERA_OTA_STM32_SECTOR    6
#endif

#include "stm32yyxx_ll_utils.h"
extern "C" char * g_pfnVectors;

class ERaOTAStorage
{
#if defined(STM32F7xx)
    const uint32_t SECTOR_SIZE = 0x40000; // From sector 5
#elif defined(STM32F2xx) || defined(STM32F4xx)
    const uint32_t SECTOR_SIZE = 0x20000; // From sector 5 (for F2, F4)
#endif
    const uint32_t SKETCH_START_ADDRESS;
    const uint32_t PAGE_SIZE;
    const uint32_t MAX_FLASH;

public:
    ERaOTAStorage(uint8_t _sector)
        : SKETCH_START_ADDRESS((uint32_t)&g_pfnVectors - FLASH_BASE)
#if defined(FLASH_PAGE_SIZE)
        , PAGE_SIZE(FLASH_PAGE_SIZE)
#else
        , PAGE_SIZE(4)
#endif
        , MAX_FLASH((min(LL_GetFlashSize(), 512UL) * 1024))
    {
        this->sector = ((_sector < 5) ? 5 : _sector);
#if defined(FLASH_TYPEERASE_SECTORS)
        this->maxSketchSize = (SECTOR_SIZE * (this->sector - 4));
        this->storageStartAddress = (FLASH_BASE + this->maxSketchSize);
        this->maxSketchSize -= SKETCH_START_ADDRESS;
        if ((MAX_FLASH - this->maxSketchSize) < this->maxSketchSize) {
            this->maxSketchSize = (MAX_FLASH - this->maxSketchSize);
        }
#else
        this->maxSketchSize = ((MAX_FLASH - SKETCH_START_ADDRESS) / 2);
        this->maxSketchSize = ((this->maxSketchSize / PAGE_SIZE) * PAGE_SIZE); // align to page
        this->storageStartAddress = (FLASH_BASE + SKETCH_START_ADDRESS + this->maxSketchSize);
#endif
        this->pageAlignedLength = 0;
        this->writeIndex = 0;
        this->flashWriteAddress = this->storageStartAddress;
    }
    ~ERaOTAStorage()
    {}

    bool open(int length) {
        if (length > this->maxSketchSize) {
            return 0;
        }

        this->pageAlignedLength = ((length / PAGE_SIZE) + 1) * PAGE_SIZE; // align to page up
        this->flashWriteAddress = this->storageStartAddress;
        this->writeIndex = 0;

        FLASH_EraseInitTypeDef EraseInitStruct;
#if defined(FLASH_TYPEERASE_SECTORS)
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
        EraseInitStruct.Sector = this->sector;
        EraseInitStruct.NbSectors = (1 + (length / SECTOR_SIZE));
        EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
#else
        EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
        EraseInitStruct.PageAddress = this->flashWriteAddress;
        EraseInitStruct.NbPages = (this->pageAlignedLength / PAGE_SIZE);
#endif
#if defined(FLASH_BANK_1)
        EraseInitStruct.Banks = FLASH_BANK_1;
#endif

        uint32_t pageError = 0;
        return ((HAL_FLASH_Unlock() == HAL_OK) &&
            (HAL_FLASHEx_Erase(&EraseInitStruct, &pageError) == HAL_OK));
    }

    size_t write(uint8_t value) {
        this->storageData.data8[this->writeIndex] = value;
        this->writeIndex++;

        if (this->writeIndex == 4) {
            this->writeIndex = 0;
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, this->flashWriteAddress,
                                            this->storageData.data32) != HAL_OK) {
                return 0;
            }
            this->flashWriteAddress += 4;
        }
        return 1;
    }

    void close() {
        while (this->writeIndex) {
            this->write(0xff);
        }
        HAL_FLASH_Lock();
    }

    void apply() {
        if (HAL_FLASH_Unlock() != HAL_OK) {
            return;
        }
        noInterrupts();
        ERaOTAStorage::copyToFlash((FLASH_BASE + SKETCH_START_ADDRESS),
                                (uint8_t*)this->storageStartAddress,
                                this->pageAlignedLength, true);
    }

    uint32_t maxSize() const {
        return this->maxSketchSize;
    }

    static ERaOTAStorage& instance(uint8_t _sector) {
        static ERaOTAStorage _instance(_sector);
        return _instance;
    }

protected:
private:
    __attribute__ ((long_call, noinline, section (".RamFunc*")))
    static void copyToFlash(uint32_t flashOffs, const uint8_t *data, uint32_t count, uint8_t reset) {
        uint32_t pageAddress = flashOffs;
        while (FLASH->SR & FLASH_SR_BSY);

#if defined(FLASH_CR_PSIZE)
        CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
        FLASH->CR |= 0x00000200U; // FLASH_PSIZE_WORD;
        // FLASH->CR |= FLASH_CR_PG;
#endif
#if defined(FLASH_PAGE_SIZE)
        SET_BIT(FLASH->CR, FLASH_CR_PER);
        for (uint16_t i = 0; i < (count / FLASH_PAGE_SIZE); ++i) {
            WRITE_REG(FLASH->AR, pageAddress);
            SET_BIT(FLASH->CR, FLASH_CR_STRT);
            pageAddress += FLASH_PAGE_SIZE;
            while (FLASH->SR & FLASH_SR_BSY);
        }
        CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
#else
        uint8_t startSector = ((flashOffs == FLASH_BASE) ? 0 : 1);
        uint8_t endSector = (1 + startSector + ((count - 1) / SMALL_SECTOR_SIZE));
        if (endSector > 4) {
            if (endSector < 8) {
                endSector = 5;
            }
            else {
                endSector = (5 + (((count - 1) + (startSector * SMALL_SECTOR_SIZE)) / LARGE_SECTOR_SIZE));
            }
        }
        for (uint8_t sector = startSector; sector < endSector; sector++) {
            CLEAR_BIT(FLASH->CR, FLASH_CR_SNB);
            FLASH->CR |= (FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos));
            FLASH->CR |= FLASH_CR_STRT;
            while (FLASH->SR & FLASH_SR_BSY);
        }
        CLEAR_BIT(FLASH->CR, (FLASH_CR_SER | FLASH_CR_SNB));
#endif

        pageAddress = flashOffs;
        uint16_t* ptr = (uint16_t*)data;
        while (FLASH->SR & FLASH_SR_BSY);
#if defined(FLASH_CR_PSIZE)
        CLEAR_BIT(FLASH->CR, FLASH_CR_PSIZE);
        FLASH->CR |= 0x00000100U; // FLASH_PSIZE_HALF_WORD
        FLASH->CR |= FLASH_CR_PG;
#else
        SET_BIT(FLASH->CR, FLASH_CR_PG);
#endif
        while (count) {
            *(volatile uint16_t*)pageAddress = *ptr;
            pageAddress += 2;
            count -= 2;
            ptr++;
            while (FLASH->SR & FLASH_SR_BSY);
        }
        CLEAR_BIT(FLASH->CR, FLASH_CR_PG);

        if (reset) {
            for (;;) { __NOP(); }
            NVIC_SystemReset();
        }
    }

    union {
        uint32_t data32;
        uint8_t data8[4];
    } storageData;

    uint8_t sector;
    uint32_t maxSketchSize;
    uint32_t storageStartAddress;
    uint32_t pageAlignedLength;
    uint8_t writeIndex;
    uint32_t flashWriteAddress;
};

#define InternalStorage ERaOTAStorage::instance(ERA_OTA_STM32_SECTOR)

#endif /* INC_ERA_OTA_STORAGE_STM32_HPP_ */
