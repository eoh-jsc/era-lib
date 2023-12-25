#include <stdio.h>
#include <stdint.h>
#include <ERa/ERaHandlers.hpp>
#include <ERa/ERaDebug.hpp>

void ERaNoHandler() {
}

void ERaNoOptConnected(void ERA_UNUSED *arg) {
}

void ERaNoModbusBaudrate(uint32_t ERA_UNUSED &baudrate) {
}

void ERaNoInfo(cJSON ERA_UNUSED *root) {
}

void ERaWidgetWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) {
    ERA_LOG(ERA_PSTR("ERa"), ERA_PSTR("No handler for writing to V%u"), pin);
}

void ERaWidgetPinRead(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) {
}

bool ERaWidgetPinWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) {
    return false;
}

#define ERA_ON_WRITE(Pin) void ERaWidgetWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) \
        __attribute__((weak, alias("ERaWidgetWrite")))

#define ERA_ON_PIN_READ(Pin) void ERaWidgetPinRead ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) \
        __attribute__((weak, alias("ERaWidgetPinRead")))

#define ERA_ON_PIN_WRITE(Pin) bool ERaWidgetPinWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) \
        __attribute__((weak, alias("ERaWidgetPinWrite")))

ERA_CONNECTED() __attribute__((weak, alias("ERaNoHandler")));
ERA_DISCONNECTED() __attribute__((weak, alias("ERaNoHandler")));
ERA_WAITING() __attribute__((weak, alias("ERaNoHandler")));
ERA_OPTION_CONNECTED() __attribute__((weak, alias("ERaNoOptConnected")));
ERA_MODBUS_BAUDRATE() __attribute__((weak, alias("ERaNoModbusBaudrate")));
ERA_INFO() __attribute__((weak, alias("ERaNoInfo")));
ERA_MODBUS_INFO() __attribute__((weak, alias("ERaNoInfo")));

ERA_ON_WRITE(Default);
ERA_ON_PIN_READ(Default);
ERA_ON_PIN_WRITE(Default);

