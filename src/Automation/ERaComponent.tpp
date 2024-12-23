#include <Automation/ERaSmart.hpp>
#include <Automation/ERaComponent.hpp>

namespace eras {

    Component::Component() {
        ERaSmart::instance()->registerComponent(this);
    }

    Component::~Component() {
        ERaSmart::instance()->timer().cancelAll(this);
    }

    const char* Component::getComponentSource() const {
        return "unknown";
    }

    uint32_t Component::millis() {
        return ERaSmart::instance()->timer().millis();
    }

    bool Component::hasOverriddenRun() const {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        bool runOverridden = (((void*)(this->*(&Component::run))) != ((void*)(&Component::run)));
#pragma GCC diagnostic pop
        return runOverridden;
    }

    bool Component::hasOverriddenUpdateValue() const {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
        bool updateValueOverridden = (((void*)(this->*(&Component::updateValue))) != ((void*)(&Component::updateValue)));
#pragma GCC diagnostic pop
        return updateValueOverridden;
    }

    void Component::setTimeout(const std::string& name, uint32_t interval, TimerCallback_t&& fn) {
        ERaSmart::instance()->timer().setTimeout(this, name, interval, std::move(fn));
    }

    void Component::setTimeout(uint32_t interval, TimerCallback_t&& fn) {
        ERaSmart::instance()->timer().setTimeout(this, "", interval, std::move(fn));
    }

    bool Component::cancelTimeout(const std::string& name) {
        return ERaSmart::instance()->timer().cancelTimeout(this, name);
    }

    bool Component::cancelTimeout() {
        return ERaSmart::instance()->timer().cancelTimeout(this, "");
    }

    void Component::sendNotify(ERaUInt_t automateId, ERaUInt_t notifyId) {
        ERaSmart::instance()->sendNotify(automateId, notifyId);
    }

    void Component::sendEmail(ERaUInt_t automateId, ERaUInt_t emailId) {
        ERaSmart::instance()->sendEmail(automateId, emailId);
    }

} /* namespace eras */
