#include <stdio.h>
#include <stdint.h>
#include <ERa/ERaHandlers.hpp>

void eraNoHandler() {
}

void eraNoModbusBaudrate(uint32_t ERA_UNUSED& baudrate) {
}

void eraWidgetWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) {
}

void eraWidgetPinWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) {
}

void eraWidgetPinRead(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) {
}

#define ERA_ON_WRITE(Pin) void eraWidgetWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) \
        __attribute__((weak, alias("eraWidgetWrite")))

#define ERA_ON_PIN_WRITE(Pin) void eraWidgetPinWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) \
        __attribute__((weak, alias("eraWidgetPinWrite")))

#define ERA_ON_PIN_READ(Pin) void eraWidgetPinRead ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) \
        __attribute__((weak, alias("eraWidgetPinRead")))

ERA_CONNECTED() __attribute__((weak, alias("eraNoHandler")));
ERA_DISCONNECTED() __attribute__((weak, alias("eraNoHandler")));
ERA_WAITING() __attribute__((weak, alias("eraNoHandler")));
ERA_MODBUS_BAUDRATE() __attribute__((weak, alias("eraNoModbusBaudrate")));

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

static const ERaWriteHandler eraWriteHandler[] = {
    eraWidgetWrite0, eraWidgetWrite1, eraWidgetWrite2, eraWidgetWrite3, eraWidgetWrite4,
    eraWidgetWrite5, eraWidgetWrite6, eraWidgetWrite7, eraWidgetWrite8, eraWidgetWrite9,
    eraWidgetWrite10, eraWidgetWrite11, eraWidgetWrite12, eraWidgetWrite13, eraWidgetWrite14,
    eraWidgetWrite15, eraWidgetWrite16, eraWidgetWrite17, eraWidgetWrite18, eraWidgetWrite19,
    eraWidgetWrite20, eraWidgetWrite21, eraWidgetWrite22, eraWidgetWrite23, eraWidgetWrite24,
    eraWidgetWrite25, eraWidgetWrite26, eraWidgetWrite27, eraWidgetWrite28, eraWidgetWrite29,
    eraWidgetWrite30, eraWidgetWrite31, eraWidgetWrite32, eraWidgetWrite33, eraWidgetWrite34,
    eraWidgetWrite35, eraWidgetWrite36, eraWidgetWrite37, eraWidgetWrite38, eraWidgetWrite39,
    eraWidgetWrite40, eraWidgetWrite41, eraWidgetWrite42, eraWidgetWrite43, eraWidgetWrite44,
    eraWidgetWrite45, eraWidgetWrite46, eraWidgetWrite47, eraWidgetWrite48, eraWidgetWrite49,
#if defined(ERA_100_PINS)
    eraWidgetWrite50, eraWidgetWrite51, eraWidgetWrite52, eraWidgetWrite53, eraWidgetWrite54,
    eraWidgetWrite55, eraWidgetWrite56, eraWidgetWrite57, eraWidgetWrite58, eraWidgetWrite59,
    eraWidgetWrite60, eraWidgetWrite61, eraWidgetWrite62, eraWidgetWrite63, eraWidgetWrite64,
    eraWidgetWrite65, eraWidgetWrite66, eraWidgetWrite67, eraWidgetWrite68, eraWidgetWrite69,
    eraWidgetWrite70, eraWidgetWrite71, eraWidgetWrite72, eraWidgetWrite73, eraWidgetWrite74,
    eraWidgetWrite75, eraWidgetWrite76, eraWidgetWrite77, eraWidgetWrite78, eraWidgetWrite79,
    eraWidgetWrite80, eraWidgetWrite81, eraWidgetWrite82, eraWidgetWrite83, eraWidgetWrite84,
    eraWidgetWrite85, eraWidgetWrite86, eraWidgetWrite87, eraWidgetWrite88, eraWidgetWrite89,
    eraWidgetWrite90, eraWidgetWrite91, eraWidgetWrite92, eraWidgetWrite93, eraWidgetWrite94,
    eraWidgetWrite95, eraWidgetWrite96, eraWidgetWrite97, eraWidgetWrite98, eraWidgetWrite99,
#endif
};

