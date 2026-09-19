/* probe_r16_sk_gator_finisher_sounds.c - GEGEN-SONDE des Skeptikers (Runde 16, 2026-09-19).
 * Kein add_test. Prueft die Datenbehauptungen des Dossiers gator-finisher-sounds.md:
 *   (1) "Opfer-Clip 1 (kf 30..149) POSE x/z = 0 in ALLEN kf, SPEED y = 0 in allen Gator-
 *       und Opfer-Frames" - je Frame gelesen (RE2 EM23.EMD Opferbank, Engine-Parser).
 *   (2) Fix-Plan A Schritt 3 laesst den Term s*(off_v.y - off_g.y) weg (Begruendung
 *       "SPEED y = 0"). Gegenrechnung: Fix-Formel vs. S-Variante vs. Original-Referenz
 *       fuer ALLE 120 P2-Frames + P3, nicht nur 6 Stichproben.
 *   (3) "6=1/7=1 in jedem Frame, in dem das Original sie hat" - Vergleich der
 *       Kiefer-AABB-Treffer Original vs. S-Variante ueber alle 120 Frames.
 * Metrik identisch zu probe_r16_gator_finisher_anker.c (leon_local / mesh_local).
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static uint8_t *slurp(const char *path, size_t *n)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); *n = (size_t)sz;
    return b;
}

static re15_enemy_bank_t B;

static int kf_of(const re15_emd_animation_t *an, int clip, int fr)
{
    const re15_emd_clip_t *c = &an->clips[clip];
    int slot = fr % c->frame_count;
    int fi = c->first_frame + slot;
    int fend = c->first_frame + c->frame_count - 1;
    while ((an->frames[fi] & 0x8000u) && fi < fend) fi++;
    return (int)(an->frames[fi] & 0xFFFu);
}
static void spd(const re15_emd_skeleton_t *sk, int kf, int32_t o[3])
{ int16_t x=0,y=0,z=0; re15_emd_get_keyframe_speed(sk, kf, &x,&y,&z); o[0]=x;o[1]=y;o[2]=z; }
static void pos(const re15_emd_skeleton_t *sk, int kf, int32_t o[3])
{ int16_t x=0,y=0,z=0; re15_emd_get_keyframe_position(sk, kf, &x,&y,&z); o[0]=x;o[1]=y;o[2]=z; }

static int mesh_local(int kf, int mesh, int32_t lo[3], int32_t hi[3])
{
    static re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    const re15_md1_mesh_t *m;
    int pass, i, r, k, n = 0;
    void *sv = g_anim_pose_actor; g_anim_pose_actor = NULL;
    r = re15_skel_compute_pose(&B.skel, kf, poses);
    g_anim_pose_actor = sv;
    if (r != 0) return -1;
    m = &B.md1.meshes[mesh];
    lo[0]=lo[1]=lo[2]=0x7fffffff; hi[0]=hi[1]=hi[2]=-0x7fffffff;
    for (pass = 0; pass < 2; pass++) {
        const re15_md1_vertex_t *V = pass ? m->quad_vertices : m->tri_vertices;
        int NV = pass ? m->quad_vertex_count : m->tri_vertex_count;
        for (i = 0; i < NV; i++) {
            int32_t v[3] = { V[i].x, V[i].y, V[i].z }, w[3];
            for (r = 0; r < 3; r++) {
                int64_t s = 0;
                for (k = 0; k < 3; k++) s += (int64_t)poses[mesh].rot[r*3+k] * v[k];
                w[r] = (int32_t)(s >> 12) + poses[mesh].trans[r];
            }
            for (r = 0; r < 3; r++) { if (w[r] < lo[r]) lo[r] = w[r]; if (w[r] > hi[r]) hi[r] = w[r]; }
            n++;
        }
    }
    return n ? 0 : -3;
}
static int inside(const int32_t p[3], const int32_t lo[3], const int32_t hi[3])
{ return p[0]>=lo[0]&&p[0]<=hi[0]&&p[1]>=lo[1]&&p[1]<=hi[1]&&p[2]>=lo[2]&&p[2]<=hi[2]; }

static void leon_local(const re15_actor_t *e, const re15_actor_t *pl, int vkf, int32_t out[3])
{
    int32_t P[3], w[3], d[3], cs, sn;
    int32_t s = e->render_scale_q12 ? e->render_scale_q12 : 4096;
    pos(&B.skel_victim, vkf, P);
    re15_skel_bone_to_world(P, (int16_t)pl->rot_y, pl->x, pl->y, pl->z, w);
    d[0] = w[0]-e->x; d[1] = w[1]-e->y; d[2] = w[2]-e->z;
    cs = re15_cos_q12((int)e->rot_y); sn = re15_sin_q12((int)e->rot_y);
    out[0] = (int32_t)(((int64_t)cs*d[0] - (int64_t)sn*d[2]) >> 12);
    out[2] = (int32_t)(((int64_t)sn*d[0] + (int64_t)cs*d[2]) >> 12);
    out[1] = d[1];
    out[0] = (int32_t)((int64_t)out[0]*4096/s);
    out[1] = (int32_t)((int64_t)out[1]*4096/s);
    out[2] = (int32_t)((int64_t)out[2]*4096/s);
}
static void ref_local(int gclip, int gfr, int vclip, int vfr, int32_t out[3])
{
    int32_t og[3], ov[3], P[3];
    spd(&B.skel, kf_of(&B.anim, gclip, gfr), og);
    spd(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), ov);
    pos(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), P);
    out[0] = ov[0]-og[0]+P[0]; out[1] = ov[1]-og[1]+P[1]; out[2] = ov[2]-og[2]+P[2];
}

/* Varianten: 'O' Original (s=4096, y-Leitung), 'S' Dossier-Sonde (mit s*(ov-og)-Term),
 * 'F' Fix-Plan A Schritt 3 (OHNE den Term: pl->y = e->y - POSy*(4096-s)>>12) */
