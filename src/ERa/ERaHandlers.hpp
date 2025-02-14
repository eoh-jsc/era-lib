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
#if defined(ERA_255_PINS)
    #define V100 100
    #define V101 101
    #define V102 102
    #define V103 103
    #define V104 104
    #define V105 105
    #define V106 106
    #define V107 107
    #define V108 108
    #define V109 109
    #define V110 110
    #define V111 111
    #define V112 112
    #define V113 113
    #define V114 114
    #define V115 115
    #define V116 116
    #define V117 117
    #define V118 118
    #define V119 119
    #define V120 120
    #define V121 121
    #define V122 122
    #define V123 123
    #define V124 124
    #define V125 125
    #define V126 126
    #define V127 127
    #define V128 128
    #define V129 129
    #define V130 130
    #define V131 131
    #define V132 132
    #define V133 133
    #define V134 134
    #define V135 135
    #define V136 136
    #define V137 137
    #define V138 138
    #define V139 139
    #define V140 140
    #define V141 141
    #define V142 142
    #define V143 143
    #define V144 144
    #define V145 145
    #define V146 146
    #define V147 147
    #define V148 148
    #define V149 149
    #define V150 150
    #define V151 151
    #define V152 152
    #define V153 153
    #define V154 154
    #define V155 155
    #define V156 156
    #define V157 157
    #define V158 158
    #define V159 159
    #define V160 160
    #define V161 161
    #define V162 162
    #define V163 163
    #define V164 164
    #define V165 165
    #define V166 166
    #define V167 167
    #define V168 168
    #define V169 169
    #define V170 170
    #define V171 171
    #define V172 172
    #define V173 173
    #define V174 174
    #define V175 175
    #define V176 176
    #define V177 177
    #define V178 178
    #define V179 179
    #define V180 180
    #define V181 181
    #define V182 182
    #define V183 183
    #define V184 184
    #define V185 185
    #define V186 186
    #define V187 187
    #define V188 188
    #define V189 189
    #define V190 190
    #define V191 191
    #define V192 192
    #define V193 193
    #define V194 194
    #define V195 195
    #define V196 196
    #define V197 197
    #define V198 198
    #define V199 199
    #define V200 200
    #define V201 201
    #define V202 202
    #define V203 203
    #define V204 204
    #define V205 205
    #define V206 206
    #define V207 207
    #define V208 208
    #define V209 209
    #define V210 210
    #define V211 211
    #define V212 212
    #define V213 213
    #define V214 214
    #define V215 215
    #define V216 216
    #define V217 217
    #define V218 218
    #define V219 219
    #define V220 220
    #define V221 221
    #define V222 222
    #define V223 223
    #define V224 224
    #define V225 225
    #define V226 226
    #define V227 227
    #define V228 228
    #define V229 229
    #define V230 230
    #define V231 231
    #define V232 232
    #define V233 233
    #define V234 234
    #define V235 235
    #define V236 236
    #define V237 237
    #define V238 238
    #define V239 239
    #define V240 240
    #define V241 241
    #define V242 242
    #define V243 243
    #define V244 244
    #define V245 245
    #define V246 246
    #define V247 247
    #define V248 248
    #define V249 249
    #define V250 250
    #define V251 251
    #define V252 252
    #define V253 253
    #define V254 254
    #define V255 255
#endif

typedef enum
    : uint8_t {
    ERA_PIN_CONFIG = 0x00,
    ERA_MODBUS_CONFIG = 0x01,
    ERA_MODBUS_CONTROL = 0x02,
    ERA_BLUETOOTH_CONFIG = 0x03,
    ERA_ZIGBEE_CONFIG = 0x04,
    ERA_AUTO_CONFIG = 0x05
} ERaConfigTypeT;

