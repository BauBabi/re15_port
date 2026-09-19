/* test_r16_liegende_unschiessbar.c — PIN (Phase 2, 2026-09-19, Thema liegende-und-aufstehen)
 *
 * Dossier: analysis/befunde_2026-09-19/liegende-zombies.md, Fix-Plan Schritt 3.
 * Nutzer: "Die Zombies, die im Original nur am Boden liegen, und die man nicht anschiessen
 *          koennen sollte und nicht auf uns reagieren sollten, reagieren auf uns —
 *          ROOM 1140, aber auch z.B. ROOM 10E0."
 *
 * Der passive Liegende (Sce_em_set-Deskriptor 0x87/0x88 = Nibble 7/8 + Bit 0x80) ist in
 * BEIDEN Originalen unschiessbar und ohne Selbst-Wecker:
 *   RE1.5: Decide-Stub `jr ra` @0x801039F4 (Tabelle @0x8011F9D8[0]); ANIMATE Phase 0 setzt
 *          `lbu v0,147(a1) / ori v0,v0,0x1 / sb v0,147(a1)` @0x80103AAC-AB8 JEDEN Tick ->
 *          der Resolver-Latch @0x80012404-18 ueberspringt ihn. Erst Phase 3 gibt frei
 *          (`andi 0xfe` @0x80103B64-68) — und dorthin kommt er nur ueber das Skript
 *          (Member_set(12,0x89/0x8A) -> `sb a2,9(a0)` @0x800411F8).
 *   RE2  : EXEC[7] P0 `ori 0x80 / sb 467` @0x80103804-14 (+0x1D3) und der Spawn-Remap
 *          `sh 0x4002,270` @0x80100A34-38 (+0x10E); die Kandidatenschleife FUN_800470C0
 *          filtert beides (@0x80047138-40 / @0x80047158-64). Decide-Stub @0x80103778.
 * Der Port nahm in re15_re2z_hit_filter_apply JEDEN EXEC[7]/EXEC[8]-Insassen von den Gates
 * (2)/(4) aus (Ausnahme fuer die 0x86-Fresser) — damit auch den 0x88-Liegenden. Seit dem Fix
 * gilt die Ausnahme nur noch, wenn der RE1.5-Zwilling treffbar ist (kein Nibble 7/8 mit 0x80).
 *
 * Echter Weg (re15_game_step + Pad, RDT + raum-eigenes sub00, ECHTE Baenke), wie die
 * Messsonde probe_r16_liegende_zombies:
 *   [C]  RE2, ROOM1140 + ROOM10E0: Pistole, Abstand 2600, D-Pad UNTEN (nur das DOWN-Band
 *        erreicht ihn, re15_damage.c Band @0x80101614-3C), 120 Bilder Dauerfeuer, danach 300
 *        Bilder Beobachtung -> 0 Treffer, HP unveraendert, Zustand 1/7/1, Pose-Hoehe gleich.
 *   [B2] RE2, beide Raeume: Sweep 12000 -> 400 mit ALLEN Aktoren (Fresser wachen) ueber 600
 *        Bilder -> keine Abweichung vom Liege-Zustand.
 *   [P]  RE2, ROOM1140 slot 2 (0x86-Fresser): derselbe Schuss mit DOWN-Band MUSS treffen
 *        (die Ausnahme fuer die Fresser lebt weiter).
 *   [N]  RE2, ROOM1140: Skript-Bump grid_id -> 0x89 (Member_set(12,0x89) @0x800411F8): der
 *        Liegende MUSS aufstehen und danach treffbar sein.
 *   [R]  RE1.5, ROOM1140: DOWN-Band -> 0 Treffer, +0x93 Bit 0 = 1 (Regressionswache).
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
#ifndef RE15_ASSET_RE2_DIR
#define RE15_ASSET_RE2_DIR "shared_assets/RE2"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

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

/* ---- Baenke (wie probe_r16_liegende_zombies.c) ------------------------------------------ */
static uint8_t *s_ems15 = NULL; static size_t s_ems15_n = 0;
static uint8_t *s_ems2  = NULL; static size_t s_ems2_n  = 0;
static uint8_t  s_scratch[3][0x80000];

