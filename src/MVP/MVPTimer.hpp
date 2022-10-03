#ifndef INC_MVP_TIMER_HPP_
#define INC_MVP_TIMER_HPP_

#include <functional>
#include <MVP/MVPDefine.hpp>

using namespace std;

class MVPTimer
{
    typedef std::function<void(void)> TimerCallback_t;
    typedef std::function<void(void*)> TimerCallback_p_t;

    const static int MAX_TIMERS = 16;
    typedef struct __Timer_t {
        unsigned long prevMillis;
        unsigned long delay;
        unsigned int limit;
        unsigned int count;
        MVPTimer::TimerCallback_t callback;
        MVPTimer::TimerCallback_p_t callback_p;
        void* param;
        bool enable;
        bool called;
    } Timer_t;

public:
    class iterator
    {
    public:
        iterator()
            : tm(nullptr)
            , id(-1)
        {}
        iterator(MVPTimer* _tm, int _id)
            : tm(_tm)
            , id(_id)
        {}
        ~iterator()
        {}

        operator int() const {
            return this->id;
        }

        operator bool() const {
            return this->isValid();
        }

        void operator() (void) const {
            if (!this->isValid()) {
                return;
            }
            this->tm->executeNow(this->id);
        }
        
        bool isValid() const {
            return ((this->tm != nullptr) && (this->id >= 0));
        }

        int getId() const {
            return this->id;
        }

        bool changeInterval(unsigned long interval) {
            if (!this->isValid()) {
                return false;
            }
            return this->tm->changeInterval(this->id, interval);
        }

        void restartTimer() {
            if (!this->isValid()) {
                return;
            }
            this->tm->restartTimer(this->id);
        }

        void deleteTimer() {
            if (!this->isValid()) {
                return;
            }
            this->tm->deleteTimer(this->id);
            this->invalidate();
        }

        bool isEnable() {
            if (!this->isValid()) {
                return false;
            }
            return this->tm->isEnable(this->id);
        }

        void enable() {
            if (this->isValid()) {
                this->tm->enable(this->id);
            }
        }

        void disable() {
            if (this->isValid()) {
                this->tm->disable(this->id);
            }
        }

    protected:
    private:
        void invalidate() {
            this->tm = nullptr;
            this->id = -1;
        }

        MVPTimer* tm;
        int id;
    };

    MVPTimer();
    ~MVPTimer()
    {}

    void run();

    iterator setInterval(unsigned long interval, MVPTimer::TimerCallback_t cb) {
        return iterator(this, this->setupTimer(interval, cb, 0));
    }

    iterator setInterval(unsigned long interval, MVPTimer::TimerCallback_p_t cb, void* arg) {
        return iterator(this, this->setupTimer(interval, cb, arg, 0));
    }

    iterator setTimeout(unsigned long interval, MVPTimer::TimerCallback_t cb) {
        return iterator(this, this->setupTimer(interval, cb, 1));
    }

    iterator setTimeout(unsigned long interval, MVPTimer::TimerCallback_p_t cb, void* arg) {
        return iterator(this, this->setupTimer(interval, cb, arg, 1));
    }

    iterator setTimer(unsigned long interval, MVPTimer::TimerCallback_t cb, unsigned int limit) {
        return iterator(this, this->setupTimer(interval, cb, limit));
    }

    iterator setTimer(unsigned long interval, MVPTimer::TimerCallback_p_t cb, void* arg, unsigned int limit) {
        return iterator(this, this->setupTimer(interval, cb, arg, limit));
    }

    bool changeInterval(unsigned int id, unsigned long interval);
    void restartTimer(unsigned int id);
    void executeNow(unsigned int id);
    void deleteTimer(unsigned int id);
    bool isEnable(unsigned int id);
    void enable(unsigned int id);
    void disable(unsigned int id);
    void enableAll();
    void disableAll();

protected:
private:
    int setupTimer(unsigned long interval, MVPTimer::TimerCallback_t cb, unsigned int limit);
    int setupTimer(unsigned long interval, MVPTimer::TimerCallback_p_t cb, void* arg, unsigned int limit);
    int findTimerFree();

    bool isValidTimer(unsigned int id) { 
        return ((this->timer[id].callback != nullptr) || (this->timer[id].callback_p != nullptr));
    }

    Timer_t timer[MAX_TIMERS] {};
    unsigned int numTimer;
};

#endif /* INC_MVP_TIMER_HPP_ */