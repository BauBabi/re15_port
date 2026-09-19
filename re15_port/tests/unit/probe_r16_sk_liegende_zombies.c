/* probe_r16_sk_liegende_zombies.c — GEGEN-SONDE Skeptiker (Runde 16, 2026-09-19)
 *
 * Prueft die Nebenbehauptungen des Dossiers analysis/befunde_2026-09-19/liegende-zombies.md,
 * die die Hauptsonde NICHT misst:
 *   P) Fix-Plan-Praemisse "Fresser (0x86/0x06) sind treffbar": Pistole DOWN auf slot 2 (0x86)
 *      in ROOM1140 bzw. slot 2 (0x06) in ROOM10E0, beide Flavors -> Trefferzahl.
 *   Q) §3 letzter Punkt ("wer auf die Fresser schiesst, erfasst den Liegenden daneben"):
 *      RE2-Flavor, ALLE Aktoren aktiv, Spieler zielt DOWN auf einen Fresser (Abstand 800/1200/
 *      2600), 120 Frames Dauerfeuer -> Treffer je Slot (kassiert der Liegende slot 1 mit?).
 *   R) Elevations-Band-Abbildung (re15_damage.c:1458-1464, @0x80101624-3c): RE2-Flavor, nur
 *      der Liegende: DOWN bei 4900 / 5200 (Grenze 0x1388=5000), LEVEL bei 1200.
 *   T) Fix-Plan Gegenprobe (N): RE2-Flavor, Liegender, Skript-Bump grid_id=0x89 -> steht er
 *      auf (topY) und ist danach treffbar?
 * Aufruf: probe_r16_sk_liegende_zombies [1140|10E0]
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
#include "re15_ems.h"
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
extern int  re15_player_aim_elevation(void);
extern int  re15_actor_clip_len(const re15_actor_t *a);

static re15_rdt_t         s_rdt;
static unsigned           s_room_id = 0x1140;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_flavor = RE15_AI_FLAVOR_RE2;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems2  = NULL; static size_t s_ems2_n  = 0;
static uint8_t  s_scratch[3][0x80000];

static void load_bank15(uint8_t type, int idxslot)
{
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems15) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return;
    if (len > sizeof s_scratch[0]) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    memcpy(s_scratch[idxslot], s_ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_scratch[idxslot], len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
        eb->ok = 1; eb->buf = NULL;
        eb->loco_ok = (re15_emd_parse_loco_bank(s_scratch[idxslot], len, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->own_ok  = (re15_emd_parse_own_bank (s_scratch[idxslot], len, &eb->skel_own,  &eb->anim_own)  == 0);
    }
}

static void load_bank2(uint8_t type)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

static void load_banks(void)
{
    if (s_flavor == RE15_AI_FLAVOR_RE2) { load_bank2(0x10); load_bank2(0x11); load_bank2(0x16); }
    else { load_bank15(0x10, 0); load_bank15(0x11, 1); load_bank15(0x16, 2); }
}

static int32_t pose_top_y(const re15_actor_t *e)
{
    re15_enemy_bank_t *b = re15_enemy_find(e->type);
    if (!b || !b->ok || b->skel.bone_count <= 0) return e->y;
    int32_t best = 0x7fffffff;
    for (int i = 0; i < b->skel.bone_count; i++) {
        int32_t w[3]; re15_enemy_bone_world_pos(e, i, w);
        if (w[1] < best) best = w[1];
    }
    return (best == 0x7fffffff) ? e->y : best;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static const char *flname(void) { return s_flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5"; }

static void bringup(void)
{
    re15_ai_flavor_set((re15_ai_flavor_t)s_flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_banks();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
}

static int find_lyer(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (e->active && (e->grid_id & 0x8f) == 0x88) return s;
    }
    return -1;
}

static void park_far(void)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->hp = 100; pl->x = 30000; pl->z = 30000;
}

static void print_actor(const char *tag, int f, const re15_actor_t *e)
{
    const re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    double q = (double)dx*dx + (double)dz*dz, dist = q > 0 ? __builtin_sqrt(q) : 0;
    printf("  %-10s f%-4d slot%02d typ=0x%02X st=%d/%d/%d/%d hp=%4d grid=0x%02X mo=%2d/%-3d fr=%-3d "
           "+93=0x%02X 1d3=0x%02X 10e=0x%04X dist=%6.0f y=%d topY=%d\n",
           tag, f, (int)(e - g_actors), e->type, e->state, e->sub_state_1, e->sub_state_2,
           e->sub_state_3, e->hp, e->grid_id, (int)e->motion, re15_actor_clip_len(e),
           (int)e->anim_frame, e->hit_react, e->re2z_self1d3, (unsigned)e->re2z_f10e, dist,
           (int)e->y, pose_top_y(e));
}

/* Spieler auf (tx,tz) + Blick auf Slot */
static void place_facing(int slot, int32_t px, int32_t pz)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = px; pl->z = pz; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Feuert 120 Frames auf Slot `target` aus (px,pz); zaehlt Treffer je Slot (hp-Fall). */
static void fire_from(int target, int32_t px, int32_t pz, int down, int hits[RE15_ACTOR_MAX])
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint16_t aimbits = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    int hp_last[RE15_ACTOR_MAX];
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    place_facing(target, px, pz);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; place_facing(target, px, pz); frame(aimbits, 0);
    }
    for (int f = 0; f < 6; f++) { pl->hp = 100; place_facing(target, px, pz); frame(aimbits, 0); }
    for (int s = 0; s < RE15_ACTOR_MAX; s++) { hits[s] = 0; hp_last[s] = g_actors[s].hp; }
    for (int f = 0; f < 120; f++) {
        pl->hp = 100; place_facing(target, px, pz);
        frame((uint16_t)(aimbits | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f % 20 == 0) ? RE15_PAD_BIT_SQUARE : 0));
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            if (g_actors[s].hp < hp_last[s]) { hits[s]++; if (hits[s] <= 2) print_actor("TREFFER", f + 1, &g_actors[s]); }
            hp_last[s] = g_actors[s].hp;
        }
    }
}

