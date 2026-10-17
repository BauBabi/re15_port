/*
 * zopf_r38.c — Elzas Zopf (Runde 38 Spur D). Byte-genauer Nachbau der RE1.5-Zopf-Routine.
 * Belege (Adressen + Instruktionen): analysis/befunde_runde38/D_zopf.md §2.
 *
 *   Init     Kommando-0-Handler @0x800318f8 (Tabelle @0x80073f90[0]), Elza-Zweig @0x80031a78-80031bf4
 *   Treiber  FUN_80024c30 (@0x80024c30-80024e3c), Aufruf @0x80031d98 hinter `andi v0,v0,0x4` @0x80031d8c
 *   Glied    FUN_80024e40 (@0x80024e40-8002535c), Winkelhelfer FUN_80025360 / FUN_800253fc
 *   libgte   RotMatrix 0x80068098, RotMatrixYXZ 0x800666d0, RotMatrixY 0x800659d0, RotMatrixZ 0x80065b70,
 *            ApplyMatrix 0x800661c0 (MVMVA 0x4a486012), CompMatrix-Kette FUN_80022da0, rsin 0x800683e8,
 *            catan 0x800658fc (re15_rsin/re15_catan, re15_math.c)
 * RE2 (Claire) fuehrt dieselbe Routine (FUN_8001844c/FUN_80018728); der Port folgt RE1.5 (Elzas Netz).
 */
#include "re15_zopf_r38.h"
#include "re15_math.h"       /* re15_rsin (0x800683e8), re15_catan (0x800658fc) */
#include "re15_actor.h"
#include "re15_skeleton.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Die Sinus/Kosinus-Tabelle DAT_800794c4 (re15_trig_lut.c, aus PSX.EXE generiert): Wort = (cos16<<16)|sin16. */
extern const uint32_t re15_trig_lut[4096];

static re15_zopf_glied_t s_gl[RE15_ZOPF_GLIEDER];
static int     s_takt_offen = 0;   /* Spieler-Dispatcher-Rumpf lief, Zopf-Schritt steht aus */
static int     s_modell = -1;      /* zuletzt gesehenes Spielermodell (PLD-Index), -1 = keins */
static int     s_init_ok = 0;      /* Kommando-0-Init lief fuer ein Elza-Modell 4/5/6 */
static int     s_geschritten = 0;  /* mindestens ein Treiber-Schritt seit dem PLD-Laden (Weltmatrizen gueltig) */
static int32_t s_lokal[RE15_EMD_MAX_BONES][9];   /* lokale Rotationen der Pose (Teil 8 = +0x578) */
static int16_t s_w8r[9];           /* Teil-8-Weltmatrix des letzten Schritts (+0x40, @0x80024c9c) — nur Riegel */
static int32_t s_w8t[3];

/* ---- libgte-Nachbauten ---------------------------------------------------------------------------- */

/* Tabellenzugriff wie alle vier Rotationsfunktionen: Winkel >= 0 -> Tabelle[a & 0xfff]; Winkel < 0 ->
 * EIGENER Zweig `subu t7,zero,t7` + `andi t7,t7,0xfff`, Sinus negiert (z.B. RotMatrixY @0x800659e0-a0c,
 * RotMatrixZ @0x80065b80-bb0, RotMatrixYXZ @0x800666e0-710, RotMatrix @0x800680a8-d8). Die Tabelle ist
 * nicht antisymmetrisch (Eintrag 4095 = sin 0), der Zweig ist also beobachtbar. */
static void sc(int32_t a, int32_t *s, int32_t *c)
{
    uint32_t w;
    if (a >= 0) { w = re15_trig_lut[(uint32_t)a & 0xfffu]; *s = (int32_t)(int16_t)(w & 0xffffu); }
    else        { w = re15_trig_lut[(uint32_t)(-a) & 0xfffu]; *s = -(int32_t)(int16_t)(w & 0xffffu); }
    *c = ((int32_t)w) >> 16;                                     /* `sra t0,t9,16` */
}

/* multu + mflo: das untere Wort des Produkts (bei den kleinen Q12-Faktoren = vorzeichenrichtig). */
static int32_t mf(int32_t a, int32_t b) { return (int32_t)((uint32_t)a * (uint32_t)b); }

