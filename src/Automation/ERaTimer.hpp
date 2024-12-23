#pragma once

#include <vector>
#include <memory>
#include <Automation/ERaComponent.hpp>
#include <Automation/ERaHelpers.hpp>

namespace eras {

    class Component;

    class Timer
    {
        typedef std::function<void(void)> TimerCallback_t;

        const char* const TAG = "Timer";

    public:
        void setTimeout(Component* component, const std::string& name, uint32_t timeout, TimerCallback_t func);
        bool cancelTimeout(Component* component, const std::string& name);
        bool cancelAll(Component* component);

        void run();
        void processToAdd();
        uint32_t millis();

    protected:
        struct TimerItem {
            Component* component;
            std::string name;
            uint32_t timeout;
            uint32_t lastExecution;
            TimerCallback_t callback;
            bool remove;
            uint8_t lastExecutionMajor;

            inline uint32_t nextExecution() {
                return this->lastExecution + this->timeout;
            }

            inline uint8_t nextExecutionMajor() {
                uint32_t nextExec = this->nextExecution();
                uint8_t nextExecMajor = this->lastExecutionMajor;
                if (nextExec < this->lastExecution) {
                    nextExecMajor++;
                }
                return nextExecMajor;
            }

            static bool cmp(TimerItem* a, TimerItem* b);
        };

        void cleanup();
        void popRaw();
        void push(TimerItem* item);
        bool cancelItem(Component* component, const std::string& name);
        bool empty() {
            this->cleanup();
            return this->mItems.empty();
        }

        Mutex mLock;
        std::vector<TimerItem*> mItems;
        std::vector<TimerItem*> mToAdd;
        uint32_t mLastMillis {0};
        uint8_t mMillisMajor {0};
        uint32_t mToRemove {0};

        static constexpr uint32_t TIMER_DONT_RUN = 4294967295UL;
        static constexpr uint32_t MAX_LOGICALLY_DELETED_ITEMS = 10;
    };

} /* namespace eras */

#include <Automation/ERaTimer.tpp>
