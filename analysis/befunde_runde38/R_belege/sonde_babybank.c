/* Exploratory probe (scratch, not committed): lowest/highest model-space vertex y of RE2 EM025/EM026 per keyframe. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_skeleton.h"
#include "re15_emd.h"
#include "re15_md1.h"
#include "re2_ems.h"
static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}
static void mess(re15_enemy_bank_t *eb, const char *name)
{
    printf("== %s: bones %d meshes %d keyframes %d clips %d\n", name, eb->skel.bone_count, eb->md1.mesh_count,
           eb->skel.keyframe_count, eb->anim.clip_count);
    for (int b = 0; b < eb->skel.bone_count && b < 4; b++)
        printf("  bind[%d] rel(%d,%d,%d) parent %d mesh %d\n", b, eb->skel.bone_relative_pos[b][0],
               eb->skel.bone_relative_pos[b][1], eb->skel.bone_relative_pos[b][2], eb->skel.bone_parent[b],
               eb->skel.bone_mesh_index[b]);
    int nkf = eb->skel.keyframe_count;
    printf("  keyframe_size %d data_size %d\n", eb->skel.keyframe_size_bytes, (int)eb->skel.keyframe_data_size);
    for (int kf = 0; kf < 3; kf++) {
        const uint8_t *k = eb->skel.keyframe_data + (size_t)kf * (size_t)eb->skel.keyframe_size_bytes;
        printf("  raw kf%d:", kf);
        for (int i = 0; i < 24 && i < eb->skel.keyframe_size_bytes; i++) printf(" %02x", k[i]);
        printf("\n");
    }
    for (int kf = 0; kf < nkf; kf++) {
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        if (re15_skel_compute_pose(&eb->skel, kf, poses) != 0) { printf("kf %d pose fail\n", kf); continue; }
        int32_t ymin = 1 << 30, ymax = -(1 << 30);
        int nb = eb->skel.bone_count;
        for (int m = 0; m < eb->md1.mesh_count; m++) {
            const re15_skel_pose_t *p = &poses[m < nb ? m : 0];
            const re15_md1_mesh_t *me = &eb->md1.meshes[m];
            for (int pass = 0; pass < 2; pass++) {
                const re15_md1_vertex_t *vs = pass ? me->quad_vertices : me->tri_vertices;
                int nv = pass ? me->quad_vertex_count : me->tri_vertex_count;
                for (int i = 0; i < nv; i++) {
                    int64_t y = ((int64_t)p->rot[3] * vs[i].x + (int64_t)p->rot[4] * vs[i].y + (int64_t)p->rot[5] * vs[i].z) >> 12;
                    int32_t yy = (int32_t)y + p->trans[1];
                    if (yy < ymin) ymin = yy; if (yy > ymax) ymax = yy;
                }
            }
        }
        if (kf < 400 && (kf % 1 == 0)) printf("  kf %3d root(%d,%d,%d) vertex y [%d .. %d] (y-down: max = tiefster Punkt)\n", kf,
               poses[0].trans[0], poses[0].trans[1], poses[0].trans[2], ymin, ymax);
    }
}
int main(void)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &n);
    if (!ems) { printf("RE2 CDEMD0 fehlt\n"); return 1; }
    for (int t = 0x25; t <= 0x26; t++) {
        re15_enemy_bank_t *eb = re15_enemy_alloc((uint8_t)t);
        if (!eb || re2_ems_load_bank(ems, n, t, eb, NULL) != 0) { printf("load %x fail\n", t); continue; }
        eb->ok = 1;
        mess(eb, t == 0x25 ? "RE2 EM025" : "RE2 EM026");
        printf("  pairs ok: loco %d own %d victim %d; clips loco %d own %d victim %d; kf loco %d own %d victim %d\n",
               eb->loco_ok, eb->own_ok, eb->victim_ok, eb->anim_loco.clip_count, eb->anim_own.clip_count,
               eb->anim_victim.clip_count, eb->skel_loco.keyframe_count, eb->skel_own.keyframe_count,
               eb->skel_victim.keyframe_count);
        if (t == 0x26) {
            re15_enemy_bank_t tmp = *eb;
            if (eb->loco_ok) { tmp.skel = eb->skel_loco; tmp.anim = eb->anim_loco; mess(&tmp, "EM026 PAAR 1 (loco, +0x17C/+0x108)");
                for (int c = 0; c < eb->anim_loco.clip_count; c++) {
                    printf("  PAAR1 clip %d: first %d frames %d ->", c, eb->anim_loco.clips[c].first_frame, eb->anim_loco.clips[c].frame_count);
                    for (int f = 0; f < eb->anim_loco.clips[c].frame_count && f < 40; f++)
                        printf(" %u", (unsigned)(eb->anim_loco.frames[eb->anim_loco.clips[c].first_frame + f] & 0xfffu));
                    printf("\n");
                }
                for (int c = 0; c < eb->anim.clip_count; c++) {
                    printf("  PAAR3 clip %d: first %d frames %d ->", c, eb->anim.clips[c].first_frame, eb->anim.clips[c].frame_count);
                    for (int f = 0; f < eb->anim.clips[c].frame_count && f < 40; f++)
                        printf(" %u", (unsigned)(eb->anim.frames[eb->anim.clips[c].first_frame + f] & 0xfffu));
                    printf("\n");
                }
            }
            if (eb->own_ok)  { tmp.skel = eb->skel_own;  tmp.anim = eb->anim_own;  mess(&tmp, "EM026 PAAR 2 (own)"); }
            if (eb->victim_ok) { tmp.skel = eb->skel_victim; tmp.anim = eb->anim_victim; mess(&tmp, "EM026 PAAR 3 (victim)"); }
        }
        for (int c = 0; c < eb->anim.clip_count; c++)
            printf("  clip %d: start %d frames %d\n", c, eb->anim.clips[c].first_frame, eb->anim.clips[c].frame_count);
    }
    return 0;
}
