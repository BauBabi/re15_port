/**
 * @file probe_schrot_bauch_y.c
 * @brief DIAGNOSE (kein ctest) — Runde 6: "Beim Schuss mit Schrotflinte Richtung Bauch
 * taumeln sie merkwuerdig, gehen dann mit Animation unterhalb des Bodens taumeln
 * weiter und stehen dann wieder."
 *
 * Faehrt den ECHTEN Weg (probe_re2_stagger-Vorlage): ROOM1140, RE2-Flavor, geladene
 * RE2-Bank, Waffe W8 (Pump-Schrot -> RE2-Zeile 7), EBEN gezielt (Bauch/Rumpf).
 * Ab der ersten HURT-Reaktion wird JEDER Frame geloggt: Zustandswort, Clip/Frame,
 * e->y (Aktor) UND die GERENDERTE Wurzel-/Brust-Welt-Y (re15_enemy_bone_world_pos -
 * die Renderwahrheit; "unter dem Boden" kann rein aus dem Clip-Root-Y kommen,
 * waehrend e->y 0 bleibt). PSX-y waechst nach UNTEN: Boden=0, unter Boden = y>0.
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

extern int  re15_player_aim_ready(void);
extern void re15_player_set_aim_elevation_for_test(int elev);
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
    int weapon = (argc > 1) ? atoi(argv[1]) : 8;      /* W8 = Pump-Schrot -> Zeile 7 */
    int budget = (argc > 2) ? atoi(argv[2]) : 2400;
    int seed_n = (argc > 3) ? atoi(argv[3]) : 0;      /* RNG-Variation je Lauf */
    int elev   = (argc > 4) ? atoi(argv[4]) : 0;      /* -1 = TIEF (Zone 0 Knockdown) */
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    if (seed_n) re15_damage_seed_rng(0x1234u + (unsigned)seed_n * 7919u);
    re15_inv_load_briefing();
    /* W8 liegt nicht im Briefing-Inventar - ohne Grant verpufft jeder SQUARE
     * (dokumentierte Falle, s. probe_re2_ztear.c:172-174). */
    if (re15_inv_find_item((uint8_t)weapon) < 0) (void)re15_inv_grant((uint8_t)weapon, 99);
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 999; }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    { re15_enemy_bank_t *b = load_re2_bank(0x10);
      printf("RE2-Bank EM010: %s (%d Knochen, %d Clips)\n", (b && b->ok) ? "geladen" : "FEHLT",
             b ? b->skel.bone_count : 0, b ? b->anim.clip_count : 0);
      if (!b || !b->ok) return 1; }
    int slot = standing_zombie();
    if (slot < 0) { printf("FAIL: kein stehender Zombie\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - 3600;                              /* Schrot FERN: >3000, sonst greift der
                                                       * RE1.5-Insta-Kill (W8<3000 -> hp=-1,
                                                       * @0x800124fc-1c, re15_damage.c:1640) und
                                                       * es gibt keine HURT-Reaktion zu messen */
    pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    e->hp = 4000;             /* HURT statt Tod: mehrere Reaktionen messen */
    printf("=== W%d EBEN (Bauch) auf Zombie Slot %d (hp=%d) — Y-Verlauf je Reaktion ===\n",
           weapon, slot, e->hp);
    int hp_last = e->hp;
    int logging = 0, still = 0, reaktionen = 0;
    int32_t y_min = 0, y_max = 0, b0y_max = -99999, b0y_min = 99999;
    int y_max_clip = -1, y_max_st = -1;
    for (int f = 0; f < budget && e->hp > 25 && reaktionen < 6; f++) {
        pl->hp = 100;
        if (elev) re15_player_set_aim_elevation_for_test(elev);
        /* Feuern nur, wenn der Zombie STEHT (state 1) — sonst Reaktion sauber ausspielen. */
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        if ((f % 120) == 0) {
            int es = re15_inv_equipped_slot();
            printf("  [takt] F%04d hp=%d qty=%d plst=%d aim=%d\n", f, e->hp,
                   (es >= 0) ? g_inv.slots[es].qty : -1, pl->state, re15_player_aim_ready());
        }
        if (e->hp < hp_last && !logging) {
            logging = 1; still = 0; reaktionen++;
            y_min = y_max = e->y; b0y_max = -99999; b0y_min = 99999;
            printf("-- Reaktion %d: Handler=%d hp %d->%d --\n",
                   reaktionen, re15_re2z_last_hit_handler(), hp_last, e->hp);
        }
        hp_last = e->hp;
        /* UNTER-BODEN-DETEKTOR unabhaengig vom Reaktions-Logging: jeder Frame mit
         * verschlepptem e->y ausserhalb des Ragdoll-Executors (st!=2) ist der
         * Abbruch-ohne-Gegenbuchung-Fall. */
        if (e->y > 100 && e->state != 2) {
            int32_t bb[3]; re15_enemy_bone_world_pos(e, 0, bb);
            printf("!UB F%04d st=%d/%d/%d clip=%2d ey=%5d b0y=%5d f10e=%04x\n",
                   f, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
                   (int)e->y, bb[1], e->re2z_f10e);
        }
        if (logging) {
            int32_t b0[3], b1[3];
            re15_enemy_bone_world_pos(e, 0, b0);
            re15_enemy_bone_world_pos(e, 1, b1);
            printf("F%04d st=%d/%d/%d clip=%2d af=%3u ey=%5d b0y=%5d b1y=%5d f10e=%04x\n",
                   f, e->state, e->sub_state_1, e->sub_state_2, (int)e->motion,
                   (unsigned)e->anim_frame, (int)e->y, b0[1], b1[1], e->re2z_f10e);
            if (e->y > y_max) { y_max = e->y; y_max_clip = e->motion; y_max_st = e->sub_state_1; }
            if (e->y < y_min) y_min = e->y;
            if (b0[1] > b0y_max) b0y_max = b0[1];
            if (b0[1] < b0y_min) b0y_min = b0[1];
            if (e->state == 1 && e->sub_state_1 == 1) still++;
            else still = 0;
            if (still >= 20) {
                logging = 0;
                printf(">> Reaktion %d ENDE: e.y[min..max]=[%d..%d] (max bei clip %d sub %d), "
                       "b0y[min..max]=[%d..%d]  %s\n",
                       reaktionen, y_min, y_max, y_max_clip, y_max_st, b0y_min, b0y_max,
                       (y_max > 100 || b0y_max > 100) ? "*** UNTER BODEN ***" : "ok");
            }
        }
    }
    if (logging)
        printf(">> Lauf endete IN der Reaktion: e.y[%d..%d] b0y[%d..%d]\n",
               y_min, y_max, b0y_min, b0y_max);
    printf("=== ENDE: %d Reaktionen, Zombie hp=%d st=%d/%d ===\n",
           reaktionen, e->hp, e->state, e->sub_state_1);
    return 0;
}
