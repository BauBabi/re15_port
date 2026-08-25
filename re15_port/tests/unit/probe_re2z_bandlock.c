/**
 * @file probe_re2z_bandlock.c
 * @brief MESSSONDE — "RE2-AI: Zombies manchmal unverwundbar, bis sie mich einmal gebissen haben."
 *
 * probe_re2z_abc Detektor (C) misst NUR den +0x93-Latch und liefert 0 Vorfaelle. Diese Sonde
 * misst die ZWEITE Sperre des Ports: das ELEVATIONS-BAND in re15_player_weapon_fire
 * (re15_damage.c). Dort gilt fuer die Zombie-Familie
 *      lying = (grid_id & 0x80) || (RE2-owned && (re2z_flags21a & 0x2))
 *      lying -> eband &= ~0x40000000 (LEVEL weg), + 0x20000000 nur wenn dist < 0x1388
 * Ein AUFRECHT stehender/laufender Zombie, der eines dieser beiden Liege-Bits noch traegt, ist
 * damit fuer jeden LEVEL-Schuss unsichtbar.
 *
 * Teil 1 (Sweep): zaehlt Frames, in denen ein Zombie state==1 / +0x5 in {0,1,2,3} (aufrecht) ist,
 *                 hp >= 0, und (grid&0x80 || 21A&2) steht.
 * Teil 2 (Orakel): faehrt einen gemeldeten Fall erneut bis zum Frame, stellt den Spieler
 *                 LEVEL vor den Zombie und feuert N Schuesse -> Trefferzahl. Danach dasselbe
 *                 mit von Hand geloeschten Liege-Bits = Kontrolle.
 *
 * Aufruf: probe_re2z_bandlock [seeds=64] [frames=900] [weapon=3]
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
extern int      re15_player_aim_elevation(void);
extern void     re15_player_set_aim_elevation_for_test(int elev);

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

static int is_zombie(const re15_actor_t *e)
{
    return e->active && re15_re2z_owns_type(e->type);
}

static int s_weapon = 3;

static re15_actor_t *setup(int seed)
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
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(s_weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    for (int i = 0; i < seed * 13; i++) (void)re15_re2_rand();
    {   long long sx = 0, szz = 0; int n = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (is_zombie(&g_actors[s])) { sx += g_actors[s].x; szz += g_actors[s].z; n++; }
        if (n) { pl->x = (int32_t)(sx / n) + 2200 + (seed % 7) * 300;
                 pl->z = (int32_t)(szz / n) + 1500 - (seed % 5) * 400; }
        pl->rot_y = (int16_t)((seed * 337) & 0xfff);
    }
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
    return pl;
}

/* ORAKEL: Spieler LEVEL vor Zombie `slot` stellen und n Schuesse absetzen -> Trefferzahl.
 * Kein game_step, also keine KI-Ticks — die Bits bleiben so, wie sie beim Aufruf stehen. */
static int oracle_hits(int slot, int shots)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int hits = 0;
    pl->y = e->y; pl->floor = e->floor;
    pl->x = e->x + 1200; pl->z = e->z;
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    for (int i = 0; i < shots; i++) {
        int16_t hp0 = e->hp;
        e->hit_react &= (uint8_t)~3u;              /* Ein-Treffer-Latch fuer die Messung neutral */
        (void)re15_player_weapon_fire(s_weapon);
        if (e->hp < hp0) hits++;
    }
    return hits;
}

typedef struct { int seed, slot, frame; uint8_t grid; uint16_t f21a; uint8_t s1; } case_t;
static case_t s_cases[64]; static int s_ncase = 0;

/* "pin"-Modus: feste Parameter + harte Zusicherungen, als ctest registriert
 * (unit_re2z_bandlock_pin). Ohne Argument bleibt es die reine Messsonde. */
