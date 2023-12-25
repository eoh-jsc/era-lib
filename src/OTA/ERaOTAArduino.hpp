#ifndef INC_ERA_OTA_ARDUINO_HPP_
#define INC_ERA_OTA_ARDUINO_HPP_

#include <ArduinoHttpClient.h>
#include <OTA/ERaOTAStorage.hpp>
#include <Utility/MD5.hpp>
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
        String domain;
        Client* client = this->thisProto().getTransp().getClient();
        if (client == nullptr) {
            return;
        }
        domain = this->getDomain(url);
        HttpClient http(*client, domain, this->getPort(url));

        http.get(url);

        ERaWatchdogFeed();

        httpCode = http.responseStatusCode();
        if (httpCode != 200) {
            http.stop();
            ERA_LOG_ERROR(TAG, ERA_PSTR("HTTP code wrong, should be 200"));
            return;
        }
        contentLength = http.contentLength();
        if (contentLength == HttpClient::kNoContentLengthHeader) {
            http.stop();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Content-Length not defined"));
            return;
        }

        ERaWatchdogFeed();

        if ((this->pHandler != nullptr) &&
            this->pHandler->begin(client, contentLength, hash, type, downSize)) {
            return http.stop();
        }

        if (hash != nullptr) {
            ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), hash);
        }

        this->flash.end();

        ERaWatchdogFeed();

        if (!InternalStorage.open(contentLength)) {
            http.stop();
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("Not enough space to begin OTA"));
            return;
        }

        ERaWatchdogFeed();

        MD5 md5;
        md5.begin();

        int written {0};
        int prevPercentage {0};
        uint8_t buf[ERA_OTA_BUFFER_SIZE] {0};
        downSize = ERaMin(downSize, (size_t)ERA_OTA_BUFFER_SIZE);
        while (client->connected() && (written < contentLength)) {
            int len = http.readBytes(buf, downSize);
            if (len <= 0) {
                continue;
            }

            md5.update(buf, len);
            bool writeFailed = false;
            for (int i = 0; i < len; ++i) {
                if (!InternalStorage.write(buf[i])) {
                    writeFailed = true;
                    break;
                }
            }
            if (writeFailed) {
                break;
            }
            written += len;

            ERaWatchdogFeed();

            const int percentage = (written * 100) / contentLength;
            if ((percentage == 100) ||
                (percentage - prevPercentage >= 10)) {
                prevPercentage = percentage;
                ERA_LOG(TAG, ERA_PSTR("Updating %d%%"), percentage);
            }
        }

        InternalStorage.close();
        http.stop();

        ERaWatchdogFeed();

        if (written != contentLength) {
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("OTA written %d/%d bytes"), written, contentLength);
            return;
        }

        /* Verify md5 */
        const char* md5Local = md5.finalize();
        if (hash != nullptr) {
            if (!strcmp(md5Local, hash)) {
                ERA_LOG(TAG, ERA_PSTR("Verify match: %s"), hash);
            }
            else {
                ERA_LOG_ERROR(TAG, ERA_PSTR("No MD5 match: Local = %s, Target = %s"), md5Local, hash);
                return;
            }
        }

        ERA_LOG(TAG, ERA_PSTR("Update successfully. Rebooting!"));
        ERaDelay(1000);
        InternalStorage.apply();
        ERaDelay(1000);
        ERaRestart(true);
        ERA_FORCE_UNUSED(hash);
        ERA_FORCE_UNUSED(root);
    }

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
    ERaOTAHandler* pHandler;
};

#endif /* INC_ERA_OTA_ARDUINO_HPP_ */
