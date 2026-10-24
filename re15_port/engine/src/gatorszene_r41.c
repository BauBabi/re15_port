/*
 * RE1.5 Rebuilt — Runde 41 Spur F "gatorszene" (ROOM2090/2091). Herleitung, Belege und Konstanten:
 * include/re15_gatorszene_r41.h, Dossier analysis/befunde_runde41/F_gatorszene.md.
 */
#include "re15_gatorszene_r41.h"

#include <stddef.h>
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* ==== 1. Spinnen ================================================================================================ */
static int s_spinnen_weg = 1;
static int s_spinnen_verworfen = 0;

int re15_gatorszene_r41_spinnen_weg_setzen(int an)
{
    int alt = s_spinnen_weg;
    s_spinnen_weg = an ? 1 : 0;
    return alt;
}

int re15_gatorszene_r41_spinnen_verworfen(void) { return s_spinnen_verworfen; }

int re15_gatorszene_r41_spawn_weg(uint16_t room_id, uint8_t type, int16_t x, int16_t z)
{
    if (!s_spinnen_weg) return 0;
    if ((uint16_t)(room_id & 0xFFFEu) != RE15_GSZ_RAUM || type != RE15_GSZ_SPINNE_TYP) return 0;
    if (!((x == RE15_GSZ_SPINNE0_X && z == RE15_GSZ_SPINNE0_Z) ||
          (x == RE15_GSZ_SPINNE1_X && z == RE15_GSZ_SPINNE1_Z))) return 0;      /* Satz-Waechter: nur die RDT-Saetze */
    s_spinnen_verworfen++;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] ROOM%04X: Spinne 0x25 bei (%d,%d) verworfen (Nutzer: \"Entferne die Spinnen aus dem "
                    "ROOM 2090\"; Form = Kill-Flag-Tor @0x80042120-38)\n", (unsigned)room_id, (int)x, (int)z);
#endif
    return 1;
}

/* ==== 2. Ankunftsszene ============================================================================================ */
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_engine.h"
#include "re15_enemy.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_gator_re2_r41.h"
#include "re15_boss_gator.h"
#include "re15_blink_r41.h"
#include "re15_math.h"
#include <string.h>
#ifdef RE15_PLATFORM_PC
#include <stdlib.h>
#endif

/* ---- Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c). Vorbild je Form:
 *   Set            `22 bb ii vv`                         ROOM1150 sub08 @0x01110
 *   Sleep          `09 0a nn nn`                         ROOM1150 sub08 @0x01162
 *   Cut_chg        `29 cc`                               ROOM1150 sub08 @0x01178
 *   Work_set       `2e 01 00` + Nop, Plc_ret `42` + Nop  ROOM1150 sub08 @0x012E0 (Rueckgabe des Spielers)
 *   Cut_auto       `3c 01`                               ROOM1150 sub08 @0x012E2
 *   Ankunfts-Poll  `11 00 08 00 02 00 12 04 21 05 bb 00` ROOM1150 sub08 @0x0112A..@0x01132 (Bit bb statt 0x20)
 *   Sce_em_set     `44 ss tt gg ff 00 p6 kk xx xx yy yy zz zz 00 00 rr rr 00 00` ROOM2020 main00 @0x008F6 (pc[4] = Band)
 *   Door_aot_set   `3b ss 02 31 00 00 <Rechteck 0> <x y z gier> st rm ct ..` ROOM2090 main00 @0x00996 (Nutzlast)
 *   Aot_on         `47 ss`                               ROOM1240 sub02 (Intro-Handoff), ROOM1080 sub07 */
#define GSZ_LE16(v)        (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define GSZ_SET(b,i,v)     0x22, (b), (i), (v)
#define GSZ_SLEEP(n)       0x09, 0x0a, GSZ_LE16(n)
#define GSZ_CUT(c)         0x29, (c)
#define GSZ_CUT_AUTO       0x3c, 0x01
#define GSZ_PLC_RET        0x2e, 0x01, 0x00, 0x00, 0x42, 0x00
#define GSZ_WAIT(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define GSZ_EM(s,t,g,p6,x,y,z,band,rot) 0x44, (s), (t), (g), (band), 0x00, (p6), 0xff, GSZ_LE16(x), GSZ_LE16(y), \
                                        GSZ_LE16(z), 0x00, 0x00, GSZ_LE16(rot), 0x00, 0x00
#define GSZ_DOOR           0x3b, RE15_GSZ_TUER_SLOT, 0x02, 0x31, 0x00, 0x00, 0,0,0,0,0,0,0,0, \
                           0,0, 0,0, 0,0, 0,0, 0x01, 0x00, 0x00, 0x00, 0,0,0,0,0,0          /* Ziel per tuer_setzen */
#define GSZ_AOT_ON         0x47, RE15_GSZ_TUER_SLOT
#define GSZ_END            0x01, 0x00
#define GSZ_RIEGEL         GSZ_SET(2, 7, 1), GSZ_SET(1, 27, 1)   /* ROOM1150 sub08 @0x01114/@0x01118 */
#define GSZ_RIEGEL_AUF     GSZ_SET(2, 7, 0), GSZ_SET(1, 27, 0)   /* ROOM1150 sub08 @0x012E4/@0x012E8 */
#define GSZ_ABSCHNITT_ENDE GSZ_SLEEP(1)                          /* irons_tod_1150.c OP_ABSCHNITT_ENDE (Riegel im ersten Spieltakt) */
#define GSZ_DOOR_LEN       32
#define GSZ_DOOR_X         14   /* Nutzlast +0 (scd_vm.c op_door_aot_set nl[0]) */
#define GSZ_DOOR_Y         16
#define GSZ_DOOR_Z         18
#define GSZ_DOOR_YAW       20
#define GSZ_DOOR_STAGE     22
#define GSZ_DOOR_ROOM      23
#define GSZ_DOOR_CUT       24

/* ---- PORT-WAHL der Inszenierung (Nachtrag 6 "Betreff Aligator darfst du alles machen was du willst. Du darfst auch die
 * Startposition des Aligators und die Position der schwimmenden Zombie Leiche ... aendern"), gemessen am Bild (Dossier §1): */
#define GSZ_CUT_LEICHE        0      /* ROOM2090 Cut 0 (RID @0x060): zeigt die Leiche obj 2 von Osten; Eintritts-Cut von 2080
                                      * (ROOM2080 main00 @0x00BC2 Nutzlast-Cut 0) - "sieht die Zombie Leiche" */
#define GSZ_LEICHE_BILDER     90     /* Halt auf der Leiche (1,5 Blinkperioden 0x3c, re15_blink_r41.h) */
#define GSZ_LEICHE2_BILDER    45     /* "erneut Kamera auf Zombie mit Schluessel" vor dem Gator-Signal */
#define GSZ_MONTAGE_BILDER    150    /* Zombies kommen die Treppe herauf (2080 Cut 0) */
/* 2080-Montage: Kamera Cut 0 (RID ROOM2080 @0x060 pos (-18060,-16688,-17526) -> (-21498,-7096,-16344)) = von oben die Treppe
 * zur 2030-Tuer; Spieler geparkt HINTER der Kamera (Lauf p2080b: (-16182,-14400,-17000) in Cut 0 unsichtbar). */