ERA_ON_WRITE( 0);
ERA_ON_WRITE( 1);
ERA_ON_WRITE( 2);
ERA_ON_WRITE( 3);
ERA_ON_WRITE( 4);
ERA_ON_WRITE( 5);
ERA_ON_WRITE( 6);
ERA_ON_WRITE( 7);
ERA_ON_WRITE( 8);
ERA_ON_WRITE( 9);
ERA_ON_WRITE(10);
ERA_ON_WRITE(11);
ERA_ON_WRITE(12);
ERA_ON_WRITE(13);
ERA_ON_WRITE(14);
ERA_ON_WRITE(15);
ERA_ON_WRITE(16);
ERA_ON_WRITE(17);
ERA_ON_WRITE(18);
ERA_ON_WRITE(19);
ERA_ON_WRITE(20);
ERA_ON_WRITE(21);
ERA_ON_WRITE(22);
ERA_ON_WRITE(23);
ERA_ON_WRITE(24);
ERA_ON_WRITE(25);
ERA_ON_WRITE(26);
ERA_ON_WRITE(27);
ERA_ON_WRITE(28);
ERA_ON_WRITE(29);
ERA_ON_WRITE(30);
ERA_ON_WRITE(31);
ERA_ON_WRITE(32);
ERA_ON_WRITE(33);
ERA_ON_WRITE(34);
ERA_ON_WRITE(35);
ERA_ON_WRITE(36);
ERA_ON_WRITE(37);
ERA_ON_WRITE(38);
ERA_ON_WRITE(39);
ERA_ON_WRITE(40);
ERA_ON_WRITE(41);
ERA_ON_WRITE(42);
ERA_ON_WRITE(43);
ERA_ON_WRITE(44);
ERA_ON_WRITE(45);
ERA_ON_WRITE(46);
ERA_ON_WRITE(47);
ERA_ON_WRITE(48);
ERA_ON_WRITE(49);
#if defined(ERA_100_PINS)
    ERA_ON_WRITE(50);
    ERA_ON_WRITE(51);
    ERA_ON_WRITE(52);
    ERA_ON_WRITE(53);
    ERA_ON_WRITE(54);
    ERA_ON_WRITE(55);
    ERA_ON_WRITE(56);
    ERA_ON_WRITE(57);
    ERA_ON_WRITE(58);
    ERA_ON_WRITE(59);
    ERA_ON_WRITE(60);
    ERA_ON_WRITE(61);
    ERA_ON_WRITE(62);
    ERA_ON_WRITE(63);
    ERA_ON_WRITE(64);
    ERA_ON_WRITE(65);
    ERA_ON_WRITE(66);
    ERA_ON_WRITE(67);
    ERA_ON_WRITE(68);
    ERA_ON_WRITE(69);
    ERA_ON_WRITE(70);
    ERA_ON_WRITE(71);
    ERA_ON_WRITE(72);
    ERA_ON_WRITE(73);
    ERA_ON_WRITE(74);
    ERA_ON_WRITE(75);
    ERA_ON_WRITE(76);
    ERA_ON_WRITE(77);
    ERA_ON_WRITE(78);
    ERA_ON_WRITE(79);
    ERA_ON_WRITE(80);
    ERA_ON_WRITE(81);
    ERA_ON_WRITE(82);
    ERA_ON_WRITE(83);
    ERA_ON_WRITE(84);
    ERA_ON_WRITE(85);
    ERA_ON_WRITE(86);
    ERA_ON_WRITE(87);
    ERA_ON_WRITE(88);
    ERA_ON_WRITE(89);
    ERA_ON_WRITE(90);
    ERA_ON_WRITE(91);
    ERA_ON_WRITE(92);
    ERA_ON_WRITE(93);
    ERA_ON_WRITE(94);
    ERA_ON_WRITE(95);
    ERA_ON_WRITE(96);
    ERA_ON_WRITE(97);
    ERA_ON_WRITE(98);
    ERA_ON_WRITE(99);
#endif

