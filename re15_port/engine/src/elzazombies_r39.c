/*
 * Runde 39 Spur Q "elzazombies" — Elza: Zombie-Durchbrueche in der Panikphase. Herleitung, Belege und alle Konstanten:
 * include/re15_elzazombies_r39.h und analysis/befunde_runde39/Q_elzazombies.md.
 *
 * FORM = Leons Montage nach Irons' Tod (engine/src/irons_tod_1150.c, Runde 35 L; engine/src/montage_r38.c, Runde 38 O):
 * Raumwechsel in der Szene = Door_aot_set (0x3B) + Aot_on (0x47) "fire now" (LAB_800407bc, jalr Handler @0x8004082c ->
 * sce-2-Handler @0x800430bc -> Warp FUN_8001d600; Vorbilder ROOM1240 sub02/sub03 Slot 0, ROOM1080 sub07..10); der Spieler
 * steht auf einem Parkplatz hinter der Kamera; der Zielraum startet das naechste Programm nach dem Init-Lauf, der erste
 * Abschnitt (Spawns) laeuft im Raumaufbau vor dem ersten Bild (Form FUN_8003ef6c -> FUN_8003f0a0 @0x8003f018). Kein Asset-
 * Patch: die Programme sind portseitig eingespielter Bytecode aus ORIGINAL-Opcodes, jede Zeile mit Vorbild.
 * Nachtrag 11 (AUFTRAG.md): kein 1071-Schritt; statt dessen der Rolltor-Durchbruch in ROOM1041 = Leons 1040-Schritt, beim
 * naechsten Betreten von 1041, solange die Zombies aus 10C1/1061 leben (Programm G).
 */
#include "re15_elzazombies_r39.h"

#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_audio.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"     /* RE15_AI_GRID_SKIP = entity+0x9 Bit 0x20, re15_nav_zone_from_pos */
#include "re15_ai_flavor.h"    /* re15_re2z_owns_type: die Zombie-Familie */
#include "re15_collision.h"    /* re15_collision_band_from_y */
#include "re15_gameflow.h"     /* re15_char_variant: Elza-Bit */
#include "re15_irons_tod.h"    /* Leons Konstanten des 1040-Schritts (Parkplatz, Tor-z, Kappe) — nur gelesen */
#include "re15_map_hint.h"     /* re15_map_hint_pending */
#include "re15_room.h"         /* g_room_change */
#include "re15_scd.h"
#include "re15_stair.h"        /* re15_stair_active: Treppe zu Ende = Etage erreicht */
#include <math.h>
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#endif

#include "gen/knall_bank.inc"   /* k_knall_bank: Satz 0 Tuerknall (RE2 ROOM1060 Se_on 0x0a), Satz 1 Knall ROOM1030 Tone 7 */

extern unsigned g_current_room_id;

/* ---- Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c). Vorbilder je Form (dieselben wie Leons Programme):
 *   Set            `22 bb ii vv`                       ROOM1150 sub08 @0x01110
 *   Sleep          `09 0a nn nn`                       ROOM1150 sub08 @0x01162
 *   Work_set       `2e kk ii` + Nop                    ROOM1150 sub08 @0x0111E
 *   Cut_chg        `29 cc`                             ROOM1031 sub08 @0x02896 / ROOM1041 sub08 @0x01A1A
 *   Knall-Signal   `22 05 1c 01` / `22 05 1d 01`       Set(5,28)/Set(5,29) -> Knall-Bank (Takt), wie Leons Signale
 *   Sce_em_set     `44 ss tt gg ee 00 p6 pp xx xx yy yy zz zz 00 00 rr rr 00 00`  ROOM10E1 sub00 @0x00D56 (Etage ee =
 *                                                      pc[4], y = -ee*1800 wie ROOM1090 sub00 @0x02214)
 *   Save           `24 vv nn nn`                       ROOM1031 main00 @0x01E58 / ROOM1041 main00 @0x01218 (Limit)
 *   Door_aot_set   `3b ss 02 31 00 00 <Rechteck 8> <x y z gier> st rm ct ee 00..` ROOM10C1 main00 @0x00E42 (ee = Etage)
 *   Aot_on         `47 ss`                             ROOM1240 sub02 (Intro-Handoff), ROOM1080 sub07
 *   Plc_ret        `42` + Nop, Cut_auto `3c 01`        ROOM1150 sub08 @0x012E0 / @0x012E2; ROOM1041 sub11 @0x02234/@0x0223A
 *   Pos_set        `32 00 xx xx yy yy zz zz`           ROOM11B0 @0x012B2 (Umparken, Leons OP_PARK_1030_12)
 *   Member_set     `34 mm vv vv`                       ROOM11B0 @0x012BA / ROOM1031 sub08 @0x0289C
 *   Ankunfts-Poll  `11 00 08 00 02 00 12 04 21 05 bb 00` ROOM1150 sub08 @0x0112A..@0x01132 (Leons "alle durch")       */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_CUT(c)          0x29, (c)
#define OP_KNALL(sig)      0x22, 0x05, (sig), 0x01
#define OP_EM(s,t,g,et,p6,pp,x,y,z,rot) 0x44, (s), (t), (g), (et), 0x00, (p6), (pp), LE16(x), LE16(y), LE16(z), \
                                        0x00, 0x00, LE16(rot), 0x00, 0x00
#define OP_DOOR(x,y,z,yaw,rm,ct,et) 0x3b, RE15_R39Q_TUER_SLOT, 0x02, 0x31, 0x00, 0x00, 0,0,0,0,0,0,0,0, \
                               LE16(x), LE16(y), LE16(z), LE16(yaw), 0x00, (rm), (ct), (et), 0,0,0,0,0,0
#define OP_AOT_ON          0x47, RE15_R39Q_TUER_SLOT
#define OP_END             0x01, 0x00
/* Spieler versetzen: Work_set(1,0) + Nop + Pos_set + Member_set(4, Gier) = ROOM11B0 @0x012B2 (Leons OP_PARK_1030_12). */
#define OP_PARK(x,y,z,yaw) 0x2e, 0x01, 0x00, 0x00, 0x32, 0x00, LE16(x), LE16(y), LE16(z), 0x34, 0x04, LE16(yaw)
#define PARK_LEN           16
/* Wie Leon (Runde 36 Spur K, K3): der erste Abschnitt endet mit Sleep 1 (Sleeping zaehlt 1 -> 0 noch im Raumaufbau und
 * gibt ab, @0x8003f428/@0x8003f488), die Szenen-Riegel (ROOM1150 sub08 @0x01114/@0x01118) laufen im ersten Spielbild. */
#define OP_ABSCHNITT_ENDE  OP_SLEEP(1)
#define OP_RIEGEL          OP_SET(2, 7, 1), OP_SET(1, 27, 1)
#define DOOR_LEN           32
#define DOOR_OFF_X         14
#define DOOR_OFF_Y         16
#define DOOR_OFF_Z         18
#define DOOR_OFF_YAW       20
#define DOOR_OFF_ROOM      23
#define DOOR_OFF_CUT       24
#define DOOR_OFF_ETAGE     25
/* Raum-Bytes der Tuersaetze (dest_room = Raum-Byte, Variante xxx1 aus dem Elza-Bit, FUN_800396fc @0x800397ec): */
#define RB_1061            0x06   /* ROOM10C1 @0x00E42 Nutzlast +9 `06` */
#define RB_1031            0x03   /* ROOM1041 Tuer Slot 1 @0x010B8 Nutzlast +9 `03` (zur Lobby; Leons OP_DOOR 0x03) */

/* ---- Lagen ROOM1061 (Etage 4) ----------------------------------------------------------------------------------------
 * Freie Flaeche Etage 4 = SCA-Zellen mit floor-Byte 0x43 (ROOM1061 SCA @0x004A0, je Quadrant dieselben 7 Saetze):
 * Waende x 26900.. (@0x00584), z 26950.. (@0x00590), x ..19800 (@0x0059C), z ..18200 (@0x005B4); Treppenkern
 * x 19600..23400 z 20800..24900 (@0x005A8); Lauf-B-Block x 22900..27100 z 20000..24100 (@0x005C0). Gegner-Radius 400
 * (+0x78-Satz +6, FUN_8003a31c; Leons R38_NAEHER_HALB). Abstand zweier Gegner >= 800 (zwei Radien).
 *
 * MONTAGE-SPAWN (Cut 2) = die naechsten freien Punkte an der Tuer-Ankunft (26000,25300) (ROOM10C1 @0x00E42 Nutzlast
 * x `90 65`, z `d4 62`), dichteste Packung im 50er-Raster: (26000,25300), (25200,25300), (26000,24500), (26000,26100);
 * Blick = Eintrittsrichtung der Tuer, Gier 2048 (@0x00E42 Nutzlast `00 08`). Wie Leons 1130-Kopien "vor der Tuer".
 *
 * NACHSPAWN ("moeglichst weit weg an der Seite") = die weitesten freien Punkte vom GEMESSENEN Ankunftspunkt der Treppe
 * von unten (25023,24568) (Lauf e1061e, Bild 165: Lauf B Etage 2 -> 4, Dossier 1.2), Abstand >= 800: (20200,26550)
 * 5214, (20200,25750) 4966, (21000,26550) 4485, (20900,25350) 4197 — die Westseite unter dem Fenster. Blick zur
 * Ankunft (Gier = atan2(-dz,dx) in 4096tel: 254, 157, 298, 122).
 *
 * PARKPLATZ Cut 2 (Kamera RID @0x000A0 (20199,21273) -> (24670,27767)): hinter der Bildebene (Tiefe < 0) auf Etage 4,
 * Richtung Tuer -> Parkplatz moeglichst gleich Richtung Tuer -> Kamera (Leons Form: Ziel = der geparkte Spieler hinter
 * der Kamera, re15_montage_r38.h Baustein 2): (20200,20400), Tiefe -718, 5,4 Grad neben der Tuer->Kamera-Linie. Ausserhalb
 * der RVD-Zonen von Cut 2 (2->1 @0x001D8 z 21000..22000 ab x 19500..23000; 2->3 @0x001EC z 23000..24000). EINE BLK-Zone
 * (@0x00CF4) -> FUN_80039e7c steuert jeden Zombie direkt auf die Spielerlage (`bne a0,a1` @0x80039fbc). */