void re15_zopf_r38_rotmatrix(const int16_t r[3], int16_t m[9])
{
    int32_t sx, cx, sy, cy, sz, cz;
    sc(r[0], &sx, &cx); sc(r[1], &sy, &cy); sc(r[2], &sz, &cz);
    m[2] = (int16_t)sy;                                                  /* sh t6,4(a1)  @0x8006816c */
    m[5] = (int16_t)((-mf(cy, sx)) >> 12);                              /* @0x80068164-180 */
    m[8] = (int16_t)(mf(cy, cx) >> 12);                                 /* @0x8006817c / @0x800681cc-d4 */
    m[0] = (int16_t)(mf(cz, cy) >> 12);                                 /* @0x800681f8-20c */
    m[1] = (int16_t)((-mf(sz, cy)) >> 12);                              /* @0x80068210-22c */
    {
        int32_t t8 = mf(cz, -sy) >> 12;                                 /* t4 = -sy, @0x80068228-238 */
        m[3] = (int16_t)((mf(sz, cx) >> 12) - (mf(t8, sx) >> 12));      /* @0x80068240-274 */
        m[6] = (int16_t)((mf(sz, sx) >> 12) + (mf(t8, cx) >> 12));      /* @0x80068270-2a4 */
    }
    {
        int32_t t8 = mf(sz, -sy) >> 12;                                 /* @0x800682a0-2b0 */
        m[4] = (int16_t)((mf(cz, cx) >> 12) + (mf(t8, sx) >> 12));      /* @0x800682b8-2ec */
        m[7] = (int16_t)((mf(cz, sx) >> 12) - (mf(t8, cx) >> 12));      /* @0x800682e8-318 */
    }
}

void re15_zopf_r38_rotmatrix_yxz(const int16_t r[3], int16_t m[9])
{
    int32_t sx, cx, sy, cy, sz, cz;
    sc(r[0], &sx, &cx); sc(r[1], &sy, &cy); sc(r[2], &sz, &cz);
    m[5] = (int16_t)(-sx);                                              /* sh t6,10(a1) @0x800667a4 */
    m[2] = (int16_t)(mf(sy, cx) >> 12);                                 /* @0x8006679c-7b8 */
    m[8] = (int16_t)(mf(cy, cx) >> 12);                                 /* @0x800667b4-7cc / @0x80066804-80c */
    m[3] = (int16_t)(mf(sz, cx) >> 12);                                 /* @0x80066830-844 */
    m[4] = (int16_t)(mf(cz, cx) >> 12);                                 /* @0x80066848-864 */
    {
        int32_t t8 = mf(sy, sx) >> 12;                                  /* @0x80066860-870 */
        m[0] = (int16_t)((mf(cy, cz) >> 12) + (mf(t8, sz) >> 12));      /* @0x80066878-8ac */
        m[1] = (int16_t)(-(mf(cy, sz) >> 12) + (mf(t8, cz) >> 12));     /* @0x800668a8-8dc */
    }
    {
        int32_t t8 = mf(cy, sx) >> 12;                                  /* @0x800668d8-8e8 */
        m[7] = (int16_t)((mf(sy, sz) >> 12) + (mf(t8, cz) >> 12));      /* @0x800668f0-924 */
        m[6] = (int16_t)(-(mf(sy, cz) >> 12) + (mf(t8, sz) >> 12));     /* @0x80066920-950 */
    }
}

/* RotMatrixY(a, m): m = Ry(a) * m, Zeilen 0 und 2 (@0x800659d0-80065b60). */
void re15_zopf_r38_rotmatrix_y(int32_t a, int16_t m[9])
{
    int32_t s, c;
    sc(a, &s, &c);
    int32_t t1 = -s, t0 = c;                                            /* @0x80065a2c / @0x80065a04 */
    int32_t m00 = m[0], m01 = m[1], m02 = m[2], m20 = m[6], m21 = m[7], m22 = m[8];
    m[0] = (int16_t)((mf(t0, m00) - mf(t1, m20)) >> 12);
    m[1] = (int16_t)((mf(t0, m01) - mf(t1, m21)) >> 12);
    m[2] = (int16_t)((mf(t0, m02) - mf(t1, m22)) >> 12);
    m[6] = (int16_t)((mf(t1, m00) + mf(t0, m20)) >> 12);
    m[7] = (int16_t)((mf(t1, m01) + mf(t0, m21)) >> 12);
    m[8] = (int16_t)((mf(t1, m02) + mf(t0, m22)) >> 12);
}

