/*
 * RE1.5 Rebuilt — Runde 38 Spur F "dining1141": Elzas Weg ROOM1021 -> 1011 -> 1131 -> 1141 und der Rolladen-Ton
 * ROOM1140/1141. Herleitung, Belege und alle Konstanten: include/re15_dining_r38.h, die Programme mit Listing:
 * engine/src/gen/dining_r38_programme.inc (tools/r38_dining/programme_bauen.py), Dossier
 * analysis/befunde_runde38/F_dining1141.md. Muster: schluessel_r36.c (Gegenstand, Tuer-Handler RE2 LAB_80051514).
 */
#include "re15_dining_r38.h"
#include "re15_dining_r39.h"   /* Runde 39 Spur J: Phase-H-Programme, Port-Bank 0x3E, Texte 50..52 */

#include <stddef.h>
#include <string.h>

#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_msg.h"
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_inventory.h"
#include "re15_lock_se.h"
#include "re15_audio.h"
#include "re15_item_discard.h"
#include "re15_engine.h"           /* g_engine.frame_count (Messschrieb log_f) */
#include "re15_szenenleon_r36.h"   /* re15_szl_tuerton: Tonteil DOOR1A (die Dachtuer 1141 spielt DOOR1A, tuer_zuordnung.inc) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>   /* getenv/atoi (Messhaken) */
#endif

#include "gen/re2_door_bank.inc"         /* RE2_DOOR_SE_AUF_KEY (TUERSE.VBS Satz 4 = RE2 ROOM1140 Satz 0x25) */
#include "gen/dining_r38_programme.inc"  /* Programme, Texte, Anker (tools/r38_dining/programme_bauen.py)   */
#include "gen/dining_r38_schluessel.inc" /* RE2-Bytes Herz-Schluessel (tools/r38_dining/re2_export.py)      */

extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);

void (*re15_dining_r38_se_hook)(int bank, int satz) = NULL;

unsigned g_re15_r38f_zu_zaehler       = 0;
unsigned g_re15_r38f_benutzt_zaehler  = 0;
unsigned g_re15_r38f_se_zaehler[3]    = { 0, 0, 0 };

static uint8_t s_phase_h;
static uint8_t s_gesperrt;
static int16_t s_tuer_next[3];

/* Leeres Ereignis-Programm (Evt_end 0x01, 2 Byte), wie schluessel_r36.c k_evt_end. */
static const uint8_t k_evt_end[2] = { 0x01, 0x00 };

static void log_f(const char *s)
{
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining] F%u %s\n", (unsigned)g_engine.frame_count, s);   /* Spielbild fuer den Messschrieb */
#else
    (void)s;
#endif
}

/* ============================ Anker (Quellbereiche im GELADENEN RDT) ============================
 * Die Programme bestehen aus Original-Bytes. Bevor eines laeuft, wird der Quellbereich im geladenen RDT per
 * FNV-1a gegen den Wert beim Bauen geprueft — weicht er ab (fremde RDT), bleibt der Original-Sub. */
enum { A_ROLL1140 = 1, A_ROLL1141 = 2, A_LEICHE = 4, A_SUB02_05 = 8, A_SUB00 = 16, A_MAIN00 = 32, A_SZENE1021 = 64 };

static uint32_t fnv(const uint8_t *p, uint32_t n)
{
    uint32_t h = 0x811C9DC5u;
    for (uint32_t i = 0; i < n; i++) { h ^= p[i]; h *= 0x01000193u; }
    return h;
}

static int bereich_ok(uint32_t off, uint32_t len, uint32_t soll)
{
    const uint8_t *raw = g_room_rdt.raw;
    if (!raw || (uint32_t)g_room_rdt.raw_size < off + len) return 0;
    return fnv(raw + off, len) == soll;
}