static const re15_r39q_lage_t k_lagen_montage[RE15_R39Q_N_10E1] = {
    { 26000, 25300, 2048 }, { 25200, 25300, 2048 }, { 26000, 24500, 2048 }, { 26000, 26100, 2048 },
};
static const re15_r39q_lage_t k_lagen_nach[RE15_R39Q_N_10E1] = {
    { 20200, 26550, 254 }, { 20200, 25750, 157 }, { 21000, 26550, 298 }, { 20900, 25350, 122 },
};
/* ROOM1041 = ROOM1040 in RID/SCA/BLK/RVD (Dossier 0b): Leons Aufstell-Gassen hinter dem Rolltor in BLK-Zone 0
 * (irons_tod_1150.c IT_AUF_FALL 0..3: Zone 0 x -27100..-23300, z > 1200, zwei Gassen der Toroeffnung, Gier 1024 = -z =
 * zum Tor; PORT-WAHL Runde 35 L) — hier die Plaetze der umgezogenen 1061-Kopien. */
static const re15_r39q_lage_t k_lagen_1041[RE15_R39Q_N_10E1] = {
    { -24700, 1600, 1024 }, { -25700, 2300, 1024 }, { -25100, 3200, 1024 }, { -24500, 4100, 1024 },
};
#define PARK_1061_X   20200
#define PARK_1061_Z   20400
#define PARK_1061_YAW 1024
/* ROOM1031 = ROOM1030 in RID Cut 0..12, SCA und BLK (Dossier 2.2): Leons Parkplaetze gelten Byte fuer Byte.
 * Tuer-Spawn (-28558,-2183) (re15_irons_tod.h RE15_IT_PARK_1030_X/_Z: Mitte BLK-Zone 7 hinter SCA 5 auf der Achse von
 * Cut 7; 1031: BLK Zone 7 @0x01C74, SCA 5 inhaltsgleich, RID @0x00140). Cut 12: (-12869,-11445) Gier 845 (re15_montage_r38.h
 * RE15_R38_PARK_1030_12_*: Kameraachse Cut 12 im Rolltor-Abstand 3985 hinter der Kamera; 1031 RID @0x001E0). */
#define PARK_1031_X      RE15_IT_PARK_1030_X
#define PARK_1031_Z      RE15_IT_PARK_1030_Z
#define PARK_1031_12_X   (-12869)
#define PARK_1031_12_Z   (-11445)
#define PARK_1031_12_YAW 845
/* ROOM1041: Leons 1040-Parkplatz (-24950,-14900) = Mitte des Schalter-Rechtecks Slot 4 (ROOM1041 main00 @0x01118
 * (-26000,-16000,2100,2200), Byte fuer Byte = ROOM1040 @0x01118), 3985 hinter der Kamera Cut 1 (re15_montage_r38.h). */
#define PARK_1041_X      RE15_IT_PARK_1040_X
#define PARK_1041_Z      RE15_IT_PARK_1040_Z
#define PARK_1041_YAW    1024

/* ---- Kopie-Tot-Bits (Zone 7). Zensus aller Sce_em_set in STAGE1-3 (132 RDTs, 176 Bits belegt, Dossier 2.4): frei sind
 * u.a. 100..105, 109..112, 117..119, 124..133, 144, 145; Leons Port-Kopien belegen 40..43, 46..49, 75..79, 83, 84, 90, 91,
 * 95. Gewaehlt: 124..127 (1061), 128..131 (1041, Nachtrag 11), 133, 144, 145 (Kriecher). */
static const uint8_t k_bits[RE15_R39Q_N_10E1 + RE15_R39Q_N_10E1 + RE15_R39Q_N_KRIECHER] = {
    124, 125, 126, 127,          /* 1061: Kopien der 10E1-Records 0xe0..0xe3 */
    128, 129, 130, 131,          /* 1041: dieselben Zombies nach dem Rolltor-Durchbruch */
    133, 144, 145,               /* 1031 Rollo: Kriecher */
};
#define BIT_1061(i)   k_bits[(i)]
#define BIT_1041(i)   k_bits[RE15_R39Q_N_10E1 + (i)]
#define BIT_KRIECH(i) k_bits[2 * RE15_R39Q_N_10E1 + (i)]
#define EM_ZONE 7

/* ================================================================================================
 * PROGRAMM A — AUFTAKT im Startraum (Spur P: ROOM1041): Riegel, kurze Pause, Knall des ersten Schritts, Tuer.
 * Zeiten = Leons Ende der 1150-Szene (k_p_szene: Cut 5 + Sleep 60 "kurze Pause", Knall, Sleep 45, Tuer). */
static const uint8_t k_p_auftakt[] = {
    OP_RIEGEL,                                             /* ROOM1150 sub08 @0x01114/@0x01118 */
    OP_SLEEP(60),                                          /* "kurze Pause" (Leon k_p_szene) */
    OP_KNALL(RE15_R39Q_SIG_KNALL_TUER), OP_SLEEP(45),      /* Knall des ersten Schritts (Satz wird gesetzt) */
    OP_DOOR(PARK_1061_X, RE15_R39Q_Y_1061, PARK_1061_Z, PARK_1061_YAW, RB_1061, RE15_R39Q_CUT_1061, RE15_R39Q_ETAGE_1061),
    OP_AOT_ON,
    OP_END,
};

/* PROGRAMM B — ROOM1061 Cut 2: die aus 10E1 uebrig gebliebenen Zombies stehen hinter der 10C1-Tuer (Raumaufbau, vor dem
 * ersten Bild — Leons K3-Form "erst Tuerknallen, dann Spawnen, dann Kamera"), DURCHBRUCH-FORM grid 0x0d (STAGE1.BIN
 * `ori v0,zero,0x201`/`sw v0,4(v1)` @0x80100f40-44; RE2-KI Deskriptor 6 = Gang `addiu v0,zero,257`/`sw v0,4(s2)`
 * @0x80100ab8-abc), Typ 0x10 und p6 01 wie die Quell-Records ROOM10E1 @0x00D56.., Etage 4. Zeiten wie Leons 1130-Schritt
 * (1 + 130 + 45 = 175 + 1 Bilder bis zur Tuer). Vier Saetze, das Gate @0x80042128-38 laesst nur die Ueberlebenden zu. */
#define EM_SATZ(et, y) OP_EM(0, 0x10, 0x0d, (et), 0x01, 0 /* Bit beim Bauen */, 0 /* x */, (y), 0 /* z */, 0 /* Gier */)
static const uint8_t k_p_1061_em[] = { EM_SATZ(RE15_R39Q_ETAGE_1061, RE15_R39Q_Y_1061) };   /* Vorlage (20 B) */
static const uint8_t k_p_1041_em[] = { EM_SATZ(0, 0) };                                     /* Vorlage (20 B) */
static const uint8_t k_p_1061_rest[] = {
    OP_ABSCHNITT_ENDE, OP_RIEGEL,
    OP_SLEEP(130),
    OP_KNALL(RE15_R39Q_SIG_KNALL_1030), OP_SLEEP(45),      /* Knall des Rollo-Schritts (Satz 1), noch auf Cut 2 */
    OP_DOOR(PARK_1031_X, 0, PARK_1031_Z, 0, RB_1031, RE15_R39Q_CUT_1031_TOR, 0),
    OP_AOT_ON,
    OP_END,
};

/* PROGRAMM C — ROOM1031: das Rollo = Leons Tor-Teil OHNE Cut-7-Teil (Leon: "1070 leer" -> der Knall kam am Ende des
 * vorigen Schritts, Bruch + Kriecher im Raumaufbau, das erste Bild zeigt Cut 12), mit den Zeilen von ROOM1031 sub08
 * (= ROOM1030 sub08 Byte fuer Byte, Versatz +0xEC; Dossier 2.2). Nachtrag 11: kein 1071-Schritt. */
static const uint8_t k_p_1031_kopf[] = {
    0x24, 0x12, LE16(20),                                   /* Save(0x12,20): Gleichzeitig-Limit (Form @0x01E58 `24 12 06 00`) */
};
/* Kriecher (Leons EM_KRIECHER_LISTE: Raum-Record-Form @0x01E5C Typ 0x16 grid 0x0d p6 01, im Warte-Rechteck Slot 5
 * @0x01D60 (-12900,-25300,9200,1100) und ausserhalb des Hauptraum-Rechtecks Slot 6 @0x01D74 z >= -24500). */
#define EM_KRIECHER \
    OP_EM(0, 0x16, 0x0d, 0, 0x01, 133,  -9500, 0, -24800, 3072), \
    OP_EM(0, 0x16, 0x0d, 0, 0x01, 144,  -7300, 0, -24800, 3072), \
    OP_EM(0, 0x16, 0x0d, 0, 0x01, 145, -11500, 0, -24800, 3072)
#define EM_KRIECHER_NACH \
    OP_EM(0, 0x16, 0x0d, 0, 0x01, 133,  -9500, 0, -19000, 1024), \
    OP_EM(0, 0x16, 0x0d, 0, 0x01, 144,  -7300, 0, -18500, 1024), \
    OP_EM(0, 0x16, 0x0d, 0, 0x01, 145, -11500, 0, -18800, 1024)
/* Das Rollo bricht — NUR wenn die Original-Szene noch nicht gelaufen ist ((4,15)=0); bei (4,15)=1 hat sub00
 * @0x020E4..@0x020F4 (Ck(4,15)=1 @0x020E0) dieselben Zeilen schon beim Raumaufbau ausgefuehrt, und Cut_replace ist ein
 * TAUSCH (LAB_80040414 @0x8004044c-a8) — ein zweiter Lauf tauschte zurueck (Leons Messung, Dossier r35 §8.3). */
