/* Karte: die ORIGINAL-KARTENZEILE gilt nur auf ihrem EIGENEN Blatt.
 *
 * NUTZER-BEFUND 2026-09-07, im Treppenhaus auf 2F:
 *     "Da habe ich auch gesehen das sich der Marker nicht bewegt, wenn sich der
 *      Spieler bewegt."
 *
 * URSACHE. Die Zeile @0x800768b0 (FUN_800473f8 @0x8004741c-0x80047528) ist eine
 * ABSOLUTE Abbildung Welt -> Blattkoordinate:
 *      mx =  ((((wx + 32000) * 10 * sx) >> 20) + 5) / 10 + ox
 *      my = -((((wz + 32000) * 10 * sy) >> 20) + 5) / 10 + oy
 * Sie ist auf das Blatt geeicht, auf dem das ORIGINAL den Raum zeichnet. Die
 * Etagen-Umschaltung ist dagegen eine PORT-ERGAENZUNG: der Port zeichnet denselben
 * Raum zusaetzlich auf den Blaettern seiner anderen Baender, dort an ANDERER Stelle.
 * Dieselbe absolute Zeile zeigt daneben; der Marker wird an die Rechteckkante
 * geklemmt und steht still.
 *
 * GEMESSEN an den 522 Standorten, die der Nutzer selbst in ROOM1060 abgelaufen ist
 * (befund.log vom 2026-09-07): die Zeile lieferte auf 2F fuer JEDEN davon x = 122 -
 * eine einzige Spalte, y nur 149..154. Ueber die begehbaren Punkte (SCA-Zellen,
 * 4x4-Raster je Zelle), verschiedene Markerpixel nach der Klemmung:
 *      ROOM1060 Seite  3 Rect  1 :  14/192 innen ->   6 Pixel | ohne Zeile  80
 *      ROOM1060 Seite  4 Rect  1 :   0/192 innen ->   9 Pixel | ohne Zeile  80
 *      ROOM1080 Seite  3 Rect  4 :  30/128 innen ->  17 Pixel | ohne Zeile  12
 *      ROOM1080 Seite  4 Rect  0 :   0/128 innen ->   5 Pixel | ohne Zeile  12
 *      ROOM4020 Seite  9 Rect 13 :   0/128 innen ->   6 Pixel | ohne Zeile  13
 *      ROOM4020 Seite 10 Rect  3 :   8/128 innen ->   3 Pixel | ohne Zeile  13
 *      ROOM50D0 Seite 11 Rect  0 :  89/496 innen ->  53 Pixel | ohne Zeile 244
 *
 * Auf dem Originalblatt bleibt die Zeile unangetastet - dort ist sie die Vorgabe des
 * Originals und damit der Massstab. Geprueft wird deshalb NUR die Gast-Zeile.
 *
 * ⛔ ZWEI PRUEFUNGEN, NICHT EINE. Die REGEL (Gast-Zeile traegt keine Original-Zeile)
 * ist das, was der Generator tut; die WIRKUNG (der Marker bewegt sich ueber den Raum)
 * ist das, was der Nutzer sieht. Eine Regel ohne Wirkungsmass hat mich hier schon
 * einmal getaeuscht - eine Zahl kann besser werden, waehrend das Ziel schlechter wird.
 */
#include <stdio.h>
#include <string.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Die Gast-Zeilen, die der Generator heute erzeugt (Raum, Zone, Blatt). */
static const struct { unsigned room; int zi; unsigned page; const char *was; } GAST[] = {
    { 0x1060, 0,  3, "Treppenhaus auf 2F" },
    { 0x1060, 0,  4, "Treppenhaus auf 3F" },
    { 0x1080, 0,  3, "Fahrstuhl auf 2F"   },
    { 0x1080, 0,  4, "Fahrstuhl auf 3F"   },
    { 0x4020, 0,  9, "ROOM4020, Blatt 9"  },
    { 0x4020, 0, 10, "ROOM4020, Blatt 10" },
    { 0x50D0, 0, 11, "ROOM50D0, Blatt 11" },
};
#define NGAST ((int)(sizeof GAST / sizeof GAST[0]))

/* Verschiedene Markerpixel ueber ein 16x16-Raster der Zonen-Box, mit derselben
 * Klemmung wie der Zeichner (re15_inv_screen.c: Reserve 4 auf einem Kunst-Rechteck). */
static int marker_pixel(const re15_map_zone_t *zn)
{
    int rx, ry, rw, rh, i, j, n = 0;
    static unsigned char gesehen[320 * 240 / 8];
    memset(gesehen, 0, sizeof gesehen);
    if (!re15_map_rect_geometry(zn->page, zn->rect, &rx, &ry, &rw, &rh)) return -1;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            int32_t x = zn->wx0 + (int32_t)((int64_t)(zn->wx1 - zn->wx0) * (2*i+1) / 32);
            int32_t z = zn->wz0 + (int32_t)((int64_t)(zn->wz1 - zn->wz0) * (2*j+1) / 32);
            int16_t mx, my; int lo_x, hi_x, lo_y, hi_y, b;
            if (!re15_map_zone_marker(zn, x, z, rx, ry, rw, rh, &mx, &my)) continue;
            lo_x = rx + 4; hi_x = rx + rw - 4; lo_y = ry + 4; hi_y = ry + rh - 4;
            if (hi_x < lo_x) { lo_x = hi_x = rx + rw / 2; }
            if (hi_y < lo_y) { lo_y = hi_y = ry + rh / 2; }
            if (mx < lo_x) mx = (int16_t)lo_x;
            if (mx > hi_x) mx = (int16_t)hi_x;
            if (my < lo_y) my = (int16_t)lo_y;
            if (my > hi_y) my = (int16_t)hi_y;
            if (mx < 0 || mx >= 320 || my < 0 || my >= 240) continue;
            b = my * 320 + mx;
            if (!((gesehen[b >> 3] >> (b & 7)) & 1)) {
                gesehen[b >> 3] |= (unsigned char)(1u << (b & 7));
                n++;
            }
        }
    }
    return n;
}

int main(void)
{
    int i, gefunden = 0;
    printf("=== Karte: Original-Zeile nur auf dem eigenen Blatt ===\n");
    for (i = 0; i < NGAST; i++) {
        const re15_map_zone_t *zn = re15_map_zone_fuer(GAST[i].room, GAST[i].zi,
                                                       GAST[i].page);
        char t[160];
        if (!zn) {
            printf("  HINWEIS: ROOM%04X Zone %d hat auf Blatt %u keine Zeile mehr"
                   " - uebersprungen\n", GAST[i].room, GAST[i].zi, GAST[i].page);
            continue;
        }
        gefunden++;
        snprintf(t, sizeof t, "%s (ROOM%04X, Blatt %u) ist eine Gast-Zeile",
                 GAST[i].was, GAST[i].room, GAST[i].page);
        CHECK(t, zn->etage != 0);
        snprintf(t, sizeof t, "%s traegt KEINE Original-Kartenzeile (sx=%d sy=%d)",
                 GAST[i].was, zn->sx, zn->sy);
        CHECK(t, zn->sx == 0 && zn->sy == 0);
        {
            int n = marker_pixel(zn);
            snprintf(t, sizeof t, "%s: Marker erreicht %d verschiedene Pixel (>= 10)",
                     GAST[i].was, n);
            CHECK(t, n >= 10);
        }
    }
    CHECK("mindestens 5 Gast-Zeilen geprueft (ABDECKUNG)", gefunden >= 5);
    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
