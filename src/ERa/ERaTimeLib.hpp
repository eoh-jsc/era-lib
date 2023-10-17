#ifndef INC_ERA_TIME_LIBRARY_HPP_
#define INC_ERA_TIME_LIBRARY_HPP_

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

public:
    ERaTime()
        : sysTime(0L)
        , prevMillis(0L)
    {}
    virtual ~ERaTime()
    {}

    virtual void begin() = 0;
    virtual void run() = 0;

    time_t now() {
        this->run();

        unsigned long currentMillis = ERaMillis();
        if ((currentMillis - this->prevMillis) < 1000L) {
            return this->sysTime;
        }

        unsigned long skipTimes = (currentMillis - this->prevMillis) / 1000L;
        this->prevMillis += (1000L * skipTimes);
        this->sysTime += skipTimes;

        return this->sysTime;
    }

    void setTime(time_t _time) {
        this->sysTime = _time;
        this->prevMillis = ERaMillis();
    }

    void setTime(int hour, int min, int secs, int day, int month, int year) {
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
        this->setTime(this->makeTime());
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

protected:
    time_t makeTime() {
        time_t seconds {0};

        seconds = this->time.year * (SECS_PER_DAY * 365);
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

    time_t sysTime;
    unsigned long prevMillis;
    TimeElement_t time;
    const uint8_t monthDays[12] {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
};

#endif /* INC_ERA_TIME_LIBRARY_HPP_ */
