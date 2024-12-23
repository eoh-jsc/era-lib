#ifndef INC_ERA_SYNC_TIME_LINUX_HPP_
#define INC_ERA_SYNC_TIME_LINUX_HPP_

#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>
#include <ERa/ERaTimeLib.hpp>
#include <Utility/ERaUtility.hpp>

class ERaSyncTimeLinux
    : public ERaTime
{
public:
    ERaSyncTimeLinux()
        : timeZone(7L)
        , syncInterval(300000L)
        , prevMillis(0L)
    {}
    virtual ~ERaSyncTimeLinux()
    {}

    void setTimeZone(long tz = 7L) override {
        this->timeZone = tz;
    }

    long getTimeZone() override {
        return this->timeZone;
    }

    void begin() override {
        this->getLocalTime();
    }

    void run() override {
        unsigned long currentMillis = ERaMillis();
        if (currentMillis - this->prevMillis < this->syncInterval) {
            return;
        }
        unsigned long skipTimes = (currentMillis - this->prevMillis) / this->syncInterval;
        this->prevMillis += this->syncInterval * skipTimes;
        this->getLocalTime();
    }

    static inline
    time_t getTimeRTC() {
        return ::time(NULL);
    }

    static inline
    void setTimeRTC(time_t time) {
        const timeval epoch = {(::time_t)time, 0};
        settimeofday(&epoch, 0);
    }

private:
    void getLocalTime() {
        struct timeval now;
        unsigned long timestamp {0};

        gettimeofday(&now, NULL);
        timestamp = now.tv_sec;
        timestamp += (this->timeZone * SECS_PER_HOUR);
        this->setTime(timestamp);
    }

    long timeZone;
    unsigned long syncInterval;
    unsigned long prevMillis;
};

typedef ERaSyncTimeLinux ERaSyncTime;

#endif /* INC_ERA_SYNC_TIME_LINUX_HPP_ */
