#pragma once

#include <cinttypes>

#include <vector>
#include <ERa/ERaDetect.hpp>
#include <Automation/ERaSchedule.hpp>
#include <Automation/ERaComponent.hpp>
#include <Automation/ERaHelpers.hpp>

namespace eras {

    class Log
    {
    protected:
        const char* TAG = "Automation";

    public:
        explicit Log() = default;

        void setID(ERaUInt_t id) {
            this->mID = id;
        }

        ERaUInt_t getID() const {
            return this->mID;
        }

        virtual void log() {
            if (!this->mID) {
                return;
            }
            this->print();
        }

    protected:
        virtual void print() = 0;

        ERaUInt_t mID {0};
    };

    class Condition
        : public Log
    {
    public:
        virtual ~Condition() = default;

        virtual bool check() = 0;

    protected:
        virtual void print() override {
            ERA_LOG_WARNING(this->TAG, "Condition '%" PRIu32 "' actived!", this->mID);
        }
    };

    class Automation;

    class Trigger
    {
    public:
        virtual ~Trigger() = default;

        void setAutomationParent(Automation* automation) {
            this->mAutomationParent = automation;
        }

        void trigger();
        void stopAction();
        bool isActionRunning();

    protected:
        Automation* mAutomationParent {nullptr};
    };

    class ActionList;

    class Action
        : public Log
    {
    public:
        virtual ~Action() = default;

        virtual bool isRunning() {
            return (this->mIsRunning || this->isRunningNext());
        }

    protected:
        friend ActionList;

        virtual void play() {
            this->mIsRunning = true;
            this->log();
            this->playNext();
        }

        void playNext() {
            if (this->mIsRunning) {
                this->mIsRunning = false;
                if (this->mNext != nullptr) {
                    this->mNext->play();
                }
            }
        }

        virtual void stop() {
            this->mIsRunning = false;
            this->stopNext();
        }

        void stopNext() {
            if (this->mNext == nullptr) {
                return;
            }
            this->mNext->stop();
        }

        bool isRunningNext() {
            if (this->mNext == nullptr) {
                return false;
            }
            return this->mNext->isRunning();
        }

        virtual void print() override {
            ERA_LOG_WARNING(this->TAG, "Action '%" PRIu32 "' triggered!", this->mID);
        }

        Action* mNext {nullptr};
        bool mIsRunning {false};
    };

    class ActionList
    {
    public:
        virtual ~ActionList() {
            auto* current = this->mActionsBegin;
            while (current != nullptr) {
                auto* next = current->mNext;
                delete current;
                current = next;
            }
        }

        void addAction(Action* action) {
            if (this->mActionsEnd == nullptr) {
                this->mActionsBegin = action;
            }
            else {
                this->mActionsEnd->mNext = action;
            }
            this->mActionsEnd = action;
        }

        void addActions(const std::vector<Action*>& actions) {
            for (auto* action : actions) {
                this->addAction(action);
            }
        }

        void play() {
            if (this->mActionsBegin == nullptr) {
                return;
            }
            this->mActionsBegin->play();
        }

        void stop() {
            if (this->mActionsBegin == nullptr) {
                return;
            }
            this->mActionsBegin->stop();
        }

        bool empty() const {
            return (this->mActionsBegin == nullptr);
        }

        bool isRunning() {
            if (this->mActionsBegin == nullptr) {
                return false;
            }
            return this->mActionsBegin->isRunning();
        }

    protected:
        Action* mActionsBegin {nullptr};
        Action* mActionsEnd {nullptr};
    };

    class Automation
    {
    public:
        explicit Automation() = default;

        explicit Automation(Trigger* trigger)
            : mTrigger(trigger)
        {
            this->mTrigger->setAutomationParent(this);
        }

        virtual ~Automation() {
            this->stop();
            delete this->mTrigger;
        }

        void setTrigger(Trigger* trigger) {
            this->mTrigger = trigger;
            this->mTrigger->setAutomationParent(this);
        }

        Trigger* getTrigger() const {
            return this->mTrigger;
        }

        void addAction(Action* action) {
            this->mActions.addAction(action);
        }

        void addActions(const std::vector<Action*>& actions) {
            this->mActions.addActions(actions);
        }

        void stop() {
            this->mActions.stop();
        }

        void trigger() {
            this->mActions.play();
        }

        bool isRunning() {
            return this->mActions.isRunning();
        }

    protected:
        Trigger* mTrigger {nullptr};
        ActionList mActions;
    };

    void Trigger::trigger() {
        if (this->mAutomationParent == nullptr) {
            return;
        }
        this->mAutomationParent->trigger();
    }

