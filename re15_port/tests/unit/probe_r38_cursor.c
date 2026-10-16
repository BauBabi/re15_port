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
 *   wicklung  Nebenwirkung des Verwurfs auf PORT-GEBAUTE Modelle (Dossier §4.5/§3b): der Verwurf zeichnet nur die
 *           Aussenseite, wenn das Modell wie das Original gewickelt ist. w = sign(((v1-v0) x (v2-v0)) . Summe der
 *           Normalen) je Flaeche.
 *           - Konvention des Originals: alle Props aller RDTs, w < 0 bei mindestens 99 % (gemessen 9364 : 41 je
 *             Modell einmal, A_belege/wicklung_props.py);
 *           - jedes eingebackene Port-Modell (Hebetisch-Cursor, Sicherung, Granate, Irons Diary/Keycard, Dokumente,
 *             Memory Card/Schrot, Lageplan-Karten, Schluessel, Feuerloescher stehend/liegend): KEINE Flaeche w > 0
 *             (vor der Behebung: Feuerloescher 112 von 148);
 *           - Mechanik: 60 Blickrichtungen, Projektion wie main.c (re15_hebetisch_cursor_projiziere), Verwurf
 *             re15_nclip_r38_verwerfen: keine nach aussen zur Kamera zeigende Flaeche verworfen, keine abgewandte
 *             gezeichnet (|cos| > 0.5 zur gespeicherten Normale); GEGENPROBE mit v1 <-> v2: der Riegel schlaegt an.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "re15_nclip_r38.h"
#include "re15_md1.h"
#include "re15_hebetisch_cursor.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_dokumente.h"
#include "re15_irons_tisch.h"
#include "re15_inhalt_r35.h"
#include "re15_kartenitems_r36.h"
#include "re15_schluessel_r36.h"
#include "re15_feuerloescher_r37.h"

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

/* --------------------------------------------------------------- wicklung */
#define R38_PI 3.14159265358979323846
typedef struct { long neg, pos, null; } wzahl_t;

/* w = sign(((p1-p0) x (p2-p0)) . (n0+..+n{k-1})) */
static int w_flaeche(const re15_md1_vertex_t *const p[3], const re15_md1_vertex_t *const *nn, int k)
{
    const double ax = p[1]->x - p[0]->x, ay = p[1]->y - p[0]->y, az = p[1]->z - p[0]->z;
    const double cx = p[2]->x - p[0]->x, cy = p[2]->y - p[0]->y, cz = p[2]->z - p[0]->z;
    const double gx = ay * cz - az * cy, gy = az * cx - ax * cz, gz = ax * cy - ay * cx;
    double nx = 0, ny = 0, nz = 0;
    for (int i = 0; i < k; i++) { nx += nn[i]->x; ny += nn[i]->y; nz += nn[i]->z; }
    const double s = gx * nx + gy * ny + gz * nz;
    return (s > 0) - (s < 0);
}

/* Ruft fn(ecken p[0..k-1], normalen n[0..k-1], k) fuer jede gueltige Flaeche; p[0..2] = v0,v1,v2 wie der Verwurf. */
typedef void (*flaeche_fn)(const re15_md1_vertex_t *const *p, const re15_md1_vertex_t *const *n, int k, void *ctx);
static void flaechen_je(const re15_md1_t *m, flaeche_fn fn, void *ctx)
{
    for (int mi = 0; mi < m->mesh_count; mi++) {
        const re15_md1_mesh_t *me = &m->meshes[mi];
        for (int i = 0; i < me->triangle_count; i++) {
            const re15_md1_triangle_t *f = &me->triangles[i];
            if (f->v0 >= me->tri_vertex_count || f->v1 >= me->tri_vertex_count || f->v2 >= me->tri_vertex_count ||
                f->n0 >= me->tri_normal_count || f->n1 >= me->tri_normal_count || f->n2 >= me->tri_normal_count)
                continue;
            const re15_md1_vertex_t *p[3] = { &me->tri_vertices[f->v0], &me->tri_vertices[f->v1], &me->tri_vertices[f->v2] };
            const re15_md1_vertex_t *n[3] = { &me->tri_normals[f->n0], &me->tri_normals[f->n1], &me->tri_normals[f->n2] };
            fn(p, n, 3, ctx);
        }
        for (int i = 0; i < me->quad_count; i++) {
            const re15_md1_quad_t *f = &me->quads[i];
            if (f->v0 >= me->quad_vertex_count || f->v1 >= me->quad_vertex_count || f->v2 >= me->quad_vertex_count ||
                f->v3 >= me->quad_vertex_count || f->n0 >= me->quad_normal_count || f->n1 >= me->quad_normal_count ||
                f->n2 >= me->quad_normal_count || f->n3 >= me->quad_normal_count)
                continue;
            const re15_md1_vertex_t *p[4] = { &me->quad_vertices[f->v0], &me->quad_vertices[f->v1],
                                              &me->quad_vertices[f->v2], &me->quad_vertices[f->v3] };
            const re15_md1_vertex_t *n[4] = { &me->quad_normals[f->n0], &me->quad_normals[f->n1],
                                              &me->quad_normals[f->n2], &me->quad_normals[f->n3] };
            fn(p, n, 4, ctx);
        }
    }
}