/* RotMatrixZ(a, m): m = Rz(a) * m, Zeilen 0 und 1 (@0x80065b70-80065d00). */
void re15_zopf_r38_rotmatrix_z(int32_t a, int16_t m[9])
{
    int32_t s, c;
    sc(a, &s, &c);
    int32_t t1 = s, t0 = c;                                             /* @0x80065bcc / @0x80065ba8 */
    int32_t m00 = m[0], m01 = m[1], m02 = m[2], m10 = m[3], m11 = m[4], m12 = m[5];
    m[0] = (int16_t)((mf(t0, m00) - mf(t1, m10)) >> 12);
    m[1] = (int16_t)((mf(t0, m01) - mf(t1, m11)) >> 12);
    m[2] = (int16_t)((mf(t0, m02) - mf(t1, m12)) >> 12);
    m[3] = (int16_t)((mf(t1, m00) + mf(t0, m10)) >> 12);
    m[4] = (int16_t)((mf(t1, m01) + mf(t0, m11)) >> 12);
    m[5] = (int16_t)((mf(t1, m02) + mf(t0, m12)) >> 12);
}

/* ApplyMatrix @0x800661c0: MVMVA 0x4a486012 (RT*V0, sf=1, ohne TR), swc2 MAC1..3. */
static void apply(const int16_t m[9], const int16_t v[3], int32_t out[3])
{
    for (int i = 0; i < 3; i++) {
        int64_t s = (int64_t)m[i*3+0] * v[0] + (int64_t)m[i*3+1] * v[1] + (int64_t)m[i*3+2] * v[2];
        out[i] = (int32_t)(s >> 12);
    }
}

static int16_t sat16(int64_t v) { return (int16_t)(v > 0x7fff ? 0x7fff : (v < -0x8000 ? -0x8000 : v)); }

/* FUN_80022da0: OUT.R = A.R * B.R (gte_rtir, IR lm=0 -> s16-Saettigung), OUT.t = (A.t<<12 + A.R*(s16)B.t)>>12
 * (gte_rt + stlvnl; B.t wird als Halbworte geladen). */
static void comp(const int16_t ar[9], const int32_t at[3], const int16_t br[9], const int32_t bt[3],
                 int16_t orr[9], int32_t ot[3])
{
    int16_t r[9];
    int32_t t[3];
    for (int col = 0; col < 3; col++)
        for (int row = 0; row < 3; row++)
            r[row*3+col] = sat16(((int64_t)ar[row*3+0] * br[0*3+col] + (int64_t)ar[row*3+1] * br[1*3+col] +
                                  (int64_t)ar[row*3+2] * br[2*3+col]) >> 12);
    for (int row = 0; row < 3; row++)
        t[row] = (int32_t)((((int64_t)at[row] << 12) + (int64_t)ar[row*3+0] * (int16_t)bt[0] +
                            (int64_t)ar[row*3+1] * (int16_t)bt[1] + (int64_t)ar[row*3+2] * (int16_t)bt[2]) >> 12);
    memcpy(orr, r, sizeof r);
    memcpy(ot, t, sizeof t);
}

/* ---- Winkelhelfer -------------------------------------------------------------------------------- */

/* FUN_80025360 (@0x80025360-800253f8): Grundwert ((x >= 0) << 11) + 0xc00; y == 0 -> Grundwert;
 * sonst catan((x<<12) / -y), bei y > 0 (-y < 0, `bgez s0` nicht genommen) + 0x800. */
int16_t re15_zopf_r38_winkel_z(int32_t x, int32_t y)
{
    int32_t a0 = (int16_t)x, a1 = (int16_t)y;
    int32_t v0 = ((a0 >= 0) ? 0x800 : 0) + 0xc00;
    int32_t s0 = -a1;
    if (s0 == 0) return (int16_t)v0;
    int32_t v1 = re15_catan((a0 << 12) / s0);
    if (s0 >= 0) return (int16_t)v1;
    return (int16_t)(v1 + 0x800);
}

