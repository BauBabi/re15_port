/**
 * @file probe_re2z_abc.c
 * @brief MESSSONDE — RNG-SEED-SWEEP fuer die drei Nutzer-Symptome (v0.3.5, RE2-Modus):
 *   (A) "Die Zombies fallen manchmal komisch um und stehen dann direkt wieder auf."
 *   (B) "Manchmal haben sie eine Laufanimation, aber laufen nicht."
 *   (C) "Es kommt vor, dass die Zombies unsterblich sind."
 *
 * Alle drei sind "manchmal" — ein Einzellauf faengt sie nicht. Diese Sonde faehrt N Laeufe
 * mit verschobenem RE2-PRNG-Zustand (der Generator hat EINEN 16-Bit-State @0x800CE318, also
 * ist "Seed" = Anzahl Vorwuerfe nach dem Room-Reset) UND verschobenem Schuss-Fahrplan, jeweils
 * ueber den ECHTEN Weg: re15_game_step + Pad, echte ROOM1140-Sce_em_set-Spawns, GELADENE
 * RE2-Baenke (ohne Bank ist re15_actor_clip_len()==0 -> jede clip-getriebene Phase endet im
 * Setz-Tick und die Messung ist wertlos).
 *
 * Pro Frame und Zombie protokolliert: state/+0x5/+0x6, grid_id(+0x9), +0x1D3, +0x10E, hp,
 * hit_react(+0x93), motion/anim_frame, POSITION (x/z) und die Weltposition von Bone 8.
 *
 * Detektoren:
 *   (A) DOWN-Fenster (state 1/+0x5=5 Sturz-Executor oder HURT-Ragdoll/Knockdown) kuerzer als
 *       --amin Frames -> "faellt und steht sofort wieder".
 *   (B) WALK (state 1/+0x5=1) ueber >= --bwin Frames mit |dx|+|dz| < --beps -> Laufanim ohne Weg.
 *   (C) hp >= 0 UND Kandidatenfilter sperrt (hit_react & 1) ueber >= --cwin Frames -> unsterblich;
 *       zusaetzlich wird die SPERRENDE URSACHE mitgefuehrt (1D3 / HP / 10E&0xC000).
 *
 * Aufruf: probe_re2z_abc [seeds=64] [frames=900] [mode]
 *   mode 0 = Standard (Spieler lebt, Dauerbeschuss nach Fahrplan)
 *   mode 1 = Spieler wird NICHT am Leben gehalten (Grab-Kill erlaubt) -> EXEC[6]-Pfad
 *   mode 2 = nur beobachten, keine Schuesse
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
extern uint32_t re15_re2_rand_draws(void);

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

static long down_hist[400]; static long down_n = 0;
static int  down_min = 1 << 30, down_max = -1;
static long walk_hist[200]; static long walk_n = 0; static int walk_min = 1 << 30;
static long cblock[5]; static int cother = 0; static int bshow = 0; static int ashow = 0;
static long crawl_frames = 0, crawl_enter = 0, crawl_walkframes = 0;
static long cen_st_g[9][20];   /* [state][sub_state_1] Frame-Zensus ueber ALLE Seeds */
static long cen_walk_move_g = 0, cen_walk_frames_g = 0, cen_deaths_g = 0, cen_hits_g = 0;
#define cen_st         cen_st_g
#define cen_walk_move  cen_walk_move_g
#define cen_walk_frames cen_walk_frames_g
#define cen_deaths     cen_deaths_g
#define cen_hits       cen_hits_g

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

/* ---- Detektor-Zustand je Slot -------------------------------------------------------------- */
typedef struct {
    int  down_start;      /* Frame, an dem das DOWN-Fenster begann (-1 = nicht unten) */
    int  down_kind;       /* 1 = EXEC[5] Sturz, 2 = HURT-Reaktion */
    int  walk_start;      /* Frame, an dem WALK begann */
    int32_t walk_x, walk_z;
    int  block_start;     /* Frame, ab dem der Filter sperrt und hp >= 0 */
    uint8_t block_1d3; uint16_t block_10e;
    int  prev_state, prev_s1;
    int32_t prev_x, prev_z;
    uint16_t prev_10e;
    long walk_path; uint16_t walk_af0; int walk_afn;
} det_t;

