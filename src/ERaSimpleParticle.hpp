#ifndef INC_ERA_SIMPLE_PARTICLE_HPP_
#define INC_ERA_SIMPLE_PARTICLE_HPP_

#define ERA_NO_RTOS

#include <ERa/ERaApiParticleDef.hpp>
#include <Adapters/ERaParticleClient.hpp>
#include <ERa/ERaApiParticle.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashArduino.hpp>
#include <Task/ERaTaskParticle.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<TCPClient, MQTTClient> mqtt;
    ERaParticle< ERaMqtt<TCPClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaParticle< ERaMqtt<TCPClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_PARTICLE_HPP_ */
