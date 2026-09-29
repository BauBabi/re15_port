/* probe_r32_hebetisch_faecher.c — RIEGEL Runde 32 / H: Granate und Sicherung liegen in den UNTEREN
 * Faechern des Hebetischs (ROOM1150/1151 Prop 0), die Granate im linken, die Sicherung im rechten.
 *
 * Auftrag (analysis/befunde_runde32/AUFTRAG.md): "Die Granate und die Sicherung die im
 * hochfahrenden Modell in ROOM 1170 rein soll, liegt aktuell oben drauf. Aber sie sollen unten, in
 * den hochfahrenden Fach liegen - ein item links ein item rechts." Dossier
 * analysis/befunde_runde32/hebetisch_faecher.md, Konstanten include/re15_granate.h /
 * include/re15_sicherung.h (⛔ PORT-WAHL, KEINE ORIGINAL-ADRESSE — das Original hat im Hebetisch
 * keine Beute; belegt sind die Geometrie-Bytes).
 *
 * PRUEFUNGEN (je ROOM1150.RDT und ROOM1151.RDT, Plattform-Koordinaten, +Y nach unten):
 *   1  Prop 0 traegt genau ZWEI durchgehende Faecher (waagrechtes Viereck-Paar Boden/Decke von
 *      x=2 bis x=-1258 plus je eine senkrechte Wand an beiden z-Grenzen), dahinter je eine volle
 *      Tafel bei x=-1258: A z 96..861 Boden -90 Decke -810, B z 950..1715 Boden -91 Decke -811
 *      (ROOM1150: Boden A Viereck 89 @0x12EE0, Boden B Viereck 93 @0x12F20)
 *   2  nach dem Raumstart (scd_room_reenter) tragen obj 7 (Granate) und obj 4 (Sicherung) genau
 *      Sitz und Drehung der Konstanten und haengen an der Plattform (parent_obj 0, rot_y 2048)
 *   3  beide liegen AUF ihrem Fachboden: tiefster Punkt genau = Boden A bzw. Boden B
 *   4  jeder Modellpunkt (Kanten in 8 Stuecke, Flaechenmitten; Engine-Trig) liegt im offenen
 *      Inneren seines Fachs: Abstand zu vorn (x=2), hinten, beiden Waenden und Decke > 0 -> kein
 *      Durchstoss, Gegenstand ganz im Fach; die Granate in A, die Sicherung in B
 *   5  keine andere Flaeche von Prop 0 ragt in das Innere von A oder B (Clip-Test) -> nichts, was
 *      die Gegenstaende schneiden koennte
 *   6  SICHTBAR von der Szenenkamera Cut 4 aus, Plattform y=-1205 (Ruhe oben, der Dialog geht auf)
 *      und y=-905 (mitten im Hub): die Sichtlinie jedes Modellpunkts zur Kamera schneidet die
 *      Vorderebene x=2 INNERHALB der Oeffnung des eigenen Fachs -> weder Trennwand noch Rahmen noch
 *      Tischplatte verdecken ihn (GEMESSEN im Framedump: Dossier §4)
 *   7  Schirm (re15_camera_build_view, Cut 4, beide Plattformlagen): Granate ganz LINKS der
 *      Trennwand-Mitte, Sicherung ganz RECHTS davon
 *
 * Rueckgabe 0 = alles bestanden, sonst die Nummer der ersten gerissenen Pruefung.
 */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"
#include "re15_md1.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_skeleton.h"
#include "re15_camera.h"
#include "re15_sicherung.h"
#include "re15_granate.h"
#include "re15_inventory.h"

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static int fehler = 0, erste = 0;
static void pruefe(int nr, const char *was, int ok_)
{
    printf("   [%s] %d. %s\n", ok_ ? "OK " : "FEHL", nr, was);
    if (!ok_) { fehler++; if (!erste) erste = nr; }
}

static uint8_t *datei(const char *rel, size_t *n)
{
    char p[700];
    snprintf(p, sizeof p, "%s/%s", RE15_XSTR(RE15_ASSETS_PATH), rel);
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "nicht lesbar: %s\n", p); return NULL; }
    fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)s);
    if (!b || fread(b, 1, (size_t)s, f) != (size_t)s) { fclose(f); free(b); return NULL; }
    fclose(f); *n = (size_t)s; return b;
}

/* ------------------------------------------------------------ Faecher --------------------- */
#define X_VORN    2
#define X_HINTEN  (-1258)

