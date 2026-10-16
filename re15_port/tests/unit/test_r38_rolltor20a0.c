/* test_r38_rolltor20a0.c — Runde 38 Spur W: der Rolladen in ROOM20A0 steht im Port nicht mehr vor der Kamera.
 *
 * NUTZER (2026-10-16): "in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in
 * the camera. I cannot see anything... this is not like this in the original...."
 *
 * Dossier analysis/befunde_runde38/W_rolltor20a0.md; Code engine/src/objpoly_r38.c, include/re15_objpoly_r38.h,
 * platform/pc/src/objpoly_r38_pc.c, Haken in platform/pc/main.c (Prop-Schleife).
 *
 * MESSUNG AM ORIGINAL (DuckStation-GDB, Auslieferungs-EXE 0800e003 @0x80026e4c, W_belege/orig1_log.txt): ROOM20A0
 * Cut 11, Rolladen obj 1 offen (7942,-10800,-20510) ry 1024, Haltepunkte im Flach-Viereck-Zeichner FUN_80023098:
 * @0x80023190 (v0 = OPZ nach NCLIP, Prim-SXY0..2), @0x80023228 (v1 = OTZ nach AVSZ4, SXY3 = Prim+44),
 * @0x8002325c (in die OT eingehaengt). Die Tabelle k_orig unten ist dieses Log, Viereck fuer Viereck.
 *
 * R1  ORIGINAL-ENTSCHEID: fuer alle 48 Vierecke liefert re15_objpoly_r38_quad (der Weg der Prop-Schleife) dasselbe
 *     NCLIP-Vorzeichen, dieselbe OTZ-Schranke (eingehaengt ja/nein) und SXY innerhalb +-3 Pixel; die GPU-Dreiecke
 *     (0,1,2)/(1,2,3) bestehen die Groessengrenze genau dann, wenn sie es mit den Original-SXY tun.
 * R2  BILD: in Cut 11 mit offenem Rolladen bedeckt der Rolladen KEIN Bildpixel mehr (Original-Bild
 *     W_belege/orig_cut11_offen_buf0.png zeigt ihn nicht).
 * R3  NEGATIVKONTROLLE = der alte Port-Weg (Scheitel `_vz < 64` -> weg, sonst ungesaettigt, ohne NCLIP/GPU-Grenze)
 *     bedeckt in derselben Lage >= 85 % des Bilds (gemessen 88,2 %) — genau der Nutzer-Befund. Faellt der Fix zurueck, beschreibt R3
 *     wieder das Bild.
 * R4  KEINE NEBENWIRKUNG: in Cut 12 (der Rolladen von vorn, zu und offen) bedeckt der neue Weg dieselben Pixel wie
 *     der alte (die Vorderseite liegt vor den nun verworfenen Rueckseiten) — der Rolladen bleibt dort sichtbar.
 * R5  HALBVIERECK: Original Q07 — Dreieck (0,1,2) zu gross, (1,2,3) nicht (psx-spx Size Restriction je Dreieck).
 * R6  ELZA: ROOM20A1.RDT ist byte-gleich mit ROOM20A0.RDT (Record obj 1 @0x01932, Kamera Cut 11/12, Modell), also
 *     gilt alles oben auch fuer 20A1.
 *
 * Werkzeug-Modus fuer integration_r38_rolltor20a0:
 *   test_r38_rolltor20a0 --bild <framedump.ppm> <ROOMrrr.BSS> <cut> [toleranz=24]
 *   -> "BG-ANTEIL <anteil>" = Anteil der Bildpixel, deren Farbe hoechstens <toleranz> Stufen vom dekodierten
 *      Raumhintergrund des Cuts abweicht (BSS-Kette bss_common/bss_vlc/bss_mdec wie im Spiel, Scheibe cut*0x10000).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_camera.h"
#include "re15_skeleton.h"   /* re15_sin_q12 / re15_cos_q12 */
#include "re15_math.h"       /* re15_gte_divide */
#include "re15_bss.h"
#include "re15_objpoly_r38.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int g_fail = 0;
#define CHECK(cond, ...) do { \
    if (!(cond)) { g_fail++; printf("  FAIL: " __VA_ARGS__); printf("\n"); } \
} while (0)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b && out_size) *out_size = (size_t)sz;
    return b;
}

