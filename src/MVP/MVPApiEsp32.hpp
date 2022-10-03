#ifndef INC_MVP_API_ESP32_HPP_
#define INC_MVP_API_ESP32_HPP_

#include <MVP/MVPApi.hpp>

#ifndef SOC_ADC_MAX_CHANNEL_NUM
	#define SOC_ADC_MAX_CHANNEL_NUM                 (10)
#endif

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::handleReadPin(cJSON* root) {
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
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
			if (MVPStrCmp(item->valuestring, "boolean")) {
				pin.pinMode = this->getPinMode(current, INPUT);
				this->getPinConfig(current, pin);
				pinMode(pin.pin, pin.pinMode);
				this->mvpPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (MVPStrCmp(item->valuestring, "integer")) {
				pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
				this->getPinConfig(current, pin);
				uint8_t adcUnit = (digitalPinToAnalogChannel(pin.pin) / SOC_ADC_MAX_CHANNEL_NUM);
				if (adcUnit == 0) {
					pinMode(pin.pin, ANALOG);
					this->mvpPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
													pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
													pin.configId);
				}
			}
		}
	}
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::handleWritePin(cJSON* root) {
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
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
			if (MVPStrCmp(item->valuestring, "boolean")) {
				pin.pinMode = this->getPinMode(current, OUTPUT);
				this->getPinConfig(current, pin);
				pinMode(pin.pin, pin.pinMode);
				this->mvpPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (MVPStrCmp(item->valuestring, "integer")) {
				pin.pwm.channel = this->mvpPinReport.findChannelFree();
				this->getPinConfig(current, pin);
				if (pin.pwm.channel >= 0) {
					ledcSetup(pin.pwm.channel, pin.pwm.frequency, pin.pwm.resolution);
					ledcAttachPin(pin.pin, pin.pwm.channel);
					this->mvpPinReport.setPWMPinReport(pin.pin, PWM, pin.pwm.channel, ledcRead,
														pin.report.interval, pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange,
														this->reportPinConfigCb, pin.configId);
				}
			}
		}
	}
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::processArduinoPinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
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
	cJSON* item = cJSON_GetObjectItem(root, "value");
	if (cJSON_IsNumber(item)) {
		MVPParam param;
		int8_t channel {0};
		uint8_t pin = atoi(str.c_str());
		switch (this->mvpPinReport.findPinMode(pin)) {
			case PWM:
			case ANALOG:
				channel = this->mvpPinReport.findChannelPWM(pin);
				if (channel >= 0) {
					ledcWrite(channel, item->valueint);
				}
				break;
			default:
				pinMode(pin, OUTPUT);
				if (item->valueint == TOGGLE) {
					::digitalWrite(pin, !digitalRead(pin));
				}
				else {
					::digitalWrite(pin, item->valueint ? HIGH : LOW);
				}
				break;
		}
		param = item->valueint;
		this->callMVPPinWriteHandler(pin, param);
	}

	cJSON_Delete(root);
	root = nullptr;
	item = nullptr;
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::handlePinRequest(const std::vector<std::string>& arrayTopic, const std::string& payload) {
	cJSON* root = cJSON_Parse(payload.c_str());
	if (!cJSON_IsObject(root)) {
		cJSON_Delete(root);
		root = nullptr;
		return;
	}

	MVPParam param;
	PinConfig_t pin {};
	cJSON* current = nullptr;

	for (current = root->child; current != nullptr && current->string != nullptr; current = current->next) {
		if (this->getGPIOPin(current, "virtual_pin", pin.pin)) {
			param = current->valuedouble;
			this->callMVPWriteHandler(pin.pin, param);
			continue;
		}
        if (getGPIOPin(current, "pin_mode", pin.pin)) {
            if (!cJSON_IsString(current)) {
                continue;
			}

			cJSON* item = cJSON_GetObjectItem(root, "channel");
			if (cJSON_IsNumber(item)) {
				pin.pwm.channel = item->valueint;
			}
			item = cJSON_GetObjectItem(root, "frequency");
			if (cJSON_IsNumber(item)) {
				pin.pwm.frequency = item->valueint;
			}
			item = cJSON_GetObjectItem(root, "resolution");
			if (cJSON_IsNumber(item)) {
				pin.pwm.resolution = item->valueint;
			}
			item = cJSON_GetObjectItem(root, "interval");
			if (cJSON_IsNumber(item)) {
				pin.report.interval = item->valueint;
			}
			item = cJSON_GetObjectItem(root, "min_interval");
			if (cJSON_IsNumber(item)) {
				pin.report.minInterval = item->valueint;
			}
			item = cJSON_GetObjectItem(root, "max_interval");
			if (cJSON_IsNumber(item)) {
				pin.report.maxInterval = item->valueint;
			}
			item = cJSON_GetObjectItem(root, "reportable_change");
			if (cJSON_IsNumber(item)) {
				pin.report.reportableChange = ((item->valuedouble) == 0 ? 1.0f : item->valuedouble);
			}

            if (MVPStrCmp(current->valuestring, "output")) {
				pinMode(pin.pin, OUTPUT);
            }
			else if (MVPStrCmp(current->valuestring, "open_drain")) {
				pinMode(pin.pin, OUTPUT_OPEN_DRAIN);
			}
            else if (MVPStrCmp(current->valuestring, "pwm")) {
				ledcSetup(pin.pwm.channel, pin.pwm.frequency, pin.pwm.resolution);
				ledcAttachPin(pin.pin, pin.pwm.channel);
				this->mvpPinReport.setPWMPinReport(pin.pin, PWM, pin.pwm.channel, ledcRead,
													pin.report.interval, pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange,
													this->reportPinCb);
            }
            else if (MVPStrCmp(current->valuestring, "input")) {
				pinMode(pin.pin, INPUT);
				this->mvpPinReport.setPinReport(pin.pin, INPUT, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (MVPStrCmp(current->valuestring, "pullup")) {
				pinMode(pin.pin, INPUT_PULLUP);
				this->mvpPinReport.setPinReport(pin.pin, INPUT_PULLUP, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (MVPStrCmp(current->valuestring, "pulldown")) {
				pinMode(pin.pin, INPUT_PULLDOWN);
				this->mvpPinReport.setPinReport(pin.pin, INPUT_PULLDOWN, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (MVPStrCmp(current->valuestring, "analog")) {
				uint8_t adcUnit = (digitalPinToAnalogChannel(pin.pin) / SOC_ADC_MAX_CHANNEL_NUM);
				if (adcUnit == 0) {
					pinMode(pin.pin, ANALOG);
					this->mvpPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
													pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
				}
            }
			else if (MVPStrCmp(current->valuestring, "remove")) {
				this->mvpPinReport.deleteWithPin(pin.pin);
			}
            continue;
        }
        if (getGPIOPin(current, "digital_pin", pin.pin)) {
			MVPParam param;
            pinMode(pin.pin, OUTPUT);
			if (current->valueint == TOGGLE) {
				::digitalWrite(pin.pin, !digitalRead(pin.pin));
			}
			else {
            	::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
			}
			param = current->valueint;
            this->digitalWrite(pin.pin, digitalRead(pin.pin));
			this->callMVPPinWriteHandler(pin.pin, param);
            continue;
        }
		if (getGPIOPin(current, "pwm_pin", pin.pin)) {
			pin.pwm.channel = this->mvpPinReport.findChannelPWM(pin.pin);
			if (pin.pwm.channel >= 0) {
				ledcWrite(pin.pwm.channel, current->valueint);
			}
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
}

#endif /* INC_MVP_API_ESP32_HPP_ */