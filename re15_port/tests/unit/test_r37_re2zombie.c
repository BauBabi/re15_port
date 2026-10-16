/**
 * @file test_r37_re2zombie.c
 * @brief Runde 37 Spur L "re2zombie" — Sonden und Riegel der RE2-Zombie-KI (enemy_ai_re2_zombie.c).
 *        Dossier analysis/befunde_runde37/L_re2zombie.md.
 *
 * Modus "mess <maske> [seeds] [waffe]": der Messer-Sweep von test_re2_zombie_abc (ROOM1140, echte Spawns, RE2-Baenke,
 *   echter re15_game_step, Spieler steht mit R1 und schlaegt alle 7..17 Bilder) mit waehlbarer Produzenten-Maske
 *   (include/re15_re2zombie_r37.h). Ausgabe je Seed: Treffer, Griff-Bilder, Wechsel nach 0x201 je Produzent.
 * Modus "zeit <maske> <seed> [waffe] [von] [bis]": Bild fuer Bild Spieler + Zombies desselben Laufs.
 * Modus "gang <maske> <raum-hex> <x> <z> <gier> <bilder> [montage 0/1] [renn 0/1]": echter Raumeintritt (Muster
 *   test_r37_zombieaktiv eintreten), Leon steht (oder rennt geradeaus: UP+CROSS); je Zombie die Wechsel nach 0x201
 *   (Produzent, Abstand), Dauer im Arme-hoch-Gang, was danach kommt, Weg je Takt in 0x101/0x201, erster Griff.
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
#include "re15_fade.h"
#include "re15_skeleton.h"
#include "re15_irons_tod.h"
#include "re15_re2zombie_r37.h"
#include "re15_math.h"

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
    unsigned p3[5];      /* Nachbesserung 2: Ausgaenge der Erholungsleiter P3 (0xB01/0xC01/0x201/0x10101/0x101) */
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
    for (int k = 0; k < 5; k++) o->p3[k] = re15_re2z_r37_p3_ausgang(k);
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

/* ---------------------------------------------------------------------------------------------------------------
 * Raumeintritt wie test_r37_zombieaktiv.c (Spur H) — eintreten/grundzustand/frame. */
static uint8_t *s_raw; static size_t s_n; static int s_shown = 0;
static void (*s_flags)(void) = NULL;
static void frame_raum(uint16_t pad, uint16_t kante)
{
    const unsigned char *raw; int len, id;
    scd_vm_tick();
    re15_actor_step_all_walkers();
    re15_msg_tick(&raw, &len, &id);
    if (re15_cam_present_tick()) s_shown = (int)g_scd.cam_id;
    s_ctx.active_cut  = s_shown;
    s_ctx.pad_current = pad;
    s_ctx.pad_pressed = kante;
    re15_game_step(&s_ctx);
    {   int schutz = 0; while (re15_item_modal_active() && schutz++ < 600) re15_item_modal_tick(0x4000, 0); }
}
static int raum_laden(uint16_t raum)
{
    if (s_raw) { free(s_raw); s_raw = NULL; }
    char p[512]; snprintf(p, sizeof p, RE15_ASSET_PSX_DIR "/STAGE%X/ROOM%04X.RDT", raum >> 12, raum);
    s_raw = slurp(p, &s_n);
    return s_raw != NULL;
}
static int eintreten(uint16_t raum, int cut, int32_t px, int32_t pz, int16_t rot)
{
    if (!raum_laden(raum)) return 0;
    if (re15_rdt_parse(s_raw, s_n, &g_room_rdt) != 0) return 0;
    g_room_rdt_ok = 1;
    s_rdt = g_room_rdt;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = cut;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_player_victim_reset();
    re15_pauseflags_clear();
    re15_damage_seed_rng(0x2545f491u);
    re15_inv_load_briefing();
    g_current_room_id = raum; g_room_change.pending = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = rot;
    pl->floor = (uint8_t)re15_collision_band_from_y(0);
    re15_collision_set_band(pl->floor);
    s_shown = cut;
    (void)load_bank2(0x10); (void)load_bank2(0x11); (void)load_bank2(0x12); (void)load_bank2(0x16); (void)load_bank2(0x18);
    if (s_flags) s_flags();
    re15_msg_load_room_block(s_rdt.messages, s_rdt.messages_size);
    scd_room_reenter(&s_rdt, px, pz, cut);
    g_scd.cut_auto_enabled = 1;
    return 1;
}
/* Flags nach der Montage-Kette (wie test_r37_zombieaktiv flags_nach_montage, re15_irons_tod.h). */
static void flags_nach_montage(void)
{
    re15_game_flag_set(9, 71, 1); re15_game_flag_set(3, 94, 1);
    for (int b = 73; b <= 76; b++) re15_game_flag_set(9, (uint8_t)b, 1);
    re15_game_flag_set(4, 4, 1); re15_game_flag_set(4, 5, 1); re15_game_flag_set(4, 15, 1);
    for (int i = 0; i < 5; i++) { re15_game_flag_set(7, (uint8_t)(0xd3 + i), 1); re15_game_flag_set(7, (uint8_t)(0xc6 + i), 1); }
    re15_irons_tod_zustand_setzen(RE15_IT_AUS);
}
static int32_t isqrt64(long long v) { if (v <= 0) return 0; long long r = 0; long long b = 1LL << 31;
    while (b) { long long t = r + b; if (t * t <= v) r = t; b >>= 1; } return (int32_t)r; }
static int32_t abst(const re15_actor_t *a, const re15_actor_t *b)
{ long long dx = a->x - b->x, dz = a->z - b->z; return isqrt64(dx * dx + dz * dz); }

typedef struct {
    int  wechsel, prod[3], nach_griff, nach_gang, nach_sonst, erster_griff;
    long d_summe, takte_201, takte_101, weg_201, weg_101;
} gz_t;
typedef struct { gz_t z[RE15_ACTOR_MAX]; int n_zombie; int griffe; int bild_erster_griff; } gang_t;