static const uint8_t k_p_1031_bruch[] = {
    0x46, 0x11, 0x01, 0x31, 0x01, 0x00, 0xff, 0xff, 0x00, 0x00,               /* sub08 @0x02858 */
    0x4b, 0x00, 0x09, 0x4b, 0x03, 0x0a, 0x4b, 0x04, 0x0b, 0x4b, 0x06, 0x0c,   /* sub08 @0x0286E..@0x02877 */
    0x37, 0x02, 0x06, 0xf7, 0x37, 0x03, 0x06, 0xf7,                           /* sub08 @0x0287A/@0x0287E */
    OP_SET(4, 15, 1),                                                         /* sub01 @0x02284 */
};
/* Leons k_p_1030_kriechen mit den 1031-Zeilen: sub08 @0x02882/@0x02886 (Save var5/var7 = 0), viermal Gosub sub09
 * (@0x0288A/@0x02890/@0x028A0/@0x028A6; unmittelbar hintereinander wie bei Leon, gemessen r35 §8), Sleep 20 @0x02892,
 * Cut_chg 12 @0x02896, Umparken auf die Kameraachse Cut 12 (Leon Runde 38 O, Form ROOM11B0 @0x012B2), Sleep 15/20/180
 * @0x028A8/@0x028AC/@0x028B0, + 120 bis die Kriecher durch sind (Leon, gemessen r35 §8.3), Set(5,20) @0x028C6. */
static const uint8_t k_p_1031_kriechen[] = {
    EM_KRIECHER,
    OP_ABSCHNITT_ENDE, OP_RIEGEL, OP_SLEEP(1),
    0x24, 0x05, 0x00, 0x00, 0x24, 0x07, 0x00, 0x00,                           /* sub08 @0x02882/@0x02886 */
    0x18, 0x09, 0x18, 0x09, 0x18, 0x09, 0x18, 0x09,                           /* Gosub sub09 x4 */
    OP_SLEEP(20),                                                             /* sub08 @0x02892 */
    OP_CUT(RE15_R39Q_CUT_1031_TOR),                                           /* sub08 @0x02896 */
    OP_PARK(PARK_1031_12_X, 0, PARK_1031_12_Z, PARK_1031_12_YAW),
    OP_SLEEP(15), OP_SLEEP(20), OP_SLEEP(180),                                /* sub08 @0x028A8..@0x028B0 */
    OP_SLEEP(120),
    OP_SET(5, 20, 1),                                                         /* sub08 @0x028C6 */
    OP_DOOR(0, 0, 0, 0, 0x04, 0, 0),                                          /* -> Startraum (beim Bauen gesetzt) */
    OP_AOT_ON,
    OP_END,
};

/* PROGRAMM D — Rueckkehr im Startraum: Riegel, kurze Halte-Zeit wie Leons Rueckkehr (Sleep 45, k_p_rueck), Plc_ret,
 * Cut_auto 1, Riegel los (ROOM1150 sub08 @0x012E0/@0x012E2/@0x012E4/@0x012E8). Der Schnitt kommt aus dem Tuersatz
 * (gemerkter Schnitt beim Start). */
static const uint8_t k_p_rueck[] = {
    OP_RIEGEL,
    OP_SLEEP(45),
    OP_WORK(1, 0),
    0x42, 0x00,
    0x3c, 0x01,
    OP_SET(2, 7, 0), OP_SET(1, 27, 0),
    OP_END,
};

/* PROGRAMM E — Nachspawn ROOM1061: die Kopien an der Westseite (k_lagen_nach), Durchbruch-Form wie der Montage-Schritt
 * und wie Leons 1130-Nachspawn (Runde 37 H: grid 0x0d = aktiv). Gehalten vom Takt (entity+0x9 |= 0x20) bis Etage 4. */
static const uint8_t k_p_nach_ende[] = { OP_END };
/* PROGRAMM F — Nachspawn ROOM1031 (Leons k_p_1030_nach ohne die 1070-Kopien: Limit 20, Kriecher im Hauptraum). */
static const uint8_t k_p_1031_nach[] = { 0x24, 0x12, LE16(20), EM_KRIECHER_NACH, OP_END };

/* PROGRAMM G — DURCHBRUCH 1041 (Nachtrag 11) = Leons 1040-Schritt (irons_tod_1150.c k_p_1040_*), im Raum selbst:
 *   Raumaufbau: Limit 20 (Leons k_p_1030_kopf-Form; 1041 main00 @0x01218 `24 12 05 00`), die umgezogenen 1061-Kopien hinter
 *   dem Rolltor in Leons Aufstell-Gassen (k_lagen_1041), Elza unsichtbar auf Leons Parkplatz hinter der Kamera Cut 1
 *   (OP_PARK), erstes Bild Cut 1 (= ROOM1041 sub08 @0x01A1A `29 01`, Leons Rolltor-Schnitt).
 *   Spielbild 1..: Riegel, Knall wie ROOM1030 (Leon: "einen weiteren lauten Knall - so wie bei der Cutscene von ROOM 1030 -
 *   Dann ... Rolltor hoch"), Vorlauf 34 (Leon k_p_1040_vorlauf), Rolltor-Fahrt nur bei (4,5)=0 = ROOM1041 sub08
 *   @0x019DC..@0x01A50 ohne Elzas Drehung @0x019FC..@0x01A07 (Leons k_p_1040_tor, Versatz +0x2A), (4,5)=1 + (4,4)=1 wie sub01
 *   @0x015A4/@0x015B0, Warten auf "alle durch" (5,30) (Leons k_p_1040_ende), Sleep 90, Elza zurueck an ihren Eintrittspunkt,
 *   Schnitt zurueck, Plc_ret, Cut_auto 1, Riegel los (ROOM1041 sub11 @0x0222C..@0x0223A). */
static const uint8_t k_p_1041_kopf[] = { 0x24, 0x12, LE16(20) };
static const uint8_t k_p_1041_auftritt[] = {
    OP_PARK(PARK_1041_X, 0, PARK_1041_Z, PARK_1041_YAW),
    OP_CUT(RE15_R39Q_CUT_1041),
    OP_ABSCHNITT_ENDE, OP_RIEGEL,
    OP_KNALL(RE15_R39Q_SIG_KNALL_1030),
    OP_SLEEP(34),
};
static const uint8_t k_p_1041_tor[] = {
    0x36, 0x02, 0x0c, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* sub08 @0x019DC */
    OP_SLEEP(15),                                                             /* sub08 @0x019E8 */
    0x36, 0x02, 0x0a, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* sub08 @0x019EC */
    OP_SLEEP(15),                                                             /* sub08 @0x019F8 */
    OP_WORK(3, 0),                                                            /* sub08 @0x01A08: Objekt 0 = Rolltor */
    0x2f, 0x01, 0xd8, 0xff,                                                   /* sub08 @0x01A0C: Speed_set y -40 */
    0x0d, 0x00, 0x04, 0x00, LE16(20), 0x30, 0x02, 0x0e, 0x00,                 /* sub08 @0x01A10: For 20 { Add_speed; Evt_next } */
    0x29, 0x01,                                                               /* sub08 @0x01A1A: Cut_chg 1 */
    0x2f, 0x01, 0xd8, 0xff,                                                   /* sub08 @0x01A1C */
    0x0d, 0x00, 0x04, 0x00, LE16(50), 0x30, 0x02, 0x0e, 0x00,                 /* sub08 @0x01A20: For 50 */
    0x37, 0x02, 0x06, 0x00, 0x37, 0x03, 0x02, 0x00,                           /* sub08 @0x01A2A/@0x01A2E: Sca_id_set */
    0x39, 0x02, 0x06, 0x00, 0x39, 0x03, 0x02, 0x00,                           /* sub08 @0x01A32/@0x01A36: Sca_floor_set */
    0x0d, 0x00, 0x04, 0x00, LE16(65), 0x30, 0x02, 0x0e, 0x00,                 /* sub08 @0x01A3A: For 65 */
    0x36, 0x02, 0x0b, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   /* sub08 @0x01A44 */
    OP_SLEEP(10),                                                             /* sub08 @0x01A50 */
    OP_SET(4, 5, 1), OP_SET(4, 4, 1),                                         /* sub01 @0x015A4 / @0x015B0 */
};
static const uint8_t k_p_1041_ende[] = {
    0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, RE15_R39Q_SIG_ALLE_DURCH, 0x00,   /* Leons k_p_1040_ende */
    OP_SLEEP(90),
    OP_PARK(0, 0, 0, 0),                                                      /* Eintrittspunkt (beim Bauen gesetzt) */
    OP_CUT(0),                                                                /* Eintritts-Schnitt (beim Bauen gesetzt) */
    OP_WORK(1, 0), 0x42, 0x00,                                                /* ROOM1041 sub11 @0x02236/@0x0223A: Plc_ret */
    0x3c, 0x01,                                                               /* ROOM1041 sub11 @0x02234: Cut_auto 1 */
    OP_SET(2, 7, 0), OP_SET(1, 27, 0),                                        /* ROOM1041 sub11 @0x0222C/@0x02230 */
    OP_END,
};
/* PROGRAMM H — Nachspawn ROOM1041 nach dem Durchbruch: die 1041-Kopien in den Gassen hinter dem (offenen) Rolltor, Limit 20
 * (wie Leons k_p_1040_nach: die Durchgebrochenen sind beim spaeteren Betreten wieder da, solange sie leben). */

