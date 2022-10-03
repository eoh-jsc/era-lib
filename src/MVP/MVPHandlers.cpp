#include <stdio.h>
#include <stdint.h>
#include <MVP/MVPHandlers.hpp>

void mvpNoHandler() {
}

void mvpWidgetWrite(uint8_t MVP_UNUSED &pin, const MVPParam MVP_UNUSED &param) {
}

void mvpWidgetPinWrite(uint8_t MVP_UNUSED &pin, const MVPParam MVP_UNUSED &param) {
}

void mvpWidgetPinRead(uint8_t MVP_UNUSED &pin, const MVPParam MVP_UNUSED &param) {
}

#define MVP_ON_WRITE(Pin) void mvpWidgetWrite ## Pin (uint8_t MVP_UNUSED &pin, const MVPParam MVP_UNUSED &param) \
        __attribute__((weak, alias("mvpWidgetWrite")))

#define MVP_ON_PIN_WRITE(Pin) void mvpWidgetPinWrite ## Pin (uint8_t MVP_UNUSED &pin, const MVPParam MVP_UNUSED &param) \
        __attribute__((weak, alias("mvpWidgetPinWrite")))

#define MVP_ON_PIN_READ(Pin) void mvpWidgetPinRead ## Pin (uint8_t MVP_UNUSED &pin, const MVPParam MVP_UNUSED &param) \
        __attribute__((weak, alias("mvpWidgetPinRead")))

MVP_CONNECTED() __attribute__((weak, alias("mvpNoHandler")));
MVP_DISCONNECTED() __attribute__((weak, alias("mvpNoHandler")));

MVP_ON_WRITE( 0);
MVP_ON_WRITE( 1);
MVP_ON_WRITE( 2);
MVP_ON_WRITE( 3);
MVP_ON_WRITE( 4);
MVP_ON_WRITE( 5);
MVP_ON_WRITE( 6);
MVP_ON_WRITE( 7);
MVP_ON_WRITE( 8);
MVP_ON_WRITE( 9);
MVP_ON_WRITE(10);
MVP_ON_WRITE(11);
MVP_ON_WRITE(12);
MVP_ON_WRITE(13);
MVP_ON_WRITE(14);
MVP_ON_WRITE(15);
MVP_ON_WRITE(16);
MVP_ON_WRITE(17);
MVP_ON_WRITE(18);
MVP_ON_WRITE(19);
MVP_ON_WRITE(20);
MVP_ON_WRITE(21);
MVP_ON_WRITE(22);
MVP_ON_WRITE(23);
MVP_ON_WRITE(24);
MVP_ON_WRITE(25);
MVP_ON_WRITE(26);
MVP_ON_WRITE(27);
MVP_ON_WRITE(28);
MVP_ON_WRITE(29);
MVP_ON_WRITE(30);
MVP_ON_WRITE(31);
MVP_ON_WRITE(32);
MVP_ON_WRITE(33);
MVP_ON_WRITE(34);
MVP_ON_WRITE(35);
MVP_ON_WRITE(36);
MVP_ON_WRITE(37);
MVP_ON_WRITE(38);
MVP_ON_WRITE(39);
MVP_ON_WRITE(40);
MVP_ON_WRITE(41);
MVP_ON_WRITE(42);
MVP_ON_WRITE(43);
MVP_ON_WRITE(44);
MVP_ON_WRITE(45);
MVP_ON_WRITE(46);
MVP_ON_WRITE(47);
MVP_ON_WRITE(48);
MVP_ON_WRITE(49);
MVP_ON_WRITE(50);

