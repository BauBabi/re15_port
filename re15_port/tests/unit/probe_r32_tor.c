/* probe_r32_tor.c — Riegel Runde 32: das Gelaendertor ROOM1170 ist so hell wie das gemalte Tor.
 *
 * Nutzer-Befund 2026-09-29: "Das von uns erstellte Tor in ROOM 1170 sieht gut aus, aber ist zu
 * dunkel." Dossier: analysis/befunde_runde32/tor_helligkeit.md. Gemessen vorher: gezeigt / gemalt
 * an denselben Stellen 0,52 (1,92fach zu dunkel) = Tuerlicht 73/128 x 5-Bit-Abschneiden 0,914.
 *
 * Teil "archiv" (nur Engine, add_test unit_r32_tor_hell):
 *   A  Die Tormaschine setzt in beiden Varianten Fluegel (Obj 0) und Pfosten (Obj 1) mit
 *      Objekt-Flag 0x1000 auf (Bild 100 nach dem Einblenden): Fluegel 0x1a80, Pfosten 0x1280.
 *      RE2 waehlt damit BK 136 statt 68: @0x800142ac lhu v0,304(s1); @0x800142b4 andi v0,v0,0x1000;
 *      @0x800142bc addiu a3,zero,136; @0x800142e8 addiu a3,zero,68.
 *   B  Vorbild DOOR2B.DO2 @Datei 0x5022 = 4d 00 00 00 01 00 80 1a (Blatt flags 0x1a80): das
 *      Torblatt traegt genau dieses Flagwort.
 *   C  Eckfarbe der Schild-Normale mit dem PORT-Lichtcode (re15_light_shade_vertex, Kontext wie
 *      door_scene_pc.c: L @0x8009a470, LCM 1600 @0x8009a490, BK nach Flag, gedreht mit der Matrix
 *      des VORIGEN Bildes): 107 in V0 und V1; Gegenprobe BK 68 -> 73 (08_re_zeichnen.md 4.3).
 *   D  Tortextur (TIM im Archiv, 5 Bit << 3 wie re15_tim_rgb555_to_argb8888): mittlere Helligkeit
 *      der Schild-Texel (128 x 78, Bereich tor_modell.BEREICH["schild"]) x 107/128 = gemaltes
 *      Schild-Mittel 33,41 (Cut-12-Pixel je Texel, tor_helligkeit.py gemalt) auf 3 % genau.
 *      Gegenprobe altes Archiv (gemessen): Texel 30,03 x 107/128 -> 0,751 (ROT); mit dem damals
 *      wirksamen 73/128 waren es 0,5125.
 *
 * Teil "bild <ppm>" (Pruefer fuer integration_r32_tor_hell, echte exe, RE15_WINDOW_SCALE=1,
 * RE15_TUER_TEST=1, Bild 100 = b_tuer_100.ppm):
 *   R1 Schild-Rechteck x 129..170, y 192..220 (1218 Punkte, alle Ecken c = 107): Mittel Y gegen das
 *      gemalte Tor an denselben Stellen P = 28,514 (tor_helligkeit.py, Raster 1x): |F/P - 1| <= 3 %.
 *      Gemessen nachher 28,754 (1,0084); Gegenprobe altes Archiv 14,348 (0,5032) -> ROT.
 *   R2 Rohrflaeche mit Eckfarbe 148 > 0x80 (y 152, x 126..133): F / Texel >= 1,10. Texel T = 99,8
 *      (Mittel), PSX: T * 148/128 = 115,4 (psx-spx GPU:349-354, 1438-1446), PC-Kappung vorher: T * 1,0.
 *      Gemessen nachher 115,26 (1,155); Gegenprobe ohne die Zusatzlage in door_scene_pc.c tri_psx
 *      99,78 (0,9998) -> ROT, altes Archiv 71,23 (0,714) -> ROT.
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_door_seq.h"
#include "re15_light.h"
#include "re15_tim.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static const int16_t L_TUER[9] = { 400, 800, -500, -1800, -1000, -2700, 3500, 6700, 1200 }; /* @0x8009a470 */
#define LCM_TUER 1600                                                                       /* @0x8009a490 */
#define GEMALT_SCHILD_Y 33.41   /* tor_helligkeit.py gemalt: textur_schild_gemalt_lum */

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* Eckfarbe wie door_scene_pc.c mesh_zeichnen (Kontext von Hand, 08_re_zeichnen.md 4.2 Schritt 2). */
static uint8_t eckfarbe(const re15_door_mat_t *welt_vor, int bk, int16_t nx, int16_t ny, int16_t nz)
{
    re15_actor_lightctx_t w, ctx;
    memset(&w, 0, sizeof w);
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++) { w.L[i][k] = L_TUER[i * 3 + k]; w.C[i][k] = LCM_TUER; }
    w.ambient[0] = w.ambient[1] = w.ambient[2] = (uint8_t)bk;
    w.active_lights = 3;
    int32_t wv[9];
    for (int i = 0; i < 9; i++) wv[i] = welt_vor->m[i];
    re15_light_ctx_rotate_for_bone(&w, wv, &ctx);
    uint8_t r, g, b;
    re15_light_shade_vertex(&ctx, nx, ny, nz, &r, &g, &b);
    CHECK(r == g && g == b, "Eckfarbe nicht grau (%u,%u,%u)", r, g, b);
    return r;
}