ERA_ON_PIN_READ( 0);
ERA_ON_PIN_READ( 1);
ERA_ON_PIN_READ( 2);
ERA_ON_PIN_READ( 3);
ERA_ON_PIN_READ( 4);
ERA_ON_PIN_READ( 5);
ERA_ON_PIN_READ( 6);
ERA_ON_PIN_READ( 7);
ERA_ON_PIN_READ( 8);
ERA_ON_PIN_READ( 9);
ERA_ON_PIN_READ(10);
ERA_ON_PIN_READ(11);
ERA_ON_PIN_READ(12);
ERA_ON_PIN_READ(13);
ERA_ON_PIN_READ(14);
ERA_ON_PIN_READ(15);
ERA_ON_PIN_READ(16);
ERA_ON_PIN_READ(17);
ERA_ON_PIN_READ(18);
ERA_ON_PIN_READ(19);
ERA_ON_PIN_READ(20);
ERA_ON_PIN_READ(21);
ERA_ON_PIN_READ(22);
ERA_ON_PIN_READ(23);
ERA_ON_PIN_READ(24);
ERA_ON_PIN_READ(25);
ERA_ON_PIN_READ(26);
ERA_ON_PIN_READ(27);
ERA_ON_PIN_READ(28);
ERA_ON_PIN_READ(29);
ERA_ON_PIN_READ(30);
ERA_ON_PIN_READ(31);
ERA_ON_PIN_READ(32);
ERA_ON_PIN_READ(33);
ERA_ON_PIN_READ(34);
ERA_ON_PIN_READ(35);
ERA_ON_PIN_READ(36);
ERA_ON_PIN_READ(37);
ERA_ON_PIN_READ(38);
ERA_ON_PIN_READ(39);
ERA_ON_PIN_READ(40);
ERA_ON_PIN_READ(41);
ERA_ON_PIN_READ(42);
ERA_ON_PIN_READ(43);
ERA_ON_PIN_READ(44);
ERA_ON_PIN_READ(45);
ERA_ON_PIN_READ(46);
ERA_ON_PIN_READ(47);
ERA_ON_PIN_READ(48);
ERA_ON_PIN_READ(49);
#if defined(ERA_100_PINS)
    ERA_ON_PIN_READ(50);
    ERA_ON_PIN_READ(51);
    ERA_ON_PIN_READ(52);
    ERA_ON_PIN_READ(53);
    ERA_ON_PIN_READ(54);
    ERA_ON_PIN_READ(55);
    ERA_ON_PIN_READ(56);
    ERA_ON_PIN_READ(57);
    ERA_ON_PIN_READ(58);
    ERA_ON_PIN_READ(59);
    ERA_ON_PIN_READ(60);
    ERA_ON_PIN_READ(61);
    ERA_ON_PIN_READ(62);
    ERA_ON_PIN_READ(63);
    ERA_ON_PIN_READ(64);
    ERA_ON_PIN_READ(65);
    ERA_ON_PIN_READ(66);
    ERA_ON_PIN_READ(67);
    ERA_ON_PIN_READ(68);
    ERA_ON_PIN_READ(69);
    ERA_ON_PIN_READ(70);
    ERA_ON_PIN_READ(71);
    ERA_ON_PIN_READ(72);
    ERA_ON_PIN_READ(73);
    ERA_ON_PIN_READ(74);
    ERA_ON_PIN_READ(75);
    ERA_ON_PIN_READ(76);
    ERA_ON_PIN_READ(77);
    ERA_ON_PIN_READ(78);
    ERA_ON_PIN_READ(79);
    ERA_ON_PIN_READ(80);
    ERA_ON_PIN_READ(81);
    ERA_ON_PIN_READ(82);
    ERA_ON_PIN_READ(83);
    ERA_ON_PIN_READ(84);
    ERA_ON_PIN_READ(85);
    ERA_ON_PIN_READ(86);
    ERA_ON_PIN_READ(87);
    ERA_ON_PIN_READ(88);
    ERA_ON_PIN_READ(89);
    ERA_ON_PIN_READ(90);
    ERA_ON_PIN_READ(91);
    ERA_ON_PIN_READ(92);
    ERA_ON_PIN_READ(93);
    ERA_ON_PIN_READ(94);
    ERA_ON_PIN_READ(95);
    ERA_ON_PIN_READ(96);
    ERA_ON_PIN_READ(97);
    ERA_ON_PIN_READ(98);
    ERA_ON_PIN_READ(99);
#endif

