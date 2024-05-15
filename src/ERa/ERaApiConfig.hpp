#ifndef INC_ERA_API_CONFIG_HPP_
#define INC_ERA_API_CONFIG_HPP_

#define ERA_SET_PIN_REPORT(mode, read)                                                              \
    this->ERaPinRp.setPinReport(pin.pin, (mode), (read),                                            \
                                pin.report.interval, pin.report.minInterval,                        \
                                pin.report.maxInterval, pin.report.reportableChange,                \
                                this->reportPinConfigCb, pin.configId).setScale(pin.scale.min,      \
                                pin.scale.max, pin.scale.rawMin, pin.scale.rawMax)

#define ERA_SET_PWM_REPORT(mode, read)                                                              \
    this->ERaPinRp.setPWMPinReport(pin.pin, (mode), pin.pwm.channel, (read),                        \
                                    pin.report.interval, pin.report.minInterval,                    \
                                    pin.report.maxInterval, pin.report.reportableChange,            \
                                    this->reportPinConfigCb, pin.configId).setScale(pin.scale.min,  \
                                    pin.scale.max, pin.scale.rawMin, pin.scale.rawMax)

#define ERA_SET_DEBUG_PIN_REPORT(mode, read)                                                        \
    this->ERaPinRp.setPinReport(pin.pin, (mode), (read),                                            \
                                pin.report.interval, pin.report.minInterval,                        \
                                pin.report.maxInterval, pin.report.reportableChange,                \
                                this->reportPinCb)

#define ERA_SET_DEBUG_PWM_REPORT(mode, read)                                                        \
    this->ERaPinRp.setPWMPinReport(pin.pin, (mode), pin.pwm.channel, (read),                        \
                                    pin.report.interval, pin.report.minInterval,                    \
                                    pin.report.maxInterval, pin.report.reportableChange,            \
                                    this->reportPinCb)

#endif /* INC_ERA_API_CONFIG_HPP_ */
