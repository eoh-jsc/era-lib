#ifndef INC_ERA_OTA_BASE_HPP_
#define INC_ERA_OTA_BASE_HPP_

#include <Client.h>
#include <OTA/ERaOTAStorage.hpp>
#include <Utility/ERaUtility.hpp>

#ifndef ERA_OTA_BUFFER_SIZE
    #define ERA_OTA_BUFFER_SIZE 256
#endif

#define OTA_RESPONSE_TIMEOUT    10000

template <class Proto, class Flash>
class ERaOTA
{
    const char* TAG = "OTA";

public:
    ERaOTA(Flash& _flash)
        : flash(_flash)
    {}
    ~ERaOTA()
    {}

    void begin(const char* url = nullptr) {
        if (url == nullptr) {
            url = this->createUrl();
        }

        this->thisProto().getTransp().disconnect();
        ERA_LOG(TAG, ERA_PSTR("Firmware update URL: %s"), url);

        int contentLength {0};
        String md5;
        Client* client = this->thisProto().getTransp().getClient();
        if (client == nullptr) {
            return;
        }

        client->connect(this->getDomain(url).c_str(), this->getPort(url));
        client->printf("GET %s HTTP/1.1\r\n"
                    "Host: %s\r\n"
                    "Connection: keep-alive\r\n\r\n",
                    url, this->getDomain(url).c_str());

        unsigned long timeout = ERaMillis();
        while (client->connected() && !client->available()) {
            ERaDelay(10);
            if (!ERaRemainingTime(timeout, OTA_RESPONSE_TIMEOUT)) {
                ERA_LOG(TAG, ERA_PSTR("Response timeout"));
                return;
            }
        }

        while (client->available()) {
            String line = client->readStringUntil('\n');
            line.trim();
            line.toLowerCase();
            if (line.startsWith("content-length:")) {
                contentLength = line.substring(line.lastIndexOf(':') + 1).toInt();
            }
            else if (line.startsWith("x-md5:")) {
                md5 = line.substring(line.lastIndexOf(':') + 1);
            }
            else if (!line.length()) {
                break;
            }
            ERaDelay(10);
        }

        if (contentLength <= 0) {
            ERA_LOG(TAG, ERA_PSTR("Content-Length not defined"));
            return;
        }

        if (!InternalStorage.open(contentLength)) {
            ERA_LOG(TAG, ERA_PSTR("Not enough space to begin OTA"));
            return;
        }

        if (md5.length() == 32) {
            md5.toLowerCase();
            ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), md5.c_str());
        }

        this->flash.end();

        int written {0};
        int prevPercentage {0};
        uint8_t buf[ERA_OTA_BUFFER_SIZE] {0};
        while (client->connected() && written < contentLength) {
            ERaDelay(10);
            timeout = ERaMillis();
            while (client->connected() && !client->available()) {
                ERaDelay(1);
                if (!ERaRemainingTime(timeout, OTA_RESPONSE_TIMEOUT)) {
                    ERA_LOG(TAG, ERA_PSTR("Response timeout"));
                }
            }

            int len = client->read(buf, sizeof(buf));
            if (len <= 0) {
                continue;
            }

            for (int i = 0; i < len; ++i) {
                InternalStorage.write(buf[i]);
            }
            written += len;

            const int percentage = (written * 100) / contentLength;
            if ((percentage == 100) ||
                (percentage - prevPercentage >= 10)) {
                prevPercentage = percentage;
                ERA_LOG(TAG, ERA_PSTR("Updating %d%%"), percentage);
            }
        }

        InternalStorage.close();
        client->stop();

        if (written != contentLength) {
            this->flash.begin();
            ERA_LOG(TAG, ERA_PSTR("OTA written %d/%d bytes"), written, contentLength);
            return;
        }

        ERA_LOG(TAG, ERA_PSTR("Update successfully. Rebooting!"));
        ERaDelay(1000);
        InternalStorage.apply();
        ERaDelay(1000);
        ERaRestart(true);
    }

protected:
private:
    const char* createUrl() {
        static char url[128] {0};
        if (!strlen(url)) {
#if defined(ERA_OTA_SSL)
            FormatString(url, "https://" ERA_SSL_DOMAIN);
#else
            FormatString(url, "http://" ERA_NOSSL_DOMAIN);
#endif
            FormatString(url, "/api/chip_manager");
            FormatString(url, "/firmware?code=%s", this->thisProto().getAuth());
            FormatString(url, "&firm_version=%d.%d", ERA_FIRMWARE_MAJOR, ERA_FIRMWARE_MINOR);
            FormatString(url, "&board=%s", ERA_MODEL_TYPE);
        }
        return url;
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
        if (url.indexOf("https") < 0) {
            return ERA_NOSSL_PORT;
        }
        return ERA_SSL_PORT;
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

#endif /* INC_ERA_OTA_BASE_HPP_ */
