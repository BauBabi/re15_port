/*
 * RE1.5 Rebuilt — RE1.5-Modellteil-Schicht (Kopf und Bitbelege: include/re15_teile_r15.h).
 * Alle Adressen PSX.EXE. Der Zeichner ist der einzige Leser; die Overlay-Handler schreiben nur Felder.
 */
#include "re15_teile_r15.h"
#include "re15_actor.h"
#include "re15_skeleton.h"
#include <string.h>

static re15_teil_t s_teile[RE15_ACTOR_MAX][RE15_TEILE_MAX];
static uint8_t     s_an[RE15_ACTOR_MAX];
/* Ergebnis- und Pose-Weltmatrix je Teil des laufenden Zeichengangs (fuer die Kinder). */
static int32_t     s_pose_r[RE15_TEILE_MAX][9], s_pose_t[RE15_TEILE_MAX][3];
static int32_t     s_erg_r[RE15_TEILE_MAX][9],  s_erg_t[RE15_TEILE_MAX][3];
static uint8_t     s_versetzt[RE15_TEILE_MAX];
static int         s_gang_slot = -1, s_gang_k = -1;

/* Scherben-Tabelle @0x80073454 (30 s16, eine Reihe). */
static const int16_t s_scherben_k[30] = {
    30, 59, 87, 114, 140, 165, 189, 212, 234, 255, 275, 294, 312, 329, 345,
    360, 374, 387, 399, 410, 420, 429, 437, 444, 450, 455, 459, 462, 464, 465 };

re15_teil_t *re15_teile(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return NULL;
    return s_teile[slot];
}
int  re15_teile_an(int slot) { return (slot >= 0 && slot < RE15_ACTOR_MAX) ? s_an[slot] : 0; }
void re15_teile_markieren(int slot) { if (slot >= 0 && slot < RE15_ACTOR_MAX) s_an[slot] = 1; }

void re15_teile_reset(int slot)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return;
    for (int k = 0; k < RE15_TEILE_MAX; k++) {
        re15_teil_t *t = &s_teile[slot][k];
        memset(t, 0, sizeof *t);
        t->w = 1;                 /* `sw s3(=1),0(s1)` FUN_8001ee68 */
        t->col = 0x808080u;       /* `sw s2(=0x808080),104(s1)` */
        t->scale[0] = t->scale[1] = t->scale[2] = 0x1000;
    }
    s_an[slot] = 0;
}
void re15_teile_reset_alle(void) { for (int i = 0; i < RE15_ACTOR_MAX; i++) re15_teile_reset(i); }

void re15_teile_umordnen(int slot, const int8_t *perm, int n)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || !perm || n <= 0) return;
    if (n > RE15_TEILE_MAX) n = RE15_TEILE_MAX;
    re15_teil_t alt[RE15_TEILE_MAX];
    memcpy(alt, s_teile[slot], sizeof alt);
    for (int i = 0; i < n; i++)
        if (perm[i] >= 0 && perm[i] < RE15_TEILE_MAX) s_teile[slot][i] = alt[(int)perm[i]];
}

uint32_t re15_teil_tinte1(const re15_teil_t *t) { return (uint32_t)t->h94[0] | ((uint32_t)t->h94[1] << 16); }
uint32_t re15_teil_tinte2(const re15_teil_t *t) { return (uint32_t)t->h94[2] | ((uint32_t)t->h94[3] << 16); }

static void mul33(const int32_t a[9], const int32_t b[9], int32_t o[9])
{
    int32_t r[9];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int64_t s = 0;
            for (int k = 0; k < 3; k++) s += (int64_t)a[i*3+k] * b[k*3+j];
            r[i*3+j] = (int32_t)(s >> 12);
        }
    memcpy(o, r, sizeof r);
}
static void apply33(const int32_t m[9], const int32_t v[3], int32_t o[3])
{
    int32_t r[3];
    for (int i = 0; i < 3; i++) r[i] = (int32_t)(((int64_t)m[i*3]*v[0] + (int64_t)m[i*3+1]*v[1] + (int64_t)m[i*3+2]*v[2]) >> 12);
    memcpy(o, r, sizeof r);
}

/* FUN_8001f024 (@0x8001f024-1dc): Flug auf der festgehaltenen Matrix. */
static void flug(re15_teil_t *t, int32_t boden_y)
{
    t->t[0] += t->v[0];                                        /* @0x8001f03c-4c */
    t->v[1] = (int16_t)(t->v[1] + t->grav);                    /* vy += (s8)+0x75 @0x8001f050-64 */
    t->t[1] += t->v[1];                                        /* @0x8001f068-78 */
    t->t[2] += t->v[2];                                        /* @0x8001f07c-8c */
    if (boden_y < t->t[1]) {                                   /* `slt` @0x8001f0a8 */
        t->t[1] = boden_y;                                     /* @0x8001f0b4 */
        t->v[1] = (int16_t)((int16_t)(-t->v[1]) >> 3);         /* sll16 / sra19 @0x8001f0c0-cc */
        if (t->w & 0x10) {                                     /* @0x8001f0d0-100 */
            t->w = (uint16_t)((t->w | 0x2000) & ~0x10);
            t->scale[0] = 10000; t->scale[2] = 8000; t->scale[1] = 0;
        }
    }
    if (t->v[1] == 0 && t->t[1] == 0) t->w &= (uint16_t)~1u;  /* verschwinden @0x8001f104-134 */
    if (t->spin != 0) {                                        /* @0x8001f138-140 */
        t->spin = (int16_t)(t->spin + t->spin_inc);            /* @0x8001f194-1b4 */
        int32_t rz[9] = { re15_cos_q12(t->spin), -re15_sin_q12(t->spin), 0,
                          re15_sin_q12(t->spin),  re15_cos_q12(t->spin), 0,
                          0, 0, 0x1000 };                      /* RotMatrixZ @0x8001f1b0 (0x80065b70) */
        mul33(t->rot, rz, t->rot);                             /* MulMatrix0(M, T, M) @0x8001f1c0 */
    }
}