static void gang_lauf(gang_t *o, unsigned maske, uint16_t raum, int32_t x, int32_t z, int16_t rot, int bilder,
                      int montage, int renn, int dump)
{
    memset(o, 0, sizeof *o); o->bild_erster_griff = -1;
    re15_re2z_r37_produzenten(maske);
    s_flags = montage ? flags_nach_montage : NULL;
    if (montage) re15_irons_tod_zustand_setzen(RE15_IT_AUS);
    if (!eintreten(raum, 0, x, z, rot)) { printf("FAIL: ROOM%04X nicht ladbar\n", raum); return; }
    re15_re2z_r37_zaehler_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int prev_s1[RE15_ACTOR_MAX], prev_st[RE15_ACTOR_MAX]; int32_t px[RE15_ACTOR_MAX], pz[RE15_ACTOR_MAX];
    for (int s = 0; s < RE15_ACTOR_MAX; s++) { prev_s1[s] = prev_st[s] = -1; px[s] = g_actors[s].x; pz[s] = g_actors[s].z; }
    int war_griff = 0;
    for (int f = 1; f <= bilder; f++) {
        pl->hp = 100;
        uint16_t pad = renn ? (uint16_t)(RE15_PAD_BIT_UP | RE15_PAD_BIT_CROSS) : 0u;
        frame_raum(pad, 0);
        int griff = re15_player_is_grabbed();
        if (griff && !war_griff) { o->griffe++; if (o->bild_erster_griff < 0) o->bild_erster_griff = f; }
        war_griff = griff;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!is_zombie(e)) { prev_s1[s] = prev_st[s] = -1; continue; }
            gz_t *g = &o->z[s];
            int st = e->state, s1 = e->sub_state_1;
            int32_t d = abst(pl, e);
            if (st == 1 && s1 == 2 && !(prev_st[s] == 1 && prev_s1[s] == 2)) {
                g->wechsel++; g->d_summe += (long)e->ai_dist;   /* +0x1F0 des Wechsel-Takts (wie RE2 d(Takt)) */
                int pr = re15_re2z_r37_letzter_produzent(s); if (pr >= 0 && pr < 3) g->prod[pr]++;
                if (dump) printf("  f%04d slot %d typ %02x -> 0x201 durch (%d) d=%d ai_dist=%u\n", f, s, e->type, pr + 1, (int)d, e->ai_dist);
            }
            if (prev_st[s] == 1 && prev_s1[s] == 2 && !(st == 1 && s1 == 2)) {
                if (st == 1 && s1 == 3) g->nach_griff++; else if (st == 1 && s1 == 1) g->nach_gang++; else g->nach_sonst++;
                if (dump) printf("  f%04d slot %d 0x201 -> %u/%u/%u d=%d ai_dist=%u\n", f, s, e->state, e->sub_state_1, e->sub_state_2, (int)d, e->ai_dist);
            }
            if (st == 1 && s1 == 3 && !(prev_st[s] == 1 && prev_s1[s] == 3) && g->erster_griff == 0) g->erster_griff = f;
            if (prev_st[s] == 1 && st == 1 && prev_s1[s] == s1 && (s1 == 1 || s1 == 2)) {
                long w = labs((long)(e->x - px[s])) + labs((long)(e->z - pz[s]));
                if (s1 == 2) { g->takte_201++; g->weg_201 += w; } else { g->takte_101++; g->weg_101 += w; }
            }
            prev_st[s] = st; prev_s1[s] = s1; px[s] = e->x; pz[s] = e->z;
        }
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (o->z[s].takte_101 || o->z[s].wechsel) o->n_zombie++;
    s_flags = NULL;
}

static void gang_druck(const gang_t *o, const char *titel)
{
    long w = 0, p1 = 0, p2 = 0, p3 = 0, dsum = 0, t201 = 0, t101 = 0, w201 = 0, w101 = 0, ng = 0, nb = 0, ns = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const gz_t *g = &o->z[s];
        if (!g->takte_101 && !g->wechsel) continue;
        printf("  slot %2d: 0x201 %d x (1)/(2)/(3) %d/%d/%d, d beim Wechsel %ld, Takte 0x201 %ld (Weg/Takt %.1f), 0x101 %ld (Weg/Takt %.1f),"
               " danach Griff %d / Gang %d / sonst %d, erster Griff Bild %d\n", s, g->wechsel, g->prod[0], g->prod[1], g->prod[2],
               g->wechsel ? g->d_summe / g->wechsel : 0L, g->takte_201, g->takte_201 ? (double)g->weg_201 / g->takte_201 : 0.0,
               g->takte_101, g->takte_101 ? (double)g->weg_101 / g->takte_101 : 0.0, g->nach_griff, g->nach_gang, g->nach_sonst,
               g->erster_griff);
        w += g->wechsel; p1 += g->prod[0]; p2 += g->prod[1]; p3 += g->prod[2]; dsum += g->d_summe;
        t201 += g->takte_201; t101 += g->takte_101; w201 += g->weg_201; w101 += g->weg_101;
        ng += g->nach_griff; nb += g->nach_gang; ns += g->nach_sonst;
    }
    printf("GANG %s: Zombies %d, Wechsel nach 0x201 %ld ((1) %ld, (2) %ld, (3) %ld), mittlerer Abstand beim Wechsel %ld,"
           " Weg/Takt 0x101 %.1f / 0x201 %.1f, nach 0x201: Griff %ld, zurueck Gang %ld, sonst %ld; Spieler gegriffen %d x, erstmals Bild %d\n",
           titel, o->n_zombie, w, p1, p2, p3, w ? dsum / w : 0L, t101 ? (double)w101 / t101 : 0.0, t201 ? (double)w201 / t201 : 0.0,
           ng, nb, ns, o->griffe, o->bild_erster_griff);
}

/* ===============================================================================================================
 * RIEGEL (Pins). Umgebung: ROOM1140-SCD spawnt die Zombies, EIN Zombie (Typ 0x10) bleibt aktiv; g_room_rdt bleibt
 * ungeladen -> keine Zellen: Sicht frei (re15_r36_zombie_sicht_frei `if (!g_room_rdt_ok) return 1`), keine Waende.
 * Ein Takt = re15_enemy_ai_run_all(1) (Prolog: Abstand +0x1F0, Sicht, Navigator; dann DECISION + EXECUTOR).
 * Der Zufallsstrom wird mit re15_re2z_rng_reset() + k Vorwuerfen positioniert und der entscheidende Wurf per
 * re15_re2z_rng_peek() VORHER gelesen — der Riegel prueft damit die Regel je Wurf, nicht einen Glueckstreffer. */
static int s_fails = 0, s_checks = 0;
#define PIN(c, ...) do { s_checks++; if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); s_fails++; } \
                         else { printf("ok:   " __VA_ARGS__); printf("\n"); } } while (0)

static int s_pin_slot = -1;
static int pin_umgebung(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
    g_room_rdt_ok = 0;
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_victim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    s_pin_slot = -1;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++)
        if (g_actors[sl].active && g_actors[sl].type == 0x10 && s_pin_slot < 0) s_pin_slot = sl;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) if (sl != s_pin_slot) g_actors[sl].active = 0;
    (void)load_bank2(0x10);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    return s_pin_slot;
}

