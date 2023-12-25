#ifndef INC_ERA_OTA_ESP8266_HPP_
#define INC_ERA_OTA_ESP8266_HPP_

#if defined(ESP32)
    #include <Update.h>
#elif defined(ESP8266)
    #include <ESP8266WiFi.h>
#endif

#include <Utility/ERaUtility.hpp>
#include <OTA/ERaOTAHelper.hpp>
#include <OTA/ERaOTAHandler.hpp>

#if !defined(ERA_OTA_BUFFER_SIZE)
    #define ERA_OTA_BUFFER_SIZE     256
#endif

#if !defined(OTA_DOWNLOAD_TIMEOUT)
    #define OTA_DOWNLOAD_TIMEOUT    (5 * 60000)
#endif

#define OTA_RESPONSE_TIMEOUT        10000UL

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

        int contentLength {0};
        String md5Hash;
        Client* client = this->thisProto().getTransp().getClient();
        if (client == nullptr) {
            return;
        }

        client->connect(this->getDomain(url).c_str(), this->getPort(url));

        ERaWatchdogFeed();

        client->print("GET ");
        this->printURL(client, url);
        client->print(" HTTP/1.1\r\n");
        client->print("Host: ");
        client->print(this->getDomain(url).c_str());
        client->print("\r\n");
        client->print("Connection: keep-alive\r\n\r\n");

        MillisTime_t timeout = ERaMillis();
        while (client->connected() && !client->available()) {
            ERaDelay(10);
            if (!ERaRemainingTime(timeout, OTA_RESPONSE_TIMEOUT)) {
                ERA_LOG_ERROR(TAG, ERA_PSTR("Response timeout"));
                return;
            }
        }

        ERaWatchdogFeed();

        while (client->available()) {
            String line = client->readStringUntil('\n');
            line.trim();
            line.toLowerCase();
            if (line.startsWith("content-length:")) {
                contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
            }
            else if (line.startsWith("x-md5:")) {
                md5Hash = line.substring(line.lastIndexOf(':') + 1);
            }
            else if (!line.length()) {
                break;
            }
            ERaDelay(10);
        }

        if (contentLength <= 0) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Content-Length not defined"));
            return;
        }

        ERaWatchdogFeed();

        if ((this->pHandler != nullptr) &&
            this->pHandler->begin(client, contentLength, hash, type, downSize)) {
            return client->stop();
        }

        ERaWatchdogFeed();

        if (!Update.begin(contentLength)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Not enough space to begin OTA"));
            return;
        }

        ERaWatchdogFeed();

        if (md5Hash.length() == 32) {
            md5Hash.toLowerCase();
            ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), md5Hash.c_str());
            Update.setMD5(md5Hash.c_str());
        }
        else if (hash != nullptr) {
            md5Hash = hash;
            md5Hash.toLowerCase();
            ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), md5Hash.c_str());
            Update.setMD5(md5Hash.c_str());
        }

        this->flash.end();

        int written {0};
        int prevPercentage {0};
        uint8_t buf[ERA_OTA_BUFFER_SIZE] {0};
        MillisTime_t startMillis = ERaMillis();
        downSize = ERaMin(downSize, (size_t)ERA_OTA_BUFFER_SIZE);
        while (client->connected() && (written < contentLength)) {
            ERaDelay(10);
            timeout = ERaMillis();
            while (client->connected() && !client->available()) {
                ERaDelay(1);
                if (!ERaRemainingTime(timeout, OTA_RESPONSE_TIMEOUT)) {
                    ERA_LOG_ERROR(TAG, ERA_PSTR("Response timeout"));
                    break;
                }
            }
            if (!ERaRemainingTime(startMillis, OTA_DOWNLOAD_TIMEOUT)) {
                ERA_LOG_ERROR(TAG, ERA_PSTR("Download timeout"));
                break;
            }

            int len = client->read(buf, downSize);
            if (len <= 0) {
                continue;
            }

            Update.write(buf, len);
            written += len;

            ERaWatchdogFeed();

            const int percentage = (written * 100) / contentLength;
            if ((percentage == 100) ||
                (percentage - prevPercentage >= 10)) {
                prevPercentage = percentage;
                ERA_LOG(TAG, ERA_PSTR("Updating %d%%"), percentage);
            }
        }

        client->stop();

        ERaWatchdogFeed();

        if (written != contentLength) {
            Update.end();
            this->flash.begin();
            ERA_LOG_ERROR(TAG, ERA_PSTR("OTA written %d/%d bytes"), written, contentLength);
            return;
        }

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
    void printURL(Client* client, const char* url) {
        size_t sent {0};
        size_t toSend {0};
        size_t sendSize = strlen(url);
        char buf[257] {0};
        while (sendSize) {
            toSend = ((sendSize > 256) ? 256 : sendSize);
            strncpy(buf, url + sent, toSend);
            buf[toSend] = 0;
            client->print(buf);
            sendSize -= toSend;
            sent += toSend;
        }
    }

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

#endif /* INC_ERA_OTA_ESP8266_HPP_ */
