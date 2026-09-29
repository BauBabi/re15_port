/* door_seq_zeichnen.c — RE2-Tuerobjekte zeichnen (FUN_8001468c je Dreieck), plattformfrei.
 *
 * ⛔ RE2-ERGAENZUNG (Beta -> Retail), Begruendung im Kopf von include/re15_door_seq.h.
 * Bis Runde 31 lag die Dreiecksschleife in platform/pc/src/door_scene_pc.c; Runde 32 hat sie
 * unveraendert hierher gezogen (Belege 08_re_zeichnen.md), damit die Unterteilung (Flag 0x20)
 * und ihr Flag-Tor im Test am echten Pfad haengen. Die Plattform bekommt je Dreieck einen
 * Aufruf (re15_door_zeichner_t) und legt es in ihre Warteschlange.
 *
 * Flag 0x20 = Dreiecke unterteilen (PsyQ DivideGT3).
 * Beleg (selbst disassembliert, info/re2leon/PSX.EXE): analysis/befunde_runde32/tueren_unterteilung.md
 * Abschnitt 1. Kurz:
 *   - FUN_8001468c @0x80014a28..34: `lhu v0,324(s7)` / `andi v0,v0,0x20` / `beq` -> sonst
 *     @0x80014a90 `jal 0x8008ebf4` = DivideGT3(v0,v1,v2, uv0,uv1,uv2, rgb0,rgb0,rgb0, s, ot, divp);
 *     die drei Farbzeiger sind ALLE s0+4 (@0x80014a48 addiu v0,s0,4, @0x80014a58/5c/60 sw 24/28/32(sp)),
 *     ot = s4 = OT-Platz des ungeteilten Dreiecks (@0x80014a78 sw s4,40(sp)).
 *   - 0x8008ebf4 = DivideGT3 (Speicherzugriffe gleich psyq-4.7 libgte/dvgt3_04.o): RotAverageNclip3
 *     @0x8008edec (RTPT, NCLIP, `bgtz v0` @0x8008ee34 -> nur MAC0 > 0), ReadSZfifo3 @0x8008edcc,
 *     RCpolyGT3A @0x8008ee84 (296/296 Worte gleich libgte/divgt3a.o).
 *   - DIVPOLYGON3 der Tuerszene: Door_init @0x80013dc0/cc ndiv = 3, @0x80013dd0/d4 pih = 320,
 *     @0x80013dd8/dc piv = 240.
 * Die Rekursion unten ist RCpolyGT3A Schritt fuer Schritt; die Ausgabe (0x8008f288) uebergibt
 * jedes Teildreieck an die Plattform, die es auf den OT-Platz des Ursprungsdreiecks legt. */
#include "re15_door_seq.h"
#include "re15_math.h"
#include "re15_light.h"

#include <string.h>

static int32_t sat16(int32_t v) { return v < -0x8000 ? -0x8000 : (v > 0x7fff ? 0x7fff : v); }

/* RTPS/RTPT (sf=1, lm=0) mit RT = W, TR = T, H = 290 (@0x80013e34), OFX/OFY = 160/120 (RE2
 * SetGeomOffset 0x8008de04, Aufruf @0x80068e80 addiu a0,zero,160 / @0x80068e88 addiu a1,zero,120):
 * IR = sat16((TR<<12 + R*V) >> 12), SZ3 = MAC3 in 0..0xffff, n = Division(H, SZ3),
 * SX = sat((OFX<<16 + IR1*n) >> 16, -0x400..0x3ff), SY ebenso (psx-spx GTE RTPS). */
re15_door_ecke_t re15_door_rtpt(const re15_door_mat_t *w, int16_t vx, int16_t vy, int16_t vz)
{
    re15_door_ecke_t e;
    int32_t ir[3], mac3 = 0;
    const int32_t v[3] = { vx, vy, vz };
    for (int i = 0; i < 3; i++) {
        int32_t s = (int32_t)w->m[i * 3 + 0] * v[0] + (int32_t)w->m[i * 3 + 1] * v[1]
                  + (int32_t)w->m[i * 3 + 2] * v[2];
        int32_t mac = (s >> 12) + w->t[i];
        ir[i] = sat16(mac);
        if (i == 2) mac3 = mac;
    }
    uint32_t sz = mac3 < 0 ? 0u : (mac3 > 0xffff ? 0xffffu : (uint32_t)mac3);
    uint32_t n = re15_gte_divide(RE15_DOOR_H, sz);
    int64_t sx = ((int64_t)RE15_DOOR_OFX << 16) + (int64_t)ir[0] * n;
    int64_t sy = ((int64_t)RE15_DOOR_OFY << 16) + (int64_t)ir[1] * n;
    int32_t x = (int32_t)(sx >> 16), y = (int32_t)(sy >> 16);
    if (x < -0x400) x = -0x400; if (x > 0x3ff) x = 0x3ff;
    if (y < -0x400) y = -0x400; if (y > 0x3ff) y = 0x3ff;
    e.sx = (int16_t)x;
    e.sy = (int16_t)y;
    e.sz = (uint16_t)sz;
    return e;
}

