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
        uint64_t millis();
        uint16_t millisMajor();

    protected:
        struct TimerItem {
            Component* component;
            std::string name;
            uint32_t timeout;
            uint64_t nextExecution;
            TimerCallback_t callback;
            bool remove;

            static bool cmp(TimerItem* a, TimerItem* b);

            const char* getSource() {
              return ((this->component != nullptr) ? this->component->getComponentSource() : "unknown");
            }
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
        uint16_t mMillisMajor {0};
        uint32_t mToRemove {0};

        static constexpr uint32_t TIMER_DONT_RUN = 4294967295UL;
        static constexpr uint32_t MAX_LOGICALLY_DELETED_ITEMS = 10;
    };

} /* namespace eras */

#include <Automation/ERaTimer.tpp>
