/**
 * @file probe_re2z_fallstand.c
 * @brief MESSSONDE — RNG-SEED-SWEEP fuer den DRITTEN Anlauf am RE2-Zombie-Sturz (nach v0.3.7):
 *   Nutzer: "Die Zombies fallen teilweise IMMER NOCH komisch hin, wenn sie einmal mit Waffe
 *            getroffen werden, oder haben manchmal eine kurze Hinfall-Animation, stehen dann
 *            aber sofort wieder."
 *
 * Beide Symptome sind SELTEN ("teilweise"/"manchmal") — ein Einzellauf zeigt sie nicht. Deshalb:
 * N Seeds x mehrere SCHUSS-FAHRPLAENE (ausdruecklich auch EINZELSCHUSS), jeweils ueber den
 * ECHTEN Weg (re15_game_step + Pad, echte ROOM1140-Sce_em_set-Spawns, GELADENE RE2-Bank —
 * ohne Bank ist re15_actor_clip_len()==0 und JEDE clip-getriebene Phase endet im Setz-Tick).
 *
 * DETEKTOREN (alle drei aus dem Auftrag):
 *   (1) FALL->STAND: das gerenderte SKELETT ist unten (Bone 8 dy > --dyth) und binnen < --amin
 *       Frames wieder oben. Gemessen wird die POSE, nicht der Zustand — nur daran sieht man,
 *       ob der Koerper wirklich faellt.
 *   (2) STURZ-ANIM BRICHT AB: Clip 1/2 (re2z_param_clips[0..1] @0x80100044, EXEC[5] P0
 *       @0x801032C8) wird verlassen, BEVOR anim_frame >= clip_len-1 (= re2z_clip_done), oder
 *       anim_frame springt zurueck, waehrend der Clip derselbe bleibt.
 *   (3) EINZELTREFFER-AUSGANG: nach GENAU EINEM Schuss wird die ganze Episode klassifiziert
 *       (Reaktions-Handler, tiefste Pose, ob und wie lange unten, Ausstiegs-Zustand).
 *   (4) POSE-SPRUNG ("komisch hinfallen"): dy8 aendert sich um > --jump Einheiten in EINEM Frame.
 *
 * Pro Frame protokolliert (Ringpuffer, bei Vorfall ausgegeben): clip/anim_frame/clip_len/
 * Frame-Slot, state/+0x5/+0x6, +0x1D3, +0x10E, +0x21A, +0x222, +0x223, +0x1D2, hp, hit_react,
 * Handler-Id und die Weltposition von Bone 8 (Brust) relativ zum Aktor-Boden.
 *
 * Aufruf: probe_re2z_fallstand [seeds=128] [frames=700] [plan=-1] [weapon=3]
 *                              [amin=25] [dyth=-1500] [jump=900]
 *   plan -1 = alle Fahrplaene 0..3 durchfahren
 *   plan  0 = GENAU EIN Schuss                (der Nutzer-Fall "einmal getroffen")
 *   plan  1 = ZWEI Schuesse, weit auseinander
 *   plan  2 = DREI Schuesse, weit auseinander
 *   plan  3 = Dauerfeuer-Takt (Referenz)
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
extern void     re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);
extern uint32_t re15_re2_rand(void);
extern int      re15_re2z_last_hit_handler(void);
extern int      re15_actor_uses_loco_bank(const re15_actor_t *a);
extern int      re15_re2z_poses_loco_bank(const re15_actor_t *a);   /* die volle Disasm-Karte */

/* A/B-SCHALTER: 0 = die HEUTIGE Port-Regel (re15_actor_uses_loco_bank, enemy_ai_common.c:4040),
 * 1 = die vollstaendige, disassemblierte Karte (re15_re2z_poses_loco_bank). Beide Laeufe sehen
 * dieselbe Engine und dieselben Seeds — nur die Bankwahl der POSE unterscheidet sich. */
static int s_bank_rule = 0;
static int bank_is_loco_now(const re15_actor_t *e)
{
    return s_bank_rule ? re15_re2z_poses_loco_bank(e) : re15_actor_uses_loco_bank(e);
}

