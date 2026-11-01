/* test_raumlicht.c — Lichttabelle nach dem 80%-Video (include/re15_raumlicht.h).
 * 50B0/50B1: drei feste rote Scheine (zugleich Gegenprobe: die Tabelle liefert noch Licht);
 * 2000/2001: NICHTS in keinem Bild (Runde 41 Spur W, analysis/befunde_runde41/W_wasserblink.md: die Glanzpunkte im Kanal
 * waren ausgedacht, das Original setzt dort keinen Effekt; Commit 4100d2aed lieferte in 3000 Bildern Licht);
 * alle anderen Raeume: nichts. */
#include <stdio.h>
#include "re15_raumlicht.h"

int main(void)
{
    int fails = 0;
    re15_raumlicht_t l[16];
    for (unsigned r = 0x50B0u; r <= 0x50B1u; r++) {
        int n = re15_raumlicht_liste(r, 123u, l, 16);
        if (n != 3 || l[0].r <= l[0].g) { fprintf(stderr, "FAIL: %04X n=%d\n", r, n); fails++; }
    }
    int an = 0;
    for (unsigned r = 0x2000u; r <= 0x2001u; r++)
        for (unsigned f = 0; f < 3000u; f++)
            if (re15_raumlicht_liste(r, f, l, 16) != 0) an++;
    if (an != 0) { fprintf(stderr, "FAIL: Kanal 2000/2001 hat in %d Bildern Licht\n", an); fails++; }
    if (re15_raumlicht_liste(0x1140u, 5u, l, 16) != 0) { fprintf(stderr, "FAIL: 1140 hat Licht\n"); fails++; }
    if (re15_raumlicht_liste(0x20B0u, 5u, l, 16) != 0) { fprintf(stderr, "FAIL: 20B0 hat Licht\n"); fails++; }
    fprintf(stderr, "test_raumlicht: 50B0 3 Scheine, Kanal 2000/2001 Licht in %d/6000 Bildern -> %s\n", an, fails ? "FEHLER" : "OK");
    return fails ? 1 : 0;
}
