/* probe_dog_victim_pose.c — MESSUNG (kein Fix): Leons OPFER-POSE beim HUNDE-Angriff.
 *
 * NUTZER-REPORT (2026-08-21): "Wenn Leon von den Hunden angegriffen wird, verschwindet er
 * teilweise. Ausserdem bekommt er keine Blutwunde/Effekt vom Angriff."
 *
 * "Verschwindet teilweise" = RENDER. Gemessen wird deshalb NICHT der Zustandswert, sondern die
 * GERENDERTE Pose: exakt die Bank-Komposition, die platform/pc/main.c:5497-5518 aufbaut
 *   s_victim_skel = pl00_skel                      (Leons Knochen + Bind-Pose)
 *   s_victim_skel.keyframe_* = vb->skel_victim.*   (Keyframe-Pool der Greifer-Bank)
 *   p_anim = &vb->anim_victim ; clip_override = player->motion
 * und daraus die Weltposition JEDES Knochens (re15_skel_bone_to_world), samt Spannweite.
 *
 * Diskriminator: PSX-Y ist nach OBEN NEGATIV. Referenz "Leon steht" = PL00 Clip 0 Frame 0.
 * Ein Knochen, der in KEINEM Frame in der Naehe des Rumpfes liegt (|d| > 5000 vom Wurzelpunkt),
 * ist im Bild NICHT mehr an Leon -> genau das Symptom "verschwindet teilweise".
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_emd.h"
#include "re15_ems.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static re15_emd_animation_t s_pl00_anim;
static re15_emd_skeleton_t  s_pl00_skel;
static int load_pl00(void)
{
    size_t esz = 0, rsz = 0;
    uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &esz);
    uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &rsz);
    return (edd && emr &&
            re15_emd_parse_animation(edd, esz, &s_pl00_anim) == 0 &&
            re15_emd_parse_skeleton (emr, rsz, &s_pl00_skel) == 0);
}

static int load_re15_bank(uint8_t type, re15_enemy_bank_t *out)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        uint8_t *blob = (uint8_t *)malloc(len);
        memcpy(blob, ems + off, len);
        memset(out, 0, sizeof *out);
        if (re15_emd_parse_container(blob, len, &out->md1, &out->skel, &out->anim, NULL) == 0) {
            out->ok = 1;
            out->victim_ok = (re15_emd_parse_victim_bank(blob, len, &out->skel_victim,
                                                         &out->anim_victim) == 0);
            ok = 1;
        }
    }
    free(ems);
    return ok;
}

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_re2_bank(uint8_t type, re15_enemy_bank_t *out)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    memset(out, 0, sizeof *out);
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, out, NULL) != 0) return 0;
    out->ok = 1;
    return 1;
}

/* Der Renderpfad aus main.c:5497-5518, alle 15 PL00-Knochen. */
static int victim_bones(const re15_enemy_bank_t *vb, int clip, int frame,
                        int32_t out[RE15_EMD_MAX_BONES][3], int *out_fc)
{
    if (!vb->victim_ok || clip < 0 || clip >= vb->anim_victim.clip_count) return -1;
    int fc = vb->anim_victim.clips[clip].frame_count;
    if (out_fc) *out_fc = fc;
    if (fc <= 0) return -2;

    re15_emd_skeleton_t vs = s_pl00_skel;                    /* Leons Knochen + Bind-Pose  */
    vs.keyframe_data       = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count      = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;

    re15_actor_t probe;
    memset(&probe, 0, sizeof probe);
    probe.active = 1; probe.motion = (uint8_t)clip;
    probe.anim_frame = (uint16_t)(frame % fc);
    probe.rot_y = 0;

    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, &probe, clip, probe.anim_frame);
    if (kf < 0) return -3;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = NULL;
    if (re15_skel_compute_pose(&vs, kf, poses) != 0) return -4;
    for (int b = 0; b < s_pl00_skel.bone_count; b++)
        re15_skel_bone_to_world(poses[b].trans, 0, 0, 0, 0, out[b]);
    return 0;
}

/* Wie weit LAEUFT ein Knochen vom Wurzelpunkt weg? Ein menschliches Rig bleibt in jedem Frame
 * innerhalb weniger tausend Einheiten; alles darueber ist im Bild nicht mehr an Leon. */
