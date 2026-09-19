/**
 * @file probe_r16_trefferhoehe.c
 * @brief MESSSONDE (Runde 16, 2026-09-19, Thema trefferhoehe) — kein ctest.
 *
 * Nutzer: "Ich kann gerade schiessen und treffe Zombies am Boden. Das kann so nicht sein."
 *
 * Misst, was der Port-Hitscan (re15_player_weapon_fire = RE1.5 FUN_80011F50 + RE2-Fenster-
 * Bruecke, re15_damage.c) bei Zielhoehe HOCH/EBEN/TIEF gegen einen Zombie in vier Lagen tut:
 *   A  STEHEND      (Ausgangszustand, ROOM1140-Spawn)
 *   B  KRIECHER     (echter Pfad: W8 EBEN fern -> Ragdoll 0x801066FC -> P2 -> +0x10E = 0x2001)
 *   C  LIEGEND      (echter EXEC[5]-Lauf ab dem 0x501-Eintrag: Phasen 3..5 = am Boden)
 *   D  LIEGE-SPAWN  (Deskriptor 0x88 aus dem RDT, falls im Raum)
 * Orakel je Lage: Waffe {W3 Pistole -> RE2-Id 3, W8 Pump-Schrot -> RE2-Id 7} x Zielhoehe
 * {+1,0,-1} x Distanz {1500,2500,3600,5500,9000}. Ein Schuss ueber den Resolver, danach wird
 * der Aktor byte-genau zurueckgesetzt (kein game_step) — reine Trefferfrage.
 *
 * Daneben rechnet die Sonde RE2s EIGENEN Applier nach (info/re2leon/PSX.EXE, FUN_800410CC +
 * FUN_80041B20, alle Tabellen selbst gedumpt 2026-09-19):
 *   Teile-Maske   word0>>26&7  (INIT |= 0x0C000000 @0x80100984 = Beine+Rumpf; liegend/kriechend
 *                 (word0 & 0xF3FFFFFF) | 0x04000000 = NUR Beine @0x801032E8/0x80104098/0x80104444/
 *                 0x80106B38/0x80107828/0x80100B38/0x80100C0C; Rueckbau |= 0x0C000000 beim
 *                 Aufstehen @0x801036DC/0x80103730/0x80103908/0x80107EA8)
 *   Fenster       Zombie-Record @0x800A412C+(id-1)*20: +8 UP / +0xC LEVEL / +0x10 DOWN
 *   Sub-Boxen     Records @0x800A6724 (DOWN, Flags 04/02/01, Start 100/4100/8100),
 *                 @0x800A6740 (LEVEL, 08/08/08, 200/4200/8200), @0x800A675C (UP, 10/20/40,
 *                 100/3100/9100); Fenster-Verschiebung je Flag = FUN_80041B20
 *   Zeilen        DAT_800A6DB4 = [04 02 01|02 01 04|01 02 04] / +9: [04 02 00|02 00 00|01 02 00]
 * und druckt je Zelle "SOLL" (RE2: Treffer? Zone? Klammer?) neben "IST" (Port).
 *
 * Aufruf: probe_r16_trefferhoehe [hp=4000]
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

/* ============================ RE2-REFERENZ (Nachrechnung) ================================ */
/* Zombie-Record @0x800A412C + (id-1)*20: Fenster-Paare +8 UP, +0xC LEVEL, +0x10 DOWN. */
static const int16_t FEN[20][6] = {
    [ 3] = {-5000,-2000,-3000,2000, -500,3000},   /* Handgun */
    [ 7] = {-5000,  500,-3000,2000, -500,3000},   /* Shotgun */
};
/* Geometrie-Records (0x1C Bytes): Byte1..3 = Sub-Box-Flags, dann 3 x {start, 0, depth/4, halfw/4} */
typedef struct { uint8_t flag[3]; int16_t start[3]; int16_t depth4[3]; } georec_t;
static const georec_t GEO_DOWN  = { {0x04,0x02,0x01}, {100,4100,8100}, {1000,1000,2500} }; /* @0x800A6724 */
static const georec_t GEO_LEVEL = { {0x08,0x08,0x08}, {200,4200,8200}, {1000,1000,7500} }; /* @0x800A6740 */
static const georec_t GEO_UP    = { {0x10,0x20,0x40}, {100,3100,9100}, { 750,1500,2500} }; /* @0x800A675C */
/* Pistole (Item 3, Zeiger @0x800A6930): eigene Records, gleiche Starts bei DOWN/LEVEL, UP 100/4100/8200 */
static const georec_t HG_DOWN   = { {0x04,0x02,0x01}, {100,4100,8100}, {1000,1000,2500} }; /* @0x800A6618 */
static const georec_t HG_LEVEL  = { {0x08,0x08,0x08}, {200,4200,8200}, {1000,1000,7500} }; /* @0x800A6634 */
static const georec_t HG_UP     = { {0x10,0x20,0x40}, {100,4100,8200}, {1000,1000,2500} }; /* @0x800A6650 */
static const uint8_t PRIO[18] = { 4,2,1, 2,1,4, 1,2,4,   4,2,0, 2,0,0, 1,2,0 };         /* DAT_800A6DB4 */

