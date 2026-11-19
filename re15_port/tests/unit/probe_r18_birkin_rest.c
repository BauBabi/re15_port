/* ============================================================================================
 * probe_r18_birkin_rest.c — Runde 18 (2026-09-21): die drei Restluecken des G5-Endkampfs
 * Dossier: analysis/befunde_2026-09-21/birkin-restluecken.md
 *
 *   A) RUMBLE-RINGE   — byte-treuer Zwilling von FUN_800396FC @0x800396fc + der drei
 *                       Alloc-Funktionen @0x8003947c / @0x80039514 / @0x800395b8.
 *                       Gepinnt wird die EXAKTE Aktuator-Kurve, Bild fuer Bild.
 *   B) BLUT-SPAWNS    — die vier FUN_8001BF10-Aufrufe der Trefferroutine
 *                       (@0x801027ec/@0x80102848/@0x801028f0/@0x80102928).
 *   C) OPFERMASCHINE  — Spieler-Routine 6 (0x80103908): Clip 0, Blend-Zaehler 7,
 *                       Drehen mit 256/Bild auf Boss-Yaw+2048, Schnapp bei Bild 8.
 *
 * Aufruf:  probe_r18_birkin_rest [pin]
 * Rueckgabe 77 = SKIP (Assets fehlen), 0 = alle Pins gehalten, 1 = Pin gerissen.
 * ========================================================================================== */
/* Runde 42 Strang finale (Plan S10): der G5-Endkampf ist entfernt; die Teile B (Blut-Spawns der G5-Trefferroutine) und C
 * (Devour-Opfermaschine) pruefen nur ihn und sind mit ihm entfallen. Teil A (Rumble-Ringe @0x800396fc, byte-treu) bleibt: das
 * Finale ruettelt mit genau diesen Ringen (FUN_8003947c(6,3) / FUN_800395b8(60,250,0,0) @0x80103ab0-cc). */
#include "re15_rumble.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int                s_fail = 0;

#define PIN(cond, ...) do { if (!(cond)) { s_fail++; printf("  PIN FEHLT: " __VA_ARGS__); printf("\n"); } \
                            else { printf("  PIN ok: " __VA_ARGS__); printf("\n"); } } while (0)

