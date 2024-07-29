#ifndef INC_ERA_API_ABBREVIATION_HPP_
#define INC_ERA_API_ABBREVIATION_HPP_

#include <ERa/ERaApi.hpp>

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::handleReadPin(cJSON* root) {
    ERA_FORCE_UNUSED(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::handleWritePin(cJSON* root) {
    ERA_FORCE_UNUSED(root);
}

template <class Proto, class Flash>
inline
void ERaApi<Proto, Flash>::processArduinoPinRequest(const ERaDataBuff& arrayTopic, const char* payload) {
    ERA_FORCE_UNUSED(arrayTopic);
    ERA_FORCE_UNUSED(payload);
}

#if defined(ERA_PIN_DEBUG)
    template <class Proto, class Flash>
    inline
    void ERaApi<Proto, Flash>::handlePinRequest(const ERaDataBuff& arrayTopic, const char* payload) {
        ERA_FORCE_UNUSED(arrayTopic);
        ERA_FORCE_UNUSED(payload);
    }
#endif

#endif /* INC_ERA_API_ABBREVIATION_HPP_ */
