/* granate_r36.c — Runde 36 Spur D: Handgranate. Belege und Einordnung: include/re15_granate_r36.h,
 * Dossier analysis/befunde_runde36/D_granate.md. */
#include "re15_granate_r36.h"

static unsigned s_russ_unterdrueckt = 0;

/* Punkt 1 — die Flammen des Spreng-Russ FUN_8010640C (@0x80106418-44 Part 0 / @0x80106450-74 Part 3).
 * RE2 Retail spawnt sie fuer die Zeilen 9 (HE-Granate 0x09, GL-Explosivrunde) und 17 (Rakete) — beide
 * sind Spreng-, keine Brandwaffen. NUTZER-VORGABE (AUFTRAG.md Runde 36, Punkt 1): kein Feuer-Effekt
 * bei normalen Granaten, Feuer nur bei der Brandgranate (deren Verkohlung FUN_80106128 laeuft ueber
 * Zeile 10/16 und ruft diese Funktion nie). Deshalb: fuer JEDE Zeile, die den Spreng-Russ erreicht,
 * keine Flammen. Die Russ-Tinten (@0x80106478-f8) setzt der Aufrufer weiterhin. */
int re15_granate_r36_russ_flammen(unsigned zeile)
{
    (void)zeile;                       /* 9 oder 17 (`beq v1,9` @0x80108cc8 / `bne v1,17` @0x80108cd0) */
    s_russ_unterdrueckt++;
    return 0;
}

unsigned re15_granate_r36_russ_unterdrueckt(void) { return s_russ_unterdrueckt; }
void     re15_granate_r36_zaehler_reset(void)     { s_russ_unterdrueckt = 0; }
