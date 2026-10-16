/* probe_r38_cursor.c — Runde 38 Spur A "cursor": RIEGEL gegen den "komischen Schatten" des Cursors.
 *
 * Nutzer (AUFTRAG.md Runde 38, woertlich): "Der Cursor, der zum Beispiel in ROOM 1150 oder ROOM 11F0 verwendet wird,
 * hat immer noch so einen komischen Schatten. Den hat er im Original nicht..."
 * Dossier: analysis/befunde_runde38/A_cursor.md. Mechanik: include/re15_nclip_r38.h.
 *
 * BEFUND: der Cursor ist ein Kasten; Oberseite y=-900 (29 Vierecke mit Muster) und UNTERSEITE y=+923 (4 Dreiecke mit
 * demselben Muster). Der PC-Zeichner reihte die rueckseitige Unterseite ein -> dunkle, kleinere Kopie von Eckwinkeln
 * und Kreuz durch die durchsichtige Oberseite. Das Original verwirft jede Objekt-Flaeche mit NCLIP-MAC0 < 0
 * (FUN_800254a0 @0x800255a8, FUN_800256b0 @0x800257c0 `bgez`; FUN_80022f0c @0x80023010 `bltz`; FUN_80023098 @0x80023190).
 *
 * Teile (je ein add_test, probes/r38_cursor.cmake):
 *   nclip   Praedikat: MAC0 woertlich nach psx-spx (NCLIP), < 0 verworfen, = 0 gezeichnet, Saettigung -0x400..+0x3FF
 *           (ein Fall, in dem NUR die Saettigung das Vorzeichen kippt), Zaehler.
 *   kasten  ROOM11F0-Cursor (MD1 @0x001928, TIM @0x018DAC) unter Kamera Cut 10 (re15_hebetisch_cursor_matrix =
 *           dieselbe Abbildung wie die Prop-Schleife, Runde 34 Nacht pixelgleich gemessen) an 25 Lagen: die 29
 *           Muster-Vierecke oben werden gezeichnet, die 4 Muster-Dreiecke unten verworfen; GEGENPROBE: ohne Verwurf
 *           laegen die 4 Unterseiten-Dreiecke innerhalb des Oberseiten-Umrisses (= der gemeldete Schatten).
 *   raeume  Zensus aller 23 Cursor-Raeume (Runde 34 Nacht §3.4 + Varianten; ROOM30E1 hat keinen Cursor): Record
 *           `2d <obj> 04` mit Box (0,-900,0,900,900,900) am Zensus-Offset, Modell aus der Prop-Tabelle, 29 Muster-
 *           Vierecke oben + 4 Muster-Dreiecke unten, Umlaufsinn der Unterseite entgegengesetzt zur Oberseite und die
 *           Oberseite gleichsinnig mit ROOM11F0 -> wo die Oberseite sichtbar ist, verwirft NCLIP die Unterseite.
 *   bild    Auswertung eines exe-Laufs (integration_r38_cursor): <ppm> <trilog> <slot> [psx11f0]
 *           - RE15_TRILOG: kein eingereihtes Dreieck des Cursor-Platzes mit NCLIP < 0 (Mechanik),
 *           - Bild: im Inneren des Eckwinkel-Rahmens kein reingruener Bildpunkt (R,B <= 12, G >= 15, G-R/G-B >= 12;
 *             die Hintergrund-Rasterung des Originals (8,16,8) @(151,110) faellt nicht darunter),
 *           - psx11f0: Klassenkarte G/g/Y/y im Fenster x 140..179, y 100..137 gleich dem Original
 *             (analysis/grundwahrheit/psx_room11f0_cut10.png, DuckStation, Auslieferungs-EXE @0x80026e4c 0800e003).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_nclip_r38.h"
#include "re15_md1.h"
#include "re15_hebetisch_cursor.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int s_fail = 0;
#define CHECK(name, cond) do { \
    if (cond) { printf("  OK   %s\n", (name)); } \
    else      { printf("  FAIL %s\n", (name)); s_fail++; } } while (0)

static uint8_t *slurp(const char *p, long *n)
{
    FILE *f = fopen(p, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz + 1);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) { b[sz] = 0; *n = sz; }
    return b;
}

static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24; }
static int16_t  rd16s(const uint8_t *p) { return (int16_t)(p[0] | p[1] << 8); }

/* ---------------------------------------------------------------- nclip */
static int teil_nclip(void)
{
    printf("[nclip] Praedikat (psx-spx NCLIP, Verwurf MAC0 < 0)\n");
    const unsigned z0 = g_re15_nclip_r38_verworfen;
    CHECK("(0,0)(10,0)(0,10): MAC0 = +100, gezeichnet",
          re15_nclip_r38_mac0(0, 0, 10, 0, 0, 10) == 100 && !re15_nclip_r38_verwerfen(0, 0, 10, 0, 0, 10));
    CHECK("(0,0)(0,10)(10,0): MAC0 = -100, verworfen",
          re15_nclip_r38_mac0(0, 0, 0, 10, 10, 0) == -100 && re15_nclip_r38_verwerfen(0, 0, 0, 10, 10, 0));
    CHECK("kollinear (0,0)(5,5)(10,10): MAC0 = 0, GEZEICHNET (bgez @0x800255a8)",
          re15_nclip_r38_mac0(0, 0, 5, 5, 10, 10) == 0 && !re15_nclip_r38_verwerfen(0, 0, 5, 5, 10, 10));
    /* Saettigung: ungesaettigt (0,0)(3000,10)(1500,6) -> 3000*6 - 10*1500 = +3000 (gezeichnet); die GTE saettigt
     * SX auf +0x3FF: 0*10 + 1023*6 + 1023*0 - 0*6 - 1023*0 - 1023*10 = -4092 -> verworfen. */
    CHECK("Saettigung SX/SY -0x400..+0x3FF kippt (0,0)(3000,10)(1500,6) auf MAC0 = -4092",
          re15_nclip_r38_mac0(0, 0, 3000, 10, 1500, 6) == -4092 && re15_nclip_r38_verwerfen(0, 0, 3000, 10, 1500, 6));
    /* (-5000,0)(0,-1)(0,1): ungesaettigt 5000 + 5000 = +10000; gesaettigt wie (-1024,0)(0,-1)(0,1):
     * (-1024)(-1) + 0 + 0 - (-1024)(1) - 0 - 0 = +2048. */
    CHECK("Saettigung unten: (-5000,0)(0,-1)(0,1) wie (-1024,0)(0,-1)(0,1) = +2048 (ungesaettigt +10000)",
          re15_nclip_r38_mac0(-5000, 0, 0, -1, 0, 1) == 2048);
    CHECK("Zaehler: genau 2 Verwuerfe in diesem Teil", g_re15_nclip_r38_verworfen - z0 == 2u);
    return 0;
}