ERA_ON_PIN_WRITE( 0);
ERA_ON_PIN_WRITE( 1);
ERA_ON_PIN_WRITE( 2);
ERA_ON_PIN_WRITE( 3);
ERA_ON_PIN_WRITE( 4);
ERA_ON_PIN_WRITE( 5);
ERA_ON_PIN_WRITE( 6);
ERA_ON_PIN_WRITE( 7);
ERA_ON_PIN_WRITE( 8);
ERA_ON_PIN_WRITE( 9);
ERA_ON_PIN_WRITE(10);
ERA_ON_PIN_WRITE(11);
ERA_ON_PIN_WRITE(12);
ERA_ON_PIN_WRITE(13);
ERA_ON_PIN_WRITE(14);
ERA_ON_PIN_WRITE(15);
ERA_ON_PIN_WRITE(16);
ERA_ON_PIN_WRITE(17);
ERA_ON_PIN_WRITE(18);
ERA_ON_PIN_WRITE(19);
ERA_ON_PIN_WRITE(20);
ERA_ON_PIN_WRITE(21);
ERA_ON_PIN_WRITE(22);
ERA_ON_PIN_WRITE(23);
ERA_ON_PIN_WRITE(24);
ERA_ON_PIN_WRITE(25);
ERA_ON_PIN_WRITE(26);
ERA_ON_PIN_WRITE(27);
ERA_ON_PIN_WRITE(28);
ERA_ON_PIN_WRITE(29);
ERA_ON_PIN_WRITE(30);
ERA_ON_PIN_WRITE(31);
ERA_ON_PIN_WRITE(32);
ERA_ON_PIN_WRITE(33);
ERA_ON_PIN_WRITE(34);
ERA_ON_PIN_WRITE(35);
ERA_ON_PIN_WRITE(36);
ERA_ON_PIN_WRITE(37);
ERA_ON_PIN_WRITE(38);
ERA_ON_PIN_WRITE(39);
ERA_ON_PIN_WRITE(40);
ERA_ON_PIN_WRITE(41);
ERA_ON_PIN_WRITE(42);
ERA_ON_PIN_WRITE(43);
ERA_ON_PIN_WRITE(44);
ERA_ON_PIN_WRITE(45);
ERA_ON_PIN_WRITE(46);
ERA_ON_PIN_WRITE(47);
ERA_ON_PIN_WRITE(48);
ERA_ON_PIN_WRITE(49);
#if defined(ERA_100_PINS)
    ERA_ON_PIN_WRITE(50);
    ERA_ON_PIN_WRITE(51);
    ERA_ON_PIN_WRITE(52);
    ERA_ON_PIN_WRITE(53);
    ERA_ON_PIN_WRITE(54);
    ERA_ON_PIN_WRITE(55);
    ERA_ON_PIN_WRITE(56);
    ERA_ON_PIN_WRITE(57);
    ERA_ON_PIN_WRITE(58);
    ERA_ON_PIN_WRITE(59);
    ERA_ON_PIN_WRITE(60);
    ERA_ON_PIN_WRITE(61);
    ERA_ON_PIN_WRITE(62);
    ERA_ON_PIN_WRITE(63);
    ERA_ON_PIN_WRITE(64);
    ERA_ON_PIN_WRITE(65);
    ERA_ON_PIN_WRITE(66);
    ERA_ON_PIN_WRITE(67);
    ERA_ON_PIN_WRITE(68);
    ERA_ON_PIN_WRITE(69);
    ERA_ON_PIN_WRITE(70);
    ERA_ON_PIN_WRITE(71);
    ERA_ON_PIN_WRITE(72);
    ERA_ON_PIN_WRITE(73);
    ERA_ON_PIN_WRITE(74);
    ERA_ON_PIN_WRITE(75);
    ERA_ON_PIN_WRITE(76);
    ERA_ON_PIN_WRITE(77);
    ERA_ON_PIN_WRITE(78);
    ERA_ON_PIN_WRITE(79);
    ERA_ON_PIN_WRITE(80);
    ERA_ON_PIN_WRITE(81);
    ERA_ON_PIN_WRITE(82);
    ERA_ON_PIN_WRITE(83);
    ERA_ON_PIN_WRITE(84);
    ERA_ON_PIN_WRITE(85);
    ERA_ON_PIN_WRITE(86);
    ERA_ON_PIN_WRITE(87);
    ERA_ON_PIN_WRITE(88);
    ERA_ON_PIN_WRITE(89);
    ERA_ON_PIN_WRITE(90);
    ERA_ON_PIN_WRITE(91);
    ERA_ON_PIN_WRITE(92);
    ERA_ON_PIN_WRITE(93);
    ERA_ON_PIN_WRITE(94);
    ERA_ON_PIN_WRITE(95);
    ERA_ON_PIN_WRITE(96);
    ERA_ON_PIN_WRITE(97);
    ERA_ON_PIN_WRITE(98);
    ERA_ON_PIN_WRITE(99);
#endif

