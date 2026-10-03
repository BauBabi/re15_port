/*
 * RE1.5 Rebuilt — Irons' Tod in ROOM1150, Knall-Montage 1130/1040/1030, Schluss-Schnitt ROOM11C0
 * (Runde 35, Spur L, Punkte 2-4).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_irons_tod.h und
 * analysis/befunde_runde35/L_cut1150.md. Kein Asset-Patch: die RDTs bleiben byte-true, die Szenen
 * sind portseitig eingespielter Bytecode aus ORIGINAL-Opcodes (jede Zeile mit Vorbild), die
 * vorhandene VM fuehrt sie mit der Original-Zeitsemantik aus. Raumwechsel = Door_aot_set + Aot_on
 * (ROOM1240 sub02/03 Slot 0, ROOM1080 sub07..10).
 */
#include "re15_irons_tod.h"

#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_audio.h"
#include "re15_enemy.h"
#include "re15_msg.h"
#include "re15_scd.h"
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

#include "gen/knall_bank.inc"   /* k_knall_bank: Satz 0 Tuerknall (RE2 DOOR04 Door_exit), Satz 1 Knall ROOM1030 */

extern unsigned g_current_room_id;

/* ---- Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c). Vorbilder je Form:
 *   Set            `22 bb ii vv`                       ROOM1150 sub08 @0x01110
 *   Sleep          `09 0a nn nn`                       ROOM1150 sub08 @0x01162
 *   Message_on     `2b id 00 00`                       ROOM1150 sub08 @0x0117E (Maske 0 = Untertitel)
 *   Plc_motion     `3f ee cc 00`                       ROOM1150 sub08 @0x01182 (ee 0 = RBJ-Overlay,
 *                                                      ee 1 = PL00.EDD, re15_actor.h:867)
 *   Plc_flg        `43 00 80 00` / `43 00 04 00`       ROOM1150 sub08 @0x0118E / @0x011D6
 *   Work_set       `2e kk ii` + Nop                    ROOM1150 sub08 @0x0111E / @0x011BA
 *   Plc_dest       `40 00 mm ff xx xx zz zz`           ROOM1150 sub08 @0x01136 (Modus 4) / ROOM11C0
 *                                                      sub02 @0x01868 (Modus 5 RUN) / @0x0114A (Modus 9)
 *   Ankunfts-Poll  `11 00 08 00 02 00 12 04 21 05 20 00` ROOM1150 sub08 @0x0112A..@0x01132
 *   Cut_chg        `29 cc`                             ROOM1150 sub08 @0x01178
 *   Se_on          `36 bb id 00 ...`                   ROOM1030 sub08 @0x02776 (bb 2)
 *   Knall-Signal   `22 05 1c 01` / `22 05 1d 01`       Set(5,28) / Set(5,29): der Port-Takt spielt die Knall-Bank
 *                                                      (s. RE15_IT_SIG_KNALL_*, wie das Arm-Signal (5,12))
 *   Sce_em_set     `44 ss tt gg 00 00 p6 pp xx xx yy yy zz zz 00 00 rr rr 00 00` ROOM1140 sub00 @0x00BAA
 *   Save           `24 vv nn nn`                       ROOM1030 main00 @0x01DE2 (Gleichzeitig-Limit)
 *   Door_aot_set   `3b ss 02 31 00 00 <Rechteck 8> <x y z yaw> st rm ct 00..` ROOM1130 main00 @0x008CE
 *   Aot_on         `47 ss`                             ROOM1240 sub02 (Intro-Handoff), ROOM1080 sub07
 *   Plc_ret        `42` + Nop, Cut_auto `3c 01`        ROOM1150 sub08 @0x012E0 / @0x012E2             */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_MSG(id)         0x2b, (id), 0x00, 0x00
#define OP_MOTION(e,c)     0x3f, (e), (c), 0x00
#define OP_FLG_REV         0x43, 0x00, 0x80, 0x00
#define OP_FLG_LOOP        0x43, 0x00, 0x04, 0x00
#define OP_FLG_REV_HALB    0x43, 0x00, 0x90, 0x00   /* Plc_flg Unterop 0 (OR, @0x80041ffc) 0x80|0x10 */
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_DEST(m,f,x,z)   0x40, 0x00, (m), (f), LE16(x), LE16(z)
#define OP_WAIT32          0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, 0x20, 0x00
#define OP_CUT(c)          0x29, (c)
#define OP_KNALL(sig)      0x22, 0x05, (sig), 0x01
#define OP_EM(s,t,g,p6,pp,x,z,rot) 0x44, (s), (t), (g), 0x00, 0x00, (p6), (pp), LE16(x), LE16(0), LE16(z), 0x00, 0x00, LE16(rot), 0x00, 0x00
#define OP_DOOR(x,z,yaw,rm,ct) 0x3b, RE15_IT_TUER_SLOT, 0x02, 0x31, 0x00, 0x00, 0,0,0,0,0,0,0,0, \
                               LE16(x), LE16(0), LE16(z), LE16(yaw), 0x00, (rm), (ct), 0,0,0,0,0,0,0
#define OP_AOT_ON          0x47, RE15_IT_TUER_SLOT
#define OP_END             0x01, 0x00
#define DOOR_LEN           32
#define DOOR_OFF_ROOM      23
#define DOOR_OFF_CUT       24
#define DOOR_OFF_X         14
#define DOOR_OFF_Z         18
#define DOOR_OFF_YAW       20

/* ================================================================================================
 * PROGRAMM 0 — ROOM1150, die Szene. Bausteine mit Vorbild:
 *   "Sir!" + Arm: rec0 Clip 0 = Bibliotheks-Clip 15 ("Hey!"-Greifen nach vorn, Hand (544,-707,120),
 *     D_adaruf §3.5; Inhalts-Hash fe313d415aee = ROOM1050/11C0/11B0 Clip 15) vor + zurueck wie
 *     ROOM1090 sub03 @0x0265C/@0x02664 (das Muster, in dem die Gesten im Original laufen).
 *   Lauf zur Liege: Plc_dest Modus 5 (RUN, ROOM11C0 sub02 @0x01868), Wegpunkt im AUTO-Rechteck der
 *     ersten Szene (@0x00DEA: z -23800..-21800), dann Couchplatz @0x01136 (-20538,-25147).
 *   Drehen/Blick/Knien/Cut 7: sub08 @0x0114A/@0x0115E/@0x01166/@0x01170/@0x01178 woertlich.
 *   Kniende Leon-Gesten: Clip 10 vor/zurueck (@0x01182..@0x01192), Clip 12 (@0x01286).
 *   Irons-Gesten: Clip 4 vor/zurueck + Liege-Loop (@0x011BE..@0x011D6), Clip 6 (sub03 @0x00F24),
 *     Clip 5 = Arm ausgestreckt + Halt (@0x0125A/@0x0125E Sleep 120).
 *   Kopfschuetteln gesenkt: ROOM11B0 sub06 @0x0154E..@0x0156A (Modus 2 Pitch 300, Modus 4 Sweep 3).
 *   Aufstehen: @0x012C6/@0x012CA (Clip 11 entity 1 rueckwaerts) im Halbtakt (Bit 0x10, @0x80030670-80).
 *   Knall + Schnitt: Signal (5,28) -> Knall-Bank Satz 0 (Port-Takt), Door_aot_set + Aot_on (Tuersatz wird bei
 *     der Ausloesung auf 1130 oder 1040 gesetzt — NUTZER-VORGABE "wenn in 1140 alle tot: nichts").
 * Zeiten (Sleep) = PORT-WAHL im Takt der ersten Szene. */