/* Zombie bei (0,0) mit Gier 0 (Blick +x: 0x800152C8 x += cos, z -= sin), Spieler bei (px,pz). Zustandswort `wort`. */
static re15_actor_t *pin_stellen(uint32_t wort, int32_t px, int32_t pz, int pl_motion)
{
    re15_actor_t *e = &g_actors[s_pin_slot], *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    e->active = 1; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0; e->floor = 0; e->hp = 100; e->hit_react = 0;
    re15_ai_set_state_word(e, wort);
    e->re2z_f10e = 0; e->re2z_flags21a = 0; e->re2z_self1d3 = 0; e->re2z_cd23e = 0; e->re2z_cd239 = 200;
    e->re2z_t158 = 200; e->re2z_t15a = 200; e->re2z_dir16a = 100;
    e->re2z_gaitinit = (uint8_t)(0x80u | (e->sub_state_1 & 0x7fu)); e->re2z_gaittmr = 500; e->re2z_gaitrow = 0;
    pl->x = px; pl->y = 0; pl->z = pz; pl->rot_y = 0; pl->motion = (uint16_t)pl_motion; pl->hp = 100;
    pl->state = 0; pl->floor = 0;
    re15_re2z_r37_zaehler_reset();
    return e;
}
/* Kleinstes dx mit SquareRoot0(dx^2) >= ziel — der Abstand +0x1F0 ist der BIOS-SquareRoot0 (re15_enemy_player_dist,
 * EXE @0x800265a4-e0), nicht die exakte Wurzel: sqrt0(5000^2) = 4990. Die Schwellen werden am Byte-Wert gepinnt. */
static int32_t dx_grenze(uint32_t ziel)
{
    int32_t dx = (int32_t)ziel - 64;
    while (re15_squareroot0((uint32_t)(dx * dx)) < ziel) dx++;
    return dx;
}
static void pin_takt(void) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; re15_enemy_ai_run_all(1); }
static void rng_auf(int k) { re15_re2z_rng_reset(); for (int i = 0; i < k; i++) (void)re15_re2_rand(); }

/* (1) TIMER @0x80101D68-DBC: +0x15A 1 -> 0 im Takt: Reseed (r0>>3)+100, zweiter Wurf r1, (r1 & 3) == 0 && dist < 0x1388
 *     -> +0x4 = 0x201. Je k: genau 2 Wuerfe im Takt, Ergebnis = Regel(r1). Gegenprobe dist 0x1388: nie, Wuerfe trotzdem 2. */
static void pin_prod1(void)
{
    int commits = 0, ohne = 0, regel = 1, wurf2 = 1, seed_ok = 1, gegen = 0, gegen_wurf = 1;
    for (int k = 0; k < 48; k++) {
        re15_actor_t *e = pin_stellen(0x00010101u, dx_grenze(0x1388) - 1, 0, 0);   /* +0x1F0 = 0x1387 */
        e->re2z_t15a = 1;
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0), r1 = re15_re2z_rng_peek(1), d0 = re15_re2_rand_draws();
        pin_takt();
        uint32_t dw = re15_re2_rand_draws() - d0;
        int ist = (e->state == 1 && e->sub_state_1 == 2);
        int soll = ((r1 & 3u) == 0u);
        if (dw != 2) wurf2 = 0;
        if (ist != soll) regel = 0;
        if (ist && re15_re2z_r37_letzter_produzent(s_pin_slot) != 0) regel = 0;
        if (!ist && e->re2z_t15a != (int16_t)((r0 >> 3) + 100)) seed_ok = 0;
        if (ist) commits++; else ohne++;
        /* Gegenprobe: dist == 0x1388 (sltiu 0x1388 @0x80101db0 = streng kleiner) */
        e = pin_stellen(0x00010101u, dx_grenze(0x1388), 0, 0);                     /* +0x1F0 = 0x1388 */
        e->re2z_t15a = 1;
        rng_auf(k);
        d0 = re15_re2_rand_draws();
        pin_takt();
        if (e->state == 1 && e->sub_state_1 == 2) gegen++;
        if (re15_re2_rand_draws() - d0 != 2) gegen_wurf = 0;
    }
    PIN(wurf2, "(1) genau zwei Wuerfe im Ablauf-Takt (Reseed @0x80101d84, rand&3 @0x80101d94)");
    PIN(regel && commits > 0 && ohne > 0, "(1) 0x201 genau bei (r1 & 3) == 0 und +0x1F0 = 0x1387 < 0x1388 (@0x80101d9c-dbc): %d Wechsel, %d ohne", commits, ohne);
    PIN(seed_ok, "(1) +0x15A neu = (r0 >> 3) + 100 (@0x80101d8c-98)");
    PIN(gegen == 0 && gegen_wurf, "(1) Gegenprobe +0x1F0 = 0x1388 (sltiu 0x1388 @0x80101db0): kein Wechsel (%d), Wuerfe trotzdem 2", gegen);
}

/* (2) NAHBEREICH @0x80101EC4-F10: dist < 0x7D0, Spieler im +-512-Kegel (0x80015614 a3=512 == 0), nicht beansprucht. */
static void pin_prod2(void)
{
    re15_actor_t *e = pin_stellen(0x00010101u, dx_grenze(0x7d0) - 1, 0, 0);       /* +0x1F0 = 0x7CF */
    pin_takt();
    int vor = (e->state == 1 && e->sub_state_1 == 2 && re15_re2z_r37_letzter_produzent(s_pin_slot) == 1);
    PIN(vor, "(2) Spieler geradeaus, +0x1F0 = 0x7CF, steht: 0x201 durch (2) (ist %u/%u/%u, Produzent %d)", e->state, e->sub_state_1,
        e->sub_state_2, re15_re2z_r37_letzter_produzent(s_pin_slot) + 1);
    e = pin_stellen(0x00010101u, -1500, 0, 0);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 2), "(2) Gegenprobe Spieler 1500 HINTER dem Zombie (ausserhalb +-512): kein 0x201 (%u/%u)",
        e->state, e->sub_state_1);
    e = pin_stellen(0x00010101u, dx_grenze(0x7d0), 0, 0);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 2), "(2) Gegenprobe +0x1F0 = 0x7D0 (sltiu 0x7d0 @0x80101ecc streng kleiner): kein 0x201 (%u/%u)",
        e->state, e->sub_state_1);
    e = pin_stellen(0x00010101u, 1500, 0, 0);
    re15_player_victim_force(0x10, 0, 0);          /* Spieler beansprucht (0x800CFDCB & 0x80 @0x80101efc-08) */
    pin_takt();
    int bean = (e->state == 1 && e->sub_state_1 == 2 && re15_re2z_r37_letzter_produzent(s_pin_slot) == 1);
    re15_player_victim_force_end();
    re15_player_victim_reset();
    PIN(!bean, "(2) Gegenprobe Spieler beansprucht: (2) committet nicht");
    re15_re2z_r37_produzenten(RE15_R37L_ALLES & ~RE15_R37L_PROD_NAH);
    e = pin_stellen(0x00010101u, 1500, 0, 0);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 2), "(2) Gegenprobe Maske ohne (2) (Stand bis Runde 37): kein 0x201 (%u/%u)",
        e->state, e->sub_state_1);
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
}

