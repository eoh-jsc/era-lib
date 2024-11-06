#include <Utility/ERaUtility.hpp>
#include <ERa/ERaReport.hpp>

using namespace std;

ERaReport::ERaReport()
    : numReport(0)
{}

void ERaReport::run() {
    unsigned long currentMillis = ERaMillis();
    const ReportIterator* e = this->report.end();
    for (ReportIterator* it = this->report.begin(); it != e; it = it->getNext()) {
        Report_t* pReport = it->get();
        if (!this->isValidReport(pReport)) {
            continue;
        }
        if ((currentMillis - pReport->prevMillis) < pReport->minInterval) {
            continue;
        }
        if (ERaDoubleCompare(pReport->data.value, pReport->data.prevValue) ||
            abs(pReport->data.value - pReport->data.prevValue) < (double)pReport->reportableChange) {
            if (((currentMillis - pReport->prevMillis) < pReport->maxInterval) ||
                (pReport->maxInterval == REPORT_MAX_INTERVAL)) {
                continue;
            }
        }
        // update time
        pReport->prevMillis = currentMillis;
        // update value
        pReport->data.prevValue = pReport->data.value;
        if (!pReport->updated) {
            continue;
        }
        // call callback
        if (!pReport->enable) {
            continue;
        }
        pReport->reported = true;
        this->setFlag(pReport->called, ReportFlagT::REPORT_ON_CALLED, true);
    }

    for (ReportIterator* it = this->report.begin(); it != e; it = it->getNext()) {
        Report_t* pReport = it->get();
        if (!this->isValidReport(pReport)) {
            continue;
        }
        if (!this->isCalled(pReport, ReportFlagT::REPORT_ON_CALLED)) {
            continue;
        }
        if (this->getFlag(pReport->called, ReportFlagT::REPORT_ON_DELETE)) {
            continue;
        }
        if (pReport->callback_p == nullptr) {
            pReport->callback();
        }
        else {
            pReport->callback_p(pReport->param);
        }
    }

    do {} while (this->deleteHandler());
}

bool ERaReport::deleteHandler() {
    const ReportIterator* e = this->report.end();
    for (ReportIterator* it = this->report.begin(); it != e; it = it->getNext()) {
        Report_t*& pReport = it->get();
        if (!this->isValidReport(pReport)) {
            continue;
        }
        if (!this->isCalled(pReport, ReportFlagT::REPORT_ON_DELETE)) {
            continue;
        }
        delete pReport;
        pReport = nullptr;
        this->report.remove(it);
        this->numReport--;
        it = nullptr;
        return true;
    }
    return false;
}

