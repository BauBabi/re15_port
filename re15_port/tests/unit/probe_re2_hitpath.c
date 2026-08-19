/* probe_re2_hitpath.c — DIAGNOSE: der ECHTE Schuss-/Schlag-Weg im RE2-Flavor.
 *
 * Nutzer-Report (nach v0.3.0): "Bei der RE2-AI kann ich immer noch keinen Zombie treffen.
 * Weder mit Schusswaffe noch mit Messer."
 *
 * Bisherige Messungen riefen re15_player_weapon_fire() SYNTHETISCH mit frisch gesetzter
 * Spielerposition. Diese Sonde faehrt stattdessen game_step() mit Pad-Eingaben (R1 halten ->
 * SQUARE) und protokolliert JEDES Gate von re15_player_weapon_fire fuer JEDEN Aktor.
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

extern int  re15_player_aim_active(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_player_aim_elevation(void);

static re15_rdt_t         s_rdt;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
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

static int wedge_in(const re15_actor_t *pl, int32_t ex, int32_t ez, int32_t reach, int32_t radius);

/* Schatten-Kopie der GATE-Kette aus re15_player_weapon_fire (nur LESEN) */
static void dump_gates(const char *tag, int weapon_id)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int elev = re15_player_aim_elevation();
    uint32_t pband = (elev > 0) ? 0x80000000u : (elev < 0) ? 0x20000000u : 0x40000000u;
    printf("  [%s] w=%d player (%d,%d,%d) rot=%d elev=%d pband=%08x aim_active=%d aim_ready=%d\n",
           tag, weapon_id, (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, elev,
           (unsigned)pband, re15_player_aim_active(), re15_player_aim_ready());
    for (int s = RE15_ACTOR_SLOT_PLAYER + 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        double dist = 0; { double q = (double)dx*dx + (double)dz*dz; dist = q > 0 ? __builtin_sqrt(q) : 0; }
        uint32_t eband;
        if (e->type == 0x21)      eband = (e->aim_band == 4) ? 0x80000000u : (e->aim_band == 1) ? 0x20000000u :
                                          (e->aim_band == 2) ? 0x40000000u : 0u;
        else if (e->grid_id & 0x80) eband = (dist < 5000.0) ? 0x20000000u : 0u;
        else                        eband = 0x40000000u;
        static const uint16_t reach_tab[22] = {1000,1100,1000,1000,1100,1000,1200,1000,1500,1000,
                                               1000,1000,1300,1800,1000,1000,1000,1000,1000,1100,
                                               1000,1000};
        int is_strip = (weapon_id==0 || (weapon_id>=3 && weapon_id<=8) || weapon_id==12 ||
                        weapon_id==13 || weapon_id==19 || weapon_id==21);
        int32_t reach = reach_tab[weapon_id];
        const char *why = "OK-Kandidat";
        if (e->hit_radius_min <= 0)         why = "RAUS: keine Hitbox (hit_radius_min<=0)";
        else if (e->state == 7)             why = "RAUS: state==7 CORPSE";
        else if ((e->hit_react & 3) == 3)   why = "RAUS: hit_react&3==3";
        else if ((pband & eband) == 0)      why = "RAUS: Elevations-Band";
        else if (is_strip) {
            if (!wedge_in(pl, e->x, e->z, reach, (int32_t)e->hit_radius_min)) why = "RAUS: Schuss-Streifen";
        } else {
            uint32_t R = (uint32_t)reach + e->hit_radius_min;
            if (dist >= (double)R) why = "RAUS: ausser Reichweite (Nahkampf)";
            else if (re15_ai_arc_test(pl, e->x, e->z, 0x400) != 0) why = "RAUS: Nahkampf-Kegel (nicht vorne)";
        }
        printf("      slot%02d typ=0x%02X st=%d s1=%d s2=%d hp=%d grid=0x%02X r=%d hitreact=0x%02X "
               "pos(%d,%d,%d) dist=%.0f eband=%08x  %s\n",
               s, e->type, e->state, e->sub_state_1, e->sub_state_2, e->hp, e->grid_id,
               (int)e->hit_radius_min, e->hit_react, (int)e->x, (int)e->y, (int)e->z, dist,
               (unsigned)eband, why);
    }
}

