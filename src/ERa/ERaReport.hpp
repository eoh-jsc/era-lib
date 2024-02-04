#ifndef INC_ERA_REPORT_HPP_
#define INC_ERA_REPORT_HPP_

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <ERa/ERaDefine.hpp>
#include <ERa/ERaDetect.hpp>
#include <Utility/ERaQueue.hpp>

#define REPORT_MAX_INTERVAL (unsigned long)0xFFFFFFFF
#define WaitForever         REPORT_MAX_INTERVAL

class ERaReport
{
public:
    typedef struct __ScaleData_t {
        bool enable;
        float min;
        float max;
        float rawMin;
        float rawMax;
    } ScaleData_t;

    typedef struct __ReportData_t {
        double prevValue;
        double value;
        ERaUInt_t configId;
        ERaReport::ScaleData_t scale;
        uint8_t pin;
        uint8_t pinMode;
    } ReportData_t;

private:
#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> ReportCallback_t;
    typedef std::function<void(void*)> ReportCallback_p_t;
#else
    typedef void (*ReportCallback_t)(void);
    typedef void (*ReportCallback_p_t)(void*);
#endif

    const static int MAX_REPORTS = ERA_MAX_REPORT;
    enum ReportFlagT {
        REPORT_ON_CALLED = 0x01,
        REPORT_ON_DELETE = 0x80
    };
    typedef struct __Report_t {
        float reportableChange;
        unsigned long minInterval;
        unsigned long maxInterval;
        unsigned long prevMillis;
        ERaReport::ReportCallback_t callback;
        ERaReport::ReportCallback_p_t callback_p;
        void* param;
        ERaReport::ReportData_t data;
        bool enable;
        bool updated;
        bool reported;
        uint8_t called;
    } Report_t;

public:
    class iterator
    {
    public:
        iterator()
            : rp(nullptr)
            , pRp(nullptr)
        {}
        iterator(ERaReport* _rp, Report_t* _pRp)
            : rp(_rp)
            , pRp(_pRp)
        {}
        ~iterator()
        {}

        operator Report_t*() const {
            return this->pRp;
        }

        operator bool() const {
            return this->isValid();
        }

        void operator() (void) const {
            this->executeReport();
        }
        
        bool isValid() const {
            return ((this->rp != nullptr) && (this->pRp != nullptr));
        }

        Report_t* getId() const {
            return this->pRp;
        }

        void executeReport() const {
            if (!this->isValid()) {
                return;
            }
            this->rp->executeReport(this->pRp);
        }

        void executeNow() const {
            if (!this->isValid()) {
                return;
            }
            this->rp->executeNow(this->pRp);
        }

        void updateReport(double value, bool isRound = false, bool execute = true) const {
            if (!this->isValid()) {
                return;
            }
            this->rp->updateReport(this->pRp, value, isRound, execute);
        }

        bool changeReportableChange(unsigned long minInterval, unsigned long maxInterval, float minChange) const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->changeReportableChange(this->pRp, minInterval, maxInterval, minChange);
        }

