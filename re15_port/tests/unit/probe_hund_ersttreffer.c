/* probe_hund_ersttreffer.c — MESSUNG zum Nutzer-Befund 2026-09-14:
 *   (1) "der erste Treffer bei den Zombie Hunden ist irgendwie delayed"
 *   (2) "man kann die Zombie Hunde erst wieder treffen, sobald sie nach dem schiessen
 *        wieder komplett stehen"
 *
 * Reine Messsonde (KEIN add_test, KEIN Engine-Eingriff). Sie faehrt den ECHTEN Weg:
 * game_step() mit Pad (R1 halten -> SQUARE halten), echte Waffen-Bank-Cliplaengen aus
 * PLD/PL00W03.PLW (statt des 12-Frame-Mocks), und protokolliert BILD FUER BILD
 *   - Aim-Phase / aim_ready / Rueckstoss-Clip
 *   - je Gegner: state/sub/hit_react/hp/Position/Distanz
 *   - das GATE, an dem der Kandidat ausscheidet (Schatten-Kopie aus re15_player_weapon_fire)
 * und misst:
 *   T1 = Bilder vom ersten SQUARE-Bild bis zur ersten hp-Aenderung
 *   Tn = Bilder zwischen den folgenden hp-Aenderungen
 * Gegenprobe ZOMBIE (ROOM1140) unter identischen Bedingungen.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern int  re15_player_aim_active(void);
extern int  re15_player_aim_ready(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_elevation(void);
extern int  re15_player_aim_clip(void);
extern void re15_player_set_aim_elevation_for_test(int elev);
extern int  re15_sin_q12(int);
extern int  re15_cos_q12(int);

static re15_rdt_t         s_rdt;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static const char        *s_base = RE15_ASSET_PSX_DIR;

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

/* ---- echte Waffen-Bank: PLD/PL00W03.PLW dir[0] = EDD (Clips) --------------------------- */
static int load_real_gun_cliplens(void)
{
    char p[600]; snprintf(p, sizeof p, "%s/PLD/PL00W03.PLW", s_base);
    size_t sz = 0; uint8_t *plw = slurp(p, &sz);
    if (!plw || sz < 16) { printf("WARN: %s nicht lesbar\n", p); return 0; }
    uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
    if (diroff + 16 > (uint32_t)sz) { free(plw); return 0; }
    uint32_t de[4];
    int k;
    for (k = 0; k < 4; k++)
        de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                           (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
    re15_emd_animation_t an; memset(&an, 0, sizeof an);
    if (!(de[0] < de[1] && de[1] <= (uint32_t)sz) ||
        re15_emd_parse_animation(plw + de[0], (int)(de[1] - de[0]), &an) != 0) {
        printf("WARN: PL00W03 EDD-Parse fehlgeschlagen\n"); free(plw); return 0;
    }
    uint16_t fcs[16]; int n = (an.clip_count < 16) ? an.clip_count : 16;
    int i;
    for (i = 0; i < n; i++) fcs[i] = (uint16_t)an.clips[i].frame_count;
    re15_player_set_aim_clip_lens(fcs, n);
    return 1;
}

static void print_gun_bank(void)
{
    char p[600]; snprintf(p, sizeof p, "%s/PLD/PL00W03.PLW", s_base);
    size_t sz = 0; uint8_t *plw = slurp(p, &sz);
    if (!plw || sz < 16) return;
    uint32_t diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
    uint32_t de[4]; int k;
    for (k = 0; k < 4; k++)
        de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                           (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
    re15_emd_animation_t an; memset(&an, 0, sizeof an);
    if (re15_emd_parse_animation(plw + de[0], (int)(de[1] - de[0]), &an) != 0) return;
    printf("PL00W03.PLW: %d Clips:", an.clip_count);
    int i;
    for (i = 0; i < an.clip_count && i < 16; i++)
        printf(" [%d]=%d", i, an.clips[i].frame_count);
    printf("\n");
    free(plw);
}

/* ---- Schatten-Kopie der GATE-Kette (nur LESEN, Stand re15_damage.c 2026-09-14) --------- */
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
    int i;
    for (i = 0; i < 5; i++) {
        Vx[i] = (int32_t)(( (long long)c*lx[i] + (long long)s*lz[i]) >> 12);
        Vz[i] = (int32_t)((-(long long)s*lx[i] + (long long)c*lz[i]) >> 12);
    }
    int32_t rx = ex - pl->x, rz = ez - pl->z;
    return wtri(Vx[0],Vz[0],Vx[1],Vz[1],Vx[4],Vz[4],rx,rz) ||
           wtri(Vx[2],Vz[2],Vx[3],Vz[3],Vx[4],Vz[4],rx,rz);
}
static const char *gate_verdict(int slot, int weapon_id)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int elev = re15_player_aim_elevation();
    uint32_t pband = (elev > 0) ? 0x80000000u : (elev < 0) ? 0x20000000u : 0x40000000u;
    uint32_t eband;
    int32_t dx, dz; double dist;
    static const uint16_t reach_tab[22] = {1000,1100,1000,1000,1100,1000,1200,1000,1500,1000,
                                           1000,1000,1300,1800,1000,1000,1000,1000,1000,1100,
                                           1000,1000};
    if (e->hit_radius_min <= 0)  return "RAUS: keine Hitbox";
    if (e->state == 7)           return "RAUS: state 7 CORPSE";
    if ((e->hit_react & 3) == 3) return "RAUS: hit_react&3==3";
    dx = e->x - pl->x; dz = e->z - pl->z;
    dist = sqrt((double)dx*dx + (double)dz*dz);
    if (e->type == 0x20 && !re15_ai_re2_for_type(0x20)) {
        eband = ((e->aim_band & 2) ? 0x40000000u : 0u) | ((e->aim_band & 1) ? 0x20000000u : 0u);
    } else if (e->grid_id & 0x80) {
        eband = (dist < 5000.0) ? 0x20000000u : 0u;
    } else {
        eband = 0x40000000u;
    }
    if ((pband & eband) == 0) return "RAUS: Elevations-Band";
    if (!wedge_in(pl, e->x, e->z, reach_tab[weapon_id], (int32_t)e->hit_radius_min))
        return "RAUS: Schuss-Streifen";
    if (e->hit_react & 1) return "RAUS(effektiv): +0x93 Bit0 Ein-Treffer-Riegel";
    return "KANDIDAT";
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
    re15_actor_t *pl;
    int i;
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
    for (i = 0; i < 120; i++) scd_vm_tick();
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    load_real_gun_cliplens();
}

