/* Karte: Raeume ohne Karten-Rechteck werden aus ihrer KOLLISIONS-BOX gezeichnet.
 *
 * Nutzer 2026-09-01: "Wo die Kartenlage fehlt oder unklar ist, die Collision-Box des
 * Raums nutzen."
 *
 * 33 der 100 Karten-Raeume hatten gar kein Rechteck, insgesamt 40,7 % der begehbaren
 * Flaeche. Der Spieler-Marker fand dort keine Zone - das ist der Sprung, den der Nutzer
 * auf 2F sah. ROOM10D0, der 2F-Flur, ist der prominenteste Fall: sein begehbarer
 * Bereich misst im ausgelieferten Massstab (@0x800768b0, STAGE1-Median 459/464
 * Welteinheiten je Pixel) 70 x 89 px, das groesste Rechteck der Seite 3 ist 72 x 64.
 *
 * ⛔ PORT-ERGAENZUNG: das Original zeichnet diese Raeume nicht. Die Zeichnung traegt
 * rect == 255 und einen Index in s_map_synth. */
#include <stdio.h>
#include "re15_room.h"
#include "re15_actor.h"
#include "re15_collision.h"

extern unsigned g_current_room_id;

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    const re15_map_zone_t *zn;
    int x, y, w, h, erste, n, i, n_synth = 0, n_ueber = 0;

    printf("=== Karte: Schema-Zeichnung aus der Kollisions-Box ===\n");

    /* (1) ROOM10D0 - der 2F-Flur - hat eine Zeichnung. */
    zn = re15_map_zone_at(0x10D0, 0, 0);
    if (!zn) { /* ueber die Weltmitte seiner Zone suchen */
        zn = re15_map_zone_at(0x10D0, -5000, 10000);
    }
    CHECK("ROOM10D0 hat ueberhaupt eine Zone", zn != 0);
    if (zn) {
        CHECK("ROOM10D0 traegt kein Karten-Rechteck des Originals (rect == 255)",
              zn->rect == 255);
        CHECK("ROOM10D0 hat eine Schema-Zeichnung",
              re15_map_zone_synth(zn, &x, &y, &w, &h, &erste, &n) && n > 0);
        printf("  [ROOM10D0] Kasten (%d,%d) %dx%d, %d Zellen\n", x, y, w, h, n);
        CHECK("ihr Kasten liegt im Kartenfeld", x >= 90 && y >= 50 &&
                                                x + w <= 240 && y + h <= 200);
    }

    /* (2) Keine Schema-Zeichnung darf ein GEMALTES Rechteck ueberdecken. Die Kunst des
     *     Originals ist nicht massstabsgetreu zur Kollision; anker-genau gesetzt lag
     *     ROOM10D0 ueber vier Nachbarn zugleich. */
    for (i = 0; i < re15_map_zone_count(); i++) {
        const re15_map_zone_t *a = re15_map_zone_by_index(i);
        int ax, ay, aw, ah, j;
        if (!a || !re15_map_zone_synth(a, &ax, &ay, &aw, &ah, 0, 0)) continue;
        n_synth++;
        for (j = 0; j < re15_map_zone_count(); j++) {
            const re15_map_zone_t *b = re15_map_zone_by_index(j);
            int bx, by, bw, bh;
            if (!b || b->page != a->page || b->rect == 255) continue;
            if (!re15_map_rect_geometry(b->page, b->rect, &bx, &by, &bw, &bh)) continue;
            if (ax < bx + bw && bx < ax + aw && ay < by + bh && by < ay + ah) {
                n_ueber++;
                printf("  [Ueberlappung] ROOM%04X Schema auf ROOM%04X Rect %d\n",
                       a->room, b->room, b->rect);
                j = re15_map_zone_count();
            }
        }
    }
    printf("  [Bestand] %d Schema-Zeichnungen, %d ueberlappen gemalte Rechtecke\n",
           n_synth, n_ueber);
    CHECK("es gibt Schema-Zeichnungen", n_synth > 0);
    CHECK("keine davon ueberdeckt ein gemaltes Rechteck", n_ueber == 0);

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
