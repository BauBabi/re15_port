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
#include "re15_gatorszene_r41.h"    /* re15_gatorszene_r41_key_lage, RE15_GSZ_KEY_SKALA_Q12 */
#include "re15_md1.h"              /* Uhr-Skala (Befund 6) */

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
/* Eine Zeile: Signal "frei" loeschen, Zeile oeffnen, dann warten, bis Zeile UND Stimme zu Ende sind (Takt setzt RE15_S2_SIG_FREI;
 * Form RE2 Wsleep/Wsleeping ROOM7020 sub12 @0x079A/@0x079B = kanister2080 RE15_K2080_SIG_WSLEEP).
 * PORT-WAHL Sleep 2 (Pruefung 1, Befund 7): der Takt (scd_vm.c re15_story2_r42_tick) laeuft in jedem Bild VOR den Faeden; die Zeile
 * steht nach dem Message_on sofort (g_scd.message_active), die Stimme aber wird im Message_on nur eingereiht (scd_queue_voice
 * scd_vm.c) und erst vom Audio-Takt gestartet, der g_re15_voice_laeuft setzt (audio_pc.c re15_audio_tick). Mit 2 Bildern sieht der
 * Takt die gestartete Stimme in jeder Reihenfolge von Audio-Takt und VM, bevor die Abfrage beginnt - sonst gaebe er "frei", solange
 * die Stimme noch in der Warteschlange liegt, und die naechste Zeile schnitte sie ab. */
#define OP_ZEILE(id)       OP_SET(5, RE15_S2_SIG_FREI, 0), 0x2b, (id), 0x00, 0x00, OP_SLEEP(2), OP_WARTE(RE15_S2_SIG_FREI)
#define OP_RIEGEL          OP_SET(2, 7, 1), OP_SET(1, 27, 1)
#define OP_RIEGEL_LOS      OP_SET(2, 7, 0), OP_SET(1, 27, 0)
#define M_GEHEN  0x04      /* WALK: ROOM1021 sub02 @0x020B2 `40 00 04 22` (Spieler-Tabelle 0x80073e30 [4]) */
#define M_LAUF   0x05      /* RUN: ROOM2040 sub02 @0x0165C `40 00 05 20` (Ankunft d < 300 @0x80051764) */
#define M_DREH   0x09      /* drehen: ROOM2040 sub02 @0x01648 `40 00 09 20` */
#define OP_DESTV(...)      OP_DEST(__VA_ARGS__)   /* Lage-Makros S2_P/S2_N/S2_S erst nach dem Einsetzen zerlegen */
#define S2_P     RE15_S2_SPIELER_X, RE15_S2_SPIELER_Z
#define S2_N     RE15_S2_NPC_X, RE15_S2_NPC_Z
#define S2_S     RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z

/* Anfang beider Fassungen: der Begleiter kommt von Suedwesten (W1 -> N) und bleibt stehen; dann wendet sich der Spieler vom Rechner
 * ab und geht nach P; dann drehen sich beide zueinander (Profil zur Kamera, re15_story2_r42.h Inszenierung). Jede Bewegung wird
 * einzeln abgewartet: die Ankunftsbits 0x20..0x22 liegen in Wort 1 der Bank 5, das der Takt in JEDEM Bild loescht (FUN_8003ebf4,
 * scd_vm.c `g_game.flags[5][1] = 0`) - ein Bit steht nur ein Bild lang und muss direkt nach seinem Plc_dest abgefragt werden (Form
 * ROOM2040 sub02 @0x0164C..@0x01658: Plc_dest, Ankunfts-Poll, naechster Plc_dest). OP_SLEEP(1) nach dem Sce_em_set =
 * Abschnittsende (irons_tod_1150.c OP_ABSCHNITT_ENDE: der neue Aktor laeuft einen Takt, bevor sein Plc_dest kommt). */
