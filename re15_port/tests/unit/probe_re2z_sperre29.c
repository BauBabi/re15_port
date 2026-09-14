/**
 * @file probe_re2z_sperre29.c
 * @brief MESSSONDE (reine Diagnose, KEIN Pin, Runde 13): WAS sperrt einen lebenden RE2-Zombie
 *        200-400 Bilder am Stueck gegen den Kandidatenfilter?
 *
 * Faehrt den Sweep von test_re2_zombie_abc::run_sweep ZEICHENGLEICH (echter game_step-Weg,
 * geladene RE2-Baenke EM010/011/016, dieselben Seeds, dasselbe Schussraster), nimmt dabei
 * fuer JEDEN Aktor und JEDES Bild einen vollstaendigen Schnappschuss auf und wertet am Ende
 * jedes Seeds aus:
 *   - jede Sperr-Episode ((hp >= 0) && (hit_react & 1)) >= RE15_MIN Bilder,
 *   - fuer diese Episoden das komplette Bild-fuer-Bild-Protokoll
 *     (state/s1/s2/s3, hp, +0x1D3, +0x10E, clip, anim_frame, clip_len, anim_frac, anim_flags,
 *      +0x21A, +0x15A, +0x158, +0x144, grid_id, +0x93) samt der vier Filter-Gates.
 *
 * Umgebung: RE15_SEEDS (64), RE15_FRAMES (900), RE15_WEAPON (3), RE15_MIN (150),
 *           RE15_ONLYSEED (-1 = alle), RE15_CTX (20 Vorlauf-Bilder vor der Episode).
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
extern uint32_t re15_re2_rand(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static uint8_t           *s_ems = NULL; static size_t s_ems_n = 0;

#define MAXF 8000

typedef struct {
    uint8_t  act, st, s1, s2, s3, frac, grid, hr, shot;
    int16_t  hp, t15a, t158, sh;
    uint8_t  d1d3;
    uint16_t f10e, aflg, f21a, af;
    int16_t  clip;
    int16_t  fc;
    int32_t  x, z;
    uint8_t  typ, useloco, poseloco, mid;
    int16_t  act_fc, loco_fc;
} snap_t;

static snap_t s_snap[MAXF][RE15_ACTOR_MAX];

/* Globale Lage je Bild — fuer die Frage "haengt der Zombie, oder haengt das GANZE Bild?" */
typedef struct {
    uint32_t pause;
    uint8_t  modal, pl_st, pl_s1, pl_dead, msg_open;
    int16_t  pl_hp, pl_mot;
    uint16_t pl_af;
} gsnap_t;
static gsnap_t s_g[MAXF];
extern int re15_item_modal_active(void);
extern int re15_player_is_dead(void);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

static int envi(const char *k, int def)
{
    const char *v = getenv(k);
    return (v && *v) ? atoi(v) : def;
}

static void gates(const snap_t *s, int g[5])
{
    int spawn_pose = (s->st == 1) && (s->s1 == 7 || s->s1 == 8);
    g[0] = (s->act != 0);
    g[1] = (spawn_pose || s->d1d3 == 0u);
    g[2] = (s->hp >= 0);
    g[3] = (spawn_pose || !(s->f10e & 0xC000u));
    g[4] = spawn_pose;
}

static void dump_line(int f, int slot, const snap_t *s)
{
    int g[5]; gates(s, g);
    int blocked = (s->hp >= 0) && (s->hr & 1u);
    printf("  f%4d %s sl%d st=%u s1=%u s2=%u s3=%u hp=%4d 1D3=0x%02X 10E=0x%04X clip=%2d "
           "af=%3u fc=%3d frac=%u aflg=0x%04X 21A=0x%04X 15A=%5d 158=%5d 144=%3d grid=0x%02X "
           "93=0x%02X G[akt%d 1D3%d hp%d 10E%d pose%d] loco=%d/%d actfc=%d locofc=%d mid=%d "
           "typ=0x%02X %s\n",
           f, s->shot ? "S" : ".", slot, s->st, s->s1, s->s2, s->s3, (int)s->hp, s->d1d3,
           s->f10e, (int)s->clip, s->af, (int)s->fc, s->frac, s->aflg, s->f21a,
           (int)s->t15a, (int)s->t158, (int)s->sh, s->grid, s->hr,
           g[0], g[1], g[2], g[3], g[4], (int)s->useloco, (int)s->poseloco,
           (int)s->act_fc, (int)s->loco_fc, (int)s->mid, s->typ,
           blocked ? "GESPERRT" : "frei");
}

