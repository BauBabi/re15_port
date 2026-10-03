/* probe_r35_raeume_arme.c — MESSUNG (kein add_test): Runde 35 Spur H, Punkt 4.
 * Wo liegt die Hand des RE2-Zellenarms EM2D (Bone 3 = Arm A, 10 = Arm B, Tabelle @0x80101414)
 * in Clip 3 (Zugriff) Bild k und in Clip 5 (Halten) Bild 0..18 — relativ zum Arm-Ursprung?
 * Das Original pinnt Leon bei B4 P0 an part[Hand]+0x5C/+0x64 (@0x80100C18-38) aus der Pose des
 * VORIGEN Takts (die Part-Matrizen baut erst 0x8002959C -> 0x80029614); der Port rechnete die
 * Hand aus dem schon gesetzten Clip 5 Bild 0. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_damage.h"
#include "re2_ems.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

int main(void)
{
    long sz = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &sz);
    if (!ems) { printf("FAIL: RE2/CDEMD0.EMS\n"); return 1; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x1A);
    if (!eb || re2_ems_load_bank(ems, (size_t)sz, 0x2D, eb, NULL) != 0) { printf("FAIL: EM2D\n"); return 1; }
    eb->buf = NULL; eb->ok = 1;
    printf("EM2D: %d Bones, %d Clips, Opfer %d Clips\n", eb->skel.bone_count, eb->anim.clip_count,
           eb->victim_ok ? eb->anim_victim.clip_count : 0);
    for (int c = 0; c < eb->anim.clip_count; c++)
        printf("  Clip %d: %d Bilder\n", c, eb->anim.clips[c].frame_count);
    if (eb->victim_ok)
        for (int c = 0; c < eb->anim_victim.clip_count; c++)
            printf("  Opfer-Clip %d: %d Bilder\n", c, eb->anim_victim.clips[c].frame_count);
    re15_actor_t *e = &g_actors[5];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x1A; e->x = 0; e->y = -2500; e->z = 0; e->rot_y = 0;
    for (int bone = 3; bone <= 10; bone += 7) {
        printf("Hand-Bone %d:\n", bone);
        for (int clip = 3; clip <= 5; clip += 2) {
            e->motion = (int16_t)clip;
            int fc = eb->anim.clips[clip].frame_count;
            for (int f = 0; f < fc; f++) {
                e->anim_frame = (uint16_t)f;
                int32_t h[3];
                re15_enemy_bone_world_pos(e, bone, h);
                printf("  clip %d f%2d hand=(%5d,%5d,%5d)\n", clip, f, h[0], h[1], h[2]);
            }
        }
    }
    /* ---- Leon im Opfer-Clip 0 (PL00-Knochen + EM2D-Paar-3-Keyframes, Renderpfad main.c:8550-8566) */
    {
        long esz = 0, rsz = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
        static re15_emd_animation_t pa; static re15_emd_skeleton_t ps;
        if (!edd || !emr || re15_emd_parse_animation(edd, (size_t)esz, &pa) != 0 ||
            re15_emd_parse_skeleton(emr, (size_t)rsz, &ps) != 0) { printf("FAIL: PL00\n"); return 1; }
        re15_emd_skeleton_t vs = ps;
        vs.keyframe_data = eb->skel_victim.keyframe_data;
        vs.keyframe_data_size = eb->skel_victim.keyframe_data_size;
        vs.keyframe_count = eb->skel_victim.keyframe_count;
        vs.keyframe_size_bytes = eb->skel_victim.keyframe_size_bytes;
        const int fcv = eb->anim_victim.clips[0].frame_count;
        const int fca = eb->anim.clips[5].frame_count;
        int32_t leon[32][RE15_EMD_MAX_BONES][3];
        int32_t hand[32][3];
        /* Arm A (Bone 3), Ursprung (0,-2500,0), Blick +x; Leon an der Hand von Clip 3 Bild 9 */
        e->motion = 3; e->anim_frame = 9;
        int32_t pin[3]; re15_enemy_bone_world_pos(e, 3, pin);
        for (int yawcase = 0; yawcase < 2; yawcase++) {
            int16_t lyaw = yawcase ? 2048 : 0;      /* 0 = Ruecken zum Fenster (same), 2048 = Gesicht */
            for (int f = 0; f < fcv && f < 32; f++) {
                re15_actor_t pr; memset(&pr, 0, sizeof pr);
                pr.active = 1; pr.motion = 0; pr.anim_frame = (uint16_t)f; pr.rot_y = lyaw;
                int kf = re15_compute_actor_kf(&eb->anim_victim, &vs, &pr, 0, pr.anim_frame);
                re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
                g_anim_pose_actor = NULL;
                if (kf < 0 || re15_skel_compute_pose(&vs, kf, poses) != 0) { printf("FAIL pose\n"); return 1; }
                for (int b = 0; b < ps.bone_count; b++)
                    re15_skel_bone_to_world(poses[b].trans, lyaw, pin[0], 0, pin[2], leon[f][b]);
            }
            for (int f = 0; f < fca && f < 32; f++) {
                e->motion = 5; e->anim_frame = (uint16_t)f;
                re15_enemy_bone_world_pos(e, 3, hand[f]);
            }
            printf("Leon-Blick %d, Pin (%d,%d): Leon-Knochen 0/8 Bild 0 = (%d,%d,%d)/(%d,%d,%d)\n", lyaw,
                   pin[0], pin[2], leon[0][0][0], leon[0][0][1], leon[0][0][2],
                   leon[0][8][0], leon[0][8][1], leon[0][8][2]);
            for (int b = 0; b < ps.bone_count; b++) {
                int bestd = -1; long best = -1; long d1 = 0;
                for (int d = 0; d < fcv; d++) {
                    long sum = 0;
                    for (int f = 0; f < fca; f++) {
                        const int32_t *L = leon[(f + d) % fcv][b];
                        long dx = L[0] - hand[f][0], dy = L[1] - hand[f][1], dz = L[2] - hand[f][2];
                        /* Bewegungs-Gleichlauf: Abweichung der VERSCHIEBUNG gegen Bild 0 */
                        const int32_t *L0 = leon[d % fcv][b];
                        long mx = (L[0]-L0[0]) - (hand[f][0]-hand[0][0]);
                        long mz = (L[2]-L0[2]) - (hand[f][2]-hand[0][2]);
                        (void)dx; (void)dy; (void)dz;
                        sum += mx*mx + mz*mz;
                    }
                    if (d == 1) d1 = sum;
                    if (best < 0 || sum < best) { best = sum; bestd = d; }
                }
                printf("  Leon-Bone %2d: bester Versatz %2d (rms %ld), Versatz 1 rms %ld\n", b, bestd,
                       (long)(best >= 0 ? (long)__builtin_sqrt((double)best / fca) : -1),
                       (long)__builtin_sqrt((double)d1 / fca));
            }
        }
        for (int b = 0; b < ps.bone_count; b++)
            printf("  Leon-Bone %2d Bild 0/6/13 (Blick 2048): (%5d,%5d,%5d) (%5d,%5d,%5d) (%5d,%5d,%5d)\n", b,
                   leon[0][b][0], leon[0][b][1], leon[0][b][2], leon[6][b][0], leon[6][b][1], leon[6][b][2],
                   leon[13][b][0], leon[13][b][1], leon[13][b][2]);
        for (int f = 0; f < fcv; f++)
            printf("  f%2d hand=(%5d,%5d,%5d) leon8=(%5d,%5d,%5d) leon0=(%5d,%5d,%5d)\n", f,
                   hand[f % fca][0], hand[f % fca][1], hand[f % fca][2], leon[f][8][0], leon[f][8][1],
                   leon[f][8][2], leon[f][0][0], leon[f][0][1], leon[f][0][2]);
    }
    return 0;
}