#define S2_ANKUNFT(typ)                                                     /* Riegel @0x0CB6/@0x0CBA, Abschnittsende */ \
    OP_RIEGEL, OP_CUT(RE15_S2_CUT_SZENE), \
    OP_EM(RE15_S2_NPC_SLOT, (typ), RE15_S2_NPC_START_X, RE15_S2_NPC_START_Z, RE15_S2_GIER_NPC_START), \
    OP_SLEEP(1), \
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_LAUF, RE15_S2_AN_NPC, RE15_S2_NPC_ZW_X, RE15_S2_NPC_ZW_Z), OP_WARTE(RE15_S2_AN_NPC), \
    OP_DESTV(M_LAUF, RE15_S2_AN_NPC, S2_N), OP_WARTE(RE15_S2_AN_NPC), \
    OP_WORK(1, 0), OP_DESTV(M_GEHEN, RE15_S2_AN_SPIELER, S2_P), OP_WARTE(RE15_S2_AN_SPIELER), \
    OP_DESTV(M_DREH, RE15_S2_AN_SPIELER, S2_N), OP_WARTE(RE15_S2_AN_SPIELER), \
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DESTV(M_DREH, RE15_S2_AN_NPC, S2_P), OP_WARTE(RE15_S2_AN_NPC)

/* Leon: Marvin kommt, sechs Zeilen (L1, K-07 "Marvin bleibt zurueck"). */
static const uint8_t k_prog_leon[] = {
    S2_ANKUNFT(RE15_S2_TYP_MARVIN),
    OP_ZEILE(43), OP_ZEILE(44), OP_ZEILE(45), OP_ZEILE(46), OP_ZEILE(47), OP_ZEILE(48),
    OP_SET(RE15_R42_BANK, RE15_R42_BIT_2080_SZENE, 1),
    OP_CUT_AUTO, OP_RIEGEL_LOS,                                        /* @0x0CDE, @0x0CE0/@0x0CE4 */
    OP_WORK(1, 0), OP_PLC_RET,                                         /* @0x0CE8/@0x0CEC */
    OP_END
};
/* Elza: John kommt, vier Zeilen; Tuer 2030 (beide wenden sich zur Treppe); Sherry kommt die Treppe herauf (E1 Punkt 2/3); Elza/Sherry;
 * John begruesst Sherry (F-03); John bleibt (K-07). */
static const uint8_t k_prog_elza[] = {
    S2_ANKUNFT(RE15_S2_TYP_JOHN),
    OP_ZEILE(43), OP_ZEILE(44), OP_ZEILE(45), OP_ZEILE(46),
    OP_SET(5, RE15_S2_SIG_TUER, 1),                                    /* "Man hoert die Tuer nach 2030" (Konzept E1 Punkt 2) */
    /* beide wenden sich zur Treppe; die Drehungen laufen, waehrend Sherry heraufkommt (ohne Abfrage: das Ankunftsbild faellt in den
     * Sleep, der naechste Plc_dest desselben Aktors setzt neu an, scd_vm.c op_plc_dest) */
    OP_WORK(1, 0), OP_DEST(M_DREH, RE15_S2_AN_SPIELER, RE15_S2_TREPPE_X, RE15_S2_TREPPE_Z),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DEST(M_DREH, RE15_S2_AN_NPC, RE15_S2_TREPPE_X, RE15_S2_TREPPE_Z),
    OP_SLEEP(RE15_S2_SHERRY_LAUF_BILDER),                              /* Weg Tuer 2030 -> Nische (re15_story2_r42.h) */
    OP_EM(RE15_S2_SHERRY_SLOT, RE15_S2_TYP_SHERRY, RE15_S2_SHERRY_START_X, RE15_S2_SHERRY_START_Z, RE15_S2_GIER_SHERRY_START),
    OP_SLEEP(1),
    OP_WORK(2, RE15_S2_SHERRY_SLOT), OP_DEST(M_LAUF, RE15_S2_AN_SHERRY, RE15_S2_SHERRY_ZW_X, RE15_S2_SHERRY_ZW_Z),
    OP_WARTE(RE15_S2_AN_SHERRY),
    OP_DESTV(M_LAUF, RE15_S2_AN_SHERRY, S2_S), OP_WARTE(RE15_S2_AN_SHERRY),
    OP_DESTV(M_DREH, RE15_S2_AN_SHERRY, S2_P), OP_WARTE(RE15_S2_AN_SHERRY),
    OP_WORK(1, 0), OP_DESTV(M_DREH, RE15_S2_AN_SPIELER, S2_S), OP_WARTE(RE15_S2_AN_SPIELER),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DESTV(M_DREH, RE15_S2_AN_NPC, S2_S), OP_WARTE(RE15_S2_AN_NPC),
    OP_ZEILE(47), OP_ZEILE(48),
    OP_WORK(2, RE15_S2_SHERRY_SLOT), OP_DESTV(M_DREH, RE15_S2_AN_SHERRY, S2_N), OP_WARTE(RE15_S2_AN_SHERRY),
    OP_ZEILE(49),
    OP_WORK(2, RE15_S2_NPC_SLOT), OP_DESTV(M_DREH, RE15_S2_AN_NPC, S2_P), OP_WARTE(RE15_S2_AN_NPC),
    OP_WORK(1, 0), OP_DESTV(M_DREH, RE15_S2_AN_SPIELER, S2_N), OP_WARTE(RE15_S2_AN_SPIELER),
    OP_ZEILE(50), OP_ZEILE(51),
    OP_SET(RE15_R42_BANK, RE15_R42_BIT_2080_SZENE, 1),
    OP_CUT_AUTO, OP_RIEGEL_LOS,
    OP_WORK(1, 0), OP_PLC_RET,
    OP_END
};
/* Wiederbetreten nach der Szene (Bit 118) und vor der Szene an der Leiter (Bit 120): der Begleiter wartet (K-07). */
static const uint8_t k_warten_leon[] = {
    OP_EM(RE15_S2_NPC_SLOT, RE15_S2_TYP_MARVIN, RE15_S2_NPC_X, RE15_S2_NPC_Z, RE15_S2_GIER_NPC_WARTEN), OP_END };
