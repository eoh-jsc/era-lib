#ifndef INC_ERA_ESP_TIME_HPP_
#define INC_ERA_ESP_TIME_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>
#include <ERa/ERaTimeLib.hpp>
#include <Utility/ERaUtility.hpp>

#define ERA_SNTP_TIMEZONE   7L
#define ERA_SNTP_SERVER1    "vn.pool.ntp.org"
#define ERA_SNTP_SERVER2    "asia.pool.ntp.org"
#define ERA_SNTP_SERVER3    "time.nist.gov"

class ERaEspTime
    : public ERaTime
{
public:
    ERaEspTime()
        : initialized(false)
        , timeZone(ERA_SNTP_TIMEZONE)
        , server1(ERA_SNTP_SERVER1)
        , server2(ERA_SNTP_SERVER2)
        , server3(ERA_SNTP_SERVER3)
    {}
    virtual ~ERaEspTime()
    {}

    void setTimeZone(long tz = 7L) {
        this->timeZone = tz;
    }

    void config(long tz, const char* sv1, const char* sv2 = NULL,
                                        const char* sv3 = NULL) {
        this->timeZone = tz;
        this->server1 = sv1;
        this->server2 = sv2;
        this->server3 = sv3;
    }

    void begin() override {
        if (this->initialized) {
            return;
        }
        this->initialized = true;
        ERaTime::setGetTimeCallback([&, this](void) -> time_t { return this->getTimeRTC(); });
        ERaTime::setSetTimeCallback([&, this](time_t time) { this->setTimeRTC(time); });
        configTime(0, 0, this->server1, this->server2, this->server3);
    }

    void run() override {
    }

    time_t getTimeRTC() {
        ::time_t now;
        ::time(&now);
        now += (this->timeZone * SECS_PER_HOUR);
        return (time_t)now;
    }

    void setTimeRTC(time_t time) {
        const timeval epoch = {(::time_t)time, 0};
        settimeofday(&epoch, 0);
    }

protected:
private:
    bool initialized;
    long timeZone;
    const char* server1;
    const char* server2;
    const char* server3;
};

#endif /* INC_ERA_ESP_TIME_HPP_ */
