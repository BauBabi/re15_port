/* probe_re2_aim4.c — MESSUNG der vier Nutzer-Befunde im RE2-KI-Modus (2026-08-21).
 *
 *  (1) "Wenn ich mit der Waffe ziele, scheinen mich die Zombies nicht angreifen/beissen zu koennen."
 *  (2) "Ich kann teilweise immer noch nicht die Zombies treffen mit der Handfeuerwaffe."
 *  (3) "Der Hund trifft mich teilweise mit seinem Sprungangriff nicht."
 *  (4) "Wenn der Hund mich toetet, verschwinde ich irgendwie komplett."
 *
 * ECHTER WEG: re15_game_step + Pad (kein synthetischer Schadensanstoss), geladene RE2-Bank,
 * g_room_rdt aus der echten RDT, SEED-SWEEP (>=64) weil "teilweise" heisst zustands-/zufalls-
 * abhaengig.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static re15_rdt_t         s_rdt;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
extern int re2_ems_load_bank(const uint8_t *ems, size_t n, int type,
                             re15_enemy_bank_t *out, void *unused);
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

static int load_room(const char *sub, int room_id, int fsub)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", sub); return 0; }
    s_room_id = room_id; s_fire_sub = fsub;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    return 1;
}

static void bringup(uint32_t seed)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(seed);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16); load_bank2(0x20);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int first_enemy(unsigned lo, unsigned hi, int standing_only)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= lo && g_actors[s].type <= hi
            && (!standing_only || !(g_actors[s].grid_id & 0x80))) return s;
    if (standing_only) return first_enemy(lo, hi, 0);
    return -1;
}

extern int re15_sin_q12(int), re15_cos_q12(int);
extern void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);
/* Spieler im Abstand `back` unter der Peilung `bear` um den Gegner herum, Blick auf den Gegner. */
static void face_at(int slot, int32_t back, int bear)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t ox = (int32_t)(((long long)re15_sin_q12(bear) * back) >> 12);
    int32_t oz = (int32_t)(((long long)re15_cos_q12(bear) * back) >> 12);
    pl->x = e->x + ox; pl->z = e->z + oz; pl->y = e->y;
    pl->rot_y = (int16_t)((bear + 2048) & 0xfff);   /* zurueck zum Gegner */
}
static void face(int slot, int32_t back) { face_at(slot, back, 3072); }

/* ===================================================================================
 * TEIL 1 — (1) Zielen blockt den Angriff?  +  (2) Treffbarkeit
 * =================================================================================== */
enum { ARM_STAND = 0, ARM_AIM, ARM_AIMFIRE, ARM_WALKFORCE, ARM_N };
static const char *arm_name[ARM_N] = { "STEHEN(pad0)", "ZIELEN(R1)", "ZIELEN+FEUER", "STEHEN+motion=WALK" };

typedef struct {
    int atk_commits;      /* Uebergaenge nach sub 3 / 12 / 14 */
    int grab3, lunge12, snap14;
    int pl_dmg_events;    /* Frames in denen Spieler-HP faellt */
    int pl_dmg_total;
    int hits;             /* Treffer auf den Gegner */
    int dead;
    int frames_latched;   /* Frames mit hit_react&1 UND lebendem, nicht gesperrtem Gegner */
    int end_hp;
    int shots;            /* verbrauchte Magazin-Runden (echte Schuesse) */
    int f_hittable;       /* Frames, in denen der Gegner die 4 Gates besteht */
    int f_alive;
    int f_sub[20];        /* Frames je sub_state_1 (state 1) */
    int f_upright_downed; /* aufrecht UND grid&0x80 = Widerspruch */
    int f_upright_1d3;    /* aufrecht UND +0x1D3 != 0 = Widerspruch */
} armres_t;