MVP_ON_PIN_WRITE( 0);
MVP_ON_PIN_WRITE( 1);
MVP_ON_PIN_WRITE( 2);
MVP_ON_PIN_WRITE( 3);
MVP_ON_PIN_WRITE( 4);
MVP_ON_PIN_WRITE( 5);
MVP_ON_PIN_WRITE( 6);
MVP_ON_PIN_WRITE( 7);
MVP_ON_PIN_WRITE( 8);
MVP_ON_PIN_WRITE( 9);
MVP_ON_PIN_WRITE(10);
MVP_ON_PIN_WRITE(11);
MVP_ON_PIN_WRITE(12);
MVP_ON_PIN_WRITE(13);
MVP_ON_PIN_WRITE(14);
MVP_ON_PIN_WRITE(15);
MVP_ON_PIN_WRITE(16);
MVP_ON_PIN_WRITE(17);
MVP_ON_PIN_WRITE(18);
MVP_ON_PIN_WRITE(19);
MVP_ON_PIN_WRITE(20);
MVP_ON_PIN_WRITE(21);
MVP_ON_PIN_WRITE(22);
MVP_ON_PIN_WRITE(23);
MVP_ON_PIN_WRITE(24);
MVP_ON_PIN_WRITE(25);
MVP_ON_PIN_WRITE(26);
MVP_ON_PIN_WRITE(27);
MVP_ON_PIN_WRITE(28);
MVP_ON_PIN_WRITE(29);
MVP_ON_PIN_WRITE(30);
MVP_ON_PIN_WRITE(31);
MVP_ON_PIN_WRITE(32);
MVP_ON_PIN_WRITE(33);
MVP_ON_PIN_WRITE(34);
MVP_ON_PIN_WRITE(35);
MVP_ON_PIN_WRITE(36);
MVP_ON_PIN_WRITE(37);
MVP_ON_PIN_WRITE(38);
MVP_ON_PIN_WRITE(39);
MVP_ON_PIN_WRITE(40);
MVP_ON_PIN_WRITE(41);
MVP_ON_PIN_WRITE(42);
MVP_ON_PIN_WRITE(43);
MVP_ON_PIN_WRITE(44);
MVP_ON_PIN_WRITE(45);
MVP_ON_PIN_WRITE(46);
MVP_ON_PIN_WRITE(47);
MVP_ON_PIN_WRITE(48);
MVP_ON_PIN_WRITE(49);
MVP_ON_PIN_WRITE(50);

MVP_ON_PIN_READ( 0);
MVP_ON_PIN_READ( 1);
MVP_ON_PIN_READ( 2);
MVP_ON_PIN_READ( 3);
MVP_ON_PIN_READ( 4);
MVP_ON_PIN_READ( 5);
MVP_ON_PIN_READ( 6);
MVP_ON_PIN_READ( 7);
MVP_ON_PIN_READ( 8);
MVP_ON_PIN_READ( 9);
MVP_ON_PIN_READ(10);
MVP_ON_PIN_READ(11);
MVP_ON_PIN_READ(12);
MVP_ON_PIN_READ(13);
MVP_ON_PIN_READ(14);
MVP_ON_PIN_READ(15);
MVP_ON_PIN_READ(16);
MVP_ON_PIN_READ(17);
MVP_ON_PIN_READ(18);
MVP_ON_PIN_READ(19);
MVP_ON_PIN_READ(20);
MVP_ON_PIN_READ(21);
MVP_ON_PIN_READ(22);
MVP_ON_PIN_READ(23);
MVP_ON_PIN_READ(24);
MVP_ON_PIN_READ(25);
MVP_ON_PIN_READ(26);
MVP_ON_PIN_READ(27);
MVP_ON_PIN_READ(28);
MVP_ON_PIN_READ(29);
MVP_ON_PIN_READ(30);
MVP_ON_PIN_READ(31);
MVP_ON_PIN_READ(32);
MVP_ON_PIN_READ(33);
MVP_ON_PIN_READ(34);
MVP_ON_PIN_READ(35);
MVP_ON_PIN_READ(36);
MVP_ON_PIN_READ(37);
MVP_ON_PIN_READ(38);
MVP_ON_PIN_READ(39);
MVP_ON_PIN_READ(40);
MVP_ON_PIN_READ(41);
MVP_ON_PIN_READ(42);
MVP_ON_PIN_READ(43);
MVP_ON_PIN_READ(44);
MVP_ON_PIN_READ(45);
MVP_ON_PIN_READ(46);
MVP_ON_PIN_READ(47);
MVP_ON_PIN_READ(48);
MVP_ON_PIN_READ(49);
MVP_ON_PIN_READ(50);

static const MVPWriteHandler mvpWriteHandler[] = {
    mvpWidgetWrite0, mvpWidgetWrite1, mvpWidgetWrite2, mvpWidgetWrite3, mvpWidgetWrite4,
    mvpWidgetWrite5, mvpWidgetWrite6, mvpWidgetWrite7, mvpWidgetWrite8, mvpWidgetWrite9,
    mvpWidgetWrite10, mvpWidgetWrite11, mvpWidgetWrite12, mvpWidgetWrite13, mvpWidgetWrite14,
    mvpWidgetWrite15, mvpWidgetWrite16, mvpWidgetWrite17, mvpWidgetWrite18, mvpWidgetWrite19,
    mvpWidgetWrite20, mvpWidgetWrite21, mvpWidgetWrite22, mvpWidgetWrite23, mvpWidgetWrite24,
    mvpWidgetWrite25, mvpWidgetWrite26, mvpWidgetWrite27, mvpWidgetWrite28, mvpWidgetWrite29,
    mvpWidgetWrite30, mvpWidgetWrite31, mvpWidgetWrite32, mvpWidgetWrite33, mvpWidgetWrite34,
    mvpWidgetWrite35, mvpWidgetWrite36, mvpWidgetWrite37, mvpWidgetWrite38, mvpWidgetWrite39,
    mvpWidgetWrite40, mvpWidgetWrite41, mvpWidgetWrite42, mvpWidgetWrite43, mvpWidgetWrite44,
    mvpWidgetWrite45, mvpWidgetWrite46, mvpWidgetWrite47, mvpWidgetWrite48, mvpWidgetWrite49,
    mvpWidgetWrite50
};