/* ---- Original-Messung (W_belege/orig1_log.txt): opz, otz (-1 = NCLIP-Abbruch), eingehaengt, SXY0..3 ------------ */
typedef struct { int32_t opz; int32_t otz; int eingehaengt; int16_t sxy[4][2]; } orig_quad_t;
static const orig_quad_t k_orig[48] = {
    {    -3521,    -1, 0, { {491,251}, {576,210}, {600,157}, {32767,32767} } },   /* Q00 */
    {     3410,   437, 1, { {630,166}, {710,131}, {508,262}, {593,217} } },   /* Q01 */
    {    -2177,    -1, 0, { {576,210}, {593,217}, {683,126}, {32767,32767} } },   /* Q02 */
    {    -1360,    -1, 0, { {593,217}, {576,210}, {508,262}, {32767,32767} } },   /* Q03 */
    {    -6175,    -1, 0, { {683,126}, {864,-16}, {600,157}, {32767,32767} } },   /* Q04 */
    {     7351,   322, 1, { {912,-18}, {710,131}, {883,-33}, {630,166} } },   /* Q05 */
    {    -4739,    -1, 0, { {683,126}, {710,131}, {864,-16}, {32767,32767} } },   /* Q06 */
    {        0,   111, 1, { {1023,-307}, {1023,-1024}, {1023,-579}, {1023,-1024} } },   /* Q07 */
    {   -15453,    -1, 0, { {864,-16}, {1023,-307}, {818,-29}, {32767,32767} } },   /* Q08 */
    {        0,    95, 1, { {1023,-1024}, {1023,-339}, {1023,-1024}, {1023,-695} } },   /* Q09 */
    {    39516,   206, 1, { {1023,-339}, {912,-18}, {1023,-695}, {883,-33} } },   /* Q10 */
    {        0,   151, 1, { {1023,-1024}, {1023,-1024}, {1023,-339}, {1023,-307} } },   /* Q11 */
    {   -13650,    -1, 0, { {864,-16}, {912,-18}, {1023,-307}, {32767,32767} } },   /* Q12 */
    {        0,    47, 0, { {1023,-1024}, {1023,-1024}, {1023,-1024}, {1023,-1024} } },   /* Q13 */
    {    -5323,    -1, 0, { {491,251}, {600,157}, {359,316}, {32767,32767} } },   /* Q14 */
    {     6374,   337, 1, { {630,166}, {508,262}, {481,231}, {374,333} } },   /* Q15 */
    {    -2681,    -1, 0, { {508,262}, {491,251}, {374,333}, {32767,32767} } },   /* Q16 */
    {   -15288,    -1, 0, { {600,157}, {818,-29}, {451,214}, {32767,32767} } },   /* Q17 */
    {    26600,   222, 1, { {883,-33}, {630,166}, {804,-76}, {481,231} } },   /* Q18 */
    {        0,    35, 0, { {1023,-579}, {1023,-1024}, {1023,-1024}, {1023,-1024} } },   /* Q19 */
    {   -70770,    -1, 0, { {818,-29}, {1023,-579}, {702,-63}, {32767,32767} } },   /* Q20 */
    {        0,    24, 0, { {1023,-1024}, {1023,-695}, {1023,-1024}, {1023,-1024} } },   /* Q21 */
    {    46060,   107, 1, { {1023,-695}, {883,-33}, {1023,-1024}, {804,-76} } },   /* Q22 */
    {        0,     0, 0, { {1023,-1024}, {1023,-1024}, {1023,-1024}, {1023,-1024} } },   /* Q23 */
    {   -56166,    -1, 0, { {-1024,1001}, {-382,678}, {104,346}, {32767,32767} } },   /* Q24 */
    {   -13166,    -1, 0, { {359,316}, {451,214}, {128,429}, {32767,32767} } },   /* Q25 */
    {   -34464,    -1, 0, { {-444,765}, {-382,678}, {-1024,1023}, {32767,32767} } },   /* Q26 */
    {    27970,   138, 1, { {-444,765}, {-1024,1023}, {131,461}, {105,395} } },   /* Q27 */
    {    20804,   238, 1, { {481,231}, {374,333}, {105,395}, {131,461} } },   /* Q28 */
    {   -28932,    -1, 0, { {-382,678}, {-444,765}, {128,429}, {32767,32767} } },   /* Q29 */
    {    -6051,    -1, 0, { {374,333}, {359,316}, {131,461}, {32767,32767} } },   /* Q30 */
    {  -964525,    -1, 0, { {-1024,-24}, {-1024,1001}, {-83,-289}, {32767,32767} } },   /* Q31 */
    {   -62987,    -1, 0, { {451,214}, {702,-63}, {104,346}, {32767,32767} } },   /* Q32 */
    {        0,    20, 0, { {-1024,1023}, {-1024,1001}, {-1024,54}, {-1024,-24} } },   /* Q33 */
    {  1094001,    44, 0, { {-1024,1023}, {-1024,54}, {105,395}, {-114,-344} } },   /* Q34 */
    {   368390,   122, 1, { {804,-76}, {481,231}, {-114,-344}, {105,395} } },   /* Q35 */
    {        0,     0, 0, { {-1024,-1024}, {-1024,-1024}, {-162,-1024}, {-162,-1024} } },   /* Q36 */
    {        0,     4, 0, { {1023,-1024}, {1023,-1024}, {-162,-1024}, {-162,-1024} } },   /* Q37 */
    {  -862000,    -1, 0, { {-1024,-1024}, {-1024,-24}, {-162,-1024}, {32767,32767} } },   /* Q38 */
    {  -826931,    -1, 0, { {702,-63}, {1023,-1024}, {-83,-289}, {32767,32767} } },   /* Q39 */
    {        0,     0, 0, { {-1024,-1024}, {-1024,-1024}, {-1024,-1024}, {-1024,-1024} } },   /* Q40 */
    {        0,     0, 0, { {-1024,54}, {-1024,-24}, {-1024,-1024}, {-1024,-1024} } },   /* Q41 */
    {        0,     0, 0, { {-1024,-1024}, {-1024,-1024}, {-114,-1024}, {-114,-1024} } },   /* Q42 */
    {        0,     0, 0, { {1023,-1024}, {1023,-1024}, {-114,-1024}, {-114,-1024} } },   /* Q43 */
    {   980980,     3, 0, { {-1024,54}, {-1024,-1024}, {-114,-344}, {-114,-1024} } },   /* Q44 */
    {  1077876,    32, 0, { {1023,-1024}, {804,-76}, {-114,-1024}, {-114,-344} } },   /* Q45 */
    {        0,     0, 0, { {-1024,-1024}, {-1024,-1024}, {-114,-1024}, {-162,-1024} } },   /* Q46 */
    {        0,     0, 0, { {1023,-1024}, {1023,-1024}, {-162,-1024}, {-114,-1024} } },   /* Q47 */
};