/* ---- Zustand ------------------------------------------------------------------------------------------------------- */
static int     s_zustand = RE15_R39Q_AUS;
static int     s_ausstehend = 0;      /* Flanke gesehen, Start wartet auf "keine Szene" */
static int     s_alt = -1;            /* (9,97) im letzten Spielbild (-1 = noch nie gelesen) */
static uint8_t s_prog[768];           /* RAM-Kopie, die die VM ausfuehrt */
static int     s_prog_len = 0;
static int     s_n10e1 = 0;           /* Ueberlebende in 10E1 beim Start (Kette) */
static uint16_t s_rueck_raum = 0;
static int32_t  s_rueck_x = 0, s_rueck_y = 0, s_rueck_z = 0;
static int16_t  s_rueck_gier = 0;
static uint8_t  s_rueck_cut = 0;
static int32_t  s_ein_x = 0, s_ein_y = 0, s_ein_z = 0;   /* Durchbruch 1041: Eintrittspunkt */
static int16_t  s_ein_gier = 0;
static uint8_t  s_ein_cut = 0;
static int      s_n1041 = 0;          /* Durchbruch 1041: umgezogene Kopien */
static int      s_d_bild = 0;         /* Durchbruch 1041: Spielbild der Szene */
static uint8_t  s_d_nord[RE15_ACTOR_MAX];   /* Durchbruch 1041: beim Szenenbeginn hinter dem Rolltor (z > Tor-z) */
static uint8_t  s_d_durch = 0;        /* Durchbruch 1041: (5,30) gesetzt */
static int     s_takt = 0;
static uint8_t s_halt_1061 = 0;       /* Nachspawn 1061: Kopien gehalten bis Etage 4 */
static uint8_t s_halt[RE15_ACTOR_MAX];/* von DIESER Regel angehalten (nur diese gibt frei) */
static int     s_an = 1;              /* Pruefhaken: 0 = Stand vor Spur Q (Gegenprobe der Riegel) */
static int     s_hp_kette = 1 << 30;  /* kleinste Spieler-HP ueber die ganze Kette */
static int32_t s_frei_px = 0, s_frei_pz = 0, s_frei_dmin = -1; static int s_frei_n = 0;   /* Freigabe 1061 */

static void log_q(const char *was)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[elzazombies-r39] ROOM%04X: %s\n", (unsigned)g_current_room_id, was);
#else
    (void)was;
#endif
}

/* ---- Ereignisfolge ------------------------------------------------------------------------------------------------- */
static re15_r39q_folge_t s_folge[64];
static int s_folge_n = 0, s_folge_gegner = -1, s_folge_cut = -1;
static void folge(char art, int wert)
{
    if (s_folge_n < (int)(sizeof s_folge / sizeof s_folge[0])) {
        re15_r39q_folge_t *f = &s_folge[s_folge_n];
        f->art = art; f->raum = (uint16_t)g_current_room_id; f->takt = s_takt; f->wert = wert;
    }
    s_folge_n++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[elzazombies-r39] Folge #%d %c ROOM%04X T%d %d\n", s_folge_n, art, (unsigned)g_current_room_id, s_takt,
            wert);
#endif
}
int re15_elzazombies_r39_folge_n(void) { return s_folge_n < 64 ? s_folge_n : 64; }
const re15_r39q_folge_t *re15_elzazombies_r39_folge(int i)
{ return (i >= 0 && i < re15_elzazombies_r39_folge_n()) ? &s_folge[i] : NULL; }

/* ---- Messung je Schnitt (wie Leons montage_r38.c: Abstand zur Kamera-Bodenprojektion im ersten/letzten Bild) ------ */
typedef struct { uint16_t raum; uint8_t cut; int32_t kx, kz, zx, zz; } kamera_t;
/* RID-Saetze (RDT camera_start 0x60 + Cut * 32: pos_x @+0x04, pos_z @+0x0C, target_x @+0x10, target_z @+0x18). */
static const kamera_t k_kameras[] = {
    { RE15_R39Q_RAUM_1061,  2,  20199,  21273,  24670,  27767 },   /* ROOM1061 @0x000A0 */
    { RE15_R39Q_RAUM_1031, 12, -11790, -15282,  -8640, -26478 },   /* ROOM1031 @0x001E0 */
    { RE15_R39Q_RAUM_1041,  1, -26262, -10692, -25020,  -1818 },   /* ROOM1041 @0x00080 (= ROOM1040, Leons Rolltor-Schnitt) */
};
#define N_KAMERAS ((int)(sizeof k_kameras / sizeof k_kameras[0]))
static const kamera_t *kamera(uint16_t raum, int cut)
{
    for (int i = 0; i < N_KAMERAS; i++) if (k_kameras[i].raum == raum && (int)k_kameras[i].cut == cut) return &k_kameras[i];
    return NULL;
}
typedef re15_r39q_spur_t spur_t;
static spur_t s_spur[64];
static int s_n_spur = 0, s_offen_von = 0, s_bild = 0, s_hp_min = 1 << 30;
static const kamera_t *s_offen = NULL;
static void abschnitt_schliessen(void)
{
    if (!s_offen) return;
#ifdef RE15_PLATFORM_PC
    for (int i = s_offen_von; i < s_n_spur; i++) {
        const spur_t *p = &s_spur[i];
        fprintf(stderr, "[elzazombies-r39] ROOM%04X Cut %d Bild %d: Aktor %d Bit %d Abstand zur Kamera %d -> %d (Bild %d..%d)"
                " Lage (%d,%d) -> (%d,%d) angehalten %d\n", (unsigned)p->raum, p->cut, s_bild, p->aktor, p->bit, (int)p->d0,
                (int)p->d1, p->b0, p->b1, (int)p->x0, (int)p->z0, (int)p->x1, (int)p->z1, p->halt);
    }
    fprintf(stderr, "[elzazombies-r39] ROOM%04X Cut %d Ende Bild %d: Spieler-HP min %d\n", (unsigned)s_offen->raum,
            s_offen->cut, s_bild, s_hp_min);
#endif
    s_offen = NULL;
}

/* ---- Hilfen -------------------------------------------------------------------------------------------------------- */
static int lebend_bits(const uint8_t *bits, int n)
{
    int k = 0;
    for (int i = 0; i < n; i++) if (!re15_game_flag_get(EM_ZONE, bits[i])) k++;
    return k;
}
int re15_elzazombies_r39_lebend_10e1(void)
{
    int k = 0;
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) if (!re15_game_flag_get(EM_ZONE, (uint8_t)(RE15_R39Q_BIT_10E1_0 + i))) k++;
    return k;
}
int re15_elzazombies_r39_lebend_1061(void) { return lebend_bits(&BIT_1061(0), RE15_R39Q_N_10E1); }
int re15_elzazombies_r39_lebend_1041(void) { return lebend_bits(&BIT_1041(0), RE15_R39Q_N_10E1); }

static int ist_elza(void) { return re15_char_variant() == 1; }

static int programm_laeuft(void)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= s_prog && t->pc < s_prog + sizeof s_prog) return 1;
    }
    return 0;
}

static int aktive_gegner_zahl(void)
{
    int n = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type) n++;
    return n;
}

/* Erster Abschnitt eines eben gestarteten Fadens SOFORT (Raumaufbau) — Leons erster_abschnitt_sofort (FUN_8003ef6c ->
 * FUN_8003f0a0 @0x8003f018 vor der Einblendung @0x8001cbb8..@0x8001cc28): die anderen Faeden ruhen dabei. */
static void erster_abschnitt_sofort(int slot)
{
    if (slot < 0 || slot >= SCD_THREAD_COUNT) return;
    uint8_t aktiv[SCD_THREAD_COUNT];
    for (int s = 0; s < SCD_THREAD_COUNT; s++) { aktiv[s] = g_scd.threads[s].active; if (s != slot) g_scd.threads[s].active = 0; }
    scd_vm_set_room_init(1);
    scd_vm_tick();
    scd_vm_set_room_init(0);
    for (int s = 0; s < SCD_THREAD_COUNT; s++) if (s != slot) g_scd.threads[s].active = aktiv[s];
}

static int starten(int sofort)
{
    int n0 = aktive_gegner_zahl();
    int slot = scd_event_fire(RE15_R39Q_EREIGNIS);
    if (slot >= 0 && sofort) {
        erster_abschnitt_sofort(slot);
        int n1 = aktive_gegner_zahl();
        if (s_zustand != RE15_R39Q_AUS) {
            if (n1 > n0) folge('S', n1 - n0);
            folge('C', (int)g_scd.cam_id);
        }
        s_folge_gegner = n1;
        s_folge_cut = (int)g_scd.cam_id;
    }
    return slot;
}

static void prog_anhaengen(const uint8_t *p, int n)
{
    if (s_prog_len + n > (int)sizeof s_prog) return;
    memcpy(s_prog + s_prog_len, p, (size_t)n);
    s_prog_len += n;
}

static int opcode_len(uint8_t op)
{
    switch (op) {
    case 0x00: return 1;  case 0x01: return 2;  case 0x09: return 4;  case 0x0d: return 6;  case 0x0e: return 2;
    case 0x11: return 4;  case 0x12: return 2;  case 0x18: return 2;  case 0x21: return 4;  case 0x22: return 4;
    case 0x24: return 4;  case 0x29: return 2;  case 0x2e: return 3;  case 0x2f: return 4;  case 0x30: return 1;
    case 0x02: return 1;  case 0x32: return 8;  case 0x34: return 4;  case 0x36: return 12; case 0x37: return 4;
    case 0x39: return 4;  case 0x3b: return 32; case 0x3c: return 2;  case 0x42: return 1;  case 0x44: return 20;
    case 0x46: return 10; case 0x47: return 2;  case 0x4b: return 3;
    default:   return 0;
    }
}
/* Freie Aktor-Plaetze fuer die Sce_em_set-Saetze in [von, s_prog_len) (Slot = Aktor - 1, scd_vm.c SCRIPT_SLOT_TO_ACTOR);
 * Leons slots_zuweisen. Kein Platz: Slot RE15_ACTOR_MAX-1 -> Aktor RE15_ACTOR_MAX = verworfen. */
