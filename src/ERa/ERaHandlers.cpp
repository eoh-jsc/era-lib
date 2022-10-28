#include <stdio.h>
#include <stdint.h>
#include <ERa/ERaHandlers.hpp>

void eraNoHandler() {
}

void eraWidgetWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) {
}

void eraWidgetPinWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) {
}

void eraWidgetPinRead(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) {
}

#define ERA_ON_WRITE(Pin) void eraWidgetWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) \
        __attribute__((weak, alias("eraWidgetWrite")))

#define ERA_ON_PIN_WRITE(Pin) void eraWidgetPinWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) \
        __attribute__((weak, alias("eraWidgetPinWrite")))

#define ERA_ON_PIN_READ(Pin) void eraWidgetPinRead ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) \
        __attribute__((weak, alias("eraWidgetPinRead")))

ERA_CONNECTED() __attribute__((weak, alias("eraNoHandler")));
ERA_DISCONNECTED() __attribute__((weak, alias("eraNoHandler")));

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
ERA_ON_WRITE(50);

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
ERA_ON_PIN_WRITE(50);

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
ERA_ON_PIN_READ(50);

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
    eraWidgetWrite50
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
    eraWidgetPinWrite50
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
    eraWidgetPinRead50
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
