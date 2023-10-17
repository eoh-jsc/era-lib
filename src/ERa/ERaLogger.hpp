#ifndef INC_ERA_LOGGER_HPP_
#define INC_ERA_LOGGER_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>
#include <ERa/ERaTimeLib.hpp>
#include <ERa/ERaLoggerDef.hpp>
#include <Utility/ERacJSON.hpp>

class ERaLogger
{
public:
    ERaLogger(ERaTime& _time)
        : time(_time)
        , dirLogger(DIRECTORY_LOGGER)
        , logInterval(LOGGER_LOG_INTERVAL)
        , prevMillis(0)
    {}
    virtual ~ERaLogger()
    {}

    virtual void begin() = 0;
    virtual void run() = 0;
    virtual void put(const char* subPath, const char* id,
                    const char* data, bool status, bool force = false) = 0;
    virtual char* get(const char* subPath, const char* id) = 0;

    void put(const char* subPath, const char* id,
            const cJSON* const root, bool status, bool force = false) {
        if (root == nullptr) {
            return;
        }
        char* buf = cJSON_PrintUnformatted(root);
        if (buf == nullptr) {
            return;
        }
        this->put(subPath, id, buf, status, force);
        free(buf);
        buf = nullptr;
    }

protected:
    bool isExpireTime(bool skip = false) {
        if (skip) {
            return true;
        }
        unsigned long currentMillis = ERaMillis();
        if (currentMillis - this->prevMillis < this->logInterval) {
            return false;
        }
        unsigned long skipTimes = (currentMillis - this->prevMillis) / this->logInterval;
        this->prevMillis += this->logInterval * skipTimes;
        return true;
    }

    void executeNow() {
        this->prevMillis = ERaMillis() - this->logInterval;
    }

    ERaTime& time;
    const char* dirLogger;
    unsigned long logInterval;
    unsigned long prevMillis;
};

#endif /* INC_ERA_LOGGER_HPP_ */