#define GSZ_2080_CUT          0
#define GSZ_2080_PARK_X       (-16182)
#define GSZ_2080_PARK_Y       (-14400)  /* Band 8 = -8 * 0x708 (ROOM2090 Tuer 0 Nutzlast-y C7C0) */
#define GSZ_2080_PARK_Z       (-17000)
#define GSZ_2080_PARK_YAW     2048

/* ---- Zustand ---------------------------------------------------------------------------------------------------- */
static int      s_zustand = RE15_GSZ_AUS;
static int32_t  s_leon_x, s_leon_y, s_leon_z;    /* Eintrittslage in 2090 (Rueckkehr-Tuersatz) */
static int16_t  s_leon_yaw;
static uint8_t  s_prog[640];
static int      s_prog_len = 0;

#define GSZ_FOLGE_MAX 64
static re15_gsz_folge_t s_folge[GSZ_FOLGE_MAX];
static int s_folge_n = 0;
static void folge(char art, int wert, int wert2)
{
    if (s_folge_n < GSZ_FOLGE_MAX) {
        re15_gsz_folge_t *f = &s_folge[s_folge_n++];
        f->art = art; f->raum = (uint16_t)g_current_room_id; f->bild = (unsigned)g_engine.frame_count;
        f->wert = wert; f->wert2 = wert2;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Folge %c ROOM%04X F%u wert=%d/%d zustand=%d\n", art, (unsigned)g_current_room_id,
            (unsigned)g_engine.frame_count, wert, wert2, s_zustand);
#endif
}
int re15_gatorszene_r41_folge_n(void) { return s_folge_n; }
const re15_gsz_folge_t *re15_gatorszene_r41_folge(int i) { return (i >= 0 && i < s_folge_n) ? &s_folge[i] : NULL; }
int re15_gatorszene_r41_zustand(void) { return s_zustand; }

/* ---- Programme --------------------------------------------------------------------------------------------------- */
/* PROGRAMM 1 - ROOM2090/2091 Ankunft: Riegel, Cut 0 auf die Leiche mit dem blinkenden Schluessel, dann Montage nach 2080. */
static const uint8_t k_p1[] = {
    GSZ_ABSCHNITT_ENDE, GSZ_RIEGEL,
    GSZ_CUT(GSZ_CUT_LEICHE),
    GSZ_SLEEP(GSZ_LEICHE_BILDER),
    GSZ_DOOR, GSZ_AOT_ON,
    GSZ_END,
};
/* PROGRAMM 2 - ROOM2080/2081 Montage: acht Zombies kommen durch die 2030-Tuer (Treppenfuss) und gehen die Treppe herauf.
 * Satzform der Zombies, die durch eine geoeffnete Tuer/ein Tor kommen: grid 0x0d = ENGAGE ab INIT (STAGE1.BIN `ori v0,zero,
 * 0x201` / `sw v0,4(v1)` @0x80100f40-44; Raum-Records ROOM1040 main00 @0x011FC, ROOM1030 main00 @0x01DE6; Port-Vorbild
 * irons_tod_1150.c EM_1130_DURCH), Kill-Flag 0xff = kein Tor (@0x80042128) - Szenenfiguren nur fuer diese Einstellung.
 * Typen 0x16/0x10/0x11/0x12 = Zombie-Typen der STAGE2-Wurzel 0x801002b8 (Rosterscan wasserzombie_r38.c), 0x16 = der
 * STAGE2-Raum-Zombie (ROOM2000/2020). Lage = Treppenfuss vor der 2030-Tuer (Tuer-Rechteck 2080 Slot 0 @0x00BA2
 * x -24900..-23900, z -18700..-16700; Ankunft aus 2030 (-23200,-9000,-16700) Gier 0, ROOM2030 main00 @0x01AEA), Band 5. */
#define GSZ_2080_ZOMBIES \
    GSZ_EM(0, 0x16, 0x0d, 0x00, -23200,  -9000, -16700, 5, 0), \
    GSZ_EM(1, 0x10, 0x0d, 0x01, -23700,  -9000, -17400, 5, 0), \
    GSZ_EM(2, 0x11, 0x0d, 0x01, -23700,  -9000, -16000, 5, 0), \
    GSZ_EM(3, 0x16, 0x0d, 0x01, -24300,  -9000, -16700, 5, 0), \
    GSZ_EM(4, 0x12, 0x0d, 0x00, -24300,  -9000, -17700, 5, 0), \
    GSZ_EM(5, 0x10, 0x0d, 0x00, -24900,  -9000, -16200, 5, 0), \
    GSZ_EM(6, 0x11, 0x0d, 0x00, -24900,  -9000, -17300, 5, 0), \
    GSZ_EM(7, 0x16, 0x0d, 0x01, -25400,  -9000, -16700, 5, 0)
static const uint8_t k_p2[] = {
    GSZ_2080_ZOMBIES,                                    /* Raumaufbau: vor dem ersten Bild (Form EM_1130_DURCH) */
    GSZ_ABSCHNITT_ENDE, GSZ_RIEGEL,
    GSZ_CUT(GSZ_2080_CUT),
    GSZ_SET(5, RE15_GSZ_SIG_TUER, 1),                    /* Metalltuer faellt hinter ihnen zu (Port-Takt, Door_exit DOOR06) */
    GSZ_SLEEP(GSZ_MONTAGE_BILDER),
    GSZ_DOOR, GSZ_AOT_ON,
    GSZ_END,
};
/* PROGRAMM 3 - ROOM2090/2091 Rueckkehr: erneut die Leiche, Gator-Signal, warten, Szene gesehen, Riegel auf. */
static const uint8_t k_p3[] = {
    GSZ_ABSCHNITT_ENDE, GSZ_RIEGEL,
    GSZ_CUT(GSZ_CUT_LEICHE),
    GSZ_SLEEP(GSZ_LEICHE2_BILDER),
    GSZ_SET(5, RE15_GSZ_SIG_GATOR, 1),
    GSZ_WAIT(RE15_GSZ_SIG_FERTIG),
    GSZ_SET(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN, 1),
    GSZ_PLC_RET,
    GSZ_CUT_AUTO,
    GSZ_RIEGEL_AUF,
    GSZ_END,
};
const uint8_t *re15_gatorszene_r41_programm(int nr, int *out_len)
{
    const uint8_t *p = NULL; int n = 0;
    if (nr == 1) { p = k_p1; n = (int)sizeof k_p1; }
    else if (nr == 2) { p = k_p2; n = (int)sizeof k_p2; }
    else if (nr == 3) { p = k_p3; n = (int)sizeof k_p3; }
    if (out_len) *out_len = n;
    return p;
}