/* CRVECTOR3 (LIBGTE.H): die drei Kantenmitten einer Stufe + Zeiger auf die Ecken. */
typedef struct {
    re15_div_rvec_t r01, r12, r20;
    const re15_div_rvec_t *r0, *r1, *r2;
} crvec3_t;

typedef struct {
    const re15_door_mat_t *w;
    uint32_t ndiv, pih, piv;
    re15_div_ausgabe_t aus;
    void *ctx;
    int n;                        /* ausgegebene Teildreiecke */
    crvec3_t cr[5];               /* DIVPOLYGON3.cr[5] */
} teil_t;

static void ausgabe(teil_t *t, const re15_div_rvec_t *a, const re15_div_rvec_t *b, const re15_div_rvec_t *c)
{
    t->aus(t->ctx, a, b, c);   /* 0x8008f288: Paket fuellen, addPrim auf divp->ot */
    t->n++;
}

/* Kantenmitte: Lage `lh`/`add`/`sra 1` (@0x8008efb4..8008f040), uv und Farbe `lbu`/`addu`/`srl 1`
 * (@0x8008f05c..88 u, @0x8008f090..bc v, @0x8008f0c0..14c r g b); cd wird nicht gemittelt. */
static void mitte(re15_div_rvec_t *m, const re15_div_rvec_t *a, const re15_div_rvec_t *b)
{
    m->vx = (int16_t)(((int32_t)a->vx + b->vx) >> 1);
    m->vy = (int16_t)(((int32_t)a->vy + b->vy) >> 1);
    m->vz = (int16_t)(((int32_t)a->vz + b->vz) >> 1);
    m->u = (uint8_t)(((unsigned)a->u + b->u) >> 1);
    m->v = (uint8_t)(((unsigned)a->v + b->v) >> 1);
    m->r = (uint8_t)(((unsigned)a->r + b->r) >> 1);
    m->g = (uint8_t)(((unsigned)a->g + b->g) >> 1);
    m->b = (uint8_t)(((unsigned)a->b + b->b) >> 1);
}

