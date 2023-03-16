#ifndef INC_ERA_VERSION_HPP_
#define INC_ERA_VERSION_HPP_

#define ERA_FIRMWARE_MAJOR              0
#define ERA_FIRMWARE_MINOR              9
#define ERA_FIRMWARE_REVISION           0

#define ERA_FIRMWARE_VERSION_NUMBER     900

#define ERA_FIRMWARE_VERSION            "0.9.0"
#define ERA_FIRMWARE_VERSION_0_9_0

#define ERA_FIRMWARE_VERSION_VAL(major, minor, patch)   \
            ((major * 10000) + (minor * 100) + (patch))

#endif /* INC_ERA_VERSION_HPP_ */
