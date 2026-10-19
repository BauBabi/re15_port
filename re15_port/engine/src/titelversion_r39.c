/*
 * Runde 39 Spur T "titelversion" — Version im Titelbild statt "Sample / Ver.2025.01.25".
 * Kopf und Belege: include/re15_titelversion_r39.h, Dossier analysis/befunde_runde39/T_titelversion.md.
 *
 * Ablauf (einmal beim Umwandeln des Titelbilds, render_pc.c re15_render_pc_show_title):
 *   1. Maske der alten Beschriftung: graue Komponenten mit Glyphenkern im Kasten x188..295 y4..35 (929 Pixel).
 *   2. Maskenpixel -> 0x8000 (der gemessene Hintergrund: Ring um die Maske 608/608 = 0x8000).
 *   3. "Ver." + Version in der Pixelschrift der alten Zeile zeichnen, rechtsbuendig an deren Tintenkante 283,60 px,
 *      Kacheln aus gen/titelversion_r39_glyphen.inc, Deckung als Alpha ueber den Hintergrund (Schriftfarbe 25).
 * Die Datei DATA/TITLEU.TIM bleibt bytegleich (nur die Kopie im Speicher wird umgeschrieben).
 */
#include <string.h>
#include <stdlib.h>
#include "re15_titelversion_r39.h"
#ifdef RE15_PLATFORM_PC
#include <stdio.h>
#endif

/* RE15_VERSION kommt aus cmake/re15_version_r39.cmake (nur fuer diese Datei gesetzt). Fehlt sie (Fremdbau ohne das
 * Modul), zeigt das Titelbild ehrlich "Ver.dev". */
#ifndef RE15_VERSION
#define RE15_VERSION "dev"
#endif

#include "gen/titelversion_r39_glyphen.inc"   /* k_r39t_glyphen[]: Kacheln + Herkunft je Glyphe */

#define K_GLYPHEN_N ((int) (sizeof k_r39t_glyphen / sizeof k_r39t_glyphen[0]))
#define K_KASTEN_W  (RE15_R39T_KASTEN_X1 - RE15_R39T_KASTEN_X0 + 1)
#define K_KASTEN_H  (RE15_R39T_KASTEN_Y1 - RE15_R39T_KASTEN_Y0 + 1)

const char *re15_titelversion_r39_version(void) { return RE15_VERSION; }

int re15_titelversion_r39_glyphen_zahl(void) { return K_GLYPHEN_N; }

const re15_r39t_glyphe_t *re15_titelversion_r39_glyphe_nr(int i)
{
    return (i >= 0 && i < K_GLYPHEN_N) ? &k_r39t_glyphen[i] : NULL;
}

const re15_r39t_glyphe_t *re15_titelversion_r39_glyphe(char c)
{
    int i;
    for (i = 0; i < K_GLYPHEN_N; i++)
        if (k_r39t_glyphen[i].zeichen == c) return &k_r39t_glyphen[i];
    return NULL;
}

static int anhaengen(char *out, int n, int *len, const char *s)
{
    while (*s) {
        if (*len + 1 >= n) return -1;
        out[(*len)++] = *s++;
    }
    out[*len] = 0;
    return 0;
}

int re15_titelversion_r39_text(const char *version, char *out, int n)
{
    char kern[24];
    int k = 0, len = 0, rest;
    const char *p = version ? version : "";
    if (!out || n <= 0) return -1;
    out[0] = 0;
    if (*p == 'v' || *p == 'V') p++;
    if (*p >= '0' && *p <= '9') {
        while (((*p >= '0' && *p <= '9') || *p == '.') && k < (int) sizeof kern - 1) kern[k++] = *p++;
        while (k > 0 && kern[k - 1] == '.') { k--; p--; }   /* "0.8." -> "0.8" + Rest */
    }
    kern[k] = 0;
    rest = (*p != 0);                                       /* " dev", "-5-gabc", ... */
    if (anhaengen(out, n, &len, "Ver.") < 0) return -1;
    if (k == 0) return anhaengen(out, n, &len, "dev") < 0 ? -1 : len;
    if (anhaengen(out, n, &len, kern) < 0) return -1;
    if (rest && anhaengen(out, n, &len, " dev") < 0) return -1;
    return len;
}

/* Runden auf die naechste ganze Spalte (Werte in 1/25 px, auch negativ). */
static int spalte_runden(long v25)
{
    return (int) (v25 >= 0 ? (v25 + 12) / 25 : -((-v25 + 12) / 25));
}

