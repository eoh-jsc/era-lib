#include <ERa/ERaParam.hpp>
#include <ERa/ERaDebug.hpp>
#include <Automation/ERaSmart.hpp>
#include <Automation/ERaComponent.hpp>
#include <Automation/ERaParseKey.hpp>

namespace eras {

    void ERaSmart::setup() {
        if (this->mTime != nullptr) {
            this->mTime->begin();
        }

        this->calculateRunningComponents();
        this->calculateValueComponents();
    }

    void ERaSmart::run() {
        this->mTimer.run();

        LockGuard guard {this->mLock};
        for (auto* component : this->mRunningComponents) {
            component->run();
        }
    }

    void ERaSmart::registerComponent(Component* component) {
        if (component == nullptr) {
            return;
        }

        for (auto* comp : this->mComponents) {
            if (comp == component) {
                return;
            }
        }
        this->mComponents.push_back(component);
    }

    void ERaSmart::unregisterComponent(Component* component) {
        if (component == nullptr) {
            return;
        }

        std::vector<Component*> validComponents;
        for (auto* comp : this->mComponents) {
            if (comp == component) {
                continue;
            }
            validComponents.push_back(comp);
        }
        this->mComponents = std::move(validComponents);
    }

    void ERaSmart::calculateRunningComponents() {
        this->mRunningComponents.clear();
        for (auto* component : this->mComponents) {
            if (component->hasOverriddenRun()) {
                this->mRunningComponents.push_back(component);
            }
        }
    }

    void ERaSmart::calculateValueComponents() {
        this->mValueComponents.clear();
        for (auto* component : this->mComponents) {
            if (component->hasOverriddenUpdateValue()) {
                this->mValueComponents.push_back(component);
            }
        }
    }

    void ERaSmart::begin(void* args) {
        this->deleteAll();

        LockGuard guard {this->mLock};

        cJSON* root = (cJSON*)args;
        if (!cJSON_IsArray(root)) {
            return;
        }

        this->parseAutomations(root);
        this->setup();
    }

    void ERaSmart::deleteAll() {
        LockGuard guard {this->mLock};

        this->mComponents.clear();
        this->mRunningComponents.clear();
        this->mValueComponents.clear();

        this->clearSmarts(true);
    }

    void ERaSmart::updateValue(ERaUInt_t configID, double value, bool trigger) {
        for (auto* component : this->mValueComponents) {
            component->updateValue(configID, value, trigger);
        }
    }

    void ERaSmart::parseAutomations(const cJSON* const root) {
        cJSON* item = root->child;
        while (item != nullptr) {
            this->parseAutomation(item);
            item = item->next;
        }
    }

    void ERaSmart::parseAutomation(const cJSON* const root) {
        if (!cJSON_IsObject(root)) {
            return;
        }

        cJSON* id = cJSON_GetObjectItem(root, AUTOMATION_ID_KEY);
        if (!cJSON_IsNumber(id)) {
            return;
        }
        if (!id->valueint) {
            return;
        }

        cJSON* enable = cJSON_GetObjectItem(root, AUTOMATION_ENABLE_KEY);
        if (!cJSON_IsBool(enable)) {
        }
        else if (!enable->valueint) {
            return;
        }

        cJSON* conditions = cJSON_GetObjectItem(root, AUTOMATION_CONDITIONS_KEY);
        if (cJSON_IsArray(conditions)) {
            this->parseConditions(conditions);
        }
        else {
            this->parseCondition(root);
        }

        cJSON* actions = cJSON_GetObjectItem(root, AUTOMATION_ACTIONS_KEY);
        if (!cJSON_IsArray(actions)) {
            this->clearCurrentConditions(true);
            return;
        }
        this->parseActions(actions, id->valueint);

        auto pTrigger = new Trigger();

        auto pCondition = new AndCondition();
        pCondition->addConditions(std::move(this->mCurrentConditions));

        auto pSmart = new Smart();
        pSmart->setTrigger(std::move(pTrigger));
        pSmart->setCondition(std::move(pCondition));
        pSmart->addActions(std::move(this->mCurrentActions));

        this->mSmarts.push_back(std::move(pSmart));

        this->clearCurrentConditions(false);
        this->clearCurrentActions(false);
    }

