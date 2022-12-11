#ifndef INC_ERA_API_WIRING_PI_HPP_
#define INC_ERA_API_WIRING_PI_HPP_

#include <ERa/ERaApi.hpp>

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
		if (pin.pin > MAX_GPIO_WIRING_PI) {
			continue;
		}
		item = cJSON_GetObjectItem(current, "value_type");
		if (cJSON_IsString(item)) {
#if defined(FORCE_VIRTUAL_PIN)
			this->eraPinReport.setPinVirtual(pin.pin, pin.configId);
#else
			if (ERaStrCmp(item->valuestring, "boolean")) {
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
		if (pin.pin > MAX_GPIO_WIRING_PI) {
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
                if (pin.pinMode == OUTPUT) {
                    this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
                                                    pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                    pin.configId);
                }
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
                this->eraPinReport.setPinReport(pin.pin, PWM, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId).setScale(pin.scale.min, pin.scale.max, pin.scale.rawMin, pin.scale.rawMax);
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
		float value = item->valuedouble;
		int pMode = this->eraPinReport.findPinMode(pin);
		const ERaReport::iterator* rp = this->eraPinReport.getReport(pin);
		if (rp != nullptr) {
			const ERaReport::ScaleData_t* scale = rp->getScale();
			if ((scale != nullptr) && scale->enable) {
				value = ERaMapNumberRange((float)item->valuedouble, scale->min, scale->max, scale->rawMin, scale->rawMax);
			}
		}
		switch (pMode) {
			case PWM:
                pinMode(pin, PWM_OUTPUT);
                ::pwmWrite(pin, value);
				if (rp != nullptr) {
					rp->updateReport(value);
				}
                break;
			case ANALOG:
                ::analogWrite(pin, value);
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
            pinMode(pin.pin, PWM_OUTPUT);
            ::pwmWrite(pin.pin, current->valueint);
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
	ERA_FORCE_UNUSED(arrayTopic);
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, INFO_BOARD, ERA_BOARD_TYPE);
    cJSON_AddStringToObject(root, INFO_MODEL, ERA_MODEL_TYPE);
	cJSON_AddStringToObject(root, INFO_AUTH_TOKEN, this->thisProto().ERA_AUTH);
    cJSON_AddStringToObject(root, INFO_FIRMWARE_VERSION, ERA_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, INFO_SSID, ((this->thisProto().transp.getSSID() == nullptr) ?
                                            ERA_PROTO_TYPE : this->thisProto().transp.getSSID()));
    cJSON_AddStringToObject(root, INFO_BSSID, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_RSSI, 100);
    cJSON_AddStringToObject(root, INFO_MAC, ERA_PROTO_TYPE);
    cJSON_AddStringToObject(root, INFO_LOCAL_IP, ERA_PROTO_TYPE);
    cJSON_AddNumberToObject(root, INFO_PING, this->thisProto().transp.getPing());
}

template <class Proto, class Flash>
void ERaApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, INFO_MB_CHIP_TEMPERATURE, 5000);
	cJSON_AddNumberToObject(root, INFO_MB_TEMPERATURE, 0);
	cJSON_AddNumberToObject(root, INFO_MB_VOLTAGE, 999);
	cJSON_AddNumberToObject(root, INFO_MB_IS_BATTERY, 0);
	cJSON_AddNumberToObject(root, INFO_MB_RSSI, 100);
	cJSON_AddStringToObject(root, INFO_MB_WIFI_USING, ((this->thisProto().transp.getSSID() == nullptr) ?
                                            		ERA_PROTO_TYPE : this->thisProto().transp.getSSID()));
}

#endif /* INC_ERA_API_WIRING_PI_HPP_ */
