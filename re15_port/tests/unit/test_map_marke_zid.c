/* Karte: JEDE MARKE HAENGT AN IHRER EIGENEN ZONE - nie an einer fremden.
 *
 * NUTZER-BEFUND 2026-09-27 (Runde 30, Thema F; analysis/befunde_runde30/
 * karten-marken.md §4b): "2F ist jetzt unten eine Tuer eingezeichnet auf der Karte
 * die es nicht gibt". Gemessen an seinem Abzug befund_1070_F259_marke1.png: eine
 * gelbe Marke bei (188,178..182), ringsum nur Panel.
 *
 * URSACHE: die Marke gehoert zur Tuer ROOM1090 -> ROOM1100 (ROOM1090.RDT @0x0213A),
 * trug aber die RUECKFALL-Nummer zid 0. tools/gen_map_zones.py setzte fuer jede
 * Marke, deren (Raum, Zone) der Generator nicht kannte, `zid_of.get((b, zi), 0)` -
 * und 0 ist eine GUELTIGE Nummer: ROOM1000 Zone 0. Die Marke erschien deshalb,
 * sobald der Spieler ROOM1000 betreten hatte, auf einem Blatt, auf dem ROOM1000 gar
 * nicht liegt, und ohne Rechteck (rect 255), an dem sie haengen koennte.
 *
 * GEPRUEFT WIRD DIE TABELLE, nicht ein Spielstand:
 *   1. die zid jeder Marke hat eine Zonenzeile AUF DEM BLATT DER MARKE;
 *   2. traegt die Marke ein Rechteck (rect != 255), so gehoert dieses Rechteck auf
 *      diesem Blatt der eigenen Zone oder - nur bei auf_partner - der Partnerzone;
 *   3. traegt sie KEIN Rechteck (rect == 255), so fuehrt ihre Zone auf diesem Blatt
 *      eine Schema-Zeichnung (synth != 0) - sonst hat sie keinen Traeger.
 *
 * Stand vor der Korrektur (gemessen am Stand d98e9639 mit genau diesem Riegel):
 * siehe Dossier karten-marken.md, Abschnitt UMSETZUNG.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_room.h"
#include "re15_map_zones.h"      /* engine/src: dieselben Tabellen, die die Engine fuehrt */

#define NZ ((int)(sizeof s_map_zones / sizeof s_map_zones[0]))
#define NM ((int)(sizeof s_map_marks / sizeof s_map_marks[0]))

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

/* Fuehrt die Zone `zid` auf Blatt `page` eine Zeile? rect < 0: egal welches Rechteck;
 * sonst muss die Zeile genau dieses Rechteck tragen. nur_schema: die Zeile muss eine
 * Schema-Zeichnung fuehren. */
static int zone_hat_zeile(int zid, int page, int rect, int nur_schema)
{
    int i;
    for (i = 0; i < NZ; i++) {
        if (s_map_zones[i].zid != zid) continue;
        if ((int)s_map_zones[i].page != page) continue;
        if (rect >= 0 && (int)s_map_zones[i].rect != rect) continue;
        if (nur_schema && !s_map_zones[i].synth) continue;
        return 1;
    }
    return 0;
}

int main(void)
{
    int i, ohne_zeile = 0, fremdes_rect = 0, ohne_traeger = 0;
    char t[200];

    printf("=== Karte: jede Marke haengt an ihrer eigenen Zone ===\n");
    printf("  %d Marken, %d Zonenzeilen\n", NM, NZ);

    for (i = 0; i < NM; i++) {
        const re15_map_mark_t *m = &s_map_marks[i];
        int eigen_blatt = zone_hat_zeile(m->zid, m->page, -1, 0);
        if (!eigen_blatt) {
            ohne_zeile++;
            printf("     Marke %3d Blatt %2d rect %3d (%3d,%3d) zid %3d: die Zone fuehrt auf "
                   "diesem Blatt KEINE Zeile\n", i, m->page, m->rect, m->mx, m->my, m->zid);
            continue;
        }
        if (m->rect != 255) {
            int eigen   = zone_hat_zeile(m->zid, m->page, m->rect, 0);
            int partner = (m->auf_partner && m->zid2 != 255)
                              ? zone_hat_zeile(m->zid2, m->page, m->rect, 0) : 0;
            if (!eigen && !partner) {
                fremdes_rect++;
                printf("     Marke %3d Blatt %2d rect %3d (%3d,%3d) zid %3d zid2 %3d "
                       "auf_partner %d: das Rechteck gehoert weder der eigenen noch der "
                       "Partnerzone\n", i, m->page, m->rect, m->mx, m->my, m->zid, m->zid2,
                       m->auf_partner);
            }
        } else if (!zone_hat_zeile(m->zid, m->page, -1, 1)) {
            ohne_traeger++;
            printf("     Marke %3d Blatt %2d rect 255 (%3d,%3d) zid %3d: kein Rechteck und "
                   "keine Schema-Zeichnung - die Marke hat keinen Traeger\n",
                   i, m->page, m->mx, m->my, m->zid);
        }
    }

    CHECK("es gibt ueberhaupt Marken zu pruefen (ABDECKUNG)", NM > 100);
    snprintf(t, sizeof t, "jede Marke: ihre Zone fuehrt eine Zeile auf dem Blatt der Marke "
             "- %d ohne", ohne_zeile);
    CHECK(t, ohne_zeile == 0);
    snprintf(t, sizeof t, "jede Marke mit Rechteck: es gehoert der eigenen oder der "
             "Partnerzone - %d fremd", fremdes_rect);
    CHECK(t, fremdes_rect == 0);
    snprintf(t, sizeof t, "jede Marke ohne Rechteck: ihre Zone fuehrt dort ein Schema "
             "- %d ohne Traeger", ohne_traeger);
    CHECK(t, ohne_traeger == 0);

    printf(g_fail ? "\nFEHLER\n" : "\nOK\n");
    return g_fail;
}
