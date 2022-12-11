#ifndef INC_ERA_SIMPLE_ETHERNET_HPP_
#define INC_ERA_SIMPLE_ETHERNET_HPP_

#include <Ethernet.h>
#include <EthernetClient.h>
#include <Adapters/ERaEthernetClient.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Utility/ERaFlashConfig.hpp>

#if defined(ERA_MODBUS)
    template <class Api>
    void ERaModbus<Api>::initModbusTask() {
    }

    template <class Api>
    void ERaModbus<Api>::modbusTask(void* args) {
    }

    template <class Api>
    void ERaModbus<Api>::writeModbusTask(void* args) {
    }
#endif

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::initERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::begin();
#endif
}

template <class Transp, class Flash>
void ERaProto<Transp, Flash>::runERaTask() {
#if defined(ERA_MODBUS)
	Base::Modbus::runRead();
	Base::Modbus::runWrite();
#endif
}

static ERaFlash flash;
static EthernetClient eraEthernetClient;
static ERaMqtt<EthernetClient, MQTTClient> mqtt(eraEthernetClient);
ERaEthernet< ERaMqtt<EthernetClient, MQTTClient> > ERa(mqtt, flash);

#endif /* INC_ERA_SIMPLE_ETHERNET_HPP_ */