static int s_all = 0;   /* 1 = ALLE Raum-Gegner aktiv lassen */
static int s_dogdist = 3000;
static int s_dogmode = 0;   /* 0 stehen 1 gehen 2 rennen 3 zurueck 4 zielen */
static int s_bear = 3072;      /* Peilung des Spielers relativ zum Gegner */
static int s_prewarm = 0;      /* RE2-PRNG N Ziehungen vorspulen (Zustands-Sweep) */

static void run_zombie_arm(uint32_t seed, int arm, int dist, armres_t *r, int verbose)
{
    memset(r, 0, sizeof *r);
    bringup(seed);
    for (int i = 0; i < s_prewarm; i++) (void)re15_re2_rand();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);       /* Pistole */

    for (int f = 0; f < 60; f++) { g_actors[0].hp = 100; frame(0, 0); }
    int slot = first_enemy(0x10, 0x1f, 1);
    if (slot < 0) { r->end_hp = -999; return; }
    if (!s_all) for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[0];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face_at(slot, dist, s_bear);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 99;   /* Munition satt */
    }
    int mag0 = (re15_inv_equipped_slot() >= 0) ? g_inv.slots[re15_inv_equipped_slot()].qty : 0;

    /* Bei den Ziel-Armen erst R1 hochfahren */
    if (arm == ARM_AIM || arm == ARM_AIMFIRE)
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) frame(RE15_PAD_BIT_R1, 0);

    int prev_sub[RE15_ACTOR_MAX], prev_hp[RE15_ACTOR_MAX];
    for (int k = 0; k < RE15_ACTOR_MAX; k++) {
        prev_sub[k] = g_actors[k].sub_state_1; prev_hp[k] = g_actors[k].hp;
    }
    int prev_plhp = pl->hp;
    for (int f = 0; f < 600; f++) {
        uint16_t pad = 0, edge = 0;
        if (arm == ARM_AIM)      pad = RE15_PAD_BIT_R1;
        if (arm == ARM_AIMFIRE) { pad = RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE;
                                  edge = (f == 0) ? RE15_PAD_BIT_SQUARE : 0; }
        if (arm == ARM_WALKFORCE) pl->motion = 105;   /* cfbf6-Bit 0x2 erzwingen */
        frame(pad, edge);
        if (arm == ARM_WALKFORCE) pl->motion = 105;

        /* ueber ALLE aktiven Gegner zaehlen (mit --all=1 ist der Raum voll besetzt) */
        for (int k = 1; k < RE15_ACTOR_MAX; k++) {
            re15_actor_t *a = &g_actors[k];
            if (!a->active || a->type < 0x10 || a->type > 0x1f) { prev_sub[k] = -1; continue; }
            if (a->state == 1 && a->sub_state_1 != prev_sub[k]) {
                if (a->sub_state_1 == 3)  { r->grab3++;   r->atk_commits++; }
                if (a->sub_state_1 == 12) { r->lunge12++; r->atk_commits++; }
                if (a->sub_state_1 == 14) { r->snap14++;  r->atk_commits++; }
            }
            prev_sub[k] = (a->state == 1) ? a->sub_state_1 : -1;
            if (a->hp < prev_hp[k]) r->hits++;
            prev_hp[k] = a->hp;
            if (a->hp >= 0) {
                r->f_alive++;
                if (!(a->hit_react & 1)) r->f_hittable++;
                if (a->state == 1 && a->sub_state_1 < 20) r->f_sub[a->sub_state_1]++;
                if (a->state == 1 && (a->grid_id & 0x80u)
                    && (a->sub_state_1 == 0 || a->sub_state_1 == 1 || a->sub_state_1 == 3
                        || a->sub_state_1 == 12 || a->sub_state_1 == 14))
                    r->f_upright_downed++;
                if ((a->state == 1 || a->state == 2) && a->re2z_self1d3 != 0
                    && (a->sub_state_1 == 0 || a->sub_state_1 == 1))
                    r->f_upright_1d3++;
            }
        }
        if (e->hp >= 0 && (e->hit_react & 1)) r->frames_latched++;
        if (pl->hp < prev_plhp) { r->pl_dmg_events++; r->pl_dmg_total += prev_plhp - pl->hp; }
        prev_plhp = pl->hp;
        if (verbose && (f % 25) == 0) {
            int32_t dx = e->x - pl->x, dz = e->z - pl->z;
            printf("        f%3d st=%d s1=%2d s2=%d dist=%.0f plmot=%d ehp=%d hr=%02X 1D3=%02X "
                   "10E=%04X clip=%d/f%d(len%d)\n",
                   f, e->state, e->sub_state_1, e->sub_state_2,
                   sqrt((double)dx*dx + (double)dz*dz), (int)pl->motion, e->hp, e->hit_react,
                   e->re2z_self1d3, e->re2z_f10e, (int)e->motion, (int)e->anim_frame,
                   re15_actor_clip_len(e));
        }
        if (e->hp < 0 && !r->dead) r->dead = f + 1;
        if (pl->hp < 0) break;
    }
    {   int es = re15_inv_equipped_slot();
        int mag_now = (es >= 0 && es < RE15_INV_MAX_SLOTS) ? g_inv.slots[es].qty : 0;
        r->shots = mag0 - mag_now;
    }
    r->end_hp = e->hp;
    if (verbose)
        printf("      [%s] atk=%d (grab%d/lunge%d/snap%d) plDmg=%dx/%d hits=%d hp=%d latched=%d\n",
               arm_name[arm], r->atk_commits, r->grab3, r->lunge12, r->snap14,
               r->pl_dmg_events, r->pl_dmg_total, r->hits, r->end_hp, r->frames_latched);
}

