#ifndef INC_ERA_PIN_HPP_
#define INC_ERA_PIN_HPP_

#include <functional>
#include <ERa/ERaDefine.hpp>
#include <Utility/ERaUtility.hpp>
#include <ERa/ERaReport.hpp>

#if !defined(TOGGLE)
    #define TOGGLE              0x2
#endif
#if !defined(PWM)
    #define PWM                 0x27
#endif
#define VIRTUAL                 0xFF
#define ERA_VIRTUAL             (int)(-1)

template <class Report>
class ERaPin
{
    typedef std::function<int(uint8_t)> ReadPinHandler_t;

    const static int MAX_CHANNELS = 16;
	const static int MAX_PINS = 16;
	const static int MAX_VPINS = ERA_MAX_VIRTUAL_PIN;
    typedef struct __Pin_t {
        unsigned long prevMillis;
        unsigned long delay;
        ERaPin::ReadPinHandler_t readPin;
        typename Report::iterator report;
		unsigned int configId;
        uint8_t pin;
        uint8_t pinMode;
        uint8_t channel; /* for pwm mode */
        bool enable;
    } Pin_t;

    typedef struct __VPin_t {
        uint8_t pin;
		unsigned int configId;
    } VPin_t;

public:
    class iterator
    {
    public:
        iterator()
            : pPin(nullptr)
            , id(-1)
        {}
        iterator(ERaPin* _pPin, int _id)
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
            return this->pPin->changeInterval(this->id, interval, minInterval, maxInterval, minChange);
        }

        void restartPin() {
            if (!this->isValid()) {
                return;
            }
            this->pPin->restartPin(this->id);
        }

        void deletePin() {
            if (!this->isValid()) {
                return;
            }
            this->pPin->deletePin(this->id);
            this->invalidate();
        }

        bool isEnable() {
            if (!this->isValid()) {
                return false;
            }
            return this->pPin->isEnable(this->id);
        }

        void enable() {
            if (!this->isValid()) {
                return;
            }
            this->pPin->enable(this->id);
        }

        void disable() {
            if (!this->isValid()) {
                return;
            }
            this->pPin->disable(this->id);
        }

        void setScale(float min, float max, float rawMin, float rawMax) {
            if (!this->isValid()) {
                return;
            }
            this->pPin->setScale(this->id, min, max, rawMin, rawMax);
        }

        typename Report::ScaleData_t* getScale() {
            if (!this->isValid()) {
                return nullptr;
            }
            this->pPin->getScale(this->id);
        }

    protected:
    private:
		void invalidate() {
			this->pPin = nullptr;
			this->id = -1;
		}

        ERaPin* pPin;
        int id;
    };

    ERaPin(Report& _report)
        : report(_report)
        , numPin(0)
    {}
    ~ERaPin()
    {}
    
    void run();

    iterator setPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb) {
        return iterator(this, this->setupPinReport(p, pMode, readPin, interval, minInterval, maxInterval, minChange, cb));
    }

    iterator setPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb,
                        unsigned int configId) {
        return iterator(this, this->setupPinReport(p, pMode, readPin, interval, minInterval, maxInterval, minChange, cb, configId));
    }

    iterator setPinVirtual(uint8_t p, unsigned int configId) {
        return iterator(this, this->setupPinVirtual(p, configId));
    }

    iterator setPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb) {
        return iterator(this, this->setupPWMPinReport(p, pMode, channel, readPin, interval, minInterval, maxInterval, minChange, cb));
    }

    iterator setPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb, unsigned int configId) {
        return iterator(this, this->setupPWMPinReport(p, pMode, channel, readPin, interval, minInterval, maxInterval, minChange, cb, configId));
    }

	bool changeInterval(unsigned int id, unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange);
	void restartPin(unsigned int id);
    void executeNow(unsigned int id);
	void deletePin(unsigned int id);
    void deleteWithPin(uint8_t p);
    void deleteAll();
	bool isEnable(unsigned int id);
	void enable(unsigned int id);
	void disable(unsigned int id);
    void setScale(unsigned int id, float min, float max, float rawMin, float rawMax);
    typename Report::ScaleData_t* getScale(unsigned int id);
    typename Report::ScaleData_t* findScale(uint8_t p);
    typename Report::iterator* getReport(uint8_t p);
	void enableAll();
	void disableAll();
    int findPinMode(uint8_t p);
    int findChannelPWM(uint8_t p);
    int findConfigId(uint8_t p);
    int findVPinConfigId(uint8_t p);
	int findChannelFree();