static const ERaPinWriteHandler eraPinWriteHandler[] = {
    eraWidgetPinWrite0, eraWidgetPinWrite1, eraWidgetPinWrite2, eraWidgetPinWrite3, eraWidgetPinWrite4,
    eraWidgetPinWrite5, eraWidgetPinWrite6, eraWidgetPinWrite7, eraWidgetPinWrite8, eraWidgetPinWrite9,
    eraWidgetPinWrite10, eraWidgetPinWrite11, eraWidgetPinWrite12, eraWidgetPinWrite13, eraWidgetPinWrite14,
    eraWidgetPinWrite15, eraWidgetPinWrite16, eraWidgetPinWrite17, eraWidgetPinWrite18, eraWidgetPinWrite19,
    eraWidgetPinWrite20, eraWidgetPinWrite21, eraWidgetPinWrite22, eraWidgetPinWrite23, eraWidgetPinWrite24,
    eraWidgetPinWrite25, eraWidgetPinWrite26, eraWidgetPinWrite27, eraWidgetPinWrite28, eraWidgetPinWrite29,
    eraWidgetPinWrite30, eraWidgetPinWrite31, eraWidgetPinWrite32, eraWidgetPinWrite33, eraWidgetPinWrite34,
    eraWidgetPinWrite35, eraWidgetPinWrite36, eraWidgetPinWrite37, eraWidgetPinWrite38, eraWidgetPinWrite39,
    eraWidgetPinWrite40, eraWidgetPinWrite41, eraWidgetPinWrite42, eraWidgetPinWrite43, eraWidgetPinWrite44,
    eraWidgetPinWrite45, eraWidgetPinWrite46, eraWidgetPinWrite47, eraWidgetPinWrite48, eraWidgetPinWrite49,
#if defined(ERA_100_PINS)
    eraWidgetPinWrite50, eraWidgetPinWrite51, eraWidgetPinWrite52, eraWidgetPinWrite53, eraWidgetPinWrite54,
    eraWidgetPinWrite55, eraWidgetPinWrite56, eraWidgetPinWrite57, eraWidgetPinWrite58, eraWidgetPinWrite59,
    eraWidgetPinWrite60, eraWidgetPinWrite61, eraWidgetPinWrite62, eraWidgetPinWrite63, eraWidgetPinWrite64,
    eraWidgetPinWrite65, eraWidgetPinWrite66, eraWidgetPinWrite67, eraWidgetPinWrite68, eraWidgetPinWrite69,
    eraWidgetPinWrite70, eraWidgetPinWrite71, eraWidgetPinWrite72, eraWidgetPinWrite73, eraWidgetPinWrite74,
    eraWidgetPinWrite75, eraWidgetPinWrite76, eraWidgetPinWrite77, eraWidgetPinWrite78, eraWidgetPinWrite79,
    eraWidgetPinWrite80, eraWidgetPinWrite81, eraWidgetPinWrite82, eraWidgetPinWrite83, eraWidgetPinWrite84,
    eraWidgetPinWrite85, eraWidgetPinWrite86, eraWidgetPinWrite87, eraWidgetPinWrite88, eraWidgetPinWrite89,
    eraWidgetPinWrite90, eraWidgetPinWrite91, eraWidgetPinWrite92, eraWidgetPinWrite93, eraWidgetPinWrite94,
    eraWidgetPinWrite95, eraWidgetPinWrite96, eraWidgetPinWrite97, eraWidgetPinWrite98, eraWidgetPinWrite99,
#endif
};

static const ERaPinReadHandler eraPinReadHandler[] = {
    eraWidgetPinRead0, eraWidgetPinRead1, eraWidgetPinRead2, eraWidgetPinRead3, eraWidgetPinRead4,
    eraWidgetPinRead5, eraWidgetPinRead6, eraWidgetPinRead7, eraWidgetPinRead8, eraWidgetPinRead9,
    eraWidgetPinRead10, eraWidgetPinRead11, eraWidgetPinRead12, eraWidgetPinRead13, eraWidgetPinRead14,
    eraWidgetPinRead15, eraWidgetPinRead16, eraWidgetPinRead17, eraWidgetPinRead18, eraWidgetPinRead19,
    eraWidgetPinRead20, eraWidgetPinRead21, eraWidgetPinRead22, eraWidgetPinRead23, eraWidgetPinRead24,
    eraWidgetPinRead25, eraWidgetPinRead26, eraWidgetPinRead27, eraWidgetPinRead28, eraWidgetPinRead29,
    eraWidgetPinRead30, eraWidgetPinRead31, eraWidgetPinRead32, eraWidgetPinRead33, eraWidgetPinRead34,
    eraWidgetPinRead35, eraWidgetPinRead36, eraWidgetPinRead37, eraWidgetPinRead38, eraWidgetPinRead39,
    eraWidgetPinRead40, eraWidgetPinRead41, eraWidgetPinRead42, eraWidgetPinRead43, eraWidgetPinRead44,
    eraWidgetPinRead45, eraWidgetPinRead46, eraWidgetPinRead47, eraWidgetPinRead48, eraWidgetPinRead49,
#if defined(ERA_100_PINS)
    eraWidgetPinRead50, eraWidgetPinRead51, eraWidgetPinRead52, eraWidgetPinRead53, eraWidgetPinRead54,
    eraWidgetPinRead55, eraWidgetPinRead56, eraWidgetPinRead57, eraWidgetPinRead58, eraWidgetPinRead59,
    eraWidgetPinRead60, eraWidgetPinRead61, eraWidgetPinRead62, eraWidgetPinRead63, eraWidgetPinRead64,
    eraWidgetPinRead65, eraWidgetPinRead66, eraWidgetPinRead67, eraWidgetPinRead68, eraWidgetPinRead69,
    eraWidgetPinRead70, eraWidgetPinRead71, eraWidgetPinRead72, eraWidgetPinRead73, eraWidgetPinRead74,
    eraWidgetPinRead75, eraWidgetPinRead76, eraWidgetPinRead77, eraWidgetPinRead78, eraWidgetPinRead79,
    eraWidgetPinRead80, eraWidgetPinRead81, eraWidgetPinRead82, eraWidgetPinRead83, eraWidgetPinRead84,
    eraWidgetPinRead85, eraWidgetPinRead86, eraWidgetPinRead87, eraWidgetPinRead88, eraWidgetPinRead89,
    eraWidgetPinRead90, eraWidgetPinRead91, eraWidgetPinRead92, eraWidgetPinRead93, eraWidgetPinRead94,
    eraWidgetPinRead95, eraWidgetPinRead96, eraWidgetPinRead97, eraWidgetPinRead98, eraWidgetPinRead99,
#endif
};

ERaWriteHandler getERaWriteHandler(uint8_t pin) {
    if (pin >= ERA_COUNT_OF(eraWriteHandler)) {
        return nullptr;
    }
    return eraWriteHandler[pin];
}

ERaPinWriteHandler getERaPinWriteHandler(uint8_t pin) {
    if (pin >= ERA_COUNT_OF(eraPinWriteHandler)) {
        return nullptr;
    }
    return eraPinWriteHandler[pin];
}

ERaPinReadHandler getERaPinReadHandler(uint8_t pin) {
    if (pin >= ERA_COUNT_OF(eraPinReadHandler)) {
        return nullptr;
    }
    return eraPinReadHandler[pin];
}
