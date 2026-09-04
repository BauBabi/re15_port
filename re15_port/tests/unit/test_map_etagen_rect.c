/* Karte: die Etagen-Tabelle nennt DASSELBE Rechteck wie die Zonen-Tabelle.
 *
 * ⛔ NUTZER-BEFUND 2026-09-04, zweimal derselbe Satz (fehler/error2.png = ROOF,
 * fehler/error3.png = 3F): "does not turn red when i am in the room".
 * Der Spielermarker stand sichtbar IN dem Rechteck, das Rechteck blieb gruen.
 *
 * URSACHE: re15_map_rect_state (re15_map_zones.c:312-320) entscheidet die rote
 * Hervorhebung eines stockwerkuebergreifenden Raums so:
 *      hat_etage = re15_map_floor_lookup(zn->room, zn->idx, band, &fp, &fr);
 *      if (hat_etage) { if (page == fp && rect_idx == fr) return CURRENT; }
 * Die Etagen-Tabelle trug pauschal rect = 255 (ein Ueberbleibsel aus der Zeit, als
 * diese Zweitzeichnungen SCHEMA-Zeichnungen waren). Fuer ein GEMALTES Rechteck
 * (Index 0..13) kann `rect_idx == 255` nie zutreffen - kein einziger der 22
 * Etagen-Eintraege konnte je rot werden. Betroffen waren ROOM1060, ROOM1080,
 * ROOM10A0, ROOM10F0, ROOM1170, ROOM11A0, ROOM3080, ROOM4020, ROOM4070, ROOM50D0.
 *
 * ⛔ WARUM DER MARKER TROTZDEM RICHTIG STAND - und warum das den Fehler VERSTECKT hat:
 * der Marker holt sein Rechteck ueber re15_map_zone_fuer aus der ZONE
 * (re15_inv_screen.c:655) und ueberschreibt fr. Er war also nie betroffen. Genau
 * dieser Widerspruch - Marker drin, Rechteck gruen - stand im Screenshot.
 *
 * GEPRUEFT WIRD die Uebereinstimmung selbst, nicht ein Einzelfall: fuer jede Zone und
 * jedes Band muss gelten - liefert die Etagen-Tabelle die Seite dieser Zone, dann muss
 * sie auch ihr Rechteck liefern. Zusaetzlich stehen die zwei gemeldeten Faelle
 * namentlich als Pin, damit sie nicht still aus der Menge fallen.
 */
#include <stdio.h>
#include "re15_room.h"

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    int i, band, nz, geprueft = 0, treffer = 0, abweichung = 0;

    printf("=== Karte: Etagen-Tabelle und Zonen-Tabelle nennen dasselbe Rechteck ===\n");

    nz = re15_map_zone_count();
    for (i = 0; i < nz; i++) {
        const re15_map_zone_t *z = re15_map_zone_by_index(i);
        if (!z) continue;
        for (band = 0; band < 16; band++) {
            int fp = -1, fr = -1;
            if (!re15_map_floor_lookup(z->room, z->idx, band, &fp, &fr)) continue;
            geprueft++;
            if (fp != (int)z->page) continue;      /* andere Etage - sagt hier nichts */
            treffer++;
            if (fr != (int)z->rect) {
                abweichung++;
                printf("  [Abweichung] ROOM%04X Zone %d Band %2d: Etagen-Tabelle sagt "
                       "Blatt %d Rect %d, die Zone steht auf Blatt %d Rect %d\n",
                       z->room, z->idx, band, fp, fr, z->page, z->rect);
            }
        }
    }

    printf("  [Abdeckung] %d Zonen, %d Etagen-Zeilen gefunden, davon %d auf der Seite "
           "der Zone (nur die sind vergleichbar), %d Abweichungen\n",
           nz, geprueft, treffer, abweichung);

    /* ⛔ DIE MENGE DARF NICHT LEER LAUFEN: ohne das waere der Test still gruen,
     * sobald die Etagen-Tabelle leer ist oder die Schluessel nicht mehr passen. */
    CHECK("es gibt ueberhaupt vergleichbare Etagen-Zeilen (>= 10)", treffer >= 10);
    CHECK("KEINE Etagen-Zeile nennt ein anderes Rechteck als ihre Zone",
          abweichung == 0);

    /* Die zwei gemeldeten Faelle namentlich - ROOM1170s zweiter Bereich. */
    {
        int fp = -1, fr = -1;
        const re15_map_zone_t *z4 = re15_map_zone_fuer(0x1170, 1, 4);
        const re15_map_zone_t *z5 = re15_map_zone_fuer(0x1170, 1, 5);
        if (re15_map_floor_lookup(0x1170, 1, 0, &fp, &fr)) {
            printf("  [3F  ] ROOM1170 Zone 1 Band 0 -> Blatt %d Rect %d "
                   "(Zone: Blatt %d Rect %d)\n", fp, fr,
                   z4 ? z4->page : -1, z4 ? z4->rect : -1);
            CHECK("ROOM1170s unterer Bereich kann auf 3F rot werden",
                  z4 && fp == (int)z4->page && fr == (int)z4->rect && fr != 255);
        } else { printf("  FAIL: keine Etagen-Zeile fuer ROOM1170 Zone 1 Band 0\n");
                 g_fail = 1; }
        if (re15_map_floor_lookup(0x1170, 1, 4, &fp, &fr)) {
            printf("  [ROOF] ROOM1170 Zone 1 Band 4 -> Blatt %d Rect %d "
                   "(Zone: Blatt %d Rect %d)\n", fp, fr,
                   z5 ? z5->page : -1, z5 ? z5->rect : -1);
            CHECK("ROOM1170s unterer Bereich kann auf dem ROOF-Blatt rot werden",
                  z5 && fp == (int)z5->page && fr == (int)z5->rect && fr != 255);
        } else { printf("  FAIL: keine Etagen-Zeile fuer ROOM1170 Zone 1 Band 4\n");
                 g_fail = 1; }
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
