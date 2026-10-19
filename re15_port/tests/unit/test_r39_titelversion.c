/*
 * Runde 39 Spur T "titelversion" — Riegel. Dossier analysis/befunde_runde39/T_titelversion.md.
 * Nutzer (Nachtrag 14): "bei dieser und zukuenftig neuen Versionen passe mir bitte die Version im Titelscreen an.
 * Entferne das bestehende."
 *
 * Aufruf: test_r39_titelversion <teil>
 *   glyphen  Glyphen-Tabelle: Ausschnitte bytegleich mit DATA/TITLEU.TIM an der dokumentierten Stelle; alle Glyphen
 *            (auch die ergaenzten) im Raster der alten Zeile (eigenes Flaechenmodell: Raster 1,12 px, Quadrat 1,52 px,
 *            oberste Schrift-Zeile y 24,88, Wert = round(25 x Deckung)) mit derselben Abweichung wie die Ausschnitte
 *   text     Anzeigetext aus der Version ("v0.8.27" -> "Ver.0.8.27", Zusatz -> " dev")
 *   layout   Layout: die alte Zeile "Ver.2025.01.25" landet mit den Konstanten wieder an ihren Original-Spalten;
 *            neue Zeilen rechtsbuendig an 283,60 px, Abstaende 0,72 / 2,74 px, nichts links vom Kasten
 *   maske    Maske auf TITLEU.TIM: 929 Pixel, bbox x198..283 y11..33, Ring 608 x 0x8000, Kunst-Staub bleibt;
 *            kuenstliche Faelle (Rand, farbig, ohne Kern) bleiben
 *   anwenden ganzes Umschreiben: ausserhalb Maske + neuer Kacheln bytegleich, Maske -> 0x8000, neue Glyphen exakt,
 *            TITLEU.TIM auf der Platte unveraendert
 *   version  die einkompilierte Version ist die von CMake bestimmte (RE39T_ERWARTET)
 *   pruefen <titel.bmp>   (Integrations-Riegel) Titelabzug der echten exe gegen TITLEU.TIM + Engine-Erwartung
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "re15_titelversion_r39.h"

#ifndef RE15_ASSET_PSX_DIR
#error "RE15_ASSET_PSX_DIR fehlt"
#endif
#ifndef RE39T_ERWARTET
#define RE39T_ERWARTET ""
#endif

static int g_fehler = 0, g_n = 0;
#define PRUEF(bed, ...) do { g_n++; if (bed) { printf("  ok   "); } else { g_fehler++; printf("  FEHL "); } \
                             printf(__VA_ARGS__); printf("\n"); } while (0)

/* ---------------------------------------------------------------------------------------------------------------- */
static uint8_t *g_datei = NULL;
static long g_datei_n = 0;

static uint16_t *tim_laden(void)
{
    char pfad[1024];
    FILE *f;
    uint16_t *px;
    snprintf(pfad, sizeof pfad, "%s/DATA/TITLEU.TIM", RE15_ASSET_PSX_DIR);
    f = fopen(pfad, "rb");
    if (!f) { printf("  FEHL %s nicht lesbar\n", pfad); exit(1); }
    fseek(f, 0, SEEK_END); g_datei_n = ftell(f); fseek(f, 0, SEEK_SET);
    g_datei = (uint8_t *) malloc((size_t) g_datei_n);
    if (!g_datei || fread(g_datei, 1, (size_t) g_datei_n, f) != (size_t) g_datei_n) { printf("  FEHL Lesen\n"); exit(1); }
    fclose(f);
    /* Kopf: Magic 0x10, Flag 2 (16 bpp ohne CLUT), 320 x 240 ab Datei-Byte 0x14 */
    if (g_datei_n != 153620 || g_datei[0] != 0x10 || g_datei[4] != 2 || g_datei[0x10] != 0x40 || g_datei[0x11] != 0x01
        || g_datei[0x12] != 0xF0) { printf("  FEHL TITLEU.TIM: unerwarteter Kopf/Groesse %ld\n", g_datei_n); exit(1); }
    px = (uint16_t *) malloc(320 * 240 * 2);
    for (int i = 0; i < 320 * 240; i++) px[i] = (uint16_t) (g_datei[0x14 + 2 * i] | (g_datei[0x15 + 2 * i] << 8));
    return px;
}

