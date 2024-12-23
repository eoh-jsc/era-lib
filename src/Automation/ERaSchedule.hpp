#pragma once

#include <ERa/ERaTimeLib.hpp>
#include <Automation/ERaScheduleDef.hpp>

namespace eras {

    class Schedule
    {
    public:
        Schedule(ERaTime& time)
            : pTime(time)
            , from(0UL)
            , to(0UL)
            , len(0UL)
            , mask(0UL)
            , repeat(0UL)
            , repeatCount(0UL)
            , prev(0UL)
        {}
        Schedule(ERaTime& time, ScheduleTimeType s,
                ScheduleTimeType e, ScheduleTimeType d,
                ScheduleConfigurationType m)
            : pTime(time)
            , from(s)
            , to(e)
            , len(d)
            , mask(m)
            , repeat(0UL)
            , repeatCount(0UL)
            , prev(0UL)
        {}

        void setSchedule(ScheduleTimeType s, ScheduleTimeType e,
                        ScheduleTimeType d, ScheduleConfigurationType m,
                        ScheduleConfigurationType l = 0UL) {
            this->from = s;
            this->to = e;
            this->len = d;
            this->mask = m;
            this->repeat = l;
            this->repeatCount = 0UL;
        }

        bool check() {
            return this->activated(this->pTime.now());
        }

        static inline
        ScheduleConfigurationType createOneShotScheduleConfiguration() {
            return (ScheduleConfigurationType)ScheduleType::OneShot;
        }

        static inline
        ScheduleConfigurationType createFixedDeltaScheduleConfiguration(ScheduleUnit unit, unsigned int delta) {
            int tempUnit = static_cast<int>(unit);
            int tempType = static_cast<int>(ScheduleType::FixedDelta);
            unsigned int tempDelta = delta;

            if (tempDelta > ERA_SCHEDULE_REP_MASK) {
                tempDelta = ERA_SCHEDULE_REP_MASK;
            }
            return ((tempUnit << ERA_SCHEDULE_UNIT_SHIFT) | (tempType << ERA_SCHEDULE_TYPE_SHIFT) | tempDelta);
        }

        static inline
        ScheduleConfigurationType createWeeklyScheduleConfiguration(ScheduleWeeklyMask weekMask) {
            unsigned int tempWeek = 0;
            int tempType = static_cast<int>(ScheduleType::Weekly);

            for (int i = 0; i < 7; ++i) {
                if (weekMask[static_cast<ScheduleWeekDay>(i)] == ScheduleState::Active) {
                    tempWeek |= (1 << i);
                }
            }
            return ((tempType << ERA_SCHEDULE_TYPE_SHIFT) | tempWeek);
        }

        static inline
        ScheduleConfigurationType createMonthlyScheduleConfiguration(int dayOfTheMonth) {
            int tempDay = dayOfTheMonth;
            int tempType = static_cast<int>(ScheduleType::Monthly);

            if (tempDay < 1) {
                tempDay = 1;
            }

            if (tempDay > 31) {
                tempDay = 31;
            }
            return ((tempType << ERA_SCHEDULE_TYPE_SHIFT) | tempDay);
        }

        static inline
        ScheduleConfigurationType createYearlyScheduleConfiguration(ScheduleMonth month, int dayOfTheMonth) {
            unsigned int tempDay = Schedule::createMonthlyScheduleConfiguration(dayOfTheMonth);
            int tempMonth = static_cast<int>(month);
            int tempType = static_cast<int>(ScheduleType::Yearly);

            return ((tempType << ERA_SCHEDULE_TYPE_SHIFT) | (tempMonth << ERA_SCHEDULE_MONTH_SHIFT)| tempDay);
        }

        Schedule& operator = (Schedule& schedule) {
            this->from = schedule.from;
            this->to = schedule.to;
            this->len = schedule.len;
            this->mask = schedule.mask;
            return (*this);
        }

        bool operator == (Schedule& schedule) {
            return ((this->from == schedule.from) &&
                    (this->to == schedule.to) &&
                    (this->len == schedule.len) &&
                    (this->mask == schedule.mask));
        }

        bool operator != (Schedule& schedule) {
            return !(operator == (schedule));
        }

        static unsigned long const SECONDS = 1;
        static unsigned long const MINUTES = SECS_PER_MIN;
        static unsigned long const HOURS   = SECS_PER_HOUR;
        static unsigned long const DAYS    = SECS_PER_DAY;

    protected:
    private:
        bool activated(ScheduleTimeType now) {
            if (!this->checkTimeValid(now)) {
                return false;
            }

            if (!this->checkSchedulePeriod(now, this->from, this->to)) {
                return false;
            }

            if (!this->checkScheduleMask(now, this->mask)) {
                return false;
            }

            ScheduleTimeType delta = this->getScheduleDelta(this->mask);
            if (((ERaMax(now, this->from) - ERaMin(now, this->from)) % delta) <= this->len) {
                return true;
            }

            return false;
        }

        ScheduleUnit getScheduleUnit(ScheduleConfigurationType msk) {
            return static_cast<ScheduleUnit>((msk & ERA_SCHEDULE_UNIT_MASK) >> ERA_SCHEDULE_UNIT_SHIFT);
        }

        ScheduleType getScheduleType(ScheduleConfigurationType msk) {
            return static_cast<ScheduleType>((msk & ERA_SCHEDULE_TYPE_MASK) >> ERA_SCHEDULE_TYPE_SHIFT);
        }

        unsigned int getScheduleRepetition(ScheduleConfigurationType msk) {
            return (msk & ERA_SCHEDULE_REP_MASK);
        }