/* FUN_800253fc (@0x800253fc-8002549c): Grundwert ((x >= 0) << 11) | 0x400; y == 0 -> Grundwert;
 * sonst catan((x<<12) / -y), bei y > 0 negiert. */
int16_t re15_zopf_r38_winkel_x(int32_t x, int32_t y)
{
    int32_t a0 = (int16_t)x, a1 = (int16_t)y;
    int32_t v0 = ((a0 >= 0) ? 0x800 : 0) | 0x400;
    int32_t s0 = -a1;
    if (s0 == 0) return (int16_t)v0;
    int32_t v1 = re15_catan((a0 << 12) / s0);
    if (s0 >= 0) return (int16_t)v1;
    return (int16_t)(-(int16_t)v1);
}

/* 12-Bit-Vorzeichenerweiterung der Winkelgeschwindigkeit (@0x8002524c-260 / @0x800252e0-2f4). */
static int16_t sext12(int32_t v)
{
    int32_t r = v & 0xfff;
    if (v & 0x800) r -= 0x1000;
    return (int16_t)r;
}

/* (Lage - P) * 4 als SVECTOR gepackt wie @0x800250f0-134: v1 = (u16 dy) << 18, v0 = ((u16 dx) & 0xffff) << 2,
 * Wort = v1 | v0 (Bits 14/15 von dx landen in den unteren 2 Bits von vy), vz = (u16 dz) << 2. */
static void pack4(const int32_t lage[3], const int32_t p[3], int16_t r[3])
{
    uint32_t dy = (uint32_t)(uint16_t)lage[1] - (uint32_t)(uint16_t)p[1];
    uint32_t dx = (uint32_t)(uint16_t)lage[0] - (uint32_t)(uint16_t)p[0];
    uint32_t dz = (uint32_t)(uint16_t)lage[2] - (uint32_t)(uint16_t)p[2];
    uint32_t w  = (dy << 18) | ((dx & 0xffffu) << 2);
    r[0] = (int16_t)(w & 0xffffu);
    r[1] = (int16_t)(w >> 16);
    r[2] = (int16_t)((dz << 2) & 0xffffu);
}

