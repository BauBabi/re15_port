/* Karte: JEDE Zone braucht ihr EIGENES Besucht-Bit.
 *
 * ⛔ WARUM ES DAS GIBT (Nutzer-Befund 2026-09-13): "bei ROOM 1000 werden gleich beide
 * kleinen Raeume - 1. geladen, 2. markiert. Das soll nicht so sein, sondern immer nur
 * der markiert und geladen werden zusaetzlich in den man auch drin ist."
 *
 * Die Bit-Vergabe in re15_map_zones.c gab jedem Basisraum ZWEI Bits: Zone 0 bekam 2n,
 * und jede WEITERE Zone teilte sich 2n+1. Bei zwei Zonen geht das auf - ab der dritten
 * fielen zwei verschiedene Orte auf dasselbe Bit, und wer einen betrat, bekam beide auf
 * die Karte. ROOM2070 trug die Kollision schon laenger; sichtbar wurde sie, als
 * ROOM1000 am selben Tag von zwei auf drei Zonen ging (die Trennwand zwischen den
 * beiden Toiletten ist gemessen, karte-1000-1050.md §1.2).
 *
 * Der Riegel prueft die EIGENSCHAFT, nicht die heutige Tabelle: zwei Zonen, die
 * verschiedene ORTE sind, duerfen nie dasselbe Bit tragen. Damit faellt er auch, wenn
 * spaeter irgendwo eine vierte Zone dazukommt, ohne dass jemand die Zusatzbit-Tabelle
 * ergaenzt.
 */
#include <stdio.h>
#include "re15_room.h"

extern int re15_map_zone_bit_test(int zonen_index);
extern int re15_map_zone_bit_kapazitaet(void);

static int g_fail;
#define CHECK(t, c) do { if (c) printf("  PASS: %s\n", t); \
                         else { printf("  FAIL: %s\n", t); g_fail = 1; } } while (0)

int main(void)
{
    int n = re15_map_zone_count();
    int kap = re15_map_zone_bit_kapazitaet();
    int i, j, kollisionen = 0, ohne_bit = 0, ueber_kapazitaet = 0, geprueft = 0;

    printf("=== Karte: ein Besucht-Bit je ORT ===\n");
    printf("  [Bestand] %d Zonen, Bitfeld fasst %d Bits\n", n, kap);

    for (i = 0; i < n; i++) {
        const re15_map_zone_t *a = re15_map_zone_by_index(i);
        int ba;
        if (!a) continue;
        ba = re15_map_zone_bit_test(i);
        if (ba < 0) {
            printf("  [kein Bit] ROOM%04X Zone %d - Raum nicht in re15_room_ids[]\n",
                   a->room, a->idx);
            ohne_bit++;
            continue;
        }
        if (ba >= kap) {
            printf("  [ausserhalb] ROOM%04X Zone %d -> Bit %d, Feld fasst nur %d\n",
                   a->room, a->idx, ba, kap);
            ueber_kapazitaet++;
            continue;
        }
        for (j = i + 1; j < n; j++) {
            const re15_map_zone_t *b = re15_map_zone_by_index(j);
            int bb;
            if (!b) continue;
            /* Derselbe ORT: gleiche Basis-Raumnummer (die ungerade Variante ist
             * dieselbe Stelle im anderen Szenario) UND dieselbe Zonen-Nummer. */
            if (((unsigned)a->room & ~1u) == ((unsigned)b->room & ~1u) &&
                a->idx == b->idx)
                continue;
            bb = re15_map_zone_bit_test(j);
            if (bb != ba) continue;
            printf("  [KOLLISION] ROOM%04X Zone %d und ROOM%04X Zone %d teilen Bit %d\n",
                   a->room, a->idx, b->room, b->idx, ba);
            kollisionen++;
            j = n;   /* je Zone hoechstens einmal melden */
        }
        geprueft++;
    }

    printf("  [Ergebnis] %d Zonen mit Bit geprueft, %d Kollisionen, %d ohne Bit, "
           "%d ausserhalb des Feldes\n", geprueft, kollisionen, ohne_bit,
           ueber_kapazitaet);
    CHECK("es gibt ueberhaupt Zonen mit Besucht-Bit", geprueft > 0);
    CHECK("keine zwei verschiedenen Orte teilen ein Besucht-Bit", kollisionen == 0);
    CHECK("kein Bit liegt ausserhalb des Feldes", ueber_kapazitaet == 0);

    if (g_fail) { printf("=== FAIL ===\n"); return 1; }
    printf("=== PASS ===\n");
    return 0;
}
