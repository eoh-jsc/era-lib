#ifndef INC_MVP_PIN_HPP_
#define INC_MVP_PIN_HPP_

#include <functional>
#include <MVP/MVPDefine.hpp>
#include <Utility/MVPUtility.hpp>
#include <MVP/MVPReport.hpp>

template <class Report>
class MVPPin
{
    typedef std::function<int(uint8_t)> ReadPinHandler_t;

    const static int MAX_CHANNELS = 16;
	const static int MAX_PINS = 16;
    typedef struct __Pin_t {
        unsigned long prevMillis;
        unsigned long delay;
        MVPPin::ReadPinHandler_t readPin;
        typename Report::iterator report;
        uint8_t pin;
        uint8_t pinMode;
        uint8_t channel; /* for pwm mode */
        bool enable;
    } Pin_t;

public:
    class iterator
    {
    public:
        iterator()
            : pPin(nullptr)
            , id(-1)
        {}
        iterator(MVPPin* _pPin, int _id)
            : pPin(_pPin)
            , id(_id)
        {}
        
		operator int() const {
			return this->id;
		}

		operator bool() const {
			return this->isValid();
		}
        
        void operator() (void) const {
            if (!this->isValid()) {
                return;
            }
            this->pPin->executeNow(this->id);
        }

		bool isValid() const {
			return ((this->pPin != nullptr) && (this->id >= 0));
		}

        bool changeInterval(unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange) {
            if (!this->isValid()) {
                return false;
            }
            return this->re->changeInterval(this->id, interval, minInterval, maxInterval, minChange);
        }

        void restartPin() {
            if (!this->isValid()) {
                return;
            }
            this->re->restartPin(this->id);
        }

        void deletePin() {
            if (!this->isValid()) {
                return;
            }
            this->re->deletePin(this->id);
            this->invalidate();
        }

        bool isEnable() {
            if (!this->isValid()) {
                return false;
            }
            return this->re->isEnable(this->id);
        }

        void enable() {
            if (!this->isValid()) {
                return;
            }
            this->re->enable(this->id);
        }

        void disable() {
            if (!this->isValid()) {
                return;
            }
            this->re->disable(this->id);
        }

    protected:
    private:
		void invalidate() {
			this->pPin = nullptr;
			this->id = -1;
		}

        MVPPin* pPin;
        int id;
    };

    MVPPin(Report& _report)
        : report(_report)
        , numPin(0)
    {}
    ~MVPPin()
    {}
    
    void run();

    iterator setPinReport(uint8_t _pin, uint8_t pinMode, MVPPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb) {
        return iterator(this, this->setupPinReport(_pin, pinMode, readPin, interval, minInterval, maxInterval, minChange, cb));
    }

    iterator setPinReport(uint8_t _pin, uint8_t pinMode, MVPPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb,
                        unsigned int configId) {
        return iterator(this, this->setupPinReport(_pin, pinMode, readPin, interval, minInterval, maxInterval, minChange, cb, configId));
    }

    iterator setPWMPinReport(uint8_t _pin, uint8_t pinMode, uint8_t channel, MVPPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb) {
        return iterator(this, this->setupPWMPinReport(_pin, pinMode, channel, readPin, interval, minInterval, maxInterval, minChange, cb));
    }

    iterator setPWMPinReport(uint8_t _pin, uint8_t pinMode, uint8_t channel, MVPPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb, unsigned int configId) {
        return iterator(this, this->setupPWMPinReport(_pin, pinMode, channel, readPin, interval, minInterval, maxInterval, minChange, cb, configId));
    }