static void tuer_setzen(int door_off, int32_t x, int32_t y, int32_t z, int yaw, uint8_t room, uint8_t cut)
{
    uint8_t *d = s_prog + door_off;
    d[GSZ_DOOR_X] = (uint8_t)(x & 0xff); d[GSZ_DOOR_X + 1] = (uint8_t)((x >> 8) & 0xff);
    d[GSZ_DOOR_Y] = (uint8_t)(y & 0xff); d[GSZ_DOOR_Y + 1] = (uint8_t)((y >> 8) & 0xff);
    d[GSZ_DOOR_Z] = (uint8_t)(z & 0xff); d[GSZ_DOOR_Z + 1] = (uint8_t)((z >> 8) & 0xff);
    d[GSZ_DOOR_YAW] = (uint8_t)(yaw & 0xff); d[GSZ_DOOR_YAW + 1] = (uint8_t)((yaw >> 8) & 0xff);
    d[GSZ_DOOR_STAGE] = 0x01;          /* STAGE2 (Nutzlast +8, ROOM2090 @0x00996 `01 08`) */
    d[GSZ_DOOR_ROOM]  = room;          /* Raum-Byte; die Elza-Variante waehlt der Lader (ROOM2091 @0x00996 traegt ebenfalls `01 08`) */
    d[GSZ_DOOR_CUT]   = cut;
}

static int programm_bauen(int nr)
{
    int n = 0;
    const uint8_t *p = re15_gatorszene_r41_programm(nr, &n);
    if (!p || n > (int)sizeof s_prog) return 0;
    memcpy(s_prog, p, (size_t)n);
    s_prog_len = n;
    const int door_off = n - (GSZ_DOOR_LEN + 2 + 2);       /* Door_aot_set + Aot_on + Evt_end am Ende (Programme 1/2) */
    if (nr == 1)
        tuer_setzen(door_off, GSZ_2080_PARK_X, GSZ_2080_PARK_Y, GSZ_2080_PARK_Z, GSZ_2080_PARK_YAW, 0x08, GSZ_2080_CUT);
    else if (nr == 2)
        tuer_setzen(door_off, s_leon_x, s_leon_y, s_leon_z, s_leon_yaw, 0x09, GSZ_CUT_LEICHE);
    return 1;
}
const uint8_t *re15_gatorszene_r41_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (event_id != RE15_GSZ_EREIGNIS) return NULL;
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    int nr = 0;
    if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_ANKUNFT) nr = 1;
    else if (basis == 0x2080u && s_zustand == RE15_GSZ_MONTAGE) nr = 2;
    else if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_RUECKKEHR) nr = 3;
    if (!nr || !programm_bauen(nr)) return NULL;
    folge('P', nr, 0);
    return s_prog;
}

static int prop_slot(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* ==== 3. Schluessel ================================================================================================ */
#include "gen/gatorszene_r41_schluessel.inc"   /* tools/r41_gatorszene/schluessel_export.py */

/* Prop-Felder wie Obj_model_set (LAB_80040914): flags pc[6..7] | 1 (@0x80040990-a4), Band pc[4] -> pool+0x82 (@0x8004096c-74).
 * attr 0 = NCCT-beleuchtet (FUN_8002c18c @0x8002c2ec-f8): Voraussetzung des Helligkeitspulses (re15_blink_r41.h). */
#define GSZ_KEY_FLAGS     0x000B    /* ROOM2090 obj 2 @0x00A6C pc[6..7] `0a 00` | 1 */
#define GSZ_KEY_BAND      0
/* Lage auf dem Ruecken der Leiche, Objektraum von obj 2 (Elternobjekt, Anhaenge-Form 0xC0|n @0x80040a84-9c): Leichen-Modell
 * ROOM2090 Prop 2 MD1 @0x2054 x -444..2626, y -341..966 -> Rumpfmitte x 1090, Oberseite y -341. PORT-WAHL am Bild. */
#define GSZ_KEY_AUF_X     1090
#define GSZ_KEY_AUF_Y     (-360)
#define GSZ_KEY_AUF_Z     0
/* Item-Zone nach dem Tod (re15_aot_set_item_tk_prop = Item_aot_set @0x80040644-Form), Rechteck 1000 x 1000 wie ROOM2090 main00
 * @0x00A12 (`e8 03 e8 03`), AOT-Platz 15: frei in 2090 (main00 0..5, Kamerazonen 48..63). */
#define GSZ_KEY_AOT       15
#define GSZ_KEY_RECHTECK  1000
#define GSZ_WASSER_Y      (-1620)   /* Wasseroberflaeche: sce-8-Zone p0 ROOM2090 main00 @0x009D6 pc[14..15] `ac f9` */
/* Ersatzlage nach dem Laden eines Spielstands (die Todeslage lebt nur im RAM): die Liegestelle der Leiche (GSZ_LEICHE_X/Z). */
#define GSZ_KEY_ERSATZ_X  (-5300)
#define GSZ_KEY_ERSATZ_Z  (-21000)

static int32_t s_key_x = 0, s_key_z = 0;
static int     s_key_lage_ok = 0;

int re15_gatorszene_r41_obj_id(uint16_t room_id)
{
    return ((room_id & 0xFFFEu) == RE15_GSZ_RAUM) ? RE15_GSZ_KEY_OBJ : -1;
}
const uint8_t *re15_gatorszene_r41_md1_bytes(uint16_t room_id, int *n)
{
    if (re15_gatorszene_r41_obj_id(room_id) < 0) { if (n) *n = 0; return NULL; }
    if (n) *n = (int)sizeof re15_gsz_key_md1;
    return re15_gsz_key_md1;
}
const uint8_t *re15_gatorszene_r41_tim_bytes(uint16_t room_id, int *n)
{
    if (re15_gatorszene_r41_obj_id(room_id) < 0) { if (n) *n = 0; return NULL; }
    if (n) *n = (int)sizeof re15_gsz_key_tim;
    return re15_gsz_key_tim;
}
const uint8_t *re15_gatorszene_r41_name(uint8_t item_id) { return item_id == RE15_GSZ_KEY_ITEM ? re15_gsz_key_name : NULL; }
const uint8_t *re15_gatorszene_r41_desc(uint8_t item_id) { return item_id == RE15_GSZ_KEY_ITEM ? re15_gsz_key_desc : NULL; }
int re15_gatorszene_r41_ist_schluessel(uint8_t item_id) { return item_id == RE15_GSZ_KEY_ITEM; }
void re15_gatorszene_r41_icon_einsetzen(uint8_t *itemall, int size)
{
    if (itemall && (RE15_GSZ_KEY_ITEM + 1) * 1200 <= size)
        memcpy(itemall + RE15_GSZ_KEY_ITEM * 1200, re15_gsz_key_icon, 1200);
}
void re15_gatorszene_r41_bild_einsetzen(uint8_t *itps, int size)
{
    if (itps && (RE15_GSZ_KEY_ITEM + 1) * 0x3000 <= size)
        memcpy(itps + RE15_GSZ_KEY_ITEM * 0x3000, re15_gsz_key_itps, 0x3000);
}

/* Prop obj 4 anlegen (falls noch nicht da) - Felder wie dining_r38.c schluessel_1011. */
static int key_prop(void)
{
    int k = prop_slot(RE15_GSZ_KEY_OBJ);
    if (k < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        k = (int)g_scd.prop_count++;
        memset(&g_scd.props[k], 0, sizeof g_scd.props[k]);
        g_scd.props[k].obj_id   = RE15_GSZ_KEY_OBJ;
        g_scd.props[k].obj_type = 0;
        g_scd.props[k].flags    = GSZ_KEY_FLAGS;
        g_scd.props[k].band     = GSZ_KEY_BAND;
        g_scd.props[k].attr     = 0;
    }
    return k;
}
/* Waehrend die Szene aussteht: Schluessel auf dem Ruecken der Leiche, Kind von obj 2 (wippt mit ihr, sub02 @0x00AF0). */
static void key_auf_leiche(uint16_t room_id)
{
    int k = key_prop();
    if (k < 0) return;
    g_scd.props[k].active = 1;
    g_scd.props[k].parent_obj = RE15_GSZ_LEICHE_OBJ;
    g_scd.props[k].x = GSZ_KEY_AUF_X; g_scd.props[k].y = GSZ_KEY_AUF_Y; g_scd.props[k].z = GSZ_KEY_AUF_Z;
    g_scd.props[k].rot_x = 0; g_scd.props[k].rot_y = 0; g_scd.props[k].rot_z = 0;
    re15_blink_r41_setzen(room_id, RE15_GSZ_KEY_OBJ, 1);
}
/* Nach dem Tod: Schluessel liegt bei der Leiche (Weltraum), Item-Zone mit Taken-Bit 112 und Prop-Verstecken. */
static void key_bei_leiche(uint16_t room_id)
{
    if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GENOMMEN)) return;
    int k = key_prop();
    if (k < 0) return;
    if (!s_key_lage_ok) { s_key_x = GSZ_KEY_ERSATZ_X; s_key_z = GSZ_KEY_ERSATZ_Z; }
    g_scd.props[k].active = 1;
    g_scd.props[k].parent_obj = -1;
    g_scd.props[k].x = s_key_x; g_scd.props[k].y = GSZ_WASSER_Y; g_scd.props[k].z = s_key_z;
    g_scd.props[k].rot_x = 0; g_scd.props[k].rot_y = 0; g_scd.props[k].rot_z = 0;
    re15_blink_r41_setzen(room_id, RE15_GSZ_KEY_OBJ, 1);
    re15_aot_set_item_tk_prop(GSZ_KEY_AOT, s_key_x, s_key_z, GSZ_KEY_RECHTECK / 2, GSZ_KEY_RECHTECK / 2,
                              RE15_GSZ_KEY_ITEM, 1, RE15_GSZ_BIT_GENOMMEN, RE15_GSZ_KEY_OBJ);
    g_aot.slots[GSZ_KEY_AOT].band = 0;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] ROOM%04X: Water Key (Item 0x%02X, Bits (9,%d)/(9,%d)) bei der Leiche (%d,%d,%d), Zone Slot %d\n",
            (unsigned)room_id, RE15_GSZ_KEY_ITEM, RE15_GSZ_BIT_LIEGT, RE15_GSZ_BIT_GENOMMEN, s_key_x, GSZ_WASSER_Y, s_key_z,
            GSZ_KEY_AOT);
