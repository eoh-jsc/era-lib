#ifndef INC_ERA_SIMPLE_PARTICLE_SSL_HPP_
#define INC_ERA_SIMPLE_PARTICLE_SSL_HPP_

#define ERA_NO_RTOS
#define ERA_MQTT_SSL

#include <ERa/ERaApiParticleDef.hpp>
#include <Adapters/ERaParticleSSLClient.hpp>
#include <ERa/ERaApiParticle.hpp>
#include <Modbus/ERaModbusArduino.hpp>
#include <Storage/ERaFlashArduino.hpp>
#include <Task/ERaTaskParticle.hpp>

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_ERA)
    static ERaFlash flash;
    static ERaMqtt<BearSSLClient, MQTTClient> mqtt;
    ERaParticle< ERaMqtt<BearSSLClient, MQTTClient> > ERa(mqtt, flash);
#else
    extern ERaParticle< ERaMqtt<BearSSLClient, MQTTClient> > ERa;
#endif

#include <ERa/ERaStatic.hpp>

#endif /* INC_ERA_SIMPLE_PARTICLE_SSL_HPP_ */
