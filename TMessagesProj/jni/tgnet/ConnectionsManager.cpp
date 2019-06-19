/*
 * This is the source code of tgnet library v. 1.1
 * It is licensed under GNU GPL v. 2 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright Nikolai Kudashov, 2015-2018.
 */

#include <stdlib.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <chrono>
#include <algorithm>
#include <fcntl.h>
#include <memory.h>
#include <openssl/rand.h>
#include <zlib.h>
#include <string>
#include <inttypes.h>
#include "ConnectionsManager.h"
#include "FileLog.h"
#include "EventObject.h"
#include "MTProtoScheme.h"
#include "ApiScheme.h"
#include "NativeByteBuffer.h"
#include "Connection.h"
#include "Datacenter.h"
#include "Request.h"
#include "BuffersStorage.h"
#include "ByteArray.h"
#include "Config.h"
#include "ProxyCheckInfo.h"

#ifdef ANDROID
#include <jni.h>
JavaVM *javaVm = nullptr;
JNIEnv *jniEnv[MAX_ACCOUNT_COUNT];
jclass jclass_ByteBuffer = nullptr;
jmethodID jclass_ByteBuffer_allocateDirect = 0;
#endif

static bool done = false;

ConnectionsManager::ConnectionsManager(int32_t instance) {
    instanceNum = instance;
    if ((epolFd = epoll_create(128)) == -1) {
        if (LOGS_ENABLED) DEBUG_E("unable to create epoll instance");
        exit(1);
    }
    int flags;
    if ((flags = fcntl(epolFd, F_GETFD, NULL)) < 0) {
        if (LOGS_ENABLED) DEBUG_W("fcntl(%d, F_GETFD)", epolFd);
    }
    if (!(flags & FD_CLOEXEC)) {
        if (fcntl(epolFd, F_SETFD, flags | FD_CLOEXEC) == -1) {
            if (LOGS_ENABLED) DEBUG_W("fcntl(%d, F_SETFD)", epolFd);
        }
    }

    if ((epollEvents = new epoll_event[128]) == nullptr) {
        if (LOGS_ENABLED) DEBUG_E("unable to allocate epoll events");
        exit(1);
    }

    eventFd = eventfd(0, EFD_NONBLOCK);
    if (eventFd != -1) {
        struct epoll_event event = {0};
        event.data.ptr = new EventObject(&eventFd, EventObjectTypeEvent);
        event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(epolFd, EPOLL_CTL_ADD, eventFd, &event) == -1) {
            eventFd = -1;
            FileLog::e("unable to add eventfd");
        }
    }

    if (eventFd == -1) {
        pipeFd = new int[2];
        if (pipe(pipeFd) != 0) {
            if (LOGS_ENABLED) DEBUG_E("unable to create pipe");
            exit(1);
        }
        flags = fcntl(pipeFd[0], F_GETFL);
        if (flags == -1) {
            if (LOGS_ENABLED) DEBUG_E("fcntl get pipefds[0] failed");
            exit(1);
        }
        if (fcntl(pipeFd[0], F_SETFL, flags | O_NONBLOCK) == -1) {
            if (LOGS_ENABLED) DEBUG_E("fcntl set pipefds[0] failed");
            exit(1);
        }

        flags = fcntl(pipeFd[1], F_GETFL);
        if (flags == -1) {
            if (LOGS_ENABLED) DEBUG_E("fcntl get pipefds[1] failed");
            exit(1);
        }
        if (fcntl(pipeFd[1], F_SETFL, flags | O_NONBLOCK) == -1) {
            if (LOGS_ENABLED) DEBUG_E("fcntl set pipefds[1] failed");
            exit(1);
        }

        EventObject *eventObject = new EventObject(pipeFd, EventObjectTypePipe);

        epoll_event eventMask = {};
        eventMask.events = EPOLLIN;
        eventMask.data.ptr = eventObject;
        if (epoll_ctl(epolFd, EPOLL_CTL_ADD, pipeFd[0], &eventMask) != 0) {
            if (LOGS_ENABLED) DEBUG_E("can't add pipe to epoll");
            exit(1);
        }
    }

    sizeCalculator = new NativeByteBuffer(true);
    networkBuffer = new NativeByteBuffer((uint32_t) READ_BUFFER_SIZE);
    if (networkBuffer == nullptr) {
        if (LOGS_ENABLED) DEBUG_E("unable to allocate read buffer");
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);
}

ConnectionsManager::~ConnectionsManager() {
    if (epolFd != 0) {
        close(epolFd);
        epolFd = 0;
    }
    pthread_mutex_destroy(&mutex);
}

