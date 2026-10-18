/*
 * RE1.5 Rebuilt — Runde 39 Spur I "funk10f1": die Funkszene in ROOM10F1 (Elza am Kommunikationssystem).
 *
 * Herleitung, Belege und alle Konstanten: include/re15_funk10f1_r39.h und analysis/befunde_runde39/I_funk10f1.md.
 * Kein Asset-Patch: die RDT bleibt byte-true, der Port spielt die Szene als Bytecode aus ORIGINAL-Opcodes ueber die
 * vorhandene VM (gen/funk10f1_r39_szene.inc, Generator tools/r39_funk10f1/szene_bauen.py — dort das Vorbild jeder
 * Opcode-Form mit Datei-Offset; Form = Elzas Szene ROOM1031 sub13 @0x02982..@0x02A58). Muster: cut_10f0.c (Runde 35 K).
 */
#include "re15_funk10f1_r39.h"

#include <stddef.h>

#include "re15_msg.h"
#include "re15_scd.h"
#include "re15_engine.h"        /* g_engine.frame_count (Protokoll) */
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

#include "gen/funk10f1_r39_szene.inc"   /* k_funk10f1_prog + k_funk10f1_msg40/41/42 (tools/r39_funk10f1/szene_bauen.py) */

static const struct { uint8_t id; const uint8_t *b; uint8_t n; } k_meldungen[] = {
    { RE15_FUNK10F1_MSG_HALLO,  k_funk10f1_msg40, (uint8_t)sizeof k_funk10f1_msg40 },
    { RE15_FUNK10F1_MSG_PUNKTE, k_funk10f1_msg41, (uint8_t)sizeof k_funk10f1_msg41 },
    { RE15_FUNK10F1_MSG_KAPUTT, k_funk10f1_msg42, (uint8_t)sizeof k_funk10f1_msg42 },
};
#define FUNK_N_MELDUNGEN ((int)(sizeof k_meldungen / sizeof k_meldungen[0]))

static int gesehen(void)
{
    return re15_game_flag_get(RE15_FUNK10F1_GESEHEN_BANK, RE15_FUNK10F1_GESEHEN_BIT) != 0;
}

/* Laeuft das Programm in einem Faden? (Muster cut_10f0.c programm_laeuft: genau EIN Faden.) */
int re15_funk10f1_r39_programm_laeuft(void)
{
    for (int s = 0; s < SCD_THREAD_COUNT; s++) {
        const scd_thread_t *t = &g_scd.threads[s];
        if (t->active && t->pc >= k_funk10f1_prog && t->pc < k_funk10f1_prog + RE15_FUNK10F1_PROG_LEN)
            return 1;
    }
    return 0;
}

const uint8_t *re15_funk10f1_r39_programm(int *out_len)
{
    if (out_len) *out_len = RE15_FUNK10F1_PROG_LEN;
    return k_funk10f1_prog;
}

const uint8_t *re15_funk10f1_r39_meldung(int msg_id, int *out_len)
{
    for (int i = 0; i < FUNK_N_MELDUNGEN; i++)
        if ((int)k_meldungen[i].id == msg_id) {
            if (out_len) *out_len = (int)k_meldungen[i].n;
            return k_meldungen[i].b;
        }
    if (out_len) *out_len = 0;
    return NULL;
}

/* Nachrichten 40..42 in die Port-Tabelle des Raums (re15_msg_install_text, Standzeit aus dem Endcode `04 01 01 63`
 * wie bei jeder Port-Zeile, cut_10f0.c meldungen_einsetzen). */
static void meldungen_einsetzen(void)
{
    for (int i = 0; i < FUNK_N_MELDUNGEN; i++) {
        re15_msg_install_text(k_meldungen[i].id, k_meldungen[i].b, k_meldungen[i].n);
        int d = re15_msg_compute_duration(k_meldungen[i].b, k_meldungen[i].n, 0);
        if (d > 0 && d < 65535) re15_msg_install_durations(k_meldungen[i].id, d);
    }
}

void re15_funk10f1_r39_install(uint16_t room_id)
{
    if (room_id != RE15_FUNK10F1_RAUM || gesehen()) return;
    meldungen_einsetzen();
    /* Start wie ein Evt_exec aus sub00 beim Raumstart (ROOM1050 sub00 @0x00C8A; Port-Muster re15_cut10f0_install):
     * der Faden laeuft ab dem ersten Spielbild, sein erstes Opcode setzt (9,70). */
    const int slot = scd_event_fire(RE15_FUNK10F1_EREIGNIS);
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[funk10f1] F%u ROOM%04X: Funkszene %s (Ereignis %d, Faden %d, Flag (%d,%d)=0)\n",
            (unsigned)g_engine.frame_count, (unsigned)room_id,
            slot >= 0 ? "gestartet" : "NICHT gestartet (kein freier Faden)", RE15_FUNK10F1_EREIGNIS, slot,
            RE15_FUNK10F1_GESEHEN_BANK, RE15_FUNK10F1_GESEHEN_BIT);
#else
    (void)slot;
#endif
}

const uint8_t *re15_funk10f1_r39_ereignis(uint16_t room_id, uint8_t event_id)
{
    if (room_id != RE15_FUNK10F1_RAUM || event_id != RE15_FUNK10F1_EREIGNIS) return NULL;
    if (gesehen() || re15_funk10f1_r39_programm_laeuft()) return NULL;
    return k_funk10f1_prog;
}

unsigned re15_funk10f1_r39_rbj_quelle(unsigned room_id)
{
    if (room_id != RE15_FUNK10F1_RAUM || gesehen()) return 0;
    return RE15_FUNK10F1_RBJ_RAUM;
}