#endif
}

/* ---- Raumaufbau -------------------------------------------------------------------------------------------------- */
/* Erster Abschnitt eines eben gestarteten Fadens SOFORT (Raumaufbau): Form elzazombies_r39.c / irons_tod_1150.c
 * erster_abschnitt_sofort = Init-Lauf FUN_8003ef6c -> FUN_8003f0a0 @0x8003f018 vor der Einblendung @0x8001cbb8..@0x8001cc28. */
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


/* Leiche obj 2: Lage der Szene (PORT-WAHL nach Nachtrag 6, Dossier §1.3) - nur waehrend die Szene noch aussteht. */
#define GSZ_LEICHE_X      (-5300)   /* Mitte des Westkanals x -8900..-1700 (Wasserzonen ROOM2090 main00 @0x009D6/@0x009FE) */
#define GSZ_LEICHE_Z      (-21000)  /* Suedbecken z -27000..-18100 (@0x009D6), im Bild von Cut 0 (Eintritts-Cut aus 2080) */
#define GSZ_LEICHE_ROT_Y  1024      /* quer zur Anschwimmrichtung des Gators (Gier 1024 = -z, Konvention cut10f0.h) */
static void leiche_lage(void)
{
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    if (k < 0) return;
    g_scd.props[k].x = GSZ_LEICHE_X;
    g_scd.props[k].z = GSZ_LEICHE_Z;
    g_scd.props[k].rot_y = GSZ_LEICHE_ROT_Y;
}
static void leiche_weg(void)
{
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    if (k >= 0) g_scd.props[k].active = 0;
    k = prop_slot(RE15_GSZ_KEY_OBJ);
    if (k >= 0) g_scd.props[k].active = 0;
}

void re15_gatorszene_r41_install(uint16_t room_id)
{
    const uint16_t basis = (uint16_t)(room_id & 0xFFFEu);
    if (basis == RE15_GSZ_RAUM) {
        if (s_zustand == RE15_GSZ_MONTAGE) {                /* zurueck aus 2080: Programm 3 */
            s_zustand = RE15_GSZ_RUECKKEHR;
            leiche_lage();
            key_auf_leiche(room_id);
            if (scd_event_fire(RE15_GSZ_EREIGNIS) < 0) { s_zustand = RE15_GSZ_AUS; folge('X', 3, 0); return; }
            return;
        }
        if (s_zustand != RE15_GSZ_AUS) { folge('X', s_zustand, 1); s_zustand = RE15_GSZ_AUS; }
        if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN)) {
            leiche_weg();                                   /* gefressen */
            if (re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT)) key_bei_leiche(room_id);
            return;
        }
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        s_leon_x = pl->x; s_leon_y = pl->y; s_leon_z = pl->z; s_leon_yaw = pl->rot_y;
        s_folge_n = 0;
        leiche_lage();
        key_auf_leiche(room_id);
        s_zustand = RE15_GSZ_ANKUNFT;
        folge('A', (int)s_leon_x, (int)s_leon_z);
        if (scd_event_fire(RE15_GSZ_EREIGNIS) < 0) { s_zustand = RE15_GSZ_AUS; folge('X', 1, 0); }
        return;
    }
    if (basis == 0x2080u) {
        if (s_zustand == RE15_GSZ_ANKUNFT) {                /* Montage-Schritt: Programm 2 */
            s_zustand = RE15_GSZ_MONTAGE;
            int slot = scd_event_fire(RE15_GSZ_EREIGNIS);
            if (slot < 0) { s_zustand = RE15_GSZ_AUS; folge('X', 2, 0); return; }
            erster_abschnitt_sofort(slot);
            int n = 0;
            for (int i = 1; i < RE15_ACTOR_MAX; i++) if (g_actors[i].active && g_actors[i].type) n++;
            folge('M', n, (int)g_scd.cam_id);
            return;
        }
        if (s_zustand != RE15_GSZ_AUS) { folge('X', s_zustand, 2); s_zustand = RE15_GSZ_AUS; }
        return;
    }
    if (s_zustand != RE15_GSZ_AUS) { folge('X', s_zustand, 3); s_zustand = RE15_GSZ_AUS; }
}