static void slots_zuweisen(int von)
{
    int aktor = 1;
    for (int o = von; o < s_prog_len; ) {
        int n = opcode_len(s_prog[o]);
        if (n <= 0) break;
        if (s_prog[o] == 0x44) {
            while (aktor < RE15_ACTOR_MAX && g_actors[aktor].active) aktor++;
            if (aktor >= RE15_ACTOR_MAX) { s_prog[o + 7] = 0xff; s_prog[o + 1] = (uint8_t)(RE15_ACTOR_MAX - 1); }
            else { s_prog[o + 1] = (uint8_t)(aktor - 1); aktor++; }
        }
        o += n;
    }
}
/* Den LETZTEN Tuersatz des Programms setzen (Form `3b` + 31 Byte, gefolgt von Aot_on + Evt_end). */
static void tuer_setzen(int x, int y, int z, int yaw, uint8_t room, uint8_t cut, uint8_t etage)
{
    if (s_prog_len < DOOR_LEN + 4) return;
    uint8_t *d = s_prog + s_prog_len - 4 - DOOR_LEN;
    if (d[0] != 0x3b) return;
    d[DOOR_OFF_X] = (uint8_t)(x & 0xff);     d[DOOR_OFF_X + 1] = (uint8_t)((x >> 8) & 0xff);
    d[DOOR_OFF_Y] = (uint8_t)(y & 0xff);     d[DOOR_OFF_Y + 1] = (uint8_t)((y >> 8) & 0xff);
    d[DOOR_OFF_Z] = (uint8_t)(z & 0xff);     d[DOOR_OFF_Z + 1] = (uint8_t)((z >> 8) & 0xff);
    d[DOOR_OFF_YAW] = (uint8_t)(yaw & 0xff); d[DOOR_OFF_YAW + 1] = (uint8_t)((yaw >> 8) & 0xff);
    d[DOOR_OFF_ROOM] = room; d[DOOR_OFF_CUT] = cut; d[DOOR_OFF_ETAGE] = etage;
}
/* Das Knall-Signal VOR dem Tuersatz (Form `22 05 ss 01` + Sleep 45 + Door + Aot_on + Evt_end = 4 + 4 + 32 + 2 + 2). */
static void knall_setzen(uint8_t signal)
{
    uint8_t *k = s_prog + s_prog_len - 44;
    if (s_prog_len >= 44 && k[0] == 0x22 && k[1] == 0x05) k[2] = signal;
}
/* Einen OP_PARK-Satz ab `o` (16 B) mit Lage/Gier fuellen. */
static void park_setzen(int o, int32_t x, int32_t y, int32_t z, int16_t yaw)
{
    if (o < 0 || o + PARK_LEN > s_prog_len || s_prog[o] != 0x2e || s_prog[o + 4] != 0x32) return;
    uint8_t *p = s_prog + o;
    p[6] = (uint8_t)(x & 0xff);  p[7] = (uint8_t)((x >> 8) & 0xff);
    p[8] = (uint8_t)(y & 0xff);  p[9] = (uint8_t)((y >> 8) & 0xff);
    p[10] = (uint8_t)(z & 0xff); p[11] = (uint8_t)((z >> 8) & 0xff);
    p[14] = (uint8_t)(yaw & 0xff); p[15] = (uint8_t)((yaw >> 8) & 0xff);
}

/* Naechster Schritt nach dem Auftakt: 1061 (Tuerknall), wenn in 10E1 Zombies leben; sonst gleich das Rollo 1031 Cut 12
 * (Knall Satz 1 wie ROOM1030 — Leons 1040-Ende ohne Cut-7-Teil). Nach 1061 immer das Rollo (k_p_1061_rest). */
static void tuer_nach_auftakt(void)
{
    if (s_n10e1 > 0) {
        tuer_setzen(PARK_1061_X, RE15_R39Q_Y_1061, PARK_1061_Z, PARK_1061_YAW, RB_1061, RE15_R39Q_CUT_1061,
                    RE15_R39Q_ETAGE_1061);
        knall_setzen(RE15_R39Q_SIG_KNALL_TUER);
    } else {
        tuer_setzen(PARK_1031_X, 0, PARK_1031_Z, 0, RB_1031, RE15_R39Q_CUT_1031_TOR, 0);
        knall_setzen(RE15_R39Q_SIG_KNALL_1030);
    }
}

/* Vier Sce_em_set-Saetze (Montage/Nachspawn 1061, Durchbruch/Nachspawn 1041) anhaengen; das Gate @0x80042128-38
 * entscheidet ueber die Kopie-Tot-Bits. */
static void em_saetze_anhaengen(const uint8_t *vorlage, const uint8_t *bits, const re15_r39q_lage_t *lagen)
{
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) {
        int o = s_prog_len;
        prog_anhaengen(vorlage, 20);
        if (s_prog_len != o + 20) return;
        uint8_t *e = s_prog + o;
        e[7] = bits[i];
        e[8] = (uint8_t)(lagen[i].x & 0xff);      e[9] = (uint8_t)((lagen[i].x >> 8) & 0xff);
        e[12] = (uint8_t)(lagen[i].z & 0xff);     e[13] = (uint8_t)((lagen[i].z >> 8) & 0xff);
        e[16] = (uint8_t)(lagen[i].gier & 0xff);  e[17] = (uint8_t)((lagen[i].gier >> 8) & 0xff);
    }
}

/* Das Programm fuer (Raum, Zustand) in s_prog zusammensetzen; 0 = keins. */
static int programm_bauen(uint16_t room_id)
{
    s_prog_len = 0;
    switch (s_zustand) {
    case RE15_R39Q_AUFTAKT:
        if (room_id != s_rueck_raum) return 0;
        prog_anhaengen(k_p_auftakt, (int)sizeof k_p_auftakt);
        tuer_nach_auftakt();
        return 1;
    case RE15_R39Q_S1061:
        if (room_id != RE15_R39Q_RAUM_1061) return 0;
        em_saetze_anhaengen(k_p_1061_em, &BIT_1061(0), k_lagen_montage);
        slots_zuweisen(0);
        prog_anhaengen(k_p_1061_rest, (int)sizeof k_p_1061_rest);
        return 1;
    case RE15_R39Q_S1031: {
        if (room_id != RE15_R39Q_RAUM_1031) return 0;
        prog_anhaengen(k_p_1031_kopf, (int)sizeof k_p_1031_kopf);
        int von = s_prog_len;
        if (!re15_game_flag_get(4, 15)) prog_anhaengen(k_p_1031_bruch, (int)sizeof k_p_1031_bruch);
        prog_anhaengen(k_p_1031_kriechen, (int)sizeof k_p_1031_kriechen);
        slots_zuweisen(von);
        tuer_setzen(s_rueck_x, s_rueck_y, s_rueck_z, s_rueck_gier, (uint8_t)((s_rueck_raum >> 4) & 0xff), s_rueck_cut,
                    (uint8_t)re15_collision_band_from_y(s_rueck_y));
        return 1;
    }
    case RE15_R39Q_RUECKKEHR:
        if (room_id != s_rueck_raum) return 0;
        prog_anhaengen(k_p_rueck, (int)sizeof k_p_rueck);
        return 1;
    case RE15_R39Q_D1041: {
        if (room_id != RE15_R39Q_RAUM_1041) return 0;
        prog_anhaengen(k_p_1041_kopf, (int)sizeof k_p_1041_kopf);
        em_saetze_anhaengen(k_p_1041_em, &BIT_1041(0), k_lagen_1041);
        slots_zuweisen(0);
        prog_anhaengen(k_p_1041_auftritt, (int)sizeof k_p_1041_auftritt);
        if (!re15_game_flag_get(4, 5)) prog_anhaengen(k_p_1041_tor, (int)sizeof k_p_1041_tor);
        int e = s_prog_len;
        prog_anhaengen(k_p_1041_ende, (int)sizeof k_p_1041_ende);
        park_setzen(e + 12 + 4, s_ein_x, s_ein_y, s_ein_z, s_ein_gier);     /* nach Poll (12) + Sleep (4) */
        s_prog[e + 12 + 4 + PARK_LEN + 1] = s_ein_cut;                       /* Cut_chg danach */
        return 1;
    }
    case RE15_R39Q_AUS:
        if (!re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_MONTAGE)) return 0;
        if (room_id == RE15_R39Q_RAUM_1061) {
            em_saetze_anhaengen(k_p_1061_em, &BIT_1061(0), k_lagen_nach);
            slots_zuweisen(0);
            prog_anhaengen(k_p_nach_ende, (int)sizeof k_p_nach_ende);
            return 1;
        }
        if (room_id == RE15_R39Q_RAUM_1031) {
            prog_anhaengen(k_p_1031_nach, (int)sizeof k_p_1031_nach);
            slots_zuweisen(4);
            return 1;
        }
        if (room_id == RE15_R39Q_RAUM_1041) {                               /* Programm H */
            prog_anhaengen(k_p_1041_kopf, (int)sizeof k_p_1041_kopf);
            em_saetze_anhaengen(k_p_1041_em, &BIT_1041(0), k_lagen_1041);
            slots_zuweisen(4);
            prog_anhaengen(k_p_nach_ende, (int)sizeof k_p_nach_ende);
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}

const uint8_t *re15_elzazombies_r39_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (event_id != RE15_R39Q_EREIGNIS) return NULL;
    if (!ist_elza()) return NULL;
    if (programm_laeuft()) return NULL;
    if (!programm_bauen(room_id)) return NULL;
    return s_prog;
}