static const uint8_t k_warten_elza[] = {
    OP_EM(RE15_S2_NPC_SLOT, RE15_S2_TYP_JOHN, RE15_S2_NPC_X, RE15_S2_NPC_Z, RE15_S2_GIER_NPC_WARTEN),
    OP_EM(RE15_S2_SHERRY_SLOT, RE15_S2_TYP_SHERRY, RE15_S2_SHERRY_X, RE15_S2_SHERRY_Z, RE15_S2_GIER_SHERRY_WARTEN), OP_END };
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
static int s_loch = 0;           /* 1 = Schluesselloch-Platz angelegt, 2 = Altstand: Platz startet nur die Szene */
static uint8_t s_an_alt = 0;     /* Ankunftsbits (5,0x20..0x22) im Vorbild (Messzeile im Takt) */
static uint32_t s_an_bild[3];    /* Bild des letzten Plc_dest-Starts je Bit (Messzeile) */
static uint8_t s_da = 0;         /* Begleiter/Sherry schon erschienen (Messzeile) */
int re15_story2_r42_szene_laeuft(void) { return s_szene; }

static const uint8_t *szene_starten(uint16_t room_id, const char *wie)
{
    if (bit9(RE15_R42_BIT_2080_SZENE)) return k_evt_end;                  /* Szene nur einmal */
    s_szene = 1; s_an_alt = 0; s_an_bild[0] = g_engine.frame_count; s_an_bild[1] = s_an_bild[2] = 0; s_da = 0;
    g_re15_s2_zaehler[7]++;
    S2_LOG("ROOM%04X: %s -> Szene %s", (unsigned)room_id, wie, elza() ? "E1 (John, Sherry)" : "L1 (Marvin)");
    return elza() ? k_prog_elza : k_prog_leon;
}

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
    /* Pruefung 1, Befund 5 - ALTSTAND v0.8.30: dort schloss der Schluessel das Tor 2090 <-> 20A0 direkt auf (Bit 113, Schluessel
     * verbraucht, gatorszene_r41.c tor_druck Stand Runde 41). Das Tor ist offen - ein Schluesselloch mit "The gate is locked. It
     * needs a key." waere falsch. Bit 119 wird nachgezogen (das Tor IST offen), und die Szene (noch nicht gesehen) liegt auf
     * demselben Rechteck: der Druck am Rechner startet sie ohne Schluessel-Text. */
    const int altstand = bit9(RE15_S2_TOR_BIT);
    if (altstand) {
        bit9_setzen(RE15_R42_BIT_2080_TOR);
        S2_LOG("ROOM%04X: Altstand - Tor 2090 schon offen (Bit %d = 1): kein Schluesselloch, Bit %d nachgezogen", (unsigned)room_id,
               RE15_S2_TOR_BIT, RE15_R42_BIT_2080_TOR);
        if (bit9(RE15_R42_BIT_2080_SZENE)) return;
    }
    const uint8_t *r = rdt_bytes(RE15_S2_RECHNER_OFF, k_rechner, (int)sizeof k_rechner);
    if (!r || g_aot.slots[RE15_S2_RECHNER_SLOT].active) {
        S2_LOG("ROOM%04X: Schluesselloch NICHT angelegt (%s)", (unsigned)room_id, r ? "Platz 4 belegt" : "Bytes @0x0C1A anders");
        return;
    }
    ereignis_platz(RE15_S2_RECHNER_SLOT, le_s16(r + 6), le_s16(r + 8), le_s16(r + 10), le_s16(r + 12), r[4]);
    s_loch = altstand ? 2 : 1;
    S2_LOG("ROOM%04X: %s = Platz %d (Rechteck @0x0C1A x %d..%d z %d..%d Band %d) Ereignis %d", (unsigned)room_id,
           altstand ? "Altstand: Szenen-Platz am Rechner" : "Schluesselloch neben dem Rechner",
           RE15_S2_RECHNER_SLOT, le_s16(r + 6), le_s16(r + 6) + le_s16(r + 10), le_s16(r + 8),
           le_s16(r + 8) + le_s16(r + 12), r[4], RE15_S2_EREIGNIS);
}