/* ZUSTANDS-SWEEP statt Seed-Sweep: der RE2-PRNG ist im Port (wie im Original) deterministisch
 * (s_re2_rng = 0xD2706CA4 bei jedem Raumladen) — Varianz entsteht ueber GEOMETRIE (Abstand,
 * Peilung) und ueber die PRNG-PHASE (Vorspul-Ziehungen). */
static const int SW_DIST[] = { 700, 1100, 1500, 2000, 2600, 3400, 4500 };
static const int SW_BEAR[] = { 0, 1024, 2048, 3072 };
static void part1(int nprewarm, int dist_only)
{
    int nd = (int)(sizeof SW_DIST / sizeof SW_DIST[0]);
    int nb = (int)(sizeof SW_BEAR / sizeof SW_BEAR[0]);
    printf("\n########## TEIL 1 — Zombie ROOM1140, RE2-Flavor: %d Abstaende x %d Peilungen x %d "
           "PRNG-Phasen = %d Zellen je Arm ##########\n", nd, nb, nprewarm, nd*nb*nprewarm);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    long tot_atk[ARM_N] = {0}, tot_dmg[ARM_N] = {0}, tot_hits[ARM_N] = {0};
    int  cells_atk[ARM_N] = {0}, cells = 0;
    int  hits_min = 9999, hits_max = -1, cells_hit0 = 0, cells_hit1 = 0;
    int  aim_blocks = 0;                       /* STAND greift an, ZIELEN nicht = Befund (1) */
    for (int di = 0; di < nd; di++) {
        if (dist_only > 0 && SW_DIST[di] != dist_only) continue;
        for (int bi = 0; bi < nb; bi++) {
            for (int p = 0; p < nprewarm; p++) {
                s_bear = SW_BEAR[bi]; s_prewarm = p;
                armres_t rr[ARM_N];
                for (int a = 0; a < ARM_N; a++) {
                    run_zombie_arm(0x1000u, a, SW_DIST[di], &rr[a], 0);
                    tot_atk[a]  += rr[a].atk_commits;
                    tot_dmg[a]  += rr[a].pl_dmg_events;
                    tot_hits[a] += rr[a].hits;
                    if (rr[a].atk_commits) cells_atk[a]++;
                }
                cells++;
                for (int a = 0; a < ARM_N; a++) {
                    if (rr[a].f_upright_downed || rr[a].f_upright_1d3)
                        printf("   (2)## d=%4d bear=%4d prng=%d arm=%s : AUFRECHT+DOWNED %d Frames, "
                               "AUFRECHT+1D3 %d Frames  <-- untreffbar obwohl er steht/laeuft\n",
                               SW_DIST[di], SW_BEAR[bi], p, arm_name[a],
                               rr[a].f_upright_downed, rr[a].f_upright_1d3);
                }
                if (rr[ARM_STAND].atk_commits > 0 && rr[ARM_AIM].atk_commits == 0) {
                    aim_blocks++;
                    printf("   (1)!! d=%4d bear=%4d prng=%d : STEHEN %d Angriffe / ZIELEN 0\n",
                           SW_DIST[di], SW_BEAR[bi], p, rr[ARM_STAND].atk_commits);
                }
                int h = rr[ARM_AIMFIRE].hits;
                if (h < hits_min) hits_min = h;
                if (h > hits_max) hits_max = h;
                if (h == 0) { cells_hit0++;
                    printf("   (2)!! d=%4d bear=%4d prng=%d : 0 TREFFER, ehp=%d latched=%d\n",
                           SW_DIST[di], SW_BEAR[bi], p, rr[ARM_AIMFIRE].end_hp,
                           rr[ARM_AIMFIRE].frames_latched); }
                else if (h == 1) { cells_hit1++;
                    printf("   (2)!! d=%4d bear=%4d prng=%d : nur 1 Treffer (Latch-Signatur), "
                           "ehp=%d latched=%d\n", SW_DIST[di], SW_BEAR[bi], p,
                           rr[ARM_AIMFIRE].end_hp, rr[ARM_AIMFIRE].frames_latched); }
                else if (rr[ARM_AIMFIRE].end_hp >= 0) {
                    armres_t *q = &rr[ARM_AIMFIRE];
                    printf("   (2)?  d=%4d bear=%4d prng=%d : %d Schuss -> %d Treffer (%.0f%%), "
                           "UEBERLEBT ehp=%d | lebend=%d treffbar=%d | "
                           "sub0=%d 1=%d 3=%d 5=%d 6=%d 7=%d 8=%d 9=%d 11=%d 12=%d 14=%d\n",
                           SW_DIST[di], SW_BEAR[bi], p, q->shots, h,
                           q->shots ? 100.0*h/q->shots : 0.0, q->end_hp, q->f_alive, q->f_hittable,
                           q->f_sub[0], q->f_sub[1], q->f_sub[3], q->f_sub[5], q->f_sub[6],
                           q->f_sub[7], q->f_sub[8], q->f_sub[9], q->f_sub[11], q->f_sub[12],
                           q->f_sub[14]);
                }
            }
        }
    }
    printf("  --- %d Zellen ---\n", cells);
    for (int a = 0; a < ARM_N; a++)
        printf("  %-20s : Angriffe %4ld ges. / %3d von %d Zellen | SpielerSchaden-Events %ld | "
               "Treffer %ld\n", arm_name[a], tot_atk[a], cells_atk[a], cells, tot_dmg[a],
               tot_hits[a]);
    printf("  (1) Zellen in denen ZIELEN den Angriff verhindert: %d\n", aim_blocks);
    printf("  (2) Trefferspanne bei Dauerfeuer: min=%d max=%d | 0 Treffer: %d Zellen | 1: %d\n",
           hits_min, hits_max, cells_hit0, cells_hit1);
}

