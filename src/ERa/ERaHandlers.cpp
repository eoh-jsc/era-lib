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

void ERaNoWriteConfig(uint8_t ERA_UNUSED type) {
}

void ERaWidgetWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) {
    ERA_LOG(ERA_PSTR("ERa"), ERA_PSTR("No handler for writing to V%u"), pin);
}

void ERaWidgetPinRead(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) {
}

bool ERaWidgetPinWrite(uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) {
    return false;
}

bool ERaNoWidgetWriteSMS(const char* to, const char* message) {
    ERA_LOG(ERA_PSTR("ERa"), ERA_PSTR("No handler for writing SMS to %s: %s"), to, message);
    return false;
}

#define ERA_ON_WRITE(Pin) void ERaWidgetWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param) \
        __attribute__((weak, alias("ERaWidgetWrite")))

#define ERA_ON_PIN_READ(Pin) void ERaWidgetPinRead ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) \
        __attribute__((weak, alias("ERaWidgetPinRead")))

#define ERA_ON_PIN_WRITE(Pin) bool ERaWidgetPinWrite ## Pin (uint8_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw) \
        __attribute__((weak, alias("ERaWidgetPinWrite")))

ERA_CONNECTED()             __attribute__((weak, alias("ERaNoHandler")));
ERA_DISCONNECTED()          __attribute__((weak, alias("ERaNoHandler")));
ERA_WAITING()               __attribute__((weak, alias("ERaNoHandler")));
ERA_APP_LOOP()              __attribute__((weak, alias("ERaNoHandler")));
ERA_OPTION_CONNECTED()      __attribute__((weak, alias("ERaNoOptConnected")));
ERA_MODBUS_BAUDRATE()       __attribute__((weak, alias("ERaNoModbusBaudrate")));
ERA_MODBUS_INFO()           __attribute__((weak, alias("ERaNoInfo")));
ERA_SELF_INFO()             __attribute__((weak, alias("ERaNoInfo")));
ERA_INFO()                  __attribute__((weak, alias("ERaNoInfo")));
ERA_WRITE_CONFIG()          __attribute__((weak, alias("ERaNoWriteConfig")));
ERA_WRITE_SMS()             __attribute__((weak, alias("ERaNoWidgetWriteSMS")));

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
#if defined(ERA_255_PINS)
    ERA_ON_WRITE(100);
    ERA_ON_WRITE(101);
    ERA_ON_WRITE(102);
    ERA_ON_WRITE(103);
    ERA_ON_WRITE(104);
    ERA_ON_WRITE(105);
    ERA_ON_WRITE(106);
    ERA_ON_WRITE(107);
    ERA_ON_WRITE(108);
    ERA_ON_WRITE(109);
    ERA_ON_WRITE(110);
    ERA_ON_WRITE(111);
    ERA_ON_WRITE(112);
    ERA_ON_WRITE(113);
    ERA_ON_WRITE(114);
    ERA_ON_WRITE(115);
    ERA_ON_WRITE(116);
    ERA_ON_WRITE(117);
    ERA_ON_WRITE(118);
    ERA_ON_WRITE(119);
    ERA_ON_WRITE(120);
    ERA_ON_WRITE(121);
    ERA_ON_WRITE(122);
    ERA_ON_WRITE(123);
    ERA_ON_WRITE(124);
    ERA_ON_WRITE(125);
    ERA_ON_WRITE(126);
    ERA_ON_WRITE(127);
    ERA_ON_WRITE(128);
    ERA_ON_WRITE(129);
    ERA_ON_WRITE(130);
    ERA_ON_WRITE(131);
    ERA_ON_WRITE(132);
    ERA_ON_WRITE(133);
    ERA_ON_WRITE(134);
    ERA_ON_WRITE(135);
    ERA_ON_WRITE(136);
    ERA_ON_WRITE(137);
    ERA_ON_WRITE(138);
    ERA_ON_WRITE(139);
    ERA_ON_WRITE(140);
    ERA_ON_WRITE(141);
    ERA_ON_WRITE(142);
    ERA_ON_WRITE(143);
    ERA_ON_WRITE(144);
    ERA_ON_WRITE(145);
    ERA_ON_WRITE(146);
    ERA_ON_WRITE(147);
    ERA_ON_WRITE(148);
    ERA_ON_WRITE(149);
    ERA_ON_WRITE(150);
    ERA_ON_WRITE(151);
    ERA_ON_WRITE(152);
    ERA_ON_WRITE(153);
    ERA_ON_WRITE(154);
    ERA_ON_WRITE(155);
    ERA_ON_WRITE(156);
    ERA_ON_WRITE(157);
    ERA_ON_WRITE(158);
    ERA_ON_WRITE(159);
    ERA_ON_WRITE(160);
    ERA_ON_WRITE(161);
    ERA_ON_WRITE(162);
    ERA_ON_WRITE(163);
    ERA_ON_WRITE(164);
    ERA_ON_WRITE(165);
    ERA_ON_WRITE(166);
    ERA_ON_WRITE(167);
    ERA_ON_WRITE(168);
    ERA_ON_WRITE(169);
    ERA_ON_WRITE(170);
    ERA_ON_WRITE(171);
    ERA_ON_WRITE(172);
    ERA_ON_WRITE(173);
    ERA_ON_WRITE(174);
    ERA_ON_WRITE(175);
    ERA_ON_WRITE(176);
    ERA_ON_WRITE(177);
    ERA_ON_WRITE(178);
    ERA_ON_WRITE(179);
    ERA_ON_WRITE(180);
    ERA_ON_WRITE(181);
    ERA_ON_WRITE(182);
    ERA_ON_WRITE(183);
    ERA_ON_WRITE(184);
    ERA_ON_WRITE(185);
    ERA_ON_WRITE(186);
    ERA_ON_WRITE(187);
    ERA_ON_WRITE(188);
    ERA_ON_WRITE(189);
    ERA_ON_WRITE(190);
    ERA_ON_WRITE(191);
    ERA_ON_WRITE(192);
    ERA_ON_WRITE(193);
    ERA_ON_WRITE(194);
    ERA_ON_WRITE(195);
    ERA_ON_WRITE(196);
    ERA_ON_WRITE(197);
    ERA_ON_WRITE(198);
    ERA_ON_WRITE(199);
    ERA_ON_WRITE(200);
    ERA_ON_WRITE(201);
    ERA_ON_WRITE(202);
    ERA_ON_WRITE(203);
    ERA_ON_WRITE(204);
    ERA_ON_WRITE(205);
    ERA_ON_WRITE(206);
    ERA_ON_WRITE(207);
    ERA_ON_WRITE(208);
    ERA_ON_WRITE(209);
    ERA_ON_WRITE(210);
    ERA_ON_WRITE(211);
    ERA_ON_WRITE(212);
    ERA_ON_WRITE(213);
    ERA_ON_WRITE(214);
    ERA_ON_WRITE(215);
    ERA_ON_WRITE(216);
    ERA_ON_WRITE(217);
    ERA_ON_WRITE(218);
    ERA_ON_WRITE(219);
    ERA_ON_WRITE(220);
    ERA_ON_WRITE(221);
    ERA_ON_WRITE(222);
    ERA_ON_WRITE(223);
    ERA_ON_WRITE(224);
    ERA_ON_WRITE(225);
    ERA_ON_WRITE(226);
    ERA_ON_WRITE(227);
    ERA_ON_WRITE(228);
    ERA_ON_WRITE(229);
    ERA_ON_WRITE(230);
    ERA_ON_WRITE(231);
    ERA_ON_WRITE(232);
    ERA_ON_WRITE(233);
    ERA_ON_WRITE(234);
    ERA_ON_WRITE(235);
    ERA_ON_WRITE(236);
    ERA_ON_WRITE(237);
    ERA_ON_WRITE(238);
    ERA_ON_WRITE(239);
    ERA_ON_WRITE(240);
    ERA_ON_WRITE(241);
    ERA_ON_WRITE(242);
    ERA_ON_WRITE(243);
    ERA_ON_WRITE(244);
    ERA_ON_WRITE(245);
    ERA_ON_WRITE(246);
    ERA_ON_WRITE(247);
    ERA_ON_WRITE(248);
    ERA_ON_WRITE(249);
    ERA_ON_WRITE(250);
    ERA_ON_WRITE(251);
    ERA_ON_WRITE(252);
    ERA_ON_WRITE(253);
    ERA_ON_WRITE(254);
    ERA_ON_WRITE(255);
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
#if defined(ERA_255_PINS)
    ERA_ON_PIN_READ(100);
    ERA_ON_PIN_READ(101);
    ERA_ON_PIN_READ(102);
    ERA_ON_PIN_READ(103);
    ERA_ON_PIN_READ(104);
    ERA_ON_PIN_READ(105);
    ERA_ON_PIN_READ(106);
    ERA_ON_PIN_READ(107);
    ERA_ON_PIN_READ(108);
    ERA_ON_PIN_READ(109);
    ERA_ON_PIN_READ(110);
    ERA_ON_PIN_READ(111);
    ERA_ON_PIN_READ(112);
    ERA_ON_PIN_READ(113);
    ERA_ON_PIN_READ(114);
    ERA_ON_PIN_READ(115);
    ERA_ON_PIN_READ(116);
    ERA_ON_PIN_READ(117);
    ERA_ON_PIN_READ(118);
    ERA_ON_PIN_READ(119);
    ERA_ON_PIN_READ(120);
    ERA_ON_PIN_READ(121);
    ERA_ON_PIN_READ(122);
    ERA_ON_PIN_READ(123);
    ERA_ON_PIN_READ(124);
    ERA_ON_PIN_READ(125);
    ERA_ON_PIN_READ(126);
    ERA_ON_PIN_READ(127);
    ERA_ON_PIN_READ(128);
    ERA_ON_PIN_READ(129);
    ERA_ON_PIN_READ(130);
    ERA_ON_PIN_READ(131);
    ERA_ON_PIN_READ(132);
    ERA_ON_PIN_READ(133);
    ERA_ON_PIN_READ(134);
    ERA_ON_PIN_READ(135);
    ERA_ON_PIN_READ(136);
    ERA_ON_PIN_READ(137);
    ERA_ON_PIN_READ(138);
    ERA_ON_PIN_READ(139);
    ERA_ON_PIN_READ(140);
    ERA_ON_PIN_READ(141);
    ERA_ON_PIN_READ(142);
    ERA_ON_PIN_READ(143);
    ERA_ON_PIN_READ(144);
    ERA_ON_PIN_READ(145);
    ERA_ON_PIN_READ(146);
    ERA_ON_PIN_READ(147);
    ERA_ON_PIN_READ(148);
    ERA_ON_PIN_READ(149);
    ERA_ON_PIN_READ(150);
    ERA_ON_PIN_READ(151);
    ERA_ON_PIN_READ(152);
    ERA_ON_PIN_READ(153);
    ERA_ON_PIN_READ(154);
    ERA_ON_PIN_READ(155);
    ERA_ON_PIN_READ(156);
    ERA_ON_PIN_READ(157);
    ERA_ON_PIN_READ(158);
    ERA_ON_PIN_READ(159);
    ERA_ON_PIN_READ(160);
    ERA_ON_PIN_READ(161);
    ERA_ON_PIN_READ(162);
    ERA_ON_PIN_READ(163);
    ERA_ON_PIN_READ(164);
    ERA_ON_PIN_READ(165);
    ERA_ON_PIN_READ(166);
    ERA_ON_PIN_READ(167);
    ERA_ON_PIN_READ(168);
    ERA_ON_PIN_READ(169);
    ERA_ON_PIN_READ(170);
    ERA_ON_PIN_READ(171);
    ERA_ON_PIN_READ(172);
    ERA_ON_PIN_READ(173);
    ERA_ON_PIN_READ(174);
    ERA_ON_PIN_READ(175);
    ERA_ON_PIN_READ(176);
    ERA_ON_PIN_READ(177);
    ERA_ON_PIN_READ(178);
    ERA_ON_PIN_READ(179);
    ERA_ON_PIN_READ(180);
    ERA_ON_PIN_READ(181);
    ERA_ON_PIN_READ(182);
    ERA_ON_PIN_READ(183);
    ERA_ON_PIN_READ(184);
    ERA_ON_PIN_READ(185);
    ERA_ON_PIN_READ(186);
    ERA_ON_PIN_READ(187);
    ERA_ON_PIN_READ(188);
    ERA_ON_PIN_READ(189);
    ERA_ON_PIN_READ(190);
    ERA_ON_PIN_READ(191);
    ERA_ON_PIN_READ(192);
    ERA_ON_PIN_READ(193);
    ERA_ON_PIN_READ(194);
    ERA_ON_PIN_READ(195);
    ERA_ON_PIN_READ(196);
    ERA_ON_PIN_READ(197);
    ERA_ON_PIN_READ(198);
    ERA_ON_PIN_READ(199);
    ERA_ON_PIN_READ(200);
    ERA_ON_PIN_READ(201);
    ERA_ON_PIN_READ(202);
    ERA_ON_PIN_READ(203);
    ERA_ON_PIN_READ(204);
    ERA_ON_PIN_READ(205);
    ERA_ON_PIN_READ(206);
    ERA_ON_PIN_READ(207);
    ERA_ON_PIN_READ(208);
    ERA_ON_PIN_READ(209);
    ERA_ON_PIN_READ(210);
    ERA_ON_PIN_READ(211);
    ERA_ON_PIN_READ(212);
    ERA_ON_PIN_READ(213);
    ERA_ON_PIN_READ(214);
    ERA_ON_PIN_READ(215);
    ERA_ON_PIN_READ(216);
    ERA_ON_PIN_READ(217);
    ERA_ON_PIN_READ(218);
    ERA_ON_PIN_READ(219);
    ERA_ON_PIN_READ(220);
    ERA_ON_PIN_READ(221);
    ERA_ON_PIN_READ(222);
    ERA_ON_PIN_READ(223);
    ERA_ON_PIN_READ(224);
    ERA_ON_PIN_READ(225);
    ERA_ON_PIN_READ(226);
    ERA_ON_PIN_READ(227);
    ERA_ON_PIN_READ(228);
    ERA_ON_PIN_READ(229);
    ERA_ON_PIN_READ(230);
    ERA_ON_PIN_READ(231);
    ERA_ON_PIN_READ(232);
    ERA_ON_PIN_READ(233);
    ERA_ON_PIN_READ(234);
    ERA_ON_PIN_READ(235);
    ERA_ON_PIN_READ(236);
    ERA_ON_PIN_READ(237);
    ERA_ON_PIN_READ(238);
    ERA_ON_PIN_READ(239);
    ERA_ON_PIN_READ(240);
    ERA_ON_PIN_READ(241);
    ERA_ON_PIN_READ(242);
    ERA_ON_PIN_READ(243);
    ERA_ON_PIN_READ(244);
    ERA_ON_PIN_READ(245);
    ERA_ON_PIN_READ(246);
    ERA_ON_PIN_READ(247);
    ERA_ON_PIN_READ(248);
    ERA_ON_PIN_READ(249);
    ERA_ON_PIN_READ(250);
    ERA_ON_PIN_READ(251);
    ERA_ON_PIN_READ(252);
    ERA_ON_PIN_READ(253);
    ERA_ON_PIN_READ(254);
    ERA_ON_PIN_READ(255);
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
#if defined(ERA_255_PINS)
    ERA_ON_PIN_WRITE(100);
    ERA_ON_PIN_WRITE(101);
    ERA_ON_PIN_WRITE(102);
    ERA_ON_PIN_WRITE(103);
    ERA_ON_PIN_WRITE(104);
    ERA_ON_PIN_WRITE(105);
    ERA_ON_PIN_WRITE(106);
    ERA_ON_PIN_WRITE(107);
    ERA_ON_PIN_WRITE(108);
    ERA_ON_PIN_WRITE(109);
    ERA_ON_PIN_WRITE(110);
    ERA_ON_PIN_WRITE(111);
    ERA_ON_PIN_WRITE(112);
    ERA_ON_PIN_WRITE(113);
    ERA_ON_PIN_WRITE(114);
    ERA_ON_PIN_WRITE(115);
    ERA_ON_PIN_WRITE(116);
    ERA_ON_PIN_WRITE(117);
    ERA_ON_PIN_WRITE(118);
    ERA_ON_PIN_WRITE(119);
    ERA_ON_PIN_WRITE(120);
    ERA_ON_PIN_WRITE(121);
    ERA_ON_PIN_WRITE(122);
    ERA_ON_PIN_WRITE(123);
    ERA_ON_PIN_WRITE(124);
    ERA_ON_PIN_WRITE(125);
    ERA_ON_PIN_WRITE(126);
    ERA_ON_PIN_WRITE(127);
    ERA_ON_PIN_WRITE(128);
    ERA_ON_PIN_WRITE(129);
    ERA_ON_PIN_WRITE(130);
    ERA_ON_PIN_WRITE(131);
    ERA_ON_PIN_WRITE(132);
    ERA_ON_PIN_WRITE(133);
    ERA_ON_PIN_WRITE(134);
    ERA_ON_PIN_WRITE(135);
    ERA_ON_PIN_WRITE(136);
    ERA_ON_PIN_WRITE(137);
    ERA_ON_PIN_WRITE(138);
    ERA_ON_PIN_WRITE(139);
    ERA_ON_PIN_WRITE(140);
    ERA_ON_PIN_WRITE(141);
    ERA_ON_PIN_WRITE(142);
    ERA_ON_PIN_WRITE(143);
    ERA_ON_PIN_WRITE(144);
    ERA_ON_PIN_WRITE(145);
    ERA_ON_PIN_WRITE(146);
    ERA_ON_PIN_WRITE(147);
    ERA_ON_PIN_WRITE(148);
    ERA_ON_PIN_WRITE(149);
    ERA_ON_PIN_WRITE(150);
    ERA_ON_PIN_WRITE(151);
    ERA_ON_PIN_WRITE(152);
    ERA_ON_PIN_WRITE(153);
    ERA_ON_PIN_WRITE(154);
    ERA_ON_PIN_WRITE(155);
    ERA_ON_PIN_WRITE(156);
    ERA_ON_PIN_WRITE(157);
    ERA_ON_PIN_WRITE(158);
    ERA_ON_PIN_WRITE(159);
    ERA_ON_PIN_WRITE(160);
    ERA_ON_PIN_WRITE(161);
    ERA_ON_PIN_WRITE(162);
    ERA_ON_PIN_WRITE(163);
    ERA_ON_PIN_WRITE(164);
    ERA_ON_PIN_WRITE(165);
    ERA_ON_PIN_WRITE(166);
    ERA_ON_PIN_WRITE(167);
    ERA_ON_PIN_WRITE(168);
    ERA_ON_PIN_WRITE(169);
    ERA_ON_PIN_WRITE(170);
    ERA_ON_PIN_WRITE(171);
    ERA_ON_PIN_WRITE(172);
    ERA_ON_PIN_WRITE(173);
    ERA_ON_PIN_WRITE(174);
    ERA_ON_PIN_WRITE(175);
    ERA_ON_PIN_WRITE(176);
    ERA_ON_PIN_WRITE(177);
    ERA_ON_PIN_WRITE(178);
    ERA_ON_PIN_WRITE(179);
    ERA_ON_PIN_WRITE(180);
    ERA_ON_PIN_WRITE(181);
    ERA_ON_PIN_WRITE(182);
    ERA_ON_PIN_WRITE(183);
    ERA_ON_PIN_WRITE(184);
    ERA_ON_PIN_WRITE(185);
    ERA_ON_PIN_WRITE(186);
    ERA_ON_PIN_WRITE(187);
    ERA_ON_PIN_WRITE(188);
    ERA_ON_PIN_WRITE(189);
    ERA_ON_PIN_WRITE(190);
    ERA_ON_PIN_WRITE(191);
    ERA_ON_PIN_WRITE(192);
    ERA_ON_PIN_WRITE(193);
    ERA_ON_PIN_WRITE(194);
    ERA_ON_PIN_WRITE(195);
    ERA_ON_PIN_WRITE(196);
    ERA_ON_PIN_WRITE(197);
    ERA_ON_PIN_WRITE(198);
    ERA_ON_PIN_WRITE(199);
    ERA_ON_PIN_WRITE(200);
    ERA_ON_PIN_WRITE(201);
    ERA_ON_PIN_WRITE(202);
    ERA_ON_PIN_WRITE(203);
    ERA_ON_PIN_WRITE(204);
    ERA_ON_PIN_WRITE(205);
    ERA_ON_PIN_WRITE(206);
    ERA_ON_PIN_WRITE(207);
    ERA_ON_PIN_WRITE(208);
    ERA_ON_PIN_WRITE(209);
    ERA_ON_PIN_WRITE(210);
    ERA_ON_PIN_WRITE(211);
    ERA_ON_PIN_WRITE(212);
    ERA_ON_PIN_WRITE(213);
    ERA_ON_PIN_WRITE(214);
    ERA_ON_PIN_WRITE(215);
    ERA_ON_PIN_WRITE(216);
    ERA_ON_PIN_WRITE(217);
    ERA_ON_PIN_WRITE(218);
    ERA_ON_PIN_WRITE(219);
    ERA_ON_PIN_WRITE(220);
    ERA_ON_PIN_WRITE(221);
    ERA_ON_PIN_WRITE(222);
    ERA_ON_PIN_WRITE(223);
    ERA_ON_PIN_WRITE(224);
    ERA_ON_PIN_WRITE(225);
    ERA_ON_PIN_WRITE(226);
    ERA_ON_PIN_WRITE(227);
    ERA_ON_PIN_WRITE(228);
    ERA_ON_PIN_WRITE(229);
    ERA_ON_PIN_WRITE(230);
    ERA_ON_PIN_WRITE(231);
    ERA_ON_PIN_WRITE(232);
    ERA_ON_PIN_WRITE(233);
    ERA_ON_PIN_WRITE(234);
    ERA_ON_PIN_WRITE(235);
    ERA_ON_PIN_WRITE(236);
    ERA_ON_PIN_WRITE(237);
    ERA_ON_PIN_WRITE(238);
    ERA_ON_PIN_WRITE(239);
    ERA_ON_PIN_WRITE(240);
    ERA_ON_PIN_WRITE(241);
    ERA_ON_PIN_WRITE(242);
    ERA_ON_PIN_WRITE(243);
    ERA_ON_PIN_WRITE(244);
    ERA_ON_PIN_WRITE(245);
    ERA_ON_PIN_WRITE(246);
    ERA_ON_PIN_WRITE(247);
    ERA_ON_PIN_WRITE(248);
    ERA_ON_PIN_WRITE(249);
    ERA_ON_PIN_WRITE(250);
    ERA_ON_PIN_WRITE(251);
    ERA_ON_PIN_WRITE(252);
    ERA_ON_PIN_WRITE(253);
    ERA_ON_PIN_WRITE(254);
    ERA_ON_PIN_WRITE(255);
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
#if defined(ERA_255_PINS)
    ERaWidgetWrite100, ERaWidgetWrite101, ERaWidgetWrite102, ERaWidgetWrite103, ERaWidgetWrite104,
    ERaWidgetWrite105, ERaWidgetWrite106, ERaWidgetWrite107, ERaWidgetWrite108, ERaWidgetWrite109,
    ERaWidgetWrite110, ERaWidgetWrite111, ERaWidgetWrite112, ERaWidgetWrite113, ERaWidgetWrite114,
    ERaWidgetWrite115, ERaWidgetWrite116, ERaWidgetWrite117, ERaWidgetWrite118, ERaWidgetWrite119,
    ERaWidgetWrite120, ERaWidgetWrite121, ERaWidgetWrite122, ERaWidgetWrite123, ERaWidgetWrite124,
    ERaWidgetWrite125, ERaWidgetWrite126, ERaWidgetWrite127, ERaWidgetWrite128, ERaWidgetWrite129,
    ERaWidgetWrite130, ERaWidgetWrite131, ERaWidgetWrite132, ERaWidgetWrite133, ERaWidgetWrite134,
    ERaWidgetWrite135, ERaWidgetWrite136, ERaWidgetWrite137, ERaWidgetWrite138, ERaWidgetWrite139,
    ERaWidgetWrite140, ERaWidgetWrite141, ERaWidgetWrite142, ERaWidgetWrite143, ERaWidgetWrite144,
    ERaWidgetWrite145, ERaWidgetWrite146, ERaWidgetWrite147, ERaWidgetWrite148, ERaWidgetWrite149,
    ERaWidgetWrite150, ERaWidgetWrite151, ERaWidgetWrite152, ERaWidgetWrite153, ERaWidgetWrite154,
    ERaWidgetWrite155, ERaWidgetWrite156, ERaWidgetWrite157, ERaWidgetWrite158, ERaWidgetWrite159,
    ERaWidgetWrite160, ERaWidgetWrite161, ERaWidgetWrite162, ERaWidgetWrite163, ERaWidgetWrite164,
    ERaWidgetWrite165, ERaWidgetWrite166, ERaWidgetWrite167, ERaWidgetWrite168, ERaWidgetWrite169,
    ERaWidgetWrite170, ERaWidgetWrite171, ERaWidgetWrite172, ERaWidgetWrite173, ERaWidgetWrite174,
    ERaWidgetWrite175, ERaWidgetWrite176, ERaWidgetWrite177, ERaWidgetWrite178, ERaWidgetWrite179,
    ERaWidgetWrite180, ERaWidgetWrite181, ERaWidgetWrite182, ERaWidgetWrite183, ERaWidgetWrite184,
    ERaWidgetWrite185, ERaWidgetWrite186, ERaWidgetWrite187, ERaWidgetWrite188, ERaWidgetWrite189,
    ERaWidgetWrite190, ERaWidgetWrite191, ERaWidgetWrite192, ERaWidgetWrite193, ERaWidgetWrite194,
    ERaWidgetWrite195, ERaWidgetWrite196, ERaWidgetWrite197, ERaWidgetWrite198, ERaWidgetWrite199,
    ERaWidgetWrite200, ERaWidgetWrite201, ERaWidgetWrite202, ERaWidgetWrite203, ERaWidgetWrite204,
    ERaWidgetWrite205, ERaWidgetWrite206, ERaWidgetWrite207, ERaWidgetWrite208, ERaWidgetWrite209,
    ERaWidgetWrite210, ERaWidgetWrite211, ERaWidgetWrite212, ERaWidgetWrite213, ERaWidgetWrite214,
    ERaWidgetWrite215, ERaWidgetWrite216, ERaWidgetWrite217, ERaWidgetWrite218, ERaWidgetWrite219,
    ERaWidgetWrite220, ERaWidgetWrite221, ERaWidgetWrite222, ERaWidgetWrite223, ERaWidgetWrite224,
    ERaWidgetWrite225, ERaWidgetWrite226, ERaWidgetWrite227, ERaWidgetWrite228, ERaWidgetWrite229,
    ERaWidgetWrite230, ERaWidgetWrite231, ERaWidgetWrite232, ERaWidgetWrite233, ERaWidgetWrite234,
    ERaWidgetWrite235, ERaWidgetWrite236, ERaWidgetWrite237, ERaWidgetWrite238, ERaWidgetWrite239,
    ERaWidgetWrite240, ERaWidgetWrite241, ERaWidgetWrite242, ERaWidgetWrite243, ERaWidgetWrite244,
    ERaWidgetWrite245, ERaWidgetWrite246, ERaWidgetWrite247, ERaWidgetWrite248, ERaWidgetWrite249,
    ERaWidgetWrite250, ERaWidgetWrite251, ERaWidgetWrite252, ERaWidgetWrite253, ERaWidgetWrite254,
    ERaWidgetWrite255,
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
#if defined(ERA_255_PINS)
    ERaWidgetPinRead100, ERaWidgetPinRead101, ERaWidgetPinRead102, ERaWidgetPinRead103, ERaWidgetPinRead104,
    ERaWidgetPinRead105, ERaWidgetPinRead106, ERaWidgetPinRead107, ERaWidgetPinRead108, ERaWidgetPinRead109,
    ERaWidgetPinRead110, ERaWidgetPinRead111, ERaWidgetPinRead112, ERaWidgetPinRead113, ERaWidgetPinRead114,
    ERaWidgetPinRead115, ERaWidgetPinRead116, ERaWidgetPinRead117, ERaWidgetPinRead118, ERaWidgetPinRead119,
    ERaWidgetPinRead120, ERaWidgetPinRead121, ERaWidgetPinRead122, ERaWidgetPinRead123, ERaWidgetPinRead124,
    ERaWidgetPinRead125, ERaWidgetPinRead126, ERaWidgetPinRead127, ERaWidgetPinRead128, ERaWidgetPinRead129,
    ERaWidgetPinRead130, ERaWidgetPinRead131, ERaWidgetPinRead132, ERaWidgetPinRead133, ERaWidgetPinRead134,
    ERaWidgetPinRead135, ERaWidgetPinRead136, ERaWidgetPinRead137, ERaWidgetPinRead138, ERaWidgetPinRead139,
    ERaWidgetPinRead140, ERaWidgetPinRead141, ERaWidgetPinRead142, ERaWidgetPinRead143, ERaWidgetPinRead144,
    ERaWidgetPinRead145, ERaWidgetPinRead146, ERaWidgetPinRead147, ERaWidgetPinRead148, ERaWidgetPinRead149,
    ERaWidgetPinRead150, ERaWidgetPinRead151, ERaWidgetPinRead152, ERaWidgetPinRead153, ERaWidgetPinRead154,
    ERaWidgetPinRead155, ERaWidgetPinRead156, ERaWidgetPinRead157, ERaWidgetPinRead158, ERaWidgetPinRead159,
    ERaWidgetPinRead160, ERaWidgetPinRead161, ERaWidgetPinRead162, ERaWidgetPinRead163, ERaWidgetPinRead164,
    ERaWidgetPinRead165, ERaWidgetPinRead166, ERaWidgetPinRead167, ERaWidgetPinRead168, ERaWidgetPinRead169,
    ERaWidgetPinRead170, ERaWidgetPinRead171, ERaWidgetPinRead172, ERaWidgetPinRead173, ERaWidgetPinRead174,
    ERaWidgetPinRead175, ERaWidgetPinRead176, ERaWidgetPinRead177, ERaWidgetPinRead178, ERaWidgetPinRead179,
    ERaWidgetPinRead180, ERaWidgetPinRead181, ERaWidgetPinRead182, ERaWidgetPinRead183, ERaWidgetPinRead184,
    ERaWidgetPinRead185, ERaWidgetPinRead186, ERaWidgetPinRead187, ERaWidgetPinRead188, ERaWidgetPinRead189,
    ERaWidgetPinRead190, ERaWidgetPinRead191, ERaWidgetPinRead192, ERaWidgetPinRead193, ERaWidgetPinRead194,
    ERaWidgetPinRead195, ERaWidgetPinRead196, ERaWidgetPinRead197, ERaWidgetPinRead198, ERaWidgetPinRead199,
    ERaWidgetPinRead200, ERaWidgetPinRead201, ERaWidgetPinRead202, ERaWidgetPinRead203, ERaWidgetPinRead204,
    ERaWidgetPinRead205, ERaWidgetPinRead206, ERaWidgetPinRead207, ERaWidgetPinRead208, ERaWidgetPinRead209,
    ERaWidgetPinRead210, ERaWidgetPinRead211, ERaWidgetPinRead212, ERaWidgetPinRead213, ERaWidgetPinRead214,
    ERaWidgetPinRead215, ERaWidgetPinRead216, ERaWidgetPinRead217, ERaWidgetPinRead218, ERaWidgetPinRead219,
    ERaWidgetPinRead220, ERaWidgetPinRead221, ERaWidgetPinRead222, ERaWidgetPinRead223, ERaWidgetPinRead224,
    ERaWidgetPinRead225, ERaWidgetPinRead226, ERaWidgetPinRead227, ERaWidgetPinRead228, ERaWidgetPinRead229,
    ERaWidgetPinRead230, ERaWidgetPinRead231, ERaWidgetPinRead232, ERaWidgetPinRead233, ERaWidgetPinRead234,
    ERaWidgetPinRead235, ERaWidgetPinRead236, ERaWidgetPinRead237, ERaWidgetPinRead238, ERaWidgetPinRead239,
    ERaWidgetPinRead240, ERaWidgetPinRead241, ERaWidgetPinRead242, ERaWidgetPinRead243, ERaWidgetPinRead244,
    ERaWidgetPinRead245, ERaWidgetPinRead246, ERaWidgetPinRead247, ERaWidgetPinRead248, ERaWidgetPinRead249,
    ERaWidgetPinRead250, ERaWidgetPinRead251, ERaWidgetPinRead252, ERaWidgetPinRead253, ERaWidgetPinRead254,
    ERaWidgetPinRead255,
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
#if defined(ERA_255_PINS)
    ERaWidgetPinWrite100, ERaWidgetPinWrite101, ERaWidgetPinWrite102, ERaWidgetPinWrite103, ERaWidgetPinWrite104,
    ERaWidgetPinWrite105, ERaWidgetPinWrite106, ERaWidgetPinWrite107, ERaWidgetPinWrite108, ERaWidgetPinWrite109,
    ERaWidgetPinWrite110, ERaWidgetPinWrite111, ERaWidgetPinWrite112, ERaWidgetPinWrite113, ERaWidgetPinWrite114,
    ERaWidgetPinWrite115, ERaWidgetPinWrite116, ERaWidgetPinWrite117, ERaWidgetPinWrite118, ERaWidgetPinWrite119,
    ERaWidgetPinWrite120, ERaWidgetPinWrite121, ERaWidgetPinWrite122, ERaWidgetPinWrite123, ERaWidgetPinWrite124,
    ERaWidgetPinWrite125, ERaWidgetPinWrite126, ERaWidgetPinWrite127, ERaWidgetPinWrite128, ERaWidgetPinWrite129,
    ERaWidgetPinWrite130, ERaWidgetPinWrite131, ERaWidgetPinWrite132, ERaWidgetPinWrite133, ERaWidgetPinWrite134,
    ERaWidgetPinWrite135, ERaWidgetPinWrite136, ERaWidgetPinWrite137, ERaWidgetPinWrite138, ERaWidgetPinWrite139,
    ERaWidgetPinWrite140, ERaWidgetPinWrite141, ERaWidgetPinWrite142, ERaWidgetPinWrite143, ERaWidgetPinWrite144,
    ERaWidgetPinWrite145, ERaWidgetPinWrite146, ERaWidgetPinWrite147, ERaWidgetPinWrite148, ERaWidgetPinWrite149,
    ERaWidgetPinWrite150, ERaWidgetPinWrite151, ERaWidgetPinWrite152, ERaWidgetPinWrite153, ERaWidgetPinWrite154,
    ERaWidgetPinWrite155, ERaWidgetPinWrite156, ERaWidgetPinWrite157, ERaWidgetPinWrite158, ERaWidgetPinWrite159,
    ERaWidgetPinWrite160, ERaWidgetPinWrite161, ERaWidgetPinWrite162, ERaWidgetPinWrite163, ERaWidgetPinWrite164,
    ERaWidgetPinWrite165, ERaWidgetPinWrite166, ERaWidgetPinWrite167, ERaWidgetPinWrite168, ERaWidgetPinWrite169,
    ERaWidgetPinWrite170, ERaWidgetPinWrite171, ERaWidgetPinWrite172, ERaWidgetPinWrite173, ERaWidgetPinWrite174,
    ERaWidgetPinWrite175, ERaWidgetPinWrite176, ERaWidgetPinWrite177, ERaWidgetPinWrite178, ERaWidgetPinWrite179,
    ERaWidgetPinWrite180, ERaWidgetPinWrite181, ERaWidgetPinWrite182, ERaWidgetPinWrite183, ERaWidgetPinWrite184,
    ERaWidgetPinWrite185, ERaWidgetPinWrite186, ERaWidgetPinWrite187, ERaWidgetPinWrite188, ERaWidgetPinWrite189,
    ERaWidgetPinWrite190, ERaWidgetPinWrite191, ERaWidgetPinWrite192, ERaWidgetPinWrite193, ERaWidgetPinWrite194,
    ERaWidgetPinWrite195, ERaWidgetPinWrite196, ERaWidgetPinWrite197, ERaWidgetPinWrite198, ERaWidgetPinWrite199,
    ERaWidgetPinWrite200, ERaWidgetPinWrite201, ERaWidgetPinWrite202, ERaWidgetPinWrite203, ERaWidgetPinWrite204,
    ERaWidgetPinWrite205, ERaWidgetPinWrite206, ERaWidgetPinWrite207, ERaWidgetPinWrite208, ERaWidgetPinWrite209,
    ERaWidgetPinWrite210, ERaWidgetPinWrite211, ERaWidgetPinWrite212, ERaWidgetPinWrite213, ERaWidgetPinWrite214,
    ERaWidgetPinWrite215, ERaWidgetPinWrite216, ERaWidgetPinWrite217, ERaWidgetPinWrite218, ERaWidgetPinWrite219,
    ERaWidgetPinWrite220, ERaWidgetPinWrite221, ERaWidgetPinWrite222, ERaWidgetPinWrite223, ERaWidgetPinWrite224,
    ERaWidgetPinWrite225, ERaWidgetPinWrite226, ERaWidgetPinWrite227, ERaWidgetPinWrite228, ERaWidgetPinWrite229,
    ERaWidgetPinWrite230, ERaWidgetPinWrite231, ERaWidgetPinWrite232, ERaWidgetPinWrite233, ERaWidgetPinWrite234,
    ERaWidgetPinWrite235, ERaWidgetPinWrite236, ERaWidgetPinWrite237, ERaWidgetPinWrite238, ERaWidgetPinWrite239,
    ERaWidgetPinWrite240, ERaWidgetPinWrite241, ERaWidgetPinWrite242, ERaWidgetPinWrite243, ERaWidgetPinWrite244,
    ERaWidgetPinWrite245, ERaWidgetPinWrite246, ERaWidgetPinWrite247, ERaWidgetPinWrite248, ERaWidgetPinWrite249,
    ERaWidgetPinWrite250, ERaWidgetPinWrite251, ERaWidgetPinWrite252, ERaWidgetPinWrite253, ERaWidgetPinWrite254,
    ERaWidgetPinWrite255,
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