ERaReport::Report_t* ERaReport::setupReport(unsigned long minInterval, unsigned long maxInterval,
                                            float minChange, ReportCallback_t cb) {
    if (!this->isReportFree()) {
        return nullptr;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Report_t* pReport = new Report_t();
    if (pReport == nullptr) {
        return nullptr;
    }

    pReport->data.prevValue = 0;
    pReport->data.value = 0;
    pReport->data.pin = 0;
    pReport->data.pinMode = 0;
    pReport->data.configId = 0;
    pReport->data.scale.enable = false;
    pReport->data.scale.min = 0;
    pReport->data.scale.max = 0;
    pReport->data.scale.rawMin = 0;
    pReport->data.scale.rawMax = 0;
    pReport->reportableChange = minChange;
    pReport->minInterval = minInterval;
    pReport->maxInterval = maxInterval;
    pReport->prevMillis = ERaMillis();
    pReport->callback = cb;
    pReport->callback_p = nullptr;
    pReport->param = nullptr;
    pReport->enable = true;
    pReport->updated = false;
    pReport->reported = false;
    pReport->called = 0;
    this->report.put(pReport);
    this->numReport++;
    return pReport;
}

ERaReport::Report_t* ERaReport::setupReport(unsigned long minInterval, unsigned long maxInterval,
                                            float minChange, ReportCallback_p_t cb,
                                            void* args) {
    if (!this->isReportFree()) {
        return nullptr;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Report_t* pReport = new Report_t();
    if (pReport == nullptr) {
        return nullptr;
    }

    pReport->data.prevValue = 0;
    pReport->data.value = 0;
    pReport->data.pin = 0;
    pReport->data.pinMode = 0;
    pReport->data.configId = 0;
    pReport->data.scale.enable = false;
    pReport->data.scale.min = 0;
    pReport->data.scale.max = 0;
    pReport->data.scale.rawMin = 0;
    pReport->data.scale.rawMax = 0;
    pReport->reportableChange = minChange;
    pReport->minInterval = minInterval;
    pReport->maxInterval = maxInterval;
    pReport->prevMillis = ERaMillis();
    pReport->callback = nullptr;
    pReport->callback_p = cb;
    pReport->param = args;
    pReport->enable = true;
    pReport->updated = false;
    pReport->reported = false;
    pReport->called = 0;
    this->report.put(pReport);
    this->numReport++;
    return pReport;
}

ERaReport::Report_t* ERaReport::setupReport(unsigned long minInterval, unsigned long maxInterval,
                                            float minChange, ReportCallback_p_t cb,
                                            uint8_t pin, uint8_t pinMode, ERaUInt_t configId) {
    if (!this->isReportFree()) {
        return nullptr;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    Report_t* pReport = new Report_t();
    if (pReport == nullptr) {
        return nullptr;
    }

    pReport->data.prevValue = 0;
    pReport->data.value = 0;
    pReport->data.pin = pin;
    pReport->data.pinMode = pinMode;
    pReport->data.configId = configId;
    pReport->data.scale.enable = false;
    pReport->data.scale.min = 0;
    pReport->data.scale.max = 0;
    pReport->data.scale.rawMin = 0;
    pReport->data.scale.rawMax = 0;
    pReport->reportableChange = minChange;
    pReport->minInterval = minInterval;
    pReport->maxInterval = maxInterval;
    pReport->prevMillis = ERaMillis();
    pReport->callback = nullptr;
    pReport->callback_p = cb;
    pReport->param = &pReport->data;
    pReport->enable = true;
    pReport->updated = false;
    pReport->reported = false;
    pReport->called = 0;
    this->report.put(pReport);
    this->numReport++;
    return pReport;
}

bool ERaReport::changeReportableChange(Report_t* pReport, unsigned long minInterval,
                                        unsigned long maxInterval, float minChange) {
    if (!this->isValidReport(pReport)) {
        return false;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    pReport->reportableChange = minChange;
    pReport->minInterval = minInterval;
    pReport->maxInterval = maxInterval;
    return true;
}

bool ERaReport::changeReportableChange(Report_t* pReport, unsigned long minInterval,
                                        unsigned long maxInterval, float minChange,
                                        ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode,
                                        ERaUInt_t configId) {
    if (!this->isValidReport(pReport)) {
        return false;
    }
    if (!minInterval) {
        minInterval = 1;
    }
    if (maxInterval < minInterval) {
        maxInterval = minInterval;
    }

    pReport->reportableChange = minChange;
    pReport->minInterval = minInterval;
    pReport->maxInterval = maxInterval;
    pReport->callback_p = cb;
    pReport->param = &pReport->data;
    pReport->data.pin = pin;
    pReport->data.pinMode = pinMode;
    pReport->data.configId = configId;
    return true;
}

void ERaReport::updateReport(Report_t* pReport, double value, bool isRound, bool execute) {
    if (!this->isValidReport(pReport)) {
        return;
    }
    if (ERaIsSpN(value)) {
        return;
    }

    if (pReport->data.scale.enable) {
        value = ERaMapNumberRange(value, (double)pReport->data.scale.rawMin,
                                         (double)pReport->data.scale.rawMax,
                                         (double)pReport->data.scale.min,
                                         (double)pReport->data.scale.max);
        if (isRound) {
            value = round(value);
        }
    }
    pReport->data.value = value;
    if (!pReport->updated) {
        pReport->data.prevValue = value;
        /* pReport->prevMillis = ERaMillis() - pReport->minInterval; */
        if (execute && (pReport->maxInterval != REPORT_MAX_INTERVAL)) {
            pReport->prevMillis = ERaMillis() - pReport->maxInterval;
        }
    }
    pReport->updated = true;
}

bool ERaReport::reportEvery(Report_t* pReport, unsigned long interval) {
    if (!this->isValidReport(pReport)) {
        return false;
    }
    if (interval < pReport->minInterval) {
        interval = pReport->minInterval;
    }

    pReport->maxInterval = interval;
    return true;
}

bool ERaReport::isUpdated(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return false;
    }

    return pReport->updated;
}

bool ERaReport::isReported(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return false;
    }

    return pReport->reported;
}

bool ERaReport::isCalled(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return false;
    }

    return this->getFlag(pReport->called, ReportFlagT::REPORT_ON_CALLED);
}

double ERaReport::getValue(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return 0.0;
    }

    return pReport->data.value;
}

double ERaReport::getPreviousValue(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return 0.0;
    }

    return pReport->data.prevValue;
}