static void clip_span(const re15_enemy_bank_t *vb, int clip, int *out_fc,
                      int *out_maxd, int *out_worstbone, int *out_worstframe,
                      int *out_heady_first, int *out_heady_last)
{
    int32_t bw[RE15_EMD_MAX_BONES][3];
    int fc = 0, maxd = -1, wb = -1, wf = -1;
    if (victim_bones(vb, clip, 0, bw, &fc) != 0) { *out_fc = fc; *out_maxd = -1; return; }
    *out_heady_first = bw[8][1];
    for (int f = 0; f < fc; f++) {
        if (victim_bones(vb, clip, f, bw, NULL) != 0) continue;
        for (int b = 0; b < s_pl00_skel.bone_count; b++) {
            int32_t dx = bw[b][0] - bw[0][0];
            int32_t dy = bw[b][1] - bw[0][1];
            int32_t dz = bw[b][2] - bw[0][2];
            int32_t d  = (dx < 0 ? -dx : dx) + (dy < 0 ? -dy : dy) + (dz < 0 ? -dz : dz);
            if (d > maxd) { maxd = d; wb = b; wf = f; }
        }
        if (f == fc - 1) *out_heady_last = bw[8][1];
    }
    *out_fc = fc; *out_maxd = maxd; *out_worstbone = wb; *out_worstframe = wf;
}

static void dump_bank(const char *tag, const re15_enemy_bank_t *vb)
{
    printf("\n== %s ==\n", tag);
    if (!vb->ok) { printf("  BANK FEHLT!\n"); return; }
    printf("  Haupt-Bank: %d Knochen, %d Clips, %d Meshes\n",
           vb->skel.bone_count, vb->anim.clip_count, vb->md1.mesh_count);
    if (!vb->victim_ok) { printf("  KEINE Victim-Bank!\n"); return; }
    printf("  Victim-Bank: %d Clips, Struktur %d Knochen, Pool %d Keyframes a %d Bytes\n",
           vb->anim_victim.clip_count, vb->skel_victim.bone_count,
           vb->skel_victim.keyframe_count, vb->skel_victim.keyframe_size_bytes);
    printf("  %-5s %-5s %-12s %-12s %-30s\n", "Clip", "fc", "Kopf-y F0", "Kopf-y Ende",
           "max |Knochen-Wurzel| (Bone@Frame)");
    for (int c = 0; c < vb->anim_victim.clip_count; c++) {
        int fc = 0, maxd = -1, wb = -1, wf = -1, h0 = 0, h1 = 0;
        clip_span(vb, c, &fc, &maxd, &wb, &wf, &h0, &h1);
        if (maxd < 0) { printf("  0x%02X  %-5d (nicht posierbar)\n", c, fc); continue; }
        printf("  0x%02X  %-5d %-12d %-12d %6d (Bone %2d @F%d) %s\n",
               c, fc, h0, h1, maxd, wb, wf,
               (maxd > 5000) ? "<== KNOCHEN AUSSER RIG" :
                 ((h1 > -600) ? "(Kopf am Boden)" : ""));
    }
}