typedef struct { int z0, z1, boden, decke, q_boden, q_decke, q_w0, q_w1, rueckwand; } fach_t;

static void viereck(const re15_md1_mesh_t *m, int qi, const re15_md1_vertex_t *e[4])
{
    const re15_md1_quad_t *q = &m->quads[qi];      /* Z-Ordnung 0-1-3-2 = Umlauf */
    e[0] = &m->quad_vertices[q->v0]; e[1] = &m->quad_vertices[q->v1];
    e[2] = &m->quad_vertices[q->v3]; e[3] = &m->quad_vertices[q->v2];
}

/* achsparalleles Viereck von x=2 bis x=-1258: art 1 = waagrecht (ein y, zwei z), art 2 = senkrecht
 * (ein z, zwei y). Liefert die konstante Koordinate und die Spanne der anderen. */
static int durchgehend(const re15_md1_mesh_t *m, int qi, int art, int *fest, int *lo, int *hi)
{
    const re15_md1_vertex_t *e[4]; viereck(m, qi, e);
    int vorn = 0, hinten = 0;
    for (int k = 0; k < 4; k++) {
        if (e[k]->x == X_VORN) vorn++; else if (e[k]->x == X_HINTEN) hinten++; else return 0;
    }
    if (vorn != 2 || hinten != 2) return 0;
    int kf = art == 1 ? 1 : 2, ks = art == 1 ? 2 : 1;          /* fest: y bzw. z */
    int f0 = kf == 1 ? e[0]->y : e[0]->z;
    *lo = 0x7fffffff; *hi = -0x7fffffff;
    for (int k = 0; k < 4; k++) {
        int f = kf == 1 ? e[k]->y : e[k]->z, s = ks == 1 ? e[k]->y : e[k]->z;
        if (f != f0) return 0;
        if (s < *lo) *lo = s;
        if (s > *hi) *hi = s;
    }
    *fest = f0;
    return *hi > *lo;
}

static int faecher_finden(const re15_md1_mesh_t *m, fach_t *out, int max)
{
    int n = 0;
    for (int a = 0; a < m->quad_count; a++) {
        int ya, za0, za1;
        if (!durchgehend(m, a, 1, &ya, &za0, &za1)) continue;
        for (int b = 0; b < m->quad_count; b++) {
            int yb, zb0, zb1;
            if (b == a || !durchgehend(m, b, 1, &yb, &zb0, &zb1)) continue;
            if (zb0 != za0 || zb1 != za1 || yb >= ya) continue;         /* b = Decke ueber a = Boden */
            int w0 = -1, w1 = -1;
            for (int c = 0; c < m->quad_count; c++) {
                int zc, yc0, yc1;
                if (!durchgehend(m, c, 2, &zc, &yc0, &yc1) || yc0 != yb || yc1 != ya) continue;
                if (zc == za0) w0 = c;
                if (zc == za1) w1 = c;
            }
            if (w0 < 0 || w1 < 0 || n >= max) continue;
            /* volle Tafel bei x=-1258, die die Fach-Spanne ganz ueberdeckt */
            int rw = -1;
            for (int c = 0; c < m->quad_count && rw < 0; c++) {
                const re15_md1_vertex_t *e[4]; viereck(m, c, e);
                int alle = 1, ylo = 0x7fff, yhi = -0x7fff, zlo = 0x7fff, zhi = -0x7fff;
                for (int k = 0; k < 4; k++) {
                    if (e[k]->x != X_HINTEN) alle = 0;
                    if (e[k]->y < ylo) ylo = e[k]->y;
                    if (e[k]->y > yhi) yhi = e[k]->y;
                    if (e[k]->z < zlo) zlo = e[k]->z;
                    if (e[k]->z > zhi) zhi = e[k]->z;
                }
                if (alle && ylo <= yb && yhi >= ya && zlo <= za0 && zhi >= za1) rw = c;
            }
            fach_t f = { za0, za1, ya, yb, a, b, w0, w1, rw };
            out[n++] = f;
        }
    }
    /* nach z sortieren: out[0] = kleineres z */
    if (n == 2 && out[0].z0 > out[1].z0) { fach_t t = out[0]; out[0] = out[1]; out[1] = t; }
    return n;
}