static int anker(uint16_t room_id)
{
    int m = 0;
    if (room_id == RE15_R38F_RAUM_1140 &&
        bereich_ok(RE15_R38F_ANKER_ROLL1140_SUB03_OFF, RE15_R38F_ANKER_ROLL1140_SUB03_LEN, RE15_R38F_ANKER_ROLL1140_SUB03_FNV))
        m |= A_ROLL1140;
    if (room_id == RE15_R38F_RAUM_1141) {
        if (bereich_ok(RE15_R38F_ANKER_ROLL1141_SUB06_OFF, RE15_R38F_ANKER_ROLL1141_SUB06_LEN, RE15_R38F_ANKER_ROLL1141_SUB06_FNV))
            m |= A_ROLL1141;
        if (bereich_ok(RE15_R38F_ANKER_SZENE1141_SUB02_05_OFF, RE15_R38F_ANKER_SZENE1141_SUB02_05_LEN, RE15_R38F_ANKER_SZENE1141_SUB02_05_FNV))
            m |= A_SUB02_05;
        if (bereich_ok(RE15_R38F_ANKER_SZENE1141_SUB00_OFF, RE15_R38F_ANKER_SZENE1141_SUB00_LEN, RE15_R38F_ANKER_SZENE1141_SUB00_FNV))
            m |= A_SUB00;
        if (bereich_ok(RE15_R38F_ANKER_SZENE1141_MAIN00_OFF, RE15_R38F_ANKER_SZENE1141_MAIN00_LEN, RE15_R38F_ANKER_SZENE1141_MAIN00_FNV))
            m |= A_MAIN00;
    }
    if (room_id == RE15_R38F_RAUM_1021 &&
        bereich_ok(RE15_R38F_ANKER_LEICHE1021_SUB06_OFF, RE15_R38F_ANKER_LEICHE1021_SUB06_LEN, RE15_R38F_ANKER_LEICHE1021_SUB06_FNV))
        m |= A_LEICHE;
    /* Nachbesserung 1 M1: die Szene sub02..sub05 (Gosub-Ziel sub04) und main00 Slot 5 .. Slot 6 bytegleich. */
    if (room_id == RE15_R38F_RAUM_1021 &&
        bereich_ok(RE15_R38F_ANKER_SZENE1021_SUB02_05_OFF, RE15_R38F_ANKER_SZENE1021_SUB02_05_LEN, RE15_R38F_ANKER_SZENE1021_SUB02_05_FNV) &&
        bereich_ok(RE15_R38F_ANKER_SZENE1021_MAIN00_OFF, RE15_R38F_ANKER_SZENE1021_MAIN00_LEN, RE15_R38F_ANKER_SZENE1021_MAIN00_FNV))
        m |= A_SZENE1021;
    return m;
}

int re15_dining_r38_anker_ok(void) { return anker((uint16_t)g_current_room_id); }

/* Phase H (re15_dining_r38.h): John sitzt im Dachgang hinter dem Notrolladen fest — (4,0xF0) gesetzt, (3,0x6A) frei.
 * Alle Quellbereiche der Programme HAEMMERN/BEFREIUNG muessen bytegleich sein, sonst bleibt der Original-Ablauf. */
static int phase_h_bedingung(uint16_t room_id)
{
    if (room_id != RE15_R38F_RAUM_1141) return 0;
    if (!re15_game_flag_get(RE15_R38F_F0_BANK, RE15_R38F_F0_BIT)) return 0;
    if (re15_game_flag_get(RE15_R38F_6A_BANK, RE15_R38F_6A_BIT)) return 0;
    const int alle = A_ROLL1141 | A_SUB02_05 | A_SUB00 | A_MAIN00;
    return (anker(room_id) & alle) == alle;
}

int re15_dining_r38_phase_h(void) { return s_phase_h; }
int re15_dining_r38_tuer_gesperrt(void) { return s_gesperrt; }

/* ============================ Texte ============================ */
static void meldung(int id, const uint8_t *b, size_t n)
{
    re15_msg_install_text((unsigned char)id, b, n);
    int d = re15_msg_compute_duration(b, n, 0);
    if (d > 0 && d < 65535) re15_msg_install_durations((unsigned char)id, d);
}

const uint8_t *re15_dining_r38_text(int nr, int *out_len)
{
    static const struct { const uint8_t *b; int n; } t[5] = {
        { k_r38f_msg1021_leiche,  (int)sizeof k_r38f_msg1021_leiche  },
        { k_r38f_msg1131_zu,      (int)sizeof k_r38f_msg1131_zu      },
        { k_r38f_msg1131_benutzt, (int)sizeof k_r38f_msg1131_benutzt },
        { k_r38f_msg1141_frage,   (int)sizeof k_r38f_msg1141_frage   },
        { k_r38f_msg1141_sperre,  (int)sizeof k_r38f_msg1141_sperre  },
    };
    if (nr < 0 || nr >= 5) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = t[nr].n;
    return t[nr].b;
}