/* ------------------------------------------------------- Cursor-Modell */
typedef struct {
    const uint8_t *pix; int w, h;
} tim8_t;

static int tim8_open(const uint8_t *d, long n, uint32_t off, tim8_t *t)
{
    if ((long)off + 32 > n) return 0;
    if (rd32(d + off) != 0x10u || rd32(d + off + 4) != 0x09u) return 0;
    const uint32_t clen = rd32(d + off + 8);
    const uint8_t *p = d + off + 8 + clen;
    if (p + 12 > d + n) return 0;
    t->w = (int)(p[8] | p[9] << 8) * 2;
    t->h = (int)(p[10] | p[11] << 8);
    t->pix = p + 12;
    return (t->pix + (long)t->w * t->h <= d + n);
}

/* 1 = das UV-Rechteck der Flaeche enthaelt ein Texel != 0 (Muster), 0 = nur Texel 0 (durchsichtig). */
static int muster(const tim8_t *t, const int *u, const int *v, int k)
{
    int u0 = 255, u1 = 0, v0 = 255, v1 = 0;
    for (int i = 0; i < k; i++) {
        if (u[i] < u0) u0 = u[i];
        if (u[i] > u1) u1 = u[i];
        if (v[i] < v0) v0 = v[i];
        if (v[i] > v1) v1 = v[i];
    }
    for (int y = v0; y <= v1 && y < t->h; y++)
        for (int x = u0; x <= u1 && x < t->w; x++)
            if (t->pix[(long)y * t->w + x]) return 1;
    return 0;
}

/* Modellraum-Normale nach Umlaufsinn (v1-v0) x (v2-v0), nur die y-Komponente (die Muster-Ebenen sind y = const). */
static long normale_y(const re15_md1_vertex_t *a, const re15_md1_vertex_t *b, const re15_md1_vertex_t *c)
{
    const long ax = b->x - a->x, az = b->z - a->z, bx = c->x - a->x, bz = c->z - a->z;
    return az * bx - ax * bz;
}