ConnectionsManager& ConnectionsManager::getInstance(int32_t instanceNum) {
  /*  switch (instanceNum) {
        case 0:
            static ConnectionsManager instance0(0);
            return instance0;
        case 1:
            static ConnectionsManager instance1(1);
            return instance1;
        case 2:
        default:
            static ConnectionsManager instance2(2);
            return instance2;
    } */

  // prafulla_edit: added instances

    switch(instanceNum)
    {
        case 0:
            static ConnectionsManager instance0(0);
            return instance0;
        case 1:
            static ConnectionsManager instance1(1);
            return instance1;
        case 2:
            static ConnectionsManager instance2(2);
            return instance2;
        case 3:
            static ConnectionsManager instance3(3);
            return instance3;
        case 4:
            static ConnectionsManager instance4(4);
            return instance4;
        case 5:
            static ConnectionsManager instance5(5);
            return instance5;
        case 6:
            static ConnectionsManager instance6(6);
            return instance6;
        case 7:
            static ConnectionsManager instance7(7);
            return instance7;
        case 8:
            static ConnectionsManager instance8(8);
            return instance8;
        case 9:
            static ConnectionsManager instance9(9);
            return instance9;
        case 10:
            static ConnectionsManager instance10(10);
            return instance10;
        case 11:
            static ConnectionsManager instance11(11);
            return instance11;
        case 12:
            static ConnectionsManager instance12(12);
            return instance12;
        case 13:
            static ConnectionsManager instance13(13);
            return instance13;
        case 14:
            static ConnectionsManager instance14(14);
            return instance14;
        case 15:
            static ConnectionsManager instance15(15);
            return instance15;
        case 16:
            static ConnectionsManager instance16(16);
            return instance16;
        case 17:
            static ConnectionsManager instance17(17);
            return instance17;
        case 18:
            static ConnectionsManager instance18(18);
            return instance18;
        case 19:
            static ConnectionsManager instance19(19);
            return instance19;
        case 20:
            static ConnectionsManager instance20(20);
            return instance20;
        case 21:
            static ConnectionsManager instance21(21);
            return instance21;
        case 22:
            static ConnectionsManager instance22(22);
            return instance22;
        case 23:
            static ConnectionsManager instance23(23);
            return instance23;
        case 24:
            static ConnectionsManager instance24(24);
            return instance24;
        case 25:
            static ConnectionsManager instance25(25);
            return instance25;
        case 26:
            static ConnectionsManager instance26(26);
            return instance26;
        case 27:
            static ConnectionsManager instance27(27);
            return instance27;
        case 28:
            static ConnectionsManager instance28(28);
            return instance28;
        case 29:
            static ConnectionsManager instance29(29);
            return instance29;
        case 30:
            static ConnectionsManager instance30(30);
            return instance30;
        case 31:
            static ConnectionsManager instance31(31);
            return instance31;
        case 32:
            static ConnectionsManager instance32(32);
            return instance32;
        case 33:
            static ConnectionsManager instance33(33);
            return instance33;
        case 34:
            static ConnectionsManager instance34(34);
            return instance34;
        case 35:
            static ConnectionsManager instance35(35);
            return instance35;
        case 36:
            static ConnectionsManager instance36(36);
            return instance36;
        case 37:
            static ConnectionsManager instance37(37);
            return instance37;
        case 38:
            static ConnectionsManager instance38(38);
            return instance38;
        case 39:
            static ConnectionsManager instance39(39);
            return instance39;
        case 40:
            static ConnectionsManager instance40(40);
            return instance40;
        case 41:
            static ConnectionsManager instance41(41);
            return instance41;
        case 42:
            static ConnectionsManager instance42(42);
            return instance42;
        case 43:
            static ConnectionsManager instance43(43);
            return instance43;
        case 44:
            static ConnectionsManager instance44(44);
            return instance44;
        case 45:
            static ConnectionsManager instance45(45);
            return instance45;
        case 46:
            static ConnectionsManager instance46(46);
            return instance46;
        case 47:
            static ConnectionsManager instance47(47);
            return instance47;
        case 48:
            static ConnectionsManager instance48(48);
            return instance48;
        case 49:
            static ConnectionsManager instance49(49);
            return instance49;
        case 50:
            static ConnectionsManager instance50(50);
            return instance50;
        case 51:
            static ConnectionsManager instance51(51);
            return instance51;
        case 52:
            static ConnectionsManager instance52(52);
            return instance52;
        case 53:
            static ConnectionsManager instance53(53);
            return instance53;
        case 54:
            static ConnectionsManager instance54(54);
            return instance54;
        case 55:
            static ConnectionsManager instance55(55);
            return instance55;
        case 56:
            static ConnectionsManager instance56(56);
            return instance56;
        case 57:
            static ConnectionsManager instance57(57);
            return instance57;
        case 58:
            static ConnectionsManager instance58(58);
            return instance58;
        case 59:
            static ConnectionsManager instance59(59);
            return instance59;
        case 60:
            static ConnectionsManager instance60(60);
            return instance60;
        case 61:
            static ConnectionsManager instance61(61);
            return instance61;
        case 62:
            static ConnectionsManager instance62(62);
            return instance62;
        case 63:
            static ConnectionsManager instance63(63);
            return instance63;
        case 64:
            static ConnectionsManager instance64(64);
            return instance64;
        case 65:
            static ConnectionsManager instance65(65);
            return instance65;
        case 66:
            static ConnectionsManager instance66(66);
            return instance66;
        case 67:
            static ConnectionsManager instance67(67);
            return instance67;
        case 68:
            static ConnectionsManager instance68(68);
            return instance68;
        case 69:
            static ConnectionsManager instance69(69);
            return instance69;
        case 70:
            static ConnectionsManager instance70(70);
            return instance70;
        case 71:
            static ConnectionsManager instance71(71);
            return instance71;
        case 72:
            static ConnectionsManager instance72(72);
            return instance72;
        case 73:
            static ConnectionsManager instance73(73);
            return instance73;
        case 74:
            static ConnectionsManager instance74(74);
            return instance74;
        case 75:
            static ConnectionsManager instance75(75);
            return instance75;
        case 76:
            static ConnectionsManager instance76(76);
            return instance76;
        case 77:
            static ConnectionsManager instance77(77);
            return instance77;
        case 78:
            static ConnectionsManager instance78(78);
            return instance78;
        case 79:
            static ConnectionsManager instance79(79);
            return instance79;
        case 80:
            static ConnectionsManager instance80(80);
            return instance80;
        case 81:
            static ConnectionsManager instance81(81);
            return instance81;
        case 82:
            static ConnectionsManager instance82(82);
            return instance82;
        case 83:
            static ConnectionsManager instance83(83);
            return instance83;
        case 84:
            static ConnectionsManager instance84(84);
            return instance84;
        case 85:
            static ConnectionsManager instance85(85);
            return instance85;
        case 86:
            static ConnectionsManager instance86(86);
            return instance86;
        case 87:
            static ConnectionsManager instance87(87);
            return instance87;
        case 88:
            static ConnectionsManager instance88(88);
            return instance88;
        case 89:
            static ConnectionsManager instance89(89);
            return instance89;
        case 90:
            static ConnectionsManager instance90(90);
            return instance90;
        case 91:
            static ConnectionsManager instance91(91);
            return instance91;
        case 92:
            static ConnectionsManager instance92(92);
            return instance92;
        case 93:
            static ConnectionsManager instance93(93);
            return instance93;
        case 94:
            static ConnectionsManager instance94(94);
            return instance94;
        case 95:
            static ConnectionsManager instance95(95);
            return instance95;
        case 96:
            static ConnectionsManager instance96(96);
            return instance96;
        case 97:
            static ConnectionsManager instance97(97);
            return instance97;
        case 98:
            static ConnectionsManager instance98(98);
            return instance98;
        case 99:
            static ConnectionsManager instance99(99);
            return instance99;
        case 100:
            static ConnectionsManager instance100(100);
            return instance100;
        case 101:
            static ConnectionsManager instance101(101);
            return instance101;
        case 102:
            static ConnectionsManager instance102(102);
            return instance102;
        case 103:
            static ConnectionsManager instance103(103);
            return instance103;
        case 104:
            static ConnectionsManager instance104(104);
            return instance104;
        case 105:
            static ConnectionsManager instance105(105);
            return instance105;
        case 106:
            static ConnectionsManager instance106(106);
            return instance106;
        case 107:
            static ConnectionsManager instance107(107);
            return instance107;
        case 108:
            static ConnectionsManager instance108(108);
            return instance108;
        case 109:
            static ConnectionsManager instance109(109);
            return instance109;
        case 110:
            static ConnectionsManager instance110(110);
            return instance110;
        case 111:
            static ConnectionsManager instance111(111);
            return instance111;
        case 112:
            static ConnectionsManager instance112(112);
            return instance112;
        case 113:
            static ConnectionsManager instance113(113);
            return instance113;
        case 114:
            static ConnectionsManager instance114(114);
            return instance114;
        case 115:
            static ConnectionsManager instance115(115);
            return instance115;
        case 116:
            static ConnectionsManager instance116(116);
            return instance116;
        case 117:
            static ConnectionsManager instance117(117);
            return instance117;
        case 118:
            static ConnectionsManager instance118(118);
            return instance118;
        case 119:
            static ConnectionsManager instance119(119);
            return instance119;
        case 120:
            static ConnectionsManager instance120(120);
            return instance120;
        case 121:
            static ConnectionsManager instance121(121);
            return instance121;
        case 122:
            static ConnectionsManager instance122(122);
            return instance122;
        case 123:
            static ConnectionsManager instance123(123);
            return instance123;
        case 124:
            static ConnectionsManager instance124(124);
            return instance124;
        case 125:
            static ConnectionsManager instance125(125);
            return instance125;
        case 126:
            static ConnectionsManager instance126(126);
            return instance126;
        case 127:
            static ConnectionsManager instance127(127);
            return instance127;
        case 128:
            static ConnectionsManager instance128(128);
            return instance128;
        case 129:
            static ConnectionsManager instance129(129);
            return instance129;
        case 130:
            static ConnectionsManager instance130(130);
            return instance130;
        case 131:
            static ConnectionsManager instance131(131);
            return instance131;
        case 132:
            static ConnectionsManager instance132(132);
            return instance132;
        case 133:
            static ConnectionsManager instance133(133);
            return instance133;
        case 134:
            static ConnectionsManager instance134(134);
            return instance134;
        case 135:
            static ConnectionsManager instance135(135);
            return instance135;
        case 136:
            static ConnectionsManager instance136(136);
            return instance136;
        case 137:
            static ConnectionsManager instance137(137);
            return instance137;
        case 138:
            static ConnectionsManager instance138(138);
            return instance138;
        case 139:
            static ConnectionsManager instance139(139);
            return instance139;
        case 140:
            static ConnectionsManager instance140(140);
            return instance140;
        case 141:
            static ConnectionsManager instance141(141);
            return instance141;
        case 142:
            static ConnectionsManager instance142(142);
            return instance142;
        case 143:
            static ConnectionsManager instance143(143);
            return instance143;
        case 144:
            static ConnectionsManager instance144(144);
            return instance144;
        case 145:
            static ConnectionsManager instance145(145);
            return instance145;
        case 146:
            static ConnectionsManager instance146(146);
            return instance146;
        case 147:
            static ConnectionsManager instance147(147);
            return instance147;
        case 148:
            static ConnectionsManager instance148(148);
            return instance148;
        case 149:
            static ConnectionsManager instance149(149);
            return instance149;
        case 150:
            static ConnectionsManager instance150(150);
            return instance150;
        case 151:
            static ConnectionsManager instance151(151);
            return instance151;
        case 152:
            static ConnectionsManager instance152(152);
            return instance152;
        case 153:
            static ConnectionsManager instance153(153);
            return instance153;
        case 154:
            static ConnectionsManager instance154(154);
            return instance154;
        case 155:
            static ConnectionsManager instance155(155);
            return instance155;
        case 156:
            static ConnectionsManager instance156(156);
            return instance156;
        case 157:
            static ConnectionsManager instance157(157);
            return instance157;
        case 158:
            static ConnectionsManager instance158(158);
            return instance158;
        case 159:
            static ConnectionsManager instance159(159);
            return instance159;
        case 160:
            static ConnectionsManager instance160(160);
            return instance160;
        case 161:
            static ConnectionsManager instance161(161);
            return instance161;
        case 162:
            static ConnectionsManager instance162(162);
            return instance162;
        case 163:
            static ConnectionsManager instance163(163);
            return instance163;
        case 164:
            static ConnectionsManager instance164(164);
            return instance164;
        case 165:
            static ConnectionsManager instance165(165);
            return instance165;
        case 166:
            static ConnectionsManager instance166(166);
            return instance166;
        case 167:
            static ConnectionsManager instance167(167);
            return instance167;
        case 168:
            static ConnectionsManager instance168(168);
            return instance168;
        case 169:
            static ConnectionsManager instance169(169);
            return instance169;
        case 170:
            static ConnectionsManager instance170(170);
            return instance170;
        case 171:
            static ConnectionsManager instance171(171);
            return instance171;
        case 172:
            static ConnectionsManager instance172(172);
            return instance172;
        case 173:
            static ConnectionsManager instance173(173);
            return instance173;
        case 174:
            static ConnectionsManager instance174(174);
            return instance174;
        case 175:
            static ConnectionsManager instance175(175);
            return instance175;
        case 176:
            static ConnectionsManager instance176(176);
            return instance176;
        case 177:
            static ConnectionsManager instance177(177);
            return instance177;
        case 178:
            static ConnectionsManager instance178(178);
            return instance178;
        case 179:
            static ConnectionsManager instance179(179);
            return instance179;
        case 180:
            static ConnectionsManager instance180(180);
            return instance180;
        case 181:
            static ConnectionsManager instance181(181);
            return instance181;
        case 182:
            static ConnectionsManager instance182(182);
            return instance182;
        case 183:
            static ConnectionsManager instance183(183);
            return instance183;
        case 184:
            static ConnectionsManager instance184(184);
            return instance184;
        case 185:
            static ConnectionsManager instance185(185);
            return instance185;
        case 186:
            static ConnectionsManager instance186(186);
            return instance186;
        case 187:
            static ConnectionsManager instance187(187);
            return instance187;
        case 188:
            static ConnectionsManager instance188(188);
            return instance188;
        case 189:
            static ConnectionsManager instance189(189);
            return instance189;
        case 190:
            static ConnectionsManager instance190(190);
            return instance190;
        case 191:
            static ConnectionsManager instance191(191);
            return instance191;
        case 192:
            static ConnectionsManager instance192(192);
            return instance192;
        case 193:
            static ConnectionsManager instance193(193);
            return instance193;
        case 194:
            static ConnectionsManager instance194(194);
            return instance194;
        case 195:
            static ConnectionsManager instance195(195);
            return instance195;
        case 196:
            static ConnectionsManager instance196(196);
            return instance196;
        case 197:
            static ConnectionsManager instance197(197);
            return instance197;
        case 198:
            static ConnectionsManager instance198(198);
            return instance198;
        case 199:
            static ConnectionsManager instance199(199);
            return instance199;
        case 200:
            static ConnectionsManager instance200(200);
            return instance200;
        case 201:
            static ConnectionsManager instance201(201);
            return instance201;
        case 202:
            static ConnectionsManager instance202(202);
            return instance202;
        case 203:
            static ConnectionsManager instance203(203);
            return instance203;
        case 204:
            static ConnectionsManager instance204(204);
            return instance204;
        case 205:
            static ConnectionsManager instance205(205);
            return instance205;
        case 206:
            static ConnectionsManager instance206(206);
            return instance206;
        case 207:
            static ConnectionsManager instance207(207);
            return instance207;
        case 208:
            static ConnectionsManager instance208(208);
            return instance208;
        case 209:
            static ConnectionsManager instance209(209);
            return instance209;
        case 210:
            static ConnectionsManager instance210(210);
            return instance210;
        case 211:
            static ConnectionsManager instance211(211);
            return instance211;
        case 212:
            static ConnectionsManager instance212(212);
            return instance212;
        case 213:
            static ConnectionsManager instance213(213);
            return instance213;
        case 214:
            static ConnectionsManager instance214(214);
            return instance214;
        case 215:
            static ConnectionsManager instance215(215);
            return instance215;
        case 216:
            static ConnectionsManager instance216(216);
            return instance216;
        case 217:
            static ConnectionsManager instance217(217);
            return instance217;
        case 218:
            static ConnectionsManager instance218(218);
            return instance218;
        case 219:
            static ConnectionsManager instance219(219);
            return instance219;
        case 220:
            static ConnectionsManager instance220(220);
            return instance220;
        case 221:
            static ConnectionsManager instance221(221);
            return instance221;
        case 222:
            static ConnectionsManager instance222(222);
            return instance222;
        case 223:
            static ConnectionsManager instance223(223);
            return instance223;
        case 224:
            static ConnectionsManager instance224(224);
            return instance224;
        case 225:
            static ConnectionsManager instance225(225);
            return instance225;
        case 226:
            static ConnectionsManager instance226(226);
            return instance226;
        case 227:
            static ConnectionsManager instance227(227);
            return instance227;
        case 228:
            static ConnectionsManager instance228(228);
            return instance228;
        case 229:
            static ConnectionsManager instance229(229);
            return instance229;
        case 230:
            static ConnectionsManager instance230(230);
            return instance230;
        case 231:
            static ConnectionsManager instance231(231);
            return instance231;
        case 232:
            static ConnectionsManager instance232(232);
            return instance232;
        case 233:
            static ConnectionsManager instance233(233);
            return instance233;
        case 234:
            static ConnectionsManager instance234(234);
            return instance234;
        case 235:
            static ConnectionsManager instance235(235);
            return instance235;
        case 236:
            static ConnectionsManager instance236(236);
            return instance236;
        case 237:
            static ConnectionsManager instance237(237);
            return instance237;
        case 238:
            static ConnectionsManager instance238(238);
            return instance238;
        case 239:
            static ConnectionsManager instance239(239);
            return instance239;
        case 240:
            static ConnectionsManager instance240(240);
            return instance240;
        case 241:
            static ConnectionsManager instance241(241);
            return instance241;
        case 242:
            static ConnectionsManager instance242(242);
            return instance242;
        case 243:
            static ConnectionsManager instance243(243);
            return instance243;
        case 244:
            static ConnectionsManager instance244(244);
            return instance244;
        case 245:
            static ConnectionsManager instance245(245);
            return instance245;
        case 246:
            static ConnectionsManager instance246(246);
            return instance246;
        case 247:
            static ConnectionsManager instance247(247);
            return instance247;
        case 248:
            static ConnectionsManager instance248(248);
            return instance248;
        case 249:
            static ConnectionsManager instance249(249);
            return instance249;
        case 250:
            static ConnectionsManager instance250(250);
            return instance250;
        case 251:
            static ConnectionsManager instance251(251);
            return instance251;
        case 252:
            static ConnectionsManager instance252(252);
            return instance252;
        case 253:
            static ConnectionsManager instance253(253);
            return instance253;
        case 254:
            static ConnectionsManager instance254(254);
            return instance254;
        case 255:
            static ConnectionsManager instance255(255);
            return instance255;
        case 256:
            static ConnectionsManager instance256(256);
            return instance256;
        case 257:
            static ConnectionsManager instance257(257);
            return instance257;
        case 258:
            static ConnectionsManager instance258(258);
            return instance258;
        case 259:
            static ConnectionsManager instance259(259);
            return instance259;
        case 260:
            static ConnectionsManager instance260(260);
            return instance260;
        case 261:
            static ConnectionsManager instance261(261);
            return instance261;
        case 262:
            static ConnectionsManager instance262(262);
            return instance262;
        case 263:
            static ConnectionsManager instance263(263);
            return instance263;
        case 264:
            static ConnectionsManager instance264(264);
            return instance264;
        case 265:
            static ConnectionsManager instance265(265);
            return instance265;
        case 266:
            static ConnectionsManager instance266(266);
            return instance266;
        case 267:
            static ConnectionsManager instance267(267);
            return instance267;
        case 268:
            static ConnectionsManager instance268(268);
            return instance268;
        case 269:
            static ConnectionsManager instance269(269);
            return instance269;
        case 270:
            static ConnectionsManager instance270(270);
            return instance270;
        case 271:
            static ConnectionsManager instance271(271);
            return instance271;
        case 272:
            static ConnectionsManager instance272(272);
            return instance272;
        case 273:
            static ConnectionsManager instance273(273);
            return instance273;
        case 274:
            static ConnectionsManager instance274(274);
            return instance274;
        case 275:
            static ConnectionsManager instance275(275);
            return instance275;
        case 276:
            static ConnectionsManager instance276(276);
            return instance276;
        case 277:
            static ConnectionsManager instance277(277);
            return instance277;
        case 278:
            static ConnectionsManager instance278(278);
            return instance278;
        case 279:
            static ConnectionsManager instance279(279);
            return instance279;
        case 280:
            static ConnectionsManager instance280(280);
            return instance280;
        case 281:
            static ConnectionsManager instance281(281);
            return instance281;
        case 282:
            static ConnectionsManager instance282(282);
            return instance282;
        case 283:
            static ConnectionsManager instance283(283);
            return instance283;
        case 284:
            static ConnectionsManager instance284(284);
            return instance284;
        case 285:
            static ConnectionsManager instance285(285);
            return instance285;
        case 286:
            static ConnectionsManager instance286(286);
            return instance286;
        case 287:
            static ConnectionsManager instance287(287);
            return instance287;
        case 288:
            static ConnectionsManager instance288(288);
            return instance288;
        case 289:
            static ConnectionsManager instance289(289);
            return instance289;
        case 290:
            static ConnectionsManager instance290(290);
            return instance290;
        case 291:
            static ConnectionsManager instance291(291);
            return instance291;
        case 292:
            static ConnectionsManager instance292(292);
            return instance292;
        case 293:
            static ConnectionsManager instance293(293);
            return instance293;
        case 294:
            static ConnectionsManager instance294(294);
            return instance294;
        case 295:
            static ConnectionsManager instance295(295);
            return instance295;
        case 296:
            static ConnectionsManager instance296(296);
            return instance296;
        case 297:
            static ConnectionsManager instance297(297);
            return instance297;
        case 298:
            static ConnectionsManager instance298(298);
            return instance298;
        case 299:
            static ConnectionsManager instance299(299);
            return instance299;
        case 300:
            static ConnectionsManager instance300(300);
            return instance300;
        case 301:
            static ConnectionsManager instance301(301);
            return instance301;
        case 302:
            static ConnectionsManager instance302(302);
            return instance302;
        case 303:
            static ConnectionsManager instance303(303);
            return instance303;
        case 304:
            static ConnectionsManager instance304(304);
            return instance304;
        case 305:
            static ConnectionsManager instance305(305);
            return instance305;
        case 306:
            static ConnectionsManager instance306(306);
            return instance306;
        case 307:
            static ConnectionsManager instance307(307);
            return instance307;
        case 308:
            static ConnectionsManager instance308(308);
            return instance308;
        case 309:
            static ConnectionsManager instance309(309);
            return instance309;
        case 310:
            static ConnectionsManager instance310(310);
            return instance310;
        case 311:
            static ConnectionsManager instance311(311);
            return instance311;
        case 312:
            static ConnectionsManager instance312(312);
            return instance312;
        case 313:
            static ConnectionsManager instance313(313);
            return instance313;
        case 314:
            static ConnectionsManager instance314(314);
            return instance314;
        case 315:
            static ConnectionsManager instance315(315);
            return instance315;
        case 316:
            static ConnectionsManager instance316(316);
            return instance316;
        case 317:
            static ConnectionsManager instance317(317);
            return instance317;
        case 318:
            static ConnectionsManager instance318(318);
            return instance318;
        case 319:
            static ConnectionsManager instance319(319);
            return instance319;
        case 320:
            static ConnectionsManager instance320(320);
            return instance320;
        case 321:
            static ConnectionsManager instance321(321);
            return instance321;
        case 322:
            static ConnectionsManager instance322(322);
            return instance322;
        case 323:
            static ConnectionsManager instance323(323);
            return instance323;
        case 324:
            static ConnectionsManager instance324(324);
            return instance324;
        case 325:
            static ConnectionsManager instance325(325);
            return instance325;
        case 326:
            static ConnectionsManager instance326(326);
            return instance326;
        case 327:
            static ConnectionsManager instance327(327);
            return instance327;
        case 328:
            static ConnectionsManager instance328(328);
            return instance328;
        case 329:
            static ConnectionsManager instance329(329);
            return instance329;
        case 330:
            static ConnectionsManager instance330(330);
            return instance330;
        case 331:
            static ConnectionsManager instance331(331);
            return instance331;
        case 332:
            static ConnectionsManager instance332(332);
            return instance332;
        case 333:
            static ConnectionsManager instance333(333);
            return instance333;
        case 334:
            static ConnectionsManager instance334(334);
            return instance334;
        case 335:
            static ConnectionsManager instance335(335);
            return instance335;
        case 336:
            static ConnectionsManager instance336(336);
            return instance336;
        case 337:
            static ConnectionsManager instance337(337);
            return instance337;
        case 338:
            static ConnectionsManager instance338(338);
            return instance338;
        case 339:
            static ConnectionsManager instance339(339);
            return instance339;
        case 340:
            static ConnectionsManager instance340(340);
            return instance340;
        case 341:
            static ConnectionsManager instance341(341);
            return instance341;
        case 342:
            static ConnectionsManager instance342(342);
            return instance342;
        case 343:
            static ConnectionsManager instance343(343);
            return instance343;
        case 344:
            static ConnectionsManager instance344(344);
            return instance344;
        case 345:
            static ConnectionsManager instance345(345);
            return instance345;
        case 346:
            static ConnectionsManager instance346(346);
            return instance346;
        case 347:
            static ConnectionsManager instance347(347);
            return instance347;
        case 348:
            static ConnectionsManager instance348(348);
            return instance348;
        case 349:
            static ConnectionsManager instance349(349);
            return instance349;
        case 350:
            static ConnectionsManager instance350(350);
            return instance350;
        case 351:
            static ConnectionsManager instance351(351);
            return instance351;
        case 352:
            static ConnectionsManager instance352(352);
            return instance352;
        case 353:
            static ConnectionsManager instance353(353);
            return instance353;
        case 354:
            static ConnectionsManager instance354(354);
            return instance354;
        case 355:
            static ConnectionsManager instance355(355);
            return instance355;
        case 356:
            static ConnectionsManager instance356(356);
            return instance356;
        case 357:
            static ConnectionsManager instance357(357);
            return instance357;
        case 358:
            static ConnectionsManager instance358(358);
            return instance358;
        case 359:
            static ConnectionsManager instance359(359);
            return instance359;
        case 360:
            static ConnectionsManager instance360(360);
            return instance360;
        case 361:
            static ConnectionsManager instance361(361);
            return instance361;
        case 362:
            static ConnectionsManager instance362(362);
            return instance362;
        case 363:
            static ConnectionsManager instance363(363);
            return instance363;
        case 364:
            static ConnectionsManager instance364(364);
            return instance364;
        case 365:
            static ConnectionsManager instance365(365);
            return instance365;
        case 366:
            static ConnectionsManager instance366(366);
            return instance366;
        case 367:
            static ConnectionsManager instance367(367);
            return instance367;
        case 368:
            static ConnectionsManager instance368(368);
            return instance368;
        case 369:
            static ConnectionsManager instance369(369);
            return instance369;
        case 370:
            static ConnectionsManager instance370(370);
            return instance370;
        case 371:
            static ConnectionsManager instance371(371);
            return instance371;
        case 372:
            static ConnectionsManager instance372(372);
            return instance372;
        case 373:
            static ConnectionsManager instance373(373);
            return instance373;
        case 374:
            static ConnectionsManager instance374(374);
            return instance374;
        case 375:
            static ConnectionsManager instance375(375);
            return instance375;
        case 376:
            static ConnectionsManager instance376(376);
            return instance376;
        case 377:
            static ConnectionsManager instance377(377);
            return instance377;
        case 378:
            static ConnectionsManager instance378(378);
            return instance378;
        case 379:
            static ConnectionsManager instance379(379);
            return instance379;
        case 380:
            static ConnectionsManager instance380(380);
            return instance380;
        case 381:
            static ConnectionsManager instance381(381);
            return instance381;
        case 382:
            static ConnectionsManager instance382(382);
            return instance382;
        case 383:
            static ConnectionsManager instance383(383);
            return instance383;
        case 384:
            static ConnectionsManager instance384(384);
            return instance384;
        case 385:
            static ConnectionsManager instance385(385);
            return instance385;
        case 386:
            static ConnectionsManager instance386(386);
            return instance386;
        case 387:
            static ConnectionsManager instance387(387);
            return instance387;
        case 388:
            static ConnectionsManager instance388(388);
            return instance388;
        case 389:
            static ConnectionsManager instance389(389);
            return instance389;
        case 390:
            static ConnectionsManager instance390(390);
            return instance390;
        case 391:
            static ConnectionsManager instance391(391);
            return instance391;
        case 392:
            static ConnectionsManager instance392(392);
            return instance392;
        case 393:
            static ConnectionsManager instance393(393);
            return instance393;
        case 394:
            static ConnectionsManager instance394(394);
            return instance394;
        case 395:
            static ConnectionsManager instance395(395);
            return instance395;
        case 396:
            static ConnectionsManager instance396(396);
            return instance396;
        case 397:
            static ConnectionsManager instance397(397);
            return instance397;
        case 398:
            static ConnectionsManager instance398(398);
            return instance398;
        case 399:
            static ConnectionsManager instance399(399);
            return instance399;
        case 400:
            static ConnectionsManager instance400(400);
            return instance400;
        case 401:
            static ConnectionsManager instance401(401);
            return instance401;
        case 402:
            static ConnectionsManager instance402(402);
            return instance402;
        case 403:
            static ConnectionsManager instance403(403);
            return instance403;
        case 404:
            static ConnectionsManager instance404(404);
            return instance404;
        case 405:
            static ConnectionsManager instance405(405);
            return instance405;
        case 406:
            static ConnectionsManager instance406(406);
            return instance406;
        case 407:
            static ConnectionsManager instance407(407);
            return instance407;
        case 408:
            static ConnectionsManager instance408(408);
            return instance408;
        case 409:
            static ConnectionsManager instance409(409);
            return instance409;
        case 410:
            static ConnectionsManager instance410(410);
            return instance410;
        case 411:
            static ConnectionsManager instance411(411);
            return instance411;
        case 412:
            static ConnectionsManager instance412(412);
            return instance412;
        case 413:
            static ConnectionsManager instance413(413);
            return instance413;
        case 414:
            static ConnectionsManager instance414(414);
            return instance414;
        case 415:
            static ConnectionsManager instance415(415);
            return instance415;
        case 416:
            static ConnectionsManager instance416(416);
            return instance416;
        case 417:
            static ConnectionsManager instance417(417);
            return instance417;
        case 418:
            static ConnectionsManager instance418(418);
            return instance418;
        case 419:
            static ConnectionsManager instance419(419);
            return instance419;
        case 420:
            static ConnectionsManager instance420(420);
            return instance420;
        case 421:
            static ConnectionsManager instance421(421);
            return instance421;
        case 422:
            static ConnectionsManager instance422(422);
            return instance422;
        case 423:
            static ConnectionsManager instance423(423);
            return instance423;
        case 424:
            static ConnectionsManager instance424(424);
            return instance424;
        case 425:
            static ConnectionsManager instance425(425);
            return instance425;
        case 426:
            static ConnectionsManager instance426(426);
            return instance426;
        case 427:
            static ConnectionsManager instance427(427);
            return instance427;
        case 428:
            static ConnectionsManager instance428(428);
            return instance428;
        case 429:
            static ConnectionsManager instance429(429);
            return instance429;
        case 430:
            static ConnectionsManager instance430(430);
            return instance430;
        case 431:
            static ConnectionsManager instance431(431);
            return instance431;
        case 432:
            static ConnectionsManager instance432(432);
            return instance432;
        case 433:
            static ConnectionsManager instance433(433);
            return instance433;
        case 434:
            static ConnectionsManager instance434(434);
            return instance434;
        case 435:
            static ConnectionsManager instance435(435);
            return instance435;
        case 436:
            static ConnectionsManager instance436(436);
            return instance436;
        case 437:
            static ConnectionsManager instance437(437);
            return instance437;
        case 438:
            static ConnectionsManager instance438(438);
            return instance438;
        case 439:
            static ConnectionsManager instance439(439);
            return instance439;
        case 440:
            static ConnectionsManager instance440(440);
            return instance440;
        case 441:
            static ConnectionsManager instance441(441);
            return instance441;
        case 442:
            static ConnectionsManager instance442(442);
            return instance442;
        case 443:
            static ConnectionsManager instance443(443);
            return instance443;
        case 444:
            static ConnectionsManager instance444(444);
            return instance444;
        case 445:
            static ConnectionsManager instance445(445);
            return instance445;
        case 446:
            static ConnectionsManager instance446(446);
            return instance446;
        case 447:
            static ConnectionsManager instance447(447);
            return instance447;
        case 448:
            static ConnectionsManager instance448(448);
            return instance448;
        case 449:
            static ConnectionsManager instance449(449);
            return instance449;
        case 450:
            static ConnectionsManager instance450(450);
            return instance450;
        case 451:
            static ConnectionsManager instance451(451);
            return instance451;
        case 452:
            static ConnectionsManager instance452(452);
            return instance452;
        case 453:
            static ConnectionsManager instance453(453);
            return instance453;
        case 454:
            static ConnectionsManager instance454(454);
            return instance454;
        case 455:
            static ConnectionsManager instance455(455);
            return instance455;
        case 456:
            static ConnectionsManager instance456(456);
            return instance456;
        case 457:
            static ConnectionsManager instance457(457);
            return instance457;
        case 458:
            static ConnectionsManager instance458(458);
            return instance458;
        case 459:
            static ConnectionsManager instance459(459);
            return instance459;
        case 460:
            static ConnectionsManager instance460(460);
            return instance460;
        case 461:
            static ConnectionsManager instance461(461);
            return instance461;
        case 462:
            static ConnectionsManager instance462(462);
            return instance462;
        case 463:
            static ConnectionsManager instance463(463);
            return instance463;
        case 464:
            static ConnectionsManager instance464(464);
            return instance464;
        case 465:
            static ConnectionsManager instance465(465);
            return instance465;
        case 466:
            static ConnectionsManager instance466(466);
            return instance466;
        case 467:
            static ConnectionsManager instance467(467);
            return instance467;
        case 468:
            static ConnectionsManager instance468(468);
            return instance468;
        case 469:
            static ConnectionsManager instance469(469);
            return instance469;
        case 470:
            static ConnectionsManager instance470(470);
            return instance470;
        case 471:
            static ConnectionsManager instance471(471);
            return instance471;
        case 472:
            static ConnectionsManager instance472(472);
            return instance472;
        case 473:
            static ConnectionsManager instance473(473);
            return instance473;
        case 474:
            static ConnectionsManager instance474(474);
            return instance474;
        case 475:
            static ConnectionsManager instance475(475);
            return instance475;
        case 476:
            static ConnectionsManager instance476(476);
            return instance476;
        case 477:
            static ConnectionsManager instance477(477);
            return instance477;
        case 478:
            static ConnectionsManager instance478(478);
            return instance478;
        case 479:
            static ConnectionsManager instance479(479);
            return instance479;
        case 480:
            static ConnectionsManager instance480(480);
            return instance480;
        case 481:
            static ConnectionsManager instance481(481);
            return instance481;
        case 482:
            static ConnectionsManager instance482(482);
            return instance482;
        case 483:
            static ConnectionsManager instance483(483);
            return instance483;
        case 484:
            static ConnectionsManager instance484(484);
            return instance484;
        case 485:
            static ConnectionsManager instance485(485);
            return instance485;
        case 486:
            static ConnectionsManager instance486(486);
            return instance486;
        case 487:
            static ConnectionsManager instance487(487);
            return instance487;
        case 488:
            static ConnectionsManager instance488(488);
            return instance488;
        case 489:
            static ConnectionsManager instance489(489);
            return instance489;
        case 490:
            static ConnectionsManager instance490(490);
            return instance490;
        case 491:
            static ConnectionsManager instance491(491);
            return instance491;
        case 492:
            static ConnectionsManager instance492(492);
            return instance492;
        case 493:
            static ConnectionsManager instance493(493);
            return instance493;
        case 494:
            static ConnectionsManager instance494(494);
            return instance494;
        case 495:
            static ConnectionsManager instance495(495);
            return instance495;
        case 496:
            static ConnectionsManager instance496(496);
            return instance496;
        case 497:
            static ConnectionsManager instance497(497);
            return instance497;
        case 498:
            static ConnectionsManager instance498(498);
            return instance498;
        case 499:
            static ConnectionsManager instance499(499);
            return instance499;
        case 500:
            static ConnectionsManager instance500(500);
            return instance500;
        case 501:
            static ConnectionsManager instance501(501);
            return instance501;
        case 502:
            static ConnectionsManager instance502(502);
            return instance502;
        case 503:
            static ConnectionsManager instance503(503);
            return instance503;
        case 504:
            static ConnectionsManager instance504(504);
            return instance504;
        case 505:
            static ConnectionsManager instance505(505);
            return instance505;
        case 506:
            static ConnectionsManager instance506(506);
            return instance506;
        case 507:
            static ConnectionsManager instance507(507);
            return instance507;
        case 508:
            static ConnectionsManager instance508(508);
            return instance508;
        case 509:
            static ConnectionsManager instance509(509);
            return instance509;
        case 510:
            static ConnectionsManager instance510(510);
            return instance510;
        case 511:
            static ConnectionsManager instance511(511);
            return instance511;
        case 512:
            static ConnectionsManager instance512(512);
            return instance512;
        case 513:
            static ConnectionsManager instance513(513);
            return instance513;
        case 514:
            static ConnectionsManager instance514(514);
            return instance514;
        case 515:
            static ConnectionsManager instance515(515);
            return instance515;
        case 516:
            static ConnectionsManager instance516(516);
            return instance516;
        case 517:
            static ConnectionsManager instance517(517);
            return instance517;
        case 518:
            static ConnectionsManager instance518(518);
            return instance518;
        case 519:
            static ConnectionsManager instance519(519);
            return instance519;
        case 520:
            static ConnectionsManager instance520(520);
            return instance520;
        case 521:
            static ConnectionsManager instance521(521);
            return instance521;
        case 522:
            static ConnectionsManager instance522(522);
            return instance522;
        case 523:
            static ConnectionsManager instance523(523);
            return instance523;
        case 524:
            static ConnectionsManager instance524(524);
            return instance524;
        case 525:
            static ConnectionsManager instance525(525);
            return instance525;
        case 526:
            static ConnectionsManager instance526(526);
            return instance526;
        case 527:
            static ConnectionsManager instance527(527);
            return instance527;
        case 528:
            static ConnectionsManager instance528(528);
            return instance528;
        case 529:
            static ConnectionsManager instance529(529);
            return instance529;
        case 530:
            static ConnectionsManager instance530(530);
            return instance530;
        case 531:
            static ConnectionsManager instance531(531);
            return instance531;
        case 532:
            static ConnectionsManager instance532(532);
            return instance532;
        case 533:
            static ConnectionsManager instance533(533);
            return instance533;
        case 534:
            static ConnectionsManager instance534(534);
            return instance534;
        case 535:
            static ConnectionsManager instance535(535);
            return instance535;
        case 536:
            static ConnectionsManager instance536(536);
            return instance536;
        case 537:
            static ConnectionsManager instance537(537);
            return instance537;
        case 538:
            static ConnectionsManager instance538(538);
            return instance538;
        case 539:
            static ConnectionsManager instance539(539);
            return instance539;
        case 540:
            static ConnectionsManager instance540(540);
            return instance540;
        case 541:
            static ConnectionsManager instance541(541);
            return instance541;
        case 542:
            static ConnectionsManager instance542(542);
            return instance542;
        case 543:
            static ConnectionsManager instance543(543);
            return instance543;
        case 544:
            static ConnectionsManager instance544(544);
            return instance544;
        case 545:
            static ConnectionsManager instance545(545);
            return instance545;
        case 546:
            static ConnectionsManager instance546(546);
            return instance546;
        case 547:
            static ConnectionsManager instance547(547);
            return instance547;
        case 548:
            static ConnectionsManager instance548(548);
            return instance548;
        case 549:
            static ConnectionsManager instance549(549);
            return instance549;
        case 550:
            static ConnectionsManager instance550(550);
            return instance550;
        case 551:
            static ConnectionsManager instance551(551);
            return instance551;
        case 552:
            static ConnectionsManager instance552(552);
            return instance552;
        case 553:
            static ConnectionsManager instance553(553);
            return instance553;
        case 554:
            static ConnectionsManager instance554(554);
            return instance554;
        case 555:
            static ConnectionsManager instance555(555);
            return instance555;
        case 556:
            static ConnectionsManager instance556(556);
            return instance556;
        case 557:
            static ConnectionsManager instance557(557);
            return instance557;
        case 558:
            static ConnectionsManager instance558(558);
            return instance558;
        case 559:
            static ConnectionsManager instance559(559);
            return instance559;
        case 560:
            static ConnectionsManager instance560(560);
            return instance560;
        case 561:
            static ConnectionsManager instance561(561);
            return instance561;
        case 562:
            static ConnectionsManager instance562(562);
            return instance562;
        case 563:
            static ConnectionsManager instance563(563);
            return instance563;
        case 564:
            static ConnectionsManager instance564(564);
            return instance564;
        case 565:
            static ConnectionsManager instance565(565);
            return instance565;
        case 566:
            static ConnectionsManager instance566(566);
            return instance566;
        case 567:
            static ConnectionsManager instance567(567);
            return instance567;
        case 568:
            static ConnectionsManager instance568(568);
            return instance568;
        case 569:
            static ConnectionsManager instance569(569);
            return instance569;
        case 570:
            static ConnectionsManager instance570(570);
            return instance570;
        case 571:
            static ConnectionsManager instance571(571);
            return instance571;
        case 572:
            static ConnectionsManager instance572(572);
            return instance572;
        case 573:
            static ConnectionsManager instance573(573);
            return instance573;
        case 574:
            static ConnectionsManager instance574(574);
            return instance574;
        case 575:
            static ConnectionsManager instance575(575);
            return instance575;
        case 576:
            static ConnectionsManager instance576(576);
            return instance576;
        case 577:
            static ConnectionsManager instance577(577);
            return instance577;
        case 578:
            static ConnectionsManager instance578(578);
            return instance578;
        case 579:
            static ConnectionsManager instance579(579);
            return instance579;
        case 580:
            static ConnectionsManager instance580(580);
            return instance580;
        case 581:
            static ConnectionsManager instance581(581);
            return instance581;
        case 582:
            static ConnectionsManager instance582(582);
            return instance582;
        case 583:
            static ConnectionsManager instance583(583);
            return instance583;
        case 584:
            static ConnectionsManager instance584(584);
            return instance584;
        case 585:
            static ConnectionsManager instance585(585);
            return instance585;
        case 586:
            static ConnectionsManager instance586(586);
            return instance586;
        case 587:
            static ConnectionsManager instance587(587);
            return instance587;
        case 588:
            static ConnectionsManager instance588(588);
            return instance588;
        case 589:
            static ConnectionsManager instance589(589);
            return instance589;
        case 590:
            static ConnectionsManager instance590(590);
            return instance590;
        case 591:
            static ConnectionsManager instance591(591);
            return instance591;
        case 592:
            static ConnectionsManager instance592(592);
            return instance592;
        case 593:
            static ConnectionsManager instance593(593);
            return instance593;
        case 594:
            static ConnectionsManager instance594(594);
            return instance594;
        case 595:
            static ConnectionsManager instance595(595);
            return instance595;
        case 596:
            static ConnectionsManager instance596(596);
            return instance596;
        case 597:
            static ConnectionsManager instance597(597);
            return instance597;
        case 598:
            static ConnectionsManager instance598(598);
            return instance598;
        case 599:
            static ConnectionsManager instance599(599);
            return instance599;
        case 600:
            static ConnectionsManager instance600(600);
            return instance600;
        case 601:
            static ConnectionsManager instance601(601);
            return instance601;
        case 602:
            static ConnectionsManager instance602(602);
            return instance602;
        case 603:
            static ConnectionsManager instance603(603);
            return instance603;
        case 604:
            static ConnectionsManager instance604(604);
            return instance604;
        case 605:
            static ConnectionsManager instance605(605);
            return instance605;
        case 606:
            static ConnectionsManager instance606(606);
            return instance606;
        case 607:
            static ConnectionsManager instance607(607);
            return instance607;
        case 608:
            static ConnectionsManager instance608(608);
            return instance608;
        case 609:
            static ConnectionsManager instance609(609);
            return instance609;
        case 610:
            static ConnectionsManager instance610(610);
            return instance610;
        case 611:
            static ConnectionsManager instance611(611);
            return instance611;
        case 612:
            static ConnectionsManager instance612(612);
            return instance612;
        case 613:
            static ConnectionsManager instance613(613);
            return instance613;
        case 614:
            static ConnectionsManager instance614(614);
            return instance614;
        case 615:
            static ConnectionsManager instance615(615);
            return instance615;
        case 616:
            static ConnectionsManager instance616(616);
            return instance616;
        case 617:
            static ConnectionsManager instance617(617);
            return instance617;
        case 618:
            static ConnectionsManager instance618(618);
            return instance618;
        case 619:
            static ConnectionsManager instance619(619);
            return instance619;
        case 620:
            static ConnectionsManager instance620(620);
            return instance620;
        case 621:
            static ConnectionsManager instance621(621);
            return instance621;
        case 622:
            static ConnectionsManager instance622(622);
            return instance622;
        case 623:
            static ConnectionsManager instance623(623);
            return instance623;
        case 624:
            static ConnectionsManager instance624(624);
            return instance624;
        case 625:
            static ConnectionsManager instance625(625);
            return instance625;
        case 626:
            static ConnectionsManager instance626(626);
            return instance626;
        case 627:
            static ConnectionsManager instance627(627);
            return instance627;
        case 628:
            static ConnectionsManager instance628(628);
            return instance628;
        case 629:
            static ConnectionsManager instance629(629);
            return instance629;
        case 630:
            static ConnectionsManager instance630(630);
            return instance630;
        case 631:
            static ConnectionsManager instance631(631);
            return instance631;
        case 632:
            static ConnectionsManager instance632(632);
            return instance632;
        case 633:
            static ConnectionsManager instance633(633);
            return instance633;
        case 634:
            static ConnectionsManager instance634(634);
            return instance634;
        case 635:
            static ConnectionsManager instance635(635);
            return instance635;
        case 636:
            static ConnectionsManager instance636(636);
            return instance636;
        case 637:
            static ConnectionsManager instance637(637);
            return instance637;
        case 638:
            static ConnectionsManager instance638(638);
            return instance638;
        case 639:
            static ConnectionsManager instance639(639);
            return instance639;
        case 640:
            static ConnectionsManager instance640(640);
            return instance640;
        case 641:
            static ConnectionsManager instance641(641);
            return instance641;
        case 642:
            static ConnectionsManager instance642(642);
            return instance642;
        case 643:
            static ConnectionsManager instance643(643);
            return instance643;
        case 644:
            static ConnectionsManager instance644(644);
            return instance644;
        case 645:
            static ConnectionsManager instance645(645);
            return instance645;
        case 646:
            static ConnectionsManager instance646(646);
            return instance646;
        case 647:
            static ConnectionsManager instance647(647);
            return instance647;
        case 648:
            static ConnectionsManager instance648(648);
            return instance648;
        case 649:
            static ConnectionsManager instance649(649);
            return instance649;
        case 650:
            static ConnectionsManager instance650(650);
            return instance650;
        case 651:
            static ConnectionsManager instance651(651);
            return instance651;
        case 652:
            static ConnectionsManager instance652(652);
            return instance652;
        case 653:
            static ConnectionsManager instance653(653);
            return instance653;
        case 654:
            static ConnectionsManager instance654(654);
            return instance654;
        case 655:
            static ConnectionsManager instance655(655);
            return instance655;
        case 656:
            static ConnectionsManager instance656(656);
            return instance656;
        case 657:
            static ConnectionsManager instance657(657);
            return instance657;
        case 658:
            static ConnectionsManager instance658(658);
            return instance658;
        case 659:
            static ConnectionsManager instance659(659);
            return instance659;
        case 660:
            static ConnectionsManager instance660(660);
            return instance660;
        case 661:
            static ConnectionsManager instance661(661);
            return instance661;
        case 662:
            static ConnectionsManager instance662(662);
            return instance662;
        case 663:
            static ConnectionsManager instance663(663);
            return instance663;
        case 664:
            static ConnectionsManager instance664(664);
            return instance664;
        case 665:
            static ConnectionsManager instance665(665);
            return instance665;
        case 666:
            static ConnectionsManager instance666(666);
            return instance666;
        case 667:
            static ConnectionsManager instance667(667);
            return instance667;
        case 668:
            static ConnectionsManager instance668(668);
            return instance668;
        case 669:
            static ConnectionsManager instance669(669);
            return instance669;
        case 670:
            static ConnectionsManager instance670(670);
            return instance670;
        case 671:
            static ConnectionsManager instance671(671);
            return instance671;
        case 672:
            static ConnectionsManager instance672(672);
            return instance672;
        case 673:
            static ConnectionsManager instance673(673);
            return instance673;
        case 674:
            static ConnectionsManager instance674(674);
            return instance674;
        case 675:
            static ConnectionsManager instance675(675);
            return instance675;
        case 676:
            static ConnectionsManager instance676(676);
            return instance676;
        case 677:
            static ConnectionsManager instance677(677);
            return instance677;
        case 678:
            static ConnectionsManager instance678(678);
            return instance678;
        case 679:
            static ConnectionsManager instance679(679);
            return instance679;
        case 680:
            static ConnectionsManager instance680(680);
            return instance680;
        case 681:
            static ConnectionsManager instance681(681);
            return instance681;
        case 682:
            static ConnectionsManager instance682(682);
            return instance682;
        case 683:
            static ConnectionsManager instance683(683);
            return instance683;
        case 684:
            static ConnectionsManager instance684(684);
            return instance684;
        case 685:
            static ConnectionsManager instance685(685);
            return instance685;
        case 686:
            static ConnectionsManager instance686(686);
            return instance686;
        case 687:
            static ConnectionsManager instance687(687);
            return instance687;
        case 688:
            static ConnectionsManager instance688(688);
            return instance688;
        case 689:
            static ConnectionsManager instance689(689);
            return instance689;
        case 690:
            static ConnectionsManager instance690(690);
            return instance690;
        case 691:
            static ConnectionsManager instance691(691);
            return instance691;
        case 692:
            static ConnectionsManager instance692(692);
            return instance692;
        case 693:
            static ConnectionsManager instance693(693);
            return instance693;
        case 694:
            static ConnectionsManager instance694(694);
            return instance694;
        case 695:
            static ConnectionsManager instance695(695);
            return instance695;
        case 696:
            static ConnectionsManager instance696(696);
            return instance696;
        case 697:
            static ConnectionsManager instance697(697);
            return instance697;
        case 698:
            static ConnectionsManager instance698(698);
            return instance698;
        case 699:
            static ConnectionsManager instance699(699);
            return instance699;
        case 700:
            static ConnectionsManager instance700(700);
            return instance700;
        case 701:
            static ConnectionsManager instance701(701);
            return instance701;
        case 702:
            static ConnectionsManager instance702(702);
            return instance702;
        case 703:
            static ConnectionsManager instance703(703);
            return instance703;
        case 704:
            static ConnectionsManager instance704(704);
            return instance704;
        case 705:
            static ConnectionsManager instance705(705);
            return instance705;
        case 706:
            static ConnectionsManager instance706(706);
            return instance706;
        case 707:
            static ConnectionsManager instance707(707);
            return instance707;
        case 708:
            static ConnectionsManager instance708(708);
            return instance708;
        case 709:
            static ConnectionsManager instance709(709);
            return instance709;
        case 710:
            static ConnectionsManager instance710(710);
            return instance710;
        case 711:
            static ConnectionsManager instance711(711);
            return instance711;
        case 712:
            static ConnectionsManager instance712(712);
            return instance712;
        case 713:
            static ConnectionsManager instance713(713);
            return instance713;
        case 714:
            static ConnectionsManager instance714(714);
            return instance714;
        case 715:
            static ConnectionsManager instance715(715);
            return instance715;
        case 716:
            static ConnectionsManager instance716(716);
            return instance716;
        case 717:
            static ConnectionsManager instance717(717);
            return instance717;
        case 718:
            static ConnectionsManager instance718(718);
            return instance718;
        case 719:
            static ConnectionsManager instance719(719);
            return instance719;
        case 720:
            static ConnectionsManager instance720(720);
            return instance720;
        case 721:
            static ConnectionsManager instance721(721);
            return instance721;
        case 722:
            static ConnectionsManager instance722(722);
            return instance722;
        case 723:
            static ConnectionsManager instance723(723);
            return instance723;
        case 724:
            static ConnectionsManager instance724(724);
            return instance724;
        case 725:
            static ConnectionsManager instance725(725);
            return instance725;
        case 726:
            static ConnectionsManager instance726(726);
            return instance726;
        case 727:
            static ConnectionsManager instance727(727);
            return instance727;
        case 728:
            static ConnectionsManager instance728(728);
            return instance728;
        case 729:
            static ConnectionsManager instance729(729);
            return instance729;
        case 730:
            static ConnectionsManager instance730(730);
            return instance730;
        case 731:
            static ConnectionsManager instance731(731);
            return instance731;
        case 732:
            static ConnectionsManager instance732(732);
            return instance732;
        case 733:
            static ConnectionsManager instance733(733);
            return instance733;
        case 734:
            static ConnectionsManager instance734(734);
            return instance734;
        case 735:
            static ConnectionsManager instance735(735);
            return instance735;
        case 736:
            static ConnectionsManager instance736(736);
            return instance736;
        case 737:
            static ConnectionsManager instance737(737);
            return instance737;
        case 738:
            static ConnectionsManager instance738(738);
            return instance738;
        case 739:
            static ConnectionsManager instance739(739);
            return instance739;
        case 740:
            static ConnectionsManager instance740(740);
            return instance740;
        case 741:
            static ConnectionsManager instance741(741);
            return instance741;
        case 742:
            static ConnectionsManager instance742(742);
            return instance742;
        case 743:
            static ConnectionsManager instance743(743);
            return instance743;
        case 744:
            static ConnectionsManager instance744(744);
            return instance744;
        case 745:
            static ConnectionsManager instance745(745);
            return instance745;
        case 746:
            static ConnectionsManager instance746(746);
            return instance746;
        case 747:
            static ConnectionsManager instance747(747);
            return instance747;
        case 748:
            static ConnectionsManager instance748(748);
            return instance748;
        case 749:
            static ConnectionsManager instance749(749);
            return instance749;
        case 750:
            static ConnectionsManager instance750(750);
            return instance750;
        case 751:
            static ConnectionsManager instance751(751);
            return instance751;
        case 752:
            static ConnectionsManager instance752(752);
            return instance752;
        case 753:
            static ConnectionsManager instance753(753);
            return instance753;
        case 754:
            static ConnectionsManager instance754(754);
            return instance754;
        case 755:
            static ConnectionsManager instance755(755);
            return instance755;
        case 756:
            static ConnectionsManager instance756(756);
            return instance756;
        case 757:
            static ConnectionsManager instance757(757);
            return instance757;
        case 758:
            static ConnectionsManager instance758(758);
            return instance758;
        case 759:
            static ConnectionsManager instance759(759);
            return instance759;
        case 760:
            static ConnectionsManager instance760(760);
            return instance760;
        case 761:
            static ConnectionsManager instance761(761);
            return instance761;
        case 762:
            static ConnectionsManager instance762(762);
            return instance762;
        case 763:
            static ConnectionsManager instance763(763);
            return instance763;
        case 764:
            static ConnectionsManager instance764(764);
            return instance764;
        case 765:
            static ConnectionsManager instance765(765);
            return instance765;
        case 766:
            static ConnectionsManager instance766(766);
            return instance766;
        case 767:
            static ConnectionsManager instance767(767);
            return instance767;
        case 768:
            static ConnectionsManager instance768(768);
            return instance768;
        case 769:
            static ConnectionsManager instance769(769);
            return instance769;
        case 770:
            static ConnectionsManager instance770(770);
            return instance770;
        case 771:
            static ConnectionsManager instance771(771);
            return instance771;
        case 772:
            static ConnectionsManager instance772(772);
            return instance772;
        case 773:
            static ConnectionsManager instance773(773);
            return instance773;
        case 774:
            static ConnectionsManager instance774(774);
            return instance774;
        case 775:
            static ConnectionsManager instance775(775);
            return instance775;
        case 776:
            static ConnectionsManager instance776(776);
            return instance776;
        case 777:
            static ConnectionsManager instance777(777);
            return instance777;
        case 778:
            static ConnectionsManager instance778(778);
            return instance778;
        case 779:
            static ConnectionsManager instance779(779);
            return instance779;
        case 780:
            static ConnectionsManager instance780(780);
            return instance780;
        case 781:
            static ConnectionsManager instance781(781);
            return instance781;
        case 782:
            static ConnectionsManager instance782(782);
            return instance782;
        case 783:
            static ConnectionsManager instance783(783);
            return instance783;
        case 784:
            static ConnectionsManager instance784(784);
            return instance784;
        case 785:
            static ConnectionsManager instance785(785);
            return instance785;
        case 786:
            static ConnectionsManager instance786(786);
            return instance786;
        case 787:
            static ConnectionsManager instance787(787);
            return instance787;
        case 788:
            static ConnectionsManager instance788(788);
            return instance788;
        case 789:
            static ConnectionsManager instance789(789);
            return instance789;
        case 790:
            static ConnectionsManager instance790(790);
            return instance790;
        case 791:
            static ConnectionsManager instance791(791);
            return instance791;
        case 792:
            static ConnectionsManager instance792(792);
            return instance792;
        case 793:
            static ConnectionsManager instance793(793);
            return instance793;
        case 794:
            static ConnectionsManager instance794(794);
            return instance794;
        case 795:
            static ConnectionsManager instance795(795);
            return instance795;
        case 796:
            static ConnectionsManager instance796(796);
            return instance796;
        case 797:
            static ConnectionsManager instance797(797);
            return instance797;
        case 798:
            static ConnectionsManager instance798(798);
            return instance798;
        case 799:
            static ConnectionsManager instance799(799);
            return instance799;
        case 800:
            static ConnectionsManager instance800(800);
            return instance800;
        case 801:
            static ConnectionsManager instance801(801);
            return instance801;
        case 802:
            static ConnectionsManager instance802(802);
            return instance802;
        case 803:
            static ConnectionsManager instance803(803);
            return instance803;
        case 804:
            static ConnectionsManager instance804(804);
            return instance804;
        case 805:
            static ConnectionsManager instance805(805);
            return instance805;
        case 806:
            static ConnectionsManager instance806(806);
            return instance806;
        case 807:
            static ConnectionsManager instance807(807);
            return instance807;
        case 808:
            static ConnectionsManager instance808(808);
            return instance808;
        case 809:
            static ConnectionsManager instance809(809);
            return instance809;
        case 810:
            static ConnectionsManager instance810(810);
            return instance810;
        case 811:
            static ConnectionsManager instance811(811);
            return instance811;
        case 812:
            static ConnectionsManager instance812(812);
            return instance812;
        case 813:
            static ConnectionsManager instance813(813);
            return instance813;
        case 814:
            static ConnectionsManager instance814(814);
            return instance814;
        case 815:
            static ConnectionsManager instance815(815);
            return instance815;
        case 816:
            static ConnectionsManager instance816(816);
            return instance816;
        case 817:
            static ConnectionsManager instance817(817);
            return instance817;
        case 818:
            static ConnectionsManager instance818(818);
            return instance818;
        case 819:
            static ConnectionsManager instance819(819);
            return instance819;
        case 820:
            static ConnectionsManager instance820(820);
            return instance820;
        case 821:
            static ConnectionsManager instance821(821);
            return instance821;
        case 822:
            static ConnectionsManager instance822(822);
            return instance822;
        case 823:
            static ConnectionsManager instance823(823);
            return instance823;
        case 824:
            static ConnectionsManager instance824(824);
            return instance824;
        case 825:
            static ConnectionsManager instance825(825);
            return instance825;
        case 826:
            static ConnectionsManager instance826(826);
            return instance826;
        case 827:
            static ConnectionsManager instance827(827);
            return instance827;
        case 828:
            static ConnectionsManager instance828(828);
            return instance828;
        case 829:
            static ConnectionsManager instance829(829);
            return instance829;
        case 830:
            static ConnectionsManager instance830(830);
            return instance830;
        case 831:
            static ConnectionsManager instance831(831);
            return instance831;
        case 832:
            static ConnectionsManager instance832(832);
            return instance832;
        case 833:
            static ConnectionsManager instance833(833);
            return instance833;
        case 834:
            static ConnectionsManager instance834(834);
            return instance834;
        case 835:
            static ConnectionsManager instance835(835);
            return instance835;
        case 836:
            static ConnectionsManager instance836(836);
            return instance836;
        case 837:
            static ConnectionsManager instance837(837);
            return instance837;
        case 838:
            static ConnectionsManager instance838(838);
            return instance838;
        case 839:
            static ConnectionsManager instance839(839);
            return instance839;
        case 840:
            static ConnectionsManager instance840(840);
            return instance840;
        case 841:
            static ConnectionsManager instance841(841);
            return instance841;
        case 842:
            static ConnectionsManager instance842(842);
            return instance842;
        case 843:
            static ConnectionsManager instance843(843);
            return instance843;
        case 844:
            static ConnectionsManager instance844(844);
            return instance844;
        case 845:
            static ConnectionsManager instance845(845);
            return instance845;
        case 846:
            static ConnectionsManager instance846(846);
            return instance846;
        case 847:
            static ConnectionsManager instance847(847);
            return instance847;
        case 848:
            static ConnectionsManager instance848(848);
            return instance848;
        case 849:
            static ConnectionsManager instance849(849);
            return instance849;
        case 850:
            static ConnectionsManager instance850(850);
            return instance850;
        case 851:
            static ConnectionsManager instance851(851);
            return instance851;
        case 852:
            static ConnectionsManager instance852(852);
            return instance852;
        case 853:
            static ConnectionsManager instance853(853);
            return instance853;
        case 854:
            static ConnectionsManager instance854(854);
            return instance854;
        case 855:
            static ConnectionsManager instance855(855);
            return instance855;
        case 856:
            static ConnectionsManager instance856(856);
            return instance856;
        case 857:
            static ConnectionsManager instance857(857);
            return instance857;
        case 858:
            static ConnectionsManager instance858(858);
            return instance858;
        case 859:
            static ConnectionsManager instance859(859);
            return instance859;
        case 860:
            static ConnectionsManager instance860(860);
            return instance860;
        case 861:
            static ConnectionsManager instance861(861);
            return instance861;
        case 862:
            static ConnectionsManager instance862(862);
            return instance862;
        case 863:
            static ConnectionsManager instance863(863);
            return instance863;
        case 864:
            static ConnectionsManager instance864(864);
            return instance864;
        case 865:
            static ConnectionsManager instance865(865);
            return instance865;
        case 866:
            static ConnectionsManager instance866(866);
            return instance866;
        case 867:
            static ConnectionsManager instance867(867);
            return instance867;
        case 868:
            static ConnectionsManager instance868(868);
            return instance868;
        case 869:
            static ConnectionsManager instance869(869);
            return instance869;
        case 870:
            static ConnectionsManager instance870(870);
            return instance870;
        case 871:
            static ConnectionsManager instance871(871);
            return instance871;
        case 872:
            static ConnectionsManager instance872(872);
            return instance872;
        case 873:
            static ConnectionsManager instance873(873);
            return instance873;
        case 874:
            static ConnectionsManager instance874(874);
            return instance874;
        case 875:
            static ConnectionsManager instance875(875);
            return instance875;
        case 876:
            static ConnectionsManager instance876(876);
            return instance876;
        case 877:
            static ConnectionsManager instance877(877);
            return instance877;
        case 878:
            static ConnectionsManager instance878(878);
            return instance878;
        case 879:
            static ConnectionsManager instance879(879);
            return instance879;
        case 880:
            static ConnectionsManager instance880(880);
            return instance880;
        case 881:
            static ConnectionsManager instance881(881);
            return instance881;
        case 882:
            static ConnectionsManager instance882(882);
            return instance882;
        case 883:
            static ConnectionsManager instance883(883);
            return instance883;
        case 884:
            static ConnectionsManager instance884(884);
            return instance884;
        case 885:
            static ConnectionsManager instance885(885);
            return instance885;
        case 886:
            static ConnectionsManager instance886(886);
            return instance886;
        case 887:
            static ConnectionsManager instance887(887);
            return instance887;
        case 888:
            static ConnectionsManager instance888(888);
            return instance888;
        case 889:
            static ConnectionsManager instance889(889);
            return instance889;
        case 890:
            static ConnectionsManager instance890(890);
            return instance890;
        case 891:
            static ConnectionsManager instance891(891);
            return instance891;
        case 892:
            static ConnectionsManager instance892(892);
            return instance892;
        case 893:
            static ConnectionsManager instance893(893);
            return instance893;
        case 894:
            static ConnectionsManager instance894(894);
            return instance894;
        case 895:
            static ConnectionsManager instance895(895);
            return instance895;
        case 896:
            static ConnectionsManager instance896(896);
            return instance896;
        case 897:
            static ConnectionsManager instance897(897);
            return instance897;
        case 898:
            static ConnectionsManager instance898(898);
            return instance898;
        case 899:
            static ConnectionsManager instance899(899);
            return instance899;
        case 900:
            static ConnectionsManager instance900(900);
            return instance900;
        case 901:
            static ConnectionsManager instance901(901);
            return instance901;
        case 902:
            static ConnectionsManager instance902(902);
            return instance902;
        case 903:
            static ConnectionsManager instance903(903);
            return instance903;
        case 904:
            static ConnectionsManager instance904(904);
            return instance904;
        case 905:
            static ConnectionsManager instance905(905);
            return instance905;
        case 906:
            static ConnectionsManager instance906(906);
            return instance906;
        case 907:
            static ConnectionsManager instance907(907);
            return instance907;
        case 908:
            static ConnectionsManager instance908(908);
            return instance908;
        case 909:
            static ConnectionsManager instance909(909);
            return instance909;
        case 910:
            static ConnectionsManager instance910(910);
            return instance910;
        case 911:
            static ConnectionsManager instance911(911);
            return instance911;
        case 912:
            static ConnectionsManager instance912(912);
            return instance912;
        case 913:
            static ConnectionsManager instance913(913);
            return instance913;
        case 914:
            static ConnectionsManager instance914(914);
            return instance914;
        case 915:
            static ConnectionsManager instance915(915);
            return instance915;
        case 916:
            static ConnectionsManager instance916(916);
            return instance916;
        case 917:
            static ConnectionsManager instance917(917);
            return instance917;
        case 918:
            static ConnectionsManager instance918(918);
            return instance918;
        case 919:
            static ConnectionsManager instance919(919);
            return instance919;
        case 920:
            static ConnectionsManager instance920(920);
            return instance920;
        case 921:
            static ConnectionsManager instance921(921);
            return instance921;
        case 922:
            static ConnectionsManager instance922(922);
            return instance922;
        case 923:
            static ConnectionsManager instance923(923);
            return instance923;
        case 924:
            static ConnectionsManager instance924(924);
            return instance924;
        case 925:
            static ConnectionsManager instance925(925);
            return instance925;
        case 926:
            static ConnectionsManager instance926(926);
            return instance926;
        case 927:
            static ConnectionsManager instance927(927);
            return instance927;
        case 928:
            static ConnectionsManager instance928(928);
            return instance928;
        case 929:
            static ConnectionsManager instance929(929);
            return instance929;
        case 930:
            static ConnectionsManager instance930(930);
            return instance930;
        case 931:
            static ConnectionsManager instance931(931);
            return instance931;
        case 932:
            static ConnectionsManager instance932(932);
            return instance932;
        case 933:
            static ConnectionsManager instance933(933);
            return instance933;
        case 934:
            static ConnectionsManager instance934(934);
            return instance934;
        case 935:
            static ConnectionsManager instance935(935);
            return instance935;
        case 936:
            static ConnectionsManager instance936(936);
            return instance936;
        case 937:
            static ConnectionsManager instance937(937);
            return instance937;
        case 938:
            static ConnectionsManager instance938(938);
            return instance938;
        case 939:
            static ConnectionsManager instance939(939);
            return instance939;
        case 940:
            static ConnectionsManager instance940(940);
            return instance940;
        case 941:
            static ConnectionsManager instance941(941);
            return instance941;
        case 942:
            static ConnectionsManager instance942(942);
            return instance942;
        case 943:
            static ConnectionsManager instance943(943);
            return instance943;
        case 944:
            static ConnectionsManager instance944(944);
            return instance944;
        case 945:
            static ConnectionsManager instance945(945);
            return instance945;
        case 946:
            static ConnectionsManager instance946(946);
            return instance946;
        case 947:
            static ConnectionsManager instance947(947);
            return instance947;
        case 948:
            static ConnectionsManager instance948(948);
            return instance948;
        case 949:
            static ConnectionsManager instance949(949);
            return instance949;
        case 950:
            static ConnectionsManager instance950(950);
            return instance950;
        case 951:
            static ConnectionsManager instance951(951);
            return instance951;
        case 952:
            static ConnectionsManager instance952(952);
            return instance952;
        case 953:
            static ConnectionsManager instance953(953);
            return instance953;
        case 954:
            static ConnectionsManager instance954(954);
            return instance954;
        case 955:
            static ConnectionsManager instance955(955);
            return instance955;
        case 956:
            static ConnectionsManager instance956(956);
            return instance956;
        case 957:
            static ConnectionsManager instance957(957);
            return instance957;
        case 958:
            static ConnectionsManager instance958(958);
            return instance958;
        case 959:
            static ConnectionsManager instance959(959);
            return instance959;
        case 960:
            static ConnectionsManager instance960(960);
            return instance960;
        case 961:
            static ConnectionsManager instance961(961);
            return instance961;
        case 962:
            static ConnectionsManager instance962(962);
            return instance962;
        case 963:
            static ConnectionsManager instance963(963);
            return instance963;
        case 964:
            static ConnectionsManager instance964(964);
            return instance964;
        case 965:
            static ConnectionsManager instance965(965);
            return instance965;
        case 966:
            static ConnectionsManager instance966(966);
            return instance966;
        case 967:
            static ConnectionsManager instance967(967);
            return instance967;
        default:
            static ConnectionsManager instance968(968);
            return instance968;
    }
}

