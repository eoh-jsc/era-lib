#ifndef INC_ERA_PERIOD_HPP_
#define INC_ERA_PERIOD_HPP_

#include <Utility/ERaUtility.hpp>

#define ERA_EVERY_N_MILLIS(n)   for (static MillisTime_t lastTime = 0; (ERaMillis() - lastTime) >= (n); lastTime += (n))

#endif /* INC_ERA_PERIOD_HPP_ */
