#include <cinttypes>

#include <ERa/ERaDebug.hpp>
#include <Utility/ERaUtility.hpp>
#include <Automation/ERaTimer.hpp>

namespace eras {

    void Timer::setTimeout(Component* component, const std::string& name,
                                uint32_t timeout, TimerCallback_t func) {
        const uint32_t now = this->millis();

        if (!name.empty()) {
            this->cancelTimeout(component, name);
        }

        if (timeout == TIMER_DONT_RUN) {
            return;
        }

        auto item = new TimerItem();
        item->component = component;
        item->name = name;
        item->timeout = timeout;
        item->lastExecution = now;
        item->lastExecutionMajor = this->mMillisMajor;
        item->callback = std::move(func);
        item->remove = false;
        this->push(std::move(item));
    }

    bool Timer::cancelTimeout(Component* component, const std::string& name) {
        return this->cancelItem(component, name);
    }

    void Timer::run() {
        const uint32_t now = this->millis();
        this->processToAdd();

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-variable"
        auto toRemoveWas = this->mToRemove;
        auto itemsWas = this->mItems.size();
    #pragma GCC diagnostic pop

        if (this->mToRemove > MAX_LOGICALLY_DELETED_ITEMS) {
            std::vector<TimerItem*> validItems;
            while (!this->empty()) {
                LockGuard guard {this->mLock};
                auto item = std::move(this->mItems[0]);
                this->popRaw();
                validItems.push_back(std::move(item));
            }

            {
                LockGuard guard {this->mLock};
                this->mItems = std::move(validItems);
            }

            this->mToRemove = 0;
        }

        while (!this->empty()) {
            {
                auto item = this->mItems[0];
                if ((now - item->lastExecution) < item->timeout) {
                    break;
                }
                uint8_t major = item->nextExecutionMajor();
                if ((this->mMillisMajor - major) > 1) {
                    break;
                }

    #if defined(ERA_DEBUG_TIMER)
                ERA_LOG(TAG, ERA_PSTR("Running Timeout '%s: %s' with timeout = %" PRIu32 " lastExecution = %" PRIu32 " (now = %" PRIu32 ")"),
                            item->component->getComponentSource(), item->name.c_str(), item->timeout, item->lastExecution, now);
    #endif  /* ERA_DEBUG_TIMER */

                item->callback();
            }

            {
                this->mLock.lock();

                auto item = std::move(this->mItems[0]);
                this->popRaw();

                this->mLock.unlock();

                if (item->remove) {
                    this->mToRemove--;
                }
                delete item;
            }
        }

        this->processToAdd();
    }

    void Timer::processToAdd() {
        LockGuard guard {this->mLock};
        for (auto it : this->mToAdd) {
            if (it->remove) {
                delete it;
                continue;
            }

            this->mItems.push_back(std::move(it));
            std::push_heap(this->mItems.begin(), this->mItems.end(), TimerItem::cmp);
        }
        this->mToAdd.clear();
    }

    void Timer::cleanup() {
        while (!this->mItems.empty()) {
            auto item = this->mItems[0];
            if (!item->remove) {
                return;
            }

            this->mToRemove--;

            {
                LockGuard guard {this->mLock};
                this->popRaw();
            }

            delete item;
        }
    }

    void Timer::popRaw() {
        std::pop_heap(this->mItems.begin(), this->mItems.end(), TimerItem::cmp);
        this->mItems.pop_back();
    }

    void Timer::push(TimerItem* item) {
        LockGuard guard {this->mLock};
        this->mToAdd.push_back(std::move(item));
    }

    bool Timer::cancelItem(Component* component, const std::string& name) {
        LockGuard guard {this->mLock};
        bool ret = false;
        for (auto it : this->mItems) {
            if ((it->component == component) && (it->name == name) && !it->remove) {
                this->mToRemove++;
                it->remove = true;
                ret = true;
            }
        }
        for (auto it : this->mToAdd) {
            if ((it->component == component) && (it->name == name)) {
                it->remove = true;
                ret = true;
            }
        }

        return ret;
    }

    bool Timer::cancelAll(Component* component) {
        LockGuard guard {this->mLock};
        bool ret = false;
        for (auto it : this->mItems) {
            if ((it->component == component) && !it->remove) {
                this->mToRemove++;
                it->remove = true;
                ret = true;
            }
        }
        for (auto it : this->mToAdd) {
            if (it->component == component) {
                it->remove = true;
                ret = true;
            }
        }

        return ret;
    }

    uint32_t Timer::millis() {
        const uint32_t now = ERaMillis();
        if (now < this->mLastMillis) {
            this->mMillisMajor++;
        }
        this->mLastMillis = now;
        return now;
    }

    bool Timer::TimerItem::cmp(TimerItem* a, TimerItem* b) {
        /* Min-heap */
        uint32_t a_nextExec = a->nextExecution();
        uint8_t a_nextExecMajor = a->nextExecutionMajor();
        uint32_t b_nextExec = b->nextExecution();
        uint8_t b_nextExecMajor = b->nextExecutionMajor();

        if (a_nextExecMajor != b_nextExecMajor) {
            uint8_t diff1 = (a_nextExecMajor - b_nextExecMajor);
            uint8_t diff2 = (b_nextExecMajor - a_nextExecMajor);
            return (diff1 < diff2);
        }

        return (a_nextExec > b_nextExec);
    }

} /* namespace eras */