int ConnectionsManager::callEvents(int64_t now) {
    if (!events.empty()) {
        for (std::list<EventObject *>::iterator iter = events.begin(); iter != events.end();) {
            EventObject *eventObject = (*iter);
            if (eventObject->time <= now) {
                iter = events.erase(iter);
                eventObject->onEvent(0);
            } else {
                int diff = (int) (eventObject->time - now);
                return diff > 1000 || diff < 0 ? 1000 : diff;
            }
        }
    }
    if (!networkPaused) {
        return 1000;
    }
    int32_t timeToPushPing = (int32_t) ((sendingPushPing ? 30000 : 60000 * 3) - llabs(now - lastPushPingTime));
    if (timeToPushPing <= 0) {
        return 1000;
    }
    if (LOGS_ENABLED) DEBUG_D("schedule next epoll wakeup in %d ms", timeToPushPing);
    return timeToPushPing;
}

void ConnectionsManager::checkPendingTasks() {
    while (true) {
        std::function<void()> task;
        pthread_mutex_lock(&mutex);
        if (pendingTasks.empty()) {
            pthread_mutex_unlock(&mutex);
            return;
        }
        task = pendingTasks.front();
        pendingTasks.pop();
        pthread_mutex_unlock(&mutex);
        task();
    }
}