static void place(re15_actor_t *e, re15_actor_t *pl, int scale, int gclip, int gfr,
                  int vclip, int vfr, char variant)
{
    int32_t ov[3], og[3], P[3], ux, uz;
    memset(e, 0, sizeof *e); memset(pl, 0, sizeof *pl);
    e->type = 0x23; e->render_scale_q12 = (int16_t)(scale == 4096 ? 0 : scale);
    e->x = -5608; e->y = -1200; e->z = -22446; e->rot_y = 113;
    e->motion = 4; e->anim_frame = 13;
    re15_clip_anchor_set_pub(e, &B.skel, &B.anim, 4, 13);
    pl->anchor_x = e->anchor_x; pl->anchor_z = e->anchor_z; pl->rot_y = e->rot_y;
    e->motion = (uint8_t)gclip; e->anim_frame = (uint32_t)gfr;
    re15_clip_root_motion_abs_pub(e, &B.skel, &B.anim, gclip, gfr);
    pl->motion = (uint8_t)vclip; pl->anim_frame = (uint32_t)vfr;
    re15_clip_root_motion_abs_pub(pl, &B.skel_victim, &B.anim_victim, vclip, vfr);
    ux = pl->x; uz = pl->z;
    spd(&B.skel, kf_of(&B.anim, gclip, gfr), og);
    spd(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), ov);
    pos(&B.skel_victim, kf_of(&B.anim_victim, vclip, vfr), P);
    switch (variant) {
    case 'O': pl->y = e->y + (ov[1]-og[1]); break;
    case 'S': {
        int s = scale;
        pl->x = e->x + (int32_t)(((int64_t)(ux - e->x) * s) >> 12);
        pl->z = e->z + (int32_t)(((int64_t)(uz - e->z) * s) >> 12);
        pl->y = e->y + (int32_t)(((int64_t)(ov[1]-og[1]) * s) >> 12)
                     - (int32_t)(((int64_t)P[1] * (4096 - s)) >> 12);
        break; }
    case 'F': {
        int s = scale;
        pl->x = e->x + (int32_t)(((int64_t)(ux - e->x) * s) >> 12);
        pl->z = e->z + (int32_t)(((int64_t)(uz - e->z) * s) >> 12);
        pl->y = e->y - (int32_t)(((int64_t)P[1] * (4096 - s)) >> 12);
        break; }
    }
}

