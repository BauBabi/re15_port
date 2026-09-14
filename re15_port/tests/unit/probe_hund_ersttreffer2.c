/* probe_hund_ersttreffer2.c — MESSUNG II zum Nutzer-Befund 2026-09-14 (Hund 0x20).
 *
 * Drei getrennte Messungen, damit Waffentakt, Geometrie und Gegner-Gate NICHT
 * miteinander vermischt werden:
 *
 *  A) GATE-TAKT (Resolver direkt, JEDES Bild): Spieler steht fest vor dem Gegner,
 *     die KI laeuft, und re15_player_weapon_fire() wird jedes Bild gerufen.
 *     Damit faellt der Waffen-/Rueckstoss-Takt weg; gemessen wird NUR, wie viele
 *     Bilder der Gegner nach einem Treffer NICHT treffbar ist (Befund 2) und ob
 *     der ERSTE Treffer verzoegert ist (Befund 1).
 *
 *  B) ECHTER PAD-WEG mit ARENA-Halterung: game_step mit R1+SQUARE; der Spieler wird
 *     jedes Bild auf feste Distanz/Blickrichtung zum Gegner gesetzt, damit der
 *     Schuss-Streifen nie das Ergebnis bestimmt.
 *
 *  C) ELEVATION: derselbe Pad-Weg, aber mit echtem D-Pad UNTEN/OBEN gehalten
 *     (nicht ueber den Test-Hook), weil der Hoehen-Band-Filter im Port an
 *     re15_player_aim_elevation() haengt.
 *
 * Reine Messsonde. Kein add_test, kein Engine-Eingriff.
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
extern void re15_player_set_aim_clip_lens(const uint16_t *fcs, int n);
extern int  re15_player_aim_elevation(void);
extern int  re15_player_aim_clip(void);
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

static int load_real_gun_cliplens(void)
{
    char p[600]; snprintf(p, sizeof p, "%s/PLD/PL00W03.PLW", s_base);
    size_t sz = 0; uint8_t *plw = slurp(p, &sz);
    uint32_t diroff, de[4]; int k, i, n;
    re15_emd_animation_t an;
    if (!plw || sz < 16) return 0;
    diroff = (uint32_t)(plw[0] | (plw[1]<<8) | (plw[2]<<16) | ((uint32_t)plw[3]<<24));
    if (diroff + 16 > (uint32_t)sz) { free(plw); return 0; }
    for (k = 0; k < 4; k++)
        de[k] = (uint32_t)(plw[diroff+4*k] | (plw[diroff+4*k+1]<<8) |
                           (plw[diroff+4*k+2]<<16) | ((uint32_t)plw[diroff+4*k+3]<<24));
    memset(&an, 0, sizeof an);
    if (!(de[0] < de[1] && de[1] <= (uint32_t)sz) ||
        re15_emd_parse_animation(plw + de[0], (int)(de[1] - de[0]), &an) != 0) { free(plw); return 0; }
    {
        uint16_t fcs[16]; n = (an.clip_count < 16) ? an.clip_count : 16;
        for (i = 0; i < n; i++) fcs[i] = (uint16_t)an.clips[i].frame_count;
        re15_player_set_aim_clip_lens(fcs, n);
    }
    free(plw);
    return 1;
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
    re15_actor_t *pl; int i;
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

static int first_enemy(unsigned want_type)
{
    int s;
    for (s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == (uint8_t)want_type) return s;
    return -1;
}

/* Spieler auf feste Distanz VOR den Gegner haengen (Blick auf ihn). */
static void hold_arena(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    /* Blickrichtung wie der Auto-Track sie stellt: bearing = atan2(dz,dx) - 0x400
     * (player_common.c:823, FUN_8001a8f8-Zwilling). */
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
    pl->hp = 100;
}