/* (3) RENNEN @0x80101F14-F60: 0x800CFBF6 & 4 (Port: Spieler-Motion 100), dist < 0x9C4, Spieler im +-1324-Kegel. */
static void pin_prod3(void)
{
    /* 90 Grad zur Seite: im +-1324-Kegel, ausserhalb +-512 -> nur (3) kann es sein. Seite per Kegeltest gewaehlt. */
    int32_t sz = dx_grenze(0x9c4) - 1;                                            /* +0x1F0 = 0x9C3 */
    {   re15_actor_t *e = pin_stellen(0x00010101u, 0, sz, 100);
        if (re15_ai_arc_test(e, 0, sz, 1324) != 0) sz = -sz; }
    re15_actor_t *e = pin_stellen(0x00010101u, 0, sz, 100);
    int k1324 = (re15_ai_arc_test(e, 0, sz, 1324) == 0), k512 = (re15_ai_arc_test(e, 0, sz, 512) == 0);
    PIN(k1324 && !k512, "(3) Vorbedingung: Spieler (0,%d) im +-1324-Kegel, ausserhalb +-512", (int)sz);
    pin_takt();
    PIN(e->state == 1 && e->sub_state_1 == 2 && re15_re2z_r37_letzter_produzent(s_pin_slot) == 2,
        "(3) Spieler RENNT seitlich, +0x1F0 = 0x9C3: 0x201 durch (3) (ist %u/%u, Produzent %d)", e->state, e->sub_state_1,
        re15_re2z_r37_letzter_produzent(s_pin_slot) + 1);
    e = pin_stellen(0x00010101u, 0, sz, 105);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 2), "(3) Gegenprobe Spieler GEHT (cfbf6 = 0x02): kein 0x201 (%u/%u)", e->state, e->sub_state_1);
    e = pin_stellen(0x00010101u, 0, sz > 0 ? dx_grenze(0x9c4) : -dx_grenze(0x9c4), 100);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 2), "(3) Gegenprobe +0x1F0 = 0x9C4 (sltiu 0x9c4 @0x80101f34): kein 0x201 (%u/%u)", e->state, e->sub_state_1);
    e = pin_stellen(0x00010101u, -(dx_grenze(0x9c4) - 1), 0, 100);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 2), "(3) Gegenprobe Spieler rennt HINTER dem Zombie (ausserhalb +-1324): kein 0x201 (%u/%u)",
        e->state, e->sub_state_1);
}

/* (b) DECISION[0] Block 3 @0x801013bc-d0: dist < 0x7D0, Spieler AUSSERHALB +-1024, cfbf6 & 0x17, Sicht, (rand & 1) == 1
 *     -> 0xC01. Spieler GEHT hinter dem Zombie: cfbf6 = 0x02 (& 0x15 == 0 -> Block 2 wirft nicht), Block 1 nicht (Kegel). */
static void pin_b(void)
{
    int regel = 1, ja = 0, nein = 0, wurf = 1;
    for (int k = 0; k < 32; k++) {
        re15_actor_t *e = pin_stellen(0x00010001u, -1500, 0, 105);
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0), d0 = re15_re2_rand_draws();
        pin_takt();
        int ist = (e->state == 1 && e->sub_state_1 == 12);
        int soll = ((r0 & 1u) == 1u);
        if (ist != soll) regel = 0;
        if (!ist && re15_re2_rand_draws() - d0 != 1) wurf = 0;
        if (ist) ja++; else nein++;
    }
    PIN(wurf, "(b) ohne Commit genau EIN Wurf im Takt (Block 3 @0x801013bc)");
    PIN(regel && ja > 0 && nein > 0, "(b) Block 3 committet 0xC01 genau bei (rand & 1) == 1 (`beq v0,zero,0x801013d4` @0x801013c8): %d ja, %d nein",
        ja, nein);
    /* Gegenprobe Stand bis Runde 37 (Bit (b) aus): Commit bei (rand & 1) == 0 — der umgekehrte Wurf. */
    re15_re2z_r37_produzenten(RE15_R37L_ALLES & ~RE15_R37L_B);
    int alt = 1;
    for (int k = 0; k < 32; k++) {
        re15_actor_t *e = pin_stellen(0x00010001u, -1500, 0, 105);
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0);
        pin_takt();
        if ((e->state == 1 && e->sub_state_1 == 12) != ((r0 & 1u) == 0u)) alt = 0;
    }
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
    PIN(alt, "(b) Gegenprobe Stand bis Runde 37: dieselben Wuerfe committen umgekehrt ((rand & 1) == 0)");
}

