#ifndef INC_ERA_HANDLERS_HPP_
#define INC_ERA_HANDLERS_HPP_

#include <ERa/ERaDetect.hpp>
#include <ERa/ERaParam.hpp>

#define V0  0
#define V1  1
#define V2  2
#define V3  3
#define V4  4
#define V5  5
#define V6  6
#define V7  7
#define V8  8
#define V9  9
#define V10 10
#define V11 11
#define V12 12
#define V13 13
#define V14 14
#define V15 15
#define V16 16
#define V17 17
#define V18 18
#define V19 19
#define V20 20
#define V21 21
#define V22 22
#define V23 23
#define V24 24
#define V25 25
#define V26 26
#define V27 27
#define V28 28
#define V29 29
#define V30 30
#define V31 31
#define V32 32
#define V33 33
#define V34 34
#define V35 35
#define V36 36
#define V37 37
#define V38 38
#define V39 39
#define V40 40
#define V41 41
#define V42 42
#define V43 43
#define V44 44
#define V45 45
#define V46 46
#define V47 47
#define V48 48
#define V49 49
#if defined(ERA_100_PINS)
    #define V50 50
    #define V51 51
    #define V52 52
    #define V53 53
    #define V54 54
    #define V55 55
    #define V56 56
    #define V57 57
    #define V58 58
    #define V59 59
    #define V60 60
    #define V61 61
    #define V62 62
    #define V63 63
    #define V64 64
    #define V65 65
    #define V66 66
    #define V67 67
    #define V68 68
    #define V69 69
    #define V70 70
    #define V71 71
    #define V72 72
    #define V73 73
    #define V74 74
    #define V75 75
    #define V76 76
    #define V77 77
    #define V78 78
    #define V79 79
    #define V80 80
    #define V81 81
    #define V82 82
    #define V83 83
    #define V84 84
    #define V85 85
    #define V86 86
    #define V87 87
    #define V88 88
    #define V89 89
    #define V90 90
    #define V91 91
    #define V92 92
    #define V93 93
    #define V94 94
    #define V95 95
    #define V96 96
    #define V97 97
    #define V98 98
    #define V99 99
#endif

#define ERA_CONNECTED()             void ERaOnConnected()
#define ERA_DISCONNECTED()          void ERaOnDisconnected()
#define ERA_WAITING()               void ERaOnWaiting()
#define ERA_OPTION_CONNECTED()      void ERaOptConnected(void ERA_UNUSED *arg)
#define ERA_MODBUS_BAUDRATE()       void ERaModbusBaudrate(uint32_t ERA_UNUSED &baudrate)
#define ERA_INFO()                  void ERaInfo(cJSON ERA_UNUSED *root)
#define ERA_MODBUS_INFO()           void ERaModbusInfo(cJSON ERA_UNUSED *root)

#define ERA_WRITE_2(Pin)            void ERaWidgetWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param)
#define ERA_WRITE(Pin)              ERA_WRITE_2(Pin)

#define ERA_PIN_READ_2(Pin)         void ERaWidgetPinRead ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw)
#define ERA_PIN_READ(Pin)           ERA_PIN_READ_2(Pin)

#define ERA_PIN_WRITE_2(Pin)        bool ERaWidgetPinWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw)
#define ERA_PIN_WRITE(Pin)          ERA_PIN_WRITE_2(Pin)

#define ERA_WRITE_DEFAULT()         ERA_WRITE_2(Default)
#define ERA_PIN_READ_DEFAULT()      ERA_PIN_READ_2(Default)
#define ERA_PIN_WRITE_DEFAULT()     ERA_PIN_WRITE_2(Default)

#define ERA_ON_CHANGE(Pin)          ERA_WRITE(Pin)
#define ERA_ON_RECEIVE(Pin)         ERA_WRITE(Pin)
#define ERA_ON_CHANGE_DEFAULT()     ERA_WRITE_DEFAULT()
#define ERA_ON_RECEIVE_DEFAULT()    ERA_WRITE_DEFAULT()

