#ifndef INC_ERA_VERSION_HPP_
#define INC_ERA_VERSION_HPP_

#define ERA_MAJOR                   1
#define ERA_MINOR                   2
#define ERA_PATCH                   1

#define ERA_VERSION_TO_STR_2(val)   # val
#define ERA_VERSION_TO_STR(val)     ERA_VERSION_TO_STR_2(val)

#define ERA_VERSION_VAL(major, minor, patch)                            \
                                    ((major * 10000) + (minor * 100) + (patch))

#define ERA_VERSION_NUMBER          ERA_VERSION_VAL(ERA_MAJOR,          \
                                    ERA_MINOR, ERA_PATCH)

#define ERA_VERSION                 ERA_VERSION_TO_STR(ERA_MAJOR) "."    \
                                    ERA_VERSION_TO_STR(ERA_MINOR) "."    \
                                    ERA_VERSION_TO_STR(ERA_PATCH)
#define ERA_VERSION_1_2_1

#if !defined(ERA_FIRMWARE_VERSION)
    #define ERA_FIRMWARE_VERSION    ERA_VERSION
#endif

#endif /* INC_ERA_VERSION_HPP_ */