static int first_enemy(uint8_t want_type)
{
    int s;
    for (s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == want_type) return s;
    return -1;
}

/* laufend: 1 = Ziel in den LAUF-Substate zwingen (state 1 / sub 2) */
static void run(const char *tag, int flavor, unsigned type, int weapon, int elev,
                int laufend, int ai_frames, int32_t fire_back)
{
    re15_actor_t *e, *pl;
    int slot, s, f, raise;
    int hp_last, treffer, last_hit_f;
    int T[16]; int nT = 0;
    int i;

    printf("\n===== %s | flavor=%s | typ=0x%02X | Waffe %d | elev=%+d | Ziel=%s =====\n",
           tag, flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", (unsigned)type, weapon, elev,
           laufend ? "LAEUFT" : "steht/Skript");
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);

    for (f = 0; f < ai_frames; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    slot = first_enemy((uint8_t)type);
    if (slot < 0) { printf("  KEIN GEGNER vom Typ 0x%02X\n", (unsigned)type); return; }
    for (s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    e  = &g_actors[slot];
    pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    if (laufend) {
        re15_ai_set_state_word(e, 0x201);       /* state 1 / sub 2 = RUN/WALK */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
    }
    pl->x = e->x - fire_back; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024;
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset();
    load_real_gun_cliplens();

    printf("  Ziel slot=%d typ=0x%02X grid=0x%02X st=%d s1=%d hp=%d r=%d band=%d pos(%d,%d,%d) dist=%d\n",
           slot, e->type, e->grid_id, e->state, e->sub_state_1, e->hp,
           (int)e->hit_radius_min, (int)e->aim_band, (int)e->x, (int)e->y, (int)e->z,
           (int)fire_back);

    raise = 0;
    for (f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); raise++;
    }
    printf("  R1 -> aim_ready nach %d Bildern (aktueller Clip %d)\n", raise, re15_player_aim_clip());

    if (elev) re15_player_set_aim_elevation_for_test(elev);
    hp_last = e->hp; treffer = 0; last_hit_f = -1;
    for (f = 0; f < 260; f++) {
        const char *g = gate_verdict(slot, weapon);
        int hpb = e->hp;
        int rdy = re15_player_aim_ready();
        int st = e->state, s1 = e->sub_state_1, s2 = e->sub_state_2, s3 = e->sub_state_3;
        unsigned hr = e->hit_react;
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        int dist = (int)sqrt((double)dx*dx + (double)dz*dz);
        int traf;
        if (elev) re15_player_set_aim_elevation_for_test(elev);
        pl->hp = 100;
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        traf = (e->hp < hpb);
        if (traf) {
            int dt = (last_hit_f < 0) ? f : (f - last_hit_f);
            treffer++;
            if (nT < 16) T[nT++] = dt;
            last_hit_f = f;
        }
        if (f < 80 || traf || (f % 20) == 0)
            printf("   f%03d rdy=%d clip=%2d | Ziel st=%d/%d/%d/%d hr=0x%02X hp=%4d d=%4d | %-44s%s\n",
                   f, rdy, re15_player_aim_clip(), st, s1, s2, s3, hr, hpb, dist, g,
                   traf ? "  <== TREFFER" : "");
        hp_last = e->hp;
        if (e->hp < 0) { printf("   (tot in f%03d)\n", f); break; }
    }
    (void)hp_last;
    printf("  ERGEBNIS: %d Treffer.", treffer);
    if (nT > 0) {
        printf("  T1 (SQUARE -> 1. hp-Aenderung) = %d Bilder", T[0]);
        printf(" | Folgeabstaende:");
        for (i = 1; i < nT; i++) printf(" %d", T[i]);
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    size_t sz = 0; uint8_t *buf;
    (void)argc; (void)argv;
    if (base && *base) s_base = base;

    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    print_gun_bank();

    snprintf(path, sizeof path, "%s/STAGE1/ROOM1190.RDT", s_base);
    buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse 1190\n"); return 1; }
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1;
    s_room_id = 0x1190; s_fire_sub = 13;

    run("HUND RE2 Skriptpark", RE15_AI_FLAVOR_RE2,  0x20, 3,  0, 0, 60, 2600);
    run("HUND RE2 laeuft",     RE15_AI_FLAVOR_RE2,  0x20, 3,  0, 1, 60, 2600);
    run("HUND RE2 laeuft TIEF",RE15_AI_FLAVOR_RE2,  0x20, 3, -1, 1, 60, 2600);
    run("HUND RE15 Skriptpark",RE15_AI_FLAVOR_RE15, 0x20, 3,  0, 0, 60, 2600);
    run("HUND RE15 laeuft",    RE15_AI_FLAVOR_RE15, 0x20, 3,  0, 1, 60, 2600);
    run("HUND RE15 laeuft TIEF",RE15_AI_FLAVOR_RE15,0x20, 3, -1, 1, 60, 2600);

    {
        size_t s2 = 0; uint8_t *b2;
        snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", s_base);
        b2 = slurp(path, &s2);
        if (!b2) { printf("RDT fehlt (1140)\n"); return 1; }
        if (re15_rdt_parse(b2, s2, &s_rdt) != 0) { printf("RDT-Parse 1140\n"); return 1; }
        s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1;
        s_room_id = 0x1140; s_fire_sub = -1;

        run("ZOMBIE RE2 steht",  RE15_AI_FLAVOR_RE2,  0x10, 3, 0, 0, 60, 2600);
        run("ZOMBIE RE2 laeuft", RE15_AI_FLAVOR_RE2,  0x10, 3, 0, 1, 60, 2600);
        run("ZOMBIE RE15 steht", RE15_AI_FLAVOR_RE15, 0x10, 3, 0, 0, 60, 2600);
    }
    return 0;
}
