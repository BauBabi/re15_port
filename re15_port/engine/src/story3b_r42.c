/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE3b: 30E0 in einem Besuch (L8-L10), Szenenende nach der letzten Zeile
 * (Nutzerbefund 15.11.), Zeile in 3070 (L-11/F-08). Belege, Formen und Konstanten: include/re15_story3b_r42.h.
 *
 * PORT-STORY. Gebaut nur aus Original-Formen: Programme aus den Original-Bytes von ROOM30E0 sub02/sub03 (vor dem Einsetzen gegen
 * die RDT geprueft), Uebergang aus belegten Opcode-Formen, ausgefuehrt von der vorhandenen VM (Form story3a_r42.c). Jede neue Zeile
 * hat Text UND Dauer (re15_msg_install_text + re15_msg_install_durations): fehlt synchro/STAGE3/room30E0/mainNN.wav bzw.
 * room3070/main06.wav, laeuft sie mit der Textdauer wie jede RDT-Zeile ohne Ton (VERTRAG 2.1).
 * NUR Leon (re15_char_variant() == 0, Raum exakt 30E0/3070).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "re15_story3b_r42.h"
#include "re15_story_r42.h"
#include "re15_scd.h"
#include "re15_room.h"
#include "re15_rdt.h"
#include "re15_msg.h"
#include "re15_actor.h"
#include "re15_engine.h"
#include "re15_gameflow.h"

#include "gen/story3b_r42_texte.inc"

extern unsigned g_current_room_id;
extern int g_re15_voice_laeuft, g_re15_voice_restbilder;

#define S3B_LOG(...) do { fprintf(stderr, "[story3b] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                          fputc('\n', stderr); } while (0)

unsigned g_re15_s3b_zaehler[6];

static int leon(void) { return re15_char_variant() == 0; }

