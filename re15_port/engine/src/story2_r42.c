/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE2. Belege, Formen und Konstanten: include/re15_story2_r42.h.
 *
 * PORT-STORY nach dem freigegebenen Konzept (3.2 L1/L1b, 4.2 E1, 6b; K-01..K-03, K-07, K-08). Gebaut wird nur aus
 * Original-Formen: Ereignis-Plaetze wie ventilkette_r41.c (Aot_set sce 3 + Aot_reset-Umwidmung LAB_80040738), Tuer-Druecke in C
 * wie gatorszene_r41.c tor_druck (RE2-Tuer-Handler LAB_80051514), Szenen als SCD-Bytecode aus Original-Opcodes, ausgefuehrt von
 * der vorhandenen VM (Form kanister2080_r41.c / annette_r41.c). Jede Zeile hat Text UND Dauer (re15_msg_install_text +
 * re15_msg_install_durations): fehlt die Aufnahme synchro/STAGE2/room208x/mainNN.wav, laeuft die Zeile mit der Textdauer wie
 * jede RDT-Zeile ohne Ton (VERTRAG 2.1).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_story2_r42.h"
#include "re15_story_r42.h"
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_msg.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_inventory.h"
#include "re15_lock_se.h"
#include "re15_audio.h"
#include "re15_item_discard.h"
#include "re15_blink_r41.h"
#include "re15_gameflow.h"          /* re15_char_variant: 0 Leon, 1 Elza */
#include "re15_aufschliessen_r36.h"
#include "re15_gatorszene_r41.h"    /* re15_gatorszene_r41_key_lage */

#include "gen/story2_r42_texte.inc"
#include "gen/re2_door_bank.inc"    /* RE2_DOOR_SE_AUF_KEY (TUERSE.VBS Satz 4 = RE2 ROOM1140 Satz 0x25), wie gatorszene tor_druck */
#include "gen/r36_aufschliessen.inc" /* RE15_AUF_ART_RIEGEL */

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;
extern void re15_scd_show_message(uint8_t index, uint32_t pause_mask);   /* scd_vm.c (sce-1-Weg), wie gatorszene_r41.c */
extern void (*re15_k2080_tuerton_hook)(void);                            /* kanister2080_r41.c: DOOR06 Satz 1 (PC-Anbindung) */

#define S2_LOG(...) do { fprintf(stderr, "[story2] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                         fputc('\n', stderr); } while (0)

unsigned g_re15_s2_zaehler[8];

static int basis_ist(uint16_t room, uint16_t b) { return (uint16_t)(room & 0xFFFEu) == b; }
static int elza(void) { return re15_char_variant() != 0; }
static int bit9(int b) { return re15_game_flag_get(RE15_R42_BANK, (uint8_t)b) ? 1 : 0; }
static void bit9_setzen(int b) { re15_game_flag_set(RE15_R42_BANK, (uint8_t)b, 1); }
static int riegel(void) { return re15_game_flag_get(RE15_S2_RIEGEL_BANK, RE15_S2_RIEGEL_BIT) ? 1 : 0; }

