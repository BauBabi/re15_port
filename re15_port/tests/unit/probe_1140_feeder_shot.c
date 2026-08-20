/* probe_1140_feeder_shot.c — MESSUNG zum Nutzer-Report v0.3.5:
 * "Im Original kann man im Dining Room gerade schiessen und trifft den am Boden fressenden
 *  Zombie. Hier nicht."
 *
 * Diese Sonde faehrt den ECHTEN Weg (re15_game_step + Pad R1 halten, KEIN D-Pad -> gerader
 * Schuss, dann SQUARE) in ROOM1140 auf JEDEN aktiven Aktor einzeln und protokolliert fuer
 * jeden Kandidaten die komplette Gate-Kette aus re15_player_weapon_fire.
 *
 * Anders als probe_re2_hitpath.c wird hier NICHTS ausgeschlossen: gerade die Aktoren mit
 * grid_id & 0x80 (Liege-/Fress-Spawns) sind der Messgegenstand.
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
extern int  re15_sin_q12(int), re15_cos_q12(int);

static re15_rdt_t         s_rdt;
static int                s_room_id = 0x1140;
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

/* Spiegel von re15_gun_wedge_inside (re15_damage.c) — nur Diagnose */
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

static const char *gate_of(const re15_actor_t *pl, re15_actor_t *e, int weapon_id, double dist)
{
    int elev = re15_player_aim_elevation();
    uint32_t pband = (elev > 0) ? 0x80000000u : (elev < 0) ? 0x20000000u : 0x40000000u;
    uint32_t eband;
    if (e->type == 0x21) {
        eband = (e->aim_band == 4) ? 0x80000000u : (e->aim_band == 1) ? 0x20000000u :
                (e->aim_band == 2) ? 0x40000000u : 0u;
    } else if (e->type == 0x10 || e->type == 0x11 || e->type == 0x12 ||
               e->type == 0x13 || e->type == 0x16 || e->type == 0x18) {
        /* Spiegel von re15_band_stamp_aa4 (FUN_80012aa4 @0x80012aa4, R = 0xbb8 @0x80101604) */
        int32_t s0   = (pl->y - e->y) / 1800;
        int32_t ring = (s0 < 0 ? -s0 : s0) * 1000;
        eband = 0;
        if ((uint32_t)s0 < 2u) eband |= 0x40000000u;
        if (dist < (double)(3000 + ring) && (double)ring < dist)
            eband |= (s0 > 0) ? 0x80000000u : 0x20000000u;
        if (e->grid_id & 0x80) {                        /* @0x80101614-3c */
            eband &= ~0x40000000u;
            if (dist < 5000.0) eband |= 0x20000000u;    /* 0x80012974(0x1388) */
        }
    } else if (e->grid_id & 0x80) eband = (dist < 5000.0) ? 0x20000000u : 0u;
    else                          eband = 0x40000000u;
    static const uint16_t reach_tab[22] = {1000,1100,1000,1000,1100,1000,1200,1000,1500,1000,
                                           1000,1000,1300,1800,1000,1000,1000,1000,1000,1100,
                                           1000,1000};
    int is_strip = (weapon_id==0 || (weapon_id>=3 && weapon_id<=8) || weapon_id==12 ||
                    weapon_id==13 || weapon_id==19 || weapon_id==21);
    int32_t reach = reach_tab[weapon_id];
    if (e->hit_radius_min <= 0)       return "RAUS: keine Hitbox";
    if (e->state == 7)                return "RAUS: state==7 CORPSE";
    if ((e->hit_react & 3) == 3)      return "RAUS: hit_react&3==3";
    if ((pband & eband) == 0)         return "RAUS: ELEVATIONS-BAND";
    if (e->hit_react & 1)             return "RAUS: bit0-LATCH (Resolver rekursiert weiter)";
    if (is_strip) {
        if (!wedge_in(pl, e->x, e->z, reach, (int32_t)e->hit_radius_min)) return "RAUS: Schuss-Streifen";
    } else {
        uint32_t R = (uint32_t)reach + (uint32_t)e->hit_radius_min;
        if (dist >= (double)R) return "RAUS: ausser Reichweite";
        if (re15_ai_arc_test(pl, e->x, e->z, 0x400) != 0) return "RAUS: Nahkampf-Kegel";
    }
    return "KANDIDAT";
}