/* ==== Texte ============================================================================================================= */
const uint8_t *re15_story3b_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < S3B_TEXTE_N; i++)
        if (k_s3b_texte[i].raum == room_id && k_s3b_texte[i].id == id) {
            if (out_len) *out_len = k_s3b_texte[i].n;
            return k_s3b_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

/* Haken in re15_msg_load_room_block NACH den RDT-Texten (wie re15_story3a_r42_texte): 30E0 msg 10/11/13 liegen im RDT-Bereich. */
void re15_story3b_r42_texte(uint16_t room_id)
{
    if (!leon() || (room_id != RE15_S3B_RAUM_30E0 && room_id != RE15_S3B_RAUM_3070)) return;
    int n = 0;
    for (int i = 0; i < S3B_TEXTE_N; i++)
        if (k_s3b_texte[i].raum == room_id) {
            re15_msg_install_text(k_s3b_texte[i].id, k_s3b_texte[i].b, (size_t)k_s3b_texte[i].n);
            const int d = re15_msg_compute_duration(k_s3b_texte[i].b, k_s3b_texte[i].n, 0);   /* wie re15_msg_load_room_block */
            if (d > 0 && d < 65535) re15_msg_install_durations(k_s3b_texte[i].id, d);
            n++;
        }
    if (n) S3B_LOG("ROOM%04X: %d Nachrichten eingesetzt (nach den RDT-Texten)", (unsigned)room_id, n);
}

/* ==== Opcode-Formen (Satzbreiten = s_opcode_sizes, scd_vm.c; Formen wie story3a_r42.c) ====================================== */
#define LE16(v)            (uint8_t)((uint16_t)(v) & 0xff), (uint8_t)(((uint16_t)(v) >> 8) & 0xff)
#define OP_SET(b,i,v)      0x22, (b), (i), (v)
#define OP_CUT(c)          0x29, (c)                                          /* ROOM30E0 sub03 @0x00E7C `29 00`              */
#define OP_WORK(k,i)       0x2e, (k), (i), 0x00                               /* Work_set + Nop (sub02 @0x00CC4)              */
#define OP_SLEEP(n)        0x09, 0x0a, LE16(n)
#define OP_WARTE(bit)      0x11, 0x00, 0x08, 0x00, 0x02, 0x00, 0x12, 0x04, 0x21, 0x05, (bit), 0x00   /* ROOM5080 @0x007FC..@0x00807 */
#define OP_ZEILE(id)       OP_SET(5, RE15_S3B_SIG_FREI, 0), 0x2b, (id), 0x00, 0x00, OP_SLEEP(2), OP_WARTE(RE15_S3B_SIG_FREI)
#define OP_END             0x01, 0x00
#define OP_NECK(x,z)       0x41, 0x01, LE16(x), 0x00, 0x00, LE16(z), 0x64, 0x00   /* ROOM3091 @0x01AA6 `41 01 .. 64 00`           */
#define OP_DREH(bit,x,z)   0x40, 0x00, 0x09, (bit), LE16(x), LE16(z)            /* ROOM3091 @0x01AB0 `40 00 09 20`              */
#define OP_GEH(bit,x,z)    0x40, 0x00, 0x04, (bit), LE16(x), LE16(z)            /* ROOM1021 sub02 @0x020B2 `40 00 04 22`        */
#define OP_POS(x,y,z)      0x32, 0x00, LE16(x), LE16(y), LE16(z)                /* ROOM30E0 sub03 @0x00E9C                      */
#define OP_DIR(g)          0x33, 0x00, 0x00, 0x00, LE16(g), 0x00, 0x00          /* ROOM30E0 sub03 @0x00EA4 `33 00 00 00 3c 0f`  */
#define OP_MEMBER(m,v)     0x34, (m), LE16(v)                                   /* ROOM3070 sub02 @0x03582 `34 00 ee b7`        */
#define OP_GLAS            0x36, RE15_S3B_GLAS_BANK, RE15_S3B_GLAS_SATZ, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#define S3B_TUER           RE15_S3B_TUER_X, RE15_S3B_TUER_Z
#define OP_NECKV(...)      OP_NECK(__VA_ARGS__)   /* Lage-Makro erst nach dem Einsetzen zerlegen (wie story3a_r42.c OP_DESTV) */
#define OP_DREHV(...)      OP_DREH(__VA_ARGS__)

static const uint8_t *rdt_bytes(int off, const uint8_t *soll, int n)
{
    const re15_rdt_t *r = &g_room_rdt;
    if (!r->raw || off < 0 || off + n > r->raw_size) return NULL;
    if (soll && memcmp(r->raw + off, soll, (size_t)n) != 0) return NULL;
    return r->raw + off;
}

/* Pruefbytes: Anfang/Schluss der beiden Subs und die zwei sub11-Saetze. */
static const uint8_t k_sub02_kopf[]    = { 0x22, 0x04, 0xc2, 0x00, 0x22, 0x03, 0x4b, 0x01 };                            /* @0x00CB0 */
static const uint8_t k_sub02_schluss[] = { 0x42, 0x00, 0x3c, 0x01, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x01, 0x00 };   /* @0x00E54 */
static const uint8_t k_sub03_kopf[]    = { 0x22, 0x03, 0x4b, 0x00, 0x22, 0x03, 0xb4, 0x01 };                            /* @0x00E62 */
static const uint8_t k_sub03_ausklang[] = { 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x42, 0x00 };   /* @0x00F86 */
static const uint8_t k_sub03_schluss[] = { 0x3c, 0x01, 0x22, 0x02, 0x07, 0x00, 0x22, 0x01, 0x1b, 0x00, 0x01, 0x00 };    /* @0x00FB4 */
static const uint8_t k_marvin_satz[]   = { 0x44, 0x01, 0x40, 0x40, 0x00, 0x01, 0x00, 0xff, 0xcc, 0xb1, 0x00, 0x00, 0xd4, 0xa4,
                                           0x00, 0x00, 0x94, 0x07, 0x00, 0x00 };                                          /* @0x010A0 */
static const uint8_t k_ada_satz[]      = { 0x44, 0x00, 0x42, 0x40, 0x00, 0x01, 0x00, 0xff, 0x26, 0xa8, 0x00, 0x00, 0xf2, 0x9a,
                                           0x00, 0x00, 0x8e, 0x0d, 0x00, 0x00 };                                          /* @0x0108C */

static int pruefen(void)
{
    return rdt_bytes(RE15_S3B_SUB02_OFF, k_sub02_kopf, (int)sizeof k_sub02_kopf) &&
           rdt_bytes(RE15_S3B_SUB02_SCHLUSS, k_sub02_schluss, (int)sizeof k_sub02_schluss) &&
           rdt_bytes(RE15_S3B_SUB03_OFF, k_sub03_kopf, (int)sizeof k_sub03_kopf) &&
           rdt_bytes(RE15_S3B_SUB03_SCHLUSS, k_sub03_schluss, (int)sizeof k_sub03_schluss) &&
           rdt_bytes(RE15_S3B_SUB03_WARTEN, k_sub03_ausklang, (int)sizeof k_sub03_ausklang) &&
           rdt_bytes(RE15_S3B_MARVIN_SATZ, k_marvin_satz, (int)sizeof k_marvin_satz) &&
           rdt_bytes(RE15_S3B_ADA_SATZ, k_ada_satz, (int)sizeof k_ada_satz);
}

/* WARTEN vor dem Ausklang @0x00F86 (Nacken/Plc_ret/Sleep 0x1E/Cut_auto): so lange Zeile oder Stimme laufen (Nutzerbefund 15.11., Mechanismus re15_story3b_r42.h). */
static const uint8_t k_warten[] = { OP_SET(5, RE15_S3B_SIG_WARTEN, 1), OP_SET(5, RE15_S3B_SIG_FREI, 0), OP_SLEEP(1),
                                    OP_WARTE(RE15_S3B_SIG_FREI) };

static uint8_t s_prog[(RE15_S3B_SUB02_SCHLUSS - RE15_S3B_SUB02_OFF) + (RE15_S3B_SUB03_ENDE - RE15_S3B_SUB03_OFF) + 320];
static int     s_prog_n;

static void prog(const uint8_t *b, int n) { if (s_prog_n + n <= (int)sizeof s_prog) { memcpy(s_prog + s_prog_n, b, (size_t)n); s_prog_n += n; } }
#define PROG(...) do { static const uint8_t _b[] = { __VA_ARGS__ }; prog(_b, (int)sizeof _b); } while (0)

/* sub03 [@0x00E62, @0x00F86) + WARTEN + [@0x00F86, @0x00FC0) anhaengen */
static void sub03_mit_warten(void)
{
    const uint8_t *r = g_room_rdt.raw;
    prog(r + RE15_S3B_SUB03_OFF, RE15_S3B_SUB03_WARTEN - RE15_S3B_SUB03_OFF);
    prog(k_warten, (int)sizeof k_warten);
    prog(r + RE15_S3B_SUB03_WARTEN, RE15_S3B_SUB03_ENDE - RE15_S3B_SUB03_WARTEN);
}

static int ein_besuch_bauen(void)
{
    s_prog_n = 0;
    if (!pruefen()) return 0;
    const uint8_t *r = g_room_rdt.raw;
    /* 1. Ada: sub02 bis vor Plc_ret @0x00E54 (Nacken beider Figuren schon zurueck @0x00E38/@0x00E4A; Riegel bleiben). */
    prog(r + RE15_S3B_SUB02_OFF, RE15_S3B_SUB02_SCHLUSS - RE15_S3B_SUB02_OFF);
    /* 2. Glasbruch und Hinhoeren (Form ROOM3091 @0x01A62..@0x01AD2), Leon und Ada drehen sich zur Tuer Slot 0. */
    PROG(OP_GLAS, OP_SLEEP(0x0a),
         OP_WORK(1, 0), OP_NECKV(S3B_TUER), OP_DREHV(RE15_S3B_AN_LEON, S3B_TUER), OP_SLEEP(5),
         OP_WORK(2, 0), OP_NECKV(S3B_TUER), OP_DREHV(RE15_S3B_AN_ADA, S3B_TUER), OP_SLEEP(0x1e),
         /* 3. Leon msg 16, Ada msg 17 */
         OP_ZEILE(RE15_R42_MSG_30E0_LOBBY), OP_ZEILE(RE15_R42_MSG_30E0_ADA_KOMMT),
         /* 4. Schnitt auf Cut 0 (sub03 @0x00E7C); im selben Bild Leon (sub03 @0x00E9C/@0x00EA4) und Ada (sub11 @0x0108C) auf ihre
          *    Lagen des zweiten Besuchs, Nacken zurueck (sub02 @0x00E38 `41 00 ..`). */
         OP_CUT(0),
         OP_WORK(1, 0), 0x41, 0x00, 0, 0, 0, 0, 0, 0, 0x64, 0x00,
         OP_POS(RE15_S3B_LEON_X, 0, RE15_S3B_LEON_Z), OP_DIR(RE15_S3B_LEON_GIER),
         OP_WORK(2, 0), 0x41, 0x00, 0, 0, 0, 0, 0, 0, 0x64, 0x00,
         OP_MEMBER(0x00, RE15_S3B_ADA_X), OP_MEMBER(0x02, RE15_S3B_ADA_Z), OP_MEMBER(0x04, RE15_S3B_ADA_GIER));
    /* 5. Marvin: Satz von sub11 @0x010A0 an der Tuer, Blick in den Raum; geht auf seine sub11-Lage. */
    {
        uint8_t satz[sizeof k_marvin_satz];
        memcpy(satz, k_marvin_satz, sizeof satz);
        satz[8]  = (uint8_t)((uint16_t)RE15_S3B_TUER_X & 0xff);  satz[9]  = (uint8_t)(((uint16_t)RE15_S3B_TUER_X) >> 8);
        satz[12] = (uint8_t)((uint16_t)RE15_S3B_TUER_Z & 0xff);  satz[13] = (uint8_t)(((uint16_t)RE15_S3B_TUER_Z) >> 8);
        satz[16] = (uint8_t)(RE15_S3B_MARVIN_GIER_TUER & 0xff);  satz[17] = (uint8_t)(RE15_S3B_MARVIN_GIER_TUER >> 8);
        prog(satz, (int)sizeof satz);
    }
    PROG(OP_SLEEP(1), OP_WORK(2, 1), OP_GEH(RE15_S3B_AN_MARVIN, RE15_S3B_MARVIN_X, RE15_S3B_MARVIN_Z), OP_WARTE(RE15_S3B_AN_MARVIN));
    /* 6. Marvin-Szene sub03 (Zeilen 9, 10, 11, 12, 13) mit WARTEN vor dem Ende. */
    sub03_mit_warten();
    return s_prog_n > 0 && s_prog_n < (int)sizeof s_prog;
}

static int altstand_bauen(void)
{
    s_prog_n = 0;
    if (!pruefen()) return 0;
    sub03_mit_warten();
    return 1;
}

/* ---- 3070: Reaktionszeile (Form Stufe STAGE3a k_p_schuesse: Pad-Sperre Set(2,7) ROOM2080 sub02 @0x0CB6, kein Letterbox) -------- */
static const uint8_t k_p_3070[] = { OP_SET(2, 7, 1), OP_ZEILE(RE15_R42_MSG_3070_ADA), OP_SET(2, 7, 0), OP_END };

const uint8_t *re15_story3b_r42_programm(int welches, int *out_len)
{
    if (welches == 0 && ein_besuch_bauen()) { if (out_len) *out_len = s_prog_n; return s_prog; }
    if (welches == 1 && altstand_bauen())   { if (out_len) *out_len = s_prog_n; return s_prog; }
    if (welches == 2) { if (out_len) *out_len = (int)sizeof k_p_3070; return k_p_3070; }
    if (out_len) *out_len = 0;
    return NULL;
}

/* ==== Zustand =========================================================================================================== */
enum { SZ_KEINE = 0, SZ_30E0, SZ_30E0_ALT, SZ_3070 };
static int s_szene = SZ_KEINE;
static int s_frisch = 0;
static int s_4c2_vorher = 0;          /* (4,194) beim Betreten von 3070 */
static uint32_t s_start_bild = 0, s_warte_ab = 0;
static int s_slot = -1;               /* Ereignis-Faden der 3070-Zeile */
static unsigned s_bilder = 0, s_warte_bild = 0;   /* Takte der laufenden Szene (frame_count springt beim Raumaufbau) */
int re15_story3b_r42_szene_laeuft(void) { return s_szene; }

void re15_story3b_r42_install(uint16_t room_id)
{
    if (s_szene != SZ_KEINE && !s_frisch) s_szene = SZ_KEINE;
    s_frisch = 0;
    s_warte_ab = 0;
    if (room_id == RE15_S3B_RAUM_3070) s_4c2_vorher = re15_game_flag_get(4, 0xC2) ? 1 : 0;   /* ROOM3070 sub01 @0x03496 */
}

const uint8_t *re15_story3b_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (room_id != RE15_S3B_RAUM_30E0 || !leon()) return pc;
    if (sub == RE15_S3B_SUB_ADA) {
        if (!ein_besuch_bauen()) { g_re15_s3b_zaehler[5]++; S3B_LOG("ROOM30E0: sub02/sub03-Bytes anders - Original-sub02"); return pc; }
        s_szene = SZ_30E0; s_frisch = 1; s_start_bild = g_engine.frame_count; s_warte_ab = 0; s_bilder = 0;
        g_re15_s3b_zaehler[0]++;
        S3B_LOG("ROOM30E0: ein Besuch = sub02 + Glasbruch + Zeilen %d/%d + Marvin tritt ein + sub03 + Warten (Programm %d B)",
                RE15_R42_MSG_30E0_LOBBY, RE15_R42_MSG_30E0_ADA_KOMMT, s_prog_n);
        return s_prog;
    }
    if (sub == RE15_S3B_SUB_MARVIN) {
        if (!altstand_bauen()) { g_re15_s3b_zaehler[5]++; S3B_LOG("ROOM30E0: sub03-Bytes anders - Original-sub03"); return pc; }
        s_szene = SZ_30E0_ALT; s_frisch = 1; s_start_bild = g_engine.frame_count; s_warte_ab = 0; s_bilder = 0;
        g_re15_s3b_zaehler[1]++;
        S3B_LOG("ROOM30E0: Altstand (3,75) = 1 -> sub03 mit Warten vor dem Ausklang @0x%05X", RE15_S3B_SUB03_WARTEN);
        return s_prog;
    }
    return pc;
}

