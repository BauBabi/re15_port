/* =============================================================================
 * probe_gator_fress_anker - MESSSONDE (kein add_test): Auftrag B, Fress-Finisher
 * ROOM2090. Reproduziert die P2/P3-Platzierung des Gator-Finishers mit der
 * ECHTEN EM23-Bank (shared_assets/RE2/EM23.EMD) und den ECHTEN Port-Helfern
 * (re15_clip_anchor_set_pub / re15_clip_root_motion_abs_pub, Pose ueber
 * re15_skel_compute_pose + Render-Skalierung +0x166) und misst:
 *   (A) Port-Stand heute  : Gator = Clip 5 @ sf=120 (120 % 120 -> f0),
 *                           Leon  = Opfer-Clip 1 @ f119, Leon-y unberuehrt
 *   (B) Original P3 Bild 1: Gator = Clip 5 @ f119 (@0x801011cc platziert VOR
 *                           FUN_8001A240 @0x801011e0 -> noch Clip 5/f119)
 *   (C) Original P3 Dauer : Gator = Clip 11 (@0x801011d8 a1=11)
 *   (D) je Variante mit der dritten Ankerkomponente y (RE2 FUN_80015B94
 *       @0x80015c88 / FUN_80015CB8 @0x80015d6c-7c).
 * ===========================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_actor.h"
#include "re15_skeleton.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *path, size_t *out_sz)
{
    FILE *f = fopen(path, "rb");
    long sz;
    uint8_t *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f);
    if (out_sz) *out_sz = (size_t)sz;
    return b;
}

static re15_enemy_bank_t s_bank;

static void bone_matrix(const re15_actor_t *e, const re15_skel_pose_t *np,
                        int32_t R[9], int32_t T[3])
{
    int32_t fs = re15_sin_q12((int)e->rot_y), fc = re15_cos_q12((int)e->rot_y);
    int32_t nyaw[9];
    int r, c, k;
    nyaw[0] = fc; nyaw[1] = 0; nyaw[2] = fs;
    nyaw[3] = 0;  nyaw[4] = 0x1000; nyaw[5] = 0;
    nyaw[6] = -fs; nyaw[7] = 0; nyaw[8] = fc;
    if (e->render_scale_q12)
        for (k = 0; k < 9; k++)
            nyaw[k] = (nyaw[k] * (int32_t)e->render_scale_q12) >> 12;
    for (r = 0; r < 3; r++) for (c = 0; c < 3; c++) {
        int64_t s = 0;
        for (k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)np->rot[k*3+c];
        R[r*3+c] = (int32_t)(s >> 12);
    }
    for (r = 0; r < 3; r++) {
        int64_t s = 0;
        for (k = 0; k < 3; k++) s += (int64_t)nyaw[r*3+k] * (int64_t)np->trans[k];
        T[r] = (int32_t)(s >> 12);
    }
    T[0] += e->x; T[1] += e->y; T[2] += e->z;
}

static int mesh_stats(const re15_actor_t *e, int mesh, int kf,
                      int32_t lo[3], int32_t hi[3], const int32_t p[3], double *out_min)
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int32_t R[9], T[3];
    const re15_md1_mesh_t *m;
    int pass, i, r, n = 0;
    double best = 1e18;
    if (re15_skel_compute_pose(&s_bank.skel, kf, poses) != 0) return -1;
    if (mesh < 0 || mesh >= s_bank.md1.mesh_count) return -2;
    bone_matrix(e, &poses[mesh], R, T);
    m = &s_bank.md1.meshes[mesh];
    lo[0] = lo[1] = lo[2] = 0x7fffffff;
    hi[0] = hi[1] = hi[2] = -0x7fffffff;
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
        int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
        for (i = 0; i < NV; i++) {
            int32_t v[3]; int32_t w[3]; double dx, dy, dz, d;
            v[0] = V[i].x; v[1] = V[i].y; v[2] = V[i].z;
            for (r = 0; r < 3; r++) {
                int64_t s = 0; int k;
                for (k = 0; k < 3; k++) s += (int64_t)R[r*3+k] * (int64_t)v[k];
                w[r] = (int32_t)(s >> 12) + T[r];
            }
            for (r = 0; r < 3; r++) {
                if (w[r] < lo[r]) lo[r] = w[r];
                if (w[r] > hi[r]) hi[r] = w[r];
            }
            dx = (double)w[0] - p[0]; dy = (double)w[1] - p[1]; dz = (double)w[2] - p[2];
            d = sqrt(dx*dx + dy*dy + dz*dz);
            if (d < best) best = d;
            n++;
        }
    }
    if (out_min) *out_min = best;
    return n ? 0 : -3;
}

static int gator_kf(const re15_actor_t *e)
{
    const re15_emd_clip_t *c = &s_bank.anim.clips[e->motion];
    int slot = (int)e->anim_frame % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    while ((s_bank.anim.frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(s_bank.anim.frames[fi] & 0xFFFu);
}

static void bericht(const char *name, re15_actor_t *e, re15_actor_t *pl)
{
    int kf = gator_kf(e);
    double ddx, ddz;
    int32_t p[3];
    int mi;
    printf("--- %s\n", name);
    printf("    Gator clip=%d f=%u kf=%d pos=(%d,%d,%d) yaw=%d scale=%u\n",
           (int)e->motion, (unsigned)e->anim_frame, kf,
           (int)e->x, (int)e->y, (int)e->z, (int)e->rot_y,
           (unsigned)e->render_scale_q12);
    printf("    Leon  clip=%d f=%u pos=(%d,%d,%d)\n",
           (int)pl->motion, (unsigned)pl->anim_frame,
           (int)pl->x, (int)pl->y, (int)pl->z);
    ddx = (double)pl->x - e->x; ddz = (double)pl->z - e->z;
    printf("    XZ-Wurzelabstand = %.2f\n", sqrt(ddx*ddx + ddz*ddz));
    /* GERENDERTE Leon-Wurzel: Entity-Pos + RotY(yaw) * Keyframe-Wurzel (px,py,pz)
     * der OPFER-Bank. Der Port nutzt sie so (skeleton_common.c rt_x/y/z, byte-true
     * FUN_8001f3bc) - sie traegt die Schleuderbahn in y/z. */
    {
        int kfv, sl, fiv, fev;
        int16_t px = 0, py = 0, pz = 0;
        const re15_emd_clip_t *cv = &s_bank.anim_victim.clips[pl->motion];
        int32_t tr[3], w[3];
        sl  = (int)pl->anim_frame % cv->frame_count;
        fiv = cv->first_frame + sl;
        fev = cv->first_frame + cv->frame_count - 1;
        while ((s_bank.anim_victim.frames[fiv] & 0x8000u) && fiv < fev) fiv++;
        kfv = (int)(s_bank.anim_victim.frames[fiv] & 0xFFFu);
        re15_emd_get_keyframe_position(&s_bank.skel_victim, kfv, &px, &py, &pz);
        tr[0] = px; tr[1] = py; tr[2] = pz;
        re15_skel_bone_to_world(tr, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, w);
        printf("    Leon Opfer-kf=%d Wurzelpose=(%d,%d,%d) -> GERENDERT (%d,%d,%d)\n",
               kfv, (int)px, (int)py, (int)pz, (int)w[0], (int)w[1], (int)w[2]);
        p[0] = w[0]; p[1] = w[1]; p[2] = w[2];
    }
    for (mi = 6; mi <= 7; mi++) {
        int32_t lo[3], hi[3];
        double dmin = -1.0;
        if (mesh_stats(e, mi, kf, lo, hi, p, &dmin) == 0)
            printf("    Mesh %d AABB x[%d..%d] y[%d..%d] z[%d..%d] minDist(Leon)=%.1f\n",
                   mi, lo[0], hi[0], lo[1], hi[1], lo[2], hi[2], dmin);
    }
}

