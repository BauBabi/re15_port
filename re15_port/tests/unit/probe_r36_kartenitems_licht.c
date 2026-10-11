/* probe_r36_kartenitems_licht.c — AUSWERTER (kein add_test) fuer integration_r36_kartenitems_bild
 * (Runde 36 Spur N, Nachbesserung 1): die FARBE der gezeichneten Lageplan-Karte.
 *
 * Byte-true wird die Karte wie jedes Objekt mit Pool+0x0c-Bit 0x10 FLACH gezeichnet (FUN_8002c18c
 * @0x8002c2f4 -> FUN_80022f0c @0x8002c320, Farbe obj+0x70 = 0x808080 aus FUN_8002b898.c:16): Texel x
 * 0x80/0x80 = Texel unveraendert. Jeder Kartenpixel im Framedump ist dann GENAU eine Farbe der CLUT
 * der eingebackenen RE2-TIM karte01 (256 Eintraege, 5 Bit -> 8 Bit mit << 3 wie render_pc.c
 * rgb555_to_argb8888) — ohne Raumlicht, in jedem Cut dieselbe Menge. Mit NCCT-Raumlicht (1140: 41/128)
 * oder einer Tint-Konstante liegen die Pixel ausserhalb dieser Menge.
 *
 * Karte = die Pixel im Fenster, die sich zwischen den Laeufen MIT und OHNE Plan-Bit unterscheiden
 * (|dR|+|dG|+|dB| > 30; beide Laeufe sind bis auf das Bit gleich, Muster probe_r34n_e_bild).
 *
 * Aufruf: probe_r36_kartenitems_licht <mit.ppm> <ohne.ppm> <x0> <y0> <x1> <y1> <raum_hex> <min_px>
 * Ausgabe: "KARTE n Pixel | Huelle x..x y..y | Mittel r g b Summe s | PALETTE k von n"
 * Rueckgabe 0 wenn n >= min_px UND k == n (jeder Kartenpixel eine Palettenfarbe), 1 sonst,
 * 2 = Lesefehler/Argumente.
 */
#include "re15_kartenitems_r36.h"
#include "re15_tim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int w, h; unsigned char *px; } bild_t;

static int lies_zahl(FILE *f)
{
    int c, v = 0, hat = 0;
    for (;;) {
        c = fgetc(f);
        if (c == '#') { while (c != '\n' && c != EOF) c = fgetc(f); continue; }
        if (c == EOF) return -1;
        if (c >= '0' && c <= '9') { v = v * 10 + (c - '0'); hat = 1; }
        else if (hat) return v;
    }
}

static int lade(const char *pfad, bild_t *b)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return -1;
    if (fgetc(f) != 'P' || fgetc(f) != '6') { fclose(f); return -1; }
    b->w = lies_zahl(f); b->h = lies_zahl(f);
    int mx = lies_zahl(f);
    if (b->w <= 0 || b->h <= 0 || mx != 255) { fclose(f); return -1; }
    size_t n = (size_t)b->w * (size_t)b->h * 3;
    b->px = (unsigned char *)malloc(n);
    if (!b->px || fread(b->px, 1, n, f) != n) { fclose(f); free(b->px); return -1; }
    fclose(f);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 9) {
        printf("Aufruf: %s <mit.ppm> <ohne.ppm> <x0> <y0> <x1> <y1> <raum_hex> <min_px>\n", argv[0]);
        return 2;
    }
    bild_t a, b;
    if (lade(argv[1], &a) || lade(argv[2], &b) || a.w != b.w || a.h != b.h) {
        printf("Lesefehler oder Groesse verschieden\n"); return 2;
    }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), x1 = atoi(argv[5]), y1 = atoi(argv[6]);
    unsigned raum = (unsigned)strtoul(argv[7], NULL, 16);
    int mn = atoi(argv[8]);

    /* Palette der eingebackenen Karten-TIM, erweitert wie der Zeichner (5 Bit << 3). */
    int tsz = 0;
    const unsigned char *tb = re15_kartenitems_r36_tim_bytes((uint16_t)raum, &tsz);
    re15_tim_t tim;
    if (!tb || re15_tim_parse(tb, tsz, &tim) != 0 || !tim.has_clut || !tim.clut || tim.clut_entries <= 0) {
        printf("TIM des Raums %04X nicht lesbar\n", raum); return 2;
    }
    static unsigned char pal[65536 / 8];   /* Bitmenge ueber RGB888 >> 3 je Kanal (15 Bit) */
    memset(pal, 0, sizeof pal);
    for (int i = 0; i < tim.clut_entries; i++) {
        uint16_t c = tim.clut[i];
        if ((c & 0x7fff) == 0) continue;            /* RGB-schwarz = transparent, wird nie gezeichnet */
        pal[(c & 0x7fff) >> 3] |= (unsigned char)(1u << ((c & 0x7fff) & 7));
    }

    long n = 0, sr = 0, sg = 0, sb = 0, k = 0;
    int hx0 = 1 << 30, hy0 = 1 << 30, hx1 = -1, hy1 = -1;
    for (int y = y0; y <= y1 && y < a.h; y++)
        for (int x = x0; x <= x1 && x < a.w; x++) {
            if (x < 0 || y < 0) continue;
            const unsigned char *p = a.px + (y * a.w + x) * 3, *q = b.px + (y * b.w + x) * 3;
            int d = abs(p[0] - q[0]) + abs(p[1] - q[1]) + abs(p[2] - q[2]);
            if (d <= 30) continue;
            n++; sr += p[0]; sg += p[1]; sb += p[2];
            if (x < hx0) hx0 = x; if (x > hx1) hx1 = x; if (y < hy0) hy0 = y; if (y > hy1) hy1 = y;
            /* Palettenfarbe? Nur exakt << 3 erweiterte Werte kommen in Frage. */
            if ((p[0] & 7) == 0 && (p[1] & 7) == 0 && (p[2] & 7) == 0) {
                unsigned c = (unsigned)(p[0] >> 3) | ((unsigned)(p[1] >> 3) << 5) | ((unsigned)(p[2] >> 3) << 10);
                if (pal[c >> 3] & (1u << (c & 7))) k++;
            }
        }
    if (n > 0)
        printf("KARTE %ld Pixel | Huelle x%d..%d y%d..%d | Mittel %ld %ld %ld Summe %ld | PALETTE %ld von %ld\n",
               n, hx0, hx1, hy0, hy1, sr / n, sg / n, sb / n, (sr + sg + sb) / n, k, n);
    else
        printf("KARTE 0 Pixel im Fenster x%d..%d y%d..%d\n", x0, x1, y0, y1);
    return (n >= mn && k == n) ? 0 : 1;
}