/* ==== Texte ============================================================================================================= */
const uint8_t *re15_story2_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < S2_TEXTE_N; i++)
        if (k_s2_texte[i].raum == room_id && k_s2_texte[i].id == id) {
            if (out_len) *out_len = k_s2_texte[i].n;
            return k_s2_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

static void texte_einsetzen(uint16_t room_id)
{
    for (int i = 0; i < S2_TEXTE_N; i++) {
        if (k_s2_texte[i].raum != room_id) continue;
        re15_msg_install_text(k_s2_texte[i].id, k_s2_texte[i].b, k_s2_texte[i].n);
        const int d = re15_msg_compute_duration(k_s2_texte[i].b, k_s2_texte[i].n, 0);   /* wie re15_msg_load_room_block */
        if (d > 0 && d < 65535) re15_msg_install_durations(k_s2_texte[i].id, d);
    }
}

/* ==== Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c). Vorbild je Form: =========================================
 *   Set `22 bb ii vv`            ROOM2080 sub02 @0x0CB6 (Set(2,7,1)) / @0x0CBA (Set(1,27,1))
 *   Cut_chg `29 cc`              ROOM2080 sub02 @0x0CC2;  Cut_auto `3c 01` @0x0CDE
 *   Work_set `2e kk ii` + Nop    ROOM2080 sub02 @0x0CE8 (`2e 01 00` `00`)
 *   Plc_ret `42` + Nop           ROOM2080 sub02 @0x0CEC
 *   Sce_em_set (20 B)            ROOM2000 @0x017B4 (Marvin), ROOM2001 @0x0179E (John) / @0x0180A (Sherry); Band = pc[4]
 *   Plc_dest `40 00 mm bb x z`   Modus 9 drehen ROOM2040 sub02 @0x01648, Modus 5 laufen ROOM2040 sub02 @0x0165C
 *   Ankunfts-Poll `11 00 08 00 02 00 12 04 21 05 bb 00`   ROOM2040 sub02 @0x01650..@0x01658 (Do {Evt_next} Edwhile Ck(5,bb)=0)
 *   Message_on `2b id 00 00`     ROOM3000 sub03 @0x016B6 (Maske 0 = Untertitel)
 *   Sleep `09 0a nn nn`          ROOM2080 sub02 @0x0CBE
 *   Evt_end `01 00`              ROOM2080 sub02 @0x0CF0 */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_CUT(c)          0x29, (c)
#define OP_CUT_AUTO        0x3c, 0x01
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00
#define OP_PLC_RET         0x42, 0x00
#define OP_DEST(m,bit,x,z) 0x40, 0x00, (m), (bit), LE16(x), LE16(z)
#define OP_WARTE(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_END             0x01, 0x00
#define OP_EM(slot,typ,x,z,gier) 0x44, (slot), (typ), RE15_S2_NPC_GRID, RE15_S2_BAND_OBEN, 0x00, 0x00, 0xff, \
                           LE16(x), LE16(RE15_S2_Y_OBEN), LE16(z), 0x00, 0x00, LE16(gier), 0x00, 0x00
/* Eine Zeile: Signal "frei" loeschen, Zeile oeffnen, 2 Bilder (die Zeile steht), dann warten, bis Zeile UND Stimme zu Ende sind
 * (Takt setzt RE15_S2_SIG_FREI; Form RE2 Wsleep/Wsleeping ROOM7020 sub12 @0x079A/@0x079B = kanister2080 RE15_K2080_SIG_WSLEEP). */
#define OP_ZEILE(id)       OP_SET(5, RE15_S2_SIG_FREI, 0), 0x2b, (id), 0x00, 0x00, OP_SLEEP(2), OP_WARTE(RE15_S2_SIG_FREI)
#define OP_RIEGEL          OP_SET(2, 7, 1), OP_SET(1, 27, 1)
#define OP_RIEGEL_LOS      OP_SET(2, 7, 0), OP_SET(1, 27, 0)
#define M_LAUF   0x05      /* RUN: ROOM2040 sub02 @0x0165C `40 00 05 20` (Ankunft d < 300 @0x80051764) */
#define M_DREH   0x09      /* drehen: ROOM2040 sub02 @0x01648 `40 00 09 20` */
#define S2_SPIELER_X  (-11160)   /* Lauf e2: Ankunft am Rechner (WEGPLAN "GEHE -11300 -7350" -> (-11160,-7316)) */
#define S2_SPIELER_Z  (-7316)

/* Leon: Marvin kommt durch den Gang, sechs Zeilen (L1, K-07 "Marvin bleibt zurueck"). */
static const uint8_t k_prog_leon[] = {
    OP_RIEGEL,                                                         /* @0x0CB6/@0x0CBA */
    OP_CUT(RE15_S2_CUT_SZENE),
    OP_EM(RE15_S2_NPC_SLOT, RE15_S2_TYP_MARVIN, RE15_S2_NPC_START_X, RE15_S2_NPC_START_Z, 1024),
    OP_SLEEP(1),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_LAUF, RE15_S2_AN_NPC, RE15_S2_NPC_X, RE15_S2_NPC_Z), OP_WARTE(RE15_S2_AN_NPC),
    OP_WORK(1, 0), OP_DEST(M_DREH, RE15_S2_AN_SPIELER, RE15_S2_NPC_X, RE15_S2_NPC_Z), OP_WARTE(RE15_S2_AN_SPIELER),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_DREH, RE15_S2_AN_NPC, S2_SPIELER_X, S2_SPIELER_Z), OP_WARTE(RE15_S2_AN_NPC),
    OP_ZEILE(43), OP_ZEILE(44), OP_ZEILE(45), OP_ZEILE(46), OP_ZEILE(47), OP_ZEILE(48),
    OP_SET(RE15_R42_BANK, RE15_R42_BIT_2080_SZENE, 1),
    OP_CUT_AUTO, OP_RIEGEL_LOS,                                        /* @0x0CDE, @0x0CE0/@0x0CE4 */
    OP_WORK(1, 0), OP_PLC_RET,                                         /* @0x0CE8/@0x0CEC */
    OP_END
};
/* Elza: John kommt, vier Zeilen; Tuer 2030; Sherry kommt; Elza/Sherry; John begruesst Sherry (F-03); John bleibt (K-07). */
static const uint8_t k_prog_elza[] = {
    OP_RIEGEL,
    OP_CUT(RE15_S2_CUT_SZENE),
    OP_EM(RE15_S2_NPC_SLOT, RE15_S2_TYP_JOHN, RE15_S2_NPC_START_X, RE15_S2_NPC_START_Z, 1024),
    OP_SLEEP(1),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_LAUF, RE15_S2_AN_NPC, RE15_S2_NPC_X, RE15_S2_NPC_Z), OP_WARTE(RE15_S2_AN_NPC),
    OP_WORK(1, 0), OP_DEST(M_DREH, RE15_S2_AN_SPIELER, RE15_S2_NPC_X, RE15_S2_NPC_Z), OP_WARTE(RE15_S2_AN_SPIELER),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_DREH, RE15_S2_AN_NPC, S2_SPIELER_X, S2_SPIELER_Z), OP_WARTE(RE15_S2_AN_NPC),
    OP_ZEILE(43), OP_ZEILE(44), OP_ZEILE(45), OP_ZEILE(46),
    OP_SET(5, RE15_S2_SIG_TUER, 1), OP_SLEEP(30),                      /* "Man hoert die Tuer nach 2030" (Konzept E1 Punkt 2) */
    OP_EM(RE15_S2_SHERRY_SLOT, RE15_S2_TYP_SHERRY, RE15_S2_SHERRY_START_X, RE15_S2_SHERRY_START_Z, 1024),
    OP_SLEEP(1),
    OP_WORK(2, RE15_S2_SHERRY_SLOT), OP_DEST(M_LAUF, RE15_S2_AN_SHERRY, RE15_S2_SHERRY_ZW_X, RE15_S2_SHERRY_ZW_Z),
    OP_WARTE(RE15_S2_AN_SHERRY),
    OP_DEST(M_LAUF, RE15_S2_AN_SHERRY, RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z), OP_WARTE(RE15_S2_AN_SHERRY),
    OP_WORK(1, 0), OP_DEST(M_DREH, RE15_S2_AN_SPIELER, RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z), OP_WARTE(RE15_S2_AN_SPIELER),
    OP_WORK(2, RE15_S2_SHERRY_SLOT), OP_DEST(M_DREH, RE15_S2_AN_SHERRY, S2_SPIELER_X, S2_SPIELER_Z), OP_WARTE(RE15_S2_AN_SHERRY),
    OP_ZEILE(47), OP_ZEILE(48),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_DREH, RE15_S2_AN_NPC, RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z), OP_WARTE(RE15_S2_AN_NPC),
    OP_WORK(2, RE15_S2_SHERRY_SLOT), OP_DEST(M_DREH, RE15_S2_AN_SHERRY, RE15_S2_NPC_X, RE15_S2_NPC_Z), OP_WARTE(RE15_S2_AN_SHERRY),
    OP_ZEILE(49),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_DREH, RE15_S2_AN_NPC, S2_SPIELER_X, S2_SPIELER_Z), OP_WARTE(RE15_S2_AN_NPC),
    OP_WORK(1, 0), OP_DEST(M_DREH, RE15_S2_AN_SPIELER, RE15_S2_NPC_X, RE15_S2_NPC_Z), OP_WARTE(RE15_S2_AN_SPIELER),
    OP_ZEILE(50), OP_ZEILE(51),
    OP_SET(RE15_R42_BANK, RE15_R42_BIT_2080_SZENE, 1),
    OP_CUT_AUTO, OP_RIEGEL_LOS,
    OP_WORK(1, 0), OP_PLC_RET,
    OP_END
};
/* Wiederbetreten nach der Szene (Bit 118) und vor der Szene an der Leiter (Bit 120): der Begleiter wartet (K-07). */
static const uint8_t k_warten_leon[] = {
    OP_EM(RE15_S2_NPC_SLOT, RE15_S2_TYP_MARVIN, RE15_S2_NPC_X, RE15_S2_NPC_Z, 1024), OP_END };