/* ---- Prop-Lage wie die Prop-Schleife (main.c pc_prop_rot_q12 mit rx = rz = 0) ---------------------------------- */
static void prop_rot(int16_t ry, int32_t m[9])
{
    const int32_t s = re15_sin_q12(ry), c = re15_cos_q12(ry);
    m[0] = c;  m[1] = 0;    m[2] = s;
    m[3] = 0;  m[4] = 4096; m[5] = 0;
    m[6] = -s; m[7] = 0;    m[8] = c;
}

typedef struct {
    re15_rdt_t rdt;
    re15_md1_t md1;
    uint8_t   *buf;
} raum_t;

static int raum_laden(raum_t *r, const char *datei)
{
    char path[512];
    snprintf(path, sizeof path, "%s/STAGE2/%s", RE15_XSTR(RE15_ASSETS_PATH), datei);
    size_t sz = 0;
    r->buf = read_file(path, &sz);
    if (!r->buf) { printf("  kann %s nicht lesen\n", path); return 0; }
    memset(&r->rdt, 0, sizeof r->rdt);
    if (re15_rdt_parse(r->buf, sz, &r->rdt) != 0) return 0;
    if (r->rdt.prop_count < 2 || !r->rdt.prop_md1[1]) return 0;
    return re15_md1_parse(r->rdt.prop_md1[1], r->rdt.prop_md1_size[1], &r->md1) == 0;
}

/* Kamera x Objekt fuer obj 1 (Lage (7942,py,-20510), ry 1024) in Cut cut — wie re15_camera_compose_view_bone in
 * der Prop-Schleife. */
