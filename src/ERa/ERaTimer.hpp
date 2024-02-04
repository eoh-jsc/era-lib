#ifndef INC_ERA_TIMER_HPP_
#define INC_ERA_TIMER_HPP_

#include <ERa/ERaDefine.hpp>
#include <ERa/ERaDetect.hpp>
#include <Utility/ERaQueue.hpp>

class ERaTimer
{
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> TimerCallback_t;
    typedef std::function<void(void*)> TimerCallback_p_t;
#else
    typedef void (*TimerCallback_t)(void);
    typedef void (*TimerCallback_p_t)(void*);
#endif

    const static int MAX_TIMERS = 16;
    enum TimerFlagT {
        TIMER_ON_CALLED = 0x01,
        TIMER_ON_DELETE = 0x80
    };
    typedef struct __Timer_t {
        unsigned long prevMillis;
        unsigned long delay;
        unsigned int limit;
        unsigned int count;
        ERaTimer::TimerCallback_t callback;
        ERaTimer::TimerCallback_p_t callback_p;
        void* param;
        bool enable;
        uint8_t called;
    } Timer_t;

public:
    class iterator
    {
    public:
        iterator()
            : tm(nullptr)
            , pTm(nullptr)
        {}
        iterator(ERaTimer* _tm, Timer_t* _pTm)
            : tm(_tm)
            , pTm(_pTm)
        {}
        ~iterator()
        {}

        operator Timer_t*() const {
            return this->pTm;
        }

        operator bool() const {
            return this->isValid();
        }

        void operator() (void) const {
            if (!this->isValid()) {
                return;
            }
            this->tm->executeNow(this->pTm);
        }

        bool isValid() const {
            return ((this->tm != nullptr) && (this->pTm != nullptr));
        }

        Timer_t* getId() const {
            return this->pTm;
        }

        bool changeInterval(unsigned long interval) const {
            if (!this->isValid()) {
                return false;
            }
            return this->tm->changeInterval(this->pTm, interval);
        }

        void restartTimer() const {
            if (!this->isValid()) {
                return;
            }
            this->tm->restartTimer(this->pTm);
        }

        bool isEnable() const {
            if (!this->isValid()) {
                return false;
            }
            return this->tm->isEnable(this->pTm);
        }

        void enable() const {
            if (!this->isValid()) {
                return;
            }
            this->tm->enable(this->pTm);
        }

        void disable() const {
            if (!this->isValid()) {
                return;
            }
            this->tm->disable(this->pTm);
        }

        void deleteTimer() {
            if (!this->isValid()) {
                return;
            }
            this->tm->deleteTimer(this->pTm);
            this->invalidate();
        }

    protected:
    private:
        void invalidate() {
            this->tm = nullptr;
            this->pTm = nullptr;
        }

        ERaTimer* tm;
        Timer_t* pTm;
    };

    ERaTimer();
    ~ERaTimer()
    {}

    void run();

    iterator setInterval(unsigned long interval, ERaTimer::TimerCallback_t cb) {
        return iterator(this, this->setupTimer(interval, cb, 0));
    }

    iterator setInterval(unsigned long interval, ERaTimer::TimerCallback_p_t cb, void* args) {
        return iterator(this, this->setupTimer(interval, cb, args, 0));
    }

    iterator setTimeout(unsigned long interval, ERaTimer::TimerCallback_t cb) {
        return iterator(this, this->setupTimer(interval, cb, 1));
    }

    iterator setTimeout(unsigned long interval, ERaTimer::TimerCallback_p_t cb, void* args) {
        return iterator(this, this->setupTimer(interval, cb, args, 1));
    }

    iterator setTimer(unsigned long interval, ERaTimer::TimerCallback_t cb, unsigned int limit) {
        return iterator(this, this->setupTimer(interval, cb, limit));
    }

    iterator setTimer(unsigned long interval, ERaTimer::TimerCallback_p_t cb, void* args, unsigned int limit) {
        return iterator(this, this->setupTimer(interval, cb, args, limit));
    }

    bool changeInterval(Timer_t* pTimer, unsigned long interval);
    void restartTimer(Timer_t* pTimer);
    void executeNow(Timer_t* pTimer);
    void deleteTimer(Timer_t* pTimer);
    bool isEnable(Timer_t* pTimer);
    void enable(Timer_t* pTimer);
    void disable(Timer_t* pTimer);
    void enableAll();
    void disableAll();

protected:
private:
    Timer_t* setupTimer(unsigned long interval, ERaTimer::TimerCallback_t cb, unsigned int limit);
    Timer_t* setupTimer(unsigned long interval, ERaTimer::TimerCallback_p_t cb, void* args, unsigned int limit);
    bool isTimerFree();

    bool isValidTimer(const Timer_t* pTimer) const {
        if (pTimer == nullptr) {
            return false;
        }
        return ((pTimer->callback != nullptr) || (pTimer->callback_p != nullptr));
    }

    void setFlag(uint8_t& flags, uint8_t mask, bool value) {
        if (value) {
            flags |= mask;
        }
        else {
            flags &= ~mask;
        }
    }

    bool getFlag(uint8_t flags, uint8_t mask) {
        return (flags & mask) == mask;
    }

    ERaList<Timer_t*> timer;
    unsigned int numTimer;
};

using TimerEntry = ERaTimer::iterator;

#endif /* INC_ERA_TIMER_HPP_ */
