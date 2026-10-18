/* zustand_r39.c — Runde 39 Spur F "zustand" (Dossier analysis/befunde_runde39/F_zustand.md).
 *
 * Stand 1 (Messschiene): env-gegatete Messhaken, KEIN Spielverhalten.
 *   RE15_R39_GIFT_AT=<bild>   setzt im genannten Spielbild das Gift-Bit 0x2 (0x800acaec, status_flags)
 *   RE15_R39_LOG=<datei>      je Spielbild: F<n> hp status zaehler(RE1.5 +0x1d3) cond(FUN_8004ed6c) menu
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_zustand_r39.h"
#include "re15_engine.h"
#include "re15_inv_screen.h"
#include "re15_menu.h"
#include "re15_spinnen_r38.h"

static uint8_t  s_takt;                /* RE2 0x800cfe12 (Stand 2) */
static unsigned s_takte;
static int      s_env_init;
static long     s_gift_at = -1;
static FILE    *s_log;

static void env_init(void)
{
    if (s_env_init) return;
    s_env_init = 1;
    const char *g = getenv("RE15_R39_GIFT_AT");
    if (g && *g) s_gift_at = atol(g);
    const char *l = getenv("RE15_R39_LOG");
    if (l && *l) s_log = fopen(l, "w");
}

void re15_zustand_r39_gift_setzen(void) { s_takt = 30; }
void re15_zustand_r39_reset(void) { s_takt = 0; s_takte = 0; }
uint8_t  re15_zustand_r39_gift_zaehler(void) { return s_takt; }
unsigned re15_zustand_r39_gift_takte(void)   { return s_takte; }

int re15_zustand_r39_spieler_takt(re15_actor_t *pl, int aktiv)
{
    if (!pl) return 0;
    env_init();
    if (aktiv && s_gift_at >= 0 && (long)g_engine.frame_count == s_gift_at) {
        pl->status_flags |= 0x2u;
        fprintf(stderr, "[r39-zustand] F%u Messhaken: Gift gesetzt (status 0x%04X)\n",
                (unsigned)g_engine.frame_count, (unsigned)pl->status_flags);
    }
    int abzug = 0;
    if (s_log) {
        fprintf(s_log, "F%u aktiv=%d hp=%d status=0x%04X z15=%u z2=%u cond=%d menu=%d abzug=%d\n",
                (unsigned)g_engine.frame_count, aktiv, (int)pl->hp, (unsigned)pl->status_flags,
                (unsigned)re15_spinnen_r38_gift_zaehler(), (unsigned)s_takt,
                re15_inv_screen_condition(pl->hp, (pl->status_flags & 2u) ? 1 : 0),
                re15_menu_is_open(), abzug);
        fflush(s_log);
    }
    return abzug;
}