static void w_zaehle_fn(const re15_md1_vertex_t *const *p, const re15_md1_vertex_t *const *n, int k, void *ctx)
{
    wzahl_t *z = (wzahl_t *)ctx;
    const int w = w_flaeche(p, n, k);
    if (w < 0) z->neg++; else if (w > 0) z->pos++; else z->null++;
}

/* Blick-Mechanik: Verwurf gegen "zeigt die Aussenseite (gespeicherte Normale) zur Kamera?" */
typedef struct {
    int32_t rot[9], trans[3]; int h;
    int umgekehrt;                 /* GEGENPROBE: v1 <-> v2 */
    long gezaehlt, falsch_gezeichnet, falsch_verworfen;
} blick_t;

static void blick_fn(const re15_md1_vertex_t *const *p, const re15_md1_vertex_t *const *n, int k, void *ctx)
{
    blick_t *b = (blick_t *)ctx;
    double c[3] = { 0, 0, 0 }, ns[3] = { 0, 0, 0 };
    for (int i = 0; i < k; i++) {
        c[0] += p[i]->x; c[1] += p[i]->y; c[2] += p[i]->z;
        ns[0] += n[i]->x; ns[1] += n[i]->y; ns[2] += n[i]->z;
    }
    for (int i = 0; i < 3; i++) c[i] /= k;
    double cv[3], nv[3];
    for (int r = 0; r < 3; r++) {
        cv[r] = (b->rot[3 * r] * c[0] + b->rot[3 * r + 1] * c[1] + b->rot[3 * r + 2] * c[2]) / 4096.0 + b->trans[r];
        nv[r] = (b->rot[3 * r] * ns[0] + b->rot[3 * r + 1] * ns[1] + b->rot[3 * r + 2] * ns[2]) / 4096.0;
    }
    const double ln = sqrt(nv[0] * nv[0] + nv[1] * nv[1] + nv[2] * nv[2]);
    const double lc = sqrt(cv[0] * cv[0] + cv[1] * cv[1] + cv[2] * cv[2]);
    if (ln < 1e-9 || lc < 1e-9) return;
    const double cosw = -(nv[0] * cv[0] + nv[1] * cv[1] + nv[2] * cv[2]) / (ln * lc);   /* > 0: zur Kamera */
    /* Die gespeicherten Normalen sind geglaettet (Licht): sie weichen von der Flaechen-Normale um delta ab (gemessen
     * bis 34,4 Grad Feuerloescher-Hals, 40,5 Grad Sicherung). "Zur Kamera" ist nur dann eindeutig, wenn der Blick
     * mehr als delta + 5 Grad von der Streiflage entfernt ist: |cos| > sin(delta + 5 Grad); sonst auslassen. */
    const double ax = p[1]->x - p[0]->x, ay = p[1]->y - p[0]->y, az = p[1]->z - p[0]->z;
    const double bx = p[2]->x - p[0]->x, by = p[2]->y - p[0]->y, bz = p[2]->z - p[0]->z;
    const double g[3] = { ay * bz - az * by, az * bx - ax * bz, ax * by - ay * bx };
    const double lg = sqrt(g[0] * g[0] + g[1] * g[1] + g[2] * g[2]);
    const double lns = sqrt(ns[0] * ns[0] + ns[1] * ns[1] + ns[2] * ns[2]);
    if (lg < 1e-9 || lns < 1e-9) return;
    const double cosd = fabs(g[0] * ns[0] + g[1] * ns[1] + g[2] * ns[2]) / (lg * lns);
    const double delta = acos(cosd > 1.0 ? 1.0 : cosd);
    double schwelle = sin(delta + 5.0 * R38_PI / 180.0);
    if (schwelle < 0.2) schwelle = 0.2;
    if (fabs(cosw) < schwelle) return;                                                   /* streifend: auslassen */
    int sx[3], sy[3];
    const int idx[3] = { 0, b->umgekehrt ? 2 : 1, b->umgekehrt ? 1 : 2 };
    for (int i = 0; i < 3; i++) {
        int32_t vz = 0;
        if (!re15_hebetisch_cursor_projiziere(b->rot, b->trans, b->h, p[idx[i]]->x, p[idx[i]]->y, p[idx[i]]->z,
                                              &sx[i], &sy[i], &vz))
            return;
    }
    /* Ganzzahlige Bildschirmpunkte (wie SX/SY der GTE): bei Splitter-Dreiecken (Sicherung: 1 Einheit breite Stufen-
     * ringe bei x = +-107) kann die Rundung (je Ecke <= 0,5 Bildpunkte) das Vorzeichen von MAC0 kippen — das gilt fuer
     * das Original genauso und ist keine Wicklungsfrage. Auslassen, wenn |MAC0| <= 2 * Umfang. */
    const int32_t mac0 = re15_nclip_r38_mac0(sx[0], sy[0], sx[1], sy[1], sx[2], sy[2]);
    double umfang = 0;
    for (int i = 0; i < 3; i++) {
        const double ex = sx[(i + 1) % 3] - sx[i], ey = sy[(i + 1) % 3] - sy[i];
        umfang += sqrt(ex * ex + ey * ey);
    }
    if (fabs((double)mac0) <= 2.0 * umfang) return;
    const int verw = re15_nclip_r38_verwerfen(sx[0], sy[0], sx[1], sy[1], sx[2], sy[2]);
    b->gezaehlt++;
    if (cosw > 0 && verw) b->falsch_verworfen++;
    if (cosw < 0 && !verw) b->falsch_gezeichnet++;
}