int main(void)
{
    size_t sz = 0;
    uint8_t *emd = slurp(RE15_ASSET_RE2_DIR "/EM23.EMD", &sz);
    int f, c, k;
    if (!emd) { printf("FAIL EM23.EMD\n"); return 1; }
    memset(&B, 0, sizeof B);
    if (re2_emd_parse_bank(emd, sz, &B) != 0) { printf("FAIL parse\n"); return 1; }
    B.ok = 1;
    printf("EM23.EMD %lu B gator clips=%d victim clips=%d kf_victim=%d kfsize=%d\n",
           (unsigned long)sz, B.anim.clip_count, B.anim_victim.clip_count,
           B.skel_victim.keyframe_count, B.skel_victim.keyframe_size_bytes);
    for (c = 0; c < B.anim_victim.clip_count; c++)
        printf("  victim clip %d: first_frame=%d frame_count=%d\n", c,
               B.anim_victim.clips[c].first_frame, B.anim_victim.clips[c].frame_count);

    /* (1) Zensus je Frame der Opfer-Clips 0/1 und der Gator-Clips 4/5/11 */
    printf("\n=== (1) Zensus Opfer-Clip 0/1: POSE x/z != 0 ? SPEED y != 0 ? (nur Verletzer gelistet) ===\n");
    for (c = 0; c < B.anim_victim.clip_count; c++) {
        int nzx = 0, nzz = 0, nsy = 0, nsx = 0, nsz = 0, n8000 = 0;
        int32_t P[3], S[3], symin = 0, symax = 0;
        for (f = 0; f < B.anim_victim.clips[c].frame_count; f++) {
            uint32_t w = B.anim_victim.frames[B.anim_victim.clips[c].first_frame + f];
            int kf = kf_of(&B.anim_victim, c, f);
            if (w & 0x8000u) n8000++;
            pos(&B.skel_victim, kf, P); spd(&B.skel_victim, kf, S);
            if (P[0]) nzx++; if (P[2]) nzz++; if (S[1]) nsy++; if (S[0]) nsx++; if (S[2]) nsz++;
            if (S[1] < symin) symin = S[1]; if (S[1] > symax) symax = S[1];
            if (P[0] || P[2] || S[1])
                printf("   clip%d f%3d kf%4d word=%08X POSE=(%d,%d,%d) SPEED=(%d,%d,%d)\n", c, f, kf, w,
                       P[0], P[1], P[2], S[0], S[1], S[2]);
        }
        printf("  OPFER clip %d (%d F): POSE x!=0: %d, POSE z!=0: %d, SPEED x!=0: %d, SPEED y!=0: %d [%d..%d], SPEED z!=0: %d, 0x8000-Woerter: %d\n",
               c, B.anim_victim.clips[c].frame_count, nzx, nzz, nsx, nsy, symin, symax, nsz, n8000);
    }
    {   static const int gc[3] = { 4, 5, 11 };
        for (k = 0; k < 3; k++) {
            int nsy = 0, npx = 0, npz = 0; int32_t P[3], S[3];
            c = gc[k];
            for (f = 0; f < B.anim.clips[c].frame_count; f++) {
                int kf = kf_of(&B.anim, c, f); pos(&B.skel, kf, P); spd(&B.skel, kf, S);
                if (S[1]) nsy++; if (P[0]) npx++; if (P[2]) npz++;
            }
            printf("  GATOR clip %d (%d F): SPEED y!=0: %d, POSE x!=0: %d, POSE z!=0: %d\n",
                   c, B.anim.clips[c].frame_count, nsy, npx, npz);
        }
    }

    /* (2)+(3) alle 120 P2-Frames + P3: O vs S vs F, Kiefer-Treffer */
    printf("\n=== (2)/(3) alle P2-Frames sf=0..119 + P3: |F-O|max, |S-O|max, Kiefer-Treffer O/S/F ===\n");
    {   int maxF[3] = {0,0,0}, maxS[3] = {0,0,0}, mism6 = 0, mism7 = 0, mismF6 = 0, mismF7 = 0;
        int n6o = 0, n7o = 0, n67o = 0;
        for (f = 0; f <= 120; f++) {
            int gclip = (f < 120) ? 5 : 11, gfr = (f < 120) ? f : 0, vfr = (f < 120) ? f : 119;
            int gkf = kf_of(&B.anim, gclip, gfr), vkf = kf_of(&B.anim_victim, 1, vfr);
            re15_actor_t e, pl; int32_t LO[3], LS[3], LF[3], lo6[3], hi6[3], lo7[3], hi7[3], ref[3];
            int o6, o7, s6, s7, f6, f7, r;
            ref_local(gclip, gfr, 1, vfr, ref);
            mesh_local(gkf, 6, lo6, hi6); mesh_local(gkf, 7, lo7, hi7);
            place(&e, &pl, 4096, gclip, gfr, 1, vfr, 'O'); leon_local(&e, &pl, vkf, LO);
            place(&e, &pl, 2731, gclip, gfr, 1, vfr, 'S'); leon_local(&e, &pl, vkf, LS);
            place(&e, &pl, 2731, gclip, gfr, 1, vfr, 'F'); leon_local(&e, &pl, vkf, LF);
            o6 = inside(LO, lo6, hi6); o7 = inside(LO, lo7, hi7);
            s6 = inside(LS, lo6, hi6); s7 = inside(LS, lo7, hi7);
            f6 = inside(LF, lo6, hi6); f7 = inside(LF, lo7, hi7);
            n6o += o6; n7o += o7; n67o += (o6 && o7);
            if (o6 != s6) mism6++; if (o7 != s7) mism7++;
            if (o6 != f6) mismF6++; if (o7 != f7) mismF7++;
            for (r = 0; r < 3; r++) {
                int d = abs(LF[r]-LO[r]); if (d > maxF[r]) maxF[r] = d;
                d = abs(LS[r]-LO[r]); if (d > maxS[r]) maxS[r] = d;
            }
            if (o6 != s6 || o7 != s7 || o6 != f6 || o7 != f7 || abs(LF[1]-LO[1]) > 5 || abs(LS[1]-LO[1]) > 5 || (f % 20) == 0 || f == 119 || f == 120)
                printf("   %s sf=%3d ref=(%6d,%6d,%6d) O=(%6d,%6d,%6d) %d/%d  S=(%6d,%6d,%6d) %d/%d  F=(%6d,%6d,%6d) %d/%d\n",
                       f < 120 ? "P2" : "P3", f, ref[0], ref[1], ref[2], LO[0], LO[1], LO[2], o6, o7,
                       LS[0], LS[1], LS[2], s6, s7, LF[0], LF[1], LF[2], f6, f7);
        }
        printf("  Original-Treffer ueber 121 Frames: 6=1: %d, 7=1: %d, 6=1&&7=1: %d\n", n6o, n7o, n67o);
        printf("  S vs O: max|dx|=%d max|dy|=%d max|dz|=%d, Kiefer-6-Abweichungen=%d, Kiefer-7-Abweichungen=%d\n",
               maxS[0], maxS[1], maxS[2], mism6, mism7);
        printf("  F(Fix-Plan, ohne s*(ov-og)) vs O: max|dx|=%d max|dy|=%d max|dz|=%d, Kiefer-6-Abweichungen=%d, Kiefer-7-Abweichungen=%d\n",
               maxF[0], maxF[1], maxF[2], mismF6, mismF7);
    }
    free(emd);
    return 0;
}