static int starten(const uint8_t *p)
{
    for (int slot = SCD_EVENT_SLOT_FIRST; slot <= SCD_EVENT_SLOT_LAST; slot++)
        if (!g_scd.threads[slot].active && scd_thread_start(slot, p) == 0) return slot;
    return -1;
}

/* MESSHAKEN RE15_S3B_BIRKIN_TOT=<n> (Durchlauf, env-gegatet, KEIN Spielverhalten): der Wegplan-Bot besiegt Birkin in 3070/3071 nicht
 * (gemessen Lauf d2: Magazin leer, kein Nachladen, Spieler gefressen). Nach <n> Kampfbildern (0x30 in Zustand 1 ausserhalb des
 * Auftritts Sub 9) wird der Tod committet wie nach einem Treffer (hp 0, Zustand 3, Phase 0 = Form birkin_formen.c
 * RE15_BIRKIN_HP_AT); Zusammenbruch, (5,0x1c) @0x8011a6b8, ROOM3070 sub01 @0x03496 und die Zeile laufen danach echt. Belebt die
 * Mutation (DEATH-Waechter 0x8011a3f0, Elzas 3071) Birkin wieder, zaehlt der Haken neu. */
static void birkin_tot_haken(void)
{
    static int n = -2, t = 0;
    static unsigned raum = 0;
    if (n == -2) { const char *v = getenv("RE15_S3B_BIRKIN_TOT"); n = v ? atoi(v) : -1; }
    if (n < 0 || (g_current_room_id != RE15_S3B_RAUM_3070 && g_current_room_id != 0x3071u)) return;
    if (raum != g_current_room_id) { raum = g_current_room_id; t = 0; }
    if (g_scd.player_mode != 0 || re15_game_flag_get(2, 7)) return;   /* erst nach der Szene (Riegel los), Spieler frei */
    for (int i = 1; i < RE15_ACTOR_MAX; i++) {
        re15_actor_t *e = &g_actors[i];
        if (!e->active || e->type != 0x30u || e->state != 1 || e->sub_state_1 == 9 || e->sub_state_1 == 10) continue;
        if (++t >= n) {   /* Kampfbilder = Bilder mit freiem Spieler und kampfbereitem Birkin; je Leben (Mutation @0x8011a3f0 belebt einmal) */
            t = 0;
            e->hp = 0; e->state = 3; e->sub_state_3 = 0;
            S3B_LOG("ROOM%04X: Messhaken RE15_S3B_BIRKIN_TOT: Tod nach %d Kampfbildern committet (Aktor %d)", g_current_room_id, n, i);
        }
        break;
    }
}