/* ragt das Polygon ins offene Innere des Fachs (um eps geschrumpft)? Sutherland-Hodgman */
static int ragt_hinein(const re15_md1_vertex_t *const *e, int ne, const fach_t *f)
{
    float poly[16][3], neu[16][3]; int n = ne;
    for (int k = 0; k < ne; k++) { poly[k][0] = e[k]->x; poly[k][1] = e[k]->y; poly[k][2] = e[k]->z; }
    const float eps = 0.5f;
    const struct { int ax; float w; int sg; } eb[6] = {
        {0, X_HINTEN + eps, +1}, {0, X_VORN - eps, -1}, {1, f->decke + eps, +1},
        {1, f->boden - eps, -1}, {2, f->z0 + eps, +1}, {2, f->z1 - eps, -1} };
    for (int p = 0; p < 6 && n > 0; p++) {
        int m = 0;
        for (int i = 0; i < n; i++) {
            const float *a = poly[i], *b = poly[(i + 1) % n];
            int ina = eb[p].sg * (a[eb[p].ax] - eb[p].w) > 0, inb = eb[p].sg * (b[eb[p].ax] - eb[p].w) > 0;
            if (ina && m < 16) { memcpy(neu[m], a, sizeof neu[m]); m++; }
            if (ina != inb && m < 16) {
                float t = (eb[p].w - a[eb[p].ax]) / (b[eb[p].ax] - a[eb[p].ax]);
                for (int j = 0; j < 3; j++) neu[m][j] = a[j] + (b[j] - a[j]) * t;
                m++;
            }
        }
        memcpy(poly, neu, sizeof(float) * 3 * (size_t)m); n = m;
    }
    return n >= 3;
}

/* ------------------------------------------------------------ Proben ---------------------- */
#define MAX_PROBEN 4000
static float G_[MAX_PROBEN][3], S_[MAX_PROBEN][3];

/* Punkte auf allen Kanten (8 Stuecke) + Flaechenmitten, gedreht wie main.c pc_prop_rot_q12
 * (Ry*Rx*Rz, hier nur Ry, Engine-Trig re15_sin_q12/re15_cos_q12). */
static int proben(const uint8_t *md1, int sz, int px, int py, int pz, int ry, float (*out)[3])
{
    static re15_md1_t m;
    if (re15_md1_parse(md1, sz, &m) != 0) return 0;
    const re15_md1_mesh_t *s = &m.meshes[0];
    int sn = re15_sin_q12(ry), cs = re15_cos_q12(ry), n = 0;
    for (int art = 0; art < 2; art++) {
        int nf = art ? s->quad_count : s->triangle_count;
        for (int i = 0; i < nf; i++) {
            const re15_md1_vertex_t *e[4]; int ne;
            if (art) { viereck(s, i, e); ne = 4; }
            else { const re15_md1_triangle_t *t = &s->triangles[i];
                e[0] = &s->tri_vertices[t->v0]; e[1] = &s->tri_vertices[t->v1];
                e[2] = &s->tri_vertices[t->v2]; ne = 3; }
            float mitte[3] = {0, 0, 0};
            for (int k = 0; k < ne; k++) {
                mitte[0] += e[k]->x / (float)ne; mitte[1] += e[k]->y / (float)ne; mitte[2] += e[k]->z / (float)ne;
                const re15_md1_vertex_t *a = e[k], *b = e[(k + 1) % ne];
                for (int j = 0; j < 8 && n < MAX_PROBEN - 1; j++) {
                    float t = j / 8.0f, v[3] = { a->x + (b->x - a->x) * t, a->y + (b->y - a->y) * t,
                                                 a->z + (b->z - a->z) * t };
                    out[n][0] = px + (v[0] * cs + v[2] * sn) / 4096.0f;
                    out[n][1] = py + v[1];
                    out[n][2] = pz + (-v[0] * sn + v[2] * cs) / 4096.0f;
                    n++;
                }
            }
            out[n][0] = px + (mitte[0] * cs + mitte[2] * sn) / 4096.0f;
            out[n][1] = py + mitte[1];
            out[n][2] = pz + (-mitte[0] * sn + mitte[2] * cs) / 4096.0f;
            n++;
        }
    }
    return n;
}

/* ------------------------------------------------------------ Raum ------------------------ */
static int slot_von_obj(uint8_t o)
{
    for (int k = 0; k < (int)g_scd.prop_count; k++) if (g_scd.props[k].obj_id == o) return k;
    return -1;
}