int main(int argc, char **argv)
{
    int seeds  = (argc > 1) ? atoi(argv[1]) : 64;
    int budget = (argc > 2) ? atoi(argv[2]) : 900;
    int mode   = (argc > 3) ? atoi(argv[3]) : 0;
    int amin   = (argc > 4) ? atoi(argv[4]) : 30;    /* (A) DOWN kuerzer als N Frames */
    int bwin   = (argc > 5) ? atoi(argv[5]) : 60;    /* (B) WALK-Fenster */
    int beps   = (argc > 6) ? atoi(argv[6]) : 120;   /* (B) Weg-Schwelle in Einheiten */
    int cwin   = (argc > 7) ? atoi(argv[7]) : 200;   /* (C) Sperr-Fenster */
    int weapon = (argc > 8) ? atoi(argv[8]) : 3;     /* 3 = Pistole */

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    int totA = 0, totB = 0, totC = 0, seedsA = 0, seedsB = 0, seedsC = 0;
    int worstA = 1 << 30, worstBmove = 1 << 30, worstC = 0;

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
        if (seed == 0) {
            for (int t = 0x10; t <= 0x16; t++) {
                if (t != 0x10 && t != 0x11 && t != 0x16) continue;
                re15_enemy_bank_t *b = re15_enemy_find((uint8_t)t);
                printf("Bank 0x%02X: %s bones=%d clips=%d loco_ok=%d own_ok=%d\n", t,
                       (b && b->ok) ? "geladen" : "FEHLT", b ? b->skel.bone_count : 0,
                       b ? b->anim.clip_count : 0, b ? b->loco_ok : -1, b ? b->own_ok : -1);
            }
        }

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

        /* Spieler in die Naehe der Spawn-Gruppe stellen (Mittelwert der Zombie-Positionen). */
        {   int64_t sx = 0, szz = 0; int n = 0;
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (is_zombie(&g_actors[s])) { sx += g_actors[s].x; szz += g_actors[s].z; n++; }
            if (n) { pl->x = (int32_t)(sx / n) + 2200 + (seed % 7) * 300;
                     pl->z = (int32_t)(szz / n) + 1500 - (seed % 5) * 400; }
            pl->rot_y = (int16_t)((seed * 337) & 0xfff);
        }
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

        det_t d[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) {
            d[s].down_start = -1; d[s].down_kind = 0; d[s].walk_start = -1;
            d[s].walk_x = 0; d[s].walk_z = 0; d[s].block_start = -1;
            d[s].block_1d3 = 0; d[s].block_10e = 0; d[s].prev_state = -1; d[s].prev_s1 = -1;
            d[s].prev_x = g_actors[s].x; d[s].prev_z = g_actors[s].z;
            d[s].prev_10e = g_actors[s].re2z_f10e;
            d[s].walk_path = 0; d[s].walk_af0 = 0; d[s].walk_afn = 0;
        }
        int hitA = 0, hitB = 0, hitC = 0;
        int shot_period = 7 + (seed % 11);
        int shot_first  = 20 + (seed % 17);
        int hp_prev[RE15_ACTOR_MAX];
        for (int s = 0; s < RE15_ACTOR_MAX; s++) hp_prev[s] = g_actors[s].hp;

        for (int f = 0; f < budget; f++) {
            if (mode != 1) pl->hp = 100;
            else if (pl->hp <= 0) pl->hp = 0;                   /* Tod zulassen */
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (mode != 2 && f >= shot_first && ((f - shot_first) % shot_period) == 0) {
                cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
            }
            frame(cur, edge);

            /* Einzelfall-TRACE: RE15_TRACE_SEED / RE15_TRACE_SLOT */
            {   const char *ts = getenv("RE15_TRACE_SEED"), *tl = getenv("RE15_TRACE_SLOT");
                if (ts && tl && atoi(ts) == seed) {
                    int sl = atoi(tl);
                    re15_actor_t *e = &g_actors[sl];
                    static uint64_t lastsig = ~0ull;
                    uint64_t sig = ((uint64_t)e->state << 48) | ((uint64_t)e->sub_state_1 << 40) |
                                   ((uint64_t)e->sub_state_2 << 32) | ((uint64_t)e->re2z_self1d3 << 24) |
                                   ((uint64_t)e->re2z_f10e << 8) | (uint64_t)(e->motion & 0xff);
                    if (sig != lastsig) {
                        printf("  T f%-4d st=%u s1=%2u s2=%2u 1D3=%02X 10E=%04X 21A=%04X 222=%u "
                               "grid=%02X clip=%2d af=%3u len=%2d hp=%4d 93=%02X\n",
                               f, e->state, e->sub_state_1, e->sub_state_2, e->re2z_self1d3,
                               e->re2z_f10e, e->re2z_flags21a, e->re2z_flag222, e->grid_id,
                               (int)e->motion, (unsigned)e->anim_frame, re15_actor_clip_len(e),
                               e->hp, e->hit_react);
                        lastsig = sig;
                    }
                }
            }

            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!is_zombie(e)) { d[s].down_start = -1; d[s].walk_start = -1;
                                     d[s].block_start = -1; continue; }
                if (e->state < 9 && e->sub_state_1 < 20) cen_st[e->state][e->sub_state_1]++;
                if ((e->re2z_f10e & 1u) && e->hp >= 0) {
                    crawl_frames++;
                    if (!(d[s].prev_10e & 1u)) {
                        crawl_enter++;
                        if (crawl_enter <= 3)
                            printf("[KRIECHER] seed %d slot %d f%d: +0x10E=%04X st=%u s1=%u s2=%u "
                                   "clip=%d hp=%d  -> Port faehrt WEITER die aufrechte Wurzel\n",
                                   seed, s, f, e->re2z_f10e, e->state, e->sub_state_1,
                                   e->sub_state_2, (int)e->motion, e->hp);
                    }
                    if (e->state == 1 && (e->sub_state_1 == 0 || e->sub_state_1 == 1))
                        crawl_walkframes++;
                }
                d[s].prev_10e = e->re2z_f10e;
                if (e->hp < hp_prev[s]) cen_hits++;
                if (e->hp < 0 && hp_prev[s] >= 0) cen_deaths++;
                hp_prev[s] = e->hp;
                if (e->state == 1 && e->sub_state_1 == 1 && d[s].prev_state == 1
                    && d[s].prev_s1 == 1) {
                    int32_t dx = e->x - d[s].prev_x, dz = e->z - d[s].prev_z;
                    cen_walk_frames++;
                    cen_walk_move += (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                }

                /* ---- (A) DOWN-Fenster ---- */
                int down = (e->state == 1 && e->sub_state_1 == 5) ? 1
                         : (e->state == 2 && (e->sub_state_1 == 11 || e->sub_state_1 == 12)) ? 2 : 0;
                if (down && d[s].down_start < 0) { d[s].down_start = f; d[s].down_kind = down; }
                if (!down && d[s].down_start >= 0) {
                    int dur = f - d[s].down_start;
                    int upright = (e->state == 1 && (e->sub_state_1 == 0 || e->sub_state_1 == 1));
                    if (e->hp >= 0) { down_hist[(dur < 400) ? dur : 399]++; down_n++;
                                      if (dur < down_min) down_min = dur;
                                      if (dur > down_max) down_max = dur;
                                      if (dur < amin && ashow < 8) { ashow++;
                                          printf("[A?] seed %2d slot %d: DOWN nur %d F (f%d) -> "
                                                 "AUSGANG st=%u s1=%u s2=%u clip=%d 1D3=%02X "
                                                 "10E=%04X hp=%d (upright=%d)\n", seed, s, dur,
                                                 d[s].down_start, e->state, e->sub_state_1,
                                                 e->sub_state_2, (int)e->motion, e->re2z_self1d3,
                                                 e->re2z_f10e, e->hp, upright); } }
                    if (upright && dur < amin && e->hp >= 0) {
                        if (!hitA) printf("[A] seed %2d slot %d: DOWN(kind %d) f%d..f%d = %d Frames "
                                          "-> st=%u s1=%u clip=%d af=%u 21A=%04X 10E=%04X\n",
                                          seed, s, d[s].down_kind, d[s].down_start, f, dur,
                                          e->state, e->sub_state_1, (int)e->motion,
                                          (unsigned)e->anim_frame, e->re2z_flags21a, e->re2z_f10e);
                        hitA++; if (dur < worstA) worstA = dur;
                    }
                    d[s].down_start = -1;
                }

                /* ---- (B) WALK ohne Weg ---- */
                int walking = (e->state == 1 && e->sub_state_1 == 1 && e->hp >= 0);
                if (walking && d[s].walk_start < 0) {
                    d[s].walk_start = f; d[s].walk_x = e->x; d[s].walk_z = e->z;
                    d[s].walk_path = 0; d[s].walk_af0 = e->anim_frame; d[s].walk_afn = 0;
                }
                if (walking && d[s].prev_state == 1 && d[s].prev_s1 == 1) {
                    int32_t px = e->x - d[s].prev_x, pz = e->z - d[s].prev_z;
                    d[s].walk_path += (px < 0 ? -px : px) + (pz < 0 ? -pz : pz);  /* WEGLAENGE */
                    d[s].walk_afn++;
                }
                if (walking && d[s].walk_start >= 0 && f - d[s].walk_start >= bwin) {
                    int32_t dx = e->x - d[s].walk_x, dz = e->z - d[s].walk_z;
                    int32_t mv = (dx < 0 ? -dx : dx) + (dz < 0 ? -dz : dz);
                    if (mv < walk_min) walk_min = mv;
                    walk_hist[(mv < 4000) ? mv / 20 : 199]++; walk_n++;
                    if (mv < beps) {
                        if (bshow < 14) { bshow++; printf("[B] seed %2d slot %d: WALK f%d..f%d (%d F) "
                                          "NETTO=%d WEGLAENGE=%ld  clip=%d len=%d af=%u->%u "
                                          "21A=%04X dist=%u pos=(%d,%d)\n",
                                          seed, s, d[s].walk_start, f, f - d[s].walk_start, mv,
                                          d[s].walk_path, (int)e->motion, re15_actor_clip_len(e),
                                          (unsigned)d[s].walk_af0, (unsigned)e->anim_frame,
                                          e->re2z_flags21a, e->ai_dist, e->x, e->z); }
                        hitB++; if (mv < worstBmove) worstBmove = mv;
                    }
                    d[s].walk_start = f; d[s].walk_x = e->x; d[s].walk_z = e->z;   /* Fenster neu */
                    d[s].walk_path = 0; d[s].walk_af0 = e->anim_frame;
                }
                if (!walking) d[s].walk_start = -1;

                /* ---- (C) unsterblich ---- */
                int blocked = (e->hp >= 0) && (e->hit_react & 1u);
                if (blocked && d[s].block_start < 0) {
                    d[s].block_start = f; d[s].block_1d3 = e->re2z_self1d3; d[s].block_10e = e->re2z_f10e;
                }
                if (!blocked) d[s].block_start = -1;
                if (blocked && f - d[s].block_start == cwin) {
                    {   int ci = (e->state == 1 && e->sub_state_1 == 7) ? 0
                               : (e->state == 1 && e->sub_state_1 == 8) ? 1
                               : (e->state == 1 && e->sub_state_1 == 5) ? 2
                               : (e->state == 2) ? 3 : 4;
                        cblock[ci]++;
                        if (ci == 4 && cother < 6) {
                            cother++;
                            printf("[C-ANDERE] seed %d slot %d f%d: st=%u s1=%u s2=%u grid=0x%02X "
                                   "hp=%d 1D3=%02X 10E=%04X clip=%d\n", seed, s, f, e->state,
                                   e->sub_state_1, e->sub_state_2, e->grid_id, e->hp,
                                   e->re2z_self1d3, e->re2z_f10e, (int)e->motion);
                        }
                    }
                    if (0) printf("[C] seed %2d slot %d: GESPERRT seit f%d (%d Frames) "
                                      "hp=%d 1D3=%02X 10E=%04X grid=0x%02X st=%u s1=%u s2=%u "
                                      "clip=%d | Ursache: %s%s%s\n",
                                      seed, s, d[s].block_start, cwin, e->hp, e->re2z_self1d3,
                                      e->re2z_f10e, e->grid_id, e->state, e->sub_state_1,
                                      e->sub_state_2, (int)e->motion,
                                      e->re2z_self1d3 ? "+0x1D3 " : "",
                                      (e->re2z_f10e & 0xC000u) ? "+0x10E&0xC000 " : "",
                                      (!e->active) ? "inaktiv" : "");
                    hitC++; if (cwin > worstC) worstC = cwin;
                }
                d[s].prev_state = e->state; d[s].prev_s1 = e->sub_state_1;
                d[s].prev_x = e->x; d[s].prev_z = e->z;
            }
        }
        totA += hitA; totB += hitB; totC += hitC;
        if (hitA) seedsA++;
        if (hitB) seedsB++;
        if (hitC) seedsC++;
    }

    printf("\n=== SWEEP: %d Seeds x %d Frames, mode %d ===\n", seeds, budget, mode);
    printf("(A) Fall->sofort Stand (< %d Frames unten): %d Vorfaelle in %d/%d Seeds (kuerzestes %d)\n",
           amin, totA, seedsA, seeds, (worstA == (1 << 30)) ? -1 : worstA);
    printf("(B) Laufanim ohne Weg (%d Frames, < %d Einheiten): %d Vorfaelle in %d/%d Seeds "
           "(kleinster Weg %d)\n", bwin, beps, totB, seedsB, seeds,
           (worstBmove == (1 << 30)) ? -1 : worstBmove);
    printf("(C) Untreffbar >= %d Frames bei hp>=0: %d Vorfaelle in %d/%d Seeds\n",
           cwin, totC, seedsC, seeds);
    printf("--- Zensus: %ld Treffer, %ld Tode; WALK %ld Frames, Weg gesamt %ld (%.2f/Frame)\n",
           cen_hits_g, cen_deaths_g, cen_walk_frames_g, cen_walk_move_g,
           cen_walk_frames_g ? (double)cen_walk_move_g / (double)cen_walk_frames_g : 0.0);
    printf("--- (C) nach Ort: EXEC[7] liegend=%ld  EXEC[8] fressend=%ld  EXEC[5] Sturz=%ld  "
           "HURT=%ld  SONST=%ld\n", cblock[0], cblock[1], cblock[2], cblock[3], cblock[4]);
    printf("--- KRIECHER (+0x10E&1): %ld Eintritte, %ld Frames davon %ld in der AUFRECHTEN "
           "Stand/Walk-Wurzel (Original: Tabelle @0x8010C854[odd] = 0x80101210)\n",
           crawl_enter, crawl_frames, crawl_walkframes);
    printf("--- DOWN-Dauern (state1/+0x5=5): n=%ld min=%d max=%d | Histogramm:\n",
           down_n, (down_min == (1 << 30)) ? -1 : down_min, down_max);
    for (int i = 0; i < 400; i++) if (down_hist[i]) printf("      %3d Frames: %ld x\n", i, down_hist[i]);
    printf("--- WALK-Fenster (%d Frames): n=%ld kleinster Weg=%d | Histogramm (20er-Bins):\n",
           bwin, walk_n, (walk_min == (1 << 30)) ? -1 : walk_min);
    for (int i = 0; i < 200; i++) if (walk_hist[i]) printf("      %4d..%4d: %ld x\n",
                                                           i * 20, i * 20 + 19, walk_hist[i]);
    for (int st = 0; st < 9; st++) {
        int any = 0;
        for (int u = 0; u < 20; u++) if (cen_st_g[st][u]) any = 1;
        if (!any) continue;
        printf("    state %d:", st);
        for (int u = 0; u < 20; u++) if (cen_st_g[st][u]) printf(" [%d]=%ld", u, cen_st_g[st][u]);
        printf("\n");
    }
    free(buf);
    return 0;
}