static void teil_a_rumble(void)
{
    uint8_t a0, a1;
    int f;

    printf("-- A) Rumble-Ringe (FUN_800396FC @0x800396fc) --\n");

    /* A1: kleiner Motor, 3 Bilder, kein Vorlauf (FUN_8003947C @0x8003947c). Slot[1] = 1,
     *     Schritt 0 -> der Tick ueberschreibt nie -> [1,1,1,0]. */
    re15_rumble_reset();
    re15_rumble_small(3, 0);
    {
        int ok = 1;
        for (f = 1; f <= 4; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0 != (f <= 3 ? 1 : 0)) ok = 0;
            if (a1 != 0) ok = 0;
        }
        PIN(ok, "small(3,0) liefert Aktuator0 = 1,1,1,0 (`sb v0,1(v1)` @0x800394cc,"
                " Schritt 0 @0x800394e4)");
    }

    /* A2: Vorlauf. small(1,120) -> erst Bild 121 zieht an (`lhu v0,1(a1)` @0x8003971c,
     *     `j 0x80039794` @0x8003972c ueberspringt das Maximum). */
    re15_rumble_reset();
    re15_rumble_small(1, 120);
    {
        int an = -1;
        for (f = 1; f <= 130; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0 && an < 0) an = f;
        }
        PIN(an == 121, "small(1,120) zieht erst auf Bild 121 an (gemessen %d)", an);
    }

    /* A3: grosser Motor konstant (FUN_80039514 @0x80039514). */
    re15_rumble_reset();
    re15_rumble_large(5, 200, 0);
    {
        int ok = 1;
        for (f = 1; f <= 6; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a1 != (f <= 5 ? 200 : 0)) ok = 0;
        }
        PIN(ok, "large(5,200,0) haelt Aktuator1 = 200 fuer genau 5 Bilder");
    }

    /* A4: RAMPE (FUN_800395B8 @0x800395b8). Schritt = ((0-180)*0x80)/150 = -153,
     *     Akku startet 180<<7 = 23040. Erstes nicht-verzoegertes Bild ist 251:
     *     (23040-153)>>7 = 178. Letztes (Bild 400): (23040-150*153)>>7 = 90>>7 = 0. */
    re15_rumble_reset();
    re15_rumble_ramp(150, 180, 0, 250);
    {
        uint8_t bei250 = 0xFF, bei251 = 0xFF, bei400 = 0xFF;
        for (f = 1; f <= 401; f++) {
            re15_rumble_tick(&a0, &a1);
            if (f == 250) bei250 = a1;
            if (f == 251) bei251 = a1;
            if (f == 400) bei400 = a1;
        }
        PIN(bei250 == 0,   "ramp(150,180,0,250): Bild 250 noch still (%u)", bei250);
        PIN(bei251 == 178, "ramp(150,180,0,250): Bild 251 = 178 = (23040-153)>>7 (%u)", bei251);
        PIN(bei400 == 0,   "ramp(150,180,0,250): Bild 400 = 0, die Rampe laeuft aus (%u)", bei400);
    }

    /* A5: KASKADE 1 (Todes-Uebergang 0->1, @0x80103104-@0x801031b8) — 10 Slots in Ring B,
     *     Bild 1 = der Grundton 180, Bild 21 = erster Puls-Schritt
     *     ((180<<7)+512)>>7 = 184, und das Maximum gewinnt (`sltu v0,a3,v1` @0x80039784). */
    re15_rumble_reset();
    re15_rumble_large(250, 180, 0);
    re15_rumble_ramp(150, 180, 0, 250);
    re15_rumble_ramp(10, 180, 220, 20);
    re15_rumble_ramp(10, 220, 180, 30);
    re15_rumble_ramp(10, 180, 220, 40);
    re15_rumble_ramp(10, 220, 180, 50);
    re15_rumble_ramp(10, 180, 220, 65);
    re15_rumble_ramp(10, 220, 180, 75);
    re15_rumble_ramp(10, 180, 220, 230);
    re15_rumble_ramp(10, 220, 180, 240);
    PIN(re15_rumble_belegt(1) == 10, "Kaskade 1 belegt 10 Slots in Ring B (%d)",
        re15_rumble_belegt(1));
    PIN(re15_rumble_belegt(0) == 0,  "Kaskade 1 ruehrt Ring A nicht an (%d)",
        re15_rumble_belegt(0));
    {
        uint8_t b1 = 0, b21 = 0, b405 = 0xFF;
        for (f = 1; f <= 405; f++) {
            re15_rumble_tick(&a0, &a1);
            if (f == 1)  b1  = a1;
            if (f == 21) b21 = a1;
            if (f == 405) b405 = a1;
        }
        PIN(b1 == 180,  "Kaskade 1 Bild 1   = 180 (Grundton)                      (%u)", b1);
        PIN(b21 == 184, "Kaskade 1 Bild 21  = 184 (erster Puls ueberlagert)       (%u)", b21);
        PIN(b405 == 0,  "Kaskade 1 Bild 405 = 0   (alles ausgelaufen)             (%u)", b405);
    }

    /* A6: KASKADE 2 (Todes-Uebergang 2->3, @0x801034c4-@0x80103568) — 12 Stoesse im
     *     kleinen Motor, Ring A. Die Stakkato-Bilder sind 1..3, 6..8, 16..17, 61..63,
     *     66..68, 76..77, 91..93, 96..98, 106..107, 111..112, 116..117, 121. */
    re15_rumble_reset();
    re15_rumble_large(5, 200, 0);
    re15_rumble_ramp(200, 200, 0, 5);
    re15_rumble_small(3, 0);   re15_rumble_small(3, 5);   re15_rumble_small(2, 15);
    re15_rumble_small(3, 60);  re15_rumble_small(3, 65);  re15_rumble_small(2, 75);
    re15_rumble_small(3, 90);  re15_rumble_small(3, 95);  re15_rumble_small(2, 105);
    re15_rumble_small(2, 110); re15_rumble_small(2, 115); re15_rumble_small(1, 120);
    PIN(re15_rumble_belegt(0) == 12, "Kaskade 2 belegt 12 Slots in Ring A (%d)",
        re15_rumble_belegt(0));
    {
        int an_bilder = 0, ok_start = 0, ok_ende = 0;
        for (f = 1; f <= 210; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0) an_bilder++;
            if (f == 1   && a0 == 1 && a1 == 200) ok_start = 1;
            if (f == 121 && a0 == 1)              ok_ende  = 1;
        }
        /* 3+3+2+3+3+2+3+3+2+2+2+1 = 29 Bilder mit aktivem kleinen Motor. */
        PIN(an_bilder == 29, "Kaskade 2: kleiner Motor zieht auf genau 29 Bildern an (%d)",
            an_bilder);
        PIN(ok_start, "Kaskade 2 Bild 1: klein=1 UND gross=200 gleichzeitig");
        PIN(ok_ende,  "Kaskade 2 Bild 121: der letzte Stoss (small(1,120)) sitzt");
    }

    /* A7: GEGENPROBE — ohne Kaskade bleibt alles still (sonst waere der Pin wertlos). */
    re15_rumble_reset();
    {
        int still = 1;
        for (f = 1; f <= 60; f++) {
            re15_rumble_tick(&a0, &a1);
            if (a0 || a1) still = 0;
        }
        PIN(still, "GEGENPROBE: frisch zurueckgesetzte Ringe liefern 60 Bilder lang 0/0");
    }
    re15_rumble_reset();
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    printf("=== R18 Rumble-Ringe (FUN_800396FC) ===\n");
    teil_a_rumble();
    printf("=== %s (%d Fehler) ===\n", s_fail ? "ROT" : "GRUEN", s_fail);
    return s_fail ? 1 : 0;
}
