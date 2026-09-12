/* probe_spinne_elevation.c - MESSUNG (Nutzer 2026-09-12: "die Spinnen scheinen immun
 * gegen Schuesse/Munition"). ADULT-Spinnen (0x25) in ROOM1090, echter game_step-
 * Schussweg, Sweep Flavor x Waffe(3,8) x Zielhoehe(TIEF/EBEN/HOCH). Klon der
 * Baby-Probe probe_re2_baby_spider_hit.c - nur Zielauswahl, Pad-Hoehenbits und
 * Bericht geaendert. Reine Messschiene: bestanden = gelaufen. */
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

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static re15_rdt_t         s_rdt;
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

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1090;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

static void track(int slot, int32_t back)
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

static void run(const char *tag, int flavor, int weapon, int budget, int pick, uint16_t elevbit)
{
    printf("\n===== %s (%s, Waffe %d, ROOM1090, Adult #%d) =====\n", tag,
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", weapon, pick);
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();
    if (re15_inv_find_item((uint8_t)weapon) < 0) (void)re15_inv_grant((uint8_t)weapon, 99);
    re15_player_set_equipped_weapon(weapon);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    int n26 = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x25u) n26++;
    printf("  Spawns Typ 0x25 direkt nach dem SCD: %d\n", n26);
    if (n26 == 0) {
        /* Die ROOM1090-Fixture spawnt in diesem Aufbau keine Adults (die zwei
         * Sce_em_set 0x25 fallen raus, enemy_ai_re2_spider.c:844) - fuer die
         * MESSUNG wird eine Adult-Spinne direkt gesetzt, exakt wie in
         * test_adult_spider_ai.c:36-37. */
        re15_actor_t *sp = &g_actors[9];
        memset(sp, 0, sizeof *sp);
        sp->active = 1; sp->type = 0x25; sp->state = 0;
        sp->x = 0; sp->y = 0; sp->z = 2000; sp->rot_y = 0;
        re15_enemy_apply_hitbox(sp, 0x25);
    }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = -1, seen = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x25u) {
            printf("   Baby #%d slot=%d grid=0x%02X hp=%d st=%d/%d/%d\n", seen, s,
                   g_actors[s].grid_id, g_actors[s].hp, g_actors[s].state,
                   g_actors[s].sub_state_1, g_actors[s].sub_state_2);
            if (seen == pick) slot = s;
            seen++;
        }
    if (slot < 0) { printf("  KEIN Aktor Typ 0x25 #%d\n", pick); return; }

    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;

    printf("  Ziel slot=%d st=%d/%d/%d hp=%d grid=0x%02X r=%d pos(%d,%d,%d)\n",
           slot, e->state, e->sub_state_1, e->sub_state_2, e->hp, e->grid_id,
           (int)e->hit_radius_min, (int)e->x, (int)e->y, (int)e->z);

    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back); frame(RE15_PAD_BIT_R1, 0);
    }
    printf("  aim_ready=%d\n", re15_player_aim_ready());

    int hp0 = e->hp, hp_last = e->hp, hits = 0, first_hit_f = -1;
    uint8_t st_at_hit = 0xff;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; track(slot, back);
        uint8_t st_before = e->state;
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elevbit),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->state != st_before || e->hp != hp_last) {
            if (hits < 6)
                printf("   Frame %3d: hp %d->%d, st %d->%d (%d/%d), +0x93=0x%02X\n",
                       f, hp_last, e->hp, st_before, e->state, e->sub_state_1,
                       e->sub_state_2, e->hit_react);
            if (first_hit_f < 0 && (e->state != st_before || e->hp != hp_last)) {
                first_hit_f = f; st_at_hit = e->state;
            }
            hits++;
        }
        hp_last = e->hp;
    }
    printf("  ERGEBNIS nach %d Frames: hp %d -> %d, state=%d/%d/%d, +0x93=0x%02X, "
           "erste Reaktion Frame %d (state %d), Zustandswechsel gesamt=%d\n",
           budget, hp0, e->hp, e->state, e->sub_state_1, e->sub_state_2,
           e->hit_react, first_hit_f, (int)st_at_hit, hits);
}

int main(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    {
        const char *base = getenv("RE15_ASSET_DIR");
        char path[600];
        snprintf(path, sizeof path, "%s/STAGE1/ROOM1090.RDT",
                 (base && *base) ? base : RE15_ASSET_PSX_DIR);
        size_t sz = 0; uint8_t *buf = slurp(path, &sz);
        if (!buf) { printf("RDT fehlt: %s - Messung uebersprungen\n", path); return 0; }
        if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 0; }
    }
    static const struct { const char *n; uint16_t bit; } EL[3] = {
        { "TIEF", RE15_PAD_BIT_DOWN }, { "EBEN", 0 }, { "HOCH", RE15_PAD_BIT_UP } };
    static const int FL[2] = { RE15_AI_FLAVOR_RE2, RE15_AI_FLAVOR_RE15 };
    static const int WP[2] = { 3, 8 };
    for (int fi = 0; fi < 2; fi++)
        for (int wi = 0; wi < 2; wi++)
            for (int ei = 0; ei < 3; ei++) {
                char tag[64];
                snprintf(tag, sizeof tag, "SPINNE %s", EL[ei].n);
                run(tag, FL[fi], WP[wi], 240, 0, EL[ei].bit);
            }
    printf("\nprobe_spinne_elevation: Messlauf beendet\n");
    return 0;
}
