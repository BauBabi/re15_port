/*
 * RE1.5 Rebuilt — Runde 41 Spur E "kleinkram_b", Nachbesserung 2 (E-N1): im Apply-Bild eines Kamerawechsels bleibt das
 * vorige Bild stehen (Present FUN_8002137c @0x80021558 `jal 0x80021bbc` + @0x80021560 `j 0x800215fc` ueberspringt die
 * DrawOTag @0x800215bc/@0x800215d0/@0x800215e4 und den Pufferwechsel @0x800215f4/@0x800215f8). Herleitung und Messung:
 * include/re15_kamerahalt_r41.h; Dossier analysis/befunde_runde41/E_kleinkram_b.md ("Nachbesserung 2").
 */
#include "re15_kamerahalt_r41.h"

#include <stdlib.h>

#include "re15_scd.h"          /* g_scd: cam_id = DAT_800afbb5, work_vars[0x0A] = DAT_800b0fe4, cam_change_pending = DAT_800b5457 */
#include "re15_montage_fx.h"   /* re15_montage_fx_active — ROOM1240-Montage (RE2-Praesentation) */

static unsigned s_raum = 0xFFFFFFFFu;   /* Raum des zuletzt durchgereichten Spielbilds */
static unsigned s_zahl;

int re15_kamerahalt_r41_haelt(unsigned raum)
{
    const unsigned vorher = s_raum;
    s_raum = raum;
    if (raum != vorher) return 0;                     /* erstes Bild des Raums: kein Bild dieses Raums zum Halten */
    {   static int s_force = -1;
        if (s_force < 0) { const char *f = getenv("RE15_FORCE_CUT"); s_force = (f && *f) ? 1 : 0; }
        if (s_force) return 0; }                      /* Messhaken setzt jedes Bild pending (main.c) */
    if (re15_montage_fx_active()) return 0;
    /* @0x800214f4-@0x80021514 Vergleich + Dirty, @0x80021530/@0x80021538 Dirty -> Apply-Zweig @0x80021558/@0x80021560 */
    if (!g_scd.cam_change_pending && (int)g_scd.work_vars[0x0A] == (int)g_scd.cam_id) return 0;
    s_zahl++;
    return 1;
}

unsigned re15_kamerahalt_r41_zahl(void) { return s_zahl; }
