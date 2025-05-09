#ifndef INC_ERA_TIME_LIBRARY_HPP_
#define INC_ERA_TIME_LIBRARY_HPP_

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <ERa/ERaDetect.hpp>
#include <Utility/ERaUtility.hpp>

#define SECS_PER_MIN            ((unsigned long)(60UL))
#define SECS_PER_HOUR           ((unsigned long)(3600UL))
#define SECS_PER_DAY            ((unsigned long)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK           ((unsigned long)(7UL))
#define SECS_PER_WEEK           ((unsigned long)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR           ((unsigned long)(SECS_PER_DAY * 365UL))
#define SECS_YR_2000            ((unsigned long)(946684800UL))
#define SECS_YR_2023            ((unsigned long)(1672531200UL))
#define SECS_YR_2000_TZ7        ((unsigned long)(946710000UL)) // Offset timezone +7
#define SECS_YR_2023_TZ7        ((unsigned long)(1672556400UL)) // Offset timezone +7
#define OFFSET_YEAR             ((unsigned long)(1970UL))
#define OFFSET_TAI_UTC          ((unsigned long)(37UL))
#define OFFSET_TAI_UNIX         ((unsigned long)(378777600UL)) // Offset 1958-01-01T00:00:00

#define DEFAULT_TIMEZONE        ((long)(7L))

typedef struct  {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t wDay;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} TimeElement_t;

class ERaTime
{
protected:
    typedef unsigned long time_t;

#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(time_t)> SetTimeCallback_t;
    typedef std::function<time_t(void)> GetTimeCallback_t;
#else
    typedef void (*SetTimeCallback_t)(time_t);
    typedef time_t (*GetTimeCallback_t)(void);
#endif

public:
    ERaTime()
        : sysTime(0L)
        , timeZone(DEFAULT_TIMEZONE)
        , prevMillis(0L)
        , setTimeCb(NULL)
        , getTimeCb(NULL)
    {}
    virtual ~ERaTime()
    {}

    virtual void begin() {}
    virtual void run() {}
    virtual void sync() {}

    virtual void setTimeZone(long tz = DEFAULT_TIMEZONE) {
        this->timeZone = tz;
    }

    virtual long getTimeZone() {
        return this->timeZone;
    }

    long getTimeZoneOffset() {
        return (this->timeZone * SECS_PER_HOUR);
    }

    void setSetTimeCallback(SetTimeCallback_t cb, bool override = false) {
        if (override) {
        }
        else if (this->setTimeCb != NULL) {
            return;
        }
        this->setTimeCb = cb;
    }

    void setGetTimeCallback(GetTimeCallback_t cb, bool override = false) {
        if (override) {
        }
        else if (this->getTimeCb != NULL) {
            return;
        }
        this->getTimeCb = cb;
    }

    time_t now() {
        this->run();

        unsigned long currentMillis = ERaMillis();
        if ((currentMillis - this->prevMillis) < 1000L) {
            return this->getSysTime(false);
        }

        unsigned long skipTimes = (currentMillis - this->prevMillis) / 1000L;
        this->prevMillis += (1000L * skipTimes);
        this->sysTime += skipTimes;

        return this->getSysTime(true);
    }

    void setTime(time_t _time, long offset = 0L) {
        _time += offset;
        if (this->setTimeCb != NULL) {
            this->setTimeCb(_time);
        }
        this->sysTime = _time;
        this->prevMillis = ERaMillis();
    }

    void setTime(int hour, int min, int secs, int day, int month, int year, long offset = 0L) {
        if (year > 99) {
            year = year - 1970;
        }
        else {
            year = year + 30;
        }

        this->time.year = year;
        this->time.month = month;
        this->time.day = day;
        this->time.hour = hour;
        this->time.minute = min;
        this->time.second = secs;
        this->setTime(this->makeTime(), offset);
    }

    void getTime(TimeElement_t& tm) {
        uint8_t year {0};
        unsigned long days {0};
        time_t timestamp = this->now();

        tm.second = timestamp % 60;
        timestamp /= 60;
        tm.minute = timestamp % 60;
        timestamp /= 60;
        tm.hour = timestamp % 24;
        timestamp /= 24;
        tm.wDay = ((timestamp + 4) % 7) + 1;

        while ((days += (this->leapYear(year) ? 366 : 365)) <= timestamp) {
            ++year;
        }
        tm.year = year; /* year is offset from 1970 */

        days -= (this->leapYear(year) ? 366 : 365);
        timestamp -= days;

        days = 0;
        uint8_t month {0};
        uint8_t monthLen {0};
        for (month = 0; month < 12; ++month) {
            if (month == 1) {
                if (this->leapYear(year)) {
                    monthLen = 29;
                }
                else {
                    monthLen = 28;
                }
            }
            else {
                monthLen = this->monthDays[month];
            }

            if (timestamp >= monthLen) {
                timestamp -= monthLen;
            }
            else {
                break;
            }
        }
        tm.month = month + 1;
        tm.day = timestamp + 1;
    }