static void raum_frisch(re15_rdt_t *rdt, uint16_t rid)
{
    re15_game_state_init();
    re15_inv_init();
    scd_vm_init(); re15_actor_init();
    g_current_room_id = rid;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;
    pl->x = -21000; pl->y = 0; pl->z = -18500; pl->rot_y = 2048; pl->state = 1;
    g_scd.player_mode = 0;
    { extern void re15_msg_load_room_block(const uint8_t *b, int n);
      re15_msg_load_room_block(rdt->messages, rdt->messages_size); }
    scd_register_room_events(rdt);
    scd_room_reenter(rdt, -21000, -18500, 0);
}

/* Schirm-x eines Plattformpunkts: Plattform rot_y 2048 (main00 Obj_model_set @0x0E00 `00 08`)
 * -> Welt x = px - x, z = pz - z. */
static float schirm_x(const re15_camera_view_t *v, float px, float py, float pz, const float p[3], int *ok)
{
    float w[3] = { px - p[0], py + p[1], pz - p[2] }, c[3];
    for (int r = 0; r < 3; r++)
        c[r] = (v->rot[r * 3] * w[0] + v->rot[r * 3 + 1] * w[1] + v->rot[r * 3 + 2] * w[2]) / 4096.0f + v->trans[r];
    *ok = c[2] >= 64;
    return 160.0f + c[0] * v->fov_screen_dist / c[2];
}