/* ---- Gator-Ablauf ------------------------------------------------------------------------------------------------ */
enum { GP_AUS = 0, GP_DURCHBRUCH, GP_BISS, GP_SCHUETTELN, GP_KAUEN, GP_WENDE, GP_SCHREI, GP_ENDE };
static struct {
    int     phase;
    int     t;                      /* Bilder in der Phase */
    int32_t anker_x, anker_z;       /* FRESSEN-Anker (FUN_80015b94 @0x80101110-34): Wurzel - R*s*off */
    int     leiche_dran;            /* Leiche haengt am Kopf (ab Clip-4-Bild 13) */
    int     leiche_weg;             /* verschlungen */
    int     kampf_gemeldet;
} s_ga;
int re15_gatorszene_r41_gator_phase(void) { return s_ga.phase; }

static inline int32_t q12m(int32_t a, int32_t b) { return (int32_t)(((int64_t)a * b) >> 12); }
static int32_t ga_scale(const re15_actor_t *e) { return e->render_scale_q12 ? e->render_scale_q12 : 4096; }

/* Keyframe-Index eines Clip-Bildes (0x8000-Marker = Tween-Wort; Form gator_re2_r41.c gr2_kf). */
static int ga_kf(const re15_emd_animation_t *an, int clip, int frame)
{
    if (!an || clip < 0 || clip >= an->clip_count) return -1;
    const re15_emd_clip_t *c = &an->clips[clip];
    if (frame < 0 || frame >= c->frame_count) return -1;
    int fi = c->first_frame + frame, fend = c->first_frame + c->frame_count;
    while ((an->frames[fi] & 0x8000u) && fi + 1 < fend) fi++;
    return (int)(an->frames[fi] & 0xfffu);
}
/* Kumulierter Wurzelversatz (Keyframe-Bytes 6..11, FUN_80015e7c) eines Clip-Bildes, Rig-Raum. */
static void ga_off(const re15_enemy_bank_t *bk, int clip, int frame, int32_t o[3])
{
    int16_t a = 0, b = 0, c = 0;
    o[0] = o[1] = o[2] = 0;
    int kf = ga_kf(&bk->anim, clip, frame);
    if (kf < 0) return;
    re15_emd_get_keyframe_speed(&bk->skel, kf, &a, &b, &c);
    o[0] = a; o[1] = b; o[2] = c;
}
/* Rig-Vektor -> Welt-Versatz (x' = c*x + s*z, z' = -s*x + c*z wie re15_skel_bone_to_world), mit Zeichen-Skala. */
static void ga_rig_welt(const re15_actor_t *e, int yaw, const int32_t v[3], int32_t out[2])
{
    int32_t s = ga_scale(e);
    int32_t c = re15_cos_q12(yaw), sn = re15_sin_q12(yaw);
    int32_t vx = q12m(v[0], s), vz = q12m(v[2], s);
    out[0] = (int32_t)(((int64_t)c * vx + (int64_t)sn * vz) >> 12);
    out[1] = (int32_t)((-(int64_t)sn * vx + (int64_t)c * vz) >> 12);
}
/* Clipwechsel FUN_8001a330(em, clip, n, 0): Bild 0, Ueberblendung n (Zaehler +0x14E = n @0x8001a364, Schritt 4096/(n+1)
 * @0x8001a37c-ac; Abbau je Takt im Boss-Takt @0x800299c0-cc) - Form gator_re2_r41.c gr2_blend. */
static void ga_clip(re15_actor_t *e, int clip, int frame, int n)
{
    e->motion = (int16_t)clip;
    e->anim_frame = frame;
    e->anim_frac = (uint8_t)n;
    e->anim_blend_rate = (uint16_t)(n > 0 ? 4096 / (n + 1) : 0);
}
/* Ein Bild authored Wurzelbewegung (FUN_8001a330 -> FUN_80015e7c + FUN_800152c8): Delta off[f] - off[f-1] gedreht und
 * skaliert auf die Wurzel; Bild 0 = Neu-Anker ohne Schritt (gator_re2_r41.c gr2_wurzel_schritt / gr2_clip_tick).
 * Rueckgabe 2 = letztes Bild liegt an. */
static int ga_schritt(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int clip = e->motion;
    int n = bk->anim.clips[clip].frame_count;
    int f = (int)e->anim_frame + 1;
    if (f >= n - 1) { e->anim_frame = n - 1; return 2; }
    int32_t a[3], b[3];
    ga_off(bk, clip, f, b); ga_off(bk, clip, f - 1, a);
    int32_t v[3] = { b[0] - a[0], 0, b[2] - a[2] }, w[2];
    ga_rig_welt(e, e->rot_y, v, w);
    e->x += w[0]; e->z += w[1];
    e->anim_frame = f;
    return 1;
}
/* Absolute Lage vom FRESSEN-Anker (FUN_80015cb8: Lage = Anker + R*off[f], `sw v0,56(s1)` @0x80015d68 / `sw v0,64(s1)`
 * @0x80015d90), fuer P1 ab dem Koppelbild sowie P2/P3 (@0x80101174 / @0x801011cc). */
static void ga_anker_lage(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int32_t o[3], w[2];
    ga_off(bk, e->motion, (int)e->anim_frame, o);
    int32_t v[3] = { o[0], 0, o[2] };
    ga_rig_welt(e, e->rot_y, v, w);
    e->x = s_ga.anker_x + w[0]; e->z = s_ga.anker_z + w[1];
}
static void ga_anker_setzen(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int32_t o[3], w[2];
    ga_off(bk, e->motion, (int)e->anim_frame, o);
    int32_t v[3] = { o[0], 0, o[2] };
    ga_rig_welt(e, e->rot_y, v, w);
    s_ga.anker_x = e->x - w[0]; s_ga.anker_z = e->z - w[1];
}

/* Weltlage des Kopfes (Part 6) einer Probe-Pose (Clip, Bild) an der Lage (x,z) mit Gier yaw. */
static int ga_kopf_probe(const re15_actor_t *e, int clip, int frame, int32_t x, int32_t z, int yaw, int32_t kopf[3])
{
    re15_actor_t b = *e;
    b.x = x; b.z = z; b.rot_y = (int16_t)yaw; b.motion = (int16_t)clip; b.anim_frame = frame;
    b.anim_frac = 0; b.anim_prev_valid = 0;
    return re15_gr2_bone_welt_pub(&b, 6, kopf);
}

/* Szenen-Gier: der Gator kommt aus dem Norden durch den Westkanal (Gier 1024 = -z) auf die Leiche zu. */
#define GSZ_GATOR_GIER        1024
/* Mundpunkt der Leiche (Leichen-Modell ROOM2090 Prop 2 MD1 @0x2054: x -444..2626, y -341..966, z -1049..822 -> Mitte
 * des Rumpfs) - im Objektraum der Leiche, gedreht mit ihrer Gier. */