static int load_bank15(uint8_t type, int idxslot)
{
    if (!s_ems15) s_ems15 = slurp(RE15_ASSET_PSX_DIR "/EMD/CDEMD0.EMS", &s_ems15_n);
    if (!s_ems15) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    int idx = re15_ems_index_for_type(type);
    size_t off = 0, len = 0;
    if (idx < 0 || re15_ems_get_entry(s_ems15, s_ems15_n, idx, &off, &len) != 0) return 0;
    if (len > sizeof s_scratch[0]) return 0;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    memcpy(s_scratch[idxslot], s_ems15 + off, len);
    re15_tim_t tim = (re15_tim_t){0};
    if (re15_emd_parse_container(s_scratch[idxslot], len, &eb->md1, &eb->skel, &eb->anim, &tim) == 0) {
        eb->ok = 1; eb->buf = NULL;
        eb->loco_ok = (re15_emd_parse_loco_bank(s_scratch[idxslot], len, &eb->skel_loco, &eb->anim_loco) == 0);
        eb->own_ok  = (re15_emd_parse_own_bank (s_scratch[idxslot], len, &eb->skel_own,  &eb->anim_own)  == 0);
        return 1;
    }
    return 0;
}

static int load_bank2(uint8_t type)
{
    if (!s_ems2) s_ems2 = slurp(RE15_ASSET_RE2_DIR "/CDEMD0.EMS", &s_ems2_n);
    if (!s_ems2) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems2, s_ems2_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

static void load_banks(void)
{
    int ok;
    if (s_flavor == RE15_AI_FLAVOR_RE2) ok = load_bank2(0x10) & load_bank2(0x11) & load_bank2(0x16);
    else ok = load_bank15(0x10, 0) & load_bank15(0x11, 1) & load_bank15(0x16, 2);
    CHECK(ok, "Baenke 0x10/0x11/0x16 laden nicht (Flavor %d) — ohne Bank ist clip_len 0 und der "
              "Pin misst eine Fiktion", s_flavor);
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

static int find_feeder(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (e->active && (e->grid_id & 0x0f) == 0x06) return s;
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
    printf("  %-10s f%-4d slot%02d typ=0x%02X st=%d/%d/%d hp=%4d grid=0x%02X mo=%2d fr=%-3d "
           "+93=0x%02X 1d3=0x%02X 10e=0x%04X topY=%d\n",
           tag, f, (int)(e - g_actors), e->type, e->state, e->sub_state_1, e->sub_state_2,
           e->hp, e->grid_id, (int)e->motion, (int)e->anim_frame, e->hit_react,
           e->re2z_self1d3, (unsigned)e->re2z_f10e, pose_top_y(e));
}

static int is_lying(const re15_actor_t *e)
{
    if (s_flavor == RE15_AI_FLAVOR_RE2) return e->state == 1 && e->sub_state_1 == 7;
    return e->state == 1 && (e->grid_id & 0x0f) == 8 && e->sub_state_1 == 0 && e->sub_state_2 == 0;
}

static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Zielen (R1 [+ D-Pad unten]) bis READY, dann `fire_frames` Bilder Dauerfeuer auf `slot`
 * (Abstand 2600, jedes Bild neu ausgerichtet). Liefert die Trefferzahl (HP-Abnahmen). */
static int aim_and_fire(int slot, int down, int fire_frames, int *hp_before)
{
    re15_actor_t *L  = &g_actors[slot];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    uint16_t aimbits = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(slot, 2600);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; face(slot, 2600); frame(aimbits, 0); }
    for (int f = 0; f < 6; f++) { pl->hp = 100; face(slot, 2600); frame(aimbits, 0); }
    CHECK(re15_player_aim_ready() && re15_player_aim_elevation() == (down ? -1 : 0),
          "%s ROOM%04X: Zielen nicht bereit (ready=%d elev=%d, soll %d)", flname(), s_room_id,
          re15_player_aim_ready(), re15_player_aim_elevation(), down ? -1 : 0);
    *hp_before = L->hp;
    int hp_last = L->hp, hits = 0;
    for (int f = 0; f < fire_frames; f++) {
        pl->hp = 100; face(slot, 2600);
        frame((uint16_t)(aimbits | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f % 20 == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (L->hp < hp_last) { hits++; if (hits <= 2) print_actor("TREFFER", f + 1, L); }
        hp_last = L->hp;
        if (L->hp < 0) break;
    }
    return hits;
}

/* ---- [C] + [R]: Schuss auf den Liegenden mit DOWN-Band ----------------------------------- */
static void part_c_down(void)
{
    printf("\n=== [C] ROOM%04X %s: Pistole, Abstand 2600, D-Pad UNTEN, 120 Bilder Feuer ===\n",
           s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    CHECK(lyer >= 0, "%s ROOM%04X: kein 0x88-Liege-Spawn", flname(), s_room_id);
    if (lyer < 0) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    print_actor("vor-schuss", 0, L);
    int32_t top0 = pose_top_y(L);
    int st0 = L->state, s10 = L->sub_state_1, s20 = L->sub_state_2;
    CHECK(is_lying(L), "%s ROOM%04X: Liegender ist vor dem Schuss nicht im Liege-Zustand (st=%d/%d/%d)",
          flname(), s_room_id, L->state, L->sub_state_1, L->sub_state_2);
    int hp0 = 0;
    int hits = aim_and_fire(lyer, 1, 120, &hp0);
    /* danach 300 Bilder beobachten */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int dev = -1;
    for (int f = 0; f < 300; f++) {
        pl->hp = 100; frame(0, 0);
        if (dev < 0 && !is_lying(L)) dev = f + 1;
    }
    print_actor("ende", 420, L);
    printf("  ERGEBNIS C(DOWN) %s ROOM%04X: %d Treffer, hp %d -> %d, topY %d -> %d, Abweichung=%d\n",
           flname(), s_room_id, hits, hp0, L->hp, top0, pose_top_y(L), dev);
    CHECK(hits == 0, "%s ROOM%04X: der passive Liegende (0x88) kassierte %d Treffer mit DOWN-Band — "
          "RE1.5 @0x80103AAC-AB8 / RE2 @0x80047138-64 sperren ihn", flname(), s_room_id, hits);
    CHECK(L->hp == hp0, "%s ROOM%04X: HP %d -> %d", flname(), s_room_id, hp0, L->hp);
    CHECK(L->state == st0 && L->sub_state_1 == s10 && L->sub_state_2 == s20,
          "%s ROOM%04X: Zustand %d/%d/%d -> %d/%d/%d (er hat auf den Schuss reagiert)",
          flname(), s_room_id, st0, s10, s20, L->state, L->sub_state_1, L->sub_state_2);
    CHECK(pose_top_y(L) == top0, "%s ROOM%04X: Pose-Hoehe %d -> %d", flname(), s_room_id,
          top0, pose_top_y(L));
    CHECK(dev < 0, "%s ROOM%04X: Liege-Zustand bei Bild %d nach dem Feuer verlassen",
          flname(), s_room_id, dev);
    CHECK((L->hit_react & 1u) != 0,
          "%s ROOM%04X: +0x93 Bit 0 muss stehen (RE1.5 `ori 0x1` @0x80103AB4 / RE2-Filter Gate "
          "2+4 -> hit_react|=1), gemessen 0x%02X", flname(), s_room_id, L->hit_react);
    if (s_flavor == RE15_AI_FLAVOR_RE2) {
        CHECK((L->re2z_self1d3 & 0x80u) && (L->re2z_f10e & 0x4000u),
              "ROOM%04X: die Original-Latches muessen stehen bleiben (+0x1D3=0x%02X, +0x10E=0x%04X; "
              "EXEC[7] P0 @0x80103804-14, Remap @0x80100A34-38)", s_room_id, L->re2z_self1d3,
              (unsigned)L->re2z_f10e);
    }
}

/* ---- [B2]: Sweep mit ALLEN Aktoren ------------------------------------------------------- */
static void part_b2(void)
{
    printf("\n=== [B2] ROOM%04X %s: Sweep 12000 -> 400, alle Aktoren, 600 Bilder ===\n", s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    CHECK(lyer >= 0, "%s ROOM%04X: kein 0x88-Liege-Spawn", flname(), s_room_id);
    if (lyer < 0) return;
    re15_actor_t *L = &g_actors[lyer];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t sx = L->x - 12000;
    pl->x = sx; pl->z = L->z; pl->y = L->y;
    int first_dev = -1; unsigned min_dist = 0xffffffffu;
    for (int f = 0; f < 600; f++) {
        if (sx < L->x - 400) sx += 200;
        pl->x = sx; pl->z = L->z; pl->hp = 100; pl->motion = 100;
        frame(0, 0);
        if (L->ai_dist < min_dist) min_dist = L->ai_dist;
        if (first_dev < 0 && !is_lying(L)) first_dev = f + 1;
    }
    print_actor("ende2", 600, L);
    printf("  ERGEBNIS B2 %s ROOM%04X: erste Abweichung=%d, min ai_dist=%u\n", flname(), s_room_id,
           first_dev, min_dist);
    CHECK(first_dev < 0, "%s ROOM%04X: Naehe weckte den 0x88-Liegenden bei Bild %d — Decide-Stub "
          "@0x801039F4 / @0x80103778 kennt keinen Naehe-Wecker", flname(), s_room_id, first_dev);
    CHECK(min_dist < 1000u, "%s ROOM%04X: Sweep kam nicht heran (min ai_dist=%u)", flname(), s_room_id, min_dist);
}

/* ---- [P]: der 0x86-Fresser bleibt mit DOWN-Band treffbar --------------------------------- */
static void part_p_feeder(void)
{
    printf("\n=== [P] ROOM%04X %s: 0x86-Fresser, DOWN-Band, 120 Bilder Feuer (muss treffen) ===\n",
           s_room_id, flname());
    bringup();
    int fd = find_feeder();                     /* Roh-Deskriptor VOR dem ersten KI-Tick (RE2-INIT
                                                 * schreibt den Deskriptor um) */
    CHECK(fd >= 0, "ROOM%04X: kein 0x86-Fresser", s_room_id);
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    if (fd < 0) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != fd) g_actors[s].active = 0;
    re15_actor_t *F = &g_actors[fd];
    print_actor("vor-schuss", 0, F);
    int hp0 = 0;
    int hits = aim_and_fire(fd, 1, 120, &hp0);
    print_actor("ende", 120, F);
    printf("  ERGEBNIS P %s ROOM%04X: %d Treffer, hp %d -> %d\n", flname(), s_room_id, hits, hp0, F->hp);
    CHECK(hits >= 1, "ROOM%04X: der 0x86-Fresser (slot %d) kassierte 0 Treffer — die Spawn-Pose-"
          "Ausnahme fuer die Fresser (Nutzer-Entscheidung, RE1.5-Zwilling treffbar) ist weg",
          s_room_id, fd);
}

/* ---- [N]: Skript-Bump 0x89 weckt ihn, danach ist er treffbar ------------------------------ */
static void part_n_script(void)
{
    printf("\n=== [N] ROOM%04X %s: Skript-Bump grid_id 0x88 -> 0x89, dann Schuss ===\n", s_room_id, flname());
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    CHECK(lyer >= 0, "%s ROOM%04X: kein 0x88-Liege-Spawn", flname(), s_room_id);
    if (lyer < 0) return;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int32_t top0 = pose_top_y(L);
    L->grid_id = (uint8_t)((L->grid_id & 0xf0u) | 9u);   /* Member_set(12,0x89) `sb a2,9(a0)` @0x800411F8 */
    int rose = -1;
    for (int f = 0; f < 600; f++) {
        pl->hp = 100; face(lyer, 2600);
        frame(0, 0);
        if (rose < 0 && L->state == 1 && L->sub_state_1 != 7) rose = f + 1;
        if (rose >= 0 && f > rose + 30) break;
    }
    print_actor("nach-bump", rose, L);
    CHECK(rose >= 0, "ROOM%04X: nach dem Skript-Bump auf 0x89 stand der Liegende in 600 Bildern "
          "nicht auf (D15.2 loescht 0x4000 @0x80104F0C)", s_room_id);
    CHECK(pose_top_y(L) < top0 - 1000, "ROOM%04X: Pose nach dem Bump nicht aufrecht (topY %d -> %d)",
          s_room_id, top0, pose_top_y(L));
    int hp0 = 0;
    int hits = aim_and_fire(lyer, 0, 120, &hp0);
    print_actor("ende", 120, L);
    printf("  ERGEBNIS N %s ROOM%04X: aufgestanden bei %d, danach %d Treffer, hp %d -> %d\n",
           flname(), s_room_id, rose, hits, hp0, L->hp);
    CHECK(hits >= 1, "ROOM%04X: der geweckte Liegende (0x89, aufgestanden) kassierte 0 Treffer — "
          "nach dem Aufstehen muessen die Gates offen sein (P4 `andi 0x7f` @0x80103914-18)", s_room_id);
}

static int load_room(unsigned room)
{
    s_room_id = room;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: RDT fehlt: %s\n", path); fails++; return 0; }
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse %s\n", path); fails++; return 0; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    return 1;
}

int main(void)
{
    printf("== test_r16_liegende_unschiessbar: passiver Liegender 0x88 im RE2-Flavor ==\n");
    /* RE2-Flavor: ROOM1140 + ROOM10E0 */
    s_flavor = RE15_AI_FLAVOR_RE2;
    if (load_room(0x1140)) { part_c_down(); part_b2(); part_p_feeder(); part_n_script(); }
    if (load_room(0x10E0)) { part_c_down(); part_b2(); }
    /* RE1.5-Flavor als Regressionswache */
    s_flavor = RE15_AI_FLAVOR_RE15;
    if (load_room(0x1140)) { part_c_down(); }

    if (fails) { printf("\n%d FAIL(s)\n", fails); return 1; }
    printf("\nPASS test_r16_liegende_unschiessbar\n");
    return 0;
}