/* RCpolyGT3A @0x8008ee84 (a2 = Stufe, a3 = cr). */
static void rcpoly(teil_t *t, int stufe, int k)
{
    crvec3_t *cr = &t->cr[k];
    const re15_div_rvec_t *t0 = cr->r0, *t1 = cr->r1, *t2 = cr->r2;

    /* Nah-Verwurf: @0x8008ee90 cfc2 t9,H / @0x8008eea0 sra t8,t9,1 / sltu x3 */
    uint32_t h2 = (uint32_t)(RE15_DOOR_H >> 1);
    if (t0->sz < h2 && t1->sz < h2 && t2->sz < h2) return;

    /* Bild-Verwurf: @0x8008eecc cfc2 OFX, sra 16; @0x8008eedc/e0 srl pih/piv,1; slt je Ecke */
    int32_t hx = (int32_t)(t->pih >> 1), hy = (int32_t)(t->piv >> 1);
    int32_t g = RE15_DOOR_OFX + hx;                                      /* @0x8008eee4 */
    if (g < t0->sx && g < t1->sx && g < t2->sx) return;                   /* @0x8008eef4..0c */
    g = RE15_DOOR_OFX - hx;                                              /* @0x8008ef1c */
    if (t0->sx < g && t1->sx < g && t2->sx < g) return;                   /* @0x8008ef20..38 */
    g = RE15_DOOR_OFY + hy;                                              /* @0x8008ef5c */
    if (g < t0->sy && g < t1->sy && g < t2->sy) return;                   /* @0x8008ef60..78 */
    g = RE15_DOOR_OFY - hy;                                              /* @0x8008ef88 */
    if (t0->sy < g && t1->sy < g && t2->sy < g) return;                   /* @0x8008ef8c..a4 */

    /* Kantenmitten im Objektraum + RTPT (@0x8008f08c) */
    mitte(&cr->r01, t0, t1);
    mitte(&cr->r12, t1, t2);
    mitte(&cr->r20, t2, t0);
    re15_door_ecke_t e01 = re15_door_rtpt(t->w, cr->r01.vx, cr->r01.vy, cr->r01.vz);
    re15_door_ecke_t e12 = re15_door_rtpt(t->w, cr->r12.vx, cr->r12.vy, cr->r12.vz);
    re15_door_ecke_t e20 = re15_door_rtpt(t->w, cr->r20.vx, cr->r20.vy, cr->r20.vz);
    cr->r01.sx = e01.sx; cr->r01.sy = e01.sy;
    cr->r12.sx = e12.sx; cr->r12.sy = e12.sy;
    cr->r20.sx = e20.sx; cr->r20.sy = e20.sy;

    /* @0x8008f150 lw t4,0(a1) (ndiv) / @0x8008f154 addiu a2,a2,1 / @0x8008f158 bne t4,a2 */
    if ((uint32_t)(stufe + 1) == t->ndiv) {
        /* Blatt (@0x8008f160..1bc): SXY abgelegt (SZ nicht), vier Dreiecke in dieser Folge */
        ausgabe(t, t1, &cr->r12, &cr->r01);
        ausgabe(t, &cr->r01, &cr->r12, &cr->r20);
        ausgabe(t, t0, &cr->r01, &cr->r20);
        ausgabe(t, t2, &cr->r20, &cr->r12);
        return;
    }
    if (k + 1 >= 5) return;   /* cr[5] voll - bei ndiv 3 nie (Stufen 0..2) */
    /* tiefer (@0x8008f1d0..e4 SZ + SXY, @0x8008f1e8 a3 += 88, @0x8008f1f0..268 vier Aufrufe) */
    cr->r01.sz = e01.sz; cr->r12.sz = e12.sz; cr->r20.sz = e20.sz;
    crvec3_t *nx = &t->cr[k + 1];
    nx->r0 = t0;        nx->r1 = &cr->r01; nx->r2 = &cr->r20; rcpoly(t, stufe + 1, k + 1);
    nx->r0 = t1;        nx->r1 = &cr->r12; nx->r2 = &cr->r01; rcpoly(t, stufe + 1, k + 1);
    nx->r0 = t2;        nx->r1 = &cr->r20; nx->r2 = &cr->r12; rcpoly(t, stufe + 1, k + 1);
    nx->r0 = &cr->r01;  nx->r1 = &cr->r12; nx->r2 = &cr->r20; rcpoly(t, stufe + 1, k + 1);
}

int re15_door_divide_gt3(const re15_door_mat_t *w, const re15_md1_vertex_t *v[3],
                         const uint8_t uv[3][2], const uint8_t rgb0[3],
                         uint32_t ndiv, uint32_t pih, uint32_t piv,
                         re15_div_ausgabe_t aus, void *ctx)
{
    static teil_t t;             /* DIVPOLYGON3 liegt in RE2 im Arbeitsbereich, nicht auf dem Stapel */
    re15_div_rvec_t r[3];
    if (!w || !aus || ndiv == 0) return 0;   /* ndiv 0 liefe in RE2 ueber cr[5] hinaus; RE2 setzt 3 */
    t.w = w; t.ndiv = ndiv; t.pih = pih; t.piv = piv; t.aus = aus; t.ctx = ctx; t.n = 0;

    /* RotAverageNclip3 @0x8008edec: RTPT der drei Ecken, NCLIP, nur MAC0 > 0 weiter (@0x8008ee34 bgtz) */
    for (int i = 0; i < 3; i++) {
        re15_door_ecke_t e = re15_door_rtpt(w, v[i]->x, v[i]->y, v[i]->z);
        r[i].vx = v[i]->x; r[i].vy = v[i]->y; r[i].vz = v[i]->z;
        r[i].u = uv[i][0]; r[i].v = uv[i][1];
        /* r0/r1/r2.c <- *rgb0/*rgb1/*rgb2 (@0x8008ed68/74/8c) - alle drei Zeiger = rgb0 (@0x80014a48) */
        r[i].r = rgb0[0]; r[i].g = rgb0[1]; r[i].b = rgb0[2];
        r[i].sx = e.sx; r[i].sy = e.sy;
        r[i].sz = e.sz;          /* ReadSZfifo3 @0x8008edcc: SZ1/SZ2/SZ3 */
    }
    int64_t mac0 = (int64_t)r[0].sx * r[1].sy + (int64_t)r[1].sx * r[2].sy + (int64_t)r[2].sx * r[0].sy
                 - (int64_t)r[0].sx * r[2].sy - (int64_t)r[1].sx * r[0].sy - (int64_t)r[2].sx * r[1].sy;
    if (mac0 <= 0) return 0;     /* @0x8008ecf8 blez v0 -> Rueckgabe ohne Paket */

    /* cr[0].r0/r1/r2 = divp->r0/r1/r2 (@0x8008ec38/3c/40), RCpolyGT3A(s, divp, 0, cr) (@0x8008ed88) */
    t.cr[0].r0 = &r[0]; t.cr[0].r1 = &r[1]; t.cr[0].r2 = &r[2];
    rcpoly(&t, 0, 0);
    return t.n;
}

