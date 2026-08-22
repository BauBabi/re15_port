/**
 * @file probe_re2z_crawl_attack.c
 * @brief MESS-SONDE — NUTZER-REPORT 2026-08-22: "Ein am Boden kriechender Zombie scheint Leon
 *        dort nicht angreifen zu koennen." (RE2-KI-Modus)
 *
 * Gemessen ueber den ECHTEN Weg (ROOM1030.RDT + SCD-Spawns + re15_game_step + GELADENE
 * RE2-Baenke): wie oft erreicht ein Kriecher den Angriffs-Substate (+0x5 = 1 = Kriecher-EXEC[1]
 * = DER GRIFF @0x801025EC), und faellt dabei Spieler-HP?
 *
 * Der Verdaechtige ist der EIN-ANGREIFER-RIEGEL Spieler+0x1D3 Bit 0x80 (0x800CFDCB):
 *   SETZER (byte-gelesen): Kriecher-DECIDE[0] @0x80102FAC-C0 / @0x80102FF8-0x80103004,
 *                          Kriecher-DECIDE[2] @0x80103B18-2C, Kriecher-Eintritt @0x8010459C-B0,
 *                          Griff-P0 @0x80102754-60
 *   LESER  : Kriecher-DECIDE[0] @0x80102F4C-58, DECIDE[2] @0x80103AD4-E0, @0x8010455C-68
 *   LOESCHER (byte-gelesen): Struggle-EXIT-P6 @0x8010AEF4/@0x8010AEF8 (`andi 0x7f` auf
 *            0x800CFDCB), HURT-Grab-Abbruch @0x80104FA0/@0x80104FAC, DEATH-Grab-Abbruch
 *            @0x801082E8/@0x801082F4
 * Die Sonde protokolliert den Riegel je Frame mit.
 *
 * Aufruf: probe_re2z_crawl_attack [frames=1200] [nkriecher=2]
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

static void load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; }
    else eb->type = 0;
}

static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

int main(int argc, char **argv)
{
    int budget = (argc > 1) ? atoi(argv[1]) : 1200;
    int ncrawl = (argc > 2) ? atoi(argv[2]) : 2;

    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1030.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1030;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    load_bank2(0x10); load_bank2(0x11); load_bank2(0x16);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();

    int cs[8], nc = 0;
    for (int s = 1; s < RE15_ACTOR_MAX && nc < ncrawl; s++)
        if (g_actors[s].active && re15_re2z_owns_type(g_actors[s].type)) cs[nc++] = s;
    if (nc == 0) { printf("FAIL: keine RE2-Zombies in ROOM1030 gespawnt\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        int keep = 0;
        for (int i = 0; i < nc; i++) if (cs[i] == s) keep = 1;
        if (!keep && g_actors[s].active && re15_re2z_owns_type(g_actors[s].type))
            g_actors[s].active = 0;
    }
    /* Kriecher erzwingen — byte-true der HURT-Eintritt @0x80107A54/@0x80107A58 (`sh 1,270` +
     * `sw 1,4` = Sub 0 = Lokomotion). Der schreibt Spieler+0x1D3 NICHT (der Riegel gehoert dem
     * Eintritt @0x8010459C-B0 mit Sub 1), also hier bewusst pl = NULL. */
    for (int i = 0; i < nc; i++) re15_re2z_enter_crawler(&g_actors[cs[i]], NULL, 0u);

    re15_actor_t *c0 = &g_actors[cs[0]];
    pl->x = c0->x + 700; pl->z = c0->z;
    pl->y = c0->y; pl->floor = c0->floor;
    printf("Kriecher: %d (Slots", nc);
    for (int i = 0; i < nc; i++) printf(" %d/typ0x%02X", cs[i], g_actors[cs[i]].type);
    printf(")  Riegel PL+0x1D3 nach dem Eintritt = 0x%02X\n", pl->re2z_self1d3);

    int grabs = 0, hpdrops = 0, s1prev[8];
    for (int i = 0; i < nc; i++) s1prev[i] = -1;
    int16_t hp_prev = pl->hp;
    long crawl_frames = 0;
    int first_lock_free = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;                                 /* nicht sterben */
        hp_prev = pl->hp;
        frame(0, 0);
        for (int i = 0; i < nc; i++) {
            re15_actor_t *e = &g_actors[cs[i]];
            if (!e->active) continue;
            if (e->re2z_f10e & 1u) crawl_frames++;
            if ((int)e->sub_state_1 != s1prev[i]) {
                if (f < 40 || e->sub_state_1 == 1)
                    printf("  f%-5d slot %d: +0x5 %d -> %d (10E=%04X dist=%u riegel=0x%02X)\n",
                           f, cs[i], s1prev[i], e->sub_state_1, e->re2z_f10e,
                           (unsigned)e->ai_dist, pl->re2z_self1d3);
                if (e->sub_state_1 == 1 && s1prev[i] != 1) grabs++;
                s1prev[i] = e->sub_state_1;
            }
        }
        if (pl->hp < hp_prev) hpdrops++;
        if (first_lock_free < 0 && !(pl->re2z_self1d3 & 0x80u)) first_lock_free = f;
        if (f % 300 == 299)
            printf("  [status] f%-5d riegel=0x%02X  c0: +0x5=%d +0x6=%d 10E=%04X dist=%u "
                   "pos=(%d,%d) PL=(%d,%d)\n",
                   f, pl->re2z_self1d3, c0->sub_state_1, c0->sub_state_2, c0->re2z_f10e,
                   (unsigned)c0->ai_dist, (int)c0->x, (int)c0->z, (int)pl->x, (int)pl->z);
    }
    printf("ERGEBNIS: Griff-Eintritte (+0x5 -> 1) = %d ; HP-Abfaelle = %d ; Kriech-Frames = %ld\n",
           grabs, hpdrops, crawl_frames);
    printf("          Riegel PL+0x1D3 am Ende = 0x%02X ; erstmals frei bei Frame %d\n",
           pl->re2z_self1d3, first_lock_free);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    return 0;
}
