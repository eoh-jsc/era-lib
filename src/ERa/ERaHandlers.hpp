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
#if defined(ERA_500_PINS)
    #define V256 256
    #define V257 257
    #define V258 258
    #define V259 259
    #define V260 260
    #define V261 261
    #define V262 262
    #define V263 263
    #define V264 264
    #define V265 265
    #define V266 266
    #define V267 267
    #define V268 268
    #define V269 269
    #define V270 270
    #define V271 271
    #define V272 272
    #define V273 273
    #define V274 274
    #define V275 275
    #define V276 276
    #define V277 277
    #define V278 278
    #define V279 279
    #define V280 280
    #define V281 281
    #define V282 282
    #define V283 283
    #define V284 284
    #define V285 285
    #define V286 286
    #define V287 287
    #define V288 288
    #define V289 289
    #define V290 290
    #define V291 291
    #define V292 292
    #define V293 293
    #define V294 294
    #define V295 295
    #define V296 296
    #define V297 297
    #define V298 298
    #define V299 299
    #define V300 300
    #define V301 301
    #define V302 302
    #define V303 303
    #define V304 304
    #define V305 305
    #define V306 306
    #define V307 307
    #define V308 308
    #define V309 309
    #define V310 310
    #define V311 311
    #define V312 312
    #define V313 313
    #define V314 314
    #define V315 315
    #define V316 316
    #define V317 317
    #define V318 318
    #define V319 319
    #define V320 320
    #define V321 321
    #define V322 322
    #define V323 323
    #define V324 324
    #define V325 325
    #define V326 326
    #define V327 327
    #define V328 328
    #define V329 329
    #define V330 330
    #define V331 331
    #define V332 332
    #define V333 333
    #define V334 334
    #define V335 335
    #define V336 336
    #define V337 337
    #define V338 338
    #define V339 339
    #define V340 340
    #define V341 341
    #define V342 342
    #define V343 343
    #define V344 344
    #define V345 345
    #define V346 346
    #define V347 347
    #define V348 348
    #define V349 349
    #define V350 350
    #define V351 351
    #define V352 352
    #define V353 353
    #define V354 354
    #define V355 355
    #define V356 356
    #define V357 357
    #define V358 358
    #define V359 359
    #define V360 360
    #define V361 361
    #define V362 362
    #define V363 363
    #define V364 364
    #define V365 365
    #define V366 366
    #define V367 367
    #define V368 368
    #define V369 369
    #define V370 370
    #define V371 371
    #define V372 372
    #define V373 373
    #define V374 374
    #define V375 375
    #define V376 376
    #define V377 377
    #define V378 378
    #define V379 379
    #define V380 380
    #define V381 381
    #define V382 382
    #define V383 383
    #define V384 384
    #define V385 385
    #define V386 386
    #define V387 387
    #define V388 388
    #define V389 389
    #define V390 390
    #define V391 391
    #define V392 392
    #define V393 393
    #define V394 394
    #define V395 395
    #define V396 396
    #define V397 397
    #define V398 398
    #define V399 399
    #define V400 400
    #define V401 401
    #define V402 402
    #define V403 403
    #define V404 404
    #define V405 405
    #define V406 406
    #define V407 407
    #define V408 408
    #define V409 409
    #define V410 410
    #define V411 411
    #define V412 412
    #define V413 413
    #define V414 414
    #define V415 415
    #define V416 416
    #define V417 417
    #define V418 418
    #define V419 419
    #define V420 420
    #define V421 421
    #define V422 422
    #define V423 423
    #define V424 424
    #define V425 425
    #define V426 426
    #define V427 427
    #define V428 428
    #define V429 429
    #define V430 430
    #define V431 431
    #define V432 432
    #define V433 433
    #define V434 434
    #define V435 435
    #define V436 436
    #define V437 437
    #define V438 438
    #define V439 439
    #define V440 440
    #define V441 441
    #define V442 442
    #define V443 443
    #define V444 444
    #define V445 445
    #define V446 446
    #define V447 447
    #define V448 448
    #define V449 449
    #define V450 450
    #define V451 451
    #define V452 452
    #define V453 453
    #define V454 454
    #define V455 455
    #define V456 456
    #define V457 457
    #define V458 458
    #define V459 459
    #define V460 460
    #define V461 461
    #define V462 462
    #define V463 463
    #define V464 464
    #define V465 465
    #define V466 466
    #define V467 467
    #define V468 468
    #define V469 469
    #define V470 470
    #define V471 471
    #define V472 472
    #define V473 473
    #define V474 474
    #define V475 475
    #define V476 476
    #define V477 477
    #define V478 478
    #define V479 479
    #define V480 480
    #define V481 481
    #define V482 482
    #define V483 483
    #define V484 484
    #define V485 485
    #define V486 486
    #define V487 487
    #define V488 488
    #define V489 489
    #define V490 490
    #define V491 491
    #define V492 492
    #define V493 493
    #define V494 494
    #define V495 495
    #define V496 496
    #define V497 497
    #define V498 498
    #define V499 499
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