#define GSZ_LEICHE_MITTE_X    1090
/* Biss: Kopf (Part 6) im Koppelbild 13 von Clip 4 (RE2 FRESSEN P1 @0x80101104-34) auf dem Rumpf der Leiche. */
#define GSZ_KOPPEL_BILD       13
#define GSZ_WENDE_RATE        75     /* RE2 R1 Ziel-Gier 75/Bild (FUN_8001569c @0x801025f8-608) */
#define GSZ_SCHREI_SE         0      /* RE2 R4: `addu a0,zero,zero` @0x80102c0c + `jal 0x8005bd6c` @0x80102c10 (ENEMSE Bank 17) */
#define GSZ_SCHREI_BLEND      3      /* RE2 R4: Clip 10 mit `addiu a2,zero,3` @0x80102c44 */
#define GSZ_SCHWIMM_BLEND     32     /* RE2 Sub 1: Clip 0 Blend 32 @0x80100c38 */
#define GSZ_BISS_CD           110    /* +0x238 = 110 nach dem RE2-Intro @0x8010091c-20 (re15_gr2_biss_cd) */

static void leiche_punkt(int32_t p[3])
{
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    if (k < 0) { p[0] = GSZ_LEICHE_X; p[1] = -1740; p[2] = GSZ_LEICHE_Z; return; }
    int ry = g_scd.props[k].rot_y;
    p[0] = g_scd.props[k].x + q12m(re15_cos_q12(ry), GSZ_LEICHE_MITTE_X);
    p[1] = g_scd.props[k].y;
    p[2] = g_scd.props[k].z - q12m(re15_sin_q12(ry), GSZ_LEICHE_MITTE_X);
}

/* Buehne: Lage S und Gier des Clip-6-Starts so, dass der Kopf im Koppelbild 13 von Clip 4 auf dem Leichen-Rumpf liegt
 * (RE2 FRESSEN P0 setzt den Gator relativ zum Opfer, @0x80101048-84). Kette: S --Clip 6 Bild 1..124--> A --Clip 4 Bild
 * 1..13--> Koppelbild; Kopf(13) = A + Weg4 + Kopf-Versatz. */
static void ga_buehne(re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    int32_t c[3]; leiche_punkt(c);
    const int yaw = GSZ_GATOR_GIER;
    int32_t kopf[3] = { 0, 0, 0 };
    ga_kopf_probe(e, 4, GSZ_KOPPEL_BILD, 0, 0, yaw, kopf);      /* Kopf relativ zur Wurzel im Koppelbild */
    int32_t a0[3], a1[3], b0[3], b1[3], w4[2], w6[2];
    ga_off(bk, 4, 0, a0); ga_off(bk, 4, GSZ_KOPPEL_BILD, a1);
    int32_t v4[3] = { a1[0] - a0[0], 0, a1[2] - a0[2] };
    ga_rig_welt(e, yaw, v4, w4);
    int n6 = bk->anim.clips[6].frame_count;
    ga_off(bk, 6, 0, b0); ga_off(bk, 6, n6 - 1, b1);
    int32_t v6[3] = { b1[0] - b0[0], 0, b1[2] - b0[2] };
    ga_rig_welt(e, yaw, v6, w6);
    int32_t ax = c[0] - kopf[0] - w4[0], az = c[2] - kopf[2] - w4[1];
    e->x = ax - w6[0]; e->z = az - w6[1];
    e->rot_y = (int16_t)yaw;
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[gatorszene] Buehne: Leiche (%d,%d) Kopf13-Versatz (%d,%d) Weg4 (%d,%d) Weg6 (%d,%d) -> A (%d,%d) S (%d,%d) Gier %d\n",
            c[0], c[2], kopf[0], kopf[2], w4[0], w4[1], w6[0], w6[1], ax, az, e->x, e->z, yaw);
#endif
}

/* SE des Gators (ENEMSE Bank 17, Paar-Tabelle @0x800A7400) - vom PC-Teil angemeldet (wie re15_gator_audio_hook). */
static void (*s_se_fn)(int, int) = NULL;
static void (*s_bank_fn)(int) = NULL;
void re15_gatorszene_r41_audio_hook(void (*se_fn)(int, int), void (*bank_fn)(int)) { s_se_fn = se_fn; s_bank_fn = bank_fn; }
static void ga_se(int id)
{
    if (s_bank_fn) s_bank_fn(17);                            /* GATOR_ENEMSE_BANK (enemy_ai_boss_gator.c) */
    if (s_se_fn) s_se_fn(id, 0);
}

static re15_actor_t s_opfer;                                  /* Messpunkt fuer die Kamerawahl (nur Lage) */
static const re15_actor_t *leiche_als_opfer(void)
{
    int32_t c[3]; leiche_punkt(c);
    memset(&s_opfer, 0, sizeof s_opfer);
    s_opfer.x = c[0]; s_opfer.y = c[1]; s_opfer.z = c[2];
    return &s_opfer;
}

static void ga_start(int slot, re15_actor_t *e, const re15_enemy_bank_t *bk)
{
    memset(&s_ga, 0, sizeof s_ga);
    ga_buehne(e, bk);
    e->y = -1200;                                             /* GB_WATER_Y = Wasser-INIT-Y der 0x23-KI @0x8010c65c */
    ga_clip(e, 6, 0, 0);                                      /* Clip 6 (0) @0x801008ac-b8 */
    s_ga.phase = GP_DURCHBRUCH;
    re15_gr2_kamera_auto(0);                                  /* die Szene haelt die Kamera selbst (Nachtrag 4 API) */
    int cut = re15_gr2_kamera_schnitt(RE15_GR2_BLICK_DURCHBRUCH, e, leiche_als_opfer(), "gatorszene Durchbruch");
    folge('G', GP_DURCHBRUCH, cut);
    (void)slot;
}

