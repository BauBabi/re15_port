/* probe_r37_kiste2feld_bild.c — MESS-WERKZEUG (kein eigener Test), Runde 37 Spur R "kiste2feld".
 * Dossier analysis/befunde_runde37/R_kiste2feld.md. Vergleicht die 2-Feld-Symbole in Bildern der ECHTEN exe
 * (RE15_FRAMEDUMP, RE15_WINDOW_SCALE=1, P6 320x240, Kanal = 5-Bit-Wert << 3) PIXELGENAU mit der Quellgrafik:
 *
 *   kiste  <ppm> <ids> <mengen>   Kisten-LISTE (RE2-Layout, Spur M: Symbol buendig rechts, 2-Feld 49x19 bei
 *          x 155..203, Zeile k bei y 53+20k .. +18). Je Zeile mit 2-Feld-Id (0x0e..0x13) zwei Erwartungen:
 *            NEU = RE2-Abbildung der BREITEN Kunst: RE2 zeichnet die Zeile als POLY_FT4 u0 40 .. u1 0x78 = 120
 *                  (`addiu t1,zero,120` @0x8007183c, `sb t1,-14(s0)` @0x80071848 = u1, `sb t1,2(s0)` @0x80071860
 *                  = u3; u0/u2 = `addiu t0,zero,40` @0x800717b4) ueber 49 x 19 px (x1 = boxX+0x93+48
 *                  @0x80071834/@0x80071884, y2-y0 = 19) — also 80 x 30 Texel auf 49 x 19 Pixel. Die 80 Texel
 *                  sind ZWEI 40x30-Kacheln: links Kachel[id], rechts Kachel[id+0x56] (FUN_80069c40 zweite
 *                  LoadImage @0x80069d4c-58, a1 = src+0x19320 = +86*1200). RE1.5 traegt die breite Kunst als
 *                  EIN 80x30-Bild im ITPS-Block id*0x3000+0x21A0 (FUN_8004dc4c-Aufruf @0x8001e0b8-c8,
 *                  FUN_800492b8 Modus 1 `ori v0,zero,0x28` @0x80049314 = 80 Texel breit).
 *                  Abtastung = PSX-GPU (DuckStation gpu_sw_rasterizer, Fixkomma 12 Bit, Startwert + 1/2, Schritt
 *                  (n_tex << 12) / n_px): Texel = (2048 + p * ((n_tex << 12) / n_px)) >> 12 — gemessen am Bild
 *                  des laufenden RE2 (analysis/itembox_re2/referenz/re2_itembox_original.png, Dossier §2.3:
 *                  Gatling Gun 770/771, R. Launcher 752/771 Masken-Pixel; abgerundet nur 759 / 746). Eigene
 *                  Umsetzung hier, NICHT aus dem Port-Kopf (unabhaengige Erwartung).
 *            ALT = der Stand vor Spur R: ITEMALL-Kachel[id] (40x30 = nur die LINKE Haelfte), Spalten 0..39 1:1,
 *                  Spalten 40..48 = Spalte 39 (Klemmung `if (sxp > 39) sxp = 39`), v = py*30/19 abgerundet.
 *          SCHMALE Zeilen (alle anderen Ids, 25x19 bei x 179..203): RE2 zeichnet sie mit DEMSELBEN POLY_FT4
 *            (u0 40 @0x800717b4 .. u1 80 `addiu t1,zero,80` @0x80071844) — NEU = ITEMALL-Kachel[id] 40x30 mit der
 *            GPU-Abtastung auf 25x19 (RE2-Bild F. Aid Spray 475/475, abgerundet 469/475), ALT = abgerundet
 *            px*40/25, py*30/19. Nicht fuer die im Port ersetzten Kacheln 0x40/0x42/0x45 (Sicherung, Schluessel).
 *          Texel mit CLUT-Wert 0 sind durchsichtig -> Listengrund (24,24,40). Ausgenommen: die Linien des
 *          Auswahl-Bands (y 92/93/112/113, liegen ueber den Zeilen) und das Ziffernfeld unten links im Symbol
 *          (x Symbol .. +6*(Stellen-1)+7, y Zeile+11 .. +18; RE2 FUN_80072c6c, Ziffern ueber dem Symbol; nur
 *          Ids < 0x22 tragen Ziffern, Gate `sltiu id,0x22` @0x80049124).
 *          Zeile OK = NEU 0 Abweichungen, bei 2-Feld-Zeilen zusaetzlich nicht alle Spalten 40..48 gleich Spalte 39.
 *   gitter <ppm> <list_x> <list_y> <kopfzellen> [cursor=k]   Inventar-GITTER (40x30-Zellen, RE1.5 Zellentabelle
 *          @0x80076274 + Listenbasis): je 2-Feld-Waffe mit Kopfzelle k ("k:id:menge", Komma-getrennt) muss Zelle
 *          k die LINKE Haelfte (ITPS-Spalten 0..39) und Zelle k+1 die RECHTE Haelfte (Spalten 40..79) zeigen —
 *          wie im Original nach dem Einfuegen (eine LoadImage 80x30 ueber Zelle 0/1, FUN_800492b8 Modus 1
 *          @0x8004df08) und nach jedem Verschieben (MoveImage je 40x30-Zelle, FUN_80049390: Einfuegen
 *          @0x8004dc98-de3c, Zusammenschieben @0x8004db30). Verglichen werden die deckenden Texel; ausgenommen das
 *          Ziffernfeld der Kopfzelle (x +3 .. +3+6*(Stellen-1)+7, y +20..+27, FUN_80048f28) und die Zellen unter
 *          dem Cursor (cursor=k).
 *
 * Aufruf: probe_r37_kiste2feld_bild <PSX-Asset-Ordner> kiste <ppm> <ids> <mengen>   (ids hex, "-" = leere Zeile)
 *         probe_r37_kiste2feld_bild <PSX-Asset-Ordner> gitter <ppm> <list_x> <list_y> <k:id:menge,...> [cursor=k]
 * Ausgabe je Zeile/Zelle eine Zeile, Ende "KISTE2FELD-BILD OK" oder "KISTE2FELD-BILD FEHLER n"; exit 0/1.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static unsigned char s_bild[240][320][3];
static uint8_t *s_itemall, *s_itps;
static long s_itemall_n, s_itps_n;
static uint16_t s_clut[256];

static uint8_t *datei(const char *ordner, const char *rel, long *n)
{
    char p[1024];
    FILE *f;
    uint8_t *b;
    snprintf(p, sizeof p, "%s/%s", ordner, rel);
    f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); *n = ftell(f); fseek(f, 0, SEEK_SET);
    b = (uint8_t *)malloc((size_t)*n);
    if (b && fread(b, 1, (size_t)*n, f) != (size_t)*n) { free(b); b = NULL; }
    fclose(f);
    return b;
}

static int ppm_laden(const char *pfad)
{
    FILE *f = fopen(pfad, "rb");
    int w = 0, h = 0, mx = 0;
    if (!f) return 0;
    if (fscanf(f, "P6 %d %d %d", &w, &h, &mx) != 3 || w != 320 || h != 240 || mx != 255) { fclose(f); return 0; }
    fgetc(f);
    if (fread(s_bild, 1, sizeof s_bild, f) != sizeof s_bild) { fclose(f); return 0; }
    fclose(f);
    return 1;
}

/* Texel -> erwartete Bildfarbe (Kanal 5 Bit << 3, Modulation 128 = neutral); 0 = durchsichtig. */
static int farbe(uint8_t idx, unsigned char rgb[3])
{
    uint16_t c = s_clut[idx];
    if (c == 0) return 0;
    rgb[0] = (unsigned char)((c & 31) << 3);
    rgb[1] = (unsigned char)(((c >> 5) & 31) << 3);
    rgb[2] = (unsigned char)(((c >> 10) & 31) << 3);
    return 1;
}