static const uint8_t k_warten_elza[] = {
    OP_EM(RE15_S2_NPC_SLOT, RE15_S2_TYP_JOHN, RE15_S2_NPC_X, RE15_S2_NPC_Z, 1024),
    OP_EM(RE15_S2_SHERRY_SLOT, RE15_S2_TYP_SHERRY, RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z, 1024), OP_END };
static const uint8_t k_evt_end[2] = { OP_END };

const uint8_t *re15_story2_r42_programm(int e, int *out_len)
{
    if (out_len) *out_len = e ? (int)sizeof k_prog_elza : (int)sizeof k_prog_leon;
    return e ? k_prog_elza : k_prog_leon;
}

static int starten(const uint8_t *p)
{
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, p) == 0) return slot;
    return -1;
}

/* ==== Plaetze ============================================================================================================ */
static int16_t le_s16(const uint8_t *p) { return (int16_t)(uint16_t)(p[0] | (p[1] << 8)); }

/* Ereignis-Platz (Form ventilkette_r41.c ereignis_platz: Aot_set sce 3 sat 0x31 Nutzlast `ff 00 18 <Ereignis>`). */
static void ereignis_platz(int slot, int32_t x, int32_t z, int32_t w, int32_t d, uint8_t band)
{
    re15_aot_set(slot, RE15_AOT_TYPE_GENERIC, RE15_S2_EREIGNIS, x + w / 2, z + d / 2, w / 2, d / 2);
    g_aot.slots[slot].sce_flags = 0;
    g_aot.slots[slot].band      = band;
    re15_aot_retype(slot, 3, RE15_S2_SAT, 0x00FF, (uint16_t)((RE15_S2_EREIGNIS << 8) | 0x18), 0);
}