#define ERA_CONNECTED()             void ERaOnConnected()
#define ERA_DISCONNECTED()          void ERaOnDisconnected()
#define ERA_WAITING()               void ERaOnWaiting()
#define ERA_APP_LOOP()              void ERaAppLoop()
#define ERA_OPTION_CONNECTED()      void ERaOptConnected(void ERA_UNUSED *arg)
#define ERA_MODBUS_BAUDRATE()       void ERaModbusBaudrate(uint32_t ERA_UNUSED &baudrate)
#define ERA_MODBUS_INFO()           void ERaModbusInfo(cJSON ERA_UNUSED *root)
#define ERA_SELF_INFO()             void ERaSelfInfo(cJSON ERA_UNUSED *root)
#define ERA_INFO()                  void ERaInfo(cJSON ERA_UNUSED *root)
#define ERA_WRITE_CONFIG()          void ERaWriteConfig(uint8_t ERA_UNUSED type)

#define ERA_WRITE_SMS()             bool ERaWidgetWriteSMS(const char* to, const char* message)
#define ERA_ON_SMS()                ERA_WRITE_SMS()

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
void ERaNoWriteConfig(uint8_t ERA_UNUSED type);
bool ERaNoWidgetWriteSMS(const char* to, const char* message);

ERA_CONNECTED();
ERA_DISCONNECTED();
ERA_WAITING();
ERA_APP_LOOP();
ERA_OPTION_CONNECTED();
ERA_MODBUS_BAUDRATE();
ERA_MODBUS_INFO();
ERA_SELF_INFO();
ERA_INFO();
ERA_WRITE_CONFIG();
ERA_WRITE_SMS();

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
#if defined(ERA_255_PINS)
    ERA_WRITE(100);
    ERA_WRITE(101);
    ERA_WRITE(102);
    ERA_WRITE(103);
    ERA_WRITE(104);
    ERA_WRITE(105);
    ERA_WRITE(106);
    ERA_WRITE(107);
    ERA_WRITE(108);
    ERA_WRITE(109);
    ERA_WRITE(110);
    ERA_WRITE(111);
    ERA_WRITE(112);
    ERA_WRITE(113);
    ERA_WRITE(114);
    ERA_WRITE(115);
    ERA_WRITE(116);
    ERA_WRITE(117);
    ERA_WRITE(118);
    ERA_WRITE(119);
    ERA_WRITE(120);
    ERA_WRITE(121);
    ERA_WRITE(122);
    ERA_WRITE(123);
    ERA_WRITE(124);
    ERA_WRITE(125);
    ERA_WRITE(126);
    ERA_WRITE(127);
    ERA_WRITE(128);
    ERA_WRITE(129);
    ERA_WRITE(130);
    ERA_WRITE(131);
    ERA_WRITE(132);
    ERA_WRITE(133);
    ERA_WRITE(134);
    ERA_WRITE(135);
    ERA_WRITE(136);
    ERA_WRITE(137);
    ERA_WRITE(138);
    ERA_WRITE(139);
    ERA_WRITE(140);
    ERA_WRITE(141);
    ERA_WRITE(142);
    ERA_WRITE(143);
    ERA_WRITE(144);
    ERA_WRITE(145);
    ERA_WRITE(146);
    ERA_WRITE(147);
    ERA_WRITE(148);
    ERA_WRITE(149);
    ERA_WRITE(150);
    ERA_WRITE(151);
    ERA_WRITE(152);
    ERA_WRITE(153);
    ERA_WRITE(154);
    ERA_WRITE(155);
    ERA_WRITE(156);
    ERA_WRITE(157);
    ERA_WRITE(158);
    ERA_WRITE(159);
    ERA_WRITE(160);
    ERA_WRITE(161);
    ERA_WRITE(162);
    ERA_WRITE(163);
    ERA_WRITE(164);
    ERA_WRITE(165);
    ERA_WRITE(166);
    ERA_WRITE(167);
    ERA_WRITE(168);
    ERA_WRITE(169);
    ERA_WRITE(170);
    ERA_WRITE(171);
    ERA_WRITE(172);
    ERA_WRITE(173);
    ERA_WRITE(174);
    ERA_WRITE(175);
    ERA_WRITE(176);
    ERA_WRITE(177);
    ERA_WRITE(178);
    ERA_WRITE(179);
    ERA_WRITE(180);
    ERA_WRITE(181);
    ERA_WRITE(182);
    ERA_WRITE(183);
    ERA_WRITE(184);
    ERA_WRITE(185);
    ERA_WRITE(186);
    ERA_WRITE(187);
    ERA_WRITE(188);
    ERA_WRITE(189);
    ERA_WRITE(190);
    ERA_WRITE(191);
    ERA_WRITE(192);
    ERA_WRITE(193);
    ERA_WRITE(194);
    ERA_WRITE(195);
    ERA_WRITE(196);
    ERA_WRITE(197);
    ERA_WRITE(198);
    ERA_WRITE(199);
    ERA_WRITE(200);
    ERA_WRITE(201);
    ERA_WRITE(202);
    ERA_WRITE(203);
    ERA_WRITE(204);
    ERA_WRITE(205);
    ERA_WRITE(206);
    ERA_WRITE(207);
    ERA_WRITE(208);
    ERA_WRITE(209);
    ERA_WRITE(210);
    ERA_WRITE(211);
    ERA_WRITE(212);
    ERA_WRITE(213);
    ERA_WRITE(214);
    ERA_WRITE(215);
    ERA_WRITE(216);
    ERA_WRITE(217);
    ERA_WRITE(218);
    ERA_WRITE(219);
    ERA_WRITE(220);
    ERA_WRITE(221);
    ERA_WRITE(222);
    ERA_WRITE(223);
    ERA_WRITE(224);
    ERA_WRITE(225);
    ERA_WRITE(226);
    ERA_WRITE(227);
    ERA_WRITE(228);
    ERA_WRITE(229);
    ERA_WRITE(230);
    ERA_WRITE(231);
    ERA_WRITE(232);
    ERA_WRITE(233);
    ERA_WRITE(234);
    ERA_WRITE(235);
    ERA_WRITE(236);
    ERA_WRITE(237);
    ERA_WRITE(238);
    ERA_WRITE(239);
    ERA_WRITE(240);
    ERA_WRITE(241);
    ERA_WRITE(242);
    ERA_WRITE(243);
    ERA_WRITE(244);
    ERA_WRITE(245);
    ERA_WRITE(246);
    ERA_WRITE(247);
    ERA_WRITE(248);
    ERA_WRITE(249);
    ERA_WRITE(250);
    ERA_WRITE(251);
    ERA_WRITE(252);
    ERA_WRITE(253);
    ERA_WRITE(254);
    ERA_WRITE(255);
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
#if defined(ERA_255_PINS)
    ERA_PIN_READ(100);
    ERA_PIN_READ(101);
    ERA_PIN_READ(102);
    ERA_PIN_READ(103);
    ERA_PIN_READ(104);
    ERA_PIN_READ(105);
    ERA_PIN_READ(106);
    ERA_PIN_READ(107);
    ERA_PIN_READ(108);
    ERA_PIN_READ(109);
    ERA_PIN_READ(110);
    ERA_PIN_READ(111);
    ERA_PIN_READ(112);
    ERA_PIN_READ(113);
    ERA_PIN_READ(114);
    ERA_PIN_READ(115);
    ERA_PIN_READ(116);
    ERA_PIN_READ(117);
    ERA_PIN_READ(118);
    ERA_PIN_READ(119);
    ERA_PIN_READ(120);
    ERA_PIN_READ(121);
    ERA_PIN_READ(122);
    ERA_PIN_READ(123);
    ERA_PIN_READ(124);
    ERA_PIN_READ(125);
    ERA_PIN_READ(126);
    ERA_PIN_READ(127);
    ERA_PIN_READ(128);
    ERA_PIN_READ(129);
    ERA_PIN_READ(130);
    ERA_PIN_READ(131);
    ERA_PIN_READ(132);
    ERA_PIN_READ(133);
    ERA_PIN_READ(134);
    ERA_PIN_READ(135);
    ERA_PIN_READ(136);
    ERA_PIN_READ(137);
    ERA_PIN_READ(138);
    ERA_PIN_READ(139);
    ERA_PIN_READ(140);
    ERA_PIN_READ(141);
    ERA_PIN_READ(142);
    ERA_PIN_READ(143);
    ERA_PIN_READ(144);
    ERA_PIN_READ(145);
    ERA_PIN_READ(146);
    ERA_PIN_READ(147);
    ERA_PIN_READ(148);
    ERA_PIN_READ(149);
    ERA_PIN_READ(150);
    ERA_PIN_READ(151);
    ERA_PIN_READ(152);
    ERA_PIN_READ(153);
    ERA_PIN_READ(154);
    ERA_PIN_READ(155);
    ERA_PIN_READ(156);
    ERA_PIN_READ(157);
    ERA_PIN_READ(158);
    ERA_PIN_READ(159);
    ERA_PIN_READ(160);
    ERA_PIN_READ(161);
    ERA_PIN_READ(162);
    ERA_PIN_READ(163);
    ERA_PIN_READ(164);
    ERA_PIN_READ(165);
    ERA_PIN_READ(166);
    ERA_PIN_READ(167);
    ERA_PIN_READ(168);
    ERA_PIN_READ(169);
    ERA_PIN_READ(170);
    ERA_PIN_READ(171);
    ERA_PIN_READ(172);
    ERA_PIN_READ(173);
    ERA_PIN_READ(174);
    ERA_PIN_READ(175);
    ERA_PIN_READ(176);
    ERA_PIN_READ(177);
    ERA_PIN_READ(178);
    ERA_PIN_READ(179);
    ERA_PIN_READ(180);
    ERA_PIN_READ(181);
    ERA_PIN_READ(182);
    ERA_PIN_READ(183);
    ERA_PIN_READ(184);
    ERA_PIN_READ(185);
    ERA_PIN_READ(186);
    ERA_PIN_READ(187);
    ERA_PIN_READ(188);
    ERA_PIN_READ(189);
    ERA_PIN_READ(190);
    ERA_PIN_READ(191);
    ERA_PIN_READ(192);
    ERA_PIN_READ(193);
    ERA_PIN_READ(194);
    ERA_PIN_READ(195);
    ERA_PIN_READ(196);
    ERA_PIN_READ(197);
    ERA_PIN_READ(198);
    ERA_PIN_READ(199);
    ERA_PIN_READ(200);
    ERA_PIN_READ(201);
    ERA_PIN_READ(202);
    ERA_PIN_READ(203);
    ERA_PIN_READ(204);
    ERA_PIN_READ(205);
    ERA_PIN_READ(206);
    ERA_PIN_READ(207);
    ERA_PIN_READ(208);
    ERA_PIN_READ(209);
    ERA_PIN_READ(210);
    ERA_PIN_READ(211);
    ERA_PIN_READ(212);
    ERA_PIN_READ(213);
    ERA_PIN_READ(214);
    ERA_PIN_READ(215);
    ERA_PIN_READ(216);
    ERA_PIN_READ(217);
    ERA_PIN_READ(218);
    ERA_PIN_READ(219);
    ERA_PIN_READ(220);
    ERA_PIN_READ(221);
    ERA_PIN_READ(222);
    ERA_PIN_READ(223);
    ERA_PIN_READ(224);
    ERA_PIN_READ(225);
    ERA_PIN_READ(226);
    ERA_PIN_READ(227);
    ERA_PIN_READ(228);
    ERA_PIN_READ(229);
    ERA_PIN_READ(230);
    ERA_PIN_READ(231);
    ERA_PIN_READ(232);
    ERA_PIN_READ(233);
    ERA_PIN_READ(234);
    ERA_PIN_READ(235);
    ERA_PIN_READ(236);
    ERA_PIN_READ(237);
    ERA_PIN_READ(238);
    ERA_PIN_READ(239);
    ERA_PIN_READ(240);
    ERA_PIN_READ(241);
    ERA_PIN_READ(242);
    ERA_PIN_READ(243);
    ERA_PIN_READ(244);
    ERA_PIN_READ(245);
    ERA_PIN_READ(246);
    ERA_PIN_READ(247);
    ERA_PIN_READ(248);
    ERA_PIN_READ(249);
    ERA_PIN_READ(250);
    ERA_PIN_READ(251);
    ERA_PIN_READ(252);
    ERA_PIN_READ(253);
    ERA_PIN_READ(254);
    ERA_PIN_READ(255);
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
#if defined(ERA_255_PINS)
    ERA_PIN_WRITE(100);
    ERA_PIN_WRITE(101);
    ERA_PIN_WRITE(102);
    ERA_PIN_WRITE(103);
    ERA_PIN_WRITE(104);
    ERA_PIN_WRITE(105);
    ERA_PIN_WRITE(106);
    ERA_PIN_WRITE(107);
    ERA_PIN_WRITE(108);
    ERA_PIN_WRITE(109);
    ERA_PIN_WRITE(110);
    ERA_PIN_WRITE(111);
    ERA_PIN_WRITE(112);
    ERA_PIN_WRITE(113);
    ERA_PIN_WRITE(114);
    ERA_PIN_WRITE(115);
    ERA_PIN_WRITE(116);
    ERA_PIN_WRITE(117);
    ERA_PIN_WRITE(118);
    ERA_PIN_WRITE(119);
    ERA_PIN_WRITE(120);
    ERA_PIN_WRITE(121);
    ERA_PIN_WRITE(122);
    ERA_PIN_WRITE(123);
    ERA_PIN_WRITE(124);
    ERA_PIN_WRITE(125);
    ERA_PIN_WRITE(126);
    ERA_PIN_WRITE(127);
    ERA_PIN_WRITE(128);
    ERA_PIN_WRITE(129);
    ERA_PIN_WRITE(130);
    ERA_PIN_WRITE(131);
    ERA_PIN_WRITE(132);
    ERA_PIN_WRITE(133);
    ERA_PIN_WRITE(134);
    ERA_PIN_WRITE(135);
    ERA_PIN_WRITE(136);
    ERA_PIN_WRITE(137);
    ERA_PIN_WRITE(138);
    ERA_PIN_WRITE(139);
    ERA_PIN_WRITE(140);
    ERA_PIN_WRITE(141);
    ERA_PIN_WRITE(142);
    ERA_PIN_WRITE(143);
    ERA_PIN_WRITE(144);
    ERA_PIN_WRITE(145);
    ERA_PIN_WRITE(146);
    ERA_PIN_WRITE(147);
    ERA_PIN_WRITE(148);
    ERA_PIN_WRITE(149);
    ERA_PIN_WRITE(150);
    ERA_PIN_WRITE(151);
    ERA_PIN_WRITE(152);
    ERA_PIN_WRITE(153);
    ERA_PIN_WRITE(154);
    ERA_PIN_WRITE(155);
    ERA_PIN_WRITE(156);
    ERA_PIN_WRITE(157);
    ERA_PIN_WRITE(158);
    ERA_PIN_WRITE(159);
    ERA_PIN_WRITE(160);
    ERA_PIN_WRITE(161);
    ERA_PIN_WRITE(162);
    ERA_PIN_WRITE(163);
    ERA_PIN_WRITE(164);
    ERA_PIN_WRITE(165);
    ERA_PIN_WRITE(166);
    ERA_PIN_WRITE(167);
    ERA_PIN_WRITE(168);
    ERA_PIN_WRITE(169);
    ERA_PIN_WRITE(170);
    ERA_PIN_WRITE(171);
    ERA_PIN_WRITE(172);
    ERA_PIN_WRITE(173);
    ERA_PIN_WRITE(174);
    ERA_PIN_WRITE(175);
    ERA_PIN_WRITE(176);
    ERA_PIN_WRITE(177);
    ERA_PIN_WRITE(178);
    ERA_PIN_WRITE(179);
    ERA_PIN_WRITE(180);
    ERA_PIN_WRITE(181);
    ERA_PIN_WRITE(182);
    ERA_PIN_WRITE(183);
    ERA_PIN_WRITE(184);
    ERA_PIN_WRITE(185);
    ERA_PIN_WRITE(186);
    ERA_PIN_WRITE(187);
    ERA_PIN_WRITE(188);
    ERA_PIN_WRITE(189);
    ERA_PIN_WRITE(190);
    ERA_PIN_WRITE(191);
    ERA_PIN_WRITE(192);
    ERA_PIN_WRITE(193);
    ERA_PIN_WRITE(194);
    ERA_PIN_WRITE(195);
    ERA_PIN_WRITE(196);
    ERA_PIN_WRITE(197);
    ERA_PIN_WRITE(198);
    ERA_PIN_WRITE(199);
    ERA_PIN_WRITE(200);
    ERA_PIN_WRITE(201);
    ERA_PIN_WRITE(202);
    ERA_PIN_WRITE(203);
    ERA_PIN_WRITE(204);
    ERA_PIN_WRITE(205);
    ERA_PIN_WRITE(206);
    ERA_PIN_WRITE(207);
    ERA_PIN_WRITE(208);
    ERA_PIN_WRITE(209);
    ERA_PIN_WRITE(210);
    ERA_PIN_WRITE(211);
    ERA_PIN_WRITE(212);
    ERA_PIN_WRITE(213);
    ERA_PIN_WRITE(214);
    ERA_PIN_WRITE(215);
    ERA_PIN_WRITE(216);
    ERA_PIN_WRITE(217);
    ERA_PIN_WRITE(218);
    ERA_PIN_WRITE(219);
    ERA_PIN_WRITE(220);
    ERA_PIN_WRITE(221);
    ERA_PIN_WRITE(222);
    ERA_PIN_WRITE(223);
    ERA_PIN_WRITE(224);
    ERA_PIN_WRITE(225);
    ERA_PIN_WRITE(226);
    ERA_PIN_WRITE(227);
    ERA_PIN_WRITE(228);
    ERA_PIN_WRITE(229);
    ERA_PIN_WRITE(230);
    ERA_PIN_WRITE(231);
    ERA_PIN_WRITE(232);
    ERA_PIN_WRITE(233);
    ERA_PIN_WRITE(234);
    ERA_PIN_WRITE(235);
    ERA_PIN_WRITE(236);
    ERA_PIN_WRITE(237);
    ERA_PIN_WRITE(238);
    ERA_PIN_WRITE(239);
    ERA_PIN_WRITE(240);
    ERA_PIN_WRITE(241);
    ERA_PIN_WRITE(242);
    ERA_PIN_WRITE(243);
    ERA_PIN_WRITE(244);
    ERA_PIN_WRITE(245);
    ERA_PIN_WRITE(246);
    ERA_PIN_WRITE(247);
    ERA_PIN_WRITE(248);
    ERA_PIN_WRITE(249);
    ERA_PIN_WRITE(250);
    ERA_PIN_WRITE(251);
    ERA_PIN_WRITE(252);
    ERA_PIN_WRITE(253);
    ERA_PIN_WRITE(254);
    ERA_PIN_WRITE(255);
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