/* ===========================================================================
 * Ein Mesh mit Objektmatrix welt (Licht mit welt_vor) - FUN_80014234 e/f + FUN_8001468c,
 * Rezept analysis/tor_1170/08_re_zeichnen.md 4.2.
 * ======================================================================== */

/* Tuerlicht RE2 (08_re_zeichnen.md 3): Lichtmatrix @0x8009a470, Farbmatrix @0x8009a490
 * (neunmal 0x0640 = 1600), Hintergrundfarbe 68 bzw. 136 bei Flag 0x1000 (@0x800142ac..308). */
static const int16_t L_TUER[9] = { 400, 800, -500,  -1800, -1000, -2700,  3500, 6700, 1200 };
#define LCM_TUER 1600

/* Flag 0x20: jedes Teildreieck auf den OT-Platz des Ursprungsdreiecks (RE2 @0x80014a78
 * sw s4,40(sp) -> RCpolyGT3A-Ausgabe 0x8008f288 addPrim auf divp->ot). Der Port-Schluessel
 * zaehlt wie beim ungeteilten Dreieck weiter: im selben Platz wird das spaeter eingehaengte
 * zuerst gezeichnet (addPrim vorn). */
typedef struct {
    int platz;
    uint16_t page, clut;
    int *lfd;
    re15_door_zeichner_t zeichner;
    void *ctx;
} teil_ziel_t;

static void teil_ausgeben(void *ctx, const re15_div_rvec_t *a, const re15_div_rvec_t *b,
                          const re15_div_rvec_t *c)
{
    teil_ziel_t *k = (teil_ziel_t *)ctx;
    const re15_div_rvec_t *e[3] = { a, b, c };
    re15_door_dreieck_t d;
    for (int i = 0; i < 3; i++) {
        d.x[i] = e[i]->sx; d.y[i] = e[i]->sy;
        d.u[i] = e[i]->u;  d.v[i] = e[i]->v;
        d.rgb[i][0] = e[i]->r; d.rgb[i][1] = e[i]->g; d.rgb[i][2] = e[i]->b;
    }
    d.page = k->page; d.clut = k->clut;
    d.z = k->platz * 4096 + ((*k->lfd)++ & 0xfff);
    d.geteilt = 1;
    k->zeichner(k->ctx, &d);
}