/* 12 Gier- x 5 Nickwinkel (60 Blicke), Modellmitte im Abstand d vor der Kamera. */
static void blicke(const re15_md1_t *m, int umgekehrt, blick_t *summe)
{
    int lo[3] = { 1 << 30, 1 << 30, 1 << 30 }, hi[3] = { -(1 << 30), -(1 << 30), -(1 << 30) };
    for (int mi = 0; mi < m->mesh_count; mi++) {
        const re15_md1_mesh_t *me = &m->meshes[mi];
        for (int s = 0; s < 2; s++) {
            const re15_md1_vertex_t *v = s ? me->quad_vertices : me->tri_vertices;
            const int nv = s ? me->quad_vertex_count : me->tri_vertex_count;
            for (int i = 0; i < nv; i++) {
                const int q[3] = { v[i].x, v[i].y, v[i].z };
                for (int a = 0; a < 3; a++) { if (q[a] < lo[a]) lo[a] = q[a]; if (q[a] > hi[a]) hi[a] = q[a]; }
            }
        }
    }
    double r = 0;
    for (int a = 0; a < 3; a++) { const double e = (hi[a] - lo[a]) / 2.0; r += e * e; }
    r = sqrt(r);
    if (r < 1) r = 1;
    const double mitte[3] = { (lo[0] + hi[0]) / 2.0, (lo[1] + hi[1]) / 2.0, (lo[2] + hi[2]) / 2.0 };
    /* Abstand d, Brennweite h: Modell vor der Nahebene (vz >= 64) und h < 2*vz (sonst saettigt die UNR-Division
     * der GTE, re15_gte_divide), Abbildung hoechstens etwa +-300 Bildpunkte (Saettigung -0x400..+0x3FF). */
    const double d = 4.0 * r + 100.0;
    double hh = 300.0 * d / r;
    if (hh > 1.9 * (d - r)) hh = 1.9 * (d - r);
    memset(summe, 0, sizeof *summe);
    for (int iy = 0; iy < 12; iy++) {
        for (int ip = -2; ip <= 2; ip++) {
            const double gier = iy * (2.0 * R38_PI / 12.0) + 0.1, nick = ip * (R38_PI / 6.0);
            const double cy = cos(gier), sy = sin(gier), cp = cos(nick), sp = sin(nick);
            /* R = Rx(nick) * Ry(gier), Q12, det +1 */
            const double R[9] = { cy, 0, sy,  sp * sy, cp, -sp * cy,  -cp * sy, sp, cp * cy };
            blick_t b;
            memset(&b, 0, sizeof b);
            for (int i = 0; i < 9; i++) b.rot[i] = (int32_t)lround(R[i] * 4096.0);
            for (int i = 0; i < 3; i++)
                b.trans[i] = (int32_t)lround(-(R[3 * i] * mitte[0] + R[3 * i + 1] * mitte[1] + R[3 * i + 2] * mitte[2])
                                              + (i == 2 ? d : 0.0));
            b.h = (int)lround(hh);
            b.umgekehrt = umgekehrt;
            flaechen_je(m, blick_fn, &b);
            summe->gezaehlt += b.gezaehlt;
            summe->falsch_gezeichnet += b.falsch_gezeichnet;
            summe->falsch_verworfen += b.falsch_verworfen;
        }
    }
}