	bool changeInterval(unsigned int id, unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange);
	void restartPin(unsigned int id);
    void executeNow(unsigned int id);
	void deletePin(unsigned int id);
    void deleteWithPin(uint8_t _pin);
    void deleteAll();
	bool isEnable(unsigned int id);
	void enable(unsigned int id);
	void disable(unsigned int id);
	void enableAll();
	void disableAll();
    int findPinMode(uint8_t _pin);
    int findChannelPWM(uint8_t _pin);
	int findChannelFree();

protected:
private:
    int setupPinReport(uint8_t _pin, uint8_t pinMode, MVPPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb);
    int setupPinReport(uint8_t _pin, uint8_t pinMode, MVPPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb,
                        unsigned int configId);
    int setupPWMPinReport(uint8_t _pin, uint8_t pinMode, uint8_t channel, MVPPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb);
    int setupPWMPinReport(uint8_t _pin, uint8_t pinMode, uint8_t channel, MVPPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb, unsigned int configId);
	int findPinFree();
    int findPinExist(uint8_t _pin);
    int findPinOfChannel(uint8_t channel);

	bool isValidPin(unsigned int id) {
		return this->pin[id].readPin != nullptr;
	}

    Report& report;
	Pin_t pin[MAX_PINS] {};
	unsigned int numPin;
};

template <class Report>
void MVPPin<Report>::run() {
	unsigned long currentMillis = MVPMillis();
	for (int i = 0; i < MAX_PINS; ++i) {
		if (!this->isValidPin(i)) {
			continue;
		}
		if (currentMillis - this->pin[i].prevMillis < this->pin[i].delay) {
			continue;
		}
        unsigned long skipTimes = (currentMillis - this->pin[i].prevMillis) / this->pin[i].delay;
        // update time
        this->pin[i].prevMillis += this->pin[i].delay * skipTimes;
        // call update data
        if (!this->pin[i].enable) {
            continue;
        }
        if (this->pin[i].pinMode != PWM) {
            this->pin[i].report.updateReport(this->pin[i].readPin(this->pin[i].pin));
        }
        else {
            this->pin[i].report.updateReport(this->pin[i].readPin(this->pin[i].channel));
        }
    }
    this->report.run();
}

template <class Report>
int MVPPin<Report>::setupPinReport(uint8_t _pin, uint8_t pinMode, MVPPin::ReadPinHandler_t readPin, unsigned long interval,
                                unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb) {
    int id = this->findPinExist(_pin);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        return -1;
    }
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = MVPMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, _pin, pinMode);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, _pin, pinMode);
    }
    this->pin[id].pin = _pin;
    this->pin[id].pinMode = pinMode;
    this->pin[id].channel = 0;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
int MVPPin<Report>::setupPinReport(uint8_t _pin, uint8_t pinMode, MVPPin::ReadPinHandler_t readPin, unsigned long interval,
                                unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb,
                                unsigned int configId) {
    int id = this->findPinExist(_pin);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        return -1;
    }
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = MVPMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, _pin, pinMode, configId);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, _pin, pinMode, configId);
    }
    this->pin[id].pin = _pin;
    this->pin[id].pinMode = pinMode;
    this->pin[id].channel = 0;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
int MVPPin<Report>::setupPWMPinReport(uint8_t _pin, uint8_t pinMode, uint8_t channel, MVPPin::ReadPinHandler_t readPin,
                                    unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                                    typename Report::ReportCallback_p_t cb) {
    int id = this->findPinExist(_pin);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        return -1;
    }
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = MVPMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, _pin, pinMode);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, _pin, pinMode);
    }
    this->pin[id].pin = _pin;
    this->pin[id].pinMode = pinMode;
    this->pin[id].channel = channel;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
int MVPPin<Report>::setupPWMPinReport(uint8_t _pin, uint8_t pinMode, uint8_t channel, MVPPin::ReadPinHandler_t readPin,
                                    unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                                    typename Report::ReportCallback_p_t cb, unsigned int configId) {
    int id = this->findPinExist(_pin);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        return -1;
    }
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = MVPMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, _pin, pinMode, configId);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, _pin, pinMode, configId);
    }
    this->pin[id].pin = _pin;
    this->pin[id].pinMode = pinMode;
    this->pin[id].channel = channel;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
