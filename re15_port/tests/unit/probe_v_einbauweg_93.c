/**
 * @file probe_v_einbauweg_93.c
 * @brief VERIFIER-GEGENPROBE: WELCHER Mechanismus erzeugt den Zombie-Trefferabstand 15/5?
 *
 * Der zu pruefende Befund behauptet: der Abstand entstehe durch die per-Bild-NEUBERECHNUNG
 * des RE1.5-Riegels +0x93 Bit 0 in re15_re2z_hit_filter_apply (enemy_ai_re2_zombie.c).
 * Konkurrierende Erklaerung: der Port hat in re15_player_weapon_fire einen DIREKTEN zweiten
 * `continue` auf +0x1D3 (re15_damage.c:1340-1341, Kommentar "RE2-Trefferpause @0x80047138-40"),
 * gespeist vom Stempel re15_re2_stun_frames (re15_damage.c:1740-1744).
 *
 * ABLATIONEN (jeweils am ENDE jedes Bildes, also nach game_step inkl. Filterlauf):
 *   mode 0 : nichts                      (Basis)
 *   mode 1 : hit_react &= ~1             (Filter-Verdikt neutralisiert)
 *   mode 2 : re2z_self1d3 = 0            (Trefferpause neutralisiert)
 *   mode 3 : beides
 * Gemessen wird die Bildnummer jedes HP-Abfalls (= gelandeter Treffer) bei Dauerfeuer.
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
    g_current_room_id = 0x1140;
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

static uint8_t *s_ems = NULL; static long s_ems_sz = 0;
static re15_enemy_bank_t *load_re2_bank(uint8_t type)
{
    if (!s_ems) { size_t n = 0;
        s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n); s_ems_sz = (long)n; }
    if (!s_ems) return NULL;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return eb;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return NULL;
    if (re2_ems_load_bank(s_ems, (size_t)s_ems_sz, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return eb;
    }
    eb->type = 0; return NULL;
}

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

static void run(int weapon, int mode, int budget, int direct)
{
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    load_re2_bank(0x10);
    int slot = standing_zombie();
    if (slot < 0) { printf("  w%d mode%d: KEIN Zombie\n", weapon, mode); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - 1200; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    int prev_hp = e->hp, first = -1, last = -1, n = 0;
    int gaps[64]; int ng = 0;
    printf("  w%-2d mode%d %s Trefferbilder:", weapon, mode, direct ? "DIREKT" : "ABZUG ");
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        e->hp = 1000;                     /* Zombie am Leben halten: nur der TAKT interessiert */
        prev_hp = e->hp;
        { int es = re15_inv_equipped_slot();
          if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 200; }
        if (direct) {
            frame(RE15_PAD_BIT_R1, 0);
            (void)re15_player_weapon_fire(weapon);   /* Aufloeser DIREKT, ohne Abzugstakt */
        } else {
            frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, RE15_PAD_BIT_SQUARE);
        }
        if (e->hp < prev_hp) {
            if (n < 14) printf(" %d", f);
            if (first < 0) first = f;
            if (last >= 0 && ng < 64) gaps[ng++] = f - last;
            last = f; n++;
        }
        if (mode & 1) e->hit_react &= (uint8_t)~1u;
        if (mode & 2) e->re2z_self1d3 = 0;
    }
    printf("  | n=%d", n);
    if (ng > 0) {
        int mn = gaps[0], mx = gaps[0], sum = 0;
        for (int i = 0; i < ng; i++) { if (gaps[i] < mn) mn = gaps[i];
                                       if (gaps[i] > mx) mx = gaps[i]; sum += gaps[i]; }
        printf("  Abstand min=%d max=%d mittel=%.2f", mn, mx, (double)sum / ng);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    int budget = (argc > 1) ? atoi(argv[1]) : 400;
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);

    printf("=== Trefferabstand RE2-Zombie 0x10, Dauerfeuer, ROOM1140 ===\n");
    printf("SOLL laut Tabelle s_re2_stun_zombie: w2=15, w3=5\n");
    for (int mode = 0; mode < 4; mode++) {
        printf("-- mode %d (%s) --\n", mode,
               mode == 0 ? "Basis" : mode == 1 ? "+0x93 Bit0 ablatiert" :
               mode == 2 ? "+0x1D3 ablatiert" : "beides ablatiert");
        run(2, mode, budget, 0);
        run(3, mode, budget, 0);
        run(2, mode, budget, 1);
        run(3, mode, budget, 1);
    }
    return 0;
}
