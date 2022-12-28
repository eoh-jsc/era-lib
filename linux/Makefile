# 
# To build on RaspberryPi:
# 1. Install WiringPi: http://wiringpi.com/download-and-install/
# 2. Run:
#    make clean all target=raspberry
#    sudo era --token=YourAuthToken
#

CC ?= gcc
CXX ?= g++
CXXFLAGS += -I ../src/ -I ./ -DLINUX
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

ifeq ($(target),raspberry)
	CXXFLAGS += -DRASPBERRY
	LDFLAGS += -lwiringPi
endif

ifeq ($(modbus),true)
	CXXFLAGS += -DERA_MODBUS
endif

ifeq ($(zigbee),true)
	CXXFLAGS += -DERA_ZIGBEE
endif

ifeq ($(config),true)
	CXXFLAGS += -DERA_ASK_CONFIG_WHEN_RESTART
endif

ifeq ($(debug),true)
	CXXFLAGS += -DERA_DEBUG
endif

ifeq ($(dump),true)
	CXXFLAGS += -DERA_DEBUG_DUMP
endif

SOURCES=main.cpp \
	../src/ERa/ERaTimer.cpp \
	../src/ERa/ERaReport.cpp \
	../src/ERa/ERaHandlers.cpp \
	../src/Utility/cJSON.cpp \
	../src/Utility/ERacJSON.cpp \
	../src/Utility/ERaUtility.cpp \
	MQTTLinux/MQTT/unix/unix.cpp \
	MQTTLinux/MQTT/MQTTLinux.cpp

SOURCES_C=../src/MQTT/MQTT/lwmqtt/client.c \
	../src/MQTT/MQTT/lwmqtt/helpers.c \
	../src/MQTT/MQTT/lwmqtt/packet.c \
	../src/MQTT/MQTT/lwmqtt/string.c

OBJECTS=$(SOURCES:.cpp=.o) $(SOURCES_C:.c=.o)
EXECUTABLE=era

all: $(SOURCES) $(SOURCES_C) $(EXECUTABLE)

clean:
	-rm $(OBJECTS) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@
