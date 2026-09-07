/* Karte: DREI BEREICHE, DREI ZUSTAENDE.
 *
 * NUTZER-BEFUND 2026-09-07 (fehler/howto4.png):
 *     "bei den Evidence room und den angrenzenden Raeumen haette ich es wie in howto4
 *      erwartet. Das wenn ich den Evidence Room betrete das so aussieht von der
 *      Markierung. Betrete ich den 1. kleinen Room oben, wird der rot markiert und der
 *      Evidence Room wieder gruen. Betrete ich den 2. kleinen Room oben, wird der rot
 *      markiert und der evidence Room gruen."
 *
 * re15_map_rect_state liefert EINEN Zustand je Rechteck - ROOM1110 leuchtete deshalb
 * als ganzes rot, obwohl seine drei Teile durch belegte Innenwaende getrennt sind.
 * Eine eigene Zone je Teil geht nicht (die Teile braeuchten eigene Rechtecke; auf
 * Blatt 3 ist keins frei - der Versuch verdraengte ROOM1100, Audit 182 -> 202).
 * Die Zone bleibt deshalb ganz und ihr Rechteck zerfaellt in Teile mit eigener Weltbox.
 *
 * DIE ZERLEGUNG GEHOERT IN KARTENKOORDINATEN. Zwei Anlaeufe in der Welt scheiterten:
 *   - nur die gezeichneten Innenwaende sperren: ROOM1110s senkrechte Wand endet bei
 *     z=7100, die Bbox reicht bis 7600 -> durch die Luecke haengt alles zusammen (1 Teil)
 *   - ALLE Wandzellen sperren: die Moebel zerhacken den Raum in Dutzende Kammern
 * Richtig ist die gemalte Flaeche, zerschnitten von genau den Linien, die der Port dort
 * zeichnet - die sind auf die Flaeche begrenzt und laufen von Rand zu Rand.
 *
 * Erwartet (aus dem Generator, gegen sein Bild geprueft):
 *     Evidence Room  (146,114) 42x31   Welt x -2909..14950
 *     klein oben     (189,122) 24x11   Welt z -3806..1320
 *     klein unten    (189,134) 24x11   Welt z  1961..7087
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_inv_screen.h"
extern re15_inv_screen_t g_inv_screen;

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

extern unsigned g_current_room_id;

/* Der Spieler steht bei (x,z) in ROOM1110 - welchen Zustand haben die drei Teile? */
static void stelle_hin(int32_t x, int32_t z)
{
    g_current_room_id = 0x1110u;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = x;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = z;
    re15_map_zone_update(0x1110u, x, z);
}