static double y_von(uint16_t c)
{
    uint32_t a = re15_tim_rgb555_to_argb8888(c);
    return 0.299 * ((a >> 16) & 0xFF) + 0.587 * ((a >> 8) & 0xFF) + 0.114 * (a & 0xFF);
}

static int teil_archiv(void)
{
    int n = 0;
    const uint8_t *teil = re15_door_seq_archiv(RE15_DOOR_ARCHIV_TOR1170, &n);
    CHECK(teil && n > 0, "Torarchiv fehlt");
    if (!teil) return 1;

    /* B: Vorbild DOOR2B */
    char pfad[512];
    snprintf(pfad, sizeof pfad, "%s/DOOR2B.DO2", RE15_RE2_DOOR_DIR);
    size_t n2b = 0;
    uint8_t *d2b = read_file(pfad, &n2b);
    CHECK(d2b && n2b > 0x5022 + 8, "%s nicht lesbar", pfad);
    static const uint8_t soll[8] = { 0x4d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x80, 0x1a };
    uint16_t flags_2b = 0;
    if (d2b && n2b > 0x5022 + 8) {
        CHECK(memcmp(d2b + 0x5022, soll, 8) == 0, "DOOR2B @0x5022 ist nicht 4d 00 00 00 01 00 80 1a");
        flags_2b = (uint16_t)(d2b[0x5022 + 6] | (d2b[0x5022 + 7] << 8));
    }
    free(d2b);
    printf("B  DOOR2B @0x5022 Blatt-Flags 0x%04x\n", flags_2b);

    for (int var = 0; var < 2; var++) {
        static re15_door_seq_t s;
        CHECK(re15_door_seq_start(&s, teil, n, var, 0, 0) == 0, "V%d: Start", var);
        for (int b = 0; b <= 100 && re15_door_seq_bild(&s, 1); b++) { }
        const re15_door_obj_t *fl = &s.obj[0], *pf = &s.obj[1];
        /* A */
        printf("A  V%d Bild %d: Fluegel on %u flags 0x%04x, Pfosten on %u flags 0x%04x\n",
               var, s.bild, fl->on, fl->flags, pf->on, pf->flags);
        CHECK(fl->on && fl->flags == 0x1a80, "V%d Fluegel flags 0x%04x statt 0x1a80", var, fl->flags);
        CHECK(pf->on && pf->flags == 0x1280, "V%d Pfosten flags 0x%04x statt 0x1280", var, pf->flags);
        CHECK(fl->flags == flags_2b, "V%d Torblatt-Flags 0x%04x != DOOR2B 0x%04x", var, fl->flags, flags_2b);

        /* C: Schild-Normalen im Fluegel-Mesh (UV v < 78 = Bereich "schild"), gezeichnete Seite */
        const re15_md1_mesh_t *m = &s.md1.meshes[fl->mesh];
        int bk = (fl->flags & 0x1000) ? 136 : 68;    /* wie door_scene_pc.c mesh_zeichnen */
        int gefunden = 0, c107 = 0;
        for (int t = 0; t < m->triangle_count; t++) {
            const re15_md1_tri_uv_t *uv = &m->triangle_uvs[t];
            if (uv->v0 >= 78 || uv->v1 >= 78 || uv->v2 >= 78) continue;
            const re15_md1_vertex_t *nv = &m->tri_normals[m->triangles[t].n0];
            /* Blickrichtung: W_vor * n, sichtbar wenn z der Sichtnormale < 0 (zur Kamera) */
            int32_t zn = ((int32_t)fl->welt_vor.m[6] * nv->x + (int32_t)fl->welt_vor.m[7] * nv->y
                        + (int32_t)fl->welt_vor.m[8] * nv->z) >> 12;
            if (zn >= 0) continue;
            gefunden++;
            uint8_t c = eckfarbe(&fl->welt_vor, bk, nv->x, nv->y, nv->z);
            uint8_t c68 = eckfarbe(&fl->welt_vor, 68, nv->x, nv->y, nv->z);
            if (c == 107) c107++;
            CHECK(c == 107, "V%d Schild-Dreieck %d: Eckfarbe %u statt 107 (BK %d)", var, t, c, bk);
            CHECK(c68 == 73, "V%d Schild-Dreieck %d: Gegenprobe BK 68 %u statt 73", var, t, c68);
        }
        printf("C  V%d: %d Schild-Dreiecke zur Kamera, %d mit Eckfarbe 107 (BK %d)\n", var, gefunden, c107, bk);
        CHECK(gefunden >= 2, "V%d: keine Schild-Dreiecke zur Kamera", var);

        /* D: Texturhelligkeit */
        if (var == 0) {
            CHECK(s.tim_ok && s.tim.bpp == 8 && s.tim.width == 128, "TIM nicht 8 bit / 128 breit");
            if (s.tim_ok && s.tim.bpp == 8) {
                const uint8_t *px = (const uint8_t *)s.tim.pixels;
                double sum = 0.0;
                for (int y = 0; y < 78; y++)
                    for (int x = 0; x < 128; x++) {
                        uint16_t c = s.tim.clut[px[y * s.tim.width + x]];
                        sum += (c == 0x8000) ? 0.0 : y_von(c);
                    }
                double texel = sum / (78.0 * 128.0);
                double gezeigt = texel * 107.0 / 128.0;
                double q = gezeigt / GEMALT_SCHILD_Y;
                printf("D  Schild-Texel Y %.3f x 107/128 = %.3f, gemalt %.2f -> %.4f\n",
                       texel, gezeigt, GEMALT_SCHILD_Y, q);
                CHECK(q > 0.97 && q < 1.03, "Schild gezeigt/gemalt %.4f ausserhalb 0,97..1,03", q);
            }
        }
        re15_door_seq_ende(&s);
    }
    return fails;
}

