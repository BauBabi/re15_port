/**
 * @file probe_re2_stagger.c
 * @brief DIAGNOSE — Zustandsfolge beim WIEDERHOLTEN Treffer auf einen RE2-Zombie.
 *
 * Nutzer-Report 2026-08-19: "Nach ein paar Treffern schwanken die Zombies nach hinten. Beim
 * Original wuerden sie dann umfallen, wenn man sie noch mal trifft. Bei uns nicht."
 *
 * Die Sonde faehrt den ECHTEN Weg (re15_game_step, Pad R1 -> SQUARE, echte ROOM1140-Spawns) und
 * protokolliert je Frame den KOMPLETTEN Reaktions-Zustand: +0x4/+0x5/+0x6, +0x222, +0x223,
 * +0x1D2, die dispatchte Tabellenzelle und HP. Damit ist messbar, OB der Niederschlag
 * (Flinch 0x501 -> Knockdown-Executor sub 5) ueberhaupt je feuert.
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

extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_hit_handler(void);
extern int  re15_actor_clip_len(const re15_actor_t *a);
extern void re15_enemy_bone_world_pos(const re15_actor_t *e, int bone, int32_t out[3]);

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

/* Die echte RE2-Bank (Spiegel von pc_enemy_load / test_re2_gore.c) — ohne sie ist
 * re15_actor_clip_len() == 0 und JEDE clip-getriebene Phase endet im Setz-Tick. */
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

int main(int argc, char **argv)
{
    int weapon = (argc > 1) ? atoi(argv[1]) : 3;
    int budget = (argc > 2) ? atoi(argv[2]) : 900;
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 999; }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    { re15_enemy_bank_t *b = load_re2_bank(0x10);
      printf("RE2-Bank EM010: %s (%d Knochen, %d Clips)\n", (b && b->ok) ? "geladen" : "FEHLT",
             b ? b->skel.bone_count : 0, b ? b->anim.clip_count : 0); }
    int slot = standing_zombie();
    if (slot < 0) { printf("FAIL: kein stehender Zombie\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - ((weapon < 3) ? 1200 : 2600);
    pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    printf("=== RE2-Zombie ROOM1140, Waffe %d — Zustandsfolge beim Dauerfeuer ===\n", weapon);
    printf("Zombie Slot %d Typ 0x%02X hp=%d\n", slot, e->type, e->hp);
    {   /* Clip-Laengen der Reaktions-Clips: 0 = keine Bank -> re2z_clip_done() liefert SOFORT 1 */
        re15_actor_t probe = *e;
        printf("clip_len:");
        for (int c = 0; c <= 8; c++) { probe.motion = (int16_t)c;
            printf(" [%d]=%d", c, re15_actor_clip_len(&probe)); }
        printf("\n");
        int32_t b0[3], b1[3], b8[3];
        re15_enemy_bone_world_pos(e, 0, b0);
        re15_enemy_bone_world_pos(e, 1, b1);
        re15_enemy_bone_world_pos(e, 8, b8);
        printf("Aktor-Wurzel  = (%d,%d,%d)\n", e->x, e->y, e->z);
        printf("Bone0 Huefte  = (%d,%d,%d)  dy=%d\n", b0[0], b0[1], b0[2], b0[1] - e->y);
        printf("Bone1 Brust   = (%d,%d,%d)  dy=%d\n", b1[0], b1[1], b1[2], b1[1] - e->y);
        printf("Bone8 Kopf    = (%d,%d,%d)  dy=%d\n", b8[0], b8[1], b8[2], b8[1] - e->y);
    }

    int hp_last = e->hp, hits = 0;
    int st_last = -1, s1_last = -1, s2_last = -1;
    int seen_stagger = 0, seen_flinch = 0, seen_kd_exec = 0, seen_kd_tbl = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        int hit = (e->hp < hp_last);
        if (hit) hits++;
        int h = re15_re2z_last_hit_handler();
        if (e->state == 2 && h == 2) seen_stagger = 1;
        if (e->state == 2 && h == 6) seen_kd_tbl  = 1;
        if (e->state == 1 && e->sub_state_1 == 5) seen_kd_exec = 1;
        if (hit || e->state != st_last || e->sub_state_1 != s1_last || e->sub_state_2 != s2_last) {
            printf("f%-4d %s st=%d s1=%2u s2=%2u | 222=%u 223=%4d 1D2=%u 1D0=%04X | h=%d "
                   "hp=%3d clip=%2u 93=%02X grid=%02X\n",
                   f, hit ? "HIT " : "    ", e->state, e->sub_state_1, e->sub_state_2,
                   e->re2z_flag222, (int)e->re2z_res223, e->re2z_hits1d2, e->re2z_hitdir1d0,
                   h, e->hp, (unsigned)e->motion, e->hit_react, e->grid_id);
        }
        st_last = e->state; s1_last = e->sub_state_1; s2_last = e->sub_state_2;
        hp_last = e->hp;
        if (e->hp < 0) { printf("f%-4d TOT\n", f); break; }
    }
    printf("--- %d Treffer | Stagger-Zelle gesehen=%d | Knockdown-Zelle(7438)=%d | "
           "Flinch->Exec5 gesehen=%d\n", hits, seen_stagger, seen_kd_tbl, seen_kd_exec);
    (void)seen_flinch;
    free(buf);
    return 0;
}