#ifdef __cplusplus
extern "C" {
#endif

void ERaNoHandler();
void ERaNoInfo(cJSON ERA_UNUSED *root);
void ERaNoOptConnected(void ERA_UNUSED *arg);
void ERaNoModbusBaudrate(uint32_t ERA_UNUSED &baudrate);

ERA_CONNECTED();
ERA_DISCONNECTED();
ERA_WAITING();
ERA_OPTION_CONNECTED();
ERA_MODBUS_BAUDRATE();
ERA_INFO();
ERA_MODBUS_INFO();

ERA_WRITE_DEFAULT();
ERA_PIN_READ_DEFAULT();
ERA_PIN_WRITE_DEFAULT();

ERA_WRITE(  );
ERA_WRITE( 0);
ERA_WRITE( 1);
ERA_WRITE( 2);
ERA_WRITE( 3);
ERA_WRITE( 4);
ERA_WRITE( 5);
ERA_WRITE( 6);
ERA_WRITE( 7);
ERA_WRITE( 8);
ERA_WRITE( 9);
ERA_WRITE(10);
ERA_WRITE(11);
ERA_WRITE(12);
ERA_WRITE(13);
ERA_WRITE(14);
ERA_WRITE(15);
ERA_WRITE(16);
ERA_WRITE(17);
ERA_WRITE(18);
ERA_WRITE(19);
ERA_WRITE(20);
ERA_WRITE(21);
ERA_WRITE(22);
ERA_WRITE(23);
ERA_WRITE(24);
ERA_WRITE(25);
ERA_WRITE(26);
ERA_WRITE(27);
ERA_WRITE(28);
ERA_WRITE(29);
ERA_WRITE(30);
ERA_WRITE(31);
ERA_WRITE(32);
ERA_WRITE(33);
ERA_WRITE(34);
ERA_WRITE(35);
ERA_WRITE(36);
ERA_WRITE(37);
ERA_WRITE(38);
ERA_WRITE(39);
ERA_WRITE(40);
ERA_WRITE(41);
ERA_WRITE(42);
ERA_WRITE(43);
ERA_WRITE(44);
ERA_WRITE(45);
ERA_WRITE(46);
ERA_WRITE(47);
ERA_WRITE(48);
ERA_WRITE(49);
#if defined(ERA_100_PINS)
    ERA_WRITE(50);
    ERA_WRITE(51);
    ERA_WRITE(52);
    ERA_WRITE(53);
    ERA_WRITE(54);
    ERA_WRITE(55);
    ERA_WRITE(56);
    ERA_WRITE(57);
    ERA_WRITE(58);
    ERA_WRITE(59);
    ERA_WRITE(60);
    ERA_WRITE(61);
    ERA_WRITE(62);
    ERA_WRITE(63);
    ERA_WRITE(64);
    ERA_WRITE(65);
    ERA_WRITE(66);
    ERA_WRITE(67);
    ERA_WRITE(68);
    ERA_WRITE(69);
    ERA_WRITE(70);
    ERA_WRITE(71);
    ERA_WRITE(72);
    ERA_WRITE(73);
    ERA_WRITE(74);
    ERA_WRITE(75);
    ERA_WRITE(76);
    ERA_WRITE(77);
    ERA_WRITE(78);
    ERA_WRITE(79);
    ERA_WRITE(80);
    ERA_WRITE(81);
    ERA_WRITE(82);
    ERA_WRITE(83);
    ERA_WRITE(84);
    ERA_WRITE(85);
    ERA_WRITE(86);
    ERA_WRITE(87);
    ERA_WRITE(88);
    ERA_WRITE(89);
    ERA_WRITE(90);
    ERA_WRITE(91);
    ERA_WRITE(92);
    ERA_WRITE(93);
    ERA_WRITE(94);
    ERA_WRITE(95);
    ERA_WRITE(96);
    ERA_WRITE(97);
    ERA_WRITE(98);
    ERA_WRITE(99);
#endif

ERA_PIN_READ(  );
ERA_PIN_READ( 0);
ERA_PIN_READ( 1);
ERA_PIN_READ( 2);
ERA_PIN_READ( 3);
ERA_PIN_READ( 4);
ERA_PIN_READ( 5);
ERA_PIN_READ( 6);
ERA_PIN_READ( 7);
ERA_PIN_READ( 8);
ERA_PIN_READ( 9);
ERA_PIN_READ(10);
ERA_PIN_READ(11);
ERA_PIN_READ(12);
ERA_PIN_READ(13);
ERA_PIN_READ(14);
ERA_PIN_READ(15);
ERA_PIN_READ(16);
ERA_PIN_READ(17);
ERA_PIN_READ(18);
ERA_PIN_READ(19);
ERA_PIN_READ(20);
ERA_PIN_READ(21);
ERA_PIN_READ(22);
ERA_PIN_READ(23);
ERA_PIN_READ(24);
ERA_PIN_READ(25);
ERA_PIN_READ(26);
ERA_PIN_READ(27);
ERA_PIN_READ(28);
ERA_PIN_READ(29);
ERA_PIN_READ(30);
ERA_PIN_READ(31);
ERA_PIN_READ(32);
ERA_PIN_READ(33);
ERA_PIN_READ(34);
ERA_PIN_READ(35);
ERA_PIN_READ(36);
ERA_PIN_READ(37);
ERA_PIN_READ(38);
ERA_PIN_READ(39);
ERA_PIN_READ(40);
ERA_PIN_READ(41);
ERA_PIN_READ(42);
ERA_PIN_READ(43);
ERA_PIN_READ(44);
ERA_PIN_READ(45);
ERA_PIN_READ(46);
ERA_PIN_READ(47);
ERA_PIN_READ(48);
ERA_PIN_READ(49);
#if defined(ERA_100_PINS)
    ERA_PIN_READ(50);
    ERA_PIN_READ(51);
    ERA_PIN_READ(52);
    ERA_PIN_READ(53);
    ERA_PIN_READ(54);
    ERA_PIN_READ(55);
    ERA_PIN_READ(56);
    ERA_PIN_READ(57);
    ERA_PIN_READ(58);
    ERA_PIN_READ(59);
    ERA_PIN_READ(60);
    ERA_PIN_READ(61);
    ERA_PIN_READ(62);
    ERA_PIN_READ(63);
    ERA_PIN_READ(64);
    ERA_PIN_READ(65);
    ERA_PIN_READ(66);
    ERA_PIN_READ(67);
    ERA_PIN_READ(68);
    ERA_PIN_READ(69);
    ERA_PIN_READ(70);
    ERA_PIN_READ(71);
    ERA_PIN_READ(72);
    ERA_PIN_READ(73);
    ERA_PIN_READ(74);
    ERA_PIN_READ(75);
    ERA_PIN_READ(76);
    ERA_PIN_READ(77);
    ERA_PIN_READ(78);
    ERA_PIN_READ(79);
    ERA_PIN_READ(80);
    ERA_PIN_READ(81);
    ERA_PIN_READ(82);
    ERA_PIN_READ(83);
    ERA_PIN_READ(84);
    ERA_PIN_READ(85);
    ERA_PIN_READ(86);
    ERA_PIN_READ(87);
    ERA_PIN_READ(88);
    ERA_PIN_READ(89);
    ERA_PIN_READ(90);
    ERA_PIN_READ(91);
    ERA_PIN_READ(92);
    ERA_PIN_READ(93);
    ERA_PIN_READ(94);
    ERA_PIN_READ(95);
    ERA_PIN_READ(96);
    ERA_PIN_READ(97);
    ERA_PIN_READ(98);
    ERA_PIN_READ(99);
#endif

ERA_PIN_WRITE(  );
ERA_PIN_WRITE( 0);
ERA_PIN_WRITE( 1);
ERA_PIN_WRITE( 2);
ERA_PIN_WRITE( 3);
ERA_PIN_WRITE( 4);
ERA_PIN_WRITE( 5);
ERA_PIN_WRITE( 6);
ERA_PIN_WRITE( 7);
ERA_PIN_WRITE( 8);
ERA_PIN_WRITE( 9);
ERA_PIN_WRITE(10);
ERA_PIN_WRITE(11);
ERA_PIN_WRITE(12);
ERA_PIN_WRITE(13);
ERA_PIN_WRITE(14);
ERA_PIN_WRITE(15);
ERA_PIN_WRITE(16);
ERA_PIN_WRITE(17);
ERA_PIN_WRITE(18);
ERA_PIN_WRITE(19);
ERA_PIN_WRITE(20);
ERA_PIN_WRITE(21);
ERA_PIN_WRITE(22);
ERA_PIN_WRITE(23);
ERA_PIN_WRITE(24);
ERA_PIN_WRITE(25);
ERA_PIN_WRITE(26);
ERA_PIN_WRITE(27);
ERA_PIN_WRITE(28);
ERA_PIN_WRITE(29);
ERA_PIN_WRITE(30);
ERA_PIN_WRITE(31);
ERA_PIN_WRITE(32);
ERA_PIN_WRITE(33);
ERA_PIN_WRITE(34);
ERA_PIN_WRITE(35);
ERA_PIN_WRITE(36);
ERA_PIN_WRITE(37);
ERA_PIN_WRITE(38);
ERA_PIN_WRITE(39);
ERA_PIN_WRITE(40);
ERA_PIN_WRITE(41);
ERA_PIN_WRITE(42);
ERA_PIN_WRITE(43);
ERA_PIN_WRITE(44);
ERA_PIN_WRITE(45);
ERA_PIN_WRITE(46);
ERA_PIN_WRITE(47);
ERA_PIN_WRITE(48);
ERA_PIN_WRITE(49);
#if defined(ERA_100_PINS)
    ERA_PIN_WRITE(50);
    ERA_PIN_WRITE(51);
    ERA_PIN_WRITE(52);
    ERA_PIN_WRITE(53);
    ERA_PIN_WRITE(54);
    ERA_PIN_WRITE(55);
    ERA_PIN_WRITE(56);
    ERA_PIN_WRITE(57);
    ERA_PIN_WRITE(58);
    ERA_PIN_WRITE(59);
    ERA_PIN_WRITE(60);
    ERA_PIN_WRITE(61);
    ERA_PIN_WRITE(62);
    ERA_PIN_WRITE(63);
    ERA_PIN_WRITE(64);
    ERA_PIN_WRITE(65);
    ERA_PIN_WRITE(66);
    ERA_PIN_WRITE(67);
    ERA_PIN_WRITE(68);
    ERA_PIN_WRITE(69);
    ERA_PIN_WRITE(70);
    ERA_PIN_WRITE(71);
    ERA_PIN_WRITE(72);
    ERA_PIN_WRITE(73);
    ERA_PIN_WRITE(74);
    ERA_PIN_WRITE(75);
    ERA_PIN_WRITE(76);
    ERA_PIN_WRITE(77);
    ERA_PIN_WRITE(78);
    ERA_PIN_WRITE(79);
    ERA_PIN_WRITE(80);
    ERA_PIN_WRITE(81);
    ERA_PIN_WRITE(82);
    ERA_PIN_WRITE(83);
    ERA_PIN_WRITE(84);
    ERA_PIN_WRITE(85);
    ERA_PIN_WRITE(86);
    ERA_PIN_WRITE(87);
    ERA_PIN_WRITE(88);
    ERA_PIN_WRITE(89);
    ERA_PIN_WRITE(90);
    ERA_PIN_WRITE(91);
    ERA_PIN_WRITE(92);
    ERA_PIN_WRITE(93);
    ERA_PIN_WRITE(94);
    ERA_PIN_WRITE(95);
    ERA_PIN_WRITE(96);
    ERA_PIN_WRITE(97);
    ERA_PIN_WRITE(98);
    ERA_PIN_WRITE(99);
#endif

typedef void (*ERaWriteHandler_t)(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param);
typedef void (*ERaPinReadHandler_t)(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw);
typedef bool (*ERaPinWriteHandler_t)(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw);

ERaWriteHandler_t getERaWriteHandler(uint8_t pin);
ERaPinReadHandler_t getERaPinReadHandler(uint8_t pin);
ERaPinWriteHandler_t getERaPinWriteHandler(uint8_t pin);

#ifdef __cplusplus
}
#endif

#endif /* INC_ERA_HANDLERS_HPP_ */