/* ---- FUN_80024e40: ein Glied ---------------------------------------------------------------------- */
static void glied(re15_zopf_glied_t *g, const int16_t pm[9], const int32_t pt[3],
                  int32_t p3, int32_t p4, int32_t p5, int32_t p6, int32_t p7, int32_t p8,
                  int32_t p9, int32_t p10, int32_t p11, int32_t p12, int32_t p13)
{
    int16_t M[9], r[3];
    int32_t A[3], B[3], out[3];

    /* 1. Vorhersage-Matrix: r = (rot.vx + v96, 0, rot.vz + v94) (@0x80024e74-ee8), RotMatrixYXZ @0x80024ee4,
     *    RotMatrixY(rot.vy) @0x80024ef0. */
    r[0] = (int16_t)(uint16_t)((uint16_t)g->rot[0] + (uint16_t)g->v96);
    r[1] = 0;
    r[2] = (int16_t)(uint16_t)((uint16_t)g->rot[2] + (uint16_t)g->v94);
    re15_zopf_r38_rotmatrix_yxz(r, M);
    re15_zopf_r38_rotmatrix_y((int32_t)g->rot[1], M);

    /* 2. MVMVA 0x4a486012 mit V0 = (0,p3,0) @0x80024f58 und (0,p8,0) @0x80024f88:
     *    A = M*(0,p3,0)>>12 + alte Lage + (0,p4,0), B = M*(0,p8,0)>>12 + alte Lage + (0,p9,0). */
    {
        int16_t v3[3] = { 0, (int16_t)p3, 0 };                         /* `sll s0,s0,16` + `sw s0,48(sp)` */
        int16_t v8[3] = { 0, (int16_t)(uint16_t)p8, 0 };               /* `lhu s7,268(sp)` + `sh s7,50(sp)` */
        apply(M, v3, A);
        apply(M, v8, B);
    }
    A[0] += g->t[0]; A[1] = (int32_t)(int16_t)p4 + g->t[1] + A[1]; A[2] += g->t[2];   /* @0x80024f8c-fc8 */
    B[0] += g->t[0]; B[1] = (int32_t)(int16_t)p9 + g->t[1] + B[1]; B[2] += g->t[2];   /* @0x80024fe8-028 */

    /* 3. Neue Lage = Elternmatrix * Versatz (gepackt (t.x<<4)|(t.y<<20), t.z<<4 @0x8002502c-050),
     *    ApplyMatrix @0x8002504c, Lage = (out>>4) + Eltern.t (@0x80025054-08c). */
    {
        uint32_t w = ((uint32_t)(uint16_t)g->off[0] << 4) | ((uint32_t)(uint16_t)g->off[1] << 20);
        r[0] = (int16_t)(w & 0xffffu);
        r[1] = (int16_t)(w >> 16);
        r[2] = (int16_t)(((uint32_t)(uint16_t)g->off[2] << 4) & 0xffffu);
        apply(pm, r, out);
        for (int i = 0; i < 3; i++) g->t[i] = (out[i] >> 4) + pt[i];
    }

    /* 4. Z-Winkel: M = RotMatrixY(-rot.vy, Einheit DAT_80072d4c) @0x800250e0, r = (Lage-A)*4, ApplyMatrix
     *    @0x80025130, zang = FUN_80025360(out.x, out.y) @0x80025140. */
    int16_t I[9] = { 0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000 };
    re15_zopf_r38_rotmatrix_y(-(int32_t)g->rot[1], I);
    pack4(g->t, A, r);
    apply(I, r, out);
    int16_t zang = re15_zopf_r38_winkel_z((int16_t)out[0], (int16_t)out[1]);

    /* 5. X-Winkel: r = (Lage-B)*4 (@0x80025148-188), M = RotMatrixZ(-(rot.vz+v94), M) @0x8002519c,
     *    ApplyMatrix @0x800251ac, xang = FUN_800253fc(-out.z, out.y) @0x800251c4. */
    pack4(g->t, B, r);
    re15_zopf_r38_rotmatrix_z(-((int32_t)g->rot[2] + (int32_t)g->v94), I);
    apply(I, r, out);
    int16_t xang = re15_zopf_r38_winkel_x((int16_t)(-out[2]), (int16_t)out[1]);

    /* 6. Feder-Daempfer (@0x800251ec-2f4). */
    {
        int32_t rs = re15_rsin((int32_t)zang - (int32_t)(int16_t)p7);
        int32_t pr = mf((int16_t)p5, rs);                               /* `mult v1,v0` / `mflo` */
        int32_t q  = (int32_t)g->v94 / (int32_t)(int16_t)p6;            /* `div v0,s3` */
        g->v94 = sext12((int32_t)g->v94 - q - (pr >> 12));
        rs = re15_rsin((int32_t)xang - (int32_t)(int16_t)p12);
        pr = mf((int16_t)p10, rs);
        q  = (int32_t)g->v96 / (int32_t)(int16_t)p11;
        g->v96 = sext12((int32_t)g->v96 - q - (pr >> 12));
    }

    /* 7. Weltmatrix: rot = (xang, 0, zang) (`sh s1,100(fp)` + `sw v0,96(fp)` @0x80025304/0c),
     *    RotMatrixYXZ @0x80025308, rot.vy = p13 (`sh t0,98(fp)` @0x80025328), RotMatrixY(p13) @0x80025324. */
    g->rot[0] = xang;
    g->rot[1] = 0;
    g->rot[2] = zang;
    re15_zopf_r38_rotmatrix_yxz(g->rot, g->m);
    g->rot[1] = (int16_t)p13;
    re15_zopf_r38_rotmatrix_y((int32_t)(int16_t)p13, g->m);
}

/* Mess-Log RE15_ZOPF_LOG=<datei> (env-gegatet, kein Verhalten). */
static FILE *zlog(void)
{
    static FILE *lf = NULL; static int lt = 0;
    if (!lt) { lt = 1; const char *p = getenv("RE15_ZOPF_LOG"); if (p && *p) lf = fopen(p, "w"); }
    return lf;
}

