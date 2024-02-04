#ifndef INC_ERA_WIDGETS_HPP_
#define INC_ERA_WIDGETS_HPP_

#include <ERa/ERaProtocol.hpp>

#if defined(ERA_HAS_FUNCTIONAL_H)
    typedef std::function<void(void)> WidgetCallback_t;
#else
    typedef void (*WidgetCallback_t)(void);
#endif

#include <Widgets/ERaWidgetTerminalBox.hpp>

#endif /* INC_ERA_WIDGETS_HPP_ */