void ConnectionsManager::select() {
    checkPendingTasks();
    int eventsCount = epoll_wait(epolFd, epollEvents, 128, callEvents(getCurrentTimeMonotonicMillis()));
    checkPendingTasks();
    int64_t now = getCurrentTimeMonotonicMillis();
    callEvents(now);
    for (int32_t a = 0; a < eventsCount; a++) {
        EventObject *eventObject = (EventObject *) epollEvents[a].data.ptr;
        eventObject->onEvent(epollEvents[a].events);
    }
    size_t count = activeConnections.size();
    for (uint32_t a = 0; a < count; a++) {
        activeConnections[a]->checkTimeout(now);
    }

    Datacenter *datacenter = getDatacenterWithId(currentDatacenterId);
    if (pushConnectionEnabled) {
        if ((sendingPushPing && llabs(now - lastPushPingTime) >= 30000) || llabs(now - lastPushPingTime) >= 60000 * 3 + 10000) {
            lastPushPingTime = 0;
            sendingPushPing = false;
            if (datacenter != nullptr) {
                Connection *connection = datacenter->getPushConnection(false);
                if (connection != nullptr) {
                    connection->suspendConnection();
                }
            }
            if (LOGS_ENABLED) DEBUG_D("push ping timeout");
        }
        if (llabs(now - lastPushPingTime) >= 60000 * 3) {
            if (LOGS_ENABLED) DEBUG_D("time for push ping");
            lastPushPingTime = now;
            if (datacenter != nullptr) {
                sendPing(datacenter, true);
            }
        }
    }

    if (lastPauseTime != 0 && llabs(now - lastPauseTime) >= nextSleepTimeout) {
        bool dontSleep = !requestingSaltsForDc.empty();
        if (!dontSleep) {
            for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                Request *request = iter->get();
                if (request->connectionType & ConnectionTypeDownload || request->connectionType & ConnectionTypeUpload) {
                    dontSleep = true;
                    break;
                }
            }
        }
        if (!dontSleep) {
            for (requestsIter iter = requestsQueue.begin(); iter != requestsQueue.end(); iter++) {
                Request *request = iter->get();
                if (request->connectionType & ConnectionTypeDownload || request->connectionType & ConnectionTypeUpload) {
                    dontSleep = true;
                    break;
                }
            }
        }
        if (!dontSleep) {
            if (!networkPaused) {
                if (LOGS_ENABLED) DEBUG_D("pausing network and timers by sleep time = %d", nextSleepTimeout);
                for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
                    iter->second->suspendConnections(false);
                }
            }
            networkPaused = true;
            return;
        } else {
            lastPauseTime = now;
            if (LOGS_ENABLED) DEBUG_D("don't sleep because of salt, upload or download request");
        }
    }
    if (networkPaused) {
        networkPaused = false;
        if (LOGS_ENABLED) DEBUG_D("resume network and timers");
    }

    if (delegate != nullptr) {
        delegate->onUpdate(instanceNum);
    }
    if (datacenter != nullptr) {
        if (datacenter->hasAuthKey(ConnectionTypeGeneric, 1)) {
            if (llabs(now - lastPingTime) >= 19000) {
                lastPingTime = now;
                sendPing(datacenter, false);
            }
            if (abs((int32_t) (now / 1000) - lastDcUpdateTime) >= DC_UPDATE_TIME) {
                updateDcSettings(0, false);
            }
            processRequestQueue(0, 0);
        } else if (!datacenter->isHandshakingAny()) {
            datacenter->beginHandshake(HandshakeTypeAll, true);
        }
    }
}

void ConnectionsManager::scheduleTask(std::function<void()> task) {
    pthread_mutex_lock(&mutex);
    pendingTasks.push(task);
    pthread_mutex_unlock(&mutex);
    wakeup();
}

void ConnectionsManager::scheduleEvent(EventObject *eventObject, uint32_t time) {
    eventObject->time = getCurrentTimeMonotonicMillis() + time;
    std::list<EventObject *>::iterator iter;
    for (iter = events.begin(); iter != events.end(); iter++) {
        if ((*iter)->time > eventObject->time) {
            break;
        }
    }
    events.insert(iter, eventObject);
}

void ConnectionsManager::removeEvent(EventObject *eventObject) {
    for (std::list<EventObject *>::iterator iter = events.begin(); iter != events.end(); iter++) {
        if (*iter == eventObject) {
            events.erase(iter);
            break;
        }
    }
}

void ConnectionsManager::wakeup() {
    if (pipeFd == nullptr) {
        eventfd_write(eventFd, 1);
    } else {
        char ch = 'x';
        write(pipeFd[1], &ch, 1);
    }
}

void *ConnectionsManager::ThreadProc(void *data) {
    if (LOGS_ENABLED) DEBUG_D("network thread started");
    ConnectionsManager *networkManager = (ConnectionsManager *) (data);
#ifdef ANDROID
    javaVm->AttachCurrentThread(&jniEnv[networkManager->instanceNum], NULL);
#endif
    if (networkManager->currentUserId != 0 && networkManager->pushConnectionEnabled) {
        Datacenter *datacenter = networkManager->getDatacenterWithId(networkManager->currentDatacenterId);
        if (datacenter != nullptr) {
            datacenter->createPushConnection()->setSessionId(networkManager->pushSessionId);
            networkManager->sendPing(datacenter, true);
        }
    }
    do {
        networkManager->select();
    } while (!done);
    return nullptr;
}

void ConnectionsManager::loadConfig() {
    if (config == nullptr) {
        config = new Config(instanceNum, "tgnet.dat");
    }
    NativeByteBuffer *buffer = config->readConfig();
    if (buffer != nullptr) {
        uint32_t version = buffer->readUint32(nullptr);
        if (LOGS_ENABLED) DEBUG_D("config version = %u", version);
        if (version <= configVersion) {
            testBackend = buffer->readBool(nullptr);
            if (version >= 3) {
                clientBlocked = buffer->readBool(nullptr);
            }
            if (version >= 4) {
                lastInitSystemLangcode = buffer->readString(nullptr);
            }
            if (buffer->readBool(nullptr)) {
                currentDatacenterId = buffer->readUint32(nullptr);
                timeDifference = buffer->readInt32(nullptr);
                lastDcUpdateTime = buffer->readInt32(nullptr);
                pushSessionId = buffer->readInt64(nullptr);
                if (version >= 2) {
                    registeredForInternalPush = buffer->readBool(nullptr);
                }

                if (LOGS_ENABLED) DEBUG_D("current dc id = %u, time difference = %d, registered for push = %d", currentDatacenterId, timeDifference, (int32_t) registeredForInternalPush);

                uint32_t count = buffer->readUint32(nullptr);
                for (uint32_t a = 0; a < count; a++) {
                    sessionsToDestroy.push_back(buffer->readInt64(nullptr));
                }

                count = buffer->readUint32(nullptr);
                for (uint32_t a = 0; a < count; a++) {
                    Datacenter *datacenter = new Datacenter(instanceNum, buffer);
                    datacenters[datacenter->getDatacenterId()] = datacenter;
                    if (LOGS_ENABLED) DEBUG_D("datacenter(%p) %u loaded (hasAuthKey = %d)", datacenter, datacenter->getDatacenterId(), (int) datacenter->hasPermanentAuthKey());
                }
            }
        }
        buffer->reuse();
    }

    if (currentDatacenterId != 0 && currentUserId) {
        Datacenter *datacenter = getDatacenterWithId(currentDatacenterId);
        if (datacenter == nullptr || !datacenter->hasPermanentAuthKey()) {
            if (datacenter != nullptr) {
                if (LOGS_ENABLED) DEBUG_D("reset authorization because of dc %d", currentDatacenterId);
            }
            currentDatacenterId = 0;
            datacenters.clear();
            scheduleTask([&] {
                if (delegate != nullptr) {
                    delegate->onLogout(instanceNum);
                }
            });
        }
    }

    initDatacenters();

    if ((datacenters.size() != 0 && currentDatacenterId == 0) || pushSessionId == 0) {
        if (pushSessionId == 0) {
            RAND_bytes((uint8_t *) &pushSessionId, 8);
        }
        if (currentDatacenterId == 0) {
            currentDatacenterId = 2;
        }
        saveConfig();
    }
    movingToDatacenterId = DEFAULT_DATACENTER_ID;
}

void ConnectionsManager::saveConfigInternal(NativeByteBuffer *buffer) {
    buffer->writeInt32(configVersion);
    buffer->writeBool(testBackend);
    buffer->writeBool(clientBlocked);
    buffer->writeString(lastInitSystemLangcode);
    Datacenter *currentDatacenter = getDatacenterWithId(currentDatacenterId);
    buffer->writeBool(currentDatacenter != nullptr);
    if (currentDatacenter != nullptr) {
        buffer->writeInt32(currentDatacenterId);
        buffer->writeInt32(timeDifference);
        buffer->writeInt32(lastDcUpdateTime);
        buffer->writeInt64(pushSessionId);
        buffer->writeBool(registeredForInternalPush);

        std::vector<int64_t> sessions;
        currentDatacenter->getSessions(sessions);

        uint32_t count = (uint32_t) sessions.size();
        buffer->writeInt32(count);
        for (uint32_t a = 0; a < count; a++) {
            buffer->writeInt64(sessions[a]);
        }
        count = (uint32_t) datacenters.size();
        buffer->writeInt32(count);
        for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
            iter->second->serializeToStream(buffer);
        }
    }
}

void ConnectionsManager::saveConfig() {
    if (config == nullptr) {
        config = new Config(instanceNum, "tgnet.dat");
    }
    sizeCalculator->clearCapacity();
    saveConfigInternal(sizeCalculator);
    NativeByteBuffer *buffer = BuffersStorage::getInstance().getFreeBuffer(sizeCalculator->capacity());
    saveConfigInternal(buffer);
    config->writeConfig(buffer);
    buffer->reuse();
}

inline NativeByteBuffer *decompressGZip(NativeByteBuffer *data) {
    int retCode;
    z_stream stream;

    memset(&stream, 0, sizeof(z_stream));
    stream.avail_in = data->limit();
    stream.next_in = data->bytes();

    retCode = inflateInit2(&stream, 15 + 32);
    if (retCode != Z_OK) {
        if (LOGS_ENABLED) DEBUG_E("can't decompress data");
        exit(1);
    }
    NativeByteBuffer *result = BuffersStorage::getInstance().getFreeBuffer(data->limit() * 4);
    stream.avail_out = result->capacity();
    stream.next_out = result->bytes();
    while (1) {
        retCode = inflate(&stream, Z_NO_FLUSH);
        if (retCode == Z_STREAM_END) {
            break;
        }
        if (retCode == Z_OK) {
            NativeByteBuffer *newResult = BuffersStorage::getInstance().getFreeBuffer(result->capacity() * 2);
            memcpy(newResult->bytes(), result->bytes(), result->capacity());
            stream.avail_out = newResult->capacity() - result->capacity();
            stream.next_out = newResult->bytes() + result->capacity();
            result->reuse();
            result = newResult;
        } else {
            if (LOGS_ENABLED) DEBUG_E("can't decompress data");
            exit(1);
        }
    }
    result->limit((uint32_t) stream.total_out);
    inflateEnd(&stream);
    return result;
}

inline NativeByteBuffer *compressGZip(NativeByteBuffer *buffer) {
    if (buffer == nullptr || buffer->limit() == 0) {
        return nullptr;
    }
    z_stream stream;
    int retCode;

    memset(&stream, 0, sizeof(z_stream));
    stream.avail_in = buffer->limit();
    stream.next_in = buffer->bytes();

    retCode = deflateInit2(&stream, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
    if (retCode != Z_OK) {
        if (LOGS_ENABLED) DEBUG_E("%s: deflateInit2() failed with error %i", __PRETTY_FUNCTION__, retCode);
        return nullptr;
    }

    NativeByteBuffer *result = BuffersStorage::getInstance().getFreeBuffer(buffer->limit());
    stream.avail_out = result->limit();
    stream.next_out = result->bytes();
    retCode = deflate(&stream, Z_FINISH);
    if ((retCode != Z_OK) && (retCode != Z_STREAM_END)) {
        if (LOGS_ENABLED) DEBUG_E("%s: deflate() failed with error %i", __PRETTY_FUNCTION__, retCode);
        deflateEnd(&stream);
        result->reuse();
        return nullptr;
    }
    if (retCode != Z_STREAM_END || stream.total_out >= buffer->limit() - 4) {
        deflateEnd(&stream);
        result->reuse();
        return nullptr;
    }
    result->limit((uint32_t) stream.total_out);
    deflateEnd(&stream);
    return result;
}

int64_t ConnectionsManager::getCurrentTimeMillis() {
    clock_gettime(CLOCK_REALTIME, &timeSpec);
    return (int64_t) timeSpec.tv_sec * 1000 + (int64_t) timeSpec.tv_nsec / 1000000;
}

int64_t ConnectionsManager::getCurrentTimeMonotonicMillis() {
    clock_gettime(CLOCK_MONOTONIC, &timeSpecMonotonic);
    return (int64_t) timeSpecMonotonic.tv_sec * 1000 + (int64_t) timeSpecMonotonic.tv_nsec / 1000000;
}

int32_t ConnectionsManager::getCurrentTime() {
    return (int32_t) (getCurrentTimeMillis() / 1000) + timeDifference;
}

bool ConnectionsManager::isTestBackend() {
    return testBackend;
}

int32_t ConnectionsManager::getTimeDifference() {
    return timeDifference;
}

int64_t ConnectionsManager::generateMessageId() {
    int64_t messageId = (int64_t) ((((double) getCurrentTimeMillis() + ((double) timeDifference) * 1000) * 4294967296.0) / 1000.0);
    if (messageId <= lastOutgoingMessageId) {
        messageId = lastOutgoingMessageId + 1;
    }
    while (messageId % 4 != 0) {
        messageId++;
    }
    lastOutgoingMessageId = messageId;
    return messageId;
}

bool ConnectionsManager::isNetworkAvailable() {
    return networkAvailable;
}

void ConnectionsManager::cleanUp(bool resetKeys) {
    scheduleTask([&, resetKeys] {
        for (requestsIter iter = requestsQueue.begin(); iter != requestsQueue.end();) {
            Request *request = iter->get();
            if (request->requestFlags & RequestFlagWithoutLogin) {
                iter++;
                continue;
            }
            if (request->onCompleteRequestCallback != nullptr) {
                TL_error *error = new TL_error();
                error->code = -1000;
                error->text = "";
                request->onComplete(nullptr, error, 0);
                delete error;
            }
            iter = requestsQueue.erase(iter);
        }
        for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end();) {
            Request *request = iter->get();
            if (request->requestFlags & RequestFlagWithoutLogin) {
                iter++;
                continue;
            }
            if (request->onCompleteRequestCallback != nullptr) {
                TL_error *error = new TL_error();
                error->code = -1000;
                error->text = "";
                request->onComplete(nullptr, error, 0);
                delete error;
            }
            iter = runningRequests.erase(iter);
        }
        quickAckIdToRequestIds.clear();

        for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
            if (resetKeys) {
                iter->second->clearAuthKey(HandshakeTypeAll);
            }
            iter->second->recreateSessions(HandshakeTypeAll);
            iter->second->authorized = false;
        }
        sessionsToDestroy.clear();
        currentUserId = 0;
        registeredForInternalPush = false;
        saveConfig();
    });
}

void ConnectionsManager::onConnectionClosed(Connection *connection, int reason) {
    Datacenter *datacenter = connection->getDatacenter();
    if ((connection->getConnectionType() == ConnectionTypeGeneric || connection->getConnectionType() == ConnectionTypeGenericMedia) && datacenter->isHandshakingAny()) {
        datacenter->onHandshakeConnectionClosed(connection);
    }
    if (connection->getConnectionType() == ConnectionTypeGeneric) {
        if (datacenter->getDatacenterId() == currentDatacenterId) {
            sendingPing = false;
            if (!connection->isSuspended() && proxyAddress.empty()) {
                if (reason == 2) {
                    disconnectTimeoutAmount += connection->getTimeout();
                } else {
                    disconnectTimeoutAmount += 4;
                }
                if (LOGS_ENABLED) DEBUG_D("increase disconnect timeout %d", disconnectTimeoutAmount);
                int32_t maxTimeout;
                if (clientBlocked) {
                    maxTimeout = 5;
                } else {
                    maxTimeout = 20;
                }
                if (disconnectTimeoutAmount >= maxTimeout) {
                    if (!connection->hasUsefullData()) {
                        if (LOGS_ENABLED) DEBUG_D("start requesting new address and port due to timeout reach");
                        requestingSecondAddress = 0;
                        delegate->onRequestNewServerIpAndPort(requestingSecondAddress, instanceNum);
                    } else {
                        if (LOGS_ENABLED) DEBUG_D("connection has usefull data, don't request anything");
                    }
                    disconnectTimeoutAmount = 0;
                }
            }

            if (networkAvailable) {
                if (proxyAddress.empty()) {
                    if (connectionState != ConnectionStateConnecting) {
                        connectionState = ConnectionStateConnecting;
                        if (delegate != nullptr) {
                            delegate->onConnectionStateChanged(connectionState, instanceNum);
                        }
                    }
                } else {
                    if (connectionState != ConnectionStateConnectingViaProxy) {
                        connectionState = ConnectionStateConnectingViaProxy;
                        if (delegate != nullptr) {
                            delegate->onConnectionStateChanged(connectionState, instanceNum);
                        }
                    }
                }
            } else {
                if (connectionState != ConnectionStateWaitingForNetwork) {
                    connectionState = ConnectionStateWaitingForNetwork;
                    if (delegate != nullptr) {
                        delegate->onConnectionStateChanged(connectionState, instanceNum);
                    }
                }
            }
        }
    } else if (connection->getConnectionType() == ConnectionTypePush) {
        if (LOGS_ENABLED) DEBUG_D("connection(%p) push connection closed", connection);
        sendingPushPing = false;
        lastPushPingTime = getCurrentTimeMonotonicMillis() - 60000 * 3 + 4000;
    } else if (connection->getConnectionType() == ConnectionTypeProxy) {
        scheduleTask([&, connection] {
            for (std::vector<std::unique_ptr<ProxyCheckInfo>>::iterator iter = proxyActiveChecks.begin(); iter != proxyActiveChecks.end(); iter++) {
                ProxyCheckInfo *proxyCheckInfo = iter->get();
                if (proxyCheckInfo->connectionNum == connection->getConnectionNum()) {
                    bool found = false;
                    for (requestsIter iter2 = runningRequests.begin(); iter2 != runningRequests.end(); iter2++) {
                        Request *request = iter2->get();
                        if (connection->getConnectionToken() == request->connectionToken && request->requestToken == proxyCheckInfo->requestToken && (request->connectionType & 0x0000ffff) == ConnectionTypeProxy) {
                            request->completed = true;
                            runningRequests.erase(iter2);
                            proxyCheckInfo->onRequestTime(-1);
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        proxyActiveChecks.erase(iter);
                        if (!proxyCheckQueue.empty()) {
                            proxyCheckInfo = proxyCheckQueue[0].release();
                            proxyCheckQueue.erase(proxyCheckQueue.begin());
                            checkProxyInternal(proxyCheckInfo);
                        }
                    }
                    break;
                }
            }
        });
    }
}

void ConnectionsManager::onConnectionConnected(Connection *connection) {
    Datacenter *datacenter = connection->getDatacenter();
    ConnectionType connectionType = connection->getConnectionType();
    if ((connectionType == ConnectionTypeGeneric || connectionType == ConnectionTypeGenericMedia) && datacenter->isHandshakingAny()) {
        datacenter->onHandshakeConnectionConnected(connection);
        return;
    }

    if (datacenter->hasAuthKey(connectionType, 1)) {
        if (connectionType == ConnectionTypePush) {
            sendingPushPing = false;
            lastPushPingTime = getCurrentTimeMonotonicMillis();
            sendPing(datacenter, true);
        } else {
            if (connectionType == ConnectionTypeGeneric && datacenter->getDatacenterId() == currentDatacenterId) {
                sendingPing = false;
            }
            if (networkPaused && lastPauseTime != 0) {
                lastPauseTime = getCurrentTimeMonotonicMillis();
            }
            processRequestQueue(connection->getConnectionType(), datacenter->getDatacenterId());
        }
    }
}

void ConnectionsManager::onConnectionQuickAckReceived(Connection *connection, int32_t ack) {
    std::map<int32_t, std::vector<int32_t>>::iterator iter = quickAckIdToRequestIds.find(ack);
    if (iter == quickAckIdToRequestIds.end()) {
        return;
    }
    for (requestsIter iter2 = runningRequests.begin(); iter2 != runningRequests.end(); iter2++) {
        Request *request = iter2->get();
        if (std::find(iter->second.begin(), iter->second.end(), request->requestToken) != iter->second.end()) {
            request->onQuickAck();
        }
    }
    quickAckIdToRequestIds.erase(iter);
}

void ConnectionsManager::onConnectionDataReceived(Connection *connection, NativeByteBuffer *data, uint32_t length) {
    bool error = false;
    if (length <= 24 + 32) {
        int32_t code = data->readInt32(&error);
        if (code == 0) {
            if (LOGS_ENABLED) DEBUG_D("mtproto noop");
        } else if (code == -1) {
            int32_t ackId = data->readInt32(&error);
            if (!error) {
                onConnectionQuickAckReceived(connection, ackId & (~(1 << 31)));
            }
        } else {
            Datacenter *datacenter = connection->getDatacenter();
            if (LOGS_ENABLED) DEBUG_W("mtproto error = %d", code);
            if (code == -444 && connection->getConnectionType() == ConnectionTypeGeneric && !proxyAddress.empty() && !proxySecret.empty()) {
                if (delegate != nullptr) {
                    delegate->onProxyError(instanceNum);
                }
            } else if (code == -404 && (datacenter->isCdnDatacenter || PFS_ENABLED)) {
                if (!datacenter->isHandshaking(connection->isMediaConnection)) {
                    datacenter->clearAuthKey(connection->isMediaConnection ? HandshakeTypeMediaTemp : HandshakeTypeTemp);
                    datacenter->beginHandshake(connection->isMediaConnection ? HandshakeTypeMediaTemp : HandshakeTypeTemp, true);
                    if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) reset auth key due to -404 error", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType());
                }
            } else {
                connection->reconnect();
            }
        }
        return;
    }
    uint32_t mark = data->position();

    int64_t keyId = data->readInt64(&error);

    if (error) {
        connection->reconnect();
        return;
    }

    Datacenter *datacenter = connection->getDatacenter();

    if (connectionState != ConnectionStateConnected && connection->getConnectionType() == ConnectionTypeGeneric && datacenter->getDatacenterId() == currentDatacenterId) {
        connectionState = ConnectionStateConnected;
        if (delegate != nullptr) {
            delegate->onConnectionStateChanged(connectionState, instanceNum);
        }
    }

    if (keyId == 0) {
        int64_t messageId = data->readInt64(&error);
        if (error) {
            connection->reconnect();
            return;
        }

        if (connection->isMessageIdProcessed(messageId)) {
            return;
        }

        uint32_t messageLength = data->readUint32(&error);
        if (error) {
            connection->reconnect();
            return;
        }

        if (!connection->allowsCustomPadding()) {
            if (messageLength != data->remaining()) {
                if (LOGS_ENABLED) DEBUG_E("connection(%p) received incorrect message length", connection);
                connection->reconnect();
                return;
            }
        }

        TLObject *request;
        if (datacenter->isHandshaking(connection->isMediaConnection)) {
            request = datacenter->getCurrentHandshakeRequest(connection->isMediaConnection);
        } else {
            request = getRequestWithMessageId(messageId);
        }

        deserializingDatacenter = datacenter;
        TLObject *object = TLdeserialize(request, messageLength, data);

        if (object != nullptr) {
            if (datacenter->isHandshaking(connection->isMediaConnection)) {
                datacenter->processHandshakeResponse(connection->isMediaConnection, object, messageId);
            } else {
                processServerResponse(object, messageId, 0, 0, connection, 0, 0);
                connection->addProcessedMessageId(messageId);
            }
            connection->setHasUsefullData();
            delete object;
        }
    } else {
        if (connection->allowsCustomPadding()) {
            uint32_t padding = (length - 24) % 16;
            if (padding != 0) {
                length -= padding;
            }
        }
        if (length < 24 + 32 || !connection->allowsCustomPadding() && (length - 24) % 16 != 0 || !datacenter->decryptServerResponse(keyId, data->bytes() + mark + 8, data->bytes() + mark + 24, length - 24, connection)) {
            if (LOGS_ENABLED) DEBUG_E("connection(%p) unable to decrypt server response", connection);
            connection->reconnect();
            return;
        }
        data->position(mark + 24);

        int64_t messageServerSalt = data->readInt64(&error);
        int64_t messageSessionId = data->readInt64(&error);

        if (messageSessionId != connection->getSessionId()) {
            if (LOGS_ENABLED) DEBUG_E("connection(%p) received invalid message session id (0x%" PRIx64 " instead of 0x%" PRIx64 ")", connection, (uint64_t) messageSessionId, (uint64_t) connection->getSessionId());
            return;
        }

        int64_t messageId = data->readInt64(&error);
        int32_t messageSeqNo = data->readInt32(&error);
        uint32_t messageLength = data->readUint32(&error);

        int32_t processedStatus = connection->isMessageIdProcessed(messageId);

        if (messageSeqNo % 2 != 0) {
            connection->addMessageToConfirm(messageId);
        }

        TLObject *object = nullptr;

        if (processedStatus != 1) {
            deserializingDatacenter = datacenter;
            object = TLdeserialize(nullptr, messageLength, data);
            if (processedStatus == 2) {
                if (object == nullptr) {
                    connection->recreateSession();
                    connection->reconnect();
                    return;
                } else {
                    delete object;
                    object = nullptr;
                }
            }
        }
        if (!processedStatus) {
            if (object != nullptr) {
                connection->setHasUsefullData();
                if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) received object %s", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), typeid(*object).name());
                processServerResponse(object, messageId, messageSeqNo, messageServerSalt, connection, 0, 0);
                connection->addProcessedMessageId(messageId);
                delete object;
                if (connection->getConnectionType() == ConnectionTypePush) {
                    std::vector<std::unique_ptr<NetworkMessage>> messages;
                    sendMessagesToConnectionWithConfirmation(messages, connection, false);
                }
            } else {
                if (delegate != nullptr) {
                    delegate->onUnparsedMessageReceived(0, data, connection->getConnectionType(), instanceNum);
                }
            }
        } else {
            std::vector<std::unique_ptr<NetworkMessage>> messages;
            sendMessagesToConnectionWithConfirmation(messages, connection, false);
        }
    }
}

