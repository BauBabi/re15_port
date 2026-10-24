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
#include "re15_engine.h"       /* g_engine.frame_count — nur fuer die Messschiene RE15_KAMERAHALT_LOG */
#include <stdio.h>

static unsigned s_raum = 0xFFFFFFFFu;   /* Raum des zuletzt durchgereichten Spielbilds */
static unsigned s_zahl;
static int      s_letzt;                /* Nachbesserung 4: letzte Entscheidung (1 = Apply-Bild) */

/* Nachbesserung 4 (Nachtrag 14): DAUER des Apply-Durchlaufs in Port-Bildern zu je 2 VSyncs, ueber das gehaltene Bild hinaus.
 * GEMESSEN (DuckStation-GDB `<sp>/E41n4/orig/o_kurbel_cut3.txt`, Weg des Nutzers: Cut 0 -> 2 -> 3 gelaufen, Kurbel, Ja):
 * Durchlauf n104 -> n105 = 14 VSyncs (sonst 2) = 2 + Lesen 8 (FUN_80021bbc @0x80021d2c `jal 0x80013c50`, a1 = 0
 * @0x80021d30 = blockierend; vs10460 -> vs10468) + MDEC 4 (@0x80021e34 `jal 0x80053a8c`; -> vs10472). 14 VSyncs = 7 Port-
 * Bilder: das gehaltene Bild + 6. Gilt fuer jedes Apply mit Lesen (RVD-Wechsel 2020 gemessen 13/12/15); die Laenge haengt im
 * Original am Kopfweg der CD (nach einem PLW-Laden 34) — der Port nimmt den gemessenen Wert dieser Stelle. */
#define KH_STANDBILDER 6

static int entscheiden(unsigned raum, unsigned vorher)
{
    if (raum != vorher) return 0;                     /* erstes Bild des Raums: kein Bild dieses Raums zum Halten */
    {   static int s_force = -1;
        if (s_force < 0) { const char *f = getenv("RE15_FORCE_CUT"); s_force = (f && *f) ? 1 : 0; }
        if (s_force) return 0; }                      /* Messhaken setzt jedes Bild pending (main.c) */
    if (re15_montage_fx_active()) return 0;
    /* @0x800214f4-@0x80021514 Vergleich + Dirty, @0x80021530/@0x80021538 Dirty -> Apply-Zweig @0x80021558/@0x80021560 */
    if (!g_scd.cam_change_pending && (int)g_scd.work_vars[0x0A] == (int)g_scd.cam_id) return 0;
    return 1;
}

int re15_kamerahalt_r41_haelt(unsigned raum)
{
    const unsigned vorher = s_raum;
    s_raum = raum;
    const int h = entscheiden(raum, vorher);
    if (h) s_zahl++;
    s_letzt = h;
    {   static int s_log = -1;   /* Messschiene RE15_KAMERAHALT_LOG (kein Verhalten) */
        if (s_log < 0) { const char *e = getenv("RE15_KAMERAHALT_LOG"); s_log = (e && *e) ? 1 : 0; }
        if (s_log) fprintf(stderr, "[kamerahalt] F%u raum=%04x vorher=%04x pend=%d wv0A=%d cam=%d -> %s\n",
                           (unsigned)g_engine.frame_count, raum & 0xFFFFu, vorher & 0xFFFFu, (int)g_scd.cam_change_pending,
                           (int)g_scd.work_vars[0x0A], (int)g_scd.cam_id, h ? "HALTEN" : "zeigen"); }
    return h;
}

unsigned re15_kamerahalt_r41_zahl(void) { return s_zahl; }

int re15_kamerahalt_r41_standbilder(void) { return s_letzt ? KH_STANDBILDER : 0; }