#define ERA_WRITE_2(Pin)            void ERaWidgetWrite ## Pin (uint16_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param)
#define ERA_WRITE(Pin)              ERA_WRITE_2(Pin)

#define ERA_PIN_READ_2(Pin)         void ERaWidgetPinRead ## Pin (uint16_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw)
#define ERA_PIN_READ(Pin)           ERA_PIN_READ_2(Pin)

#define ERA_PIN_WRITE_2(Pin)        bool ERaWidgetPinWrite ## Pin (uint16_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw)
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
#if defined(ERA_500_PINS)
    ERA_WRITE(256);
    ERA_WRITE(257);
    ERA_WRITE(258);
    ERA_WRITE(259);
    ERA_WRITE(260);
    ERA_WRITE(261);
    ERA_WRITE(262);
    ERA_WRITE(263);
    ERA_WRITE(264);
    ERA_WRITE(265);
    ERA_WRITE(266);
    ERA_WRITE(267);
    ERA_WRITE(268);
    ERA_WRITE(269);
    ERA_WRITE(270);
    ERA_WRITE(271);
    ERA_WRITE(272);
    ERA_WRITE(273);
    ERA_WRITE(274);
    ERA_WRITE(275);
    ERA_WRITE(276);
    ERA_WRITE(277);
    ERA_WRITE(278);
    ERA_WRITE(279);
    ERA_WRITE(280);
    ERA_WRITE(281);
    ERA_WRITE(282);
    ERA_WRITE(283);
    ERA_WRITE(284);
    ERA_WRITE(285);
    ERA_WRITE(286);
    ERA_WRITE(287);
    ERA_WRITE(288);
    ERA_WRITE(289);
    ERA_WRITE(290);
    ERA_WRITE(291);
    ERA_WRITE(292);
    ERA_WRITE(293);
    ERA_WRITE(294);
    ERA_WRITE(295);
    ERA_WRITE(296);
    ERA_WRITE(297);
    ERA_WRITE(298);
    ERA_WRITE(299);
    ERA_WRITE(300);
    ERA_WRITE(301);
    ERA_WRITE(302);
    ERA_WRITE(303);
    ERA_WRITE(304);
    ERA_WRITE(305);
    ERA_WRITE(306);
    ERA_WRITE(307);
    ERA_WRITE(308);
    ERA_WRITE(309);
    ERA_WRITE(310);
    ERA_WRITE(311);
    ERA_WRITE(312);
    ERA_WRITE(313);
    ERA_WRITE(314);
    ERA_WRITE(315);
    ERA_WRITE(316);
    ERA_WRITE(317);
    ERA_WRITE(318);
    ERA_WRITE(319);
    ERA_WRITE(320);
    ERA_WRITE(321);
    ERA_WRITE(322);
    ERA_WRITE(323);
    ERA_WRITE(324);
    ERA_WRITE(325);
    ERA_WRITE(326);
    ERA_WRITE(327);
    ERA_WRITE(328);
    ERA_WRITE(329);
    ERA_WRITE(330);
    ERA_WRITE(331);
    ERA_WRITE(332);
    ERA_WRITE(333);
    ERA_WRITE(334);
    ERA_WRITE(335);
    ERA_WRITE(336);
    ERA_WRITE(337);
    ERA_WRITE(338);
    ERA_WRITE(339);
    ERA_WRITE(340);
    ERA_WRITE(341);
    ERA_WRITE(342);
    ERA_WRITE(343);
    ERA_WRITE(344);
    ERA_WRITE(345);
    ERA_WRITE(346);
    ERA_WRITE(347);
    ERA_WRITE(348);
    ERA_WRITE(349);
    ERA_WRITE(350);
    ERA_WRITE(351);
    ERA_WRITE(352);
    ERA_WRITE(353);
    ERA_WRITE(354);
    ERA_WRITE(355);
    ERA_WRITE(356);
    ERA_WRITE(357);
    ERA_WRITE(358);
    ERA_WRITE(359);
    ERA_WRITE(360);
    ERA_WRITE(361);
    ERA_WRITE(362);
    ERA_WRITE(363);
    ERA_WRITE(364);
    ERA_WRITE(365);
    ERA_WRITE(366);
    ERA_WRITE(367);
    ERA_WRITE(368);
    ERA_WRITE(369);
    ERA_WRITE(370);
    ERA_WRITE(371);
    ERA_WRITE(372);
    ERA_WRITE(373);
    ERA_WRITE(374);
    ERA_WRITE(375);
    ERA_WRITE(376);
    ERA_WRITE(377);
    ERA_WRITE(378);
    ERA_WRITE(379);
    ERA_WRITE(380);
    ERA_WRITE(381);
    ERA_WRITE(382);
    ERA_WRITE(383);
    ERA_WRITE(384);
    ERA_WRITE(385);
    ERA_WRITE(386);
    ERA_WRITE(387);
    ERA_WRITE(388);
    ERA_WRITE(389);
    ERA_WRITE(390);
    ERA_WRITE(391);
    ERA_WRITE(392);
    ERA_WRITE(393);
    ERA_WRITE(394);
    ERA_WRITE(395);
    ERA_WRITE(396);
    ERA_WRITE(397);
    ERA_WRITE(398);
    ERA_WRITE(399);
    ERA_WRITE(400);
    ERA_WRITE(401);
    ERA_WRITE(402);
    ERA_WRITE(403);
    ERA_WRITE(404);
    ERA_WRITE(405);
    ERA_WRITE(406);
    ERA_WRITE(407);
    ERA_WRITE(408);
    ERA_WRITE(409);
    ERA_WRITE(410);
    ERA_WRITE(411);
    ERA_WRITE(412);
    ERA_WRITE(413);
    ERA_WRITE(414);
    ERA_WRITE(415);
    ERA_WRITE(416);
    ERA_WRITE(417);
    ERA_WRITE(418);
    ERA_WRITE(419);
    ERA_WRITE(420);
    ERA_WRITE(421);
    ERA_WRITE(422);
    ERA_WRITE(423);
    ERA_WRITE(424);
    ERA_WRITE(425);
    ERA_WRITE(426);
    ERA_WRITE(427);
    ERA_WRITE(428);
    ERA_WRITE(429);
    ERA_WRITE(430);
    ERA_WRITE(431);
    ERA_WRITE(432);
    ERA_WRITE(433);
    ERA_WRITE(434);
    ERA_WRITE(435);
    ERA_WRITE(436);
    ERA_WRITE(437);
    ERA_WRITE(438);
    ERA_WRITE(439);
    ERA_WRITE(440);
    ERA_WRITE(441);
    ERA_WRITE(442);
    ERA_WRITE(443);
    ERA_WRITE(444);
    ERA_WRITE(445);
    ERA_WRITE(446);
    ERA_WRITE(447);
    ERA_WRITE(448);
    ERA_WRITE(449);
    ERA_WRITE(450);
    ERA_WRITE(451);
    ERA_WRITE(452);
    ERA_WRITE(453);
    ERA_WRITE(454);
    ERA_WRITE(455);
    ERA_WRITE(456);
    ERA_WRITE(457);
    ERA_WRITE(458);
    ERA_WRITE(459);
    ERA_WRITE(460);
    ERA_WRITE(461);
    ERA_WRITE(462);
    ERA_WRITE(463);
    ERA_WRITE(464);
    ERA_WRITE(465);
    ERA_WRITE(466);
    ERA_WRITE(467);
    ERA_WRITE(468);
    ERA_WRITE(469);
    ERA_WRITE(470);
    ERA_WRITE(471);
    ERA_WRITE(472);
    ERA_WRITE(473);
    ERA_WRITE(474);
    ERA_WRITE(475);
    ERA_WRITE(476);
    ERA_WRITE(477);
    ERA_WRITE(478);
    ERA_WRITE(479);
    ERA_WRITE(480);
    ERA_WRITE(481);
    ERA_WRITE(482);
    ERA_WRITE(483);
    ERA_WRITE(484);
    ERA_WRITE(485);
    ERA_WRITE(486);
    ERA_WRITE(487);
    ERA_WRITE(488);
    ERA_WRITE(489);
    ERA_WRITE(490);
    ERA_WRITE(491);
    ERA_WRITE(492);
    ERA_WRITE(493);
    ERA_WRITE(494);
    ERA_WRITE(495);
    ERA_WRITE(496);
    ERA_WRITE(497);
    ERA_WRITE(498);
    ERA_WRITE(499);
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
#if defined(ERA_500_PINS)
    ERA_PIN_READ(256);
    ERA_PIN_READ(257);
    ERA_PIN_READ(258);
    ERA_PIN_READ(259);
    ERA_PIN_READ(260);
    ERA_PIN_READ(261);
    ERA_PIN_READ(262);
    ERA_PIN_READ(263);
    ERA_PIN_READ(264);
    ERA_PIN_READ(265);
    ERA_PIN_READ(266);
    ERA_PIN_READ(267);
    ERA_PIN_READ(268);
    ERA_PIN_READ(269);
    ERA_PIN_READ(270);
    ERA_PIN_READ(271);
    ERA_PIN_READ(272);
    ERA_PIN_READ(273);
    ERA_PIN_READ(274);
    ERA_PIN_READ(275);
    ERA_PIN_READ(276);
    ERA_PIN_READ(277);
    ERA_PIN_READ(278);
    ERA_PIN_READ(279);
    ERA_PIN_READ(280);
    ERA_PIN_READ(281);
    ERA_PIN_READ(282);
    ERA_PIN_READ(283);
    ERA_PIN_READ(284);
    ERA_PIN_READ(285);
    ERA_PIN_READ(286);
    ERA_PIN_READ(287);
    ERA_PIN_READ(288);
    ERA_PIN_READ(289);
    ERA_PIN_READ(290);
    ERA_PIN_READ(291);
    ERA_PIN_READ(292);
    ERA_PIN_READ(293);
    ERA_PIN_READ(294);
    ERA_PIN_READ(295);
    ERA_PIN_READ(296);
    ERA_PIN_READ(297);
    ERA_PIN_READ(298);
    ERA_PIN_READ(299);
    ERA_PIN_READ(300);
    ERA_PIN_READ(301);
    ERA_PIN_READ(302);
    ERA_PIN_READ(303);
    ERA_PIN_READ(304);
    ERA_PIN_READ(305);
    ERA_PIN_READ(306);
    ERA_PIN_READ(307);
    ERA_PIN_READ(308);
    ERA_PIN_READ(309);
    ERA_PIN_READ(310);
    ERA_PIN_READ(311);
    ERA_PIN_READ(312);
    ERA_PIN_READ(313);
    ERA_PIN_READ(314);
    ERA_PIN_READ(315);
    ERA_PIN_READ(316);
    ERA_PIN_READ(317);
    ERA_PIN_READ(318);
    ERA_PIN_READ(319);
    ERA_PIN_READ(320);
    ERA_PIN_READ(321);
    ERA_PIN_READ(322);
    ERA_PIN_READ(323);
    ERA_PIN_READ(324);
    ERA_PIN_READ(325);
    ERA_PIN_READ(326);
    ERA_PIN_READ(327);
    ERA_PIN_READ(328);
    ERA_PIN_READ(329);
    ERA_PIN_READ(330);
    ERA_PIN_READ(331);
    ERA_PIN_READ(332);
    ERA_PIN_READ(333);
    ERA_PIN_READ(334);
    ERA_PIN_READ(335);
    ERA_PIN_READ(336);
    ERA_PIN_READ(337);
    ERA_PIN_READ(338);
    ERA_PIN_READ(339);
    ERA_PIN_READ(340);
    ERA_PIN_READ(341);
    ERA_PIN_READ(342);
    ERA_PIN_READ(343);
    ERA_PIN_READ(344);
    ERA_PIN_READ(345);
    ERA_PIN_READ(346);
    ERA_PIN_READ(347);
    ERA_PIN_READ(348);
    ERA_PIN_READ(349);
    ERA_PIN_READ(350);
    ERA_PIN_READ(351);
    ERA_PIN_READ(352);
    ERA_PIN_READ(353);
    ERA_PIN_READ(354);
    ERA_PIN_READ(355);
    ERA_PIN_READ(356);
    ERA_PIN_READ(357);
    ERA_PIN_READ(358);
    ERA_PIN_READ(359);
    ERA_PIN_READ(360);
    ERA_PIN_READ(361);
    ERA_PIN_READ(362);
    ERA_PIN_READ(363);
    ERA_PIN_READ(364);
    ERA_PIN_READ(365);
    ERA_PIN_READ(366);
    ERA_PIN_READ(367);
    ERA_PIN_READ(368);
    ERA_PIN_READ(369);
    ERA_PIN_READ(370);
    ERA_PIN_READ(371);
    ERA_PIN_READ(372);
    ERA_PIN_READ(373);
    ERA_PIN_READ(374);
    ERA_PIN_READ(375);
    ERA_PIN_READ(376);
    ERA_PIN_READ(377);
    ERA_PIN_READ(378);
    ERA_PIN_READ(379);
    ERA_PIN_READ(380);
    ERA_PIN_READ(381);
    ERA_PIN_READ(382);
    ERA_PIN_READ(383);
    ERA_PIN_READ(384);
    ERA_PIN_READ(385);
    ERA_PIN_READ(386);
    ERA_PIN_READ(387);
    ERA_PIN_READ(388);
    ERA_PIN_READ(389);
    ERA_PIN_READ(390);
    ERA_PIN_READ(391);
    ERA_PIN_READ(392);
    ERA_PIN_READ(393);
    ERA_PIN_READ(394);
    ERA_PIN_READ(395);
    ERA_PIN_READ(396);
    ERA_PIN_READ(397);
    ERA_PIN_READ(398);
    ERA_PIN_READ(399);
    ERA_PIN_READ(400);
    ERA_PIN_READ(401);
    ERA_PIN_READ(402);
    ERA_PIN_READ(403);
    ERA_PIN_READ(404);
    ERA_PIN_READ(405);
    ERA_PIN_READ(406);
    ERA_PIN_READ(407);
    ERA_PIN_READ(408);
    ERA_PIN_READ(409);
    ERA_PIN_READ(410);
    ERA_PIN_READ(411);
    ERA_PIN_READ(412);
    ERA_PIN_READ(413);
    ERA_PIN_READ(414);
    ERA_PIN_READ(415);
    ERA_PIN_READ(416);
    ERA_PIN_READ(417);
    ERA_PIN_READ(418);
    ERA_PIN_READ(419);
    ERA_PIN_READ(420);
    ERA_PIN_READ(421);
    ERA_PIN_READ(422);
    ERA_PIN_READ(423);
    ERA_PIN_READ(424);
    ERA_PIN_READ(425);
    ERA_PIN_READ(426);
    ERA_PIN_READ(427);
    ERA_PIN_READ(428);
    ERA_PIN_READ(429);
    ERA_PIN_READ(430);
    ERA_PIN_READ(431);
    ERA_PIN_READ(432);
    ERA_PIN_READ(433);
    ERA_PIN_READ(434);
    ERA_PIN_READ(435);
    ERA_PIN_READ(436);
    ERA_PIN_READ(437);
    ERA_PIN_READ(438);
    ERA_PIN_READ(439);
    ERA_PIN_READ(440);
    ERA_PIN_READ(441);
    ERA_PIN_READ(442);
    ERA_PIN_READ(443);
    ERA_PIN_READ(444);
    ERA_PIN_READ(445);
    ERA_PIN_READ(446);
    ERA_PIN_READ(447);
    ERA_PIN_READ(448);
    ERA_PIN_READ(449);
    ERA_PIN_READ(450);
    ERA_PIN_READ(451);
    ERA_PIN_READ(452);
    ERA_PIN_READ(453);
    ERA_PIN_READ(454);
    ERA_PIN_READ(455);
    ERA_PIN_READ(456);
    ERA_PIN_READ(457);
    ERA_PIN_READ(458);
    ERA_PIN_READ(459);
    ERA_PIN_READ(460);
    ERA_PIN_READ(461);
    ERA_PIN_READ(462);
    ERA_PIN_READ(463);
    ERA_PIN_READ(464);
    ERA_PIN_READ(465);
    ERA_PIN_READ(466);
    ERA_PIN_READ(467);
    ERA_PIN_READ(468);
    ERA_PIN_READ(469);
    ERA_PIN_READ(470);
    ERA_PIN_READ(471);
    ERA_PIN_READ(472);
    ERA_PIN_READ(473);
    ERA_PIN_READ(474);
    ERA_PIN_READ(475);
    ERA_PIN_READ(476);
    ERA_PIN_READ(477);
    ERA_PIN_READ(478);
    ERA_PIN_READ(479);
    ERA_PIN_READ(480);
    ERA_PIN_READ(481);
    ERA_PIN_READ(482);
    ERA_PIN_READ(483);
    ERA_PIN_READ(484);
    ERA_PIN_READ(485);
    ERA_PIN_READ(486);
    ERA_PIN_READ(487);
    ERA_PIN_READ(488);
    ERA_PIN_READ(489);
    ERA_PIN_READ(490);
    ERA_PIN_READ(491);
    ERA_PIN_READ(492);
    ERA_PIN_READ(493);
    ERA_PIN_READ(494);
    ERA_PIN_READ(495);
    ERA_PIN_READ(496);
    ERA_PIN_READ(497);
    ERA_PIN_READ(498);
    ERA_PIN_READ(499);
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
#if defined(ERA_500_PINS)
    ERA_PIN_WRITE(256);
    ERA_PIN_WRITE(257);
    ERA_PIN_WRITE(258);
    ERA_PIN_WRITE(259);
    ERA_PIN_WRITE(260);
    ERA_PIN_WRITE(261);
    ERA_PIN_WRITE(262);
    ERA_PIN_WRITE(263);
    ERA_PIN_WRITE(264);
    ERA_PIN_WRITE(265);
    ERA_PIN_WRITE(266);
    ERA_PIN_WRITE(267);
    ERA_PIN_WRITE(268);
    ERA_PIN_WRITE(269);
    ERA_PIN_WRITE(270);
    ERA_PIN_WRITE(271);
    ERA_PIN_WRITE(272);
    ERA_PIN_WRITE(273);
    ERA_PIN_WRITE(274);
    ERA_PIN_WRITE(275);
    ERA_PIN_WRITE(276);
    ERA_PIN_WRITE(277);
    ERA_PIN_WRITE(278);
    ERA_PIN_WRITE(279);
    ERA_PIN_WRITE(280);
    ERA_PIN_WRITE(281);
    ERA_PIN_WRITE(282);
    ERA_PIN_WRITE(283);
    ERA_PIN_WRITE(284);
    ERA_PIN_WRITE(285);
    ERA_PIN_WRITE(286);
    ERA_PIN_WRITE(287);
    ERA_PIN_WRITE(288);
    ERA_PIN_WRITE(289);
    ERA_PIN_WRITE(290);
    ERA_PIN_WRITE(291);
    ERA_PIN_WRITE(292);
    ERA_PIN_WRITE(293);
    ERA_PIN_WRITE(294);
    ERA_PIN_WRITE(295);
    ERA_PIN_WRITE(296);
    ERA_PIN_WRITE(297);
    ERA_PIN_WRITE(298);
    ERA_PIN_WRITE(299);
    ERA_PIN_WRITE(300);
    ERA_PIN_WRITE(301);
    ERA_PIN_WRITE(302);
    ERA_PIN_WRITE(303);
    ERA_PIN_WRITE(304);
    ERA_PIN_WRITE(305);
    ERA_PIN_WRITE(306);
    ERA_PIN_WRITE(307);
    ERA_PIN_WRITE(308);
    ERA_PIN_WRITE(309);
    ERA_PIN_WRITE(310);
    ERA_PIN_WRITE(311);
    ERA_PIN_WRITE(312);
    ERA_PIN_WRITE(313);
    ERA_PIN_WRITE(314);
    ERA_PIN_WRITE(315);
    ERA_PIN_WRITE(316);
    ERA_PIN_WRITE(317);
    ERA_PIN_WRITE(318);
    ERA_PIN_WRITE(319);
    ERA_PIN_WRITE(320);
    ERA_PIN_WRITE(321);
    ERA_PIN_WRITE(322);
    ERA_PIN_WRITE(323);
    ERA_PIN_WRITE(324);
    ERA_PIN_WRITE(325);
    ERA_PIN_WRITE(326);
    ERA_PIN_WRITE(327);
    ERA_PIN_WRITE(328);
    ERA_PIN_WRITE(329);
    ERA_PIN_WRITE(330);
    ERA_PIN_WRITE(331);
    ERA_PIN_WRITE(332);
    ERA_PIN_WRITE(333);
    ERA_PIN_WRITE(334);
    ERA_PIN_WRITE(335);
    ERA_PIN_WRITE(336);
    ERA_PIN_WRITE(337);
    ERA_PIN_WRITE(338);
    ERA_PIN_WRITE(339);
    ERA_PIN_WRITE(340);
    ERA_PIN_WRITE(341);
    ERA_PIN_WRITE(342);
    ERA_PIN_WRITE(343);
    ERA_PIN_WRITE(344);
    ERA_PIN_WRITE(345);
    ERA_PIN_WRITE(346);
    ERA_PIN_WRITE(347);
    ERA_PIN_WRITE(348);
    ERA_PIN_WRITE(349);
    ERA_PIN_WRITE(350);
    ERA_PIN_WRITE(351);
    ERA_PIN_WRITE(352);
    ERA_PIN_WRITE(353);
    ERA_PIN_WRITE(354);
    ERA_PIN_WRITE(355);
    ERA_PIN_WRITE(356);
    ERA_PIN_WRITE(357);
    ERA_PIN_WRITE(358);
    ERA_PIN_WRITE(359);
    ERA_PIN_WRITE(360);
    ERA_PIN_WRITE(361);
    ERA_PIN_WRITE(362);
    ERA_PIN_WRITE(363);
    ERA_PIN_WRITE(364);
    ERA_PIN_WRITE(365);
    ERA_PIN_WRITE(366);
    ERA_PIN_WRITE(367);
    ERA_PIN_WRITE(368);
    ERA_PIN_WRITE(369);
    ERA_PIN_WRITE(370);
    ERA_PIN_WRITE(371);
    ERA_PIN_WRITE(372);
    ERA_PIN_WRITE(373);
    ERA_PIN_WRITE(374);
    ERA_PIN_WRITE(375);
    ERA_PIN_WRITE(376);
    ERA_PIN_WRITE(377);
    ERA_PIN_WRITE(378);
    ERA_PIN_WRITE(379);
    ERA_PIN_WRITE(380);
    ERA_PIN_WRITE(381);
    ERA_PIN_WRITE(382);
    ERA_PIN_WRITE(383);
    ERA_PIN_WRITE(384);
    ERA_PIN_WRITE(385);
    ERA_PIN_WRITE(386);
    ERA_PIN_WRITE(387);
    ERA_PIN_WRITE(388);
    ERA_PIN_WRITE(389);
    ERA_PIN_WRITE(390);
    ERA_PIN_WRITE(391);
    ERA_PIN_WRITE(392);
    ERA_PIN_WRITE(393);
    ERA_PIN_WRITE(394);
    ERA_PIN_WRITE(395);
    ERA_PIN_WRITE(396);
    ERA_PIN_WRITE(397);
    ERA_PIN_WRITE(398);
    ERA_PIN_WRITE(399);
    ERA_PIN_WRITE(400);
    ERA_PIN_WRITE(401);
    ERA_PIN_WRITE(402);
    ERA_PIN_WRITE(403);
    ERA_PIN_WRITE(404);
    ERA_PIN_WRITE(405);
    ERA_PIN_WRITE(406);
    ERA_PIN_WRITE(407);
    ERA_PIN_WRITE(408);
    ERA_PIN_WRITE(409);
    ERA_PIN_WRITE(410);
    ERA_PIN_WRITE(411);
    ERA_PIN_WRITE(412);
    ERA_PIN_WRITE(413);
    ERA_PIN_WRITE(414);
    ERA_PIN_WRITE(415);
    ERA_PIN_WRITE(416);
    ERA_PIN_WRITE(417);
    ERA_PIN_WRITE(418);
    ERA_PIN_WRITE(419);
    ERA_PIN_WRITE(420);
    ERA_PIN_WRITE(421);
    ERA_PIN_WRITE(422);
    ERA_PIN_WRITE(423);
    ERA_PIN_WRITE(424);
    ERA_PIN_WRITE(425);
    ERA_PIN_WRITE(426);
    ERA_PIN_WRITE(427);
    ERA_PIN_WRITE(428);
    ERA_PIN_WRITE(429);
    ERA_PIN_WRITE(430);
    ERA_PIN_WRITE(431);
    ERA_PIN_WRITE(432);
    ERA_PIN_WRITE(433);
    ERA_PIN_WRITE(434);
    ERA_PIN_WRITE(435);
    ERA_PIN_WRITE(436);
    ERA_PIN_WRITE(437);
    ERA_PIN_WRITE(438);
    ERA_PIN_WRITE(439);
    ERA_PIN_WRITE(440);
    ERA_PIN_WRITE(441);
    ERA_PIN_WRITE(442);
    ERA_PIN_WRITE(443);
    ERA_PIN_WRITE(444);
    ERA_PIN_WRITE(445);
    ERA_PIN_WRITE(446);
    ERA_PIN_WRITE(447);
    ERA_PIN_WRITE(448);
    ERA_PIN_WRITE(449);
    ERA_PIN_WRITE(450);
    ERA_PIN_WRITE(451);
    ERA_PIN_WRITE(452);
    ERA_PIN_WRITE(453);
    ERA_PIN_WRITE(454);
    ERA_PIN_WRITE(455);
    ERA_PIN_WRITE(456);
    ERA_PIN_WRITE(457);
    ERA_PIN_WRITE(458);
    ERA_PIN_WRITE(459);
    ERA_PIN_WRITE(460);
    ERA_PIN_WRITE(461);
    ERA_PIN_WRITE(462);
    ERA_PIN_WRITE(463);
    ERA_PIN_WRITE(464);
    ERA_PIN_WRITE(465);
    ERA_PIN_WRITE(466);
    ERA_PIN_WRITE(467);
    ERA_PIN_WRITE(468);
    ERA_PIN_WRITE(469);
    ERA_PIN_WRITE(470);
    ERA_PIN_WRITE(471);
    ERA_PIN_WRITE(472);
    ERA_PIN_WRITE(473);
    ERA_PIN_WRITE(474);
    ERA_PIN_WRITE(475);
    ERA_PIN_WRITE(476);
    ERA_PIN_WRITE(477);
    ERA_PIN_WRITE(478);
    ERA_PIN_WRITE(479);
    ERA_PIN_WRITE(480);
    ERA_PIN_WRITE(481);
    ERA_PIN_WRITE(482);
    ERA_PIN_WRITE(483);
    ERA_PIN_WRITE(484);
    ERA_PIN_WRITE(485);
    ERA_PIN_WRITE(486);
    ERA_PIN_WRITE(487);
    ERA_PIN_WRITE(488);
    ERA_PIN_WRITE(489);
    ERA_PIN_WRITE(490);
    ERA_PIN_WRITE(491);
    ERA_PIN_WRITE(492);
    ERA_PIN_WRITE(493);
    ERA_PIN_WRITE(494);
    ERA_PIN_WRITE(495);
    ERA_PIN_WRITE(496);
    ERA_PIN_WRITE(497);
    ERA_PIN_WRITE(498);
    ERA_PIN_WRITE(499);
#endif

typedef void (*ERaWriteHandler_t)(uint16_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param);
typedef void (*ERaPinReadHandler_t)(uint16_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw);
typedef bool (*ERaPinWriteHandler_t)(uint16_t ERA_UNUSED &pin, const ERaParam ERA_UNUSED &param, const ERaParam ERA_UNUSED &raw);

ERaWriteHandler_t getERaWriteHandler(uint16_t pin);
ERaPinReadHandler_t getERaPinReadHandler(uint16_t pin);
ERaPinWriteHandler_t getERaPinWriteHandler(uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif /* INC_ERA_HANDLERS_HPP_ */
