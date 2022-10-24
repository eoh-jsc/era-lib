#ifndef INC_MVP_REPORT_HPP_
#define INC_MVP_REPORT_HPP_

#include <functional>
#include <MVP/MVPDefine.hpp>

class MVPReport
{
public:
	typedef std::function<void(void*)> ReportCallback_p_t;

	typedef struct __ReportData_t {
		float prevValue;
		float value;
		unsigned int configId;
        uint8_t pin;
        uint8_t pinMode;
	} ReportData_t;

private:
	typedef std::function<void(void)> ReportCallback_t;

	const static int MAX_REPORTS = 16;
	typedef struct __Report_t {
		float reportableChange;
		unsigned long minInterval;
		unsigned long maxInterval;
		unsigned long prevMillis;
		MVPReport::ReportCallback_t callback;
		MVPReport::ReportCallback_p_t callback_p;
		void* param;
		MVPReport::ReportData_t data;
		bool enable;
		bool updated;
		bool called;
	} Report_t;

public:
	class iterator
	{
	public:
		iterator()
			: rp(nullptr)
			, id(-1)
		{}
		iterator(MVPReport* _rp, int _id)
			: rp(_rp)
			, id(_id)
		{}
		~iterator()
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
			this->rp->executeNow(this->id);
		}
		
		bool isValid() const {
			return ((this->rp != nullptr) && (this->id >= 0));
		}

        void updateReport(float value) {
            if (!this->isValid()) {
                return;
            }
            this->rp->updateReport(this->id, value);
        }

        bool changeReportableChange(unsigned long minInterval, unsigned long maxInterval, float minChange) {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->changeReportableChange(this->id, minInterval, maxInterval, minChange);
        }

        bool changeReportableChange(unsigned long minInterval, unsigned long maxInterval, float minChange,
									MVPReport::ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode,
									unsigned int configId) {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->changeReportableChange(this->id, minInterval, maxInterval, minChange, cb, pin, pinMode, configId);
        }

        bool changeReportableChange(unsigned long minInterval, unsigned long maxInterval, float minChange,
									MVPReport::ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode) {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->changeReportableChange(this->id, minInterval, maxInterval, minChange, cb, pin, pinMode);
        }

        void restartReport() {
            if (!this->isValid()) {
                return;
            }
            this->rp->restartReport(this->id);
        }

        void deleteReport() {
            if (!this->isValid()) {
                return;
            }
            this->rp->deleteReport(this->id);
            this->invalidate();
        }

        bool isEnable() {
            if (!this->isValid()) {
                return false;
            }
            return this->rp->isEnable(this->id);
        }

        void enable() {
            if (!this->isValid()) {
                return;
            }
            this->rp->enable(this->id);
        }

        void disable() {
            if (!this->isValid()) {
                return;
            }
            this->rp->disable(this->id);
        }

	protected:
	private:
		void invalidate() {
			this->rp = nullptr;
			this->id = -1;
		}

		MVPReport* rp;
		int id;
	};

	MVPReport();
	~MVPReport()
	{}

	void run();

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_t cb) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb));
    }

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_p_t cb,
						void* arg) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb, arg));
    }

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_p_t cb,
						uint8_t pin, uint8_t pinMode, unsigned int configId) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb, pin, pinMode, configId));
    }

    iterator setReporting(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_p_t cb,
						uint8_t pin, uint8_t pinMode) {
        return iterator(this, this->setupReport(minInterval, maxInterval, minChange, cb, pin, pinMode));
    }

	void updateReport(unsigned int id, float value);
	bool changeReportableChange(unsigned int id, unsigned long minInterval, unsigned long maxInterval, float minChange);
	bool changeReportableChange(unsigned int id, unsigned long minInterval, unsigned long maxInterval, float minChange,
								MVPReport::ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode, unsigned int configId);

	bool changeReportableChange(unsigned int id, unsigned long minInterval, unsigned long maxInterval, float minChange,
								MVPReport::ReportCallback_p_t cb, uint8_t pin, uint8_t pinMode) {
		return this->changeReportableChange(id, minInterval, maxInterval, minChange, cb, pin, pinMode, 0);
	}

	void restartReport(unsigned int id);
	void executeNow(unsigned int id);
	void deleteReport(unsigned int id);
	bool isEnable(unsigned int id);
	void enable(unsigned int id);
	void disable(unsigned int id);
	void enableAll();
	void disableAll();

protected:
private:
	int setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_t cb);
	int setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_p_t cb,
					void* arg);
	int setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_p_t cb,
					uint8_t pin, uint8_t pinMode, unsigned int configId);

	int setupReport(unsigned long minInterval, unsigned long maxInterval, float minChange, MVPReport::ReportCallback_p_t cb,
					uint8_t pin, uint8_t pinMode) {
		return this->setupReport(minInterval, maxInterval, minChange, cb, pin, pinMode, 0);
	}

	int findReportFree();

	bool isValidReport(unsigned int id) {
		return ((this->report[id].callback != nullptr) || (this->report[id].callback_p != nullptr));
	}

	Report_t report[MAX_REPORTS] {};
	unsigned int numReport;
};

#endif /* INC_MVP_REPORT_HPP_ */