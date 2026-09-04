/* Karte: die Achse eines Tuersymbols folgt der gemeinsamen Wand, nicht dem Zufall.
 *
 * ⛔ NUTZER-BEFUND 2026-09-04 (fehler/error1.png): auf Blatt 4 zeigt ein Pfeil auf das
 * Tuersymbol zwischen ROOM1130 und ROOM1150 mit dem Text
 *     "Wrong Rotation, Need 90 Degree turn"
 * Es wurde WAAGERECHT gezeichnet, obwohl die beiden Rechtecke NEBENEINANDER liegen -
 * die gemeinsame Wand ist also senkrecht.
 *
 * Der Zeichner (re15_inv_screen.c) liest aus kind nur die ACHSE:
 *   kind 0/2 (Nord/Sued) -> waagerechter 5x2-Balken
 *   kind 1/3 (Ost/West)  -> senkrechter  2x5-Balken
 *
 * PRUEFUNG: Fuer eine GEPAARTE Tuermarke sind beide Rechtecke bekannt. Ueberlappen sie
 * sich in x deutlich weniger als in y, trennt sie eine SENKRECHTE Wand (und umgekehrt).
 * Die Achse der Marke muss dazu passen.
 *
 * ⛔ WARUM "DEUTLICH" UND NICHT "IRGENDWIE". Bei annaehernd gleicher Ueberdeckung sagt
 * die Geometrie nichts - die Rechtecke beruehren sich dann ueber Eck. Solche Faelle
 * werden NICHT bewertet, statt sie mit einer Zufallsantwort zu belasten. Gemessen am
 * 2026-09-04 ueber die ganze Tabelle:
 *     Trennschaerfe >=  0 px : 27 passen, 13 widersprechen,  0 unentschieden
 *     Trennschaerfe >=  4 px : 25 passen, 10 widersprechen,  5 unentschieden
 *     Trennschaerfe >=  8 px : 23 passen,  8 widersprechen,  9 unentschieden
 *     Trennschaerfe >= 16 px : 21 passen,  8 widersprechen, 11 unentschieden
 * Ab 8 px aendert sich die MENGE der Widersprueche nicht mehr (identisch bei 8 und 16) -
 * das ist die stabile Schranke, keine gefuehlte Zahl.
 *
 * Der Test gibt seine ABDECKUNG aus (wie viele Marken bewertet, wie viele nicht und
 * warum), damit kein Aggregat einen Einzelfall zudeckt und niemand still gruen bleibt,
 * weil die Menge leer lief (Memory reai-v2-schiene-abdeckung).
 */
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

#define TRENNSCHAERFE 8

/* Rechteck-Index der Zone zid auf Blatt page (erste passende Zone). */
static int rect_von_zone(int zid, int page, int *rx, int *ry, int *rw, int *rh)
{
    int i, n = re15_map_zone_count();
    for (i = 0; i < n; i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (!z || z->zid != zid || z->page != page) continue;
        if (z->rect == 255) return -1;               /* Grundriss, kein gemaltes Rechteck */
        if (!re15_map_rect_geometry((unsigned)page, z->rect, rx, ry, rw, rh)) return -1;
        return z->rect;
    }
    return -1;
}