int re15_titelversion_r39_layout(const char *anzeige, char *zeichen, int *spalte, int max)
{
    char tz[RE15_R39T_TEXT_MAX];
    int ts[RE15_R39T_TEXT_MAX];
    int n, i, cnt = 0, erster = 1, wort = 0;
    long ziel = RE15_R39T_RECHTS25;
    if (!anzeige) return -1;
    n = (int) strlen(anzeige);
    for (i = n - 1; i >= 0; i--) {
        const re15_r39t_glyphe_t *g;
        int x;
        if (anzeige[i] == ' ') { wort = 1; continue; }
        g = re15_titelversion_r39_glyphe(anzeige[i]);
        if (!g || cnt >= RE15_R39T_TEXT_MAX || cnt >= max) return -1;
        /* Abstand zum rechten Nachbarn: 0,72 px; Wortabstand 2,74 px vor einem Leerzeichen und nach dem Praefix "Ver."
         * (gemessen: Tinte "." bis 224,10, "2" ab 226,84). */
        if (!erster) ziel -= (wort || (i == 3 && strncmp(anzeige, "Ver.", 4) == 0)) ? RE15_R39T_WORT25 : RE15_R39T_ABSTAND25;
        x = spalte_runden(ziel - (long) g->tinte_r);
        tz[cnt] = anzeige[i];
        ts[cnt] = x;
        cnt++;
        ziel = (long) x * 25 + g->tinte_l;
        erster = 0;
        wort = 0;
    }
    for (i = 0; i < cnt; i++) {          /* von links nach rechts ausgeben */
        if (zeichen) zeichen[i] = tz[cnt - 1 - i];
        if (spalte) spalte[i] = ts[cnt - 1 - i];
    }
    return cnt;
}