bool ConnectionsManager::hasPendingRequestsForConnection(Connection *connection) {
    ConnectionType type = connection->getConnectionType();
    if (type == ConnectionTypeGeneric || type == ConnectionTypeTemp || type == ConnectionTypeGenericMedia) {
        Datacenter *datacenter = connection->getDatacenter();
        int8_t num = connection->getConnectionNum();
        uint32_t token = connection->getConnectionToken();
        if (type == ConnectionTypeGeneric) {
            if (sendingPing && type == ConnectionTypeGeneric && datacenter->getDatacenterId() == currentDatacenterId) {
                return true;
            } else if (datacenter->isHandshaking(false)) {
                return true;
            }
        } else if (type == ConnectionTypeGenericMedia) {
            if (datacenter->isHandshaking(true)) {
                return true;
            }
        }
        for (requestsIter iter2 = runningRequests.begin(); iter2 != runningRequests.end(); iter2++) {
            Request *request = iter2->get();
            uint8_t connectionNum = (uint8_t) (request->connectionType >> 16);
            ConnectionType connectionType = (ConnectionType) (request->connectionType & 0x0000ffff);
            if (connectionType == type && connectionNum == num || request->connectionToken == token) {
                return true;
            }
        }
        return false;
    }
    return true;
}

TLObject *ConnectionsManager::getRequestWithMessageId(int64_t messageId) {
    for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
        Request *request = iter->get();
        if (request->messageId == messageId) {
            return request->rawRequest;
        }
    }
    return nullptr;
}

TLObject *ConnectionsManager::TLdeserialize(TLObject *request, uint32_t bytes, NativeByteBuffer *data) {
    bool error = false;
    uint32_t position = data->position();
    uint32_t constructor = data->readUint32(&error);
    if (error) {
        data->position(position);
        return nullptr;
    }

    TLObject *object = TLClassStore::TLdeserialize(data, bytes, constructor, instanceNum, error);

    if (error) {
        if (object != nullptr) {
            delete object;
        }
        data->position(position);
        return nullptr;
    }

    if (object == nullptr) {
        if (request != nullptr) {
            TL_api_request *apiRequest = dynamic_cast<TL_api_request *>(request);
            if (apiRequest != nullptr) {
                object = apiRequest->deserializeResponse(data, bytes, error);
                if (LOGS_ENABLED) DEBUG_D("api request constructor 0x%x, don't parse", constructor);
            } else {
                object = request->deserializeResponse(data, constructor, instanceNum, error);
                if (object != nullptr && error) {
                    delete object;
                    object = nullptr;
                }
            }
        } else {
            if (LOGS_ENABLED) DEBUG_D("not found request to parse constructor 0x%x", constructor);
        }
    }
    if (object == nullptr) {
        data->position(position);
    }
    return object;
}

void ConnectionsManager::processServerResponse(TLObject *message, int64_t messageId, int32_t messageSeqNo, int64_t messageSalt, Connection *connection, int64_t innerMsgId, int64_t containerMessageId) {
    const std::type_info &typeInfo = typeid(*message);

    if (LOGS_ENABLED) DEBUG_D("process server response %p - %s", message, typeInfo.name());

    Datacenter *datacenter = connection->getDatacenter();

    if (typeInfo == typeid(TL_new_session_created)) {
        TL_new_session_created *response = (TL_new_session_created *) message;

        if (!connection->isSessionProcessed(response->unique_id)) {
            if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) new session created (first message id: 0x%" PRIx64 ", server salt: 0x%" PRIx64 ", unique id: 0x%" PRIx64 ")", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), (uint64_t) response->first_msg_id, (uint64_t) response->server_salt, (uint64_t) response->unique_id);

            std::unique_ptr<TL_future_salt> salt = std::unique_ptr<TL_future_salt>(new TL_future_salt());
            salt->valid_until = salt->valid_since = getCurrentTime();
            salt->valid_until += 30 * 60;
            salt->salt = response->server_salt;
            datacenter->addServerSalt(salt);

            for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                Request *request = iter->get();
                Datacenter *requestDatacenter = getDatacenterWithId(request->datacenterId);
                if (request->messageId < response->first_msg_id && request->connectionType & connection->getConnectionType() && requestDatacenter != nullptr && requestDatacenter->getDatacenterId() == datacenter->getDatacenterId()) {
                    if (LOGS_ENABLED) DEBUG_D("clear request %p - %s", request->rawRequest, typeid(*request->rawRequest).name());
                    request->clear(true);
                }
            }

            saveConfig();

            if (datacenter->getDatacenterId() == currentDatacenterId && currentUserId) {
                if (connection->getConnectionType() == ConnectionTypePush) {
                    registerForInternalPushUpdates();
                } else if (connection->getConnectionType() == ConnectionTypeGeneric) {
                    if (delegate != nullptr) {
                        delegate->onSessionCreated(instanceNum);
                    }
                }
            }
            connection->addProcessedSession(response->unique_id);
        }
    } else if (typeInfo == typeid(TL_msg_container)) {
        TL_msg_container *response = (TL_msg_container *) message;
        size_t count = response->messages.size();
        if (LOGS_ENABLED) DEBUG_D("received container with %d items", (int32_t) count);
        for (uint32_t a = 0; a < count; a++) {
            TL_message *innerMessage = response->messages[a].get();
            int64_t innerMessageId = innerMessage->msg_id;
            if (innerMessage->seqno % 2 != 0) {
                connection->addMessageToConfirm(innerMessageId);
            }
            int32_t processedStatus = connection->isMessageIdProcessed(innerMessageId);
            if (processedStatus == 2) {
                if (innerMessage->unparsedBody != nullptr) {
                    connection->recreateSession();
                    connection->reconnect();
                    return;
                }
                processedStatus = 0;
            }
            if (processedStatus) {
                if (LOGS_ENABLED) DEBUG_D("inner message %d id 0x%" PRIx64 " already processed", a, innerMessageId);
                continue;
            }
            if (innerMessage->unparsedBody != nullptr) {
                if (LOGS_ENABLED) DEBUG_D("inner message %d id 0x%" PRIx64 " is unparsed", a, innerMessageId);
                if (delegate != nullptr) {
                    delegate->onUnparsedMessageReceived(0, innerMessage->unparsedBody.get(), connection->getConnectionType(), instanceNum);
                }
            } else {
                if (LOGS_ENABLED) DEBUG_D("inner message %d id 0x%" PRIx64 " process", a, innerMessageId);
                processServerResponse(innerMessage->body.get(), 0, innerMessage->seqno, messageSalt, connection, innerMessageId, messageId);
            }
            connection->addProcessedMessageId(innerMessageId);
        }
    } else if (typeInfo == typeid(TL_pong)) {
        if (connection->getConnectionType() == ConnectionTypePush) {
            if (!registeredForInternalPush) {
                registerForInternalPushUpdates();
            }
            if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) received push ping", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType());
            sendingPushPing = false;
        } else {
            TL_pong *response = (TL_pong *) message;
            if (response->ping_id >= 2000000) {
                for (std::vector<std::unique_ptr<ProxyCheckInfo>>::iterator iter = proxyActiveChecks.begin(); iter != proxyActiveChecks.end(); iter++) {
                    ProxyCheckInfo *proxyCheckInfo = iter->get();
                    if (proxyCheckInfo->pingId == response->ping_id) {
                        for (requestsIter iter2 = runningRequests.begin(); iter2 != runningRequests.end(); iter2++) {
                            Request *request = iter2->get();
                            if (request->requestToken == proxyCheckInfo->requestToken) {
                                int64_t ping = llabs(getCurrentTimeMonotonicMillis() - request->startTimeMillis);
                                if (LOGS_ENABLED) DEBUG_D("got ping response for request %p, %" PRId64, request->rawRequest, ping);
                                request->completed = true;
                                proxyCheckInfo->onRequestTime(ping);
                                runningRequests.erase(iter2);
                                break;
                            }
                        }
                        proxyActiveChecks.erase(iter);

                        if (!proxyCheckQueue.empty()) {
                            proxyCheckInfo = proxyCheckQueue[0].release();
                            proxyCheckQueue.erase(proxyCheckQueue.begin());
                            checkProxyInternal(proxyCheckInfo);
                        }
                        break;
                    }
                }
            } else if (response->ping_id == lastPingId) {
                int32_t diff = (int32_t) (getCurrentTimeMonotonicMillis() / 1000) - pingTime;

                if (abs(diff) < 10) {
                    currentPingTime = (diff + currentPingTime) / 2;
                    if (messageId != 0) {
                        int64_t timeMessage = (int64_t) (messageId / 4294967296.0 * 1000);
                        timeDifference = (int32_t) ((timeMessage - getCurrentTimeMillis()) / 1000 - currentPingTime / 2);
                    }
                }
                sendingPing = false;
            }
        }
    } else if (typeInfo == typeid(TL_future_salts)) {
        TL_future_salts *response = (TL_future_salts *) message;
        int64_t requestMid = response->req_msg_id;
        for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
            Request *request = iter->get();
            if (request->respondsToMessageId(requestMid)) {
                request->onComplete(response, nullptr, connection->currentNetworkType);
                request->completed = true;
                runningRequests.erase(iter);
                break;
            }
        }
    } else if (dynamic_cast<DestroySessionRes *>(message)) {
        DestroySessionRes *response = (DestroySessionRes *) message;
        if (LOGS_ENABLED) DEBUG_D("destroyed session 0x%" PRIx64 " (%s)", (uint64_t) response->session_id, typeInfo == typeid(TL_destroy_session_ok) ? "ok" : "not found");
    } else if (typeInfo == typeid(TL_rpc_result)) {
        TL_rpc_result *response = (TL_rpc_result *) message;
        int64_t resultMid = response->req_msg_id;

        bool hasResult = response->result.get() != nullptr;
        bool ignoreResult = false;
        if (hasResult) {
            TLObject *object = response->result.get();
            if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) received rpc_result with %s", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), typeid(*object).name());
        }
        RpcError *error = hasResult ? dynamic_cast<RpcError *>(response->result.get()) : nullptr;
        if (error != nullptr) {
            if (LOGS_ENABLED) DEBUG_E("connection(%p, account%u, dc%u, type %d) rpc error %d: %s", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), error->error_code, error->error_message.c_str());
            if (error->error_code == 303) {
                uint32_t migrateToDatacenterId = DEFAULT_DATACENTER_ID;

                static std::vector<std::string> migrateErrors = {"NETWORK_MIGRATE_", "PHONE_MIGRATE_", "USER_MIGRATE_"};

                size_t count = migrateErrors.size();
                for (uint32_t a = 0; a < count; a++) {
                    std::string &possibleError = migrateErrors[a];
                    if (error->error_message.find(possibleError) != std::string::npos) {
                        std::string num = error->error_message.substr(possibleError.size(), error->error_message.size() - possibleError.size());
                        uint32_t val = (uint32_t) atoi(num.c_str());
                        migrateToDatacenterId = val;
                    }
                }

                if (migrateToDatacenterId != DEFAULT_DATACENTER_ID) {
                    ignoreResult = true;
                    moveToDatacenter(migrateToDatacenterId);
                }
            }
        }

        uint32_t retryRequestsFromDatacenter = DEFAULT_DATACENTER_ID - 1;
        uint32_t retryRequestsConnections = 0;

        if (!ignoreResult) {
            for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                Request *request = iter->get();
                if (!request->respondsToMessageId(resultMid)) {
                    continue;
                }
                if (LOGS_ENABLED) DEBUG_D("got response for request %p - %s", request->rawRequest, typeid(*request->rawRequest).name());
                bool discardResponse = false;
                bool isError = false;
                bool allowInitConnection = true;

                if (request->onCompleteRequestCallback != nullptr) {
                    TL_error *implicitError = nullptr;
                    NativeByteBuffer *unpacked_data = nullptr;
                    TLObject *result = response->result.get();
                    if (typeid(*result) == typeid(TL_gzip_packed)) {
                        TL_gzip_packed *innerResponse = (TL_gzip_packed *) result;
                        unpacked_data = decompressGZip(innerResponse->packed_data.get());
                        TLObject *object = TLdeserialize(request->rawRequest, unpacked_data->limit(), unpacked_data);
                        if (object != nullptr) {
                            response->result = std::unique_ptr<TLObject>(object);
                        } else {
                            response->result = std::unique_ptr<TLObject>(nullptr);
                        }
                    }

                    hasResult = response->result.get() != nullptr;
                    error = hasResult ? dynamic_cast<RpcError *>(response->result.get()) : nullptr;
                    TL_error *error2 = hasResult ? dynamic_cast<TL_error *>(response->result.get()) : nullptr;
                    if (error != nullptr) {
                        allowInitConnection = false;
                        static std::string authRestart = "AUTH_RESTART";
                        static std::string authKeyPermEmpty = "AUTH_KEY_PERM_EMPTY";
                        static std::string workerBusy = "WORKER_BUSY_TOO_LONG_RETRY";
                        bool processEvenFailed = error->error_code == 500 && error->error_message.find(authRestart) != std::string::npos;
                        bool isWorkerBusy = error->error_code == 500 && error->error_message.find(workerBusy) != std::string::npos;
                        if (LOGS_ENABLED) DEBUG_E("request %p rpc error %d: %s", request, error->error_code, error->error_message.c_str());

                        if (error->error_code == 401 && error->error_message.find(authKeyPermEmpty) != std::string::npos) {
                            discardResponse = true;
                            request->minStartTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000 + 1);
                            request->startTime = 0;

                            if (!datacenter->isHandshaking(connection->isMediaConnection)) {
                                datacenter->clearAuthKey(connection->isMediaConnection ? HandshakeTypeMediaTemp : HandshakeTypeTemp);
                                saveConfig();
                                datacenter->beginHandshake(connection->isMediaConnection ? HandshakeTypeMediaTemp : HandshakeTypeTemp, false);
                            }
                        } else if ((request->requestFlags & RequestFlagFailOnServerErrors) == 0 || processEvenFailed) {
                            if (error->error_code == 500 || error->error_code < 0) {
                                discardResponse = true;
                                if (isWorkerBusy) {
                                    request->minStartTime = 0;
                                } else {
                                    request->minStartTime = request->startTime + (request->serverFailureCount > 10 ? 10 : request->serverFailureCount);
                                }
                                request->serverFailureCount++;
                            } else if (error->error_code == 420) {
                                int32_t waitTime = 2;
                                static std::string floodWait = "FLOOD_WAIT_";
                                if (error->error_message.find(floodWait) != std::string::npos) {
                                    std::string num = error->error_message.substr(floodWait.size(), error->error_message.size() - floodWait.size());
                                    waitTime = atoi(num.c_str());
                                    if (waitTime <= 0) {
                                        waitTime = 2;
                                    }
                                }

                                discardResponse = true;
                                request->failedByFloodWait = waitTime;
                                request->startTime = 0;
                                request->startTimeMillis = 0;
                                request->minStartTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000 + waitTime);
                            } else if (error->error_code == 400) {
                                static std::string waitFailed = "MSG_WAIT_FAILED";
                                if (error->error_message.find(waitFailed) != std::string::npos) {
                                    discardResponse = true;
                                    request->minStartTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000 + 1);
                                    request->startTime = 0;
                                    request->startTimeMillis = 0;
                                }
                            }
                        }
                        if (!discardResponse) {
                            implicitError = new TL_error();
                            implicitError->code = error->error_code;
                            implicitError->text = error->error_message;
                        }
                    } else if (error2 == nullptr) {
                        if (request->rawRequest == nullptr || response->result == nullptr) {
                            allowInitConnection = false;
                            if (LOGS_ENABLED) DEBUG_E("rawRequest is null");
                            implicitError = new TL_error();
                            implicitError->code = -1000;
                            implicitError->text = "";
                        }
                    }

                    if (!discardResponse) {
                        if (implicitError != nullptr || error2 != nullptr) {
                            isError = true;
                            request->onComplete(nullptr, implicitError != nullptr ? implicitError : error2, connection->currentNetworkType);
                            if (error2 != nullptr) {
                                delete error2;
                            }
                        } else {
                            request->onComplete(response->result.get(), nullptr, connection->currentNetworkType);
                        }
                    }

                    if (implicitError != nullptr) {
                        if (implicitError->code == 401) {
                            allowInitConnection = false;
                            isError = true;
                            static std::string sessionPasswordNeeded = "SESSION_PASSWORD_NEEDED";

                            if (implicitError->text.find(sessionPasswordNeeded) != std::string::npos) {
                                //ignore this error
                            } else if (datacenter->getDatacenterId() == currentDatacenterId || datacenter->getDatacenterId() == movingToDatacenterId) {
                                if (request->connectionType & ConnectionTypeGeneric && currentUserId) {
                                    currentUserId = 0;
                                    if (delegate != nullptr) {
                                        delegate->onLogout(instanceNum);
                                    }
                                    cleanUp(false);
                                }
                            } else {
                                datacenter->authorized = false;
                                saveConfig();
                                discardResponse = true;
                                if (request->connectionType & ConnectionTypeDownload || request->connectionType & ConnectionTypeUpload) {
                                    retryRequestsFromDatacenter = datacenter->datacenterId;
                                    retryRequestsConnections = request->connectionType;
                                }
                            }
                        } else if (currentUserId == 0 && implicitError->code == 406) {
                            static std::string authKeyDuplicated = "AUTH_KEY_DUPLICATED";
                            if (implicitError->text.find(authKeyDuplicated) != std::string::npos) {
                                cleanUp(true);
                            }
                        }
                    }

                    if (unpacked_data != nullptr) {
                        unpacked_data->reuse();
                    }
                    if (implicitError != nullptr) {
                        delete implicitError;
                    }
                }

                if (!discardResponse) {
                    if (allowInitConnection && !isError) {
                        bool save = false;
                        if (request->isInitRequest && datacenter->lastInitVersion != currentVersion) {
                            datacenter->lastInitVersion = currentVersion;
                            save = true;
                        } else if (request->isInitMediaRequest && datacenter->lastInitMediaVersion != currentVersion) {
                            datacenter->lastInitMediaVersion = currentVersion;
                            save = true;
                        }
                        if (save) {
                            saveConfig();
                            if (LOGS_ENABLED) DEBUG_D("dc%d init connection completed", datacenter->getDatacenterId());
                        }
                    }
                    request->completed = true;
                    removeRequestFromGuid(request->requestToken);
                    runningRequests.erase(iter);
                } else {
                    request->messageId = 0;
                    request->messageSeqNo = 0;
                    request->connectionToken = 0;
                }
                break;
            }
        }

        if (retryRequestsFromDatacenter != DEFAULT_DATACENTER_ID - 1) {
            processRequestQueue(retryRequestsConnections, retryRequestsFromDatacenter);
        } else {
            processRequestQueue(0, 0);
        }
    } else if (typeInfo == typeid(TL_msgs_ack)) {

    } else if (typeInfo == typeid(TL_bad_msg_notification)) {
        TL_bad_msg_notification *result = (TL_bad_msg_notification *) message;
        if (LOGS_ENABLED) DEBUG_E("bad message notification %d for messageId 0x%" PRIx64 ", seqno %d", result->error_code, result->bad_msg_id, result->bad_msg_seqno);
        switch (result->error_code) {
            case 16:
            case 17:
            case 19:
            case 32:
            case 33:
            case 64: {
                int64_t realId = messageId != 0 ? messageId : containerMessageId;
                if (realId == 0) {
                    realId = innerMsgId;
                }

                if (realId != 0) {
                    int64_t time = (int64_t) (messageId / 4294967296.0 * 1000);
                    int64_t currentTime = getCurrentTimeMillis();
                    timeDifference = (int32_t) ((time - currentTime) / 1000 - currentPingTime / 2);
                }

                datacenter->recreateSessions(HandshakeTypeAll);
                saveConfig();

                lastOutgoingMessageId = 0;
                clearRequestsForDatacenter(datacenter, HandshakeTypeAll);
                break;
            }
            case 20: {
                for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                    Request *request = iter->get();
                    if (request->respondsToMessageId(result->bad_msg_id)) {
                        if (request->completed) {
                            break;
                        }
                        connection->addMessageToConfirm(result->bad_msg_id);
                        request->clear(true);
                        break;
                    }
                }
            }
            default:
                break;
        }
    } else if (typeInfo == typeid(TL_bad_server_salt)) {
        TL_bad_server_salt *response = (TL_bad_server_salt *) message;
        if (messageId != 0) {
            int64_t time = (int64_t) (messageId / 4294967296.0 * 1000);
            int64_t currentTime = getCurrentTimeMillis();
            timeDifference = (int32_t) ((time - currentTime) / 1000 - currentPingTime / 2);
            lastOutgoingMessageId = messageId > (lastOutgoingMessageId ? messageId : lastOutgoingMessageId);
        }
        int64_t resultMid = response->bad_msg_id;
        if (resultMid != 0) {
            for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                Request *request = iter->get();
                if ((request->connectionType & ConnectionTypeDownload) == 0) {
                    continue;
                }
                Datacenter *requestDatacenter = getDatacenterWithId(request->datacenterId);
                if (requestDatacenter != nullptr && requestDatacenter->getDatacenterId() == datacenter->getDatacenterId()) {
                    request->retryCount = 0;
                    request->failedBySalt = true;
                }
            }
        }

        datacenter->clearServerSalts();

        std::unique_ptr<TL_future_salt> salt = std::unique_ptr<TL_future_salt>(new TL_future_salt());
        salt->valid_until = salt->valid_since = getCurrentTime();
        salt->valid_until += 30 * 60;
        salt->salt = messageSalt;
        datacenter->addServerSalt(salt);
        saveConfig();

        requestSaltsForDatacenter(datacenter);
        if (datacenter->hasAuthKey(ConnectionTypeGeneric, 1)) {
            processRequestQueue(AllConnectionTypes, datacenter->getDatacenterId());
        }
    } else if (typeInfo == typeid(MsgsStateInfo)) {
        MsgsStateInfo *response = (MsgsStateInfo *) message;
        if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) got %s for messageId 0x%" PRIx64, connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), typeInfo.name(), response->req_msg_id);

        std::map<int64_t, int64_t>::iterator mIter = resendRequests.find(response->req_msg_id);
        if (mIter != resendRequests.end()) {
            if (LOGS_ENABLED) DEBUG_D("found resend for messageId 0x%" PRIx64, mIter->second);
            connection->addMessageToConfirm(mIter->second);
            for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                Request *request = iter->get();
                if (request->respondsToMessageId(mIter->second)) {
                    if (request->completed) {
                        break;
                    }
                    request->clear(true);
                    break;
                }
            }
            resendRequests.erase(mIter);
        }
    } else if (dynamic_cast<MsgDetailedInfo *>(message)) {
        MsgDetailedInfo *response = (MsgDetailedInfo *) message;

        bool requestResend = false;
        bool confirm = true;

        if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) got %s for messageId 0x%" PRIx64, connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), typeInfo.name(), response->msg_id);
        if (typeInfo == typeid(TL_msg_detailed_info)) {
            for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
                Request *request = iter->get();
                if (request->respondsToMessageId(response->msg_id)) {
                    if (request->completed) {
                        break;
                    }
                    if (LOGS_ENABLED) DEBUG_D("got TL_msg_detailed_info for rpc request %p - %s", request->rawRequest, typeid(*request->rawRequest).name());
                    int32_t currentTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000);
                    if (request->lastResendTime == 0 || abs(currentTime - request->lastResendTime) >= 60) {
                        request->lastResendTime = currentTime;
                        requestResend = true;
                    } else {
                        confirm = false;
                    }
                    break;
                }
            }
        } else {
            if (!connection->isMessageIdProcessed(messageId)) {
                requestResend = true;
            }
        }

        if (requestResend) {
            TL_msg_resend_req *request = new TL_msg_resend_req();
            request->msg_ids.push_back(response->answer_msg_id);
            NetworkMessage *networkMessage = new NetworkMessage();
            networkMessage->message = std::unique_ptr<TL_message>(new TL_message());
            networkMessage->message->msg_id = generateMessageId();
            networkMessage->message->bytes = request->getObjectSize();
            networkMessage->message->body = std::unique_ptr<TLObject>(request);
            networkMessage->message->seqno = connection->generateMessageSeqNo(false);
            resendRequests[networkMessage->message->msg_id] = response->answer_msg_id;

            std::vector<std::unique_ptr<NetworkMessage>> array;
            array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));

            sendMessagesToConnection(array, connection, false);
        } else if (confirm) {
            connection->addMessageToConfirm(response->answer_msg_id);
        }
    } else if (typeInfo == typeid(TL_gzip_packed)) {
        TL_gzip_packed *response = (TL_gzip_packed *) message;
        NativeByteBuffer *data = decompressGZip(response->packed_data.get());
        TLObject *object = TLdeserialize(getRequestWithMessageId(messageId), data->limit(), data);
        if (object != nullptr) {
            if (LOGS_ENABLED) DEBUG_D("connection(%p, account%u, dc%u, type %d) received object %s", connection, instanceNum, datacenter->getDatacenterId(), connection->getConnectionType(), typeid(*object).name());
            processServerResponse(object, messageId, messageSeqNo, messageSalt, connection, innerMsgId, containerMessageId);
            delete object;
        } else {
            if (delegate != nullptr) {
                delegate->onUnparsedMessageReceived(messageId, data, connection->getConnectionType(), instanceNum);
            }
        }
        data->reuse();
    } else if (typeInfo == typeid(TL_updatesTooLong)) {
        if (connection->connectionType == ConnectionTypePush) {
            if (networkPaused) {
                lastPauseTime = getCurrentTimeMonotonicMillis();
                if (LOGS_ENABLED) DEBUG_D("received internal push: wakeup network in background");
            } else if (lastPauseTime != 0) {
                lastPauseTime = getCurrentTimeMonotonicMillis();
                if (LOGS_ENABLED) DEBUG_D("received internal push: reset sleep timeout");
            } else {
                if (LOGS_ENABLED) DEBUG_D("received internal push");
            }
            if (delegate != nullptr) {
                delegate->onInternalPushReceived(instanceNum);
            }
        } else {
            if (delegate != nullptr) {
                NativeByteBuffer *data = BuffersStorage::getInstance().getFreeBuffer(message->getObjectSize());
                message->serializeToStream(data);
                data->position(0);
                delegate->onUnparsedMessageReceived(0, data, connection->getConnectionType(), instanceNum);
                data->reuse();
            }
        }
    }
}