int main(void)
{
    size_t sz = 0;
    uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    re15_actor_t e, pl;
    int rc, f;
    unsigned qi;
    struct {
        const char *n;
        const re15_emd_skeleton_t *sk;
        const re15_emd_animation_t *an;
        int clip, fr;
    } q[7];

    if (!emd) { printf("EM23.EMD fehlt\n"); return 1; }
    memset(&s_bank, 0, sizeof s_bank);
    rc = re2_emd_parse_bank(emd, sz, &s_bank);
    printf("EM23.EMD %lu B parse=%d clips=%d victim_ok=%d meshes=%d bones=%d\n",
           (unsigned long)sz, rc, s_bank.anim.clip_count, s_bank.victim_ok,
           s_bank.md1.mesh_count, s_bank.skel.bone_count);
    if (rc != 0) return 1;
    s_bank.ok = 1;

    memset(&e, 0, sizeof e); memset(&pl, 0, sizeof pl);
    e.type = 0x23; e.rot_y = 0; e.render_scale_q12 = 2731;
    e.x = 0; e.y = -1200; e.z = 0;
    e.motion = 4; e.anim_frame = 13;
    re15_clip_anchor_set_pub(&e, &s_bank.skel, &s_bank.anim, 4, 13);
    printf("Anker(Gator) = (%d,%d); y-Leitung wuerde anchor_y = %d tragen\n",
           (int)e.anchor_x, (int)e.anchor_z, (int)e.y);
    pl.anchor_x = e.anchor_x; pl.anchor_z = e.anchor_z;
    pl.rot_y = e.rot_y;

    pl.motion = 1; pl.anim_frame = 119; pl.y = 0;
    re15_clip_root_motion_abs_pub(&pl, &s_bank.skel_victim, &s_bank.anim_victim, 1, 119);

    e.motion = 5; e.anim_frame = 120;
    re15_clip_root_motion_abs_pub(&e, &s_bank.skel, &s_bank.anim, 5, 120);
    e.anim_frame = 0;
    bericht("(A) PORT HEUTE Gator Clip5 f120 mod 120 = 0, Leon y=0", &e, &pl);

    {
        re15_actor_t p2 = pl;
        p2.y = e.y;
        bericht("(A2) PORT HEUTE + y-Leitung (Leon y = Anker-y)", &e, &p2);
    }

    e.motion = 5; e.anim_frame = 119;
    re15_clip_root_motion_abs_pub(&e, &s_bank.skel, &s_bank.anim, 5, 119);
    bericht("(B) ORIGINAL P3 Bild 1: Gator Clip5 f119", &e, &pl);

    for (f = 0; f <= 2; f++) {
        char nm[96];
        e.motion = 11; e.anim_frame = (uint32_t)f;
        re15_clip_root_motion_abs_pub(&e, &s_bank.skel, &s_bank.anim, 11, f);
        snprintf(nm, sizeof nm, "(C) ORIGINAL P3 Dauer: Gator Clip11 f%d", f);
        bericht(nm, &e, &pl);
    }

    e.motion = 11; e.anim_frame = 0;
    re15_clip_root_motion_abs_pub(&e, &s_bank.skel, &s_bank.anim, 11, 0);
    {
        re15_actor_t p2 = pl;
        p2.y = e.y;
        bericht("(D) ORIGINAL P3 Clip11 f0 + y-Leitung", &e, &p2);
    }

    printf("\nWURZELVERSAETZE (byte-gelesen aus EM23.EMD):\n");
    q[0].n = "Gator Clip4  f13 "; q[0].sk = &s_bank.skel; q[0].an = &s_bank.anim; q[0].clip = 4;  q[0].fr = 13;
    q[1].n = "Gator Clip5  f0  "; q[1].sk = &s_bank.skel; q[1].an = &s_bank.anim; q[1].clip = 5;  q[1].fr = 0;
    q[2].n = "Gator Clip5  f119"; q[2].sk = &s_bank.skel; q[2].an = &s_bank.anim; q[2].clip = 5;  q[2].fr = 119;
    q[3].n = "Gator Clip11 f0  "; q[3].sk = &s_bank.skel; q[3].an = &s_bank.anim; q[3].clip = 11; q[3].fr = 0;
    q[4].n = "Gator Clip11 f58 "; q[4].sk = &s_bank.skel; q[4].an = &s_bank.anim; q[4].clip = 11; q[4].fr = 58;
    q[5].n = "Opfer Clip1  f0  "; q[5].sk = &s_bank.skel_victim; q[5].an = &s_bank.anim_victim; q[5].clip = 1; q[5].fr = 0;
    q[6].n = "Opfer Clip1  f119"; q[6].sk = &s_bank.skel_victim; q[6].an = &s_bank.anim_victim; q[6].clip = 1; q[6].fr = 119;
    for (qi = 0; qi < sizeof q / sizeof q[0]; qi++) {
        re15_actor_t t;
        memset(&t, 0, sizeof t);
        t.rot_y = 0; t.anchor_x = 0; t.anchor_z = 0;
        re15_clip_root_motion_abs_pub(&t, q[qi].sk, q[qi].an, q[qi].clip, q[qi].fr);
        printf("   %s -> (sx=%d, sz=%d)\n", q[qi].n, (int)t.x, (int)t.z);
    }
    free(emd);
    return 0;
}
