#ifndef INC_ERA_OTA_ESP32_HPP_
#define INC_ERA_OTA_ESP32_HPP_

#include <Update.h>
#include <HTTPClient.h>
#include <Utility/ERaUtility.hpp>
#include <OTA/ERaOTAHelper.hpp>
#include <OTA/ERaOTAHandler.hpp>

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
        , pHandler(nullptr)
    {}
    ~ERaOTA()
    {}

    void setOTAHandler(ERaOTAHandler* handler) {
        this->pHandler = handler;
    }

    void setOTAHandler(ERaOTAHandler& handler) {
        this->pHandler = &handler;
    }

protected:
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

        int httpCode {0};
        int contentLength {0};
        const char* headerKeys[] = { "x-MD5", "Content-Type" };
        HTTPClient http;

        Client* client = this->thisProto().getTransp().getClient();
        if (client == nullptr) {
            return;
        }

#if defined(ERA_DETECT_SSL)
        http.begin(url);
#elif defined(ERA_OTA_SSL)
        http.begin(static_cast<WiFiClient&>(*client), url);
#else
        String finalURL = url;
        int index = finalURL.indexOf(':');
        if (index > 0) {
            finalURL.remove(0, index + 3);
        }
        http.begin(static_cast<WiFiClient&>(*client), (String("http://") + finalURL));
#endif
        http.collectHeaders(headerKeys, sizeof(headerKeys) / sizeof(char*));
        httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK) {
            http.end();
            ERA_LOG_ERROR(TAG, ERA_PSTR("HTTP code wrong, should be 200"));
            return;
        }
        contentLength = http.getSize();
        if (contentLength <= 0) {
            http.end();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Content-Length not defined"));
            return;
        }

        ERaWatchdogFeed();

        if ((this->pHandler != nullptr) &&
            this->pHandler->begin(client, contentLength, hash, type, downSize)) {
            return http.end();
        }

        int prevPercentage {0};
        Update.onProgress([&prevPercentage](size_t progress, size_t size) {
            const int percentage = (progress * 100) / size;
            if ((percentage == 100) ||
                (percentage - prevPercentage >= 10)) {
                prevPercentage = percentage;
                ERaWatchdogFeed();
                ERA_LOG(ERA_PSTR("OTA"), ERA_PSTR("Updating %d%%"), percentage);
            }
        });

        ERaWatchdogFeed();

        if (!Update.begin(contentLength)) {
            http.end();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Not enough space to begin OTA"));
            return;
        }

        ERaWatchdogFeed();

        if (http.hasHeader("x-MD5")) {
            String md5Hash = http.header("x-MD5");
            if (md5Hash.length() == 32) {
                md5Hash.toLowerCase();
                ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), md5Hash.c_str());
                Update.setMD5(md5Hash.c_str());
            }
        }
        else if (hash != nullptr) {
            ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), hash);
            Update.setMD5(hash);
        }

        this->flash.end();

        Client& clientStream = http.getStream();
        size_t written = Update.writeStream(clientStream);
        http.end();
        if (written != contentLength) {
            Update.end();
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("OTA written %d/%d bytes"), written, contentLength);
            return;
        }

        ERaWatchdogFeed();

        if (!Update.end()) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Error #%d"), Update.getError());
            return;
        }

        if (!Update.isFinished()) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Update failed"));
            return;
        }

        ERA_LOG(TAG, ERA_PSTR("Update successfully. Rebooting!"));
        ERaDelay(1000);
        ERaRestart(true);
        ERA_FORCE_UNUSED(root);
    }

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
    ERaOTAHandler* pHandler;
};

#endif /* INC_ERA_OTA_ESP32_HPP_ */