    void Trigger::stopAction() {
        if (this->mAutomationParent == nullptr) {
            return;
        }
        this->mAutomationParent->stop();
    }

    bool Trigger::isActionRunning() {
        if (this->mAutomationParent == nullptr) {
            return false;
        }
        return this->mAutomationParent->isRunning();
    }

} /* namespace eras */

namespace eras {

    class AndCondition
        : public Condition
    {
    public:
        explicit AndCondition() = default;

        explicit AndCondition(const std::vector<Condition*>& conditions)
            : mConditions(conditions)
        {}

        virtual ~AndCondition() override {
            for (auto* condition : this->mConditions) {
                delete condition;
            }
            this->mConditions.clear();
        }

        void addCondition(Condition* condition) {
            this->mConditions.push_back(condition);
        }

        void addConditions(const std::vector<Condition*>& conditions) {
            for (auto* condition : conditions) {
                this->addCondition(condition);
            }
        }

        bool check() override {
            for (auto* condition : this->mConditions) {
                if (!condition->check()) {
                    return false;
                }
            }

            return true;
        }

    protected:
        void log() override {
            this->print();
        }

        void print() override {
            std::string ids = "";
            for (const auto& condition : this->mConditions) {
                ids += to_string(condition->getID());
                if (&condition != &this->mConditions.back()) {
                    ids += ", ";
                }
            }
            ERA_LOG_WARNING(this->TAG, "AndCondition actived with conditions: '%s'", ids.c_str());
        }

        std::vector<Condition*> mConditions {};
    };

    template <typename T>
    class ValueCondition
        : public Condition
        , public Component
    {
    public:
        explicit ValueCondition() = default;

        explicit ValueCondition(std::function<T()>&& fn)
            : mFn(std::move(fn))
        {}

        explicit ValueCondition(std::function<T()>&& fn,
                                const std::vector<T>& thresholds)
            : mFn(std::move(fn))
            , mPrimaryThresholds(thresholds)
        {}

        explicit ValueCondition(const std::vector<T>& thresholds)
            : mPrimaryThresholds(thresholds)
        {}

        void setValue(T value) {
            this->mValue = value;
        }

        void setLambdaFunction(std::function<T()>&& fn) {
            this->mFn = std::move(fn);
        }

        void addThreshold(const T& threshold) {
            this->mPrimaryThresholds.emplace_back(std::move(threshold));
        }

        void addThresholds(const std::vector<T>& thresholds) {
            for (const auto& threshold : thresholds) {
                this->addThreshold(threshold);
            }
        }

        void setScale(const T& scale) {
            this->mScale = scale;
        }

        void setConfigID(ERaUInt_t configID) {
            this->mConfigID = configID;
        }

        void updateValue(ERaUInt_t configID, double value, bool trigger = false) override {
            if (this->mConfigID != configID) {
                return;
            }
            this->mValue = value;
            this->mHasValue = true;
            this->mReset = trigger;
        }

        bool check() override {
            T value;
            if (this->mFn) {
                value = this->mFn();
            }
            else if (this->mHasValue) {
                value = this->mValue;
                value *= this->mScale;
            }
            else {
                return false;
            }
            return this->checkThreshold(value);
        }

    protected:
        virtual bool checkThreshold(const T& value) = 0;

        std::function<T()> mFn;
        std::vector<T> mPrimaryThresholds {};
        ERaUInt_t mConfigID {0};
        T mScale {};
        T mValue {};
        bool mHasValue {false};
        bool mReset {false};
    };

