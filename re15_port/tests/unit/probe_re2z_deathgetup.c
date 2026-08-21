/**
 * @file probe_re2z_deathgetup.c
 * @brief MESSSONDE (kein ctest) — RNG-SEED-SWEEP fuer die ZWEI Nutzer-Befunde im RE2-KI-Modus:
 *
 *   (A) "Bei RE2 AI ist die Sterbeanimation nicht vollstaendig, friert ein vor dem kompletten
 *        Tod."                     -> Modus A, ROOM1140, echter Weg (game_step + Pad + Schuesse)
 *   (B) "Nach dem Loesen des Raetsels im Generator-Raum steht der Zombie direkt neben Leon
 *        abrupt, quasi ohne Animation, direkt vom Liegen zum Stehen."
 *                                  -> Modus B, ROOM11F0, echter Weg (die ECHTEN sub18-Bytes
 *                                     durch die ECHTE SCD-VM)
 *
 * ⛔ DIE POSE MUSS AUS DER BANK KOMMEN, AUS DER DER RENDERER POSIERT.
 * re15_enemy_bone_world_pos() (re15_damage.c) posiert IMMER die Aktions-Bank; der Renderer
 * (platform/pc/main.c) schaltet ueber re15_actor_uses_loco_bank -> re15_re2z_poses_loco_bank
 * um. Diese Sonde bildet die Renderer-Auswahl 1:1 nach (Vorlage probe_re2z_fallstand.c).
 *
 * Protokolliert pro Frame und Aktor: state/+0x5/+0x6, Clip, anim_frame, BERECHNETER KEYFRAME,
 * posierte BANK, Clip-Laenge DIESER Bank, +0x1D3, +0x10E, +0x21A, grid, hp und die
 * Weltposition von Bone 8 (Brust) relativ zum Aktor-Boden.
 *
 * DETEKTOREN
 *   A1 "eingefroren vor dem Tod": in state 3 aendert sich der BERECHNETE KEYFRAME >= --afreeze
 *      Frames nicht, obwohl anim_frame < clip_len-1.
 *   A2 "Tod erreicht CORPSE nie": state 3 laeuft > --adead Frames ohne state 7.
 *   A3 "Pose-Sprung am Uebergang": |dy8| aendert sich um > --jump Einheiten in EINEM Frame,
 *      waehrend hp < 0 (also in der Todes-/Leichenkette).
 *   A4 "Leichen-Clip haelt Frame 0": nach state 7 bleibt anim_frame == 0 und der Keyframe fest.
 *   B1 "abruptes Aufstehen": dy8 springt vom Liegen (> --dyth) in den Stand (< --dyth)
 *      in <= --bmin Frames.
 *   B2 "Get-up-Clip laeuft nicht durch": EXEC[9] wird verlassen, bevor anim_frame >= clip_len-1.
 *
 * Aufruf: probe_re2z_deathgetup <A|B> [seeds=64] [frames=900] [weapon=3]
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
extern int      re15_re2z_poses_loco_bank(const re15_actor_t *a);
extern int      re15_re2z_last_death_handler(void);

/* ---- Pose aus der Bank, aus der der Renderer posiert --------------------------------------- */
static int probe_pose(const re15_actor_t *e, int bone, int32_t out[3],
                      int *bank_is_loco, int *out_kf, int *out_fc)
{
    out[0] = e->x; out[1] = e->y; out[2] = e->z;
    if (bank_is_loco) *bank_is_loco = 0;
    if (out_kf) *out_kf = -1;
    if (out_fc) *out_fc = 0;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b) return 0;
    const re15_emd_skeleton_t  *sk = &b->skel;
    const re15_emd_animation_t *an = &b->anim;
    int clip_override = -1;
    if (re15_re2z_poses_loco_bank(e) && b->loco_ok
        && (int)e->motion < b->anim_loco.clip_count) {
        sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion;
        if (bank_is_loco) *bank_is_loco = 1;
    }
    if (sk->bone_count <= 0 || bone < 0 || bone >= sk->bone_count) return 0;
    if (an->clip_count <= 0 || sk->keyframe_count <= 0) return 0;
    if (out_fc && (int)e->motion < an->clip_count)
        *out_fc = an->clips[e->motion].frame_count;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    if (out_kf) *out_kf = kf;
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;
    int rv = re15_skel_compute_pose(sk, kf, poses);
    g_anim_pose_actor = save;
    if (rv != 0) return 0;
    re15_skel_bone_to_world(poses[bone].trans, e->rot_y, e->x, e->y, e->z, out);
    return 1;
}

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;
static uint8_t           *s_raw = NULL;

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

