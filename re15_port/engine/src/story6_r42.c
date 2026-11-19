/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE6: Zug 6030 (Leon, L21 Texte) / 6031 (Elza, E6 Wagen-Szene, E9 Zugstart).
 * Belege, Formen und Konstanten: include/re15_story6_r42.h; Programme gen/story6_r42_szenen.inc (tools/r42_story/szenen_stage6.py),
 * Texte gen/story6_r42_texte.inc (tools/r42_story/texte_stage6.py). Leons Wagen-Programm steht in gen/story80_szenen.inc.
 *
 * PORT-STORY. Kein Asset-Patch: der Port tauscht nur das Ziel eines Evt_exec, wenn die ersten 16 Bytes des Original-Subs der
 * Signatur gleichen (Form story80_r41.c). Jede neue Zeile hat Text UND Dauer (re15_msg_install_text + re15_msg_install_durations):
 * fehlt synchro/STAGE6/room603x/mainNN.wav, laeuft sie mit der Textdauer wie jede RDT-Zeile ohne Ton (VERTRAG 2.1); NEUAUF-Ids
 * (6030 msg 2/3/4/5/22, 6031 msg 1/2) bleiben bis zur Neuaufnahme ebenfalls stumm: die vorhandene Datei traegt den alten Satz
 * und gilt wie keine Datei (Strang namen Stufe 6, re15_altaufnahme_r42.h; vorher spielte sie unter dem neuen Text). 6030 msg 21
 * spielt mit der vorhandenen Aufnahme: sie spricht den neuen Satz bis auf ein Wort (Stufe 7, Entscheid 19.11.2026).
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_story6_r42.h"
#include "re15_story_r42.h"
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_engine.h"
#include "re15_annette_r41.h"   /* RE15_AN_SE_* (Leons Zugstart-Toene) */
#include "re15_rumble.h"        /* Runde 42 rest (b): RE2 Vib_set1 des Fahrt-Ruckelns */

#include "gen/story6_r42_texte.inc"
#include "gen/story6_r42_szenen.inc"

extern unsigned g_current_room_id;

#define S6_LOG(...) do { fprintf(stderr, "[story6] F%u ", (unsigned)g_engine.frame_count); fprintf(stderr, __VA_ARGS__); \
                         fputc('\n', stderr); } while (0)

unsigned g_re15_s6_zaehler[RE15_S6_P_N + 2];