static int grau(uint16_t c, int *v)
{
    int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
    if (v) *v = r;
    return r == g && g == b;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Eigenes Flaechenmodell (unabhaengig vom Generator): Positionen in 1/25 px, 25x25 Teilpunkte je Pixel. */
static int modell(const char *const *bm, int zeilen, int phase25, int c, int r)
{
    int spalten = (int) strlen(bm[0]), n = 0;
    for (int sy = 0; sy < 25; sy++) {
        int y = r * 25 + sy;
        for (int sx = 0; sx < 25; sx++) {
            int x = c * 25 + sx, hit = 0;
            for (int j = 0; j < zeilen && !hit; j++) {
                int y0 = 622 + 28 * j;               /* 24,88 px + j x 1,12 px */
                if (y < y0 || y >= y0 + 38) continue; /* Quadrat 1,52 px */
                for (int i = 0; i < spalten; i++)
                    if (bm[j][i] == '#' && x >= phase25 + 28 * i && x < phase25 + 28 * i + 38) { hit = 1; break; }
            }
            n += hit;
        }
    }
    return (n * 25 + 312) / 625;
}

typedef struct { char ch; int x0; int phase25; const char *bm[7]; } quelle_t;
/* Ausschnitte: Stelle in TITLEU.TIM (erste Kachelspalte) und angepasstes Bitmuster/Phase (Dossier 1.2,
 * tools/r39_titelversion/messen.py) */
static const quelle_t k_aus[] = {
    { 'V', 198, 20, { "###.###", ".#...#.", ".#...#.", ".#..#..", ".#.#...", ".##....", ".#....." } },
    { 'e', 207, 21, { ".....", ".....", ".###.", "#...#", "#####", "#....", ".####" } },
    { 'r', 214, 15, { ".....", ".....", "#.##.", "##..#", "#....", "#....", "#...." } },
    { '.', 221,  8, { "..", "..", "..", "..", "..", "##", "##" } },
    { '0', 233, 15, { ".###.", "#...#", "#..##", "#.#.#", "##..#", "#...#", ".###." } },
    { '1', 264,  0, { ".#", "##", ".#", ".#", ".#", ".#", ".#" } },
    { '2', 226, 21, { ".###.", "#...#", "....#", "...#.", "..#..", ".#...", "#####" } },
    { '5', 277, 15, { "#####", "#....", "####.", "#...#", "....#", "#...#", ".###." } },
};
/* Ergaenzte Glyphen: Form PORT-WAHL (HD44780 A00), Phase .60 wie 0/5/r */
static const quelle_t k_neu[] = {
    { '3', 0, 15, { "#####", "...#.", "..#..", "...#.", "....#", "#...#", ".###." } },
    { '4', 0, 15, { "...#.", "..##.", ".#.#.", "#..#.", "#####", "...#.", "...#." } },
    { '6', 0, 15, { "..##.", ".#...", "#....", "####.", "#...#", "#...#", ".###." } },
    { '7', 0, 15, { "#####", "....#", "...#.", "..#..", ".#...", ".#...", ".#..." } },
    { '8', 0, 15, { ".###.", "#...#", "#...#", ".###.", "#...#", "#...#", ".###." } },
    { '9', 0, 15, { ".###.", "#...#", "#...#", ".####", "....#", "...#.", ".##.." } },
    { 'd', 0, 15, { "....#", "....#", ".##.#", "#..##", "#...#", "#...#", ".####" } },
    { 'v', 0, 15, { ".....", ".....", "#...#", "#...#", "#...#", ".#.#.", "..#.." } },
};
#define N_AUS ((int) (sizeof k_aus / sizeof k_aus[0]))
#define N_NEU ((int) (sizeof k_neu / sizeof k_neu[0]))

static int teil_glyphen(void)
{
    uint16_t *px = tim_laden();
    int max_aus = 0;
    printf("== Glyphen-Tabelle ==\n");
    PRUEF(re15_titelversion_r39_glyphen_zahl() == N_AUS + N_NEU, "16 Glyphen (ist %d)", re15_titelversion_r39_glyphen_zahl());
    for (int k = 0; k < N_AUS; k++) {
        const re15_r39t_glyphe_t *g = re15_titelversion_r39_glyphe(k_aus[k].ch);
        int abw = 0, fremd_ok = 1, modell_max = 0;
        if (!g) { PRUEF(0, "'%c' fehlt", k_aus[k].ch); continue; }
        PRUEF(g->quelle == RE15_R39T_AUSGESCHNITTEN, "'%c' als AUSGESCHNITTEN markiert", g->zeichen);
        for (int r = 0; r < RE15_R39T_ZEILEN; r++) {
            for (int c = 0; c < g->breite; c++) {
                int x = k_aus[k].x0 + c, y = RE15_R39T_ZEILE0 + r, v, t = g->werte[r * g->breite + c], m;
                uint16_t q = px[y * 320 + x];
                /* Spalte 207: Zeilen 24..26 gehoeren zum V, 27..33 zum e (Dossier 2.1) */
                int fremd = (x == 207) && ((k_aus[k].ch == 'V' && y >= 27) || (k_aus[k].ch == 'e' && y <= 26));
                if (fremd) { if (t != 0) fremd_ok = 0; continue; }
                if (!grau(q, &v) || v != t) abw++;
                m = modell(k_aus[k].bm, 7, k_aus[k].phase25, c, y);
                if (abs(m - t) > modell_max) modell_max = abs(m - t);
            }
        }
        if (modell_max > max_aus) max_aus = modell_max;
        PRUEF(abw == 0 && fremd_ok, "'%c' bytegleich mit TITLEU.TIM x%d..%d y24..33 (Abweichungen %d)", g->zeichen,
              k_aus[k].x0, k_aus[k].x0 + g->breite - 1, abw);
        PRUEF(modell_max <= 7, "'%c' im Raster: Flaechenmodell weicht hoechstens %d Stufen ab (Grenze 7)", g->zeichen,
              modell_max);
    }
    for (int k = 0; k < N_NEU; k++) {
        const re15_r39t_glyphe_t *g = re15_titelversion_r39_glyphe(k_neu[k].ch);
        int modell_max = 0, maxv = 0, halo = 0, voll = 0;
        if (!g) { PRUEF(0, "'%c' fehlt", k_neu[k].ch); continue; }
        PRUEF(g->quelle == RE15_R39T_ERGAENZT && g->breite == 7 && g->tinte_l == 15 && g->tinte_r == 15 + 4 * 28 + 38,
              "'%c' ERGAENZT, 7 Spalten, Tinte [0,60; 6,60) wie 0/5 (ist %d/%u/%u..%u)", g->zeichen, g->quelle,
              g->breite, g->tinte_l, g->tinte_r);
        for (int r = 0; r < RE15_R39T_ZEILEN; r++)
            for (int c = 0; c < g->breite; c++) {
                int t = g->werte[r * g->breite + c], m = modell(k_neu[k].bm, 7, k_neu[k].phase25, c, RE15_R39T_ZEILE0 + r);
                if (abs(m - t) > modell_max) modell_max = abs(m - t);
                if (t > maxv) maxv = t;
                if ((r == 0 || r == RE15_R39T_ZEILEN - 1) && t > halo) halo = t;
                if (t == 25) voll++;
            }
        PRUEF(modell_max <= max_aus, "'%c' im selben Raster: Modellabweichung %d <= %d (der Ausschnitte)", g->zeichen,
              modell_max, max_aus);
        PRUEF(maxv == 25 && voll >= 2 && halo <= 4, "'%c' Schattierung wie die Ausschnitte: Kern 25 (%d Pixel), "
              "Halo-Zeilen 24/33 <= 4 (ist %d)", g->zeichen, voll, halo);
    }
    /* die Halo-Zeilen der Ausschnitte (Ziffern) zum Vergleich */
    {
        int halo = 0;
        const char *z = "0125";
        for (int k = 0; z[k]; k++) {
            const re15_r39t_glyphe_t *g = re15_titelversion_r39_glyphe(z[k]);
            for (int c = 0; c < g->breite; c++) {
                if (g->werte[c] > halo) halo = g->werte[c];
                if (g->werte[(RE15_R39T_ZEILEN - 1) * g->breite + c] > halo) halo = g->werte[(RE15_R39T_ZEILEN - 1) * g->breite + c];
            }
        }
        PRUEF(halo <= 4, "Ausschnitte 0/1/2/5: Halo-Zeilen 24/33 <= 4 (ist %d)", halo);
    }
    free(px);
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
static int teil_text(void)
{
    static const struct { const char *ein, *aus; } f[] = {
        { "v0.8.27", "Ver.0.8.27" }, { "v0.8.26 dev", "Ver.0.8.26 dev" }, { "v0.8.26-5-g832b68bd", "Ver.0.8.26 dev" },
        { "0.9.0", "Ver.0.9.0" }, { "v9.9.9", "Ver.9.9.9" }, { "v1.2.", "Ver.1.2 dev" }, { "dev", "Ver.dev" },
        { "", "Ver.dev" }, { "v10.20.30", "Ver.10.20.30" },
    };
    char out[64];
    printf("== Anzeigetext ==\n");
    for (unsigned i = 0; i < sizeof f / sizeof f[0]; i++) {
        int n = re15_titelversion_r39_text(f[i].ein, out, (int) sizeof out);
        PRUEF(n == (int) strlen(f[i].aus) && strcmp(out, f[i].aus) == 0, "\"%s\" -> \"%s\" (ist \"%s\")", f[i].ein,
              f[i].aus, out);
    }
    PRUEF(re15_titelversion_r39_text(NULL, out, (int) sizeof out) > 0 && strcmp(out, "Ver.dev") == 0, "NULL -> \"Ver.dev\"");
    PRUEF(re15_titelversion_r39_text("v0.8.27", out, 6) == -1, "zu kleiner Puffer -> -1");
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
static int teil_layout(void)
{
    char z[RE15_R39T_TEXT_MAX];
    int s[RE15_R39T_TEXT_MAX], n;
    /* Original-Spalten der Zeichen in TITLEU.TIM (Kachelanfang der Ausschnitte bzw. derselben Glyphe an anderer Stelle,
     * Dossier 1.2: 2 bei x226/240/270, 0 bei x233/257, 5 bei x247/277, Punkt bei x221/253/267, 1 bei x264) */
    static const int orig[] = { 198, 207, 214, 221, 226, 233, 240, 247, 253, 257, 264, 267, 270, 277 };
    printf("== Layout ==\n");
    n = re15_titelversion_r39_layout("Ver.2025.01.25", z, s, RE15_R39T_TEXT_MAX);
    PRUEF(n == 14, "alte Zeile: 14 Kacheln (ist %d)", n);
    if (n == 14) {
        int max = 0;
        for (int i = 0; i < 14; i++) if (abs(s[i] - orig[i]) > max) max = abs(s[i] - orig[i]);
        PRUEF(s[13] == 277 && max <= 1, "alte Zeile mit den Konstanten: letzte '5' bei x277 (ist %d), alle Zeichen "
              "hoechstens 1 Spalte neben dem Original (max %d)", s[13], max);
        printf("       Spalten:");
        for (int i = 0; i < 14; i++) printf(" %c%d", z[i], s[i]);
        printf("\n");
    }
    {
        static const char *const neu[] = { "Ver.0.8.27", "Ver.0.8.26 dev", "Ver.9.9.9", "Ver.10.20.30 dev" };
        for (unsigned t = 0; t < sizeof neu / sizeof neu[0]; t++) {
            int ok = 1, wort = 0;
            long letzte_r;
            n = re15_titelversion_r39_layout(neu[t], z, s, RE15_R39T_TEXT_MAX);
            if (n <= 0) { PRUEF(0, "\"%s\": Layout %d", neu[t], n); continue; }
            letzte_r = (long) s[n - 1] * 25 + re15_titelversion_r39_glyphe(z[n - 1])->tinte_r;
            for (int i = 1; i < n; i++) {
                const re15_r39t_glyphe_t *a = re15_titelversion_r39_glyphe(z[i - 1]), *b = re15_titelversion_r39_glyphe(z[i]);
                long gap = ((long) s[i] * 25 + b->tinte_l) - ((long) s[i - 1] * 25 + a->tinte_r);
                int soll = (i == 4) ? RE15_R39T_WORT25 : RE15_R39T_ABSTAND25;
                if (strchr(neu[t], ' ') && z[i] == 'd') soll = RE15_R39T_WORT25, wort = 1;
                if (gap < soll - 12 || gap > soll + 12) { ok = 0; printf("       %c->%c Abstand %ld/25 (Soll %d)\n", a->zeichen, b->zeichen, gap, soll); }
            }
            PRUEF(n == (int) strlen(neu[t]) - (wort ? 1 : 0) && ok && labs(letzte_r - RE15_R39T_RECHTS25) <= 12,
                  "\"%s\": %d Kacheln, rechts an 283,60 (Tinte %ld/25), Abstaende 0,72/2,74 px +-0,48", neu[t], n, letzte_r);
            if (t < 3) PRUEF(s[0] >= RE15_R39T_LINKS, "\"%s\": links ab x%d (>= %d)", neu[t], s[0], RE15_R39T_LINKS);
        }
    }
    PRUEF(re15_titelversion_r39_layout("Ver.0.8.27x", z, s, RE15_R39T_TEXT_MAX) == -1, "unbekanntes Zeichen -> -1");
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
static int teil_maske(void)
{
    uint16_t *px = tim_laden();
    uint8_t *m = (uint8_t *) calloc(320 * 240, 1);
    int n, x0 = 999, x1 = -1, y0 = 999, y1 = -1, l1 = 0, l2 = 0, unpassend = 0, ring = 0, ring_bad = 0, kern_fehlt = 0;
    printf("== Maske ==\n");
    n = re15_titelversion_r39_maske(px, 320, 240, m);
    for (int y = 0; y < 240; y++)
        for (int x = 0; x < 320; x++) {
            int v;
            if (m[y * 320 + x]) {
                if (x < x0) x0 = x;
                if (x > x1) x1 = x;
                if (y < y0) y0 = y;
                if (y > y1) y1 = y;
                if (y <= 23) l1++; else l2++;
                if (!grau(px[y * 320 + x], NULL) || (px[y * 320 + x] & 0x8000u)) unpassend++;
            } else {
                int nb = 0;
                for (int dy = -1; dy <= 1; dy++)
                    for (int dx = -1; dx <= 1; dx++) {
                        int xx = x + dx, yy = y + dy;
                        if (xx >= 0 && yy >= 0 && xx < 320 && yy < 240 && m[yy * 320 + xx]) nb = 1;
                    }
                if (nb) { ring++; if (px[y * 320 + x] != 0x8000u) ring_bad++; }
                if (x >= RE15_R39T_KASTEN_X0 && x <= RE15_R39T_KASTEN_X1 && y >= RE15_R39T_KASTEN_Y0 &&
                    y <= RE15_R39T_KASTEN_Y1 && grau(px[y * 320 + x], &v) && v >= 20) kern_fehlt++;
            }
        }
    PRUEF(n == 929, "929 Pixel (ist %d)", n);
    PRUEF(x0 == 198 && x1 == 283 && y0 == 11 && y1 == 33, "bbox x198..283 y11..33 (ist x%d..%d y%d..%d)", x0, x1, y0, y1);
    PRUEF(l1 == 357 && l2 == 572, "Zeile \"Sample\" 357 / Zeile \"Ver.\" 572 (ist %d / %d)", l1, l2);
    PRUEF(unpassend == 0, "alle Maskenpixel grau mit STP 0 (Abweichler %d)", unpassend);
    PRUEF(ring == 608 && ring_bad == 0, "Ring um die Maske 608 Pixel, alle 0x8000 (ist %d, davon anders %d)", ring, ring_bad);
    PRUEF(kern_fehlt == 0, "jeder Glyphenkern (Grau >= 20) im Kasten liegt in der Maske (fehlend %d)", kern_fehlt);
    {
        int staub = 0, staub_maske = 0;
        for (int y = 21; y <= 25; y++)
            for (int x = 191; x <= 204; x++) {
                uint16_t c = px[y * 320 + x];
                int zone = (x == 204 && y == 21) || (x <= 196 && y >= 22);   /* die beiden Kunst-Komponenten */
                if (!zone) continue;
                if (c & 0x7fffu) { staub++; if (m[y * 320 + x]) staub_maske++; }
            }
        PRUEF(staub == 9 && staub_maske == 0, "Staub der Hintergrund-Ader (204,21) + (191..196,22..25): 9 Pixel, keiner in "
              "der Maske (ist %d / %d)", staub, staub_maske);
    }
    /* kuenstliche Faelle in einem schwarzen Bild */
    {
        uint16_t *b = (uint16_t *) malloc(320 * 240 * 2);
        for (int i = 0; i < 320 * 240; i++) b[i] = 0x8000u;
        for (int y = 10; y < 14; y++) for (int x = 200; x < 205; x++) b[y * 320 + x] = (uint16_t) (25 | 25 << 5 | 25 << 10);
        PRUEF(re15_titelversion_r39_maske(b, 320, 240, NULL) == 20, "grauer Block mit Kern -> 20 Pixel");
        for (int y = 10; y < 14; y++) for (int x = 200; x < 205; x++) b[y * 320 + x] = (uint16_t) (25);   /* rot */
        PRUEF(re15_titelversion_r39_maske(b, 320, 240, NULL) == 0, "roter Block (nicht grau) -> 0");
        for (int y = 10; y < 14; y++) for (int x = 200; x < 205; x++) b[y * 320 + x] = (uint16_t) (12 | 12 << 5 | 12 << 10);
        PRUEF(re15_titelversion_r39_maske(b, 320, 240, NULL) == 0, "grauer Block ohne Kern (12) -> 0");
        for (int y = 2; y < 8; y++) for (int x = 250; x < 253; x++) b[y * 320 + x] = (uint16_t) (25 | 25 << 5 | 25 << 10);
        PRUEF(re15_titelversion_r39_maske(b, 320, 240, NULL) == 0, "grauer Block am Kastenrand (y4) -> 0");
        free(b);
    }
    free(m);
    free(px);
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
static int teil_anwenden(void)
{
    uint16_t *px = tim_laden();
    uint16_t *b = (uint16_t *) malloc(320 * 240 * 2);
    uint8_t *m = (uint8_t *) calloc(320 * 240, 1), *kachel = (uint8_t *) calloc(320 * 240, 1);
    uint8_t *vorher = (uint8_t *) malloc((size_t) g_datei_n);
    static const char *const versionen[] = { "v0.8.27", "v0.8.26 dev", "v9.9.9" };
    printf("== Umschreiben ==\n");
    memcpy(vorher, g_datei, (size_t) g_datei_n);
    re15_titelversion_r39_maske(px, 320, 240, m);
    for (unsigned t = 0; t < sizeof versionen / sizeof versionen[0]; t++) {
        char anz[64], z[RE15_R39T_TEXT_MAX];
        int s[RE15_R39T_TEXT_MAX], n, rest = 0, maske_bad = 0, glyph_bad = 0, glyph_px = 0, r;
        memcpy(b, px, 320 * 240 * 2);
        memset(kachel, 0, 320 * 240);
        r = re15_titelversion_r39_anwenden(b, 320, 240, versionen[t]);
        re15_titelversion_r39_text(versionen[t], anz, (int) sizeof anz);
        n = re15_titelversion_r39_layout(anz, z, s, RE15_R39T_TEXT_MAX);
        /* Erwartung je Kachelpixel: Wert ueber 0x8000 (Maske/Schwarz) bzw. ueber dem Original */
        for (int i = 0; i < n; i++) {
            const re15_r39t_glyphe_t *g = re15_titelversion_r39_glyphe(z[i]);
            for (int rr = 0; rr < RE15_R39T_ZEILEN; rr++)
                for (int c = 0; c < g->breite; c++) {
                    int x = s[i] + c, y = RE15_R39T_ZEILE0 + rr, v = g->werte[rr * g->breite + c];
                    if (v) kachel[y * 320 + x] = 1;
                }
        }
        for (int y = 0; y < 240; y++)
            for (int x = 0; x < 320; x++) {
                int i = y * 320 + x;
                if (kachel[i]) continue;
                if (m[i]) { if (b[i] != 0x8000u) maske_bad++; }
                else if (b[i] != px[i]) rest++;
            }
        /* Kachelpixel: alle Kachelwerte uebereinander ueber den Hintergrund gelegt (Neben-Kacheln koennen sich eine
         * Teil-Spalte teilen) - nachgerechnet mit der Formel der Kopfdatei. */
        {
            uint16_t *soll = (uint16_t *) malloc(320 * 240 * 2);
            for (int i = 0; i < 320 * 240; i++) soll[i] = m[i] ? 0x8000u : px[i];
            for (int i = 0; i < n; i++) {
                const re15_r39t_glyphe_t *g = re15_titelversion_r39_glyphe(z[i]);
                for (int rr = 0; rr < RE15_R39T_ZEILEN; rr++)
                    for (int c = 0; c < g->breite; c++) {
                        int x = s[i] + c, y = RE15_R39T_ZEILE0 + rr, v = g->werte[rr * g->breite + c], k[3];
                        uint16_t a = soll[y * 320 + x];
                        if (!v) continue;
                        k[0] = a & 31; k[1] = (a >> 5) & 31; k[2] = (a >> 10) & 31;
                        for (int q = 0; q < 3; q++) k[q] = (k[q] * (25 - v) + 25 * v + 12) / 25;
                        soll[y * 320 + x] = (uint16_t) (k[0] | k[1] << 5 | k[2] << 10);
                    }
            }
            for (int i = 0; i < 320 * 240; i++)
                if (kachel[i]) { glyph_px++; if (b[i] != soll[i]) glyph_bad++; }
            free(soll);
        }
        PRUEF(r == 929, "\"%s\": 929 Pixel entfernt (ist %d)", versionen[t], r);
        PRUEF(rest == 0, "\"%s\": ausserhalb Maske + neuer Kacheln bytegleich (Abweichungen %d)", versionen[t], rest);
        PRUEF(maske_bad == 0, "\"%s\": Maske ohne neue Kachel = 0x8000 (Abweichungen %d)", versionen[t], maske_bad);
        PRUEF(n > 0 && glyph_bad == 0 && glyph_px > 100, "\"%s\": neue Zeile \"%s\" exakt gezeichnet (%d Pixel, "
              "Abweichungen %d)", versionen[t], anz, glyph_px, glyph_bad);
    }
    /* direkte Gegenprobe ohne Kachel-Rechnung: "9" (ergaenzt) bei v9.9.9 an ihrer Spalte, auf Schwarz = Kachelwert */
    {
        char z[RE15_R39T_TEXT_MAX];
        int s[RE15_R39T_TEXT_MAX], n, bad = 0;
        const re15_r39t_glyphe_t *g9 = re15_titelversion_r39_glyphe('9');
        memcpy(b, px, 320 * 240 * 2);
        re15_titelversion_r39_anwenden(b, 320, 240, "v9.9.9");
        n = re15_titelversion_r39_layout("Ver.9.9.9", z, s, RE15_R39T_TEXT_MAX);
        for (int rr = 0; rr < RE15_R39T_ZEILEN; rr++)
            for (int c = 1; c < g9->breite - 1; c++) {
                int v, x = s[n - 1] + c, y = RE15_R39T_ZEILE0 + rr;
                if (!grau(b[y * 320 + x], &v) || v != g9->werte[rr * g9->breite + c]) bad++;
            }
        PRUEF(n == 9 && s[n - 1] == 277 && bad == 0, "\"Ver.9.9.9\": letzte '9' bei x277 (ist %d) = Kachel (Abweichungen %d)",
              n > 0 ? s[n - 1] : -1, bad);
    }
    PRUEF(re15_titelversion_r39_anwenden(b, 256, 240, "v0.8.27") == -1, "falsches Bildmass -> -1");
    /* TITLEU.TIM auf der Platte unveraendert */
    free(g_datei); g_datei = NULL;
    free(tim_laden());
    PRUEF(memcmp(vorher, g_datei, (size_t) g_datei_n) == 0, "DATA/TITLEU.TIM auf der Platte bytegleich (%ld Bytes)", g_datei_n);
    free(vorher); free(kachel); free(m); free(b); free(px);
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
static int teil_version(void)
{
    char anz[64];
    printf("== Version ==\n");
    PRUEF(strcmp(re15_titelversion_r39_version(), RE39T_ERWARTET) == 0, "einkompilierte Version \"%s\" == CMake \"%s\"",
          re15_titelversion_r39_version(), RE39T_ERWARTET);
    PRUEF(re15_titelversion_r39_text(re15_titelversion_r39_version(), anz, (int) sizeof anz) > 0 &&
          strncmp(anz, "Ver.", 4) == 0, "Anzeige \"%s\"", anz);
    return 0;
}

/* ---------------------------------------------------------------------------------------------------------------- */
/* Integrations-Riegel: Titelabzug (BMP 24 bit, SDL_SaveBMP) der echten exe bei Fensterskala 1. */
static uint8_t *bmp_laden(const char *pfad, int *w, int *h)
{
    FILE *f = fopen(pfad, "rb");
    uint8_t kopf[54], *rgb;
    int off, bpp, hoch, stride;
    if (!f) return NULL;
    if (fread(kopf, 1, 54, f) != 54 || kopf[0] != 'B' || kopf[1] != 'M') { fclose(f); return NULL; }
    off = kopf[10] | kopf[11] << 8 | kopf[12] << 16 | kopf[13] << 24;
    *w = kopf[18] | kopf[19] << 8 | kopf[20] << 16 | kopf[21] << 24;
    hoch = (int) (int32_t) (kopf[22] | kopf[23] << 8 | kopf[24] << 16 | (uint32_t) kopf[25] << 24);
    bpp = kopf[28] | kopf[29] << 8;
    *h = hoch < 0 ? -hoch : hoch;
    if ((bpp != 24 && bpp != 32) || *w <= 0 || *h <= 0) { fclose(f); return NULL; }
    stride = ((*w * (bpp / 8)) + 3) & ~3;
    rgb = (uint8_t *) malloc((size_t) *w * (size_t) *h * 3);
    for (int y = 0; y < *h; y++) {
        uint8_t *zeile = (uint8_t *) malloc((size_t) stride);
        int ziel = hoch > 0 ? (*h - 1 - y) : y;
        fseek(f, off + (long) y * stride, SEEK_SET);
        if (fread(zeile, 1, (size_t) stride, f) != (size_t) stride) { free(zeile); free(rgb); fclose(f); return NULL; }
        for (int x = 0; x < *w; x++) {
            rgb[(ziel * *w + x) * 3 + 0] = zeile[x * (bpp / 8) + 2];
            rgb[(ziel * *w + x) * 3 + 1] = zeile[x * (bpp / 8) + 1];
            rgb[(ziel * *w + x) * 3 + 2] = zeile[x * (bpp / 8) + 0];
        }
        free(zeile);
    }
    fclose(f);
    return rgb;
}

static int teil_pruefen(const char *bmp)
{
    int w = 0, h = 0, rest = 0, erwartet = 0, sample_rest = 0, n_neu = 0;
    uint16_t *px = tim_laden(), *soll = (uint16_t *) malloc(320 * 240 * 2);
    uint8_t *m = (uint8_t *) calloc(320 * 240, 1), *shot = bmp_laden(bmp, &w, &h);
    char anz[64];
    printf("== Titelabzug %s ==\n", bmp);
    if (!shot || w != 320 || h != 240) { PRUEF(0, "Abzug 320x240 lesbar (ist %dx%d)", w, h); return 0; }
    re15_titelversion_r39_maske(px, 320, 240, m);
    memcpy(soll, px, 320 * 240 * 2);
    re15_titelversion_r39_anwenden(soll, 320, 240, re15_titelversion_r39_version());
    re15_titelversion_r39_text(re15_titelversion_r39_version(), anz, (int) sizeof anz);
    printf("text=%s\n", anz);
    for (int y = 0; y < 240; y++)
        for (int x = 0; x < 320; x++) {
            int i = y * 320 + x, im_menue = (x >= 80 && x <= 240 && y >= 130 && y <= 225);
            int sr = shot[i * 3], sg = shot[i * 3 + 1], sb = shot[i * 3 + 2];
            int tr = (px[i] & 31) << 3, tg = ((px[i] >> 5) & 31) << 3, tb = ((px[i] >> 10) & 31) << 3;
            int er = (soll[i] & 31) << 3, eg = ((soll[i] >> 5) & 31) << 3, eb = ((soll[i] >> 10) & 31) << 3;
            int im_kasten = (x >= RE15_R39T_KASTEN_X0 && x <= RE15_R39T_KASTEN_X1 && y >= RE15_R39T_KASTEN_Y0 &&
                             y <= RE15_R39T_KASTEN_Y1);
            if (im_menue) continue;   /* NEW GAME / LOAD GAME / OPTION (TMOJI-Sprites): gemessen x88..229 y136..218 */
            /* (1) unabhaengig vom neuen Code: ausserhalb des Kastens = rohes TITLEU.TIM (RGB555 << 3) */
            if (!im_kasten && (sr != tr || sg != tg || sb != tb)) rest++;
            /* (2) Zeile "Sample" (y11..23): Maske schwarz, sonst rohes Bild - dort wird nichts Neues gezeichnet */
            if (im_kasten && y >= 11 && y <= 23) {
                if (m[i] ? (sr | sg | sb) != 0 : (sr != tr || sg != tg || sb != tb)) sample_rest++;
            }
            /* (3) gegen die Engine-Erwartung (TITLEU.TIM + re15_titelversion_r39_anwenden mit der exe-Version) */
            if (sr != er || sg != eg || sb != eb) erwartet++;
            if (soll[i] != (m[i] ? 0x8000u : px[i])) n_neu++;
        }
    PRUEF(rest == 0, "ausserhalb des Kastens x188..295 y4..35 (ohne Menue) = rohes TITLEU.TIM (Abweichungen %d)", rest);
    PRUEF(sample_rest == 0, "Zeile \"Sample\" weg: Maske schwarz, Rest unveraendert (Abweichungen %d)", sample_rest);
    PRUEF(erwartet == 0 && n_neu > 100, "Abzug == Erwartung \"%s\" pixelgleich (Abweichungen %d, neue Pixel %d)", anz,
          erwartet, n_neu);
    free(shot); free(m); free(soll); free(px);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "";
    if (!strcmp(teil, "glyphen")) teil_glyphen();
    else if (!strcmp(teil, "text")) teil_text();
    else if (!strcmp(teil, "layout")) teil_layout();
    else if (!strcmp(teil, "maske")) teil_maske();
    else if (!strcmp(teil, "anwenden")) teil_anwenden();
    else if (!strcmp(teil, "version")) teil_version();
    else if (!strcmp(teil, "pruefen") && argc > 2) teil_pruefen(argv[2]);
    else { fprintf(stderr, "unbekannter Teil '%s'\n", teil); return 2; }
    printf("== r39_titelversion %s: %d/%d Pruefungen gruen ==\n", teil, g_n - g_fehler, g_n);
    return (g_n > 0 && g_fehler == 0) ? 0 : 1;
}