static int setup(int flavor, unsigned type, int weapon, int laufend, int ai_frames, int *slot_out)
{
    int f, s, slot;
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    for (f = 0; f < ai_frames; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    slot = first_enemy(type);
    if (slot < 0) return 0;
    for (s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (laufend) {
        re15_ai_set_state_word(&g_actors[slot], 0x201);
        g_actors[slot].sub_state_2 = 0; g_actors[slot].sub_state_3 = 0;
    }
    *slot_out = slot;
    return 1;
}

/* ============================ A) GATE-TAKT ============================================= */
static void mess_A(const char *tag, int flavor, unsigned type, int weapon, int laufend)
{
    int slot, f, last = -1, n = 0, nlog = 0;
    int gaps[24];
    int mot_ref = -1, mot_watch = -1;
    re15_actor_t *e;
    printf("\n--- A) GATE-TAKT %s (flavor=%s, typ=0x%02X, w=%d, %s) ---\n", tag,
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", type, weapon,
           laufend ? "LAUF" : "Spawnzustand");
    if (!setup(flavor, type, weapon, laufend, 60, &slot)) { printf("  kein Gegner\n"); return; }
    e = &g_actors[slot];
    printf("  Ziel st=%d s1=%d hp=%d grid=0x%02X r=%d\n", e->state, e->sub_state_1, e->hp,
           e->grid_id, (int)e->hit_radius_min);
    for (f = 0; f < 400; f++) {
        int hpb, ret;
        hold_arena(slot, 2600);
        hpb = e->hp;
        ret = re15_player_weapon_fire(weapon);
        if (e->hp < hpb) {
            int dt = (last < 0) ? f : (f - last);
            if (n < 24) gaps[n] = dt;
            n++;
            if (nlog++ < 12)
                printf("   TREFFER in f%03d (Abstand %3d) hp %d->%d  st=%d/%d/%d/%d hr=0x%02X\n",
                       f, dt, hpb, e->hp, e->state, e->sub_state_1, e->sub_state_2,
                       e->sub_state_3, e->hit_react);
            last = f;
            if (n == 1) { mot_ref = (int)e->motion; mot_watch = 0; }
        }
        (void)ret;
        if (e->hp < 0) { printf("   (tot in f%03d)\n", f); break; }
        if (mot_watch >= 0) {
            mot_watch++;
            if ((int)e->motion != mot_ref) {
                printf("      -> Reaktions-Clip %d -> %d, %d Bild(er) nach dem Treffer\n",

                       mot_ref, (int)e->motion, mot_watch);
                mot_watch = -1;
            } else if (mot_watch > 40) { printf("      -> KEIN Clipwechsel in 40 Bildern\n"); mot_watch = -1; }

        }
        re15_enemy_ai_run_all(1);
    }
    printf("  A-ERGEBNIS %s: %d Treffer; T1=%d; Abstaende:", tag, n, n ? gaps[0] : -1);
    { int i; for (i = 1; i < n && i < 24; i++) printf(" %d", gaps[i]); }
    printf("\n");
}

/* ============================ B/C) PAD-WEG mit ARENA ==================================== */
static void mess_B(const char *tag, int flavor, unsigned type, int weapon, int laufend,
                   uint16_t elev_bit)
{
    int slot, f, last = -1, n = 0, nlog = 0, raise = 0;
    int gaps[24];
    re15_actor_t *e, *pl;
    uint16_t pad;
    printf("\n--- B) PAD-WEG %s (flavor=%s, typ=0x%02X, w=%d, %s, elev-Pad=%s) ---\n", tag,
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", type, weapon,
           laufend ? "LAUF" : "Spawnzustand",
           elev_bit == RE15_PAD_BIT_DOWN ? "UNTEN" : elev_bit == RE15_PAD_BIT_UP ? "OBEN" : "-");
    if (!setup(flavor, type, weapon, laufend, 60, &slot)) { printf("  kein Gegner\n"); return; }
    e = &g_actors[slot]; pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    hold_arena(slot, 2600);
    re15_player_cmd_reset(); re15_player_aim_reset(); load_real_gun_cliplens();
    pad = (uint16_t)(RE15_PAD_BIT_R1 | elev_bit);
    for (f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        hold_arena(slot, 2600); frame(pad, 0); raise++;
        re15_enemy_ai_run_all(0);
    }
    printf("  R1 -> aim_ready nach %d Bildern; elev=%+d clip=%d\n",
           raise, re15_player_aim_elevation(), re15_player_aim_clip());
    for (f = 0; f < 400; f++) {
        int hpb;
        hold_arena(slot, 2600);
        hpb = e->hp;
        frame((uint16_t)(pad | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hpb) {
            int dt = (last < 0) ? f : (f - last);
            if (n < 24) gaps[n] = dt;
            n++;
            if (nlog++ < 12)
                printf("   TREFFER in f%03d (Abstand %3d) hp %d->%d st=%d/%d/%d/%d hr=0x%02X elev=%+d\n",
                       f, dt, hpb, e->hp, e->state, e->sub_state_1, e->sub_state_2,
                       e->sub_state_3, e->hit_react, re15_player_aim_elevation());
            last = f;
        }
        if (e->hp < 0) { printf("   (tot in f%03d)\n", f); break; }
    }
    printf("  B-ERGEBNIS %s: %d Treffer; T1=%d; Abstaende:", tag, n, n ? gaps[0] : -1);
    { int i; for (i = 1; i < n && i < 24; i++) printf(" %d", gaps[i]); }
    printf("  (elev am Ende %+d, aim_ready=%d, pl.state=%d)\n",
           re15_player_aim_elevation(), re15_player_aim_ready(), pl->state);
}


/* ============================ D) REALISTISCH (keine Arena-Halterung) ==================== */
/* Spieler steht fest, Gegner laeuft frei. Gezaehlt wird JEDER Abzug (Uebergang
 * aim_ready 1 -> Entladung) und ob er getroffen hat; bei Fehlschuss wird das Gate
 * protokolliert, an dem der Kandidat ausschied. */
static long long dwcross(int32_t ax, int32_t az, int32_t bx, int32_t bz)
{ return (long long)ax*bz - (long long)az*bx; }
static int dwtri(int32_t VAx,int32_t VAz,int32_t VBx,int32_t VBz,int32_t V5x,int32_t V5z,
                 int32_t rx,int32_t rz)
{
    if (dwcross(V5x,V5z,rx-VAx,rz-VAz) > 0) return 0;
    if (dwcross(V5x,V5z,rx-VBx,rz-VBz) < 0) return 0;
    if (dwcross(VAx-VBx,VAz-VBz,rx-V5x,rz-V5z) > 0) return 0;
    return 1;
}
static int dwedge(const re15_actor_t *pl, int32_t ex, int32_t ez, int32_t reach, int32_t radius)
{
    int32_t c = re15_cos_q12(pl->rot_y), s = re15_sin_q12(pl->rot_y);
    int32_t Vx[5], Vz[5]; int i;
    static const int32_t lx[5] = { 50, 50, 650, 650, 650 };
    int32_t lz[5]; lz[0]=radius+200; lz[1]=-(radius+200); lz[2]=radius+reach; lz[3]=-(radius+reach); lz[4]=0;
    for (i = 0; i < 5; i++) {
        Vx[i] = (int32_t)(( (long long)c*lx[i] + (long long)s*lz[i]) >> 12);
        Vz[i] = (int32_t)((-(long long)s*lx[i] + (long long)c*lz[i]) >> 12);
    }
    return dwtri(Vx[0],Vz[0],Vx[1],Vz[1],Vx[4],Vz[4],ex-pl->x,ez-pl->z) ||
           dwtri(Vx[2],Vz[2],Vx[3],Vz[3],Vx[4],Vz[4],ex-pl->x,ez-pl->z);
}
static int dgate(int slot)   /* 0 = Kandidat, sonst Gate-Code */
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int elev = re15_player_aim_elevation();
    uint32_t pband = (elev > 0) ? 0x80000000u : (elev < 0) ? 0x20000000u : 0x40000000u;
    uint32_t eband;
    int32_t dx, dz; double dist;
    if (e->hit_radius_min <= 0)  return 1;
    if (e->state == 7)           return 2;
    if ((e->hit_react & 3) == 3) return 3;
    dx = e->x - pl->x; dz = e->z - pl->z;
    dist = sqrt((double)dx*dx + (double)dz*dz);
    if (e->type == 0x20 && !re15_ai_re2_for_type(0x20))
        eband = ((e->aim_band & 2) ? 0x40000000u : 0u) | ((e->aim_band & 1) ? 0x20000000u : 0u);
    else if (e->grid_id & 0x80) eband = (dist < 5000.0) ? 0x20000000u : 0u;
    else                        eband = 0x40000000u;
    if ((pband & eband) == 0) return 4;                  /* Hoehen-Band */
    if (!dwedge(pl, e->x, e->z, 1000, (int32_t)e->hit_radius_min)) return 5;  /* Streifen */
    if (e->hit_react & 1) return 6;                      /* Ein-Treffer-Riegel */
    return 0;
}
static void mess_D(const char *tag, int flavor, unsigned type, int weapon, uint16_t elev_bit)
{
    int slot, f, raise = 0, abzug = 0, treffer = 0, erst_abzug = -1, i;
    int why[8]; 
    re15_actor_t *e, *pl;
    uint16_t pad;
    for (i = 0; i < 8; i++) why[i] = 0;
    printf("\n--- D) REALISTISCH %s (flavor=%s, typ=0x%02X, w=%d, elev-Pad=%s) ---\n", tag,
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", type, weapon,
           elev_bit == RE15_PAD_BIT_DOWN ? "UNTEN" : elev_bit == RE15_PAD_BIT_UP ? "OBEN" : "-");
    if (!setup(flavor, type, weapon, 1, 60, &slot)) { printf("  kein Gegner\n"); return; }
    e = &g_actors[slot]; pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->x = e->x - 3000; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0xfff);
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); load_real_gun_cliplens();
    pad = (uint16_t)(RE15_PAD_BIT_R1 | elev_bit);
    for (f = 0; f < 60 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(pad, 0); raise++; }
    for (f = 0; f < 600; f++) {
        int rdy = re15_player_aim_ready();
        int hpb = e->hp;
        int g = dgate(slot);
        pl->hp = 100;
        frame((uint16_t)(pad | RE15_PAD_BIT_SQUARE),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (rdy) {                              /* aim_ready + SQUARE gehalten = Entladung */
            abzug++;
            if (e->hp < hpb) { treffer++; if (erst_abzug < 0) erst_abzug = abzug; }
            else if (g >= 0 && g < 8) why[g]++;
        }
        if (e->hp < 0) break;
    }
    printf("  D-ERGEBNIS %s: %d Abzuege, %d Treffer, erster Treffer beim %d. Abzug\n",
           tag, abzug, treffer, erst_abzug);
    printf("     Fehlschuss-Gruende: keineHitbox=%d Leiche=%d hr3=%d BAND=%d STREIFEN=%d Riegel=%d (Kandidat-aber-daneben=%d)\n",
           why[1], why[2], why[3], why[4], why[5], why[6], why[0]);
}

static int load_room(const char *sub, int room_id, int fsub)
{
    char path[600]; size_t sz = 0; uint8_t *b;
    snprintf(path, sizeof path, "%s/%s", s_base, sub);
    b = slurp(path, &sz);
    if (!b) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(b, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", sub); return 0; }
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1;
    s_room_id = room_id; s_fire_sub = fsub;
    return 1;
}

int main(int argc, char **argv)
{
    const char *base = getenv("RE15_ASSET_DIR");
    (void)argc; (void)argv;
    if (base && *base) s_base = base;
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    if (!load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) return 1;
    mess_A("HUND-RE2-Lauf",   RE15_AI_FLAVOR_RE2,  0x20, 3, 1);
    mess_A("HUND-RE2-Park",   RE15_AI_FLAVOR_RE2,  0x20, 3, 0);
    mess_A("HUND-RE15-Lauf",  RE15_AI_FLAVOR_RE15, 0x20, 3, 1);
    mess_A("HUND-RE15-Park",  RE15_AI_FLAVOR_RE15, 0x20, 3, 0);
    mess_B("HUND-RE2-Lauf",   RE15_AI_FLAVOR_RE2,  0x20, 3, 1, 0);
    mess_B("HUND-RE2-LaufTIEF",RE15_AI_FLAVOR_RE2, 0x20, 3, 1, RE15_PAD_BIT_DOWN);
    mess_B("HUND-RE15-Lauf",  RE15_AI_FLAVOR_RE15, 0x20, 3, 1, 0);
    mess_B("HUND-RE15-LaufTIEF",RE15_AI_FLAVOR_RE15,0x20,3, 1, RE15_PAD_BIT_DOWN);

    mess_D("HUND-RE2-eben",  RE15_AI_FLAVOR_RE2,  0x20, 3, 0);
    mess_D("HUND-RE2-tief",  RE15_AI_FLAVOR_RE2,  0x20, 3, RE15_PAD_BIT_DOWN);
    mess_D("HUND-RE2-hoch",  RE15_AI_FLAVOR_RE2,  0x20, 3, RE15_PAD_BIT_UP);
    mess_D("HUND-RE15-eben", RE15_AI_FLAVOR_RE15, 0x20, 3, 0);
    mess_D("HUND-RE15-tief", RE15_AI_FLAVOR_RE15, 0x20, 3, RE15_PAD_BIT_DOWN);

    if (!load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) return 1;
    mess_A("ZOMBIE-RE2-Lauf",  RE15_AI_FLAVOR_RE2,  0x10, 3, 1);
    mess_A("ZOMBIE-RE15-Lauf", RE15_AI_FLAVOR_RE15, 0x10, 3, 1);
    mess_B("ZOMBIE-RE2-Lauf",  RE15_AI_FLAVOR_RE2,  0x10, 3, 1, 0);
    mess_B("ZOMBIE-RE2-LaufTIEF", RE15_AI_FLAVOR_RE2, 0x10, 3, 1, RE15_PAD_BIT_DOWN);
    mess_D("ZOMBIE-RE2-eben", RE15_AI_FLAVOR_RE2, 0x10, 3, 0);
    mess_D("ZOMBIE-RE2-tief", RE15_AI_FLAVOR_RE2, 0x10, 3, RE15_PAD_BIT_DOWN);
    mess_D("ZOMBIE-RE2-hoch", RE15_AI_FLAVOR_RE2, 0x10, 3, RE15_PAD_BIT_UP);
    return 0;
}
