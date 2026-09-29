/*
 * fx_plattform_pc.c — Runde 34 (Granaten), Spur C (Plattform). Belege je Funktion im Header
 * fx_plattform_pc.h; Dossier analysis/befunde_runde34_granaten/bau_c.md.
 */
#include "fx_plattform_pc.h"
#include "re15_esp.h"
#include "re2_fx.h"

/* ===== C1 — ESP-Takt hinter dem Spielschritt ============================================== */

static int s_fx_takt_frei = 0;

void re15_pc_fx_takt_setzen(int frei) { s_fx_takt_frei = frei ? 1 : 0; }

int re15_pc_fx_takt(void)
{
    if (!s_fx_takt_frei) return 0;
    s_fx_takt_frei = 0;
    /* RE1.5 @0x8001ce2c `jal 0x80019e20` — hinter Gegner (@0x8001ce04) und Spieler (@0x8001ce0c).
     * Das Pausen-Selbst-Gate (RE15_PAUSE_ACTION, @0x80019e40) traegt re15_esp_fx_tick selbst. */
    re15_esp_fx_tick(re15_esp_room_bank());
    /* RE2 @0x80026980 `jal 0x8001d300` — die Pumpe laeuft hinter der Gegner-Schleife
     * (0x800267c0 .. `bne s2,v0,0x800267c0` @0x80026930); im Port direkt hinter dem RE1.5-Tick. */
    re2fx_tick();
    return 1;
}
