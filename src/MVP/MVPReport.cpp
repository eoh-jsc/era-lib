#include <MVP/MVPReport.hpp>
#include <Utility/MVPUtility.hpp>

using namespace std;

MVPReport::MVPReport()
	: numReport(0)
{}

void MVPReport::run() {
	unsigned long currentMillis = MVPMillis();
	for (int i = 0; i < MAX_REPORTS; ++i) {
		if (!this->isValidReport(i)) {
			continue;
		}
		if (currentMillis - this->report[i].prevMillis < this->report[i].minInterval) {
			continue;
		}
		if (std::abs(this->report[i].data.value - this->report[i].data.prevValue) < this->report[i].reportableChange) {
			if (currentMillis - this->report[i].prevMillis < this->report[i].maxInterval) {
				continue;
			}
		}
		// update time
		this->report[i].prevMillis = currentMillis;
		// update value
		this->report[i].data.prevValue = this->report[i].data.value;
        if (!this->report[i].updated) {
            continue;
        }
		// call callback
		if (!this->report[i].enable) {
			continue;
		}
		this->report[i].called = true;
	}

	for (int i = 0; i < MAX_REPORTS; ++i) {
		if (!this->report[i].called) {
			continue;
		}
        if (this->report[i].callback_p == nullptr) {
			this->report[i].callback();
		}
		else {
			this->report[i].callback_p(this->report[i].param);
		}
		this->report[i].called = false;
	}
}

int MVPReport::setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, ReportCallback_t cb) {
	int id = this->findReportFree();
	if (id < 0) {
		return -1;
	}
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}

	this->report[id].data.prevValue = 0;
	this->report[id].data.value = 0;
	this->report[id].reportableChange = minChange;
	this->report[id].minInterval = minInterval;
	this->report[id].maxInterval = maxInterval;
	this->report[id].prevMillis = MVPMillis();
	this->report[id].callback = cb;
	this->report[id].callback_p = nullptr;
	this->report[id].param = nullptr;
    this->report[id].data.pin = 0;
    this->report[id].data.pinMode = 0;
    this->report[id].data.configId = 0;
	this->report[id].enable = true;
	this->report[id].updated = false;
	this->report[id].called = false;
	this->numReport++;
	return id;
}

int MVPReport::setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, ReportCallback_p_t cb,
							void* arg) {
	int id = this->findReportFree();
	if (id < 0) {
		return -1;
	}
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}

	this->report[id].data.prevValue = 0;
	this->report[id].data.value = 0;
	this->report[id].reportableChange = minChange;
	this->report[id].minInterval = minInterval;
	this->report[id].maxInterval = maxInterval;
	this->report[id].prevMillis = MVPMillis();
	this->report[id].callback = nullptr;
	this->report[id].callback_p = cb;
	this->report[id].param = arg;
    this->report[id].data.pin = 0;
    this->report[id].data.pinMode = 0;
    this->report[id].data.configId = 0;
	this->report[id].enable = true;
	this->report[id].updated = false;
	this->report[id].called = false;
	this->numReport++;
	return id;
}

int MVPReport::setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, ReportCallback_p_t cb,
							uint8_t pin, uint8_t pinMode, unsigned int configId) {
	int id = this->findReportFree();
	if (id < 0) {
		return -1;
	}
	if (!minInterval) {
		return -1;
	}
	if (maxInterval < minInterval) {
		return -1;
	}

	this->report[id].data.prevValue = 0;
	this->report[id].data.value = 0;
	this->report[id].reportableChange = minChange;
	this->report[id].minInterval = minInterval;
	this->report[id].maxInterval = maxInterval;
	this->report[id].prevMillis = MVPMillis();
	this->report[id].callback = nullptr;
	this->report[id].callback_p = cb;
	this->report[id].param = &this->report[id].data;
    this->report[id].data.pin = pin;
    this->report[id].data.pinMode = pinMode;
    this->report[id].data.configId = configId;
	this->report[id].enable = true;
	this->report[id].updated = false;
	this->report[id].called = false;
	this->numReport++;
	return id;
}

bool MVPReport::changeReportableChange(unsigned int id, unsigned long minInterval, unsigned long maxInterval, float minChange) {
	if (id >= MAX_REPORTS) {
		return false;
	}
	if (!minInterval) {
		return false;
	}
	if (maxInterval < minInterval) {
		return false;
	}

	if (!this->isValidReport(id)) {
		return false;
	}

	this->report[id].reportableChange = minChange;
	this->report[id].minInterval = minInterval;
	this->report[id].maxInterval = maxInterval;
	return true;
}

bool MVPReport::changeReportableChange(unsigned int id, unsigned long minInterval, unsigned long maxInterval, float minChange,
										ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode, unsigned int configId) {
	if (id >= MAX_REPORTS) {
		return false;
	}
	if (!minInterval) {
		return false;
	}
	if (maxInterval < minInterval) {
		return false;
	}

	if (!this->isValidReport(id)) {
		return false;
	}

	this->report[id].reportableChange = minChange;
	this->report[id].minInterval = minInterval;
	this->report[id].maxInterval = maxInterval;
	this->report[id].callback_p = cb;
	this->report[id].param = &this->report[id].data;
    this->report[id].data.pin = pin;
    this->report[id].data.pinMode = pinMode;
	this->report[id].data.configId = configId;
    return true;
}

void MVPReport::updateReport(unsigned int id, float value) {
	if (id >= MAX_REPORTS) {
		return;
	}

	this->report[id].data.value = value;
    if (!this->report[id].updated) {
        this->report[id].data.prevValue = value;
        this->report[id].prevMillis = MVPMillis() - this->report[id].minInterval;
    }
	this->report[id].updated = true;
}

void MVPReport::restartReport(unsigned int id) {
	if (id >= MAX_REPORTS) {
		return;
	}

    this->report[id].updated = false;
    this->report[id].prevMillis = MVPMillis();
}

void MVPReport::executeNow(unsigned int id) {
	if (id >= MAX_REPORTS) {
		return;
	}

	this->report[id].prevMillis = MVPMillis() - this->report[id].maxInterval;
}

void MVPReport::deleteReport(unsigned int id) {
	if (id >= MAX_REPORTS) {
		return;
	}

	if (!this->numReport) {
		return;
	}

	if (this->isValidReport(id)) {
		this->report[id] = Report_t();
		this->report[id].prevMillis = MVPMillis();
		this->numReport--;
	}
}

bool MVPReport::isEnable(unsigned int id) {
	if (id >= MAX_REPORTS) {
		return false;
	}

	return this->report[id].enable;
}

void MVPReport::enable(unsigned int id) {
	if (id >= MAX_REPORTS) {
		return;
	}

	this->report[id].enable = true;
}

void MVPReport::disable(unsigned int id) {
	if (id >= MAX_REPORTS) {
		return;
	}
	
	this->report[id].enable = false;
}

void MVPReport::enableAll() {
	for (int i = 0; i < MAX_REPORTS; ++i) {
		if (this->isValidReport(i)) {
			this->report[i].enable = true;
		}
	}
}

void MVPReport::disableAll() {
	for (int i = 0; i < MAX_REPORTS; ++i) {
		if (this->isValidReport(i)) {
			this->report[i].enable = false;
		}
	}
}

int MVPReport::findReportFree() {
	if (this->numReport >= MAX_REPORTS) {
		return -1;
	}

	for (int i = 0; i < MAX_REPORTS; ++i) {
		if (!this->isValidReport(i)) {
			return i;
		}
	}
    
    return -1;
}