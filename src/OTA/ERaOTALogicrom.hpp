#ifndef INC_ERA_OTA_LOGICROM_HPP_
#define INC_ERA_OTA_LOGICROM_HPP_

#include <fota.h>
#include <Utility/ERaUtility.hpp>
#include <OTA/ERaOTAHelper.hpp>

#if !defined(ERA_OTA_BUFFER_SIZE)
    #define ERA_OTA_BUFFER_SIZE     256
#endif

#if !defined(OTA_DOWNLOAD_TIMEOUT)
    #define OTA_DOWNLOAD_TIMEOUT    (5 * 60000)
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

        this->flash.end();

        ERaWatchdogFeed();

        fota_init();

        ERaWatchdogFeed();

        int ret = fota_start(url);
        if (ret != 0) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Update failed"));
            return;
        }

        MillisTime_t startMillis = ERaMillis();
        while (fota_in_progress() == 1) {
            ERaDelay(1000);
            ERaWatchdogFeed();
            if (!ERaRemainingTime(startMillis, OTA_DOWNLOAD_TIMEOUT)) {
                this->flash.begin();
                ERA_LOG_ERROR(TAG, ERA_PSTR("Update failed"));
                return;
            }
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
    String getDomain(String url) {
        int index = url.indexOf(':');
        if (index > 0) {
            url.remove(0, index + 3);
            index = url.indexOf('/');
            url.remove(index);
        }
        return url;
    }

    uint16_t getPort(String url) {
#if defined(ERA_OTA_SSL)
        if (url.indexOf("https") >= 0) {
            return ERA_DEFAULT_PORT_SSL;
        }
#else
        ERA_FORCE_UNUSED(url);
#endif
        return ERA_DEFAULT_PORT;
    }

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

#endif /* INC_ERA_OTA_LOGICROM_HPP_ */