static int matrix(const raum_t *r, int cut, int32_t py, int32_t bm[9], int32_t bt[3], int *h)
{
    re15_camera_view_t view;
    if (cut >= r->rdt.cut_count || re15_camera_build_view(&r->rdt.cuts[cut], &view) != 0) return 0;
    int32_t rot[9], t[3] = { 7942, py, -20510 };   /* Obj_model_set @0x01932, y nach sub07/sub00 */
    prop_rot(1024, rot);
    re15_camera_compose_view_bone(&view, rot, t, bm, bt);
    *h = view.fov_screen_dist;
    return 1;
}

static void sichtraum(const re15_md1_vertex_t *v, const int32_t bm[9], const int32_t bt[3], int32_t o[3])
{
    const int64_t x = v->x, y = v->y, z = v->z;
    o[0] = (int32_t)((x * bm[0] + y * bm[1] + z * bm[2]) >> 12) + bt[0];
    o[1] = (int32_t)((x * bm[3] + y * bm[4] + z * bm[5]) >> 12) + bt[1];
    o[2] = (int32_t)((x * bm[6] + y * bm[7] + z * bm[8]) >> 12) + bt[2];
}

/* ---- Bildbedeckung (Pixelmitten im 320x240-Bild) --------------------------------------------------------------- */
static uint8_t s_bedeckt[240][320];
static int64_t kante(int ax, int ay, int bx, int by, int px, int py)
{
    return (int64_t)(bx - ax) * (py - ay) - (int64_t)(by - ay) * (px - ax);
}
static void dreieck_fuellen(int x0, int y0, int x1, int y1, int x2, int y2)
{
    int mnx = x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int mxx = x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int mny = y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int mxy = y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);
    if (mnx < 0) mnx = 0;
    if (mny < 0) mny = 0;
    if (mxx > 319) mxx = 319;
    if (mxy > 239) mxy = 239;
    for (int y = mny; y <= mxy; y++)
        for (int x = mnx; x <= mxx; x++) {
            const int64_t a = kante(x0, y0, x1, y1, x, y), b = kante(x1, y1, x2, y2, x, y);
            const int64_t c = kante(x2, y2, x0, y0, x, y);
            if ((a > 0 && b > 0 && c > 0) || (a < 0 && b < 0 && c < 0)) s_bedeckt[y][x] = 1;
        }
}
static int bedeckt_zaehlen(void)
{
    int n = 0;
    for (int y = 0; y < 240; y++) for (int x = 0; x < 320; x++) n += s_bedeckt[y][x];
    return n;
}

/* Neuer Weg (wie main.c): re15_objpoly_r38_quad_mesh, dann die GPU-Dreiecke; voll = PC-Teilung (0,1,3)+(0,3,2),
 * halb = nur das bestandene PSX-Dreieck (objpoly_r38_pc.c). */
static int bedeckung_neu(const raum_t *r, int cut, int32_t py)
{
    int32_t bm[9], bt[3]; int h = 0;
    if (!matrix(r, cut, py, bm, bt, &h)) return -1;
    memset(s_bedeckt, 0, sizeof s_bedeckt);
    const re15_md1_mesh_t *m = &r->md1.meshes[0];
    for (int q = 0; q < m->quad_count; q++) {
        const re15_md1_quad_t *qd = &m->quads[q];
        const re15_md1_vertex_t *vp[4] = { &m->quad_vertices[qd->v0], &m->quad_vertices[qd->v1],
                                           &m->quad_vertices[qd->v2], &m->quad_vertices[qd->v3] };
        int ax[4], ay[4], wz = 0; uint8_t ok[2];
        if (!re15_objpoly_r38_quad_mesh(vp, bm, bt, h, 160, 120, ax, ay, &wz, ok)) continue;
        if (ok[0] && ok[1]) {
            dreieck_fuellen(ax[0], ay[0], ax[1], ay[1], ax[3], ay[3]);
            dreieck_fuellen(ax[0], ay[0], ax[3], ay[3], ax[2], ay[2]);
        } else {
            for (int t = 0; t < 2; t++)
                if (ok[t]) dreieck_fuellen(ax[t], ay[t], ax[t + 1], ay[t + 1], ax[t + 2], ay[t + 2]);
        }
    }
    return bedeckt_zaehlen();
}