/* ---- FUN_80024c30: Treiber ----------------------------------------------------------------------- */
void re15_zopf_r38_schritt(const re15_zopf_eingang_t *in)
{
    int16_t er[9], l0r[9], l8r[9], tr[9], w8r[9];
    int32_t tt[3], w8t[3];
    re15_zopf_r38_rotmatrix(in->ent_rot, er);                          /* RotMatrix(ent+0x68, ent+0x20) @0x80024c68 */
    for (int i = 0; i < 9; i++) { l0r[i] = (int16_t)in->l0_rot[i]; l8r[i] = (int16_t)in->l8_rot[i]; }
    comp(er, in->ent_pos, l0r, in->l0_t, tr, tt);                      /* FUN_80022da0 @0x80024c88 */
    comp(tr, tt, l8r, in->l8_t, w8r, w8t);                             /* FUN_80022da0 @0x80024c9c -> Teil 8 +0x40 */
    memcpy(s_w8r, w8r, sizeof s_w8r);
    memcpy(s_w8t, w8t, sizeof s_w8t);

    /* Koerperwinkel (@0x80024ca4-d34): lhu-Summe, +0x800 bei Teil0.vz in [0x5dd, 0x5dd+0x3e7), + Nacken-Gier
     * ohne Bit 0 von 0x800acc0c, dann (short). */
    uint32_t t0 = (uint32_t)(uint16_t)in->ent_rot[1] + (uint32_t)(uint16_t)in->teil0_vy +
                  (uint32_t)(uint16_t)in->teil8_vy;
    uint32_t vz = (uint32_t)(uint16_t)in->teil0_vz & 0xfffu;           /* `andi v0,v0,0xfff` @0x80024cc0 */
    if ((uint32_t)(vz - 0x5ddu) < 0x3e7u) t0 += 0x800u;                /* addiu -1501 / sltiu 0x3e7 @0x80024cc8-cc */
    if ((in->nacken_flags & 1u) == 0) t0 += (uint32_t)(uint16_t)in->nacken_gier;   /* @0x80024ce0-d00 */
    int32_t s0 = (int16_t)t0;                                          /* sll/sra @0x80024d30-34 */

    glied(&s_gl[0], w8r, w8t,             0x190, 0x64, 0x28, 2, 0,     0x190, 0x64, 0x18, 4, 0, s0);  /* @0x80024d50 */
    glied(&s_gl[1], s_gl[0].m, s_gl[0].t, 0x15e, 0,    0x30, 4, 0x12c, 0x96,  0,    0x18, 6, 0, s0);  /* @0x80024d98 */
    glied(&s_gl[2], s_gl[1].m, s_gl[1].t, 0x12c, 0,    0x30, 4, 0,     0x96,  0,    0x18, 6, 0, s0);  /* @0x80024dd0 */
    glied(&s_gl[3], s_gl[2].m, s_gl[2].t, 0xfa,  0,    0x30, 4, 0,     0x64,  0,    0x18, 6, 0, s0);  /* @0x80024e08 */
    s_geschritten = 1;

    {   /* Mess-Log RE15_ZOPF_LOG=<datei>: Koerperwinkel + je Glied rot/v94/v96/Lage. */
        static unsigned n = 0;
        FILE *lf = zlog();
        if (lf) {
            fprintf(lf, "Z%u s0=%d", n++, (int)s0);
            for (int k = 0; k < RE15_ZOPF_GLIEDER; k++)
                fprintf(lf, " g%d=(%d,%d,%d v94=%d v96=%d t=%d,%d,%d)", k, (int)s_gl[k].rot[0], (int)s_gl[k].rot[1],
                        (int)s_gl[k].rot[2], (int)s_gl[k].v94, (int)s_gl[k].v96,
                        (int)s_gl[k].t[0], (int)s_gl[k].t[1], (int)s_gl[k].t[2]);
            fprintf(lf, " kopf=%d,%d,%d w8=[%d %d %d|%d %d %d|%d %d %d] ein=(ent %d,%d,%d t0vy %d t0vz %d t8vy %d ng %d nf %02x)\n",
                    (int)w8t[0], (int)w8t[1], (int)w8t[2], w8r[0], w8r[1], w8r[2], w8r[3], w8r[4], w8r[5], w8r[6], w8r[7],
                    w8r[8], (int)in->ent_rot[0], (int)in->ent_rot[1], (int)in->ent_rot[2], (int)in->teil0_vy,
                    (int)in->teil0_vz, (int)in->teil8_vy, (int)in->nacken_gier, (unsigned)in->nacken_flags);
            fflush(lf);
        }
    }
}

