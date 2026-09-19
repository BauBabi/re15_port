/**
 * @file probe_r16_sk_trefferhoehe.c
 * @brief SKEPTIKER-GEGENSONDE (Runde 16, 2026-09-19, Thema trefferhoehe) — kein ctest.
 *
 * Prueft die Sub-Box-GRENZEN des Dossiers analysis/befunde_2026-09-19/trefferhoehe.md:
 * Das Dossier (§2.2/§2.4/§4) rechnet die RE2-Nah-Box als [start, start + 4*depth4), d.h.
 * TIEF/Stehend "ab 4100 MISS", Schrot-HOCH "ab 3100 MISS". FUN_800410CC addiert aber VOR dem
 * Box-Test die Ziel-Radien: `*(short*)(rec+8) += +0x1EE >> 2` (Nah-Box-TIEFE) und
 * `rec+10/+0x12/+0x1A += +0x9A >> 2` (Breiten). Der Zombie-INIT schreibt `sh 500,494(s2)`
 * @0x80100980 (+0x1EE = 500, einziger Schreiber im Overlay, gilt auch fuer Kriecher). In der
 * /4-Skala des Dossiers heisst das: Nah-Box-Tiefe + 500 Welt-Einheiten ->
 *   DOWN/LEVEL Nah-Box [100, 4600) statt [100, 4100)   (Pistole UND Schrot)
 *   Schrot-UP Nah-Box  [100, 3600) statt [100, 3100)
 * Die Sonde misst dieselbe Aufstellung wie probe_r16_trefferhoehe (dy = 0, +x-Seite), aber
 * Distanzen um diese Grenzen herum, und druckt SOLL zweimal: Dossier-Modell (ohne Radius)
 * und mit +0x1EE-Erweiterung. Stehend (Maske 3) und Kriecher (Maske 1).
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

static int find_zombie(int want_lying)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        const re15_actor_t *e = &g_actors[s];
        if (!e->active || !(e->type >= 0x10 && e->type <= 0x18)) continue;
        int ly = (e->grid_id & 0x8f) == 0x88;
        if (want_lying ? ly : !(e->grid_id & 0x80)) return s;
    }
    return -1;
}

/* ============================ RE2-REFERENZ (eigener Dump 2026-09-19) ===================== */
/* PTR_DAT_800a6a88[0x10] = 0x800A412C; Record = base + (item-1)*20 (a1 = (item-1)<<16 @0x80047ec0-c8);
 * +8 UP / +0xC LEVEL / +0x10 DOWN. Eigener Dump: idx2 (Id 3) UP(-5000,-2000) LV(-3000,2000)
 * DN(-500,3000); idx6 (Id 7) UP(-5000,500) LV(-3000,2000) DN(-500,3000). */
static const int16_t FEN[20][6] = {
    [ 3] = {-5000,-2000,-3000,2000, -500,3000},
    [ 7] = {-5000,  500,-3000,2000, -500,3000},
};
typedef struct { uint8_t flag[3]; int16_t start[3]; int16_t depth4[3]; } georec_t;
/* Zeiger @0x800A68E8 + item*24 + grp*8 (eigener Dump): */
static const georec_t GEO_DOWN  = { {0x04,0x02,0x01}, {100,4100,8100}, {1000,1000,2500} }; /* item7 grp0 @0x800A6724 */
static const georec_t GEO_LEVEL = { {0x08,0x08,0x08}, {200,4200,8200}, {1000,1000,7500} }; /* item7 grp1 @0x800A6740 */
static const georec_t GEO_UP    = { {0x10,0x20,0x40}, {100,3100,9100}, { 750,1500,2500} }; /* item7 grp2 @0x800A675C */
static const georec_t HG_DOWN   = { {0x04,0x02,0x01}, {100,4100,8100}, {1000,1000,2500} }; /* item3 grp0 @0x800A6618 */
static const georec_t HG_LEVEL  = { {0x08,0x08,0x08}, {200,4200,8200}, {1000,1000,7500} }; /* item3 grp1 @0x800A6634 */
static const georec_t HG_UP     = { {0x10,0x20,0x40}, {100,4100,8200}, {1000,1000,2500} }; /* item3 grp2 @0x800A6650 */
static const uint8_t PRIO[18] = { 4,2,1, 2,1,4, 1,2,4,   4,2,0, 2,0,0, 1,2,0 };         /* DAT_800A6DB4, eigener Dump */

