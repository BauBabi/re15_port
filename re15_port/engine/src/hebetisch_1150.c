/*
 * RE1.5 Rebuilt — RUHE OBEN des Hebetischs (ROOM1150/1151 sub04). Herleitung, Skript-Records und
 * Belege: include/re15_hebetisch.h. Dossier analysis/befunde_runde31/hebetisch.md §2.
 */
#include "re15_hebetisch.h"

#include <string.h>
#include "re15_scd.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#include <stdlib.h>
#include "re15_engine.h"      /* g_engine.frame_count — nur fuer das Mess-Protokoll */
#include "re15_item_modal.h"  /* Zustand der Aufnahme-FSM — nur fuer das Mess-Protokoll */
#endif

/* ROOM1150.RDT @0x1010..@0x1047 (ROOM1151.RDT @0x0FEE..): For-Setzen, Sleep 30, Work_set, Se_on,
 * Sleep 10, Se_on, Speed_set, For-Abfahrt — Byte fuer Byte aus der Datei (Kopf re15_hebetisch.h). */
static const uint8_t s_sig[56] = {
    0x0d,0x00,0x04,0x00,0x0a,0x00,                         /* @0x1010 For 10            */
    0x30, 0x02, 0x0e,0x00,                                  /* @0x1016 Add_speed, Evt_next, Next */
    0x09,0x0a,0x1e,0x00,                                    /* @0x101A Sleep 30          */
    0x2e,0x03,0x00, 0x00,                                   /* @0x101E Work_set 0        */
    0x36,0x02,0x0a,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* @0x1022 Se_on    */
    0x09,0x0a,0x0a,0x00,                                    /* @0x102E Sleep 10          */
    0x36,0x02,0x0c,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00, /* @0x1032 Se_on    */
    0x2f,0x01,0x0a,0x00,                                    /* @0x103E Speed_set vy +10  */
    0x0d,0x00,0x04,0x00,0x5a,0x00                           /* @0x1042 For 90 (Abfahrt)  */
};
#define RUHE_VON  0x0A   /* Signatur + 0x0A = Sleep 30 @0x101A */
#define RUHE_BIS  0x32   /* Signatur + 0x32 = For-Abfahrt @0x1042 (ausschliesslich) */

static const uint8_t *s_raw;
static const uint8_t *s_von;
static const uint8_t *s_bis;

void re15_hebetisch_raum_scan(const uint8_t *raw, int raw_size)
{
    s_raw = raw;
    s_von = s_bis = NULL;
    if (raw == NULL || raw_size < (int)sizeof s_sig) return;
    for (int i = 0; i + (int)sizeof s_sig <= raw_size; i++) {
        if (raw[i] != s_sig[0] || memcmp(raw + i, s_sig, sizeof s_sig) != 0) continue;
        s_von = raw + i + RUHE_VON;
        s_bis = raw + i + RUHE_BIS;
        return;                      /* je Raum genau ein Treffer (Zensus, Kopf) */
    }
}

static const uint8_t *ruhe_pc(void)
{
    if (!s_von) return NULL;
    for (int t = 0; t < SCD_THREAD_COUNT; t++) {
        const scd_thread_t *th = &g_scd.threads[t];
        if (!th->active || th->kill_pending || !th->pc) continue;
        if (th->pc >= s_von && th->pc < s_bis) return th->pc;
    }
    return NULL;
}

int re15_hebetisch_ruht_oben(void) { return ruhe_pc() != NULL; }

long re15_hebetisch_ruhe_pc_off(void)
{
    const uint8_t *pc = ruhe_pc();
    return pc ? (long)(pc - s_raw) : -1;
}

long re15_hebetisch_fenster_von(void) { return s_von ? (long)(s_von - s_raw) : -1; }
long re15_hebetisch_fenster_bis(void) { return s_bis ? (long)(s_bis - s_raw) : -1; }

/* MESS-PROTOKOLL (nur PC, nur mit RE15_HEBETISCH_LOG=<datei>): je Spielbild eine Zeile, solange
 * der Raum das Ruhe-Fenster traegt — Plattform-y, Ruhe ja/nein, sub04-PC im Fenster, Zustand der
 * Aufnahme-FSM. Aendert kein Verhalten. Gerufen in game_step_common.c NACH dem SCD-Tick des Bilds
 * (main.c) und VOR re15_sicherung_tick. */
void re15_hebetisch_protokoll(void)
{
#ifdef RE15_PLATFORM_PC
    static int an = -1;
    static FILE *f;
    if (an < 0) {
        const char *e = getenv("RE15_HEBETISCH_LOG");
        an = (e && *e) ? 1 : 0;
        if (an) { f = fopen(e, "w"); if (!f) an = 0; }
    }
    if (!an || !s_von) return;
    int y = 0, da = 0;
    for (int k = 0; k < (int)g_scd.prop_count; k++)
        if (g_scd.props[k].obj_id == 0) { y = (int)g_scd.props[k].y; da = 1; break; }
    if (!da) return;
    fprintf(f, "F%u y=%d ruht=%d pc=%ld modal=%u\n", (unsigned)g_engine.frame_count, y,
            re15_hebetisch_ruht_oben(), re15_hebetisch_ruhe_pc_off(),
            (unsigned)re15_item_modal_state());
    fflush(f);
#endif
}
