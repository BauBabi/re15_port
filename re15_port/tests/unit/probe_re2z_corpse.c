/**
 * @file probe_re2z_corpse.c
 * @brief MESSSONDE (kein ctest) — RNG-SEED-SWEEP fuer die ZWEI Nutzer-Befunde am RE2-Leichnam:
 *
 *   (A) "Der am Boden getoetete Zombie laeuft sofort aus, nicht langsam wie normal."
 *       -> die Blutlache unter der Leiche ist schlagartig voll statt langsam zu wachsen.
 *   (B) "Ausserdem gibt es keine finale Todesanimation."
 *
 * Weg: ROOM1140, echter Weg (re15_game_step + Pad R1/SQUARE), RE2-Flavor, RE2-Bank geladen,
 * g_room_rdt gefuellt. Vorlage: probe_re2z_deathgetup.c (Modus A).
 *
 * ⛔ MESSFALLE 0c9d9f6d: re15_enemy_bone_world_pos() posiert IMMER die Aktions-Bank; der
 * Renderer schaltet ueber re15_re2z_poses_loco_bank um. Diese Sonde bildet die Renderer-
 * Auswahl 1:1 nach.
 *
 * PROTOKOLL pro Frame (ab dem Tod): state/+0x5/+0x6, Clip, anim_frame, BERECHNETER Keyframe,
 * posierte Bank, +0x16A, +0x158, +0x15A, +0x16B, hp, Bone-8-Hoehe, dazu die LACHEN-KANAELE:
 *   - grab_kill_ctr (+0x9E)      = RE1.5-Pool-Budget, das der Renderer heute auswertet
 *   - crow_shadow_w/h            = der Port-Kanal fuer [+0x16C]+4/+6 (RE2/Kraehe-Schattenrecord)
 *   - POOL_HX/POOL_HZ            = die Halb-Ausdehnung, die platform/pc/main.c:6397-6423 HEUTE
 *                                  zeichnet (1:1 nachgebaute Renderer-Formel)
 *
 * DETEKTOREN
 *   C1 "Lache sofort voll": POOL_HX im ERSTEN state-7-Frame ist bereits >= 95% des Endwerts.
 *   C2 "kein Wachstum":     POOL_HX aendert sich ueber die ganze Leichenphase NICHT.
 *   C3 "keine Leichen-Maschine": in state 7 wird nur EIN einziger +0x5-Wert gesehen.
 *   C4 "Leichen-Pose steht": der berechnete Keyframe aendert sich ab dem N-ten state-7-Frame nie.
 *
 * Aufruf: probe_re2z_corpse [seeds=64] [frames=1400] [weapon=3] [flavor=2]   (flavor 1 = RE1.5)
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

/* ---- 1:1 die Renderer-Formel aus platform/pc/main.c:6380-6423 ------------------------------ */
static void render_pool(const re15_actor_t *n, int32_t *hx, int32_t *hz, int *tinted)
{
    int32_t nhx = 500, nhz = 600;
    int pool = 0;
    int nis_zombie = (n->type == 0x10 || n->type == 0x11 || n->type == 0x12 ||
                      n->type == 0x16 || n->type == 0x18);
    if (n->state == (uint8_t)RE15_AI_STATE_CORPSE && nis_zombie
        && re15_ai_flavor() != RE15_AI_FLAVOR_RE2) {
        int grow = (n->sub_state_1 <= 1)
                     ? (0x5a - (n->grab_kill_ctr > 0 ? n->grab_kill_ctr : 0))
                     : 0x5a;
        if (grow < 0) grow = 0;
        nhx = 600 + 8 * grow;
        nhz = 700 + 8 * grow;
        pool = 1;
    }
    if (nis_zombie && n->crow_shadow_w != 0) {   /* RE2-Leichenmaschine (FUN_8010A440) */
        nhx = n->crow_shadow_w; nhz = n->crow_shadow_h;
        pool = n->crow_pool ? 1 : 0;
    }
    if (n->type == 0x21 && n->crow_shadow_w != 0) {
        nhx = n->crow_shadow_w; nhz = n->crow_shadow_h;
        pool = n->crow_pool ? 1 : 0;
    }
    *hx = nhx; *hz = nhz; *tinted = pool;
}

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

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

