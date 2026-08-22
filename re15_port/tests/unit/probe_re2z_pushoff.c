/**
 * @file probe_re2z_pushoff.c
 * @brief MESS-SONDE (kein PIN) — NUTZER-REPORT: "Beim Wegpushen, wenn Leon vom Zombie
 *        gebissen wird, geht er hinter…" (RE2-KI-Modus).
 *
 * Gemessen werden BEIDE Aktoren Frame fuer Frame durch den ganzen Griff:
 *   Leon   : Weltposition (x,z), rot_y, Victim-Zustand, Clip (motion) + anim_frame, Anker
 *   Zombie : Weltposition (x,z), rot_y, +0x5/+0x6, Clip + anim_frame, Anker
 * Zusaetzlich die GERENDERTE Pose (PL00-Rig + Keyframe-Pool der Greifer-Bank, exakt die
 * Komposition aus platform/pc/main.c:5537-5556) als Weltposition von PL00-Bone 8 (Kopf).
 *
 * Sollwerte aus dem Original (EMOVL10_S0.BIN, selbst disassembliert):
 *   Spieler-Griff-Handler 0x8010A924 -> Maschine 0x8010A9B8, Phasen @0x801001DC.
 *   P0 @0x8010AA50:
 *     8010aa68/6c  addiu v0,zero,2048 / sh v0,344(s1)   -> PL+0x158 = 0x800 (Pose-Yaw-Offset)
 *     8010aa80-88  lw a1,56(a2) / lw a2,64(a2) / jal 0x80015558 (a3=2048)
 *                                                       -> PL-Yaw SNAP auf den Greifer
 *     8010aa98-b0  Variante&1 (BEHIND): sh zero,344(s1) (+0x158=0) und +0x76 += 2048
 *   => FRONT : rot_y = bearing(PL->Z)          , +0x158 = 0x800
 *      BEHIND: rot_y = bearing(PL->Z) + 0x800  , +0x158 = 0
 *   P6 (EXIT) @0x8010AEDC schreibt NUR cmd/Flags — KEINEN Yaw.
 *
 * Aufruf: probe_re2z_pushoff [seeds=6] [frames=600] [mash=1]
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

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
extern int      re15_actor_clip_len(const re15_actor_t *a);
extern uint32_t re15_re2_rand(void);
extern int      re15_ai_facing_aligned(const re15_actor_t *e, const re15_actor_t *t);

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

/* GERENDERTE Pose Leons (main.c:5537-5556): PL00-Knochen + Keyframes der Greifer-Bank. */
static int leon_bone_world(const re15_actor_t *pl, uint8_t gtype, int bone, int32_t out[3])
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
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor; g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(&vs, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[bone].trans, pl->rot_y, pl->x, pl->y, pl->z, out);
    return 1;
}