static void raum(uint16_t rid, const char *rel)
{
    printf("\n== %04X (%s) ==\n", rid, rel);
    size_t sz = 0;
    uint8_t *buf = datei(rel, &sz);
    static re15_rdt_t rdt;
    if (!buf || re15_rdt_parse(buf, sz, &rdt) != 0 || rdt.prop_count < 1) {
        pruefe(1, "RDT lesbar", 0); free(buf); return;
    }
    static re15_md1_t p0;
    if (re15_md1_parse(rdt.prop_md1[0], rdt.prop_md1_size[0], &p0) != 0) { pruefe(1, "Prop-0-MD1 lesbar", 0); free(buf); return; }
    const re15_md1_mesh_t *m = &p0.meshes[0];

    /* 1 ---------------------------------------------------------------- Faecher */
    fach_t fa[4];
    int nf = faecher_finden(m, fa, 4);
    long md1_off = (long)(rdt.prop_md1[0] - buf);
    for (int i = 0; i < nf; i++)
        printf("   Fach %c: z %d..%d, Boden y=%d (Viereck %d), Decke y=%d (Viereck %d), Waende Viereck %d/%d, "
               "Rueckwand Viereck %d | Boden-Record @0x%05lX\n", 'A' + i, fa[i].z0, fa[i].z1, fa[i].boden,
               fa[i].q_boden, fa[i].decke, fa[i].q_decke, fa[i].q_w0, fa[i].q_w1, fa[i].rueckwand,
               (long)((const uint8_t *)&m->quads[fa[i].q_boden] - buf));
    printf("   Prop 0 MD1 @0x%05lX\n", md1_off);
    int ok1 = nf == 2 &&
              fa[0].z0 == 96 && fa[0].z1 == 861 && fa[0].boden == -90 && fa[0].decke == -810 &&
              fa[1].z0 == 950 && fa[1].z1 == 1715 && fa[1].boden == -91 && fa[1].decke == -811 &&
              fa[0].rueckwand >= 0 && fa[1].rueckwand >= 0;
    pruefe(1, "genau zwei Faecher: A z 96..861 Boden -90 Decke -810, B z 950..1715 Boden -91 Decke -811, "
              "je eine volle Rueckwand bei x=-1258", ok1);
    if (nf != 2) { free(buf); return; }
    const fach_t *A = &fa[0], *B = &fa[1];

    /* 2 ---------------------------------------------------------------- im Pool */
    raum_frisch(&rdt, rid);
    int gi = slot_von_obj(RE15_GRANATE_OBJ_ID), si = slot_von_obj(RE15_SICHERUNG_OBJ_ID), pi = slot_von_obj(0);
    int ok2 = gi >= 0 && si >= 0 && pi >= 0;
    float plat_x = 0, plat_z = 0;
    if (ok2) {
#define g (&g_scd.props[gi])
#define s (&g_scd.props[si])
#define p (&g_scd.props[pi])
        printf("   Pool: Granate (%ld,%ld,%ld) rot (%d,%d,%d) parent %d | Sicherung (%ld,%ld,%ld) rot (%d,%d,%d) "
               "parent %d | Plattform (%ld,%ld,%ld) rot_y %d\n",
               (long)g->x, (long)g->y, (long)g->z, g->rot_x, g->rot_y, g->rot_z, g->parent_obj,
               (long)s->x, (long)s->y, (long)s->z, s->rot_x, s->rot_y, s->rot_z, s->parent_obj,
               (long)p->x, (long)p->y, (long)p->z, p->rot_y);
        ok2 = g->x == RE15_GRANATE_POS_X && g->y == RE15_GRANATE_POS_Y && g->z == RE15_GRANATE_POS_Z &&
              g->rot_x == 0 && g->rot_y == RE15_GRANATE_ROT_Y && g->rot_z == 0 && g->parent_obj == 0 &&
              s->x == RE15_SICHERUNG_POS_X && s->y == RE15_SICHERUNG_POS_Y && s->z == RE15_SICHERUNG_POS_Z &&
              s->rot_x == 0 && s->rot_y == RE15_SICHERUNG_ROT_Y && s->rot_z == 0 && s->parent_obj == 0 &&
              p->rot_y == 2048;
        plat_x = (float)p->x; plat_z = (float)p->z;
#undef g
#undef s
#undef p
    }
    pruefe(2, "Raumstart: Sitz/Drehung der Konstanten im Pool, beide an der Plattform (parent 0, rot_y 2048)", ok2);

    /* 3/4 -------------------------------------------------------------- Lage im Fach */
    int gsz = 0, ssz = 0;
    const uint8_t *gmd = re15_granate_md1_bytes(&gsz), *smd = re15_sicherung_md1_bytes(&ssz);
    int ng = proben(gmd, gsz, RE15_GRANATE_POS_X, RE15_GRANATE_POS_Y, RE15_GRANATE_POS_Z, RE15_GRANATE_ROT_Y, G_);
    int ns = proben(smd, ssz, RE15_SICHERUNG_POS_X, RE15_SICHERUNG_POS_Y, RE15_SICHERUNG_POS_Z, RE15_SICHERUNG_ROT_Y, S_);
    float ab[2][6];   /* tief, vorn, hinten, wand0, wand1, decke */
    for (int k = 0; k < 2; k++) {
        float (*P)[3] = k ? S_ : G_; int n = k ? ns : ng; const fach_t *f = k ? B : A;
        float ymax = -1e9f, xmax = -1e9f, xmin = 1e9f, zmin = 1e9f, zmax = -1e9f, ymin = 1e9f;
        for (int i = 0; i < n; i++) {
            if (P[i][0] > xmax) xmax = P[i][0];
            if (P[i][0] < xmin) xmin = P[i][0];
            if (P[i][1] > ymax) ymax = P[i][1];
            if (P[i][1] < ymin) ymin = P[i][1];
            if (P[i][2] > zmax) zmax = P[i][2];
            if (P[i][2] < zmin) zmin = P[i][2];
        }
        ab[k][0] = ymax - f->boden; ab[k][1] = X_VORN - xmax; ab[k][2] = xmin - X_HINTEN;
        ab[k][3] = zmin - f->z0;    ab[k][4] = f->z1 - zmax;  ab[k][5] = ymin - f->decke;
        printf("   %s (%d Proben) in Fach %c: tiefster Punkt %+.2f zum Boden | Abstand vorn %.1f hinten %.1f "
               "Wand %.1f / %.1f Decke %.1f\n", k ? "Sicherung" : "Granate  ", n, k ? 'B' : 'A',
               ab[k][0], ab[k][1], ab[k][2], ab[k][3], ab[k][4], ab[k][5]);
    }
    pruefe(3, "beide liegen AUF ihrem Fachboden (tiefster Punkt = Boden A -90 / Boden B -91)",
           ng > 0 && ns > 0 && fabsf(ab[0][0]) < 0.01f && fabsf(ab[1][0]) < 0.01f);
    int ok4 = ng > 0 && ns > 0;
    for (int k = 0; k < 2; k++) for (int j = 1; j < 6; j++) if (!(ab[k][j] > 0.0f)) ok4 = 0;
    pruefe(4, "ganz im Fach, kein Durchstoss: Granate in A, Sicherung in B (alle Abstaende > 0)", ok4);

    /* 5 ---------------------------------------------------------------- Fach leer */
    int hinein = 0;
    for (int qi = 0; qi < m->quad_count; qi++) {
        const re15_md1_vertex_t *e[4]; viereck(m, qi, e);
        if (ragt_hinein(e, 4, A) || ragt_hinein(e, 4, B)) hinein++;
    }
    for (int ti = 0; ti < m->triangle_count; ti++) {
        const re15_md1_triangle_t *t = &m->triangles[ti];
        const re15_md1_vertex_t *e[3] = { &m->tri_vertices[t->v0], &m->tri_vertices[t->v1], &m->tri_vertices[t->v2] };
        if (ragt_hinein(e, 3, A) || ragt_hinein(e, 3, B)) hinein++;
    }
    printf("   Flaechen von Prop 0 im Inneren von A/B: %d (von %d)\n", hinein, m->quad_count + m->triangle_count);
    pruefe(5, "keine andere Flaeche von Prop 0 ragt in ein Fach", hinein == 0);

    /* 6/7 -------------------------------------------------------------- Kamera Cut 4 */
    re15_camera_view_t v;
    int ok6 = ok2 && rdt.cut_count > 4 && re15_camera_build_view(&rdt.cuts[4], &v) == 0, ok7 = ok6;
    for (int lage = 0; ok6 && lage < 2; lage++) {
        int py = lage ? -905 : -1205;
        const re15_camera_cut_t *c = &rdt.cuts[4];
        /* Kamera in Plattform-Koordinaten (rot_y 2048: lokal x = px - Welt x, z = pz - Welt z) */
        float kx = plat_x - (float)c->pos_x, ky = (float)c->pos_y - (float)py, kz = plat_z - (float)c->pos_z;
        int verdeckt[2] = { 0, 0 };
        for (int k = 0; k < 2; k++) {
            float (*P)[3] = k ? S_ : G_; int n = k ? ns : ng; const fach_t *f = k ? B : A;
            for (int i = 0; i < n; i++) {
                float t = (X_VORN - P[i][0]) / (kx - P[i][0]);
                float yc = P[i][1] + t * (ky - P[i][1]), zc = P[i][2] + t * (kz - P[i][2]);
                if (!(t > 0 && t < 1 && yc > f->decke && yc < f->boden && zc > f->z0 && zc < f->z1)) verdeckt[k]++;
            }
        }
        printf("   Plattform y=%d: Kamera lokal (%.0f,%.0f,%.0f); Sichtlinie ausserhalb der eigenen Oeffnung: "
               "Granate %d von %d, Sicherung %d von %d\n", py, kx, ky, kz, verdeckt[0], ng, verdeckt[1], ns);
        if (verdeckt[0] || verdeckt[1]) ok6 = 0;

        /* Trennwand-Mitte vorn: x=2, z=(861+950)/2, halbe Fachhoehe */
        float tw[3] = { (float)X_VORN, (A->boden + A->decke) / 2.0f, (A->z1 + B->z0) / 2.0f };
        int okp; float x_tw = schirm_x(&v, plat_x, (float)py, plat_z, tw, &okp);
        float gmax = -1e9f, smin = 1e9f, gs = 0, ss = 0;
        for (int i = 0; i < ng; i++) { int o; float x = schirm_x(&v, plat_x, (float)py, plat_z, G_[i], &o);
            if (!o) okp = 0; gs += x; if (x > gmax) gmax = x; }
        for (int i = 0; i < ns; i++) { int o; float x = schirm_x(&v, plat_x, (float)py, plat_z, S_[i], &o);
            if (!o) okp = 0; ss += x; if (x < smin) smin = x; }
        printf("   Plattform y=%d, Cut 4: Granate Schirm-x Mitte %.1f rechts bis %.1f | Trennwand %.1f | "
               "Sicherung links ab %.1f Mitte %.1f\n", py, gs / ng, gmax, x_tw, smin, ss / ns);
        if (!(okp && gmax < x_tw && x_tw < smin)) ok7 = 0;
    }
    pruefe(6, "Cut 4, Plattform y=-1205 und -905: jede Sichtlinie geht durch die Oeffnung des eigenen Fachs", ok6);
    pruefe(7, "Cut 4: Granate ganz LINKS der Trennwand, Sicherung ganz RECHTS", ok7);
    free(buf);
}

int main(void)
{
    raum(0x1150, "STAGE1/ROOM1150.RDT");
    raum(0x1151, "STAGE1/ROOM1151.RDT");
    printf("\n%s - %d Pruefung(en) gerissen\n", fehler ? "FEHLGESCHLAGEN" : "ALLES BESTANDEN", fehler);
    return fehler ? (erste ? erste : 1) : 0;
}
