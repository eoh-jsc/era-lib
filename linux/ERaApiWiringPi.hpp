#ifndef INC_ERA_API_WIRING_PI_HPP_
#define INC_ERA_API_WIRING_PI_HPP_

#include <ERa/ERaApi.hpp>

#define ERA_MODEL_TYPE                "ERa"
#define ERA_BOARD_TYPE                "Raspberry"

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::handleReadPin(cJSON* root) {
	if (!cJSON_IsArray(root)) {
		return;
	}

	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr; current = current->next) {
		if (!cJSON_IsObject(current)) {
			continue;
		}
		pin = PinConfig_t();
		cJSON* item = cJSON_GetObjectItem(current, "config_id");
		if (cJSON_IsNumber(item)) {
			pin.configId = item->valueint;
		}
		item = cJSON_GetObjectItem(current, "pin_number");
		if (cJSON_IsNumber(item)) {
			pin.pin = item->valueint;
		}
		if (pin.pin > MAX_GPIO_WIRING_PI) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
			if (ERaStrCmp(item->valuestring, "boolean")) {
				pin.pinMode = this->getPinMode(current, INPUT);
				this->getPinConfig(current, pin);
				pinMode(pin.pin, INPUT);
                switch (pin.pinMode) {
                    case INPUT:
                        pullUpDnControl(pin.pin, PUD_OFF);
                        break;
                    case INPUT_PULLUP:
                        pullUpDnControl(pin.pin, PUD_UP);
                        break;
                    case INPUT_PULLDOWN:
                        pullUpDnControl(pin.pin, PUD_DOWN);
                        break;
                    default:
                        break;
                }
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
				this->eraPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
			}
		}
	}
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::handleWritePin(cJSON* root) {
	if (!cJSON_IsArray(root)) {
		return;
	}

	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr; current = current->next) {
		if (!cJSON_IsObject(current)) {
			continue;
		}
		pin = PinConfig_t();
		cJSON* item = cJSON_GetObjectItem(current, "config_id");
		if (cJSON_IsNumber(item)) {
			pin.configId = item->valueint;
		}
		item = cJSON_GetObjectItem(current, "pin_number");
		if (cJSON_IsNumber(item)) {
			pin.pin = item->valueint;
		}
		if (pin.pin > MAX_GPIO_WIRING_PI) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
			if (ERaStrCmp(item->valuestring, "boolean")) {
				pin.pinMode = this->getPinMode(current, OUTPUT);
				this->getPinConfig(current, pin);
				pinMode(pin.pin, pin.pinMode);
                if (pin.pinMode == OUTPUT) {
                    this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
                                                    pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                    pin.configId);
                }
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				pin.pinMode = this->getPinMode(current, PWM);
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
                this->eraPinReport.setPinReport(pin.pin, pin.pinMode, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
			}
		}
	}
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::processArduinoPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	if (arrayTopic.size() != 3) {
		return;
	}
	const std::string& str = arrayTopic.at(2);
	if (!this->isDigit(str)) {
		return;
	}
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}
	uint8_t pin = atoi(str.c_str());
	cJSON* item = cJSON_GetObjectItem(root, "value");
	if (cJSON_IsNumber(item) && (pin <= MAX_GPIO_WIRING_PI)) {
		ERaParam param;
		float value = item->valuedouble;
		const ERaReport::ScaleData_t* scale = this->eraPinReport.findScale(pin);
		if ((scale != nullptr) && scale->enable) {
			value = ERaMapNumberRange((float)item->valuedouble, scale->min, scale->max, scale->rawMin, scale->rawMax);
		}
		switch (this->eraPinReport.findPinMode(pin)) {
			case PWM:
                pinMode(pin, PWM_OUTPUT);
                ::pwmWrite(pin, value);
                break;
			case ANALOG:
                ::analogWrite(pin, value);
				break;
			default:
				pinMode(pin, OUTPUT);
				if (value == TOGGLE) {
					::digitalWrite(pin, !digitalRead(pin));
				}
				else {
					::digitalWrite(pin, value ? HIGH : LOW);
				}
				break;
		}
		param = item->valuedouble;
		this->callERaPinWriteHandler(pin, param);
	}

	cJSON_Delete(root);
	root = nullptr;
	item = nullptr;
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::handlePinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}

	ERaParam param;
	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
		if (this->getGPIOPin(current, "virtual_pin", pin.pin)) {
			param = current->valuedouble;
			this->callERaWriteHandler(pin.pin, param);
			continue;
		}
        if (getGPIOPin(current, "pin_mode", pin.pin)) {
            if (!cJSON_IsString(current)) {
                continue;
			}
			if (pin.pin > MAX_GPIO_WIRING_PI) {
				continue;
			}

			this->getReportConfig(root, pin);

            if (ERaStrCmp(current->valuestring, "output")) {
				pinMode(pin.pin, OUTPUT);
            }
			else if (ERaStrCmp(current->valuestring, "open_drain")) {
				pinMode(pin.pin, OUTPUT);
			}
            else if (ERaStrCmp(current->valuestring, "pwm")) {
                this->eraPinReport.setPinReport(pin.pin, PWM, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId);
            }
            else if (ERaStrCmp(current->valuestring, "input")) {
				pinMode(pin.pin, INPUT);
                pullUpDnControl(pin.pin, PUD_OFF);
				this->eraPinReport.setPinReport(pin.pin, INPUT, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pullup")) {
				pinMode(pin.pin, INPUT);
                pullUpDnControl(pin.pin, PUD_UP);
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLUP, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pulldown")) {
				pinMode(pin.pin, INPUT);
                pullUpDnControl(pin.pin, PUD_DOWN);
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLDOWN, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (ERaStrCmp(current->valuestring, "analog")) {
				this->eraPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (ERaStrCmp(current->valuestring, "remove")) {
				this->eraPinReport.deleteWithPin(pin.pin);
			}
            continue;
        }
        if (getGPIOPin(current, "digital_pin", pin.pin)) {
		    ERaParam param;
            pinMode(pin.pin, OUTPUT);
			if (current->valueint == TOGGLE) {
				::digitalWrite(pin.pin, !digitalRead(pin.pin));
			}
			else {
            	::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
			}
			param = current->valueint;
            this->digitalWrite(pin.pin, digitalRead(pin.pin));
		    this->callERaPinWriteHandler(pin.pin, param);
            continue;
        }
		if (getGPIOPin(current, "pwm_pin", pin.pin)) {
            pinMode(pin.pin, PWM_OUTPUT);
            ::pwmWrite(pin.pin, current->valueint);
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, static_cast<Proto*>(this)->ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, "Linux");
    cJSON_AddStringToObject(root, INFO_BSSID, "Linux");
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, "Linux");
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, "Linux");
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, "Linux");
}

#endif /* INC_ERA_API_PI_HPP_ */