/* Tuer-Satz -> Ereignis (Form gatorszene_r41.c tor_sperren: Aot_reset-Umwidmung, Nutzlast +3 = Sub), Lage fuers Zurueckwidmen. */
static int16_t s_tuer_next[RE15_AOT_MAX][3];
static int tuer_zu_ereignis(int slot)
{
    const re15_aot_t *a = &g_aot.slots[slot];
    if (!a->active || a->type != RE15_AOT_TYPE_DOOR) return 0;
    const re15_aot_door_params_t *dp = &g_aot.door_params[slot];
    s_tuer_next[slot][0] = (int16_t)dp->spawn_x; s_tuer_next[slot][1] = (int16_t)dp->spawn_y; s_tuer_next[slot][2] = (int16_t)dp->spawn_z;
    g_aot.slots[slot].band = dp->band;
    re15_aot_retype(slot, 3, RE15_S2_SAT, 0, (uint16_t)(RE15_S2_EREIGNIS << 8), 0);
    return 1;
}
static void ereignis_zu_tuer(int slot)
{
    g_aot.slots[slot].band = g_aot.door_params[slot].band;
    re15_aot_retype(slot, 2, RE15_S2_SAT, (uint16_t)s_tuer_next[slot][0], (uint16_t)s_tuer_next[slot][1],
                    (uint16_t)s_tuer_next[slot][2]);
    g_aot.slots[slot].was_inside = 1;   /* kein Durchgang bei DIESEM Druck (RE2 @0x80051698, wie tor_druck) */
}

/* Ausgelieferte Bytes im GELADENEN RDT pruefen (Raumwechsel-sicher: g_room_rdt ist der aktuelle Raum). */
static const uint8_t *rdt_bytes(int off, const uint8_t *soll, int n)
{
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->raw || off < 0 || off + n > r->raw_size) return NULL;
    if (soll && memcmp(r->raw + off, soll, (size_t)n) != 0) return NULL;
    return r->raw + off;
}

/* ==== 2080/2081: Hinweis beim Ablassen, Schluesselloch, Szene (K-01, L1, E1, K-07) ====================================== */
static const uint8_t k_rechner[20] = { 0x2c, 0x04, 0x03, 0x31, 0x08, 0x00, 0xee, 0xd0, 0xae, 0xe3, 0xe8, 0x03, 0xe8, 0x03,
                                       0xff, 0x00, 0x18, 0x02, 0x00, 0x00 };   /* sub00 @0x0C1A */
/* sub02 mit dem Hinweis: Original-Bytes @0x0C86..@0x0CEE, dann Message_on 52 `2b 34 ff ff` + Evt_next + Nop (Form des ersten
 * Satzes @0x0C86 `2b 00 ff ff 02 00`: Text-Platz-Maske 0xffff) VOR dem Endif @0x0CEE, Blocklaenge @0x0C8E 0x60 -> 0x66. Der
 * Hinweis kommt nur im Ja-Zweig, nach Plc_ret @0x0CEC (Wasser gesunken, Spieler frei). */
static uint8_t s_ablass[(RE15_S2_ABLASS_ENDE - RE15_S2_ABLASS_OFF) + 6];
static int     s_ablass_n;
static int ablass_bauen(void)
{
    const int n0 = RE15_S2_ABLASS_ENDIF - RE15_S2_ABLASS_OFF;   /* bis vor das Endif */
    static const uint8_t kopf[] = { 0x2b, 0x00, 0xff, 0xff, 0x02, 0x00, 0x06, 0x00, 0x60, 0x00, 0x21, 0x0c, 0x1f, 0x00,
                                    0x22, 0x03, 0xf5, 0x01 };   /* @0x0C86..@0x0C97 */
    static const uint8_t fuss[] = { 0x2e, 0x01, 0x00, 0x00, 0x42, 0x00, 0x08, 0x00, 0x01, 0x00 };   /* @0x0CE8..@0x0CF1 */
    const uint8_t *a = rdt_bytes(RE15_S2_ABLASS_OFF, kopf, (int)sizeof kopf);
    const uint8_t *e = rdt_bytes(RE15_S2_ABLASS_ENDIF - 6, fuss, (int)sizeof fuss);
    if (!a || !e) return 0;
    memcpy(s_ablass, a, (size_t)n0);
    s_ablass[RE15_S2_ABLASS_IFEL - RE15_S2_ABLASS_OFF + 2] = 0x66;
    const uint8_t hinweis[6] = { 0x2b, RE15_R42_MSG_2080_LOCH, 0xff, 0xff, 0x02, 0x00 };
    memcpy(s_ablass + n0, hinweis, 6);
    memcpy(s_ablass + n0 + 6, a + n0, (size_t)(RE15_S2_ABLASS_ENDE - RE15_S2_ABLASS_ENDIF));
    s_ablass_n = n0 + 6 + (RE15_S2_ABLASS_ENDE - RE15_S2_ABLASS_ENDIF);
    return 1;
}

static int s_szene = 0;          /* 1 = Szenen-Programm laeuft (Takt setzt die Signale) */
static int s_loch = 0;           /* 1 = Schluesselloch-Platz angelegt */
int re15_story2_r42_szene_laeuft(void) { return s_szene; }