const uint8_t *re15_dining_r38_programm(int nr, int *out_len)
{
    static const struct { const uint8_t *b; int n; } p[6] = {
        { k_r38f_p_roll1140,   (int)sizeof k_r38f_p_roll1140   },
        { k_r38f_p_roll1141,   (int)sizeof k_r38f_p_roll1141   },
        { k_r38f_p_leiche1021, (int)sizeof k_r38f_p_leiche1021 },
        { k_r38f_p_haemmern,   (int)sizeof k_r38f_p_haemmern   },
        { k_r38f_p_befreiung,  (int)sizeof k_r38f_p_befreiung  },
        { k_r38f_p_szene1021,  (int)sizeof k_r38f_p_szene1021  },   /* Nachbesserung 1 M1 */
    };
    if (nr < 0 || nr >= 6) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = p[nr].n;
    return p[nr].b;
}

/* ============================ Raumaufbau ============================ */
static int slot_von_obj_id(uint8_t obj_id)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj_id) return k;
    return -1;
}

/* ROOM1011: der Schluessel an Roys Liegestelle, sobald Roy in 1021 tot ist ((3,0x5F)) und solange er nicht genommen
 * ist ((9,64)). Felder wie Obj_model_set (LAB_80040914) / Item_aot_set (@0x80040644), Muster schluessel_r36.c. */
static void schluessel_1011(void)
{
    if (!re15_game_flag_get(RE15_R38F_ROY_BANK, RE15_R38F_ROY_BIT)) return;
    if (re15_game_flag_get(9, RE15_R38F_BIT_GENOMMEN)) return;
    if (slot_von_obj_id(RE15_R38F_1011_OBJ) < 0 && g_scd.prop_count < RE15_SCD_MAX_PROPS) {
        int i = (int)g_scd.prop_count++;
        memset(&g_scd.props[i], 0, sizeof g_scd.props[i]);
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = RE15_R38F_1011_OBJ;
        g_scd.props[i].obj_type   = 0;
        g_scd.props[i].band       = RE15_R38F_PROP_BAND;
        g_scd.props[i].parent_obj = -1;
        g_scd.props[i].x = RE15_R38F_KEY_X;
        g_scd.props[i].y = RE15_R38F_KEY_Y;
        g_scd.props[i].z = RE15_R38F_KEY_Z;
        g_scd.props[i].rot_y = RE15_R38F_KEY_ROT_Y;
        g_scd.props[i].flags = RE15_R38F_PROP_FLAGS;
        g_scd.props[i].attr  = RE15_R38F_PROP_ATTR;        /* 0x0010 unbeleuchtet wie RE2 room2020 @0x01D90 */
    }
    if (!g_aot.slots[RE15_R38F_1011_SLOT].active) {
        re15_aot_set_item_tk_prop(RE15_R38F_1011_SLOT,
                                  RE15_R38F_KEY_RECT_X + RE15_R38F_KEY_RECT_W / 2,
                                  RE15_R38F_KEY_RECT_Z + RE15_R38F_KEY_RECT_D / 2,
                                  RE15_R38F_KEY_RECT_W / 2, RE15_R38F_KEY_RECT_D / 2,
                                  RE15_R38F_ITEM, RE15_R38F_MENGE, RE15_R38F_BIT_GENOMMEN, RE15_R38F_1011_OBJ);
        g_aot.slots[RE15_R38F_1011_SLOT].sce_flags = RE15_R38F_AOT_SAT;
        g_aot.slots[RE15_R38F_1011_SLOT].band      = 0;
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining] ROOM1011: Dining Room Key (Item 0x%02X, Bit (9,%d)) Prop obj %d bei (%d,%d,%d), Zone Slot %d\n",
            RE15_R38F_ITEM, RE15_R38F_BIT_GENOMMEN, RE15_R38F_1011_OBJ, RE15_R38F_KEY_X, RE15_R38F_KEY_Y,
            RE15_R38F_KEY_Z, RE15_R38F_1011_SLOT);
#endif
}