protected:
private:
    int setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb);
    int setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                        unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb,
                        unsigned int configId);
    int setupPinVirtual(uint8_t p, unsigned int configId);
    int setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb);
    int setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel, ERaPin::ReadPinHandler_t readPin,
                        unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                        typename Report::ReportCallback_p_t cb, unsigned int configId);
	int findPinFree();
	int findVPinFree();
    int findPinExist(uint8_t p);
    int findVPinExist(uint8_t p);
    int findPinOfChannel(uint8_t channel);

	bool isValidPin(unsigned int id) {
		return ((this->pin[id].readPin != nullptr) ||
                (this->pin[id].pinMode == PWM) ||
                (this->pin[id].pinMode == VIRTUAL));
	}

    Report& report;
	Pin_t pin[MAX_PINS] {};
    VPin_t vPin[MAX_VPINS] {};
	unsigned int numPin;
	unsigned int numVPin;
};

template <class Report>
void ERaPin<Report>::run() {
	unsigned long currentMillis = ERaMillis();
	for (int i = 0; i < MAX_PINS; ++i) {
		if (!this->isValidPin(i)) {
			continue;
		}
        if (this->pin[i].pinMode == VIRTUAL) {
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
        else if (this->pin[i].readPin != nullptr) {
            this->pin[i].report.updateReport(this->pin[i].readPin(this->pin[i].channel));
        }
    }
    this->report.run();
}

template <class Report>
int ERaPin<Report>::setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                                unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb) {
    int id = this->findPinExist(p);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        interval = 1;
    }
	if (!minInterval) {
        minInterval = 1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = ERaMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.setScale(0, 0, 0, 0);
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    this->pin[id].configId = 0;
    this->pin[id].pin = p;
    this->pin[id].pinMode = pMode;
    this->pin[id].channel = 0;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
int ERaPin<Report>::setupPinReport(uint8_t p, uint8_t pMode, ERaPin::ReadPinHandler_t readPin, unsigned long interval,
                                unsigned long minInterval, unsigned long maxInterval, float minChange, typename Report::ReportCallback_p_t cb,
                                unsigned int configId) {
    int id = this->findPinExist(p);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        interval = 1;
    }
	if (!minInterval) {
        minInterval = 1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = ERaMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.setScale(0, 0, 0, 0);
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    this->pin[id].configId = configId;
    this->pin[id].pin = p;
    this->pin[id].pinMode = pMode;
    this->pin[id].channel = 0;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
int ERaPin<Report>::setupPinVirtual(uint8_t p, unsigned int configId) {
    int id = this->findVPinExist(p);
    if (id < 0) {
        id = this->findVPinFree();
    }
	if (id < 0) {
		return -1;
	}
    
    this->vPin[id].pin = p;
    this->vPin[id].configId = configId;
	this->numVPin++;
	return id;
}

template <class Report>
int ERaPin<Report>::setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel, ERaPin::ReadPinHandler_t readPin,
                                    unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                                    typename Report::ReportCallback_p_t cb) {
    int id = this->findPinExist(p);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        interval = 1;
    }
	if (!minInterval) {
        minInterval = 1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = ERaMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.setScale(0, 0, 0, 0);
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode);
    }
    this->pin[id].configId = 0;
    this->pin[id].pin = p;
    this->pin[id].pinMode = pMode;
    this->pin[id].channel = channel;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
