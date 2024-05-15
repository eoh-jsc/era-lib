#ifndef INC_ERA_OTA_LINUX_HPP_
#define INC_ERA_OTA_LINUX_HPP_

#include <Utility/MD5.hpp>
#include <Utility/ERaUtility.hpp>
#include <OTA/ERaOTAHelper.hpp>
#include <OTA/ERaOTAHandler.hpp>

#if defined(ERA_OTA_SSL)
    #include <Network/ERaSocketSecureLinux.hpp>
#endif

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
                const cJSON* info = nullptr) {
        if ((url == nullptr) || !strlen(url)) {
            url = ERaOTAHelper::createUrl(this->thisProto().getAuth());
        }

        this->thisProto().getTransp().disconnect();
        ERA_LOG(TAG, ERA_PSTR("Firmware update URL: %s"), url);

        int contentLength {0};
        Client* client = nullptr;
        if (this->getPort(url) == ERA_DEFAULT_PORT) {
            client = new ERaSocket();
        }
        else {
#if defined(ERA_OTA_SSL)
            client = new ERaSocketSecure();
#else
            return;
#endif
        }
        if (client == nullptr) {
            return;
        }

        client->connect(this->getDomain(url).c_str(), this->getPort(url));

        client->print("GET ");
        ERaOTAHelper::printURL(client, url);
        client->print(" HTTP/1.1\r\n");
        client->print("Host: ");
        client->print(this->getDomain(url).c_str());
        client->print("\r\n");
        client->print("Connection: keep-alive\r\n\r\n");

        MillisTime_t timeout = ERaMillis();
        while (client->connected() && !client->available()) {
            ERaDelay(10);
            if (!ERaRemainingTime(timeout, OTA_RESPONSE_TIMEOUT)) {
                client->stop();
                delete client;
                ERA_LOG_ERROR(TAG, ERA_PSTR("Response timeout"));
                return;
            }
        }

        while (client->available()) {
            size_t pos {0};
            char line[ERA_OTA_BUFFER_SIZE] {0};

            do {
                char c = client->read();
                if (c < 0) {
                    continue;
                }
                else if ((c == '\n') ||
                        (pos == sizeof(line))) {
                    break;
                }
                line[pos++] = c;
            } while (client->available());

            char* pData = ERaFindStr(line, "Content-Length:");
            if (pData != nullptr) {
                pData += 15;
                contentLength = atoi(pData);
            }
            else if (!strlen(line) || (pos == 1)) {
                break;
            }
            ERaDelay(10);
        }

        if (contentLength <= 0) {
            client->stop();
            delete client;
            ERA_LOG_ERROR(TAG, ERA_PSTR("Content-Length not defined"));
            return;
        }

        if ((this->pHandler != nullptr) &&
            this->pHandler->begin(client, contentLength, hash, type, downSize, info)) {
            client->stop();
            delete client;
            return;
        }

        if (hash != nullptr) {
            ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), hash);
        }

        const char path[] = "era.bin";
        int fd = ::open(path, O_WRONLY | O_CREAT | O_TRUNC, 0755);
        if (fd < 0) {
            client->stop();
            delete client;
            return;
        }
        ::close(fd);
        FILE* file = fopen(path, "wb");
        if (file == nullptr) {
            client->stop();
            delete client;
            return;
        }

        MD5 md5;
        md5.begin();

        int written {0};
        int prevPercentage {0};
        uint8_t buf[ERA_OTA_BUFFER_SIZE] {0};
        MillisTime_t startMillis = ERaMillis();
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

            int len = client->read(buf, sizeof(buf));
            if (len <= 0) {
                continue;
            }

            md5.update(buf, len);
            fwrite(buf, 1, len, file);
            written += len;

            const int percentage = (written * 100) / contentLength;
            if ((percentage == 100) ||
                (percentage - prevPercentage >= 10)) {
                prevPercentage = percentage;
                ERA_LOG(TAG, ERA_PSTR("Updating %d%%"), percentage);
            }
        }

        fclose(file);
        client->stop();
        delete client;
        client = nullptr;

        if (written != contentLength) {
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

        ::remove("era");
        if (::rename(path, "era")) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Update failed"));
            return;
        }

        ERA_LOG(TAG, ERA_PSTR("Update successfully. Rebooting!"));
        ERaDelay(1000);
        ERaRestart(true);
        ERA_FORCE_UNUSED(info);
    }

private:
    void storeOTAConfig(const char* url,
                        const char* hash) {
        if ((url == nullptr) || !strlen(url)) {
            return;
        }

        char* otaConfig = nullptr;
        size_t sizeConfig = (strlen(url) + 50);
        if (hash != nullptr) {
            sizeConfig += strlen(hash);
        }

        otaConfig = (char*)ERA_MALLOC(sizeConfig + 1);
        if (otaConfig == nullptr) {
            return;
        }

        if (hash != nullptr) {
            snprintf(otaConfig, sizeConfig, R"json({"url":"%s","hash":"%s"})json", url, hash);
        }
        else {
            snprintf(otaConfig, sizeConfig, R"json({"url":"%s"})json", url);
        }
        this->thisProto().Proto::Base::writeToFlash("ota.txt", otaConfig);
        free(otaConfig);
        otaConfig = nullptr;
        exit(27);
    }

    ERaString getDomain(const char* url) {
        char domain[50] {0};
        char* start = (ERaFindStr(url, ":") + 3);
        char* end = ERaFindStr(start, "/");
        strncpy(domain, start, (end - start));
        return domain;
    }

    uint16_t getPort(const char* url) {
#if defined(ERA_OTA_SSL)
        if (!strncmp(url, "https", strlen("https"))) {
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

#endif /* INC_ERA_OTA_LINUX_HPP_ */