/* FUN_80041B20: Fenster + Drittel aus Flag. third = (lo-hi)/3 (negativ). */
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
/* FUN_800410CC-Kern fuer dy und Distanz (XZ-Box = Streifen ab start, Tiefe depth4*4 — der
 * Box-Test FUN_80041CE4 laeuft in Viertel-Einheiten, s. Dossier §2.4). Rueckgabe: -1 = kein
 * Treffer, sonst Teil (0 Beine/1 Rumpf/2 Kopf) + 8*Klammer(0..2). */
static int re2_soll(unsigned mask, int rid, int elev, int dist, int dy, int hp, unsigned self1d3)
{
    if (hp < 0) return -1;          /* Kandidaten-Gate HP<0: `bltz` @0x80047150 / 410cc `-1 < +0x156` */
    if (self1d3 != 0) return -1;    /* Kandidaten-Gate +0x1D3 != 0: @0x80047140 / 410cc `+0x1d3 == 0` */
    const georec_t *g = (rid == 7) ? ((elev > 0) ? &GEO_UP : (elev < 0) ? &GEO_DOWN : &GEO_LEVEL)
                                   : ((elev > 0) ? &HG_UP  : (elev < 0) ? &HG_DOWN  : &HG_LEVEL);
    const int16_t *fw = FEN[rid];
    int res = -1;
    if (mask == 0) return -1;                       /* `if (uVar5 != 0)` — ohne Maske kein Ziel */
    for (int b = 0; b < 3; b++) {
        int lo, hi, third = re2_window(fw, g->flag[b], &lo, &hi);
        if ((unsigned)(dy - lo) > (unsigned)(hi - lo)) continue;     /* Fenster */
        if (dist < g->start[b] || dist >= g->start[b] + g->depth4[b]*4) continue;  /* XZ-Box */
        int row = 6;
        if (hi + 2*third <= dy) row = 3;
        if (hi + third   <= dy) row = 0;
        const uint8_t *pb = PRIO + row + ((g->flag[b] & 8) ? 9 : 0);
        int part = -1;
        if (pb[2] & mask) part = pb[2] >> 1;
        if (pb[1] & mask) part = pb[1] >> 1;
        if (pb[0] & mask) part = pb[0] >> 1;
        if (part >= 0) res = part + 8*b;
    }
    return res;
}

/* ================================== ORAKEL ============================================ */
static const char *EL[3] = { "TIEF", "EBEN", "HOCH" };

static void oracle_row(const char *lage, unsigned re2_mask, int slot, int weapon, int elev, int dist)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t esave = *e, psave = *pl;
    pl->x = e->x + dist; pl->z = e->z; pl->y = e->y; pl->floor = e->floor;
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    pl->hp = 100;                       /* Aufstellung wie probe_re2z_bandlock::oracle_hits */
    re15_player_set_aim_elevation_for_test(elev);   /* Latch/Pausenfilter (+0x93) NICHT angefasst:
                                                     * der Snapshot-Restore haelt ihn sauber */
    int16_t hp0 = e->hp;
    int ret = re15_player_weapon_fire(weapon);
    int soll = re2_soll(re2_mask, (weapon == 8) ? 7 : 3, elev, dist, 0, esave.hp, esave.re2z_self1d3);
    char sollbuf[32];
    if (soll < 0) snprintf(sollbuf, sizeof sollbuf, "MISS");
    else snprintf(sollbuf, sizeof sollbuf, "HIT zone=%d br=%d", soll & 7, soll >> 3);
    int ist_hit = (ret != 0);
    int agree = (ist_hit == (soll >= 0)) && (!ist_hit || ((soll & 7) == e->re2z_hits1d2 % 3));
    printf("  %-10s W%d %-4s d=%4d | IST %s dhp=%4d 1d2=%d zeile=%d st=%d hnd=%d | SOLL(RE2) %-16s %s\n",
           lage, weapon, EL[elev + 1], dist,
           ist_hit ? "HIT " : "MISS", (int)(hp0 - e->hp), e->re2z_hits1d2, e->sub_state_1,
           e->state, re15_re2z_last_hit_handler(), sollbuf, agree ? "" : "<-- DIVERGENZ");
    *e = esave; *pl = psave;
}

