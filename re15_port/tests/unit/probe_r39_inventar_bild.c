/* probe_r39_inventar_bild.c — AUSWERTER (kein add_test) fuer integration_r39_inventar_status (Runde 39 Spur C).
 *
 * Vergleicht den Ausweis-Block des Statusschirms in einem Framedump (PPM P6, RE15_FRAMEDUMP, Fensterskalierung 1 =
 * 320x240) mit den Texeln einer ITEM/STPIC_<k>.TIM (8bpp + CLUT (0,485), 58hw = 116 Spalten x 103 Zeilen). Der Block
 * steht im Original bei (14,26) (DAT_800b25f0/25f2, FUN_800460b8 @0x80049584-95d0; re15_inv_screen.c ID_BX/ID_BY).
 * Gezaehlt werden die Texel, deren RGB (5 Bit -> 8 Bit, <<3) im Dump bis auf +-12 je Kanal uebereinstimmen; Texel mit
 * CLUT-Wert 0x0000 (durchsichtig) zaehlen nicht.
 *
 * Aufruf: probe_r39_inventar_bild <dump.ppm> <STPIC.TIM> <x0> <y0> <zeilen> [min_prozent]
 * Ausgabe: "TREFFER n / GESAMT m (p%) bei (x0,y0) Zeilen z"; Rueckgabe 0 bei p >= min_prozent (Vorgabe 90), sonst 1
 * (2 = Lesefehler). Mit <zeilen> = 0 wird der ganze Block (103) verglichen.
 */
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

static int lade_ppm(const char *pfad, bild_t *b)
{
    FILE *f = fopen(pfad, "rb");
    if (!f) return -1;
    if (fgetc(f) != 'P' || fgetc(f) != '6') { fclose(f); return -1; }
    b->w = lies_zahl(f); b->h = lies_zahl(f);
    int maxv = lies_zahl(f);
    if (b->w <= 0 || b->h <= 0 || maxv != 255) { fclose(f); return -1; }
    size_t n = (size_t)b->w * (size_t)b->h * 3;
    b->px = (unsigned char *)malloc(n);
    if (!b->px || fread(b->px, 1, n, f) != n) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

static unsigned char *lade_datei(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    unsigned char *b = (unsigned char *)malloc((size_t)*n);
    if (!b || fread(b, 1, (size_t)*n, f) != (size_t)*n) { fclose(f); free(b); return NULL; }
    fclose(f);
    return b;
}

int main(int argc, char **argv)
{
    if (argc < 6) { fprintf(stderr, "Aufruf: %s <dump.ppm> <STPIC.TIM> <x0> <y0> <zeilen> [min_prozent]\n", argv[0]); return 2; }
    bild_t d; long tn = 0;
    if (lade_ppm(argv[1], &d) != 0) { printf("LESEFEHLER %s\n", argv[1]); return 2; }
    unsigned char *t = lade_datei(argv[2], &tn);
    if (!t || tn < 32 || t[0] != 0x10 || (t[4] & 0x0F) != 0x09) { printf("LESEFEHLER %s\n", argv[2]); return 2; }
    int x0 = atoi(argv[3]), y0 = atoi(argv[4]), zeilen = atoi(argv[5]);
    int min_prozent = argc > 6 ? atoi(argv[6]) : 90;
    unsigned long cl = (unsigned long)t[8] | ((unsigned long)t[9] << 8) | ((unsigned long)t[10] << 16);
    int cw = t[16] | t[17] << 8;
    const unsigned char *clut = t + 20;
    const unsigned char *pb = t + 8 + cl;
    int pw = pb[8] | pb[9] << 8, ph = pb[10] | pb[11] << 8;
    int breite = pw * 2;
    const unsigned char *tex = pb + 12;
    if (zeilen <= 0 || zeilen > ph) zeilen = ph;
    long treffer = 0, gesamt = 0;
    for (int y = 0; y < zeilen; y++)
        for (int x = 0; x < breite; x++) {
            int idx = tex[y * breite + x];
            if (idx >= cw) continue;
            unsigned c = clut[idx * 2] | (clut[idx * 2 + 1] << 8);
            if (c == 0) continue;                       /* durchsichtig */
            int r = (c & 31) << 3, g = ((c >> 5) & 31) << 3, b = ((c >> 10) & 31) << 3;
            int sx = x0 + x, sy = y0 + y;
            if (sx < 0 || sy < 0 || sx >= d.w || sy >= d.h) continue;
            const unsigned char *p = d.px + ((size_t)sy * (size_t)d.w + (size_t)sx) * 3;
            gesamt++;
            if (abs(p[0] - r) <= 12 && abs(p[1] - g) <= 12 && abs(p[2] - b) <= 12) treffer++;
        }
    int prozent = gesamt ? (int)(treffer * 100 / gesamt) : 0;
    printf("TREFFER %ld / GESAMT %ld (%d%%) bei (%d,%d) Zeilen %d\n", treffer, gesamt, prozent, x0, y0, zeilen);
    free(d.px); free(t);
    return prozent >= min_prozent ? 0 : 1;
}