static int gleich(int x, int y, const unsigned char rgb[3])
{
    return s_bild[y][x][0] == rgb[0] && s_bild[y][x][1] == rgb[1] && s_bild[y][x][2] == rgb[2];
}

static int stellen(int menge) { return menge >= 100 ? 3 : (menge >= 10 ? 2 : 1); }
static int band_zeile(int y) { return y == 92 || y == 93 || y == 112 || y == 113; }

static const uint8_t *breit(int id)   /* 80x30-Kunst der 2-Feld-Waffe (ITPS-Block id*0x3000+0x21A0) */
{
    long o = (long)id * 0x3000 + 0x21A0;
    return (s_itps && o + 80 * 30 <= s_itps_n) ? s_itps + o : NULL;
}

/* PSX-GPU-Abtastung eines gestauchten POLY_FT4 (s. Kopf): Texel fuer Pixel p, n_tex Texel auf n_px Pixel. */
static int gpu_texel(int n_tex, int n_px, int p)
{
    return (2048 + p * ((n_tex << 12) / n_px)) >> 12;
}

static int kiste(const char *ppm, const char *ids, const char *mengen)
{
    int k = 0, fehler = 0, geprueft = 0;
    char *pi = (char *)ids, *pm = (char *)mengen;
    if (!ppm_laden(ppm)) { printf("%s: FEHLER Bild nicht lesbar\n", ppm); return 1; }
    for (k = 0; k < 5 && *pi; k++) {
        int id, menge, breit_z, w, sw, sx, ry = 53 + 20 * k, px, py, n = 0, neu_ab = 0, alt_ab = 0, wdh = 0;
        int nd, ziffern;
        const uint8_t *quelle;
        if (*pi == '-') { pi++; id = 0; } else id = (int)strtol(pi, &pi, 16);
        menge = (int)strtol(pm, &pm, 10);
        if (*pi == ',') pi++;
        if (*pm == ',') pm++;
        if (id == 0) continue;                                              /* leere Zeile */
        breit_z = ((unsigned)(id - 0x0e) < 6);                              /* 2-Feld-Waffe */
        w = breit_z ? 49 : 25;
        sw = breit_z ? 80 : 40;
        sx = 204 - w;                                                       /* Spur M: buendig rechts */
        nd = stellen(menge);
        ziffern = (id < 0x22 && menge > 0);
        quelle = breit_z ? breit(id) : ((s_itemall && (long)(id + 1) * 1200 <= s_itemall_n)
                                        ? s_itemall + (long)id * 1200 : NULL);
        if (!quelle || !s_itemall || (long)(id + 1) * 1200 > s_itemall_n) {
            printf("FEHLER Quelle Id %02x\n", id);
            fehler++;
            continue;
        }
        for (py = 0; py < 19; py++) {
            int y = ry + py, sv = gpu_texel(30, 19, py), sv_alt = (py * 30) / 19;
            if (band_zeile(y)) continue;                                        /* Auswahl-Band */
            for (px = 0; px < w; px++) {
                unsigned char e_neu[3] = { 24, 24, 40 }, e_alt[3] = { 24, 24, 40 };
                int su = gpu_texel(sw, w, px);
                int sa = breit_z ? (px > 39 ? 39 : px) : (px * 40) / 25;
                if (ziffern && py >= 11 && px <= 6 * (nd - 1) + 7) continue;     /* Ziffernfeld */
                farbe(quelle[sv * sw + su], e_neu);
                farbe(s_itemall[(long)id * 1200 + sv_alt * 40 + sa], e_alt);
                n++;
                if (!gleich(sx + px, y, e_neu)) neu_ab++;
                if (!gleich(sx + px, y, e_alt)) alt_ab++;
            }
        }
        if (breit_z) {
            /* Wiederholte Spalten: Spalte 40..48 bildgleich Spalte 39 (in allen Zeilen ausser dem Band) */
            for (px = 40; px < 49; px++) {
                int same = 1;
                for (py = 0; py < 19; py++) {
                    int y = ry + py;
                    if (band_zeile(y)) continue;
                    if (memcmp(s_bild[y][sx + px], s_bild[y][sx + 39], 3) != 0) { same = 0; break; }
                }
                wdh += same;
            }
            printf("%s: Zeile %d Id %02x (2-Feld 49x19): NEU (80x30 -> 49x19, GPU) %d/%d Pixel gleich, ALT (40er-Kachel, "
                   "Spalte 39 wiederholt) %d/%d gleich, Spalten 40..48 gleich Spalte 39: %d/9 -> %s\n", ppm, k, id,
                   n - neu_ab, n, n - alt_ab, n, wdh, (neu_ab == 0 && wdh < 9) ? "ok" : "FALSCH");
            if (neu_ab != 0 || wdh == 9) fehler++;
        } else {
            printf("%s: Zeile %d Id %02x (schmal 25x19): NEU (40x30 -> 25x19, GPU) %d/%d Pixel gleich, ALT (abgerundet) "
                   "%d/%d gleich -> %s\n", ppm, k, id, n - neu_ab, n, n - alt_ab, n, neu_ab == 0 ? "ok" : "FALSCH");
            if (neu_ab != 0) fehler++;
        }
        geprueft++;
    }
    if (geprueft == 0) { printf("%s: FEHLER keine Zeile geprueft\n", ppm); fehler++; }
    return fehler;
}