static void install_2080(uint16_t room_id)
{
    texte_einsetzen(room_id);
    s_szene = 0; s_loch = 0;
    if (bit9(RE15_R42_BIT_2080_SZENE) && !bit9(RE15_R42_BIT_LEITER)) {
        const int t = starten(elza() ? k_warten_elza : k_warten_leon);
        S2_LOG("ROOM%04X: %s wartet (Bit %d = 1, Bit %d = 0, K-07) Faden %d", (unsigned)room_id,
               elza() ? "John + Sherry" : "Marvin", RE15_R42_BIT_2080_SZENE, RE15_R42_BIT_LEITER, t);
    }
    if (!re15_game_flag_get(RE15_S2_WASSER_BANK, RE15_S2_WASSER_BIT) || bit9(RE15_R42_BIT_2080_TOR)) return;
    const uint8_t *r = rdt_bytes(RE15_S2_RECHNER_OFF, k_rechner, (int)sizeof k_rechner);
    if (!r || g_aot.slots[RE15_S2_RECHNER_SLOT].active) {
        S2_LOG("ROOM%04X: Schluesselloch NICHT angelegt (%s)", (unsigned)room_id, r ? "Platz 4 belegt" : "Bytes @0x0C1A anders");
        return;
    }
    ereignis_platz(RE15_S2_RECHNER_SLOT, le_s16(r + 6), le_s16(r + 8), le_s16(r + 10), le_s16(r + 12), r[4]);
    s_loch = 1;
    S2_LOG("ROOM%04X: Schluesselloch neben dem Rechner = Platz %d (Rechteck @0x0C1A x %d..%d z %d..%d Band %d) Ereignis %d",
           (unsigned)room_id, RE15_S2_RECHNER_SLOT, le_s16(r + 6), le_s16(r + 6) + le_s16(r + 10), le_s16(r + 8),
           le_s16(r + 8) + le_s16(r + 12), r[4], RE15_S2_EREIGNIS);
}

static const uint8_t *loch_druck(uint16_t room_id)
{
    if (re15_inv_find_item(RE15_S2_KEY_ITEM) >= 0) {
        /* RE2 LAB_80051514 Treffer-Zweig wie tor_druck: Besitz vor dem Text (@0x80051628/34), Text (@0x8005164c), Aufschliess-Ton
         * (@0x80051654/58), Wegwerf-Fortsetzung (@0x80051670), Schlossbit (@0x80051690). */
        (void)re15_discard_besitz_vor_nachricht(room_id, RE15_R42_MSG_2080_TOR_AUF);
        re15_scd_show_message(RE15_R42_MSG_2080_TOR_AUF, 0xffffu << 16);
        re15_audio_re2_door_se(RE2_DOOR_SE_AUF_KEY);
        re15_discard_notice_message(room_id, RE15_R42_MSG_2080_TOR_AUF);
        re15_game_flag_set(RE15_R42_BANK, RE15_S2_TOR_BIT, 1);           /* Tor 2090 <-> 20A0 offen (gatorszene tor_sperren) */
        bit9_setzen(RE15_R42_BIT_2080_TOR);
        re15_aot_retype(RE15_S2_RECHNER_SLOT, 0, 0, 0, 0, 0);            /* Platz tot (Aot_reset sce 0, @0x8004305C inert) */
        s_loch = 0;
        g_re15_s2_zaehler[2]++;
        if (bit9(RE15_R42_BIT_2080_SZENE)) return k_evt_end;              /* Szene nur einmal */
        s_szene = 1;
        g_re15_s2_zaehler[7]++;
        S2_LOG("ROOM%04X: Schluessel benutzt (msg %d), Bits (9,%d)/(9,%d) = 1 -> Szene %s", (unsigned)room_id,
               RE15_R42_MSG_2080_TOR_AUF, RE15_S2_TOR_BIT, RE15_R42_BIT_2080_TOR, elza() ? "E1 (John, Sherry)" : "L1 (Marvin)");
        return elza() ? k_prog_elza : k_prog_leon;
    }
    /* Nicht-Treffer-Zweig: ein Verschlossen-Ton Art M (@0x800516a0/a4) + Text (@0x800516b8). */
    re15_audio_re2_door_se(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));
    re15_scd_show_message(RE15_R42_MSG_2080_TOR_ZU, 0xffffu << 16);
    g_re15_s2_zaehler[1]++;
    S2_LOG("ROOM%04X: Schluesselloch ohne Schluessel (msg %d)", (unsigned)room_id, RE15_R42_MSG_2080_TOR_ZU);
    return k_evt_end;
}