/* ⛔ DIE POSE MUSS AUS DER BANK KOMMEN, AUS DER DER RENDERER POSIERT.
 * re15_enemy_bone_world_pos() (re15_damage.c:1566) posiert IMMER b->anim/b->skel — waehrend
 * platform/pc/main.c:6512-6523 fuer den RE2-Zombie im WALK/BUMP (state 1, +0x5 == 1/2) auf
 * b->anim_loco/b->skel_loco umschaltet (die EINE Regel re15_actor_uses_loco_bank,
 * enemy_ai_common.c:4040). Wer das ignoriert, misst fuer Clip 6 die 20-Frame-KRIECHER-Pose der
 * Aktions-Bank statt der 55-Frame-BUMP-Pose der Loco-Bank — und sieht einen "Sturz", den es auf
 * dem Bildschirm gar nicht gibt. Diese Sonde bildet die Renderer-Auswahl 1:1 nach. */
static int probe_bone_pos(const re15_actor_t *e, int bone, int32_t out[3], int *bank_is_loco)
{
    out[0] = e->x; out[1] = e->y; out[2] = e->z;
    if (bank_is_loco) *bank_is_loco = 0;
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b) return 0;
    const re15_emd_skeleton_t  *sk = &b->skel;
    const re15_emd_animation_t *an = &b->anim;
    int clip_override = -1;
    if (bank_is_loco_now(e) && b->loco_ok
        && (int)e->motion < b->anim_loco.clip_count) {
        sk = &b->skel_loco; an = &b->anim_loco; clip_override = (int)e->motion;
        if (bank_is_loco) *bank_is_loco = 1;
    }
    if (sk->bone_count <= 0 || bone < 0 || bone >= sk->bone_count) return 0;
    if (an->clip_count <= 0 || sk->keyframe_count <= 0) return 0;
    int kf = re15_compute_actor_kf(an, sk, e, clip_override, (uint32_t)e->anim_frame);
    re15_skel_pose_t poses[RE15_EMD_MAX_BONES];
    void *save = g_anim_pose_actor;
    g_anim_pose_actor = NULL;                       /* QUERY: Crossfade nicht anfassen */
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

/* ---- Ringpuffer je Lauf --------------------------------------------------------------------- */
typedef struct {
    int      f;
    uint8_t  st, s1, s2, r1d3, h93;
    uint16_t f10e, f21a, hd1d0;
    uint8_t  f222, c1d2, grid;
    int8_t   r223;
    int16_t  clip;
    int32_t  af;
    int      clen, slot;
    int      hp;
    int32_t  dy8, dy0;
    int      hitH;
    int      shot, hit, loco;
} row_t;
#define RING 1400
static row_t s_ring[RING]; static int s_ring_n = 0;

static void ring_dump(const char *why, int from, int to)
{
    printf("      --- %s ---\n", why);
    for (int i = 0; i < s_ring_n; i++) {
        const row_t *r = &s_ring[i];
        if (r->f < from || r->f > to) continue;
        printf("      f%-4d %s%s st=%u s1=%2u s2=%2u clip=%2d af=%3d/%2d slot=%2d %s"
               "| 1D3=%02X 10E=%04X 21A=%04X 222=%u 223=%4d 1D2=%u 1D0=%04X 93=%02X grid=%02X "
               "| h=%d hp=%4d b8dy=%-6d b0dy=%-6d\n",
               r->f, r->shot ? "S" : " ", r->hit ? "H" : " ", r->st, r->s1, r->s2,
               (int)r->clip, (int)r->af, r->clen, r->slot, r->loco ? "LOCO " : "act  ",
               r->r1d3, r->f10e, r->f21a,
               r->f222, (int)r->r223, r->c1d2, r->hd1d0, r->h93, r->grid,
               r->hitH, r->hp, r->dy8, r->dy0);
    }
}