    static inline
    time_t getTimeFromStringNumber(const char* input, long timeZone = 0L) {
        if (input == NULL) {
            return 0;
        }

        struct tm t = {
            0 /* tm_sec   */,
            0 /* tm_min   */,
            0 /* tm_hour  */,
            0 /* tm_mday  */,
            0 /* tm_mon   */,
            0 /* tm_year  */,
            0 /* tm_wday  */,
            0 /* tm_yday  */,
            0 /* tm_isdst */
        };

        int month {0};
        int day   {0};
        int year  {0};
        int hour  {0};
        int min   {0};
        int sec   {0};
        static const char* TAG = "Time";
        static const int expectedLength = 19;
        static const int expectedParameters = 6;
        const int inputLength = strlen(input);

        if (inputLength != expectedLength) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Invalid input length: %d"), inputLength);
            return 0;
        }

        int scannedParameters = sscanf(input, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);

        if (scannedParameters != expectedParameters) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Invalid input parameters number: %d"), scannedParameters);
            return 0;
        }

        if ((month < 1) || (month > 12) || (day < 1) || (day > 31) || (year < 1900) ||
            (hour  < 0) || (hour  > 23) || (min < 0) || (min > 59) || (sec  < 0) || (sec > 59)) {
            ERA_LOG_ERROR(TAG, ERA_PSTR("Invalid date values"));
            return 0;
        }

        t.tm_mon  = (month - 1);
        t.tm_mday = day;
        t.tm_year = (year - 1900);
        t.tm_hour = hour;
        t.tm_min  = min;
        t.tm_sec  = sec;
        t.tm_isdst = -1;

        ERA_FORCE_UNUSED(TAG);

        long offset = (timeZone * SECS_PER_HOUR);

        return (mktime(&t) + offset);
    }

    static inline
    time_t getTimeCompile(long timeZone = 0L) {
        static const char* input = __DATE__ " " __TIME__;
        static time_t buildTime {0};

        if (buildTime) {
            return buildTime;
        }

        struct tm t = {
            0 /* tm_sec   */,
            0 /* tm_min   */,
            0 /* tm_hour  */,
            0 /* tm_mday  */,
            0 /* tm_mon   */,
            0 /* tm_year  */,
            0 /* tm_wday  */,
            0 /* tm_yday  */,
            0 /* tm_isdst */
        };

        char sMonth[16];
        int month {0};
        int day   {0};
        int year  {0};
        int hour  {0};
        int min   {0};
        int sec   {0};
        static const char monthNames[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

        sscanf(input, "%s %d %d %d:%d:%d", sMonth, &day, &year, &hour, &min, &sec);

        month = ((strstr(monthNames, sMonth) - monthNames) / 3);

        t.tm_mon  = month;
        t.tm_mday = day;
        t.tm_year = (year - 1900);
        t.tm_hour = hour;
        t.tm_min  = min;
        t.tm_sec  = sec;
        t.tm_isdst = -1;

        long offset = (timeZone * SECS_PER_HOUR);

        buildTime = (mktime(&t) + offset);
        return buildTime;
    }

protected:
    time_t makeTime() {
        time_t seconds {0};

        seconds = (this->time.year * (SECS_PER_DAY * 365));
        for (int i = 0; i < this->time.year; ++i) {
            if (this->leapYear(i)) {
                seconds += SECS_PER_DAY;
            }
        }

        for (int i = 1; i < this->time.month; ++i) {
            if (i == 2 && this->leapYear(this->time.year)) {
                seconds += (SECS_PER_DAY * 29);
            }
            else {
                seconds += (SECS_PER_DAY * this->monthDays[i - 1]);
            }
        }

        seconds += ((this->time.day - 1) * SECS_PER_DAY);
        seconds += (this->time.hour * SECS_PER_HOUR);
        seconds += (this->time.minute * SECS_PER_MIN);
        seconds += this->time.second;

        return seconds;
    }

    bool leapYear(int year) {
        return (((1970 + (year)) > 0) && !((1970 + (year)) % 4) &&
                (((1970 + (year)) % 100) || !((1970 + (year)) % 400)));
    }

    time_t getSysTime(bool call) {
        if (!call) {
        }
        else if (this->getTimeCb != NULL) {
            this->sysTime = this->getTimeCb();
        }
        return (this->sysTime + (this->timeZone * SECS_PER_HOUR));
    }

    time_t sysTime;
    long timeZone;
    unsigned long prevMillis;
    TimeElement_t time;
    const uint8_t monthDays[12] {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    SetTimeCallback_t setTimeCb;
    GetTimeCallback_t getTimeCb;
};

#endif /* INC_ERA_TIME_LIBRARY_HPP_ */