/* ---- Lebenszyklus --------------------------------------------------------------------------------- */

void re15_zopf_r38_pld_reset(void)
{
    /* FUN_8001e5b0-Vorlage: lokale + Weltmatrix = Einheit (local_b8[5..12] / local_7c.., t = 0), rot = 0. */
    memset(s_gl, 0, sizeof s_gl);
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) { s_gl[k].m[0] = s_gl[k].m[4] = s_gl[k].m[8] = 0x1000; }
    s_init_ok = 0;
    s_takt_offen = 0;
    s_geschritten = 0;
}

void re15_zopf_r38_kommando0(int modell)
{
    if (modell < 0) modell = s_modell;
    /* `addiu v0,v0,-4; sltiu v0,v0,2` @0x80031a88-8c (4,5) oder `bne v1,6` @0x80031aa0 (6). */
    if (!((unsigned)(modell - 4) < 2u || modell == 6)) { s_init_ok = 0; return; }
    static const int32_t off[RE15_ZOPF_GLIEDER][2] = {
        { -0x73, -0x15a },   /* Teil 17: +0xb98 @0x80031b44-48, +0xb9c @0x80031b4c-50 */
        { -0x2d, -0x20  },   /* Teil 18: +0xc44 @0x80031b54-58, +0xc48 @0x80031b5c-60 */
        { -0x65,  0x61  },   /* Teil 19: +0xcf0 @0x80031b64-68, +0xcf4 @0x80031b6c-70 */
        { -0x1,   0x6a  },   /* Teil 20: +0xd9c @0x80031b74-78, +0xda0 @0x80031b7c-80 */
    };
    for (int k = 0; k < RE15_ZOPF_GLIEDER; k++) {
        s_gl[k].off[0] = off[k][0];
        s_gl[k].off[1] = off[k][1];
        s_gl[k].off[2] = 0;           /* +0xba0/+0xc4c/+0xcf8/+0xda4 @0x80031b88/98/a8/b8 */
        s_gl[k].v94 = 0;              /* +0xc00/+0xcac/+0xd58/+0xe04 */
        s_gl[k].v96 = 0;              /* +0xc02/+0xcae/+0xd5a/+0xe06 */
        s_gl[k].rot[1] = 0;           /* +0xbce/+0xc7a/+0xd26/+0xdd2 */
    }
    s_init_ok = 1;                    /* Flags |= 0x40 @0x80031bc4-bf0 */
    { FILE *lf = zlog(); if (lf) { fprintf(lf, "R kommando0 modell=%d\n", modell); fflush(lf); } }
}

void re15_zopf_r38_takt(int lief)
{
    if (lief) s_takt_offen = 1;
}

int re15_zopf_r38_aktiv(int modell, int mesh_count)
{
    if (modell != s_modell) {          /* neues Spielermodell = PLD neu geladen (FUN_800314b0 -> Kommando 0) */
        s_modell = modell;
        re15_zopf_r38_pld_reset();
        re15_zopf_r38_kommando0(modell);
    }
    if (!(modell & 4)) return 0;                         /* `andi v0,v0,0x4` @0x80031d8c */
    if (!s_init_ok) return 0;                            /* Flag 0x40 nur ueber den Init @0x80031bc4-bf0 */
    return mesh_count >= RE15_ZOPF_MIN_TEILE;            /* Teile 17..20 vorhanden (FUN_8001e56c) */
}