int re15_elzazombies_r39_sub01_gesperrt(void)
{
    /* Montage-Schritt / Durchbruch: das sub01 des Raums bleibt aus (FUN_8003f038 @0x8003f064-84, Leons Haken). ROOM1031
     * sub01 @0x0226C wuerde bei (3,116)=1/(4,15)=0 sonst die Original-Rollo-Szene sub08 neben dem Programm starten
     * (`04 ff 18 08` @0x02288); ROOM1041 sub01 @0x01590 startete auf dem Parkplatz = Schalter-Rechteck Slot 4 (Aot sce 4 ->
     * (5,33) `.. 21 00 01 00` @0x01118) bei (4,5)=0 die Original-Rolltor-Szene sub08 (`04 ff 18 08` @0x015B4). */
    return s_zustand == RE15_R39Q_S1061 || s_zustand == RE15_R39Q_S1031 || s_zustand == RE15_R39Q_D1041;
}

/* Beim Start der Kette: die 10E1-Ueberlebenden "ziehen um" (Leons umzug_vorbereiten): Quell-Record stillgelegt (Tot-Bit
 * 1), Kopie in 1061 scharf (Kopie-Bit 0); schon tote bekommen eine stille Kopie (Kopie-Bit 1). Die 1041-Kopien sind bis
 * zum Durchbruch still (Bit 1). Kriecher scharf. (9,95)=1. ROOM1071 bleibt unberuehrt (Nachtrag 11). */
static void umzug_vorbereiten(void)
{
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) {
        const uint8_t q = (uint8_t)(RE15_R39Q_BIT_10E1_0 + i);
        if (re15_game_flag_get(EM_ZONE, q)) re15_game_flag_set(EM_ZONE, BIT_1061(i), 1);
        else { re15_game_flag_set(EM_ZONE, q, 1); re15_game_flag_set(EM_ZONE, BIT_1061(i), 0); }
        re15_game_flag_set(EM_ZONE, BIT_1041(i), 1);
    }
    for (int i = 0; i < RE15_R39Q_N_KRIECHER; i++) re15_game_flag_set(EM_ZONE, BIT_KRIECH(i), 0);
    re15_game_flag_set(RE15_R39Q_BANK, RE15_R39Q_BIT_MONTAGE, 1);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[elzazombies-r39] Umzug: 10E1 lebend %d -> 1061; (9,95)=1\n", s_n10e1);
#endif
}

/* Durchbruch 1041: die lebenden 1061-Kopien ziehen nach 1041 um (1061-Bit 1, 1041-Bit 0). */
static void umzug_1041(void)
{
    s_n1041 = 0;
    for (int i = 0; i < RE15_R39Q_N_10E1; i++) {
        if (re15_game_flag_get(EM_ZONE, BIT_1061(i))) continue;
        re15_game_flag_set(EM_ZONE, BIT_1061(i), 1);
        re15_game_flag_set(EM_ZONE, BIT_1041(i), 0);
        s_n1041++;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[elzazombies-r39] Durchbruch 1041: %d Zombies aus 1061 (nicht besiegt) -> hinter das Rolltor\n", s_n1041);
#endif
}

static void knallbank_laden(void)
{
    if (!re15_audio_re2_tuer_laden(k_knall_bank, (int)KNALL_BANK_SIZE)) log_q("Knall-Tonbank nicht geladen");
}

/* "Keine Szene laeuft": die Rahmen-Flags jeder Original-Szene ((1,27) || (2,7), re15_cine_active), kein Skript-Spieler
 * (player_mode 2), kein Raumwechsel, kein angeforderter Kartenhinweis (re15_map_hint_pending: -1 = keiner). */
static int frei(void)
{
    return !re15_cine_active() && g_scd.player_mode != 2 && !g_room_change.pending && re15_map_hint_pending() < 0;
}

static void start_kette(void)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    s_rueck_raum = (uint16_t)g_current_room_id;
    s_rueck_x = pl->x; s_rueck_y = pl->y; s_rueck_z = pl->z; s_rueck_gier = pl->rot_y; s_rueck_cut = g_scd.cam_id;
    s_folge_n = 0; s_takt = 0; s_folge_gegner = -1; s_folge_cut = -1;
    s_n_spur = 0; s_offen = NULL; s_hp_min = 1 << 30; s_hp_kette = 1 << 30;
    /* Erst die Zahl (Programm A waehlt damit die erste Tuer), dann der Faden; erst wenn er laeuft, ziehen die Zombies um
     * und (9,95) wird gesetzt — sonst (alle Ereignis-Plaetze belegt) bliebe ein Umzug ohne Montage stehen. Kein Platz: der
     * Start bleibt ausstehend und wird im naechsten freien Bild wiederholt. */
    s_n10e1 = re15_elzazombies_r39_lebend_10e1();
    s_zustand = RE15_R39Q_AUFTAKT;
    if (scd_event_fire(RE15_R39Q_EREIGNIS) < 0) {
        s_zustand = RE15_R39Q_AUS;
        s_ausstehend = 1;
        log_q("Auftakt: KEIN Ereignis-Slot frei - Start bleibt ausstehend");
        return;
    }
    umzug_vorbereiten();
    knallbank_laden();
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[elzazombies-r39] Montage startet in ROOM%04X: Rueckkehr (%d,%d,%d) Gier %d Cut %d\n",
            (unsigned)s_rueck_raum, (int)s_rueck_x, (int)s_rueck_y, (int)s_rueck_z, (int)s_rueck_gier, (int)s_rueck_cut);
#endif
    log_q("Auftakt (Programm A)");
}

/* ---- Verhalten in den Schnitten (Leons Bausteine, re15_montage_r38.h) --------------------------------------------- */
/* Baustein 3: Einzelstopp hinter der Bildebene (entity+0x9 |= 0x20 -> Zombie-Takt uebersprungen, STAGE1.BIN `lbu v0,9(a0)`
 * @0x80100450 / `andi v0,v0,0x20` @0x80100458 / `bne v0,zero,0x80100658` @0x8010045c; Port RE15_AI_GRID_SKIP).
 * Baustein 4 (nur 1031 Cut 12): Regel von sub06 @0x025B4.. je Index i (Ck(5,i)=0 `21 05 i 00`, Work_set(2,i), Member_cmp
 * member 0x0F == 5 `3e 00 0f 00 05 00` @0x025CC, Gosub sub07 = member 0x10 |= 0x1000 `26 00 05 04 00 10` @0x02844 /
 * `35 10 04` @0x0284A, Set(5,i) `22 05 i 01` @0x025D4). (Baustein 5, das Warten hinter dem Rollo waehrend Cut 7, entfaellt
 * mit dem Cut-7-Teil: der 1031-Schritt zeigt von Anfang an Cut 12.) */
#define WARTE_STEMPEL 5          /* sub06 @0x025CC `3e 00 0f 00 05 00` */
#define KRIECH_BIT    0x1000u    /* sub07 @0x02844 `26 00 05 04 00 10` */
#define KRIECH_BANK   5          /* sub06 @0x025D4 `22 05 00 01` */
#define KRIECH_N      20         /* sub06 prueft die Indizes 0x00 .. 0x13 (wie ROOM1030 sub06 @0x024D4..@0x02734) */

static int schritt_raum(void)
{
    switch (s_zustand) {
    case RE15_R39Q_S1061: return RE15_R39Q_RAUM_1061;
    case RE15_R39Q_S1031: return RE15_R39Q_RAUM_1031;
    case RE15_R39Q_D1041: return RE15_R39Q_RAUM_1041;
    default:              return -1;
    }
}

static void montage_takt(void)
{
    const int schritt = schritt_raum();
    const kamera_t *k = NULL;
    if (schritt >= 0 && (int)(uint16_t)g_current_room_id == schritt) k = kamera((uint16_t)schritt, (int)g_scd.cam_id);
    if (k != s_offen) {
        abschnitt_schliessen();
        if (k) { s_offen = k; s_offen_von = s_n_spur; s_bild = 0; s_hp_min = 1 << 30; }
    }
    if (!k) return;
    s_bild++;
    {
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        if (pl->hp < s_hp_min) s_hp_min = pl->hp;
        if (pl->hp < s_hp_kette) s_hp_kette = pl->hp;
    }
    const double fx = (double)k->zx - k->kx, fz = (double)k->zz - k->kz;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        if (!a->active || !a->type || !re15_re2z_owns_type(a->type)) continue;
        const double dx = (double)a->x - k->kx, dz = (double)a->z - k->kz;
        const double tiefe = dx * fx + dz * fz;
        spur_t *p = NULL;
        for (int s = s_offen_von; s < s_n_spur; s++) if (s_spur[s].aktor == i && s_spur[s].bit == a->em_flag_id) { p = &s_spur[s]; break; }
        if (!p && s_n_spur < (int)(sizeof s_spur / sizeof s_spur[0])) {
            p = &s_spur[s_n_spur++];
            memset(p, 0, sizeof *p);
            p->raum = k->raum; p->cut = k->cut; p->aktor = (uint8_t)i; p->bit = a->em_flag_id; p->b0 = s_bild;
            p->d0 = (int32_t)lround(sqrt(dx * dx + dz * dz)); p->x0 = a->x; p->z0 = a->z;
        }
        if (p) { p->b1 = s_bild; p->d1 = (int32_t)lround(sqrt(dx * dx + dz * dz)); p->x1 = a->x; p->z1 = a->z; }
        if (tiefe < 0 && !(a->grid_id & RE15_AI_GRID_SKIP)) {        /* Baustein 3 */
            a->grid_id = (uint8_t)(a->grid_id | RE15_AI_GRID_SKIP);
            s_halt[i] = 1;
            if (p) p->halt = s_bild;
        }
    }
    if (k->raum == RE15_R39Q_RAUM_1031 && k->cut == RE15_R39Q_CUT_1031_TOR) {   /* Baustein 4 */
        for (int i = 0; i < KRIECH_N && i + 1 < RE15_ACTOR_MAX; i++) {
            re15_actor_t *a = &g_actors[i + 1];
            if (!a->active) continue;
            if (re15_game_flag_get(KRIECH_BANK, (uint8_t)i)) continue;
            if (a->member_0b != WARTE_STEMPEL) continue;
            a->anim_flags = (uint16_t)(a->anim_flags | KRIECH_BIT);
            re15_game_flag_set(KRIECH_BANK, (uint8_t)i, 1);
        }
    }
}