static const uint8_t *loch_druck(uint16_t room_id)
{
    if (s_loch == 2) {                                                   /* Altstand (Befund 5) */
        re15_aot_retype(RE15_S2_RECHNER_SLOT, 0, 0, 0, 0, 0);
        s_loch = 0;
        return szene_starten(room_id, "Altstand: Druck am Rechner (Tor schon offen)");
    }
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
        char wie[96];
        snprintf(wie, sizeof wie, "Schluessel benutzt (msg %d), Bits (9,%d)/(9,%d) = 1", RE15_R42_MSG_2080_TOR_AUF, RE15_S2_TOR_BIT,
                 RE15_R42_BIT_2080_TOR);
        return szene_starten(room_id, wie);
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
    /* Modell-Lage (Pruefung 1, Befund 6): neben dem Schluessel in DERSELBEN Einstellung sichtbar (gatorszene_r41.c
     * re15_gatorszene_r41_neben_lage: Wasser, im Bild, nicht hinter dem Schluessel, nicht von Leiche/Spielfigur verdeckt, der Bildmitte am
     * naechsten). Einmal gewaehlt, bleibt sie fuer dieselbe Schluessel-Lage (Wiederbetreten: kein Springen). Ohne gueltige Lage der
     * alte Versatz laengs x. */
    /* Neigung (Befund 6, gemessen Lauf s2b/bild_leon Bild 480: flach auf dem Wasser zeigt die Kamera die Uhr fast von der Kante): die
     * Uhr wird so gross gezeichnet wie der Schluessel (re15_story2_r42_scheitel) und uebernimmt dessen Neigung zur Kamera und dessen
     * Hoehe (gatorszene_r41.c key_neigung: Kippung zur Kamera, tiefste Ecke auf der Wasserflaeche, Quadrat 270) - dieselbe Groesse,
     * dieselbe Kamera, 450 daneben. */
    static int32_t s_uhr[3], s_uhr_key[3];
    static int16_t s_uhr_rx = 0, s_uhr_rz = 0;
    static int s_uhr_ok = 0;
    int cut = -1, sx = -1, sy = -1;
    const char *wie = "gespeichert";
    if (!(s_uhr_ok && s_uhr_key[0] == kx && s_uhr_key[1] == ky && s_uhr_key[2] == kz)) {
        if (re15_gatorszene_r41_neben_lage(RE15_S2_UHR_VERSATZ, s_uhr, &cut, &sx, &sy)) wie = "neben dem Schluessel im Bild";
        else { s_uhr[0] = kx + RE15_S2_UHR_VERSATZ; s_uhr[1] = ky; s_uhr[2] = kz; wie = "Ersatz +x"; }
        re15_gatorszene_r41_key_neigung_lesen(&s_uhr_rx, &s_uhr_rz);   /* auch wenn der Schluessel schon genommen ist */
        s_uhr[1] = ky;                                                 /* Hoehe des gekippten Schluessel-Quadrats */
        s_uhr_key[0] = kx; s_uhr_key[1] = ky; s_uhr_key[2] = kz; s_uhr_ok = 1;
    }
    const int k = prop_von_obj(RE15_S2_UHR_OBJ);
    if (k >= 0) {
        g_scd.props[k].active = 1;
        g_scd.props[k].x = s_uhr[0]; g_scd.props[k].y = s_uhr[1]; g_scd.props[k].z = s_uhr[2];
        g_scd.props[k].rot_x = s_uhr_rx; g_scd.props[k].rot_y = 0; g_scd.props[k].rot_z = s_uhr_rz;
        g_scd.props[k].parent_obj = -1;
    }
    re15_blink_r41_setzen(room_id, RE15_S2_UHR_OBJ, 1);              /* dasselbe Mittel wie der Schluessel (gatorszene key_bei_leiche) */
    s_uhr_da = 1;
    S2_LOG("ROOM%04X: Taschenuhr (Item 0x%02X, Platz %d, obj %d) neben dem Schluessel (%d,%d,%d), Modell bei (%d,%d,%d) %s"
           " (Kamera %d, Bild %d,%d), Neigung rx %d rz %d wie der Schluessel, Puls an", (unsigned)room_id, RE15_S2_UHR_ITEM,
           RE15_S2_UHR_PLATZ, RE15_S2_UHR_OBJ, (int)kx, (int)ky, (int)kz, (int)s_uhr[0], (int)s_uhr[1], (int)s_uhr[2], wie, cut, sx, sy,
           (int)s_uhr_rx, (int)s_uhr_rz);
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

/* Pruefung 1, Befund 6 ("Uhr nach echtem Tod im Bild nicht eindeutig zu sehen"): gemessen (Lauf s2b/k_elza5, gdigrab Bild 512/516,
 * Ausschnitt x4): die Uhr in Originalgroesse = ein brauner Fleck von ~3 x 1,5 Bildpunkten (320er Bild) neben dem Schluessel-Ring
 * (~12 Bildpunkte, Skala 3/2). PORT-WAHL: die Uhr wird so gross gezeichnet wie der Schluessel - ihre groesste Ausdehnung (alle
 * Netze des Modells obj 0, ROOM2090 Modelltabelle RDT+0x30) wird auf die Quadratseite des Schluessels 2 x GSZ_KEY_HALB = 180 x 3/2
 * = 270 (re15_gatorszene_r41.h RE15_GSZ_KEY_SKALA_Q12, ROOM7040 MD1 @0x354C+0x38 Ecken -90/90) skaliert, nie verkleinert. Nur
 * waehrend sie neben dem Schluessel liegt (s_uhr_da); Normalen bleiben (Licht wie bisher, Puls re15_blink_r41). */
#define S2_UHR_ZIEL_GROESSE  (2 * 90 * RE15_GSZ_KEY_SKALA_Q12 / 4096)   /* 270 */
#define S2_UHR_MAX_SCHEITEL  512
static int32_t s_uhr_skala = 0;          /* Q12; 0 = noch nicht bestimmt */
static int     s_uhr_skala_raum = -1;
static int32_t uhr_skala(void)
{
    if (s_uhr_skala_raum == (int)g_current_room_id && s_uhr_skala) return s_uhr_skala;
    s_uhr_skala_raum = (int)g_current_room_id; s_uhr_skala = 4096;
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->prop_md1[RE15_S2_UHR_OBJ] || r->prop_md1_size[RE15_S2_UHR_OBJ] <= 0) return s_uhr_skala;
    re15_md1_t m;
    if (re15_md1_parse(r->prop_md1[RE15_S2_UHR_OBJ], (size_t)r->prop_md1_size[RE15_S2_UHR_OBJ], &m) != 0) return s_uhr_skala;
    int32_t lo[3] = { 32767, 32767, 32767 }, hi[3] = { -32768, -32768, -32768 };
    for (int k = 0; k < m.mesh_count; k++)
        for (int q = 0; q < 2; q++) {
            const re15_md1_vertex_t *v = q ? m.meshes[k].quad_vertices : m.meshes[k].tri_vertices;
            const int n = q ? m.meshes[k].quad_vertex_count : m.meshes[k].tri_vertex_count;
            for (int i = 0; v && i < n; i++) {
                const int32_t c[3] = { v[i].x, v[i].y, v[i].z };
                for (int a = 0; a < 3; a++) { if (c[a] < lo[a]) lo[a] = c[a]; if (c[a] > hi[a]) hi[a] = c[a]; }
            }
        }
    int32_t aus = 0;
    for (int a = 0; a < 3; a++) if (hi[a] - lo[a] > aus) aus = hi[a] - lo[a];
    if (aus > 0 && aus < S2_UHR_ZIEL_GROESSE) s_uhr_skala = (S2_UHR_ZIEL_GROESSE * 4096) / aus;
    S2_LOG("ROOM%04X: Uhr-Modell obj %d groesste Ausdehnung %d -> Skala %d/4096 (Ziel %d = Schluessel-Quadrat)", (unsigned)g_current_room_id,
           RE15_S2_UHR_OBJ, (int)aus, (int)s_uhr_skala, S2_UHR_ZIEL_GROESSE);
    return s_uhr_skala;
}
const re15_md1_vertex_t *re15_story2_r42_scheitel(uint16_t room_id, int obj_id, const re15_md1_vertex_t *v, int n, int quad)
{
    static re15_md1_vertex_t s_buf[2][S2_UHR_MAX_SCHEITEL];
    if (!s_uhr_da || obj_id != RE15_S2_UHR_OBJ || !basis_ist(room_id, RE15_S2_RAUM_2090) || !v || n <= 0 || n > S2_UHR_MAX_SCHEITEL)
        return NULL;
    const int32_t sk = uhr_skala();
    if (sk == 4096) return NULL;
    re15_md1_vertex_t *b = s_buf[quad ? 1 : 0];
    for (int i = 0; i < n; i++) {
        b[i].x = (int16_t)(((int32_t)v[i].x * sk) >> 12);
        b[i].y = (int16_t)(((int32_t)v[i].y * sk) >> 12);
        b[i].z = (int16_t)(((int32_t)v[i].z * sk) >> 12);
        b[i].pad = v[i].pad;
    }
    return b;
}