bool MVPPin<Report>::changeInterval(unsigned int id, unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange) {
	if (id >= MAX_PINS) {
		return false;
	}

    if (!interval) {
        return false;
    }
    if (!minInterval) {
        return false;
    }
    if (maxInterval < minInterval) {
        return false;
    }

    this->pin[id].delay = interval;
    this->pin[id].report = this->report.changeReportableChange(minInterval, maxInterval, minChange);
    return true;
}

template <class Report>
void MVPPin<Report>::restartPin(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

    this->pin[id].report.restartReport();
    this->pin[id].prevMillis = MVPMillis();
}

template <class Report>
void MVPPin<Report>::executeNow(unsigned int id) {
    if (id >= MAX_PINS) {
        return;
    }

    this->pin[id].prevMillis = MVPMillis() - this->pin[id].delay;
    this->pin[id].report();
}

template <class Report>
void MVPPin<Report>::deletePin(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

	if (!this->numPin) {
		return;
	}
    
	if (this->isValidPin(id)) {
        this->pin[id].report.deleteReport();
		this->pin[id] = Pin_t();
		this->pin[id].prevMillis = MVPMillis();
		this->numPin--;
    }
}

template <class Report>
void MVPPin<Report>::deleteWithPin(uint8_t _pin) {
    int id = this->findPinExist(_pin);
    if (id < 0) {
        return;
    }

    this->deletePin(id);
}

template <class Report>
void MVPPin<Report>::deleteAll() {
	for (int i = 0; i < MAX_PINS; ++i) {
        this->deletePin(i);
    }
}

template <class Report>
bool MVPPin<Report>::isEnable(unsigned int id) {
	if (id >= MAX_PINS) {
		return false;
	}

	return this->pin[id].enable;
}

template <class Report>
void MVPPin<Report>::enable(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

	this->pin[id].enable = true;
    this->pin[id].report.enable();
}

template <class Report>
void MVPPin<Report>::disable(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

	this->pin[id].enable = false;
    this->pin[id].report.disable();
}

template <class Report>
void MVPPin<Report>::enableAll() {
	for (int i = 0; i < MAX_PINS; ++i) {
		if (this->isValidPin(i)) {
			this->pin[i].enable = true;
            this->pin[i].report.enable();
		}
	}
}

template <class Report>
void MVPPin<Report>::disableAll() {
	for (int i = 0; i < MAX_PINS; ++i) {
		if (this->isValidPin(i)) {
			this->pin[i].enable = false;
            this->pin[i].report.disable();
		}
	}
}

template <class Report>
int MVPPin<Report>::findPinFree() {
	if (this->numPin >= MAX_PINS) {
		return -1;
	}

	for (int i = 0; i < MAX_PINS; ++i) {
		if (!this->isValidPin(i)) {
			return i;
		}
	}
    
    return -1;
}

template <class Report>
int MVPPin<Report>::findPinExist(uint8_t _pin) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == _pin) {
                return i;
            }
        }
    }
    
    return -1;
}

template <class Report>
int MVPPin<Report>::findPinMode(uint8_t _pin) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == _pin) {
                return this->pin[i].pinMode;
            }
        }
    }
    
    return -1;
}

template <class Report>
int MVPPin<Report>::findChannelPWM(uint8_t _pin) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == _pin &&
                this->pin[i].pinMode == PWM) {
                return this->pin[i].channel;
            }
        }
    }
    
    return -1;
}

template <class Report>
int MVPPin<Report>::findChannelFree() {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (this->findPinOfChannel(i) == -1) {
            return i;
        }
    }

    return -1;
}

template <class Report>
int MVPPin<Report>::findPinOfChannel(uint8_t channel) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].channel == channel) {
                return i;
            }
        }
    }
    
    return -1;
}

#endif /* INC_MVP_PIN_HPP_ */