        unsigned int getScheduleWeekMask(ScheduleConfigurationType msk) {
            return (msk & ERA_SCHEDULE_WEEK_MASK);
        }

        unsigned int getScheduleDay(ScheduleConfigurationType msk) {
            return (msk & ERA_SCHEDULE_DAY_MASK);
        }

        unsigned int getScheduleMonth(ScheduleConfigurationType msk) {
            return ((msk & ERA_SCHEDULE_MONTH_MASK) >> ERA_SCHEDULE_MONTH_SHIFT);
        }

        bool isScheduleOneShot(ScheduleConfigurationType msk) {
            return ((this->getScheduleType(msk) == ScheduleType::OneShot) ? true : false);
        }

        bool isScheduleFixed(ScheduleConfigurationType msk) {
            return ((this->getScheduleType(msk) == ScheduleType::FixedDelta) ? true : false);
        }

        bool isScheduleWeekly(ScheduleConfigurationType msk) {
            return ((this->getScheduleType(msk) == ScheduleType::Weekly) ? true : false);
        }

        bool isScheduleMonthly(ScheduleConfigurationType msk) {
            return ((this->getScheduleType(msk) == ScheduleType::Monthly) ? true : false);
        }

        bool isScheduleYearly(ScheduleConfigurationType msk) {
            return ((this->getScheduleType(msk) == ScheduleType::Yearly) ? true : false);
        }

        bool isScheduleInSeconds(ScheduleConfigurationType msk) {
            if (this->isScheduleFixed(msk)) {
                return ((this->getScheduleUnit(msk) == ScheduleUnit::Seconds) ? true : false);
            }
            else {
                return false;
            }
        }

        bool isScheduleInMinutes(ScheduleConfigurationType msk) {
            if (this->isScheduleFixed(msk)) {
                return ((this->getScheduleUnit(msk) == ScheduleUnit::Minutes) ? true : false);
            }
            else {
                return false;
            }
        }

        bool isScheduleInHours(ScheduleConfigurationType msk) {
            if (this->isScheduleFixed(msk)) {
                return ((this->getScheduleUnit(msk) == ScheduleUnit::Hours) ? true : false);
            }
            else {
                return false;
            }
        }

        bool isScheduleInDays(ScheduleConfigurationType msk) {
            if (this->isScheduleFixed(msk)) {
                return ((this->getScheduleUnit(msk) == ScheduleUnit::Days) ? true : false);
            }
            else {
                return false;
            }
        }

        unsigned int getCurrentDayMask(time_t time) {
            struct tm* ptm;
            ptm = gmtime(&time);

            return (1 << ptm->tm_wday);
        }

        unsigned int getCurrentDay(time_t time) {
            struct tm* ptm;
            ptm = gmtime(&time);

            return ptm->tm_mday;
        }

        unsigned int getCurrentMonth(time_t time) {
            struct tm* ptm;
            ptm = gmtime(&time);

            return ptm->tm_mon;
        }

        bool checkTimeValid(ScheduleTimeType now) {
            return (now > ERaTime::getTimeCompile());
        }

        bool checkSchedulePeriod(ScheduleTimeType now, ScheduleTimeType frm, ScheduleTimeType to) {
            if ((now >= frm) && ((now < to) || (to == 0))) {
                return true;
            }
            else {
                return false;
            }
        }

        bool checkScheduleMask(ScheduleTimeType now, ScheduleConfigurationType msk) {
            if (this->isScheduleFixed(msk) || this->isScheduleOneShot(msk)) {
                return true;
            }

            if (this->isScheduleWeekly(msk)) {
                unsigned int currentDayMask = this->getCurrentDayMask(now);
                unsigned int scheduleMask = this->getScheduleWeekMask(msk);
                
                if ((currentDayMask & scheduleMask) != 0) {
                    return true;
                }
            }

            if (this->isScheduleMonthly(msk)) {
                unsigned int currentDay = this->getCurrentDay(now);
                unsigned int scheduleDay = this->getScheduleDay(msk);

                if (currentDay == scheduleDay) {
                    return true;
                }
            }

            if (this->isScheduleYearly(msk)) {
                unsigned int currentDay = this->getCurrentDay(now);
                unsigned int scheduleDay = this->getScheduleDay(msk);
                unsigned int currentMonth = this->getCurrentMonth(now);
                unsigned int scheduleMonth = this->getScheduleMonth(msk);

                if ((currentDay == scheduleDay) && (currentMonth == scheduleMonth)) {
                    return true;
                }
            }

            return false;
        }

        ScheduleTimeType getScheduleDelta(ScheduleConfigurationType msk) {
            if (this->isScheduleInSeconds(msk)) {
                return (Schedule::SECONDS * this->getScheduleRepetition(msk));
            }

            if (this->isScheduleInMinutes(msk)) {
                return (Schedule::MINUTES * this->getScheduleRepetition(msk));
            }

            if (this->isScheduleInHours(msk)) {
                return (Schedule::HOURS * this->getScheduleRepetition(msk));
            }

            if (this->isScheduleInDays(msk)) {
                return (Schedule::DAYS * this->getScheduleRepetition(msk));
            }

            if (this->isScheduleWeekly(msk) || this->isScheduleMonthly(msk) || this->isScheduleYearly(msk)) {
                return Schedule::DAYS;
            }

            return ERA_SCHEDULE_ONE_SHOT;
        }

        ERaTime& pTime;

        ScheduleTimeType from;
        ScheduleTimeType to;
        ScheduleTimeType len;
        ScheduleConfigurationType mask;
        ScheduleConfigurationType repeat;
        ScheduleConfigurationType repeatCount;

        ScheduleTimeType prev;

    };

} /* namespace eras */
