/* test_raumlicht.c — Lichttabelle nach dem 80%-Video (include/re15_raumlicht.h).
 * 50B0/50B1: drei feste rote Scheine; 2000/2001: Glanzpunkte blitzen einzeln und nur zeitweise auf;
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
    int an = 0, max = 0;
    for (unsigned f = 0; f < 3000u; f++) {
        int n = re15_raumlicht_liste(0x2000u, f, l, 16);
        if (n) an++;
        if (n > max) max = n;
    }
    if (an == 0 || an > 2400 || max > 3) { fprintf(stderr, "FAIL: Glanz an=%d max=%d\n", an, max); fails++; }
    if (re15_raumlicht_liste(0x1140u, 5u, l, 16) != 0) { fprintf(stderr, "FAIL: 1140 hat Licht\n"); fails++; }
    fprintf(stderr, "test_raumlicht: Glanz in %d/3000 Bildern, max %d gleichzeitig -> %s\n", an, max, fails ? "FEHLER" : "OK");
    return fails ? 1 : 0;
}