/* ==== 11A0/11A1: Begleiter nur an EINEM Ort (Pruefung 1, Befund 8; K-07 "In 11A0 und 11A1 sind die Begleiter weiterhin nicht da") ===
 * Der Mod-Baum stellt sie dort hin: ROOM11A0 sub00 @0x0118C `44 00 40 40 ..` (Marvin), ROOM11A1 sub00 @0x0118C `44 00 49 40 ..` (John)
 * und @0x011A0 `44 01 4b 40 ..` (Sherry); kein Skript des Raums spricht sie an (scd_dump_room.py: kein Work_set kind 2). Solange sie
 * nach der Szene in 2080/2081 warten (Bit 118 = 1, Bit 120 = 0), wird der Satz verworfen wie am Kill-Flag-Tor @0x80042120-38 (Form
 * re15_gatorszene_r41_spawn_weg).
 * Stufe STAGE1 (Szene an der Leiter, story1_r42.c): K-07 "In 11A0 und 11A1 sind die Begleiter weiterhin nicht da" gilt IMMER -
 * vor der Szene in 2080 (Altstand), waehrend sie dort warten und nachdem sie an der Leiter vorausgegangen sind (Bit 120). Nur das
 * Programm der Szene selbst setzt sie (re15_story1_r42_szene_laeuft). */