static const ERaWriteHandler_t ERaWriteHandlerVector[] ERA_PROGMEM = {
    ERaWidgetWrite0, ERaWidgetWrite1, ERaWidgetWrite2, ERaWidgetWrite3, ERaWidgetWrite4,
    ERaWidgetWrite5, ERaWidgetWrite6, ERaWidgetWrite7, ERaWidgetWrite8, ERaWidgetWrite9,
    ERaWidgetWrite10, ERaWidgetWrite11, ERaWidgetWrite12, ERaWidgetWrite13, ERaWidgetWrite14,
    ERaWidgetWrite15, ERaWidgetWrite16, ERaWidgetWrite17, ERaWidgetWrite18, ERaWidgetWrite19,
    ERaWidgetWrite20, ERaWidgetWrite21, ERaWidgetWrite22, ERaWidgetWrite23, ERaWidgetWrite24,
    ERaWidgetWrite25, ERaWidgetWrite26, ERaWidgetWrite27, ERaWidgetWrite28, ERaWidgetWrite29,
    ERaWidgetWrite30, ERaWidgetWrite31, ERaWidgetWrite32, ERaWidgetWrite33, ERaWidgetWrite34,
    ERaWidgetWrite35, ERaWidgetWrite36, ERaWidgetWrite37, ERaWidgetWrite38, ERaWidgetWrite39,
    ERaWidgetWrite40, ERaWidgetWrite41, ERaWidgetWrite42, ERaWidgetWrite43, ERaWidgetWrite44,
    ERaWidgetWrite45, ERaWidgetWrite46, ERaWidgetWrite47, ERaWidgetWrite48, ERaWidgetWrite49,
#if defined(ERA_100_PINS)
    ERaWidgetWrite50, ERaWidgetWrite51, ERaWidgetWrite52, ERaWidgetWrite53, ERaWidgetWrite54,
    ERaWidgetWrite55, ERaWidgetWrite56, ERaWidgetWrite57, ERaWidgetWrite58, ERaWidgetWrite59,
    ERaWidgetWrite60, ERaWidgetWrite61, ERaWidgetWrite62, ERaWidgetWrite63, ERaWidgetWrite64,
    ERaWidgetWrite65, ERaWidgetWrite66, ERaWidgetWrite67, ERaWidgetWrite68, ERaWidgetWrite69,
    ERaWidgetWrite70, ERaWidgetWrite71, ERaWidgetWrite72, ERaWidgetWrite73, ERaWidgetWrite74,
    ERaWidgetWrite75, ERaWidgetWrite76, ERaWidgetWrite77, ERaWidgetWrite78, ERaWidgetWrite79,
    ERaWidgetWrite80, ERaWidgetWrite81, ERaWidgetWrite82, ERaWidgetWrite83, ERaWidgetWrite84,
    ERaWidgetWrite85, ERaWidgetWrite86, ERaWidgetWrite87, ERaWidgetWrite88, ERaWidgetWrite89,
    ERaWidgetWrite90, ERaWidgetWrite91, ERaWidgetWrite92, ERaWidgetWrite93, ERaWidgetWrite94,
    ERaWidgetWrite95, ERaWidgetWrite96, ERaWidgetWrite97, ERaWidgetWrite98, ERaWidgetWrite99,
#endif
};