/* Alter Port-Weg (bis df08d83a, main.c "H28 near-clip"): je Scheitel `_vz < 64` -> weg, sonst ungesaettigt. */
static int bedeckung_alt(const raum_t *r, int cut, int32_t py)
{
    int32_t bm[9], bt[3]; int h = 0;
    if (!matrix(r, cut, py, bm, bt, &h)) return -1;
    memset(s_bedeckt, 0, sizeof s_bedeckt);
    const re15_md1_mesh_t *m = &r->md1.meshes[0];
    for (int q = 0; q < m->quad_count; q++) {
        const re15_md1_quad_t *qd = &m->quads[q];
        const re15_md1_vertex_t *vp[4] = { &m->quad_vertices[qd->v0], &m->quad_vertices[qd->v1],
                                           &m->quad_vertices[qd->v2], &m->quad_vertices[qd->v3] };
        int ax[4], ay[4], ok = 1;
        for (int v = 0; v < 4; v++) {
            int32_t s[3];
            sichtraum(vp[v], bm, bt, s);
            if (s[2] < 64) { ok = 0; break; }
            const int32_t ir1 = s[0] > 0x7FFF ? 0x7FFF : (s[0] < -0x8000 ? -0x8000 : s[0]);
            const int32_t ir2 = s[1] > 0x7FFF ? 0x7FFF : (s[1] < -0x8000 ? -0x8000 : s[1]);
            const uint32_t n = re15_gte_divide((uint32_t)h, (uint32_t)(s[2] > 0xFFFF ? 0xFFFF : s[2]));
            ax[v] = 160 + (int)(((int64_t)ir1 * (int64_t)n) >> 16);
            ay[v] = 120 + (int)(((int64_t)ir2 * (int64_t)n) >> 16);
        }
        if (!ok) continue;
        dreieck_fuellen(ax[0], ay[0], ax[1], ay[1], ax[3], ay[3]);
        dreieck_fuellen(ax[0], ay[0], ax[3], ay[3], ax[2], ay[2]);
    }
    return bedeckt_zaehlen();
}