static const uint8_t k_p_szene[] = {
    OP_SET(RE15_IT_BANK, RE15_IT_BIT_GESEHEN, 1),         /* Einmal-Riegel als ERSTES Opcode (ROOM11B0 sub06 @0x01478) */
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),                     /* Pad-Sperre + Letterbox (sub08 @0x01114/@0x01118) */
    OP_WORK(1, 0),
    OP_SLEEP(20),
    OP_MSG(22),                                            /* "Leon: Sir!" */
    OP_MOTION(0, 0), OP_SLEEP(20), OP_MOTION(0, 0), OP_FLG_REV, OP_SLEEP(20),
    OP_DEST(5, 0x20, -18000, -22500), OP_WAIT32,           /* rennt: Wegpunkt */
    OP_DEST(5, 0x20, RE15_IT_COUCH_X, RE15_IT_COUCH_Z), OP_WAIT32,   /* rennt: Liege */
    OP_DEST(9, 0x20, -21150, -26131), OP_WAIT32,           /* dreht sich zu Irons (@0x0114A) */
    0x34, 0x04, 0xdc, 0x05,                                /* Member_set(4,1500) @0x0115E */
    OP_SLEEP(10),
    0x41, 0x01, 0x10, 0xaa, 0x30, 0xfd, 0xed, 0x99, 0x60, 0x60,   /* Plc_neck -> Irons @0x01166 */
    OP_MOTION(1, 11), OP_SLEEP(40),                        /* kniet (PL00.EDD Clip 11, @0x01170) */
    OP_CUT(RE15_IT_CUT_1150), OP_SLEEP(20),                /* Cut 7 (@0x01178) */
    OP_MSG(23),                                            /* "Leon: Sir, the communication system..." */
    OP_MOTION(0, 10), OP_SLEEP(60), OP_MOTION(0, 10), OP_FLG_REV, OP_SLEEP(61),
    OP_MSG(24),                                            /* "Leon: I came to get you, come with me!" */
    OP_MOTION(0, 12), OP_SLEEP(40), OP_MOTION(0, 12), OP_FLG_REV, OP_SLEEP(40),
    OP_WORK(2, 0),                                         /* Irons (@0x011BA) */
    OP_MSG(25),                                            /* "Irons: Leon... I... I'm proud..." */
    OP_MOTION(0, 4), OP_SLEEP(60), OP_MOTION(0, 4), OP_FLG_REV, OP_SLEEP(60),
    OP_MOTION(0, 3), OP_FLG_LOOP, OP_SLEEP(10),
    OP_MSG(26),                                            /* "Irons: But... I... I'm not going to make it..." */
    OP_MOTION(0, 6), OP_SLEEP(60), OP_MOTION(0, 6), OP_FLG_REV, OP_SLEEP(60),
    OP_MOTION(0, 3), OP_FLG_LOOP, OP_SLEEP(10),
    OP_MSG(27),                                            /* "Irons: Please... one last favor..." */
    OP_MOTION(0, 5), OP_SLEEP(120),                        /* Arm ausgestreckt, Halt (@0x0125A/@0x0125E) */
    OP_MSG(28),                                            /* "Irons: Be a hero... Leon..." */
    OP_SLEEP(100),
    OP_SET(5, RE15_IT_SIG_ARM, 1),                         /* SIGNAL (5,12): Arm faellt (Port-Takt, Clip 2 ab Bild 72) */
    OP_SLEEP(30),
    OP_WORK(1, 0),
    OP_MSG(29),                                            /* "Leon: SIR, Sir?!" */
    OP_MOTION(0, 10), OP_SLEEP(40), OP_MOTION(0, 10), OP_FLG_REV, OP_SLEEP(40),
    0x41, 0x02, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x01, 0x00, 0x0a,   /* Kopf gesenkt (11B0 @0x0154E) */
    OP_SLEEP(30),
    /* Kopfschuetteln LANGSAM (NUTZER-VORGABE "schuettelt ... den Kopf langsam"): Form = 11B0 sub06 @0x0155C
     * `41 04 03 00 00 00 00 00 64 00` (Modus 4, 3 Schwuenge); die Gier-Rate (pc[8] -> +0x9e @0x80041f5c-6c,
     * Schritt je Bild @0x800375dc ff.) ist die langsamste ausgelieferte: 0x1e aus ROOM4001 @0x018A4
     * `41 04 02 00 00 00 00 00 1e 00` (Zensus aller 36 Modus-4-Saetze: 0x1e 2x, 0x3c 14x, 0x5e 6x, 0x60 1x,
     * 0x64 12x, 0x78 1x). Gemessen (RE15_NECK_TRACE): Rate 100 = 26 Bilder, Rate 30 = 86 Bilder. */
    0x41, 0x04, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x00,
    OP_SLEEP(110),
    0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x60,   /* Kopf frei (@0x012D2) */
    OP_SLEEP(20),
    /* steht LANGSAM auf (NUTZER-VORGABE "langsam"; Nachbesserung 1 M3, Dossier §9.2): Clip 11 rueckwaerts wie
     * @0x012C6/@0x012CA, dazu Bit 0x10 = HALBTAKT (Spieler-Kommando 4 @0x80030670-80, Kippbit 0x20 @0x800306c4;
     * RE2 Retail @0x80065d28, benutzt ROOM4100 `3f 00 14 10`) -> 25 Clip-Bilder in 50. Sleep = Original
     * @0x012CE 40 (fuer 25 Bilder) + 25 = 65. */
    OP_MOTION(1, 11), OP_FLG_REV_HALB, OP_SLEEP(65),
    OP_CUT(5),                                             /* Totale wie das Ende der ersten Szene (@0x012C4) */
    OP_SLEEP(60),                                          /* kurze Pause */
    OP_KNALL(RE15_IT_SIG_KNALL_TUER), OP_SLEEP(45),        /* der Knall (aus dem Buero gehoert) */
    OP_DOOR(RE15_IT_PARK_1130_X, RE15_IT_PARK_1130_Z, 0, 0x13, RE15_IT_CUT_1130),   /* -> 1130 (oder 1040, Weiche) */
    OP_AOT_ON,
    OP_END,
};

/* PROGRAMM 1 — ROOM1130 Cut 0: Tuerknall, die aus 1140 uebrig gebliebenen Zombies erscheinen.
 * Records = ROOM1140 sub00, STEHENDE Form des Else-Zweigs ((3,210)=1, @0x00C12..@0x00C62: grid 0x02 fuer
 * alle fuenf, p6 = 00/01/01/01/01) — der Erst-Zweig @0x00BAA.. (grid 0x86/0x88) sind die LIEGENDEN
 * Leichen, die erst aufstehen (gemessen: Lauf v1130, Haufen am Boden). Tot-Bits neu,
 * Standorte = PORT-WAHL im Flur vor der Briefing-Room-Tuer (Spawn 1140->1130 @ROOM1140 0x00A52
 * (-1300,-13950)), Blick zur Kamera (+z = Gierung 3072; Gierung 0 = +x, 1024 = -z, 2048 = -x: Vorwaerts-
 * punkt fx = x + cos, fz = z - sin, aot_common.c). */
#define EM_1130_LISTE \
    OP_EM(0, 0x16, 0x02, 0x00, 40, -1400, -14200, 3072), \
    OP_EM(1, 0x10, 0x02, 0x01, 41,  -900, -12900, 3072), \
    OP_EM(2, 0x10, 0x02, 0x01, 42, -2000, -12000, 3072), \
    OP_EM(3, 0x11, 0x02, 0x01, 43, -1300, -10800, 3072), \
    OP_EM(4, 0x11, 0x02, 0x01, 46,  -700, -11600, 3072)
static const uint8_t k_p_1130[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),
    OP_CUT(RE15_IT_CUT_1130), OP_SLEEP(15),
    OP_KNALL(RE15_IT_SIG_KNALL_TUER), OP_SLEEP(10),
    EM_1130_LISTE,
    OP_SLEEP(150),
    OP_DOOR(RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 0x04, RE15_IT_CUT_1040),
    OP_AOT_ON,
    OP_END,
};
static const uint8_t k_p_1130_nach[] = { EM_1130_LISTE, OP_END };   /* spaeteres Betreten */

/* PROGRAMM 2 — ROOM1040 Cut 1: Knall wie ROOM1030, Rolltor hoch (nur wenn (4,5)=0), die Zombies
 * (Raum-Records, Limit 5 @0x011F0) kommen durchs Tor zur Kamera. Tor-Fahrt = sub08 @0x019B2..@0x01A26
 * woertlich (Se_on 0x0c/0x0a, Speed_set y -40, For 20 / Cut_chg 1 / For 50 / Sca_id_set+Sca_floor_set 2/3 /
 * For 65, Se_on 0x0b, Sleep 10 — Nachbesserung 1 M6: vorher zu For 135 zusammengefasst, die Sca-Zellen erst
 * nach 135 statt nach 70 Bildern frei), Zustand (4,5)=1 + (4,4)=1 wie sub01 @0x0157A/@0x01586.
 * AUFSTELLUNG (Nachbesserung 1 M1/M2): die erschienenen Zombies werden in der Form von ROOM1030 sub00
 * @0x020C6..@0x0216E (Else-Zweig: For 20 ueber die Gegner-Plaetze, Member 6 != 0, Switch var5, je Fall Pos_set +
 * Member_set 4) HINTER dem Tor in Navigations-Zone 0 aufgestellt. GEMESSEN (Abnahme s1040/s1040offen, Dossier
 * §9.1): die Raum-Records @0x013B8.. stehen in Zone 5 (block.blk @0x0FF4, x < -27340, Nachbar nur Zone 3) und
 * laufen ueber die Kreuzung (-27340,22950) VOM Tor weg; Record @0x01224 steht bei offenem Tor schon vor dem
 * Tor 3800 vor dem Parkplatz und biss den Spieler.
 * AUFFUELLUNG auf 5 (NUTZER-VORGABE "5 Zombies durch kommen ... Wenn es bereits offen ist, kommen nur 5
 * Zombies"): die Raum-Records (main00 @0x011FC.., 20 Stueck, Tot-Bits 0x14..0x27) liefern durch das
 * Gleichzeitig-Limit 5 (Save(0x12,5) @0x011F0, Gate @0x80042214-3c) genau fuenf — solange noch fuenf
 * leben. Hat der Spieler vorher mehr als 15 getoetet, fuellen diese Records auf fuenf auf: Form der
 * Raum-Records (Typ 0x16, grid 0x0d, p6 01; @0x01238), Tot-Bits 83,84,90,91,95 (frei, Zensus Dossier
 * §2.3), Standort hinter dem Tor auf der x-Linie des Records 3 (@0x01238 x = 0x9116 = -28394) zwischen
 * der Reihe x -29248 (@0x0124C..) und Record 0 (-26063,12188) = PORT-WAHL. umzug_vorbereiten legt die
 * nicht gebrauchten still (Tot-Bit 1), das Gate @0x80042128-38 entscheidet. */
#define EM_1040_LISTE \
    OP_EM(0, 0x16, 0x0d, 0x01, 83, -28394, 15500, 0), \
    OP_EM(0, 0x16, 0x0d, 0x01, 84, -28394, 14500, 0), \
    OP_EM(0, 0x16, 0x0d, 0x01, 90, -28394, 13500, 0), \
    OP_EM(0, 0x16, 0x0d, 0x01, 91, -28394, 12500, 0), \
    OP_EM(0, 0x16, 0x0d, 0x01, 95, -28394, 11500, 0)
static const uint8_t k_p_1040_kopf[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),
    OP_CUT(RE15_IT_CUT_1040),
    EM_1040_LISTE,                                          /* erst erscheinen, dann im selben Takt aufstellen */
};
/* Aufstellung hinter dem Tor = ROOM1030 sub00 Else-Zweig @0x020C6..@0x0216E mit FUENF Faellen (Gleichzeitig-
 * Limit 5 @0x011F0) statt sechs; Laengenfelder nachgerechnet wie im Original (Fall 6+14 B; Switch = 5*20 + Eswitch
 * = 0x66; Ifel = Member_cmp 6 + Switch 4+0x66 + Calc 6 + Endif 2 = 0x78; For = 4 + 4 + 0x78 + 6 + 2 = 0x88).
 * Orte/Gierung = PORT-WAHL: Zone 0 hinter dem Tor (x -27100..-23300, z > 1200 = dort warteten die Records
 * @0x01390/@0x013A4 am geschlossenen Tor, Messzeilen T180..T360), in zwei Gassen der Toroeffnung (dort gingen
 * sie durch: x -24.6k / -25.7k) gestaffelt, Gierung 1024 = -z = zum Tor (Vorwaertspunkt fz = z - sin,
 * aot_common.c). Startabstand zum Parkplatz >= 16400 (bei ~23 je Bild > 700 Bilder; die Montage dauert < 460). */
