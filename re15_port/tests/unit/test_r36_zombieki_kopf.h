/*
 * Runde 36 Spur F "zombieki", Nachbesserung 2 (Abnahme 1, Mangel 5) — Leons Kopfhoehe fuer die Sonden AUS DEN DATEN.
 *
 * Das Spiel liest Leons Kopf fuer den RE2-Sichtstrahl (re15_r36_zombie_sicht_frei) nur aus head_world: der Plc_neck-
 * Ursprung, den skeleton_common.c bei JEDEM gezeichneten Bild aus Leons Pose setzt (hm = Wurzel-Rot * relpos[neck_bone]
 * + Wurzel-Trans, neck_bone = 8 @0x80031938 actor_common.c). Die Sonden zeichnen nicht — sie setzen head_world hier
 * mit DERSELBEN Formel aus Leons Skelett/Animation (PLD/PL00.EMR + PLD/PL00.EDD, Clip RE15_R36_KOPF_CLIP Bild 0) statt
 * einer eingetragenen Zahl (vor NB2: RE15_R36_LEON_KOPF_Y = -2514 "GEMESSEN", ohne Beleg).
 */
#ifndef TEST_R36_ZOMBIEKI_KOPF_H
#define TEST_R36_ZOMBIEKI_KOPF_H

#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re15_actor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_R36_KOPF_CLIP
#define RE15_R36_KOPF_CLIP 0
#endif

static re15_emd_animation_t s_kopf_anim;
static re15_emd_skeleton_t  s_kopf_skel;
static int     s_kopf_ok = -1;
static int32_t s_kopf_hm[3];

static uint8_t *kopf_slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* Kopf (Knochen 8) im Modellraum fuer Clip `clip` Bild `bild` — die Formel aus skeleton_common.c (Plc_neck-Ursprung). */
static int kopf_modell(int clip, int bild, int32_t hm[3])
{
    if (clip < 0 || clip >= s_kopf_anim.clip_count) return -1;
    re15_actor_t a; memset(&a, 0, sizeof a);
    a.active = 1; a.motion = (uint8_t)clip; a.anim_frame = (uint16_t)bild;
    int kf = re15_compute_actor_kf(&s_kopf_anim, &s_kopf_skel, &a, clip, (uint16_t)bild);
    if (kf < 0) return -2;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = NULL;
    if (re15_skel_compute_pose(&s_kopf_skel, kf, poses) != 0) return -3;
    const int b = 8;                                           /* neck_bone = 8 (@0x80031938, actor_common.c) */
    int32_t rp[3] = { s_kopf_skel.bone_relative_pos[b][0], s_kopf_skel.bone_relative_pos[b][1], s_kopf_skel.bone_relative_pos[b][2] };
    for (int r = 0; r < 3; r++)
        hm[r] = ((poses[0].rot[r * 3 + 0] * rp[0] + poses[0].rot[r * 3 + 1] * rp[1] + poses[0].rot[r * 3 + 2] * rp[2]) >> 12) + poses[0].trans[r];
    return 0;
}

static int kopf_laden(void)
{
    if (s_kopf_ok >= 0) return s_kopf_ok;
    size_t esz = 0, rsz = 0;
    uint8_t *edd = kopf_slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = kopf_slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    s_kopf_ok = (edd && emr && re15_emd_parse_animation(edd, esz, &s_kopf_anim) == 0 &&
                 re15_emd_parse_skeleton(emr, rsz, &s_kopf_skel) == 0 &&
                 kopf_modell(RE15_R36_KOPF_CLIP, 0, s_kopf_hm) == 0) ? 1 : 0;
    return s_kopf_ok;
}

/* head_world des Spielers wie skeleton_common.c (headWorld = actorPos + Ryaw(rot_y) * hm). */
static void kopf_setzen(re15_actor_t *pl)
{
    if (!kopf_laden()) { pl->head_world_ok = 0; return; }
    int32_t cs = re15_cos_q12((int)pl->rot_y), sn = re15_sin_q12((int)pl->rot_y);
    pl->head_world[0] = pl->x + ((cs * s_kopf_hm[0] + sn * s_kopf_hm[2]) >> 12);
    pl->head_world[1] = pl->y + s_kopf_hm[1];
    pl->head_world[2] = pl->z + ((-sn * s_kopf_hm[0] + cs * s_kopf_hm[2]) >> 12);
    pl->head_world_ok = 1;
}

#endif /* TEST_R36_ZOMBIEKI_KOPF_H */