int main(void)
{
    int k, n, bewertet = 0, passt = 0, unentschieden = 0, keine_daten = 0, treppen = 0,
        ungepaart = 0, gleiches_rect = 0, widerspruch = 0;

    printf("=== Karte: Tuersymbol-Achse folgt der gemeinsamen Wand ===\n");

    /* ⛔ OHNE DAS MISST DER TEST NICHTS. re15_map_mark_get liefert eine Marke nur,
     * wenn eine ihrer Zonen BESUCHT ist (re15_map_zones.c:586) - im frischen
     * Testprozess ist keine besucht, und die Schleife lief ueber alle 189 Marken,
     * ohne eine einzige zu bewerten. Aufgefallen ist das NUR, weil die
     * Abdeckungszeile "189 ohne gemaltes Rechteck" ausgab und die Bedingung
     * "Pruefmenge nicht leer" ansprang - genau dafuer stehen beide da
     * (Memory reai-v2-schiene-abdeckung). */
    re15_map_visited_reset();
    for (k = 0; k < 13; k++) re15_map_debug_reveal_page((unsigned)k);

    n = re15_map_mark_count();
    for (k = 0; k < n; k++) {
        int pg, rc, mx, my, kind, za, zb;
        int ax, ay, aw, ah, bx, by, bw, bh, ra, rb;
        int ux, uy, erwartet_senk, ist_senk;

        if (!re15_map_mark_get(k, &pg, &rc, &mx, &my, &kind)) { keine_daten++; continue; }
        if (kind > 3)                       { treppen++;   continue; }   /* Treppe */
        if (!re15_map_mark_zonen(k, &za, &zb)) { keine_daten++; continue; }
        if (zb == 255)                      { ungepaart++; continue; }

        ra = rect_von_zone(za, pg, &ax, &ay, &aw, &ah);
        rb = rect_von_zone(zb, pg, &bx, &by, &bw, &bh);
        if (ra < 0 || rb < 0)               { keine_daten++;    continue; }
        if (ra == rb)                       { gleiches_rect++;  continue; }

        ux = (ax + aw < bx + bw ? ax + aw : bx + bw) - (ax > bx ? ax : bx);
        uy = (ay + ah < by + bh ? ay + ah : by + bh) - (ay > by ? ay : by);
        if (abs(ux - uy) < TRENNSCHAERFE)   { unentschieden++;  continue; }

        /* ⛔ EINE UEBERDECKUNG IST NUR DANN EINE WAND, WENN SIE FUER BEIDE RECHTECKE
         * SCHMAL IST. Steckt ein Rechteck IM anderen, ueberlappen sie grossflaechig und
         * es gibt gar keine gemeinsame Wandlinie - die laengere Ueberdeckungsseite sagt
         * dann nichts ueber die Wandrichtung. Gemessen 2026-09-04 an drei Faellen, die
         * ohne diese Probe falsch bewertet wurden:
         *     Blatt 7 (206,106): Ueberdeckung x=8, aber der Nachbar ist selbst nur 8 breit
         *     Blatt 7 (224,175): Ueberdeckung x=24, aber der eigene Kasten ist 24 breit
         *     Blatt 2 (111, 84): Ueberdeckung x=17 gegen einen 24 px breiten Nachbarn
         * Der Nutzer-Fall ROOM1130 <-> ROOM1150 besteht sie klar: Ueberdeckung x=8 bei
         * zwei je 32 px breiten Rechtecken. Dieselbe Probe steht im Generator
         * (tools/gen_map_zones.py) - beide muessen dasselbe messen. */
        {
            int kw = (aw < bw ? aw : bw), kh = (ah < bh ? ah : bh);
            int wand_senk = (uy > ux) && (ux * 2 < kw);
            int wand_waag = (ux > uy) && (uy * 2 < kh);
            if (!wand_senk && !wand_waag) { unentschieden++; continue; }
        }

        bewertet++;
        erwartet_senk = (ux < uy);          /* schmale x-Ueberdeckung = senkrechte Wand */
        ist_senk      = (kind == 1 || kind == 3);
        if (erwartet_senk == ist_senk) {
            passt++;
        } else {
            widerspruch++;
            printf("  [Widerspruch] Marke #%d Blatt %d rect %d (%d,%d) kind %d: "
                   "Ueberdeckung x=%d y=%d verlangt %s, gezeichnet wird %s\n",
                   k, pg, rc, mx, my, kind, ux, uy,
                   erwartet_senk ? "senkrecht" : "waagerecht",
                   ist_senk ? "senkrecht" : "waagerecht");
        }
    }

    printf("  [Abdeckung] %d Marken gesamt: %d bewertet (%d passen, %d widersprechen), "
           "%d unentschieden (<%d px Trennschaerfe)\n",
           n, bewertet, passt, widerspruch, unentschieden, TRENNSCHAERFE);
    printf("  [Abdeckung] nicht bewertbar: %d Treppen, %d ungepaart, %d gleiches Rechteck, "
           "%d ohne gemaltes Rechteck\n", treppen, ungepaart, gleiches_rect, keine_daten);

    /* ⛔ DIE MENGE DARF NICHT LEER LAUFEN. Ohne diese Bedingung waere der Test still
     * gruen, sobald sich die Tabellenform aendert und keine Marke mehr durchkommt. */
    CHECK("die Pruefmenge ist nicht leer (>= 20 bewertete Marken)", bewertet >= 20);
    CHECK("KEIN Tuersymbol steht quer zu seiner gemeinsamen Wand", widerspruch == 0);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
