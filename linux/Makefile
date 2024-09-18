# 
# To build on RaspberryPi:
# 1. Install WiringPi: https://github.com/WiringPi/WiringPi.git
# 2. Run:
#    make clean all target=raspberry
#    sudo ./era --token=YourAuthToken
#    sudo ./era --token=YourAuthToken --id=YourBoardID
#
# To build on TinkerBoard:
# 1. Install WiringPi: https://github.com/TinkerBoard/gpio_lib_c.git
# 2. Run:
#    make clean all target=tinker
#    sudo ./era --token=YourAuthToken
#    sudo ./era --token=YourAuthToken --id=YourBoardID
#
# To build on OrangePi:
# 1. Install WiringPi: https://github.com/orangepi-xunlong/wiringOP.git
# 2. Run:
#    make clean all target=orangepi
#    sudo ./era --token=YourAuthToken
#    sudo ./era --token=YourAuthToken --id=YourBoardID
#

CC ?= gcc
CXX ?= g++
CXXFLAGS += -I ../src/ -I ./MbedTLS/ -I ./Core/inc/ -I ./User/inc/ -I ./ -DLINUX
LDFLAGS += -lrt -lpthread

ifeq ($(build),debug)
	CXXFLAGS += -c -g2 -O0 \
		-Wall -Weffc++ \
		-Wextra -Wcast-align \
		-Wchar-subscripts  -Wcomment -Wconversion \
		-Wdisabled-optimization \
		-Wfloat-equal  -Wformat  -Wformat=2 \
		-Wformat-nonliteral -Wformat-security  \
		-Wformat-y2k \
		-Wimplicit  -Wimport  -Winit-self  -Winline \
		-Winvalid-pch   \
		-Wunsafe-loop-optimizations  -Wlong-long -Wmissing-braces \
		-Wmissing-field-initializers -Wmissing-format-attribute   \
		-Wmissing-include-dirs -Wmissing-noreturn \
		-Wpacked  -Wparentheses  -Wpointer-arith \
		-Wredundant-decls -Wreturn-type \
		-Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector \
		-Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch  -Wswitch-default \
		-Wswitch-enum -Wtrigraphs  -Wuninitialized \
		-Wunknown-pragmas  -Wunreachable-code -Wunused \
		-Wunused-function  -Wunused-label  \
		-Wunused-value  -Wunused-variable \
		-Wvolatile-register-var  -Wwrite-strings
	
	# Disable some warnings
	CXXFLAGS += -Wno-variadic-macros -Wno-unused-parameter -Wno-vla
	
	# Uncomment this to get pedantic warnings:
	#CXXFLAGS += -pedantic -Wvariadic-macros -Wunused-parameter -Waggregate-return -Wcast-qual -Wpadded
else
	CXXFLAGS += -c -O3 -w
	LDFLAGS += -s
endif

ifeq ($(modbus),true)
	CXXFLAGS += -DERA_MODBUS
endif

ifeq ($(zigbee),true)
	CXXFLAGS += -DERA_ZIGBEE
endif

ifeq ($(config),false)
	CXXFLAGS += -DERA_DISABLE_SYNC_CONFIG
endif

ifeq ($(debug),true)
	CXXFLAGS += -DERA_DEBUG
endif

ifeq ($(dump),true)
	CXXFLAGS += -DERA_DEBUG_DUMP
endif

ifeq ($(ota),true)
	CXXFLAGS += -DERA_OTA
endif

ifeq ($(nortos),true)
	CXXFLAGS += -DERA_NO_RTOS
endif

ifeq ($(location),VN)
	CXXFLAGS += -DERA_LOCATION_VN
else ifeq ($(location),SG)
	CXXFLAGS += -DERA_LOCATION_SG
endif

ifdef dev_modbus
	CXXFLAGS += -DERA_DEV_MODBUS=$(dev_modbus)
endif

ifdef dev_zigbee
	CXXFLAGS += -DERA_DEV_ZIGBEE=$(dev_zigbee)
endif

ifdef event
	CXXFLAGS += -DERA_MAX_EVENTS=$(event)
endif

ifdef qos
	CXXFLAGS += -DDEFAULT_MQTT_PUBLISH_QOS=$(qos)
	CXXFLAGS += -DDEFAULT_MQTT_SUBSCRIBE_QOS=$(qos)
endif

SOURCES=main.cpp \
	$(wildcard User/src/*.cpp) \
	$(wildcard Core/src/*.cpp) \
	../src/ERa/ERaTimer.cpp \
	../src/ERa/ERaReport.cpp \
	../src/ERa/ERaButton.cpp \
	../src/ERa/ERaHandlers.cpp \
	../src/Utility/ERaLoc.cpp \
	../src/Utility/cJSON.cpp \
	../src/Utility/ERacJSON.cpp \
	../src/Utility/ERaUtility.cpp \
	../src/Utility/Base64.cpp \
	../src/Utility/CRC32.cpp \
	../src/Utility/MD5.cpp \
	Network/ERaNetworkInfo.cpp \
	MQTT/MQTT/posix/posix.cpp \
	MQTT/MQTT/MQTTLinux.cpp

ifeq ($(target),raspberry)
	CXXFLAGS += -DRASPBERRY \
		-DWIRING_PI
	LDFLAGS += -lwiringPi
else ifeq ($(target),tinker)
	CXXFLAGS += -DTINKER_BOARD \
		-DWIRING_PI
	LDFLAGS += -lwiringPi
else ifeq ($(target),orangepi)
	CXXFLAGS += -DORANGE_PI \
		-DWIRING_PI
	LDFLAGS += -lwiringPi
else
	SOURCES += Utility/Compat/SerialLinux.cpp
endif

ifeq ($(findstring true, $(ssl) $(tls)),true)
	CXXFLAGS += -DERA_MQTT_SSL
	SOURCES += MQTT/MQTT/posix/posix_tls.cpp
endif

ifdef cxxflags
	CXXFLAGS += $(cxxflags)
endif

ifdef ldflags
	LDFLAGS += $(ldflags)
endif

SOURCES_C=../src/MQTT/MQTT/lwmqtt/client.c \
	../src/MQTT/MQTT/lwmqtt/helpers.c \
	../src/MQTT/MQTT/lwmqtt/packet.c \
	../src/MQTT/MQTT/lwmqtt/string.c \
	$(wildcard User/src/*.c) \
	$(wildcard Core/src/*.c)

SOURCES_TLS=

ifeq ($(findstring true, $(ssl) $(tls)),true)
	SOURCES_TLS += MbedTLS/libmbedtls.a \
		MbedTLS/libmbedx509.a \
		MbedTLS/libmbedcrypto.a
endif

OBJECTS=$(SOURCES:.cpp=.o) $(SOURCES_C:.c=.o)
EXECUTABLE=era

all: $(SOURCES) $(SOURCES_C) $(EXECUTABLE)

clean:
	-rm $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(OBJECTS) $(LDFLAGS) $(SOURCES_TLS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	$(CC) $(CXXFLAGS) $< -o $@
