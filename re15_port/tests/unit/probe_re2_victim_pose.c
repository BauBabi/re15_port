/* probe_re2_victim_pose.c — MESSUNG (kein Fix): Leons OPFER-POSE beim RE2-Zombie-Fressen.
 *
 * NUTZER-REPORT (2026-08-21): "Bei RE2 AI, wenn Leon gefressen wird, steht er noch komisch."
 *
 * "Steht komisch" = falsche POSE. Gemessen wird deshalb NICHT der Zustandswert, sondern die
 * GERENDERTE Pose: exakt die Bank-Komposition, die platform/pc/main.c:5460-5478 aufbaut
 *   s_victim_skel = pl00_skel                      (Leons Knochen + Bind-Pose)
 *   s_victim_skel.keyframe_* = vb->skel_victim.*   (Keyframe-Pool der Greifer-Bank, Paar 3)
 *   p_anim = &vb->anim_victim ; clip_override = player->motion
 * und daraus die Weltposition jedes Knochens (re15_skel_bone_to_world).
 *
 * Diskriminator STEHEND/LIEGEND: PSX-Y ist nach OBEN NEGATIV. Ein stehender Leon hat den Kopf
 * (PL00-Bone 8) rund 1400-1600 Einheiten UEBER dem Wurzelpunkt (y ~= -1450); ein am Boden
 * liegender Leon hat den Kopf nahe der Bodenebene (|y| klein).
 *
 * Die Clip-Wahl trifft `re15_victim_clip_map` (enemy_ai_common.c:709). Fuer die ZOMBIE-Familie
 * (default-Zweig) ist sie die RE1.5-Bank-Belegung:
 *     intro = v*3 | hold = v*3+1 | release = v*3+2 | collapse = v+6      (v = 0 vorn / 1 hinten)
 * Fuer den HUND 0x20 gibt es bereits einen expliziten RE2-Zweig (WELLE C). Fuer den ZOMBIE NICHT
 * — er faellt unter dem RE2-Flavor auf die RE1.5-Belegung zurueck, obwohl die RE2-Victim-Bank
 * (EM010 Paar 3) eine ANDERE Clip-Zahl hat. Genau das misst diese Sonde.
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

/* ---- PL00 (Leons eigenes Rig) ---------------------------------------------------------- */
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

