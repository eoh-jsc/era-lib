#ifndef INC_ERA_ZIGBEE_CONFIG_HPP_
#define INC_ERA_ZIGBEE_CONFIG_HPP_

#ifndef ZIGBEE_RXD_Pin
    #define ZIGBEE_RXD_Pin      32
#endif

#ifndef ZIGBEE_TXD_Pin
    #define ZIGBEE_TXD_Pin      33
#endif

#define ZIGBEE_BUFFER_SIZE      1024

#if !defined(ERA_ZIGBEE_YIELD)
    #if !defined(ERA_ZIGBEE_YIELD_MS)
        #define ERA_ZIGBEE_YIELD_MS 10
    #endif
    #define ERA_ZIGBEE_YIELD() { ERaDelay(ERA_ZIGBEE_YIELD_MS); }
#endif

#endif /* INC_ERA_ZIGBEE_CONFIG_HPP_ */