int ERaPin<Report>::setupPWMPinReport(uint8_t p, uint8_t pMode, uint8_t channel, ERaPin::ReadPinHandler_t readPin,
                                    unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange,
                                    typename Report::ReportCallback_p_t cb, unsigned int configId) {
    int id = this->findPinExist(p);
    if (id < 0) {
        id = this->findPinFree();
    }
	if (id < 0) {
		return -1;
	}
    if (!interval) {
        interval = 1;
    }
	if (!minInterval) {
        minInterval = 1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}
    
    this->pin[id].prevMillis = ERaMillis();
    this->pin[id].delay = interval;
    this->pin[id].readPin = readPin;
    if (this->pin[id].report) {
        this->pin[id].report.setScale(0, 0, 0, 0);
        this->pin[id].report.changeReportableChange(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    else {
        this->pin[id].report = this->report.setReporting(minInterval, maxInterval, minChange, cb, p, pMode, configId);
    }
    this->pin[id].configId = configId;
    this->pin[id].pin = p;
    this->pin[id].pinMode = pMode;
    this->pin[id].channel = channel;
    this->pin[id].enable = true;
	this->numPin++;
	return id;
}

template <class Report>
bool ERaPin<Report>::changeInterval(unsigned int id, unsigned long interval, unsigned long minInterval, unsigned long maxInterval, float minChange) {
	if (id >= MAX_PINS) {
		return false;
	}

    if (!interval) {
        interval = 1;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        return false;
    }

    this->pin[id].delay = interval;
    this->pin[id].report = this->report.changeReportableChange(minInterval, maxInterval, minChange);
    return true;
}

template <class Report>
void ERaPin<Report>::restartPin(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

    this->pin[id].report.restartReport();
    this->pin[id].prevMillis = ERaMillis();
}

template <class Report>
void ERaPin<Report>::executeNow(unsigned int id) {
    if (id >= MAX_PINS) {
        return;
    }

    this->pin[id].prevMillis = ERaMillis() - this->pin[id].delay;
    this->pin[id].report();
}

template <class Report>
void ERaPin<Report>::deletePin(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

	if (!this->numPin) {
		return;
	}
    
	if (this->isValidPin(id)) {
        this->pin[id].report.deleteReport();
		this->pin[id] = Pin_t();
		this->pin[id].prevMillis = ERaMillis();
		this->numPin--;
    }
}

template <class Report>
void ERaPin<Report>::deleteWithPin(uint8_t p) {
    int id = this->findPinExist(p);
    if (id < 0) {
        return;
    }

    this->deletePin(id);
}

template <class Report>
void ERaPin<Report>::deleteAll() {
	for (int i = 0; i < MAX_PINS; ++i) {
        this->deletePin(i);
    }
}

template <class Report>
bool ERaPin<Report>::isEnable(unsigned int id) {
	if (id >= MAX_PINS) {
		return false;
	}

	return this->pin[id].enable;
}

template <class Report>
void ERaPin<Report>::enable(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

	this->pin[id].enable = true;
    this->pin[id].report.enable();
}

template <class Report>
void ERaPin<Report>::disable(unsigned int id) {
	if (id >= MAX_PINS) {
		return;
	}

	this->pin[id].enable = false;
    this->pin[id].report.disable();
}

template <class Report>
void ERaPin<Report>::setScale(unsigned int id, float min, float max, float rawMin, float rawMax) {
	if (id >= MAX_PINS) {
		return;
	}

    this->pin[id].report.setScale(min, max, rawMin, rawMax);
}

template <class Report>
typename Report::ScaleData_t* ERaPin<Report>::getScale(unsigned int id) {
	if (id >= MAX_PINS) {
		return nullptr;
	}

    return this->pin[id].report.getScale();
}

template <class Report>
typename Report::ScaleData_t* ERaPin<Report>::findScale(uint8_t p) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == p) {
                return this->getScale(i);
            }
        }
    }
    
    return nullptr;
}

template <class Report>
typename Report::iterator* ERaPin<Report>::getReport(uint8_t p) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == p) {
                return &this->pin[i].report;
            }
        }
    }
    
    return nullptr;
}

template <class Report>
void ERaPin<Report>::enableAll() {
	for (int i = 0; i < MAX_PINS; ++i) {
		if (this->isValidPin(i)) {
			this->pin[i].enable = true;
            this->pin[i].report.enable();
		}
	}
}

template <class Report>
void ERaPin<Report>::disableAll() {
	for (int i = 0; i < MAX_PINS; ++i) {
		if (this->isValidPin(i)) {
			this->pin[i].enable = false;
            this->pin[i].report.disable();
		}
	}
}

template <class Report>
int ERaPin<Report>::findPinFree() {
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
int ERaPin<Report>::findVPinFree() {
	if (this->numVPin >= MAX_VPINS) {
		return -1;
	}

	for (int i = 0; i < MAX_VPINS; ++i) {
		if (!this->vPin[i].configId) {
			return i;
		}
	}
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findPinExist(uint8_t p) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == p) {
                return i;
            }
        }
    }
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findVPinExist(uint8_t p) {
    for (int i = 0; i < MAX_VPINS; ++i) {
        if (this->vPin[i].configId &&
            (this->vPin[i].pin == p)) {
            return i;
        }
    }
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findPinMode(uint8_t p) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == p) {
                return this->pin[i].pinMode;
            }
        }
    }
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findChannelPWM(uint8_t p) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if (this->pin[i].pin == p &&
                this->pin[i].pinMode == PWM) {
                return this->pin[i].channel;
            }
        }
    }
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findConfigId(uint8_t p) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if ((this->pin[i].pin == p) &&
                this->pin[i].configId) {
                return this->pin[i].configId;
            }
        }
    }
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findVPinConfigId(uint8_t p) {
    for (int i = 0; i < MAX_VPINS; ++i) {
        if ((this->vPin[i].pin == p) &&
            this->vPin[i].configId) {
            return this->vPin[i].configId;
        }
    }
    
    return -1;
}

template <class Report>
int ERaPin<Report>::findChannelFree() {
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        if (this->findPinOfChannel(i) == -1) {
            return i;
        }
    }

    return -1;
}

template <class Report>
int ERaPin<Report>::findPinOfChannel(uint8_t channel) {
    for (int i = 0; i < MAX_PINS; ++i) {
        if (this->isValidPin(i)) {
            if ((this->pin[i].channel == channel) &&
                (this->pin[i].pinMode == PWM)) {
                return i;
            }
        }
    }
    
    return -1;
}

#endif /* INC_ERA_PIN_HPP_ */
