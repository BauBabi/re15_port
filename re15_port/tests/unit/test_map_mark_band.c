/* Karte: eine Marke gehoert auf das Blatt IHRES Bandes, nicht auf das der Zone.
 *
 * Nutzer 2026-09-01: "Bei ROOM 1170 zeigt er die Tuer, die eigentlich fuer die Etage
 * unten gedacht ist, bereits bei der Roof-Etage an."
 *
 * ROOM1170s zweiter Bereich (Zone 1) ist auf ZWEI Blaettern gezeichnet: Seite 5 Rect 0
 * (ROOF) und Seite 4 Rect 3 (3F). Er fuehrt drei Tueren, und das Band steht in ihrem
 * Aot_set-Datensatz (pc[4] = obj[0x82]; das Original gattet die Interaktion darauf,
 * FUN_8002bd44 @0x8002bf38):
 *     Band 0, Welt(-21230,-17925) -> ROOM1130   (Seite 4 = 3F)
 *     Band 0, Welt(-21600,-27005) -> ROOM1140   (Seite 4 = 3F)
 *     Band 4, Welt(-11065,-27850) -> ROOM1170   (Seite 5 = ROOF)
 * und vier Treppen-Datensaetze mit den Baendern 0, 2, 2, 4.
 *
 * Bis v0.3.80 legte der Generator ALLE Marken einer Zone auf die Vorgabeseite der Zone
 * (hier ROOF) und kopierte sie von dort auf die Etagenblaetter — die beiden 3F-Tueren
 * standen damit auf dem Dach. Dieser Pin haelt fest, dass das ROOF-Blatt keine
 * Band-0-Marke mehr traegt und dass die Treppe auf BEIDEN Etagen erscheint, die sie
 * verbindet (sie liegt als zwei Datensaetze vor, einer je Ende, jeder mit eigenem Band). */
#include <stdio.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    int n, k, roof_tueren = 0, roof_treppen = 0, f3_treppen = 0, f3_tueren = 0;
    const re15_map_zone_t *z1170;
    int zid1170;

    printf("=== Karte: Marke folgt ihrem Band (ROOM1170 ROOF gegen 3F) ===\n");
    re15_map_visited_reset();
    re15_map_debug_reveal_page(5);
    re15_map_debug_reveal_page(4);

    /* GEZAEHLT WIRD JETZT JE BLATT, NICHT JE RECHTECK (2026-09-02).
     * Seit dem Umbau auf Grundrisse traegt die Marke ihres Ortes die Kennung
     * rect = 255 - ein Filter auf "Seite 5 Rect 0" faende gar nichts mehr. Die
     * Aussage des Pins ist davon unberuehrt, denn Seite 5 (ROOF) enthaelt genau
     * ROOM1170s beide Orte und sonst nichts; das Blatt IST also der Bereich.
     * Fuer die Zweitzeichnung auf 3F bleibt Rect 3 gueltig - dorthin legt
     * blatt_fuer_band die Marken der unteren Baender, eingepasst in das Rechteck
     * des Kuenstlers. */
    z1170 = re15_map_zone_fuer(0x1170, 1, 4);
    if (!z1170) { printf("  FAIL: ROOM1170s zweiter Bereich liegt nicht auf 3F\n"); return 1; }
    zid1170 = z1170->zid;
    printf("  [Gast-Lage] ROOM1170 z1 auf Seite 4, zid %d, Kennung etage=%d\n",
           zid1170, (int)z1170->etage);
    CHECK("ROOM1170s zweiter Bereich ist auf 3F eine GAST-Lage", z1170->etage == 1);

    n = re15_map_mark_count();
    for (k = 0; k < n; k++) {
        int pg, r, mx, my, kind, zid = 0, zid2 = 255;
        if (!re15_map_mark_get(k, &pg, &r, &mx, &my, &kind)) continue;
        re15_map_mark_zonen(k, &zid, &zid2);
        if (pg == 5) {
            if (kind >= 4) roof_treppen++;
            /* Eine Tuer, die BEIDE Orte von ROOM1170 verbindet, ist die Selbst-Tuer
             * zwischen Hof und oberem Absatz - die gehoert aufs Dach. Gezaehlt wird
             * nur, was nach DRAUSSEN fuehrt. */
            else if (zid2 == 255) roof_tueren++;
        }
        /* ⛔ AUCH AUF 3F GEHT ES JETZT JE BLATT, NICHT JE RECHTECK (2026-09-02).
         * ROOM1170s zweiter Bereich hat auf dem 3F-Blatt eine EIGENE Lage vom Loeser
         * (Gast-Zeile, rect = 255) statt eines eingepassten Kunst-Rechtecks. Erkannt
         * wird er an seiner Zonen-Nummer. */
        if (pg == 4 && zid == zid1170) { if (kind >= 4) f3_treppen++; else f3_tueren++; }
    }
    printf("  [ROOF Seite 5] %d Tueren nach draussen, %d Treppen\n",
           roof_tueren, roof_treppen);
    printf("  [3F Seite 4 Rect 3] %d Tueren, %d Treppen\n", f3_tueren, f3_treppen);

    CHECK("das ROOF-Blatt traegt keine Tuer des unteren Bereichs mehr", roof_tueren == 0);
    CHECK("die Treppe erscheint auf dem ROOF-Blatt", roof_treppen > 0);
    CHECK("die Treppe erscheint auch auf dem 3F-Blatt", f3_treppen > 0);
    /* Gegenprobe: ohne sie waere "keine Tuer nach draussen" auch dann erfuellt,
     * wenn Seite 5 ueberhaupt keine Marke traegt. */
    CHECK("Seite 5 traegt ueberhaupt Marken", roof_treppen > 0);
    {   /* Und dass ROOM1170s zweiter Ort auf 3F ueber eine Etagen-Zeile haengt. */
        int fp = -1, fr = -1;
        CHECK("ROOM1170s zweiter Ort ist auf 3F als Etagen-Zeichnung gefuehrt",
              re15_map_floor_lookup(0x1170, 1, 0, &fp, &fr) && fp == 4);
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