/* Spiegel von re15_gun_wedge_inside (re15_damage.c) — nur zur Diagnose */
extern int re15_sin_q12(int), re15_cos_q12(int);
static long long wcross(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{ return (long long)ax*bz - (long long)az*bx; }
static int wtri(int32_t VAx,int32_t VAz,int32_t VBx,int32_t VBz,int32_t V5x,int32_t V5z,
                int32_t rx,int32_t rz)
{
    if (wcross(V5x,V5z,rx-VAx,rz-VAz) > 0) return 0;
    if (wcross(V5x,V5z,rx-VBx,rz-VBz) < 0) return 0;
    if (wcross(VAx-VBx,VAz-VBz,rx-V5x,rz-V5z) > 0) return 0;
    return 1;
}
static int wedge_in(const re15_actor_t *pl, int32_t ex, int32_t ez, int32_t reach, int32_t radius)
{
    int32_t c = re15_cos_q12(pl->rot_y), s = re15_sin_q12(pl->rot_y);
    int32_t Vx[5], Vz[5];
    static const int32_t lx[5] = { 50, 50, 650, 650, 650 };
    int32_t lz[5]; lz[0]=radius+200; lz[1]=-(radius+200); lz[2]=radius+reach; lz[3]=-(radius+reach); lz[4]=0;
    for (int i = 0; i < 5; i++) {
        Vx[i] = (int32_t)(( (long long)c*lx[i] + (long long)s*lz[i]) >> 12);
        Vz[i] = (int32_t)((-(long long)s*lx[i] + (long long)c*lz[i]) >> 12);
    }
    int32_t rx = ex - pl->x, rz = ez - pl->z;
    return wtri(Vx[0],Vz[0],Vx[1],Vz[1],Vx[4],Vz[4],rx,rz) ||
           wtri(Vx[2],Vz[2],Vx[3],Vz[3],Vx[4],Vz[4],rx,rz);
}

static int pl_state(void)  { return g_actors[RE15_ACTOR_SLOT_PLAYER].state; }
static int pl_motion(void) { return (int)g_actors[RE15_ACTOR_SLOT_PLAYER].motion; }
static int pl_animf(void)  { return (int)g_actors[RE15_ACTOR_SLOT_PLAYER].anim_frame; }

static int first_enemy(void)
{
    /* bevorzugt ein STEHENDER Zombie (grid ohne 0x80) — die Liege-Spawns sind byte-true per
     * Elevations-Band ausgeschlossen und wuerden die Messung verfaelschen. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40
            && !(g_actors[s].grid_id & 0x80)) return s;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type < 0x40) return s;
    return -1;
}

static void bringup(void)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = (uint16_t)s_room_id;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_fire_sub >= 0 && s_rdt.sub_scd_count > s_fire_sub && s_rdt.sub_scd[s_fire_sub])
        scd_thread_start(2, s_rdt.sub_scd[s_fire_sub]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

/* Spieler direkt vor den Gegner stellen und ihn ansehen (wie nach dem Hinlaufen). */
static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    /* +X-Richtung: rot_y so, dass der Gegner vorne ist. */
    pl->rot_y = 1024;
}

static const char *s_room = "STAGE1/ROOM1140.RDT";
static void run(const char *tag, int flavor, int weapon, int ai_frames)
{
    printf("\n===== %s (flavor=%s, weapon=%d, %d AI-Frames vorher) =====\n",
           tag, flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", weapon, ai_frames);
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();                 /* Slot0 Messer, Slot1 Pistole x15, Slot2 Ammo */
    re15_player_set_equipped_weapon(weapon);  /* setzt auch den Equip-Slot (find_item) */

    /* AI erst hochlaufen lassen (Spieler weit weg), DANN das Ziel waehlen: erst nach dem INIT
     * traegt grid_id die laufende Semantik statt des Spawn-Verhaltensbytes. */
    for (int f = 0; f < ai_frames; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = first_enemy();
    if (slot < 0) { printf("  KEIN GEGNER\n"); return; }
    printf("  Gegner slot=%d typ=0x%02X grid=0x%02X pos(%d,%d,%d)\n", slot,
           g_actors[slot].type, g_actors[slot].grid_id,
           (int)g_actors[slot].x, (int)g_actors[slot].y, (int)g_actors[slot].z);

    /* ARENA: nur DIESER Gegner bleibt aktiv — sonst zielt die Auto-Aim-Latch auf einen anderen
     * und der Test misst Geometrie statt der Trefferkette. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    /* Feuerdistanz: Messer braucht Naehe (reach 1100 + r 400), Pistole nicht (Streifen unbegrenzt) */
    const int32_t fire_back = (weapon < 3) ? 1200 : 2600;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(slot, fire_back);

    int hp0 = g_actors[slot].hp;
    dump_gates("vor R1", weapon);

    /* R1 halten bis die Aim-FSM bereit ist */
    int raise = 0;
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        frame(RE15_PAD_BIT_R1, 0); raise++;
        if (f < 6 || (f % 10) == 0)
            printf("   R1 f%02d: pl st=%d motion=%d anim_f=%d hp=%d aim_act=%d ready=%d "
                   "player_mode=%d msgdisp=%d msgq=%d pause=%08x\n", f,
                   pl_state(), pl_motion(), pl_animf(), g_actors[0].hp,
                   re15_player_aim_active(), re15_player_aim_ready(),
                   (int)g_scd.player_mode, (int)g_scd.message_display_frames,
                   (int)g_scd.message_query, (unsigned)g_re15_pauseflags);
    }
    printf("  aim_ready nach %d R1-Frames: active=%d ready=%d\n", raise,
           re15_player_aim_active(), re15_player_aim_ready());
    dump_gates("aim bereit", weapon);

    /* SQUARE -> feuern, bis der Gegner TOT ist (oder 900 Frames): WIE OFT trifft es? */
    int hp_min = hp0, hits = 0, hp_last = hp0, kd = 0;
    uint8_t grid_last = g_actors[slot].grid_id;
    for (int f = 0; f < 900; f++) {
        if (g_actors[slot].grid_id != grid_last) {
            printf("   grid_id %02X -> %02X in Frame %d (st=%d s1=%d)\n", grid_last,
                   g_actors[slot].grid_id, f, g_actors[slot].state, g_actors[slot].sub_state_1);
            grid_last = g_actors[slot].grid_id;
        }
        if (g_actors[slot].state == 1 && g_actors[slot].sub_state_1 == 5) kd++;
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        if (g_actors[slot].hp < hp_min) hp_min = g_actors[slot].hp;
        if (g_actors[slot].hp < hp_last) {
            hits++;
            printf("   TREFFER #%d in Frame %d: hp %d -> %d, hitreact=0x%02X st=%d\n",
                   hits, f, hp_last, g_actors[slot].hp, g_actors[slot].hit_react,
                   g_actors[slot].state);
        }
        hp_last = g_actors[slot].hp;
        if (g_actors[slot].hp < 0) break;
    }
    printf("  ERGEBNIS: %d TREFFER (max 900 Frames), hp %d -> %d (min %d), state=%d, "
           "hitreact=0x%02X, grid=0x%02X, Knockdown-Frames=%d\n",
           hits, hp0, g_actors[slot].hp, hp_min, g_actors[slot].state, g_actors[slot].hit_react,
           g_actors[slot].grid_id, kd);
    dump_gates("nach dem Feuern", weapon);
    /* Direktaufruf zum Vergleich (der alte synthetische Weg) */
    g_actors[slot].hit_react = 0;
    int direct = re15_player_weapon_fire(weapon);
    printf("  Direktaufruf re15_player_weapon_fire(%d) = %d (hp jetzt %d)\n",
           weapon, direct, g_actors[slot].hp);
}

int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    run("RE1.5 / Pistole",  RE15_AI_FLAVOR_RE15, 3, 60);
    run("RE2   / Pistole",  RE15_AI_FLAVOR_RE2,  3, 60);
    run("RE1.5 / Messer",   RE15_AI_FLAVOR_RE15, 1, 60);
    run("RE2   / Messer",   RE15_AI_FLAVOR_RE2,  1, 60);

    /* Gegenprobe: dieselbe Trefferkette bei den ANDEREN RE2-Brains (Hund / Kraehe / Spinne). */
    struct { const char *sub; int room_id; int fsub; const char *name; } more[] = {
        { "STAGE1/ROOM1190.RDT", 0x1190, 13, "HUND"   },
    };
    for (unsigned i = 0; i < sizeof more / sizeof more[0]; i++) {
        snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, more[i].sub);
        size_t s2 = 0; uint8_t *b2 = slurp(path, &s2);
        if (!b2) { printf("\n(%s: RDT fehlt)\n", more[i].name); continue; }
        if (re15_rdt_parse(b2, s2, &s_rdt) != 0) { printf("\n(%s: Parse)\n", more[i].name); continue; }
        s_room_id = more[i].room_id; s_fire_sub = more[i].fsub;
        char t1[80], t2[80];
        snprintf(t1, sizeof t1, "RE1.5 / %s / Pistole", more[i].name);
        snprintf(t2, sizeof t2, "RE2   / %s / Pistole", more[i].name);
        run(t1, RE15_AI_FLAVOR_RE15, 3, 60);
        run(t2, RE15_AI_FLAVOR_RE2,  3, 60);
    }
    return 0;
}