/* ROOM1131: Slot 0 (-> 1141) bis zum Schluessel umwidmen (Muster schluessel_r36.c tuer_sperren). */
static void tuer_1131(void)
{
    meldung(RE15_R38F_MSG_1131_ZU, k_r38f_msg1131_zu, sizeof k_r38f_msg1131_zu);
    meldung(RE15_R38F_MSG_1131_BENUTZT, k_r38f_msg1131_benutzt, sizeof k_r38f_msg1131_benutzt);
    /* Nachbesserung 1 M3: die Doppeltuer 1141 -> 1131 ist frei, sobald die Gorillas tot sind; Johns Abgang wartet, solange
     * Elza in seiner sichtbaren Bahn steht. Geht sie in dieser Zeit hinaus, geht John aufs Dach, waehrend sie fort ist —
     * derselbe Endzustand wie nach dem Abgang im Raum (BEFREIUNG: Set(9,67)=1, sub05 @0x00FD2 Set(4,0xC3)=1 = Riegel offen).
     * Erkennung: Szene 1141 gelaufen ((3,0xC2) = sub02 @0x00C20), Gorillas tot ((4,0xF2) = 0, sub01 @0x00C0C), John noch
     * nicht fort ((9,67) = 0), Port-Weg ((9,66) = Tuer aufgeschlossen). Beim naechsten Eintritt baut main00 @0x00A72
     * Ck(4,0xC3) die Dachtuer offen, sub00 setzt bei (4,0xF0) = (4,0xF2) = 0 keinen John (@0x00AFE/@0x00B4A). */
    if (re15_game_flag_get(9, RE15_R38F_BIT_TUER) && re15_game_flag_get(3, 0xC2) &&
        !re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT) && !re15_game_flag_get(9, RE15_R38F_BIT_JOHN_WEG)) {
        re15_game_flag_set(9, RE15_R38F_BIT_JOHN_WEG, 1);
        re15_game_flag_set(4, 0xC3, 1);
        log_f("ROOM1131: Elza hat 1141 vor Johns Abgang verlassen -> John ist aufs Dach gegangen: (9,67) = 1, Riegel (4,0xC3) = 1");
    }
    if (re15_game_flag_get(9, RE15_R38F_BIT_TUER)) return;
    const re15_aot_t *a = &g_aot.slots[RE15_R38F_1131_TUER_SLOT];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return;
    s_tuer_next[0] = (int16_t)g_aot.door_params[RE15_R38F_1131_TUER_SLOT].spawn_x;
    s_tuer_next[1] = (int16_t)g_aot.door_params[RE15_R38F_1131_TUER_SLOT].spawn_y;
    s_tuer_next[2] = (int16_t)g_aot.door_params[RE15_R38F_1131_TUER_SLOT].spawn_z;
    g_aot.slots[RE15_R38F_1131_TUER_SLOT].band = g_aot.door_params[RE15_R38F_1131_TUER_SLOT].band;
    re15_aot_retype(RE15_R38F_1131_TUER_SLOT, 3, RE15_R38F_TUER_FLAGS, 0,
                    (uint16_t)(RE15_R38F_1131_EREIGNIS << 8), 0);
    s_gesperrt = 1;
    log_f("ROOM1131 Slot 0 (Tuer -> ROOM1141) gesperrt: Ereignis 29, Flag (9,66) = 0");
}

/* ROOM1141: Texte; in Phase H John in den Dachgang hinter den Rolladen setzen und die geparkten Gorillas herausnehmen
 * (sub00 @0x00B06/@0x00B1A/@0x00B2E). Laeuft nach dem Init-Lauf und VOR dem ersten Bild (scd_room_setup.c Blockende,
 * platform/pc/main.c Boot-/CONTINUE-Weg): John ist ab dem ersten Bild im Gang — er taucht nie auf (Nachtrag 2). */
