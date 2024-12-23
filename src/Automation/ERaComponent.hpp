#pragma once

#include <cmath>
#include <cstdint>
#include <functional>
#include <string>
#include <ERa/ERaParam.hpp>

namespace eras {

    class Component
    {
        typedef std::function<void(void)> TimerCallback_t;

    public:
        Component();
        ~Component();

        virtual void run() {}
        virtual void updateValue(ERaUInt_t configID, double value, bool trigger = false) {}

        virtual const char* getComponentSource() const;

        uint32_t millis();

    protected:
        friend class ERaSmart;

        bool hasOverriddenRun() const;
        bool hasOverriddenUpdateValue() const;

        void setTimeout(const std::string& name, uint32_t interval, TimerCallback_t&& fn);
        void setTimeout(uint32_t interval, TimerCallback_t&& fn);
        bool cancelTimeout(const std::string& name);
        bool cancelTimeout();

        void sendNotify(ERaUInt_t automateId, ERaUInt_t notifyId);
        void sendEmail(ERaUInt_t automateId, ERaUInt_t emailId);
    };

} /* namespace eras */