/* ==== Texte ============================================================================================================= */
const uint8_t *re15_story6_r42_text(uint16_t room_id, uint8_t id, int *out_len)
{
    for (int i = 0; i < S6_TEXTE_N; i++)
        if (k_s6_texte[i].raum == room_id && k_s6_texte[i].id == id) {
            if (out_len) *out_len = k_s6_texte[i].n;
            return k_s6_texte[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

/* Haken in re15_msg_load_room_block NACH den RDT-Texten und nach story_r36 (6030 msg 2..5 liegen im RDT-Bereich, 6031 msg 0 bleibt
 * die story_r36-Fassung). Raum 6030 ist nur Leons, 6031 nur Elzas RDT. */
void re15_story6_r42_texte(uint16_t room_id)
{
    if (room_id != RE15_S6_RAUM_LEON && room_id != RE15_S6_RAUM_ELZA) return;
    int n = 0;
    for (int i = 0; i < S6_TEXTE_N; i++)
        if (k_s6_texte[i].raum == room_id) {
            re15_msg_install_text(k_s6_texte[i].id, k_s6_texte[i].b, (size_t)k_s6_texte[i].n);
            const int d = re15_msg_compute_duration(k_s6_texte[i].b, k_s6_texte[i].n, 0);   /* wie re15_msg_load_room_block */
            if (d > 0 && d < 65535) re15_msg_install_durations(k_s6_texte[i].id, d);
            n++;
        }
    if (n) S6_LOG("ROOM%04X: %d Nachrichten eingesetzt (nach den RDT-Texten)", (unsigned)room_id, n);
}

/* ==== Programme ========================================================================================================== */
typedef struct {
    const uint8_t *prog; size_t prog_n;
    const uint8_t *sig;  const uint8_t *folge; int folge_n;
    uint8_t sub;
    const char *was;
} s6_prog_t;

static const s6_prog_t k_prog[RE15_S6_P_N] = {
    { k_s6_6031_wagen, sizeof k_s6_6031_wagen, k_s6_6031_wagen_sig, k_s6_6031_wagen_folge, K_S6_6031_WAGEN_FOLGE_N,
      RE15_S6_SUB_WAGEN, "Wagen-Szene E6 (msg 0, 1, 2, 12, 13)" },
    { k_s6_6031_zug, sizeof k_s6_6031_zug, k_s6_6031_zug_sig, k_s6_6031_zug_folge, K_S6_6031_ZUG_FOLGE_N,
      RE15_S6_SUB_LICHT, "Zugstart E9 statt sub05 (sub07 @0x00F54): Sherry startet, Krachen, msg 14..17" },
    { k_s6_6031_tuer, sizeof k_s6_6031_tuer, k_s6_6031_tuer_sig, k_s6_6031_tuer_folge, K_S6_6031_TUER_FOLGE_N,
      RE15_S6_SUB_LICHT, "Wagen vor dem Zugstart (sub06 @0x00EEA): Ausgang = Tuer (main00 @0x00C72), Tuer nach 5091 = msg 4" },
};

const uint8_t *re15_story6_r42_programm(int p, int *out_len)
{
    if (p < 0 || p >= RE15_S6_P_N) { if (out_len) *out_len = 0; return NULL; }
    if (out_len) *out_len = (int)k_prog[p].prog_n;
    return k_prog[p].prog;
}

const uint8_t *re15_story6_r42_folge(int p, int *out_n)
{
    if (p < 0 || p >= RE15_S6_P_N) { if (out_n) *out_n = 0; return NULL; }
    if (out_n) *out_n = k_prog[p].folge_n;
    return k_prog[p].folge;
}

static const uint8_t *umleiten(int p, const uint8_t *pc)
{
    if (memcmp(pc, k_prog[p].sig, sizeof k_s6_6031_wagen_sig) != 0) {
        S6_LOG("ROOM6031 sub%02u: Signatur weicht ab - Original-Sub", (unsigned)k_prog[p].sub);
        return pc;
    }
    g_re15_s6_zaehler[p]++;
    S6_LOG("ROOM6031 Evt_exec sub%02u -> Programm (%u B): %s", (unsigned)k_prog[p].sub, (unsigned)k_prog[p].prog_n, k_prog[p].was);
    return k_prog[p].prog;
}

/* Runde 42 rest (b): Anfang der Licht-Schleife sub05 in ROOM6031 (24 Byte @0x00E2A..@0x00E42), wie op_evt_exec sie startet.
 * Runde 42 rest Stufe 4: ebenso in ROOM6030 (24 Byte @0x01004..@0x0101C); s_licht_raum = Raum, in dem sie gestartet wurde. */
static const uint8_t *s_licht_pc;
static uint16_t       s_licht_raum;
#define S6_LICHT_LEN 24    /* ROOM6031 sub05 @0x00E2A..@0x00E42 = ROOM6030 sub05 @0x01004..@0x0101C (Set/Sleep 8/Set/Sleep 20/Goto/Evt_end) */
/* Die Licht-Schleife, bytegleich in beiden RDTs (ROOM6030 @0x01004, ROOM6031 @0x00E2A). */
static const uint8_t k_licht_sig[S6_LICHT_LEN] = {
    0x22, 0x01, 0x1c, 0x01, 0x09, 0x0a, 0x08, 0x00, 0x22, 0x01, 0x1c, 0x00,
    0x09, 0x0a, 0x14, 0x00, 0x17, 0xff, 0xff, 0x00, 0xf0, 0xff, 0x01, 0x00 };

const uint8_t *re15_story6_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    /* Runde 42 rest Stufe 4 (NUTZERWUNSCH 18.11.2026: "Ja, na klar, wenn du die ruckeln lassen kannst waehrend der Zug faehrt lass
     * sie auch ruckeln."): Leons 6030 bekommt dasselbe Ruckeln wie Elzas 6031. Der Haken merkt sich hier NUR den Anfang der
     * Licht-Schleife und gibt das Ziel unveraendert zurueck - Leons Ablauf (annette_r41.c k_zug_seg, abgenommen) bleibt bitgleich.
     * Nach dem Zugstart ((9,133) = 1, gesetzt von z_bit am Programmanfang) liefern sub07 @0x01132, sub06 @0x010C4 und z_fahrt im
     * Zugstart-Programm (Kopie von sub07 @0x01132) das Original-sub05; vorher ist das Ziel das Zugstart-Programm bzw. k_tuer. */
    if (room_id == RE15_S6_RAUM_LEON) {
        if (pc && sub == RE15_AN_ZUG_SUB && re15_game_flag_get(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT) &&
            !memcmp(pc, k_licht_sig, S6_LICHT_LEN)) { s_licht_pc = pc; s_licht_raum = RE15_S6_RAUM_LEON; }
        return pc;
    }
    if (room_id != RE15_S6_RAUM_ELZA || !pc) return pc;
    if (sub == RE15_S6_SUB_WAGEN) return umleiten(RE15_S6_P_WAGEN, pc);
    if (sub != RE15_S6_SUB_LICHT) return pc;
    /* Zugstart: erst nach dem Tor, einmalig. Danach laeuft sub05 (Licht-Schleife) wie im Original. */
    if (!re15_game_flag_get(RE15_S6_TOR_BANK, RE15_S6_TOR_BIT) || re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA)) {
        if (re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA)) { s_licht_pc = pc; s_licht_raum = RE15_S6_RAUM_ELZA; }   /* Fahrt (E9 @0x00F54 oder spaeter) */
        return pc;
    }
    /* sub00 @0x00D2A Switch(Cut) Case 0 -> sub07 (Fuehrerstand), Case 6/8/12 (@0x00D3A..@0x00D4E) -> sub06 (Wagen). */
    if (g_scd.work_vars[0x0A] != 0) return umleiten(RE15_S6_P_TUER, pc);
    return umleiten(RE15_S6_P_ZUG, pc);
}