int main(int argc, char **argv)
{
    int pin    = (argc > 1 && strcmp(argv[1], "pin") == 0);
    int seeds  = pin ? 64  : ((argc > 1) ? atoi(argv[1]) : 64);
    int budget = pin ? 900 : ((argc > 2) ? atoi(argv[2]) : 900);
    s_weapon   = pin ? 3   : ((argc > 3) ? atoi(argv[3]) : 3);

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    long stale_frames = 0, upright_frames = 0; int stale_seeds = 0;
    long via_grid = 0, via_21a = 0, via_both = 0;
    int shown = 0;

    for (int seed = 0; seed < seeds; seed++) {
        re15_actor_t *pl = setup(seed);
        int seed_hit = 0;
        int prev_stale[RE15_ACTOR_MAX]; memset(prev_stale, 0, sizeof prev_stale);
        int shot_period = 7 + (seed % 11), shot_first = 20 + (seed % 17);
        for (int f = 0; f < budget; f++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (f >= shot_first && ((f - shot_first) % shot_period) == 0) {
                cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
            }
            frame(cur, edge);
            for (int s = 1; s < RE15_ACTOR_MAX; s++) {
                re15_actor_t *e = &g_actors[s];
                if (!is_zombie(e)) { prev_stale[s] = 0; continue; }
                int upright = (e->state == 1 && e->sub_state_1 <= 3 && e->hp >= 0);
                if (!upright) { prev_stale[s] = 0; continue; }
                upright_frames++;
                int g = (e->grid_id & 0x80) != 0, a = (e->re2z_flags21a & 2u) != 0;
                if (!g && !a) { prev_stale[s] = 0; continue; }
                stale_frames++; seed_hit = 1;
                if (g && a) via_both++; else if (g) via_grid++; else via_21a++;
                if (!prev_stale[s]) {
                    if (shown < 12) {
                        shown++;
                        printf("[STALE] seed %2d slot %d f%-4d st=%u s1=%u s2=%u grid=0x%02X "
                               "21A=%04X 1D3=%02X 10E=%04X hp=%d clip=%d\n",
                               seed, s, f, e->state, e->sub_state_1, e->sub_state_2,
                               e->grid_id, e->re2z_flags21a, e->re2z_self1d3, e->re2z_f10e,
                               e->hp, (int)e->motion);
                    }
                    if (s_ncase < 64) { s_cases[s_ncase].seed = seed; s_cases[s_ncase].slot = s;
                                        s_cases[s_ncase].frame = f; s_cases[s_ncase].grid = e->grid_id;
                                        s_cases[s_ncase].f21a = e->re2z_flags21a;
                                        s_cases[s_ncase].s1 = e->sub_state_1; s_ncase++; }
                }
                prev_stale[s] = 1;
            }
        }
        if (seed_hit) stale_seeds++;
    }

    printf("\n=== TEIL 1: STALE-LIEGE-BITS auf AUFRECHTEN Zombies ===\n");
    printf("Sweep %d Seeds x %d Frames, Waffe %d\n", seeds, budget, s_weapon);
    printf("aufrechte Zombie-Frames gesamt : %ld\n", upright_frames);
    printf("davon mit Liege-Bit            : %ld  (%.2f%%) in %d/%d Seeds\n",
           stale_frames, upright_frames ? 100.0 * (double)stale_frames / (double)upright_frames : 0.0,
           stale_seeds, seeds);
    printf("   nur grid&0x80 : %ld    nur 21A&2 : %ld    beide : %ld\n", via_grid, via_21a, via_both);

    printf("\n=== TEIL 2: ORAKEL (LEVEL-Schuss aus 1200 Einheiten, 30 Schuss) ===\n");
    int ncheck = (s_ncase < 6) ? s_ncase : 6;
    int pin_fail = 0, pin_checked = 0;
    for (int i = 0; i < ncheck; i++) {
        case_t *c = &s_cases[i];
        re15_actor_t *pl = setup(c->seed);
        int shot_period = 7 + (c->seed % 11), shot_first = 20 + (c->seed % 17);
        for (int f = 0; f <= c->frame; f++) {
            pl->hp = 100;
            uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
            if (f >= shot_first && ((f - shot_first) % shot_period) == 0) {
                cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
            }
            frame(cur, edge);
        }
        re15_actor_t *e = &g_actors[c->slot];
        int16_t hp_save = e->hp; uint8_t g_save = e->grid_id; uint16_t a_save = e->re2z_flags21a;
        uint8_t st = e->state, s1 = e->sub_state_1;
        int elev = re15_player_aim_elevation();
        int h_stale = oracle_hits(c->slot, 30);
        e->hp = hp_save; e->state = st; e->sub_state_1 = s1;
        e->grid_id = g_save; e->re2z_flags21a = a_save;
        re15_player_set_aim_elevation_for_test(-1);
        int h_down = oracle_hits(c->slot, 30);
        re15_player_set_aim_elevation_for_test(0);
        e->hp = hp_save; e->state = st; e->sub_state_1 = s1;
        e->grid_id = (uint8_t)(g_save & 0x7f); e->re2z_flags21a = (uint16_t)(a_save & ~2u);
        int h_clean = oracle_hits(c->slot, 30);
        printf("  Fall %d: seed %2d slot %d f%-4d st=%u s1=%u grid=0x%02X 21A=%04X elev=%d "
               "-> LEVEL = %2d | DOWN-Aim = %2d | ohne Liege-Bit = %2d\n",
               i, c->seed, c->slot, c->frame, st, s1, g_save, a_save, elev, h_stale, h_down, h_clean);
        if (pin) {
            pin_checked++;
            /* Der Nutzer-Befund selbst: ein LEBENDER, aufrecht laufender RE2-Zombie im
             * Kriech-Root darf mit normal gehaltener Waffe nicht unverwundbar sein. Vor dem
             * Fix stand hier LEVEL = 0 (gemessen 2026-08-27, alle sechs Faelle), danach 30. */
            if (h_stale <= 0) {
                printf("  FAIL Fall %d: LEVEL-Treffer = %d — der Kriecher ist wieder "
                       "unverwundbar (Band-Sperre zurueck). RE2s eigener Kandidatenfilter "
                       "FUN_800470C0 @0x80047124-64 hat KEIN Hoehen-Band.\n", i, h_stale);
                pin_fail = 1;
            }
            /* Gegenrichtung: nach unten zielen muss weiter treffen — sonst waere das Band
             * nicht praezisiert, sondern kaputt. */
            if (h_down <= 0) {
                printf("  FAIL Fall %d: DOWN-Aim-Treffer = %d — Regression im Band-Stempel\n",
                       i, h_down);
                pin_fail = 1;
            }
        }
    }

    if (pin) {
        /* NICHT VAKUANT: der Sweep MUSS Faelle gefunden haben. Findet er keine, ist entweder
         * die Messung kollabiert oder die Ursache anderswo behoben — beides verlangt eine neue
         * Herleitung, kein stilles Gruen. */
        if (pin_checked == 0) {
            printf("FAIL: 0 Faelle ausgewertet (Sweep fand kein stehengebliebenes Liege-Bit). "
                   "Gemessen 2026-08-27: 6 Faelle in 64 Seeds. Die Wache misst nichts mehr — "
                   "Basis neu herleiten, nicht die Schranke senken.\n");
            pin_fail = 1;
        }
        /* Der RE1.5-Kriecher (grid_id = 0x81, @0x801050d0 `ori v0,zero,0x81` /
         * @0x801050d4 `sb v0,9(v1)`) darf NICHT betroffen sein — dort ist die Band-Sperre
         * byte-true und bleibt bestehen. */
        if (via_grid != 0 || via_both != 0) {
            printf("FAIL: %ld via grid&0x80 / %ld via beide — der Fix haette den RE1.5-"
                   "Kriecher angefasst, der byte-true gesperrt bleiben muss.\n",
                   via_grid, via_both);
            pin_fail = 1;
        }
        printf("\n%d Faelle geprueft, %ld aufrechte Frames, %ld mit Liege-Bit (%ld via 21A&2).\n",
               pin_checked, upright_frames, stale_frames, via_21a);
        free(buf);
        if (pin_fail) { printf("RE2Z BANDLOCK PIN: FAIL\n"); return 1; }
        printf("RE2Z BANDLOCK PIN: Kriech-Root ist mit LEVEL-Zielen treffbar\n");
        return 0;
    }
    free(buf);
    return 0;
}