void re15_door_mesh_zeichnen(const re15_md1_mesh_t *m, const re15_door_mat_t *welt,
                             const re15_door_mat_t *welt_vor, uint16_t flags, int *lfd,
                             re15_door_zeichner_t zeichner, void *zctx)
{
    /* Licht: LLM = L * W des VORIGEN Bildes (Schritt e liest obj+84, bevor Schritt i es neu
     * schreibt - 08_re_zeichnen.md 1.3), BK 68/136, LCM 1600. */
    re15_actor_lightctx_t w, ctx;
    memset(&w, 0, sizeof w);
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++) {
            w.L[i][k] = L_TUER[i * 3 + k];
            w.C[i][k] = LCM_TUER;
        }
    uint8_t bk = (flags & 0x1000) ? 136 : 68;
    w.ambient[0] = w.ambient[1] = w.ambient[2] = bk;
    w.active_lights = 3;
    int32_t wv[9];
    for (int i = 0; i < 9; i++) wv[i] = welt_vor->m[i];
    re15_light_ctx_rotate_for_bone(&w, wv, &ctx);

    int platz = 0;   /* Flags & 0xc0 == 0: Platz des vorigen Dreiecks (@0x800149e0) */
    for (int t = 0; t < m->triangle_count; t++) {
        const re15_md1_triangle_t *tr = &m->triangles[t];
        if (tr->v0 >= m->tri_vertex_count || tr->v1 >= m->tri_vertex_count || tr->v2 >= m->tri_vertex_count)
            continue;
        const re15_md1_vertex_t *vv[3] = { &m->tri_vertices[tr->v0], &m->tri_vertices[tr->v1],
                                           &m->tri_vertices[tr->v2] };
        re15_door_ecke_t e0 = re15_door_rtpt(welt, vv[0]->x, vv[0]->y, vv[0]->z);
        re15_door_ecke_t e1 = re15_door_rtpt(welt, vv[1]->x, vv[1]->y, vv[1]->z);
        re15_door_ecke_t e2 = re15_door_rtpt(welt, vv[2]->x, vv[2]->y, vv[2]->z);
        /* NCLIP: gezeichnet bei MAC0 >= 0 (@0x800148d0 / @0x800148f8 bgez) */
        int64_t mac0 = (int64_t)e0.sx * e1.sy + (int64_t)e1.sx * e2.sy + (int64_t)e2.sx * e0.sy
                     - (int64_t)e0.sx * e2.sy - (int64_t)e1.sx * e0.sy - (int64_t)e2.sx * e1.sy;
        if (mac0 < 0) continue;
        /* NCCT mit den drei Eckennormalen (@0x80014958) */
        uint8_t c[3][3];
        const uint16_t ni[3] = { tr->n0, tr->n1, tr->n2 };
        for (int k = 0; k < 3; k++) {
            if (ni[k] < m->tri_normal_count) {
                const re15_md1_vertex_t *nv = &m->tri_normals[ni[k]];
                re15_light_shade_vertex(&ctx, nv->x, nv->y, nv->z, &c[k][0], &c[k][1], &c[k][2]);
            } else {
                c[k][0] = c[k][1] = c[k][2] = 0x80;
            }
        }
        /* AVSZ3 mit ZSF3 = 341 (@0x8008d29c), verworfen bei otz < 64 (@0x800149a8/ac) */
        int32_t otz = (341 * ((int32_t)e0.sz + e1.sz + e2.sz)) >> 12;
        if ((otz >> 6) == 0) continue;
        /* Ordnungstabelle (08_re_zeichnen.md 2.2): 0x80 -> (otz>>7)+511, 0xc0 -> otz>>7,
         * 0x40 -> eigene 16er-Tabelle, vor allem gezeichnet. Port-Schluessel: groesser = frueher
         * gezeichnet; innerhalb eines Platzes das SPAETER eingehaengte zuerst (addPrim vorn). */
        switch (flags & 0xC0) {
        case 0x80: platz = (otz >> 7) + 511; break;
        case 0xC0: platz = otz >> 7; break;
        case 0x40: platz = 1024 + (otz >> 12); break;
        default: break;
        }
        const re15_md1_tri_uv_t *uv = &m->triangle_uvs[t];
        if (flags & RE15_DOOR_FLAG_TEILEN) {
            /* @0x80014a30 andi 0x20 -> DivideGT3(v0,v1,v2, uv0,uv1,uv2, rgb0 x3, s, ot=s4, divp)
             * mit ndiv 3 / pih 320 / piv 240 (Door_init @0x80013dcc/d4/dc); das eigene POLY_GT3
             * wird NICHT eingehaengt (@0x80014ab8 j 0x80014ae8). */
            const uint8_t tuv[3][2] = { { uv->u0, uv->v0 }, { uv->u1, uv->v1 }, { uv->u2, uv->v2 } };
            teil_ziel_t k = { platz, uv->page, uv->clut, lfd, zeichner, zctx };
            re15_door_divide_gt3(welt, vv, tuv, c[0], RE15_DOOR_NDIV, RE15_DOOR_PIH, RE15_DOOR_PIV,
                                 teil_ausgeben, &k);
            continue;
        }
        /* addPrim des einen POLY_GT3 (@0x80014ac0..e4) */
        re15_door_dreieck_t d;
        d.x[0] = e0.sx; d.y[0] = e0.sy; d.x[1] = e1.sx; d.y[1] = e1.sy; d.x[2] = e2.sx; d.y[2] = e2.sy;
        d.u[0] = uv->u0; d.v[0] = uv->v0; d.u[1] = uv->u1; d.v[1] = uv->v1; d.u[2] = uv->u2; d.v[2] = uv->v2;
        memcpy(d.rgb, c, sizeof d.rgb);
        d.page = uv->page; d.clut = uv->clut;
        d.z = platz * 4096 + ((*lfd)++ & 0xfff);
        d.geteilt = 0;
        zeichner(zctx, &d);
    }
}