/* ---- Aggregate ------------------------------------------------------------------------------ */
static long g_D1 = 0, g_D2 = 0, g_D4 = 0;
static int  g_D1seeds = 0, g_D2seeds = 0, g_D4seeds = 0;
static int  g_show1 = 0, g_show2 = 0, g_show4 = 0;
static long g_down_hist[600]; static long g_down_n = 0;
static int  g_down_min = 1 << 30, g_down_max = -1;
static long g_out_class[10];   /* Einzelschuss-Ausgang */
static long g_fallclip_ok = 0, g_fallclip_abort = 0;
static long g_D1_bankflip = 0, g_D4_bankflip = 0;   /* Vorfaelle MIT Bankwechsel an der Kante */

int main(int argc, char **argv)
{
    int seeds  = (argc > 1) ? atoi(argv[1]) : 128;
    int budget = (argc > 2) ? atoi(argv[2]) : 700;
    int planarg= (argc > 3) ? atoi(argv[3]) : -1;
    int weapon = (argc > 4) ? atoi(argv[4]) : 3;
    int amin   = (argc > 5) ? atoi(argv[5]) : 25;
    int dyth   = (argc > 6) ? atoi(argv[6]) : -1500;
    int jumpth = (argc > 7) ? atoi(argv[7]) : 900;
    s_bank_rule = (argc > 8) ? atoi(argv[8]) : 0;   /* 0 = Port-Regel heute, 1 = Disasm-Karte */

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    int plan_lo = (planarg < 0) ? 0 : planarg;
    int plan_hi = (planarg < 0) ? 3 : planarg;
    int banner  = 0;

    for (int plan = plan_lo; plan <= plan_hi; plan++)
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
        re15_player_set_equipped_weapon(weapon);
        {   int es = re15_inv_equipped_slot();
            if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

        /* SEED = Vorwuerfe auf dem einen 16-Bit-PRNG-State (@0x800CE318). */
        for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

        /* Die Sce_em_set-Spawns brauchen Frames (probe_re2_zfall faehrt dafuer 60 leere Ticks). */
        for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }

        /* EIN Zombie — nur so ist "EINMAL getroffen" eindeutig. */
        int slot = -1;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)
                && !(g_actors[s].grid_id & 0x80)) { slot = s; break; }
        if (slot < 0) { printf("FAIL: kein stehender Zombie (seed %d)\n", seed); continue; }
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
            printf("Bank 0x%02X: %s ACT bones=%d clips=%d | LOCO(Paar1) ok=%d bones=%d clips=%d "
                   "| OWN(Paar2) ok=%d clips=%d | VICTIM(Paar3) ok=%d clips=%d\n",
                   e->type, (b && b->ok) ? "geladen" : "FEHLT", b ? b->skel.bone_count : 0,
                   b ? b->anim.clip_count : 0, b ? b->loco_ok : -1,
                   b ? b->skel_loco.bone_count : 0, b ? b->anim_loco.clip_count : 0,
                   b ? b->own_ok : -1, b ? b->anim_own.clip_count : 0,
                   b ? b->victim_ok : -1, b ? b->anim_victim.clip_count : 0);
            if (b) {
                printf("  ACT  len:");
                for (int c = 0; c < b->anim.clip_count && c < 32; c++)
                    printf(" [%d]=%d", c, b->anim.clips[c].frame_count);
                printf("\n  LOCO len:");
                for (int c = 0; c < b->anim_loco.clip_count && c < 32; c++)
                    printf(" [%d]=%d", c, b->anim_loco.clips[c].frame_count);
                printf("\n");
                /* Brusthoehe je Clip/Frame 0 in BEIDEN Baenken — so ist eine "liegende" Pose
                 * von einer stehenden unterscheidbar, ohne zu raten. */
                re15_actor_t q = *e; q.rot_y = 0; q.anim_frame = 0; q.anim_freeze = 0;
                q.state = 1; q.sub_state_1 = 8;              /* -> Aktions-Bank */
                printf("  ACT  b8dy@f0:");
                for (int c = 0; c < b->anim.clip_count && c < 32; c++) {
                    q.motion = (int16_t)c; int32_t p[3];
                    if (probe_bone_pos(&q, 8, p, NULL)) printf(" [%d]=%d", c, p[1] - q.y);
                }
                q.sub_state_1 = 2;                            /* -> Loco-Bank (BUMP) */
                printf("\n  LOCO b8dy@f0:");
                for (int c = 0; c < b->anim_loco.clip_count && c < 32; c++) {
                    q.motion = (int16_t)c; int32_t p[3];
                    if (probe_bone_pos(&q, 8, p, NULL)) printf(" [%d]=%d", c, p[1] - q.y);
                }
                printf("\n");
            }
        }

        int shot_first = 30 + (seed % 23);
        int shot_gap   = 140 + (seed % 61);
        int nshots     = (plan == 0) ? 1 : (plan == 1) ? 2 : (plan == 2) ? 3 : 999;
        int shot_per   = (plan == 3) ? (7 + (seed % 11)) : shot_gap;

        s_ring_n = 0;
        int hp_last = e->hp, shots = 0, hits = 0;
        int down_start = -1, down_deep = 0;
        int32_t dy_prev = 0; int have_prev = 0;
        int fall_start = -1; int32_t fall_af = -1; int16_t fall_clip = -1; int fall_maxaf = -1;
        int hitD1 = 0, hitD2 = 0, hitD4 = 0;
        int firstHitF = -1, firstHandler = -1;
        int32_t deepest = -100000; int downFrames = 0; int sawFallClip = 0; int armed = 0;
        int loco_prev = 0, down_loco0 = 0;

        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            int want = 0;
            if (f >= shot_first && shots < nshots && ((f - shot_first) % shot_per) == 0) want = 1;
            if (want) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
            frame(cur, edge);
            if (want) shots++;

            int hit = (e->hp < hp_last); if (hit) { hits++; if (firstHitF < 0) firstHitF = f; }
            hp_last = e->hp;

            int32_t b8[3], b0[3]; int isloco = 0;
            probe_bone_pos(e, 8, b8, &isloco);
            probe_bone_pos(e, 0, b0, NULL);
            int32_t dy8 = b8[1] - e->y, dy0 = b0[1] - e->y;
            int clen = re15_actor_clip_len(e);
            int slotf = (clen > 0) ? (int)((uint32_t)e->anim_frame % (uint32_t)clen) : (int)e->anim_frame;
            int hH = re15_re2z_last_hit_handler();
            if (hit && firstHandler < 0) firstHandler = hH;

            if (s_ring_n < RING) {
                row_t *r = &s_ring[s_ring_n++];
                r->f = f; r->st = e->state; r->s1 = e->sub_state_1; r->s2 = e->sub_state_2;
                r->r1d3 = e->re2z_self1d3; r->h93 = e->hit_react; r->f10e = e->re2z_f10e;
                r->f21a = e->re2z_flags21a; r->hd1d0 = e->re2z_hitdir1d0;
                r->f222 = e->re2z_flag222; r->c1d2 = e->re2z_hits1d2; r->grid = e->grid_id;
                r->r223 = e->re2z_res223; r->clip = e->motion; r->af = (int32_t)e->anim_frame;
                r->clen = clen; r->slot = slotf; r->hp = e->hp; r->dy8 = dy8; r->dy0 = dy0;
                r->hitH = hH; r->shot = want; r->hit = hit; r->loco = isloco;
            }

            if (dy8 > deepest) deepest = dy8;

            /* Die Detektoren erst SCHARF machen, wenn der Zombie einmal wirklich aufrecht war —
             * die ROOM1140-Spawns starten liegend/aufstehend, das ist kein Sturz. */
            if (!armed) { if (dy8 < dyth) { armed = 1; have_prev = 0; } else { dy_prev = dy8; continue; } }

            /* ---- (4) POSE-SPRUNG ---- */
            if (have_prev) {
                int32_t d = dy8 - dy_prev; if (d < 0) d = -d;
                if (d > jumpth && e->hp >= 0) {
                    hitD4++;
                    if (isloco != loco_prev) g_D4_bankflip++;
                    if (g_show4 < 6) { g_show4++;
                        printf("[4-SPRUNG] plan %d seed %3d f%d: dy8 %d -> %d (%d in EINEM Frame) "
                               "st=%u s1=%u s2=%u clip=%d af=%d/%d\n", plan, seed, f, dy_prev,
                               dy8, dy8 - dy_prev, e->state, e->sub_state_1, e->sub_state_2,
                               (int)e->motion, (int)e->anim_frame, clen);
                        ring_dump("Kontext", f - 12, f + 4); }
                }
            }
            dy_prev = dy8; have_prev = 1;

            /* ---- (1) DOWN-Fenster ueber die POSE ---- */
            int down = (dy8 > dyth);
            if (down) downFrames++;
            if (down && down_start < 0) { down_start = f; down_deep = 0; down_loco0 = isloco; }
            if (down) { if (dy8 > -700) down_deep = 1; }
            if (!down && down_start >= 0) {
                int dur = f - down_start;
                if (e->hp >= 0) {
                    g_down_hist[(dur < 600) ? dur : 599]++; g_down_n++;
                    if (dur < g_down_min) g_down_min = dur;
                    if (dur > g_down_max) g_down_max = dur;
                    if (dur < amin) {
                        hitD1++;
                        if (isloco != down_loco0) g_D1_bankflip++;
                        if (g_show1 < 10) { g_show1++;
                            printf("[1-FALL/STAND] plan %d seed %3d slot %d: unten nur %d Frames "
                                   "(f%d..f%d, tiefste Pose %s) -> AUSGANG st=%u s1=%u s2=%u "
                                   "clip=%d 21A=%04X 10E=%04X hp=%d\n", plan, seed, slot, dur,
                                   down_start, f, down_deep ? "PRONE" : "nur halb", e->state,
                                   e->sub_state_1, e->sub_state_2, (int)e->motion,
                                   e->re2z_flags21a, e->re2z_f10e, e->hp);
                            ring_dump("Kontext", down_start - 10, f + 6); }
                    }
                }
                down_start = -1;
            }

            /* ---- (2) STURZ-CLIP-INTEGRITAET (Clip 1/2 = re2z_param_clips[0..1], EXEC[5] P1).
             * Nur in der Sturz-Kette pruefen: Clip 2 ist in der LOCO-Bank auch ein Gang-Clip. */
            int isfall = (e->state == 1 && e->sub_state_1 == 5 && !isloco
                          && (e->motion == 1 || e->motion == 2));
            if (isfall) sawFallClip = 1;
            if (isfall && fall_start < 0) {
                fall_start = f; fall_clip = e->motion; fall_af = (int32_t)e->anim_frame;
                fall_maxaf = (int)e->anim_frame;
            } else if (isfall && fall_start >= 0) {
                if (e->motion != fall_clip) {           /* Clipwechsel 1<->2 mitten drin */
                    fall_clip = e->motion; fall_af = (int32_t)e->anim_frame; fall_maxaf = (int)e->anim_frame;
                } else {
                    if ((int32_t)e->anim_frame < fall_af && e->hp >= 0) {
                        hitD2++;
                        if (g_show2 < 8) { g_show2++;
                            printf("[2-ANIM-RUECKSPRUNG] plan %d seed %3d f%d: clip=%d af %d -> %d "
                                   "(len %d) st=%u s1=%u s2=%u\n", plan, seed, f, (int)e->motion,
                                   fall_af, (int)e->anim_frame, clen, e->state, e->sub_state_1,
                                   e->sub_state_2);
                            ring_dump("Kontext", f - 14, f + 4); }
                    }
                    fall_af = (int32_t)e->anim_frame;
                    if ((int)e->anim_frame > fall_maxaf) fall_maxaf = (int)e->anim_frame;
                }
            } else if (!isfall && fall_start >= 0) {
                /* Sturz-Clip verlassen — lief er zu Ende? */
                re15_actor_t probe = *e; probe.motion = fall_clip;
                int fl = re15_actor_clip_len(&probe);
                if (fl > 0 && fall_maxaf < fl - 1 && e->hp >= 0) {
                    g_fallclip_abort++;
                    hitD2++;
                    if (g_show2 < 8) { g_show2++;
                        printf("[2-ANIM-ABBRUCH] plan %d seed %3d f%d: Sturzclip %d nur bis af=%d "
                               "von %d -> WEITER mit clip=%d st=%u s1=%u s2=%u 21A=%04X\n",
                               plan, seed, f, (int)fall_clip, fall_maxaf, fl, (int)e->motion,
                               e->state, e->sub_state_1, e->sub_state_2, e->re2z_flags21a);
                        ring_dump("Kontext", fall_start - 8, f + 6); }
                } else if (fl > 0) g_fallclip_ok++;
                fall_start = -1;
            }
            loco_prev = isloco;
        }

        /* ---- (3) EINZELTREFFER-AUSGANG ---- */
        if (plan == 0) {
            int cls;
            if (hits == 0)                    cls = 0;    /* nicht getroffen (Messfehler) */
            else if (e->hp < 0)               cls = 1;    /* tot */
            else if (!sawFallClip && downFrames == 0) cls = 2;  /* blieb stehen (erwartet) */
            else if (sawFallClip && downFrames >= amin) cls = 3;/* sauberer Sturz + lag */
            else if (sawFallClip && downFrames <  amin) cls = 4;/* Sturz, aber kaum unten */
            else if (!sawFallClip && downFrames >  0)   cls = 5;/* unten OHNE Sturzclip! */
            else                              cls = 6;
            g_out_class[cls]++;
        }

        if (hitD1) { g_D1 += hitD1; g_D1seeds++; }
        if (hitD2) { g_D2 += hitD2; g_D2seeds++; }
        if (hitD4) { g_D4 += hitD4; g_D4seeds++; }
        (void)firstHitF; (void)firstHandler; (void)deepest;
    }

    int runs = (plan_hi - plan_lo + 1) * seeds;
    printf("\n=== SWEEP: %d Seeds x %d Frames x Fahrplaene %d..%d = %d Laeufe (Waffe %d) "
           "| BANKREGEL: %s ===\n", seeds, budget, plan_lo, plan_hi, runs, weapon,
           s_bank_rule ? "DISASM-KARTE (re15_re2z_poses_loco_bank)"
                       : "PORT HEUTE (re15_actor_uses_loco_bank)");
    printf("(1) FALL->STAND  (Pose unten < %d Frames, Schwelle dy8 > %d): %ld Vorfaelle in %d/%d Laeufen"
           " [davon %ld mit BANKWECHSEL an der Kante]\n",
           amin, dyth, g_D1, g_D1seeds, runs, g_D1_bankflip);
    printf("(2) STURZ-ANIM abgebrochen/rueckgesprungen: %ld Vorfaelle in %d/%d Laeufen "
           "(Sturzclips vollstaendig %ld, abgebrochen %ld)\n", g_D2, g_D2seeds, runs,
           g_fallclip_ok, g_fallclip_abort);
    printf("(4) POSE-SPRUNG > %d Einheiten/Frame: %ld Vorfaelle in %d/%d Laeufen "
           "[davon %ld mit BANKWECHSEL]\n", jumpth, g_D4, g_D4seeds, runs, g_D4_bankflip);
    if (plan_lo == 0)
        printf("(3) EINZELSCHUSS-AUSGANG: nicht getroffen=%ld tot=%ld blieb-stehen=%ld "
               "sauberer-Sturz=%ld Sturz-aber-kaum-unten=%ld unten-OHNE-Sturzclip=%ld sonst=%ld\n",
               g_out_class[0], g_out_class[1], g_out_class[2], g_out_class[3],
               g_out_class[4], g_out_class[5], g_out_class[6]);
    printf("--- DOWN-Dauern (Pose): n=%ld min=%d max=%d | Histogramm:\n", g_down_n,
           (g_down_min == (1 << 30)) ? -1 : g_down_min, g_down_max);
    for (int i = 0; i < 600; i++) if (g_down_hist[i]) printf("      %3d Frames: %ld x\n", i, g_down_hist[i]);
    free(buf);
    return 0;
}