/* ==== Runde 42 rest (b): Ruckeln der Fahrt (Belege und PORT-WAHL: include/re15_story6_r42.h "E9 Ruckeln") ================== */
/* RE2 ROOM7030 sub14 @0x0B12..@0x0BF4 (Leon PL0/RDT), ein Durchlauf = 64 Bilder. dx/dy = Sce_shake_on pc[1]/pc[2], vib = Vib_set1
 * pc[1] (0 = keiner), Bild = Takt des Opcodes nach der Sleep-Regel (@0x800539dc/@0x80053a24: Sleep n -> naechster Opcode n Takte
 * spaeter). */
typedef struct { uint8_t bild; int8_t dx, dy; uint8_t vib; } s6_ruck_t;
static const s6_ruck_t k_ruck[] = {
    {  0,  1, 1, 0x96 },   /* @0x0B12 `8b 96 03 00 00 00`, @0x0B18 `5c 01 01` */
    {  1, -1, 1, 0 },      /* @0x0B20 `5c ff 01` */
    {  2,  1, 0, 0 },      /* @0x0B28 `5c 01 00`, Sleep 5 @0x0B2C */
    {  7,  1, 0, 0x64 },   /* @0x0B30 `8b 64 03 ..`, @0x0B36 `5c 01 00` */
    {  8, -1, 1, 0 },      /* @0x0B3E */
    {  9,  1, 1, 0 },      /* @0x0B46 */
    { 10, -1, 0, 0 },      /* @0x0B4E, Sleep 10 @0x0B52 */
    { 20, -1, 1, 0x64 },   /* @0x0B56 `8b 64 ..`, @0x0B5C `5c ff 01` */
    { 21,  1, 1, 0 },      /* @0x0B64, Sleep 1 @0x0B68, Sleep 10 @0x0B6C */
    { 32,  1, 1, 0x96 },   /* @0x0B70 `8b 96 ..`, @0x0B76 */
    { 33, -1, 1, 0 },      /* @0x0B7E */
    { 34,  1, 0, 0 },      /* @0x0B86, Sleep 5 @0x0B8A */
    { 39,  1, 0, 0x64 },   /* @0x0B8E `8b 64 ..`, @0x0B94 */
    { 40, -1, 1, 0 },      /* @0x0B9C */
    { 41,  1, 1, 0 },      /* @0x0BA4 */
    { 42, -1, 0, 0 },      /* @0x0BAC, Sleep 10 @0x0BB0 */
    { 52,  1, 1, 0x64 },   /* @0x0BB4 `8b 64 ..`, @0x0BBA */
    { 53, -1, 0, 0 },      /* @0x0BC2, Sleep 1 @0x0BC6, Sleep 10 @0x0BCA -> Bild 64 */
};
#define S6_RUCK_N    ((int)(sizeof k_ruck / sizeof k_ruck[0]))
#define S6_RUCK_LANG 64    /* Bild des Ifel_ck @0x0BCE */
static int s_ruck_t = -1, s_ruck_var9, s_ruck_next, s_ruck_dx, s_ruck_dy;
unsigned g_re15_s6_ruck_bilder;   /* Messhaken: Bilder mit Verschiebung (Tests) */

static int licht_laeuft(void)
{
    if (!s_licht_pc || s_licht_raum != (uint16_t)g_current_room_id) return 0;
    for (int i = 0; i < SCD_THREAD_COUNT; i++) {
        const scd_thread_t *t = &g_scd.threads[i];
        if (t->active && t->pc >= s_licht_pc && t->pc < s_licht_pc + S6_LICHT_LEN) return 1;
    }
    return 0;
}