/* ===================================================================================
 * TEIL 2 — (3) Hunde-Sprungangriff
 * =================================================================================== */
static void part2(int nseeds)
{
    printf("\n########## TEIL 2 — Hund ROOM1190, RE2-Flavor, %d Seeds: Sprungangriff ##########\n",
           nseeds);
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) return;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    int tot_launch = 0, tot_contact = 0, tot_latch = 0, tot_touchdown = 0, seeds_ok = 0;
    long tot_dmg = 0;
    for (int s = 0; s < nseeds; s++) {
        uint32_t seed = 0x2000u + (uint32_t)s * 0x9E3779B9u;
        bringup(seed);
        re15_inv_load_briefing(); re15_player_set_equipped_weapon(3);
        for (int f = 0; f < 60; f++) { g_actors[0].hp = 100; frame(0, 0); }
        int slot = first_enemy(0x20, 0x20, 0);
        if (slot < 0) continue;
        for (int t = 1; t < RE15_ACTOR_MAX; t++) if (t != slot) g_actors[t].active = 0;
        re15_actor_t *pl = &g_actors[0], *e = &g_actors[slot];
        pl->hp = 100; pl->floor = e->floor;
        re15_player_cmd_reset(); re15_player_aim_reset();
        face(slot, s_dogdist);
        pl->y = e->y;

        /* Phasen-Uebergaenge sind die Wahrheit (der Kontakt-Handler nullt +0x21E sofort wieder):
         *   sub3/P1  = FLUG           (Absprung, @0x801013C4)
         *   sub3/P3  = BODEN-BISS     nur ueber r>0 = KONTAKT (+0x6=3 @0x80104F7C)
         *   sub3/P2  = LANDUNG        nur ueber r<0 & +0x21E!=2 = VERFEHLT (@0x80101424-50)
         *   sub 7    = LATCH          nur ueber r<0 & +0x21E==2 = TOEDLICHER TREFFER */
        int launches = 0, contacts = 0, latches = 0, touchdown = 0, plhp0 = pl->hp;
        int prev_s1 = e->sub_state_1, prev_s2 = e->sub_state_2, in_flight = 0, flight_hit = 0;
        double miss_kd_sum = 0; int miss_kd_n = 0; double hit_kd_sum = 0; int hit_kd_n = 0;
        double last_kd = 0; int last_flr_ok = 1, last_yok = 1;
        long dmg_sum = 0;
        for (int f = 0; f < 900; f++) {
            uint16_t pad = 0;
            switch (s_dogmode) {
                case 1: pad = RE15_PAD_BIT_UP; break;                       /* gehen */
                case 2: pad = RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS; break;  /* rennen */
                case 3: pad = RE15_PAD_BIT_DOWN; break;                     /* zurueck */
                case 4: pad = RE15_PAD_BIT_R1; break;                       /* zielen */
                default: break;
            }
            frame(pad, 0);
            if (pl->hp < 100) { dmg_sum += 100 - pl->hp; }
            pl->hp = 100;                    /* Spieler am Leben halten: wir zaehlen KONTAKTE */
            int s1 = e->sub_state_1, s2 = e->sub_state_2;
            if (s1 == 3 && s2 == 1) {
                if (!in_flight) { in_flight = 1; flight_hit = 0; launches++; }
                int32_t jaw[3]; re15_enemy_bone_world_pos(e, 4, jaw);
                double kd = sqrt((double)(pl->x-jaw[0])*(pl->x-jaw[0])
                               + (double)(pl->z-jaw[2])*(pl->z-jaw[2]));
                if (!in_flight || kd < last_kd || last_kd == 0) last_kd = kd;
                if (kd < last_kd) last_kd = kd;
                last_flr_ok = (pl->floor == e->floor);
                last_yok    = (e->y > (int32_t)e->dog_floor_y - 1800);
            } else if (in_flight) {
                in_flight = 0;
                if (s1 == 3 && s2 == 3) { contacts++; hit_kd_sum += last_kd; hit_kd_n++; }
                else if (s1 == 7)       { latches++;  hit_kd_sum += last_kd; hit_kd_n++; }
                else                    { touchdown++; miss_kd_sum += last_kd; miss_kd_n++;
                    if (s < 4) printf("     VERFEHLT f%d: naechster Kiefer-Abstand %.0f "
                                      "(Radius %d), Etage %s, Y-Gate %s\n", f, last_kd,
                                      (pl->hp < 21) ? 700 : 1000, last_flr_ok ? "ok" : "UNGLEICH",
                                      last_yok ? "ok" : "ZU HOCH"); }
                last_kd = 0;
            }
            if (s < 1 && (f % 20) == 0) {
                int32_t dx = e->x - pl->x, dz = e->z - pl->z;
                int32_t jaw[3]; re15_enemy_bone_world_pos(e, 4, jaw);
                printf("     f%3d st=%d s1=%2d s2=%d s3=%d dist=%.0f(ai=%u) ehp=%d clip=%d/f%d(len%d) "
                       "y=%d floor=%d 21E=%d 21A=%04x kiefer(%d,%d) kd=%.0f flr=%d/%d\n",
                       f, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3,
                       sqrt((double)dx*dx + (double)dz*dz), (unsigned)e->ai_dist, e->hp,
                       (int)e->motion, (int)e->anim_frame, re15_actor_clip_len(e),
                       (int)e->y, (int)e->dog_floor_y, e->re2d_bite21e, e->re2z_flags21a,
                       jaw[0], jaw[2],
                       sqrt((double)(pl->x-jaw[0])*(pl->x-jaw[0]) + (double)(pl->z-jaw[2])*(pl->z-jaw[2])),
                       e->floor, pl->floor);
            }
            prev_s1 = s1; prev_s2 = s2; (void)prev_s1; (void)prev_s2;
            if (e->hp < 0) break;
        }
        (void)plhp0;
        tot_launch += launches; tot_contact += contacts; tot_latch += latches;
        tot_touchdown += touchdown; tot_dmg += dmg_sum;
        if (launches) seeds_ok++;
        if (s < 8)
            printf("   seed %2d: Spruenge=%d Treffer=%d Latch=%d VERFEHLT=%d | Kiefer-Abstand "
                   "Treffer %.0f / Fehl %.0f | Schaden %ld\n",
                   s, launches, contacts, latches, touchdown,
                   hit_kd_n ? hit_kd_sum / hit_kd_n : 0.0,
                   miss_kd_n ? miss_kd_sum / miss_kd_n : 0.0, dmg_sum);
    }
    printf("  GESAMT: %d Spruenge in %d/%d Seeds, %d Kontakte (%.0f%%), %d toedliche Latches, "
           "%d Fehlschlaege | dmg %ld\n",
           tot_launch, seeds_ok, nseeds, tot_contact,
           tot_launch ? 100.0 * tot_contact / tot_launch : 0.0, tot_latch, tot_touchdown, tot_dmg);
}

