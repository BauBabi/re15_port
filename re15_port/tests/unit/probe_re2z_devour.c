/**
 * @file probe_re2z_devour.c
 * @brief MESS-SONDE (kein PIN) — NUTZER-REPORT 2026-08-23 (RE2-KI): "Wenn Leon stirbt und von
 *        Zombies gefressen wird, ist der Zombie bei der Finisher-Position irgendwie am BEIN,
 *        nicht mehr wie sonst am Oberkoerper/Kopf."
 *
 * Gemessen wird der komplette Fress-Vorgang (Griff -> Biss-Tod -> Kollaps) mit ECHTEM
 * game_step, ROOM1140-Spawns und GELADENEN RE2-Baenken:
 *   LEON  : Weltposition + Anker + Yaw + Clip/Frame; GERENDERTE Knochen (PL00-Rig +
 *           Keyframe-Pool der Greifer-Opfer-Bank — exakt platform/pc/main.c)
 *   ZOMBIE: Weltposition + Anker + Yaw + Clip/Frame + +0x5/+0x6; GERENDERTE Knochen aus
 *           DER BANK, DIE AUCH DER RENDERER NIMMT (re15_re2z_poses_loco_bank; EXEC[6] =
 *           Zustand 1 / +0x5 = 6 -> Paar 2 = Aktionsbank)
 *
 * KERNZAHL: an WELCHEM Leon-Knochen haengt der Zombiekopf? Ausgegeben wird der Leon-Knochen
 * mit dem kleinsten Abstand zum Zombiekopf plus die Abstaende zu Kopf / Brust / Fuessen.
 *
 * SOLLSEITE (EMOVL10_S0.BIN, RAW @0x80100000, selbst disassembliert):
 *   EXEC[6] "ueber der Leiche" @0x80103954
 *     P0 @0x801039B0: sw 0xF0018,332(s0)          ; Clip 0x18, Frame 0, Rate 0xF
 *                     a0 = 0x800CFBF8 (SPIELER)   @0x801039B4-B8
 *                     jal 0x80015B94              @0x801039D0   ** ANKER **
 *                     sb 1,6(s0)                  @0x801039D4 (Delay-Slot) -> FALLTHROUGH P1
 *     P1 @0x80103A08: jal 0x80015CB8 (a0 = s0 = ZOMBIE)  @0x80103A14  ** PLATZIERUNG **
 *                     jal 0x8002959C (a3 = 256)          @0x80103A28  (Advance)
 *   0x80015B94 (PSX.EXE): s0 = *(0x800CE330) = die getickte Entity (der Zombie);
 *                     s0->anchor(+0x164/166/168) = s0->pos - RotY(s0->yaw) * off(clip,frame)
 *                     @0x80015C40-78, und KOPIE in a0 (den Spieler): sh a0,356(s3) /
 *                     sh v0,358(s3) / sh v0,360(s3) @0x80015C7C/C88/C94.
 *   0x80015CB8 (PSX.EXE): s1 = a0; s1->pos(+0x38/3C/40) = s1->anchor + RotY(s1->yaw) * off
 *                     @0x80015D58-90.
 *
 * Aufruf: probe_re2z_devour [seeds=4] [frames=900]
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re15_camera.h"
#include "re15_game_step.h"
#include "re15_collision.h"
#include "re15_inventory.h"
#include "re15_msg.h"
#include "re15_emd.h"
#include "re15_skeleton.h"
#include "re15_anim_select.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern uint32_t re15_re2_rand(void);
extern int      re15_ai_facing_aligned(const re15_actor_t *e, const re15_actor_t *t);
extern int      re15_re2z_poses_loco_bank(const re15_actor_t *a);
extern void     re15_re2z_onesave_reset(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static re15_emd_skeleton_t  s_pl00_skel;
static re15_emd_animation_t s_pl00_anim;
static int s_pl00_ok = 0;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* ---- LEON, GERENDERT (main.c: PL00-Knochen + Keyframe-Pool der Greifer-Opfer-Bank) -------- */
static int leon_bones(const re15_actor_t *pl, uint8_t gtype, re15_skel_pose_t *out, int *nbone)
{
    if (!s_pl00_ok) return 0;
    re15_enemy_bank_t *vb = re15_enemy_find(gtype);
    if (!vb || !vb->victim_ok || vb->anim_victim.clip_count <= 0) return 0;
    if ((int)pl->motion >= vb->anim_victim.clip_count) return 0;
    re15_emd_skeleton_t vs = s_pl00_skel;
    vs.keyframe_data       = vb->skel_victim.keyframe_data;
    vs.keyframe_data_size  = vb->skel_victim.keyframe_data_size;
    vs.keyframe_count      = vb->skel_victim.keyframe_count;
    vs.keyframe_size_bytes = vb->skel_victim.keyframe_size_bytes;
    int kf = re15_compute_actor_kf(&vb->anim_victim, &vs, pl, (int)pl->motion,
                                   (uint32_t)pl->anim_frame);
    if (kf < 0) return 0;
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&vs, kf, out);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    *nbone = vs.bone_count;
    return 1;
}

