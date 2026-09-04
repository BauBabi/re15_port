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
 * ⛔ GEGENPROBE, unfreiwillig gefahren: derselbe Test lief am 2026-09-04 versehentlich
 * gegen eine VERALTETE libre15_engine.a (zwei gleichzeitige Builds im selben build/) -
 * also gegen genau den Stand VOR dem Fix. Ausgabe damals:
 *     [Abdeckung] ... 290 auf der Seite der Zone, 290 Abweichungen
 *     [3F  ] ROOM1170 Zone 1 Band 0 -> Blatt 4 Rect 255 (Zone: Blatt 4 Rect 3)
 *     FAIL: ROOM1170s unterer Bereich kann auf 3F rot werden
 * Der Waechter schlaegt also nachweislich aus, wenn der Fehler da ist.
 *
 * GEPRUEFT WIRD die Uebereinstimmung selbst, nicht ein Einzelfall: fuer jede Zone und
 * jedes Band muss gelten - liefert die Etagen-Tabelle die Seite dieser Zone, dann muss
 * sie auch ihr Rechteck liefern. Zusaetzlich stehen die zwei gemeldeten Faelle
 * namentlich als Pin, damit sie nicht still aus der Menge fallen.
 */
#include <stdio.h>
#include "re15_room.h"
#include "re15_collision.h"

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

    /* ⛔ DAS EIGENTLICHE VERSPRECHEN IST NICHT "die Tabellen stimmen ueberein",
     * SONDERN "das Rechteck wird ROT". Die Uebereinstimmung oben ist nur die
     * Vorbedingung; der Nutzer sieht die Hervorhebung. Deshalb hier die echte Funktion,
     * die der Zeichner fragt - mit gesetztem Band und gesetzter aktueller Zone, so wie
     * im Spiel (Memory reai-v2-absicht-statt-ergebnis: die Abnahme gehoert ans
     * ERGEBNIS, nicht an die Absicht). */
    {
        const int32_t px = -18842, pz = -22955;   /* Mitte von ROOM1170 Zone 1 */
        int st3f, stroof, st_fremd;
        re15_map_visited_reset();
        for (i = 0; i < 13; i++) re15_map_debug_reveal_page((unsigned)i);

        re15_collision_set_band(0);                 /* unten -> 3F-Blatt */
        re15_map_zone_update(0x1170, px, pz);
        st3f = re15_map_rect_state(4, 3);
        printf("  [Live] Band 0 in ROOM1170 Zone 1: Blatt 4 Rect 3 -> Zustand %d (%d = aktuell)\n",
               st3f, RE15_MAP_RECT_CURRENT);
        CHECK("3F: das Rechteck des Spielers wird als AKTUELL gemeldet",
              st3f == RE15_MAP_RECT_CURRENT);

        re15_collision_set_band(4);                 /* oben -> ROOF-Blatt */
        re15_map_zone_update(0x1170, px, pz);
        stroof = re15_map_rect_state(5, 0);
        printf("  [Live] Band 4 in ROOM1170 Zone 1: Blatt 5 Rect 0 -> Zustand %d\n",
               stroof);
        CHECK("ROOF: das Rechteck des Spielers wird als AKTUELL gemeldet",
              stroof == RE15_MAP_RECT_CURRENT);

        /* ⛔ GEGENPROBE: sonst waere die Bedingung auch erfuellt, wenn ALLES rot ist.
         * Auf einer Etagen-Umschaltung darf nur das Rechteck DIESER Etage aktuell sein. */
        st_fremd = re15_map_rect_state(4, 3);
        printf("  [Gegenprobe] Band 4: Blatt 4 Rect 3 -> Zustand %d (nicht %d)\n",
               st_fremd, RE15_MAP_RECT_CURRENT);
        CHECK("die ANDERE Etage desselben Ortes ist dabei NICHT aktuell",
              st_fremd != RE15_MAP_RECT_CURRENT);
        re15_collision_reset_band();
    }

    printf(g_fail ? "FAIL\n" : "OK\n");
    return g_fail;
}