/* Maske im Kasten (K_KASTEN_W x K_KASTEN_H), Rueckgabe: Zahl der Pixel. */
static int maske_kasten(const uint16_t *px, int w, int h, uint8_t *km)
{
    static const int dx8[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    static const int dy8[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    uint8_t  gesehen[K_KASTEN_W * K_KASTEN_H];
    uint16_t stapel[K_KASTEN_W * K_KASTEN_H];
    uint16_t teil[K_KASTEN_W * K_KASTEN_H];
    int bx, by, gesamt = 0;
    memset(km, 0, (size_t) K_KASTEN_W * K_KASTEN_H);
    if (!px || w <= RE15_R39T_KASTEN_X1 || h <= RE15_R39T_KASTEN_Y1) return 0;
    memset(gesehen, 0, sizeof gesehen);
    for (by = 0; by < K_KASTEN_H; by++) {
        for (bx = 0; bx < K_KASTEN_W; bx++) {
            int start = by * K_KASTEN_W + bx, sp = 0, nt = 0, grau = 1, rand = 0, maxv = 0, k;
            uint16_t c0 = px[(by + RE15_R39T_KASTEN_Y0) * w + bx + RE15_R39T_KASTEN_X0];
            if (gesehen[start] || (c0 & 0x7fffu) == 0) continue;
            gesehen[start] = 1;
            stapel[sp++] = (uint16_t) start;
            while (sp > 0) {
                int q = stapel[--sp], qx = q % K_KASTEN_W, qy = q / K_KASTEN_W, d;
                uint16_t c = px[(qy + RE15_R39T_KASTEN_Y0) * w + qx + RE15_R39T_KASTEN_X0];
                int r = c & 31, g = (c >> 5) & 31, b = (c >> 10) & 31;
                teil[nt++] = (uint16_t) q;
                if (r != g || g != b) grau = 0;
                if (r > maxv) maxv = r;
                if (qx == 0 || qy == 0 || qx == K_KASTEN_W - 1 || qy == K_KASTEN_H - 1) rand = 1;
                for (d = 0; d < 8; d++) {
                    int nx = qx + dx8[d], ny = qy + dy8[d], ni;
                    if (nx < 0 || ny < 0 || nx >= K_KASTEN_W || ny >= K_KASTEN_H) continue;
                    ni = ny * K_KASTEN_W + nx;
                    if (gesehen[ni]) continue;
                    if ((px[(ny + RE15_R39T_KASTEN_Y0) * w + nx + RE15_R39T_KASTEN_X0] & 0x7fffu) == 0) continue;
                    gesehen[ni] = 1;
                    stapel[sp++] = (uint16_t) ni;
                }
            }
            /* Beschriftung: ganz grau, Glyphenkern (Maximum >= 20), ganz im Kasten. */
            if (!grau || maxv < RE15_R39T_KERN || rand) continue;
            for (k = 0; k < nt; k++) km[teil[k]] = 1;
            gesamt += nt;
        }
    }
    return gesamt;
}

int re15_titelversion_r39_maske(const uint16_t *px, int w, int h, uint8_t *maske)
{
    uint8_t km[K_KASTEN_W * K_KASTEN_H];
    int n = maske_kasten(px, w, h, km), x, y;
    if (maske && px && w > RE15_R39T_KASTEN_X1 && h > RE15_R39T_KASTEN_Y1) {
        memset(maske, 0, (size_t) w * (size_t) h);
        for (y = 0; y < K_KASTEN_H; y++)
            for (x = 0; x < K_KASTEN_W; x++)
                maske[(y + RE15_R39T_KASTEN_Y0) * w + x + RE15_R39T_KASTEN_X0] = km[y * K_KASTEN_W + x];
    }
    return n;
}

/* Eine Kachel mit Alpha = Wert/25 in Schriftfarbe 25 ueber das Bild legen (auf 0x8000 ergibt das genau den Wert). */
static void kachel_zeichnen(uint16_t *px, int w, int h, const re15_r39t_glyphe_t *g, int x0)
{
    int r, c;
    for (r = 0; r < RE15_R39T_ZEILEN; r++) {
        int y = RE15_R39T_ZEILE0 + r;
        if (y < 0 || y >= h) continue;
        for (c = 0; c < (int) g->breite; c++) {
            int v = g->werte[r * g->breite + c], x = x0 + c;
            uint16_t alt;
            int k[3], i;
            if (v <= 0 || x < 0 || x >= w) continue;
            alt = px[y * w + x];
            k[0] = alt & 31; k[1] = (alt >> 5) & 31; k[2] = (alt >> 10) & 31;
            for (i = 0; i < 3; i++) k[i] = (k[i] * (25 - v) + 25 * v + 12) / 25;
            px[y * w + x] = (uint16_t) (k[0] | (k[1] << 5) | (k[2] << 10));   /* STP 0 wie die alte Beschriftung */
        }
    }
}

int re15_titelversion_r39_anwenden(uint16_t *px, int w, int h, const char *version)
{
    uint8_t km[K_KASTEN_W * K_KASTEN_H];
    char anzeige[RE15_R39T_TEXT_MAX], zeichen[RE15_R39T_TEXT_MAX];
    int spalte[RE15_R39T_TEXT_MAX];
    int n, i, entfernt, x, y, links = 0, rechts = 0, ok = 0;
    if (!px || w != 320 || h != 240) return -1;
    /* 1+2: alte Beschriftung -> gemessener Hintergrund 0x8000 */
    entfernt = maske_kasten(px, w, h, km);
    for (y = 0; y < K_KASTEN_H; y++)
        for (x = 0; x < K_KASTEN_W; x++)
            if (km[y * K_KASTEN_W + x])
                px[(y + RE15_R39T_KASTEN_Y0) * w + x + RE15_R39T_KASTEN_X0] = (uint16_t) RE15_R39T_HINTERGRUND;
    /* 3: neue Zeile */
    if (re15_titelversion_r39_text(version, anzeige, (int) sizeof anzeige) > 0) {
        n = re15_titelversion_r39_layout(anzeige, zeichen, spalte, RE15_R39T_TEXT_MAX);
        if (n > 0 && spalte[0] >= RE15_R39T_LINKS) {
            for (i = 0; i < n; i++)
                kachel_zeichnen(px, w, h, re15_titelversion_r39_glyphe(zeichen[i]), spalte[i]);
            links = spalte[0];
            rechts = spalte[n - 1] + re15_titelversion_r39_glyphe(zeichen[n - 1])->breite - 1;
            ok = 1;
        }
    }
#ifdef RE15_PLATFORM_PC
    fprintf(stderr, "[titel_r39] alte Beschriftung entfernt: %d Pixel; neu \"%s\" (RE15_VERSION \"%s\")%s Spalten %d..%d\n",
            entfernt, ok ? anzeige : "", version ? version : "", ok ? "" : " NICHT GEZEICHNET", links, rechts);
#endif
    return entfernt;
}

const uint16_t *re15_titelversion_r39_bild(const uint16_t *px, int w, int h)
{
    static uint16_t *kopie = NULL;
    size_t n;
    if (!px || w != 320 || h != 240) return px;
    n = (size_t) w * (size_t) h;
    if (!kopie) kopie = (uint16_t *) malloc(n * sizeof *kopie);   /* einmal je Prozess, bleibt fuer weitere Aufrufe */
    if (!kopie) return px;
    memcpy(kopie, px, n * sizeof *kopie);
    re15_titelversion_r39_anwenden(kopie, w, h, re15_titelversion_r39_version());
    return kopie;
}