static const ERaPinReadHandler_t ERaPinReadHandlerVector[] ERA_PROGMEM = {
    ERaWidgetPinRead0, ERaWidgetPinRead1, ERaWidgetPinRead2, ERaWidgetPinRead3, ERaWidgetPinRead4,
    ERaWidgetPinRead5, ERaWidgetPinRead6, ERaWidgetPinRead7, ERaWidgetPinRead8, ERaWidgetPinRead9,
    ERaWidgetPinRead10, ERaWidgetPinRead11, ERaWidgetPinRead12, ERaWidgetPinRead13, ERaWidgetPinRead14,
    ERaWidgetPinRead15, ERaWidgetPinRead16, ERaWidgetPinRead17, ERaWidgetPinRead18, ERaWidgetPinRead19,
    ERaWidgetPinRead20, ERaWidgetPinRead21, ERaWidgetPinRead22, ERaWidgetPinRead23, ERaWidgetPinRead24,
    ERaWidgetPinRead25, ERaWidgetPinRead26, ERaWidgetPinRead27, ERaWidgetPinRead28, ERaWidgetPinRead29,
    ERaWidgetPinRead30, ERaWidgetPinRead31, ERaWidgetPinRead32, ERaWidgetPinRead33, ERaWidgetPinRead34,
    ERaWidgetPinRead35, ERaWidgetPinRead36, ERaWidgetPinRead37, ERaWidgetPinRead38, ERaWidgetPinRead39,
    ERaWidgetPinRead40, ERaWidgetPinRead41, ERaWidgetPinRead42, ERaWidgetPinRead43, ERaWidgetPinRead44,
    ERaWidgetPinRead45, ERaWidgetPinRead46, ERaWidgetPinRead47, ERaWidgetPinRead48, ERaWidgetPinRead49,
#if defined(ERA_100_PINS)
    ERaWidgetPinRead50, ERaWidgetPinRead51, ERaWidgetPinRead52, ERaWidgetPinRead53, ERaWidgetPinRead54,
    ERaWidgetPinRead55, ERaWidgetPinRead56, ERaWidgetPinRead57, ERaWidgetPinRead58, ERaWidgetPinRead59,
    ERaWidgetPinRead60, ERaWidgetPinRead61, ERaWidgetPinRead62, ERaWidgetPinRead63, ERaWidgetPinRead64,
    ERaWidgetPinRead65, ERaWidgetPinRead66, ERaWidgetPinRead67, ERaWidgetPinRead68, ERaWidgetPinRead69,
    ERaWidgetPinRead70, ERaWidgetPinRead71, ERaWidgetPinRead72, ERaWidgetPinRead73, ERaWidgetPinRead74,
    ERaWidgetPinRead75, ERaWidgetPinRead76, ERaWidgetPinRead77, ERaWidgetPinRead78, ERaWidgetPinRead79,
    ERaWidgetPinRead80, ERaWidgetPinRead81, ERaWidgetPinRead82, ERaWidgetPinRead83, ERaWidgetPinRead84,
    ERaWidgetPinRead85, ERaWidgetPinRead86, ERaWidgetPinRead87, ERaWidgetPinRead88, ERaWidgetPinRead89,
    ERaWidgetPinRead90, ERaWidgetPinRead91, ERaWidgetPinRead92, ERaWidgetPinRead93, ERaWidgetPinRead94,
    ERaWidgetPinRead95, ERaWidgetPinRead96, ERaWidgetPinRead97, ERaWidgetPinRead98, ERaWidgetPinRead99,
#endif
};

