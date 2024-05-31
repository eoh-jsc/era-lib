#ifndef INC_TINYGSM_DETECT_HPP_
#define INC_TINYGSM_DETECT_HPP_

#if defined(ERA_TINY_GSM_VERSION)
    /* OK, use the specified value */
#elif defined(__has_include)
    #if __has_include(<TinyGsmClientA7672x.h>)
        #define ERA_TINY_GSM_VERSION    2
    #endif
#endif

#if !defined(ERA_TINY_GSM_VERSION)
    #define ERA_TINY_GSM_VERSION        1
#endif

#endif
