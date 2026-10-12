/* bissarm_r37.c — Runde 37 Spur J "bissarm": der dranhaengende Zombie-Arm beim Biss.
 * Herleitung, Adressen und Messungen: include/re15_bissarm_r37.h und analysis/befunde_runde37/J_bissarm.md. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re15_bissarm_r37.h"
#include "re15_ai_flavor.h"
#include "re15_engine.h"   /* g_engine.frame_count (nur Messhaken) */

extern void *g_anim_pose_actor;

/* ---- RE2-Bindtabelle je Zombie-Typ ------------------------------------------------------------ */
static int16_t s_re2_bind[256][RE15_EMD_MAX_BONES][3];
static uint8_t s_re2_bind_ok[256];

void re15_bissarm_r37_re2_bind_merken(int kind, const re15_emd_skeleton_t *re2skel)
{
    if (kind < 0 || kind > 255 || !re2skel) return;
    memcpy(s_re2_bind[kind], re2skel->bone_relative_pos, sizeof s_re2_bind[kind]);
    s_re2_bind_ok[kind] = 1;
}

int re15_bissarm_r37_re2_bind_da(uint8_t type) { return s_re2_bind_ok[type] != 0; }

/* ---- (B) EIN Wandklemm-Durchgang je Bild (RE2 FUN_8003567c / RE1.5 @0x80031d70) ------------------ */
int re15_bissarm_r37_ein_durchgang(uint8_t greifer_typ)
{
    return re15_re2z_owns_type(greifer_typ);   /* Zombie-Familie 0x10/0x11/0x12/0x13/0x16/0x18 */
}

/* ---- (A) Handkontakt ------------------------------------------------------------------------- */
static int32_t s_ziel[2][3], s_rest[2];

void re15_bissarm_r37_letzte_ziele(int32_t ziel[2][3], int32_t rest[2])
{
    memcpy(ziel, s_ziel, sizeof s_ziel);
    memcpy(rest, s_rest, sizeof s_rest);
}

/* Kuerzeste Drehung, die Richtung a auf Richtung b legt (Rodrigues), als 3x3 (Zeilen). */
static void dreh_auf(const double a[3], const double b[3], double R[9])
{
    double la = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    double lb = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
    memset(R, 0, 9 * sizeof(double)); R[0] = R[4] = R[8] = 1.0;
    if (la < 1e-9 || lb < 1e-9) return;
    double u[3] = { a[0] / la, a[1] / la, a[2] / la }, v[3] = { b[0] / lb, b[1] / lb, b[2] / lb };
    double c = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
    double k[3] = { u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0] };
    double s = sqrt(k[0] * k[0] + k[1] * k[1] + k[2] * k[2]);
    if (s < 1e-9) return;                       /* gleichgerichtet (antiparallel kommt hier nicht vor) */
    k[0] /= s; k[1] /= s; k[2] /= s;
    double t = 1.0 - c;
    R[0] = c + k[0] * k[0] * t;        R[1] = k[0] * k[1] * t - k[2] * s; R[2] = k[0] * k[2] * t + k[1] * s;
    R[3] = k[1] * k[0] * t + k[2] * s; R[4] = c + k[1] * k[1] * t;        R[5] = k[1] * k[2] * t - k[0] * s;
    R[6] = k[2] * k[0] * t - k[1] * s; R[7] = k[2] * k[1] * t + k[0] * s; R[8] = c + k[2] * k[2] * t;
}
static void mat_mal(const double A[9], const double B[9], double C[9])
{
    for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++)
        C[r * 3 + c] = A[r * 3 + 0] * B[0 * 3 + c] + A[r * 3 + 1] * B[1 * 3 + c] + A[r * 3 + 2] * B[2 * 3 + c];
}
static void vor_drehen(const double R[9], int32_t m[9])   /* m := R * m (Q12 bleibt Q12) */
{
    double o[9];
    for (int r = 0; r < 3; r++) for (int c = 0; c < 3; c++)
        o[r * 3 + c] = R[r * 3 + 0] * m[0 * 3 + c] + R[r * 3 + 1] * m[1 * 3 + c] + R[r * 3 + 2] * m[2 * 3 + c];
    for (int i = 0; i < 9; i++) m[i] = (int32_t)lround(o[i]);
}
static double laenge(const int16_t b[3])
{
    return sqrt((double)b[0] * b[0] + (double)b[1] * b[1] + (double)b[2] * b[2]);
}