#define IT_AUF_FALL(n, x, z) 0x14, 0x00, 0x0e, 0x00, (n), 0x00, 0x32, 0x00, LE16(x), LE16(0), LE16(z), \
                             0x34, 0x04, LE16(1024), 0x1a, 0x00
static const uint8_t k_p_1040_aufstellen[] = {
    0x24, 0x04, 0x00, 0x00, 0x24, 0x05, 0x00, 0x00,                   /* @0x020C6/@0x020CA var4 = var5 = 0 */
    0x0d, 0x00, 0x88, 0x00, 0x14, 0x00,                               /* @0x020CE For 20 (Laenge 0x88) */
    0x53, 0x02, 0x04, 0x00,                                           /* @0x020D4 Work = Gegner[var4] + Nop */
    0x06, 0x00, 0x78, 0x00,                                           /* @0x020D8 Ifel_ck (Laenge 0x78) */
    0x3e, 0x00, 0x06, 0x05, 0x00, 0x00,                               /* @0x020DC Member 6 != 0 (Platz besetzt) */
    0x13, 0x05, 0x66, 0x00,                                           /* @0x020E2 Switch var5 (Laenge 0x66) */
    IT_AUF_FALL(0, -24700, 1600),
    IT_AUF_FALL(1, -25700, 2300),
    IT_AUF_FALL(2, -25100, 3200),
    IT_AUF_FALL(3, -24500, 4100),
    IT_AUF_FALL(4, -25900, 4800),
    0x16, 0x00,                                                       /* @0x0215E Eswitch */
    0x26, 0x00, 0x00, 0x05, 0x01, 0x00,                               /* @0x02160 var5 += 1 */
    0x08, 0x00,                                                       /* @0x02166 Endif */
    0x26, 0x00, 0x00, 0x04, 0x01, 0x00,                               /* @0x02168 var4 += 1 */
    0x0e, 0x00,                                                       /* @0x0216E Next */
};
static const uint8_t k_p_1040_knall[] = {
    OP_SLEEP(15),
    OP_KNALL(RE15_IT_SIG_KNALL_1030), OP_SLEEP(20),
};
static const uint8_t k_p_1040_tor[] = {
    0x36, 0x02, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* @0x019B2 */
    OP_SLEEP(15),                                                             /* @0x019BE */
    0x36, 0x02, 0x0a, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* @0x019C2 */
    OP_SLEEP(15),                                                             /* @0x019CE */
    OP_WORK(3, 0),                                          /* Objekt 0 = Rolltor (@0x019DE) */
    0x2f, 0x01, 0xd8, 0xff,                                 /* Speed_set y -40 (@0x019E2) */
    0x0d, 0x00, 0x04, 0x00, LE16(20), 0x30, 0x02, 0x0e, 0x00,   /* For 20 { Add_speed; Evt_next } (@0x019E6) */
    0x29, 0x01,                                             /* Cut_chg 1 (@0x019F0) */
    0x2f, 0x01, 0xd8, 0xff,                                 /* Speed_set y -40 (@0x019F2) */
    0x0d, 0x00, 0x04, 0x00, LE16(50), 0x30, 0x02, 0x0e, 0x00,   /* For 50 (@0x019F6) */
    0x37, 0x02, 0x06, 0x00, 0x37, 0x03, 0x02, 0x00,         /* Sca_id_set (@0x01A00/@0x01A04) */
    0x39, 0x02, 0x06, 0x00, 0x39, 0x03, 0x02, 0x00,         /* Sca_floor_set (@0x01A08/@0x01A0C) */
    0x0d, 0x00, 0x04, 0x00, LE16(65), 0x30, 0x02, 0x0e, 0x00,   /* For 65 (@0x01A10) */
    0x36, 0x02, 0x0b, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* @0x01A1A */
    OP_SLEEP(10),                                                             /* @0x01A26 */
    OP_SET(4, 5, 1), OP_SET(4, 4, 1),
};
/* Ende des 1040-Schritts: warten, bis der Port-Takt "alle durch" meldet (Signal (5,30), Poll-Form ROOM1150 sub08
 * @0x0112A..@0x01132), dann noch ein Blick auf die Durchgekommenen (Sleep 90 = PORT-WAHL, 3 s), dann der Schnitt.
 * Vorher: feste Sleep 240 — bei offenem Tor kam damit unter der RE1.5-KI nur einer von fuenf durch (Riegel). */
static const uint8_t k_p_1040_ende[] = {
    0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_IT_SIG_ALLE_DURCH, 0x00,
    OP_SLEEP(90),
    OP_DOOR(RE15_IT_PARK_1030_X, RE15_IT_PARK_1030_Z, 0, 0x03, RE15_IT_CUT_1030_TUER),   /* Cut 7 oder 6 (Weiche) */
    OP_AOT_ON,
    OP_END,
};
static const uint8_t k_p_1040_nach[] = { EM_1040_LISTE, OP_END };   /* spaeteres Betreten */

/* PROGRAMM 3 — ROOM1030: Cut 7 (nur wenn aus 1070 Zombies uebrig: Tuerknall + Kopien vor der
 * 1070-Tuer, Records = ROOM1070 sub00 Else-Zweig @0x01632.., Blick zur Kamera = -x = Gierung 2048), dann
 * Cut 6: "noch ein Knall und die Zombies kriechen noch einmal durch das Tor" = die ORIGINAL-Szene des
 * Raums (sub08 @0x02764..@0x027DE) in ihrer eigenen Form:
 *   @0x02776 Se_on 0x0c (der Knall); nur bei (4,15)=0 (sonst hat sub00 es schon getan): @0x0276C Aot_reset
 *   Slot 17 -> msg 1, @0x02782..@0x0278B Cut_replace 0<->9 3<->10 4<->11 6<->12 (Tor aufgebrochen),
 *   @0x0278E/@0x02792 Sca_id_set 2/3, (4,15)=1 (sub01 @0x02198); @0x02796/@0x0279A Save var5/var7 = 0, dann VIERMAL `18 09` Gosub sub09 (@0x0279E, @0x027A4,
 *   @0x027B4, @0x027BA; im Original mit Pausen 5/20/10, hier unmittelbar hintereinander, weil die frisch
 *   erschienenen Zombies sonst aus dem Warte-Rechteck laufen, bevor sub09 sie findet — gemessen: von drei
 *   wurde mit den Original-Pausen nur einer erfasst, Dossier §8) — sub09 @0x027E0 sucht ab Index var5
 *   den naechsten Zombie mit AOT-Stempel 5 (member 0x0f == 5 @0x02804 = steht im Warte-Rechteck Slot 5
 *   @0x01CF2 (-12900,-25300,9200,1100) hinter dem Tor) und setzt ihm das Kriech-Bit (member 0x10 |= 0x1000
 *   @0x0280A..@0x02814 -> Toggle-Handler @0x8011f890[0x10] = FUN_80104f80), @0x027AA Cut_chg 12,
 *   @0x027BC..@0x027C4 Sleep 15+20+180, @0x027DA (5,20)=1.
 * GEMESSEN (Laeufe w1030/w1030x, Dossier §8): ohne diese Form standen die Zusatz-Zombies nur hinter dem
 * Tor ((4,15)=0) bzw. es kroch keiner ((4,15)=1) — das Kriechen loest im Original das SKRIPT aus (sub09 in
 * der Szene, sub02/sub06 im Spiel, dort nur solange weniger als 4 Zombies im Hauptraum stehen @0x021CC).
 * Die drei Zusatz-Zombies (Raum-Record-Form @0x01DE6, Typ 0x16 grid 0x0d; Standort = PORT-WAHL IM
 * Warte-Rechteck Slot 5 und ausserhalb des Hauptraum-Rechtecks Slot 6 @0x01D06 z >= -24500) sind die, die
 * kriechen; war die Original-Szene noch nicht gelaufen ((4,15)=0), ist sie damit verbraucht (Tor offen).
 * Save(0x12,20) hebt das Gleichzeitig-Limit 6 (@0x01DE2) fuer die Zusatz-Records (@0x80042214-3c). Slots
 * werden bei der Ausloesung auf freie Aktor-Plaetze gesetzt (RE15_ACTOR_MAX = 16). */
/* 1070-Kopien: STEHENDE Form des Else-Zweigs von ROOM1070 sub00 ((4,197)=1, @0x01632..@0x01682: grid 0x00,
 * p6 = 00/01/01/01/01); der Erst-Zweig @0x015CA.. (grid 0x88/0x87) sind die liegenden Leichen. */
#define EM_1070_LISTE \
    OP_EM(0, 0x10, 0x00, 0x00, 47, -18600, -3500, 2048), \
    OP_EM(0, 0x10, 0x00, 0x01, 48, -20600, -3000, 2048), \
    OP_EM(0, 0x10, 0x00, 0x01, 49, -22400, -4000, 2048), \
    OP_EM(0, 0x11, 0x00, 0x01, 75, -19600, -2400, 2048), \
    OP_EM(0, 0x11, 0x00, 0x01, 76, -21600, -4600, 2048)
#define EM_KRIECHER_LISTE \
    OP_EM(0, 0x16, 0x0d, 0x01, 77,  -9500, -24800, 3072), \
    OP_EM(0, 0x16, 0x0d, 0x01, 78,  -7300, -24800, 3072), \
    OP_EM(0, 0x16, 0x0d, 0x01, 79, -11500, -24800, 3072)
#define EM_KRIECHER_NACH \
    OP_EM(0, 0x16, 0x0d, 0x01, 77,  -9500, -19000, 1024), \
    OP_EM(0, 0x16, 0x0d, 0x01, 78,  -7300, -18500, 1024), \
    OP_EM(0, 0x16, 0x0d, 0x01, 79, -11500, -18800, 1024)