int main(void)
{
    printf("== Leons Opfer-Pose beim HUNDE-Angriff: RE1.5-Bank gegen RE2-Bank ==\n");
    if (!load_pl00()) { printf("FAIL: PL00.EDD/EMR nicht ladbar — Messung wertlos\n"); return 1; }
    printf("PL00: %d Knochen, %d Keyframes a %d Bytes, %d Clips\n",
           s_pl00_skel.bone_count, s_pl00_skel.keyframe_count,
           s_pl00_skel.keyframe_size_bytes, s_pl00_anim.clip_count);
    {
        re15_actor_t p; memset(&p, 0, sizeof p);
        p.active = 1; p.motion = 0; p.anim_frame = 0;
        int kf = re15_compute_actor_kf(&s_pl00_anim, &s_pl00_skel, &p, 0, 0);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        if (kf >= 0 && re15_skel_compute_pose(&s_pl00_skel, kf, poses) == 0) {
            int32_t hw[3], rw[3]; int maxd = 0;
            re15_skel_bone_to_world(poses[8].trans, 0, 0, 0, 0, hw);
            re15_skel_bone_to_world(poses[0].trans, 0, 0, 0, 0, rw);
            for (int b = 0; b < s_pl00_skel.bone_count; b++) {
                int32_t w[3]; re15_skel_bone_to_world(poses[b].trans, 0, 0, 0, 0, w);
                int32_t d = abs(w[0]-rw[0]) + abs(w[1]-rw[1]) + abs(w[2]-rw[2]);
                if (d > maxd) maxd = d;
            }
            printf("REFERENZ Leon STEHEND (PL00 Clip 0 F0): Kopf y=%d, max |Knochen-Wurzel|=%d\n",
                   hw[1], maxd);
        }
    }

    static re15_enemy_bank_t d15, d2;
    if (load_re15_bank(0x20, &d15)) dump_bank("RE1.5 EM020 (Cerberus)", &d15);
    else printf("\nRE1.5-Bank EM020 FEHLT\n");
    if (load_re2_bank(0x20, &d2))   dump_bank("RE2 EM_TYPE20 (Cerberus)", &d2);
    else printf("\nRE2-Bank EM020 FEHLT\n");

    /* ---- A/B: der BLUT-ANKER (Spieler-Bone 8) ------------------------------------------
     * Der Renderer posiert Leon mit PL00-Hierarchie + Victim-Pool (main.c:5503-5510).
     * re15_player_victim_bone_pos (enemy_ai_common.c:822) posiert dagegen mit
     * vb->skel_victim — also der HIERARCHIE DES GREIFERS (dir[2]-EMR) und nur denselben
     * Keyframes. Differenz = Fehlplatzierung des Fress-/Release-Bluts. */
    printf("\n== A/B Blut-Anker (Spieler-Bone 8) — Renderer-Rig vs. Greifer-Rig ==\n");
    for (int bank = 0; bank < 2; bank++) {
        re15_enemy_bank_t *vb = bank ? &d2 : &d15;
        const char *tag = bank ? "RE2 EM_TYPE20" : "RE1.5 EM020";
        if (!vb->victim_ok) { printf("  %s: keine Victim-Bank\n", tag); continue; }
        int clip = bank ? 0 : 4;                        /* Kollaps-Clip je Modus */
        if (clip >= vb->anim_victim.clip_count) clip = 0;
        int fc = vb->anim_victim.clips[clip].frame_count;
        int fr = (0x29 < fc) ? 0x29 : fc - 1;           /* Blut-Frame @0x80111e30 */
        int32_t bwA[RE15_EMD_MAX_BONES][3];
        if (victim_bones(vb, clip, fr, bwA, NULL) != 0) { printf("  %s: nicht posierbar\n", tag); continue; }
        /* (B) mit der GREIFER-Hierarchie, exakt wie re15_player_victim_bone_pos */
        re15_actor_t probe; memset(&probe, 0, sizeof probe);
        probe.active = 1; probe.motion = (uint8_t)clip; probe.anim_frame = (uint16_t)fr;
        int kf = re15_compute_actor_kf(&vb->anim_victim, &vb->skel_victim, &probe, -1, (uint32_t)fr);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        int32_t bwB[3] = {0,0,0};
        if (re15_skel_compute_pose(&vb->skel_victim, kf, poses) == 0)
            re15_skel_bone_to_world(poses[8].trans, 0, 0, 0, 0, bwB);
        int32_t dx = bwA[8][0]-bwB[0], dy = bwA[8][1]-bwB[1], dz = bwA[8][2]-bwB[2];
        printf("  %-14s Clip 0x%02X F%d: Renderer-Rig (%6d,%6d,%6d) | Greifer-Rig "
               "(%6d,%6d,%6d) | Abweichung %d\n", tag, clip, fr,
               bwA[8][0], bwA[8][1], bwA[8][2], bwB[0], bwB[1], bwB[2],
               (dx<0?-dx:dx)+(dy<0?-dy:dy)+(dz<0?-dz:dz));
    }

    printf("\n== Port-Clipwahl (re15_victim_clip_map, Hunde-Zweig) ==\n");
    printf("  RE1.5: intro=3*v hold=1 release=2 collapse=4  (Bank hat %d Clips)\n",
           d15.victim_ok ? d15.anim_victim.clip_count : -1);
    printf("  RE2:   intro=0 hold=0 release=0 collapse=0    (Bank hat %d Clips)\n",
           d2.victim_ok ? d2.anim_victim.clip_count : -1);
    return 0;
}