static void oracle_matrix(const char *lage, unsigned re2_mask, int slot)
{
    static const int D[5] = { 1500, 2500, 3600, 5500, 9000 };
    const re15_actor_t *e = &g_actors[slot];
    printf("--- Lage %s: slot %d st=%d/%d/%d/%d grid=%02x f10e=%04x 21a=%04x 1d3=%02x 93=%02x y=%d hp=%d "
           "box r=%u h=%u ofs_y=%d | RE2-Maske=%u ---\n",
           lage, slot, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, e->grid_id,
           e->re2z_f10e, e->re2z_flags21a, e->re2z_self1d3, e->hit_react, (int)e->y, e->hp,
           e->hit_radius_min, e->hit_height, e->hit_offset_y, re2_mask);
    for (int w = 0; w < 2; w++)
        for (int el = 1; el >= -1; el--)
            for (int d = 0; d < 5; d++)
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
    if (lyer >= 0) g_actors[lyer].x = e->x + 20000;      /* aus dem Streifen schieben */
    e->hp = (int16_t)hp;
    printf("Spieler-Box r=%u h=%u ofs_y=%d ; Zombie slot %d typ %02x @(%d,%d,%d)\n",
           pl->hit_radius_min, pl->hit_height, pl->hit_offset_y, slot, e->type,
           (int)e->x, (int)e->y, (int)e->z);

    /* ---- A STEHEND: erst die Spawn-Pose (EXEC[8] Fresser, +0x10E=0x4004, +0x1D3=0x80)
     * verlassen lassen — sonst misst man die Pose, nicht den stehenden Zombie ---- */
    pl->x = e->x + 1500; pl->z = e->z;                /* nah: der Fresser wacht auf (EXEC[8] P3/P4) */
    {   int f;
        for (f = 0; f < 900; f++) {
            pl->hp = 100; frame(0, 0);
            if (e->state == 1 && e->sub_state_1 <= 3 && e->re2z_self1d3 == 0) break;
            if (f > 200 && e->state == 1 && e->sub_state_1 <= 3) {
                /* Pose verlassen, aber +0x1D3 haengt (Port-Latch): EXEC[8]-P3-Zwilling
                 * `andi 0x7f` @0x80103CE4-FC ist dann noch nicht gelaufen — abwarten */
                if (f > 600) { printf("[A] +0x1D3 haengt auf %02x - wird fuer die Messung "
                                      "geloescht (Original-Zwilling @0x80103CE4-FC)\n",
                                      e->re2z_self1d3); e->re2z_self1d3 = 0; break; }
            }
        }
        pl->x = e->x + 12000;                           /* wieder weit weg */
        printf("[A] Spawn-Pose verlassen nach %d Bildern: st=%d/%d/%d f10e=%04x 1d3=%02x\n", f,
               e->state, e->sub_state_1, e->sub_state_2, e->re2z_f10e, e->re2z_self1d3);
    }
    oracle_matrix("STEHEND", 3u, slot);

    /* ---- D LIEGE-SPAWN 0x88 (ohne Ticks, direkt nach dem Spawn) ---- */
    if (lyer >= 0) {
        re15_actor_t *L = &g_actors[lyer];
        L->x = e->x; L->z = e->z + 20000;                 /* eigener Streifen */
        e->x -= 20000;                                    /* stehenden aus dem Weg */
        /* HP NICHT anfassen: Lyer-INIT setzt `sh -1,342` @0x80100A3C-40 (HP=-1) und EXEC[7] P0
         * `+0x1D3 |= 0x80` @0x80103804-14 - beides RE2-Kandidaten-Gates. Maske bleibt 3: der
         * Lyer-INIT @0x80100A24-4C hat KEINEN Maskenwechsel (lui-Vollscan EMOVL10_S0.BIN). */
        oracle_matrix("LIEGESPAWN", 3u, lyer);
        e->x += 20000; L->x = e->x + 20000; L->z = e->z;
    } else printf("(kein 0x88-Liege-Spawn in ROOM1140 aktiv)\n");

    /* ---- B KRIECHER ueber den echten Ragdoll-Pfad (W8 EBEN, fern > 3000) ---- */
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
        if (!fired && e->hp < hp) { fired = 1;
            printf("[B] F%04d Treffer: hp %d->%d Handler=%d zeile=%d 1d2=%d st=%d/%d/%d\n", f, hp,
                   e->hp, re15_re2z_last_hit_handler(), e->sub_state_1, e->re2z_hits1d2,
                   e->state, e->sub_state_1, e->sub_state_2); }
        if (fired && (e->re2z_f10e & 1u) && e->state == 1) { crawl_frame = f; break; }
        if (fired && (f % 40) == 0)
            printf("[B] F%04d st=%d/%d/%d f10e=%04x 21a=%04x grid=%02x y=%d\n", f, e->state,
                   e->sub_state_1, e->sub_state_2, e->re2z_f10e, e->re2z_flags21a, e->grid_id, (int)e->y);
    }
    if (crawl_frame < 0) printf("[B] FAIL: kein Kriecher erreicht (fired=%d)\n", fired);
    else {
        printf("[B] Kriecher ab F%04d\n", crawl_frame);
        for (int f = 0; f < 30; f++) { pl->hp = 100; pl->x = e->x + 9000; frame(0, 0); }  /* 30 Bilder Kriechen */
        oracle_matrix("KRIECHER", 1u, slot);
    }

    /* ---- C LIEGEND: EXEC[5] ab dem 0x501-Eintrag (sw 0x501,4 = Zustand 1, Sub 5, Phase 0) ---- */
    {
        re15_actor_t *z = &g_actors[slot];
        z->hp = (int16_t)hp;
        z->state = 1; z->sub_state_1 = 5; z->sub_state_2 = 0; z->sub_state_3 = 0;
        z->re2z_f10e &= (uint16_t)~1u;    /* kein Kriecher-Bit: EXEC[5] laeuft als Steh-Zombie */
        z->grid_id &= 0x7fu;
        pl->x = z->x + 9000; pl->z = z->z;
        int done = 0, in_p3 = 0;
        for (int f = 0; f < 600 && !done; f++) {
            pl->hp = 100; frame(0, 0);
            if (z->state == 1 && z->sub_state_1 == 5 && z->sub_state_2 == 3) in_p3++; else in_p3 = 0;
            if (in_p3 == 20) {                     /* P3 @0x801034DC = Boden-Zucken (liegt) */
                printf("[C] F%04d EXEC[5] Phase 3 (am Boden) seit 20 Bildern\n", f);
                oracle_matrix("LIEGEND-P3", 1u, slot);
                done = 1;
            }
            if ((f % 40) == 0)
                printf("[C] F%04d st=%d/%d/%d f10e=%04x 21a=%04x grid=%02x\n", f, z->state,
                       z->sub_state_1, z->sub_state_2, z->re2z_f10e, z->re2z_flags21a, z->grid_id);
        }
        if (!done) printf("[C] FAIL: Phase 3 nicht erreicht\n");
        /* weiter bis zum Aufstehen (P6/P7) — dort haelt RE2 die Maske noch auf NUR Beine bis
         * zum Rueckbau @0x801036DC/0x80103730 */
        for (int f = 0; f < 600; f++) {
            pl->hp = 100; frame(0, 0);
            if (z->state == 1 && z->sub_state_1 == 5 && z->sub_state_2 == 6) {
                printf("[C] F%04d EXEC[5] Phase 6 (Aufstehen) erreicht\n", f);
                oracle_matrix("AUFSTEHEN-P6", 1u, slot);
                break;
            }
        }
    }
    printf("=== ENDE ===\n");
    return 0;
}