static const uint8_t k_p_1030_kopf[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),
    0x24, 0x12, LE16(20),                                   /* Save(0x12,20): Gleichzeitig-Limit */
};
static const uint8_t k_p_1030_tuer[] = {
    OP_CUT(RE15_IT_CUT_1030_TUER), OP_SLEEP(15),
    OP_KNALL(RE15_IT_SIG_KNALL_TUER), OP_SLEEP(10),
    EM_1070_LISTE,
    OP_SLEEP(150),
};
static const uint8_t k_p_1030_tor[] = {
    OP_CUT(RE15_IT_CUT_1030_TOR), OP_SLEEP(15),
    0x36, 0x02, 0x0c, 0x00, 0x00, 0x00, 0xcc, 0xdd, 0xf8, 0xf8, 0xf0, 0xa7,   /* sub08 @0x02776: der Knall */
};
/* Das Tor bricht auf — NUR wenn die Original-Szene noch nicht gelaufen ist ((4,15)=0). Bei (4,15)=1 hat
 * sub00 @0x01FF4..@0x02004 dieselben Zeilen schon beim Raumaufbau ausgefuehrt, und Cut_replace ist ein
 * TAUSCH (LAB_80040414 @0x8004044c-a8: jeder Zonen-Satz mit a bekommt b UND umgekehrt): ein zweiter Lauf
 * tauschte die Zonen zurueck — gemessen (Lauf x1030, Dossier §8.3): die Kriecher standen im Raum
 * (Messzeile), wurden unter Cut 12 aber nicht gezeichnet (Sicht-Zonen wieder die des alten Cuts). */
static const uint8_t k_p_1030_bruch[] = {
    0x46, 0x11, 0x01, 0x31, 0x01, 0x00, 0xff, 0xff, 0x00, 0x00,               /* sub08 @0x0276C */
    0x4b, 0x00, 0x09, 0x4b, 0x03, 0x0a, 0x4b, 0x04, 0x0b, 0x4b, 0x06, 0x0c,   /* sub08 @0x02782..@0x0278B */
    0x37, 0x02, 0x06, 0xf7, 0x37, 0x03, 0x06, 0xf7,                           /* sub08 @0x0278E/@0x02792 */
    OP_SET(4, 15, 1),                                                         /* sub01 @0x02198 */
};
static const uint8_t k_p_1030_kriechen[] = {
    EM_KRIECHER_LISTE,                                      /* erscheinen im Warte-Rechteck hinter dem Tor */
    OP_SLEEP(2),                                            /* ein Bild fuer den AOT-Stempel (Scan nach der VM, @0x8001ce1c) */
    0x24, 0x05, 0x00, 0x00, 0x24, 0x07, 0x00, 0x00,                           /* sub08 @0x02796/@0x0279A */
    0x18, 0x09, 0x18, 0x09, 0x18, 0x09, 0x18, 0x09,         /* sub08 @0x0279E/@0x027A4/@0x027B4/@0x027BA: Gosub sub09 x4 */
    OP_SLEEP(20),                                                             /* sub08 @0x027A6 */
    OP_CUT(12),                                                               /* sub08 @0x027AA */
    OP_SLEEP(15), OP_SLEEP(20), OP_SLEEP(180),                                /* sub08 @0x027BC..@0x027C4 */
    OP_SLEEP(120),                                          /* bis die Kriecher durchs Tor sind (gemessen, Dossier §8.3) */
    OP_SET(5, 20, 1),                                                         /* sub08 @0x027DA */
    OP_DOOR(RE15_IT_PARK_11C0_X, RE15_IT_PARK_11C0_Z, RE15_IT_PARK_11C0_YAW, 0x1c, RE15_IT_CUT_11C0),   /* Raum 0x1c = ROOM11C0 (dest_id = 0x1000 | 0x1c<<4) */
    OP_AOT_ON,
    OP_END,
};
static const uint8_t k_p_1030_nach_kopf[] = { 0x24, 0x12, LE16(20) };
static const uint8_t k_p_1030_nach_tuer[] = { EM_1070_LISTE };
static const uint8_t k_p_1030_nach_tor[]  = { EM_KRIECHER_NACH, OP_END };

/* PROGRAMM 4 — ROOM11C0 Cut 13: Ada steht bereits dort (sub00 @0x01770, (4,64)=0), Marvin kommt
 * dazu (Record-Form ROOM11B0 main00 @0x01080, Typ 0x40, grid 0x40; Standort links von Ada =
 * PORT-WAHL), beide drehen sich zum Gebaeude (Tuer Slot 0 @0x01712), Dialog, Marvin dreht sich zu
 * Ada, Arm (NPC-Bibliotheks-Clip 15 vor/zurueck wie 11B0 sub06 @0x014F6), rennt zum Gebaeude
 * (Modus 5, 11B0 sub06 @0x015EA), Ada: "Marvin!..." + Arm, Schnitt zurueck nach 1150. */
static const uint8_t k_p_11c0[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),
    OP_EM(3, 0x40, 0x40, 0x00, 0xff, -10500, -12300, 475),   /* Marvin, Slot 3 = Aktor 4 */
    OP_CUT(RE15_IT_CUT_11C0), OP_SLEEP(30),
    OP_WORK(2, 0), OP_DEST(9, 0x21, -27100, 15900),         /* Ada dreht sich zum Gebaeude */
    OP_WORK(2, 3), OP_DEST(9, 0x22, -27100, 15900),         /* Marvin ebenso */
    OP_SLEEP(45),
    OP_WORK(2, 0), OP_MSG(10), OP_SLEEP(90),                /* "Ada: What was this noise? Did you hear that?" */
    OP_WORK(2, 3), OP_MSG(11), OP_SLEEP(60),                /* "Marvin: Yes!...." */
    OP_MSG(12), OP_SLEEP(60),                               /* "Marvin: Oh, no, Leon!" */
    OP_DEST(9, 0x22, -8965, -14347), OP_SLEEP(40),          /* Marvin dreht sich zu Ada */
    OP_MSG(13),                                             /* "Marvin: I have to help him, sorry!" */
    OP_MOTION(0, 15), OP_SLEEP(25), OP_MOTION(0, 15), OP_FLG_REV, OP_SLEEP(26),
    OP_DEST(5, 0x22, -22000, 8000), OP_SLEEP(30),           /* Marvin rennt aus dem Bild Richtung Gebaeude */
    OP_WORK(2, 0), OP_MSG(14),                              /* "Ada: Marvin!..." */
    OP_MOTION(0, 15), OP_SLEEP(25), OP_MOTION(0, 15), OP_FLG_REV, OP_SLEEP(26),
    OP_SLEEP(60),
    OP_DOOR(RE15_IT_COUCH_X, RE15_IT_COUCH_Z, RE15_IT_COUCH_YAW, 0x15, RE15_IT_CUT_1150),
    OP_AOT_ON,
    OP_END,
};

/* PROGRAMM 5 — ROOM1150 Rueckkehr: Cut 7, dann Cut 5 + Cut_auto wie das Ende der ersten Szene
 * (@0x012C4/@0x012E2), Plc_ret, Latches loesen (@0x012E4/@0x012E8). */
static const uint8_t k_p_rueck[] = {
    OP_SET(2, 7, 1), OP_SET(1, 27, 1),
    OP_CUT(RE15_IT_CUT_1150), OP_SLEEP(45),
    OP_CUT(5),
    OP_WORK(1, 0),
    0x42, 0x00,                                             /* Plc_ret + Nop */
    0x3c, 0x01,                                             /* Cut_auto 1 */
    OP_SET(2, 7, 0), OP_SET(1, 27, 0),
    OP_END,
};

/* ---- Nachrichten (.msg-Rohbytes, tools/r35_l/texte_bauen.py). ⛔ NEUE TEXTE (Nutzervorgabe), Form
 * belegt: Dialog `04 00 05 cc <Name> 16 05 00 00 ... 04 01 01 63` (ROOM1090 msg 0 @0x275C / msg 1
 * @0x279C), Farben Leon 01 (ROOM1150 msg 4 @0x1453), Irons 02 (msg 5 @0x149E), Ada 02 (ROOM11C0 msg 1
 * @0x1CD0), Marvin 07 (ROOM11B0 msg 1 @0x190E); Umbruch 0x08, jede Zeile <= 271 px. */