/* (c) EXEC[0] P1: Wander-Wurf @0x8010152c-40 und Selbstwecker @0x80101544-a0 im SELBEN Takt, P2 erst im naechsten. */
static void pin_c(void)
{
    int regel = 1, wuerfe = 1, nachher = 1, n_wurf = 0, n_kein = 0;
    for (int k = 0; k < 32; k++) {
        re15_actor_t *e = pin_stellen(0x00010001u, -12000, 0, 0);    /* fern + hinten: DECISION[0] wirft nicht */
        e->re2z_t15a = 1; e->re2z_t158 = 10;
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0), d0 = re15_re2_rand_draws();
        pin_takt();
        int wurf = ((r0 & 1u) == 0u);
        if (wurf) {
            n_wurf++;
            if (!(e->state == 1 && e->sub_state_1 == 0 && e->sub_state_2 == 2)) regel = 0;   /* P2 NICHT im selben Takt */
            if (e->re2z_t158 != 9) regel = 0;                                                /* Wecker-Dekrement @0x80101590-a0 */
            if (re15_re2_rand_draws() - d0 != 1) wuerfe = 0;
            uint32_t r1 = re15_re2z_rng_peek(0);
            pin_takt();                                                                      /* naechster Takt: P2 -> P3 */
            if (!(e->sub_state_2 == 3 && e->re2z_t158 == (int16_t)(r1 + 150 - 1))) nachher = 0;
        } else {
            n_kein++;
            if (!(e->sub_state_2 == 1 && e->re2z_t158 == 9)) regel = 0;
        }
    }
    PIN(regel && n_wurf > 0 && n_kein > 0, "(c) Wurf-Takt: +0x6 = 2 bleibt stehen, +0x158 10 -> 9 im selben Takt (%d Wuerfe, %d ohne)", n_wurf, n_kein);
    PIN(wuerfe, "(c) im Wurf-Takt genau EIN Wurf (kein P2-Wurf rand+150 / Zonen-Wurf im selben Takt)");
    PIN(nachher, "(c) P2 im NAECHSTEN Takt: +0x6 = 3 (faellt nach P3), +0x158 = rand + 150 (@0x801015b8-c8), in P3 einmal dekrementiert");
    {   re15_re2z_r37_produzenten(RE15_R37L_ALLES & ~RE15_R37L_C);   /* Gegenprobe Stand bis Runde 37 */
        int k = 0; for (; k < 32; k++) { rng_auf(k); if ((re15_re2z_rng_peek(0) & 1u) == 0u) break; }
        re15_actor_t *e = pin_stellen(0x00010001u, -12000, 0, 0);
        e->re2z_t15a = 1; e->re2z_t158 = 10;
        rng_auf(k);
        pin_takt();
        re15_re2z_r37_produzenten(RE15_R37L_ALLES);
        PIN(e->sub_state_2 == 3, "(c) Gegenprobe Stand bis Runde 37: Wurf-Takt faellt im SELBEN Takt nach P2/P3 (+0x6 = %u)", e->sub_state_2);
    }
    /* Der Wecker gewinnt gegen den Wurf: +0x158 == 0, dist < 0x1D4C, Sicht -> `sw 257,4` @0x8010157c (ganzes Wort). */
    int gewinnt = 1, n = 0;
    for (int k = 0; k < 32; k++) {
        re15_actor_t *e = pin_stellen(0x00010001u, -6000, 0, 0);
        e->re2z_t15a = 1; e->re2z_t158 = 0;
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0);
        pin_takt();
        if ((r0 & 1u) == 0u) {
            n++;
            if (!(e->state == 1 && e->sub_state_1 == 1 && e->sub_state_2 == 0)) gewinnt = 0;
        }
    }
    PIN(gewinnt && n > 0, "(c) Wurf und Wecker im selben Takt: der Wecker schreibt 0x101 ueber +0x6 = 2 (%d Faelle)", n);
    {   re15_actor_t *e = pin_stellen(0x00010001u, -8000, 0, 0);
        e->re2z_t15a = 1; e->re2z_t158 = 0;
        int k = 0; for (; k < 32; k++) { rng_auf(k); if ((re15_re2z_rng_peek(0) & 1u) == 0u) break; }
        rng_auf(k);
        pin_takt();
        PIN(e->sub_state_1 == 0 && e->sub_state_2 == 2 && e->re2z_t158 == 0,
            "(c) Gegenprobe dist 8000 >= 0x1D4C: Wurf bleibt (+0x6 = 2), +0x158 bleibt 0 (@0x80101580-88), ist %u/%u/%u t158 %d",
            e->state, e->sub_state_1, e->sub_state_2, (int)e->re2z_t158);
    }
}

/* DECISION[12] @0x8010460C: Griff aus dem Ausfall-Stoss (EXEC[12] Phase 3, Clip 0x1B): +0x1F0 < 0x514, Bild < 11, gleiche
 * Etage, Spieler unbeansprucht, Spieler im Sektor +-512 um die Blickrichtung -> 0x301. */
static re15_actor_t *pin_lunge(int32_t px, int32_t pz, int bild, int phase)
{
    re15_actor_t *e = pin_stellen(0x00030C01u, px, pz, 0);
    e->sub_state_2 = (uint8_t)phase;
    e->motion = 0x1B; e->anim_frame = (uint16_t)bild;
    return e;
}
static void pin_d12(void)
{
    re15_actor_t *e = pin_lunge(dx_grenze(0x514) - 1, 0, 5, 3);
    pin_takt();
    PIN(e->state == 1 && e->sub_state_1 == 3, "D12: Stoss-Bild 5, +0x1F0 = 0x513, Spieler geradeaus -> Griff 0x301 (ist %u/%u/%u)",
        e->state, e->sub_state_1, e->sub_state_2);
    e = pin_lunge(dx_grenze(0x514), 0, 5, 3);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 3), "D12 Gegenprobe +0x1F0 = 0x514 (sltiu 0x514 @0x8010463c): kein Griff (%u/%u)", e->state, e->sub_state_1);
    e = pin_lunge(1000, 0, 11, 3);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 3), "D12 Gegenprobe Bild 11 (sltiu 0xb @0x80104668): kein Griff (%u/%u)", e->state, e->sub_state_1);
    e = pin_lunge(-1000, 0, 5, 3);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 3), "D12 Gegenprobe Spieler hinten (Sektoren @0x801046c8/@0x8010470c): kein Griff (%u/%u)", e->state, e->sub_state_1);
    e = pin_lunge(1000, 0, 5, 1);
    pin_takt();
    PIN(!(e->state == 1 && e->sub_state_1 == 3), "D12 Gegenprobe Phase 1 (bne v0,3 @0x80104634): kein Griff (%u/%u)", e->state, e->sub_state_1);
    re15_re2z_r37_produzenten(RE15_R37L_ALLES & ~RE15_R37L_D12);
    e = pin_lunge(dx_grenze(0x514) - 1, 0, 5, 3);
    pin_takt();
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
    PIN(!(e->state == 1 && e->sub_state_1 == 3), "D12 Gegenprobe Stand bis Runde 37 (`default: break`): kein Griff aus dem Stoss (%u/%u)",
        e->state, e->sub_state_1);
}

/* Nachbesserung 2 — die Erholungsleiter des Treffer-Handlers 0x80105438 P3 (re2z_hit_main case 3), EMZ0.BIN selbst
 * disassembliert (Dossier Abschnitt "Nachbesserung 2"):
 *   80105a7c jal 0x80015614 (a3 = 512) / 80105a94 sltiu 0xfa0 / 80105aa4 lbu 0x800cfdcb / 80105aac andi 0x80 /
 *   80105ab0 bne -> 0x80105ad8 / 80105ab8 jal rand / 80105ac0 andi 0xf / 80105acc addiu 2817         ; 0xB01
 *   80105ae8 jal 0x80015614 (a3 = 1024) / 80105af0 beq -> 0x80105b14 / 80105b00 andi 1 / 80105b08 addiu 3073 ; 0xC01
 *   80105b14 jal rand / 80105b1c andi 1 / 80105b20 beq -> 0x80105b30 / 80105b24 addiu 513               ; 0x201
 *   80105b30 lbu a0,536 / 80105b34 lbu v1,332 / 80105b38 addiu 257 / 80105b3c bne a0,v1,Epilog / 80105b40 sw ; 0x101
 *   80105b44-50 0x10101 / 80105b5c +0x16B = (r1&0xf)<<1 / 80105b68 +0x158 = 30 / 80105b78 +0x16A = (r2&0x1f)+30 /
 *   80105b80 +0x15A = r3 + 300 / 80105b98 jal 0x800152c8(self, 0)
 * Zombie (0,0) Gier 1024 (Blick -z), Treffer-Zeile 1 Spalte 1 (= Haupt-Handler), Zustandswort 0x00030102 (2/1/3).
 * Der +0x144-Vektor des letzten e7c steht in re2z_root144/148 (37, -5); er wird im Gleichheitsfall EINMAL gedreht
 * angewandt. */