        bool changeReportableChange(unsigned long minInterval, unsigned long maxInterval, float minChange,
                                    ERaReport::ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode,
                                    ERaUInt_t configId) const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->changeReportableChange(this->pRp, minInterval, maxInterval, minChange, cb, pin, pinMode, configId);
        }

        bool changeReportableChange(unsigned long minInterval, unsigned long maxInterval, float minChange,
                                    ERaReport::ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode) const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->changeReportableChange(this->pRp, minInterval, maxInterval, minChange, cb, pin, pinMode);
        }

        bool reportEvery(unsigned long interval) const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->reportEvery(this->pRp, interval);
        }

        bool isUpdated() const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->isUpdated(this->pRp);
        }

        bool isReported() const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->isReported(this->pRp);
        }

        bool isCalled() const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->isCalled(this->pRp);
        }

        double getValue() const {
            if (!this->isValid()) {
                return 0.0;
            }
            return this->rp->getValue(this->pRp);
        }

        double getPreviousValue() const {
            if (!this->isValid()) {
                return 0.0;
            }
            return this->rp->getPreviousValue(this->pRp);
        }

        void skipReport() const {
            if (!this->isValid()) {
                return;
            }
            this->rp->skipReport(this->pRp);
        }

        void restartReport() const {
            if (!this->isValid()) {
                return;
            }
            this->rp->restartReport(this->pRp);
        }

        bool isEnable() const {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->isEnable(this->pRp);
        }

        void enable() const {
            if (!this->isValid()) {
                return;
            }
            this->rp->enable(this->pRp);
        }

        void disable() const {
            if (!this->isValid()) {
                return;
            }
            this->rp->disable(this->pRp);
        }

        void setScale(float min, float max, float rawMin, float rawMax) const {
            if (!this->isValid()) {
                return;
            }
            this->rp->setScale(this->pRp, min, max, rawMin, rawMax);
        }

        ERaReport::ScaleData_t* getScale() const {
            if (!this->isValid()) {
                return nullptr;
            }
            return this->rp->getScale(this->pRp);
        }

        void deleteReport() {
            if (!this->isValid()) {
                return;
            }
            this->rp->deleteReport(this->pRp);
            this->invalidate();
        }

    protected:
    private:
        void invalidate() {
            this->rp = nullptr;
            this->pRp = nullptr;
        }

        ERaReport* rp;
        Report_t* pRp;
    };

    ERaReport();
    ~ERaReport()
    {}

    void run();

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_t cb) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb));
    }

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_p_t cb,
                        void* args) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb, args));
    }

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_p_t cb,
                        uint8_t pin, uint8_t pinMode, ERaUInt_t configId) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb, pin, pinMode, configId));
    }

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_p_t cb,
                        uint8_t pin, uint8_t pinMode) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb, pin, pinMode));
    }

    void updateReport(Report_t* pReport, double value, bool isRound = false, bool execute = true);
    bool changeReportableChange(Report_t* pReport, unsigned long minInterval,
                                unsigned long maxInterval, float minChange);
    bool changeReportableChange(Report_t* pReport, unsigned long minInterval,
                                unsigned long maxInterval, float minChange,
                                ERaReport::ReportCallback_p_t cb, uint8_t pin,
                                uint8_t pinMode, ERaUInt_t configId);

    bool changeReportableChange(Report_t* pReport, unsigned long minInterval,
                                unsigned long maxInterval, float minChange,
                                ERaReport::ReportCallback_p_t cb, uint8_t pin,
                                uint8_t pinMode) {
        return this->changeReportableChange(pReport, minInterval, maxInterval, minChange, cb, pin, pinMode, 0);
    }

    bool reportEvery(Report_t* pReport, unsigned long interval);
    bool isUpdated(Report_t* pReport);
    bool isReported(Report_t* pReport);
    bool isCalled(Report_t* pReport);
    double getValue(Report_t* pReport);
    double getPreviousValue(Report_t* pReport);
    void skipReport(Report_t* pReport);
    void restartReport(Report_t* pReport);
    void executeReport(Report_t* pReport);
    void executeNow(Report_t* pReport);
    void deleteReport(Report_t* pReport);
    bool isEnable(Report_t* pReport);
    void enable(Report_t* pReport);
    void disable(Report_t* pReport);
    void setScale(Report_t* pReport, float min, float max, float rawMin, float rawMax);
    void enableAll();
    void disableAll();

    ERaReport::ScaleData_t* getScale(Report_t* pReport) const;

protected:
private:
    Report_t* setupReport(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_t cb);
    Report_t* setupReport(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_p_t cb,
                        void* args);
    Report_t* setupReport(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_p_t cb,
                        uint8_t pin, uint8_t pinMode, ERaUInt_t configId);

    Report_t* setupReport(unsigned long minInterval, unsigned long maxInterval,
                        float minChange, ERaReport::ReportCallback_p_t cb,
                        uint8_t pin, uint8_t pinMode) {
        return this->setupReport(minInterval, maxInterval, minChange, cb, pin, pinMode, 0);
    }

    bool isReportFree();

    bool isValidReport(const Report_t* pReport) const {
        if (pReport == nullptr) {
            return false;
        }
        return ((pReport->callback != nullptr) || (pReport->callback_p != nullptr));
    }

    void setFlag(uint8_t& flags, uint8_t mask, bool value) {
        if (value) {
            flags |= mask;
        }
        else {
            flags &= ~mask;
        }
    }

    bool getFlag(uint8_t flags, uint8_t mask) {
        return (flags & mask) == mask;
    }

    ERaList<Report_t*> report;
    unsigned int numReport;
};

using ReportEntry = ERaReport::iterator;

#endif /* INC_ERA_REPORT_HPP_ */
