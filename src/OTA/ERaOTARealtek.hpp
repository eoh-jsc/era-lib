#ifndef INC_ERA_OTA_REALTEK_HPP_
#define INC_ERA_OTA_REALTEK_HPP_

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

        this->flash.end();

        ERaWatchdogFeed();

        int ret {-1};
        int port = this->getPort(url);
        if (port == ERA_DEFAULT_PORT) {
#if defined(HTTP_OTA_UPDATE)
            ret = http_update_ota((char*)this->getDomain(url).c_str(),
                                    this->getPort(url), (char*)url);
#endif
        }
        else {
#if defined(HTTPS_OTA_UPDATE)
            ret = https_update_ota((char*)this->getDomain(url).c_str(),
                                    this->getPort(url), (char*)url);
#endif
        }
        if (ret != 0) {
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

#endif /* INC_ERA_OTA_REALTEK_HPP_ */