static re15_actor_t *pin_p3_stellen(int32_t px, int32_t pz, int gleich)
{
    re15_actor_t *e = pin_stellen(0x00030102u, px, pz, 0);
    e->rot_y = 1024; e->ai_flags = 0; e->re2z_hits1d2 = 1; e->re2z_flag222 = 0;
    e->motion = gleich ? (int16_t)e->re2z_walkclip : (int16_t)0x1B;
    e->anim_frame = 7; e->re2z_root144 = 37; e->re2z_root148 = -5;
    return e;
}
static void pin_p3(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Vorbedingungen der Lage: Spieler 6000 vor dem Zombie (im +-512- und +-1024-Kegel, +0x1F0 >= 0xFA0 -> weder
     * 0xB01- noch 0xC01-Wurf, erster Wurf = der 0x201-Wurf) bzw. 3000 vor ihm (< 0xFA0 -> der 0xB01-Wurf laeuft). */
    {   re15_actor_t *e = pin_p3_stellen(0, -6000, 1);
        PIN(re15_ai_arc_test(e, 0, -6000, 512) == 0 && re15_ai_arc_test(e, 0, -6000, 1024) == 0,
            "P3 Vorbedingung: Spieler (0,-6000) liegt vor dem Zombie mit Gier 1024 (Kegel +-512 und +-1024)"); }
    int32_t cs = re15_cos_q12(1024), sn = re15_sin_q12(1024);
    int32_t wx = (int32_t)(((int64_t)cs * 37 + (int64_t)sn * -5) >> 12);   /* 0x800152C8: ApplyMatrixSV(RotY(+0x76)) */
    int32_t wz = (int32_t)((-(int64_t)sn * 37 + (int64_t)cs * -5) >> 12);

    /* A: GLEICHER Clip (Normalfall). Je Strom-Lage k: r0 & 1 -> 0x201 (1 Wurf); sonst 0x10101 mit +0x16B/+0x158/+0x16A/
     *    +0x15A aus r1..r3 (4 Wuerfe) und genau einem Schritt um den gedrehten Vektor; naechster Takt = EXEC[1] P1 ohne P0. */
    int a_regel = 1, a_n10101 = 0, a_n201 = 0, a_weg = 1, a_folge = 1;
    for (int k = 0; k < 32; k++) {
        re15_actor_t *e = pin_p3_stellen(0, -6000, 1);
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0), r1 = re15_re2z_rng_peek(1), r2 = re15_re2z_rng_peek(2), r3 = re15_re2z_rng_peek(3);
        uint32_t d0 = re15_re2_rand_draws();
        pin_takt();
        uint32_t dw = re15_re2_rand_draws() - d0;
        if (r0 & 1u) {
            a_n201++;
            if (!(e->state == 1 && e->sub_state_1 == 2 && e->sub_state_2 == 0 && dw == 1)) a_regel = 0;
            if (e->x != 0 || e->z != 0) a_weg = 0;
        } else {
            a_n10101++;
            if (!(e->state == 1 && e->sub_state_1 == 1 && e->sub_state_2 == 1 && dw == 4)) a_regel = 0;
            if (e->re2z_gaitrow != (uint8_t)((r1 & 0xfu) << 1) || e->re2z_t158 != 30
                || e->re2z_dir16a != (uint8_t)((r2 & 0x1fu) + 30u) || e->re2z_t15a != (int16_t)(r3 + 300u)) a_regel = 0;
            if (e->x != wx || e->z != wz) a_weg = 0;
            /* naechster Takt: EXEC[1] P1 direkt (`beq v1,v0,0x80101b24` @0x80101A64): kein P0 (kein Zufalls-Startbild,
             * keine Gang-Seeds), der Gang-Zeilen-Timer +0x158 = 30 aus P3 wird dekrementiert (@0x80101B24-34), +0x15A
             * nur dekrementiert (@0x80101D68-74). */
            int16_t t15a = e->re2z_t15a; uint16_t af = e->anim_frame; uint8_t zeile = e->re2z_gaitrow;
            uint32_t d1 = re15_re2_rand_draws();
            pin_takt();
            if (!(e->state == 1 && e->sub_state_1 == 1 && e->re2z_t15a == (int16_t)(t15a - 1) && e->anim_frame == af
                  && e->re2z_gaittmr == 29 && e->re2z_gaitrow == zeile)) {
                a_folge = 0;
                printf("  k=%d Folgetakt: st %u/%u/%u t15a %d (soll %d) af %u (soll %u) gaittmr %u (soll 29) zeile %u (soll %u) wuerfe %u\n",
                       k, e->state, e->sub_state_1, e->sub_state_2, (int)e->re2z_t15a, (int)t15a - 1, e->anim_frame, af,
                       (unsigned)e->re2z_gaittmr, (unsigned)e->re2z_gaitrow, (unsigned)zeile,
                       (unsigned)(re15_re2_rand_draws() - d1));
            }
        }
    }
    PIN(a_regel && a_n10101 > 0 && a_n201 > 0,
        "P3 gleicher Clip: 0x201 bei r0&1 (1 Wurf), sonst 0x10101 mit +0x16B=(r1&0xf)<<1, +0x158=30, +0x16A=(r2&0x1f)+30,"
        " +0x15A=r3+300, 4 Wuerfe (@0x80105B14-80): %d x 0x10101, %d x 0x201", a_n10101, a_n201);
    PIN(a_weg, "P3 gleicher Clip: 0x800152C8(self,0) @0x80105B98 wendet den +0x144-Vektor (37,-5) gedreht EINMAL an -> (%d,%d);"
        " beim 0x201-Ausgang keine Bewegung", (int)wx, (int)wz);
    PIN(a_folge, "P3 -> 0x10101: der naechste Takt ist EXEC[1] P1 ohne P0 (Bild bleibt, Gang-Zeilen-Timer 30 -> 29 mit der"
        " P3-Zeile, +0x15A nur -1)");

    /* B: ANDERER Clip (`bne a0,v1` @0x80105B3C genommen): 0x101 aus dem Delay-Slot @0x80105B40, kein weiterer Wurf, keine
     *    Bewegung, +0x15A unberuehrt. */
    int b_regel = 1, b_n = 0;
    for (int k = 0; k < 32; k++) {
        rng_auf(k);
        if (re15_re2z_rng_peek(0) & 1u) continue;
        re15_actor_t *e = pin_p3_stellen(0, -6000, 0);
        rng_auf(k);
        uint32_t d0 = re15_re2_rand_draws();
        pin_takt();
        b_n++;
        if (!(e->state == 1 && e->sub_state_1 == 1 && e->sub_state_2 == 0 && re15_re2_rand_draws() - d0 == 1
              && e->x == 0 && e->z == 0 && e->re2z_t15a == 200)) b_regel = 0;
    }
    PIN(b_regel && b_n > 0, "P3 anderer Clip (Gegenprobe): 0x101, 1 Wurf, keine Bewegung, +0x15A bleibt (%d Lagen)", b_n);

    /* C: der EIN-ANGREIFER-RIEGEL @0x80105AA0-B0. Spieler 3000 vor dem Zombie: frei -> 0xB01-Wurf (B01 genau bei
     *    r0 & 0xf == 0); beansprucht -> KEIN 0xB01-Wurf, der erste Wurf ist der 0x201-Wurf. */
    int c_frei = 1, c_bean = 1, c_b01 = 0, c_n = 0;
    for (int k = 0; k < 96; k++) {
        rng_auf(k);
        uint32_t r0 = re15_re2z_rng_peek(0), r1 = re15_re2z_rng_peek(1);
        re15_actor_t *e = pin_p3_stellen(0, -3000, 1);
        rng_auf(k);
        uint32_t d0 = re15_re2_rand_draws();
        pin_takt();
        uint32_t dw = re15_re2_rand_draws() - d0;
        if ((r0 & 0xfu) == 0u) {
            c_b01++;
            if (!(e->state == 1 && e->sub_state_1 == 11 && dw == 1)) c_frei = 0;
        } else {
            int soll201 = (r1 & 1u) != 0u;
            if (soll201 ? !(e->sub_state_1 == 2 && dw == 2) : !(e->sub_state_1 == 1 && e->sub_state_2 == 1 && dw == 5)) c_frei = 0;
        }
        if ((r0 & 0xfu) != 0u && k > 40) continue;   /* beansprucht: alle B01-Lagen + die ersten Lagen */
        e = pin_p3_stellen(0, -3000, 1);
        re15_player_victim_force(0x10, 0, 0);        /* Spieler beansprucht (Port-Zwilling von 0x800CFDCB & 0x80) */
        rng_auf(k);
        d0 = re15_re2_rand_draws();
        pin_takt();
        dw = re15_re2_rand_draws() - d0;
        re15_player_victim_force_end();
        re15_player_victim_reset();
        pl->hp = 100;
        c_n++;
        int soll201 = (r0 & 1u) != 0u;            /* der erste Wurf ist jetzt der 0x201-Wurf */
        if (e->sub_state_1 == 11) c_bean = 0;
        if (soll201 ? !(e->sub_state_1 == 2 && dw == 1) : !(e->sub_state_1 == 1 && e->sub_state_2 == 1 && dw == 4)) c_bean = 0;
    }
    PIN(c_frei && c_b01 > 0, "P3 Spieler frei, +0x1F0 3000 < 0xFA0, vorn: 0xB01 genau bei r0&0xf == 0 (%d Lagen), sonst 0x201-Wurf", c_b01);
    PIN(c_bean && c_n > 0, "P3 Spieler BEANSPRUCHT (0x800CFDCB & 0x80 @0x80105AA0-B0): nie 0xB01, kein Ausfall-Wurf (%d Lagen)", c_n);

    /* D: Gegenprobe Stand bis Runde 37 (Bit P3 aus): die umgekehrte Bedingung und kein Riegel. */
    re15_re2z_r37_produzenten(RE15_R37L_ALLES & ~RE15_R37L_P3);
    int d_alt = 1, d_n = 0, d_b01 = 0;
    for (int k = 0; k < 32; k++) {
        rng_auf(k);
        if (re15_re2z_rng_peek(0) & 1u) continue;
        re15_actor_t *e = pin_p3_stellen(0, -6000, 1);
        rng_auf(k);
        pin_takt();
        d_n++;
        if (!(e->state == 1 && e->sub_state_1 == 1 && e->sub_state_2 == 0 && e->x == 0 && e->z == 0)) d_alt = 0;
        pin_takt();                                  /* naechster Takt: EXEC[1] P0 -> +0x15A = (rand>>3)+100, im P1-Rumpf -1 */
        if (!(e->sub_state_2 == 1 && e->re2z_t15a >= 99 && e->re2z_t15a <= 130)) d_alt = 0;
    }
    for (int k = 0; k < 96; k++) {
        rng_auf(k);
        if (re15_re2z_rng_peek(0) & 0xfu) continue;
        re15_actor_t *e = pin_p3_stellen(0, -3000, 1);
        re15_player_victim_force(0x10, 0, 0);
        rng_auf(k);
        pin_takt();
        re15_player_victim_force_end();
        re15_player_victim_reset();
        pl->hp = 100;
        if (e->sub_state_1 == 11) d_b01++;
    }
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
    PIN(d_alt && d_n > 0 && d_b01 > 0, "P3 Gegenprobe Stand bis Runde 37: gleicher Clip -> 0x101 (EXEC[1] P0 im naechsten Takt),"
        " beansprucht -> 0xB01 moeglich (%d / %d Lagen)", d_n, d_b01);
}

