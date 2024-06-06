#ifndef INC_ERA_PROPERTY_STATIC_HPP_
#define INC_ERA_PROPERTY_STATIC_HPP_

#if !defined(ERA_ABBR)

    #include <ERa/ERaProperty.hpp>

    #if !defined(ERA_HAS_FUNCTIONAL_H)
        template <class Api>
        inline
        void ERaProperty<Api>::_onCallback(void* args) {
            ERa.ERaProperty::onCallback(args);
        }
    #endif

#endif

#endif /* INC_ERA_PROPERTY_STATIC_HPP_ */