/* ===================================================================================
 * TEIL 3 — (4) Spieler-Weltposition Frame fuer Frame durch den Hunde-Tod
 * =================================================================================== */
static void part3(int nseeds)
{
    printf("\n########## TEIL 3 — Hunde-Toetung: Leons Weltposition Frame fuer Frame ##########\n");
    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) return;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    for (int s = 0; s < nseeds; s++) {
        uint32_t seed = 0x3000u + (uint32_t)s * 0x9E3779B9u;
        bringup(seed);
        re15_inv_load_briefing(); re15_player_set_equipped_weapon(3);
        for (int f = 0; f < 60; f++) { g_actors[0].hp = 100; frame(0, 0); }
        int slot = first_enemy(0x20, 0x20, 0);
        if (slot < 0) continue;
        for (int t = 1; t < RE15_ACTOR_MAX; t++) if (t != slot) g_actors[t].active = 0;
        re15_actor_t *pl = &g_actors[0], *e = &g_actors[slot];
        pl->hp = 18;                            /* schwach -> der toedliche Latch kann greifen */
        pl->floor = e->floor;
        re15_player_cmd_reset(); re15_player_aim_reset();
        face(slot, s_dogdist); pl->y = e->y;

        int32_t px0 = pl->x, pz0 = pl->z, py0 = pl->y;
        int latched = -1, maxjump = 0, maxjump_f = -1;
        int32_t lx = pl->x, lz = pl->z, ly = pl->y;
        int printed = 0;
        for (int f = 0; f < 1200; f++) {
            frame(0, 0);
            int32_t dx = pl->x - lx, dz = pl->z - lz, dy = pl->y - ly;
            int mag = (int)(labs((long)dx) + labs((long)dz) + labs((long)dy));
            if (mag > maxjump) { maxjump = mag; maxjump_f = f; }
            lx = pl->x; lz = pl->z; ly = pl->y;
            if (s == 0 && mag > 300)
                printf("     f%4d SPRUNG %6d: PL(%8d,%8d) HUND(%8d,%8d) anker PL(%8d,%8d) "
                       "HUND(%8d,%8d) | s1=%d s2=%d s3=%d clip=%d/f%d(len%d) rot=%d victim=%d\n",
                       f, mag, (int)pl->x, (int)pl->z, (int)e->x, (int)e->z,
                       (int)pl->anchor_x, (int)pl->anchor_z, (int)e->anchor_x, (int)e->anchor_z,
                       e->sub_state_1, e->sub_state_2, e->sub_state_3, (int)e->motion,
                       (int)e->anim_frame, re15_actor_clip_len(e), (int)e->rot_y,
                       re15_player_victim_state());
            if (latched < 0 && e->sub_state_1 == 7) { latched = f; }
            if (latched >= 0 && f - latched < 90 && s < 2 && (f - latched) % 6 == 0) {
                printf("   seed%d f%+4d: PL(%7d,%7d,%7d) HUND(%7d,%7d,%7d) d=%7.0f "
                       "plhp=%d victim=%d s1=%d s2=%d\n",
                       s, f - latched, (int)pl->x, (int)pl->y, (int)pl->z,
                       (int)e->x, (int)e->y, (int)e->z,
                       sqrt((double)(pl->x-e->x)*(pl->x-e->x) + (double)(pl->z-e->z)*(pl->z-e->z)),
                       pl->hp, re15_player_victim_state(), e->sub_state_1, e->sub_state_2);
                printed = 1;
            }
            if (latched >= 0 && f - latched > 240) break;
        }
        double dist_from_start = sqrt((double)(pl->x-px0)*(pl->x-px0) + (double)(pl->z-pz0)*(pl->z-pz0));
        printf("   seed %2d: Latch@%d | Endpos(%d,%d,%d) Startpos(%d,%d,%d) Abstand=%.0f | "
               "groesster 1-Frame-Sprung=%d @f%d | plhp=%d victim=%d%s\n",
               s, latched, (int)pl->x, (int)pl->y, (int)pl->z, (int)px0, (int)py0, (int)pz0,
               dist_from_start, maxjump, maxjump_f, pl->hp, re15_player_victim_state(),
               printed ? "" : " (keine Detailzeilen)");
    }
}