static void raum_1141(void)
{
    meldung(RE15_R38F_MSG_1141_FRAGE, k_r38f_msg1141_frage, sizeof k_r38f_msg1141_frage);
    meldung(RE15_R38F_MSG_1141_SPERRE, k_r38f_msg1141_sperre, sizeof k_r38f_msg1141_sperre);
    if (!phase_h_bedingung(RE15_R38F_RAUM_1141)) return;
    s_phase_h = 1;
    for (int s = RE15_R38F_JOHN_SCRIPT_SLOT; s <= RE15_R38F_JOHN_SCRIPT_SLOT + 2; s++) {
        re15_actor_t *a = &g_actors[s + 1];                     /* SCRIPT_SLOT_TO_ACTOR = +1 (scd_vm.c) */
        if (!a->active) continue;
        if (s == RE15_R38F_JOHN_SCRIPT_SLOT && a->type == RE15_R38F_JOHN_TYP) {
            a->x = RE15_R38F_JOHN_GANG_X;                       /* Spawn @0x00B06 (-6750,0,-13350) -> Dachgang */
            a->y = 0;
            a->z = RE15_R38F_JOHN_GANG_Z;
            a->rot_y = RE15_R38F_JOHN_GANG_YAW;
        } else if (s != RE15_R38F_JOHN_SCRIPT_SLOT && a->type == RE15_R38F_GORILLA_TYP) {
            re15_actor_free(s + 1);
        }
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[dining] F%u ROOM1141 Phase H: John im Dachgang hinter dem Rolladen (%d,0,%d) Blick %d, Gorillas "
                    "heraus, Klopf-Schleife statt Szene sub02\n", (unsigned)g_engine.frame_count,
            RE15_R38F_JOHN_GANG_X, RE15_R38F_JOHN_GANG_Z, RE15_R38F_JOHN_GANG_YAW);
#endif
}

void re15_dining_r38_install(uint16_t room_id)
{
    s_phase_h = 0;
    s_gesperrt = 0;
    if (room_id == RE15_R38F_RAUM_1011) schluessel_1011();
    else if (room_id == RE15_R38F_RAUM_1021) meldung(RE15_R38F_MSG_1021_LEICHE, k_r38f_msg1021_leiche, sizeof k_r38f_msg1021_leiche);
    else if (room_id == RE15_R38F_RAUM_1131) tuer_1131();
    else if (room_id == RE15_R38F_RAUM_1141) raum_1141();
    re15_dining_r39_install(room_id, s_phase_h);   /* Runde 39 Spur J: Texte 50..52; Phase H: John auf dem Dach */
}

/* ============================ Programme ============================ */
const uint8_t *re15_dining_r38_evt_exec(uint16_t room_id, uint8_t sub_id, const uint8_t *orig)
{
    if (room_id == RE15_R38F_RAUM_1141 && sub_id == RE15_R38F_SUB_SZENE && phase_h_bedingung(room_id)) {
        log_f("ROOM1141 sub00 @0x00B02 Evt_exec sub02 -> Klopf-Schleife (Phase H)");
        return re15_dining_r39_haemmern();   /* Runde 39 Spur J: Klopfen + help01..05 + Elzas Reaktion statt HAEMMERN */
    }
    return orig;
}

