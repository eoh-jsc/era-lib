#ifndef INC_ERA_PROTOCOL_DEFINE_HPP_
#define INC_ERA_PROTOCOL_DEFINE_HPP_

#if defined(ESP32) ||   \
    defined(ESP8266)
    #include <lwip/def.h>
#elif !defined(htons)
    #if (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        #define htons(x) ( ((x) << 8) | (((x) >> 8) & 0xFF) )
        #define htonl(x) ( ((x) << 24 & 0xFF000000UL) | \
                           ((x) <<  8 & 0x00FF0000UL) | \
                           ((x) >>  8 & 0x0000FF00UL) | \
                           ((x) >> 24 & 0x000000FFUL) )
        #define ntohs(x) htons(x)
        #define ntohl(x) htonl(x)
    #elif (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
        #define htons(x) (x)
        #define htonl(x) (x)
        #define ntohs(x) (x)
        #define ntohl(x) (x)
    #endif
#endif

#endif /* INC_ERA_PROTOCOL_DEFINE_HPP_ */
