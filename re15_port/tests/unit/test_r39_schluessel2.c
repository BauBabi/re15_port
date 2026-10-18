/*
 * test_r39_schluessel2.c — Runde 39 Spur M "schluessel2": die Schluessel an der Pinnwand ROOM10F0 (Leon, Parking Lot Key)
 * und ROOM10F1 (Elza, Prison Key) hoechstens halb so gross wie der Stand Spur L.
 * Dossier analysis/befunde_runde39/M_schluessel2.md; Werkzeug re15_port/tools/r38_itemorte/itemorte_export.py
 * (NUTZER_FAKTOR_M, gte_ecken) -> gen/itemorte_r38_lage.inc (RE15_R39M_*) + gen/itemorte_r38_md1.inc.
 *
 * NUTZER-VORGABE (AUFTRAG.md Runde 39, Nachtrag 5, 2026-10-18 19:29, woertlich): "Und die Schlüssel im Communicationsroom
 * sind immer noch zu groß, die sollten maximal halb so groß sein." Zahl 0,5 = Nutzer-Zahl, KEIN RE-Wert. FORM, zwei Grenzen,
 * je in Breite UND Hoehe <= 0,5 x Spur L:
 *   (1) stetig: die gezeichnete Huelle (der Massstab steckt nur in den MD1-Ecken; RTPS SX2 = OFX + IR1*H/SZ3, H = fov>>7
 *       @0x80021e70 vor SetGeomScreen @0x80066c30 -> Bild linear im Massstab);
 *   (2) GTE-Raster: das Viereck der GANZZAHLIGEN Bildschirm-Ecken — der Objekt-Zeichner FUN_80022f0c rechnet RTPT
 *       (@0x80022fb0) und schreibt SXY0..2 unmittelbar ins GPU-Polygon (@0x80022fd8..e0); der Port ebenso
 *       (objpoly_r38.c). Gemessen: mit nur (1) wurde der Parking Lot Key 2 x 2 statt 6 x 3 (Hoehe 2/3).
 * Geliefert wird die Mitte des Massstab-Intervalls, in dem der groesste zulaessige Ecken-Satz entsteht.
 *
 *   test_r39_schluessel2 halb    (1) am GELIEFERTEN Netz: MD1 + Prop-Lage/-Drehung durch die Engine-Kamera Cut 5, jeder
 *        sichtbare Texel (CLUT-Farbe != 0) baryzentrisch abgetastet wie unit_r38_itemorte_lage. Stand Spur L = dasselbe
 *        RE2-Modell (info/re2leon), Ecken rint((v - Mitte) x Spur-L-Massstab) wie das Werkzeug. Soll: Breite UND Hoehe
 *        <= 0,5 x Spur L (+0,03 Abtastrest), Mitte unveraendert (+-0,1 px), dieselben sichtbaren Abtastpunkte, alles in der
 *        Nutzer-Marke; das gelieferte MD1 IST das RE2-Modell mit genau diesen Ecken (alle anderen Bytes gleich).
 *   test_r39_schluessel2 raster  (2) mit den ECHTEN Engine-Funktionen (re15_camera_build_view, re15_camera_compose_view_bone,
 *        re15_objpoly_r38_tri_mesh = RTPS/UNR-Division, Prop-Drehung wie pc_prop_rot_q12): GTE-Viereck Spur L (6 x 3 /
 *        6 x 4) und geliefert (2 x 1 / 3 x 2) == Werkzeug-Konstanten (== RE15_TRILOG der exe), geliefert <= 0,5 x Spur L;
 *        knapp ueber dem Intervall (S_OBEN) verletzt der Massstab eine der beiden Grenzen (= groesstes zulaessiges Viereck);
 *        an beiden Intervall-Enden dieselben ganzzahligen Ecken wie geliefert (= dasselbe Bild, Abstand zu den Spruengen).
 *   test_r39_schluessel2 konst   Herkunft/Zusammenhang der Konstanten: RE15_R39M_FAKTOR100 == 50 (NUTZER-VORGABE),
 *        Basis == Spur-L-Abnahme (RE15_R39L_*), Lage/Drehung/Aufhebe-Zone/Slot/Licht unveraendert, Engine liefert den
 *        Spur-M-Massstab.
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "re15_itemorte_r38.h"
#include "re15_schluessel_r36.h"
#include "re15_rdt.h"
#include "re15_camera.h"
#include "re15_md1.h"
#include "re15_tim.h"
#include "re15_skeleton.h"
#include "re15_objpoly_r38.h"

#ifndef RE15_ASSET_PSX_DIR
#error RE15_ASSET_PSX_DIR fehlt
#endif
#ifndef RE15_RE2_DIR
#error RE15_RE2_DIR fehlt
#endif

static int fails = 0, checks = 0;
#define CHECK(c, ...) do { checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

/* Nutzer-Marke key_location.bmp (Runde 38 K: ROOM10F0 Cut 5, exakte Markenfarbe (237,28,36), volles Rechteck) */
#define KS_MX0 133
#define KS_MY0 77
#define KS_MX1 143
#define KS_MY1 85
#define MD1_MAX 512