static const uint8_t k_msg22[] = {   /* "Leon: Sir!" */
    0x04,0x00,0x05,0x01,0x28,0x41,0x4b,0x4a,0x16,0x05,0x00,0x00,
    0x2f,0x45,0x4e,0x1a,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg23[] = {   /* "Leon: Sir, the communication system / can't be fixed! We're going to use the / patrol car to get out of here." */
    0x04,0x00,0x05,0x01,0x28,0x41,0x4b,0x4a,0x16,0x05,0x00,0x00,
    0x2f,0x45,0x4e,0x18,0x00,0x50,0x44,0x41,0x00,0x3f,0x4b,0x49,0x49,0x51,0x4a,0x45,0x3f,0x3d,0x50,0x45,
    0x4b,0x4a,0x00,0x4f,0x55,0x4f,0x50,0x41,0x49,0x08,
    0x3f,0x3d,0x4a,0x3a,0x50,0x00,0x3e,0x41,0x00,0x42,0x45,0x54,0x41,0x40,0x1a,0x00,0x33,0x41,0x3a,0x4e,
    0x41,0x00,0x43,0x4b,0x45,0x4a,0x43,0x00,0x50,0x4b,0x00,0x51,0x4f,0x41,0x00,0x50,0x44,0x41,0x08,
    0x4c,0x3d,0x50,0x4e,0x4b,0x48,0x00,0x3f,0x3d,0x4e,0x00,0x50,0x4b,0x00,0x43,0x41,0x50,0x00,0x4b,0x51,
    0x50,0x00,0x4b,0x42,0x00,0x44,0x41,0x4e,0x41,0x57,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg24[] = {   /* "Leon: I came to get you, come with me!" */
    0x04,0x00,0x05,0x01,0x28,0x41,0x4b,0x4a,0x16,0x05,0x00,0x00,
    0x25,0x00,0x3f,0x3d,0x49,0x41,0x00,0x50,0x4b,0x00,0x43,0x41,0x50,0x00,0x55,0x4b,0x51,0x18,0x00,0x3f,
    0x4b,0x49,0x41,0x00,0x53,0x45,0x50,0x44,0x00,0x49,0x41,0x1a,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg25[] = {   /* "Irons: Leon... I... I'm proud to have an / officer as dependable as you!" */
    0x04,0x00,0x05,0x02,0x25,0x4e,0x4b,0x4a,0x4f,0x16,0x05,0x00,0x00,
    0x28,0x41,0x4b,0x4a,0x57,0x57,0x57,0x00,0x25,0x57,0x57,0x57,0x00,0x25,0x3a,0x49,0x00,0x4c,0x4e,0x4b,
    0x51,0x40,0x00,0x50,0x4b,0x00,0x44,0x3d,0x52,0x41,0x00,0x3d,0x4a,0x08,
    0x4b,0x42,0x42,0x45,0x3f,0x41,0x4e,0x00,0x3d,0x4f,0x00,0x40,0x41,0x4c,0x41,0x4a,0x40,0x3d,0x3e,0x48,
    0x41,0x00,0x3d,0x4f,0x00,0x55,0x4b,0x51,0x1a,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg26[] = {   /* "Irons: But... I... I'm not going to make it... / I'm sorry..." */
    0x04,0x00,0x05,0x02,0x25,0x4e,0x4b,0x4a,0x4f,0x16,0x05,0x00,0x00,
    0x1e,0x51,0x50,0x57,0x57,0x57,0x00,0x25,0x57,0x57,0x57,0x00,0x25,0x3a,0x49,0x00,0x4a,0x4b,0x50,0x00,
    0x43,0x4b,0x45,0x4a,0x43,0x00,0x50,0x4b,0x00,0x49,0x3d,0x47,0x41,0x00,0x45,0x50,0x57,0x57,0x57,0x08,
    0x25,0x3a,0x49,0x00,0x4f,0x4b,0x4e,0x4e,0x55,0x57,0x57,0x57,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg27[] = {   /* "Irons: Please... one last favor... Look / after yourself and the others who / survived." */
    0x04,0x00,0x05,0x02,0x25,0x4e,0x4b,0x4a,0x4f,0x16,0x05,0x00,0x00,
    0x2c,0x48,0x41,0x3d,0x4f,0x41,0x57,0x57,0x57,0x00,0x4b,0x4a,0x41,0x00,0x48,0x3d,0x4f,0x50,0x00,0x42,
    0x3d,0x52,0x4b,0x4e,0x57,0x57,0x57,0x00,0x28,0x4b,0x4b,0x47,0x08,
    0x3d,0x42,0x50,0x41,0x4e,0x00,0x55,0x4b,0x51,0x4e,0x4f,0x41,0x48,0x42,0x00,0x3d,0x4a,0x40,0x00,0x50,
    0x44,0x41,0x00,0x4b,0x50,0x44,0x41,0x4e,0x4f,0x00,0x53,0x44,0x4b,0x08,
    0x4f,0x51,0x4e,0x52,0x45,0x52,0x41,0x40,0x57,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg28[] = {   /* "Irons: Be a hero... Leon..." */
    0x04,0x00,0x05,0x02,0x25,0x4e,0x4b,0x4a,0x4f,0x16,0x05,0x00,0x00,
    0x1e,0x41,0x00,0x3d,0x00,0x44,0x41,0x4e,0x4b,0x57,0x57,0x57,0x00,0x28,0x41,0x4b,0x4a,0x57,0x57,0x57,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg29[] = {   /* "Leon: SIR, Sir?!" */
    0x04,0x00,0x05,0x01,0x28,0x41,0x4b,0x4a,0x16,0x05,0x00,0x00,
    0x2f,0x25,0x2e,0x18,0x00,0x2f,0x45,0x4e,0x1b,0x1a,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg10[] = {   /* "Ada: What was this noise? Did you hear / that?" */
    0x04,0x00,0x05,0x02,0x1d,0x40,0x3d,0x16,0x05,0x00,0x00,
    0x33,0x44,0x3d,0x50,0x00,0x53,0x3d,0x4f,0x00,0x50,0x44,0x45,0x4f,0x00,0x4a,0x4b,0x45,0x4f,0x41,0x1b,
    0x00,0x20,0x45,0x40,0x00,0x55,0x4b,0x51,0x00,0x44,0x41,0x3d,0x4e,0x08,
    0x50,0x44,0x3d,0x50,0x1b,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg11[] = {   /* "Marvin: Yes!...." */
    0x04,0x00,0x05,0x07,0x29,0x3d,0x4e,0x52,0x45,0x4a,0x16,0x05,0x00,0x00,
    0x35,0x41,0x4f,0x1a,0x57,0x57,0x57,0x57,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg12[] = {   /* "Marvin: Oh, no, Leon!" */
    0x04,0x00,0x05,0x07,0x29,0x3d,0x4e,0x52,0x45,0x4a,0x16,0x05,0x00,0x00,
    0x2b,0x44,0x18,0x00,0x4a,0x4b,0x18,0x00,0x28,0x41,0x4b,0x4a,0x1a,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg13[] = {   /* "Marvin: I have to help him, sorry!" */
    0x04,0x00,0x05,0x07,0x29,0x3d,0x4e,0x52,0x45,0x4a,0x16,0x05,0x00,0x00,
    0x25,0x00,0x44,0x3d,0x52,0x41,0x00,0x50,0x4b,0x00,0x44,0x41,0x48,0x4c,0x00,0x44,0x45,0x49,0x18,0x00,
    0x4f,0x4b,0x4e,0x4e,0x55,0x1a,
    0x04,0x01,0x01,0x63,
};
static const uint8_t k_msg14[] = {   /* "Ada: Marvin!..." */
    0x04,0x00,0x05,0x02,0x1d,0x40,0x3d,0x16,0x05,0x00,0x00,
    0x29,0x3d,0x4e,0x52,0x45,0x4a,0x1a,0x57,0x57,0x57,
    0x04,0x01,0x01,0x63,
};

static const struct { uint16_t raum; uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    { 0x1150, 22, k_msg22, (uint8_t)sizeof k_msg22 }, { 0x1150, 23, k_msg23, (uint8_t)sizeof k_msg23 },
    { 0x1150, 24, k_msg24, (uint8_t)sizeof k_msg24 }, { 0x1150, 25, k_msg25, (uint8_t)sizeof k_msg25 },
    { 0x1150, 26, k_msg26, (uint8_t)sizeof k_msg26 }, { 0x1150, 27, k_msg27, (uint8_t)sizeof k_msg27 },
    { 0x1150, 28, k_msg28, (uint8_t)sizeof k_msg28 }, { 0x1150, 29, k_msg29, (uint8_t)sizeof k_msg29 },
    { 0x11C0, 10, k_msg10, (uint8_t)sizeof k_msg10 }, { 0x11C0, 11, k_msg11, (uint8_t)sizeof k_msg11 },
    { 0x11C0, 12, k_msg12, (uint8_t)sizeof k_msg12 }, { 0x11C0, 13, k_msg13, (uint8_t)sizeof k_msg13 },
    { 0x11C0, 14, k_msg14, (uint8_t)sizeof k_msg14 },
};
#define IT_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

/* ---- Zustand --------------------------------------------------------------------------------- */
static int     s_zustand = RE15_IT_AUS;
static uint8_t s_prog[640];        /* die RAM-Kopie, die die VM ausfuehrt (groesstes Programm 574 B) */
static int     s_prog_len = 0;

static void log_it(const char *was)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[irons-tod] ROOM%04X Zustand %d: %s\n", (unsigned)g_current_room_id, s_zustand, was);
#else
    (void)was;
#endif
}

int  re15_irons_tod_zustand(void) { return s_zustand; }
void re15_irons_tod_zustand_setzen(int z) { s_zustand = z; }

const uint8_t *re15_irons_tod_programm(int welches, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    switch (welches) {
    case 0: p = k_p_szene;     n = (int)sizeof k_p_szene;     break;
    case 1: p = k_p_1130;      n = (int)sizeof k_p_1130;      break;
    case 2: p = k_p_1040_kopf; n = (int)sizeof k_p_1040_kopf; break;
    case 3: p = k_p_1040_tor;  n = (int)sizeof k_p_1040_tor;  break;
    case 4: p = k_p_1040_ende; n = (int)sizeof k_p_1040_ende; break;
    case 5: p = k_p_1030_kopf; n = (int)sizeof k_p_1030_kopf; break;
    case 6: p = k_p_1030_tuer; n = (int)sizeof k_p_1030_tuer; break;
    case 7: p = k_p_1030_tor;  n = (int)sizeof k_p_1030_tor;  break;
    case 8: p = k_p_11c0;      n = (int)sizeof k_p_11c0;      break;
    case 9: p = k_p_rueck;     n = (int)sizeof k_p_rueck;     break;
    case 10: p = k_p_1130_nach; n = (int)sizeof k_p_1130_nach; break;
    case 11: p = k_p_1030_nach_kopf; n = (int)sizeof k_p_1030_nach_kopf; break;
    case 12: p = k_p_1030_nach_tuer; n = (int)sizeof k_p_1030_nach_tuer; break;
    case 13: p = k_p_1030_nach_tor;  n = (int)sizeof k_p_1030_nach_tor;  break;
    case 14: p = k_p_1040_nach;      n = (int)sizeof k_p_1040_nach;      break;
    case 15: p = k_p_1030_bruch;     n = (int)sizeof k_p_1030_bruch;     break;
    case 16: p = k_p_1030_kriechen;  n = (int)sizeof k_p_1030_kriechen;  break;
    case 17: p = k_p_1040_aufstellen; n = (int)sizeof k_p_1040_aufstellen; break;
    case 18: p = k_p_1040_knall;     n = (int)sizeof k_p_1040_knall;     break;
    default: break;
    }
    if (out_len) *out_len = n;
    return p;
}

const uint8_t *re15_irons_tod_laufprogramm(int *out_len)
{
    if (out_len) *out_len = s_prog_len;
    return s_prog;
}

const uint8_t *re15_irons_tod_meldung(uint16_t room_id, int msg_id, int *out_len)
{
    for (int i = 0; i < IT_N_MELDUNGEN; i++)
        if (k_meldungen[i].raum == room_id && (int)k_meldungen[i].id == msg_id) {
            if (out_len) *out_len = (int)k_meldungen[i].n;
            return k_meldungen[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

static void meldungen_einsetzen(uint16_t room_id)
{
    for (int i = 0; i < IT_N_MELDUNGEN; i++) {
        if (k_meldungen[i].raum != room_id) continue;
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

/* Zone-7-Tot-Bits (re15_em_status_zone: Stage 1-3 -> 7). */
#define IT_ZONE 7
static const uint8_t k_bits_1130[RE15_IT_N_1140] = { 40, 41, 42, 43, 46 };
static const uint8_t k_bits_1030[RE15_IT_N_1070] = { 47, 48, 49, 75, 76 };
static const uint8_t k_bits_1040[RE15_IT_N_1040_AUF] = { 83, 84, 90, 91, 95 };

/* ROOM1040: wie viele der 20 Raum-Records (Tot-Bits 0x14..0x27, main00 @0x011FC..) leben noch? */
int re15_irons_tod_lebend_1040(void)
{
    int n = 0;
    for (int i = 0; i < RE15_IT_N_1040; i++) if (!re15_game_flag_get(IT_ZONE, (uint8_t)(RE15_IT_BIT_1040_0 + i))) n++;
    return n;
}

int re15_irons_tod_lebend(int welche)
{
    int n = 0;
    uint8_t basis = welche ? RE15_IT_BIT_1070_0 : RE15_IT_BIT_1140_0;
    for (int i = 0; i < 5; i++) if (!re15_game_flag_get(IT_ZONE, (uint8_t)(basis + i))) n++;
    return n;
}

/* Beim Start der Szene: die Ueberlebenden "ziehen um" — ihr Quell-Record wird stillgelegt (Tot-Bit
 * gesetzt), die Kopie im Zielraum bleibt scharf; schon tote Records bekommen eine stille Kopie
 * (Kopie-Bit gesetzt). Danach entscheidet nur noch das Gate @0x80042128-38, wer erscheint. */
static void umzug_vorbereiten(void)
{
    int l1140 = re15_irons_tod_lebend(0), l1070 = re15_irons_tod_lebend(1);
    for (int i = 0; i < RE15_IT_N_1140; i++) {
        if (re15_game_flag_get(IT_ZONE, (uint8_t)(RE15_IT_BIT_1140_0 + i)))
            re15_game_flag_set(IT_ZONE, k_bits_1130[i], 1);            /* war schon tot: keine Kopie */
        else
            re15_game_flag_set(IT_ZONE, (uint8_t)(RE15_IT_BIT_1140_0 + i), 1);   /* hat 1140 verlassen */
    }
    for (int i = 0; i < RE15_IT_N_1070; i++) {
        if (re15_game_flag_get(IT_ZONE, (uint8_t)(RE15_IT_BIT_1070_0 + i)))
            re15_game_flag_set(IT_ZONE, k_bits_1030[i], 1);
        else
            re15_game_flag_set(IT_ZONE, (uint8_t)(RE15_IT_BIT_1070_0 + i), 1);
    }
    /* 1040: auf fuenf auffuellen — Auffuell-Record i ist scharf, wenn weniger als 5 - i Raum-Records leben. */
    int l1040 = re15_irons_tod_lebend_1040();
    for (int i = 0; i < RE15_IT_N_1040_AUF; i++)
        re15_game_flag_set(IT_ZONE, k_bits_1040[i], (l1040 + i < RE15_IT_N_1040_AUF) ? 0 : 1);
    re15_game_flag_set(RE15_IT_BANK, RE15_IT_BIT_1130, l1140 > 0 ? 1 : 0);
    re15_game_flag_set(RE15_IT_BANK, RE15_IT_BIT_1040, 1);
    re15_game_flag_set(RE15_IT_BANK, RE15_IT_BIT_1030, l1070 > 0 ? 1 : 0);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[irons-tod] Umzug: 1140 lebend %d -> 1130 (9,74)=%d; 1070 lebend %d -> 1030 (9,76)=%d\n",
            l1140, l1140 > 0, l1070, l1070 > 0);
    fprintf(stderr, "[irons-tod] 1040: %d Raum-Records leben, Auffuellung %d\n",
            l1040, l1040 < RE15_IT_N_1040_AUF ? RE15_IT_N_1040_AUF - l1040 : 0);
#endif
}

/* Die Knall-Tonbank in den Tuerbank-Platz laden (Format DO2-Tonteil, audio_pc.c TORSE_EDT_SIZE;
 * ueberlebt re15_audio_load_room_banks, wird erst von der naechsten Tuersequenz ersetzt). Das
 * Programm loest sie mit den Signalen (5,28)/(5,29) aus, der Takt ruft re15_audio_re2_tuer_se. Der Se_on-
 * Verteiler bleibt byte-true (Bank >= 6 verworfen, `sltiu v0,v1,0x6` @0x80045094; Pin unit_se_bank_routing). */
static void knallbank_laden(void)
{
    if (!re15_audio_re2_tuer_laden(k_knall_bank, (int)KNALL_BANK_SIZE)) log_it("Knall-Tonbank nicht geladen");
}

/* Laeuft das Programm schon in einem Faden? (wie adaruf_1050.c) */
static int programm_laeuft(void)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= s_prog && t->pc < s_prog + sizeof s_prog) return 1;
    }
    return 0;
}

static void prog_anhaengen(const uint8_t *p, int n)
{
    if (s_prog_len + n > (int)sizeof s_prog) return;
    memcpy(s_prog + s_prog_len, p, (size_t)n);
    s_prog_len += n;
}

/* Freie Aktor-Plaetze fuer die Zusatz-Records eintragen (Sce_em_set-Slot = Aktor - 1, scd_vm.c
 * SCRIPT_SLOT_TO_ACTOR): jeder `44`-Satz in [von, s_prog_len) bekommt den naechsten freien Platz.
 * Gelaufen wird opcode-weise (Satzbreiten der hier vorkommenden Opcodes = s_opcode_sizes). */
static int it_opcode_len(uint8_t op)
{
    switch (op) {
    case 0x01: return 2;  case 0x09: return 4;  case 0x22: return 4;  case 0x24: return 4;
    case 0x29: return 2;  case 0x36: return 12; case 0x3b: return 32; case 0x44: return 20;
    case 0x47: return 2;  case 0x46: return 10; case 0x4b: return 3;  case 0x37: return 4;
    case 0x18: return 2;
    default:   return 0;
    }
}
static void slots_zuweisen(int von)
{
    int aktor = 1;
    for (int o = von; o < s_prog_len; ) {
        int n = it_opcode_len(s_prog[o]);
        if (n <= 0) break;                                   /* unbekannt: nicht raten */
        if (s_prog[o] == 0x44) {
            while (aktor < RE15_ACTOR_MAX && g_actors[aktor].active) aktor++;
            if (aktor >= RE15_ACTOR_MAX) { s_prog[o + 7] = 0xff; s_prog[o + 1] = 0x0f; }   /* kein Platz: Slot 15 -> Aktor 16 = verworfen */
            else { s_prog[o + 1] = (uint8_t)(aktor - 1); aktor++; }
        }
        o += n;
    }
}

static void tuer_setzen(int door_off, int x, int z, int yaw, uint8_t room, uint8_t cut)
{
    uint8_t *d = s_prog + door_off;
    d[DOOR_OFF_X]   = (uint8_t)(x & 0xff);   d[DOOR_OFF_X + 1]   = (uint8_t)((x >> 8) & 0xff);
    d[DOOR_OFF_Z]   = (uint8_t)(z & 0xff);   d[DOOR_OFF_Z + 1]   = (uint8_t)((z >> 8) & 0xff);
    d[DOOR_OFF_YAW] = (uint8_t)(yaw & 0xff); d[DOOR_OFF_YAW + 1] = (uint8_t)((yaw >> 8) & 0xff);
    d[DOOR_OFF_ROOM] = room;
    d[DOOR_OFF_CUT]  = cut;
}

/* Das Programm fuer (Raum, Zustand) in s_prog zusammensetzen; 0 = keins. */
static int programm_bauen(uint16_t room_id)
{
    s_prog_len = 0;
    switch (s_zustand) {
    case RE15_IT_SZENE:
        if (room_id != RE15_IT_RAUM_1150) return 0;
        prog_anhaengen(k_p_szene, (int)sizeof k_p_szene);
        if (!re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1130))   /* 1140 leer: "passiert nichts" -> 1040 */
            tuer_setzen(s_prog_len - 36, RE15_IT_PARK_1040_X, RE15_IT_PARK_1040_Z, 1024, 0x04, RE15_IT_CUT_1040);
        return 1;
    case RE15_IT_S1130:
        if (room_id != RE15_IT_RAUM_1130) return 0;
        prog_anhaengen(k_p_1130, (int)sizeof k_p_1130);
        return 1;
    case RE15_IT_S1040:
        if (room_id != RE15_IT_RAUM_1040) return 0;
        prog_anhaengen(k_p_1040_kopf, (int)sizeof k_p_1040_kopf);
        slots_zuweisen(0);                                   /* Auffuell-Records auf freie Plaetze */
        prog_anhaengen(k_p_1040_aufstellen, (int)sizeof k_p_1040_aufstellen);   /* hinter das Tor (Zone 0) */
        prog_anhaengen(k_p_1040_knall, (int)sizeof k_p_1040_knall);
        if (!re15_game_flag_get(4, 5)) prog_anhaengen(k_p_1040_tor, (int)sizeof k_p_1040_tor);
        prog_anhaengen(k_p_1040_ende, (int)sizeof k_p_1040_ende);
        if (!re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1030))   /* 1070 leer: gleich Cut 6 */
            tuer_setzen(s_prog_len - 36, RE15_IT_PARK_1030_X, RE15_IT_PARK_1030_Z, 0, 0x03,
                        re15_game_flag_get(4, 15) ? 12 : RE15_IT_CUT_1030_TOR);   /* aufgebrochenes Tor = Cut 12 (s. S1030) */
        return 1;
    case RE15_IT_S1030: {
        if (room_id != RE15_IT_RAUM_1030) return 0;
        prog_anhaengen(k_p_1030_kopf, (int)sizeof k_p_1030_kopf);
        int von = s_prog_len;
        if (re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1030)) prog_anhaengen(k_p_1030_tuer, (int)sizeof k_p_1030_tuer);
        int tor = s_prog_len;
        prog_anhaengen(k_p_1030_tor, (int)sizeof k_p_1030_tor);
        if (re15_game_flag_get(4, 15)) s_prog[tor + 1] = 12;   /* Tor schon aufgebrochen: die Tor-Ansicht ist nach dem
                                                                * Tausch von sub00 (@0x01FFD `4b 06 0c`) Cut 12 */
        else prog_anhaengen(k_p_1030_bruch, (int)sizeof k_p_1030_bruch);
        prog_anhaengen(k_p_1030_kriechen, (int)sizeof k_p_1030_kriechen);
        slots_zuweisen(von);
        if (re15_irons_tod_ohne_11c0())                      /* Ada steht nicht mehr an Cut 13: gleich zurueck */
            tuer_setzen(s_prog_len - 36, RE15_IT_COUCH_X, RE15_IT_COUCH_Z, RE15_IT_COUCH_YAW, 0x15, RE15_IT_CUT_1150);
        return 1;
    }
    case RE15_IT_S11C0:
        if (room_id != RE15_IT_RAUM_11C0) return 0;
        prog_anhaengen(k_p_11c0, (int)sizeof k_p_11c0);
        return 1;
    case RE15_IT_RUECKKEHR:
        if (room_id != RE15_IT_RAUM_1150) return 0;
        prog_anhaengen(k_p_rueck, (int)sizeof k_p_rueck);
        return 1;
    case RE15_IT_AUS:
        if (room_id == RE15_IT_RAUM_1130 && re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1130)) {
            prog_anhaengen(k_p_1130_nach, (int)sizeof k_p_1130_nach);
            return 1;
        }
        if (room_id == RE15_IT_RAUM_1040 && re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1040)) {
            prog_anhaengen(k_p_1040_nach, (int)sizeof k_p_1040_nach);
            slots_zuweisen(0);
            return 1;
        }
        if (room_id == RE15_IT_RAUM_1030 && re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_GESEHEN)) {
            prog_anhaengen(k_p_1030_nach_kopf, (int)sizeof k_p_1030_nach_kopf);
            int von = s_prog_len;
            if (re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1030))
                prog_anhaengen(k_p_1030_nach_tuer, (int)sizeof k_p_1030_nach_tuer);
            prog_anhaengen(k_p_1030_nach_tor, (int)sizeof k_p_1030_nach_tor);
            slots_zuweisen(von);
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

const uint8_t *re15_irons_tod_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (event_id != RE15_IT_EREIGNIS) return NULL;
    if (programm_laeuft()) return NULL;
    if (!programm_bauen(room_id)) return NULL;
    return s_prog;
}

/* ROOM11C0 sub00 @0x0176C `21 04 40 00`: Ada steht nur bei (4,64)=0 an Cut 13 (@0x01770); nach ihrer eigenen
 * Szene (sub02 @0x0184E `22 04 40 01`) ist sie geparkt (@0x017D4 y -20000) bzw. gar nicht im Raum. Dann
 * entfaellt der Schnitt nach 11C0 (die Vorbedingung des Nutzers "wo Ada in der spaeteren cutscene schon
 * steht" gilt nicht mehr) und die Montage kehrt aus 1030 direkt nach 1150 zurueck. */
int re15_irons_tod_ohne_11c0(void) { return re15_game_flag_get(4, 64) ? 1 : 0; }

int re15_irons_tod_sub01_gesperrt(void)
{
    /* Irons ist tot ((9,73)=1): der Ansprech-Poll von ROOM1150 sub01 (@0x00EC0 `23 00 00 00 02 00` +
     * @0x00EC6 Ck(3,157)==0 -> @0x00ECE Evt_exec sub03 "Irons: I'll be fine...", Irons Clip 6 @0x00F24) darf
     * nicht mehr laufen — NUTZER-VORGABE "sich garnicht mehr bewegen - tot". sub01 traegt in ROOM1150
     * nichts anderes (26 B @0x00EBC..@0x00ED6). Gemessen vorher: Riegel tot_bleibt_tot (msg 2, Clip 6). */
    if ((uint16_t)g_current_room_id == RE15_IT_RAUM_1150 && re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_GESEHEN))
        return 1;
    return s_zustand == RE15_IT_S1130 || s_zustand == RE15_IT_S1040 ||
           s_zustand == RE15_IT_S1030 || s_zustand == RE15_IT_S11C0;
}

/* Irons: Clip `clip` ab Bild `bild` im Motion-Sub 0 (Phase 1 = spielt bis Clip-Ende, dann Halt
 * @0x80050da4; `gehalten` = direkt Phase 2). Die Pose selbst kommt aus dem RBJ-Record 1. */
static re15_actor_t *irons(void)
{
    re15_actor_t *a = &g_actors[RE15_IT_IRONS_SLOT];
    return (a->active && a->type == RE15_IT_IRONS_TYP) ? a : NULL;
}

static void irons_pose(int clip, int bild, int gehalten)
{
    re15_actor_t *a = irons();
    if (!a) return;
    re15_actor_set_motion(a, (int16_t)clip);
    a->state = 4; a->sub_state_1 = 0;
    a->sub_state_2 = (uint8_t)(gehalten ? 2 : 1);
    a->anim_frame = (uint8_t)bild;
    a->anim_flags = 0;
    a->anim_frac  = (uint8_t)(gehalten ? 0 : 7);
    a->motion_init_delay = 0;
}

/* MESS-HAKEN RE15_IT_LOG=<n> (env-gegatet, kein Spielverhalten): waehrend eines Montage-Schritts alle n Takte
 * eine Zeile je Raum mit allen Gegnern — Platz:Typ(x,z) g=grid_id s=+0x05 st=AOT-Stempel fl=+0x1c4 mo=Clip.
 * Damit laesst sich an der echten exe messen, wer wo steht, kriecht (g & 0xf == 1, fl & 0x1000) und durchs
 * Tor ist. */
static void mess_zeile(void)
{
#ifdef RE15_PLATFORM_PC
    static int s_init = 0, s_n = 0, s_takt = 0;
    if (!s_init) { s_init = 1; const char *e = getenv("RE15_IT_LOG"); s_n = (e && *e) ? atoi(e) : 0; }
    if (s_n <= 0 || !re15_irons_tod_sub01_gesperrt() || (uint16_t)g_current_room_id == RE15_IT_RAUM_1150) { s_takt = 0; return; }
    if ((s_takt++ % s_n) != 0) return;
    fprintf(stderr, "[irons-tod-mess] ROOM%04X T%d cam=%d:", (unsigned)g_current_room_id, s_takt - 1, (int)g_scd.cam_id);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || !a->type) continue;
        fprintf(stderr, " %d:%02x(%d,%d)->(%d,%d) g=%02x s=%d st=%d fl=%04x mo=%d", i, a->type, (int)a->x, (int)a->z,
                (int)a->steer_x, (int)a->steer_z, a->grid_id, a->sub_state_1, a->member_0b, a->anim_flags, a->motion);
    }
    fprintf(stderr, " PL(%d,%d) hp=%d\n", (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].z,
            (int)g_actors[RE15_ACTOR_SLOT_PLAYER].hp);
#endif
}

/* BILANZ (Nachbesserung 1 M4; zaehlt und meldet "alle durch" als Signal (5,30), s.u.): waehrend des 1040-Schritts je Aktor
 * das erste Bild mit z < Tor-z (Rolltor-Objekt main00 @0x01168 `2d 00 .. 90 9d 00 00 98 fe` = (-25200,0,-360)),
 * NACHDEM er hinter dem Tor (z >= Tor-z) gesehen wurde = ein echter Durchgang (gemessen Lauf n1_offen: Record
 * @0x01224 steht in Bild 1 noch an seinem Raum-Platz z -11126 vor dem Tor, die Aufstellung folgt im Programm);
 * ueber die ganze Kette (Szene .. Rueckkehr) die Spieler-HP (Start und Minimum). Ausgabe im debug.log beim
 * Wechsel 1040 -> 1030 bzw. am Ende der Rueckkehr; die Riegel lesen dieselben Zahlen ueber
 * re15_irons_tod_bilanz_1040 / re15_irons_tod_hp. */
static int     s_b1040_bild = 0, s_b1040_n = 0;
static int16_t s_b1040_durch[RE15_ACTOR_MAX];
static uint8_t s_b1040_da[RE15_ACTOR_MAX];
static uint8_t s_b1040_hinten[RE15_ACTOR_MAX];
static int     s_hp_start = -1, s_hp_min = 0;
static uint8_t s_b1040_frei = 0, s_b1040_kappe = 0, s_b1040_halt = 0;

static void bilanz_takt(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (s_zustand != RE15_IT_AUS) {
        if (s_hp_start < 0) { s_hp_start = pl->hp; s_hp_min = pl->hp; }
        if (pl->hp < s_hp_min) s_hp_min = pl->hp;
    }
    if (s_zustand != RE15_IT_S1040 || (uint16_t)g_current_room_id != RE15_IT_RAUM_1040) return;
    if (s_b1040_bild == 0) { s_b1040_n = 0; s_b1040_frei = 0; s_b1040_kappe = 0; s_b1040_halt = 0; for (int i = 0; i < RE15_ACTOR_MAX; i++) { s_b1040_durch[i] = -1; s_b1040_da[i] = 0; s_b1040_hinten[i] = 0; } }
    s_b1040_bild++;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!a->active || a->type != 0x16) continue;
        if (!s_b1040_da[i]) { s_b1040_da[i] = 1; s_b1040_n++; }
        if (a->z >= RE15_IT_TOR_Z_1040) s_b1040_hinten[i] = 1;
        else if (s_b1040_hinten[i] && s_b1040_durch[i] < 0) s_b1040_durch[i] = (int16_t)s_b1040_bild;
        /* Schutz des Parkplatzes (re15_irons_tod.h): nur wer vorher HINTER dem Tor stand (gemessen: Record @0x01224 steht
         * in Bild 1 vor der Aufstellung noch bei z -11126 und darf nicht schon dort angehalten werden). */
        if (s_b1040_hinten[i] && a->z < RE15_IT_KAMERA_Z_1040 && !(a->grid_id & 0x20)) {
            g_actors[i].grid_id |= 0x20; s_b1040_halt++;
            log_it("1040: Zombie hinter der Kamera angehalten (entity+0x9 |= 0x20)");
        }
    }
    /* "Alle durch" -> Signal (5,30) fuer k_p_1040_ende (re15_irons_tod.h RE15_IT_SIG_ALLE_DURCH); Kappe als Sicherung. */
    if (!s_b1040_frei) {
        int n = 0, durch = re15_irons_tod_bilanz_1040(&n, NULL);
        if ((n > 0 && durch == n) || s_b1040_bild >= RE15_IT_1040_KAPPE) {
            s_b1040_frei = 1; s_b1040_kappe = (uint8_t)!(n > 0 && durch == n);
            re15_game_flag_set(5, RE15_IT_SIG_ALLE_DURCH, 1);
            log_it(s_b1040_kappe ? "1040: KAPPE erreicht, Schnitt ohne alle Durchgaenge" : "1040: alle durchs Tor (Signal (5,30))");
        }
    }
}

int re15_irons_tod_1040_kappe(void) { return s_b1040_kappe; }
int re15_irons_tod_1040_angehalten(void) { return s_b1040_halt; }

int re15_irons_tod_bilanz_1040(int *out_gesehen, int *out_letztes_bild)
{
    int durch = 0, letztes = -1;
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (s_b1040_da[i] && s_b1040_durch[i] >= 0) { durch++; if (s_b1040_durch[i] > letztes) letztes = s_b1040_durch[i]; }
    if (out_gesehen) *out_gesehen = s_b1040_n;
    if (out_letztes_bild) *out_letztes_bild = letztes;
    return durch;
}

void re15_irons_tod_hp(int *out_start, int *out_min)
{
    if (out_start) *out_start = s_hp_start;
    if (out_min) *out_min = s_hp_min;
}

void re15_irons_tod_bilanz_reset(void)
{
    s_b1040_bild = 0; s_b1040_n = 0; s_hp_start = -1; s_hp_min = 0; s_b1040_frei = 0; s_b1040_kappe = 0; s_b1040_halt = 0;
    for (int i = 0; i < RE15_ACTOR_MAX; i++) { s_b1040_durch[i] = -1; s_b1040_da[i] = 0; s_b1040_hinten[i] = 0; }
}

static void bilanz_1040_melden(void)
{
#ifdef RE15_PLATFORM_PC
    int n = 0, letztes = -1, durch = re15_irons_tod_bilanz_1040(&n, &letztes);
    fprintf(stderr, "[irons-tod] Bilanz 1040: %d Zombies, durchs Tor (z < %d) %d, letzter bei Bild %d von %d;",
            n, RE15_IT_TOR_Z_1040, durch, letztes, s_b1040_bild);
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (s_b1040_da[i]) fprintf(stderr, " %d@%d", i, (int)s_b1040_durch[i]);
    fprintf(stderr, "; Spieler-HP %d -> min %d; Kappe %d, hinter der Kamera angehalten %d\n", s_hp_start, s_hp_min,
            s_b1040_kappe, s_b1040_halt);
#endif
}

void re15_irons_tod_tick(void)
{
    mess_zeile();
    bilanz_takt();
    /* Marvins Marker-Alias (Aktor 4 spielt Adas Gesten-Record) JEDEN Takt setzen: auf dem Tuer-Weg bindet
     * main.c das Raum-RBJ erst NACH dem Installer (debug.log: "[irons-tod] ... Montage 11C0" vor "[rbj] room 11C0
     * cinematic overlay"), und re15_rbj_bind_room setzt die Aliase zurueck. Gemessen (Lauf v11c0, Bilder 340/370):
     * ohne den Alias stand Marvin bei "I have to help him, sorry!" mit haengenden Armen. Der Takt laeuft vor den
     * Faeden, der Binder loest Aktor 4 erst nach dessen Erscheinen auf (rbj_resolve_slot: !active -> kein Cache). */
    if (s_zustand == RE15_IT_S11C0 && (uint16_t)g_current_room_id == RE15_IT_RAUM_11C0) re15_rbj_set_alias(4, 1);
    /* Knall-Signale: (5,28) = Tuerknall (Knall-Bank Satz 0), (5,29) = Knall wie ROOM1030 (Satz 1). Nur waehrend
     * der Kette (in ROOM1150/1130/1040/1030 ist Bank 5 Wort 0 oberhalb von Bit 20 unbenutzt, Zensus Dossier §8.8). */
    if (s_zustand != RE15_IT_AUS) {
        if (re15_game_flag_get(5, RE15_IT_SIG_KNALL_TUER)) {
            re15_game_flag_set(5, RE15_IT_SIG_KNALL_TUER, 0);
            re15_audio_re2_tuer_se(KNALL_SE_TUER);
            log_it("Signal (5,28): Tuerknall (Knall-Bank Satz 0)");
        }
        if (re15_game_flag_get(5, RE15_IT_SIG_KNALL_1030)) {
            re15_game_flag_set(5, RE15_IT_SIG_KNALL_1030, 0);
            re15_audio_re2_tuer_se(KNALL_SE_1030);
            log_it("Signal (5,29): Knall wie ROOM1030 (Knall-Bank Satz 1)");
        }
    }
    if ((uint16_t)g_current_room_id == RE15_IT_RAUM_1150 && re15_game_flag_get(5, RE15_IT_SIG_ARM)) {
        re15_game_flag_set(5, RE15_IT_SIG_ARM, 0);
        irons_pose(RE15_IT_IRONS_CLIP_TOT, RE15_IT_IRONS_BILD_FALL, 0);
        log_it("Signal (5,12): Irons' Arm faellt (Clip 2 ab Bild 72)");
    }
    if (s_zustand == RE15_IT_RUECKKEHR && !programm_laeuft()) {
        s_zustand = RE15_IT_AUS;
        log_it("Rueckkehr beendet, Steuerung frei");
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[irons-tod] Bilanz Kette: Spieler-HP %d -> min %d\n", s_hp_start, s_hp_min);
#endif
    }
}

void re15_irons_tod_install(uint16_t room_id)
{
    switch (room_id) {
    case RE15_IT_RAUM_1150:
        if (s_zustand == RE15_IT_S11C0 || (s_zustand == RE15_IT_S1030 && re15_irons_tod_ohne_11c0())) {
            s_zustand = RE15_IT_RUECKKEHR;
            irons_pose(RE15_IT_IRONS_CLIP_TOT, RE15_IT_IRONS_BILD_TOT, 1);
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Rueckkehr: Programm 5");
            else { s_zustand = RE15_IT_AUS; log_it("Rueckkehr: KEIN Ereignis-Slot frei"); }
            return;
        }
        if (s_zustand != RE15_IT_AUS) { s_zustand = RE15_IT_AUS; log_it("Kette unterbrochen (1150)"); }
        if (re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_GESEHEN)) {
            irons_pose(RE15_IT_IRONS_CLIP_TOT, RE15_IT_IRONS_BILD_TOT, 1);
            log_it("(9,73)=1: Irons tot (Clip 2 Bild 89 gehalten)");
            return;
        }
        if (!re15_game_flag_get(RE15_IT_K_BANK, RE15_IT_K_BIT) ||
            !re15_game_flag_get(RE15_IT_IRONS_BANK, RE15_IT_IRONS_BIT)) return;
        if (!irons()) { log_it("kein Irons im Raum - Szene unterbleibt"); return; }
        re15_irons_tod_bilanz_reset();
        umzug_vorbereiten();
        meldungen_einsetzen(room_id);
        knallbank_laden();
        s_zustand = RE15_IT_SZENE;
        if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Szene startet (Programm 0)");
        else { s_zustand = RE15_IT_AUS; log_it("Szene: KEIN Ereignis-Slot frei"); }
        return;
    case RE15_IT_RAUM_1130:
        if (s_zustand == RE15_IT_SZENE && re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1130)) {
            s_zustand = RE15_IT_S1130;
            knallbank_laden();
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Montage 1130 (Programm 1)");
            return;
        }
        if (s_zustand != RE15_IT_AUS) { s_zustand = RE15_IT_AUS; log_it("Kette unterbrochen (1130)"); }
        if (re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1130))
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Nachspawn 1130 (Programm 10)");
        return;
    case RE15_IT_RAUM_1040:
        if (s_zustand == RE15_IT_S1130 ||
            (s_zustand == RE15_IT_SZENE && !re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1130))) {
            s_zustand = RE15_IT_S1040;
            knallbank_laden();
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Montage 1040 (Programm 2)");
            return;
        }
        if (s_zustand != RE15_IT_AUS) { s_zustand = RE15_IT_AUS; log_it("Kette unterbrochen (1040)"); }
        if (re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_1040))
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Nachspawn 1040 (Programm 14)");
        return;
    case RE15_IT_RAUM_1030:
        if (s_zustand == RE15_IT_S1040) {
            bilanz_1040_melden();
            s_zustand = RE15_IT_S1030;
            knallbank_laden();
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Montage 1030 (Programm 3)");
            return;
        }
        if (s_zustand != RE15_IT_AUS) { s_zustand = RE15_IT_AUS; log_it("Kette unterbrochen (1030)"); }
        if (re15_game_flag_get(RE15_IT_BANK, RE15_IT_BIT_GESEHEN))
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Nachspawn 1030 (Programm 11-13)");
        return;
    case RE15_IT_RAUM_11C0:
        if (s_zustand == RE15_IT_S1030) {
            s_zustand = RE15_IT_S11C0;
            re15_rbj_set_alias(4, 1);          /* Marvin (Aktor 4) spielt Adas Gesten-Record (Marker 2) */
            meldungen_einsetzen(room_id);
            if (scd_event_fire(RE15_IT_EREIGNIS) >= 0) log_it("Montage 11C0 (Programm 4)");
            return;
        }
        if (s_zustand != RE15_IT_AUS) { s_zustand = RE15_IT_AUS; log_it("Kette unterbrochen (11C0)"); }
        return;
    default:
        if (s_zustand != RE15_IT_AUS) { s_zustand = RE15_IT_AUS; log_it("Kette unterbrochen (fremder Raum)"); }
        return;
    }
}
