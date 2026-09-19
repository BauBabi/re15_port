/* probe_r16_liegende_zombies.c — MESSSONDE (Runde 16, 2026-09-19)
 *
 * Nutzer: "Die Zombies, die im Original nur am Boden liegen, und die man nicht anschiessen
 *          koennen sollte und nicht auf uns reagieren sollten, reagieren auf uns —
 *          befund_1140_F2567_marke1 — ROOM 1140, aber auch z.B. ROOM 10E0."
 *
 * Faehrt den ECHTEN Weg (re15_game_step + Pad) in ROOM1140 bzw. ROOM10E0 (RDT + raum-eigenes
 * sub00 -> Sce_em_set-Spawns, ECHTE Baenke) in BEIDEN KI-Flavors und misst fuer den
 * Liege-Spawn (Deskriptor 0x88 = Nibble 8 + Bit 0x80):
 *   A) Spawn-Zustand nach 60 KI-Frames (Spieler fern): state/+0x5/+0x6/grid/Clip/+0x93/+0x1D3/+0x10E
 *   B) Distanz-Sweep: Spieler faehrt von +12000 auf +400 heran (200/Frame), dann 300 Frames
 *      daneben — erste Abweichung vom Liege-Zustand + Abstand dabei
 *   C) Schuss (Pistole 3, Abstand 2600): gerade (LEVEL) und mit D-Pad UNTEN (DOWN-Band) —
 *      Treffer? und was der Zombie danach 300 Frames lang tut (Zustandswort, Clip, Pose-Hoehe)
 *
 * Aufruf: probe_r16_liegende_zombies [1140|10E0] [re15|re2|both]
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

/* ---- Baenke (wie probe_lyer_1140.c) ------------------------------------------------------ */
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

/* Hoechster Punkt der posierten Skelett-Pose (min. Welt-Y; PSX-Y zeigt nach unten). */
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

static uint64_t sig_of(const re15_actor_t *e)
{
    return ((uint64_t)e->state << 40) | ((uint64_t)e->sub_state_1 << 32) |
           ((uint64_t)e->sub_state_2 << 24) | ((uint64_t)e->grid_id << 8) | (uint64_t)e->motion;
}

static int is_lying(const re15_actor_t *e)
{
    if (s_flavor == RE15_AI_FLAVOR_RE2) return e->state == 1 && e->sub_state_1 == 7;
    return e->state == 1 && (e->grid_id & 0x0f) == 8 && e->sub_state_1 == 0 && e->sub_state_2 == 0;
}

/* ---- A) Spawn-Zustand ------------------------------------------------------------------- */
static int part_a(void)
{
    printf("\n=== [A] ROOM%04X Flavor %s: Spawn-Zustand nach 60 KI-Frames, Spieler fern ===\n",
           s_room_id, flname());
    bringup();
    printf("  Roh-Deskriptoren (vor jedem KI-Tick):\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf("    slot%02d typ=0x%02X +0x9=0x%02X (nibble %u, bit80=%d) pos=(%d,%d,%d) mo=%d\n",
               s, e->type, e->grid_id, e->grid_id & 0xf, (e->grid_id & 0x80) ? 1 : 0,
               (int)e->x, (int)e->y, (int)e->z, (int)e->motion);
    }
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) print_actor("spawn", 60, &g_actors[s]);
    int lyer = find_lyer();
    printf("  -> LIEGENDER (Deskriptor 0x88) = slot %d\n", lyer);
    return lyer;
}

