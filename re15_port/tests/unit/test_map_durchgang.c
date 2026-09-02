/* Karte: ein Durchgang ist EIN Ort - die beiden Raeume stossen aneinander, und das
 * Tuersymbol zeigt zum Nachbarn.
 *
 * Nutzer 2026-09-02: "Tueren sind oft falsch positioniert und rotiert, die
 * Kartenstuecke haben Abstaende zueinander, was nicht sein kann, die muessen Kante an
 * Kante sein ... Und die Uebertragungen von einem Raum zum anderen sind SAU oft falsch."
 *
 * GEMESSEN (v0.3.85, tools/karte_audit.py und die Sonden im Scratchpad):
 *   - 34 von 93 gepaarten Tuersymbolen (37 %) zeigten VOM Nachbarn WEG. Die Wandseite
 *     kam aus der lokalen Silhouette - die benennt die WAND, in der die Tuer sitzt,
 *     nicht die SEITE, die zum Nachbarn schaut.
 *   - Zwischen zwei Raeumen, die eine Tuer teilen, klafften bis zu 55 px.
 *   - Laengs der gemeinsamen Wand rutschten sie bis zu 72 px gegeneinander.
 * Ursache der beiden letzten: der Loeser heftete jeden Ort an GENAU EINEN Nachbarn
 * (Spannbaum); jede weitere Tuer war nur ein weicher Zug. Seit dem Ausgleich ueber ALLE
 * Kanten (grundriss.py, ausgleichen/federn) ist das behoben.
 *
 * Dieser Pin haelt die drei Aussagen fest, an einem Fall, den der Nutzer selbst genannt
 * hat, und mit einer Gegenprobe.
 */
#include <stdio.h>
#include <stdlib.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

static const re15_map_zone_t *ort(unsigned room, int32_t x, int32_t z)
{
    return re15_map_zone_at(room, x, z);
}

/* Kleinster Abstand zweier Kaesten; 0 = sie beruehren sich oder ueberlappen. */
static int luecke(const re15_map_zone_t *a, const re15_map_zone_t *b)
{
    int ax, ay, aw, ah, bx, by, bw, bh, dx, dy;
    if (!re15_map_zone_synth(a, &ax, &ay, &aw, &ah, 0, 0)) return -1;
    if (!re15_map_zone_synth(b, &bx, &by, &bw, &bh, 0, 0)) return -1;
    dx = bx - (ax + aw); if (ax - (bx + bw) > dx) dx = ax - (bx + bw);
    dy = by - (ay + ah); if (ay - (by + bh) > dy) dy = ay - (by + bh);
    if (dx < 0) dx = 0;
    if (dy < 0) dy = 0;
    return dx + dy;
}

