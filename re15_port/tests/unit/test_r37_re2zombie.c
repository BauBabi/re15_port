/**
 * @file test_r37_re2zombie.c
 * @brief Runde 37 Spur L "re2zombie" — Sonden und Riegel der RE2-Zombie-KI (enemy_ai_re2_zombie.c).
 *        Dossier analysis/befunde_runde37/L_re2zombie.md.
 *
 * Modus "mess <maske> [seeds] [waffe]": der Messer-Sweep von test_re2_zombie_abc (ROOM1140, echte Spawns, RE2-Baenke,
 *   echter re15_game_step, Spieler steht mit R1 und schlaegt alle 7..17 Bilder) mit waehlbarer Produzenten-Maske
 *   (include/re15_re2zombie_r37.h). Ausgabe je Seed: Treffer, Griff-Bilder, Wechsel nach 0x201 je Produzent.
 * Modus "zeit <maske> <seed> [waffe] [von] [bis]": Bild fuer Bild Spieler + Zombies desselben Laufs.
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
#include "re15_item_modal.h"
#include "re15_emd.h"
#include "re2_ems.h"
#include "re15_re2zombie_r37.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void     re15_player_aim_reset(void);
extern void     re15_player_set_aim_clip_len(int fc);
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

static long s_modal_auf, s_modal_im_griff;
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
    if (re15_item_modal_active()) { s_modal_auf++; if (re15_player_is_grabbed()) s_modal_im_griff++; }
    {   int schutz = 0;   /* Item-Modal leerfahren wie test_re2_zombie_abc frame() (182db277) */
        while (re15_item_modal_active() && schutz++ < 600) re15_item_modal_tick(0x4000, 0);
    }
}

static int is_zombie(const re15_actor_t *e) { return e->active && re15_re2z_owns_type(e->type); }

typedef struct {
    long hits, deaths, grab_frames, s2_frames, s3_frames, walk_frames, modal_auf, modal_im_griff;
    unsigned prod[3];
    uint32_t draws;
} lauf_t;

/* Aufbau wortgleich zu test_re2_zombie_abc run_sweep (Seed-Vorwuerfe seed*13, Lage, Schlagtakt). */
static void lauf(lauf_t *o, int seed, int budget, int weapon, unsigned maske, int dump_von, int dump_bis)
{
    memset(o, 0, sizeof *o);
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_re2z_r37_produzenten(maske);
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
    int hp_prev[RE15_ACTOR_MAX];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) hp_prev[s] = g_actors[s].hp;
    re15_re2z_r37_zaehler_reset();
    s_modal_auf = s_modal_im_griff = 0;
    uint32_t d0 = re15_re2_rand_draws();
    int shot_period = 7 + (seed % 11), shot_first = 20 + (seed % 17);
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
        if (f >= shot_first && ((f - shot_first) % shot_period) == 0) { cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE; }
        frame(cur, edge);
        if (re15_player_is_grabbed()) o->grab_frames++;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!is_zombie(e)) continue;
            int hit = (e->hp < hp_prev[s]);
            if (hit) o->hits++;
            if (e->hp < 0 && hp_prev[s] >= 0) o->deaths++;
            hp_prev[s] = e->hp;
            if (e->state == 1 && e->sub_state_1 == 2) o->s2_frames++;
            if (e->state == 1 && e->sub_state_1 == 3) o->s3_frames++;
            if (e->state == 1 && e->sub_state_1 == 1) o->walk_frames++;
            if (f >= dump_von && f <= dump_bis)
                printf("  f%04d z%d st=%u/%u/%u hp=%d%s d=%u xz=(%d,%d) yaw=%d clip=%d af=%u t158=%d t15a=%d 16a=%d"
                       " | pl st=%u/%u mot=%d xz=(%d,%d) yaw=%d grab=%d%s\n",
                       f, s, e->state, e->sub_state_1, e->sub_state_2, (int)e->hp, hit ? "*" : "",
                       e->ai_dist, e->x, e->z, (int)e->rot_y & 0xfff, (int)e->motion, e->anim_frame,
                       (int)e->re2z_t158, (int)e->re2z_t15a, (int)(int8_t)e->re2z_dir16a,
                       pl->state, pl->sub_state_1, (int)pl->motion, pl->x, pl->z, (int)pl->rot_y & 0xfff,
                       re15_player_is_grabbed(), (edge & RE15_PAD_BIT_SQUARE) ? " SQ" : "");
        }
    }
    for (int p = 0; p < 3; p++) o->prod[p] = re15_re2z_r37_zaehler(p);
    o->draws = re15_re2_rand_draws() - d0;
    o->modal_auf = s_modal_auf; o->modal_im_griff = s_modal_im_griff;
}

static int lade_raum(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 0; }
    return 1;
}

int main(int argc, char **argv)
{
    const char *modus = argc > 1 ? argv[1] : "mess";
    if (!lade_raum()) return 1;
    if (!strcmp(modus, "mess")) {
        unsigned maske = argc > 2 ? (unsigned)strtoul(argv[2], NULL, 0) : 0u;
        int seeds = argc > 3 ? atoi(argv[3]) : 64;
        int waffe = argc > 4 ? atoi(argv[4]) : 1;
        long th = 0, tg = 0, t2 = 0, t3 = 0, tw = 0, td = 0, tm = 0, tmg = 0; unsigned tp[3] = {0, 0, 0};
        for (int seed = 0; seed < seeds; seed++) {
            lauf_t o; lauf(&o, seed, 900, waffe, maske, -1, -1);
            tm += o.modal_auf; tmg += o.modal_im_griff;
            th += o.hits; tg += o.grab_frames; t2 += o.s2_frames; t3 += o.s3_frames; tw += o.walk_frames; td += o.deaths;
            for (int p = 0; p < 3; p++) tp[p] += o.prod[p];
            printf("seed %2d: treffer %4ld tode %ld griff %4ld s1=1 %5ld s1=2 %5ld s1=3 %5ld 0x201 %u/%u/%u wuerfe %u\n",
                   seed, o.hits, o.deaths, o.grab_frames, o.walk_frames, o.s2_frames, o.s3_frames,
                   o.prod[0], o.prod[1], o.prod[2], (unsigned)o.draws);
        }
        printf("SUMME maske=%u waffe=%d: treffer %ld tode %ld griff %ld s1=1 %ld s1=2 %ld s1=3 %ld 0x201 %u/%u/%u"
               " item-modal %ld (davon im Griff %ld)\n",
               maske, waffe, th, td, tg, tw, t2, t3, tp[0], tp[1], tp[2], tm, tmg);
        return 0;
    }
    if (!strcmp(modus, "zeit")) {
        unsigned maske = argc > 2 ? (unsigned)strtoul(argv[2], NULL, 0) : 0u;
        int seed  = argc > 3 ? atoi(argv[3]) : 0;
        int waffe = argc > 4 ? atoi(argv[4]) : 1;
        int von   = argc > 5 ? atoi(argv[5]) : 0;
        int bis   = argc > 6 ? atoi(argv[6]) : 899;
        lauf_t o; lauf(&o, seed, 900, waffe, maske, von, bis);
        printf("SUMME seed %d maske %u: treffer %ld griff %ld s1=2 %ld 0x201 %u/%u/%u\n", seed, maske,
               o.hits, o.grab_frames, o.s2_frames, o.prod[0], o.prod[1], o.prod[2]);
        return 0;
    }
    printf("unbekannter Modus %s\n", modus);
    return 1;
}