/* ---- B) Distanz-Sweep (nur der Liegende aktiv) ------------------------------------------ */
static void part_b(void)
{
    printf("\n=== [B] ROOM%04X Flavor %s: Distanz-Sweep 12000 -> 400, dann 300 Frames daneben ===\n",
           s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0) { printf("  FAIL: kein 0x88-Spawn\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* Der Spieler wird JEDEN Frame auf die Sweep-Position gesetzt (wie face() in Teil C):
     * sonst schiebt die Raumkollision ihn zurueck und der Sweep bleibt bei ~9700 haengen
     * (erster Lauf: min ai_dist 9714). */
    int32_t sx = L->x - 12000;
    pl->x = sx; pl->z = L->z; pl->y = L->y;
    uint64_t last = sig_of(L);
    int first_dev = -1; unsigned dev_dist = 0;
    int min_dist = 1 << 30;
    print_actor("sweep", 0, L);
    for (int f = 0; f < 360; f++) {
        if (sx < L->x - 400) sx += 200;
        pl->x = sx; pl->z = L->z; pl->hp = 100; pl->motion = 100;
        frame(0, 0);
        if ((int)L->ai_dist < min_dist) min_dist = (int)L->ai_dist;
        if (sig_of(L) != last || f % 100 == 99) print_actor("sweep", f + 1, L);
        if (first_dev < 0 && !is_lying(L)) { first_dev = f + 1; dev_dist = L->ai_dist; }
        last = sig_of(L);
    }
    printf("  ERGEBNIS B: erste Abweichung vom Liege-Zustand: %s (Frame %d, ai_dist=%u); "
           "min ai_dist=%d; Ende: st=%d/%d/%d grid=0x%02X mo=%d topY=%d +93=0x%02X\n",
           first_dev < 0 ? "KEINE" : "JA", first_dev, dev_dist, min_dist,
           L->state, L->sub_state_1, L->sub_state_2, L->grid_id, (int)L->motion,
           pose_top_y(L), L->hit_react);
}

/* ---- B2) Distanz-Sweep mit ALLEN Aktoren (Fresser wach, greifen den Spieler) ------------- */
static void part_b2(void)
{
    printf("\n=== [B2] ROOM%04X Flavor %s: Sweep wie B, aber ALLE Aktoren aktiv (Fresser wach) ===\n",
           s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0) { printf("  FAIL: kein 0x88-Spawn\n"); return; }
    re15_actor_t *L = &g_actors[lyer];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t sx = L->x - 12000;
    pl->x = sx; pl->z = L->z; pl->y = L->y;
    uint64_t last = sig_of(L);
    int first_dev = -1; unsigned dev_dist = 0;
    for (int f = 0; f < 600; f++) {
        if (sx < L->x - 400) sx += 200;
        pl->x = sx; pl->z = L->z; pl->hp = 100; pl->motion = 100;
        frame(0, 0);
        if (sig_of(L) != last || f % 100 == 99) print_actor("sweep2", f + 1, L);
        if (first_dev < 0 && !is_lying(L)) { first_dev = f + 1; dev_dist = L->ai_dist; }
        last = sig_of(L);
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) print_actor("ende2", 600, &g_actors[s]);
    printf("  ERGEBNIS B2: erste Abweichung vom Liege-Zustand: %s (Frame %d, ai_dist=%u); "
           "Ende: st=%d/%d/%d grid=0x%02X mo=%d topY=%d +93=0x%02X hp=%d\n",
           first_dev < 0 ? "KEINE" : "JA", first_dev, dev_dist,
           L->state, L->sub_state_1, L->sub_state_2, L->grid_id, (int)L->motion,
           pose_top_y(L), L->hit_react, L->hp);
}

/* ---- C) Schuss --------------------------------------------------------------------------- */
static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static void part_c(int down)
{
    printf("\n=== [C] ROOM%04X Flavor %s: Schuss Pistole, Abstand 2600, Zielen %s ===\n",
           s_room_id, flname(), down ? "UNTEN (D-Pad DOWN)" : "GERADE (LEVEL)");
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0) { printf("  FAIL: kein 0x88-Spawn\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint16_t aimbits = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(lyer, 2600);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; face(lyer, 2600); frame(aimbits, 0);
    }
    /* Elevation ist erst mit dem D-Pad im READY-Zustand gesetzt: ein paar Frames halten */
    for (int f = 0; f < 6; f++) { pl->hp = 100; face(lyer, 2600); frame(aimbits, 0); }
    printf("  aim_ready=%d elev=%d (soll %d)\n", re15_player_aim_ready(),
           re15_player_aim_elevation(), down ? -1 : 0);
    print_actor("vor-schuss", 0, L);
    int hp0 = L->hp, hp_last = L->hp, hits = 0;
    uint64_t last = sig_of(L);
    for (int f = 0; f < 120; f++) {
        pl->hp = 100; face(lyer, 2600);
        frame((uint16_t)(aimbits | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f % 20 == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (L->hp < hp_last) {
            hits++;
            if (hits <= 3) print_actor("TREFFER", f + 1, L);
        }
        hp_last = L->hp;
        if (sig_of(L) != last) print_actor("nach-shot", f + 1, L);
        last = sig_of(L);
        if (L->hp < 0) break;
    }
    /* danach 300 Frames beobachten (Spieler bleibt stehen, kein Schuss) */
    for (int f = 0; f < 300; f++) {
        pl->hp = 100;
        frame(0, 0);
        if (sig_of(L) != last || f % 100 == 99) print_actor("danach", 120 + f + 1, L);
        last = sig_of(L);
    }
    printf("  ERGEBNIS C(%s): %d Treffer, hp %d -> %d; Ende st=%d/%d/%d grid=0x%02X mo=%d topY=%d "
           "liegt_noch=%d\n", down ? "DOWN" : "LEVEL", hits, hp0, L->hp, L->state, L->sub_state_1,
           L->sub_state_2, L->grid_id, (int)L->motion, pose_top_y(L), is_lying(L));
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
    part_a();
    part_b();
    part_b2();
    part_c(0);
    part_c(1);
}

int main(int argc, char **argv)
{
    unsigned room = 0x1140;
    const char *fl = "both";
    if (argc > 1) room = (unsigned)strtoul(argv[1], NULL, 16);
    if (argc > 2) fl = argv[2];
    if (strcmp(fl, "re15") == 0 || strcmp(fl, "both") == 0) { s_flavor = RE15_AI_FLAVOR_RE15; run_room(room); }
    if (strcmp(fl, "re2")  == 0 || strcmp(fl, "both") == 0) { s_flavor = RE15_AI_FLAVOR_RE2;  run_room(room); }
    return 0;
}