static const ERaPinWriteHandler_t ERaPinWriteHandlerVector[] ERA_PROGMEM = {
    ERaWidgetPinWrite0, ERaWidgetPinWrite1, ERaWidgetPinWrite2, ERaWidgetPinWrite3, ERaWidgetPinWrite4,
    ERaWidgetPinWrite5, ERaWidgetPinWrite6, ERaWidgetPinWrite7, ERaWidgetPinWrite8, ERaWidgetPinWrite9,
    ERaWidgetPinWrite10, ERaWidgetPinWrite11, ERaWidgetPinWrite12, ERaWidgetPinWrite13, ERaWidgetPinWrite14,
    ERaWidgetPinWrite15, ERaWidgetPinWrite16, ERaWidgetPinWrite17, ERaWidgetPinWrite18, ERaWidgetPinWrite19,
    ERaWidgetPinWrite20, ERaWidgetPinWrite21, ERaWidgetPinWrite22, ERaWidgetPinWrite23, ERaWidgetPinWrite24,
    ERaWidgetPinWrite25, ERaWidgetPinWrite26, ERaWidgetPinWrite27, ERaWidgetPinWrite28, ERaWidgetPinWrite29,
    ERaWidgetPinWrite30, ERaWidgetPinWrite31, ERaWidgetPinWrite32, ERaWidgetPinWrite33, ERaWidgetPinWrite34,
    ERaWidgetPinWrite35, ERaWidgetPinWrite36, ERaWidgetPinWrite37, ERaWidgetPinWrite38, ERaWidgetPinWrite39,
    ERaWidgetPinWrite40, ERaWidgetPinWrite41, ERaWidgetPinWrite42, ERaWidgetPinWrite43, ERaWidgetPinWrite44,
    ERaWidgetPinWrite45, ERaWidgetPinWrite46, ERaWidgetPinWrite47, ERaWidgetPinWrite48, ERaWidgetPinWrite49,
#if defined(ERA_100_PINS)
    ERaWidgetPinWrite50, ERaWidgetPinWrite51, ERaWidgetPinWrite52, ERaWidgetPinWrite53, ERaWidgetPinWrite54,
    ERaWidgetPinWrite55, ERaWidgetPinWrite56, ERaWidgetPinWrite57, ERaWidgetPinWrite58, ERaWidgetPinWrite59,
    ERaWidgetPinWrite60, ERaWidgetPinWrite61, ERaWidgetPinWrite62, ERaWidgetPinWrite63, ERaWidgetPinWrite64,
    ERaWidgetPinWrite65, ERaWidgetPinWrite66, ERaWidgetPinWrite67, ERaWidgetPinWrite68, ERaWidgetPinWrite69,
    ERaWidgetPinWrite70, ERaWidgetPinWrite71, ERaWidgetPinWrite72, ERaWidgetPinWrite73, ERaWidgetPinWrite74,
    ERaWidgetPinWrite75, ERaWidgetPinWrite76, ERaWidgetPinWrite77, ERaWidgetPinWrite78, ERaWidgetPinWrite79,
    ERaWidgetPinWrite80, ERaWidgetPinWrite81, ERaWidgetPinWrite82, ERaWidgetPinWrite83, ERaWidgetPinWrite84,
    ERaWidgetPinWrite85, ERaWidgetPinWrite86, ERaWidgetPinWrite87, ERaWidgetPinWrite88, ERaWidgetPinWrite89,
    ERaWidgetPinWrite90, ERaWidgetPinWrite91, ERaWidgetPinWrite92, ERaWidgetPinWrite93, ERaWidgetPinWrite94,
    ERaWidgetPinWrite95, ERaWidgetPinWrite96, ERaWidgetPinWrite97, ERaWidgetPinWrite98, ERaWidgetPinWrite99,
#endif
};

ERaWriteHandler_t getERaWriteHandler(uint8_t pin) {
    if (pin >= ERA_COUNT_OF(ERaWriteHandlerVector)) {
        return nullptr;
    }
#if defined(ERA_HAS_PROGMEM)
    return (ERaWriteHandler_t)pgm_read_ptr(&ERaWriteHandlerVector[pin]);
#else
    return ERaWriteHandlerVector[pin];
#endif
}

ERaPinReadHandler_t getERaPinReadHandler(uint8_t pin) {
    if (pin >= ERA_COUNT_OF(ERaPinReadHandlerVector)) {
        return nullptr;
    }
#if defined(ERA_HAS_PROGMEM)
    return (ERaPinReadHandler_t)pgm_read_ptr(&ERaPinReadHandlerVector[pin]);
#else
    return ERaPinReadHandlerVector[pin];
#endif
}

ERaPinWriteHandler_t getERaPinWriteHandler(uint8_t pin) {
    if (pin >= ERA_COUNT_OF(ERaPinWriteHandlerVector)) {
        return nullptr;
    }
#if defined(ERA_HAS_PROGMEM)
    return (ERaPinWriteHandler_t)pgm_read_ptr(&ERaPinWriteHandlerVector[pin]);
#else
    return ERaPinWriteHandlerVector[pin];
#endif
}
