/* schwanz_r41.c — Feder-Kette fuer den Schwanz der Sichel-Form (Runde 41 Spur SF N1.9, re15_schwanz_r41.h).
 *
 * PORT-WAHL, keine Original-Konstante: die Form (EM30_HANGAR.EMD) existiert auf der Disc nicht. Alle Zahlen hier sind
 * Gestaltung, abgestimmt am 80%-Video (Video 4 Hangar: Schwanz schwingt h28 -> h38 in ~10 Bildern von Birkins rechter
 * Seite durch die Knie auf die linke; Dossier SF_sichel.md N1.9). */
#include "re15_schwanz_r41.h"
#include "re15_engine.h"     /* g_engine.frame_count: ein Kettenschritt je Spieltakt */
#include <math.h>
#include <string.h>

#define SW_K0   16            /* erster Schwanzknochen (Kette 16-22 an Knochen 8, tools/birkin_formen.py schwanz_kette) */
#define SW_N    7             /* Glieder */
#define SW_J    (SW_N + 1)    /* Gelenke: Wurzel (Knochen 16) .. Spitze */

/* Feder je Gelenk 1..7 zum Keyframe-Ziel (Anteil des Abstands je Takt): Wurzel steifer, Spitze weich -> Peitsche.
 * Mit k ~0,1 schwingt die Kette mit ~20 Takten Periode: eine freigegebene Seitenlage kreuzt die Mitte nach wenigen
 * Takten und liegt nach ~10 auf der anderen Seite (Video: Schwanz h28 links oben, h30-h34 zwischen den Knien, ab h38
 * Bild-rechts). Erster Versuch k 0,34..0,11 / 0,85: Seitenwechsel
 * in 3 Takten (Unit-Test), zu hart. Keine Schwerkraft: die Keyframes haengen den Schwanz schon (Ruhe = Keyframe). */
static const float k_feder[SW_J] = { 1.0f, 0.22f, 0.18f, 0.15f, 0.12f, 0.10f, 0.08f, 0.07f };
#define SW_ERHALT   0.80f     /* Geschwindigkeit, die je Takt erhalten bleibt (0,88: Lauf n8/n9 schwang die Spitze nach
                                 * der Freigabe bis Bild-x 260-276 statt ~205-214 wie das Video h38-h48 -> 0,80) */
#define SW_SPRUNG   3000.0f   /* Wurzel springt weiter (Teleport, Raumwechsel): Kette neu auf das Ziel */

static struct {
    int      gueltig;
    uint32_t takt;
    float    p[SW_J][3], q[SW_J][3];          /* Lage jetzt / vorher (Welt) */
    int32_t  seite;                            /* Szenenvorgabe, Birkin-Raum (+ = rechte Seite = -z) */
} s_sw[RE15_ACTOR_MAX];

void re15_schwanz_r41_reset(void) { memset(s_sw, 0, sizeof s_sw); }

void re15_schwanz_r41_seite(int slot, int32_t seite)
{
    if (slot >= 0 && slot < RE15_ACTOR_MAX) s_sw[slot].seite = seite;
}

int re15_schwanz_r41_gelenk(int slot, int j, int32_t out[3])
{
    if (slot < 0 || slot >= RE15_ACTOR_MAX || j < 0 || j >= SW_J || !s_sw[slot].gueltig) return 0;
    for (int k = 0; k < 3; k++) out[k] = (int32_t)lrintf(s_sw[slot].p[j][k]);
    return 1;
}

static int kette_ok(const re15_emd_skeleton_t *s)
{
    if (s->bone_count != SW_K0 + SW_N || s->bone_parent[SW_K0] != 8) return 0;
    for (int b = SW_K0 + 1; b < SW_K0 + SW_N; b++)
        if (s->bone_parent[b] != b - 1) return 0;
    return 1;
}

/* Modell -> Welt wie re15_skel_bone_to_world (Gier um y, dann Aktorlage); Welt -> Modell umgekehrt. */
static void zu_welt(const re15_actor_t *a, float cs, float sn, const float m[3], float w[3])
{
    w[0] = (float)a->x + cs * m[0] + sn * m[2];
    w[1] = (float)a->y + m[1];
    w[2] = (float)a->z - sn * m[0] + cs * m[2];
}
static void zu_modell(const re15_actor_t *a, float cs, float sn, const float w[3], float m[3])
{
    const float dx = w[0] - (float)a->x, dz = w[2] - (float)a->z;
    m[0] = cs * dx - sn * dz;
    m[1] = w[1] - (float)a->y;
    m[2] = sn * dx + cs * dz;
}

static float norm(float v[3]);
static void mat_aus(const int32_t r[9], float f[9]) { for (int i = 0; i < 9; i++) f[i] = (float)r[i] / 4096.0f; }
/* Die Posenmatrizen des Ports sind nicht ganz normiert (Q12-Kette wie RotMatrix/MulMatrix: Spaltenlaengen 0,99..0,94
 * entlang der Kette, gemessen test_schwanz_r41). Fuer die Drehrechnung orthonormieren (Gram-Schmidt der Spalten),
 * den Massstab merken und am Ende wieder anbringen, damit Glieder und Netze so lang bleiben wie im Keyframe. */