/* ---- Werkzeug-Modus --bild ------------------------------------------------------------------------------------ */
static int bild_modus(const char *ppm, const char *bss, int cut, int tol)
{
    size_t psz = 0, bsz = 0;
    uint8_t *p = read_file(ppm, &psz), *b = read_file(bss, &bsz);
    if (!p || !b) { printf("BILD-FEHLER Datei\n"); return 2; }
    int w = 0, hh = 0, mx = 0, off = 0;
    if (sscanf((const char *)p, "P6 %d %d %d%n", &w, &hh, &mx, &off) != 3 || w != 320 || hh != 240 ||
        psz < (size_t)off + 1 + 320 * 240 * 3) {
        printf("BILD-FEHLER PPM %dx%d\n", w, hh); return 2;
    }
    const uint8_t *px = p + off + 1;
    if ((size_t)cut * 0x10000 + 0x10000 > bsz) { printf("BILD-FEHLER Cut\n"); return 2; }
    re15_bss_chunk_t ch;
    if (!re15_bss_parse_chunk(b + (size_t)cut * 0x10000, 0x10000, &ch) || !re15_bss_chunk_has_video(&ch)) {
        printf("BILD-FEHLER BSS\n"); return 2;
    }
    size_t cap = ((size_t)ch.run_length_words + 2) * 8 + 4096;
    int16_t *co = (int16_t *)malloc(cap * sizeof(int16_t));
    uint8_t *bg = (uint8_t *)malloc(320 * 240 * 3);
    int n = re15_bss_vlc_decode(ch.vlc_payload, ch.vlc_payload_size, ch.run_length_words, ch.quant, ch.version, co, cap);
    if (n < 0 || re15_bss_mdec_decode(co, (size_t)n, 320, 240, bg) != 0) { printf("BILD-FEHLER MDEC\n"); return 2; }
    int gleich = 0;
    for (int i = 0; i < 320 * 240; i++) {
        int d = 0;
        for (int k = 0; k < 3; k++) { int e = abs((int)px[i * 3 + k] - (int)bg[i * 3 + k]); if (e > d) d = e; }
        gleich += (d <= tol);
    }
    printf("BG-ANTEIL %.4f\n", gleich / (320.0 * 240.0));
    free(co); free(bg); free(p); free(b);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 5 && strcmp(argv[1], "--bild") == 0)
        return bild_modus(argv[2], argv[3], atoi(argv[4]), argc >= 6 ? atoi(argv[5]) : 24);

    raum_t a, e;
    CHECK(raum_laden(&a, "ROOM20A0.RDT"), "ROOM20A0.RDT / obj-1-Modell nicht geladen");
    if (g_fail) return 1;
    CHECK(a.md1.mesh_count == 1 && a.md1.meshes[0].quad_count == 48 && a.md1.meshes[0].triangle_count == 0,
          "obj 1 = 1 Netz, 48 Vierecke, 0 Dreiecke (ist %d/%d/%d)", a.md1.mesh_count,
          a.md1.meshes[0].quad_count, a.md1.meshes[0].triangle_count);

    /* R1: Original-Entscheid je Viereck, Cut 11, Rolladen offen */
    {
        int32_t bm[9], bt[3]; int h = 0;
        CHECK(matrix(&a, 11, -10800, bm, bt, &h) && h == 208, "Cut 11: H = fov>>7 = 208 (ist %d)", h);
        const re15_md1_mesh_t *m = &a.md1.meshes[0];
        int n_nclip = 0, n_otz = 0, n_ein = 0, max_d = 0;
        for (int q = 0; q < 48 && q < m->quad_count; q++) {
            const re15_md1_quad_t *qd = &m->quads[q];
            const re15_md1_vertex_t *vp[4] = { &m->quad_vertices[qd->v0], &m->quad_vertices[qd->v1],
                                               &m->quad_vertices[qd->v2], &m->quad_vertices[qd->v3] };
            int32_t mac[4][3];
            for (int v = 0; v < 4; v++) sichtraum(vp[v], bm, bt, mac[v]);
            re15_objpoly_t p;
            const int ein = re15_objpoly_r38_quad(mac, h, 160, 120, &p);
            int ax[4], ay[4], wz; uint8_t ok[2];
            const int ein2 = re15_objpoly_r38_quad_mesh(vp, bm, bt, h, 160, 120, ax, ay, &wz, ok);
            CHECK(ein == ein2, "Q%02d: quad und quad_mesh uneinig", q);
            const orig_quad_t *o = &k_orig[q];
            CHECK((p.opz >= 0) == (o->opz >= 0), "Q%02d: NCLIP Port opz %d, Original %d", q, (int)p.opz, (int)o->opz);
            const int port_eingehaengt = (p.opz >= 0) && p.otz >= RE15_OBJPOLY_OTZ_MIN;
            CHECK(port_eingehaengt == o->eingehaengt, "Q%02d: eingehaengt Port %d (otz %d), Original %d (otz %d)", q,
                  port_eingehaengt, (int)p.otz, o->eingehaengt, (int)o->otz);
            if (o->opz < 0) n_nclip++;
            else if (!o->eingehaengt) n_otz++;
            else n_ein++;
            const int nv = (o->sxy[3][0] == 0x7fff) ? 3 : 4;
            for (int v = 0; v < nv; v++) {
                int d = abs(p.sx[v] - o->sxy[v][0]);
                if (abs(p.sy[v] - o->sxy[v][1]) > d) d = abs(p.sy[v] - o->sxy[v][1]);
                if (d > max_d) max_d = d;
            }
            if (nv == 4 && o->eingehaengt) {
                for (int t = 0; t < 2; t++) {
                    const int og = re15_objpoly_r38_gpu_ok(o->sxy[t][0], o->sxy[t][1], o->sxy[t + 1][0],
                                                           o->sxy[t + 1][1], o->sxy[t + 2][0], o->sxy[t + 2][1]);
                    CHECK(og == p.tri_ok[t], "Q%02d T%d: GPU-Grenze Port %d, mit Original-SXY %d", q, t, p.tri_ok[t], og);
                }
            }
            if (ein && ein == ein2) CHECK(ok[0] == p.tri_ok[0] && ok[1] == p.tri_ok[1], "Q%02d: tri_ok uneinig", q);
        }
        CHECK(n_nclip == 20 && n_otz == 16 && n_ein == 12, "Original-Tabelle: NCLIP 20 / OTZ<64 16 / eingehaengt 12 "
              "(ist %d/%d/%d)", n_nclip, n_otz, n_ein);
        CHECK(max_d <= 3, "SXY Port gegen Original: max Abweichung %d > 3", max_d);
        printf("R1 Cut 11 offen: 48 Vierecke wie im Original (NCLIP %d, OTZ<64 %d, eingehaengt %d), max |dSXY| %d\n",
               n_nclip, n_otz, n_ein, max_d);
    }

    /* R2 + R3: Bildbedeckung Cut 11, neu gegen alt */
    {
        const int neu = bedeckung_neu(&a, 11, -10800), alt = bedeckung_alt(&a, 11, -10800);
        printf("R2/R3 Cut 11 offen: Rolladen bedeckt neu %d Pixel, alt %d von 76800 (%.1f %%)\n", neu, alt,
               alt * 100.0 / 76800.0);
        CHECK(neu == 0, "R2: der offene Rolladen bedeckt in Cut 11 noch %d Pixel (Original: keinen)", neu);
        /* gemessen 2026-10-16: alter Weg 67728 von 76800 Pixeln (88,2 %; frei bleibt nur die linke Wand, vgl.
         * W_belege/vorher_cut11_F711_offen.png) */
        CHECK(alt >= 76800 * 85 / 100, "R3: alter Weg bedeckt nur %d Pixel (< 85 %%) — Negativkontrolle beschreibt den "
              "Befund nicht mehr", alt);
        const int neu_zu = bedeckung_neu(&a, 11, -7200);
        CHECK(neu_zu == 0, "R2: auch der geschlossene Rolladen bedeckt in Cut 11 %d Pixel", neu_zu);
    }

    /* R4: Cut 12 (Rolladen von vorn) unveraendert sichtbar */
    {
        const int32_t ys[2] = { -7200, -10800 };
        for (int k = 0; k < 2; k++) {
            const int neu = bedeckung_neu(&a, 12, ys[k]), alt = bedeckung_alt(&a, 12, ys[k]);
            printf("R4 Cut 12 y %d: bedeckt neu %d, alt %d\n", (int)ys[k], neu, alt);
            CHECK(neu > 5000, "R4: Cut 12 y %d — der Rolladen ist von vorn kaum noch sichtbar (%d Pixel)", (int)ys[k], neu);
            CHECK(abs(neu - alt) <= alt / 100, "R4: Cut 12 y %d — neue Bedeckung %d weicht > 1 %% von %d ab", (int)ys[k],
                  neu, alt);
        }
    }

    /* R5: Halbviereck (Original Q07: SXY (1023,-307) (1023,-1024) (1023,-579) (1023,-1024)) */
    {
        CHECK(!re15_objpoly_r38_gpu_ok(1023, -307, 1023, -1024, 1023, -579), "R5: Q07 T0 (dy 717) muss zu gross sein");
        CHECK(re15_objpoly_r38_gpu_ok(1023, -1024, 1023, -579, 1023, -1024), "R5: Q07 T1 (dy 445) muss bestehen");
        CHECK(re15_objpoly_r38_gpu_ok(0, 0, 1023, 0, 0, 511) && !re15_objpoly_r38_gpu_ok(0, 0, 1024, 0, 0, 0) &&
              !re15_objpoly_r38_gpu_ok(0, 0, 0, 512, 0, 0), "R5: Grenze 1023 x 511 (psx-spx)");
        printf("R5 GPU-Grenze je Dreieck: Q07 T0 zu gross, T1 besteht\n");
    }

    /* R6: Elza ROOM20A1 */
    {
        CHECK(raum_laden(&e, "ROOM20A1.RDT"), "ROOM20A1.RDT nicht geladen");
        size_t za = 0, ze = 0;
        char pa[512], pe[512];
        snprintf(pa, sizeof pa, "%s/STAGE2/ROOM20A0.RDT", RE15_XSTR(RE15_ASSETS_PATH));
        snprintf(pe, sizeof pe, "%s/STAGE2/ROOM20A1.RDT", RE15_XSTR(RE15_ASSETS_PATH));
        uint8_t *ba = read_file(pa, &za), *be = read_file(pe, &ze);
        CHECK(ba && be && za == ze && memcmp(ba, be, za) == 0, "R6: ROOM20A1.RDT ist nicht byte-gleich mit ROOM20A0.RDT");
        if (!g_fail) {
            const int neu = bedeckung_neu(&e, 11, -10800);
            CHECK(neu == 0, "R6: 20A1 Cut 11 offen bedeckt %d Pixel", neu);
            printf("R6 ROOM20A1 = ROOM20A0 (%u Bytes), Cut 11 offen bedeckt %d Pixel\n", (unsigned)za, neu);
        }
        free(ba); free(be);
    }

    if (g_fail) { printf("test_r38_rolltor20a0: %d FEHLER\n", g_fail); return 1; }
    printf("test_r38_rolltor20a0: OK\n");
    return 0;
}