int re15_gatorszene_r41_boss(int slot, void *gator_actor)
{
    re15_actor_t *e = (re15_actor_t *)gator_actor;
    if (!e || (g_current_room_id & 0xFFFEu) != RE15_GSZ_RAUM) return 0;
    if (s_zustand == RE15_GSZ_ANKUNFT || s_zustand == RE15_GSZ_RUECKKEHR) return 1;   /* untergetaucht halten (B9) */
    if (s_zustand == RE15_GSZ_KAMPF) {
        if (!s_ga.kampf_gemeldet) { s_ga.kampf_gemeldet = 1; return 3; }
        return 0;
    }
    if (s_zustand != RE15_GSZ_GATOR) return 0;
    re15_enemy_bank_t *bk = re15_enemy_find(e->type);
    if (!bk || !bk->ok || bk->anim.clip_count < 12) return 1;
    if (s_ga.phase == GP_AUS) ga_start(slot, e, bk);
    e->y = -1200;
    s_ga.t++;
    switch (s_ga.phase) {
    case GP_DURCHBRUCH:                                       /* Clip 6 Bild 1..124: aus dem Wasser, auf die Leiche zu */
        if (ga_schritt(e, bk) == 2) {
            ga_clip(e, 4, 0, 0);                              /* FRESSEN P1: FUN_8001a330(em,4,0,0) @0x801010ec-f4 */
            s_ga.phase = GP_BISS; s_ga.t = 0;
            int cut = re15_gr2_kamera_schnitt(RE15_GR2_BLICK_FINISHER, e, leiche_als_opfer(), "gatorszene Fressen");
            folge('G', GP_BISS, cut);
        }
        break;
    case GP_BISS:                                             /* Clip 4 Bild 1..14 (P1), Koppelbild 13 */
        if (!s_ga.leiche_dran) ga_schritt(e, bk); else { e->anim_frame++; ga_anker_lage(e, bk); }
        if (!s_ga.leiche_dran && (int)e->anim_frame >= GSZ_KOPPEL_BILD) {
            s_ga.leiche_dran = 1;
            ga_anker_setzen(e, bk);                           /* FUN_80015b94 @0x80101110-34 */
            folge('K', (int)e->anim_frame, 0);
        }
        if ((int)e->anim_frame >= 14) {
            ga_clip(e, 5, 0, 0);                              /* P2 Clip 5 @0x80101180-88 */
            ga_anker_lage(e, bk);
            s_ga.phase = GP_SCHUETTELN; s_ga.t = 0;
            folge('G', GP_SCHUETTELN, 0);
        }
        break;
    case GP_SCHUETTELN: {                                     /* Clip 5, 120 Bilder (das Rumschleudern) */
        int n = bk->anim.clips[5].frame_count;
        if ((int)e->anim_frame + 1 >= n) {
            ga_clip(e, 11, 0, 0);                             /* P3 Clip 11 @0x801011d8-e0 */
            ga_anker_lage(e, bk);
            s_ga.phase = GP_KAUEN; s_ga.t = 0;
            folge('G', GP_KAUEN, 0);
        } else { e->anim_frame++; ga_anker_lage(e, bk); }
        break;
    }
    case GP_KAUEN: {                                          /* Clip 11, ein Kau-Kreis (59 Bilder), dann verschlungen */
        int n = bk->anim.clips[11].frame_count;
        if ((int)e->anim_frame + 1 >= n) {
            s_ga.leiche_dran = 0; s_ga.leiche_weg = 1;
            leiche_weg();
            folge('V', 0, 0);                                 /* verschlungen: Leiche + Schluessel weg */
            ga_clip(e, 0, 0, GSZ_SCHWIMM_BLEND);
            s_ga.phase = GP_WENDE; s_ga.t = 0;
            folge('G', GP_WENDE, 0);
        } else { e->anim_frame++; ga_anker_lage(e, bk); }
        break;
    }
    case GP_WENDE: {                                          /* Clip 0 schwimmend, Gier 75/Bild auf Leon */
        const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        int ziel = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 1024) & 0xfff;
        int d = ((ziel - (int)e->rot_y + 0x800) & 0xfff) - 0x800;
        int fertig = (d <= GSZ_WENDE_RATE && d >= -GSZ_WENDE_RATE);
        if (d > GSZ_WENDE_RATE) d = GSZ_WENDE_RATE; else if (d < -GSZ_WENDE_RATE) d = -GSZ_WENDE_RATE;
        e->rot_y = (int16_t)((e->rot_y + d) & 0xfff);
        if (ga_schritt(e, bk) == 2) e->anim_frame = 0;
        if (fertig || s_ga.t > 60) {
            ga_clip(e, 10, 0, GSZ_SCHREI_BLEND);              /* R4: Clip 10 Blend 3 @0x80102c40-48 */
            ga_se(GSZ_SCHREI_SE);                             /* R4: SE 0 @0x80102c0c-10 */
            re15_gr2_frame_wort(slot, 0x28000000u);           /* Stoss-Ereignis 1 -> Ruetteln + Schuetteln (@0x801003f0-40c) */
            s_ga.phase = GP_SCHREI; s_ga.t = 0;
            int cut = re15_gr2_kamera_schnitt(RE15_GR2_BLICK_ANGRIFF, e, NULL, "gatorszene Schrei");   /* Gesicht frontal, ohne Leon-Bedingung */
            folge('G', GP_SCHREI, cut);
        }
        break;
    }
    case GP_SCHREI:                                           /* Clip 10 (30 Bilder, Aufbaeumen + Rueckstoss) */
        if (ga_schritt(e, bk) == 2) {
            ga_clip(e, 0, 0, GSZ_SCHWIMM_BLEND);
            re15_gr2_biss_cd(slot, GSZ_BISS_CD);
            re15_gr2_kamera_frei("gatorszene");
            s_ga.phase = GP_ENDE; s_ga.t = 0;
            s_zustand = RE15_GSZ_KAMPF;
            re15_game_flag_set(5, RE15_GSZ_SIG_FERTIG, 1);    /* Programm 3 laeuft weiter (Ankunfts-Poll) */
            folge('G', GP_ENDE, 0);
        }
        break;
    default:
        break;
    }
    return 2;
}

/* ---- Leiche im Maul: Kind der Part-6-Weltmatrix wie der RE2-Kanister (@0x801018d0-e0) ---------------------------- */
/* Anhaengen im Koppelbild = FUN_8002d030(Part-6-Matrix, Objekt-Matrix) @0x801018f0: rel = R6^T * Ro, lokal = R6^T * (o - p6)
 * (Lokallage `sw v0,56(s2)` / `sw v1,60(s2)` / `sw a3,64(s2)` @0x80101910-18); danach je Bild Ro = R6 * rel, o = p6 + R6 * lokal
 * (Form gator_re2_r41.c re15_gr2_nachtakt / gr2_kan_folgen). Die Leiche haengt also so im Maul, wie sie im Koppelbild lag. */