void ERaReport::skipReport(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return;
    }

    // update time
    pReport->prevMillis = ERaMillis();
    // update value
    pReport->data.prevValue = pReport->data.value;
    // clear flag called
    this->setFlag(pReport->called, ReportFlagT::REPORT_ON_CALLED, false);
}

void ERaReport::restartReport(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return;
    }

    pReport->updated = false;
    pReport->prevMillis = ERaMillis();
}

void ERaReport::executeReport(Report_t* pReport) {
    if (!this->isValidReport(pReport)) {
        return;
    }

    // update time
    pReport->prevMillis = ERaMillis();
    // update value
    pReport->data.prevValue = pReport->data.value;
    // clear flag called
    this->setFlag(pReport->called, ReportFlagT::REPORT_ON_CALLED, true);
}

void ERaReport::executeNow(Report_t* pReport) {
    if (this->isValidReport(pReport)) {
        pReport->prevMillis = ERaMillis() - pReport->maxInterval;
    }
}

void ERaReport::deleteReport(Report_t* pReport) {
    if (!this->numReport) {
        return;
    }

    if (this->isValidReport(pReport)) {
        pReport->enable = false;
        this->setFlag(pReport->called, ReportFlagT::REPORT_ON_DELETE, true);
    }
}

bool ERaReport::isEnable(Report_t* pReport) {
    if (this->isValidReport(pReport)) {
        return pReport->enable;
    }
    else {
        return false;
    }
}

void ERaReport::enable(Report_t* pReport) {
    if (this->isValidReport(pReport)) {
        pReport->enable = true;
    }
}

void ERaReport::disable(Report_t* pReport) {
    if (this->isValidReport(pReport)) {
        pReport->enable = false;
    }
}

void ERaReport::setScale(Report_t* pReport, float min, float max, float rawMin, float rawMax) {
    if (!this->isValidReport(pReport)) {
        return;
    }
    if (ERaFloatCompare(min, max) ||
        ERaFloatCompare(rawMin, rawMax)) {
        pReport->data.scale.enable = false;
        return;
    }

    pReport->data.scale.enable = true;
    pReport->data.scale.min = min;
    pReport->data.scale.max = max;
    pReport->data.scale.rawMin = rawMin;
    pReport->data.scale.rawMax = rawMax;

    /*
    pReport->reportableChange = ERaMapNumberRange(pReport->reportableChange,
                                        0.0f, max - min, 0.0f, rawMax - rawMin);

    pReport->reportableChange = ERaMapNumberRange(pReport->reportableChange,
                                        0.0f, rawMax - rawMin, 0.0f, max - min);
    */
}

void ERaReport::restartAll() {
    const ReportIterator* e = this->report.end();
    for (ReportIterator* it = this->report.begin(); it != e; it = it->getNext()) {
        Report_t* pReport = it->get();
        this->restartReport(pReport);
    }
}

void ERaReport::enableAll() {
    const ReportIterator* e = this->report.end();
    for (ReportIterator* it = this->report.begin(); it != e; it = it->getNext()) {
        Report_t* pReport = it->get();
        this->enable(pReport);
    }
}

void ERaReport::disableAll() {
    const ReportIterator* e = this->report.end();
    for (ReportIterator* it = this->report.begin(); it != e; it = it->getNext()) {
        Report_t* pReport = it->get();
        this->disable(pReport);
    }
}

ERaReport::ScaleData_t* ERaReport::getScale(Report_t* pReport) const {
    if (this->isValidReport(pReport)) {
        return &pReport->data.scale;
    }
    else {
        return nullptr;
    }
}

float ERaReport::getScaleMin(Report_t* pReport) const {
    if (this->isValidReport(pReport)) {
        return pReport->data.scale.min;
    }
    else {
        return 0.0f;
    }
}

float ERaReport::getScaleMax(Report_t* pReport) const {
    if (this->isValidReport(pReport)) {
        return pReport->data.scale.max;
    }
    else {
        return 0.0f;
    }
}

float ERaReport::getScaleRawMin(Report_t* pReport) const {
    if (this->isValidReport(pReport)) {
        return pReport->data.scale.rawMin;
    }
    else {
        return 0.0f;
    }
}

float ERaReport::getScaleRawMax(Report_t* pReport) const {
    if (this->isValidReport(pReport)) {
        return pReport->data.scale.rawMax;
    }
    else {
        return 0.0f;
    }
}

float ERaReport::getReportableChange(Report_t* pReport) const {
    if (this->isValidReport(pReport)) {
        return pReport->reportableChange;
    }
    else {
        return 0.0f;
    }
}

bool ERaReport::isReportFree() {
    if (this->numReport >= MAX_REPORTS) {
        return false;
    }

    return true;
}
