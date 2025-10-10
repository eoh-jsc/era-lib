#pragma once

#include <string>
#include <vector>
#include <ERa/ERaAutomation.hpp>
#include <ERa/ERaApiHandler.hpp>
#include <Automation/ERaAutomation.hpp>
#include <Automation/ERaComponent.hpp>
#include <Automation/ERaHelpers.hpp>
#include <Automation/ERaTimer.hpp>

namespace eras {

    class Component;

    class ERaSmart
        : public ERaAutomation
    {
        typedef std::function<void(void)> TimerCallback_t;

        const char* const TAG = "Smart";

    public:
        explicit ERaSmart(ERaApiHandler& api)
            : ERaAutomation()
            , mApi(api)
            , mTime(nullptr)
        {
            ERaSmart::instance() = this;
#if defined(ERA_AUTOMATION)
            this->mApi.setERaAutomation(this);
#endif
        }

        explicit ERaSmart(ERaApiHandler& api, ERaTime& time)
            : ERaAutomation()
            , mApi(api)
            , mTime(&time)
        {
            ERaSmart::instance() = this;
#if defined(ERA_AUTOMATION)
            this->mApi.setERaAutomation(this);
#endif
        }

        void setTime(ERaTime* time) {
            this->mTime = time;
        }

        void setTime(ERaTime& time) {
            this->mTime = &time;
        }

        void setup();
        void registerComponent(Component* component);
        void unregisterComponent(Component* component);
        void calculateRunningComponents();
        void calculateValueComponents();

        Timer& timer() {
            return this->mTimer;
        }

        void begin(void* args = nullptr) override;
        void run() override;
        void deleteAll() override;
        void updateValue(ERaUInt_t configID, double value, bool trigger = false) override;
        bool isRunning() const override;

        static ERaSmart* getInstance() {
            return ERaSmart::instance();
        }

    protected:
        void parseAutomations(const cJSON* const root);
        void parseAutomation(const cJSON* const root);

        void parseBooleanEvaluation(const cJSON* const root, const cJSON* const id, const cJSON* const configuration);
        void parseRangeEvaluation(const cJSON* const root, const cJSON* const id, const cJSON* const configuration);
        void parseEvaluation(const cJSON* const root, const cJSON* const id, const cJSON* const type,
                                                                            const cJSON* const configuration);
        void parseValueEvaluation(const cJSON* const root, const cJSON* const id);
        void parseValueChange(const cJSON* const root, const cJSON* const id);
        void parseScheduleOnce(const cJSON* const root, const cJSON* const id, const std::string& dateTime);
        void parseScheduleEveryDay(const cJSON* const root, const cJSON* const id, const std::string& dateTime);
        void parseScheduleEveryWeek(const cJSON* const root, const cJSON* const id, const std::string& dateTime);
        void parseSchedule(const cJSON* const root, const cJSON* const id);
        void parseConditions(const cJSON* const root);
        void parseCondition(const cJSON* const root);

        void parseAduinoPinAction(const cJSON* const root, const cJSON* const id, const cJSON* const value);
        void parseVirtualPinAction(const cJSON* const root, const cJSON* const id, const cJSON* const value);
        void parseArduinoAction(const cJSON* const root, const cJSON* const data);
        void parseModbusAction(const cJSON* const root, const cJSON* const data);
        void parseZigbeeDevice(const cJSON* const root, const cJSON* const data, const cJSON* const actionID,
                                                                                const char* controlPayload);
        void parseZigbeeAction(const cJSON* const root, const cJSON* const data);

        void parseDelayAction(uint32_t delay);
        void parseNotificationAction(uint32_t automateId, uint32_t id);
        void parseEmailAction(uint32_t automateId, uint32_t id);
        void parseWebhookAction(uint32_t automateId, uint32_t id);
        void parseSMSAction(const cJSON* const root);

        void parseActionScript(const cJSON* const root, const cJSON* const data);
        void parseActions(const cJSON* const root, uint32_t automateId);
        void parseAction(const cJSON* const root, uint32_t automateId);

        void getDate(std::string& date);
        void getTime(std::string& time);

        void sendNotify(ERaUInt_t automateId, ERaUInt_t notifyId);
        void sendEmail(ERaUInt_t automateId, ERaUInt_t emailId);
        void sendWebhook(ERaUInt_t automateId, ERaUInt_t webhookId);

        static ERaSmart*& instance() {
            static ERaSmart* smart = nullptr;
            return smart;
        }

        friend class Component;

        ERaApiHandler& mApi;
        ERaTime* mTime;
        Timer mTimer;
        std::vector<Component*> mComponents {};
        std::vector<Component*> mRunningComponents {};
        std::vector<Component*> mValueComponents {};

        void clearCurrentConditions(bool del) {
            for (auto* condition : this->mCurrentConditions) {
                if (!del) {
                    break;
                }
                if (condition == nullptr) {
                    continue;
                }
                delete condition;
            }
            this->mCurrentConditions.clear();
        }

        void clearCurrentActions(bool del) {
            for (auto* action : this->mCurrentActions) {
                if (!del) {
                    break;
                }
                if (action == nullptr) {
                    continue;
                }
                delete action;
            }
            this->mCurrentActions.clear();
        }

        void clearSmarts(bool del) {
            for (auto* smart : this->mSmarts) {
                if (!del) {
                    break;
                }
                if (smart == nullptr) {
                    continue;
                }
                delete smart;
            }
            this->mSmarts.clear();
        }

        std::vector<Condition*> mCurrentConditions {};
        std::vector<Action*> mCurrentActions {};
        std::vector<Smart*> mSmarts {};

        Mutex mLock;

        bool mInitialized {false};
    };

} /* namespace eras */

#include <Automation/ERaSmart.tpp>
#include <Automation/ERaComponent.tpp>

using eras::ERaSmart;