int main(int argc, char **argv)
{
    int nseeds = 64;
    int part = 0;
    int dist = 2400;
    for (int i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "--seeds=", 8)) nseeds = atoi(argv[i] + 8);
        else if (!strncmp(argv[i], "--part=", 7)) part = atoi(argv[i] + 7);
        else if (!strncmp(argv[i], "--dist=", 7)) dist = atoi(argv[i] + 7);
        else if (!strncmp(argv[i], "--dogmode=", 10)) s_dogmode = atoi(argv[i] + 10);
        else if (!strncmp(argv[i], "--all=", 6)) s_all = atoi(argv[i] + 6);
        else if (!strncmp(argv[i], "--bear=", 7)) s_bear = atoi(argv[i] + 7);
        else if (!strncmp(argv[i], "--prewarm=", 10)) s_prewarm = atoi(argv[i] + 10);
        else if (!strncmp(argv[i], "--dogdist=", 10)) s_dogdist = atoi(argv[i] + 10);
    }
    if (part == 8) {   /* ZENSUS: Spawn-grid_id aller STAGE1-Gegner (Gate RE15_AI_GRID_SKIP 0x20) */
        static const struct { const char *sub; int id; } rooms[] = {
            {"STAGE1/ROOM1000.RDT",0x1000},{"STAGE1/ROOM1010.RDT",0x1010},
            {"STAGE1/ROOM1020.RDT",0x1020},{"STAGE1/ROOM1030.RDT",0x1030},
            {"STAGE1/ROOM1040.RDT",0x1040},{"STAGE1/ROOM1050.RDT",0x1050},
            {"STAGE1/ROOM1060.RDT",0x1060},{"STAGE1/ROOM1070.RDT",0x1070},
            {"STAGE1/ROOM1080.RDT",0x1080},{"STAGE1/ROOM1090.RDT",0x1090},
            {"STAGE1/ROOM10A0.RDT",0x10A0},{"STAGE1/ROOM10B0.RDT",0x10B0},
            {"STAGE1/ROOM10C0.RDT",0x10C0},{"STAGE1/ROOM10D0.RDT",0x10D0},
            {"STAGE1/ROOM10E0.RDT",0x10E0},{"STAGE1/ROOM10F0.RDT",0x10F0},
            {"STAGE1/ROOM1100.RDT",0x1100},{"STAGE1/ROOM1110.RDT",0x1110},
            {"STAGE1/ROOM1120.RDT",0x1120},{"STAGE1/ROOM1130.RDT",0x1130},
            {"STAGE1/ROOM1140.RDT",0x1140},{"STAGE1/ROOM1150.RDT",0x1150},
            {"STAGE1/ROOM1160.RDT",0x1160},{"STAGE1/ROOM1170.RDT",0x1170},
            {"STAGE1/ROOM1180.RDT",0x1180},{"STAGE1/ROOM1190.RDT",0x1190},
            {"STAGE1/ROOM11A0.RDT",0x11A0},{"STAGE1/ROOM11B0.RDT",0x11B0},
            {"STAGE1/ROOM11C0.RDT",0x11C0},{"STAGE1/ROOM11D0.RDT",0x11D0},
            {"STAGE1/ROOM11E0.RDT",0x11E0},{"STAGE1/ROOM11F0.RDT",0x11F0},
            {"STAGE1/ROOM1200.RDT",0x1200},{"STAGE1/ROOM1210.RDT",0x1210},
            {"STAGE1/ROOM1220.RDT",0x1220},{"STAGE1/ROOM1230.RDT",0x1230},
            {"STAGE1/ROOM1240.RDT",0x1240},
        };
        int skipped = 0, total = 0;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        for (unsigned i = 0; i < sizeof rooms / sizeof rooms[0]; i++) {
            if (!load_room(rooms[i].sub, rooms[i].id, -1)) continue;
            bringup(0x1000u);
            for (int f = 0; f < 3; f++) frame(0, 0);
            for (int k = 1; k < RE15_ACTOR_MAX; k++) {
                re15_actor_t *a = &g_actors[k];
                if (!a->active) continue;
                total++;
                if (a->grid_id & 0x20u) {
                    skipped++;
                    printf("  ROOM%04X slot%02d typ=0x%02X grid=0x%02X  <-- AI-TICK WIRD "
                           "UEBERSPRUNGEN (grid&0x20)\n", rooms[i].id, k, a->type, a->grid_id);
                }
            }
        }
        printf("ZENSUS grid&0x20: %d von %d Spawns in STAGE1\n", skipped, total);
        return 0;
    }
    if (part == 4) {                              /* Einzel-Trace */
        if (!load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) return 1;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        for (int a = 0; a < ARM_N; a++) {
            armres_t r; printf("\n--- ARM %s (dist %d) ---\n", arm_name[a], dist);
            run_zombie_arm(0x1000u, a, dist, &r, 1);
        }
        return 0;
    }
    if (part == 0 || part == 1) {
        if (!load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) return 1;
        part1(nseeds > 8 ? 8 : nseeds, dist > 0 ? dist : 0);
    }
    if (part == 0 || part == 2) part2(nseeds);
    if (part == 0 || part == 3) part3(nseeds < 8 ? nseeds : 8);
    return 0;
}
