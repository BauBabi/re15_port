/* GEGENPRUEFUNG GB_SCALE_Q12 (Verifikationsauftrag 2026-09-14).
 * Misst die Ausdehnung des RE2-EM23 (alle Meshes/Bones) in Modellkoordinaten -
 * ohne Skalierung, mit 2731 und mit 4096 (Q12 1.0), einmal als gerenderte
 * Keyframe-Pose und einmal als reine REST-/Bindpose (nur EMR-Bone-Offsets).
 * Zweck: die im Quellkommentar enemy_ai_boss_gator.c:132-135 behaupteten Masse
 * (x-Laenge 18313, Hoehe 3984 aus y -1896..+2088, Breite 3378) nachmessen.
 * Kein add_test. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_enemy.h"
#include "re15_actor.h"
#include "re15_skeleton.h"

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *path, size_t *out_sz)
{
    FILE *f = fopen(path, "rb"); long sz; uint8_t *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); sz = ftell(f); fseek(f, 0, SEEK_SET);
    b = (uint8_t *)malloc((size_t)sz);
    if (!b || fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); fclose(f); return NULL; }
    fclose(f); if (out_sz) *out_sz = (size_t)sz; return b;
}

static re15_enemy_bank_t s_bank;

/* Gerenderte Pose: exakt der Weg aus platform/pc/main.c:8245-8261 (nyaw-Matrix
 * mit render_scale_q12 multipliziert, dann Bone-Rotation, dann Vertices). */
static void extents_kf(int kf, int32_t scale, int32_t lo[3], int32_t hi[3])
{
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    int mesh, pass, i, r, k;
    lo[0]=lo[1]=lo[2]=0x7fffffff; hi[0]=hi[1]=hi[2]=-0x7fffffff;
    if (re15_skel_compute_pose(&s_bank.skel, kf, poses) != 0) { printf("pose fail kf=%d\n", kf); return; }
    for (mesh = 0; mesh < s_bank.md1.mesh_count && mesh < s_bank.skel.bone_count; mesh++) {
        const re15_md1_mesh_t *m = &s_bank.md1.meshes[mesh];
        int32_t nyaw[9] = { 0x1000,0,0, 0,0x1000,0, 0,0,0x1000 };
        int32_t R[9], T[3];
        if (scale) for (k=0;k<9;k++) nyaw[k] = (nyaw[k]*scale) >> 12;
        for (r=0;r<3;r++) { int c; for (c=0;c<3;c++) {
            int64_t s=0; for (k=0;k<3;k++) s += (int64_t)nyaw[r*3+k]*(int64_t)poses[mesh].rot[k*3+c];
            R[r*3+c] = (int32_t)(s>>12); } }
        for (r=0;r<3;r++) { int64_t s=0;
            for (k=0;k<3;k++) s += (int64_t)nyaw[r*3+k]*(int64_t)poses[mesh].trans[k];
            T[r] = (int32_t)(s>>12); }
        for (pass=0; pass<2; pass++) {
            const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
            int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
            for (i=0;i<NV;i++) {
                int32_t v[3], w[3];
                v[0]=V[i].x; v[1]=V[i].y; v[2]=V[i].z;
                for (r=0;r<3;r++) { int64_t s=0;
                    for (k=0;k<3;k++) s += (int64_t)R[r*3+k]*(int64_t)v[k];
                    w[r] = (int32_t)(s>>12) + T[r]; }
                for (r=0;r<3;r++) { if (w[r]<lo[r]) lo[r]=w[r]; if (w[r]>hi[r]) hi[r]=w[r]; }
            }
        }
    }
}

/* REST-/BINDPOSE: nur EMR-Bone-Offsets akkumuliert (Rotationen = Identitaet) -
 * genau das, was der Quellkommentar "MD1-Vertices + EMR-Bone-Offsets" nennt. */
static void extents_rest(int32_t scale, int32_t lo[3], int32_t hi[3])
{
    int32_t acc[RE15_EMD_MAX_BONES][3];
    int b, mesh, pass, i, r;
    lo[0]=lo[1]=lo[2]=0x7fffffff; hi[0]=hi[1]=hi[2]=-0x7fffffff;
    for (b=0; b<s_bank.skel.bone_count; b++) {
        int par = s_bank.skel.bone_parent[b];
        for (r=0;r<3;r++) acc[b][r] = (int32_t)s_bank.skel.bone_relative_pos[b][r]
                                     + ((par>=0 && par<b) ? acc[par][r] : 0);
    }
    for (mesh = 0; mesh < s_bank.md1.mesh_count && mesh < s_bank.skel.bone_count; mesh++) {
        const re15_md1_mesh_t *m = &s_bank.md1.meshes[mesh];
        for (pass=0; pass<2; pass++) {
            const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
            int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
            for (i=0;i<NV;i++) {
                int32_t w[3];
                w[0]=(int32_t)V[i].x+acc[mesh][0];
                w[1]=(int32_t)V[i].y+acc[mesh][1];
                w[2]=(int32_t)V[i].z+acc[mesh][2];
                if (scale) for (r=0;r<3;r++) w[r] = (w[r]*scale)>>12;
                for (r=0;r<3;r++) { if (w[r]<lo[r]) lo[r]=w[r]; if (w[r]>hi[r]) hi[r]=w[r]; }
            }
        }
    }
}

static void zeile(const char *tag, int32_t scale, const int32_t lo[3], const int32_t hi[3])
{
    printf("   %-14s scale=%4d  x[%7d..%7d] len=%6d | y[%7d..%7d] hoehe=%6d | z[%7d..%7d] breite=%6d\n",
           tag, (int)scale, lo[0],hi[0],hi[0]-lo[0], lo[1],hi[1],hi[1]-lo[1],
           lo[2],hi[2],hi[2]-lo[2]);
}

int main(void)
{
    size_t sz=0; uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    int rc, si; const int32_t scales[3] = { 0, 2731, 4096 };
    int32_t lo[3], hi[3];
    if (!emd) { printf("EM23.EMD fehlt\n"); return 1; }
    memset(&s_bank,0,sizeof s_bank);
    rc = re2_emd_parse_bank(emd, sz, &s_bank);
    printf("parse=%d bones=%d meshes=%d clips=%d\n", rc,
           s_bank.skel.bone_count, s_bank.md1.mesh_count, s_bank.anim.clip_count);
    if (rc) return 1;
    s_bank.ok = 1;

    for (si=0; si<3; si++) { extents_kf(0, scales[si], lo, hi); zeile("KF0", scales[si], lo, hi); }
    for (si=0; si<3; si++) { extents_kf(1, scales[si], lo, hi); zeile("KF1", scales[si], lo, hi); }
    for (si=0; si<3; si++) { extents_rest(scales[si], lo, hi); zeile("RESTPOSE", scales[si], lo, hi); }
    return 0;
}
