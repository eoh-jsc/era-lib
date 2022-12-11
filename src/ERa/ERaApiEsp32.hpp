#ifndef INC_ERA_API_ESP32_HPP_
#define INC_ERA_API_ESP32_HPP_

#include <ERa/ERaApi.hpp>

#ifndef SOC_ADC_MAX_CHANNEL_NUM
	#define SOC_ADC_MAX_CHANNEL_NUM                 (10)
#endif

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
			pin.pin = ERA_DECODE_PIN_NUMBER(item->valueint);
		}
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
		pin.pinMode = this->getPinMode(current);
		if (pin.pinMode == VIRTUAL) {
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
			continue;
		}
		if (!this->isReadPinMode(pin.pinMode)) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
#if defined(FORCE_VIRTUAL_PIN)
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
#else
			if (ERaStrCmp(item->valuestring, "boolean")) {
				this->getPinConfig(current, pin);
				pinMode(pin.pin, pin.pinMode);
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
				uint8_t adcUnit = (digitalPinToAnalogChannel(pin.pin) / SOC_ADC_MAX_CHANNEL_NUM);
				if (adcUnit == 0) {
					pinMode(pin.pin, ANALOG);
					this->eraPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
													pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
													pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
				}
			}
#endif
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
			pin.pin = ERA_DECODE_PIN_NUMBER(item->valueint);
		}
        else if (cJSON_IsString(item)) {
            pin.pin = ERA_DECODE_PIN_NAME(item->valuestring);
        }
		pin.pinMode = this->getPinMode(current);
		if (pin.pinMode == VIRTUAL) {
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
			continue;
		}
		if (this->isReadPinMode(pin.pinMode)) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
#if defined(FORCE_VIRTUAL_PIN)
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
#else
			if (ERaStrCmp(item->valuestring, "boolean")) {
				this->getPinConfig(current, pin);
				pinMode(pin.pin, pin.pinMode);
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				pin.pwm.channel = this->eraPinReport.findChannelFree();
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
				if (pin.pwm.channel >= 0) {
					ledcSetup(pin.pwm.channel, pin.pwm.frequency, pin.pwm.resolution);
					ledcDetachPin(pin.pin);
					ledcAttachPin(pin.pin, pin.pwm.channel);
					this->eraPinReport.setPWMPinReport(pin.pin, PWM, pin.pwm.channel, ledcRead,
														pin.report.interval, pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange,
														this->reportPinConfigCb, pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
				}
			}
#endif
		}
	}
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::processArduinoPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	if (arrayTopic.size() != 3) {
		return;
	}
	const std::string& str = arrayTopic.at(2);
	if (str.empty()) {
		return;
	}
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}
	uint8_t pin = ERA_DECODE_PIN_NAME(str.c_str());
	cJSON* item = cJSON_GetObjectItem(root, "value");
	if (cJSON_IsNumber(item)) {
		ERaParam raw;
		ERaParam param(item->valuedouble);
		int8_t channel {0};
		float value = item->valuedouble;
		int pMode = this->eraPinReport.findPinMode(pin);
		const ERaReport::ScaleData_t* scale = this->eraPinReport.findScale(pin);
		if ((scale != nullptr) && scale->enable) {
			value = ERaMapNumberRange((float)item->valuedouble, scale->min, scale->max, scale->rawMin, scale->rawMax);
		}
		switch (pMode) {
			case PWM:
			case ANALOG:
				channel = this->eraPinReport.findChannelPWM(pin);
				if (channel >= 0) {
					ledcWrite(channel, value);
				}
				break;
			case VIRTUAL:
			case ERA_VIRTUAL:
				this->callERaWriteHandler(pin, param);
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
		if (pMode != ERA_VIRTUAL) {
			raw = value;
			this->callERaPinWriteHandler(pin, param, raw);
		}
	}
	else if (cJSON_IsString(item)) {
		ERaParam param;
		param.add_static(item->valuestring);
		this->callERaWriteHandler(pin, param);
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
			if (cJSON_IsNumber(current)) {
				param = current->valuedouble;
			}
			else if (cJSON_IsString(current)) {
				param.add_static(current->valuestring);
			}
			this->callERaWriteHandler(pin.pin, param);
			continue;
		}
        if (getGPIOPin(current, "pin_mode", pin.pin)) {
            if (!cJSON_IsString(current)) {
                continue;
			}

			this->getReportConfig(root, pin);

            if (ERaStrCmp(current->valuestring, "output")) {
				pinMode(pin.pin, OUTPUT);
            }
			else if (ERaStrCmp(current->valuestring, "open_drain")) {
				pinMode(pin.pin, OUTPUT_OPEN_DRAIN);
			}
            else if (ERaStrCmp(current->valuestring, "pwm")) {
				ledcSetup(pin.pwm.channel, pin.pwm.frequency, pin.pwm.resolution);
				ledcDetachPin(pin.pin);
				ledcAttachPin(pin.pin, pin.pwm.channel);
				this->eraPinReport.setPWMPinReport(pin.pin, PWM, pin.pwm.channel, ledcRead,
													pin.report.interval, pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange,
													this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "input")) {
				pinMode(pin.pin, INPUT);
				this->eraPinReport.setPinReport(pin.pin, INPUT, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pullup")) {
				pinMode(pin.pin, INPUT_PULLUP);
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLUP, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pulldown")) {
				pinMode(pin.pin, INPUT_PULLDOWN);
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLDOWN, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (ERaStrCmp(current->valuestring, "analog")) {
				uint8_t adcUnit = (digitalPinToAnalogChannel(pin.pin) / SOC_ADC_MAX_CHANNEL_NUM);
				if (adcUnit == 0) {
					pinMode(pin.pin, ANALOG);
					this->eraPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
													pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
				}
            }
			else if (ERaStrCmp(current->valuestring, "remove")) {
				this->eraPinReport.deleteWithPin(pin.pin);
			}
            continue;
        }
        if (getGPIOPin(current, "digital_pin", pin.pin)) {
			ERaParam param(current->valueint);
            pinMode(pin.pin, OUTPUT);
			if (current->valueint == TOGGLE) {
				::digitalWrite(pin.pin, !digitalRead(pin.pin));
			}
			else {
            	::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
			}
            this->digitalWrite(pin.pin, digitalRead(pin.pin));
			this->callERaPinWriteHandler(pin.pin, param, param);
            continue;
        }
		if (getGPIOPin(current, "pwm_pin", pin.pin)) {
			pin.pwm.channel = this->eraPinReport.findChannelPWM(pin.pin);
			if (pin.pwm.channel >= 0) {
				ledcWrite(pin.pwm.channel, current->valueint);
			}
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
	ERA_FORCE_UNUSED(arrayTopic);
}

#endif /* INC_ERA_API_ESP32_HPP_ */