void re15_story3b_r42_tick(void)
{
    const uint16_t room = (uint16_t)g_current_room_id;
    birkin_tot_haken();
    if (room != RE15_S3B_RAUM_30E0 && room != RE15_S3B_RAUM_3070) return;
    if (s_szene != SZ_KEINE) {
        s_bilder++;
        const int zeile  = g_scd.message_active != 0;
        const int stimme = g_re15_voice_laeuft && g_re15_voice_restbilder > 0;
        if (!zeile && !stimme) re15_game_flag_set(5, RE15_S3B_SIG_FREI, 1);
        if (re15_game_flag_get(5, RE15_S3B_SIG_WARTEN)) {   /* Messung: das Programm steht am WARTEN vor @0x00F86 */
            re15_game_flag_set(5, RE15_S3B_SIG_WARTEN, 0);
            s_warte_ab = 1; s_warte_bild = s_bilder;
            g_re15_s3b_zaehler[4]++;
            S3B_LOG("ROOM30E0: Sleep-Kette nach msg 13 durch (Bild %u der Szene) - Zeile aktiv %d, Stimme %d (Rest %d Bilder)",
                    s_bilder, zeile, stimme, stimme ? g_re15_voice_restbilder : 0);
        }
        if ((s_szene == SZ_30E0 || s_szene == SZ_30E0_ALT) && !re15_game_flag_get(2, 7) && re15_game_flag_get(3, 0xB4)) {
            const unsigned gew = s_warte_ab ? s_bilder - s_warte_bild : 0u;
            g_re15_s3b_zaehler[3] += gew;
            S3B_LOG("ROOM30E0: Szene fertig nach %u Bildern (Riegel (2,7) los), davon %u Bilder auf Zeile/Stimme gewartet",
                    s_bilder, gew);
            s_szene = SZ_KEINE;
        }
        if (s_szene == SZ_3070 && (s_slot < 0 || !g_scd.threads[s_slot].active)) s_szene = SZ_KEINE;   /* Faden zu Ende (Evt_end) */
        return;
    }
    if (room != RE15_S3B_RAUM_3070 || !leon()) return;
    if (s_4c2_vorher || !re15_game_flag_get(4, 0xC2)) return;
    if (g_room_change.pending || g_scd.message_active || g_scd.player_mode != 0) return;
    s_4c2_vorher = 1;   /* einmal je Uebergang */
    if ((s_slot = starten(k_p_3070)) >= 0) {
        s_szene = SZ_3070; s_start_bild = g_engine.frame_count;
        g_re15_s3b_zaehler[2]++;
        S3B_LOG("ROOM3070: (4,194) im Besuch gesetzt (sub01 @0x03496, Birkins Tod) -> msg %d", RE15_R42_MSG_3070_ADA);
    }
}