int re15_teil_stufe(int slot, int k, int parent, int32_t rot[9], int32_t trans[3], int32_t boden_y, int sichtbar)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || k < 0 || k >= RE15_TEILE_MAX) return 1;
    if (slot != s_gang_slot || k <= s_gang_k) { s_gang_slot = slot; memset(s_versetzt, 0, sizeof s_versetzt); }
    s_gang_k = k;
    re15_teil_t *t = &s_teile[slot][k];
    memcpy(s_pose_r[k], rot, sizeof s_pose_r[k]); memcpy(s_pose_t[k], trans, sizeof s_pose_t[k]);
    int versetzt = 0;
    if (t->w & 0x40) {                                         /* @0x8001ea10: keine Eltern-Verkettung */
        if (!t->fest) { memcpy(t->rot, rot, sizeof t->rot); memcpy(t->t, trans, sizeof t->t); t->fest = 1; }
        versetzt = 1;
    } else {
        t->fest = 0;
        if (parent >= 0 && parent < RE15_TEILE_MAX && s_versetzt[parent]) {
            /* +0x40 = Eltern+0x40 (Ergebnis) o lokal (CompMatrix 0x80022da0 @0x8001ea24); lokal = Pose_p^T o Pose_k. */
            int32_t pt[9]; for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) pt[i*3+j] = s_pose_r[parent][j*3+i];
            int32_t lr[9]; mul33(pt, rot, lr);
            int32_t d[3] = { trans[0] - s_pose_t[parent][0], trans[1] - s_pose_t[parent][1], trans[2] - s_pose_t[parent][2] };
            int32_t lt[3]; apply33(pt, d, lt);
            mul33(s_erg_r[parent], lr, rot);
            apply33(s_erg_r[parent], lt, trans);
            for (int i = 0; i < 3; i++) trans[i] += s_erg_t[parent][i];
            versetzt = 1;
        }
    }
    if (t->w & 0x40) {
        if (t->w & 0x20) flug(t, boden_y);                     /* @0x8001eb14 -> FUN_8001f024 @0x8001eb24 */
        memcpy(rot, t->rot, sizeof t->rot); memcpy(trans, t->t, sizeof t->t);
    }
    memcpy(s_erg_r[k], rot, sizeof s_erg_r[k]); memcpy(s_erg_t[k], trans, sizeof s_erg_t[k]);
    s_versetzt[k] = (uint8_t)versetzt;
    if (!(t->w & 1)) return 0;                                 /* @0x8001ecc4 */
    if (t->w & 0x08) {
        /* Bild-Zaehler +0x9c: ++ am Ende der Quad-Zeichnung, nur sichtbar (@0x80023a00-a0c); FUN_8001f1e0: vy += inc,
         * t.y += vy, ab Bild 0x1d Teil-Wort = 0 (`sltiu v1,v1,0x1d` @0x8001f208, `sw zero` @0x8001f214). Der Zeichner
         * liest das Bild VOR dem Hochzaehlen (re15_teil_scherbe). */
        return 2;
    }
    return 1;
}

/* Nach dem Zeichnen eines 0x08-Teils (FUN_80023708-Ende + FUN_8001f1e0). */
void re15_teil_scherben_nach(int slot, int k, int sichtbar);
void re15_teil_scherben_nach(int slot, int k, int sichtbar)
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || k < 0 || k >= RE15_TEILE_MAX) return;
    re15_teil_t *t = &s_teile[slot][k];
    if (!(t->w & 0x08)) return;
    if (sichtbar) t->h94[4]++;                                 /* `sh v0,156(s4)` @0x80023a0c */
    t->h94[1] = (uint16_t)((int16_t)t->h94[1] + (int16_t)t->h94[3]);   /* +0x96 += +0x9a */
    t->t[1] += (int16_t)t->h94[1];                             /* t.y (+0x58) += +0x96 */
    if (t->h94[4] >= 0x1d) t->w = 0;                           /* @0x8001f208-214 */
}

void re15_teil_scherbe(const re15_teil_t *t, int32_t v1x, int32_t v1y, int32_t v1z, int32_t out[3])
{
    int f = t->h94[4]; if (f > 29) f = 29;
    int32_t k = s_scherben_k[f];                               /* @0x800234ec-f8 */
    out[0] = (int32_t)(int16_t)(((int16_t)v1x >> 5) * k);      /* @0x80023528-78, als s16 (lhu/lwc2) */
    out[1] = (int32_t)(int16_t)((((int16_t)v1y + 300) >> 6) * k);
    out[2] = (int32_t)(int16_t)(((int16_t)v1z >> 5) * k);
}