void ConnectionsManager::sendPing(Datacenter *datacenter, bool usePushConnection) {
    if (usePushConnection && (currentUserId == 0 || !usePushConnection)) {
        return;
    }
    Connection *connection = nullptr;
    if (usePushConnection) {
        connection = datacenter->getPushConnection(true);
    } else {
        connection = datacenter->getGenericConnection(true, 0);
    }
    if (connection == nullptr || (!usePushConnection && connection->getConnectionToken() == 0)) {
        return;
    }
    TL_ping_delay_disconnect *request = new TL_ping_delay_disconnect();
    request->ping_id = ++lastPingId;
    if (usePushConnection) {
        request->disconnect_delay = 60 * 7;
    } else {
        request->disconnect_delay = 35;
        pingTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000);
    }

    NetworkMessage *networkMessage = new NetworkMessage();
    networkMessage->message = std::unique_ptr<TL_message>(new TL_message());
    networkMessage->message->msg_id = generateMessageId();
    networkMessage->message->bytes = request->getObjectSize();
    networkMessage->message->body = std::unique_ptr<TLObject>(request);
    networkMessage->message->seqno = connection->generateMessageSeqNo(false);

    std::vector<std::unique_ptr<NetworkMessage>> array;
    array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
    NativeByteBuffer *transportData = datacenter->createRequestsData(array, nullptr, connection, false);
    if (usePushConnection) {
        if (LOGS_ENABLED) DEBUG_D("dc%d send ping to push connection", datacenter->getDatacenterId());
        sendingPushPing = true;
    } else {
        sendingPing = true;
    }
    connection->sendData(transportData, false, true);
}

bool ConnectionsManager::isIpv6Enabled() {
    return ipv6Enabled;
}

void ConnectionsManager::initDatacenters() {
    Datacenter *datacenter;
    if (!testBackend) {
        if (datacenters.find(1) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 1);
            datacenter->addAddressAndPort("149.154.175.50", 443, 0, "");
            datacenter->addAddressAndPort("2001:b28:f23d:f001:0000:0000:0000:000a", 443, 1, "");
            datacenters[1] = datacenter;
        }

        if (datacenters.find(2) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 2);
            datacenter->addAddressAndPort("149.154.167.51", 443, 0, "");
            datacenter->addAddressAndPort("2001:67c:4e8:f002:0000:0000:0000:000a", 443, 1, "");
            datacenters[2] = datacenter;
        }

        if (datacenters.find(3) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 3);
            datacenter->addAddressAndPort("149.154.175.100", 443, 0, "");
            datacenter->addAddressAndPort("2001:b28:f23d:f003:0000:0000:0000:000a", 443, 1, "");
            datacenters[3] = datacenter;
        }

        if (datacenters.find(4) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 4);
            datacenter->addAddressAndPort("149.154.167.91", 443, 0, "");
            datacenter->addAddressAndPort("2001:67c:4e8:f004:0000:0000:0000:000a", 443, 1, "");
            datacenters[4] = datacenter;
        }

        if (datacenters.find(5) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 5);
            datacenter->addAddressAndPort("149.154.171.5", 443, 0, "");
            datacenter->addAddressAndPort("2001:b28:f23f:f005:0000:0000:0000:000a", 443, 1, "");
            datacenters[5] = datacenter;
        }
    } else {
        if (datacenters.find(1) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 1);
            datacenter->addAddressAndPort("149.154.175.40", 443, 0, "");
            datacenter->addAddressAndPort("2001:b28:f23d:f001:0000:0000:0000:000e", 443, 1, "");
            datacenters[1] = datacenter;
        }

        if (datacenters.find(2) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 2);
            datacenter->addAddressAndPort("149.154.167.40", 443, 0, "");
            datacenter->addAddressAndPort("2001:67c:4e8:f002:0000:0000:0000:000e", 443, 1, "");
            datacenters[2] = datacenter;
        }

        if (datacenters.find(3) == datacenters.end()) {
            datacenter = new Datacenter(instanceNum, 3);
            datacenter->addAddressAndPort("149.154.175.117", 443, 0, "");
            datacenter->addAddressAndPort("2001:b28:f23d:f003:0000:0000:0000:000e", 443, 1, "");
            datacenters[3] = datacenter;
        }
    }
}

void ConnectionsManager::attachConnection(ConnectionSocket *connection) {
    if (std::find(activeConnections.begin(), activeConnections.end(), connection) != activeConnections.end()) {
        return;
    }
    activeConnections.push_back(connection);
}

void ConnectionsManager::detachConnection(ConnectionSocket *connection) {
    std::vector<ConnectionSocket *>::iterator iter = std::find(activeConnections.begin(), activeConnections.end(), connection);
    if (iter != activeConnections.end()) {
        activeConnections.erase(iter);
    }
}

int32_t ConnectionsManager::sendRequestInternal(TLObject *object, onCompleteFunc onComplete, onQuickAckFunc onQuickAck, uint32_t flags, uint32_t datacenterId, ConnectionType connetionType, bool immediate) {
    if (!currentUserId && !(flags & RequestFlagWithoutLogin)) {
        if (LOGS_ENABLED) DEBUG_D("can't do request without login %s", typeid(*object).name());
        delete object;
        return 0;
    }
    Request *request = new Request(instanceNum, lastRequestToken++, connetionType, flags, datacenterId, onComplete, onQuickAck, nullptr);
    request->rawRequest = object;
    request->rpcRequest = wrapInLayer(object, getDatacenterWithId(datacenterId), request);
    requestsQueue.push_back(std::unique_ptr<Request>(request));
    if (immediate) {
        processRequestQueue(0, 0);
    }
    return request->requestToken;
}

int32_t ConnectionsManager::sendRequest(TLObject *object, onCompleteFunc onComplete, onQuickAckFunc onQuickAck, uint32_t flags, uint32_t datacenterId, ConnectionType connetionType, bool immediate) {
    int32_t requestToken = lastRequestToken++;
    return sendRequest(object, onComplete, onQuickAck, flags, datacenterId, connetionType, immediate, requestToken);
}

int32_t ConnectionsManager::sendRequest(TLObject *object, onCompleteFunc onComplete, onQuickAckFunc onQuickAck, uint32_t flags, uint32_t datacenterId, ConnectionType connetionType, bool immediate, int32_t requestToken) {
    if (!currentUserId && !(flags & RequestFlagWithoutLogin)) {
        if (LOGS_ENABLED) DEBUG_D("can't do request without login %s", typeid(*object).name());
        delete object;
        return 0;
    }
    if (requestToken == 0) {
        requestToken = lastRequestToken++;
    }
    scheduleTask([&, requestToken, object, onComplete, onQuickAck, flags, datacenterId, connetionType, immediate] {
        Request *request = new Request(instanceNum, requestToken, connetionType, flags, datacenterId, onComplete, onQuickAck, nullptr);
        request->rawRequest = object;
        request->rpcRequest = wrapInLayer(object, getDatacenterWithId(datacenterId), request);
        requestsQueue.push_back(std::unique_ptr<Request>(request));
        if (immediate) {
            processRequestQueue(0, 0);
        }
    });
    return requestToken;
}

#ifdef ANDROID
void ConnectionsManager::sendRequest(TLObject *object, onCompleteFunc onComplete, onQuickAckFunc onQuickAck, onWriteToSocketFunc onWriteToSocket, uint32_t flags, uint32_t datacenterId, ConnectionType connetionType, bool immediate, int32_t requestToken, jobject ptr1, jobject ptr2, jobject ptr3) {
    if (!currentUserId && !(flags & RequestFlagWithoutLogin)) {
        if (LOGS_ENABLED) DEBUG_D("can't do request without login %s", typeid(*object).name());
        delete object;
        JNIEnv *env = 0;
        if (javaVm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            if (LOGS_ENABLED) DEBUG_E("can't get jnienv");
            exit(1);
        }
        if (ptr1 != nullptr) {
            env->DeleteGlobalRef(ptr1);
            ptr1 = nullptr;
        }
        if (ptr2 != nullptr) {
            env->DeleteGlobalRef(ptr2);
            ptr2 = nullptr;
        }
        if (ptr3 != nullptr) {
            env->DeleteGlobalRef(ptr3);
            ptr3 = nullptr;
        }
        return;
    }
    scheduleTask([&, requestToken, object, onComplete, onQuickAck, onWriteToSocket, flags, datacenterId, connetionType, immediate, ptr1, ptr2, ptr3] {
        if (LOGS_ENABLED) DEBUG_D("send request %p - %s", object, typeid(*object).name());
        Request *request = new Request(instanceNum, requestToken, connetionType, flags, datacenterId, onComplete, onQuickAck, onWriteToSocket);
        request->rawRequest = object;
        request->ptr1 = ptr1;
        request->ptr2 = ptr2;
        request->ptr3 = ptr3;
        request->rpcRequest = wrapInLayer(object, getDatacenterWithId(datacenterId), request);
        if (LOGS_ENABLED) DEBUG_D("send request wrapped %p - %s", request->rpcRequest.get(), typeid(*(request->rpcRequest.get())).name());
        requestsQueue.push_back(std::unique_ptr<Request>(request));
        if (immediate) {
            processRequestQueue(0, 0);
        }
    });
}
#endif

void ConnectionsManager::cancelRequestsForGuid(int32_t guid) {
    scheduleTask([&, guid] {
        std::map<int32_t, std::vector<int32_t>>::iterator iter = requestsByGuids.find(guid);
        if (iter != requestsByGuids.end()) {
            std::vector<int32_t> &requests = iter->second;
            size_t count = requests.size();
            for (uint32_t a = 0; a < count; a++) {
                cancelRequestInternal(requests[a], 0, true, false);
                std::map<int32_t, int32_t>::iterator iter2 = guidsByRequests.find(requests[a]);
                if (iter2 != guidsByRequests.end()) {
                    guidsByRequests.erase(iter2);
                }
            }
            requestsByGuids.erase(iter);
        }
    });
}

void ConnectionsManager::bindRequestToGuid(int32_t requestToken, int32_t guid) {
    scheduleTask([&, requestToken, guid] {
        std::map<int32_t, std::vector<int32_t>>::iterator iter = requestsByGuids.find(guid);
        if (iter != requestsByGuids.end()) {
            iter->second.push_back(requestToken);
        } else {
            std::vector<int32_t> array;
            array.push_back(requestToken);
            requestsByGuids[guid] = array;
        }
        guidsByRequests[requestToken] = guid;
    });
}

void ConnectionsManager::setUserId(int32_t userId) {
    scheduleTask([&, userId] {
        int32_t oldUserId = currentUserId;
        currentUserId = userId;
        if (oldUserId == userId && userId != 0) {
            registerForInternalPushUpdates();
        }
        if (currentUserId != userId && userId != 0) {
            updateDcSettings(0, false);
        }
        if (currentUserId != 0 && pushConnectionEnabled) {
            Datacenter *datacenter = getDatacenterWithId(currentDatacenterId);
            if (datacenter != nullptr) {
                datacenter->createPushConnection()->setSessionId(pushSessionId);
                sendPing(datacenter, true);
            }
        }
    });
}

void ConnectionsManager::switchBackend() {
    scheduleTask([&] {
        currentDatacenterId = 1;
        testBackend = !testBackend;
        datacenters.clear();
        initDatacenters();
        saveConfig();
        exit(1);
    });
}

void ConnectionsManager::removeRequestFromGuid(int32_t requestToken) {
    std::map<int32_t, int32_t>::iterator iter2 = guidsByRequests.find(requestToken);
    if (iter2 != guidsByRequests.end()) {
        std::map<int32_t, std::vector<int32_t>>::iterator iter = requestsByGuids.find(iter2->first);
        if (iter != requestsByGuids.end()) {
            std::vector<int32_t>::iterator iter3 = std::find(iter->second.begin(), iter->second.end(), iter->first);
            if (iter3 != iter->second.end()) {
                iter->second.erase(iter3);
                if (iter->second.empty()) {
                    requestsByGuids.erase(iter);
                }
            }
        }
        guidsByRequests.erase(iter2);
    }
}

bool ConnectionsManager::cancelRequestInternal(int32_t token, int64_t messageId, bool notifyServer, bool removeFromClass) {
    for (requestsIter iter = requestsQueue.begin(); iter != requestsQueue.end(); iter++) {
        Request *request = iter->get();
        if (token != 0 && request->requestToken == token || messageId != 0 && request->respondsToMessageId(messageId)) {
            request->cancelled = true;
            if (LOGS_ENABLED) DEBUG_D("cancelled queued rpc request %p - %s", request->rawRequest, typeid(*request->rawRequest).name());
            requestsQueue.erase(iter);
            if (removeFromClass) {
                removeRequestFromGuid(token);
            }
            return true;
        }
    }

    for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
        Request *request = iter->get();
        if (token != 0 && request->requestToken == token || messageId != 0 && request->respondsToMessageId(messageId)) {
            if (notifyServer) {
                TL_rpc_drop_answer *dropAnswer = new TL_rpc_drop_answer();
                dropAnswer->req_msg_id = request->messageId;
                sendRequest(dropAnswer, nullptr, nullptr, RequestFlagEnableUnauthorized | RequestFlagWithoutLogin | RequestFlagFailOnServerErrors, request->datacenterId, request->connectionType, true);
            }
            request->cancelled = true;
            if (LOGS_ENABLED) DEBUG_D("cancelled running rpc request %p - %s", request->rawRequest, typeid(*request->rawRequest).name());
            runningRequests.erase(iter);
            if (removeFromClass) {
                removeRequestFromGuid(token);
            }
            return true;
        }
    }
    return false;
}

void ConnectionsManager::cancelRequest(int32_t token, bool notifyServer) {
    if (token == 0) {
        return;
    }
    scheduleTask([&, token, notifyServer] {
        cancelRequestInternal(token, 0, notifyServer, true);
    });
}

void ConnectionsManager::onDatacenterHandshakeComplete(Datacenter *datacenter, HandshakeType type, int32_t timeDiff) {
    saveConfig();
    uint32_t datacenterId = datacenter->getDatacenterId();
    if (datacenterId == currentDatacenterId || datacenterId == movingToDatacenterId) {
        timeDifference = timeDiff;
        datacenter->recreateSessions(type);
        clearRequestsForDatacenter(datacenter, type);
    }
    processRequestQueue(AllConnectionTypes, datacenterId);
    if (type == HandshakeTypeTemp && !proxyCheckQueue.empty()) {
        ProxyCheckInfo *proxyCheckInfo = proxyCheckQueue[0].release();
        proxyCheckQueue.erase(proxyCheckQueue.begin());
        checkProxyInternal(proxyCheckInfo);
    }
}

void ConnectionsManager::onDatacenterExportAuthorizationComplete(Datacenter *datacenter) {
    saveConfig();
    scheduleTask([&, datacenter] {
        processRequestQueue(AllConnectionTypes, datacenter->getDatacenterId());
    });
}

void ConnectionsManager::sendMessagesToConnection(std::vector<std::unique_ptr<NetworkMessage>> &messages, Connection *connection, bool reportAck) {
    if (messages.empty() || connection == nullptr) {
        return;
    }

    std::vector<std::unique_ptr<NetworkMessage>> currentMessages;
    Datacenter *datacenter = connection->getDatacenter();

    uint32_t currentSize = 0;
    size_t count = messages.size();
    for (uint32_t a = 0; a < count; a++) {
        NetworkMessage *networkMessage = messages[a].get();
        currentMessages.push_back(std::move(messages[a]));
        currentSize += networkMessage->message->bytes;

        if (currentSize >= 3 * 1024 || a == count - 1) {
            int32_t quickAckId = 0;
            NativeByteBuffer *transportData = datacenter->createRequestsData(currentMessages, reportAck ? &quickAckId : nullptr, connection, false);

            if (transportData != nullptr) {
                if (reportAck && quickAckId != 0) {
                    std::vector<int32_t> requestIds;

                    size_t count2 = currentMessages.size();
                    for (uint32_t b = 0; b < count2; b++) {
                        NetworkMessage *message = currentMessages[b].get();
                        if (message->requestId != 0) {
                            requestIds.push_back(message->requestId);
                        }
                    }

                    if (!requestIds.empty()) {
                        std::map<int32_t, std::vector<int32_t>>::iterator iter = quickAckIdToRequestIds.find(quickAckId);
                        if (iter == quickAckIdToRequestIds.end()) {
                            quickAckIdToRequestIds[quickAckId] = requestIds;
                        } else {
                            iter->second.insert(iter->second.end(), requestIds.begin(), requestIds.end());
                        }
                    }
                }

                connection->sendData(transportData, reportAck, true);
            } else {
                if (LOGS_ENABLED) DEBUG_E("connection(%p) connection data is empty", connection);
            }

            currentSize = 0;
            currentMessages.clear();
        }
    }
}

void ConnectionsManager::sendMessagesToConnectionWithConfirmation(std::vector<std::unique_ptr<NetworkMessage>> &messages, Connection *connection, bool reportAck) {
    NetworkMessage *networkMessage = connection->generateConfirmationRequest();
    if (networkMessage != nullptr) {
        messages.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
    }
    sendMessagesToConnection(messages, connection, reportAck);
}

void ConnectionsManager::requestSaltsForDatacenter(Datacenter *datacenter) {
    if (std::find(requestingSaltsForDc.begin(), requestingSaltsForDc.end(), datacenter->getDatacenterId()) != requestingSaltsForDc.end()) {
        return;
    }
    requestingSaltsForDc.push_back(datacenter->getDatacenterId());
    TL_get_future_salts *request = new TL_get_future_salts();
    request->num = 32;
    sendRequest(request, [&, datacenter](TLObject *response, TL_error *error, int32_t networkType) {
        std::vector<uint32_t>::iterator iter = std::find(requestingSaltsForDc.begin(), requestingSaltsForDc.end(), datacenter->getDatacenterId());
        if (iter != requestingSaltsForDc.end()) {
            requestingSaltsForDc.erase(iter);
        }
        if (error == nullptr) {
            TL_future_salts *res = (TL_future_salts *) response;
            datacenter->mergeServerSalts(res->salts);
            saveConfig();
        }
    }, nullptr, RequestFlagWithoutLogin | RequestFlagEnableUnauthorized | RequestFlagUseUnboundKey, datacenter->getDatacenterId(), ConnectionTypeGeneric, true);
}

void ConnectionsManager::clearRequestsForDatacenter(Datacenter *datacenter, HandshakeType type) {
    for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end(); iter++) {
        Request *request = iter->get();
        Datacenter *requestDatacenter = getDatacenterWithId(request->datacenterId);
        if (requestDatacenter->getDatacenterId() != datacenter->getDatacenterId()) {
            continue;
        }
        if (type == HandshakeTypePerm || type == HandshakeTypeAll || type == HandshakeTypeMediaTemp && request->isMediaRequest() || type == HandshakeTypeTemp && !request->isMediaRequest()) {
            request->clear(true);
        }
    }
}

void ConnectionsManager::registerForInternalPushUpdates() {
    if (registeringForPush || !currentUserId) {
        return;
    }
    registeredForInternalPush = false;
    registeringForPush = true;
    TL_account_registerDevice *request = new TL_account_registerDevice();
    request->token_type = 7;
    request->token = to_string_uint64((uint64_t) pushSessionId);

    sendRequest(request, [&](TLObject *response, TL_error *error, int32_t networkType) {
        if (error == nullptr) {
            registeredForInternalPush = true;
            if (LOGS_ENABLED) DEBUG_D("registered for internal push");
        } else {
            registeredForInternalPush = false;
            if (LOGS_ENABLED) DEBUG_E("unable to registering for internal push");
        }
        saveConfig();
        registeringForPush = false;
    }, nullptr, 0, DEFAULT_DATACENTER_ID, ConnectionTypeGeneric, true);
}


inline void addMessageToDatacenter(uint32_t datacenterId, NetworkMessage *networkMessage, std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>> &messagesToDatacenters) {
    std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>>::iterator iter = messagesToDatacenters.find(datacenterId);
    if (iter == messagesToDatacenters.end()) {
        std::vector<std::unique_ptr<NetworkMessage>> &array = messagesToDatacenters[datacenterId] = std::vector<std::unique_ptr<NetworkMessage>>();
        array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
    } else {
        iter->second.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
    }
}