/* ==== Tueren K-02/K-03 =================================================================================================== */
static uint8_t s_tuer_prog[32 + 2];
static void install_2070(uint16_t room_id)
{
    texte_einsetzen(room_id);
    if (!riegel() && g_aot.slots[RE15_S2_2070_RIEGEL_SLOT].active && g_aot.slots[RE15_S2_2070_RIEGEL_SLOT].type != RE15_AOT_TYPE_NONE) {
        re15_aot_retype(RE15_S2_2070_RIEGEL_SLOT, 3, RE15_S2_SAT, 0, (uint16_t)(RE15_S2_EREIGNIS << 8), 0);
        S2_LOG("ROOM%04X: Tuer nach 11A0 (Platz %d @0x0AC2) -> \"locked from the other side\" (K-02, (3,%d) = 0)",
               (unsigned)room_id, RE15_S2_2070_RIEGEL_SLOT, RE15_S2_RIEGEL_BIT);
    }
    if (!bit9(RE15_R42_BIT_TUER_2040) && tuer_zu_ereignis(RE15_S2_2070_ZU2040_SLOT))
        S2_LOG("ROOM%04X: Tuer nach 2040 (Platz %d @0x0B1C) -> \"locked from the other side\" (K-03, Bit %d = 0)",
               (unsigned)room_id, RE15_S2_2070_ZU2040_SLOT, RE15_R42_BIT_TUER_2040);
}
static void install_11a0(uint16_t room_id)
{
    texte_einsetzen(room_id);
    const re15_aot_t *a = &g_aot.slots[RE15_S2_11A0_TUER_SLOT];
    if (!riegel() && a->active && a->type == RE15_AOT_TYPE_MESSAGE) {
        re15_aot_retype(RE15_S2_11A0_TUER_SLOT, 3, RE15_S2_SAT, 0, (uint16_t)(RE15_S2_EREIGNIS << 8), 0);
        S2_LOG("ROOM%04X: Tuer nach 2070 (Platz %d @0x00F42) schliesst von hier auf (K-02)", (unsigned)room_id, RE15_S2_11A0_TUER_SLOT);
    }
}
static void install_2040(uint16_t room_id)
{
    texte_einsetzen(room_id);
    if (!bit9(RE15_R42_BIT_TUER_2040) && tuer_zu_ereignis(RE15_S2_2040_TUER_SLOT))
        S2_LOG("ROOM%04X: Tuer nach 2070 (Platz %d @0x0131C) schliesst von hier auf (K-03)", (unsigned)room_id, RE15_S2_2040_TUER_SLOT);
}

static const uint8_t *andere_seite(uint16_t room_id)
{
    re15_audio_re2_door_se(re15_lock_se_satz_fuer_art(RE15_LOCK_SE_ART_M));   /* 11A0 msg 2 = Art M (gen/lock_se_sites.inc) */
    re15_scd_show_message(RE15_R42_MSG_2070_ANDERE_SEITE, 0xffffu << 16);
    g_re15_s2_zaehler[3]++;
    S2_LOG("ROOM%04X: \"It's locked from the other side.\" (msg %d)", (unsigned)room_id, RE15_R42_MSG_2070_ANDERE_SEITE);
    return k_evt_end;
}

static const uint8_t *aufschliessen_11a0(uint16_t room_id)
{
    /* Form ROOM2070 sub02 @0x0CDA gespiegelt: Riegel (3,151) @0x0CE4, Text @0x0CE8 (+ Riegel-Ton RE2 @0x800515f8), Tuer-Satz.
     * Die Tuer selbst = der ausgelieferte Door_aot_set @0x00F5A (Else-Zweig von main00), als Programm ausgefuehrt. */
    static const uint8_t soll[8] = { 0x3b, 0x00, 0x02, 0x31, 0x00, 0x00, 0x18, 0x15 };
    const uint8_t *t = rdt_bytes(RE15_S2_11A0_TUER_OFF, soll, (int)sizeof soll);
    if (!t) { S2_LOG("ROOM%04X: Door_aot_set @0x00F5A anders - kein Aufschliessen", (unsigned)room_id); return k_evt_end; }
    memcpy(s_tuer_prog, t, 32);
    s_tuer_prog[32] = 0x01; s_tuer_prog[33] = 0x00;
    re15_game_flag_set(RE15_S2_RIEGEL_BANK, RE15_S2_RIEGEL_BIT, 1);
    bit9_setzen(RE15_R42_BIT_TUER_11A);
    re15_scd_show_message(RE15_R42_MSG_11A_AUFGESPERRT, 0xffffu << 16);
    re15_aufschliessen_r36_art_starten(RE15_AUF_ART_RIEGEL, room_id, RE15_R42_MSG_11A_AUFGESPERRT);
    g_re15_s2_zaehler[4]++;
    S2_LOG("ROOM%04X: Tuer nach 2070 aufgeschlossen (msg %d), (3,%d) = 1, Bit %d = 1", (unsigned)room_id,
           RE15_R42_MSG_11A_AUFGESPERRT, RE15_S2_RIEGEL_BIT, RE15_R42_BIT_TUER_11A);
    return s_tuer_prog;
}

static const uint8_t *aufschliessen_2040(uint16_t room_id)
{
    bit9_setzen(RE15_R42_BIT_TUER_2040);
    re15_scd_show_message(RE15_R42_MSG_2040_AUFGESPERRT, 0xffffu << 16);
    re15_aufschliessen_r36_art_starten(RE15_AUF_ART_RIEGEL, room_id, RE15_R42_MSG_2040_AUFGESPERRT);
    ereignis_zu_tuer(RE15_S2_2040_TUER_SLOT);
    g_re15_s2_zaehler[5]++;
    S2_LOG("ROOM%04X: Tuer nach 2070 aufgeschlossen (msg %d), Bit %d = 1", (unsigned)room_id, RE15_R42_MSG_2040_AUFGESPERRT,
           RE15_R42_BIT_TUER_2040);
    return k_evt_end;
}