/* ---- Pruefer fuer das Serienbild der echten exe -------------------------------------- */
static int ppm_lesen(const char *pfad, int *w, int *h, uint8_t **rgb)
{
    size_t n = 0;
    uint8_t *d = read_file(pfad, &n);
    if (!d) return -1;
    int mx = 0, off = 0;
    if (sscanf((const char *)d, "P6 %d %d %d%n", w, h, &mx, &off) != 3 || mx != 255) { free(d); return -1; }
    off++;   /* ein Trennzeichen nach maxval */
    if ((size_t)(off + *w * *h * 3) > n) { free(d); return -1; }
    *rgb = (uint8_t *)malloc((size_t)(*w * *h * 3));
    memcpy(*rgb, d + off, (size_t)(*w * *h * 3));
    free(d);
    return 0;
}

static double y_px(const uint8_t *rgb, int w, int x, int y)
{
    const uint8_t *p = rgb + (y * w + x) * 3;
    return 0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2];
}

static int teil_bild(const char *pfad)
{
    int w = 0, h = 0;
    uint8_t *rgb = NULL;
    CHECK(ppm_lesen(pfad, &w, &h, &rgb) == 0, "%s nicht lesbar (P6)", pfad);
    if (!rgb) return fails;
    CHECK(w == 320 && h == 240, "%s: %dx%d statt 320x240 (RE15_WINDOW_SCALE=1)", pfad, w, h);
    if (w == 320 && h == 240) {
        double s = 0.0; int n = 0;
        for (int y = 192; y <= 220; y++)
            for (int x = 129; x <= 170; x++) { s += y_px(rgb, w, x, y); n++; }
        double r1 = s / n, q1 = r1 / 28.514;
        printf("R1 Schild x129..170 y192..220: F %.3f, gemalt 28,514 -> %.4f\n", r1, q1);
        CHECK(q1 > 0.97 && q1 < 1.03, "R1 gezeigt/gemalt %.4f ausserhalb 0,97..1,03", q1);
        s = 0.0; n = 0;
        for (int x = 126; x <= 133; x++) { s += y_px(rgb, w, x, 152); n++; }
        double r2 = s / n, q2 = r2 / 99.8;
        printf("R2 Rohr c=148 y152 x126..133: F %.2f, Texel 99,8 -> %.4f (PSX 1,156, PC-Kappung 1,000)\n", r2, q2);
        CHECK(q2 >= 1.10 && q2 <= 1.20, "R2 F/Texel %.4f ausserhalb 1,10..1,20", q2);
    }
    free(rgb);
    return fails;
}

int main(int argc, char **argv)
{
    if (argc >= 3 && !strcmp(argv[1], "bild")) teil_bild(argv[2]);
    else teil_archiv();
    if (fails) { fprintf(stderr, "probe_r32_tor: %d Fehler\n", fails); return 1; }
    printf("probe_r32_tor: OK\n");
    return 0;
}