static const MVPPinWriteHandler mvpPinWriteHandler[] = {
    mvpWidgetPinWrite0, mvpWidgetPinWrite1, mvpWidgetPinWrite2, mvpWidgetPinWrite3, mvpWidgetPinWrite4,
    mvpWidgetPinWrite5, mvpWidgetPinWrite6, mvpWidgetPinWrite7, mvpWidgetPinWrite8, mvpWidgetPinWrite9,
    mvpWidgetPinWrite10, mvpWidgetPinWrite11, mvpWidgetPinWrite12, mvpWidgetPinWrite13, mvpWidgetPinWrite14,
    mvpWidgetPinWrite15, mvpWidgetPinWrite16, mvpWidgetPinWrite17, mvpWidgetPinWrite18, mvpWidgetPinWrite19,
    mvpWidgetPinWrite20, mvpWidgetPinWrite21, mvpWidgetPinWrite22, mvpWidgetPinWrite23, mvpWidgetPinWrite24,
    mvpWidgetPinWrite25, mvpWidgetPinWrite26, mvpWidgetPinWrite27, mvpWidgetPinWrite28, mvpWidgetPinWrite29,
    mvpWidgetPinWrite30, mvpWidgetPinWrite31, mvpWidgetPinWrite32, mvpWidgetPinWrite33, mvpWidgetPinWrite34,
    mvpWidgetPinWrite35, mvpWidgetPinWrite36, mvpWidgetPinWrite37, mvpWidgetPinWrite38, mvpWidgetPinWrite39,
    mvpWidgetPinWrite40, mvpWidgetPinWrite41, mvpWidgetPinWrite42, mvpWidgetPinWrite43, mvpWidgetPinWrite44,
    mvpWidgetPinWrite45, mvpWidgetPinWrite46, mvpWidgetPinWrite47, mvpWidgetPinWrite48, mvpWidgetPinWrite49,
    mvpWidgetPinWrite50
};

static const MVPPinReadHandler mvpPinReadHandler[] = {
    mvpWidgetPinRead0, mvpWidgetPinRead1, mvpWidgetPinRead2, mvpWidgetPinRead3, mvpWidgetPinRead4,
    mvpWidgetPinRead5, mvpWidgetPinRead6, mvpWidgetPinRead7, mvpWidgetPinRead8, mvpWidgetPinRead9,
    mvpWidgetPinRead10, mvpWidgetPinRead11, mvpWidgetPinRead12, mvpWidgetPinRead13, mvpWidgetPinRead14,
    mvpWidgetPinRead15, mvpWidgetPinRead16, mvpWidgetPinRead17, mvpWidgetPinRead18, mvpWidgetPinRead19,
    mvpWidgetPinRead20, mvpWidgetPinRead21, mvpWidgetPinRead22, mvpWidgetPinRead23, mvpWidgetPinRead24,
    mvpWidgetPinRead25, mvpWidgetPinRead26, mvpWidgetPinRead27, mvpWidgetPinRead28, mvpWidgetPinRead29,
    mvpWidgetPinRead30, mvpWidgetPinRead31, mvpWidgetPinRead32, mvpWidgetPinRead33, mvpWidgetPinRead34,
    mvpWidgetPinRead35, mvpWidgetPinRead36, mvpWidgetPinRead37, mvpWidgetPinRead38, mvpWidgetPinRead39,
    mvpWidgetPinRead40, mvpWidgetPinRead41, mvpWidgetPinRead42, mvpWidgetPinRead43, mvpWidgetPinRead44,
    mvpWidgetPinRead45, mvpWidgetPinRead46, mvpWidgetPinRead47, mvpWidgetPinRead48, mvpWidgetPinRead49,
    mvpWidgetPinRead50
};

MVPWriteHandler getMVPWriteHandler(uint8_t pin) {
    if (pin >= MVP_COUNT_OF(mvpWriteHandler)) {
        return nullptr;
    }
    return mvpWriteHandler[pin];
}

MVPPinWriteHandler getMVPPinWriteHandler(uint8_t pin) {
    if (pin >= MVP_COUNT_OF(mvpPinWriteHandler)) {
        return nullptr;
    }
    return mvpPinWriteHandler[pin];
}

MVPPinReadHandler getMVPPinReadHandler(uint8_t pin) {
    if (pin >= MVP_COUNT_OF(mvpPinReadHandler)) {
        return nullptr;
    }
    return mvpPinReadHandler[pin];
}