/* ==== Uhr 2090/2091 (K-08, L-01/E-01) ===================================================================================== */
static int s_uhr_da = 0;          /* Uhr liegt neben dem Schluessel (dieser Raumbesuch) */
static int s_key_vorher = 0;      /* Bit 112 beim Raumaufbau */
static int s_hinweis = 0;         /* 1 = Hinweis faellig, 2 = gezeigt */
static int prop_von_obj(int obj)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == obj) return k;
    return -1;
}
static void uhr_legen(uint16_t room_id)
{
    if (bit9(RE15_S2_UHR_BIT)) return;
    re15_aot_t *a = &g_aot.slots[RE15_S2_UHR_SLOT];
    if (a->active && a->type == RE15_AOT_TYPE_ITEM && g_aot.item_params[RE15_S2_UHR_SLOT].item_type == RE15_S2_UHR_ITEM)
        re15_aot_retype(RE15_S2_UHR_SLOT, 0, 0, 0, 0, 0);            /* der Original-Platz am Tor entfaellt (Aot_reset sce 0) */
    int32_t kx = 0, ky = 0, kz = 0;
    re15_gatorszene_r41_key_lage(&kx, &ky, &kz);
    /* Item-Zone wie der Schluessel (gatorszene key_bei_leiche: re15_aot_set_item_tk_prop = Item_aot_set @0x80040644-Form) mit den
     * Original-Werten von @0x00A12 (Item 0x3F x1, Taken-Bit 90, Modell obj 0), Rechteck = das des Schluessels: wer den Schluessel
     * erreicht, erreicht auch die Uhr; der Platz liegt hinter dem Schluessel-Platz, die Suche (aot_common.c, aufsteigend) nimmt
     * zuerst den Schluessel - dann folgt der Hinweis msg 47. */
    re15_aot_set_item_tk_prop(RE15_S2_UHR_PLATZ, kx, kz, RE15_S2_UHR_HALB, RE15_S2_UHR_HALB, RE15_S2_UHR_ITEM, 1, RE15_S2_UHR_BIT,
                              RE15_S2_UHR_OBJ);
    g_aot.slots[RE15_S2_UHR_PLATZ].sce_flags = RE15_S2_SAT;
    g_aot.slots[RE15_S2_UHR_PLATZ].band = 0;                         /* wie die Schluessel-Zone (GSZ_KEY_AOT band 0) */
    const int k = prop_von_obj(RE15_S2_UHR_OBJ);
    if (k >= 0) {
        g_scd.props[k].active = 1;
        g_scd.props[k].x = kx + RE15_S2_UHR_VERSATZ; g_scd.props[k].y = ky; g_scd.props[k].z = kz;
        g_scd.props[k].parent_obj = -1;
    }
    re15_blink_r41_setzen(room_id, RE15_S2_UHR_OBJ, 1);              /* dasselbe Mittel wie der Schluessel (gatorszene key_bei_leiche) */
    s_uhr_da = 1;
    S2_LOG("ROOM%04X: Taschenuhr (Item 0x%02X, Platz %d, obj %d) neben dem Schluessel (%d,%d,%d), Modell bei x %d, Puls an",
           (unsigned)room_id, RE15_S2_UHR_ITEM, RE15_S2_UHR_PLATZ, RE15_S2_UHR_OBJ, (int)kx, (int)ky, (int)kz,
           (int)(kx + RE15_S2_UHR_VERSATZ));
}
static void install_2090(uint16_t room_id)
{
    texte_einsetzen(room_id);
    s_uhr_da = 0; s_hinweis = 0;
    s_key_vorher = bit9(RE15_S2_KEY_GENOMMEN_BIT);
    if (bit9(RE15_S2_UHR_BIT)) return;
    if (!bit9(RE15_S2_GATOR_TOT_BIT)) {
        /* vor dem Tod liegt sie nicht da (Konzept 6b: die Stroemung trug sie zum Alligator; "Nach dem Kampf liegt sie ...") */
        if (g_aot.slots[RE15_S2_UHR_SLOT].active && g_aot.slots[RE15_S2_UHR_SLOT].type == RE15_AOT_TYPE_ITEM)
            re15_aot_retype(RE15_S2_UHR_SLOT, 0, 0, 0, 0, 0);
        const int k = prop_von_obj(RE15_S2_UHR_OBJ);
        if (k >= 0) g_scd.props[k].active = 0;
        S2_LOG("ROOM%04X: Taschenuhr (Platz %d @0x00A12) bis zum Tod des Alligators verborgen (Bit %d = 0)", (unsigned)room_id,
               RE15_S2_UHR_SLOT, RE15_S2_GATOR_TOT_BIT);
        return;
    }
    uhr_legen(room_id);
}
void re15_story2_r42_schluessel_liegt(uint16_t room_id)
{
    if (!basis_ist(room_id, RE15_S2_RAUM_2090) || bit9(RE15_S2_UHR_BIT)) return;
    uhr_legen(room_id);
}

/* ==== Haken =============================================================================================================== */
static int s_tor_alt = 0;
void re15_story2_r42_tor2090_alt_setzen(int an) { s_tor_alt = an ? 1 : 0; }
int  re15_story2_r42_tor2090_nimmt_schluessel(void) { return s_tor_alt; }