void ConnectionsManager::processRequestQueue(uint32_t connectionTypes, uint32_t dc) {
    genericMessagesToDatacenters.clear();
    genericMediaMessagesToDatacenters.clear();
    tempMessagesToDatacenters.clear();
    unknownDatacenterIds.clear();
    neededDatacenters.clear();
    unauthorizedDatacenters.clear();
    downloadRunningRequestCount.clear();

    int64_t currentTimeMillis = getCurrentTimeMonotonicMillis();
    int32_t currentTime = (int32_t) (currentTimeMillis / 1000);
    uint32_t genericRunningRequestCount = 0;
    uint32_t uploadRunningRequestCount = 0;

    for (requestsIter iter = runningRequests.begin(); iter != runningRequests.end();) {
        Request *request = iter->get();
        const std::type_info &typeInfo = typeid(*request->rawRequest);

        uint32_t datacenterId = request->datacenterId;
        if (datacenterId == DEFAULT_DATACENTER_ID) {
            if (movingToDatacenterId != DEFAULT_DATACENTER_ID) {
                iter++;
                continue;
            }
            datacenterId = currentDatacenterId;
        }

        switch (request->connectionType & 0x0000ffff) {
            case ConnectionTypeGeneric:
                genericRunningRequestCount++;
                break;
            case ConnectionTypeDownload: {
                uint32_t currentCount;
                std::map<uint32_t, uint32_t>::iterator dcIter = downloadRunningRequestCount.find(datacenterId);
                if (dcIter != downloadRunningRequestCount.end()) {
                    currentCount = dcIter->second;
                } else {
                    currentCount = 0;
                }
                downloadRunningRequestCount[datacenterId] = currentCount + 1;
                break;
            }
            case ConnectionTypeUpload:
                uploadRunningRequestCount++;
                break;
            default:
                break;
        }

        if (request->requestFlags & RequestFlagTryDifferentDc) {
            int32_t requestStartTime = request->startTime;
            int32_t timeout = 30;
            if (updatingDcSettings && dynamic_cast<TL_help_getConfig *>(request->rawRequest)) {
                requestStartTime = updatingDcStartTime;
                updatingDcStartTime = currentTime;
                timeout = 60;
            }
            if (request->startTime != 0 && abs(currentTime - requestStartTime) >= timeout) {
                if (LOGS_ENABLED) DEBUG_D("move %s to requestsQueue", typeid(*request->rawRequest).name());
                requestsQueue.push_back(std::move(*iter));
                iter = runningRequests.erase(iter);
                continue;
            }
        }
        int32_t canUseUnboundKey = 0;
        if ((request->requestFlags & RequestFlagUseUnboundKey) != 0) {
            canUseUnboundKey |= 1;
        }

        Datacenter *requestDatacenter = getDatacenterWithId(datacenterId);
        if (requestDatacenter == nullptr) {
            if (std::find(unknownDatacenterIds.begin(), unknownDatacenterIds.end(), datacenterId) == unknownDatacenterIds.end()) {
                unknownDatacenterIds.push_back(datacenterId);
            }
            iter++;
            continue;
        } else {
            if (requestDatacenter->isCdnDatacenter) {
                request->requestFlags |= RequestFlagEnableUnauthorized;
            }
            if (request->needInitRequest(requestDatacenter, currentVersion) && !request->hasInitFlag() && request->rawRequest->isNeedLayer()) {
                if (LOGS_ENABLED) DEBUG_D("move %p - %s to requestsQueue because of initConnection", request->rawRequest, typeid(*request->rawRequest).name());
                requestsQueue.push_back(std::move(*iter));
                iter = runningRequests.erase(iter);
                continue;
            }

            if (!requestDatacenter->hasAuthKey(request->connectionType, canUseUnboundKey)) {
                std::pair<Datacenter *, ConnectionType> pair = std::make_pair(requestDatacenter, request->connectionType);
                if (std::find(neededDatacenters.begin(), neededDatacenters.end(), pair) == neededDatacenters.end()) {
                    neededDatacenters.push_back(pair);
                }
                iter++;
                continue;
            } else if (!(request->requestFlags & RequestFlagEnableUnauthorized) && !requestDatacenter->authorized && request->datacenterId != DEFAULT_DATACENTER_ID && request->datacenterId != currentDatacenterId) {
                if (std::find(unauthorizedDatacenters.begin(), unauthorizedDatacenters.end(), requestDatacenter) == unauthorizedDatacenters.end()) {
                    unauthorizedDatacenters.push_back(requestDatacenter);
                }
                iter++;
                continue;
            }
        }

        Connection *connection = requestDatacenter->getConnectionByType(request->connectionType, true, canUseUnboundKey);
        int32_t maxTimeout = request->connectionType & ConnectionTypeGeneric ? 8 : 30;
        if (!networkAvailable || connection->getConnectionToken() == 0) {
            iter++;
            continue;
        }

        uint32_t requestConnectionType = request->connectionType & 0x0000ffff;

        bool forceThisRequest = (connectionTypes & requestConnectionType) && requestDatacenter->getDatacenterId() == dc;

        if (typeInfo == typeid(TL_get_future_salts) || typeInfo == typeid(TL_destroy_session)) {
            if (request->messageId != 0) {
                request->addRespondMessageId(request->messageId);
            }
            request->clear(false);
            forceThisRequest = false;
        }

        if (forceThisRequest || (abs(currentTime - request->startTime) > maxTimeout &&
                                 (currentTime >= request->minStartTime ||
                                  (request->failedByFloodWait != 0 && (request->minStartTime - currentTime) > request->failedByFloodWait) ||
                                  (request->failedByFloodWait == 0 && abs(currentTime - request->minStartTime) >= 60))
        )
                ) {
            if (!forceThisRequest && request->connectionToken > 0) {
                if ((request->connectionType & ConnectionTypeGeneric || request->connectionType & ConnectionTypeTemp) && request->connectionToken == connection->getConnectionToken()) {
                    if (LOGS_ENABLED) DEBUG_D("request token is valid, not retrying %s (%p)", typeInfo.name(), request->rawRequest);
                    iter++;
                    continue;
                } else {
                    if (connection->getConnectionToken() != 0 && request->connectionToken == connection->getConnectionToken()) {
                        if (LOGS_ENABLED) DEBUG_D("request download token is valid, not retrying %s (%p)", typeInfo.name(), request->rawRequest);
                        iter++;
                        continue;
                    }
                }
            }

            if (request->connectionToken != 0 && request->connectionToken != connection->getConnectionToken()) {
                request->lastResendTime = 0;
            }

            request->retryCount++;

            if (!request->failedBySalt) {
                if (request->connectionType & ConnectionTypeDownload) {
                    uint32_t retryMax = 10;
                    if (!(request->requestFlags & RequestFlagForceDownload)) {
                        if (request->failedByFloodWait) {
                            retryMax = 1;
                        } else {
                            retryMax = 6;
                        }
                    }
                    if (request->retryCount >= retryMax) {
                        if (LOGS_ENABLED) DEBUG_E("timed out %s", typeInfo.name());
                        TL_error *error = new TL_error();
                        error->code = -123;
                        error->text = "RETRY_LIMIT";
                        request->onComplete(nullptr, error, connection->currentNetworkType);
                        delete error;
                        iter = runningRequests.erase(iter);
                        continue;
                    }
                }
            } else {
                request->failedBySalt = false;
            }

            if (request->messageSeqNo == 0) {
                request->messageSeqNo = connection->generateMessageSeqNo((request->connectionType & ConnectionTypeProxy) == 0);
                request->messageId = generateMessageId();
                if (request->rawRequest->initFunc != nullptr) {
                    request->rawRequest->initFunc(request->messageId);
                }
            }
            request->startTime = currentTime;
            request->startTimeMillis = currentTimeMillis;

            NetworkMessage *networkMessage = new NetworkMessage();
            networkMessage->message = std::unique_ptr<TL_message>(new TL_message());
            networkMessage->message->msg_id = request->messageId;
            networkMessage->message->bytes = request->serializedLength;
            networkMessage->message->outgoingBody = request->getRpcRequest();
            networkMessage->message->seqno = request->messageSeqNo;
            networkMessage->requestId = request->requestToken;
            networkMessage->invokeAfter = (request->requestFlags & RequestFlagInvokeAfter) != 0;
            networkMessage->needQuickAck = (request->requestFlags & RequestFlagNeedQuickAck) != 0;

            request->connectionToken = connection->getConnectionToken();
            switch (requestConnectionType) {
                case ConnectionTypeGeneric:
                    addMessageToDatacenter(requestDatacenter->getDatacenterId(), networkMessage, genericMessagesToDatacenters);
                    break;
                case ConnectionTypeGenericMedia:
                    addMessageToDatacenter(requestDatacenter->getDatacenterId(), networkMessage, genericMediaMessagesToDatacenters);
                    break;
                case ConnectionTypeTemp:
                    addMessageToDatacenter(requestDatacenter->getDatacenterId(), networkMessage, tempMessagesToDatacenters);
                    break;
                case ConnectionTypeProxy: {
                    std::vector<std::unique_ptr<NetworkMessage>> array;
                    array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
                    sendMessagesToConnection(array, connection, false);
                    break;
                }
                case ConnectionTypeDownload:
                case ConnectionTypeUpload: {
                    std::vector<std::unique_ptr<NetworkMessage>> array;
                    array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
                    sendMessagesToConnectionWithConfirmation(array, connection, false);
                    request->onWriteToSocket();
                    break;
                }
                default:
                    delete networkMessage;
            }
        }
        iter++;
    }

    Connection *genericConnection = nullptr;
    Datacenter *defaultDatacenter = getDatacenterWithId(currentDatacenterId);
    if (defaultDatacenter != nullptr) {
        genericConnection = defaultDatacenter->getGenericConnection(true, 0);
        if (genericConnection != nullptr && !sessionsToDestroy.empty() && genericConnection->getConnectionToken() != 0) {
            std::vector<int64_t>::iterator iter = sessionsToDestroy.begin();

            sessionsToDestroy.erase(iter);

            if (abs(currentTime - lastDestroySessionRequestTime) > 2) {
                lastDestroySessionRequestTime = currentTime;
                TL_destroy_session *request = new TL_destroy_session();
                request->session_id = *iter;

                NetworkMessage *networkMessage = new NetworkMessage();
                networkMessage->message = std::unique_ptr<TL_message>(new TL_message());
                networkMessage->message->msg_id = generateMessageId();
                networkMessage->message->bytes = request->getObjectSize();
                networkMessage->message->body = std::unique_ptr<TLObject>(request);
                networkMessage->message->seqno = genericConnection->generateMessageSeqNo(false);
                addMessageToDatacenter(defaultDatacenter->getDatacenterId(), networkMessage, genericMessagesToDatacenters);
            }
        }
    }

    for (requestsIter iter = requestsQueue.begin(); iter != requestsQueue.end();) {
        Request *request = iter->get();
        if (request->cancelled) {
            iter = requestsQueue.erase(iter);
            continue;
        }

        uint32_t datacenterId = request->datacenterId;
        if (datacenterId == DEFAULT_DATACENTER_ID) {
            if (movingToDatacenterId != DEFAULT_DATACENTER_ID) {
                iter++;
                continue;
            }
            datacenterId = currentDatacenterId;
        }

        int32_t canUseUnboundKey = 0;
        if ((request->requestFlags & RequestFlagUseUnboundKey) != 0) {
            canUseUnboundKey |= 1;
        }

        if (request->requestFlags & RequestFlagTryDifferentDc) {
            int32_t requestStartTime = request->startTime;
            int32_t timeout = 30;
            if (updatingDcSettings && dynamic_cast<TL_help_getConfig *>(request->rawRequest)) {
                requestStartTime = updatingDcStartTime;
                timeout = 60;
            } else {
                request->startTime = 0;
                request->startTimeMillis = 0;
            }
            if (requestStartTime != 0 && abs(currentTime - requestStartTime) >= timeout) {
                std::vector<uint32_t> allDc;
                for (std::map<uint32_t, Datacenter *>::iterator iter2 = datacenters.begin(); iter2 != datacenters.end(); iter2++) {
                    if (iter2->first == datacenterId || iter2->second->isCdnDatacenter) {
                        continue;
                    }
                    allDc.push_back(iter2->first);
                }
                uint8_t index;
                RAND_bytes(&index, 1);
                datacenterId = allDc[index % allDc.size()];
                if (dynamic_cast<TL_help_getConfig *>(request->rawRequest)) {
                    updatingDcStartTime = currentTime;
                    request->datacenterId = datacenterId;
                } else {
                    currentDatacenterId = datacenterId;
                }
            }
        }

        Datacenter *requestDatacenter = getDatacenterWithId(datacenterId);
        if (requestDatacenter == nullptr) {
            if (std::find(unknownDatacenterIds.begin(), unknownDatacenterIds.end(), datacenterId) == unknownDatacenterIds.end()) {
                unknownDatacenterIds.push_back(datacenterId);
            }
            iter++;
            continue;
        } else {
            if (request->needInitRequest(requestDatacenter, currentVersion) && !request->hasInitFlag()) {
                request->rpcRequest.release();
                request->rpcRequest = wrapInLayer(request->rawRequest, requestDatacenter, request);
            }

            if (!requestDatacenter->hasAuthKey(request->connectionType, canUseUnboundKey)) {
                std::pair<Datacenter *, ConnectionType> pair = std::make_pair(requestDatacenter, request->connectionType);
                if (std::find(neededDatacenters.begin(), neededDatacenters.end(), pair) == neededDatacenters.end()) {
                    neededDatacenters.push_back(pair);
                }
                iter++;
                continue;
            } else if (!(request->requestFlags & RequestFlagEnableUnauthorized) && !requestDatacenter->authorized && request->datacenterId != DEFAULT_DATACENTER_ID && request->datacenterId != currentDatacenterId) {
                if (std::find(unauthorizedDatacenters.begin(), unauthorizedDatacenters.end(), requestDatacenter) == unauthorizedDatacenters.end()) {
                    unauthorizedDatacenters.push_back(requestDatacenter);
                }
                iter++;
                continue;
            }
        }

        Connection *connection = requestDatacenter->getConnectionByType(request->connectionType, true, canUseUnboundKey);

        if (request->connectionType & ConnectionTypeGeneric && connection->getConnectionToken() == 0) {
            iter++;
            continue;
        }

        switch (request->connectionType & 0x0000ffff) {
            case ConnectionTypeGeneric:
            case ConnectionTypeGenericMedia:
                if (!canUseUnboundKey && genericRunningRequestCount >= 60) {
                    iter++;
                    continue;
                }
                genericRunningRequestCount++;
                break;
            case ConnectionTypeDownload: {
                uint32_t currentCount;
                std::map<uint32_t, uint32_t>::iterator dcIter = downloadRunningRequestCount.find(datacenterId);
                if (dcIter != downloadRunningRequestCount.end()) {
                    currentCount = dcIter->second;
                } else {
                    currentCount = 0;
                }
                if (!networkAvailable || currentCount >= 6) {
                    iter++;
                    continue;
                }
                downloadRunningRequestCount[datacenterId] = currentCount + 1;
                break;
            }
            case ConnectionTypeProxy:
            case ConnectionTypeTemp:
                if (!networkAvailable) {
                    iter++;
                    continue;
                }
                break;
            case ConnectionTypeUpload:
                if (!networkAvailable || uploadRunningRequestCount >= 10) {
                    iter++;
                    continue;
                }
                uploadRunningRequestCount++;
                break;
            default:
                break;
        }

        request->messageId = generateMessageId();
        if (request->rawRequest->initFunc != nullptr) {
            request->rawRequest->initFunc(request->messageId);
        }

        uint32_t requestLength = request->rpcRequest->getObjectSize();
        if (request->requestFlags & RequestFlagCanCompress) {
            request->requestFlags &= ~RequestFlagCanCompress;
            NativeByteBuffer *original = BuffersStorage::getInstance().getFreeBuffer(requestLength);
            request->rpcRequest->serializeToStream(original);
            NativeByteBuffer *buffer = compressGZip(original);
            if (buffer != nullptr) {
                TL_gzip_packed *packed = new TL_gzip_packed();
                packed->originalRequest = std::move(request->rpcRequest);
                packed->packed_data_to_send = buffer;
                request->rpcRequest = std::unique_ptr<TLObject>(packed);
                requestLength = packed->getObjectSize();
            }
            original->reuse();
        }

        request->serializedLength = requestLength;
        request->messageSeqNo = connection->generateMessageSeqNo((request->connectionType & ConnectionTypeProxy) == 0);
        request->startTime = currentTime;
        request->startTimeMillis = currentTimeMillis;
        request->connectionToken = connection->getConnectionToken();

        NetworkMessage *networkMessage = new NetworkMessage();
        networkMessage->message = std::unique_ptr<TL_message>(new TL_message());
        networkMessage->message->msg_id = request->messageId;
        networkMessage->message->bytes = request->serializedLength;
        networkMessage->message->outgoingBody = request->getRpcRequest();
        networkMessage->message->seqno = request->messageSeqNo;
        networkMessage->requestId = request->requestToken;
        networkMessage->invokeAfter = (request->requestFlags & RequestFlagInvokeAfter) != 0;
        networkMessage->needQuickAck = (request->requestFlags & RequestFlagNeedQuickAck) != 0;

        runningRequests.push_back(std::move(*iter));

        switch (request->connectionType & 0x0000ffff) {
            case ConnectionTypeGeneric:
                addMessageToDatacenter(requestDatacenter->getDatacenterId(), networkMessage, genericMessagesToDatacenters);
                break;
            case ConnectionTypeGenericMedia:
                addMessageToDatacenter(requestDatacenter->getDatacenterId(), networkMessage, genericMediaMessagesToDatacenters);
                break;
            case ConnectionTypeTemp:
                addMessageToDatacenter(requestDatacenter->getDatacenterId(), networkMessage, tempMessagesToDatacenters);
                break;
            case ConnectionTypeProxy: {
                std::vector<std::unique_ptr<NetworkMessage>> array;
                array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
                sendMessagesToConnection(array, connection, false);
                break;
            }
            case ConnectionTypeDownload:
            case ConnectionTypeUpload: {
                std::vector<std::unique_ptr<NetworkMessage>> array;
                array.push_back(std::unique_ptr<NetworkMessage>(networkMessage));
                sendMessagesToConnectionWithConfirmation(array, connection, false);
                break;
            }
            default:
                delete networkMessage;
        }

        iter = requestsQueue.erase(iter);
    }

    for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
        Datacenter *datacenter = iter->second;
        std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>>::iterator iter2 = genericMessagesToDatacenters.find(datacenter->getDatacenterId());
        if (iter2 == genericMessagesToDatacenters.end()) {
            Connection *connection = datacenter->getGenericConnection(false, 1);
            if (connection != nullptr && connection->getConnectionToken() != 0 && connection->hasMessagesToConfirm()) {
                genericMessagesToDatacenters[datacenter->getDatacenterId()] = std::vector<std::unique_ptr<NetworkMessage>>();
            }
        }

        iter2 = genericMediaMessagesToDatacenters.find(datacenter->getDatacenterId());
        if (iter2 == genericMediaMessagesToDatacenters.end()) {
            Connection *connection = datacenter->getGenericMediaConnection(false, 1);
            if (connection != nullptr && connection->getConnectionToken() != 0 && connection->hasMessagesToConfirm()) {
                genericMediaMessagesToDatacenters[datacenter->getDatacenterId()] = std::vector<std::unique_ptr<NetworkMessage>>();
            }
        }

        iter2 = tempMessagesToDatacenters.find(datacenter->getDatacenterId());
        if (iter2 == tempMessagesToDatacenters.end()) {
            Connection *connection = datacenter->getTempConnection(false);
            if (connection != nullptr && connection->getConnectionToken() != 0 && connection->hasMessagesToConfirm()) {
                tempMessagesToDatacenters[datacenter->getDatacenterId()] = std::vector<std::unique_ptr<NetworkMessage>>();
            }
        }
    }

    for (std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>>::iterator iter = genericMessagesToDatacenters.begin(); iter != genericMessagesToDatacenters.end(); iter++) {
        Datacenter *datacenter = getDatacenterWithId(iter->first);
        if (datacenter != nullptr) {
            bool scannedPreviousRequests = false;
            int64_t lastSentMessageRpcId = 0;
            bool needQuickAck = false;
            std::vector<std::unique_ptr<NetworkMessage>> &array = iter->second;
            size_t count = array.size();
            for (uint32_t b = 0; b < count; b++) {
                NetworkMessage *networkMessage = array[b].get();
                if (networkMessage->needQuickAck) {
                    needQuickAck = true;
                }
                if (networkMessage->invokeAfter) {
                    if (!scannedPreviousRequests) {
                        scannedPreviousRequests = true;

                        std::vector<int64_t> currentRequests;
                        for (uint32_t a = 0; a < count; a++) {
                            NetworkMessage *currentNetworkMessage = array[a].get();
                            TL_message *currentMessage = currentNetworkMessage->message.get();
                            if (currentNetworkMessage->invokeAfter) {
                                currentRequests.push_back(currentMessage->msg_id);
                            }
                        }

                        int64_t maxRequestId = 0;
                        for (requestsIter iter2 = runningRequests.begin(); iter2 != runningRequests.end(); iter2++) {
                            Request *request = iter2->get();
                            if (request->requestFlags & RequestFlagInvokeAfter) {
                                if (request->messageId > maxRequestId && std::find(currentRequests.begin(), currentRequests.end(), request->messageId) == currentRequests.end()) {
                                    maxRequestId = request->messageId;
                                }
                            }
                        }

                        lastSentMessageRpcId = maxRequestId;
                    }

                    TL_message *message = networkMessage->message.get();

                    if (lastSentMessageRpcId != 0 && lastSentMessageRpcId != message->msg_id) {
                        TL_invokeAfterMsg *request = new TL_invokeAfterMsg();
                        request->msg_id = lastSentMessageRpcId;
                        if (message->outgoingBody != nullptr) {
                            if (LOGS_ENABLED) DEBUG_D("wrap outgoingBody(%p, %s) to TL_invokeAfterMsg", message->outgoingBody, typeid(*message->outgoingBody).name());
                            request->outgoingQuery = message->outgoingBody;
                            message->outgoingBody = nullptr;
                        } else {
                            if (LOGS_ENABLED) DEBUG_D("wrap body(%p, %s) to TL_invokeAfterMsg", message->body.get(), typeid(*(message->body.get())).name());
                            request->query = std::move(message->body);
                        }
                        message->body = std::unique_ptr<TLObject>(request);
                        message->bytes += 4 + 8;
                    }

                    lastSentMessageRpcId = message->msg_id;
                }
            }

            sendMessagesToConnectionWithConfirmation(array, datacenter->getGenericConnection(true, 1), needQuickAck);
        }
    }

    for (std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>>::iterator iter = tempMessagesToDatacenters.begin(); iter != tempMessagesToDatacenters.end(); iter++) {
        Datacenter *datacenter = getDatacenterWithId(iter->first);
        if (datacenter != nullptr) {
            std::vector<std::unique_ptr<NetworkMessage>> &array = iter->second;
            sendMessagesToConnectionWithConfirmation(array, datacenter->getTempConnection(true), false);
        }
    }

    for (std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>>::iterator iter = genericMediaMessagesToDatacenters.begin(); iter != genericMediaMessagesToDatacenters.end(); iter++) {
        Datacenter *datacenter = getDatacenterWithId(iter->first);
        if (datacenter != nullptr) {
            std::vector<std::unique_ptr<NetworkMessage>> &array = iter->second;
            sendMessagesToConnectionWithConfirmation(array, datacenter->getGenericMediaConnection(true, 1), false);
        }
    }

    if (connectionTypes == ConnectionTypeGeneric && dc == currentDatacenterId) {
        std::map<uint32_t, std::vector<std::unique_ptr<NetworkMessage>>>::iterator iter2 = genericMessagesToDatacenters.find(currentDatacenterId);
        if (iter2 == genericMessagesToDatacenters.end()) {
            sendPing(getDatacenterWithId(currentDatacenterId), false);
        }
    }

    if (!unknownDatacenterIds.empty()) {
        updateDcSettings(0, false);
    }

    size_t count = neededDatacenters.size();
    for (uint32_t a = 0; a < count; a++) {
        Datacenter *datacenter = neededDatacenters[a].first;
        bool media = Connection::isMediaConnectionType(neededDatacenters[a].second) && datacenter->hasMediaAddress();
        if (datacenter->getDatacenterId() != movingToDatacenterId && !datacenter->isHandshaking(media) && !datacenter->hasAuthKey(neededDatacenters[a].second, 1)) {
            datacenter->beginHandshake(media ? HandshakeTypeMediaTemp : HandshakeTypeTemp, true);
        }
    }

    if (currentUserId) {
        count = unauthorizedDatacenters.size();
        for (uint32_t a = 0; a < count; a++) {
            Datacenter *datacenter = unauthorizedDatacenters[a];
            uint32_t id = datacenter->getDatacenterId();
            if (id != currentDatacenterId && id != movingToDatacenterId && !datacenter->isExportingAuthorization()) {
                datacenter->exportAuthorization();
            }
        }
    }
}

Datacenter *ConnectionsManager::getDatacenterWithId(uint32_t datacenterId) {
    if (datacenterId == DEFAULT_DATACENTER_ID) {
        return datacenters[currentDatacenterId];
    }
    std::map<uint32_t, Datacenter *>::iterator iter = datacenters.find(datacenterId);
    return iter != datacenters.end() ? iter->second : nullptr;
}