static int32_t s_rel[9], s_lokal[3];
static int     s_rel_ok = 0;
/* Prop-Drehmatrix wie main.c pc_prop_rot_q12 (RotMatrix-Konvention des Prop-Zeichners). */
static void prop_matrix(int16_t rx, int16_t ry, int16_t rz, int32_t m[9])
{
    int32_t sx = re15_sin_q12(rx), cx = re15_cos_q12(rx);
    int32_t sy = re15_sin_q12(ry), cy = re15_cos_q12(ry);
    int32_t sz = re15_sin_q12(rz), cz = re15_cos_q12(rz);
    m[0] = q12m(cz, cy);
    m[1] = -q12m(sz, cy);
    m[2] = sy;
    m[3] = (int32_t)(((((int64_t)sz * cx) << 12) + (int64_t)cz * sy * sx) >> 24);
    m[4] = (int32_t)(((((int64_t)cz * cx) << 12) - (int64_t)sz * sy * sx) >> 24);
    m[5] = -q12m(cy, sx);
    m[6] = (int32_t)(((((int64_t)sz * sx) << 12) - (int64_t)cz * sy * cx) >> 24);
    m[7] = (int32_t)(((((int64_t)cz * sx) << 12) + (int64_t)sz * sy * cx) >> 24);
    m[8] = q12m(cy, cx);
}
static void euler_aus_matrix(const int32_t m[9], int16_t *rx, int16_t *ry, int16_t *rz)
{
    /* Umkehrung von prop_matrix: m[2] = sin(ry), m[0] = cz*cy, m[1] = -sz*cy, m[5] = -cy*sx, m[8] = cy*cx.
     * math-atan2(Y, X) = re15_atan2_q12(-Y, X) - 1024 (re15_actor.h: Gier 0 = +X, Vorwaerts (cos,-sin)). */
    int32_t cy = (int32_t)re15_squareroot0((uint32_t)((int64_t)m[0] * m[0] + (int64_t)m[1] * m[1]));
    *ry = (int16_t)(((int)re15_atan2_q12(-m[2], cy) - 1024) & 0xfff);
    *rx = (int16_t)(((int)re15_atan2_q12(m[5], m[8]) - 1024) & 0xfff);
    *rz = (int16_t)(((int)re15_atan2_q12(m[1], m[0]) - 1024) & 0xfff);
}
void re15_gatorszene_r41_nachtakt(int slot, void *gator_actor)
{
    (void)slot;
    re15_actor_t *e = (re15_actor_t *)gator_actor;
    if (!e || !s_ga.leiche_dran || s_ga.leiche_weg) return;
    int k = prop_slot(RE15_GSZ_LEICHE_OBJ);
    int32_t r6[9], p6[3];
    if (k < 0 || !re15_gr2_part6_gezeichnet(r6, p6)) return;
    if (!s_rel_ok) {                                         /* Koppelbild: Relation einfrieren (FUN_8002d030 @0x801018f0) */
        int32_t ro[9];
        prop_matrix(g_scd.props[k].rot_x, g_scd.props[k].rot_y, g_scd.props[k].rot_z, ro);
        int32_t d[3] = { g_scd.props[k].x - p6[0], g_scd.props[k].y - p6[1], g_scd.props[k].z - p6[2] };
        for (int r = 0; r < 3; r++) {
            s_lokal[r] = (int32_t)(((int64_t)r6[0*3+r] * d[0] + (int64_t)r6[1*3+r] * d[1] + (int64_t)r6[2*3+r] * d[2]) >> 12);
            for (int c = 0; c < 3; c++)
                s_rel[r*3+c] = (int32_t)(((int64_t)r6[0*3+r] * ro[0*3+c] + (int64_t)r6[1*3+r] * ro[1*3+c]
                                        + (int64_t)r6[2*3+r] * ro[2*3+c]) >> 12);
        }
        s_rel_ok = 1;
        folge('H', s_lokal[0], s_lokal[2]);
    }
    int32_t rw[9];
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            rw[r*3+c] = (int32_t)(((int64_t)r6[r*3] * s_rel[c] + (int64_t)r6[r*3+1] * s_rel[3+c] + (int64_t)r6[r*3+2] * s_rel[6+c]) >> 12);
    g_scd.props[k].x = p6[0] + (int32_t)(((int64_t)r6[0] * s_lokal[0] + (int64_t)r6[1] * s_lokal[1] + (int64_t)r6[2] * s_lokal[2]) >> 12);
    g_scd.props[k].y = p6[1] + (int32_t)(((int64_t)r6[3] * s_lokal[0] + (int64_t)r6[4] * s_lokal[1] + (int64_t)r6[5] * s_lokal[2]) >> 12);
    g_scd.props[k].z = p6[2] + (int32_t)(((int64_t)r6[6] * s_lokal[0] + (int64_t)r6[7] * s_lokal[1] + (int64_t)r6[8] * s_lokal[2]) >> 12);
    euler_aus_matrix(rw, &g_scd.props[k].rot_x, &g_scd.props[k].rot_y, &g_scd.props[k].rot_z);
}

/* ---- Takt (scd_vm_tick, Gameplay-Lauf) --------------------------------------------------------------------------- */
static void (*s_tuerton_fn)(void) = NULL;   /* PC: Door_exit-Satz (se 1) des RE2-Archivs DOOR06 = Tuer 2080 Slot 0 */
void re15_gatorszene_r41_tuer_hook(void (*fn)(void)) { s_tuerton_fn = fn; }
static re15_actor_t *gator_aktor(void)
{
    for (int i = 1; i < RE15_ACTOR_MAX; i++)
        if (g_actors[i].active && g_actors[i].type == 0x23u) return &g_actors[i];
    return NULL;
}
/* Schluessel-Lage nach dem Tod: Kopf (Part 6) der Leiche (dort platzte der Kiefer, R1 @0x80102354-58), in eine Wasserzone
 * (re15_aot_water_at = FUN_800527B4-Zwilling) gezogen - sonst Richtung Wurzel, sonst die Ersatzlage. */
static void key_lage_bei_tod(void)
{
    re15_actor_t *g = gator_aktor();
    s_key_x = GSZ_KEY_ERSATZ_X; s_key_z = GSZ_KEY_ERSATZ_Z;
    if (g) {
        int32_t kopf[3] = { g->x, g->y, g->z };
        if (!re15_gr2_bone_welt_pub(g, 6, kopf)) { kopf[0] = g->x; kopf[2] = g->z; }
        for (int i = 0; i <= 8; i++) {
            int32_t x = kopf[0] + (int32_t)(((int64_t)(g->x - kopf[0]) * i) / 8);
            int32_t z = kopf[2] + (int32_t)(((int64_t)(g->z - kopf[2]) * i) / 8);
            if (re15_aot_water_at(x, z) != 0) { s_key_x = x; s_key_z = z; break; }
        }
    }
    s_key_lage_ok = 1;
}
void re15_gatorszene_r41_tick(void)
{
    const uint16_t basis = (uint16_t)(g_current_room_id & 0xFFFEu);
    if (basis == RE15_GSZ_RAUM && re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_GESEHEN)
        && !re15_game_flag_get(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT) && re15_gr2_tot()) {
        key_lage_bei_tod();                                  /* Alligator tot (R1-Endzustand +0x10E |= 0x4000 @0x80102654-64) */
        re15_game_flag_set(RE15_GSZ_BANK, RE15_GSZ_BIT_LIEGT, 1);
        key_bei_leiche((uint16_t)g_current_room_id);
        folge('L', (int)s_key_x, (int)s_key_z);
    }
    if (basis == 0x2080u && s_zustand == RE15_GSZ_MONTAGE && re15_game_flag_get(5, RE15_GSZ_SIG_TUER)) {
        re15_game_flag_set(5, RE15_GSZ_SIG_TUER, 0);
        if (s_tuerton_fn) s_tuerton_fn();
        folge('T', 1, 0);
    }
    if (basis == RE15_GSZ_RAUM && s_zustand == RE15_GSZ_RUECKKEHR && re15_game_flag_get(5, RE15_GSZ_SIG_GATOR)) {
        re15_game_flag_set(5, RE15_GSZ_SIG_GATOR, 0);
        memset(&s_ga, 0, sizeof s_ga);
        s_rel_ok = 0;
        s_zustand = RE15_GSZ_GATOR;
        folge('S', 0, 0);
    }
}
