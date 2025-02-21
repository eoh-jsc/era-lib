#include <float.h>
#include <Automation/ERaHelpers.hpp>

namespace eras {

    bool StringReplace(std::string& str, const std::string& from, const std::string& to) {
        size_t startPos = str.find(from);
        if (startPos == std::string::npos) {
            return false;
        }
        str.replace(startPos, from.length(), to);
        return true;
    }

    Mutex::Mutex()
        : mMutex(nullptr)
    {}

    void Mutex::lock() {
        ERaGuardLock(this->mMutex);
    }

    bool Mutex::tryLock() {
        return ERaGuardTryLock(this->mMutex);
    }

    void Mutex::unlock() {
        ERaGuardUnlock(this->mMutex);
    }

    double ParseNumber(const char* str) {
        char* end = nullptr;
        double value = ::strtod(str, &end);
        if ((end == str) || (*end != '\0') || (value == HUGE_VAL)) {
            return NAN;
        }
        return value;
    }

    bool CompareNumbers(double a, double b) {
        double maxVal = (fabs(a) > fabs(b)) ? fabs(a) : fabs(b);
        return (fabs(a - b) <= (maxVal * DBL_EPSILON));
    }

} /* namespace eras */