    template <typename T>
    class HigherCondition
        : public ValueCondition<T>
    {
    public:
        using ValueCondition<T>::ValueCondition;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mPrimaryThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mPrimaryThresholds) {
                if (value <= threshold) {
                    return false;
                }
            }
            return true;
        }

        const char* getComponentSource() const override {
            return "HigherCondition";
        }
    };

    template <typename T>
    class LowerCondition
        : public ValueCondition<T>
    {
    public:
        using ValueCondition<T>::ValueCondition;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mPrimaryThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mPrimaryThresholds) {
                if (value >= threshold) {
                    return false;
                }
            }
            return true;
        }

        const char* getComponentSource() const override {
            return "LowerCondition";
        }
    };

    template <typename T>
    class EqualsCondition
        : public ValueCondition<T>
    {
    public:
        using ValueCondition<T>::ValueCondition;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mPrimaryThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mPrimaryThresholds) {
                if (value == threshold) {
                    return true;
                }
            }
            return false;
        }

        const char* getComponentSource() const override {
            return "EqualsCondition";
        }
    };

    template <typename T>
    class NotEqualsCondition
        : public ValueCondition<T>
    {
    public:
        using ValueCondition<T>::ValueCondition;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mPrimaryThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mPrimaryThresholds) {
                if (value == threshold) {
                    return false;
                }
            }
            return true;
        }

        const char* getComponentSource() const override {
            return "NotEqualsCondition";
        }
    };

    template <typename T>
    class RangeCondition
        : public ValueCondition<T>
    {
    public:
        using ValueCondition<T>::ValueCondition;

        explicit RangeCondition(std::function<T()>&& fn,
                                const std::vector<T>& primaryThresholds,
                                const std::vector<T>& secondaryThresholds)
            : ValueCondition<T>(std::move(fn), primaryThresholds)
            , mSecondaryThresholds(secondaryThresholds)
        {}

        explicit RangeCondition(const std::vector<T>& primaryThresholds,
                                const std::vector<T>& secondaryThresholds)
            : ValueCondition<T>(primaryThresholds)
            , mSecondaryThresholds(secondaryThresholds)
        {}

        void addPrimaryThreshold(const T& threshold) {
            this->mPrimaryThresholds.emplace_back(std::move(threshold));
        }

        void addPrimaryThresholds(const std::vector<T>& thresholds) {
            for (const auto& threshold : thresholds) {
                this->addPrimaryThreshold(threshold);
            }
        }

        void addSecondaryThreshold(const T& threshold) {
            this->mSecondaryThresholds.emplace_back(std::move(threshold));
        }

        void addSecondaryThresholds(const std::vector<T>& thresholds) {
            for (const auto& threshold : thresholds) {
                this->addSecondaryThreshold(threshold);
            }
        }

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mPrimaryThresholds.empty() ||
                this->mSecondaryThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mPrimaryThresholds) {
                if (value < threshold) {
                    return false;
                }
            }
            for (const auto& threshold : this->mSecondaryThresholds) {
                if (value >= threshold) {
                    return false;
                }
            }
            return true;
        }

        const char* getComponentSource() const override {
            return "RangeCondition";
        }

        std::vector<T> mSecondaryThresholds {};
    };

    template <typename T>
    using OnCondition = EqualsCondition<T>;

    template <typename T>
    using OffCondition = NotEqualsCondition<T>;

    class ScheduleCondition
        : public Condition
    {
    public:
        explicit ScheduleCondition() = default;

        explicit ScheduleCondition(Schedule* schedule)
            : mSchedule(schedule)
        {}

        virtual ~ScheduleCondition() override {
            delete this->mSchedule;
        }

        void setSchedule(Schedule* schedule) {
            this->mSchedule = schedule;
        }

        bool check() override {
            return this->mSchedule->check();
        }

    protected:
        Schedule* mSchedule {nullptr};
    };

    class DelayAction
        : public Action
        , public Component
    {
    public:
        explicit DelayAction() = default;

        explicit DelayAction(unsigned long delay)
            : mDelay(delay)
        {}

        void setDelay(unsigned long delay) {
            this->mDelay = delay;
        }

        void play() override {
            auto fn = std::bind(&DelayAction::playNext, this);
            this->mIsRunning = true;
            this->setTimeout("", this->mDelay, fn);
            this->log();
        }

        void stop() override {
            this->cancelTimeout("");
            this->mIsRunning = false;
            this->stopNext();
        }

    protected:
        const char* getComponentSource() const override {
            return "DelayAction";
        }

        void log() override {
            this->print();
        }

        void print() override {
            ERA_LOG_WARNING(this->TAG, "Delay '%" PRIu32 "' triggered!", this->mDelay);
        }

        unsigned long mDelay {0};
    };

    class LambdaAction
        : public Action
    {
    public:
        explicit LambdaAction() = default;

        explicit LambdaAction(std::function<void()>&& fn)
            : mFn(std::move(fn))
        {}

        void setAction(std::function<void()>&& fn) {
            this->mFn = std::move(fn);
        }

        void play() override {
            this->mFn();
            this->mIsRunning = true;
            this->log();
            this->playNext();
        }

    protected:
        std::function<void()> mFn;
    };

    class NotifyAction
        : public Action
        , public Component
    {
    public:
        explicit NotifyAction() = default;

        explicit NotifyAction(ERaUInt_t automateId,
                              ERaUInt_t notifyId,
                              unsigned long delay)
            : mAutomateId(automateId)
            , mNotifyId(notifyId)
            , mDelay(delay)
        {}

        void setDelay(unsigned long delay) {
            this->mDelay = delay;
        }

        void setNotify(ERaUInt_t automateId, ERaUInt_t notifyId) {
            this->mAutomateId = automateId;
            this->mNotifyId = notifyId;
        }

        void play() override {
            this->mIsRunning = true;
            const uint32_t now = this->millis();
            if (!this->mDelay) {
                this->mLastNotify = now;
                this->execute();
                return;
            }

            const uint32_t before = this->mLastNotify;
            const uint32_t elapsed = (now - before);
            const uint32_t delay = this->mDelay;

            const uint32_t amount = (elapsed / delay);
            if ((amount > 0) && (now > before)) {
                this->mLastNotify += (amount * delay);
                this->execute();
            }
            else {
                const uint32_t next = (before + delay);
                const uint32_t remaining = (next - now);

                auto fn = std::bind(&NotifyAction::execute, this);
                this->mLastNotify = next;
                this->setTimeout("", remaining, fn);
            }
        }

        void stop() override {
            this->cancelTimeout("");
            this->mIsRunning = false;
            this->stopNext();
        }

    protected:
        void execute() {
            this->sendNotify(this->mAutomateId, this->mNotifyId);
            this->log();
            this->playNext();
        }

        const char* getComponentSource() const override {
            return "NotifyAction";
        }

        void log() override {
            this->print();
        }

        void print() override {
            ERA_LOG_WARNING(this->TAG, "Notify '%" PRIu32 ": %" PRIu32 "' triggered!",
                            this->mAutomateId, this->mNotifyId);
        }

        ERaUInt_t mAutomateId {0};
        ERaUInt_t mNotifyId {0};
        unsigned long mDelay {0};
        unsigned long mLastNotify {0};
    };

    class EmailAction
        : public Action
        , public Component
    {
    public:
        explicit EmailAction() = default;

        explicit EmailAction(ERaUInt_t automateId,
                             ERaUInt_t emailId,
                             unsigned long delay)
            : mAutomateId(automateId)
            , mEmailId(emailId)
            , mDelay(delay)
        {}

        void setDelay(unsigned long delay) {
            this->mDelay = delay;
        }

        void setEmail(ERaUInt_t automateId, ERaUInt_t emailId) {
            this->mAutomateId = automateId;
            this->mEmailId = emailId;
        }

        void play() override {
            this->mIsRunning = true;
            const uint32_t now = this->millis();
            if (!this->mDelay) {
                this->mLastEmail = now;
                this->execute();
                return;
            }

            const uint32_t before = this->mLastEmail;
            const uint32_t elapsed = (now - before);
            const uint32_t delay = this->mDelay;

            const uint32_t amount = (elapsed / delay);
            if ((amount > 0) && (now > before)) {
                this->mLastEmail += (amount * delay);
                this->execute();
            }
            else {
                const uint32_t next = (before + delay);
                const uint32_t remaining = (next - now);

                auto fn = std::bind(&EmailAction::execute, this);
                this->mLastEmail = next;
                this->setTimeout("", remaining, fn);
            }
        }

        void stop() override {
            this->cancelTimeout("");
            this->mIsRunning = false;
            this->stopNext();
        }

    protected:
        void execute() {
            this->sendEmail(this->mAutomateId, this->mEmailId);
            this->log();
            this->playNext();
        }

        const char* getComponentSource() const override {
            return "EmailAction";
        }

        void log() override {
            this->print();
        }

        void print() override {
            ERA_LOG_WARNING(this->TAG, "Email '%" PRIu32 ": %" PRIu32 "' triggered!",
                            this->mAutomateId, this->mEmailId);
        }

        ERaUInt_t mAutomateId {0};
        ERaUInt_t mEmailId {0};
        unsigned long mDelay {0};
        unsigned long mLastEmail {0};
    };

} /* namespace eras */

namespace eras {

    class Smart
        : public Automation
        , public Component
    {
    public:
        explicit Smart() = default;

        explicit Smart(Condition* condition, Trigger* trigger, bool valueChange)
            : Automation(trigger)
            , mCondition(condition)
            , mValueChange(valueChange)
        {}

        virtual ~Smart() override {
            delete this->mCondition;
        }

        void setCondition(Condition* condition) {
            this->mCondition = condition;
        }

        void setValueChange(bool enable) {
            this->mValueChange = enable;
        }

        void run() override {
            bool cond = this->mCondition->check();
            if (this->mValueChange) {
                if (!this->mConditionDedup.next(cond)) {
                    return;
                }
            }
            if (!cond) {
                return;
            }
            this->mCondition->log();
            this->check();
        }

    protected:
        void check() {
            if (this->mTrigger->isActionRunning()) {
                return;
            }
            this->mTrigger->trigger();
        }

        const char* getComponentSource() const override {
            return "Smart";
        }

        Condition* mCondition {nullptr};
        Deduplicator<bool> mConditionDedup;
        bool mValueChange {true};
    };

} /* namespace eras */
