/* Scratch probe: drawn bottom of the RE2-hybrid adult spider (RE1.5 EM025 mesh/bind + RE2 keyframes), per clip. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re15_ems.h"
#include "re15_tim.h"
#include "re2_ems.h"
static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static re15_md1_t md15; static re15_emd_skeleton_t sk15; static re15_emd_animation_t an15;
int main(void)
{
    size_t n2 = 0, n15 = 0;
    uint8_t *ems2 = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &n2);
    uint8_t *ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n15);
    if (!ems2 || !ems15) { printf("EMS fehlt\n"); return 1; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x25);
    if (re2_ems_load_bank(ems2, n2, 0x25, eb, NULL) != 0) { printf("RE2 load fail\n"); return 1; }
    eb->ok = 1;
    int idx = re15_ems_index_for_type(0x25); size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(ems15, n15, idx, &off, &len) != 0) { printf("RE1.5 entry fail\n"); return 1; }
    uint8_t *blob = (uint8_t *)malloc(len); memcpy(blob, ems15 + off, len);
    re15_tim_t tim; memset(&tim, 0, sizeof tim);
    if (re15_emd_parse_container(blob, len, &md15, &sk15, &an15, &tim) != 0) { printf("RE1.5 parse fail\n"); return 1; }
    int um = -1;
    int rc = re2_hybrid_apply(eb, 0x25, &md15, &sk15, &um);
    printf("hybrid rc %d unmapped %d bones %d meshes %d clips %d rootfix %p\n", rc, um, eb->skel.bone_count, eb->md1.mesh_count,
           eb->anim.clip_count, (void *)eb->skel.root_y_fix);
    for (int c = 0; c < eb->anim.clip_count; c++) {
        int32_t lo = 1 << 30, hi = -(1 << 30);
        for (int f = 0; f < eb->anim.clips[c].frame_count; f++) {
            int kf = (int)(eb->anim.frames[eb->anim.clips[c].first_frame + f] & 0xfffu);
            re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
            g_anim_pose_actor = NULL;
            if (re15_skel_compute_pose(&eb->skel, kf, poses) != 0) continue;
            int32_t tief = -(1 << 30);
            for (int m = 0; m < eb->md1.mesh_count; m++) {
                const re15_skel_pose_t *p = &poses[m < eb->skel.bone_count ? m : 0];
                const re15_md1_mesh_t *me = &eb->md1.meshes[m];
                for (int pass = 0; pass < 2; pass++) {
                    const re15_md1_vertex_t *vs = pass ? me->quad_vertices : me->tri_vertices;
                    int nv = pass ? me->quad_vertex_count : me->tri_vertex_count;
                    for (int i = 0; i < nv; i++) {
                        int32_t y = (int32_t)(((int64_t)p->rot[3] * vs[i].x + (int64_t)p->rot[4] * vs[i].y + (int64_t)p->rot[5] * vs[i].z) >> 12) + p->trans[1];
                        if (y > tief) tief = y;
                    }
                }
            }
            if (tief < lo) lo = tief; if (tief > hi) hi = tief;
        }
        printf("  clip %2d (%3d Bilder): gezeichnete Unterkante %d .. %d\n", c, eb->anim.clips[c].frame_count, lo, hi);
    }
    return 0;
}