/* Nahkampf-Positivkontrolle: Messer-Sweep (test_re2_zombie_abc-Aufbau) mit allen Produzenten vs. Maske 0. */
static void pin_nahkampf(void)
{
    long t[2] = { 0, 0 };
    for (int m = 0; m < 2; m++)
        for (int seed = 0; seed < 64; seed++) { lauf_t o; lauf(&o, seed, 900, 1, m ? RE15_R37L_ALLES : 0u, -1, -1); t[m] += o.hits; }
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
    PIN(t[1] > 250, "Nahkampf (Messer, 64 Seeds x 900 Bilder) mit Spur L: %ld Treffer (Schranke test_re2_zombie_abc > 250)", t[1]);
    PIN(t[1] * 100 >= t[0] * 95, "Nahkampf nicht schlechter als der Stand bis Runde 37 (Maske 0): %ld gegen %ld (>= 95 %%)", t[1], t[0]);
}

/* RE2-Muster im Raum: gehende Zombies wechseln vor Leon in den Arme-hoch-Gang und greifen aus ihm (RE2-Original gemessen:
 * Wechsel bei d 1999/1987 durch (2), Griff bei d 1180/1152 — L_belege/re2_original_room20a0_jeder_takt.txt.gz). */
static void pin_raum(uint16_t raum, int32_t x, int32_t z, int montage)
{
    static gang_t o, g;
    gang_lauf(&o, RE15_R37L_ALLES, raum, x, z, 0, 900, montage, 0, 0);
    gang_lauf(&g, 0u, raum, x, z, 0, 900, montage, 0, 0);
    long w = 0, p2 = 0, dsum = 0, griff = 0, t201 = 0;
    for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) {
        w += o.z[sl].wechsel; p2 += o.z[sl].prod[1]; dsum += o.z[sl].d_summe; griff += o.z[sl].nach_griff; t201 += o.z[sl].takte_201; }
    long wg = 0; for (int sl = 1; sl < RE15_ACTOR_MAX; sl++) wg += g.z[sl].wechsel;
    PIN(w >= 1 && p2 >= 1, "ROOM%04X: gehende Zombies wechseln vor Leon in den Arme-hoch-Gang 0x201: %ld Wechsel, davon (2) %ld", raum, w, p2);
    PIN(w && dsum / w < 2000 && dsum / w > 1500, "ROOM%04X: mittleres +0x1F0 im Wechsel-Takt %ld (< 0x7D0; RE2-Original 1987/1999)", raum, w ? dsum / w : 0L);
    PIN(griff >= 1, "ROOM%04X: aus dem Arme-hoch-Gang wird gegriffen (%ld Griffe aus 0x201, %ld Takte im Gang)", raum, griff, t201);
    PIN(wg == 0, "ROOM%04X Gegenprobe Maske 0 (Stand bis Runde 37): kein Arme-hoch-Gang vor Leon (%ld)", raum, wg);
    re15_re2z_r37_produzenten(RE15_R37L_ALLES);
}