static uint8_t *slurp(const char *p, int *n)
{
    FILE *f = fopen(p, "rb");
    long sz;
    uint8_t *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { fclose(f); free(b); return NULL; }
    fclose(f);
    if (n) *n = (int)sz;
    return b;
}

/* Kamera-Ansicht von Cut `cut` des Raums aus der echten RDT (Engine: re15_camera_build_view, FUN_80053ca4). */
static int kamera(uint16_t raum, int cut, re15_camera_view_t *v)
{
    char p[600]; int n = 0, rc = -1; uint8_t *b; re15_rdt_t rdt;
    snprintf(p, sizeof p, "%s/STAGE%X/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, (unsigned)(raum >> 12), (unsigned)raum);
    b = slurp(p, &n);
    if (!b) { printf("FAIL: %s fehlt\n", p); fails++; return -1; }
    if (re15_rdt_parse(b, (size_t)n, &rdt) == 0 && cut < rdt.cut_count)
        rc = re15_camera_build_view(&rdt.cuts[cut], v);
    free(b);   /* die Ansicht ist kopiert */
    return rc;
}

/* Projektion wie unit_r38_itemorte_lage / Sonde probe_r34n_e_dokumente (Engine-Matrix, H = fov_screen_dist), stetig. */
static double projiziere(const re15_camera_view_t *v, double x, double y, double z, double *sx, double *sy)
{
    double vx = (v->rot[0] * x + v->rot[1] * y + v->rot[2] * z) / 4096.0 + v->trans[0];
    double vy = (v->rot[3] * x + v->rot[4] * y + v->rot[5] * z) / 4096.0 + v->trans[1];
    double vz = (v->rot[6] * x + v->rot[7] * y + v->rot[8] * z) / 4096.0 + v->trans[2];
    if (vz <= 1) { *sx = *sy = -1; return vz; }
    *sx = 160.0 + v->fov_screen_dist * vx / vz;
    *sy = 120.0 + v->fov_screen_dist * vy / vz;
    return vz;
}

/* Prop-Drehung wie pc_prop_rot_q12 (platform/pc/main.c, Ry*Rx*Rz, ganzzahlig) — fuer die GTE-Kette. */
static void prop_rot_q12(int rx, int ry, int rz, int32_t m[9])
{
    int32_t rsx = re15_sin_q12(rx), rcx = re15_cos_q12(rx), rsy = re15_sin_q12(ry), rcy = re15_cos_q12(ry);
    int32_t rsz = re15_sin_q12(rz), rcz = re15_cos_q12(rz);
#define Q12M(a, b) ((int32_t)(((int64_t)(a) * (int64_t)(b)) >> 12))
    m[0] = Q12M(rcz, rcy);
    m[1] = -Q12M(rsz, rcy);
    m[2] = rsy;
    m[3] = (int32_t)(((((int64_t)rsz * rcx) << 12) + (int64_t)rcz * rsy * rsx) >> 24);
    m[4] = (int32_t)(((((int64_t)rcz * rcx) << 12) - (int64_t)rsz * rsy * rsx) >> 24);
    m[5] = -Q12M(rcy, rsx);
    m[6] = (int32_t)(((((int64_t)rsz * rsx) << 12) - (int64_t)rcz * rsy * rcx) >> 24);
    m[7] = (int32_t)(((((int64_t)rcz * rsx) << 12) + (int64_t)rsz * rsy * rcx) >> 24);
    m[8] = Q12M(rcy, rcx);
#undef Q12M
}

/* dieselbe Drehung als double (stetige Abtastung), wie unit_r38_itemorte_lage */
static void prop_matrix(int rx, int ry, int rz, double m[9])
{
    const double k = 2.0 * 3.14159265358979323846 / 4096.0;
    double sx = sin(rx * k), cx = cos(rx * k), sy = sin(ry * k), cy = cos(ry * k), sz = sin(rz * k), cz = cos(rz * k);
    m[0] = cz * cy;                 m[1] = -sz * cy;                m[2] = sy;
    m[3] = sz * cx + cz * sy * sx;  m[4] = cz * cx - sz * sy * sx;  m[5] = -cy * sx;
    m[6] = sz * sx - cz * sy * cx;  m[7] = cz * sx + sz * sy * cx;  m[8] = cy * cx;
}

typedef struct {
    const char *name; uint16_t raum; uint8_t item; const char *re2_rdt; long re2_off;
    int s10000, sl10000, s_unten, s_oben; long mx100000, mz100000;
    int l_gte_x0, l_gte_y0, l_gte_b, l_gte_h, gte_x0, gte_y0, gte_b, gte_h;
    int l_breite1000, l_hoehe1000;
} schl_t;

static const schl_t k_schl[2] = {
    { "Parking Lot Key (Leon)", 0x10F0, 0x45, "PL0/RDT/ROOM6150.RDT", 0x4FBC,
      RE15_R38K_PARK_S10000, RE15_R39M_PARK_SL_10000, RE15_R39M_PARK_S_UNTEN_10000, RE15_R39M_PARK_S_OBEN_10000,
      RE15_R39M_PARK_MX100000, RE15_R39M_PARK_MZ100000,
      RE15_R39M_PARK_L_GTE_X0, RE15_R39M_PARK_L_GTE_Y0, RE15_R39M_PARK_L_GTE_B, RE15_R39M_PARK_L_GTE_H,
      RE15_R39M_PARK_GTE_X0, RE15_R39M_PARK_GTE_Y0, RE15_R39M_PARK_GTE_B, RE15_R39M_PARK_GTE_H,
      RE15_R39M_PARK_L_BREITE_PX1000, RE15_R39M_PARK_L_HOEHE_PX1000 },
    { "Prison Key (Elza)", 0x10F1, 0x42, "PL0/RDT/ROOM10F0.RDT", 0x1650,
      RE15_R38K_PRISON_S10000, RE15_R39M_PRISON_SL_10000, RE15_R39M_PRISON_S_UNTEN_10000, RE15_R39M_PRISON_S_OBEN_10000,
      RE15_R39M_PRISON_MX100000, RE15_R39M_PRISON_MZ100000,
      RE15_R39M_PRISON_L_GTE_X0, RE15_R39M_PRISON_L_GTE_Y0, RE15_R39M_PRISON_L_GTE_B, RE15_R39M_PRISON_L_GTE_H,
      RE15_R39M_PRISON_GTE_X0, RE15_R39M_PRISON_GTE_Y0, RE15_R39M_PRISON_GTE_B, RE15_R39M_PRISON_GTE_H,
      RE15_R39M_PRISON_L_BREITE_PX1000, RE15_R39M_PRISON_L_HOEHE_PX1000 },
};

/* Arbeitsstand je Schluessel: geliefertes MD1, RE2-Original-MD1, TIM, Kamera, Lage. */
typedef struct {
    const uint8_t *md1; int md1_sz;
    uint8_t *re2; re15_md1_t re2_md;    /* RE2-Original (info/re2leon) */
    re15_tim_t tim; re15_camera_view_t kv; int32_t pos[3]; int16_t rot[3];
} lauf_t;

static int laden(const schl_t *k, lauf_t *l)
{
    char p[700]; int n = 0, ts = 0; const uint8_t *tb;
    memset(l, 0, sizeof *l);
    l->md1 = re15_itemorte_r38_schluessel_md1(k->item, &l->md1_sz);
    tb = re15_schluessel_r36_tim_bytes(k->raum, &ts);
    snprintf(p, sizeof p, "%s/%s", RE15_RE2_DIR, k->re2_rdt);
    {
        uint8_t *rdt = slurp(p, &n);
        if (rdt && k->re2_off + l->md1_sz <= n) {
            l->re2 = (uint8_t *)malloc((size_t)l->md1_sz);
            memcpy(l->re2, rdt + k->re2_off, (size_t)l->md1_sz);
        }
        free(rdt);
    }
    if (!l->md1 || !tb || !l->re2 || re15_md1_parse(l->re2, l->md1_sz, &l->re2_md) != 0 || l->re2_md.mesh_count != 1 ||
        re15_tim_parse(tb, ts, &l->tim) != 0 || l->tim.bpp != 8 || kamera(k->raum, 5, &l->kv) != 0 ||
        !re15_itemorte_r38_schluessel_lage(k->item, l->pos, l->rot)) {
        printf("FAIL: %s: MD1/TIM/RE2-Modell (%s @0x%lX)/Kamera/Lage nicht ladbar\n", k->name, p, k->re2_off);
        fails++;
        return -1;
    }
    return 0;
}

/* Netz bei Massstab s wie das Werkzeug (md1_skaliert): RE2-Modell, Ecken rint((v - Mitte) * s) (Python round = halbe
 * Werte zur geraden Zahl = rint im Standard-Rundungsmodus), y = 0; Rueckgabe: geparstes Netz in `buf`. */
static int netz_bei(const schl_t *k, const lauf_t *l, double s, uint8_t *buf, re15_md1_t *md)
{
    const re15_md1_mesh_t *h0 = &l->re2_md.meshes[0];
    long voff = (long)((const uint8_t *)h0->tri_vertices - l->re2);
    double mx = k->mx100000 / 100000.0, mz = k->mz100000 / 100000.0;
    int i;
    memcpy(buf, l->re2, (size_t)l->md1_sz);
    for (i = 0; i < h0->tri_vertex_count; i++) {
        int16_t x = (int16_t)rint((h0->tri_vertices[i].x - mx) * s);
        int16_t z = (int16_t)rint((h0->tri_vertices[i].z - mz) * s), y = 0;
        memcpy(buf + voff + 8 * i + 0, &x, 2);
        memcpy(buf + voff + 8 * i + 2, &y, 2);
        memcpy(buf + voff + 8 * i + 4, &z, 2);
    }
    return re15_md1_parse(buf, l->md1_sz, md);
}

typedef struct { long sicht, ausser; double x0, x1, y0, y1; } huelle_t;

/* Gezeichnete Huelle (stetig): Dreiecke direkt, Vierecke wie der Port (0,1,3)+(0,3,2), Raster N = 48, Texel = (int) des
 * baryzentrisch interpolierten UV, CLUT-Farbe 0x0000 = durchsichtig (wie unit_r38_itemorte_lage). */
static huelle_t huelle(const re15_md1_mesh_t *h, const lauf_t *l)
{
    huelle_t r; double m[9];
    const uint8_t *px = (const uint8_t *)l->tim.pixels;
    int nf = h->triangle_count + 2 * h->quad_count, fi;
    prop_matrix(l->rot[0], l->rot[1], l->rot[2], m);
    r.sicht = 0; r.ausser = 0; r.x0 = 1e9; r.x1 = -1e9; r.y0 = 1e9; r.y1 = -1e9;
    for (fi = 0; fi < nf; fi++) {
        const re15_md1_vertex_t *P[3]; int U[3], V[3], a, b, c; uint16_t clut; int zeile, spalte;
        const int N = 48;
        if (fi < h->triangle_count) {
            const re15_md1_triangle_t *t = &h->triangles[fi]; const re15_md1_tri_uv_t *uv = &h->triangle_uvs[fi];
            P[0] = &h->tri_vertices[t->v0]; P[1] = &h->tri_vertices[t->v1]; P[2] = &h->tri_vertices[t->v2];
            U[0] = uv->u0; V[0] = uv->v0; U[1] = uv->u1; V[1] = uv->v1; U[2] = uv->u2; V[2] = uv->v2; clut = uv->clut;
        } else {
            static const int idx[2][3] = { { 0, 1, 3 }, { 0, 3, 2 } };
            int qi = (fi - h->triangle_count) / 2, half = (fi - h->triangle_count) % 2;
            const re15_md1_quad_t *qd = &h->quads[qi]; const re15_md1_quad_uv_t *uv = &h->quad_uvs[qi];
            const re15_md1_vertex_t *Q[4];
            int QU[4], QV[4];
            Q[0] = &h->quad_vertices[qd->v0]; Q[1] = &h->quad_vertices[qd->v1];
            Q[2] = &h->quad_vertices[qd->v2]; Q[3] = &h->quad_vertices[qd->v3];
            QU[0] = uv->u0; QU[1] = uv->u1; QU[2] = uv->u2; QU[3] = uv->u3;
            QV[0] = uv->v0; QV[1] = uv->v1; QV[2] = uv->v2; QV[3] = uv->v3;
            for (c = 0; c < 3; c++) { P[c] = Q[idx[half][c]]; U[c] = QU[idx[half][c]]; V[c] = QV[idx[half][c]]; }
            clut = uv->clut;
        }
        zeile = (int)(clut >> 6) - l->tim.clut_y; spalte = (int)(clut & 0x3F) * 16 - l->tim.clut_x;
        for (a = 0; a <= N; a++) for (b = 0; a + b <= N; b++) {
            double l1 = (double)a / N, l2 = (double)b / N, l0 = 1.0 - l1 - l2, mx, my, mz, w[3], sx, sy;
            int tu = (int)(l0 * U[0] + l1 * U[1] + l2 * U[2]), tv = (int)(l0 * V[0] + l1 * V[1] + l2 * V[2]), ci;
            if (tu < 0 || tv < 0 || tu >= l->tim.width || tv >= l->tim.height) continue;
            ci = px[tv * l->tim.width + tu];
            if (l->tim.clut[zeile * 256 + spalte + ci] == 0x0000) continue;
            mx = l0 * P[0]->x + l1 * P[1]->x + l2 * P[2]->x;
            my = l0 * P[0]->y + l1 * P[1]->y + l2 * P[2]->y;
            mz = l0 * P[0]->z + l1 * P[1]->z + l2 * P[2]->z;
            w[0] = m[0] * mx + m[1] * my + m[2] * mz + l->pos[0];
            w[1] = m[3] * mx + m[4] * my + m[5] * mz + l->pos[1];
            w[2] = m[6] * mx + m[7] * my + m[8] * mz + l->pos[2];
            projiziere(&l->kv, w[0], w[1], w[2], &sx, &sy);
            r.sicht++;
            if (sx < KS_MX0 - 0.5 || sx > KS_MX1 + 1.5 || sy < KS_MY0 - 0.5 || sy > KS_MY1 + 1.5) r.ausser++;
            if (sx < r.x0) r.x0 = sx;
            if (sx > r.x1) r.x1 = sx;
            if (sy < r.y0) r.y0 = sy;
            if (sy > r.y1) r.y1 = sy;
        }
    }
    return r;
}

/* GTE-Viereck: ganzzahlige Bildschirm-Ecken JEDES Dreiecks wie der Port-Prop-Zeichner (platform/pc/main.c: Drehung
 * pc_prop_rot_q12, re15_camera_compose_view_bone, re15_objpoly_r38_tri_mesh mit OFX/OFY 160/120). ecken = die 5
 * Netz-Ecken in MD1-Reihenfolge (fuer den Vergleich "dieselben Ecken"). Rueckgabe x0,x1,y0,y1. */
static void gte(const re15_md1_mesh_t *h, const lauf_t *l, int q[4], int ecken[8][2])
{
    int32_t pr[9], bm[9], bt[3]; int ti, i;
    prop_rot_q12(l->rot[0], l->rot[1], l->rot[2], pr);
    re15_camera_compose_view_bone(&l->kv, pr, l->pos, bm, bt);
    q[0] = 100000; q[1] = -100000; q[2] = 100000; q[3] = -100000;
    for (i = 0; i < 8; i++) ecken[i][0] = ecken[i][1] = -9999;
    for (ti = 0; ti < h->triangle_count; ti++) {
        const re15_md1_triangle_t *t = &h->triangles[ti];
        const re15_md1_vertex_t *vp[3];
        const int vi[3] = { t->v0, t->v1, t->v2 };
        int ax[3], ay[3], wz, k;
        vp[0] = &h->tri_vertices[t->v0]; vp[1] = &h->tri_vertices[t->v1]; vp[2] = &h->tri_vertices[t->v2];
        (void)re15_objpoly_r38_tri_mesh(vp, bm, bt, l->kv.fov_screen_dist, 160, 120, ax, ay, &wz);
        for (k = 0; k < 3; k++) {
            if (ax[k] < q[0]) q[0] = ax[k];
            if (ax[k] > q[1]) q[1] = ax[k];
            if (ay[k] < q[2]) q[2] = ay[k];
            if (ay[k] > q[3]) q[3] = ay[k];
            if (vi[k] < 8) { ecken[vi[k]][0] = ax[k]; ecken[vi[k]][1] = ay[k]; }
        }
    }
}

static void teil_halb(void)
{
    const double faktor = RE15_R39M_FAKTOR100 / 100.0;
    int i;
    printf("\n[halb] (1) gezeichnete Huelle des gelieferten Netzes gegen Stand Spur L, ROOM10F0/10F1 Cut 5\n");
    for (i = 0; i < 2; i++) {
        const schl_t *k = &k_schl[i];
        static lauf_t l; static re15_md1_t md, mdl, mdn; static uint8_t bl[MD1_MAX], bn[MD1_MAX];
        huelle_t hm, hl; double bm, hm_, bw, hw; int gleich = 1, j, voff, nv;
        if (laden(k, &l)) continue;
        if (l.md1_sz > MD1_MAX || re15_md1_parse(l.md1, l.md1_sz, &md) != 0 ||
            netz_bei(k, &l, k->sl10000 / 10000.0, bl, &mdl) != 0 || netz_bei(k, &l, k->s10000 / 10000.0, bn, &mdn) != 0) {
            printf("FAIL: %s: MD1 nicht lesbar\n", k->name); fails++; continue;
        }
        /* das gelieferte MD1 = RE2-Modell mit den Ecken des Werkzeugs bei s = RE15_R38K_*_S10000 (sonst nichts geaendert) */
        voff = (int)((const uint8_t *)l.re2_md.meshes[0].tri_vertices - l.re2);
        nv = l.re2_md.meshes[0].tri_vertex_count;
        for (j = 0; j < l.md1_sz; j++) {
            int in_ecken = j >= voff && j < voff + 8 * nv;
            if (in_ecken ? l.md1[j] != bn[j] : l.md1[j] != l.re2[j]) gleich = 0;
        }
        CHECK(gleich, "%s: geliefertes MD1 (%d B) == RE2-Modell %s @0x%lX mit Ecken rint((v - Mitte) x %d/10000), sonst bytegleich",
              k->name, l.md1_sz, k->re2_rdt, k->re2_off, k->s10000);
        hm = huelle(&md.meshes[0], &l);
        hl = huelle(&mdl.meshes[0], &l);
        bm = hm.x1 - hm.x0; hm_ = hm.y1 - hm.y0; bw = hl.x1 - hl.x0; hw = hl.y1 - hl.y0;
        printf("      %s: Massstab %d/10000 (Spur L %d): Huelle geliefert x %.3f..%.3f y %.3f..%.3f = %.3f x %.3f px,"
               " Spur L x %.3f..%.3f y %.3f..%.3f = %.3f x %.3f px -> %.1f %% / %.1f %%; Abtastpunkte %ld / %ld\n",
               k->name, k->s10000, k->sl10000, hm.x0, hm.x1, hm.y0, hm.y1, bm, hm_, hl.x0, hl.x1, hl.y0, hl.y1, bw, hw,
               100.0 * bm / bw, 100.0 * hm_ / hw, hm.sicht, hl.sicht);
        /* Raster 48 hier gegen 48 + 480 im Werkzeug: hier bis ~0,06 px schmaler */
        CHECK(fabs(bw - k->l_breite1000 / 1000.0) <= 0.1 && fabs(hw - k->l_hoehe1000 / 1000.0) <= 0.1,
              "%s: Stand Spur L nachgebildet: %.3f x %.3f px == Werkzeug %.3f x %.3f (+-0,1)", k->name, bw, hw,
              k->l_breite1000 / 1000.0, k->l_hoehe1000 / 1000.0);
        CHECK(bm <= faktor * bw + 0.03 && hm_ <= faktor * hw + 0.03,
              "%s: (1) NUTZER-VORGABE \"maximal halb so gross\": Breite %.3f <= %.2f x %.3f, Hoehe %.3f <= %.2f x %.3f (+0,03)",
              k->name, bm, faktor, bw, hm_, faktor, hw);
        CHECK(fabs((hm.x0 + hm.x1) - (hl.x0 + hl.x1)) / 2.0 <= 0.1 && fabs((hm.y0 + hm.y1) - (hl.y0 + hl.y1)) / 2.0 <= 0.1,
              "%s: Mitte unveraendert (%.2f ; %.2f) gegen Spur L (%.2f ; %.2f) +-0,1 px - skaliert um die Mitte der Marke",
              k->name, (hm.x0 + hm.x1) / 2.0, (hm.y0 + hm.y1) / 2.0, (hl.x0 + hl.x1) / 2.0, (hl.y0 + hl.y1) / 2.0);
        CHECK(hm.sicht > 0 && hm.sicht == hl.sicht && hm.ausser == 0,
              "%s: dieselben %ld sichtbaren Abtastpunkte wie Spur L (nur kleiner), alle in der Marke x %d..%d y %d..%d",
              k->name, hm.sicht, KS_MX0, KS_MX1, KS_MY0, KS_MY1);
        free(l.re2);
    }
}

static int gleiche_ecken(int a[8][2], int b[8][2], int n)
{
    int i;
    for (i = 0; i < n; i++) if (a[i][0] != b[i][0] || a[i][1] != b[i][1]) return 0;
    return 1;
}

static void teil_raster(void)
{
    const double faktor = RE15_R39M_FAKTOR100 / 100.0;
    int i;
    printf("\n[raster] (2) GTE-Viereck mit den Engine-Funktionen (RTPS/UNR wie FUN_80022f0c @0x80022fb0 -> SXY @0x80022fd8)\n");
    for (i = 0; i < 2; i++) {
        const schl_t *k = &k_schl[i];
        static lauf_t l; static re15_md1_t md, mdl, mdo, mdu1, mdu2; static uint8_t bl[MD1_MAX], bo[MD1_MAX], bu1[MD1_MAX], bu2[MD1_MAX];
        int qm[4], ql[4], qo[4], qu1[4], qu2[4], em[8][2], el[8][2], eo[8][2], eu1[8][2], eu2[8][2], nv;
        double so = k->s_oben / 10000.0 + 0.0005;
        huelle_t ho, hl;
        if (laden(k, &l)) continue;
        if (l.md1_sz > MD1_MAX || re15_md1_parse(l.md1, l.md1_sz, &md) != 0 ||
            netz_bei(k, &l, k->sl10000 / 10000.0, bl, &mdl) != 0 || netz_bei(k, &l, so, bo, &mdo) != 0 ||
            netz_bei(k, &l, k->s_unten / 10000.0 + 0.0002, bu1, &mdu1) != 0 ||
            netz_bei(k, &l, k->s_oben / 10000.0 - 0.0002, bu2, &mdu2) != 0) {
            printf("FAIL: %s: MD1 nicht lesbar\n", k->name); fails++; free(l.re2); continue;
        }
        nv = md.meshes[0].tri_vertex_count;
        gte(&md.meshes[0], &l, qm, em);
        gte(&mdl.meshes[0], &l, ql, el);
        gte(&mdo.meshes[0], &l, qo, eo);
        gte(&mdu1.meshes[0], &l, qu1, eu1);
        gte(&mdu2.meshes[0], &l, qu2, eu2);
        printf("      %s: Spur L x %d..%d y %d..%d (%d x %d) | geliefert x %d..%d y %d..%d (%d x %d) | knapp ueber S_OBEN (s %.4f)"
               " x %d..%d y %d..%d (%d x %d)\n", k->name, ql[0], ql[1], ql[2], ql[3], ql[1] - ql[0], ql[3] - ql[2],
               qm[0], qm[1], qm[2], qm[3], qm[1] - qm[0], qm[3] - qm[2], so, qo[0], qo[1], qo[2], qo[3], qo[1] - qo[0], qo[3] - qo[2]);
        CHECK(ql[0] == k->l_gte_x0 && ql[2] == k->l_gte_y0 && ql[1] - ql[0] == k->l_gte_b && ql[3] - ql[2] == k->l_gte_h,
              "%s: GTE-Viereck Spur L (%d,%d) %d x %d == Werkzeug (%d,%d) %d x %d (RE15_TRILOG der exe: 6 x 3 / 6 x 4)",
              k->name, ql[0], ql[2], ql[1] - ql[0], ql[3] - ql[2], k->l_gte_x0, k->l_gte_y0, k->l_gte_b, k->l_gte_h);
        CHECK(qm[0] == k->gte_x0 && qm[2] == k->gte_y0 && qm[1] - qm[0] == k->gte_b && qm[3] - qm[2] == k->gte_h,
              "%s: GTE-Viereck geliefert (%d,%d) %d x %d == Werkzeug (%d,%d) %d x %d", k->name, qm[0], qm[2],
              qm[1] - qm[0], qm[3] - qm[2], k->gte_x0, k->gte_y0, k->gte_b, k->gte_h);
        CHECK(qm[1] - qm[0] <= faktor * (ql[1] - ql[0]) && qm[3] - qm[2] <= faktor * (ql[3] - ql[2]) && qm[1] > qm[0] &&
              qm[3] > qm[2],
              "%s: (2) NUTZER-VORGABE \"maximal halb so gross\" im GTE-Raster: %d x %d <= %.2f x (%d x %d), nicht entartet",
              k->name, qm[1] - qm[0], qm[3] - qm[2], faktor, ql[1] - ql[0], ql[3] - ql[2]);
        /* groesstes zulaessiges Viereck: knapp ueber dem Intervall verletzt der Massstab (1) oder (2) */
        ho = huelle(&mdo.meshes[0], &l);
        hl = huelle(&mdl.meshes[0], &l);
        CHECK((qo[1] - qo[0] > faktor * (ql[1] - ql[0]) || qo[3] - qo[2] > faktor * (ql[3] - ql[2])) ||
              (ho.x1 - ho.x0 > faktor * (hl.x1 - hl.x0) || ho.y1 - ho.y0 > faktor * (hl.y1 - hl.y0)),
              "%s: knapp ueber S_OBEN (s %.4f) verletzt eine Grenze: GTE %d x %d (Grenze %.1f x %.1f), Huelle %.3f x %.3f"
              " (Grenze %.3f x %.3f) - geliefert ist das groesste zulaessige Viereck", k->name, so, qo[1] - qo[0],
              qo[3] - qo[2], faktor * (ql[1] - ql[0]), faktor * (ql[3] - ql[2]), ho.x1 - ho.x0, ho.y1 - ho.y0,
              faktor * (hl.x1 - hl.x0), faktor * (hl.y1 - hl.y0));
        CHECK(gleiche_ecken(eu1, em, nv) && gleiche_ecken(eu2, em, nv),
              "%s: an beiden Enden des Intervalls (s %.4f / %.4f) dieselben ganzzahligen Ecken wie geliefert (%d,%d)..: dasselbe Bild",
              k->name, k->s_unten / 10000.0 + 0.0002, k->s_oben / 10000.0 - 0.0002, em[0][0], em[0][1]);
        CHECK(k->s_unten < k->s10000 && k->s10000 < k->s_oben,
              "%s: geliefert %d/10000 liegt im Intervall gleicher Ecken %d..%d", k->name, k->s10000, k->s_unten, k->s_oben);
        free(l.re2);
    }
}

static void teil_konst(void)
{
    int pl_ok, pr_ok; int32_t lp[3]; int16_t lr[3];
    printf("\n[konst] Herkunft der Spur-M-Konstanten (gen/itemorte_r38_lage.inc)\n");
    CHECK(RE15_R39M_FAKTOR100 == 50, "RE15_R39M_FAKTOR100 == 50 (NUTZER-VORGABE Nachtrag 5 \"maximal halb so gross\", kein RE-Wert)");
    CHECK(RE15_R38K_PARK_S10000 < RE15_R39M_PARK_SL_10000 / 2 + 1 && RE15_R38K_PRISON_S10000 < RE15_R39M_PRISON_SL_10000 / 2 + 1,
          "Massstab Park %d <= 0,5 x Spur L %d, Prison %d <= 0,5 x %d", RE15_R38K_PARK_S10000, RE15_R39M_PARK_SL_10000,
          RE15_R38K_PRISON_S10000, RE15_R39M_PRISON_SL_10000);
    /* Huellen des Werkzeugs (Raster 48 UND 480): <= Faktor x Spur L, ganzzahlig in Tausendstel Pixel (+1 Rundung) */
    CHECK(RE15_R39M_PARK_BREITE_PX1000 * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PARK_L_BREITE_PX1000 + 100 &&
          RE15_R39M_PARK_HOEHE_PX1000 * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PARK_L_HOEHE_PX1000 + 100 &&
          RE15_R39M_PRISON_BREITE_PX1000 * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PRISON_L_BREITE_PX1000 + 100 &&
          RE15_R39M_PRISON_HOEHE_PX1000 * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PRISON_L_HOEHE_PX1000 + 100,
          "Werkzeug-Huellen <= 0,5 x Spur L: Park %d x %d (L %d x %d), Prison %d x %d (L %d x %d) /1000 px",
          RE15_R39M_PARK_BREITE_PX1000, RE15_R39M_PARK_HOEHE_PX1000, RE15_R39M_PARK_L_BREITE_PX1000,
          RE15_R39M_PARK_L_HOEHE_PX1000, RE15_R39M_PRISON_BREITE_PX1000, RE15_R39M_PRISON_HOEHE_PX1000,
          RE15_R39M_PRISON_L_BREITE_PX1000, RE15_R39M_PRISON_L_HOEHE_PX1000);
    CHECK(RE15_R39M_PARK_GTE_B * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PARK_L_GTE_B &&
          RE15_R39M_PARK_GTE_H * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PARK_L_GTE_H &&
          RE15_R39M_PRISON_GTE_B * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PRISON_L_GTE_B &&
          RE15_R39M_PRISON_GTE_H * 100 <= RE15_R39M_FAKTOR100 * RE15_R39M_PRISON_L_GTE_H,
          "GTE-Vierecke <= 0,5 x Spur L: Park %d x %d (L %d x %d), Prison %d x %d (L %d x %d)",
          RE15_R39M_PARK_GTE_B, RE15_R39M_PARK_GTE_H, RE15_R39M_PARK_L_GTE_B, RE15_R39M_PARK_L_GTE_H,
          RE15_R39M_PRISON_GTE_B, RE15_R39M_PRISON_GTE_H, RE15_R39M_PRISON_L_GTE_B, RE15_R39M_PRISON_L_GTE_H);
    /* Basis = der von Spur L abgenommene Stand (RE2 oberes Quartil, RE15_R39L_*_HUELLE_PX100) */
    CHECK(abs(RE15_R39M_PARK_L_BREITE_PX1000 - RE15_R39L_PARK_HUELLE_PX100 * 10) <= 5 &&
          abs(RE15_R39M_PRISON_L_BREITE_PX1000 - RE15_R39L_PRISON_HUELLE_PX100 * 10) <= 5 &&
          abs(RE15_R39L_PARK_HUELLE_PX100 - RE15_R39L_PARK_ZIEL_PX100) <= 6 &&
          abs(RE15_R39L_PRISON_HUELLE_PX100 - RE15_R39L_PRISON_ZIEL_PX100) <= 6 &&
          RE15_R39M_PARK_SL_10000 == 24100 && RE15_R39M_PRISON_SL_10000 == 25623,
          "Basis = Spur L: Massstab 24100 / 25623, Huelle %d / %d (x1000) == RE15_R39L_*_HUELLE_PX100 %d / %d == RE2-Quartil %d / %d",
          RE15_R39M_PARK_L_BREITE_PX1000, RE15_R39M_PRISON_L_BREITE_PX1000, RE15_R39L_PARK_HUELLE_PX100,
          RE15_R39L_PRISON_HUELLE_PX100, RE15_R39L_PARK_ZIEL_PX100, RE15_R39L_PRISON_ZIEL_PX100);
    /* Lage, Drehung, Aufhebe-Zone: Runde 38 K / Spur L unveraendert -> Aufnahme wie bisher */
    pl_ok = re15_itemorte_r38_schluessel_lage(0x45, lp, lr) && lp[0] == 1358 && lp[1] == -2495 && lp[2] == -3400 &&
            lr[0] == 3072 && lr[1] == 0 && lr[2] == 2048;
    pr_ok = re15_itemorte_r38_schluessel_lage(0x42, lp, lr) && lp[0] == 1358 && lp[1] == -2495 && lp[2] == -3400 &&
            lr[0] == 1024 && lr[1] == 0 && lr[2] == 0;
    CHECK(pl_ok && pr_ok, "Lage (1358,-2495,-3400), Drehung Park (3072,0,2048) / Prison (1024,0,0) unveraendert");
    CHECK(RE15_R38K_KS_RECT_X == 798 && RE15_R38K_KS_RECT_Z == -3600 && RE15_R38K_KS_RECT_W == 1000 &&
          RE15_R38K_KS_RECT_D == 1000 && RE15_R38K_SCHL_SLOT == 8 && RE15_R38K_SCHL_ATTR == 0x0010,
          "Aufhebe-Rechteck (798,-3600) 1000x1000, Slot 8, flaches Licht 0x0010 unveraendert");
    CHECK(re15_itemorte_r38_schluessel_massstab(0x45) == RE15_R38K_PARK_S10000 &&
          re15_itemorte_r38_schluessel_massstab(0x42) == RE15_R38K_PRISON_S10000,
          "Engine liefert den Spur-M-Massstab (re15_itemorte_r38_schluessel_massstab %d / %d)",
          re15_itemorte_r38_schluessel_massstab(0x45), re15_itemorte_r38_schluessel_massstab(0x42));
}

int main(int argc, char **argv)
{
    const char *teil = argc > 1 ? argv[1] : "alle";
    if (!strcmp(teil, "halb") || !strcmp(teil, "alle")) teil_halb();
    if (!strcmp(teil, "raster") || !strcmp(teil, "alle")) teil_raster();
    if (!strcmp(teil, "konst") || !strcmp(teil, "alle")) teil_konst();
    printf("\nr39_schluessel2 %s: %d Pruefungen, %d Fehler -> %s\n", teil, checks, fails, fails ? "FEHLER" : "OK");
    return fails ? 1 : 0;
}
