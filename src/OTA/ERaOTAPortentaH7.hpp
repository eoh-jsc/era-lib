#ifndef INC_ERA_OTA_PORTENTA_H7_HPP_
#define INC_ERA_OTA_PORTENTA_H7_HPP_

#include <Arduino_Portenta_OTA.h>
#include <Utility/ERaUtility.hpp>
#include <OTA/ERaOTAHelper.hpp>

#if !defined(ERA_OTA_BUFFER_SIZE)
    #define ERA_OTA_BUFFER_SIZE     256
#endif

template <class Proto, class Flash>
class ERaOTA
    : public ERaOTAHelper
{
    const char* TAG = "OTA";

public:
    ERaOTA(Flash& _flash)
        : flash(_flash)
    {}
    ~ERaOTA()
    {}

    void begin(const char* url = nullptr,
                const char* hash = nullptr,
                const char* type = nullptr,
                size_t downSize = ERA_OTA_BUFFER_SIZE,
                cJSON* root = nullptr) {
        if (url == nullptr) {
            url = ERaOTAHelper::createUrl(this->thisProto().getAuth());
        }

        ERaWatchdogFeed();

        this->thisProto().getTransp().disconnect();
        ERA_LOG(TAG, ERA_PSTR("Firmware update URL: %s"), url);

        Arduino_Portenta_OTA::Error err = Arduino_Portenta_OTA::Error::None;
        Arduino_Portenta_OTA_QSPI qspiOTA(QSPI_FLASH_FATFS_MBR, 2);

        err = qspiOTA.begin();
        if (err != Arduino_Portenta_OTA::Error::None) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Update begin failed"));
            return;
        }

        ERaWatchdogFeed();

        this->flash.end();

        remove("/fs/UPDATE.BIN");
        remove("/fs/UPDATE.BIN.LZSS");

        ERaWatchdogFeed();

#if defined(ERA_OTA_VIA_ETHERNET)
        MbedSocketClass* otaSocket = static_cast<MbedSocketClass*>(&Ethernet);
#else
        MbedSocketClass* otaSocket = static_cast<MbedSocketClass*>(&WiFi);
#endif
#if defined(ERA_OTA_SSL)
        int retCode = qspiOTA.download(url, true, otaSocket);
#else
        int retCode = qspiOTA.download(url, false, otaSocket);
#endif
        if (retCode < 0) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Download file failed"));
            return;
        }

        ERaWatchdogFeed();

        retCode = qspiOTA.decompress();
        if (retCode < 0) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Decompress file failed"));
            return;
        }

        ERaWatchdogFeed();

        err = qspiOTA.update();
        if (err != Arduino_Portenta_OTA::Error::None) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Update failed"));
            return;
        }

        ERA_LOG(TAG, ERA_PSTR("Update successfully. Rebooting!"));
        ERaDelay(1000);
        ERaRestart(true);
        ERA_FORCE_UNUSED(hash);
        ERA_FORCE_UNUSED(type);
        ERA_FORCE_UNUSED(downSize);
        ERA_FORCE_UNUSED(root);
    }

protected:
private:
    inline
    const Proto& thisProto() const {
        return static_cast<const Proto&>(*this);
    }

    inline
    Proto& thisProto() {
        return static_cast<Proto&>(*this);
    }

    Flash& flash;
};

#endif /* INC_ERA_OTA_PORTENTA_H7_HPP_ */