static void print_hits(const char *tag, int hits[RE15_ACTOR_MAX])
{
    printf("  %s Treffer je Slot:", tag);
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active || hits[s]) printf(" slot%02d=%d(hp %d)", s, hits[s], g_actors[s].hp);
    printf("\n");
}

/* ---- P) Fresser treffbar? -------------------------------------------------------------- */
static void part_p(void)
{
    printf("\n=== [P] ROOM%04X Flavor %s: Pistole DOWN auf slot 2 (Fresser), Abstand 2600, nur slot 2 aktiv ===\n",
           s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    if (!g_actors[2].active) { printf("  FAIL: slot 2 inaktiv\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != 2) g_actors[s].active = 0;
    re15_actor_t *F = &g_actors[2];
    print_actor("vor-schuss", 0, F);
    int hits[RE15_ACTOR_MAX];
    fire_from(2, F->x - 2600, F->z, 1, hits);
    print_hits("P(DOWN)", hits);
    printf("  ERGEBNIS P(%s): slot2 grid=0x%02X %d Treffer; Ende st=%d/%d/%d topY=%d\n", flname(),
           F->grid_id, hits[2], F->state, F->sub_state_1, F->sub_state_2, pose_top_y(F));
}

/* ---- Q) Streuung auf den Liegenden bei Feuer auf einen Fresser ------------------------- */
static void part_q(int32_t back)
{
    printf("\n=== [Q] ROOM%04X Flavor %s: ALLE aktiv, Pistole DOWN auf slot 2 (Fresser) aus Abstand %d hinter ihm (Liegender in Schussrichtung dahinter) ===\n",
           s_room_id, flname(), (int)back);
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0 || !g_actors[2].active) { printf("  FAIL: Spawns fehlen\n"); return; }
    re15_actor_t *L = &g_actors[lyer], *F = &g_actors[2];
    /* Spieler auf der Geraden Liegender -> Fresser, `back` hinter dem Fresser */
    double dx = (double)(F->x - L->x), dz = (double)(F->z - L->z);
    double n = __builtin_sqrt(dx*dx + dz*dz); if (n <= 0) n = 1;
    int32_t px = F->x + (int32_t)(dx / n * back), pz = F->z + (int32_t)(dz / n * back);
    printf("  Liegender slot%d (%d,%d), Fresser slot2 (%d,%d), Spieler (%d,%d) -> Abstand zum Liegenden %.0f\n",
           lyer, (int)L->x, (int)L->z, (int)F->x, (int)F->z, (int)px, (int)pz, n + back);
    print_actor("vor-schuss", 0, L);
    int hits[RE15_ACTOR_MAX];
    fire_from(2, px, pz, 1, hits);
    print_hits("Q(DOWN)", hits);
    printf("  ERGEBNIS Q(back=%d): Liegender slot%d %d Treffer, Fresser slot2 %d Treffer; Liegender Ende st=%d/%d/%d topY=%d\n",
           (int)back, lyer, hits[lyer], hits[2], L->state, L->sub_state_1, L->sub_state_2, pose_top_y(L));
}

/* ---- R) Band-Grenzen am Liegenden ------------------------------------------------------ */
static void part_r(int32_t dist, int down)
{
    printf("\n=== [R] ROOM%04X Flavor %s: nur Liegender, Pistole %s, Abstand %d ===\n",
           s_room_id, flname(), down ? "DOWN" : "LEVEL", (int)dist);
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0) { printf("  FAIL: kein 0x88\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    int hits[RE15_ACTOR_MAX];
    fire_from(lyer, L->x - dist, L->z, down, hits);
    printf("  ERGEBNIS R(%s,%d): %d Treffer hp=%d; Ende st=%d/%d/%d grid=0x%02X topY=%d\n",
           down ? "DOWN" : "LEVEL", (int)dist, hits[lyer], L->hp, L->state, L->sub_state_1,
           L->sub_state_2, L->grid_id, pose_top_y(L));
}

/* ---- T) Skript-Bump 0x89 ---------------------------------------------------------------- */
static void part_t(void)
{
    printf("\n=== [T] ROOM%04X Flavor %s: nur Liegender, Skript-Bump grid_id=0x89, 300 Frames, dann DOWN-Feuer ===\n",
           s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0) { printf("  FAIL: kein 0x88\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    L->grid_id = 0x89;                                   /* Member_set(12,0x89) = sb a2,9(a0) @0x800411f8 */
    int first_up = -1;
    for (int f = 0; f < 300; f++) {
        park_far(); frame(0, 0);
        if (first_up < 0 && pose_top_y(L) < -1500) { first_up = f + 1; print_actor("AUFRECHT", f + 1, L); }
        if (f == 0 || f == 59 || f == 299) print_actor("bump", f + 1, L);
    }
    int hits[RE15_ACTOR_MAX];
    fire_from(lyer, L->x - 2600, L->z, 1, hits);
    printf("  ERGEBNIS T: erster aufrechter Frame=%d; nach Bump %d Treffer (DOWN, 2600); Ende st=%d/%d/%d grid=0x%02X 10e=0x%04X 1d3=0x%02X topY=%d\n",
           first_up, hits[lyer], L->state, L->sub_state_1, L->sub_state_2, L->grid_id,
           (unsigned)L->re2z_f10e, L->re2z_self1d3, pose_top_y(L));
}

static void run_room(unsigned room)
{
    s_room_id = room;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return; }
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", path); return; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    s_flavor = RE15_AI_FLAVOR_RE15; part_p();
    s_flavor = RE15_AI_FLAVOR_RE2;  part_p();
    s_flavor = RE15_AI_FLAVOR_RE2;  part_q(800); part_q(1200); part_q(2600);
    s_flavor = RE15_AI_FLAVOR_RE2;  part_r(4900, 1); part_r(5200, 1); part_r(1200, 0);
    s_flavor = RE15_AI_FLAVOR_RE15; part_r(4900, 1); part_r(1200, 0);
    s_flavor = RE15_AI_FLAVOR_RE2;  part_t();
    s_flavor = RE15_AI_FLAVOR_RE15; part_t();
}

int main(int argc, char **argv)
{
    unsigned room = 0x1140;
    if (argc > 1) room = (unsigned)strtoul(argv[1], NULL, 16);
    run_room(room);
    return 0;
}
