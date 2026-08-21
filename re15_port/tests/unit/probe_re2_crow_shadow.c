/* probe_re2_crow_shadow.c — MESSUNG (Nutzer-Report 2026-08-21): "Platzende Kraehen durch die
 * Pistole hinterlassen immer noch Schatten."
 *
 * Gemessen wird der ECHTE Weg: ROOM10C0 (3x Sce_em_set type 0x21), RE2-Flavor, echter
 * re15_game_step mit Pad R1 -> SQUARE, echter Hitscan-Resolver. Nach dem Kill wird die Leiche
 * 240 Frames weiter getickt und JEDER Kanal protokolliert, den der PC-Renderer fuer den
 * Boden-Schatten liest (platform/pc/main.c:6428-6444):
 *     crow_shadow_w/h  = [+0x16C]+4/+6   (Halb-Ausdehnungen, EIN Wort 0x00C800C8 im Original)
 *     crow_pool        = Farbwort-Zustand (1 = Lache statt Grau)
 *     crow_tint        = Grauwert
 *     crow_hide        = Mesh verborgen (GIB)
 * plus die daraus abgeleitete RENDER-ENTSCHEIDUNG (zeichnet der Port ein Quad, und wie gross).
 *
 * Zweiter Lauf: die GIB-Zeile. Der Zeilen-Dispatch @0x80104A18 schickt +0x5 in {5,6,9,10,11,17}
 * zu 0x80102CA0 (GIB) -> CORPSE Sub 1 -> `sb zero,14([+0x16C])` @0x80103C34 = POOL-SLOT
 * FREIGEBEN (Allokator 0x80016480: Slot-Stride 104 ab 0x800CE698, +0x0E == 0 heisst FREI,
 * @0x800164AC-B4 / @0x800164D4). Der Port muss dort NICHTS mehr zeichnen.
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

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_aim_ready(void);
extern int  re15_player_aim_elevation(void);

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
    g_current_room_id = 0x10c0;
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

static int find_type(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == type) return s;
    return -1;
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
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static uint16_t elev_pad_for(const re15_actor_t *e)
{
    if (e->aim_band == 4) return RE15_PAD_BIT_UP;
    if (e->aim_band == 1) return RE15_PAD_BIT_DOWN;
    return 0;
}

/* Die RENDER-ENTSCHEIDUNG aus platform/pc/main.c:6428-6490, 1:1 nachgezogen. */
static void render_decide(const re15_actor_t *n, int *draws, int *hx, int *pool, int *tinted)
{
    int32_t nhx = 500;                       /* Default-Charakterschatten (500x600) */
    int corpse_pool = 0, crow_tinted = 0, no_record = 0;
    if (n->type == 0x21 && re15_ai_flavor() == RE15_AI_FLAVOR_RE2 && n->crow_shadow_w == 0)
        no_record = 1;                       /* rec+0x0E == 0 (@0x80103C34/@0x80103CBC) */
    if (n->type == 0x21 && n->crow_shadow_w != 0) {
        nhx = n->crow_shadow_w;
        if (n->crow_pool) corpse_pool = 1;
        else              crow_tinted = 1;
    }
    *hx = (int)nhx; *pool = corpse_pool; *tinted = crow_tinted;
    *draws = !no_record;
}

static void dump(const char *tag, const re15_actor_t *e, int f)
{
    int draws, hx, pool, tinted;
    render_decide(e, &draws, &hx, &pool, &tinted);
    printf("  %-6s f=%3d st=%d/%d/%d hp=%-4d y=%-6d dogflr=%-6d crowflr=%-6d shW=%-5u shH=%-5u "
           "pool=%d tint=%-3u hide=%d | RENDER: quadY=%d quad=%d halb=%d %s\n",
           tag, f, e->state, e->sub_state_1, e->sub_state_2, (int)e->hp,
           (int)e->y, (int)e->dog_floor_y, (int)e->crow_floor,
           e->crow_shadow_w, e->crow_shadow_h, (int)e->crow_pool, (unsigned)e->crow_tint,
           (int)e->crow_hide, (int)e->crow_floor, draws, hx,
           pool ? "LACHE(0x00BFBF10)" : (tinted ? "GRAU-TINT" : "STD-SCHATTEN"));
    if (!draws) printf("        (Record freigegeben -> kein Quad)\n");
}

static void run(const char *tag, int weapon)
{
    printf("\n===== %s =====\n", tag);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x21)
            printf("  KRAEHE slot %d pos=(%d,%d,%d) st=%d/%d/%d\n", s,
                   (int)g_actors[s].x, (int)g_actors[s].y, (int)g_actors[s].z,
                   g_actors[s].state, g_actors[s].sub_state_1, g_actors[s].sub_state_2);
    int slot = find_type(0x21);
    if (slot < 0) { printf("  KEINE KRAEHE\n"); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    printf("  SPAWN-LAGE: y=%d dog_floor_y=%d crow_floor=%d (crow_floor ist der Quad-Y des "
           "Renderers)\n", (int)e->y, (int)e->dog_floor_y, (int)e->crow_floor);
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, 2000);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
    }
    printf("  VOR DEM SCHUSS: "); dump("live", e, -1);

    int killed_at = -1;
    for (int f = 0; f < 400; f++) {
        pl->hp = 100; track(slot, 2000);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elev_pad_for(e)),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < 0) { killed_at = f; break; }
    }
    if (killed_at < 0) { printf("  NICHT GETOETET\n"); return; }
    printf("  TOT in Frame %d: ", killed_at); dump("kill", e, killed_at);

    for (int f = 0; f < 240; f++) {
        pl->hp = 100;
        frame(0, 0);
        if (f < 6 || f % 20 == 0 || f == 239) dump("corpse", e, f);
    }
}

int main(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM10C0.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 0; }

    /* +0x5 == RE1.5-Waffen-Id (@0x800124BC). Zeilen-Dispatch @0x80104A18:
     *   0..4, 13..16, 18 -> FLUG-Treffer | 5,6,9,10,11,17 -> GIB | 7,8 -> LAUNCH | 12 -> BODEN
     * Die Briefing-Ausruestung fuehrt nur Messer (1) und Browning (3), also misst diese Sonde
     * den PISTOLEN-Fall (die GIB-/Wandsplat-Zeilen pinnt test_re2_crow_shadow_release.c am
     * Brain-Einstieg). */
    run("KRAEHE RE2 / Waffe 3 (Pistole)  — FLUG-Zeile",   3);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    free(buf);
    return 0;
}
