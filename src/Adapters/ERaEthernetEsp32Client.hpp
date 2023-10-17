#ifndef INC_ERA_ETHERNET_ESP32_CLIENT_HPP_
#define INC_ERA_ETHERNET_ESP32_CLIENT_HPP_

#if (ESP_IDF_VERSION_MAJOR > 4)
    #include <Adapters/ERaEthernetEsp32ClientIDF5.hpp>
#else
    #include <Adapters/ERaEthernetEsp32ClientIDF4.hpp>
#endif

#endif /* INC_ERA_ETHERNET_ESP32_CLIENT_HPP_ */