static int s_tick_vm = 0;
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    if (s_tick_vm) scd_vm_tick();
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* ---- Ringpuffer ---------------------------------------------------------------------------- */
typedef struct {
    int      f, slotid;
    uint8_t  st, s1, s2, r1d3, grid;
    uint16_t f10e, f21a;
    int16_t  clip;
    int32_t  af;
    int      kf, fc, loco;
    int      hp;
    int32_t  dy8;
    int      dh;
    int16_t  t158, t15a;
} row_t;
#define RING 2600
static row_t s_ring[RING]; static int s_ring_n = 0;

static void ring_snap(int f, int slotid, const re15_actor_t *e)
{
    if (s_ring_n >= RING) return;
    int32_t b8[3]; int loco = 0, kf = -1, fc = 0;
    probe_pose(e, 8, b8, &loco, &kf, &fc);
    row_t *r = &s_ring[s_ring_n++];
    r->f = f; r->slotid = slotid;
    r->st = e->state; r->s1 = e->sub_state_1; r->s2 = e->sub_state_2;
    r->r1d3 = e->re2z_self1d3; r->grid = e->grid_id;
    r->f10e = e->re2z_f10e; r->f21a = e->re2z_flags21a;
    r->clip = e->motion; r->af = (int32_t)e->anim_frame;
    r->kf = kf; r->fc = fc; r->loco = loco;
    r->hp = e->hp; r->dy8 = b8[1] - e->y;
    r->dh = re15_re2z_last_death_handler();
    r->t158 = e->re2z_t158; r->t15a = e->re2z_t15a;
}

static void ring_dump(const char *why, int slotid, int from, int to)
{
    printf("      --- %s ---\n", why);
    for (int i = 0; i < s_ring_n; i++) {
        const row_t *r = &s_ring[i];
        if (r->slotid != slotid || r->f < from || r->f > to) continue;
        printf("      f%-4d st=%u s1=%2u s2=%2u clip=%2d af=%3d/%-3d kf=%4d %s"
               "| 1D3=%02X 10E=%04X 21A=%04X grid=%02X dh=%2d hp=%5d 158=%5d 15A=%5d b8dy=%d\n",
               r->f, r->st, r->s1, r->s2, (int)r->clip, (int)r->af, r->fc, r->kf,
               r->loco ? "LOCO " : "act  ", r->r1d3, r->f10e, r->f21a, r->grid,
               r->dh, r->hp, (int)r->t158, (int)r->t15a, r->dy8);
    }
}

/* ============================================================================================
 * MODUS A — der Tod in ROOM1140 ueber den echten Weg
 * ========================================================================================== */
static long gA_freeze = 0, gA_nocorpse = 0, gA_jump = 0, gA_corpsepin = 0;
static int  gA_fz_seeds = 0, gA_nc_seeds = 0, gA_jp_seeds = 0, gA_cp_seeds = 0;
static int  gA_show = 0;
static long gA_deaths = 0, gA_runs = 0;
static long gA_dh_hist[10];
static long gA_deathlen_min = 1 << 30, gA_deathlen_max = -1;
static long gA_pin_runs = 0, gA_pin_phase[8];
static int  gA_pin_max = 0;