typedef struct { const char *name; const uint8_t *b; int n; } port_modell_t;

static int port_dazu(port_modell_t *l, int *nl, int max, const char *name, const uint8_t *b, int n)
{
    if (!b || n <= 12) return 0;
    for (int i = 0; i < *nl; i++) if (l[i].b == b) return 0;
    if (*nl >= max) return 0;
    l[*nl].name = name; l[*nl].b = b; l[*nl].n = n; (*nl)++;
    return 1;
}

static int teil_wicklung(void)
{
    printf("[wicklung] Wicklung der Objekt-Modelle gegen den Verwurf (Dossier §4.5/§3b)\n");
    /* 1. Konvention des Originals: alle Props aller RDTs */
    wzahl_t orig = { 0, 0, 0 };
    int modelle = 0, raeume = 0;
    for (int st = 1; st <= 6; st++) {
        for (int r = 0; r < 0x100; r++) {
            for (int var = 0; var < 2; var++) {
                char raum[8];
                snprintf(raum, sizeof raum, "%d%02X%d", st, r, var);
                uint8_t *d = NULL;
                const long n = rdt_lesen(raum, &d);
                if (n < 0x100) { free(d); continue; }
                raeume++;
                const int n_om = d[2];
                const uint32_t tab = rd32(d + 0x30);
                for (int oid = 0; oid < n_om && (long)tab + 8 * oid + 8 <= n; oid++) {
                    const uint32_t md1 = rd32(d + tab + 8 * oid + 4);
                    if (!md1 || (long)md1 >= n) continue;
                    static re15_md1_t m;
                    if (re15_md1_parse(d + md1, (int)(n - md1), &m) != 0) continue;
                    flaechen_je(&m, w_zaehle_fn, &orig);
                    modelle++;
                }
                free(d);
            }
        }
    }
    printf("       Original: %d RDTs, %d Props, Flaechen w<0 %ld, w>0 %ld, w=0 %ld\n", raeume, modelle, orig.neg,
           orig.pos, orig.null);
    CHECK("Original-Konvention: w < 0 bei >= 99 % der Flaechen mit w != 0 (alle Props aller RDTs)",
          raeume > 200 && orig.neg > 0 && orig.neg >= 99 * orig.pos);

    /* 2. alle eingebackenen Port-Modelle */
    port_modell_t l[64];
    int nl = 0, n = 0;
    const uint8_t *b;
    b = re15_hebetisch_cursor_md1_bytes(&n);                 port_dazu(l, &nl, 64, "Hebetisch-Cursor", b, n);
    b = re15_sicherung_md1_bytes(&n);                        port_dazu(l, &nl, 64, "Sicherung", b, n);
    b = re15_granate_md1_bytes(&n);                          port_dazu(l, &nl, 64, "Granate", b, n);
    b = re15_irons_tisch_md1_bytes(RE15_IRONS_DIARY_OBJ_ID, &n); port_dazu(l, &nl, 64, "Irons Diary", b, n);
    b = re15_irons_tisch_md1_bytes(RE15_IRONS_KARTE_OBJ_ID, &n); port_dazu(l, &nl, 64, "Irons Keycard", b, n);
    b = re15_feuerloescher_r37_md1(RE15_FL37_STEHEND, &n);   port_dazu(l, &nl, 64, "Feuerloescher stehend", b, n);
    b = re15_feuerloescher_r37_md1(RE15_FL37_LIEGEND, &n);   port_dazu(l, &nl, 64, "Feuerloescher liegend", b, n);
    for (int raum = 0x1000; raum <= 0x6FFF; raum++) {
        b = re15_dokumente_md1_bytes((uint16_t)raum, &n);       port_dazu(l, &nl, 64, "Dokument", b, n);
        b = re15_inhalt_r35_md1_bytes((uint16_t)raum, &n);      port_dazu(l, &nl, 64, "Memory Card/Schrot", b, n);
        b = re15_kartenitems_r36_md1_bytes((uint16_t)raum, &n); port_dazu(l, &nl, 64, "Lageplan-Karte", b, n);
        b = re15_schluessel_r36_md1_bytes((uint16_t)raum, &n);  port_dazu(l, &nl, 64, "Schluessel", b, n);
        b = re15_feuerloescher_r37_md1_bytes((uint16_t)raum, &n); port_dazu(l, &nl, 64, "Feuerloescher (Raum)", b, n);
    }
    printf("       %d eingebackene Port-Modelle\n", nl);
    CHECK("mindestens 15 Port-Modelle gefunden (Cursor, Sicherung, Granate, 2 Irons, 2 Loescher, Dokumente, ...)",
          nl >= 15);
    int alle_ok = 1, mech_ok = 1, gegen_ok = 1, fl_gesehen = 0;
    for (int i = 0; i < nl; i++) {
        static re15_md1_t m;
        if (re15_md1_parse(l[i].b, l[i].n, &m) != 0) {
            printf("  FAIL %s: MD1 parst nicht\n", l[i].name); s_fail++; alle_ok = 0; continue;
        }
        wzahl_t z = { 0, 0, 0 };
        flaechen_je(&m, w_zaehle_fn, &z);
        blick_t s, g;
        blicke(&m, 0, &s);
        blicke(&m, 1, &g);
        printf("       %-24s w<0 %3ld w>0 %3ld w=0 %ld | 60 Blicke: %ld Entscheide, falsch verworfen %ld, falsch "
               "gezeichnet %ld | GEGENPROBE v1<->v2: falsch %ld/%ld\n", l[i].name, z.neg, z.pos, z.null, s.gezaehlt,
               s.falsch_verworfen, s.falsch_gezeichnet, g.falsch_verworfen, g.falsch_gezeichnet);
        if (z.pos != 0 || z.neg == 0) alle_ok = 0;
        if (s.gezaehlt == 0 || s.falsch_verworfen != 0 || s.falsch_gezeichnet != 0) mech_ok = 0;
        if (g.falsch_verworfen == 0 || g.falsch_gezeichnet == 0) gegen_ok = 0;
        if (!strncmp(l[i].name, "Feuerloescher", 13)) fl_gesehen++;
    }
    CHECK("Feuerloescher (stehend + liegend) unter den Port-Modellen", fl_gesehen >= 2);
    CHECK("jedes Port-Modell wie das Original gewickelt: keine Flaeche w > 0 (Feuerloescher vorher 112 von 148)",
          alle_ok);
    CHECK("Mechanik: an 60 Blicken verwirft NCLIP keine Aussenseite und zeichnet keine abgewandte Flaeche", mech_ok);
    CHECK("GEGENPROBE: mit v1 <-> v2 (umgekehrte Wicklung) schlaegt der Riegel an jedem Modell an", gegen_ok);
    return 0;
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    int rc = 0;
    if (!strcmp(teil, "nclip") || !strcmp(teil, "alle")) rc |= teil_nclip();
    if (!strcmp(teil, "kasten") || !strcmp(teil, "alle")) rc |= teil_kasten();
    if (!strcmp(teil, "raeume") || !strcmp(teil, "alle")) rc |= teil_raeume();
    if (!strcmp(teil, "wicklung") || !strcmp(teil, "alle")) rc |= teil_wicklung();
    if (!strcmp(teil, "bild")) rc |= teil_bild(argc, argv);
    if (rc || s_fail) { printf("probe_r38_cursor %s: FEHLER (%d)\n", teil, s_fail); return 1; }
    printf("probe_r38_cursor %s: OK\n", teil);
    return 0;
}
