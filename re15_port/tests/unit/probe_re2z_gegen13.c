/**
 * @file probe_re2z_gegen13.c
 * @brief GEGENPROBE (Runde 13): faehrt run_sweep aus test_re2_zombie_abc mit der
 *        EXAKTEN Vorfall-Bedingung (cwin Bilder gesperrt UND kein Zustandswechsel),
 *        aber mit frei waehlbarem Budget/CWIN - und protokolliert dabei, ob der
 *        ITEM-GET-MODAL den ganzen Schritt schon eingefroren hat.
 *
 * Hintergrund: probe_re2z_sperre29 zaehlt NUR zusammenhaengende (hp>=0 && +0x93&1)-
 * Laeufe - nicht die Zustands-Einfrier-Bedingung, die test_re2_zombie_abc seit
 * Runde 12 verlangt. Und es wertet Bilder mit, in denen re15_game_step wegen
 * re15_item_modal_active() SOFORT zurueckkehrt (game_step_common.c:866) - dort
 * bewegt sich NICHTS mehr, also ist jede "Sperre" dort ein Harness-Artefakt.
 *
 * Umgebung: RE15_SEEDS (64) RE15_FRAMES (900) RE15_WEAPON (3) RE15_CWIN (400)
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
extern int      re15_item_modal_active(void);

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
{ const char *v = getenv(k); return (v && *v) ? atoi(v) : def; }

static long g_inc_roh = 0;     /* Vorfaelle wie test_re2_zombie_abc sie zaehlt */
static long g_inc_echt = 0;    /* dito, aber ohne die Modal-Einfrierbilder */
static long g_modal_seeds = 0;

static void run_seed(int seed, int budget, int weapon, int cwin)
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

    int block_start[RE15_ACTOR_MAX];
    uint8_t block_st[RE15_ACTOR_MAX], block_s1[RE15_ACTOR_MAX];
    memset(block_st, 0, sizeof block_st); memset(block_s1, 0, sizeof block_s1);
    for (int s = 0; s < RE15_ACTOR_MAX; s++) block_start[s] = -1;

    int shot_period = 7 + (seed % 11), shot_first = 20 + (seed % 17);
    int modal_frame = -1;

    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        uint16_t cur = RE15_PAD_BIT_R1, edge = 0;
        if (f >= shot_first && ((f - shot_first) % shot_period) == 0) {
            cur |= RE15_PAD_BIT_SQUARE; edge = RE15_PAD_BIT_SQUARE;
        }
        frame(cur, edge);
        if (modal_frame < 0 && re15_item_modal_active()) modal_frame = f;

        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            re15_actor_t *e = &g_actors[s];
            if (!is_zombie(e)) { block_start[s] = -1; continue; }
            /* ---- ZEICHENGLEICH zu test_re2_zombie_abc.c (Runde-12-Bedingung) ---- */
            int blocked = (e->hp >= 0) && (e->hit_react & 1u);
            if (!blocked) block_start[s] = -1;
            else if (block_start[s] >= 0
                     && (e->state != block_st[s] || e->sub_state_1 != block_s1[s])) {
                block_start[s] = -1;
            }
            if (blocked) {
                if (block_start[s] < 0) {
                    block_start[s] = f; block_st[s] = e->state; block_s1[s] = e->sub_state_1;
                }
                if (f - block_start[s] == cwin) {
                    int frozen = (modal_frame >= 0 && modal_frame <= f);
                    g_inc_roh++;
                    if (!frozen) g_inc_echt++;
                    printf("  [VORFALL] seed=%d slot=%d typ=0x%02X f=%d (Beginn %d) st=%u s1=%u "
                           "hp=%d 1D3=0x%02X 10E=0x%04X  MODAL ab %d -> %s\n",
                           seed, s, e->type, f, block_start[s], e->state, e->sub_state_1,
                           (int)e->hp, e->re2z_self1d3, e->re2z_f10e, modal_frame,
                           frozen ? "HARNESS-ARTEFAKT (Welt steht)" : "ECHT");
                }
            }
        }
    }
    if (modal_frame >= 0) {
        g_modal_seeds++;
        printf("  [MODAL] seed=%d friert ab Bild %d ein (game_step_common.c:866)\n", seed, modal_frame);
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

    int seeds  = envi("RE15_SEEDS", 64);
    int budget = envi("RE15_FRAMES", 900);
    int weapon = envi("RE15_WEAPON", 3);
    int cwin   = envi("RE15_CWIN", 400);

    printf("== GEGENPROBE seeds=%d frames=%d waffe=%d cwin=%d ==\n", seeds, budget, weapon, cwin);
    for (int s = 0; s < seeds; s++) run_seed(s, budget, weapon, cwin);
    printf("== ERGEBNIS: %ld Vorfaelle (Testbedingung), davon %ld OHNE Modal-Einfrieren; "
           "%ld/%d Seeds mit Modal ==\n", g_inc_roh, g_inc_echt, g_modal_seeds, seeds);
    free(buf);
    return 0;
}
