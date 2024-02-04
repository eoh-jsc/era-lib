#include <Utility/ERaUtility.hpp>
#include <ERa/ERaTimer.hpp>

using namespace std;

ERaTimer::ERaTimer()
    : numTimer(0)
{}

void ERaTimer::run() {
    unsigned long currentMillis = ERaMillis();
    const ERaList<Timer_t*>::iterator* e = this->timer.end();
    for (ERaList<Timer_t*>::iterator* it = this->timer.begin(); it != e; it = it->getNext()) {
        Timer_t* pTimer = it->get();
        if (!this->isValidTimer(pTimer)) {
            continue;
        }
        if ((currentMillis - pTimer->prevMillis) < pTimer->delay) {
            continue;
        }
        unsigned long skipTimes = ((currentMillis - pTimer->prevMillis) / pTimer->delay);
        // update time
        pTimer->prevMillis += (pTimer->delay * skipTimes);
        // call callback
        if (!pTimer->enable) {
            continue;
        }
        this->setFlag(pTimer->called, TimerFlagT::TIMER_ON_CALLED, true);
    }

    ERaList<Timer_t*>::iterator* next = nullptr;
    for (ERaList<Timer_t*>::iterator* it = this->timer.begin(); it != e; it = next) {
        next = it->getNext();
        Timer_t* pTimer = it->get();
        if (!this->isValidTimer(pTimer)) {
            continue;
        }
        if (!pTimer->called) {
            continue;
        }
        if (this->getFlag(pTimer->called, TimerFlagT::TIMER_ON_CALLED)) {
            if (pTimer->callback_p == nullptr) {
                pTimer->callback();
            }
            else {
                pTimer->callback_p(pTimer->param);
            }
            if (pTimer->limit) {
                if (++pTimer->count >= pTimer->limit) {
                    this->deleteTimer(pTimer);
                }
            }
        }
        if (this->getFlag(pTimer->called, TimerFlagT::TIMER_ON_DELETE)) {
            delete pTimer;
            pTimer = nullptr;
            it->get() = nullptr;
            this->timer.remove(it);
            this->numTimer--;
            continue;
        }
        pTimer->called = 0;
    }
}

ERaTimer::Timer_t* ERaTimer::setupTimer(unsigned long interval, TimerCallback_t cb, unsigned int limit) {
    if (!this->isTimerFree()) {
        return nullptr;
    }
    if (!interval) {
        interval = 1;
    }

    Timer_t* pTimer = new Timer_t();
    if (pTimer == nullptr) {
        return nullptr;
    }

    pTimer->delay = interval;
    pTimer->callback = cb;
    pTimer->callback_p = nullptr;
    pTimer->param = nullptr;
    pTimer->limit = limit;
    pTimer->count = 0;
    pTimer->enable = true;
    pTimer->called = 0;
    pTimer->prevMillis = ERaMillis();
    this->timer.put(pTimer);
    this->numTimer++;
    return pTimer;
}

ERaTimer::Timer_t* ERaTimer::setupTimer(unsigned long interval, TimerCallback_p_t cb, void* args, unsigned int limit) {
    if (!this->isTimerFree()) {
        return nullptr;
    }
    if (!interval) {
        interval = 1;
    }

    Timer_t* pTimer = new Timer_t();
    if (pTimer == nullptr) {
        return nullptr;
    }

    pTimer->delay = interval;
    pTimer->callback = nullptr;
    pTimer->callback_p = cb;
    pTimer->param = args;
    pTimer->limit = limit;
    pTimer->count = 0;
    pTimer->enable = true;
    pTimer->called = 0;
    pTimer->prevMillis = ERaMillis();
    this->timer.put(pTimer);
    this->numTimer++;
    return pTimer;
}

bool ERaTimer::changeInterval(Timer_t* pTimer, unsigned long interval) {
    if (!this->isValidTimer(pTimer)) {
        return false;
    }
    if (!interval) {
        interval = 1;
    }

    pTimer->delay = interval;
    pTimer->prevMillis = ERaMillis();
    return true;
}

void ERaTimer::restartTimer(Timer_t* pTimer) {
    if (this->isValidTimer(pTimer)) {
        pTimer->prevMillis = ERaMillis();
    }
}

void ERaTimer::executeNow(Timer_t* pTimer) {
    if (this->isValidTimer(pTimer)) {
        pTimer->prevMillis = ERaMillis() - pTimer->delay;
    }
}

void ERaTimer::deleteTimer(Timer_t* pTimer) {
    if (!this->numTimer) {
        return;
    }

    if (this->isValidTimer(pTimer)) {
        this->setFlag(pTimer->called, TimerFlagT::TIMER_ON_DELETE, true);
    }
}

bool ERaTimer::isEnable(Timer_t* pTimer) {
    if (this->isValidTimer(pTimer)) {
        return pTimer->enable;
    }
    else {
        return false;
    }
}

void ERaTimer::enable(Timer_t* pTimer) {
    if (this->isValidTimer(pTimer)) {
        pTimer->enable = true;
    }
}

void ERaTimer::disable(Timer_t* pTimer) {
    if (this->isValidTimer(pTimer)) {
        pTimer->enable = false;
    }
}

void ERaTimer::enableAll() {
    const ERaList<Timer_t*>::iterator* e = this->timer.end();
    for (ERaList<Timer_t*>::iterator* it = this->timer.begin(); it != e; it = it->getNext()) {
        Timer_t* pTimer = it->get();
        if (this->isValidTimer(pTimer)) {
            pTimer->enable = true;
        }
    }
}

void ERaTimer::disableAll() {
    const ERaList<Timer_t*>::iterator* e = this->timer.end();
    for (ERaList<Timer_t*>::iterator* it = this->timer.begin(); it != e; it = it->getNext()) {
        Timer_t* pTimer = it->get();
        if (this->isValidTimer(pTimer)) {
            pTimer->enable = false;
        }
    }
}

bool ERaTimer::isTimerFree() {
    if (this->numTimer >= MAX_TIMERS) {
        return false;
    }

    return true;
}