const uint8_t *re15_dining_r38_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id == RE15_R38F_RAUM_1140 && event_id == RE15_R38F_1140_EREIGNIS && (anker(room_id) & A_ROLL1140))
        return k_r38f_p_roll1140;
    if (room_id == RE15_R38F_RAUM_1141 && event_id == RE15_R38F_1141_EREIGNIS_GEMAELDE && phase_h_bedingung(room_id)) {
        /* Gemaelde in Phase H: BEFREIUNG (Rolladen hoch mit Ton, John kommt heraus, Szene, Sperre, Abgang). Die
         * Tuerbank laedt schon hier den Tonteil der Dachtuer (DOOR1A, gen/tuer_zuordnung.inc S064 ROOM1141 Slot 1)
         * fuer Johns Abgang am Ende. */
        { int n = 0; const uint8_t *ton = re15_szl_tuerton(&n);
          if (!re15_audio_re2_tuer_laden(ton, n)) log_f("Tuerbank DOOR1A nicht geladen (kein Audiogeraet?)"); }
        log_f("ROOM1141 Gemaelde (Ereignis 6) in Phase H -> Programm BEFREIUNG");
        return re15_dining_r39_befreiung();   /* Runde 39 Spur J: John durch die Tuer, Glas, Blick, Zeilen 51/52 */
    }
    if (room_id == RE15_R38F_RAUM_1141 && event_id == RE15_R38F_1141_EREIGNIS_GEMAELDE && (anker(room_id) & A_ROLL1141))
        return k_r38f_p_roll1141;
    if (room_id == RE15_R38F_RAUM_1021 && event_id == RE15_R38F_1021_EREIGNIS && (anker(room_id) & A_LEICHE))
        return k_r38f_p_leiche1021;
    /* Nachbesserung 1 M1: die Szene (Begehzone Slot 5 -> Ereignis 2) laeuft als Gosub sub02 aus dem RDT, danach entsteht die
     * Leichen-Zone Slot 6 in der Form von main00 @0x01E2C — der Hinweis ist direkt nach der Szene erreichbar. */
    if (room_id == RE15_R38F_RAUM_1021 && event_id == RE15_R38F_1021_EREIGNIS_SZENE && (anker(room_id) & A_SZENE1021)) {
        log_f("ROOM1021 Ereignis 2 (Szene sub02) -> Programm SZENE1021: Gosub sub02, danach Leichen-Zone Slot 6 (main00 @0x01E2C)");
        return k_r38f_p_szene1021;
    }
    if (room_id == RE15_R38F_RAUM_1131 && event_id == RE15_R38F_1131_EREIGNIS && s_gesperrt) {
        /* RE2 LAB_80051514: Besitz-Test im INVENTAR (`jal FUN_800696cc` @0x80051628, `bltz` @0x80051634). */
        if (re15_inv_find_item(RE15_R38F_ITEM) >= 0) {
            (void)re15_discard_besitz_vor_nachricht(room_id, RE15_R38F_MSG_1131_BENUTZT);
            re15_scd_show_message(RE15_R38F_MSG_1131_BENUTZT, RE15_R38F_MASKE << 16);
            re15_audio_re2_door_se(RE2_DOOR_SE_AUF_KEY);                 /* @0x80051654/58 Se_on(2,0x25) */
            re15_discard_notice_message(room_id, RE15_R38F_MSG_1131_BENUTZT);   /* @0x80051670 */
            re15_game_flag_set(9, RE15_R38F_BIT_TUER, 1);                 /* @0x80051690 FUN_8007730c */
            g_aot.slots[RE15_R38F_1131_TUER_SLOT].band = g_aot.door_params[RE15_R38F_1131_TUER_SLOT].band;
            re15_aot_retype(RE15_R38F_1131_TUER_SLOT, 2, RE15_R38F_TUER_FLAGS,
                            (uint16_t)s_tuer_next[0], (uint16_t)s_tuer_next[1], (uint16_t)s_tuer_next[2]);
            g_aot.slots[RE15_R38F_1131_TUER_SLOT].was_inside = 1;        /* kein Durchgang bei diesem Druck (@0x80051698) */
            s_gesperrt = 0;
            g_re15_r38f_benutzt_zaehler++;
            log_f("ROOM1131: Dining Room Key benutzt -> Tuer offen, Flag (9,66) = 1");
        } else {
            re15_audio_re2_door_se(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));   /* @0x800516a0/a4 */
            re15_scd_show_message(RE15_R38F_MSG_1131_ZU, RE15_R38F_MASKE << 16);      /* @0x800516b8 */
            g_re15_r38f_zu_zaehler++;
            log_f("ROOM1131: Tuer zum Besprechungsraum verschlossen (kein Dining Room Key im Inventar)");
        }
        return k_evt_end;
    }
    return NULL;
}

int re15_dining_r38_se_on(unsigned bank, int satz)
{
    int k;
    if (re15_dining_r39_se_on(bank, satz)) return 1;   /* Runde 39 Spur J: Port-Bank 0x3E */
    if (bank == RE15_R38F_SE_BANK_ROLLADEN)     k = 0;
    else if (bank == RE15_R38F_SE_BANK_KLOPFEN) k = 1;
    else if (bank == RE15_R38F_SE_BANK_HILFE)   k = 2;
    else return 0;
    g_re15_r38f_se_zaehler[k]++;
    if (re15_dining_r38_se_hook) re15_dining_r38_se_hook((int)bank, satz);
    return 1;
}

/* ============================ Messhaken (env-gegatet, kein Spielverhalten) ============================
 * RE15_R38F_GORILLAS_TOT=<n>: in ROOM1141 nach n freien Kampfbildern ((4,0xF2)=1 und Spieler frei, player_mode 0) die
 * beiden Gorillas (Typ 0x27) herausnehmen und ihre Kill-Flags setzen — genau die Bits, die ihr Tod setzt: Sce_em_set
 * sub00 @0x00B1A/@0x00B2E tragen pc[7] = 0xCB/0xCC (scd_vm.c op_sce_em_set: em_flag_id = pc[7]), Bank =
 * re15_em_status_zone() (Stage 1 -> 7, @0x800420ec), gelesen von sub01 @0x00BFA/@0x00C02. Damit erreicht ein Messlauf
 * Johns Abgang ohne Kampf-Eingaben (der Zaehler laeuft ab dem Ende der Szene, unabhaengig von der Dauer davor). */
