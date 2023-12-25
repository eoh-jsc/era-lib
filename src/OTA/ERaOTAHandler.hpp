#ifndef INC_ERA_OTA_HANDLER_HPP_
#define INC_ERA_OTA_HANDLER_HPP_

#include <Utility/ERaUtility.hpp>

#if !defined(ERA_OTA_BUFFER_SIZE)
    #define ERA_OTA_BUFFER_SIZE     256
#endif

class ERaOTAHandler
{
    const char* TAG = "OTA";

public:
    ERaOTAHandler()
    {}
    ~ERaOTAHandler()
    {}

    bool begin(Client* client, int contentLength, const char* hash,
                const char* type, size_t downSize = ERA_OTA_BUFFER_SIZE) {
        if (client == nullptr) {
            return false;
        }
        return this->begin(*client, contentLength, hash, type, downSize);
    }

    virtual bool begin(Client& client, int contentLength, const char* hash,
                        const char* type, size_t downSize = ERA_OTA_BUFFER_SIZE) = 0;

protected:
    uint32_t calcCRC32(const void* data, size_t length, uint32_t previousCrc32 = 0) {
        const uint32_t Polynomial = 0xEDB88320;
        const uint8_t* ptr = (const uint8_t*)data;
        uint32_t crc = ~previousCrc32;
        while (length--) {
            crc ^= *ptr++;
            for (unsigned int j = 0; j < 8; j++) {
                crc = ((crc >> 1) ^ (-int(crc & 1) & Polynomial));
            }
        }
        return ~crc;
    }
};

#endif /* INC_ERA_OTA_HANDLER_HPP_ */
