/* Pruefer UMGEHUNG R4-1, H6f: SHA-256 von asset_abgleich.c (ausgelieferte Datei, UNVERAENDERT mituebersetzt)
 * gegen einen unabhaengigen Rechner (Python hashlib, u1_sha_vergleich.py).
 *   modus "zufall <seed> <n>": n Nachrichten, Laenge 0..(1<<20)+300 (gewichtet auf kleine Laengen und
 *        Blockgrenzen), Inhalt xorshift, je Nachricht zufaellige Stueckelung (1..4097 B, auch 0-B-Aufrufe);
 *        Ausgabe je Zeile "<laenge> <seed_nachricht> <hex>"
 *   modus "strom <bytes> <muster_bytes>": <bytes> Bytes eines wiederholten Musters (Byte i = (i*131+7)&255 der
 *        Musterlaenge) in 1-MiB-Stuecken durch re15_sha256_dazu -> "<bytes> <hex>" (Laengen > 4 GiB: 64-Bit-Zaehler)
 *   modus "datei <pfad>": re15_sha256_datei -> "<rc> <groesse> <hex>"
 */
#include "asset_abgleich.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint64_t s_x;
static uint64_t xs(void) { s_x ^= s_x << 13; s_x ^= s_x >> 7; s_x ^= s_x << 17; return s_x; }

static void fuellen(unsigned char *p, size_t n, uint64_t seed)
{
    uint64_t x = seed ? seed : 0x9e3779b97f4a7c15ull;
    for (size_t i = 0; i < n; i++) { x ^= x << 13; x ^= x >> 7; x ^= x << 17; p[i] = (unsigned char)(x >> 24); }
}

int main(int argc, char **argv)
{
    if (argc >= 4 && strcmp(argv[1], "zufall") == 0) {
        s_x = strtoull(argv[2], NULL, 10) | 1u;
        long n = atol(argv[3]);
        size_t maxlen = (1u << 20) + 300;
        unsigned char *buf = (unsigned char *)malloc(maxlen);
        if (!buf) return 3;
        for (long k = 0; k < n; k++) {
            size_t len;
            uint64_t r = xs() % 10;
            if (r < 4) len = (size_t)(xs() % 300);                         /* klein, alle Auffuellgrenzen */
            else if (r < 7) len = (size_t)(64 * (xs() % 40) + (xs() % 3) + 53); /* um 55/56/63/64 je Block */
            else len = (size_t)(xs() % maxlen);
            uint64_t seed = xs();
            fuellen(buf, len, seed);
            re15_sha256_t c;
            re15_sha256_start(&c);
            size_t o = 0;
            while (o < len) {
                size_t st = (size_t)(xs() % 4098);                         /* 0..4097, 0 = leerer Aufruf */
                if (st > len - o) st = len - o;
                re15_sha256_dazu(&c, buf + o, st);
                o += st;
                if (xs() % 17 == 0) re15_sha256_dazu(&c, buf, 0);
            }
            char hex[65];
            re15_sha256_ende(&c, hex);
            printf("%zu %llu %s\n", len, (unsigned long long)seed, hex);
        }
        free(buf);
        return 0;
    }
    if (argc >= 4 && strcmp(argv[1], "strom") == 0) {
        unsigned long long gesamt = strtoull(argv[2], NULL, 10);
        size_t muster = (size_t)strtoull(argv[3], NULL, 10);
        size_t stueck = 1u << 20;
        if (muster == 0 || stueck % muster) return 4;                     /* Stueck = ganze Zahl Muster */
        unsigned char *buf = (unsigned char *)malloc(stueck);
        if (!buf) return 3;
        for (size_t i = 0; i < stueck; i++) buf[i] = (unsigned char)(((i % muster) * 131u + 7u) & 255u);
        re15_sha256_t c;
        re15_sha256_start(&c);
        unsigned long long o = 0;
        while (o < gesamt) {
            size_t k = (gesamt - o < stueck) ? (size_t)(gesamt - o) : stueck;
            re15_sha256_dazu(&c, buf, k);
            o += k;
        }
        char hex[65];
        re15_sha256_ende(&c, hex);
        printf("%llu %s\n", gesamt, hex);
        free(buf);
        return 0;
    }
    if (argc >= 3 && strcmp(argv[1], "datei") == 0) {
        char hex[65] = "-";
        long long g = -1;
        int rc = re15_sha256_datei(argv[2], hex, &g);
        printf("%d %lld %s\n", rc, g, hex);
        return 0;
    }
    fprintf(stderr, "Aufruf: zufall <seed> <n> | strom <bytes> <muster> | datei <pfad>\n");
    return 2;
}