static void run_mode_a(int seeds, int budget, int weapon, int afreeze, int adead, int jumpth)
{
    int banner = 0;
    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
        s_tick_vm = 0;

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
        /* Die Briefing-Ladung fuehrt nur Messer + Browning HP — schwere Waffen erst granten,
         * sonst liefert re15_inv_find_item() -1 und jeder SQUARE-Druck verpufft. */
        if (re15_inv_find_item((uint8_t)weapon) < 0) (void)re15_inv_grant((uint8_t)weapon, 99);
        re15_player_set_equipped_weapon(weapon);
        {   int es = re15_inv_equipped_slot();
            if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();
        for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }

        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)
                && !(g_actors[s].grid_id & 0x80)) { slot = s; break; }
        if (slot < 0) continue;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        re15_actor_t *e = &g_actors[slot];

        pl->x = e->x - (2000 + (seed % 9) * 220);
        pl->z = e->z + ((seed % 5) - 2) * 150;
        pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
        re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

        if (!banner) {
            banner = 1;
            re15_enemy_bank_t *b = re15_enemy_find((uint8_t)e->type);
            printf("Bank 0x%02X: ACT clips=%d LOCO ok=%d clips=%d\n", e->type,
                   b ? b->anim.clip_count : 0, b ? b->loco_ok : -1,
                   b ? b->anim_loco.clip_count : 0);
            if (b) {
                printf("  ACT  len:");
                for (int c = 0; c < b->anim.clip_count && c < 32; c++)
                    printf(" [%d]=%d", c, b->anim.clips[c].frame_count);
                printf("\n  LOCO len:");
                for (int c = 0; c < b->anim_loco.clip_count && c < 32; c++)
                    printf(" [%d]=%d", c, b->anim_loco.clips[c].frame_count);
                printf("\n");
            }
        }

        gA_runs++;
        s_ring_n = 0;
        /* Erst schiessen, wenn der Zombie WIRKLICH aufrecht laeuft — sonst trifft man den
         * aufstehenden Fresser (EXEC[8] P4, +0x21A Bit 0x10) und misst nur den ZWEIG-2-
         * Stand-in statt des normalen Todes. */
        int shot_first = 150 + (seed % 40);
        int shot_per   = 7 + (seed % 9);

        int death_f = -1, corpse_f = -1;
        int kf_prev = -2, kf_same = 0, kf_same_start = -1;
        int32_t dy_prev = 0; int have_prev = 0;
        int hitF = 0, hitC = 0, hitJ = 0, hitP = 0;
        int corpse_kf0 = -2, corpse_pin = 0;
        int deathH = 0;
        int pin_run = 0, pin_max = 0, pin_s2 = -1;

        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (f >= shot_first && ((f - shot_first) % shot_per) == 0 && e->hp >= 0) {
                cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
            }
            frame(cur, edge);
            ring_snap(f, slot, e);

            int32_t b8[3]; int loco = 0, kf = -1, fc = 0;
            probe_pose(e, 8, b8, &loco, &kf, &fc);
            int32_t dy8 = b8[1] - e->y;

            if (e->state == 3 && death_f < 0) { death_f = f; deathH = re15_re2z_last_death_handler(); }
            if (e->state == 7 && corpse_f < 0 && death_f >= 0) {
                corpse_f = f;
                if (death_f >= 0) {
                    int dur = f - death_f;
                    if (dur < gA_deathlen_min) gA_deathlen_min = dur;
                    if (dur > gA_deathlen_max) gA_deathlen_max = dur;
                }
            }

            /* ---- A1: Keyframe steht still, waehrend die Todes-Kette laeuft ----
             * OHNE die Zusatzbedingung `af < fc-1`: der Port-Advancer PINNT play-once-Clips auf
             * fc-1 (player_common.c), und genau das ist der zweite Freeze-Weg (Original wrappt). */
            if (e->state == 3) {
                if (kf == kf_prev) {
                    if (kf_same == 0) kf_same_start = f - 1;
                    kf_same++;
                    if (kf_same == afreeze && fc > 0) {
                        hitF++;
                        if (gA_show < 8) { gA_show++;
                            printf("[A1-FREEZE] seed %3d slot %d f%d: kf=%d steht seit f%d "
                                   "(%d Frames) clip=%d af=%d/%d st=%u s1=%u s2=%u dh=%d\n",
                                   seed, slot, f, kf, kf_same_start, kf_same, (int)e->motion,
                                   (int)e->anim_frame, fc, e->state, e->sub_state_1,
                                   e->sub_state_2, re15_re2z_last_death_handler());
                            ring_dump("Kontext", slot, f - 20, f + 4); }
                    }
                } else { kf_same = 0; }
            } else { kf_same = 0; }
            kf_prev = kf;

            /* ---- A5: play-once-PIN in state 3 (der Port pinnt auf fc-1, das Original WRAPPT
             * @0x80029B48 `sb zero,333(s2)` und liefert 1). Nur Phasen, die die Rueckgabe NICHT
             * uebernehmen, duerfen ueberhaupt so lange auf demselben Clip stehen. ---- */
            if (e->state == 3 && fc > 1 && (int)e->anim_frame == fc - 1) {
                pin_run++;
                if (pin_run > pin_max) { pin_max = pin_run; pin_s2 = e->sub_state_2; }
            } else if (e->state == 3) pin_run = 0;

            /* ---- A3: Pose-Sprung in der Todeskette ---- */
            if (have_prev && e->hp < 0) {
                int32_t d = dy8 - dy_prev; if (d < 0) d = -d;
                if (d > jumpth) {
                    hitJ++;
                    if (gA_show < 12) { gA_show++;
                        printf("[A3-SPRUNG] seed %3d slot %d f%d: dy8 %d -> %d (%d) st=%u s1=%u "
                               "s2=%u clip=%d af=%d/%d kf=%d\n", seed, slot, f, dy_prev, dy8,
                               dy8 - dy_prev, e->state, e->sub_state_1, e->sub_state_2,
                               (int)e->motion, (int)e->anim_frame, fc, kf);
                        ring_dump("Kontext", slot, f - 10, f + 4); }
                }
            }
            dy_prev = dy8; have_prev = 1;

            /* ---- A4: Leichen-Clip haelt Frame 0 ---- */
            if (e->state == 7) {
                if (corpse_kf0 == -2) { corpse_kf0 = kf; corpse_pin = 0; }
                else if (kf == corpse_kf0) corpse_pin++;
                if (corpse_pin == 60 && fc > 1 && e->anim_frame == 0) {
                    hitP++;
                    if (gA_show < 14) { gA_show++;
                        printf("[A4-LEICHE-PIN] seed %3d slot %d f%d: state 7, clip=%d af=0/%d "
                               "kf=%d seit 60 Frames unbewegt\n", seed, slot, f, (int)e->motion,
                               fc, kf);
                        ring_dump("Kontext", slot, f - 12, f + 2); }
                }
            }
        }

        if (death_f >= 0) {
            gA_deaths++;
            if (deathH + 3 >= 0 && deathH + 3 < 10) gA_dh_hist[deathH + 3]++;
            if (corpse_f < 0 && (budget - death_f) > adead) {
                hitC++;
                if (gA_show < 16) { gA_show++;
                    printf("[A2-KEIN-CORPSE] seed %3d slot %d: state 3 ab f%d, in %d Frames NIE "
                           "state 7 (Endstand st=%u s1=%u s2=%u clip=%d af=%d hp=%d dh=%d)\n",
                           seed, slot, death_f, budget - death_f, e->state, e->sub_state_1,
                           e->sub_state_2, (int)e->motion, (int)e->anim_frame, e->hp,
                           re15_re2z_last_death_handler());
                    ring_dump("Kontext", slot, death_f - 4, death_f + 60); }
            }
        }
        if (pin_max >= 2) {
            gA_pin_runs++;
            if (pin_max > gA_pin_max) gA_pin_max = pin_max;
            if (pin_s2 >= 0 && pin_s2 < 8) gA_pin_phase[pin_s2]++;
        }
        if (seed < 2) {
            printf("[ENDSTAND] seed %d slot %d: st=%u s1=%u s2=%u clip=%d af=%d hp=%d "
                   "grid=%02X 21A=%04X pin_max=%d(s2=%d)\n", seed, slot, e->state,
                   e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame,
                   e->hp, e->grid_id, e->re2z_flags21a, pin_max, pin_s2);
            ring_dump("letzte Frames", slot, budget - 10, budget);
        }
        if (hitF) { gA_freeze += hitF; gA_fz_seeds++; }
        if (hitC) { gA_nocorpse += hitC; gA_nc_seeds++; }
        if (hitJ) { gA_jump += hitJ; gA_jp_seeds++; }
        if (hitP) { gA_corpsepin += hitP; gA_cp_seeds++; }
    }

    printf("\n=== MODUS A: %ld Laeufe, %ld Tode ===\n", gA_runs, gA_deaths);
    printf("(A1) Keyframe eingefroren waehrend state 3 : %ld Vorfaelle in %d Laeufen\n",
           gA_freeze, gA_fz_seeds);
    printf("(A2) state 3 erreicht NIE CORPSE           : %ld Vorfaelle in %d Laeufen\n",
           gA_nocorpse, gA_nc_seeds);
    printf("(A3) Pose-Sprung > %d in der Todeskette  : %ld Vorfaelle in %d Laeufen\n",
           jumpth, gA_jump, gA_jp_seeds);
    printf("(A4) Leichen-Clip haelt Frame 0           : %ld Vorfaelle in %d Laeufen\n",
           gA_corpsepin, gA_cp_seeds);
    printf("(A5) play-once-PIN in state 3 (Original WRAPPT): %ld Laeufe, laengster Pin %d "
           "Frames | Phase:", gA_pin_runs, gA_pin_max);
    for (int i = 0; i < 8; i++) if (gA_pin_phase[i]) printf(" s2=%d:%ld", i, gA_pin_phase[i]);
    printf("\n");
    printf("Todesdauer state3->state7: min=%d max=%d | Handler-Histogramm:",
           (gA_deathlen_min == (1 << 30)) ? -1 : (int)gA_deathlen_min, (int)gA_deathlen_max);
    for (int i = 0; i < 10; i++) if (gA_dh_hist[i]) printf(" [%d]=%ld", i - 3, gA_dh_hist[i]);
    printf("\n");
}