void re15_story2_r42_install(uint16_t room_id)
{
    s_szene = 0;
    if (basis_ist(room_id, RE15_S2_RAUM_2080)) install_2080(room_id);
    else if (basis_ist(room_id, RE15_S2_RAUM_2090)) install_2090(room_id);
    else if (basis_ist(room_id, RE15_S2_RAUM_2070)) install_2070(room_id);
    else if (basis_ist(room_id, RE15_S2_RAUM_11A0)) install_11a0(room_id);
    else if (basis_ist(room_id, RE15_S2_RAUM_2040)) install_2040(room_id);
}

const uint8_t *re15_story2_r42_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (basis_ist(room_id, RE15_S2_RAUM_2080)) {
        if (event_id == RE15_S2_RECHNER_EREIGNIS && !re15_game_flag_get(RE15_S2_WASSER_BANK, RE15_S2_WASSER_BIT) && ablass_bauen()) {
            g_re15_s2_zaehler[0]++;
            S2_LOG("ROOM%04X: Rechner (Ereignis 2) = sub02 mit Hinweis msg %d vor dem Endif @0x0CEE", (unsigned)room_id,
                   RE15_R42_MSG_2080_LOCH);
            return s_ablass;
        }
        if (event_id == RE15_S2_EREIGNIS && s_loch) return loch_druck(room_id);
        return NULL;
    }
    if (event_id != RE15_S2_EREIGNIS) return NULL;
    if (basis_ist(room_id, RE15_S2_RAUM_2070)) return andere_seite(room_id);
    if (basis_ist(room_id, RE15_S2_RAUM_11A0) && !riegel()) return aufschliessen_11a0(room_id);
    if (basis_ist(room_id, RE15_S2_RAUM_2040) && !bit9(RE15_R42_BIT_TUER_2040)) return aufschliessen_2040(room_id);
    return NULL;
}

void re15_story2_r42_tick(void)
{
    const uint16_t room = (uint16_t)g_current_room_id;
    if (basis_ist(room, RE15_S2_RAUM_2080) && s_szene) {
        const int zeile  = g_scd.message_active != 0;
        const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
        if (!zeile && !stimme) re15_game_flag_set(5, RE15_S2_SIG_FREI, 1);
        if (re15_game_flag_get(5, RE15_S2_SIG_TUER)) {
            re15_game_flag_set(5, RE15_S2_SIG_TUER, 0);
            if (re15_k2080_tuerton_hook) re15_k2080_tuerton_hook();
            S2_LOG("Tuer-Ton 2030 (DOOR06 Satz 1, Door_exit @0x800141f0) - Sherry kommt");
        }
        if (bit9(RE15_R42_BIT_2080_SZENE) && !g_scd.message_active) {
            s_szene = 0;
            for (int i = 1; i < RE15_ACTOR_MAX; i++)
                if (g_actors[i].active && (g_actors[i].type == RE15_S2_TYP_MARVIN || g_actors[i].type == RE15_S2_TYP_JOHN ||
                                           g_actors[i].type == RE15_S2_TYP_SHERRY))
                    S2_LOG("ROOM%04X: NPC 0x%02X Aktor %d steht bei (%d,%d,%d) Gier %d", (unsigned)room, g_actors[i].type, i,
                           (int)g_actors[i].x, (int)g_actors[i].y, (int)g_actors[i].z, (int)g_actors[i].rot_y);
            S2_LOG("ROOM%04X: Szene %s fertig, Bit %d = 1, Begleiter bleibt zurueck (K-07)", (unsigned)room, elza() ? "E1" : "L1",
                   RE15_R42_BIT_2080_SZENE);
        }
        return;
    }
    if (basis_ist(room, RE15_S2_RAUM_2090) && s_uhr_da) {
        if (bit9(RE15_S2_UHR_BIT)) { s_uhr_da = 0; return; }
        if (!s_key_vorher && bit9(RE15_S2_KEY_GENOMMEN_BIT) && s_hinweis == 0) s_hinweis = 1;
        if (s_hinweis == 1 && !g_scd.message_active) {
            re15_scd_show_message(RE15_R42_MSG_2090_UHR, 0xffffu << 16);
            s_hinweis = 2;
            g_re15_s2_zaehler[6]++;
            S2_LOG("ROOM%04X: Schluessel genommen, Uhr liegt noch -> msg %d", (unsigned)room, RE15_R42_MSG_2090_UHR);
        }
    }
}

int re15_story2_r42_discard_stelle(unsigned room_id, uint8_t msg_id, uint8_t *out_item)
{
    if (((uint16_t)room_id & 0xFFFEu) != RE15_S2_RAUM_2080 || msg_id != RE15_R42_MSG_2080_TOR_AUF) return 0;
    if (out_item) *out_item = RE15_S2_KEY_ITEM;
    return 1;
}

const uint8_t *re15_story2_r42_desc(uint8_t item_id)
{
    if (item_id == RE15_S2_UHR_ITEM && !elza()) return k_s2_uhr_leon_desc;   /* F-19 Gravur (Leon behaelt die Uhr) */
    return NULL;
}
