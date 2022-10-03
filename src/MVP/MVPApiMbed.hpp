#ifndef INC_MVP_API_MBED_HPP_
#define INC_MVP_API_MBED_HPP_

#include <MVP/MVPApi.hpp>
#include <mbed.h>

#define MVP_MODEL_TYPE                "MVP"
#define MVP_BOARD_TYPE                "Mbed"

static uint16_t analogRead(uint8_t pin) {
    AnalogIn p((PinName)pin);
    return uint16_t(p.read() * 4095);
}

static void pwmWrite(uint8_t pin, uint8_t value) {
    PwmOut p((PinName)pin);
    p.write(value / 100.0f);
}

static int digitalRead(uint8_t pin) {
    DigitalIn p((PinName)pin);
    return int(p);
}

static int digitalOutRead(uint8_t pin) {
    DigitalOut p((PinName)pin);
    return int(p);
}

static void digitalWrite(uint8_t pin, int value) {
    DigitalOut p((PinName)pin);
    p = value;
}

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
				this->mvpPinReport.setPinReport(pin.pin, pin.pinMode, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (MVPStrCmp(item->valuestring, "integer")) {
				pin.report = PinConfig_t::__ReportConfig_t(1000, 1000, 60000, 10.0f);
				this->getPinConfig(current, pin);
				this->mvpPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
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
				this->mvpPinReport.setPinReport(pin.pin, pin.pinMode, digitalOutRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
			}
			else if (MVPStrCmp(item->valuestring, "integer")) {
				pin.pinMode = this->getPinMode(current, PWM);
				this->getPinConfig(current, pin);
                if (pin.pinMode == PWM) {
                    PwmOut p((PinName)pin);
                    p.period(1.0f / pin.frequency);
                    p.write(0);
                }
                this->mvpPinReport.setPinReport(pin.pin, pin.pinMode, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId);
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
		uint8_t pin = std::stoi(str);
		switch (this->mvpPinReport.findPinMode(pin)) {
			case PWM:
			case ANALOG:
                ::pwmWrite(pin, item->valueint);
				break;
			default:
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
                PwmOut p((PinName)pin);
                p.period(1.0f / pin.frequency);
                p.write(0);
                this->mvpPinReport.setPinReport(pin.pin, PWM, nullptr, pin.report.interval,
                                                pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
                                                pin.configId);
            }
            else if (MVPStrCmp(current->valuestring, "input")) {
				this->mvpPinReport.setPinReport(pin.pin, INPUT, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (MVPStrCmp(current->valuestring, "pullup")) {
				this->mvpPinReport.setPinReport(pin.pin, INPUT_PULLUP, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
            else if (MVPStrCmp(current->valuestring, "pulldown")) {
				this->mvpPinReport.setPinReport(pin.pin, INPUT_PULLDOWN, digitalRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinCb);
            }
			else if (MVPStrCmp(current->valuestring, "analog")) {
				this->mvpPinReport.setPinReport(pin.pin, ANALOG, analogRead, pin.report.interval,
												pin.report.minInterval, pin.report.maxInterval, pin.report.reportableChange, this->reportPinConfigCb,
												pin.configId);
            }
			else if (MVPStrCmp(current->valuestring, "remove")) {
				this->mvpPinReport.deleteWithPin(pin.pin);
			}
            continue;
        }
        if (getGPIOPin(current, "digital_pin", pin.pin)) {
			MVPParam param;
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
            ::pwmWrite(pin.pin, current->valueint);
			continue;
		}
	}

	cJSON_Delete(root);
	root = nullptr;
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::addInfo(cJSON* root) {
    cJSON_AddStringToObject(root, "board", MVP_BOARD_TYPE);
    cJSON_AddStringToObject(root, "model", MVP_MODEL_TYPE);
	cJSON_AddStringToObject(root, "template_id", static_cast<Proto*>(this)->MVP_AUTH);
    cJSON_AddStringToObject(root, "firmware_version", MVP_FIRMWARE_VERSION);
    cJSON_AddStringToObject(root, "ssid", "");
    cJSON_AddStringToObject(root, "bssid", "");
    cJSON_AddNumberToObject(root, "rssi", 100);
    cJSON_AddStringToObject(root, "mac", "");
    cJSON_AddStringToObject(root, "ip", "");
}

template <class Proto, class Flash>
void MVPApi<Proto, Flash>::addModbusInfo(cJSON* root) {
	cJSON_AddNumberToObject(root, "chip_temperature", 5000);
	cJSON_AddNumberToObject(root, "temperature", 0);
	cJSON_AddNumberToObject(root, "voltage", 999);
	cJSON_AddNumberToObject(root, "is_battery", 0);
	cJSON_AddNumberToObject(root, "rssi", 100);
	cJSON_AddStringToObject(root, "wifi_is_using", "");
}

#endif /* INC_MVP_API_MBED_HPP_ */