/* Zugstart-Bit je Raum: Elza (9,130) RE15_R42_BIT_ZUG_ELZA, Leon (9,133) RE15_AN_ZUG_BIT (Runde 42 rest Stufe 4). */
static void ruck_takt(unsigned bank, unsigned bit)
{
    s_ruck_dx = s_ruck_dy = 0;
    if (!re15_game_flag_get(bank, bit) || !licht_laeuft()) { s_ruck_t = -1; return; }
    if (s_ruck_t < 0) {
        s_ruck_t = 0; s_ruck_var9 = 0; s_ruck_next = 0;
        S6_LOG("ROOM%04X Fahrt: Ruckeln an (RE2 ROOM7030 sub14)", (unsigned)s_licht_raum);
    }
    if (s_ruck_t == S6_RUCK_LANG) {
        /* @0x0BCE Ifel_ck / Cmp(var 9 >= 2) `23 00 09 02 02 00` (RE2 Cmp @0x80054474, Op 2 = >= @0x800544e0) -> Save(9,0) @0x0BD8 +
         * Evt_next @0x0BDC (ein Takt, @0x80053860 gibt 2); danach Copy/Calc/Copy @0x0BE0..@0x0BEA = var9 + 1, Goto @0x0BEE (gibt 1). */
        if (s_ruck_var9 >= 2 && !s_ruck_next) { s_ruck_var9 = 0; s_ruck_next = 1; return; }
        s_ruck_next = 0; s_ruck_var9++; s_ruck_t = 0;
    }
    for (int i = 0; i < S6_RUCK_N; i++)
        if (k_ruck[i].bild == s_ruck_t) {
            if (k_ruck[i].vib) re15_rumble_large(3, k_ruck[i].vib, 0);   /* Vib_set1 @0x80059394: Dauer pc[2..3] = 3, Staerke pc[1], Vorlauf 0 */
            /* RE2 Sce_shake_on @0x80058164 -> FUN_8002ac94(Zeile, -pc[1], pc[2], ..): `bgtz a1` @0x8002acb8 = pc[1] < 0 kopiert nichts,
             * sonst Quelle x = pc[1] (@0x8002acc0), Zeile + pc[2] (@0x8002ad10) -> Bild um pc[1] nach links, pc[2] nach oben. */
            if (k_ruck[i].dx >= 0 && (k_ruck[i].dx | k_ruck[i].dy)) { s_ruck_dx = k_ruck[i].dx; s_ruck_dy = k_ruck[i].dy; g_re15_s6_ruck_bilder++; }
        }
    s_ruck_t++;
}

int re15_story6_r42_ruck(int *dx, int *dy)
{
    const uint16_t raum = (uint16_t)g_current_room_id;
    const int an = (raum == RE15_S6_RAUM_ELZA || raum == RE15_S6_RAUM_LEON) && (s_ruck_dx || s_ruck_dy);
    if (dx) *dx = an ? s_ruck_dx : 0;
    if (dy) *dy = an ? s_ruck_dy : 0;
    return an;
}

/* ==== Takt: Toene der Signale (Form annette_r41.c zug_tick) ================================================================ */
void re15_story6_r42_tick(void)
{
    const uint16_t raum = (uint16_t)g_current_room_id;
    /* Runde 42 rest Stufe 4: in Leons 6030 nur das Ruckeln - seine Toene laufen weiter in annette_r41.c zug_tick. */
    if (raum == RE15_S6_RAUM_LEON) { ruck_takt(RE15_AN_ZUG_BANK, RE15_AN_ZUG_BIT); return; }
    if (raum != RE15_S6_RAUM_ELZA) { s_licht_pc = NULL; s_ruck_t = -1; s_ruck_dx = s_ruck_dy = 0; return; }
    ruck_takt(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA);
    if (re15_game_flag_get(5, RE15_S6_SIG_ZUG_AN)) {
        re15_game_flag_set(5, RE15_S6_SIG_ZUG_AN, 0);
        g_re15_s6_zaehler[RE15_S6_P_N]++;
        S6_LOG("ROOM6031 Zug-an-Ton (ANNETTE_SE Satz 3)");
        re15_annette_r41_ton_spielen(RE15_AN_SE_ZUG_AN);
    }
    if (re15_game_flag_get(5, RE15_S6_SIG_KRACH)) {
        re15_game_flag_set(5, RE15_S6_SIG_KRACH, 0);
        g_re15_s6_zaehler[RE15_S6_P_N + 1]++;
        S6_LOG("ROOM6031 Krachen aus dem hinteren Wagen (ANNETTE_SE Satz 0)");
        re15_annette_r41_ton_spielen(RE15_AN_SE_KRACH);
    }
}