static const short k_zelle[10][2] = {   /* RE1.5 Zellentabelle @0x80076274 */
    { 4, 32 }, { 44, 32 }, { 4, 62 }, { 44, 62 }, { 4, 92 }, { 44, 92 }, { 4, 122 }, { 44, 122 }, { 4, 152 }, { 44, 152 }
};

static int gitter(const char *ppm, int lx, int ly, const char *spec, int cursor)
{
    int fehler = 0, geprueft = 0;
    char *p = (char *)spec;
    if (!ppm_laden(ppm)) { printf("%s: FEHLER Bild nicht lesbar\n", ppm); return 1; }
    while (*p) {
        int kopf = (int)strtol(p, &p, 10), id = 0, menge = 0, h;
        const uint8_t *w;
        if (*p == ':') id = (int)strtol(p + 1, &p, 16);
        if (*p == ':') menge = (int)strtol(p + 1, &p, 10);
        if (*p == ',') p++;
        w = breit(id);
        if (!w || kopf < 0 || kopf > 8) { printf("FEHLER Angabe %d:%02x\n", kopf, id); fehler++; continue; }
        for (h = 0; h < 2; h++) {
            int zelle = kopf + h, x0 = k_zelle[zelle][0] + lx, y0 = k_zelle[zelle][1] + ly, u, v, n = 0, ab = 0;
            int nd = stellen(menge);
            if (zelle == cursor || kopf == cursor) {
                printf("%s: Zelle %d (Id %02x %s) unter dem Cursor - nicht verglichen\n", ppm, zelle, id,
                       h ? "rechte Haelfte" : "linke Haelfte");
                continue;
            }
            for (v = 0; v < 30; v++)
                for (u = 0; u < 40; u++) {
                    unsigned char e[3];
                    if (!farbe(w[v * 80 + h * 40 + u], e)) continue;               /* durchsichtig */
                    if (h == 0 && menge > 0 && v >= 20 && v <= 27 && u >= 3 && u <= 3 + 6 * (nd - 1) + 7) continue;
                    n++;
                    if (!gleich(x0 + u, y0 + v, e)) ab++;
                }
            printf("%s: Zelle %d (Id %02x %s, Bild x %d y %d): %d/%d deckende Texel gleich -> %s\n", ppm, zelle, id,
                   h ? "rechte Haelfte" : "linke Haelfte", x0, y0, n - ab, n, ab == 0 ? "ok" : "FALSCH");
            if (ab) fehler++;
            geprueft++;
        }
    }
    if (geprueft == 0) { printf("%s: FEHLER keine Zelle geprueft\n", ppm); fehler++; }
    return fehler;
}