/* Durchbruch 1041: "alle durch" (Leons Signal (5,30), RE15_IT_SIG_ALLE_DURCH): jede umgezogene Kopie (Bits 128..131), die
 * beim Szenenbeginn hinter dem Rolltor stand (z > Tor-z -360 = Rolltor Objekt 0 @0x01190 (-25200,0,-360); Leons
 * RE15_IT_TOR_Z_1040), hat es durchquert — oder Leons Kappe RE15_IT_1040_KAPPE (1532 Bilder) ist erreicht. Die Raum-Records
 * hinter dem Rolltor zaehlen nicht mit: GEMESSEN (unit durch1041, erster Lauf) liefen drei von fuenf ueber die Kreuzung
 * VOM Tor weg (Leons Messung derselben Records, re15_irons_tod.h "Zone 5") — mit ihnen haette erst die Kappe die Szene
 * beendet. Die Lebend-Pruefung ueber die HP taugt im ersten Bild nicht (Sce_em_set setzt hp 0, die HP setzt erst die INIT
 * im ersten KI-Takt, scd_vm.c op_sce_em_set "a->hp = 0") — gemessen: "alle durch" schon in Bild 1. */
static void durchbruch_takt(void)
{
    if (s_zustand != RE15_R39Q_D1041 || (uint16_t)g_current_room_id != RE15_R39Q_RAUM_1041) return;
    s_d_bild++;
    if (s_d_bild == 1) {
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            int kopie = 0;
            for (int b = 0; b < RE15_R39Q_N_10E1; b++) if (a->em_flag_id == BIT_1041(b)) kopie = 1;
            s_d_nord[i] = (uint8_t)(a->active && a->type && kopie && a->z > RE15_IT_TOR_Z_1040);
        }
    }
    if (s_d_durch) return;
    int offen = 0;
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        const re15_actor_t *a = &g_actors[i];
        if (!s_d_nord[i]) continue;
        if (a->active && a->type && a->z > RE15_IT_TOR_Z_1040) offen++;
    }
    const int kappe = s_d_bild >= RE15_IT_1040_KAPPE;
    if (offen == 0 || kappe) {
        re15_game_flag_set(5, RE15_R39Q_SIG_ALLE_DURCH, 1);
        s_d_durch = 1;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[elzazombies-r39] ROOM1041: Durchbruch Bild %d: %s -> (5,%d)=1\n", s_d_bild,
                kappe && offen ? "KAPPE" : "alle durch das Rolltor", RE15_R39Q_SIG_ALLE_DURCH);
#endif
    }
}

/* Haltebits dieser Regeln loesen (Ende eines Schritts im Raum der Spielerin: Durchbruch 1041). */
static void halt_loesen(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        if (!s_halt[i]) continue;
        re15_actor_t *a = &g_actors[i];
        if (a->active) a->grid_id = (uint8_t)(a->grid_id & ~RE15_AI_GRID_SKIP);
        s_halt[i] = 0;
    }
}

/* Nachspawn ROOM1061: die Kopien stehen gehalten (entity+0x9 |= 0x20, Form ROOM1141 sub02 @0x00E70), bis die Spielerin
 * auf Etage 4 steht (Band = -(y/0x708), re15_collision.c; Ankunft ueber die Treppe von unten: Lauf B, Ende y -7200);
 * dann einmal frei (`34 0c 10 00`-Form @0x00F64), und frei bleiben sie fuer diesen Besuch. */
static void halten_1061(void)
{
    if (!s_halt_1061) return;
    if ((uint16_t)g_current_room_id != RE15_R39Q_RAUM_1061) { s_halt_1061 = 0; return; }
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* ERREICHT = auf Etage 4 UND die Treppe ist zu Ende (re15_stair_active = 0: Phase 2 "finalise, commit band, return to
     * normal state", stair_common.c, Handler LAB_80038850). Gemessen (Lauf t5/t6): das Band springt schon MITTEN im
     * Treppen-Clip auf 4 (bei z 23503, Ankunft z 24568) — frei ab dort, liefen die Kopien ~1 s, bevor Elza die Steuerung
     * hatte, und standen bei der Ankunft schon halb heran (Bild 540). */
    const int auf_etage = re15_collision_band_from_y(pl->y) == RE15_R39Q_ETAGE_1061 && !re15_stair_active();
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *a = &g_actors[i];
        int kopie = 0;
        for (int b = 0; b < RE15_R39Q_N_10E1; b++) if (a->em_flag_id == BIT_1061(b)) kopie = 1;
        if (!a->active || !a->type || !kopie) continue;
        if (auf_etage) a->grid_id = (uint8_t)(a->grid_id & ~RE15_AI_GRID_SKIP);
        else           a->grid_id = (uint8_t)(a->grid_id | RE15_AI_GRID_SKIP);
    }
    if (auf_etage) {
        s_halt_1061 = 0;
        s_frei_px = pl->x; s_frei_pz = pl->z; s_frei_dmin = -1; s_frei_n = 0;
        for (int i = 1; i < RE15_ACTOR_MAX; i++) {
            const re15_actor_t *a = &g_actors[i];
            int kopie = 0;
            for (int b = 0; b < RE15_R39Q_N_10E1; b++) if (a->em_flag_id == BIT_1061(b)) kopie = 1;
            if (!a->active || !kopie) continue;
            const int32_t d = (int32_t)lround(sqrt((double)(a->x - pl->x) * (a->x - pl->x) + (double)(a->z - pl->z) * (a->z - pl->z)));
            if (s_frei_dmin < 0 || d < s_frei_dmin) s_frei_dmin = d;
            s_frei_n++;
#ifdef RE15_PLATFORM_PC
            fprintf(stderr, "[elzazombies-r39] ROOM1061: Kopie Aktor %d Bit %d bei (%d,%d), Abstand zur Spielerin %d\n", i,
                    (int)a->em_flag_id, (int)a->x, (int)a->z, (int)d);
#endif
        }
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[elzazombies-r39] ROOM1061: Spielerin auf Etage 4 bei (%d,%d) -> Kopien frei, kleinster Abstand %d\n",
                (int)pl->x, (int)pl->z, (int)s_frei_dmin);
#endif
    }
}

int re15_elzazombies_r39_1061_gehalten(void) { return s_halt_1061; }

void re15_elzazombies_r39_tick(void)
{
    s_takt++;
    /* Flanke (9,97) 0 -> 1 im Spiel. Laden setzt s_alt (re15_elzazombies_r39_geladen), ein geladenes Bit ist keine Flanke. */
    const int b = re15_game_flag_get(RE15_R39Q_PANIK_BANK, RE15_R39Q_PANIK_BIT) ? 1 : 0;
    if (s_an && s_alt == 0 && b == 1 && ist_elza() && !re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_MONTAGE)) {
        s_ausstehend = 1;
#ifdef RE15_PLATFORM_PC
        fprintf(stderr, "[elzazombies-r39] Flanke (9,97) 0->1 in ROOM%04X Takt %d -> Montage ausstehend\n",
                (unsigned)g_current_room_id, s_takt);
#endif
    }
    s_alt = b;
    /* Start nicht in 1061/1031 selbst (eine Tuer in den eigenen Raum ist ein Teleport ohne Raumaufbau, aot_common.c) —
     * dort bleibt der Start ausstehend bis zum naechsten Raum. Spur P setzt (9,97) in ROOM1041. */
    if (s_ausstehend && s_zustand == RE15_R39Q_AUS && frei() &&
        (uint16_t)g_current_room_id != RE15_R39Q_RAUM_1061 && (uint16_t)g_current_room_id != RE15_R39Q_RAUM_1031) {
        s_ausstehend = 0;
        start_kette();
    }
    /* Folge: Spawns und Kamerawechsel waehrend eines Schritts */
    if (s_zustand == RE15_R39Q_S1061 || s_zustand == RE15_R39Q_S1031 || s_zustand == RE15_R39Q_D1041) {
        int n = aktive_gegner_zahl();
        if (s_folge_gegner >= 0 && n > s_folge_gegner) folge('S', n - s_folge_gegner);
        s_folge_gegner = n;
        if (s_folge_cut >= 0 && (int)g_scd.cam_id != s_folge_cut) folge('C', (int)g_scd.cam_id);
        s_folge_cut = (int)g_scd.cam_id;
    }
    if (s_zustand != RE15_R39Q_AUS) {
        /* MESS-HAKEN RE15_R39Q_KNALL_STUMM (kein Spielverhalten, Form Leons RE15_IT_KNALL_STUMM): Signal verbrauchen, aber
         * nicht spielen — die Differenz zweier RE15_AUDIO_CAP_SYNC-Mitschnitte (mit/ohne) ist genau der Knall. */
        int stumm = 0;
#ifdef RE15_PLATFORM_PC
        stumm = getenv("RE15_R39Q_KNALL_STUMM") ? 1 : 0;
#endif
        if (re15_game_flag_get(5, RE15_R39Q_SIG_KNALL_TUER)) {
            re15_game_flag_set(5, RE15_R39Q_SIG_KNALL_TUER, 0);
            if (!stumm) re15_audio_re2_tuer_se(KNALL_SE_TUER);
            log_q("Signal (5,28): Tuerknall (Knall-Bank Satz 0)");
            folge('K', KNALL_SE_TUER);
        }
        if (re15_game_flag_get(5, RE15_R39Q_SIG_KNALL_1030)) {
            re15_game_flag_set(5, RE15_R39Q_SIG_KNALL_1030, 0);
            if (!stumm) re15_audio_re2_tuer_se(KNALL_SE_1030);
            log_q("Signal (5,29): Knall wie ROOM1030 (Knall-Bank Satz 1)");
            folge('K', KNALL_SE_1030);
        }
    }
    montage_takt();
    durchbruch_takt();
    halten_1061();
    if (s_zustand == RE15_R39Q_RUECKKEHR && !programm_laeuft()) {
        s_zustand = RE15_R39Q_AUS;
        log_q("Rueckkehr beendet, Steuerung frei");
    }
    if (s_zustand == RE15_R39Q_D1041 && !programm_laeuft()) {
        abschnitt_schliessen();
        halt_loesen();
        s_zustand = RE15_R39Q_AUS;
        log_q("Durchbruch beendet, Steuerung frei (Haltebits geloest)");
    }
}

