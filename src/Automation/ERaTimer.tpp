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
        item->nextExecution = now + timeout;
        item->callback = std::move(func);
        item->remove = false;
        this->push(item);
    }

    bool Timer::cancelTimeout(Component* component, const std::string& name) {
        return this->cancelItem(component, name);
    }

    void Timer::run() {
        const uint32_t now = this->millis();
        this->processToAdd();

        if (this->mToRemove > MAX_LOGICALLY_DELETED_ITEMS) {
            std::vector<TimerItem*> validItems;
            while (!this->empty()) {
                LockGuard guard {this->mLock};
                auto item = this->mItems[0];
                this->popRaw();
                validItems.push_back(item);
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
                if (item->nextExecution > now) {
                    break;
                }

    #if defined(ERA_DEBUG_TIMER)
                ERA_LOG(TAG, ERA_PSTR("Running Timeout '%s: %s' with timeout = %" PRIu32 " nextExecution = %" PRIu32 " (now = %" PRIu32 ")"),
                            item->getSource(), item->name.c_str(), item->timeout, item->nextExecution, now);
    #endif  /* ERA_DEBUG_TIMER */

                item->callback();
            }

            {
                this->mLock.lock();

                auto item = this->mItems[0];
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

            this->mItems.push_back(it);
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
        this->mToAdd.push_back(item);
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

    uint64_t Timer::millis() {
        const uint32_t now = ERaMillis();
        if (now < this->mLastMillis) {
            this->mMillisMajor++;
        }
        this->mLastMillis = now;
        return (now + (static_cast<uint64_t>(this->mMillisMajor) << 32));
    }

    uint16_t Timer::millisMajor() {
        return this->mMillisMajor;
    }

    bool Timer::TimerItem::cmp(TimerItem* a, TimerItem* b) {
        /* Min-heap */
        return a->nextExecution > b->nextExecution;
    }

} /* namespace eras */