static int re2_window(const int16_t *fw, uint8_t flag, int *lo, int *hi)
{
    int l, h, third;
    switch (flag) {
    case 0x01: l = fw[4]; h = fw[5]; third = (l - h) / 3; l -= 2*third; h -= 2*third; break;
    case 0x02: l = fw[4]; h = fw[5]; third = (l - h) / 3; l -= third;   h -= third;   break;
    case 0x04: l = fw[4]; h = fw[5]; third = (l - h) / 3; break;
    case 0x08: l = fw[2]; h = fw[3]; third = (l - h) / 3; break;
    case 0x10: l = fw[0]; h = fw[1]; third = (l - h) / 3; break;
    case 0x20: l = fw[0]; h = fw[1]; third = (l - h) / 3; l += third;   h += third;   break;
    default:   l = fw[0]; h = fw[1]; third = (l - h) / 3; l += 2*third; h += 2*third; break;
    }
    *lo = l; *hi = h; return third;
}
/* ext_1ee: 0 = Dossier-Modell; sonst der Wert von +0x1EE (500 @0x80100980), der als
 * `rec+8 += +0x1EE>>2` NUR auf depth4 der Nah-Box (Sub-Box 0) wirkt (FUN_800410CC). Der
 * Box-Test FUN_80041CE4 ist ein Vorzeichen-Test der Kreuzprodukte; die ferne Kante zaehlt
 * als AUSSEN (Produkt 0 -> Vorzeichenbit 0 -> "gleich" -> false), also [start, ende). Wie im
 * Dossier ist uVar7 pro Sub-Box neu 0, wenn die XZ-Box trifft, aber keine Maske passt. */
static int re2_soll(unsigned mask, int rid, int elev, int dist, int dy, int ext_1ee)
{
    const georec_t *g = (rid == 7) ? ((elev > 0) ? &GEO_UP : (elev < 0) ? &GEO_DOWN : &GEO_LEVEL)
                                   : ((elev > 0) ? &HG_UP  : (elev < 0) ? &HG_DOWN  : &HG_LEVEL);
    const int16_t *fw = FEN[rid];
    int res = -1;
    if (mask == 0) return -1;
    for (int b = 0; b < 3; b++) {
        int lo, hi, third = re2_window(fw, g->flag[b], &lo, &hi);
        if ((unsigned)(dy - lo) > (unsigned)(hi - lo)) continue;
        int depth4 = g->depth4[b] + ((b == 0) ? (ext_1ee >> 2) : 0);
        if (dist < g->start[b] || dist >= g->start[b] + depth4*4) continue;
        int row = 6;
        if (hi + 2*third <= dy) row = 3;
        if (hi + third   <= dy) row = 0;
        const uint8_t *pb = PRIO + row + ((g->flag[b] & 8) ? 9 : 0);
        int part = -1;
        if (pb[2] & mask) part = pb[2] >> 1;
        if (pb[1] & mask) part = pb[1] >> 1;
        if (pb[0] & mask) part = pb[0] >> 1;
        res = (part >= 0) ? part + 8*b : -1;      /* `uVar7 = 0` je getroffener Sub-Box */
    }
    return res;
}

static const char *EL[3] = { "TIEF", "EBEN", "HOCH" };
static void fmt(char *buf, size_t n, int soll)
{
    if (soll < 0) snprintf(buf, n, "MISS");
    else snprintf(buf, n, "HIT z=%d br=%d", soll & 7, soll >> 3);
}

static void oracle_row(const char *lage, unsigned re2_mask, int slot, int weapon, int elev, int dist)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t esave = *e, psave = *pl;
    pl->x = e->x + dist; pl->z = e->z; pl->y = e->y; pl->floor = e->floor;
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    pl->hp = 100;
    re15_player_set_aim_elevation_for_test(elev);
    int16_t hp0 = e->hp;
    int ret = re15_player_weapon_fire(weapon);
    int rid = (weapon == 8) ? 7 : 3;
    int s0 = re2_soll(re2_mask, rid, elev, dist, 0, 0);
    int s1 = re2_soll(re2_mask, rid, elev, dist, 0, 500);
    char b0[24], b1[24]; fmt(b0, sizeof b0, s0); fmt(b1, sizeof b1, s1);
    int ist_hit = (ret != 0);
    printf("  %-8s W%d %-4s d=%4d | IST %s dhp=%4d 1d2=%d | SOLL-Dossier %-13s | SOLL+1EE %-13s %s\n",
           lage, weapon, EL[elev + 1], dist, ist_hit ? "HIT " : "MISS", (int)(hp0 - e->hp),
           e->re2z_hits1d2, b0, b1, (s0 < 0) != (s1 < 0) ? "<-- GRENZE VERSCHOBEN" : "");
    *e = esave; *pl = psave;
}