int main(int argc, char **argv)
{
    int seeds  = (argc > 1) ? atoi(argv[1]) : 6;
    int budget = (argc > 2) ? atoi(argv[2]) : 600;
    int mash   = (argc > 3) ? atoi(argv[3]) : 1;

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
    printf("PL00-Rig: %s\n", s_pl00_ok ? "geladen" : "FEHLT (Pose-Spalten leer)");

    int grabs_total = 0;
    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_player_aim_reset();
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

        /* EINEN Zombie behalten, Leon direkt davor absetzen (der Griff braucht dist<Kontakt). */
        /* NICHT den Liegenden (grid-Nibble 7/8) nehmen — der hat byte-true keinen Wecker
         * (DECIDE-Stub @0x801039F4) und wuerde nie greifen. */
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
        {   int ang = (seed * 4096) / (seeds > 0 ? seeds : 1);
            pl->x = z->x + (int32_t)((900 * (int64_t)re15_cos_q12(ang)) >> 12);
            pl->z = z->z - (int32_t)((900 * (int64_t)re15_sin_q12(ang)) >> 12);
            pl->y = z->y;
            pl->rot_y = (int16_t)((seed * 337 + 700) & 0xfff);   /* BEWUSST beliebiger Start-Yaw */
        }

        int vs_prev = 0, printed = 0, grabbed_seen = 0;
        int32_t bearing_at_grab = -1; int variant_at_grab = -1;
        int16_t yaw_at_grab = 0, yaw_before_grab = 0;
        for (int f = 0; f < budget; f++) {
            pl->hp = 100;                       /* nicht sterben — nur der Wegstoss interessiert */
            pl->floor = z->floor;
            uint16_t cur = 0, edge = 0;
            if (mash && re15_player_is_grabbed() && (f & 1)) {
                cur |= RE15_PAD_BIT_CIRCLE; edge = RE15_PAD_BIT_CIRCLE;   /* Mash 0xf0f0 */
            }
            int16_t yaw_pre = pl->rot_y;
            frame(cur, edge);

            int vs = re15_player_victim_state();
            if (vs != 0 || vs_prev != 0) {
                if (!grabbed_seen) {
                    grabbed_seen = 1; grabs_total++;
                    yaw_before_grab = yaw_pre;
                    yaw_at_grab     = pl->rot_y;
                    bearing_at_grab = ((int)re15_atan2_q12(z->z - pl->z, z->x - pl->x) - 0x400) & 0xfff;
                    variant_at_grab = re15_ai_facing_aligned(z, pl);
                    printf("\n== seed %d: GRIFF ab f%d (Zombie slot %d typ 0x%02X) ==\n",
                           seed, f, zs, z->type);
                    printf("   Leon-Yaw VOR dem Griff = %d ; NACH dem Latch = %d\n",
                           (int)yaw_before_grab, (int)yaw_at_grab);
                    printf("   SOLL (@0x8010AA88): bearing(PL->Z) = %d  | Variante(15910)=%d\n",
                           (int)bearing_at_grab, variant_at_grab);
                    printf("   SOLL rot_y = %d (front) bzw. %d (behind); +0x158 = %s\n",
                           (int)bearing_at_grab, (int)((bearing_at_grab + 0x800) & 0xfff),
                           variant_at_grab ? "0" : "0x800");
                    printf("   %-5s %-3s | %-26s %-5s %-4s %-4s | %-22s %-5s %-4s %-3s %-3s | %s\n",
                           "f", "vs", "LEON pos(x,z)/anker", "yaw", "clip", "afr",
                           "ZOMBIE pos(x,z)", "yaw", "clip", "s1", "s2", "Kopf(Bone8) Welt");
                }
                if (printed < 260) {
                    printed++;
                    int32_t h[3] = { 0, 0, 0 };
                    int hok = leon_bone_world(pl, z->type, 8, h);
                    printf("   %-5d %-3d | (%7d,%7d)/(%7d,%7d) %5d %4d %4d | (%7d,%7d) %5d %4d %3d %3d | %s\n",
                           f, vs, (int)pl->x, (int)pl->z, (int)pl->anchor_x, (int)pl->anchor_z,
                           (int)pl->rot_y, (int)pl->motion, (int)pl->anim_frame,
                           (int)z->x, (int)z->z, (int)z->rot_y, (int)z->motion,
                           (int)z->sub_state_1, (int)z->sub_state_2,
                           hok ? "" : "(keine Pose)");
                    if (hok)
                        printf("        Kopf=(%d,%d,%d)\n", h[0], h[1], h[2]);
                }
                if (vs == 0 && vs_prev != 0) {
                    int32_t bear_now = ((int)re15_atan2_q12(z->z - pl->z, z->x - pl->x) - 0x400) & 0xfff;
                    printf("   ENDE f%d: Leon-Yaw = %d ; bearing(PL->Z) jetzt = %d ; Delta = %d\n",
                           f, (int)pl->rot_y, (int)bear_now,
                           (int)(((int)pl->rot_y - bear_now) & 0xfff));
                    printf("   (byte-true erwartet: Delta ~0 bei FRONT-Variante, ~2048 bei BEHIND)\n");
                    break;
                }
            }
            vs_prev = vs;
            if (!grabbed_seen && getenv("RE15_PUSH_STATUS") && (f % 100) == 99)
                printf("   [status] seed %d f%d Z st=%u s1=%u s2=%u dist=%u clip=%d hp=%d 10E=%04X "
                       "1D3=%02X  Zpos=(%d,%d) PLpos=(%d,%d)\n",
                       seed, f, z->state, z->sub_state_1, z->sub_state_2, (unsigned)z->ai_dist,
                       (int)z->motion, z->hp, z->re2z_f10e, z->re2z_self1d3,
                       (int)z->x, (int)z->z, (int)pl->x, (int)pl->z);
        }
        if (!grabbed_seen) printf("seed %d: KEIN Griff in %d Frames\n", seed, budget);
    }
    printf("\nSUMMARY: %d Griffe gemessen\n", grabs_total);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}