static void dump_all(const char *tag, int weapon_id)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    int elev = re15_player_aim_elevation();
    printf("  [%s] Spieler(%d,%d,%d) rot=%d elev=%d aim_ready=%d\n", tag,
           (int)pl->x, (int)pl->y, (int)pl->z, (int)pl->rot_y, elev, re15_player_aim_ready());
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        double q = (double)dx*dx + (double)dz*dz, dist = q > 0 ? __builtin_sqrt(q) : 0;
        printf("      slot%02d typ=0x%02X st=%d/%d/%d hp=%4d grid=0x%02X mo=0x%02X r=%d "
               "y=%6d dist=%6.0f react=0x%02X 1d3=0x%02X f10e=0x%04X  %s\n",
               s, e->type, e->state, e->sub_state_1, e->sub_state_2, e->hp, e->grid_id,
               e->motion, (int)e->hit_radius_min, (int)e->y, dist, e->hit_react,
               e->re2z_self1d3, (unsigned)e->re2z_f10e,
               gate_of(pl, e, weapon_id, dist));
    }
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
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
}

/* Spieler auf `back` Einheiten vor das Ziel stellen, Blick zum Ziel (Tank-Ausrichtung). */
static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Ein gerader Schuss (kein D-Pad) auf EIN Ziel. Rueckgabe: Treffer ja/nein. */
static int straight_shot(int slot, int weapon, int32_t back, int frames)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(slot, back);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; face(slot, back); frame(RE15_PAD_BIT_R1, 0);
    }
    dump_all("aim bereit (KEIN D-Pad = gerader Schuss)", weapon);
    int hp0 = e->hp, hp_last = e->hp, hits = 0;
    for (int f = 0; f < frames; f++) {
        pl->hp = 100; face(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) {
            hits++;
            if (hits <= 3)
                printf("   TREFFER #%d Frame %d: hp %d->%d st=%d/%d/%d grid=0x%02X mo=0x%02X\n",
                       hits, f, hp_last, e->hp, e->state, e->sub_state_1, e->sub_state_2,
                       e->grid_id, e->motion);
        }
        hp_last = e->hp;
        if (e->hp < 0) break;
    }
    printf("   ERGEBNIS slot%02d: %d Treffer, hp %d -> %d, st=%d/%d/%d, grid=0x%02X, mo=0x%02X\n",
           slot, hits, hp0, e->hp, e->state, e->sub_state_1, e->sub_state_2, e->grid_id, e->motion);
    return hits;
}

static void run(int flavor, int weapon, int32_t back, int ai_frames)
{
    printf("\n========== ROOM1140, Flavor %s, Waffe %d, Abstand %d, %d AI-Frames ==========\n",
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", weapon, (int)back, ai_frames);
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    printf("  --- ROH-Deskriptoren direkt nach dem SCD-Spawn (noch KEIN AI-Tick) ---\n");
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        printf("      slot%02d typ=0x%02X Sce_em_set-Deskriptor(+0x9)=0x%02X mo=0x%02X pos(%d,%d,%d)\n",
               s, e->type, e->grid_id, e->motion, (int)e->x, (int)e->y, (int)e->z);
    }
    /* Spieler WEIT weg parken, damit die Fresser nicht sofort aufwachen (Wake-Gate dist<4000) */
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000;
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
    for (int f = 0; f < ai_frames; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
        frame(0, 0);
    }
    printf("  --- Spawn-Zustand nach %d AI-Frames (Spieler weit weg) ---\n", ai_frames);
    dump_all("Spawn", weapon);

    /* Jeden Aktor EINZELN beschiessen (Arena: nur er aktiv), Zustand jedesmal frisch. */
    int slots[RE15_ACTOR_MAX], n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (g_actors[s].active) slots[n++] = s;
    for (int i = 0; i < n; i++) {
        re15_ai_flavor_set((re15_ai_flavor_t)flavor);
        bringup();
        re15_inv_load_briefing();
        re15_player_set_equipped_weapon(weapon);
        { int es = re15_inv_equipped_slot();
          if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
        for (int f = 0; f < ai_frames; f++) {
            g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
            g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
            frame(0, 0);
        }
        int slot = slots[i];
        if (!g_actors[slot].active) { printf("\n  -- slot%02d inaktiv --\n", slot); continue; }
        printf("\n  -- ZIEL slot%02d typ=0x%02X grid=0x%02X mo=0x%02X st=%d/%d/%d --\n",
               slot, g_actors[slot].type, g_actors[slot].grid_id, g_actors[slot].motion,
               g_actors[slot].state, g_actors[slot].sub_state_1, g_actors[slot].sub_state_2);
        for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
        straight_shot(slot, weapon, back, 240);
    }
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

    run(RE15_AI_FLAVOR_RE15, 3, 2600, 60);
    run(RE15_AI_FLAVOR_RE2,  3, 2600, 60);
    /* Abstand 6000: ausserhalb des RE1.5-Fress-Weckers (dist < 0xfa0 = 4000) -> die Fresser
     * bleiben in der Fress-Pose, der Schuss-Streifen ist unbegrenzt. GENAU der Nutzer-Fall. */
    run(RE15_AI_FLAVOR_RE15, 3, 6000, 60);
    run(RE15_AI_FLAVOR_RE2,  3, 6000, 60);
    return 0;
}