/* ---- ZOMBIE, GERENDERT (die Bankwahl des Renderers: re15_re2z_poses_loco_bank) ------------ */
static int zombie_bones(const re15_actor_t *e, re15_skel_pose_t *out, int *nbone)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok) return 0;
    const re15_emd_skeleton_t  *s = &b->skel;
    const re15_emd_animation_t *a = &b->anim;
    if (b->loco_ok && re15_re2z_poses_loco_bank(e)) { s = &b->skel_loco; a = &b->anim_loco; }
    if (s->bone_count <= 0 || a->clip_count <= 0) return 0;
    int kf = re15_compute_actor_kf(a, s, e, -1, (uint32_t)e->anim_frame);
    if (kf < 0) return 0;
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(s, kf, out);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    *nbone = s->bone_count;
    return 1;
}

static int32_t dist3(const int32_t a[3], const int32_t b[3])
{
    double dx = (double)a[0] - b[0], dy = (double)a[1] - b[1], dz = (double)a[2] - b[2];
    return (int32_t)(sqrt(dx * dx + dy * dy + dz * dz) + 0.5);
}

int main(int argc, char **argv)
{
    int seeds  = (argc > 1) ? atoi(argv[1]) : 4;
    int budget = (argc > 2) ? atoi(argv[2]) : 900;

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    {   size_t a = 0, b = 0;
        uint8_t *edd = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EDD", &a);
        uint8_t *emr = slurp(RE15_ASSET_PSX_DIR "/PLD/PL00.EMR", &b);
        s_pl00_ok = (edd && emr &&
                     re15_emd_parse_animation(edd, a, &s_pl00_anim) == 0 &&
                     re15_emd_parse_skeleton (emr, b, &s_pl00_skel) == 0);
    }
    printf("PL00-Rig: %s (bones=%d)\n", s_pl00_ok ? "geladen" : "FEHLT",
           s_pl00_ok ? s_pl00_skel.bone_count : -1);
    /* REFERENZ: Leon STEHEND (PL00 eigene Bank, Clip 0 Frame 0) — Knochen nach Hoehe
     * benennbar machen (PSX: Y nach oben NEGATIV, Boden y=0). */
    if (s_pl00_ok && s_pl00_anim.clip_count > 0) {
        re15_actor_t ref; memset(&ref, 0, sizeof ref);
        ref.rot_y = 0; ref.motion = 0; ref.anim_frame = 0;
        int kf = re15_compute_actor_kf(&s_pl00_anim, &s_pl00_skel, &ref, 0, 0u);
        re15_skel_pose_t rp[RE15_EMD_MAX_BONES];
        void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
        int rv = re15_skel_compute_pose(&s_pl00_skel, kf, rp);
        g_anim_pose_actor = save;
        if (rv == 0) {
            printf("PL00 STEHEND (Clip0 F0), lokale Knochenhoehe y (negativ = oben):\n   ");
            for (int b = 0; b < s_pl00_skel.bone_count; b++)
                printf("b%d=%d ", b, (int)rp[b].trans[1]);
            printf("\n");
        }
    }

    int devours = 0;
    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_player_aim_reset();
        re15_re2z_onesave_reset();
        re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
        g_current_room_id = 0x1140;
        if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
        if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
        g_scd.work_vars[10] = 0;
        for (int i = 0; i < 120; i++) scd_vm_tick();
        load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
        pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
        re15_collision_set_band(0);
        re15_player_set_aim_clip_len(12);
        re15_inv_load_briefing();
        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

        int zs = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) {
                int nib = g_actors[s].grid_id & 0x0f;
                if (zs < 0 && nib != 7 && nib != 8) zs = s;
            }
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (s != zs && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type))
                g_actors[s].active = 0;
        if (zs < 0) { printf("seed %d: kein Zombie\n", seed); continue; }
        re15_actor_t *z = &g_actors[zs];
        if (seed == 0) {   /* BANK-GRUNDWAHRHEIT: der Wurzel-Offset-Verlauf von Clip 0x18
                            * (Paar 2 = Aktionsbank, die der Dispatcher an EXEC[6] reicht) */
            re15_enemy_bank_t *b = re15_enemy_find(z->type);
            if (b && b->ok && 0x18 < b->anim.clip_count) {
                const re15_emd_clip_t *c = &b->anim.clips[0x18];
                printf("EM%02X Paar2 Clip 0x18: %d Frames; Wurzel-Offset (sx,sy,sz) je Frame:\n",
                       z->type, c->frame_count);
                for (int fr = 0; fr < c->frame_count; fr++) {
                    int kf = (int)(b->anim.frames[c->first_frame + fr] & 0xFFFu);
                    int16_t sx, sy, sz;
                    if (!re15_emd_get_keyframe_speed(&b->skel, kf, &sx, &sy, &sz)) continue;
                    if (fr % 8 == 0 || fr == c->frame_count - 1)
                        printf("   f%-3d kf%-4d (%6d,%6d,%6d)\n", fr, kf, sx, sy, sz);
                }
            }
            if (b && b->victim_ok) {
                for (int cl = 13; cl <= 15; cl += 2) {
                    if (cl >= b->anim_victim.clip_count) continue;
                    const re15_emd_clip_t *c = &b->anim_victim.clips[cl];
                    printf("EM%02X Opfer-Bank Clip %d: %d Frames; Wurzel-Offset:\n",
                           z->type, cl, c->frame_count);
                    for (int fr = 0; fr < c->frame_count; fr++) {
                        int kf = (int)(b->anim_victim.frames[c->first_frame + fr] & 0xFFFu);
                        int16_t sx, sy, sz;
                        if (!re15_emd_get_keyframe_speed(&b->skel_victim, kf, &sx, &sy, &sz)) continue;
                        if (fr % 16 == 0 || fr == c->frame_count - 1)
                            printf("   f%-3d kf%-4d (%6d,%6d,%6d)\n", fr, kf, sx, sy, sz);
                    }
                }
            }
        }
        {   int ang = (seed * 4096) / (seeds > 0 ? seeds : 1);
            pl->x = z->x + (int32_t)((900 * (int64_t)re15_cos_q12(ang)) >> 12);
            pl->z = z->z - (int32_t)((900 * (int64_t)re15_sin_q12(ang)) >> 12);
            pl->y = z->y;
            pl->rot_y = (int16_t)((seed * 337 + 700) & 0xfff);
        }

        int vs_prev = 0, printed = 0, header = 0, dead = 0;
        for (int f = 0; f < budget; f++) {
            pl->floor = z->floor;
            /* Griff: nicht wehren; HP auf 0 halten -> der Biss (dmg 20 @0x80100014) toetet
             * beim naechsten Biss-Frame (hp-20 < -14 -> ret 2 -> Devour @0x80102920-50). */
            if (!dead && re15_player_victim_state() != 2 && pl->hp > 0) pl->hp = 0;
            frame(0, 0);

            int vs = re15_player_victim_state();
            if (vs == 2) dead = 1;
            if (vs != 0 || vs_prev != 0) {
                if (!header) {
                    header = 1;
                    printf("\n== seed %d: GRIFF ab f%d (Zombie slot %d typ 0x%02X) ==\n",
                           seed, f, zs, z->type);
                    printf("   %-5s %-2s | %-9s %-6s %-4s %-3s | %-9s %-6s %-4s %-2s %-2s | "
                           "%s\n",
                           "f", "vs", "LEON x/z", "yaw", "clip", "afr",
                           "ZOMB x/z", "yaw", "clip", "s1", "s2",
                           "ZKopf->naechster Leon-Knochen | d(Kopf) d(Brust) d(Fuss)");
                }
                if (printed < 400) {
                    printed++;
                    char note[220]; note[0] = 0;
                    re15_skel_pose_t lp[RE15_EMD_MAX_BONES], zp[RE15_EMD_MAX_BONES];
                    int ln = 0, zn = 0;
                    if (leon_bones(pl, z->type, lp, &ln) && zombie_bones(z, zp, &zn)) {
                        /* Zombiekopf = der Knochen mit der kleinsten (hoechsten) Y-Koordinate
                         * in der STEHENDEN Referenz — hier direkt der aktuelle Hoechste des
                         * Zombie-Rigs, EM010 Bone 8 (dieselbe Wahl wie re15_re2z_gore/Blut). */
                        int zhead = 8; if (zhead >= zn) zhead = zn - 1;
                        int32_t zh[3];
                        re15_skel_bone_to_world(zp[zhead].trans, z->rot_y, z->x, z->y, z->z, zh);
                        int best = -1; int32_t bestd = 0x7fffffff;
                        for (int bi = 0; bi < ln; bi++) {
                            int32_t w[3];
                            re15_skel_bone_to_world(lp[bi].trans, pl->rot_y, pl->x, pl->y, pl->z, w);
                            int32_t d = dist3(zh, w);
                            if (d < bestd) { bestd = d; best = bi; }
                        }
                        int32_t wh[3], wc[3], wf[3];
                        re15_skel_bone_to_world(lp[8 < ln ? 8 : 0].trans, pl->rot_y,
                                                pl->x, pl->y, pl->z, wh);   /* PL00 Bone 8 */
                        re15_skel_bone_to_world(lp[0].trans, pl->rot_y,
                                                pl->x, pl->y, pl->z, wc);   /* Root/Huefte */
                        re15_skel_bone_to_world(lp[3 < ln ? 3 : 0].trans, pl->rot_y,
                                                pl->x, pl->y, pl->z, wf);   /* Bein-Kette */
                        snprintf(note, sizeof note,
                                 "Zkopf(%d,%d,%d) -> Leon-Bone %d d=%d | b8 %d b0 %d b3 %d",
                                 zh[0], zh[1], zh[2], best, bestd,
                                 dist3(zh, wh), dist3(zh, wc), dist3(zh, wf));
                    }
                    printf("   %-5d %-2d | %5d/%5d %6d %4d %3d | %5d/%5d %6d %4d.%-3d %2d %2d | %s\n",
                           f, vs, (int)pl->x, (int)pl->z, (int)pl->rot_y,
                           (int)pl->motion, (int)pl->anim_frame,
                           (int)z->x, (int)z->z, (int)z->rot_y, (int)z->motion,
                           (int)z->anim_frame,
                           (int)z->sub_state_1, (int)z->sub_state_2, note);
                    printf("         anker L(%d,%d) Z(%d,%d)\n",
                           (int)pl->anchor_x, (int)pl->anchor_z,
                           (int)z->anchor_x, (int)z->anchor_z);
                }
            }
            if (vs == 2 && vs_prev != 2) devours++;
            vs_prev = vs;
        }
        if (!header) printf("seed %d: KEIN Griff in %d Frames\n", seed, budget);
    }
    printf("\nSUMMARY: %d Fress-Kollapse gemessen\n", devours);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}