int re15_zopf_r38_zeichnen(int modell, int mesh_count)
{
    /* Original: Spieler-Tick (Zopf) laeuft im selben Bild VOR dem Zeichner (Haupt-Loop @0x8001ce0c ->
     * @0x8001d09c), die Weltmatrix ist beim Zeichnen also immer schon gerechnet. */
    /* MESS-SONDE RE15_ZOPF_SONDE_AUS=1 (nur Riegel/Messung, kein Spielverhalten): Zopf rechnen, aber nicht
     * zeichnen — der Riegel vergleicht damit dasselbe Bild mit/ohne Zopfglieder (Pixel am Hinterkopf). */
    static int sonde = -1;
    if (sonde < 0) { const char *e = getenv("RE15_ZOPF_SONDE_AUS"); sonde = (e && *e && *e != '0') ? 1 : 0; }
    if (sonde) return 0;
    return re15_zopf_r38_aktiv(modell, mesh_count) && s_geschritten;
}

int re15_zopf_r38_bild(const re15_zopf_eingang_t *in)
{
    if (!s_takt_offen) return 0;
    s_takt_offen = 0;
    re15_zopf_r38_schritt(in);
    return 1;
}

int re15_zopf_r38_welt(int k, int32_t rot[9], int32_t t[3])
{
    if (k < 0 || k >= RE15_ZOPF_GLIEDER) return 0;
    for (int i = 0; i < 9; i++) rot[i] = s_gl[k].m[i];
    for (int i = 0; i < 3; i++) t[i] = s_gl[k].t[i];
    return 1;
}

const re15_zopf_glied_t *re15_zopf_r38_glied(int k)
{
    return (k >= 0 && k < RE15_ZOPF_GLIEDER) ? &s_gl[k] : NULL;
}

void re15_zopf_r38_glied_setzen(int k, const re15_zopf_glied_t *g)
{
    if (k >= 0 && k < RE15_ZOPF_GLIEDER && g) s_gl[k] = *g;
}

void re15_zopf_r38_kopf_welt(int16_t m[9], int32_t t[3])
{
    memcpy(m, s_w8r, sizeof s_w8r);
    memcpy(t, s_w8t, sizeof s_w8t);
}

int32_t (*re15_zopf_r38_lokal(void))[9] { return s_lokal; }

/* Eingang aus der Port-Pose: Teil 0 = Wurzel (poses[0] IST die lokale Matrix, kein Elter), Teil 8 = die
 * eingefangene lokale Rotation (skeleton_common.c, g_skel_lokal_rot) + EMR-Versatz Knochen 8; Winkel aus
 * prev_angles (= part+0x60, vor dem Nacken-Zuschlag, wie FUN_80037358 @0x80037358 ihn nach dem RotMatrix
 * wieder abzieht); Nacken aus neck_yaw (+0x94) / neck_flags (+0x1b8). */
int re15_zopf_r38_bild_aus_pose(const void *akteur, const void *skelett, const void *posen)
{
    const re15_actor_t *pl = (const re15_actor_t *)akteur;
    const re15_emd_skeleton_t *sk = (const re15_emd_skeleton_t *)skelett;
    const re15_skel_pose_t *po = (const re15_skel_pose_t *)posen;
    if (!pl || !sk || !po || sk->bone_count <= RE15_ZOPF_KOPF_TEIL) return 0;
    re15_zopf_eingang_t in;
    memset(&in, 0, sizeof in);
    in.ent_rot[0] = pl->rot_x; in.ent_rot[1] = pl->rot_y; in.ent_rot[2] = pl->rot_z;
    in.ent_pos[0] = pl->x; in.ent_pos[1] = pl->y; in.ent_pos[2] = pl->z;
    memcpy(in.l0_rot, po[0].rot, sizeof in.l0_rot);
    memcpy(in.l0_t, po[0].trans, sizeof in.l0_t);
    memcpy(in.l8_rot, s_lokal[RE15_ZOPF_KOPF_TEIL], sizeof in.l8_rot);
    for (int i = 0; i < 3; i++) in.l8_t[i] = sk->bone_relative_pos[RE15_ZOPF_KOPF_TEIL][i];
    in.teil0_vy = pl->prev_angles[0][1];
    in.teil0_vz = pl->prev_angles[0][2];
    in.teil8_vy = pl->prev_angles[RE15_ZOPF_KOPF_TEIL][1];
    in.nacken_gier  = pl->neck_yaw;
    in.nacken_flags = pl->neck_flags;
    return re15_zopf_r38_bild(&in);
}