int main(void)
{
    int n, k;
    char t[220];
    printf("=== Karte: drei Bereiche, drei Zustaende ===\n");

    re15_map_visited_reset();
    re15_map_visited_mark(0x1110u);

    n = re15_map_teil_count(3, 5);
    snprintf(t, sizeof t, "ROOM1110 (Blatt 3 Rect 5) hat drei Teilbereiche - sind %d", n);
    CHECK(t, n == 3);
    if (n != 3) { printf("\nFEHLER\n"); return 1; }

    for (k = 0; k < n; k++) {
        int tx, ty, tw, th, ts;
        if (!re15_map_teil_get(3, 5, k, &tx, &ty, &tw, &th, &ts)) continue;
        printf("     Teil %d: (%3d,%3d) %2dx%-2d\n", k, tx, ty, tw, th);
    }

    /* ⛔ DIE DREI STANDORTE SIND GEMESSEN, NICHT GEWAEHLT: es sind die Mitten der drei
     * Weltboxen aus s_map_teile. Wer an einer Box dreht, muss auch hier nachziehen -
     * und dann steht in der Ausgabe, welcher Teil rot war. */
    {
        struct { int32_t x, z; int soll; const char *wo; } P[] = {
            {  6020,  -217, 0, "Evidence Room" },
            { -8603, -1243, 1, "kleiner Raum oben" },
            { -8603,  4524, 2, "kleiner Raum unten" },
        };
        int p;
        for (p = 0; p < 3; p++) {
            int rot = -1, anz_rot = 0;
            stelle_hin(P[p].x, P[p].z);
            for (k = 0; k < n; k++) {
                int tx, ty, tw, th, ts;
                if (!re15_map_teil_get(3, 5, k, &tx, &ty, &tw, &th, &ts)) continue;
                if (ts == RE15_MAP_RECT_CURRENT) { rot = k; anz_rot++; }
            }
            snprintf(t, sizeof t,
                     "im %s (Welt %d,%d): GENAU EIN Teil ist aktuell - sind %d",
                     P[p].wo, (int)P[p].x, (int)P[p].z, anz_rot);
            CHECK(t, anz_rot == 1);
            snprintf(t, sizeof t,
                     "im %s ist Teil %d aktuell - ist Teil %d",
                     P[p].wo, P[p].soll, rot);
            CHECK(t, rot == P[p].soll);
            /* Und die anderen beiden muessen GRUEN sein, nicht schwarz - der Raum
             * ist ja besucht. Das ist der zweite Halbsatz des Nutzers: "und der
             * Evidence Room wieder gruen". */
            for (k = 0; k < n; k++) {
                int tx, ty, tw, th, ts;
                if (k == rot) continue;
                if (!re15_map_teil_get(3, 5, k, &tx, &ty, &tw, &th, &ts)) continue;
                snprintf(t, sizeof t, "im %s ist Teil %d besucht (gruen), nicht schwarz",
                         P[p].wo, k);
                CHECK(t, ts == RE15_MAP_RECT_VISITED);
            }
        }
    }

    /* Die Teile duerfen sich nicht ueberlappen - sonst malt einer den anderen zu. */
    {
        int a, b2, ok = 1;
        for (a = 0; a < n; a++) for (b2 = a + 1; b2 < n; b2++) {
            int ax, ay, aw, ah, as, bx, by, bw, bh, bs;
            if (!re15_map_teil_get(3, 5, a, &ax, &ay, &aw, &ah, &as)) continue;
            if (!re15_map_teil_get(3, 5, b2, &bx, &by, &bw, &bh, &bs)) continue;
            if (ax < bx + bw && bx < ax + aw && ay < by + bh && by < ay + ah) ok = 0;
        }
        CHECK("die drei Bildausschnitte ueberlappen einander nicht", ok);
    }

    /* DIE WAND DARF NICHT IN DEN FREMDEN TEIL HINEINROTEN.
     * NUTZER-BEFUND 2026-09-07 (fehler/error2.png): "this half should be green" - er
     * steht im oberen kleinen Raum; der untere ist korrekt gruen GEFUELLT, aber seine
     * linke Kante leuchtete rot mit. Die senkrechte Wand (188,122) laeuft 23 px ueber
     * BEIDE Teile und bekam EINE Farbe.
     * (Mein Befund davor - die Teile wuerden gar nicht gemalt - war FALSCH: mein
     * Farbfilter erfasste nur helles Gruen, nicht die Fuellung rgb(0,64,40).)
     * Geprueft an der Op-Liste: kein roter Wandabschnitt darf INNEN in einem Teil
     * liegen, der nicht aktuell ist. */
    {
        static re15_inv_op_t ops[RE15_INV_MAX_OPS];
        int p2;
        struct { int32_t x, z; const char *wo; } Q[] = {
            {  6020,  -217, "Evidence Room" },
            { -8603, -1243, "kleiner Raum oben" },
            { -8603,  4524, "kleiner Raum unten" },
        };
        for (p2 = 0; p2 < 3; p2++) {
            int nops, o, schlecht = 0;
            stelle_hin(Q[p2].x, Q[p2].z);
            memset(&g_inv_screen, 0, sizeof g_inv_screen);
            g_inv_screen.substate = 1; g_inv_screen.item_state = 1;
            g_inv_screen.map_page = 3;
            nops = re15_inv_screen_build(&g_inv_screen, ops, RE15_INV_MAX_OPS);
            for (o = 0; o < nops; o++) {
                re15_inv_op_t *q = &ops[o];
                int k4;
                if (q->kind != RE15_INV_OP_FILL) continue;
                if (!(q->r > 150 && q->g < 80)) continue;
                if (q->w > 4 && q->h > 4) continue;
                for (k4 = 0; k4 < n; k4++) {
                    int tx, ty, tw, th, ts;
                    if (!re15_map_teil_get(3, 5, k4, &tx, &ty, &tw, &th, &ts)) continue;
                    if (ts == RE15_MAP_RECT_CURRENT) continue;
                    if (q->x >= tx && q->x + q->w <= tx + tw &&
                        q->y >= ty && q->y + q->h <= ty + th) schlecht++;
                }
            }
            snprintf(t, sizeof t,
                     "im %s: kein roter Wandabschnitt in einem fremden Teil - sind %d",
                     Q[p2].wo, schlecht);
            CHECK(t, schlecht == 0);
        }
    }

    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
