#pragma once

#include <vector>
#include <ERa/ERaDetect.hpp>
#include <Automation/ERaSchedule.hpp>
#include <Automation/ERaComponent.hpp>
#include <Automation/ERaHelpers.hpp>

namespace eras {

    class Condition
    {
    public:
        virtual ~Condition() = default;

        virtual bool check() = 0;

    protected:
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
                this->mActionsBegin = std::move(action);;
            }
            else {
                this->mActionsEnd->mNext = std::move(action);;
            }
            this->mActionsEnd = action;
        }

        void addActions(const std::vector<Action*>& actions) {
            for (auto* action : actions) {
                this->addAction(std::move(action));
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
            : mTrigger(std::move(trigger))
        {
            this->mTrigger->setAutomationParent(this);
        }

        virtual ~Automation() {
            this->stop();
            delete this->mTrigger;
        }

        void setTrigger(Trigger* trigger) {
            this->mTrigger = std::move(trigger);
            this->mTrigger->setAutomationParent(this);
        }

        Trigger* getTrigger() const {
            return this->mTrigger;
        }

        void addAction(Action* action) {
            this->mActions.addAction(std::move(action));
        }

        void addActions(const std::vector<Action*>& actions) {
            this->mActions.addActions(std::move(actions));
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
            : mConditions(std::move(conditions))
        {}

        virtual ~AndCondition() override {
            for (auto* condition : this->mConditions) {
                delete condition;
            }
            this->mConditions.clear();
        }

        void addCondition(Condition* condition) {
            this->mConditions.push_back(std::move(condition));
        }

        void addConditions(const std::vector<Condition*>& conditions) {
            for (auto* condition : conditions) {
                this->addCondition(std::move(condition));
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
            , mThresholds(std::move(thresholds))
        {}

        explicit ValueCondition(const std::vector<T>& thresholds)
            : mThresholds(std::move(thresholds))
        {}

        void setValue(T value) {
            this->mValue = value;
        }

        void setLambdaFunction(std::function<T()>&& fn) {
            this->mFn = std::move(fn);
        }

        void addThreshold(const T& threshold) {
            this->mThresholds.push_back(threshold);
        }

        void setThreshold(const std::vector<T>& thresholds) {
            this->mThresholds = std::move(thresholds);
        }

        void addThresholdEnd(const T& threshold) {
            this->mThresholdsEnd.push_back(threshold);
        }

        void setThresholdEnd(const std::vector<T>& thresholds) {
            this->mThresholdsEnd = std::move(thresholds);
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
            }
            else {
                return false;
            }
            return this->checkThreshold(value);
        }

    protected:
        virtual bool checkThreshold(const T& value) = 0;

        std::function<T()> mFn;
        std::vector<T> mThresholds {};
        std::vector<T> mThresholdsEnd {};
        ERaUInt_t mConfigID {0};
        T mValue {};
        bool mHasValue {false};
        bool mReset {false};
    };

    template <typename T>
    class HigherCondition
        : public ValueCondition<T>
    {
    public:
        explicit HigherCondition() = default;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            for (const auto& threshold : this->mThresholds) {
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
        explicit LowerCondition() = default;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mThresholds) {
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
        explicit EqualsCondition() = default;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mThresholds) {
                if (value != threshold) {
                    return false;
                }
            }
            return true;
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
        explicit NotEqualsCondition() = default;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mThresholds.empty()) {
                return false;
            }
            for (const auto& threshold : this->mThresholds) {
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
        explicit RangeCondition() = default;

    protected:
        bool checkThreshold(const T& value) override {
            if (this->mReset) {
                this->mReset = false;
                return false;
            }
            if (this->mThresholds.empty() ||
                this->mThresholdsEnd.empty()) {
                return false;
            }
            for (const auto& threshold : this->mThresholds) {
                if (value < threshold) {
                    return false;
                }
            }
            for (const auto& threshold : this->mThresholdsEnd) {
                if (value >= threshold) {
                    return false;
                }
            }
            return true;
        }

        const char* getComponentSource() const override {
            return "RangeCondition";
        }
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
            : mSchedule(std::move(schedule))
        {}

        virtual ~ScheduleCondition() override {
            delete this->mSchedule;
        }

        void setSchedule(Schedule* schedule) {
            this->mSchedule = std::move(schedule);
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
            this->playNext();
        }

        const char* getComponentSource() const override {
            return "NotifyAction";
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
            this->playNext();
        }

        const char* getComponentSource() const override {
            return "EmailAction";
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
            : Automation(std::move(trigger))
            , mCondition(std::move(condition))
            , mValueChange(valueChange)
        {}

        virtual ~Smart() override {
            delete this->mCondition;
        }

        void setCondition(Condition* condition) {
            this->mCondition = std::move(condition);
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
