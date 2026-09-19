/* probe_r16_skept2_liegende_zombies.c — GEGEN-SONDE Skeptiker 2 (Runde 16, 2026-09-19)
 *
 * Prueft, ob die 0 Treffer des RE1.5-Flavors am Liegenden (0x88) wirklich vom Tick-Guard
 * +0x93 Bit 0 (@0x80103aac-ab8) + Resolver-Latch (@0x80012404-18) kommen — und nicht von
 * Band/Reichweite/Test-Kontamination.
 *   D) Phase 0, DOWN-Zielen, 120 Frames Dauerfeuer: pro Frame Bit 0 pruefen; Bit 1 (0x2)
 *      = Resolver hat ihn als Kandidat GEWAEHLT und per Latch uebersprungen (@0x8001240c).
 *   D2) Phase 0, LEVEL-Zielen: Bit 1 darf NICHT erscheinen (Band schliesst ihn aus).
 *   E) Phase 1 (+0x6=1, Timer 0x7fff; Original setzt Bit 0 in Phase 1 NICHT neu), Bit 0 EINMAL
 *      testseitig geloescht, DOWN-Zielen: Treffer MUESSEN landen (=> Guard war der Blocker).
 *   F) wie E, aber LEVEL-Zielen: 0 Treffer (Band @0x80101614-3c).
 * Aufruf: probe_r16_skept2_liegende_zombies [1140|10E0]
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

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

static void bringup(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank15(0x10, 0); load_bank15(0x11, 1); load_bank15(0x16, 2);
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

static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* mode: 0 = Phase 0 unveraendert; 1 = Phase 1 + Bit 0 einmal geloescht */
static void shoot(int mode, int down)
{
    printf("\n=== ROOM%04X RE1.5 %s, Zielen %s, Pistole, Abstand 2600 ===\n", s_room_id,
           mode ? "PHASE 1 (+0x6=1, Timer 0x7fff) + Bit0 EINMAL geloescht" : "PHASE 0 (unveraendert)",
           down ? "DOWN" : "LEVEL");
    bringup();
    for (int f = 0; f < 60; f++) { park_far(); frame(0, 0); }
    int lyer = find_lyer();
    if (lyer < 0) { printf("  FAIL: kein 0x88-Spawn\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != lyer) g_actors[s].active = 0;
    re15_actor_t *L = &g_actors[lyer];
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    printf("  vor Eingriff: st=%d/%d/%d grid=0x%02X hp=%d +93=0x%02X mo=%d\n",
           L->state, L->sub_state_1, L->sub_state_2, L->grid_id, L->hp, L->hit_react, (int)L->motion);
    if (mode) {
        L->sub_state_2 = 1; L->ai_timer = 0x7fff;
        L->hit_react &= (uint8_t)~1u;
    }
    uint16_t aimbits = (uint16_t)(RE15_PAD_BIT_R1 | (down ? RE15_PAD_BIT_DOWN : 0));
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(lyer, 2600);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; face(lyer, 2600); frame(aimbits, 0); }
    for (int f = 0; f < 6; f++) { pl->hp = 100; face(lyer, 2600); frame(aimbits, 0); }
    printf("  aim_ready=%d elev=%d  vor-schuss: st=%d/%d/%d grid=0x%02X hp=%d +93=0x%02X\n",
           re15_player_aim_ready(), re15_player_aim_elevation(),
           L->state, L->sub_state_1, L->sub_state_2, L->grid_id, L->hp, L->hit_react);
    int hp0 = L->hp, hp_last = L->hp, hits = 0, bit0_low = 0, bit1_seen = 0, first_bit1 = -1;
    for (int f = 0; f < 120; f++) {
        pl->hp = 100; face(lyer, 2600);
        frame((uint16_t)(aimbits | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f % 20 == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (!mode && !(L->hit_react & 1)) bit0_low++;
        if (L->hit_react & 2) { bit1_seen++; if (first_bit1 < 0) first_bit1 = f + 1; }
        if (L->hp < hp_last) { hits++; if (hits <= 3)
            printf("  TREFFER f%-3d st=%d/%d/%d grid=0x%02X hp=%d +93=0x%02X mo=%d\n", f + 1,
                   L->state, L->sub_state_1, L->sub_state_2, L->grid_id, L->hp, L->hit_react, (int)L->motion); }
        hp_last = L->hp;
        if (L->hp < 0) break;
    }
    printf("  ERGEBNIS: %d Treffer, hp %d -> %d; Frames mit Bit0==0: %d/120; Frames mit Bit1(0x2)==1: %d "
           "(erstes f%d); Ende st=%d/%d/%d grid=0x%02X +93=0x%02X mo=%d\n",
           hits, hp0, L->hp, bit0_low, bit1_seen, first_bit1,
           L->state, L->sub_state_1, L->sub_state_2, L->grid_id, L->hit_react, (int)L->motion);
}

int main(int argc, char **argv)
{
    unsigned room = 0x1140;
    if (argc > 1) room = (unsigned)strtoul(argv[1], NULL, 16);
    s_room_id = room;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", RE15_ASSET_PSX_DIR, room);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    memset(&s_rdt, 0, sizeof s_rdt);
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", path); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    shoot(0, 1);   /* D  */
    shoot(0, 0);   /* D2 */
    shoot(1, 1);   /* E  */
    shoot(1, 0);   /* F  */
    return 0;
}