static void nachspawn(uint16_t room_id)
{
    if (!s_an || !ist_elza() || !re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_MONTAGE)) return;
    if (room_id == RE15_R39Q_RAUM_1041 && re15_elzazombies_r39_lebend_1041() == 0) return;
    if (room_id != RE15_R39Q_RAUM_1061 && room_id != RE15_R39Q_RAUM_1031 && room_id != RE15_R39Q_RAUM_1041) return;
    if (starten(1) < 0) return;
    if (room_id == RE15_R39Q_RAUM_1061) {
        s_halt_1061 = 1;
        halten_1061();   /* sofort: vor dem ersten KI-Takt */
        log_q("Nachspawn 1061 (Programm E): Kopien an der Westseite, gehalten bis Etage 4");
    } else if (room_id == RE15_R39Q_RAUM_1031) {
        log_q("Nachspawn 1031 (Programm F)");
    } else {
        log_q("Nachspawn 1041 (Programm H): die Durchgebrochenen hinter dem Rolltor");
    }
}

/* Durchbruch 1041 (Nachtrag 11): beim Betreten von 1041 NACH der Montage ((9,95)=1, Kette AUS — also nicht die Rueckkehr
 * der Montage selbst), solange mindestens eine 1061-Kopie lebt, und nur im Suedteil vor dem Rolltor (z < -360; Tueren
 * 1061/1031/Aufzug), damit die Zombies durchs Rolltor auf sie zukommen. */
static int s_laden = 0;   /* 1 = Aufruf aus dem CONTINUE-Weg (re15_elzazombies_r39_geladen) */
static int durchbruch_1041(uint16_t room_id)
{
    /* Nur der Tuer-Weg: der CONTINUE-Weg stellt den Schnitt des Spielstands NACH den Installern her (main.c "FE-4 CONTINUE:
     * restore the SAVE-TIME camera cut LAST") und ueberschriebe den Rolltor-Schnitt des ersten Bilds. Laden in 1041 laesst
     * den Durchbruch fuer das naechste Betreten durch eine Tuer stehen. */
    if (s_laden) return 0;
    if (!s_an || room_id != RE15_R39Q_RAUM_1041 || !ist_elza()) return 0;
    if (!re15_game_flag_get(RE15_R39Q_BANK, RE15_R39Q_BIT_MONTAGE)) return 0;
    if (re15_elzazombies_r39_lebend_1061() == 0) return 0;
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    if (pl->z >= RE15_IT_TOR_Z_1040) { log_q("Durchbruch 1041 wartet: Eintritt hinter dem Rolltor (Nordteil)"); return 0; }
    s_ein_x = pl->x; s_ein_y = pl->y; s_ein_z = pl->z; s_ein_gier = pl->rot_y; s_ein_cut = g_scd.cam_id;
    umzug_1041();
    s_folge_n = 0; s_takt = 0; s_folge_gegner = -1; s_folge_cut = -1;
    s_n_spur = 0; s_offen = NULL; s_hp_min = 1 << 30; s_hp_kette = 1 << 30;
    s_d_bild = 0; s_d_durch = 0; memset(s_d_nord, 0, sizeof s_d_nord);
    knallbank_laden();
    s_zustand = RE15_R39Q_D1041;
    folge('R', room_id);
    if (starten(1) < 0) { s_zustand = RE15_R39Q_AUS; log_q("Durchbruch 1041: KEIN Ereignis-Slot frei"); return 0; }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[elzazombies-r39] Durchbruch 1041 startet (Programm G): Eintritt (%d,%d,%d) Gier %d Cut %d, Rolltor %s\n",
            (int)s_ein_x, (int)s_ein_y, (int)s_ein_z, (int)s_ein_gier, (int)s_ein_cut,
            re15_game_flag_get(4, 5) ? "schon offen" : "faehrt hoch");
#endif
    return 1;
}

void re15_elzazombies_r39_install(uint16_t room_id)
{
    s_halt_1061 = 0;
    memset(s_halt, 0, sizeof s_halt);
    if (!ist_elza()) { s_zustand = RE15_R39Q_AUS; return; }
    if (s_zustand == RE15_R39Q_AUFTAKT && room_id == RE15_R39Q_RAUM_1061 && s_n10e1 > 0) {
        s_zustand = RE15_R39Q_S1061;
        knallbank_laden();
        folge('R', room_id);
        if (starten(1) >= 0) log_q("Montage 1061 (Programm B)");
        return;
    }
    if ((s_zustand == RE15_R39Q_AUFTAKT || s_zustand == RE15_R39Q_S1061) && room_id == RE15_R39Q_RAUM_1031) {
        abschnitt_schliessen();
        s_zustand = RE15_R39Q_S1031;
        knallbank_laden();
        folge('R', room_id);
        if (starten(1) >= 0) log_q("Montage 1031 (Programm C)");
        return;
    }
    if (s_zustand == RE15_R39Q_S1031 && room_id == s_rueck_raum) {
        abschnitt_schliessen();
        s_zustand = RE15_R39Q_RUECKKEHR;
        folge('R', room_id);
        if (scd_event_fire(RE15_R39Q_EREIGNIS) >= 0) log_q("Rueckkehr (Programm D)");
        else { s_zustand = RE15_R39Q_AUS; log_q("Rueckkehr: KEIN Ereignis-Slot frei"); }
        return;
    }
    if (s_zustand != RE15_R39Q_AUS) { s_zustand = RE15_R39Q_AUS; abschnitt_schliessen(); log_q("Kette unterbrochen"); }
    if (durchbruch_1041(room_id)) return;
    nachspawn(room_id);
}

void re15_elzazombies_r39_geladen(uint16_t room_id)
{
    s_zustand = RE15_R39Q_AUS;
    s_ausstehend = 0;
    s_alt = re15_game_flag_get(RE15_R39Q_PANIK_BANK, RE15_R39Q_PANIK_BIT) ? 1 : 0;   /* Laden ist keine Flanke */
    s_laden = 1;
    re15_elzazombies_r39_install(room_id);
    s_laden = 0;
}

/* ---- Pruefhaken ---------------------------------------------------------------------------------------------------- */
int  re15_elzazombies_r39_zustand(void) { return s_zustand; }
void re15_elzazombies_r39_zustand_setzen(int z) { s_zustand = z; }
int  re15_elzazombies_r39_ausstehend(void) { return s_ausstehend; }
const uint8_t *re15_elzazombies_r39_laufprogramm(int *out_len) { if (out_len) *out_len = s_prog_len; return s_prog; }
int  re15_elzazombies_r39_kopiebits(const uint8_t **out) { if (out) *out = k_bits; return (int)sizeof k_bits; }
int  re15_elzazombies_r39_lagen(int welche, const re15_r39q_lage_t **out)
{
    if (out) *out = welche == 2 ? k_lagen_1041 : welche ? k_lagen_nach : k_lagen_montage;
    return RE15_R39Q_N_10E1;
}
void re15_elzazombies_r39_park_1061(int32_t *x, int32_t *z) { if (x) *x = PARK_1061_X; if (z) *z = PARK_1061_Z; }
void re15_elzazombies_r39_rueck(uint16_t *raum, int32_t *x, int32_t *y, int32_t *z, int16_t *gier, uint8_t *cut)
{
    if (raum) *raum = s_rueck_raum; if (x) *x = s_rueck_x; if (y) *y = s_rueck_y; if (z) *z = s_rueck_z;
    if (gier) *gier = s_rueck_gier; if (cut) *cut = s_rueck_cut;
}
int  re15_elzazombies_r39_durchbruch_zahl(void) { return s_n1041; }
void re15_elzazombies_r39_reset(void)
{
    s_zustand = RE15_R39Q_AUS; s_ausstehend = 0; s_alt = -1; s_prog_len = 0; s_n10e1 = 0;
    s_rueck_raum = 0; s_rueck_x = s_rueck_y = s_rueck_z = 0; s_rueck_gier = 0; s_rueck_cut = 0;
    s_ein_x = s_ein_y = s_ein_z = 0; s_ein_gier = 0; s_ein_cut = 0; s_n1041 = 0; s_d_bild = 0; s_d_durch = 0;
    memset(s_d_nord, 0, sizeof s_d_nord);
    s_takt = 0; s_halt_1061 = 0; memset(s_halt, 0, sizeof s_halt);
    s_folge_n = 0; s_folge_gegner = -1; s_folge_cut = -1; s_n_spur = 0; s_offen = NULL;
    s_hp_kette = 1 << 30; s_frei_px = s_frei_pz = 0; s_frei_dmin = -1; s_frei_n = 0;
}
void re15_elzazombies_r39_enable(int an) { s_an = an ? 1 : 0; }
int  re15_elzazombies_r39_spuren(const re15_r39q_spur_t **out) { if (out) *out = s_spur; return s_n_spur; }
void re15_elzazombies_r39_abschnitt_beenden(void) { abschnitt_schliessen(); }
int  re15_elzazombies_r39_hp_min(void) { return s_hp_kette; }
int  re15_elzazombies_r39_freigabe(int32_t *px, int32_t *pz, int32_t *dmin)
{
    if (px) *px = s_frei_px; if (pz) *pz = s_frei_pz; if (dmin) *dmin = s_frei_dmin;
    return s_frei_n;
}