int main(int argc, char **argv)
{
    long n = 0;
    uint8_t *st;
    int fehler;
    if (argc < 4) { printf("Aufruf: siehe Kopf\n"); return 2; }
    s_itemall = datei(argv[1], "DATA/ITEMALL.PIX", &s_itemall_n);
    s_itps = datei(argv[1], "ITEM/ITPS.ITP", &s_itps_n);
    st = datei(argv[1], "DATA/ST_00.TIM", &n);
    if (!s_itemall || !s_itps || !st || n < 20 + 512) { printf("FEHLER: Assets in %s\n", argv[1]); return 2; }
    memcpy(s_clut, st + 20, sizeof s_clut);     /* ST_00.TIM CLUT Zeile 0 (wie inv_render_pc.c s_clut[8]) */
    if (!strcmp(argv[2], "kiste") && argc >= 6) fehler = kiste(argv[3], argv[4], argv[5]);
    else if (!strcmp(argv[2], "gitter") && argc >= 7) {
        int cursor = -1;
        if (argc >= 8 && !strncmp(argv[7], "cursor=", 7)) cursor = atoi(argv[7] + 7);
        fehler = gitter(argv[3], atoi(argv[4]), atoi(argv[5]), argv[6], cursor);
    } else { printf("Aufruf: siehe Kopf\n"); return 2; }
    if (fehler) { printf("KISTE2FELD-BILD FEHLER %d\n", fehler); return 1; }
    printf("KISTE2FELD-BILD OK\n");
    return 0;
}