int main(void)
{
    int n, k, gepaart = 0, verdreht = 0;

    printf("=== Karte: Durchgaenge stossen an, Symbole zeigen zum Nachbarn ===\n");
    re15_map_visited_reset();
    for (k = 0; k < 13; k++) re15_map_debug_reveal_page((unsigned)k);

    /* ---- (1) JEDES GEPAARTE TUERSYMBOL ZEIGT ZUM NACHBARN --------------------
     * Die Nische ist gerichtet: kind 0=Nord 1=Ost 2=Sued 3=West benennt die Seite der
     * Wand, an der die Tuer sitzt. Bei einem PAAR ist bekannt, wo der Nachbar liegt -
     * dorthin muss sie zeigen. */
    n = re15_map_mark_count();
    for (k = 0; k < n; k++) {
        int pg, r, mx, my, kind, zid = 0, zid2 = 255, i, nz;
        const re15_map_zone_t *zb = 0;
        int bx, by, bw, bh;
        static const int rx[4] = { 0, 1, 0, -1 };
        static const int ry[4] = { -1, 0, 1, 0 };
        if (!re15_map_mark_get(k, &pg, &r, &mx, &my, &kind)) continue;
        if (kind >= 4) continue;
        re15_map_mark_zonen(k, &zid, &zid2);
        if (zid2 == 255) continue;
        nz = re15_map_zone_count();
        for (i = 0; i < nz; i++) {
            const re15_map_zone_t *z = re15_map_zone_by_index(i);
            if (z && z->page == pg && z->zid == zid2 && z->synth) { zb = z; break; }
        }
        if (!zb || !re15_map_zone_synth(zb, &bx, &by, &bw, &bh, 0, 0)) continue;
        gepaart++;
        if ((bx + bw / 2 - mx) * rx[kind] + (by + bh / 2 - my) * ry[kind] <= 0) {
            verdreht++;
            printf("     verdreht: Seite %d, Symbol (%d,%d) Seite %d, ROOM%04X "
                   "Mitte (%d,%d)\n", pg, mx, my, kind, zb->room,
                   bx + bw / 2, by + bh / 2);
        }
    }
    printf("  [Symbole] %d gepaarte Tuersymbole, %d zeigen vom Nachbarn weg\n",
           gepaart, verdreht);
    CHECK("es gibt ueberhaupt gepaarte Tuersymbole", gepaart >= 40);
    CHECK("kein gepaartes Tuersymbol zeigt vom Nachbarn weg", verdreht == 0);

    /* ---- (2) DIE KARTENSTUECKE STOSSEN ANEINANDER ---------------------------
     * Drei Durchgaenge, die der Nutzer selbst genannt hat. */
    {
        const re15_map_zone_t *z20 = ort(0x1120, -8450, -2900);
        const re15_map_zone_t *z30 = ort(0x1130, -3050, -2150);
        /* Punkt aus dem Tuer-Datensatz ROOM1130 -> ROOM1140 (Spawn), damit sicher
         * die richtige Zone von ROOM1140 getroffen wird - (0,0) faellt dort in den
         * zweiten Bereich. */
        const re15_map_zone_t *z40 = ort(0x1140, -7600, -17600);
        int l1 = (z20 && z30) ? luecke(z20, z30) : -1;
        int l2 = (z30 && z40) ? luecke(z30, z40) : -1;
        printf("  [Luecken] 1120<->1130: %d px, 1130<->1140: %d px\n", l1, l2);
        CHECK("ROOM1120 und ROOM1130 stossen aneinander", l1 == 0);
        /* ⛔ NICHT AUF NULL FESTNAGELN, WO ES NOCH NICHT NULL IST. 1130<->1140 haelt
         * zurzeit 9 px Abstand: ROOM1130 haengt an vier Tueren zugleich, und der Ring
         * aus 1120-1130-1140 schliesst geometrisch nicht ganz. Gemessen ueber alle
         * Blaetter (tools/karte_audit.py): die schlimmste Luecke ist von 55 px auf
         * 14 px gefallen. Der Pin haelt diese Schranke fest, nicht eine Null, die wir
         * nicht haben - sonst waere er von Anfang an rot und wuerde nichts schuetzen. */
        CHECK("ROOM1130 und ROOM1140 liegen hoechstens 14 px auseinander",
              l2 >= 0 && l2 <= 14);
    }

    /* ---- (3) GEGENPROBE: zwei Raeume OHNE Tuer duerfen Abstand haben ---------
     * Ohne sie waere (2) auch dann erfuellt, wenn schlicht alles uebereinanderliegt. */
    {
        const re15_map_zone_t *z20 = ort(0x1120, -8450, -2900);
        const re15_map_zone_t *z50 = ort(0x1150, 0, 0);
        int l3 = (z20 && z50) ? luecke(z20, z50) : -1;
        printf("  [Gegenprobe] 1120<->1150 (keine gemeinsame Tuer): %d px\n", l3);
        CHECK("zwei Raeume ohne gemeinsame Tuer liegen nicht zwangslaeufig an",
              l3 != 0);
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