static float ortho(float m[9])
{
    float c[3][3];
    for (int k = 0; k < 3; k++) for (int r = 0; r < 3; r++) c[k][r] = m[r * 3 + k];
    const float s = (sqrtf(c[0][0]*c[0][0] + c[0][1]*c[0][1] + c[0][2]*c[0][2]) +
                     sqrtf(c[1][0]*c[1][0] + c[1][1]*c[1][1] + c[1][2]*c[1][2]) +
                     sqrtf(c[2][0]*c[2][0] + c[2][1]*c[2][1] + c[2][2]*c[2][2])) / 3.0f;
    norm(c[1]);                                            /* Gliedachse (lokal y) zuerst */
    float d = c[0][0]*c[1][0] + c[0][1]*c[1][1] + c[0][2]*c[1][2];
    for (int r = 0; r < 3; r++) c[0][r] -= d * c[1][r];
    norm(c[0]);
    c[2][0] = c[0][1]*c[1][2] - c[0][2]*c[1][1];
    c[2][1] = c[0][2]*c[1][0] - c[0][0]*c[1][2];
    c[2][2] = c[0][0]*c[1][1] - c[0][1]*c[1][0];
    for (int k = 0; k < 3; k++) for (int r = 0; r < 3; r++) m[r * 3 + k] = c[k][r];
    return s;
}
static void mul(const float a[9], const float b[9], float o[9])
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            o[r * 3 + c] = a[r * 3] * b[c] + a[r * 3 + 1] * b[3 + c] + a[r * 3 + 2] * b[6 + c];
}
static void mul_tn(const float a[9], const float b[9], float o[9])   /* a^T * b */
{
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            o[r * 3 + c] = a[r] * b[c] + a[3 + r] * b[3 + c] + a[6 + r] * b[6 + c];
}
static void mul_v(const float a[9], const float v[3], float o[3])
{
    for (int r = 0; r < 3; r++) o[r] = a[r * 3] * v[0] + a[r * 3 + 1] * v[1] + a[r * 3 + 2] * v[2];
}
static float norm(float v[3])
{
    const float l = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if (l > 1e-6f) { v[0] /= l; v[1] /= l; v[2] /= l; }
    return l;
}
/* Kleinste Drehung, die Einheitsvektor a auf b dreht (Rodrigues). */
static void min_dreh(const float a[3], const float b[3], float R[9])
{
    float v[3] = { a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0] };
    const float c = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
    static const float I[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
    if (c < -0.9999f) { memcpy(R, I, sizeof I); return; }   /* entgegengesetzt: nicht drehen (kommt durch die Feder nicht vor) */
    const float k = 1.0f / (1.0f + c);
    const float K[9] = { 0, -v[2], v[1], v[2], 0, -v[0], -v[1], v[0], 0 };
    float KK[9];
    mul(K, K, KK);
    for (int i = 0; i < 9; i++) R[i] = I[i] + K[i] + KK[i] * k;
}