#include "re15_story1_r42.h"
int re15_story2_r42_spawn_weg(uint16_t room_id, uint8_t type)
{
    if (!basis_ist(room_id, RE15_S2_RAUM_11A0)) return 0;
    if (type != RE15_S2_TYP_MARVIN && type != RE15_S2_TYP_JOHN && type != RE15_S2_TYP_SHERRY) return 0;
    if (re15_story1_r42_szene_laeuft()) return 0;
    if (bit9(RE15_R42_BIT_2080_SZENE) && !bit9(RE15_R42_BIT_LEITER))
        S2_LOG("ROOM%04X: NPC 0x%02X verworfen - wartet in 208%c (Bit %d = 1, Bit %d = 0, K-07)", (unsigned)room_id, type,
               (room_id & 1) ? '1' : '0', RE15_R42_BIT_2080_SZENE, RE15_R42_BIT_LEITER);
    else
        S2_LOG("ROOM%04X: NPC 0x%02X verworfen - Mod-Satz, Begleiter nicht in 11A%c (Bit %d = %d, Bit %d = %d, K-07)",
               (unsigned)room_id, type, (room_id & 1) ? '1' : '0', RE15_R42_BIT_2080_SZENE, bit9(RE15_R42_BIT_2080_SZENE),
               RE15_R42_BIT_LEITER, bit9(RE15_R42_BIT_LEITER));
    return 1;
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
            S2_LOG("Tuer-Ton 2030 (DOOR06 Satz 1, Door_exit @0x800141f0) - Sherry kommt in %d Bildern (Weg %d / %d je Bild)",
                   RE15_S2_SHERRY_LAUF_BILDER, RE15_S2_TREPPE_WEG, RE15_S2_SHERRY_V);
        }
        /* Messzeilen: Ankunft je Plc_dest-Bit (0 -> 1) mit Lage und Bildern seit dem letzten Abfallen (1 -> 0 = Plc_dest-Start,
         * scd_vm.c op_plc_dest loescht das Bit) bzw. seit dem Erscheinen (Sce_em_set); die Laufgeschwindigkeit folgt daraus. */
        static const char *const wer[3] = { "Spieler", "Begleiter", "Sherry" };
        for (int b = 0; b < 3; b++) {
            const int an = re15_game_flag_get(5, (uint8_t)(RE15_S2_AN_SPIELER + b)) ? 1 : 0;
            const int war = (s_an_alt >> b) & 1;
            const re15_actor_t *a = NULL;
            if (b == 0) a = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            else for (int i = 1; i < RE15_ACTOR_MAX; i++)
                if (g_actors[i].active && (b == 2 ? g_actors[i].type == RE15_S2_TYP_SHERRY
                                                  : (g_actors[i].type == RE15_S2_TYP_MARVIN || g_actors[i].type == RE15_S2_TYP_JOHN)))
                    { a = &g_actors[i]; break; }
            if (a && b > 0 && !(s_da & (1u << b))) {
                s_da |= (uint8_t)(1u << b); s_an_bild[b] = g_engine.frame_count;
                S2_LOG("ROOM%04X: %s erscheint bei (%d,%d)", (unsigned)room, wer[b], (int)a->x, (int)a->z);
            }
            if (war && !an) s_an_bild[b] = g_engine.frame_count;
            if (!war && an && a)
                S2_LOG("ROOM%04X: Ankunft %s bei (%d,%d) Gier %d nach %u Bildern", (unsigned)room, wer[b], (int)a->x, (int)a->z,
                       (int)a->rot_y, (unsigned)(g_engine.frame_count - s_an_bild[b]));
            s_an_alt = (uint8_t)((s_an_alt & ~(1u << b)) | ((unsigned)an << b));
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