static long g_incidents = 0;

static void run_one(int seed, int budget, int weapon, int minlen, int ctx, int verbose)
{
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

    (void)(load_bank2(0x10) && load_bank2(0x11) && load_bank2(0x16));

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();

    {   int64_t sx = 0, szz = 0; int n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (is_zombie(&g_actors[s])) { sx += g_actors[s].x; szz += g_actors[s].z; n++; }
        if (n) { pl->x = (int32_t)(sx / n) + 2200 + (seed % 7) * 300;
                 pl->z = (int32_t)(szz / n) + 1500 - (seed % 5) * 400; }
        pl->rot_y = (int16_t)((seed * 337) & 0xfff);
    }
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    int shot_period = 7 + (seed % 11), shot_first = 20 + (seed % 17);
    if (budget > MAXF) budget = MAXF;

    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
        int shot = (f >= shot_first && ((f - shot_first) % shot_period) == 0);
        if (shot) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
        frame(cur, edge);
        {   gsnap_t *G = &s_g[f];
            G->pause   = g_re15_pauseflags;
            G->modal   = (uint8_t)re15_item_modal_active();
            G->pl_dead = (uint8_t)re15_player_is_dead();
            G->pl_st   = pl->state; G->pl_s1 = pl->sub_state_1;
            G->pl_hp   = pl->hp;    G->pl_mot = (int16_t)pl->motion;
            G->pl_af   = (uint16_t)pl->anim_frame;
            G->msg_open = 0;
        }
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            snap_t *q = &s_snap[f][s];
            q->act = (uint8_t)(is_zombie(e) ? 1 : 0);
            q->shot = (uint8_t)shot;
            q->st = e->state; q->s1 = e->sub_state_1; q->s2 = e->sub_state_2; q->s3 = e->sub_state_3;
            q->hp = e->hp; q->d1d3 = e->re2z_self1d3; q->f10e = e->re2z_f10e;
            q->clip = (int16_t)e->motion; q->af = (uint16_t)e->anim_frame;
            q->fc = (int16_t)re15_actor_clip_len(e); q->frac = e->anim_frac;
            q->aflg = e->anim_flags; q->f21a = e->re2z_flags21a;
            q->t15a = e->re2z_t15a; q->t158 = e->re2z_t158; q->sh = e->speed_h;
            q->grid = e->grid_id; q->hr = e->hit_react;
            q->x = e->x; q->z = e->z;
            q->typ = e->type;
            q->useloco  = (uint8_t)re15_actor_uses_loco_bank(e);
            q->poseloco = (uint8_t)re15_re2z_poses_loco_bank(e);
            q->mid = e->motion_init_delay;
            q->act_fc = q->loco_fc = -1;
            if (q->act) {
                re15_enemy_bank_t *bb = re15_enemy_find(e->type);
                int mo = (int)e->motion;
                if (bb && bb->ok && mo >= 0) {
                    if (mo < bb->anim.clip_count)
                        q->act_fc = (int16_t)bb->anim.clips[mo].frame_count;
                    if (bb->loco_ok && mo < bb->anim_loco.clip_count)
                        q->loco_fc = (int16_t)bb->anim_loco.clips[mo].frame_count;
                } }
        }
    }

    /* ---- Harness-Wache: ab wann friert das ITEM-GET-MODAL den ganzen Schritt ein? -------
     * re15_game_step kehrt bei aktivem Modal SOFORT zurueck (game_step_common.c:866,
     * byte-true FUN_8001db28 g_pauseflags |= 0xff000000). Die Modal-FSM wird im echten
     * Spiel von der Plattform-Schleife getickt — in einem headless-Test NIE. Ab diesem
     * Bild misst der Sweep also nichts mehr. */
    {   int mf = -1;
        for (int f = 0; f < budget; f++) if (s_g[f].modal) { mf = f; break; }
        printf("[HARNESS] seed=%d  Item-Modal ab Bild %d (-1 = nie), am Budget-Ende %s\n",
               seed, mf, s_g[budget - 1].modal ? "NOCH OFFEN" : "zu");
    }

    /* ---- Auswertung: Sperr-Episoden je Slot --------------------------------------------- */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        int start = -1;
        for (int f = 0; f <= budget; f++) {
            snap_t *q = (f < budget) ? &s_snap[f][s] : NULL;
            int blocked = q && q->act && (q->hp >= 0) && (q->hr & 1u);
            if (blocked && start < 0) start = f;
            else if (!blocked && start >= 0) {
                int dur = f - start;
                if (dur >= minlen) {
                    g_incidents++;
                    snap_t *a = &s_snap[start][s], *b = &s_snap[f - 1][s];
                    printf("\n[SPERRE] seed=%d slot=%d  f%d..f%d  DAUER %d Bilder\n"
                           "   Beginn st=%u s1=%u s2=%u 1D3=0x%02X 10E=0x%04X clip=%d af=%u\n"
                           "   Ende   st=%u s1=%u s2=%u 1D3=0x%02X 10E=0x%04X clip=%d af=%u\n",
                           seed, s, start, f - 1, dur,
                           a->st, a->s1, a->s2, a->d1d3, a->f10e, (int)a->clip, a->af,
                           b->st, b->s1, b->s2, b->d1d3, b->f10e, (int)b->clip, b->af);
                    if (verbose) {
                        int lo = start - ctx; if (lo < 0) lo = 0;
                        int hi = f + ctx;     if (hi > budget) hi = budget;
                        int all = envi("RE15_ALLSLOTS", 0);
                        for (int k = lo; k < hi; k++) {
                            if (all) { for (int t = 1; t < RE15_ACTOR_MAX; t++)
                                           if (s_snap[k][t].act) dump_line(k, t, &s_snap[k][t]); }
                            else dump_line(k, s, &s_snap[k][s]);
                            printf("        GLOBAL f%4d pause=0x%08X modal=%d msg=%d pl(st=%u "
                                   "s1=%u hp=%d mot=%d af=%u dead=%d)\n",
                                   k, s_g[k].pause, s_g[k].modal, s_g[k].msg_open, s_g[k].pl_st,
                                   s_g[k].pl_s1, (int)s_g[k].pl_hp, (int)s_g[k].pl_mot,
                                   s_g[k].pl_af, s_g[k].pl_dead);
                        }
                    }
                }
                start = -1;
            }
        }
    }
}

int main(void)
{
    setvbuf(stdout, NULL, _IONBF, 0);
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    int seeds   = envi("RE15_SEEDS", 64);
    int budget  = envi("RE15_FRAMES", 900);
    int weapon  = envi("RE15_WEAPON", 3);
    int minlen  = envi("RE15_MIN", 150);
    int only    = envi("RE15_ONLYSEED", -1);
    int ctx     = envi("RE15_CTX", 20);
    int verbose = envi("RE15_VERBOSE", 1);

    printf("== SWEEP  seeds=%d frames=%d waffe=%d minlen=%d ==\n", seeds, budget, weapon, minlen);
    for (int s = 0; s < seeds; s++) {
        int v = verbose && (only < 0 || s == only);
        run_one(s, budget, weapon, minlen, ctx, v);
    }
    printf("\n== %ld Sperr-Episoden >= %d Bilder ==\n", g_incidents, minlen);
    free(buf);
    return 0;
}
