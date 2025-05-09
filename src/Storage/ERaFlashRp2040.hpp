#ifndef INC_ERA_FLASH_RP2040_HPP_
#define INC_ERA_FLASH_RP2040_HPP_

#include <Storage/ERaFlashSetup.hpp>

#if defined(ERA_HAS_PREFERENCES_H)

    #include <Storage/ERaFlashPreferences.hpp>

#elif defined(ERA_HAS_FLASH_DB_H)

    #include <Storage/ERaFlashDB.hpp>

#else

    #include <Storage/ERaFlashLittleFS.hpp>

#endif

#endif /* INC_ERA_FLASH_RP2040_HPP_ */