int re15_schwanz_r41_pose(const re15_actor_t *a, const re15_emd_skeleton_t *skel, re15_skel_pose_t *poses)
{
    if (!a || !skel || !poses || a->type != 0x30u || !kette_ok(skel)) return 0;
    const int slot = (int)(a - g_actors);
    if (slot < 0 || slot >= RE15_ACTOR_MAX) return 0;
    const float cs = (float)re15_cos_q12(a->rot_y) / 4096.0f, sn = (float)re15_sin_q12(a->rot_y) / 4096.0f;

    /* Ziel: Keyframe-Gelenke (Modell), Spitze = Knochen 22 + Achse * Laenge des Vorglieds (gleichlange Glieder). */
    float zm[SW_J][3], zw[SW_J][3], laenge[SW_J];
    for (int j = 0; j < SW_N; j++)
        for (int k = 0; k < 3; k++) zm[j][k] = (float)poses[SW_K0 + j].trans[k];
    {
        const int16_t *r = skel->bone_relative_pos[SW_K0 + SW_N - 1];
        const float lt = sqrtf((float)r[0] * r[0] + (float)r[1] * r[1] + (float)r[2] * r[2]);
        const int32_t *R = poses[SW_K0 + SW_N - 1].rot;
        for (int k = 0; k < 3; k++) zm[SW_N][k] = zm[SW_N - 1][k] + (float)R[k * 3 + 1] / 4096.0f * lt;
    }
    /* Seitenvorgabe quadratisch zur Spitze (oberes Stueck haengt weiter, die Spitze schwingt aus): linear gewichtet
     * stand der Schwanz im Lauf n4 h22-h26 waagerecht in Hufthoehe ab, das Video zeigt ihn von der Huefte bis zum Boden. */
    for (int j = 1; j < SW_J; j++)
        zm[j][2] -= (float)s_sw[slot].seite * (float)(j * j) / (float)(SW_N * SW_N);   /* rechte Seite = Birkin-Raum -z */
    for (int j = 0; j < SW_J; j++) zu_welt(a, cs, sn, zm[j], zw[j]);
    laenge[0] = 0.0f;
    for (int j = 1; j < SW_J; j++) {
        float d[3] = { zw[j][0] - zw[j - 1][0], zw[j][1] - zw[j - 1][1], zw[j][2] - zw[j - 1][2] };
        laenge[j] = norm(d);
    }

    /* Ein Kettenschritt je Spieltakt. */
    if (!s_sw[slot].gueltig || s_sw[slot].takt != g_engine.frame_count) {
        float sp[3] = { s_sw[slot].p[0][0] - zw[0][0], s_sw[slot].p[0][1] - zw[0][1], s_sw[slot].p[0][2] - zw[0][2] };
        if (!s_sw[slot].gueltig || norm(sp) > SW_SPRUNG) {
            memcpy(s_sw[slot].p, zw, sizeof zw);
            memcpy(s_sw[slot].q, zw, sizeof zw);
            s_sw[slot].gueltig = 1;
        } else {
            float (*p)[3] = s_sw[slot].p, (*q)[3] = s_sw[slot].q;
            for (int k = 0; k < 3; k++) { q[0][k] = p[0][k]; p[0][k] = zw[0][k]; }     /* Wurzel haengt am Rumpf */
            for (int j = 1; j < SW_J; j++)
                for (int k = 0; k < 3; k++) {
                    const float v = (p[j][k] - q[j][k]) * SW_ERHALT;
                    q[j][k] = p[j][k];
                    p[j][k] += v + (zw[j][k] - p[j][k]) * k_feder[j];
                }
            for (int j = 1; j < SW_J; j++) {                                  /* feste Gliedlaengen, Boden */
                float d[3] = { p[j][0] - p[j - 1][0], p[j][1] - p[j - 1][1], p[j][2] - p[j - 1][2] };
                if (norm(d) < 1e-3f) { d[0] = zw[j][0] - zw[j - 1][0]; d[1] = zw[j][1] - zw[j - 1][1];
                                      d[2] = zw[j][2] - zw[j - 1][2]; norm(d); }
                for (int k = 0; k < 3; k++) p[j][k] = p[j - 1][k] + d[k] * laenge[j];
                if (p[j][1] > (float)a->y) p[j][1] = (float)a->y;            /* nicht unter den Boden (y+ = unten) */
            }
        }
        s_sw[slot].takt = g_engine.frame_count;
    }

    /* Knochen 16-22 neu drehen: Keyframe-Lokaldrehung behalten, dann kleinste Drehung auf das simulierte Glied. */
    float alt[SW_N + 1][9], neu_p[9], t_p[3], s_p;
    mat_aus(poses[8].rot, alt[0]);
    s_p = ortho(alt[0]);
    memcpy(neu_p, alt[0], sizeof neu_p);
    for (int k = 0; k < 3; k++) t_p[k] = (float)poses[8].trans[k];
    for (int j = 0; j < SW_N; j++) {
        const int b = SW_K0 + j;
        float Rb[9], L[9], Rc[9], Rm[9], Rn[9], tb[3], ax[3], ds[3], ziel_m[3], s_b;
        mat_aus(poses[b].rot, Rb);
        s_b = ortho(Rb);
        mul_tn(alt[j], Rb, L);                                /* lokal = Eltern^T * Knochen (Keyframe, normiert) */
        memcpy(alt[j + 1], Rb, sizeof Rb);
        mul(neu_p, L, Rc);
        {   const float rel[3] = { (float)skel->bone_relative_pos[b][0], (float)skel->bone_relative_pos[b][1],
                                   (float)skel->bone_relative_pos[b][2] };
            mul_v(neu_p, rel, tb);                            /* wie re15_skel_compute_pose: Elternmatrix (mit Massstab) * rel */
            for (int k = 0; k < 3; k++) tb[k] = tb[k] * s_p + t_p[k]; }
        ax[0] = Rc[1]; ax[1] = Rc[4]; ax[2] = Rc[7];          /* Gliedachse = lokale y */
        norm(ax);
        zu_modell(a, cs, sn, s_sw[slot].p[j + 1], ziel_m);
        for (int k = 0; k < 3; k++) ds[k] = ziel_m[k] - tb[k];
        if (norm(ds) > 1e-3f) { min_dreh(ax, ds, Rm); mul(Rm, Rc, Rn); }
        else memcpy(Rn, Rc, sizeof Rn);
        for (int i = 0; i < 9; i++) poses[b].rot[i] = (int32_t)lrintf(Rn[i] * s_b * 4096.0f);
        for (int k = 0; k < 3; k++) poses[b].trans[k] = (int32_t)lrintf(tb[k]);
        memcpy(neu_p, Rn, sizeof neu_p);
        memcpy(t_p, tb, sizeof t_p);
        s_p = s_b;
    }
    return 1;
}