int main(int argc, char **argv)
{
    int seeds  = (argc > 1) ? atoi(argv[1]) : 64;
    int budget = (argc > 2) ? atoi(argv[2]) : 1400;
    int weapon = (argc > 3) ? atoi(argv[3]) : 3;
    int flavor = (argc > 4) ? atoi(argv[4]) : 2;

    const char *base = getenv("RE15_ASSET_DIR");
    if (!base || !*base) base = RE15_ASSET_PSX_DIR;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", base);
    size_t sz = 0;
    s_raw = slurp(path, &sz);
    if (!s_raw) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(s_raw, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    long runs = 0, deaths = 0, corpses = 0;
    long c1 = 0, c2 = 0, c3 = 0, c4 = 0;
    long s1hist[16]; memset(s1hist, 0, sizeof s1hist);
    long growhist[16]; memset(growhist, 0, sizeof growhist);
    int shown = 0;
    int banner = 0;

    for (int seed = 0; seed < seeds; seed++) {
        memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

        re15_ai_flavor_set(flavor == 2 ? RE15_AI_FLAVOR_RE2 : RE15_AI_FLAVOR_RE15);
        re15_actor_init(); re15_aot_init(); scd_vm_init();
        re15_enemy_reset(); re15_enemy_ai_set_paused(0);
        re15_player_cmd_reset(); re15_player_aim_reset();
        re15_damage_seed_rng(0x0badf00du + (uint32_t)seed * 0x9E3779B9u);
        g_current_room_id = 0x1140;
        if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
        if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
        g_scd.work_vars[10] = 0;
        for (int i = 0; i < 120; i++) scd_vm_tick();
        if (flavor == 2) { load_bank2(0x10); load_bank2(0x11); load_bank2(0x16); }

        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
        pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
        re15_collision_set_band(0);
        re15_player_set_aim_clip_len(12);
        re15_inv_load_briefing();
        if (re15_inv_find_item((uint8_t)weapon) < 0) (void)re15_inv_grant((uint8_t)weapon, 99);
        re15_player_set_equipped_weapon(weapon);
        {   int es = re15_inv_equipped_slot();
            if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();
        for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }

        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
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
            printf("Flavor=%s Bank 0x%02X: ACT clips=%d LOCO ok=%d clips=%d\n",
                   flavor == 2 ? "RE2" : "RE15", e->type,
                   b ? b->anim.clip_count : 0, b ? b->loco_ok : -1,
                   b ? b->anim_loco.clip_count : 0);
        }

        runs++;
        int shot_first = 150 + (seed % 40);
        int shot_per   = 7 + (seed % 9);

        int death_f = -1, corpse_f = -1, corpse_n = 0;
        int32_t pool_first = 0, pool_last = 0, pool_min = 0, pool_max = 0;
        int kf_first = -2, kf_changes = 0;
        unsigned s1seen = 0;
        int s2seen_n = 0;
        int printed_this = 0;

        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (f >= shot_first && ((f - shot_first) % shot_per) == 0 && e->hp >= 0) {
                cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
            }
            frame(cur, edge);

            int32_t b8[3]; int loco = 0, kf = -1, fc = 0;
            probe_pose(e, 8, b8, &loco, &kf, &fc);
            int32_t hx, hz; int tinted;
            render_pool(e, &hx, &hz, &tinted);

            if (e->state == 3 && death_f < 0) death_f = f;
            if (e->state == (uint8_t)RE15_AI_STATE_CORPSE) {
                if (corpse_f < 0) {
                    corpse_f = f; pool_first = hx; pool_min = hx; pool_max = hx;
                    kf_first = kf;
                }
                corpse_n++;
                pool_last = hx;
                if (hx < pool_min) pool_min = hx;
                if (hx > pool_max) pool_max = hx;
                if (e->sub_state_1 < 16) s1seen |= (1u << e->sub_state_1);
                if (kf != kf_first) kf_changes++;
                (void)s2seen_n;

                /* eine Beispielspur je Lauf, erste 3 Laeufe */
                if (seed < 3 && corpse_n <= 260 &&
                    (corpse_n <= 8 || (corpse_n % 20) == 0)) {
                    printf("  seed%-2d f%-4d CORPSE+%-3d s1=%2u s2=%2u clip=%2d af=%3d/%-3d kf=%4d %s"
                           " 16A=%3u 158=%4d 15A=%4d 16B=%3u 9E=%4d shW=%4u pool=%d POOLHX=%4d%s\n",
                           seed, f, corpse_n, e->sub_state_1, e->sub_state_2, (int)e->motion,
                           (int)e->anim_frame, fc, kf, loco ? "LOCO" : "act ",
                           e->re2z_dir16a, (int)e->re2z_t158, (int)e->re2z_t15a, e->re2z_gaitrow,
                           (int)e->grab_kill_ctr, e->crow_shadow_w, e->crow_pool, hx,
                           tinted ? " TINT" : "");
                }
            }
            (void)printed_this;
        }

        if (death_f >= 0) deaths++;
        if (corpse_f >= 0 && corpse_n > 120) {
            corpses++;
            int nbits = 0; for (int i = 0; i < 16; i++) if (s1seen & (1u << i)) nbits++;
            if (nbits < 16) s1hist[nbits]++;
            int span = (int)(pool_max - pool_min);
            growhist[(span / 100 < 16) ? span / 100 : 15]++;
            /* C1: schon im ERSTEN Leichenframe >= 95% des Endwerts */
            if (pool_last > 0 && pool_first * 100 >= pool_last * 95) c1++;
            /* C2: gar kein Wachstum ueber die ganze Leichenphase */
            if (span == 0) c2++;
            /* C3: nur EIN +0x5-Wert in state 7 */
            if (nbits <= 1) c3++;
            /* C4: Keyframe bleibt ueber die letzten 100 Leichenframes unveraendert */
            if (kf_changes == 0) c4++;
            if (shown < 4) {
                shown++;
                printf("[LAUF] seed %3d slot %d: corpse ab f%d, %d Leichenframes | "
                       "POOL_HX first=%d min=%d max=%d last=%d (span %d) | +0x5-Werte:",
                       seed, slot, corpse_f, corpse_n, pool_first, pool_min, pool_max,
                       pool_last, span);
                for (int i = 0; i < 16; i++) if (s1seen & (1u << i)) printf(" %d", i);
                printf(" | kf-Wechsel=%d shW=%u 9E=%d\n", kf_changes, e->crow_shadow_w,
                       (int)e->grab_kill_ctr);
            }
        }
    }

    printf("\n=== %ld Laeufe, %ld Tode, %ld ausgewertete Leichen (>120 Frames) ===\n",
           runs, deaths, corpses);
    printf("(C1) Lache im ERSTEN Leichenframe schon >= 95%% des Endwerts : %ld\n", c1);
    printf("(C2) Lache waechst ueberhaupt nicht (span == 0)             : %ld\n", c2);
    printf("(C3) state 7 zeigt nur EINEN +0x5-Wert (keine Maschine)     : %ld\n", c3);
    printf("(C4) Leichen-Keyframe aendert sich NIE                      : %ld\n", c4);
    printf("--- Anzahl verschiedener +0x5-Werte in state 7:\n");
    for (int i = 0; i < 16; i++) if (s1hist[i]) printf("      %2d Werte: %ld x\n", i, s1hist[i]);
    printf("--- Wachstums-Spanne POOL_HX (100er-Klassen):\n");
    for (int i = 0; i < 16; i++) if (growhist[i]) printf("      %4d-%4d: %ld x\n",
                                                        i * 100, i * 100 + 99, growhist[i]);
    return 0;
}