int main(int argc, char **argv)
{
    const char *modus = argc > 1 ? argv[1] : "mess";
    if (!lade_raum()) return 1;
    if (!strcmp(modus, "prod1"))    { if (pin_umgebung() < 0) { printf("FAIL: kein Zombie 0x10\n"); return 1; } pin_prod1(); }
    else if (!strcmp(modus, "prod2"))    { if (pin_umgebung() < 0) return 1; pin_prod2(); }
    else if (!strcmp(modus, "prod3"))    { if (pin_umgebung() < 0) return 1; pin_prod3(); }
    else if (!strcmp(modus, "b"))        { if (pin_umgebung() < 0) return 1; pin_b(); }
    else if (!strcmp(modus, "c"))        { if (pin_umgebung() < 0) return 1; pin_c(); }
    else if (!strcmp(modus, "d12"))      { if (pin_umgebung() < 0) return 1; pin_d12(); }
    else if (!strcmp(modus, "p3"))       { if (pin_umgebung() < 0) return 1; pin_p3(); }
    else if (!strcmp(modus, "nahkampf")) { pin_nahkampf(); }
    else if (!strcmp(modus, "raum1130")) { pin_raum(0x1130, -5900, 16350, 1); }
    else if (!strcmp(modus, "raum1180")) { pin_raum(0x1180, -4729, -16018, 0); }
    if (s_checks) {
        printf("test_r37_re2zombie %s: %d/%d\n", modus, s_checks - s_fails, s_checks);
        return s_fails ? 1 : 0;
    }
    if (!strcmp(modus, "mess")) {
        unsigned maske = argc > 2 ? (unsigned)strtoul(argv[2], NULL, 0) : 0u;
        int seeds = argc > 3 ? atoi(argv[3]) : 64;
        int waffe = argc > 4 ? atoi(argv[4]) : 1;
        long th = 0, tg = 0, t2 = 0, t3 = 0, tw = 0, td = 0, tm = 0, tmg = 0; unsigned tp[3] = {0, 0, 0};
        unsigned tq[5] = {0, 0, 0, 0, 0}; unsigned long long tdr = 0;
        for (int seed = 0; seed < seeds; seed++) {
            lauf_t o; lauf(&o, seed, 900, waffe, maske, -1, -1);
            tm += o.modal_auf; tmg += o.modal_im_griff;
            th += o.hits; tg += o.grab_frames; t2 += o.s2_frames; t3 += o.s3_frames; tw += o.walk_frames; td += o.deaths;
            for (int p = 0; p < 3; p++) tp[p] += o.prod[p];
            for (int q = 0; q < 5; q++) tq[q] += o.p3[q];
            tdr += o.draws;
            printf("seed %2d: treffer %4ld tode %ld griff %4ld s1=1 %5ld s1=2 %5ld s1=3 %5ld 0x201 %u/%u/%u wuerfe %u\n",
                   seed, o.hits, o.deaths, o.grab_frames, o.walk_frames, o.s2_frames, o.s3_frames,
                   o.prod[0], o.prod[1], o.prod[2], (unsigned)o.draws);
        }
        printf("SUMME maske=%u waffe=%d: treffer %ld tode %ld griff %ld s1=1 %ld s1=2 %ld s1=3 %ld 0x201 %u/%u/%u"
               " item-modal %ld (davon im Griff %ld)\n",
               maske, waffe, th, td, tg, tw, t2, t3, tp[0], tp[1], tp[2], tm, tmg);
        printf("SUMME P3-Ausgaenge 0xB01 %u / 0xC01 %u / 0x201 %u / 0x10101 %u / 0x101 %u, Wuerfe %llu\n",
               tq[0], tq[1], tq[2], tq[3], tq[4], tdr);
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
    if (!strcmp(modus, "gang")) {
        unsigned maske = argc > 2 ? (unsigned)strtoul(argv[2], NULL, 0) : RE15_R37L_ALLES;
        uint16_t raum  = argc > 3 ? (uint16_t)strtoul(argv[3], NULL, 16) : 0x1130;
        int32_t  x     = argc > 4 ? atoi(argv[4]) : -5900;
        int32_t  z     = argc > 5 ? atoi(argv[5]) : 16350;
        int16_t  rot   = argc > 6 ? (int16_t)atoi(argv[6]) : 0;
        int      bild  = argc > 7 ? atoi(argv[7]) : 900;
        int      mont  = argc > 8 ? atoi(argv[8]) : 0;
        int      renn  = argc > 9 ? atoi(argv[9]) : 0;
        static gang_t o; gang_lauf(&o, maske, raum, x, z, rot, bild, mont, renn, 1);
        char t[64]; snprintf(t, sizeof t, "ROOM%04X maske %u", raum, maske);
        gang_druck(&o, t);
        return 0;
    }
    printf("unbekannter Modus %s\n", modus);
    return 1;
}
