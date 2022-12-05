#ifndef INC_ERA_API_ARDUINO_HPP_
#define INC_ERA_API_ARDUINO_HPP_

#include <ERa/ERaApi.hpp>

#if defined(STM32F4xx)

	#undef ERA_DECODE_PIN_NAME
	#define ERA_DECODE_PIN_NAME(pin) ((stringToPinName(pin) != PinName::NC) ? \
									pinNametoDigitalPin(stringToPinName(pin)) : \
									(((pin[0] == 'a') || (pin[0] == 'A')) ? ERA_DECODE_PIN(atoi(pin + 1)) : atoi(pin)))

	inline
	static PinName stringToPinName(std::string str) {
		if (str.size() < 3) {
			return PinName::NC;
		}

		PinName p = PinName::NC;
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
			return std::tolower(c);
		});

		if (str.at(0) != 'p' || !isdigit(str.at(2))) {
			return PinName::NC;
		}

		switch (str.at(1)) {
			case 'a':
				p = (PinName)(PortName::PortA + atoi(&str.at(2)));
				break;
			case 'b':
				p = (PinName)(PortName::PortB + atoi(&str.at(2)));
				break;
	#if defined GPIOC_BASE
			case 'c':
				p = (PinName)(PortName::PortC + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOD_BASE
			case 'd':
				p = (PinName)(PortName::PortD + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOE_BASE
			case 'e':
				p = (PinName)(PortName::PortE + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOF_BASE
			case 'f':
				p = (PinName)(PortName::PortF + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOG_BASE
			case 'g':
				p = (PinName)(PortName::PortG + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOH_BASE
			case 'h':
				p = (PinName)(PortName::PortH + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOI_BASE
			case 'i':
				p = (PinName)(PortName::PortI + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOJ_BASE
			case 'j':
				p = (PinName)(PortName::PortJ + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOK_BASE
			case 'k':
				p = (PinName)(PortName::PortK + atoi(&str.at(2)));
				break;
	#endif
	#if defined GPIOZ_BASE
			case 'z':
				p = (PinName)(PortName::PortZ + atoi(&str.at(2)));
				break;
	#endif
			default:
				return PinName::NC;
		}

		return p;
	}

#endif

inline
static void pinModeArduino(uint8_t pin, uint8_t mode) {
	pinMode(pin, mode);
}

inline
static int digitalReadArduino(uint8_t pin) {
	return digitalRead(pin);
}

inline
static int analogReadArduino(uint8_t pin) {
	return analogRead(pin);
}

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
				pinModeArduino(pin.pin, pin.pinMode);
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalReadArduino, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
#if defined(ESP32)
				pinModeArduino(pin.pin, ANALOG);
#elif defined(STM32F4xx)
				if (!digitalpinIsAnalogInput(pin.pin)) {
					continue;
				}
				pin.pin = digitalPinToAnalogInput(pin.pin) + PNUM_ANALOG_BASE;
				pinModeArduino(pin.pin, ANALOG);
#endif
				this->eraPinReport.setPinReport(pin.pin, ANALOG, analogReadArduino, pin.report.interval,
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
			pin.pin = item->valueint;
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
				pinModeArduino(pin.pin, pin.pinMode);
				this->eraPinReport.setPinReport(pin.pin, pin.pinMode, digitalReadArduino, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (ERaStrCmp(item->valuestring, "integer")) {
				this->getPinConfig(current, pin);
				this->getScaleConfig(current, pin);
                pinModeArduino(pin.pin, OUTPUT);
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
			case ANALOG:
                ::analogWrite(pin, value);
				if (rp != nullptr) {
					rp->updateReport(value);
				}
				break;
			case VIRTUAL:
			case ERA_VIRTUAL:
				this->callERaWriteHandler(pin, param);
				break;
			default:
				pinModeArduino(pin, OUTPUT);
				if (value == TOGGLE) {
					::digitalWrite(pin, ((digitalReadArduino(pin) == LOW) ? HIGH : LOW));
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
				pinModeArduino(pin.pin, OUTPUT);
            }
			else if (ERaStrCmp(current->valuestring, "open_drain")) {
				pinModeArduino(pin.pin, OUTPUT_OPEN_DRAIN);
			}
            else if (ERaStrCmp(current->valuestring, "pwm")) {
                pinModeArduino(pin.pin, OUTPUT);
                this->eraPinReport.setPinReport(pin.pin, PWM, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId);
            }
            else if (ERaStrCmp(current->valuestring, "input")) {
				pinModeArduino(pin.pin, INPUT);
				this->eraPinReport.setPinReport(pin.pin, INPUT, digitalReadArduino, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pullup")) {
				pinModeArduino(pin.pin, INPUT_PULLUP);
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLUP, digitalReadArduino, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (ERaStrCmp(current->valuestring, "pulldown")) {
				pinModeArduino(pin.pin, INPUT_PULLDOWN);
				this->eraPinReport.setPinReport(pin.pin, INPUT_PULLDOWN, digitalReadArduino, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (ERaStrCmp(current->valuestring, "analog")) {
#if defined(ESP32)
				pinModeArduino(pin.pin, ANALOG);
#elif defined(STM32F4xx)
				if (!digitalpinIsAnalogInput(pin.pin)) {
					continue;
				}
				pin.pin = digitalPinToAnalogInput(pin.pin) + PNUM_ANALOG_BASE;
				pinModeArduino(pin.pin, ANALOG);
#endif
				this->eraPinReport.setPinReport(pin.pin, ANALOG, analogReadArduino, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (ERaStrCmp(current->valuestring, "remove")) {
				this->eraPinReport.deleteWithPin(pin.pin);
			}
            continue;
        }
        if (getGPIOPin(current, "digital_pin", pin.pin)) {
			ERaParam param(current->valueint);
            pinModeArduino(pin.pin, OUTPUT);
			if (current->valueint == TOGGLE) {
				::digitalWrite(pin.pin, ((digitalReadArduino(pin.pin) == LOW) ? HIGH : LOW));
			}
			else {
            	::digitalWrite(pin.pin, current->valueint ? HIGH : LOW);
			}
            this->digitalWrite(pin.pin, digitalReadArduino(pin.pin));
			this->callERaPinWriteHandler(pin.pin, param, param);
            continue;
        }
		if (getGPIOPin(current, "pwm_pin", pin.pin)) {
            ::analogWrite(pin.pin, current->valueint);
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
}

#endif /* INC_ERA_API_ARDUINO_HPP_ */
