/**
 * @file probe_1d3gate_verify.c
 * @brief DIAGNOSE-SONDE: wo genau haengt test_re2_hit_repeat mit RE15_DIAG_1D3GATE=1?
 *
 * Kopie des Abschnitts (6) aus test_re2_hit_repeat.c (ROOM1190, Hunde-Welle sub13,
 * s_want_type 0x20, s_fire_sub 13) — der Test selbst bleibt unveraendert.
 * Neu ist ausschliesslich die Protokollierung: VOR jedem frame() werden
 * +0x1D3 (beide Haelften), +0x93, state, hp und der Aim-Zustand ungepuffert
 * ausgegeben. Die LETZTE Zeile im Protokoll ist damit der Zustand, in dem
 * re15_player_weapon_fire() nicht mehr zurueckkehrt.
 *
 * Zusaetzlich zaehlt die Sonde, wie oft je Bild wirklich gefeuert wird
 * (re15_player_weapon_fire-Aufrufe pro frame()) — dazu wird der Schuss NICHT
 * nachgebaut, sondern der HP-Abfall und der Stempel gemessen.
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

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);

static re15_rdt_t         s_rdt;
static int                s_room_id  = 0x1190;
static int                s_fire_sub = 13;
static uint8_t            s_want_type = 0x20;
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

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void hr_track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    extern void re15_esp_fx_reset(void);
    re15_esp_fx_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static int want_slot(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == s_want_type) return s;
    return -1;
}

static void run(const char *tag, re15_ai_flavor_t flavor, int weapon, int budget)
{
    re15_ai_flavor_set(flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250;
    }
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = want_slot();
    if (slot < 0) { printf("[%s] kein Gegner Typ %02X\n", tag, s_want_type); fflush(stdout); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (s_want_type == 0x20 && g_actors[slot].state == 4 && g_actors[slot].grid_id == 0x40)
        g_actors[slot].grid_id = 0x43;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - ((weapon < 3) ? 1200 : 2600);
    pl->z = e->z; pl->y = e->y; pl->rot_y = 1024;
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);

    printf("[%s] ---- ZIELPHASE (R1 halten, max 40 Bilder) ----\n", tag); fflush(stdout);
    int aimf = 0;
    for (; aimf < 40 && !re15_player_aim_ready(); aimf++) {
        pl->hp = 100; frame(RE15_PAD_BIT_R1, 0);
    }
    printf("[%s] aim_ready nach %d Bildern (ready=%d)\n", tag, aimf, re15_player_aim_ready());
    fflush(stdout);

    /* EXPERIMENT-SCHALTER (nur diese Sonde):
     *   PROBE_SET93=<hex>  schreibt +0x93 VOR jedem Bild auf den Wert
     *   PROBE_CLR93_ONCE=1 loescht +0x93 genau einmal vor Bild 0
     *   PROBE_MAXF=<n>     kuerzt das Budget */
    const char *env_set93 = getenv("PROBE_SET93");
    const char *env_clr1  = getenv("PROBE_CLR93_ONCE");
    const char *env_set1d3 = getenv("PROBE_SET1D3");   /* NEU: zweite Variable */
    const char *env_norad  = getenv("PROBE_NO_HITBOX"); /* NEU: Kandidat ausschliessen */
    int hp_last = e->hp, hits = 0;
    const int32_t back = (weapon < 3) ? 1200 : 2600;
    printf("[%s] ---- SCHUSS-SCHLEIFE (budget %d) ----\n", tag, budget); fflush(stdout);
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        hr_track(slot, back);
        if (env_set93) e->hit_react = (uint8_t)strtoul(env_set93, NULL, 16);
        if (env_clr1 && f == 0) e->hit_react = 0;
        if (env_set1d3) e->re2z_self1d3 = (uint8_t)strtoul(env_set1d3, NULL, 16);
        if (env_norad) e->hit_radius_min = 0;
        /* VOR dem Bild protokollieren: die LETZTE Zeile ist der Haenge-Zustand. */
        printf("  f=%-4d VOR   1d3=0x%02X(low7=%2u,bit80=%u) 93=0x%02X st=%d hp=%-4d "
               "grid=0x%02X 10E=0x%04X aimready=%d\n",
               f, e->re2z_self1d3, (unsigned)(e->re2z_self1d3 & 0x7f),
               (unsigned)((e->re2z_self1d3 >> 7) & 1), e->hit_react, e->state, e->hp,
               e->grid_id, (unsigned)e->re2z_f10e, re15_player_aim_ready());
        fflush(stdout);
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        if (e->hp < hp_last) hits++;
        printf("  f=%-4d NACH  1d3=0x%02X(low7=%2u) 93=0x%02X st=%d hp=%-4d Treffer=%d\n",
               f, e->re2z_self1d3, (unsigned)(e->re2z_self1d3 & 0x7f), e->hit_react,
               e->state, e->hp, hits);
        fflush(stdout);
        hp_last = e->hp;
        if (e->hp < 0) break;
    }
    printf("[%s] ENDE: %d Treffer, hp %d, 1d3=0x%02X, 93=0x%02X\n",
           tag, hits, e->hp, e->re2z_self1d3, e->hit_react);
    fflush(stdout);
}

int main(int argc, char **argv)
{
    int budget = (argc > 1) ? atoi(argv[1]) : 900;
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1190.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1190 HUND (Abschnitt 6 aus test_re2_hit_repeat) ===\n"); fflush(stdout);
    run("RE2 HUND", RE15_AI_FLAVOR_RE2, 3, budget);
    free(buf);
    return 0;
}
