/* probe_pruef_tempoursache.c — UNABHAENGIGE GEGENPRUEFUNG (nur Messung, kein Engine-Edit).
 *
 * Frage: verursacht die WIRKUNG des Gangtempo-Bits +0x21A&0x8000 (Setzer B
 * @0x801008D8-0x80100950, Trefferzweig @0x80100930-40) den ROOM1030-Steckenbleiber?
 *
 * Laeufe:
 *  (A) unveraendert
 *  (B1) Bit ab Frame 1 vor jedem Tick geloescht   (= Lauf (B) der Sonde tempo_weiche)
 *  (B0) Bit ab Frame 0 vor jedem Tick geloescht   (strenger: das Bit wirkt NIE)
 *  (S4) nur z4 entbittet, (S6) nur z6 entbittet
 * Zusaetzlich wird VOR dem ersten Tick gemeldet, welcher Slot das Bit schon traegt
 * (= Beweis, wann der INIT-Setzer laeuft).
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
#include "re15_ems.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

#define GATE_Z_NORTH (-22771)

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

static uint8_t *s_re2_ems = NULL; static size_t s_re2_n = 0;
static int load_bank_re2(uint8_t type)
{
    if (!s_re2_ems) s_re2_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_re2_n);
    if (!s_re2_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_re2_ems, s_re2_n, (int)type, eb, NULL) == 0) {
        eb->buf = NULL; eb->ok = 1; return 1;
    }
    eb->type = 0; return 0;
}

static void frame_step(void)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = 0; s_ctx.pad_pressed = 0;
    scd_vm_tick();
    re15_game_step(&s_ctx);
}

static int is_zombie(const re15_actor_t *e)
{
    return e->active && (e->type == 0x10 || e->type == 0x11 || e->type == 0x12 ||
                         e->type == 0x16 || e->type == 0x18);
}

/* killmode: 0 = aus, 1 = ab Frame 1, 2 = ab Frame 0. onlyslot>0 = nur dieser Slot. */
static void run(const char *name, int killmode, int onlyslot, int frames)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset();
    re15_damage_seed_rng(0x2545f491u);
    g_current_room_id = 0x1030;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    pl->x = -18050; pl->z = -8300;
    re15_collision_set_band(0);
    re15_inv_load_briefing();
    scd_room_reenter(&s_rdt, pl->x, pl->z, 0);
    (void)load_bank_re2(0x10); (void)load_bank_re2(0x11); (void)load_bank_re2(0x16);

    int live[RE15_ACTOR_MAX]; memset(live, 0, sizeof live);
    double path[RE15_ACTOR_MAX]; int32_t lx[RE15_ACTOR_MAX], lz[RE15_ACTOR_MAX];
    int32_t x0[RE15_ACTOR_MAX];
    int32_t z0[RE15_ACTOR_MAX];
    int f_z5[RE15_ACTOR_MAX], f_arm[RE15_ACTOR_MAX], crossed[RE15_ACTOR_MAX];
    uint16_t pre[RE15_ACTOR_MAX];
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!is_zombie(&g_actors[s])) continue;
        live[s] = 1; z0[s] = g_actors[s].z; x0[s] = g_actors[s].x;
        lx[s] = g_actors[s].x; lz[s] = g_actors[s].z; path[s] = 0.0;
        f_z5[s] = f_arm[s] = -1; crossed[s] = 0;
        pre[s] = (uint16_t)(g_actors[s].re2z_flags21a & 0x8000u);
    }

    printf("\n=== %s ===\n", name);
    printf("  VOR dem 1. Tick, +0x21A&0x8000:");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (live[s]) printf(" z%d=%s", s, pre[s] ? "SET" : "0");
    printf("\n");

    for (int f = 0; f < frames; f++) {
        if (killmode && f >= (killmode == 2 ? 0 : 1))
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (live[s] && (onlyslot <= 0 || s == onlyslot))
                    g_actors[s].re2z_flags21a &= (uint16_t)~0x8000u;
        frame_step();
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!live[s]) continue;
            re15_actor_t *e = &g_actors[s];
            double dx = (double)(e->x - lx[s]), dz = (double)(e->z - lz[s]);
            path[s] += (dx<0?-dx:dx) + (dz<0?-dz:dz);
            lx[s] = e->x; lz[s] = e->z;
            if (f == 99 || f == 199 || f == 299 || f == 445)
                printf("    f%-4d z%d Weg=%-8.0f Netto=%.0f\n", f, s, path[s],
                       (double)((e->x-x0[s])<0?-(e->x-x0[s]):(e->x-x0[s])) +
                       (double)((e->z-z0[s])<0?-(e->z-z0[s]):(e->z-z0[s])));
            if (e->member_0b == 5 && f_z5[s] < 0) f_z5[s] = f;
            if ((e->anim_flags & 0x1000u) && f_arm[s] < 0) f_arm[s] = f;
            if (z0[s] <= GATE_Z_NORTH && e->z > GATE_Z_NORTH) crossed[s] = 1;
        }
    }
    int narm = 0, ncross = 0, stuck = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!live[s]) continue;
        if (f_arm[s] >= 0) narm++;
        if (crossed[s]) ncross++;
        else if (z0[s] <= GATE_Z_NORTH && stuck < 0) stuck = s;
        printf("  z%d bit(nach Tick0)=%s  st.Z5=%-5d ARM=%-5d %s\n", s,
               (g_actors[s].re2z_flags21a & 0x8000u) ? "SET" : "0",
               f_z5[s], f_arm[s], crossed[s] ? "DURCH" : "*** STECKT ***");
    }
    printf("  => scharf=%d durch=%d steckt=z%d\n", narm, ncross, stuck);
}

int main(void)
{
    printf("== GEGENPRUEFUNG Tempo-Bit ROOM1030 ==\n");
    size_t n = 0;
    uint8_t *buf = slurp(RE15_ASSET_PSX_DIR "/STAGE1/ROOM1030.RDT", &n);
    if (!buf) { printf("FAIL: RDT\n"); return 1; }
    if (re15_rdt_parse(buf, n, &g_room_rdt) != 0) { printf("FAIL: parse\n"); return 1; }
    g_room_rdt_ok = 1; s_rdt = g_room_rdt;

    run("(A) unveraendert", 0, 0, 2000);
    re15_enemy_reset();
    run("(B1) Bit ab Frame 1 geloescht", 1, 0, 2000);
    re15_enemy_reset();
    run("(B0) Bit ab Frame 0 geloescht (wirkt NIE)", 2, 0, 2000);
    re15_enemy_reset();
    run("(S4) nur z4 entbittet ab Frame 0", 2, 4, 2000);
    re15_enemy_reset();
    run("(S6) nur z6 entbittet ab Frame 0", 2, 6, 2000);

    free(buf);
    g_room_rdt_ok = 0;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    printf("\n== FERTIG ==\n");
    return 0;
}
