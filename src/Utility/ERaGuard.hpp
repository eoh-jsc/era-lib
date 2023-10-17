#ifndef INC_ERA_GUARD_HPP_
#define INC_ERA_GUARD_HPP_

#include <Utility/ERaUtility.hpp>

class ERaGuard
{
public:
    ERaGuard(ERaMutex_t& mt)
        : mutex(mt)
    {
        ERaGuardLock(this->mutex);
    }
    ~ERaGuard()
    {
        ERaGuardUnlock(this->mutex);
    }

private:
    ERaMutex_t& mutex;
};

#endif /* INC_ERA_GUARD_HPP_ */