std::unique_ptr<TLObject> ConnectionsManager::wrapInLayer(TLObject *object, Datacenter *datacenter, Request *baseRequest) {
    if (object->isNeedLayer()) {
        bool media = PFS_ENABLED && datacenter != nullptr && baseRequest->isMediaRequest() && datacenter->hasMediaAddress();
        if (datacenter == nullptr || baseRequest->needInitRequest(datacenter, currentVersion)) {
            if (datacenter != nullptr && datacenter->getDatacenterId() == currentDatacenterId) {
                registerForInternalPushUpdates();
            }
            if (media) {
                baseRequest->isInitMediaRequest = true;
            } else {
                baseRequest->isInitRequest = true;
            }
            initConnection *request = new initConnection();
            if (delegate != nullptr) {
                request->flags = delegate->getInitFlags(instanceNum);
            } else {
                request->flags = 0;
            }
            request->query = std::unique_ptr<TLObject>(object);
            request->api_id = currentApiId;
            request->app_version = currentAppVersion;
            request->lang_code = currentLangCode;
            request->system_lang_code = currentLangCode;
            request->lang_pack = "android";
            request->system_lang_code = currentSystemLangCode;
            if (!proxyAddress.empty() && !proxySecret.empty()) {
                request->flags |= 1;
                request->proxy = std::unique_ptr<TL_inputClientProxy>(new TL_inputClientProxy());
                request->proxy->address = proxyAddress;
                request->proxy->port = proxyPort;
            }

            if (datacenter == nullptr || datacenter->isCdnDatacenter) {
                request->device_model = "n/a";
                request->system_version = "n/a";
            } else {
                request->device_model = currentDeviceModel;
                request->system_version = currentSystemVersion;
            }
            if (request->lang_code.empty()) {
                request->lang_code = "en";
            }
            if (request->device_model.empty()) {
                request->device_model = "n/a";
            }
            if (request->app_version.empty()) {
                request->app_version = "n/a";
            }
            if (request->system_version.empty()) {
                request->system_version = "n/a";
            }
            invokeWithLayer *request2 = new invokeWithLayer();
            request2->layer = currentLayer;
            request2->query = std::unique_ptr<TLObject>(request);
            if (LOGS_ENABLED) DEBUG_D("wrap in layer %s", typeid(*object).name());
            return std::unique_ptr<TLObject>(request2);
        }
    }
    return std::unique_ptr<TLObject>(object);
}

inline std::string hexStr(unsigned char *data, uint32_t len) {
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
    std::string s(len * 2, ' ');
    for (uint32_t i = 0; i < len; ++i) {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}

void ConnectionsManager::updateDcSettings(uint32_t dcNum, bool workaround) {
    if (workaround) {
        if (updatingDcSettingsWorkaround) {
            return;
        }
        updatingDcSettingsWorkaround = true;
    } else {
        if (updatingDcSettings) {
            return;
        }
        updatingDcSettings = true;
        updatingDcStartTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000);
    }

    TL_help_getConfig *request = new TL_help_getConfig();
    sendRequest(request, [&, workaround](TLObject *response, TL_error *error, int32_t networkType) {
        if (!workaround && !updatingDcSettings || workaround && !updatingDcSettingsWorkaround) {
            return;
        }

        if (response != nullptr) {
            TL_config *config = (TL_config *) response;
            clientBlocked = (config->flags & 256) != 0;
            if (!workaround) {
                int32_t updateIn = config->expires - getCurrentTime();
                if (updateIn <= 0) {
                    updateIn = 120;
                }
                lastDcUpdateTime = (int32_t) (getCurrentTimeMonotonicMillis() / 1000) - DC_UPDATE_TIME + updateIn;
            }

            struct DatacenterInfo {
                std::vector<TcpAddress> addressesIpv4;
                std::vector<TcpAddress> addressesIpv6;
                std::vector<TcpAddress> addressesIpv4Download;
                std::vector<TcpAddress> addressesIpv6Download;
                bool isCdn = false;

                void addAddressAndPort(TL_dcOption *dcOption) {
                    std::vector<TcpAddress> *addresses;
                    if (!isCdn) {
                        isCdn = dcOption->cdn;
                    }
                    if (dcOption->media_only) {
                        if (dcOption->ipv6) {
                            addresses = &addressesIpv6Download;
                        } else {
                            addresses = &addressesIpv4Download;
                        }
                    } else {
                        if (dcOption->ipv6) {
                            addresses = &addressesIpv6;
                        } else {
                            addresses = &addressesIpv4;
                        }
                    }
                    for (std::vector<TcpAddress>::iterator iter = addresses->begin(); iter != addresses->end(); iter++) {
                        if (iter->address == dcOption->ip_address && iter->port == dcOption->port) {
                            return;
                        }
                    }
                    std::string secret;
                    if (dcOption->secret != nullptr) {
                        secret = hexStr(dcOption->secret->bytes, dcOption->secret->length);
                    }
                    if (LOGS_ENABLED) DEBUG_D("getConfig add %s:%d to dc%d, flags %d, has secret = %d[%d]", dcOption->ip_address.c_str(), dcOption->port, dcOption->id, dcOption->flags, dcOption->secret != nullptr ? 1 : 0, dcOption->secret != nullptr ? dcOption->secret->length : 0);
                    addresses->push_back(TcpAddress(dcOption->ip_address, dcOption->port, dcOption->flags, secret));
                }
            };

            std::map<uint32_t, std::unique_ptr<DatacenterInfo>> map;
            size_t count = config->dc_options.size();
            for (uint32_t a = 0; a < count; a++) {
                TL_dcOption *dcOption = config->dc_options[a].get();
                std::map<uint32_t, std::unique_ptr<DatacenterInfo>>::iterator iter = map.find((uint32_t) dcOption->id);
                DatacenterInfo *info;
                if (iter == map.end()) {
                    map[dcOption->id] = std::unique_ptr<DatacenterInfo>(info = new DatacenterInfo);
                } else {
                    info = iter->second.get();
                }
                info->addAddressAndPort(dcOption);
            }

            if (!map.empty()) {
                for (std::map<uint32_t, std::unique_ptr<DatacenterInfo>>::iterator iter = map.begin(); iter != map.end(); iter++) {
                    Datacenter *datacenter = getDatacenterWithId(iter->first);
                    DatacenterInfo *info = iter->second.get();
                    if (datacenter == nullptr) {
                        datacenter = new Datacenter(instanceNum, iter->first);
                        datacenters[iter->first] = datacenter;
                    }
                    datacenter->replaceAddresses(info->addressesIpv4, info->isCdn ? 8 : 0);
                    datacenter->replaceAddresses(info->addressesIpv6, info->isCdn ? 9 : 1);
                    datacenter->replaceAddresses(info->addressesIpv4Download, info->isCdn ? 10 : 2);
                    datacenter->replaceAddresses(info->addressesIpv6Download, info->isCdn ? 11 : 3);
                    if (iter->first == movingToDatacenterId) {
                        movingToDatacenterId = DEFAULT_DATACENTER_ID;
                        moveToDatacenter(iter->first);
                    }
                }
                saveConfig();
                scheduleTask([&] {
                    processRequestQueue(AllConnectionTypes, 0);
                });
            }
            if (delegate != nullptr) {
                delegate->onUpdateConfig(config, instanceNum);
            }
        }
        if (workaround) {
            updatingDcSettingsWorkaround = false;
        } else {
            updatingDcSettings = false;
        }
    }, nullptr, RequestFlagEnableUnauthorized | RequestFlagWithoutLogin | RequestFlagUseUnboundKey | (workaround ? 0 : RequestFlagTryDifferentDc), dcNum == 0 ? currentDatacenterId : dcNum, workaround ? ConnectionTypeTemp : ConnectionTypeGeneric, true);
}

void ConnectionsManager::moveToDatacenter(uint32_t datacenterId) {
    if (movingToDatacenterId == datacenterId) {
        return;
    }
    movingToDatacenterId = datacenterId;

    Datacenter *currentDatacenter = getDatacenterWithId(currentDatacenterId);
    clearRequestsForDatacenter(currentDatacenter, HandshakeTypeAll);

    if (currentUserId) {
        TL_auth_exportAuthorization *request = new TL_auth_exportAuthorization();
        request->dc_id = datacenterId;
        sendRequest(request, [&, datacenterId](TLObject *response, TL_error *error, int32_t networkType) {
            if (error == nullptr) {
                movingAuthorization = std::move(((TL_auth_exportedAuthorization *) response)->bytes);
                authorizeOnMovingDatacenter();
            } else {
                moveToDatacenter(datacenterId);
            }
        }, nullptr, RequestFlagWithoutLogin, DEFAULT_DATACENTER_ID, ConnectionTypeGeneric, true);
    } else {
        authorizeOnMovingDatacenter();
    }
}

void ConnectionsManager::authorizeOnMovingDatacenter() {
    Datacenter *datacenter = getDatacenterWithId(movingToDatacenterId);
    if (datacenter == nullptr) {
        updateDcSettings(0, false);
        return;
    }
    datacenter->recreateSessions(HandshakeTypeAll);
    clearRequestsForDatacenter(datacenter, HandshakeTypeAll);

    if (!datacenter->hasAuthKey(ConnectionTypeGeneric, 0) && !datacenter->isHandshakingAny()) {
        datacenter->clearServerSalts();
        datacenter->beginHandshake(HandshakeTypeAll, true);
    }

    if (movingAuthorization != nullptr) {
        TL_auth_importAuthorization *request = new TL_auth_importAuthorization();
        request->id = currentUserId;
        request->bytes = std::move(movingAuthorization);
        sendRequest(request, [&](TLObject *response, TL_error *error, int32_t networkType) {
            if (error == nullptr) {
                authorizedOnMovingDatacenter();
            } else {
                moveToDatacenter(movingToDatacenterId);
            }
        }, nullptr, RequestFlagWithoutLogin, datacenter->getDatacenterId(), ConnectionTypeGeneric, true);
    } else {
        authorizedOnMovingDatacenter();
    }
}

void ConnectionsManager::authorizedOnMovingDatacenter() {
    movingAuthorization.reset();
    currentDatacenterId = movingToDatacenterId;
    movingToDatacenterId = DEFAULT_DATACENTER_ID;
    saveConfig();
    scheduleTask([&] {
        processRequestQueue(0, 0);
    });
}

void ConnectionsManager::applyDatacenterAddress(uint32_t datacenterId, std::string ipAddress, uint32_t port) {
    scheduleTask([&, datacenterId, ipAddress, port] {
        Datacenter *datacenter = getDatacenterWithId(datacenterId);
        if (datacenter != nullptr) {
            std::vector<TcpAddress> addresses;
            addresses.push_back(TcpAddress(ipAddress, port, 0, ""));
            datacenter->suspendConnections(true);
            datacenter->replaceAddresses(addresses, 0);
            datacenter->resetAddressAndPortNum();
            saveConfig();
            if (datacenter->isHandshakingAny()) {
                datacenter->beginHandshake(HandshakeTypeCurrent, true);
            }
            updateDcSettings(datacenterId, false);
        }
    });
}

ConnectionState ConnectionsManager::getConnectionState() {
    return connectionState;
}

void ConnectionsManager::setDelegate(ConnectiosManagerDelegate *connectiosManagerDelegate) {
    delegate = connectiosManagerDelegate;
}

void ConnectionsManager::setPushConnectionEnabled(bool value) {
    pushConnectionEnabled = value;
    Datacenter *datacenter = getDatacenterWithId(currentDatacenterId);
    if (datacenter != nullptr) {
        if (!pushConnectionEnabled) {
            Connection *connection = datacenter->getPushConnection(false);
            if (connection != nullptr) {
                connection->suspendConnection();
            }
        } else {
            datacenter->createPushConnection()->setSessionId(pushSessionId);
            sendPing(datacenter, true);
        }
    }
}

inline bool checkPhoneByPrefixesRules(std::string phone, std::string rules) {
    if (rules.empty() || phone.empty()) {
        return true;
    }
    bool found = false;

    std::stringstream ss(rules);
    std::string prefix;
    while (std::getline(ss, prefix, ',')) {
        if (prefix == "") {
            found = true;
        } else if (prefix[0] == '+' && phone.find(prefix.substr(1)) == 0) {
            found = true;
        } else if (prefix[0] == '-' && phone.find(prefix.substr(1)) == 0) {
            return false;
        }
    }
    return found;
}

void ConnectionsManager::applyDnsConfig(NativeByteBuffer *buffer, std::string phone) {
    scheduleTask([&, buffer, phone] {
        if (LOGS_ENABLED) DEBUG_D("trying to decrypt config %d", requestingSecondAddress);
        TL_help_configSimple *config = Datacenter::decodeSimpleConfig(buffer);
        int currentDate = getCurrentTime();
        if (config != nullptr && config->date <= currentDate && currentDate <= config->expires) {
            for (std::vector<std::unique_ptr<TL_accessPointRule>>::iterator iter = config->rules.begin(); iter != config->rules.end(); iter++) {
                TL_accessPointRule *rule = iter->get();
                if (!checkPhoneByPrefixesRules(phone, rule->phone_prefix_rules)) {
                    continue;
                }
                Datacenter *datacenter = getDatacenterWithId(rule->dc_id);
                if (datacenter != nullptr) {
                    std::vector<TcpAddress> addresses;
                    for (std::vector<std::unique_ptr<IpPort>>::iterator iter2 = rule->ips.begin(); iter2 != rule->ips.end(); iter2++) {
                        IpPort *port = iter2->get();
                        const std::type_info &typeInfo = typeid(*port);
                        if (typeInfo == typeid(TL_ipPort)) {
                            TL_ipPort *ipPort = (TL_ipPort *) port;
                            addresses.push_back(TcpAddress(ipPort->ipv4, ipPort->port, 0, ""));
                            if (LOGS_ENABLED) DEBUG_D("got address %s and port %d for dc%d", ipPort->ipv4.c_str(), ipPort->port, rule->dc_id);
                        } else if (typeInfo == typeid(TL_ipPortSecret)) {
                            TL_ipPortSecret *ipPort = (TL_ipPortSecret *) port;
                            addresses.push_back(TcpAddress(ipPort->ipv4, ipPort->port, 0, hexStr(ipPort->secret->bytes, ipPort->secret->length)));
                            if (LOGS_ENABLED) DEBUG_D("got address %s and port %d for dc%d with secret", ipPort->ipv4.c_str(), ipPort->port, rule->dc_id);
                        }
                    }
                    if (!addresses.empty()) {
                        datacenter->replaceAddresses(addresses, TcpAddressFlagTemp);
                        Connection *connection = datacenter->getTempConnection(false);
                        if (connection != nullptr) {
                            connection->suspendConnection();
                        }
                        if (datacenter->isHandshakingAny()) {
                            datacenter->beginHandshake(HandshakeTypeCurrent, true);
                        }
                        updateDcSettings(rule->dc_id, true);
                    }
                } else {
                    if (LOGS_ENABLED) DEBUG_D("config datacenter %d not found", rule->dc_id);
                }
            }
            delete config;
        } else {
            if (config == nullptr) {
                if (LOGS_ENABLED) DEBUG_D("can't decrypt dns config");
            } else {
                delete config;
                if (LOGS_ENABLED) DEBUG_D("dns config not valid due to date or expire");
            }
            if (requestingSecondAddress == 0) {
                requestingSecondAddress = 1;
                delegate->onRequestNewServerIpAndPort(requestingSecondAddress, instanceNum);
            } else if (requestingSecondAddress == 1) {
                requestingSecondAddress = 2;
                delegate->onRequestNewServerIpAndPort(requestingSecondAddress, instanceNum);
            } else {
                requestingSecondAddress = 0;
            }
        }
        buffer->reuse();
    });
}

void ConnectionsManager::init(uint32_t version, int32_t layer, int32_t apiId, std::string deviceModel, std::string systemVersion, std::string appVersion, std::string langCode, std::string systemLangCode, std::string configPath, std::string logPath, int32_t userId, bool isPaused, bool enablePushConnection, bool hasNetwork, int32_t networkType) {
    currentVersion = version;
    currentLayer = layer;
    currentApiId = apiId;
    currentConfigPath = configPath;
    currentDeviceModel = deviceModel;
    currentSystemVersion = systemVersion;
    currentAppVersion = appVersion;
    currentLangCode = langCode;
    currentSystemLangCode = systemLangCode;
    currentUserId = userId;
    currentLogPath = logPath;
    pushConnectionEnabled = enablePushConnection;
    currentNetworkType = networkType;
    networkAvailable = hasNetwork;
    if (isPaused) {
        lastPauseTime = getCurrentTimeMonotonicMillis();
    }

    if (!currentConfigPath.empty() && currentConfigPath.find_last_of('/') != currentConfigPath.size() - 1) {
        currentConfigPath += "/";
    }

    if (!logPath.empty()) {
        LOGS_ENABLED = true;
        FileLog::getInstance().init(logPath);
    }

    loadConfig();

    bool needLoadConfig = false;
    if (systemLangCode.compare(lastInitSystemLangcode) != 0) {
        lastInitSystemLangcode = systemLangCode;
        for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
            iter->second->resetInitVersion();
        }
        needLoadConfig = true;
        saveConfig();
    }

    pthread_create(&networkThread, NULL, (ConnectionsManager::ThreadProc), this);

    if (needLoadConfig) {
        updateDcSettings(0, false);
    }
}

void ConnectionsManager::setProxySettings(std::string address, uint16_t port, std::string username, std::string password, std::string secret) {
    scheduleTask([&, address, port, username, password, secret] {
        bool secretChanged = proxySecret != secret;
        bool reconnect = proxyAddress != address || proxyPort != port || username != proxyUser || proxyPassword != password || secretChanged;
        proxyAddress = address;
        proxyPort = port;
        proxyUser = username;
        proxyPassword = password;
        proxySecret = secret;
        if (!proxyAddress.empty() && connectionState == ConnectionStateConnecting) {
            connectionState = ConnectionStateConnectingViaProxy;
            if (delegate != nullptr) {
                delegate->onConnectionStateChanged(connectionState, instanceNum);
            }
        } else if (proxyAddress.empty() && connectionState == ConnectionStateConnectingViaProxy) {
            connectionState = ConnectionStateConnecting;
            if (delegate != nullptr) {
                delegate->onConnectionStateChanged(connectionState, instanceNum);
            }
        }
        if (secretChanged) {
            Datacenter *datacenter = getDatacenterWithId(DEFAULT_DATACENTER_ID);
            if (datacenter != nullptr) {
                datacenter->resetInitVersion();
            }
        }
        if (reconnect) {
            for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
                iter->second->suspendConnections(true);
            }
            Datacenter *datacenter = getDatacenterWithId(DEFAULT_DATACENTER_ID);
            if (datacenter != nullptr && datacenter->isHandshakingAny()) {
                datacenter->beginHandshake(HandshakeTypeCurrent, true);
            }
            processRequestQueue(0, 0);
        }
    });
}

void ConnectionsManager::setLangCode(std::string langCode) {
    scheduleTask([&, langCode] {
        if (currentLangCode.compare(langCode) == 0) {
            return;
        }
        currentLangCode = langCode;
        for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
            iter->second->resetInitVersion();
        }
        saveConfig();
    });
}

void ConnectionsManager::setSystemLangCode(std::string langCode) {
    scheduleTask([&, langCode] {
        if (currentSystemLangCode.compare(langCode) == 0) {
            return;
        }
        lastInitSystemLangcode = currentSystemLangCode = langCode;
        for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
            iter->second->resetInitVersion();
        }
        saveConfig();
        updateDcSettings(0, false);
    });
}

void ConnectionsManager::resumeNetwork(bool partial) {
    scheduleTask([&, partial] {
        if (partial) {
            if (networkPaused) {
                lastPauseTime = getCurrentTimeMonotonicMillis();
                networkPaused = false;
                if (LOGS_ENABLED) DEBUG_D("wakeup network in background");
            } else if (lastPauseTime != 0) {
                lastPauseTime = getCurrentTimeMonotonicMillis();
                networkPaused = false;
                if (LOGS_ENABLED) DEBUG_D("reset sleep timeout");
            }
        } else {
            if (LOGS_ENABLED) DEBUG_D("wakeup network");
            lastPauseTime = 0;
            networkPaused = false;
        }
    });
}

void ConnectionsManager::pauseNetwork() {
    if (lastPauseTime != 0) {
        return;
    }
    lastPauseTime = getCurrentTimeMonotonicMillis();
}

void ConnectionsManager::setNetworkAvailable(bool value, int32_t type, bool slow) {
    scheduleTask([&, value, type] {
        networkAvailable = value;
        currentNetworkType = type;
        networkSlow = slow;
        if (!networkAvailable) {
            connectionState = ConnectionStateWaitingForNetwork;
        } else {
            for (std::map<uint32_t, Datacenter *>::iterator iter = datacenters.begin(); iter != datacenters.end(); iter++) {
                if (iter->second->isHandshaking(false)) {
                    iter->second->createGenericConnection()->connect();
                } else if (iter->second->isHandshaking(true)) {
                    iter->second->createGenericMediaConnection()->connect();
                }
            }
        }
        if (delegate != nullptr) {
            delegate->onConnectionStateChanged(connectionState, instanceNum);
        }
    });
}

void ConnectionsManager::setUseIpv6(bool value) {
    scheduleTask([&, value] {
        ipv6Enabled = value;
    });
}

void ConnectionsManager::setMtProtoVersion(int version) {
    mtProtoVersion = version;
}

int32_t ConnectionsManager::getMtProtoVersion() {
    return mtProtoVersion;
}

int64_t ConnectionsManager::checkProxy(std::string address, uint16_t port, std::string username, std::string password, std::string secret, onRequestTimeFunc requestTimeFunc, jobject ptr1) {
    ProxyCheckInfo *proxyCheckInfo = new ProxyCheckInfo();
    proxyCheckInfo->address = address;
    proxyCheckInfo->port = port;
    proxyCheckInfo->username = username;
    proxyCheckInfo->password = password;
    proxyCheckInfo->secret = secret;
    proxyCheckInfo->onRequestTime = requestTimeFunc;
    proxyCheckInfo->pingId = ++lastPingProxyId;
    proxyCheckInfo->instanceNum = instanceNum;
    proxyCheckInfo->ptr1 = ptr1;

    checkProxyInternal(proxyCheckInfo);

    return proxyCheckInfo->pingId;
}

void ConnectionsManager::checkProxyInternal(ProxyCheckInfo *proxyCheckInfo) {
    scheduleTask([&, proxyCheckInfo] {
        int32_t freeConnectionNum = -1;
        if (proxyActiveChecks.size() != PROXY_CONNECTIONS_COUNT) {
            for (int32_t a = 0; a < PROXY_CONNECTIONS_COUNT; a++) {
                bool found = false;
                for (std::vector<std::unique_ptr<ProxyCheckInfo>>::iterator iter = proxyActiveChecks.begin(); iter != proxyActiveChecks.end(); iter++) {
                    if (iter->get()->connectionNum == a) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    freeConnectionNum = a;
                    break;
                }
            }
        }
        if (freeConnectionNum == -1) {
            proxyCheckQueue.push_back(std::unique_ptr<ProxyCheckInfo>(proxyCheckInfo));
        } else {
            ConnectionType connectionType = (ConnectionType) (ConnectionTypeProxy | (freeConnectionNum << 16));
            Datacenter *datacenter = getDatacenterWithId(DEFAULT_DATACENTER_ID);
            Connection *connection = datacenter->getConnectionByType(connectionType, true, 1);
            if (connection != nullptr) {
                connection->setOverrideProxy(proxyCheckInfo->address, proxyCheckInfo->port, proxyCheckInfo->username, proxyCheckInfo->password, proxyCheckInfo->secret);
                connection->suspendConnection();
                proxyCheckInfo->connectionNum = freeConnectionNum;
                TL_ping *request = new TL_ping();
                request->ping_id = proxyCheckInfo->pingId;
                proxyCheckInfo->requestToken = sendRequest(request, nullptr, nullptr, RequestFlagEnableUnauthorized | RequestFlagWithoutLogin, DEFAULT_DATACENTER_ID, connectionType, true, 0);
                proxyActiveChecks.push_back(std::unique_ptr<ProxyCheckInfo>(proxyCheckInfo));
            } else if (PFS_ENABLED) {
                if (datacenter->isHandshaking(false)) {
                    datacenter->beginHandshake(HandshakeTypeTemp, false);
                }
                proxyCheckQueue.push_back(std::unique_ptr<ProxyCheckInfo>(proxyCheckInfo));
            }
        }
    });
}

#ifdef ANDROID
void ConnectionsManager::useJavaVM(JavaVM *vm, bool useJavaByteBuffers) {
    javaVm = vm;
    if (useJavaByteBuffers) {
        JNIEnv *env = 0;
        if (javaVm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
            if (LOGS_ENABLED) DEBUG_E("can't get jnienv");
            exit(1);
        }
        jclass_ByteBuffer = (jclass) env->NewGlobalRef(env->FindClass("java/nio/ByteBuffer"));
        if (jclass_ByteBuffer == 0) {
            if (LOGS_ENABLED) DEBUG_E("can't find java ByteBuffer class");
            exit(1);
        }
        jclass_ByteBuffer_allocateDirect = env->GetStaticMethodID(jclass_ByteBuffer, "allocateDirect", "(I)Ljava/nio/ByteBuffer;");
        if (jclass_ByteBuffer_allocateDirect == 0) {
            if (LOGS_ENABLED) DEBUG_E("can't find java ByteBuffer allocateDirect");
            exit(1);
        }
        if (LOGS_ENABLED) DEBUG_D("using java ByteBuffer");
    }
}
#endif