static void oracle_matrix(const char *lage, unsigned re2_mask, int slot)
{
    static const int D[9] = { 3500, 3599, 3600, 3900, 4100, 4300, 4599, 4600, 4700 };
    const re15_actor_t *e = &g_actors[slot];
    printf("--- Lage %s: slot %d st=%d/%d/%d grid=%02x f10e=%04x 21a=%04x 1d3=%02x y=%d hp=%d | RE2-Maske=%u ---\n",
           lage, slot, e->state, e->sub_state_1, e->sub_state_2, e->grid_id,
           e->re2z_f10e, e->re2z_flags21a, e->re2z_self1d3, (int)e->y, e->hp, re2_mask);
    for (int w = 0; w < 2; w++)
        for (int el = 1; el >= -1; el--)
            for (int d = 0; d < 9; d++)
                oracle_row(lage, re2_mask, slot, w ? 8 : 3, el, D[d]);
}

int main(int argc, char **argv)
{
    int hp = (argc > 1) ? atoi(argv[1]) : 4000;
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    if (re15_inv_find_item(8) < 0) (void)re15_inv_grant(8, 99);
    re15_player_set_equipped_weapon(8);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    {   re15_enemy_bank_t *b = load_re2_bank(0x10);
        printf("RE2-Bank EM010: %s\n", (b && b->ok) ? "geladen" : "FEHLT");
        if (!b || !b->ok) return 1; }
    int slot = find_zombie(0), lyer = find_zombie(1);
    if (slot < 0) { printf("FAIL: kein stehender Zombie\n"); return 1; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot && s != lyer) g_actors[s].active = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    if (lyer >= 0) g_actors[lyer].x = e->x + 20000;
    e->hp = (int16_t)hp;

    /* A STEHEND: Fresser-Pose (EXEC[8], Exit `sw 0x101` @0x80103D94 OHNE Maskenwechsel) verlassen */
    pl->x = e->x + 1500; pl->z = e->z;
    {   int f;
        for (f = 0; f < 900; f++) {
            pl->hp = 100; frame(0, 0);
            if (e->state == 1 && e->sub_state_1 <= 3 && e->re2z_self1d3 == 0) break;
            if (f > 600 && e->state == 1 && e->sub_state_1 <= 3) { e->re2z_self1d3 = 0; break; }
        }
        pl->x = e->x + 12000;
        printf("[A] Spawn-Pose verlassen nach %d Bildern: st=%d/%d/%d f10e=%04x 1d3=%02x\n", f,
               e->state, e->sub_state_1, e->sub_state_2, e->re2z_f10e, e->re2z_self1d3);
    }
    oracle_matrix("STEHEND", 3u, slot);

    /* B KRIECHER ueber den Ragdoll-Pfad (wie im Dossier) */
    pl->x = e->x + 3600; pl->z = e->z; pl->y = e->y; pl->hp = 100;
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }
    int fired = 0, crawl_frame = -1;
    for (int f = 0; f < 1200; f++) {
        pl->hp = 100;
        re15_player_set_aim_elevation_for_test(0);
        int shoot = (!fired && e->state == 1);
        frame(RE15_PAD_BIT_R1 | (shoot ? RE15_PAD_BIT_SQUARE : 0), shoot ? RE15_PAD_BIT_SQUARE : 0);
        if (!fired && e->hp < hp) fired = 1;
        if (fired && (e->re2z_f10e & 1u) && e->state == 1) { crawl_frame = f; break; }
    }
    if (crawl_frame < 0) printf("[B] FAIL: kein Kriecher erreicht (fired=%d)\n", fired);
    else {
        printf("[B] Kriecher ab F%04d\n", crawl_frame);
        for (int f = 0; f < 30; f++) { pl->hp = 100; pl->x = e->x + 9000; frame(0, 0); }
        oracle_matrix("KRIECHER", 1u, slot);
    }
    printf("=== ENDE ===\n");
    return 0;
}
