#ifndef INC_ERA_OTA_ESP32_HPP_
#define INC_ERA_OTA_ESP32_HPP_

#include <Update.h>
#include <HTTPClient.h>
#include <Utility/ERaUtility.hpp>

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

        int httpCode {0};
        int contentLength {0};
        const char* headerkeys[] = { "x-MD5" };
        HTTPClient http;

        Client* client = this->thisProto().getTransp().getClient();
        if (client == nullptr) {
            return;
        }

        http.begin(static_cast<WiFiClient&>(*client), url);
        http.collectHeaders(headerkeys, sizeof(headerkeys) / sizeof(char*));
        httpCode = http.GET();
        if (httpCode != HTTP_CODE_OK) {
            http.end();
            ERA_LOG(TAG, ERA_PSTR("HTTP code wrong, should be 200"));
            return;
        }
        contentLength = http.getSize();
        if (contentLength <= 0) {
            http.end();
            ERA_LOG(TAG, ERA_PSTR("Content-Length not defined"));
            return;
        }

        int prevPercentage {0};
        Update.onProgress([&prevPercentage](size_t progress, size_t size) {
            const int percentage = (progress * 100) / size;
            if ((percentage == 100) ||
                (percentage - prevPercentage >= 10)) {
                prevPercentage = percentage;
                ERA_LOG(ERA_PSTR("OTA"), ERA_PSTR("Updating %d%%"), percentage);
            }
        });

        if (!Update.begin(contentLength)) {
            http.end();
            ERA_LOG(TAG, ERA_PSTR("Not enough space to begin OTA"));
            return;
        }

        if (http.hasHeader("x-MD5")) {
            String md5 = http.header("x-MD5");
            if (md5.length() == 32) {
                md5.toLowerCase();
                ERA_LOG(TAG, ERA_PSTR("Expected MD5: %s"), md5.c_str());
                Update.setMD5(md5.c_str());
            }
        }

        this->flash.end();

        // Client& client = http.getStream();
        size_t written = Update.writeStream(*client);
        http.end();
        if (written != contentLength) {
            Update.end();
            this->flash.begin();
            ERA_LOG(TAG, ERA_PSTR("OTA written %d/%d bytes"), written, contentLength);
            return;
        }

        if (!Update.end()) {
            this->flash.begin();
            ERA_LOG(TAG, ERA_PSTR("Error #%d"), Update.getError());
            return;
        }

        if (!Update.isFinished()) {
            this->flash.begin();
            ERA_LOG(TAG, ERA_PSTR("Update failed"));
            return;
        }

        ERA_LOG(TAG, ERA_PSTR("Update successfully. Rebooting!"));
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

#endif /* INC_ERA_OTA_ESP32_HPP_ */