/* ============================================================================================
 * MODUS B — der Skript-Wecker in ROOM11F0 ueber die ECHTEN sub18-Bytes
 * ========================================================================================== */
static long gB_snap = 0, gB_cut = 0, gB_never = 0;
static int  gB_show = 0;
static long gB_rises = 0, gB_runs = 0;
static long gB_risehist[400];
static int  s_legacy_901 = 0;
static long gB_bigjump = 0, gB_jumphist[40];
static int32_t gB_jumpmax = 0, gB_maxjump[8];

static void run_mode_b(int seeds, int budget, int dyth, int bmin)
{
    int banner = 0;
    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
        s_tick_vm = 1;

        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_player_aim_reset();
        re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
        g_current_room_id = 0x11F0;
        g_room_change.pending = 0;

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->y = 0; pl->floor = 0;
        pl->x = -19554; pl->z = 22684; pl->rot_y = 0;
        re15_collision_set_band(0);
        scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
        load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);

        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

        /* Die Liegenden entstehen aus sub00 (Sce_em_set) — Frames geben. */
        for (int f = 0; f < 90; f++) { pl->hp = 100; frame(0, 0); }

        int slots[8], ns = 0;
        for (int s = 1; s < RE15_ACTOR_MAX && ns < 8; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) slots[ns++] = s;
        if (ns == 0) { printf("FAIL: keine Zombies in ROOM11F0 (seed %d)\n", seed); continue; }

        if (!banner) {
            banner = 1;
            printf("ROOM11F0: %d RE2-Zombies\n", ns);
            for (int i = 0; i < ns; i++) {
                re15_actor_t *e = &g_actors[slots[i]];
                int32_t b8[3]; int lo = 0, kf = -1, fc = 0;
                probe_pose(e, 8, b8, &lo, &kf, &fc);
                printf("  slot %d type=0x%02X grid=0x%02X st=%u s1=%u s2=%u clip=%d af=%d/%d "
                       "kf=%d %s b8dy=%d\n", slots[i], e->type, e->grid_id, e->state,
                       e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame, fc,
                       kf, lo ? "LOCO" : "act", b8[1] - e->y);
            }
            re15_enemy_bank_t *b = re15_enemy_find(0x10);
            if (b) {
                printf("  ACT  len:");
                for (int c = 0; c < b->anim.clip_count && c < 32; c++)
                    printf(" [%d]=%d", c, b->anim.clips[c].frame_count);
                printf("\n");
                /* Brusthoehe je Clip in Frame 0 der Aktions-Bank */
                re15_actor_t q = g_actors[slots[0]]; q.rot_y = 0; q.anim_frame = 0;
                q.anim_freeze = 0; q.state = 1; q.sub_state_1 = 9;
                printf("  ACT b8dy@f0:");
                for (int c = 0; c < b->anim.clip_count && c < 32; c++) {
                    q.motion = (int16_t)c; int32_t p[3];
                    if (probe_pose(&q, 8, p, NULL, NULL, NULL)) printf(" [%d]=%d", c, p[1] - q.y);
                }
                printf("\n");
            }
        }

        /* ---- DIE ECHTEN RAETSEL-BYTES: sub18 @Datei 0x16F6 durch die ECHTE VM ---- */
        int started = 0;
        for (int t = 2; t < 8 && !started; t++) {
            if (scd_thread_start(t, s_raw + 0x16F6) == 0) started = 1;
        }
        if (!started) { printf("FAIL: sub18 nicht startbar (seed %d)\n", seed); continue; }

        gB_runs++;
        s_ring_n = 0;
        int32_t dyprev[8]; int have[8]; int down_f[8]; int done[8];
        for (int i = 0; i < 8; i++) { dyprev[i] = 0; have[i] = 0; down_f[i] = -1; done[i] = 0; }
        int getup_start[8], getup_maxaf[8], getup_clip[8];
        for (int i = 0; i < 8; i++) { getup_start[i] = -1; getup_maxaf[i] = -1; getup_clip[i] = -1;
                                      gB_maxjump[i] = 0; }

        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            frame(0, 0);
            for (int i = 0; i < ns; i++) {
                re15_actor_t *e = &g_actors[slots[i]];
                /* NEGATIV-KONTROLLE: der ALTE D15.2-Wecker committete zusaetzlich `0x901`
                 * (EXEC[9]). Mit --legacy wird genau dieser eine Zusatz von der Sonde aus
                 * nachgestellt, damit BEIDE Staende mit DEMSELBEN Detektor gemessen werden. */
                if (s_legacy_901) {
                    uint8_t nib = (uint8_t)(e->grid_id & 0x0fu);
                    if (nib >= 9 && nib <= 10 && e->state == 1 && e->sub_state_1 == 7) {
                        re15_ai_set_state_word(e, 0x901);
                        e->grid_id = 0;
                    }
                }
                ring_snap(f, slots[i], e);
                int32_t b8[3]; int lo = 0, kf = -1, fc = 0;
                probe_pose(e, 8, b8, &lo, &kf, &fc);
                int32_t dy8 = b8[1] - e->y;

                /* B1: Liegen -> Stehen. Gemessen wird die ECHTE Uebergangsdauer: vom LETZTEN
                 * Frame, in dem die Brust noch klar am Boden war (dy8 > PRONE = -700; Clip 23
                 * Frame 0 liegt bei -150), bis zum ERSTEN Frame, in dem sie klar oben ist
                 * (dy8 < UPRIGHT = -2400; der Steh-Clip 0 Frame 0 liegt bei -2766). Alles
                 * dazwischen ist der sichtbare Aufsteh-Weg. */
                #define B_PRONE   (-700)
                #define B_UPRIGHT (-2400)
                /* B4 (bau-unabhaengig): der GROESSTE Brust-Sprung in EINEM Frame, solange der
                 * Zombie noch nicht steht. Das ist das eigentliche "abrupt" des Nutzers und
                 * misst dasselbe, egal welchen Clip der Wecker startet. */
                if (!done[i] && have[i]) {
                    int32_t d = dy8 - dyprev[i]; if (d < 0) d = -d;
                    if (d > gB_maxjump[i]) gB_maxjump[i] = d;
                }
                if (!done[i]) {
                    if (dy8 > B_PRONE) { down_f[i] = f; }   /* LETZTER klar liegender Frame */
                    else if (dy8 < B_UPRIGHT && down_f[i] >= 0) {
                        int dur = f - down_f[i];
                        gB_jumphist[(gB_maxjump[i] / 100 < 40) ? gB_maxjump[i] / 100 : 39]++;
                        if (gB_maxjump[i] > gB_jumpmax) gB_jumpmax = gB_maxjump[i];
                        if (gB_maxjump[i] >= 500) {
                            gB_bigjump++;
                            if (gB_show < 6) { gB_show++;
                                printf("[B4-POSE-SPRUNG] seed %3d slot %d f%d: groesster Brust-"
                                       "Sprung im Aufstehen = %d Einheiten in EINEM Frame "
                                       "(Dauer %d Frames, clip=%d af=%d/%d)\n", seed, slots[i], f,
                                       (int)gB_maxjump[i], dur, (int)e->motion,
                                       (int)e->anim_frame, fc);
                                ring_dump("Kontext", slots[i], f - 34, f + 4); }
                        }
                        gB_rises++;
                        gB_risehist[(dur < 400) ? dur : 399]++;
                        if (dur <= bmin) {
                            gB_snap++;
                            if (gB_show < 8) { gB_show++;
                                printf("[B1-ABRUPT] seed %3d slot %d f%d: Liegen->Stehen in %d "
                                       "Frames (dy8 %d -> %d) st=%u s1=%u s2=%u clip=%d af=%d/%d "
                                       "kf=%d grid=%02X\n", seed, slots[i], f, dur, dyprev[i],
                                       dy8, e->state, e->sub_state_1, e->sub_state_2,
                                       (int)e->motion, (int)e->anim_frame, fc, kf, e->grid_id);
                                ring_dump("Kontext", slots[i], f - 24, f + 6); }
                        }
                        done[i] = 1;
                    }
                }

                /* B2: EXEC[9] Get-up-Clip vollstaendig? */
                int in9 = (e->state == 1 && e->sub_state_1 == 9);
                if (in9) {
                    if (getup_start[i] < 0) { getup_start[i] = f; getup_clip[i] = (int)e->motion;
                                              getup_maxaf[i] = (int)e->anim_frame; }
                    if ((int)e->motion != getup_clip[i]) {
                        getup_clip[i] = (int)e->motion; getup_maxaf[i] = (int)e->anim_frame;
                    } else if ((int)e->anim_frame > getup_maxaf[i]) {
                        getup_maxaf[i] = (int)e->anim_frame;
                    }
                } else if (getup_start[i] >= 0) {
                    re15_actor_t q = *e; q.motion = (int16_t)getup_clip[i];
                    int fl = re15_actor_clip_len(&q);
                    if (fl > 0 && getup_maxaf[i] < fl - 1) {
                        gB_cut++;
                        if (gB_show < 10) { gB_show++;
                            printf("[B2-CLIP-ABBRUCH] seed %3d slot %d f%d: Get-up-Clip %d nur bis "
                                   "af=%d von %d (ab f%d) -> weiter st=%u s1=%u s2=%u clip=%d\n",
                                   seed, slots[i], f, getup_clip[i], getup_maxaf[i], fl,
                                   getup_start[i], e->state, e->sub_state_1, e->sub_state_2,
                                   (int)e->motion);
                            ring_dump("Kontext", slots[i], getup_start[i] - 6, f + 4); }
                    }
                    getup_start[i] = -1;
                }
                dyprev[i] = dy8; have[i] = 1;
            }
        }
        for (int i = 0; i < ns; i++)
            if (!done[i]) {
                gB_never++;
                if (gB_show < 12) { gB_show++;
                    re15_actor_t *e = &g_actors[slots[i]];
                    printf("[B3-STEHT-NIE-AUF] seed %3d slot %d: Endstand st=%u s1=%u s2=%u "
                           "clip=%d af=%d grid=%02X 10E=%04X\n", seed, slots[i], e->state,
                           e->sub_state_1, e->sub_state_2, (int)e->motion, (int)e->anim_frame,
                           e->grid_id, e->re2z_f10e);
                    ring_dump("Kontext", slots[i], budget - 40, budget); }
            }
    }

    printf("\n=== MODUS B: %ld Laeufe, %ld Aufsteh-Vorgaenge ===\n", gB_runs, gB_rises);
    printf("(B1) Liegen->Stehen in <= %d Frames (ABRUPT): %ld\n", bmin, gB_snap);
    printf("(B2) Get-up-Clip abgebrochen               : %ld\n", gB_cut);
    printf("(B3) steht nie auf                         : %ld\n", gB_never);
    printf("(B4) Aufstehen mit Brust-Sprung >= 500/Frame: %ld (groesster %d)\n",
           gB_bigjump, (int)gB_jumpmax);
    printf("--- groesster Sprung je Aufstehen (100er-Klassen):\n");
    for (int i = 0; i < 40; i++) if (gB_jumphist[i])
        printf("      %4d-%4d: %ld x\n", i * 100, i * 100 + 99, gB_jumphist[i]);
    printf("--- Aufsteh-Dauern (Pose) Histogramm:\n");
    for (int i = 0; i < 400; i++) if (gB_risehist[i]) printf("      %3d Frames: %ld x\n", i, gB_risehist[i]);
}

int main(int argc, char **argv)
{
    const char *mode = (argc > 1) ? argv[1] : "A";
    int seeds  = (argc > 2) ? atoi(argv[2]) : 64;
    int budget = (argc > 3) ? atoi(argv[3]) : 900;
    int weapon = (argc > 4) ? atoi(argv[4]) : 3;
    s_legacy_901 = (argc > 5) ? atoi(argv[5]) : 0;   /* Modus B: 1 = ALTER Wecker (0x901) */

    const char *base = getenv("RE15_ASSET_DIR");
    if (!base || !*base) base = RE15_ASSET_PSX_DIR;
    char path[600];
    int isB = (mode[0] == 'B' || mode[0] == 'b');
    snprintf(path, sizeof path, "%s/STAGE1/%s.RDT", base, isB ? "ROOM11F0" : "ROOM1140");
    size_t sz = 0;
    s_raw = slurp(path, &sz);
    if (!s_raw) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(s_raw, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    if (isB) run_mode_b(seeds, budget, -700, 3);
    else     run_mode_a(seeds, budget, weapon, 12, 200, 900);
    return 0;
}