/* ---- Baenke ----------------------------------------------------------------------------- */
static int load_re15_bank(uint8_t type, re15_enemy_bank_t *out)
{
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &n);
    if (!ems) return 0;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0; int ok = 0;
    if (idx >= 0 && re15_ems_get_entry(ems, n, idx, &off, &len) == 0) {
        uint8_t *blob = (uint8_t *)malloc(len);      /* resident: die Bank aliast hinein */
        memcpy(blob, ems + off, len);
        re15_tim_t tim = (re15_tim_t){0};
        memset(out, 0, sizeof *out);
        if (re15_emd_parse_container(blob, len, &out->md1, &out->skel, &out->anim, &tim) == 0) {
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

/* ---- Der Renderpfad aus main.c:5460 ------------------------------------------------------ */
static int victim_pose(const re15_enemy_bank_t *vb, int clip, uint32_t frame,
                       int32_t head_world[3], int32_t root_world[3], int *out_fc)
{
    if (!vb->victim_ok || clip < 0 || clip >= vb->anim_victim.clip_count) return -1;
    int fc = vb->anim_victim.clips[clip].frame_count;
    if (out_fc) *out_fc = fc;
    if (fc <= 0) return -2;

    re15_emd_skeleton_t vs = s_pl00_skel;                    /* Leons Knochen + Bind-Pose  */
    vs.keyframe_data       = vb->skel_victim.keyframe_data;  /* Posen der Greifer-Bank     */
    vs.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count      = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;

    re15_actor_t probe;
    memset(&probe, 0, sizeof probe);
    probe.active = 1; probe.motion = (uint8_t)clip; probe.anim_frame = (uint16_t)(frame % (uint32_t)fc);
    probe.rot_y = 0; probe.x = 0; probe.y = 0; probe.z = 0;

    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, &probe, clip, probe.anim_frame);
    if (kf < 0) return -3;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    g_anim_pose_actor = NULL;                                /* Pose-QUERY, kein Crossfade */
    if (re15_skel_compute_pose(&vs, kf, poses) != 0) return -4;
    re15_skel_bone_to_world(poses[8].trans, 0, 0, 0, 0, head_world);   /* PL00 Bone 8 = Kopf */
    re15_skel_bone_to_world(poses[0].trans, 0, 0, 0, 0, root_world);   /* Bone 0 = Wurzel    */
    return 0;
}

static void dump_bank(const char *tag, const re15_enemy_bank_t *vb)
{
    printf("\n== %s ==\n", tag);
    if (!vb->victim_ok) { printf("  KEINE Victim-Bank!\n"); return; }
    printf("  Victim-Bank: %d Clips, Pool %d Keyframes a %d Bytes\n",
           vb->anim_victim.clip_count, vb->skel_victim.keyframe_count,
           vb->skel_victim.keyframe_size_bytes);
    printf("  %-5s %-4s %-28s %-28s %s\n", "Clip", "fc", "Kopf(Bone8) Frame 0", "Kopf Mitte", "Kopf Ende");
    for (int c = 0; c < vb->anim_victim.clip_count; c++) {
        int fc = 0;
        int32_t h0[3] = {0,0,0}, h1[3] = {0,0,0}, h2[3] = {0,0,0}, r0[3];
        int e0 = victim_pose(vb, c, 0, h0, r0, &fc);
        if (e0 != 0) { printf("  %-5d %-4d  (nicht posierbar, err %d)\n", c, fc, e0); continue; }
        victim_pose(vb, c, (uint32_t)(fc / 2), h1, r0, NULL);
        victim_pose(vb, c, (uint32_t)(fc - 1), h2, r0, NULL);
        printf("  0x%02X  %-4d (%6d,%6d,%6d) (%6d,%6d,%6d) (%6d,%6d,%6d)  %s\n",
               c, fc, h0[0], h0[1], h0[2], h1[0], h1[1], h1[2], h2[0], h2[1], h2[2],
               (h2[1] > -600) ? "<- Kopf am BODEN (liegend)" : (h2[1] < -1200 ? "<- Kopf HOCH (stehend)" : ""));
    }
}

int main(void)
{
    printf("== Leons Opfer-Pose: RE1.5-Bank gegen RE2-Bank (gerenderte Knochen, PL00-Rig) ==\n");
    if (!load_pl00()) { printf("FAIL: PL00.EDD/EMR nicht ladbar — Messung wertlos\n"); return 1; }
    printf("PL00: %d Knochen, %d Keyframes a %d Bytes, %d Clips\n",
           s_pl00_skel.bone_count, s_pl00_skel.keyframe_count,
           s_pl00_skel.keyframe_size_bytes, s_pl00_anim.clip_count);

    /* Referenz: Leon STEHEND aus seinem eigenen Rig (Clip 0 = Idle) */
    {
        re15_actor_t p; memset(&p, 0, sizeof p);
        p.active = 1; p.motion = 0; p.anim_frame = 0;
        int kf = re15_compute_actor_kf(&s_pl00_anim, &s_pl00_skel, &p, 0, 0);
        re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
        g_anim_pose_actor = NULL;
        if (kf >= 0 && re15_skel_compute_pose(&s_pl00_skel, kf, poses) == 0) {
            int32_t hw[3]; re15_skel_bone_to_world(poses[8].trans, 0, 0, 0, 0, hw);
            printf("REFERENZ Leon STEHEND (PL00 Clip 0 Frame 0): Kopf y=%d\n", hw[1]);
        }
    }

    static re15_enemy_bank_t b15, b2;
    if (load_re15_bank(0x10, &b15)) dump_bank("RE1.5 EM010 (STAGE1-Zombie)", &b15);
    else printf("\nRE1.5-Bank EM010 FEHLT\n");
    if (load_re2_bank(0x10, &b2))   dump_bank("RE2 EM010 (RE2-Retail-Zombie)", &b2);
    else printf("\nRE2-Bank EM010 FEHLT\n");

    /* Was WAEHLT der Port heute? re15_victim_clip_map, Zombie-Zweig (RE1.5-Belegung) */
    printf("\n== Port-Clipwahl (re15_victim_clip_map, Zombie-Default) ==\n");
    for (int v = 0; v <= 1; v++)
        printf("  Variante %d (%s): intro=%d hold=%d release=%d COLLAPSE=%d\n",
               v, v ? "hinten" : "vorn", v * 3, v * 3 + 1, v * 3 + 2, v + 6);
    printf("  -> RE1.5-Victim-Bank hat %d Clips, RE2-Victim-Bank hat %d Clips.\n",
           b15.victim_ok ? b15.anim_victim.clip_count : -1,
           b2.victim_ok ? b2.anim_victim.clip_count : -1);
    return 0;
}
