/*
 * RE1.5 Rebuilt — Runde 42 "story" (v0.8.31), Stufe STAGE6: Zug 6030 (Leon, L21 Texte) / 6031 (Elza, E6 Wagen-Szene, E9 Zugstart).
 * Belege, Formen und Konstanten: include/re15_story6_r42.h; Programme gen/story6_r42_szenen.inc (tools/r42_story/szenen_stage6.py),
 * Texte gen/story6_r42_texte.inc (tools/r42_story/texte_stage6.py). Leons Wagen-Programm steht in gen/story80_szenen.inc.
 *
 * PORT-STORY. Kein Asset-Patch: der Port tauscht nur das Ziel eines Evt_exec, wenn die ersten 16 Bytes des Original-Subs der
 * Signatur gleichen (Form story80_r41.c). Jede neue Zeile hat Text UND Dauer (re15_msg_install_text + re15_msg_install_durations):
 * fehlt synchro/STAGE6/room603x/mainNN.wav, laeuft sie mit der Textdauer wie jede RDT-Zeile ohne Ton (VERTRAG 2.1); NEUAUF-Ids
 * spielen bis zur Neuaufnahme die vorhandene Aufnahme (AUFNAHMELISTE).
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

const uint8_t *re15_story6_r42_evt_exec(uint16_t room_id, uint8_t sub, const uint8_t *pc)
{
    if (room_id != RE15_S6_RAUM_ELZA || !pc) return pc;
    if (sub == RE15_S6_SUB_WAGEN) return umleiten(RE15_S6_P_WAGEN, pc);
    if (sub != RE15_S6_SUB_LICHT) return pc;
    /* Zugstart: erst nach dem Tor, einmalig. Danach laeuft sub05 (Licht-Schleife) wie im Original. */
    if (!re15_game_flag_get(RE15_S6_TOR_BANK, RE15_S6_TOR_BIT) || re15_game_flag_get(RE15_R42_BANK, RE15_R42_BIT_ZUG_ELZA)) return pc;
    /* sub00 @0x00D2A Switch(Cut) Case 0 -> sub07 (Fuehrerstand), Case 6/8/12 (@0x00D3A..@0x00D4E) -> sub06 (Wagen). */
    if (g_scd.work_vars[0x0A] != 0) return umleiten(RE15_S6_P_TUER, pc);
    return umleiten(RE15_S6_P_ZUG, pc);
}

/* ==== Takt: Toene der Signale (Form annette_r41.c zug_tick) ================================================================ */
void re15_story6_r42_tick(void)
{
    if ((uint16_t)g_current_room_id != RE15_S6_RAUM_ELZA) return;
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