typedef struct {
    int oben_muster, unten_muster, sonst_muster;   /* Flaechen mit Texel != 0 je Ebene */
    int oben_sinn, unten_sinn;                     /* Vorzeichen der Normale y (+1/-1), 0 = uneinheitlich */
} modell_t;

static void sinn_merken(int *ziel, long ny, int *erst)
{
    const int s = ny > 0 ? 1 : (ny < 0 ? -1 : 0);
    if (*erst) { *ziel = s; *erst = 0; }
    else if (*ziel != s) *ziel = 0;
}

static int modell_lesen(const uint8_t *d, long n, uint32_t md1_off, uint32_t tim_off, re15_md1_t *md1, tim8_t *tim,
                        modell_t *m)
{
    memset(m, 0, sizeof *m);
    if (!tim8_open(d, n, tim_off, tim)) return 0;
    if (re15_md1_parse(d + md1_off, (int)(n - md1_off), md1) != 0 || md1->mesh_count < 1) return 0;
    int erst_o = 1, erst_u = 1;
    for (int mi = 0; mi < md1->mesh_count; mi++) {
        const re15_md1_mesh_t *me = &md1->meshes[mi];
        for (int i = 0; i < me->triangle_count; i++) {
            const re15_md1_triangle_t *f = &me->triangles[i];
            const re15_md1_tri_uv_t *uv = &me->triangle_uvs[i];
            const int u[3] = { uv->u0, uv->u1, uv->u2 }, v[3] = { uv->v0, uv->v1, uv->v2 };
            if (!muster(tim, u, v, 3)) continue;
            const re15_md1_vertex_t *a = &me->tri_vertices[f->v0], *b = &me->tri_vertices[f->v1],
                                    *c = &me->tri_vertices[f->v2];
            if (a->y == -900 && b->y == -900 && c->y == -900) { m->oben_muster++; sinn_merken(&m->oben_sinn, normale_y(a, b, c), &erst_o); }
            else if (a->y == 923 && b->y == 923 && c->y == 923) { m->unten_muster++; sinn_merken(&m->unten_sinn, normale_y(a, b, c), &erst_u); }
            else m->sonst_muster++;
        }
        for (int i = 0; i < me->quad_count; i++) {
            const re15_md1_quad_t *f = &me->quads[i];
            const re15_md1_quad_uv_t *uv = &me->quad_uvs[i];
            const int u[4] = { uv->u0, uv->u1, uv->u2, uv->u3 }, v[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
            if (!muster(tim, u, v, 4)) continue;
            const re15_md1_vertex_t *a = &me->quad_vertices[f->v0], *b = &me->quad_vertices[f->v1],
                                    *c = &me->quad_vertices[f->v2], *e = &me->quad_vertices[f->v3];
            if (a->y == -900 && b->y == -900 && c->y == -900 && e->y == -900) { m->oben_muster++; sinn_merken(&m->oben_sinn, normale_y(a, b, c), &erst_o); }
            else if (a->y == 923 && b->y == 923 && c->y == 923 && e->y == 923) { m->unten_muster++; sinn_merken(&m->unten_sinn, normale_y(a, b, c), &erst_u); }
            else m->sonst_muster++;
        }
    }
    return 1;
}

static long rdt_lesen(const char *raum, uint8_t **out)
{
    char p[512];
    snprintf(p, sizeof p, "%s/STAGE%c/ROOM%s.RDT", RE15_ASSET_PSX_DIR, raum[0], raum);
    long n = 0;
    *out = slurp(p, &n);
    return *out ? n : 0;
}

/* --------------------------------------------------------------- kasten */
static int teil_kasten(void)
{
    printf("[kasten] ROOM11F0-Cursor unter Cut 10 (Ober- und Unterseite)\n");
    uint8_t *d = NULL;
    const long n = rdt_lesen("11F0", &d);
    CHECK("ROOM11F0.RDT gelesen", n > 0);
    if (n <= 0) return 1;
    const uint32_t tab = rd32(d + 0x30);
    CHECK("Prop-Tabelle @0x30 -> 0x240, Eintrag obj 0 = TIM 0x018DAC / MD1 0x001928",
          tab == 0x240u && rd32(d + tab) == 0x018DACu && rd32(d + tab + 4) == 0x001928u);
    re15_md1_t md1; tim8_t tim; modell_t m;
    const int ok = modell_lesen(d, n, 0x001928u, 0x018DACu, &md1, &tim, &m);
    CHECK("MD1 + TIM lesbar", ok);
    if (!ok) { free(d); return 1; }
    printf("       Muster-Flaechen: oben %d, unten %d, sonst %d\n", m.oben_muster, m.unten_muster, m.sonst_muster);
    CHECK("29 Muster-Vierecke oben (y=-900), 4 Muster-Dreiecke unten (y=+923), sonst keine",
          m.oben_muster == 29 && m.unten_muster == 4 && m.sonst_muster == 0);

    int lagen = 0, lagen_ok = 0, gegen_ok = 0, start_innen = -1;
    for (int ix = -2; ix <= 2; ix++) {
        for (int iz = -2; iz <= 2; iz++) {
            const int32_t x = RE15_HC_START_X + ix * 10 * RE15_HC_SCHRITT;   /* +-4000 um die Startlage */
            const int32_t z = RE15_HC_START_Z + iz * 10 * RE15_HC_SCHRITT;
            int32_t rot[9], trans[3]; int h = 0;
            re15_hebetisch_cursor_matrix(x, z, rot, trans, &h);
            int oben_gez = 0, oben_verw = 0, unten_gez = 0, unten_verw = 0, unten_ohne = 0, unten_innen = 0;
            int unten_ueber = 0;
            int obx0 = 100000, oby0 = 100000, obx1 = -100000, oby1 = -100000;
            /* Oberseite: Umriss + Entscheid */
            for (int mi = 0; mi < md1.mesh_count; mi++) {
                const re15_md1_mesh_t *me = &md1.meshes[mi];
                for (int i = 0; i < me->quad_count; i++) {
                    const re15_md1_quad_t *f = &me->quads[i];
                    const re15_md1_quad_uv_t *uv = &me->quad_uvs[i];
                    const int u[4] = { uv->u0, uv->u1, uv->u2, uv->u3 }, v[4] = { uv->v0, uv->v1, uv->v2, uv->v3 };
                    const re15_md1_vertex_t *vp[4] = { &me->quad_vertices[f->v0], &me->quad_vertices[f->v1],
                                                       &me->quad_vertices[f->v2], &me->quad_vertices[f->v3] };
                    if (vp[0]->y != -900) continue;
                    int sx[4], sy[4];
                    for (int k = 0; k < 4; k++) {
                        int32_t vz = 0;
                        re15_hebetisch_cursor_projiziere(rot, trans, h, vp[k]->x, vp[k]->y, vp[k]->z, &sx[k], &sy[k], &vz);
                        if (sx[k] < obx0) obx0 = sx[k];
                        if (sx[k] > obx1) obx1 = sx[k];
                        if (sy[k] < oby0) oby0 = sy[k];
                        if (sy[k] > oby1) oby1 = sy[k];
                    }
                    if (!muster(&tim, u, v, 4)) continue;
                    if (re15_nclip_r38_verwerfen(sx[0], sy[0], sx[1], sy[1], sx[2], sy[2])) oben_verw++;
                    else oben_gez++;
                }
            }
            for (int mi = 0; mi < md1.mesh_count; mi++) {
                const re15_md1_mesh_t *me = &md1.meshes[mi];
                for (int i = 0; i < me->triangle_count; i++) {
                    const re15_md1_triangle_t *f = &me->triangles[i];
                    const re15_md1_tri_uv_t *uv = &me->triangle_uvs[i];
                    const int u[3] = { uv->u0, uv->u1, uv->u2 }, v[3] = { uv->v0, uv->v1, uv->v2 };
                    const re15_md1_vertex_t *vp[3] = { &me->tri_vertices[f->v0], &me->tri_vertices[f->v1],
                                                       &me->tri_vertices[f->v2] };
                    if (vp[0]->y != 923 || !muster(&tim, u, v, 3)) continue;
                    int sx[3], sy[3], innen = 1, ux0 = 100000, uy0 = 100000, ux1 = -100000, uy1 = -100000;
                    for (int k = 0; k < 3; k++) {
                        int32_t vz = 0;
                        re15_hebetisch_cursor_projiziere(rot, trans, h, vp[k]->x, vp[k]->y, vp[k]->z, &sx[k], &sy[k], &vz);
                        if (sx[k] < obx0 || sx[k] > obx1 || sy[k] < oby0 || sy[k] > oby1) innen = 0;
                        if (sx[k] < ux0) ux0 = sx[k];
                        if (sx[k] > ux1) ux1 = sx[k];
                        if (sy[k] < uy0) uy0 = sy[k];
                        if (sy[k] > uy1) uy1 = sy[k];
                    }
                    unten_ohne++;                       /* GEGENPROBE: ohne Verwurf eingereiht */
                    unten_innen += innen;               /* ... ganz im Oberseiten-Umriss */
                    unten_ueber += !(ux1 < obx0 || ux0 > obx1 || uy1 < oby0 || uy0 > oby1);   /* ... ueberdeckt ihn */
                    if (re15_nclip_r38_verwerfen(sx[0], sy[0], sx[1], sy[1], sx[2], sy[2])) unten_verw++;
                    else unten_gez++;
                }
            }
            lagen++;
            if (oben_gez == 29 && oben_verw == 0 && unten_gez == 0 && unten_verw == 4) lagen_ok++;
            else printf("       Lage (%d,%d): oben %d/%d, unten gezeichnet %d verworfen %d\n",
                        (int)x, (int)z, oben_gez, oben_verw, unten_gez, unten_verw);
            if (unten_ohne == 4 && unten_ueber == 4) gegen_ok++;
            if (ix == 0 && iz == 0) start_innen = unten_innen;
        }
    }
    printf("       %d Lagen: Entscheid richtig %d, Gegenprobe (4 Unterseiten-Dreiecke ueberdecken den Oberseiten-Umriss) %d,"
           " Startlage ganz im Umriss %d\n", lagen, lagen_ok, gegen_ok, start_innen);
    CHECK("an allen 25 Lagen: Oberseite gezeichnet (29), Unterseite verworfen (4)", lagen_ok == 25);
    CHECK("GEGENPROBE: ohne Verwurf ueberdeckten an allen 25 Lagen 4 Muster-Dreiecke den Cursor", gegen_ok == 25);
    CHECK("GEGENPROBE Startlage (Bildmitte): alle 4 Muster-Dreiecke der Unterseite ganz im Oberseiten-Umriss",
          start_innen == 4);
    free(d);
    return 0;
}

/* --------------------------------------------------------------- raeume */
typedef struct { const char *raum; uint32_t rec; int obj; uint32_t md1, tim; } cursor_raum_t;
/* Zensus analysis/befunde_runde38/A_belege/cursor_raeume_zensus.py (Byte-Muster `2d <obj> 04` + Box 900/900/900). */
static const cursor_raum_t k_raeume[] = {
    { "1080", 0x004FA, 0, 0x008E4, 0x23400 }, { "1081", 0x004FA, 0, 0x008E4, 0x23400 },
    { "4020", 0x006B6, 0, 0x00BCC, 0x236E8 }, { "4021", 0x006B6, 0, 0x00BCC, 0x236E8 },
    { "30E0", 0x00A96, 0, 0x13F10, 0x2FB74 },
    { "10D0", 0x0114E, 1, 0x0D404, 0x3DC50 }, { "10D1", 0x0114E, 1, 0x02080, 0x328CC },
    { "11E0", 0x0169E, 2, 0x02C40, 0x2B0F4 }, { "11E1", 0x0169E, 2, 0x02C40, 0x2B0F4 },
    { "1230", 0x00CC0, 0, 0x0175C, 0x1853C }, { "1231", 0x00CC0, 0, 0x0175C, 0x1853C },
    { "11F0", 0x00E54, 0, 0x01928, 0x18DAC }, { "11F1", 0x00E54, 0, 0x01928, 0x18DAC },
    { "1100", 0x00B64, 0, 0x00EBC, 0x243A8 }, { "1101", 0x00B64, 0, 0x00EBC, 0x243A8 },
    { "2060", 0x00EF0, 3, 0x033E4, 0x382B0 }, { "2061", 0x00EF0, 3, 0x033E4, 0x382B0 },
    { "2040", 0x0147E, 0, 0x01A0C, 0x2A0D8 }, { "2041", 0x0147E, 0, 0x01A0C, 0x2A0D8 },
    { "5050", 0x00794, 0, 0x00A64, 0x278D0 }, { "5051", 0x00794, 0, 0x00A64, 0x278D0 },
    { "3050", 0x01FDC, 3, 0x05190, 0x37FDC }, { "3051", 0x01FDC, 3, 0x05190, 0x37FDC },
};

static int teil_raeume(void)
{
    printf("[raeume] Zensus aller Cursor-Raeume\n");
    int sinn_11f0 = 0;
    {   /* Bezug: Oberseite 11F0 (unter Cut 10 nachweislich gezeichnet, Teil kasten) */
        uint8_t *d = NULL; const long n = rdt_lesen("11F0", &d);
        re15_md1_t md1; tim8_t tim; modell_t m;
        if (n > 0 && modell_lesen(d, n, 0x001928u, 0x018DACu, &md1, &tim, &m)) sinn_11f0 = m.oben_sinn;
        free(d);
    }
    CHECK("Bezug ROOM11F0: Oberseite einheitlich gewunden", sinn_11f0 != 0);
    const int n_r = (int)(sizeof k_raeume / sizeof k_raeume[0]);
    int gut = 0;
    for (int i = 0; i < n_r; i++) {
        const cursor_raum_t *r = &k_raeume[i];
        uint8_t *d = NULL; const long n = rdt_lesen(r->raum, &d);
        int anker = 0, tab_ok = 0, ok = 0;
        modell_t m; memset(&m, 0, sizeof m);
        if (n > (long)r->rec + 34) {
            const uint8_t *p = d + r->rec;
            anker = p[0] == 0x2D && p[1] == r->obj && p[2] == 4 && rd16s(p + 22) == 0 && rd16s(p + 24) == -900 &&
                    rd16s(p + 26) == 0 && rd16s(p + 28) == 900 && rd16s(p + 30) == 900 && rd16s(p + 32) == 900;
            const uint32_t tab = rd32(d + 0x30);
            tab_ok = rd32(d + tab + 8u * (uint32_t)r->obj) == r->tim && rd32(d + tab + 8u * (uint32_t)r->obj + 4) == r->md1;
            re15_md1_t md1; tim8_t tim;
            ok = modell_lesen(d, n, r->md1, r->tim, &md1, &tim, &m);
        }
        const int richtig = anker && tab_ok && ok && m.oben_muster == 29 && m.unten_muster == 4 && m.sonst_muster == 0 &&
                            m.oben_sinn == sinn_11f0 && m.unten_sinn == -sinn_11f0;
        printf("  %s ROOM%s rec @0x%05X obj %d: Anker %d Tabelle %d | Muster oben %d unten %d sonst %d | Umlauf oben %+d unten %+d\n",
               richtig ? "OK  " : "FAIL", r->raum, (unsigned)r->rec, r->obj, anker, tab_ok,
               m.oben_muster, m.unten_muster, m.sonst_muster, m.oben_sinn, m.unten_sinn);
        gut += richtig;
        free(d);
    }
    if (gut != n_r) s_fail++;
    printf("       %d/%d Cursor-Raeume: Unterseite traegt das Muster und ist entgegengesetzt gewunden -> NCLIP verwirft sie\n",
           gut, n_r);
    return 0;
}

/* ----------------------------------------------------------------- bild */
/* Klassen wie analysis/befunde_runde38/A_belege/cursor_klassen.py (Paletteneintraege des Cursor-TIMs 1..4). */
static char klasse(int r, int g, int b)
{
    if (b > 60 && !(r > 150 && g > 150)) return '.';
    if (r > 150 && g > 150 && b > 90) return '.';      /* 5 = weiss: wird nicht verglichen (Schalter-Glanzlicht) */
    if (g > 140 && r < 90 && b < 60) return 'G';
    if (r > 140 && g > 130 && b < 60) return 'Y';
    if (g >= 50 && r <= 20 && b <= 20 && g - r >= 40) return 'g';
    if (r >= 40 && g >= 32 && b <= 16 && r - b >= 30 && g - b >= 25 && abs(r - g) <= 24) return 'y';
    return '.';
}

/* Original ROOM11F0 Cut 10, Cursor an der Startlage: analysis/grundwahrheit/psx_room11f0_cut10.png (DuckStation,
 * Framebuffer des Spielstands psx_room11f0_cut10.sav bitgleich), Fenster x 140..179, y 100..137. */
static const char *const k_psx11f0[38] = {
    "........................................",   /* y=100 */
    "........................................",   /* y=101 */
    "........................................",   /* y=102 */
    "........................................",   /* y=103 */
    "........................................",   /* y=104 */
    "........................................",   /* y=105 */
    "........................................",   /* y=106 */
    "........................................",   /* y=107 */
    "........Ggggggggg.........gggggGg.......",   /* y=108 */
    "........Ggggggggg.........gggggGg.......",   /* y=109 */
    "........Gg.....................Gg.......",   /* y=110 */
    "........Gg.....................Gg.......",   /* y=111 */
    "........Gg.....................Gg.......",   /* y=112 */
    "........Gg..........Y..........Gg.......",   /* y=113 */
    "........gg..........Yy.........gg.......",   /* y=114 */
    "....................Yy..................",   /* y=115 */
    "....................Yy..................",   /* y=116 */
    ".....................y..................",   /* y=117 */
    "..............YYYY.....YYYy.............",   /* y=118 */
    "...............yyyyy.yyyyyy.............",   /* y=119 */
    ".....................y..................",   /* y=120 */
    "....................Yy..................",   /* y=121 */
    "....................Yy..................",   /* y=122 */
    "........G...........Yy.........G........",   /* y=123 */
    "........Gg..........yy.........Gg.......",   /* y=124 */
    "........Gg.....................Gg.......",   /* y=125 */
    "........Gg.....................Gg.......",   /* y=126 */
    "........Gg.....................Gg.......",   /* y=127 */
    "........Gg.....................Gg.......",   /* y=128 */
    "........GGGGGGGgg........GGGGGGGg.......",   /* y=129 */
    ".........gggggggg.........ggggggg.......",   /* y=130 */
    "........................................",   /* y=131 */
    "........................................",   /* y=132 */
    "........................................",   /* y=133 */
    "........................................",   /* y=134 */
    "........................................",   /* y=135 */
    "........................................",   /* y=136 */
    "........................................",   /* y=137 */
};

static uint8_t *ppm_lesen(const char *p, int *w, int *h)
{
    long n = 0;
    uint8_t *b = slurp(p, &n);
    if (!b || n < 16 || b[0] != 'P' || b[1] != '6') { free(b); return NULL; }
    int mx = 0, pos = 0;
    if (sscanf((const char *)b, "P6 %d %d %d%n", w, h, &mx, &pos) != 3 || mx != 255) { free(b); return NULL; }
    pos++;   /* ein Trennzeichen nach maxval */
    if ((long)pos + (long)(*w) * (*h) * 3 > n) { free(b); return NULL; }
    uint8_t *px = (uint8_t *)malloc((size_t)(*w) * (*h) * 3);
    if (px) memcpy(px, b + pos, (size_t)(*w) * (*h) * 3);
    free(b);
    return px;
}

static int teil_bild(int argc, char **argv)
{
    if (argc < 5) { printf("bild: <ppm> <trilog> <slot> [psx11f0]\n"); return 2; }
    const char *ppm = argv[2], *trilog = argv[3];
    const int slot = atoi(argv[4]);
    const int psx = argc > 5 && !strcmp(argv[5], "psx11f0");
    printf("[bild] %s | %s | Platz %d%s\n", ppm, trilog, slot, psx ? " | Original ROOM11F0" : "");

    /* 1) Mechanik: kein eingereihtes Dreieck des Cursor-Platzes mit NCLIP < 0 (alle Bilder des Laufs). */
    long n = 0;
    uint8_t *t = slurp(trilog, &n);
    CHECK("RE15_TRILOG lesbar", t != NULL);
    int dreiecke = 0, rueck = 0;
    if (t) {
        char *z = (char *)t;
        while (z && *z) {
            char *nl = strchr(z, '\n');
            if (nl) *nl = 0;
            unsigned seq; char fn[32]; int sl, clut, zz, x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2;
            if (sscanf(z, "%u %31s slot=%d clut=0x%X z=%d (%d,%d)uv(%d,%d) (%d,%d)uv(%d,%d) (%d,%d)uv(%d,%d)",
                       &seq, fn, &sl, (unsigned *)&clut, &zz, &x0, &y0, &u0, &v0, &x1, &y1, &u1, &v1,
                       &x2, &y2, &u2, &v2) == 17 && sl == slot) {
                dreiecke++;
                if (re15_nclip_r38_mac0(x0, y0, x1, y1, x2, y2) < 0) rueck++;
            }
            z = nl ? nl + 1 : NULL;
        }
        free(t);
    }
    printf("       Platz %d: %d eingereihte Dreiecke im Fenster, davon mit NCLIP < 0: %d\n", slot, dreiecke, rueck);
    CHECK("Cursor-Platz hat Dreiecke im Fenster", dreiecke > 0);
    CHECK("kein rueckseitiges Dreieck des Cursors eingereiht (FUN_800254a0 @0x800255a8 / FUN_800256b0 @0x800257c0)",
          rueck == 0);

    /* 2) Bild: innerhalb des Eckwinkel-Rahmens kein reingruener Bildpunkt. */
    int w = 0, h = 0;
    uint8_t *px = ppm_lesen(ppm, &w, &h);
    CHECK("Framedump lesbar, 320x240 (RE15_WINDOW_SCALE=1)", px && w == 320 && h == 240);
    if (!px || w != 320 || h != 240) { free(px); return 0; }
    int bx0 = 1000, by0 = 1000, bx1 = -1, by1 = -1;
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            const uint8_t *p = px + ((long)y * w + x) * 3;
            if (p[1] > 200 && p[0] < 60 && p[2] < 60) {
                if (x < bx0) bx0 = x;
                if (x > bx1) bx1 = x;
                if (y < by0) by0 = y;
                if (y > by1) by1 = y;
            }
        }
    printf("       Eckwinkel-Huelle x %d..%d, y %d..%d\n", bx0, bx1, by0, by1);
    CHECK("Eckwinkel gefunden (Breite 20..30, Hoehe 18..28)",
          bx1 - bx0 >= 20 && bx1 - bx0 <= 30 && by1 - by0 >= 18 && by1 - by0 <= 28);
    int innen = 0;
    for (int y = by0 + 2; y <= by1 - 2; y++)
        for (int x = bx0 + 2; x <= bx1 - 2; x++) {
            const uint8_t *p = px + ((long)y * w + x) * 3;
            if (p[0] <= 12 && p[2] <= 12 && p[1] >= 15 && p[1] - p[0] >= 12 && p[1] - p[2] >= 12) {
                if (innen < 6) printf("       reingruen innen (%d,%d) = (%d,%d,%d)\n", x, y, p[0], p[1], p[2]);
                innen++;
            }
        }
    printf("       reingruene Bildpunkte im Rahmen-Inneren: %d (Original ROOM11F0: 0; Port vor Runde 38: 32..33)\n", innen);
    CHECK("keine Schatten-Kopie im Rahmen-Inneren", innen == 0);

    /* 3) Original-Vergleich ROOM11F0. */
    if (psx) {
        int abw = 0;
        for (int y = 0; y < 38; y++)
            for (int x = 0; x < 40; x++) {
                const uint8_t *p = px + ((long)(100 + y) * w + 140 + x) * 3;
                const char k = klasse(p[0], p[1], p[2]), o = k_psx11f0[y][x];
                const int fehler = (o != '.' && k != o) || ((k == 'G' || k == 'g') && k != o);
                if (fehler) {
                    if (abw < 8) printf("       Abweichung (%d,%d): Port '%c' Original '%c'\n", 140 + x, 100 + y, k, o);
                    abw++;
                }
            }
        printf("       Klassenkarte gegen das Original: %d Abweichungen (Port vor Runde 38: 21)\n", abw);
        CHECK("Cursor-Klassenkarte gleich dem Original (G/g/Y/y)", abw == 0);
    }
    free(px);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int rc = 0;
    if (!strcmp(teil, "nclip") || !strcmp(teil, "alle")) rc |= teil_nclip();
    if (!strcmp(teil, "kasten") || !strcmp(teil, "alle")) rc |= teil_kasten();
    if (!strcmp(teil, "raeume") || !strcmp(teil, "alle")) rc |= teil_raeume();
    if (!strcmp(teil, "bild")) rc |= teil_bild(argc, argv);
    if (rc || s_fail) { printf("probe_r38_cursor %s: FEHLER (%d)\n", teil, s_fail); return 1; }
    printf("probe_r38_cursor %s: OK\n", teil);
    return 0;
}