void re15_dining_r38_messhaken(void)
{
    re15_dining_r39_messhaken();   /* Runde 39 Spur J: RE15_R39J_LOG */
#ifdef RE15_PLATFORM_PC
    static int s_init = 0, s_n = -1, s_done = 0, s_kampf = 0, s_starr = 0;
    if (!s_init) { const char *e = getenv("RE15_R38F_GORILLAS_TOT"); s_init = 1; if (e && *e) s_n = atoi(e);
                   e = getenv("RE15_R38F_GORILLAS_STARR"); s_starr = (e && *e); }
    if (s_n < 0 || s_done || g_current_room_id != RE15_R38F_RAUM_1141) return;
    if (!re15_game_flag_get(RE15_R38F_F2_BANK, RE15_R38F_F2_BIT) || g_scd.player_mode != 0) return;
    /* RE15_R38F_GORILLAS_STARR=1: die Gorillas im Kampf einfrieren (grid_id | 0x20 = Member_set 0x0C=0x30 @0x800410b8,
     * wie sub06 @0x0101A), damit der Messlauf zu Fuss an die Tueren kommt, ohne gegriffen zu werden. */
    if (s_starr)
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) g_actors[s].grid_id |= 0x20;
    if (++s_kampf < s_n) return;
    s_done = 1;
    extern uint8_t re15_em_status_zone(void);
    const uint8_t zone = re15_em_status_zone();
    int n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == RE15_R38F_GORILLA_TYP) { re15_actor_free(s); n++; }
    re15_game_flag_set(zone, 0xCB, 1);
    re15_game_flag_set(zone, 0xCC, 1);
    fprintf(stderr, "[dining] F%u MESSHAKEN RE15_R38F_GORILLAS_TOT: %d Gorillas heraus, Flags (%u,0xCB)/(%u,0xCC) = 1\n",
            (unsigned)g_engine.frame_count, n, (unsigned)zone, (unsigned)zone);
#endif
}

/* ============================ Gegenstand 0x43 ============================ */
const uint8_t *re15_dining_r38_name(uint8_t item_id) { return item_id == RE15_R38F_ITEM ? k_r38f_name : NULL; }
const uint8_t *re15_dining_r38_desc(uint8_t item_id) { return item_id == RE15_R38F_ITEM ? k_r38f_desc : NULL; }
int re15_dining_r38_ist_schluessel(uint8_t item_id)  { return item_id == RE15_R38F_ITEM; }

void re15_dining_r38_icon_einsetzen(uint8_t *itemall, int size)
{
    if (itemall && (RE15_R38F_ITEM + 1) * 1200 <= size)
        memcpy(itemall + RE15_R38F_ITEM * 1200, re15_r38f_key_icon, 1200);
}

void re15_dining_r38_bild_einsetzen(uint8_t *itps, int size)
{
    if (itps && (RE15_R38F_ITEM + 1) * 0x3000 <= size)
        memcpy(itps + RE15_R38F_ITEM * 0x3000, re15_r38f_key_itps, 0x3000);
}

/* Benutzungsstelle fuer die Wegwerf-Kette (item_discard_common.c, RE2 @0x80051670): (1131, msg 41) -> 0x43. */
int re15_dining_r38_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item)
{
    if ((uint16_t)room_id != RE15_R38F_RAUM_1131 || msg_id != RE15_R38F_MSG_1131_BENUTZT) return 0;
    if (out_item) *out_item = RE15_R38F_ITEM;
    return 1;
}

int re15_dining_r38_obj_id(uint16_t room_id)
{
    return room_id == RE15_R38F_RAUM_1011 ? RE15_R38F_1011_OBJ : -1;
}

const uint8_t *re15_dining_r38_md1_bytes(uint16_t room_id, int *out_size)
{
    if (out_size) *out_size = 0;
    if (room_id != RE15_R38F_RAUM_1011) return NULL;
    if (out_size) *out_size = (int)sizeof re15_r38f_key_md1;
    return re15_r38f_key_md1;
}

const uint8_t *re15_dining_r38_tim_bytes(uint16_t room_id, int *out_size)
{
    if (out_size) *out_size = 0;
    if (room_id != RE15_R38F_RAUM_1011) return NULL;
    if (out_size) *out_size = (int)sizeof re15_r38f_key_tim;
    return re15_r38f_key_tim;
}
