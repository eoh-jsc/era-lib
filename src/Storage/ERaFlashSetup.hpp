#ifndef INC_ERA_FLASH_SETUP_HPP_
#define INC_ERA_FLASH_SETUP_HPP_

#if defined(__has_include)
    #if __has_include(<flashdb.h>)
        #include <flashdb.h>
        #define ERA_HAS_FLASH_DB_H
    #endif

    #if __has_include(<Preferences.h>)
        #include <Preferences.h>
        #define ERA_HAS_PREFERENCES_H
    #endif
#endif

#endif /* INC_ERA_FLASH_SETUP_HPP_ */