    void ERaSmart::parseBooleanEvaluation(const cJSON* const root, const cJSON* const configuration) {
        cJSON* values = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_VALUE_KEY);
        if (!cJSON_IsArray(values)) {
            return;
        }
        if (cJSON_GetArraySize(values) < 2) {
            return;
        }
        cJSON* config = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_CONFIG_KEY);
        if (!cJSON_IsNumber(config)) {
            return;
        }

        cJSON* threshold = cJSON_GetArrayItem(values, 1);
        if (!cJSON_IsNumber(threshold)) {
            return;
        }
        cJSON* isOnValues = cJSON_GetObjectItem(configuration, AUTOMATION_VALUE_EVALUATION_IOV_KEY);
        if (!cJSON_IsArray(isOnValues)) {
            return;
        }

        ValueCondition<double>* pCondition = nullptr;
        if (threshold->valueint) {
            pCondition = new OnCondition<double>();
        }
        else {
            pCondition = new OffCondition<double>();
        }

        cJSON* item = isOnValues->child;
        while (item != nullptr) {
            if (cJSON_IsNumber(item)) {
                pCondition->addThreshold(item->valuedouble);
            }
            else if (cJSON_IsString(item)) {
                pCondition->addThreshold(ParseNumber(item->valuestring));
            }
            item = item->next;
        }
        pCondition->setConfigID(config->valueint);
        this->mCurrentConditions.push_back(std::move(pCondition));
    }

    void ERaSmart::parseRangeEvaluation(const cJSON* const root, const cJSON* const configuration) {
        cJSON* values = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_VALUE_KEY);
        if (!cJSON_IsArray(values)) {
            return;
        }
        if (cJSON_GetArraySize(values) < 2) {
            return;
        }
        cJSON* config = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_CONFIG_KEY);
        if (!cJSON_IsNumber(config)) {
            return;
        }

        cJSON* index = cJSON_GetArrayItem(values, 1);
        if (!cJSON_IsNumber(index)) {
            return;
        }
        cJSON* ranges = cJSON_GetObjectItem(configuration, AUTOMATION_VALUE_EVALUATION_RANGES_KEY);
        if (!cJSON_IsArray(ranges)) {
            return;
        }
        int sizeRanges = cJSON_GetArraySize(ranges);
        if (index->valueint >= sizeRanges) {
            return;
        }
        cJSON* item = cJSON_GetArrayIndex(ranges, index->valueint);
        if (!cJSON_IsObject(item)) {
            return;
        }

        cJSON* start = cJSON_GetObjectItem(item, AUTOMATION_VALUE_EVALUATION_START_KEY);
        if (!cJSON_IsNumber(start)) {
            return;
        }
        cJSON* end = cJSON_GetObjectItem(item, AUTOMATION_VALUE_EVALUATION_END_KEY);
        if (!cJSON_IsNumber(end)) {
            return;
        }

        ValueCondition<double>* pCondition = nullptr;
        pCondition = new RangeCondition<double>();
        pCondition->addThreshold(start->valuedouble);
        pCondition->addThresholdEnd(end->valuedouble);
        pCondition->setConfigID(config->valueint);
        this->mCurrentConditions.push_back(std::move(pCondition));
    }

    void ERaSmart::parseEvaluation(const cJSON* const root, const cJSON* const type,
                                                    const cJSON* const configuration) {
        if (ERaStrCmp(type->valuestring, "boolean")) {
            this->parseBooleanEvaluation(root, configuration);
        }
        else if (ERaStrCmp(type->valuestring, "range")) {
            this->parseRangeEvaluation(root, configuration);
        }
    }

    void ERaSmart::parseValueEvaluation(const cJSON* const root) {
        cJSON* evaluation = cJSON_GetObjectItem(root, AUTOMATION_VALUE_EVALUATION_KEY);
        if (!cJSON_IsObject(evaluation)) {
            return;
        }
        cJSON* type = cJSON_GetObjectItem(evaluation, AUTOMATION_VALUE_EVALUATION_TEMPLATE_KEY);
        if (!cJSON_IsString(type)) {
            return;
        }
        cJSON* configuration = cJSON_GetObjectItem(evaluation, AUTOMATION_VALUE_EVALUATION_CONFIG_KEY);
        if (!cJSON_IsObject(configuration)) {
            return;
        }
        this->parseEvaluation(root, type, configuration);
    }

    void ERaSmart::parseValueChange(const cJSON* const root) {
        cJSON* condition = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_CONDITION_KEY);
        if (!cJSON_IsString(condition)) {
            return;
        }
        if (ERaStrCmp(condition->valuestring, "value_evaluation")) {
            return this->parseValueEvaluation(root);
        }

        double threshold;
        cJSON* value = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_VALUE_KEY);
        if (cJSON_IsNumber(value)) {
            threshold = value->valuedouble;
        }
        else if (cJSON_IsString(value)) {
            threshold = ParseNumber(value->valuestring);
        }
        if (std::isnan(threshold)) {
            return;
        }
        cJSON* config = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_CONFIG_KEY);
        if (!cJSON_IsNumber(config)) {
            return;
        }

        ValueCondition<double>* pCondition = nullptr;
        if (ERaStrCmp(condition->valuestring, "=")) {
            pCondition = new EqualsCondition<double>();
        }
        else if (ERaStrCmp(condition->valuestring, ">")) {
            pCondition = new HigherCondition<double>();
        }
        else if (ERaStrCmp(condition->valuestring, "<")) {
            pCondition = new LowerCondition<double>();
        }
        else {
            return;
        }
        pCondition->addThreshold(threshold);
        pCondition->setConfigID(config->valueint);
        this->mCurrentConditions.push_back(std::move(pCondition));
    }

    void ERaSmart::parseScheduleOnce(const cJSON* const root, const std::string& dateTime) {
        ScheduleTimeType startingFrom = ERaTime::getTimeFromStringNumber(dateTime.c_str());
        ScheduleTimeType until = startingFrom + (Schedule::MINUTES * 10);
        ScheduleTimeType activePeriod = Schedule::MINUTES * 5;
        ScheduleConfigurationType activeRepeat = 1UL;

        ScheduleConfigurationType scheduleConfiguration =  Schedule::createOneShotScheduleConfiguration();

        auto oneShot = new Schedule(*this->mTime);
        oneShot->setSchedule(startingFrom, until, activePeriod, scheduleConfiguration, activeRepeat);

        auto pCondition = new ScheduleCondition();
        pCondition->setSchedule(std::move(oneShot));
        this->mCurrentConditions.push_back(std::move(pCondition));
    }

    void ERaSmart::parseScheduleEveryDay(const cJSON* const root, const std::string& dateTime) {
        ScheduleTimeType startingFrom = ERaTime::getTimeFromStringNumber(dateTime.c_str());
        ScheduleTimeType until = 0UL; // Forever
        ScheduleTimeType activePeriod = Schedule::MINUTES * 5;
        ScheduleConfigurationType activeRepeat = 1UL;
        unsigned int repetitionPeriod = 1;

        ScheduleConfigurationType scheduleConfiguration =  Schedule::createFixedDeltaScheduleConfiguration(ScheduleUnit::Days, repetitionPeriod);

        auto daily = new Schedule(*this->mTime);
        daily->setSchedule(startingFrom, until, activePeriod, scheduleConfiguration, activeRepeat);

        auto pCondition = new ScheduleCondition();
        pCondition->setSchedule(std::move(daily));
        this->mCurrentConditions.push_back(std::move(pCondition));
    }

    void ERaSmart::parseScheduleEveryWeek(const cJSON* const root, const std::string& dateTime) {
        unsigned int startingFrom = ERaTime::getTimeFromStringNumber(dateTime.c_str());
        unsigned int until = 0UL; // Forever
        unsigned int executionPeriod = Schedule::MINUTES * 5;
        ScheduleConfigurationType activeRepeat = 1UL;

        ScheduleWeeklyMask WeeklyMask = {
            ScheduleState::Inactive,   /* Sunday */
            ScheduleState::Inactive,   /* Monday */
            ScheduleState::Inactive,   /* Tuesday */
            ScheduleState::Inactive,   /* Wednesday */
            ScheduleState::Inactive,   /* Thursday */
            ScheduleState::Inactive,   /* Friday */
            ScheduleState::Inactive,   /* Saturday */
        };

        cJSON* weekday = cJSON_GetObjectItem(root, AUTOMATION_SCHEDULE_WEEKDAY_KEY);
        if (!cJSON_IsArray(weekday)) {
            return;
        }
        cJSON* item = weekday->child;
        while (item != nullptr) {
            if (cJSON_IsString(item)) {
                int day = (item->valuestring[0] - '0');
                if (day >= 0 && day < 7) {
                    WeeklyMask[(ScheduleWeekDay)day] = ScheduleState::Active;
                }
            }
            item = item->next;
        }

        ScheduleConfigurationType scheduleConfiguration =  Schedule::createWeeklyScheduleConfiguration(WeeklyMask);

        auto weekly = new Schedule(*this->mTime);
        weekly->setSchedule(startingFrom, until, executionPeriod, scheduleConfiguration, activeRepeat);

        auto pCondition = new ScheduleCondition();
        pCondition->setSchedule(std::move(weekly));
        this->mCurrentConditions.push_back(std::move(pCondition));
    }

    void ERaSmart::parseSchedule(const cJSON* const root) {
        if (this->mTime == nullptr) {
            return;
        }
        cJSON* type = cJSON_GetObjectItem(root, AUTOMATION_SCHEDULE_TYPE_KEY);
        if (!cJSON_IsString(type)) {
            return;
        }

        std::string dateTime;
        cJSON* date = cJSON_GetObjectItem(root, AUTOMATION_SCHEDULE_DATE_KEY);
        if (cJSON_IsString(date)) {
            dateTime = date->valuestring;
        }
        cJSON* time = cJSON_GetObjectItem(root, AUTOMATION_SCHEDULE_TIME_KEY);
        if (cJSON_IsString(time)) {
            dateTime += " ";
            dateTime += time->valuestring;
        }

        if (ERaStrCmp(type->valuestring, "once")) {
            this->parseScheduleOnce(root, dateTime);
        }
        else if (ERaStrCmp(type->valuestring, "every_day")) {
            this->parseScheduleEveryDay(root, dateTime);
        }
        else if (ERaStrCmp(type->valuestring, "every_week")) {
            this->parseScheduleEveryWeek(root, dateTime);
        }
    }

    void ERaSmart::parseConditions(const cJSON* const root) {
        cJSON* item = root->child;
        while (item != nullptr) {
            this->parseCondition(item);
            item = item->next;
        }
    }

    void ERaSmart::parseCondition(const cJSON* const root) {
        if (!cJSON_IsObject(root)) {
            return;
        }

        cJSON* valueChange = cJSON_GetObjectItem(root, AUTOMATION_VALUE_CHANGE_KEY);
        if (cJSON_IsObject(valueChange)) {
            this->parseValueChange(valueChange);
        }
        cJSON* schedule = cJSON_GetObjectItem(root, AUTOMATION_SCHEDULE_KEY);
        if (cJSON_IsObject(schedule)) {
            this->parseSchedule(schedule);
        }
    }

    void ERaSmart::parseAduinoPinAction(const cJSON* const root, const cJSON* const value) {
        cJSON* pinNumber = cJSON_GetObjectItem(root, AUTOMATION_ARDUINO_PIN_NUMBER_KEY);
        if (!cJSON_IsNumber(pinNumber)) {
            return;
        }

        cJSON_Int_t pin = pinNumber->valueint;
        std::string sValue;
        double dValue {0.0};
        if (cJSON_IsBool(value) || cJSON_IsNumber(value)) {
            dValue = value->valuedouble;
        }
        else if (cJSON_IsString(value)) {
            sValue = value->valuestring;
        }
        else {
            return;
        }

        auto action = new LambdaAction([&, this, pin, dValue, sValue]() {
            if (this->mBaseTopic == nullptr) {
                return;
            }
            if (this->mCallback == nullptr) {
                return;
            }
            char topic[MAX_TOPIC_LENGTH] {0};
            FormatString(topic, this->mBaseTopic);
            FormatString(topic, ERA_PUB_PREFIX_ARDUINO_DOWN_TOPIC, pin);
            cJSON* object = cJSON_CreateObject();
            if (object == nullptr) {
                return;
            }
            if (!sValue.empty()) {
                cJSON_AddStringToObject(object, "value", sValue.c_str());
            }
            else {
                cJSON_AddNumberWithDecimalToObject(object, "value", dValue, 2);
            }
            char* payload = cJSON_PrintUnformatted(object);
            if (payload != nullptr) {
                this->mCallback(topic, payload);
                free(payload);
            }
            cJSON_Delete(object);
            object = nullptr;
            payload = nullptr;
        });
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseVirtualPinAction(const cJSON* const root, const cJSON* const value) {
        cJSON* pinNumber = cJSON_GetObjectItem(root, AUTOMATION_ARDUINO_PIN_NUMBER_KEY);
        if (!cJSON_IsNumber(pinNumber)) {
            return;
        }

        cJSON_Int_t pin = pinNumber->valueint;
        std::string sValue;
        double dValue {0.0};
        if (cJSON_IsBool(value) || cJSON_IsNumber(value)) {
            dValue = value->valuedouble;
        }
        else if (cJSON_IsString(value)) {
            sValue = value->valuestring;
        }
        else {
            return;
        }

        auto action = new LambdaAction([&, this, pin, dValue, sValue]() {
            if (this->mBaseTopic == nullptr) {
                return;
            }
            if (this->mCallback == nullptr) {
                return;
            }
            char topic[MAX_TOPIC_LENGTH] {0};
            FormatString(topic, this->mBaseTopic);
            FormatString(topic, ERA_PUB_PREFIX_VIRTUAL_DOWN_TOPIC, pin);
            cJSON* object = cJSON_CreateObject();
            if (object == nullptr) {
                return;
            }
            if (!sValue.empty()) {
                cJSON_AddStringToObject(object, "value", sValue.c_str());
            }
            else {
                cJSON_AddNumberWithDecimalToObject(object, "value", dValue, 2);
            }
            char* payload = cJSON_PrintUnformatted(object);
            if (payload != nullptr) {
                this->mCallback(topic, payload);
                free(payload);
            }
            cJSON_Delete(object);
            object = nullptr;
            payload = nullptr;
        });
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseArduinoAction(const cJSON* const root, const cJSON* const data) {
        cJSON* value {nullptr};
        if (cJSON_IsObject(data)) {
            value = cJSON_GetObjectItem(data, AUTOMATION_ARDUINO_VALUE_KEY);
        }
        else {
            value = cJSON_GetObjectItem(root, AUTOMATION_ARDUINO_VALUE_KEY);
        }
        if (value == nullptr) {
            return;
        }
        cJSON* arduinoPin = cJSON_GetObjectItem(root, AUTOMATION_ARDUINO_PIN_KEY);
        if (cJSON_IsObject(arduinoPin)) {
            this->parseAduinoPinAction(arduinoPin, value);
        }
        cJSON* virtualPin = cJSON_GetObjectItem(root, AUTOMATION_ARDUINO_VIRTUAL_PIN_KEY);
        if (cJSON_IsObject(virtualPin)) {
            this->parseVirtualPinAction(virtualPin, value);
        }
    }

    void ERaSmart::parseModbusAction(const cJSON* const root, const cJSON* const data) {
        cJSON* value {nullptr};
        if (cJSON_IsObject(data)) {
            value = cJSON_GetObjectItem(data, AUTOMATION_MODBUS_VALUE_KEY);
        }
        cJSON* alias = cJSON_GetObjectItem(root, AUTOMATION_MODBUS_ALIAS_KEY);
        if (!cJSON_IsString(alias)) {
            return;
        }

        std::string key = alias->valuestring;
        std::string sValue;
        double dValue {0.0};
        bool hasValue {false};
        if (cJSON_IsBool(value) || cJSON_IsNumber(value)) {
            dValue = value->valuedouble;
            hasValue = true;
        }
        else if (cJSON_IsString(value)) {
            sValue = value->valuestring;
            hasValue = true;
        }

        auto action = new LambdaAction([&, this, key, hasValue, dValue, sValue]() {
            if (this->mBaseTopic == nullptr) {
                return;
            }
            if (this->mCallback == nullptr) {
                return;
            }
            char topic[MAX_TOPIC_LENGTH] {0};
            FormatString(topic, this->mBaseTopic);
            FormatString(topic, ERA_PUB_PREFIX_DOWN_TOPIC);

            cJSON* object = cJSON_CreateObject();
            if (object == nullptr) {
                return;
            }
            cJSON* subObject = cJSON_CreateObject();
            if (subObject == nullptr) {
                cJSON_Delete(object);
                object = nullptr;
                return;
            }
            cJSON_AddStringToObject(object, "action", "send_command");
            cJSON_AddItemToObject(object, "data", subObject);
            cJSON* array = cJSON_CreateArray();
            if (array == nullptr) {
                cJSON_Delete(object);
                object = nullptr;
                return;
            }

            cJSON_AddItemToArray(array, cJSON_CreateString(key.c_str()));
            cJSON_AddItemToObject(subObject, "commands", array);

            if (!hasValue) {
            }
            else if (!sValue.empty()) {
                cJSON_AddStringToObject(subObject, "value", sValue.c_str());
            }
            else {
                cJSON_AddNumberWithDecimalToObject(subObject, "value", dValue, 2);
            }

            char* payload = cJSON_PrintUnformatted(object);
            if (payload != nullptr) {
                this->mCallback(topic, payload);
                free(payload);
            }
            cJSON_Delete(object);
            object = nullptr;
            payload = nullptr;
        });
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseZigbeeDevice(const cJSON* const root, const cJSON* const data, const char* controlPayload) {
        cJSON* value {nullptr};
        if (cJSON_IsObject(data)) {
            value = cJSON_GetObjectItem(data, AUTOMATION_ZIGBEE_VALUE_KEY);
        }
        cJSON* ieee = cJSON_GetObjectItem(root, AUTOMATION_ZIGBEE_IEEE_KEY);
        if (!cJSON_IsString(ieee)) {
            return;
        }

        std::string ieeeAddress = ieee->valuestring;
        std::string payload = controlPayload;
        if (cJSON_IsBool(value) || cJSON_IsNumber(value)) {
            StringReplace(payload, "%(value)d", to_string(value->valueint));
            StringReplace(payload, "%(value)f", to_string(value->valuedouble));
        }
        else if (cJSON_IsString(value)) {
            StringReplace(payload, "%(value)s", value->valuestring);
        }

        auto action = new LambdaAction([&, this, ieeeAddress, payload]() {
            if (this->mBaseTopic == nullptr) {
                return;
            }
            if (this->mCallback == nullptr) {
                return;
            }
            char topic[MAX_TOPIC_LENGTH] {0};
            FormatString(topic, this->mBaseTopic);
            FormatString(topic, ERA_PUB_PREFIX_ZIGBEE_DOWN_TOPIC, ieeeAddress.c_str());

            this->mCallback(topic, payload.c_str());
        });
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseZigbeeAction(const cJSON* const root, const cJSON* const data) {
        cJSON* device = cJSON_GetObjectItem(root, AUTOMATION_ZIGBEE_DEVICE_KEY);
        cJSON* controlPayload = cJSON_GetObjectItem(root, AUTOMATION_ZIGBEE_CONTROL_PAYLOAD_KEY);
        if (cJSON_IsObject(device) && cJSON_IsString(controlPayload)) {
            this->parseZigbeeDevice(device, data, controlPayload->valuestring);
        }
    }

    void ERaSmart::parseSMSAction(const cJSON* const root) {
        cJSON* phoneItem = cJSON_GetObjectItem(root, AUTOMATION_SMS_PHONE_KEY);
        if (!cJSON_IsString(phoneItem)) {
            return;
        }
        cJSON* messageItem = cJSON_GetObjectItem(root, AUTOMATION_SMS_MESSAGE_KEY);
        if (!cJSON_IsString(messageItem)) {
            return;
        }

        std::string phone = phoneItem->valuestring;
        std::string message = messageItem->valuestring;

        auto action = new LambdaAction([&, this, phone, message]() {
            if (this->mBaseTopic == nullptr) {
                return;
            }
            if (this->mCallback == nullptr) {
                return;
            }
            char topic[MAX_TOPIC_LENGTH] {0};
            FormatString(topic, this->mBaseTopic);
            FormatString(topic, ERA_PUB_PREFIX_DOWN_TOPIC);

            cJSON* object = cJSON_CreateObject();
            if (object == nullptr) {
                return;
            }
            cJSON* subObject = cJSON_CreateObject();
            if (subObject == nullptr) {
                cJSON_Delete(object);
                object = nullptr;
                return;
            }
            cJSON_AddStringToObject(object, "action", "send_sms");
            cJSON_AddItemToObject(object, "data", subObject);
            cJSON* array = cJSON_CreateArray();
            if (array == nullptr) {
                cJSON_Delete(object);
                object = nullptr;
                return;
            }

            cJSON* smsObject = cJSON_CreateObject();
            if (smsObject != nullptr) {
                cJSON_AddStringToObject(smsObject, "to", phone.c_str());
                cJSON_AddStringToObject(smsObject, "message", message.c_str());
                cJSON_AddItemToArray(array, smsObject);
            }

            cJSON_AddItemToObject(subObject, "messages", array);

            char* payload = cJSON_PrintUnformatted(object);
            if (payload != nullptr) {
                this->mCallback(topic, payload);
                free(payload);
            }
            cJSON_Delete(object);
            object = nullptr;
            payload = nullptr;
            smsObject = nullptr;
        });
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseActionScript(const cJSON* const root, const cJSON* const data) {
        cJSON* arduinoAction = cJSON_GetObjectItem(root, AUTOMATION_ARDUINO_ACTION_KEY);
        if (cJSON_IsObject(arduinoAction)) {
            this->parseArduinoAction(arduinoAction, data);
        }
        cJSON* modbusAction = cJSON_GetObjectItem(root, AUTOMATION_MODBUS_ACTION_KEY);
        if (cJSON_IsObject(modbusAction)) {
            this->parseModbusAction(modbusAction, data);
        }
        cJSON* zigbeeAction = cJSON_GetObjectItem(root, AUTOMATION_ZIGBEE_ACTION_KEY);
        if (cJSON_IsObject(zigbeeAction)) {
            this->parseZigbeeAction(zigbeeAction, data);
        }
    }

    void ERaSmart::parseDelayAction(uint32_t delay) {
        if (!delay) {
            return;
        }
        auto action = new DelayAction(delay * 1000L);
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseNotificationAction(uint32_t automateId, uint32_t id) {
        if (!id) {
            return;
        }
        auto action = new NotifyAction();
        action->setNotify(automateId, id);
        action->setDelay(ERA_NOTIFY_DELAY);
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseEmailAction(uint32_t automateId, uint32_t id) {
        if (!id) {
            return;
        }
        auto action = new EmailAction();
        action->setEmail(automateId, id);
        action->setDelay(ERA_EMAIL_DELAY);
        this->mCurrentActions.push_back(std::move(action));
    }

    void ERaSmart::parseActions(const cJSON* const root, uint32_t automateId) {
        cJSON* item = root->child;
        while (item != nullptr) {
            this->parseAction(item, automateId);
            item = item->next;
        }
    }

    void ERaSmart::parseAction(const cJSON* const root, uint32_t automateId) {
        if (!cJSON_IsObject(root)) {
            return;
        }

        cJSON* actionData = cJSON_GetObjectItem(root, AUTOMATION_ACTION_DATA_KEY);
        cJSON* actionScript = cJSON_GetObjectItem(root, AUTOMATION_ACTION_SCRIPT_KEY);
        if (cJSON_IsObject(actionScript)) {
            this->parseActionScript(actionScript, actionData);
        }
        cJSON* actionDelay = cJSON_GetObjectItem(root, AUTOMATION_ACTION_DELAY_KEY);
        if (cJSON_IsNumber(actionDelay)) {
            this->parseDelayAction(actionDelay->valueint);
        }
        cJSON* notifiScript = cJSON_GetObjectItem(root, AUTOMATION_ACTION_NOTIFICATION_KEY);
        if (cJSON_IsNumber(notifiScript)) {
            this->parseNotificationAction(automateId, notifiScript->valueint);
        }
        cJSON* emailScript = cJSON_GetObjectItem(root, AUTOMATION_ACTION_EMAIL_KEY);
        if (cJSON_IsNumber(emailScript)) {
            this->parseEmailAction(automateId, emailScript->valueint);
        }
        cJSON* smsScript = cJSON_GetObjectItem(root, AUTOMATION_ACTION_SMS_KEY);
        if (cJSON_IsObject(smsScript)) {
            this->parseSMSAction(smsScript);
        }
    }

    void ERaSmart::sendNotify(ERaUInt_t automateId, ERaUInt_t notifyId) {
        this->mApi.sendNotify(automateId, notifyId);
    }

    void ERaSmart::sendEmail(ERaUInt_t automateId, ERaUInt_t emailId) {
        this->mApi.sendEmail(automateId, emailId);
    }

} /* namespace eras */