int re15_bissarm_r37_handkontakt(const re15_actor_t *e, const re15_emd_skeleton_t *sk, int kf,
                                 re15_skel_pose_t *poses, int n)
{
    s_rest[0] = s_rest[1] = -1;
    if (!e || !sk || !poses || n < 15 || sk->bone_count < 15 || kf < 0) return 0;
    if (!re15_re2z_owns_type(e->type) || !re15_ai_re2_for_type(e->type)) return 0;
    if (!s_re2_bind_ok[e->type]) return 0;                 /* kein Hybrid -> RE2-Rig, nichts zu tun */
    /* Griff = RE2-Exec[3] (Zustand 1, +0x5 == 3): P0 Zupacken bis P5 Abwurf (enemy_ai_re2_zombie.c
     * re2z_exec_grab, @0x801025EC; Tabelle @0x8010001C). */
    if (e->state != 1 || e->sub_state_1 != 3) return 0;
    /* Nur das HYBRID-Rig: dessen Arm-Bindlaengen weichen von der RE2-Tabelle ab. */
    if (memcmp(sk->bone_relative_pos[10], s_re2_bind[e->type][10], sizeof sk->bone_relative_pos[10]) == 0)
        return 0;

    /* Dieselbe Pose zweimal OHNE Ueberblendung: einmal mit dem Hybrid-Rig, einmal mit dem RE2-Rig (RE2-Bind,
     * keine Wurzelkorrektur) — das RE2-Rig zeichnet exakt das RE2-Original (gemessen: Hand 11 Y -2423 im Port
     * wie im RE2-RAM). Der Unterschied je Gelenk wird auf die GEZEICHNETE (ueberblendete) Pose gelegt, damit die
     * Ueberblendung (+0x8F) unveraendert weiterlaeuft. */
    static re15_emd_skeleton_t r2;
    static re15_skel_pose_t hu[RE15_EMD_MAX_BONES], ru[RE15_EMD_MAX_BONES];
    r2 = *sk;
    memcpy(r2.bone_relative_pos, s_re2_bind[e->type], sizeof r2.bone_relative_pos);
    r2.root_y_fix = NULL; r2.root_y_fix_count = 0;
    void *sav = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int ok = (re15_skel_compute_pose(sk, kf, hu) == 0) && (re15_skel_compute_pose(&r2, kf, ru) == 0);
    g_anim_pose_actor = sav;
    if (!ok) return 0;

    for (int arm = 0; arm < 2; arm++) {
        const int a = arm ? 12 : 9;                         /* Schulter; a+1 Ellbogen, a+2 Hand (RE2-Reihenfolge) */
        double S[3], T[3], P[3];
        for (int i = 0; i < 3; i++) {
            S[i] = poses[a].trans[i];
            T[i] = poses[a + 2].trans[i] + (double)(ru[a + 2].trans[i] - hu[a + 2].trans[i]);
            P[i] = poses[a + 1].trans[i] + (double)(ru[a + 1].trans[i] - hu[a + 1].trans[i]);
            s_ziel[arm][i] = (int32_t)lround(T[i]);
        }
        const double L1 = laenge(sk->bone_relative_pos[a + 1]);   /* RE1.5-Oberarm (Hybrid-Bind) */
        const double L2 = laenge(sk->bone_relative_pos[a + 2]);   /* RE1.5-Unterarm */
        double D[3] = { T[0] - S[0], T[1] - S[1], T[2] - S[2] };
        double d = sqrt(D[0] * D[0] + D[1] * D[1] + D[2] * D[2]);
        if (d < 1.0 || L1 < 1.0 || L2 < 1.0) continue;
        double u[3] = { D[0] / d, D[1] / d, D[2] / d };
        double dm = d;
        if (dm > L1 + L2 - 1.0) dm = L1 + L2 - 1.0;               /* nicht erreichbar: Arm gestreckt zum Ziel */
        if (dm < fabs(L1 - L2) + 1.0) dm = fabs(L1 - L2) + 1.0;
        double Tm[3] = { S[0] + u[0] * dm, S[1] + u[1] * dm, S[2] + u[2] * dm };
        s_rest[arm] = (int32_t)lround(d - dm);
        /* Beugeseite = RE2-Ellbogen (Anteil senkrecht zur Schulter-Hand-Achse). */
        double w[3] = { P[0] - S[0], P[1] - S[1], P[2] - S[2] };
        double wu = w[0] * u[0] + w[1] * u[1] + w[2] * u[2];
        double v[3] = { w[0] - wu * u[0], w[1] - wu * u[1], w[2] - wu * u[2] };
        double lv = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (lv < 1e-6) {                                           /* Ellbogen auf der Achse: alte Beugeseite */
            double w2[3] = { poses[a + 1].trans[0] - S[0], poses[a + 1].trans[1] - S[1], poses[a + 1].trans[2] - S[2] };
            double w2u = w2[0] * u[0] + w2[1] * u[1] + w2[2] * u[2];
            v[0] = w2[0] - w2u * u[0]; v[1] = w2[1] - w2u * u[1]; v[2] = w2[2] - w2u * u[2];
            lv = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            if (lv < 1e-6) continue;
        }
        v[0] /= lv; v[1] /= lv; v[2] /= lv;
        double ca = (L1 * L1 + dm * dm - L2 * L2) / (2.0 * L1 * dm);
        if (ca > 1.0) ca = 1.0;
        if (ca < -1.0) ca = -1.0;
        double sa = sqrt(1.0 - ca * ca);
        double E[3];
        for (int i = 0; i < 3; i++) E[i] = S[i] + L1 * (ca * u[i] + sa * v[i]);
        /* Oberarm: alte Richtung (Schulter->Ellbogen) auf die neue drehen; Unter-/Hand-Glied erben die Drehung. */
        double alt_o[3] = { poses[a + 1].trans[0] - S[0], poses[a + 1].trans[1] - S[1], poses[a + 1].trans[2] - S[2] };
        double neu_o[3] = { E[0] - S[0], E[1] - S[1], E[2] - S[2] };
        double Ra[9]; dreh_auf(alt_o, neu_o, Ra);
        double alt_u0[3] = { poses[a + 2].trans[0] - poses[a + 1].trans[0], poses[a + 2].trans[1] - poses[a + 1].trans[1],
                             poses[a + 2].trans[2] - poses[a + 1].trans[2] };
        double alt_u[3];
        for (int r = 0; r < 3; r++) alt_u[r] = Ra[r * 3 + 0] * alt_u0[0] + Ra[r * 3 + 1] * alt_u0[1] + Ra[r * 3 + 2] * alt_u0[2];
        double neu_u[3] = { Tm[0] - E[0], Tm[1] - E[1], Tm[2] - E[2] };
        double Rb[9]; dreh_auf(alt_u, neu_u, Rb);
        double Rba[9]; mat_mal(Rb, Ra, Rba);
        vor_drehen(Ra,  poses[a].rot);
        vor_drehen(Rba, poses[a + 1].rot);
        vor_drehen(Rba, poses[a + 2].rot);
        for (int i = 0; i < 3; i++) {
            poses[a + 1].trans[i] = (int32_t)lround(E[i]);
            poses[a + 2].trans[i] = (int32_t)lround(Tm[i]);
        }
    }

    /* Mess-Schiene RE15_BISSARM_LOG=<datei>: je Griffbild die gezeichneten Hand-Gelenke gegen den RE2-Zielpunkt
     * (Weltkoordinaten). Kein Verhalten. */
    {
        static int s_log_init = 0; static FILE *s_log = NULL;
        if (!s_log_init) { s_log_init = 1; const char *lp = getenv("RE15_BISSARM_LOG"); if (lp && *lp) s_log = fopen(lp, "w"); }
        if (s_log) {
            int32_t h0[3], h1[3], z0[3], z1[3];
            re15_skel_bone_to_world(poses[11].trans, e->rot_y, e->x, e->y, e->z, h0);
            re15_skel_bone_to_world(poses[14].trans, e->rot_y, e->x, e->y, e->z, h1);
            re15_skel_bone_to_world(s_ziel[0], e->rot_y, e->x, e->y, e->z, z0);
            re15_skel_bone_to_world(s_ziel[1], e->rot_y, e->x, e->y, e->z, z1);
            fprintf(s_log, "F%u typ=0x%02X ss2=%d clip=%d fr=%d hand11=(%d,%d,%d) ziel11=(%d,%d,%d) rest11=%d"
                           " hand14=(%d,%d,%d) ziel14=(%d,%d,%d) rest14=%d\n",
                    (unsigned)g_engine.frame_count, (unsigned)e->type, (int)e->sub_state_2, (int)e->motion,
                    (int)e->anim_frame, h0[0], h0[1], h0[2], z0[0], z0[1], z0[2], (int)s_rest[0],
                    h1[0], h1[1], h1[2], z1[0], z1[1], z1[2], (int)s_rest[1]);
            fflush(s_log);
        }
    }
    return 1;
}
