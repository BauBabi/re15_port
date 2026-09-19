/**
 * @file probe_r16_trefferhoehe.c
 * @brief MESSSONDE + PIN (Runde 16, 2026-09-19, Thema trefferhoehe).
 *
 * Nutzer: "Ich kann gerade schiessen und treffe Zombies am Boden. Das kann so nicht sein."
 *
 * Misst, was der Port-Hitscan (re15_player_weapon_fire = RE1.5 FUN_80011F50 + RE2-Applier
 * re15_re2_gun_probe, re15_damage.c) bei Zielhoehe HOCH/EBEN/TIEF gegen einen Zombie in fuenf
 * Lagen tut:
 *   A  STEHEND      (Ausgangszustand, ROOM1140-Spawn)
 *   D  LIEGE-SPAWN  (Deskriptor 0x88 aus dem RDT — nur Messung, Entscheidung beim Nachbar-
 *                    Dossier liegende-zombies)
 *   B  KRIECHER     (echter Pfad: W8 EBEN fern -> Ragdoll 0x801066FC -> P2 -> +0x10E = 0x2001)
 *   C  LIEGEND-P3   (echter EXEC[5]-Lauf ab dem 0x501-Eintrag: Phase 3 = am Boden)
 *      AUFSTEHEN-P6 (EXEC[5] Phase 6) und der P7-Lauf bis zum Masken-Rueckbau bei Clip-Bild 55
 * Orakel je Lage: Waffe {W3 Pistole -> RE2-Id 3, W8 Pump-Schrot -> RE2-Id 7} x Zielhoehe
 * {+1,0,-1} x Distanz {1500,2500,3599,3600,4599,4600,5500,9000}. Ein Schuss ueber den Resolver,
 * danach wird der Aktor byte-genau zurueckgesetzt (kein game_step) — reine Trefferfrage.
 *
 * Daneben rechnet die Sonde RE2s EIGENEN Applier nach (info/re2leon/PSX.EXE, FUN_800410CC +
 * FUN_80041B20 + FUN_80041CE4; Tabellen selbst gedumpt, re15_port/tools/re2_gun_tables_dump.py):
 *   Teile-Maske   word0>>26&7  (INIT |= 0x0C000000 @0x80100984 = Beine+Rumpf; liegend/kriechend
 *                 (word0 & 0xF3FFFFFF) | 0x04000000 = NUR Beine @0x801032E8/0x80104098/0x80104444/
 *                 0x80106B38/0x80107828/0x80100B38/0x80100C0C/0x80102C10/0x80102D80/0x801089B4;
 *                 Rueckbau |= 0x0C000000 @0x801036DC (nur +0x14D == 55)/0x80103730/0x80103908/
 *                 0x80107EA8/0x801049F0)
 *   Fenster       Zombie-Record @0x800A412C+(id-1)*20: +8 UP / +0xC LEVEL / +0x10 DOWN
 *   Sub-Boxen     Records @0x800A6724 (DOWN, Flags 04/02/01, Start 100/4100/8100),
 *                 @0x800A6740 (LEVEL, 08/08/08, 200/4200/8200), @0x800A675C (UP, 10/20/40,
 *                 100/3100/9100); Pistole @0x800A6618/34/50; Fenster-Verschiebung = FUN_80041B20
 *   Radien        Nah-Box-Tiefe += +0x1EE>>2 (500 @0x80100980), Breiten += +0x9A>>2
 *                 (Skeptiker-Korrektur 1: TIEF/EBEN-Nah-Box bis 4600, Schrot-HOCH bis 3600)
 *   Zeilen        DAT_800A6DB4 = [04 02 01|02 01 04|01 02 04] / +9: [04 02 00|02 00 00|01 02 00]
 *   Sub-Box 2/3   setzen das Ergebnis VOR der Maskenpruefung auf 0 (Skeptiker-Korrektur 5)
 * und druckt je Zelle "SOLL" (RE2: Treffer? Zone? Klammer?) neben "IST" (Port).
 *
 * Aufruf: probe_r16_trefferhoehe [pin | hp]
 *   pin -> ctest unit_r16_trefferhoehe_pin: jede Zelle der Lagen STEHEND/KRIECHER/LIEGEND-P3/
 *          AUFSTEHEN-P6 muss dem RE2-SOLL entsprechen (Treffer, Zone, Klammer), dazu die
 *          Schadensspalten je Klammer (16/15/14, 200/60/40) und der P7-Rueckbau bei Bild 55.
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
static int                s_pin = 0;
static int                s_fail = 0;
static int                s_cells = 0;

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
    [ 3] = {-5000,-2000,-3000,2000, -500,3000},   /* Handgun @0x800A4154 */
    [ 7] = {-5000,  500,-3000,2000, -500,3000},   /* Shotgun @0x800A41A4 */
};
/* Schaden je Klammer (Record-Wort 0, 10 Bit je Spalte): Id 3 = 0x00e03c10, Id 7 = 0x0280f0c8. */
static const int DMG[20][3] = { [3] = {16,15,14}, [7] = {200,60,40} };
/* Geometrie-Records (0x1C Bytes): Byte1..3 = Sub-Box-Flags, dann 3 x {start, 0, depth/4, halfw/4} */
typedef struct { uint8_t flag[3]; int16_t start[3]; int16_t depth4[3]; } georec_t;
static const georec_t GEO_DOWN  = { {0x04,0x02,0x01}, {100,4100,8100}, {1000,1000,2500} }; /* @0x800A6724 */
static const georec_t GEO_LEVEL = { {0x08,0x08,0x08}, {200,4200,8200}, {1000,1000,7500} }; /* @0x800A6740 */
static const georec_t GEO_UP    = { {0x10,0x20,0x40}, {100,3100,9100}, { 750,1500,2500} }; /* @0x800A675C */
static const georec_t HG_DOWN   = { {0x04,0x02,0x01}, {100,4100,8100}, {1000,1000,2500} }; /* @0x800A6618 */
static const georec_t HG_LEVEL  = { {0x08,0x08,0x08}, {200,4200,8200}, {1000,1000,7500} }; /* @0x800A6634 */
static const georec_t HG_UP     = { {0x10,0x20,0x40}, {100,4100,8200}, {1000,1000,2500} }; /* @0x800A6650 */
static const uint8_t PRIO[18] = { 4,2,1, 2,1,4, 1,2,4,   4,2,0, 2,0,0, 1,2,0 };         /* DAT_800A6DB4 */
#define RAD_1EE 500   /* +0x1EE: `sh v1,494(s2)` @0x80100980, einziger Schreiber im Overlay */

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
/* FUN_800410CC-Kern fuer dy und Distanz (Spieler exakt ausgerichtet, seitlicher Versatz 0 —
 * die Breiten-Erweiterung +0x9A>>2 ist damit wirkungslos; die Nah-Box-Tiefe traegt +0x1EE>>2).
 * XZ-Box = [start, start + 4*depth4) (FUN_80041CE4, ferne Kante aussen). Rueckgabe -1 = kein
 * Treffer, sonst Teil (0 Beine/1 Rumpf/2 Kopf) + 8*Sub-Box-Index; Sub-Box 2/3 setzen das
 * Ergebnis vor der Maskenpruefung auf 0 (`uVar7 = 0`). */
static int re2_soll(unsigned mask, int rid, int elev, int dist, int dy)
{
    const georec_t *g = (rid == 7) ? ((elev > 0) ? &GEO_UP : (elev < 0) ? &GEO_DOWN : &GEO_LEVEL)
                                   : ((elev > 0) ? &HG_UP  : (elev < 0) ? &HG_DOWN  : &HG_LEVEL);
    const int16_t *fw = FEN[rid];
    int res = -1;
    if (mask == 0) return -1;                       /* `if (uVar5 != 0)` — ohne Maske kein Ziel */
    for (int b = 0; b < 3; b++) {
        int lo, hi, third = re2_window(fw, g->flag[b], &lo, &hi);
        if ((unsigned)(dy - lo) > (unsigned)(hi - lo)) continue;     /* Fenster */
        int depth4 = g->depth4[b] + ((b == 0) ? (RAD_1EE >> 2) : 0);  /* rec+8 += +0x1EE>>2 */
        if (dist < g->start[b] || dist >= g->start[b] + depth4*4) continue;  /* XZ-Box */
        int row = 6;
        if (hi + 2*third <= dy) row = 3;
        if (hi + third   <= dy) row = 0;
        const uint8_t *pb = PRIO + row + ((g->flag[b] & 8) ? 9 : 0);
        int part = -1;
        if (pb[2] & mask) part = pb[2] >> 1;
        if (pb[1] & mask) part = pb[1] >> 1;
        if (pb[0] & mask) part = pb[0] >> 1;
        res = (part >= 0) ? part + 8*b : -1;
    }
    return res;
}

/* ================================== ORAKEL ============================================ */
static const char *EL[3] = { "TIEF", "EBEN", "HOCH" };

static void oracle_row(const char *lage, int slot, int weapon, int elev, int dist, int assert_cell)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    re15_actor_t esave = *e, psave = *pl;
    unsigned re2_mask = (unsigned)e->re2z_parts & 7u;   /* das Port-Feld = word0>>26&7 */
    pl->x = e->x + dist; pl->z = e->z; pl->y = e->y; pl->floor = e->floor;
    pl->rot_y = (int16_t)((re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
    pl->hp = 100;                       /* Aufstellung wie probe_re2z_bandlock::oracle_hits */
    re15_player_set_aim_elevation_for_test(elev);   /* Latch/Pausenfilter (+0x93) NICHT angefasst:
                                                     * der Snapshot-Restore haelt ihn sauber */
    int16_t hp0 = e->hp;
    int ret = re15_player_weapon_fire(weapon);
    int rid  = (weapon == 8) ? 7 : 3;
    int soll = (esave.hp < 0 || esave.re2z_self1d3 != 0) ? -1      /* Gates @0x80047140/50 */
             : re2_soll(re2_mask, rid, elev, dist, 0);
    char sollbuf[32];
    if (soll < 0) snprintf(sollbuf, sizeof sollbuf, "MISS");
    else snprintf(sollbuf, sizeof sollbuf, "HIT zone=%d br=%d", soll & 7, soll >> 3);
    int ist_hit = (ret != 0);
    int dhp = (int)(hp0 - e->hp);
    int soll_br = (soll < 0) ? 0 : (soll >> 3);
    int agree = (ist_hit == (soll >= 0)) &&
                (!ist_hit || ((soll & 7) == e->re2z_hits1d2 % 3 && soll_br == e->re2z_hits1d2 / 3));
    /* Schaden je Klammer: Record-Wort 0 (Crit = RE1.5-Regel hp=-1 bei W8 < 3000, Typ < 0x20 —
     * dort ist dhp = hp0+1, das ist Port-Bestand und wird nicht geprueft). */
    int dmg_ok = 1;
    if (ist_hit && !(weapon == 8 && dist < 3000) && soll >= 0)
        dmg_ok = (dhp == DMG[rid][soll_br]);
    printf("  %-12s W%d %-4s d=%4d | IST %s dhp=%4d 1d2=%d zeile=%d st=%d hnd=%d | SOLL(RE2) %-16s %s%s\n",
           lage, weapon, EL[elev + 1], dist,
           ist_hit ? "HIT " : "MISS", dhp, e->re2z_hits1d2, e->sub_state_1,
           e->state, re15_re2z_last_hit_handler(), sollbuf,
           agree ? "" : "<-- DIVERGENZ", dmg_ok ? "" : " <-- SCHADEN");
    if (assert_cell) { s_cells++; if (!agree || !dmg_ok) s_fail++; }
    *e = esave; *pl = psave;
}

static void oracle_matrix(const char *lage, int slot, int assert_cells)
{
    static const int D[8] = { 1500, 2500, 3599, 3600, 4599, 4600, 5500, 9000 };
    re15_actor_t *e = &g_actors[slot];
    /* FUN_80041CE4 rechnet im /4-Raum: Box-Ecke `(R*v + T) >> 2` und Gegner `+0x38 >> 2` werden
     * BEIDE abgerundet — die ferne Kante liegt damit auf einem 4er-Raster, das von den absoluten
     * Positionen abhaengt (Gegner-x = 4k+3: 4599 faellt wie 4600 nach aussen). Fuer die
     * Grenz-Zusicherungen 4599/4600 und 3599/3600 steht der Zombie deshalb auf einem Vielfachen
     * von 4 (der Ausgangs-Spawn -1800/-19600 ist es; nach dem Kriechen nicht mehr). */
    e->x &= ~3; e->z &= ~3;
    printf("--- Lage %s: slot %d st=%d/%d/%d/%d grid=%02x f10e=%04x 21a=%04x 1d3=%02x 93=%02x y=%d hp=%d "
           "box r=%u h=%u ofs_y=%d | Maske=%u rad9a=%u ---\n",
           lage, slot, e->state, e->sub_state_1, e->sub_state_2, e->sub_state_3, e->grid_id,
           e->re2z_f10e, e->re2z_flags21a, e->re2z_self1d3, e->hit_react, (int)e->y, e->hp,
           e->hit_radius_min, e->hit_height, e->hit_offset_y, (unsigned)e->re2z_parts,
           (unsigned)e->re2z_rad9a);
    for (int w = 0; w < 2; w++)
        for (int el = 1; el >= -1; el--)
            for (int d = 0; d < 8; d++)
                oracle_row(lage, slot, w ? 8 : 3, el, D[d], assert_cells);
}

static void expect(int cond, const char *what)
{
    if (!cond) { printf("  FAIL: %s\n", what); s_fail++; }
    else         printf("  ok:   %s\n", what);
}

int main(int argc, char **argv)
{
    int hp = 4000;
    if (argc > 1) { if (strcmp(argv[1], "pin") == 0) s_pin = 1; else hp = atoi(argv[1]); }
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
    printf("Spieler-Box r=%u h=%u ofs_y=%d ; Zombie slot %d typ %02x @(%d,%d,%d) Maske=%u rad9a=%u\n",
           pl->hit_radius_min, pl->hit_height, pl->hit_offset_y, slot, e->type,
           (int)e->x, (int)e->y, (int)e->z, (unsigned)e->re2z_parts, (unsigned)e->re2z_rad9a);
    expect(e->re2z_parts == 3u && e->re2z_rad9a == 500u,
           "INIT: Maske Beine+Rumpf (@0x80100984-998) und +0x9A = 500 (@0x8010096C-70)");

    /* ---- A STEHEND: erst die Spawn-Pose (EXEC[8] Fresser, +0x10E=0x4004, +0x1D3=0x80)
     * verlassen lassen — sonst misst man die Pose, nicht den stehenden Zombie ---- */
    pl->x = e->x + 1500; pl->z = e->z;                /* nah: der Fresser wacht auf (EXEC[8] P3/P4) */
    {   int f;
        for (f = 0; f < 900; f++) {
            pl->hp = 100; frame(0, 0);
            if (e->state == 1 && e->sub_state_1 <= 3 && e->re2z_self1d3 == 0) break;
            if (f > 200 && e->state == 1 && e->sub_state_1 <= 3) {
                if (f > 600) { printf("[A] +0x1D3 haengt auf %02x - wird fuer die Messung "
                                      "geloescht (Original-Zwilling @0x80103CE4-FC)\n",
                                      e->re2z_self1d3); e->re2z_self1d3 = 0; break; }
            }
        }
        pl->x = e->x + 12000;                           /* wieder weit weg */
        printf("[A] Spawn-Pose verlassen nach %d Bildern: st=%d/%d/%d f10e=%04x 1d3=%02x Maske=%u\n", f,
               e->state, e->sub_state_1, e->sub_state_2, e->re2z_f10e, e->re2z_self1d3,
               (unsigned)e->re2z_parts);
    }
    expect(e->re2z_parts == 3u, "Fresser-Pose verlassen: Maske bleibt 3 (Exit sw 0x101 @0x80103D94 "
                                "ohne Maskenwechsel - Skeptiker-Korrektur 2)");
    oracle_matrix("STEHEND", slot, 1);

    /* ---- D LIEGE-SPAWN 0x88 (ohne Ticks, direkt nach dem Spawn) — NUR MESSUNG ---- */
    if (lyer >= 0) {
        re15_actor_t *L = &g_actors[lyer];
        L->x = e->x; L->z = e->z + 20000;                 /* eigener Streifen */
        e->x -= 20000;                                    /* stehenden aus dem Weg */
        /* HP NICHT anfassen: Lyer-INIT setzt `sh -1,342` @0x80100A3C-40 (HP=-1) und EXEC[7] P0
         * `+0x1D3 |= 0x80` @0x80103804-14 - beides RE2-Kandidaten-Gates. Maske bleibt 3: der
         * Lyer-INIT @0x80100A24-4C hat KEINEN Maskenwechsel (lui-Vollscan EMOVL10_S0.BIN).
         * Der Port laesst ihn per spawn_pose-Ausnahme treffen (Nachbar-Dossier liegende-zombies);
         * hier bleibt er auf der bisherigen Liege-Regel (kein Pin). */
        oracle_matrix("LIEGESPAWN", lyer, 0);
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
            printf("[B] F%04d st=%d/%d/%d f10e=%04x 21a=%04x grid=%02x y=%d Maske=%u rad9a=%u\n", f, e->state,
                   e->sub_state_1, e->sub_state_2, e->re2z_f10e, e->re2z_flags21a, e->grid_id, (int)e->y,
                   (unsigned)e->re2z_parts, (unsigned)e->re2z_rad9a);
    }
    if (crawl_frame < 0) { printf("[B] FAIL: kein Kriecher erreicht (fired=%d)\n", fired); s_fail++; }
    else {
        printf("[B] Kriecher ab F%04d\n", crawl_frame);
        for (int f = 0; f < 30; f++) { pl->hp = 100; pl->x = e->x + 9000; frame(0, 0); }  /* 30 Bilder Kriechen */
        expect(e->re2z_parts == 1u && e->re2z_rad9a == 200u,
               "Kriecher: Maske NUR BEINE (@0x80106B38-50) und +0x9A = 200 (@0x80106B14-18)");
        oracle_matrix("KRIECHER", slot, 1);
    }

    /* ---- C LIEGEND: EXEC[5] ab dem 0x501-Eintrag (sw 0x501,4 = Zustand 1, Sub 5, Phase 0) ---- */
    {
        re15_actor_t *z = &g_actors[slot];
        z->hp = (int16_t)hp;
        z->state = 1; z->sub_state_1 = 5; z->sub_state_2 = 0; z->sub_state_3 = 0;
        z->re2z_f10e &= (uint16_t)~1u;    /* kein Kriecher-Bit: EXEC[5] laeuft als Steh-Zombie */
        z->grid_id &= 0x7fu;
        z->re2z_parts = 3u; z->re2z_rad9a = 500u;   /* wie ein Stehender, der jetzt faellt */
        pl->x = z->x + 9000; pl->z = z->z;
        int done = 0, in_p3 = 0;
        for (int f = 0; f < 600 && !done; f++) {
            pl->hp = 100; frame(0, 0);
            if (z->state == 1 && z->sub_state_1 == 5 && z->sub_state_2 == 3) in_p3++; else in_p3 = 0;
            if (in_p3 == 20) {                     /* P3 @0x801034DC = Boden-Zucken (liegt) */
                printf("[C] F%04d EXEC[5] Phase 3 (am Boden) seit 20 Bildern\n", f);
                expect(z->re2z_parts == 1u && z->re2z_rad9a == 0u,
                       "EXEC[5] P3: Maske NUR BEINE (P0 @0x801032E8-FC) und +0x9A = 0 (P2 @0x80103478)");
                oracle_matrix("LIEGEND-P3", slot, 1);
                done = 1;
            }
            if ((f % 40) == 0)
                printf("[C] F%04d st=%d/%d/%d f10e=%04x 21a=%04x grid=%02x Maske=%u rad9a=%u\n", f, z->state,
                       z->sub_state_1, z->sub_state_2, z->re2z_f10e, z->re2z_flags21a, z->grid_id,
                       (unsigned)z->re2z_parts, (unsigned)z->re2z_rad9a);
        }
        if (!done) { printf("[C] FAIL: Phase 3 nicht erreicht\n"); s_fail++; }
        /* weiter bis zum Aufstehen (P6/P7) — dort haelt RE2 die Maske noch auf NUR Beine bis
         * zum Rueckbau @0x801036DC (Bild 55) / 0x80103730 (P8) */
        int p6 = 0;
        for (int f = 0; f < 600; f++) {
            pl->hp = 100; frame(0, 0);
            if (z->state == 1 && z->sub_state_1 == 5 && z->sub_state_2 == 6) {
                printf("[C] F%04d EXEC[5] Phase 6 (Aufstehen) erreicht\n", f);
                expect(z->re2z_parts == 1u, "EXEC[5] P6: Maske noch NUR BEINE (Rueckbau erst P7 Bild 55)");
                oracle_matrix("AUFSTEHEN-P6", slot, 1);
                p6 = 1;
                break;
            }
        }
        if (!p6) { printf("[C] FAIL: Phase 6 nicht erreicht\n"); s_fail++; }
        /* ---- P7-Lauf: Tick-Zaehlung bis zum Masken-Rueckbau (Skeptiker-Korrektur 4) ----
         * Original: P6 setzt den Aufsteh-Clip auf Bild 0 (@0x80103574-A8); jeder P7-Tick advanced
         * um 1 (FUN_80029614 `+0x14D += 1` @0x80029B28-34) und prueft DANACH `+0x14D == 55`
         * (@0x801036D0-D8) -> der Rueckbau faellt auf den 55. P7-Tick. */
        if (p6) {
            int p7_ticks = 0, sw_tick = -1, sw_frame = -1, fc = -1, clip = -1, before = -1;
            int rad_at_p8 = -1, hit_eben_before = -1, hit_eben_after = -1;
            for (int f = 0; f < 400; f++) {
                pl->hp = 100; frame(0, 0);
                if (z->state == 1 && z->sub_state_1 == 5 && z->sub_state_2 == 7) {
                    p7_ticks++;
                    if (p7_ticks == 1) { fc = re15_actor_clip_len(z); clip = (int)z->motion; }
                    if (sw_tick < 0 && !(z->re2z_parts & 2u)) before = (int)z->anim_frame;
                    if (sw_tick < 0 && (z->re2z_parts & 2u)) { sw_tick = p7_ticks; sw_frame = (int)z->anim_frame; }
                    /* EBEN-Orakel UM den Rueckbau herum: davor NUR Beine -> MISS, danach HIT Rumpf */
                    if (p7_ticks == 30) {
                        re15_actor_t es = *z, ps = *pl; int16_t hp0 = z->hp;
                        pl->x = z->x + 1500; pl->z = z->z; pl->y = z->y;
                        pl->rot_y = (int16_t)((re15_atan2_q12(z->z - pl->z, z->x - pl->x) - 0x400) & 0x0fff);
                        re15_player_set_aim_elevation_for_test(0);
                        (void)re15_player_weapon_fire(3); hit_eben_before = (z->hp < hp0);
                        *z = es; *pl = ps;
                    }
                    if (sw_tick > 0 && p7_ticks == sw_tick + 1) {
                        re15_actor_t es = *z, ps = *pl; int16_t hp0 = z->hp;
                        pl->x = z->x + 1500; pl->z = z->z; pl->y = z->y;
                        pl->rot_y = (int16_t)((re15_atan2_q12(z->z - pl->z, z->x - pl->x) - 0x400) & 0x0fff);
                        re15_player_set_aim_elevation_for_test(0);
                        (void)re15_player_weapon_fire(3); hit_eben_after = (z->hp < hp0);
                        *z = es; *pl = ps;
                    }
                } else if (p7_ticks > 0) { rad_at_p8 = (int)z->re2z_rad9a; break; }
            }
            printf("[C] P7: Clip %d (%d Bilder) - Rueckbau auf Beine+Rumpf im P7-Tick %d bei anim_frame %d "
                   "(letztes Bild davor %d), P7 dauerte %d Ticks, +0x9A danach %d; EBEN-Pistole aus 1500: "
                   "vor dem Rueckbau %s, danach %s\n",
                   clip, fc, sw_tick, sw_frame, before, p7_ticks, rad_at_p8,
                   hit_eben_before ? "HIT" : "MISS", hit_eben_after ? "HIT" : "MISS");
            /* Original: P6 setzt Bild 0 (@0x80103574-A8); jeder P7-Tick advanced +1 (FUN_80029614
             * `+0x14D += 1` @0x80029B28-34) und prueft DANACH `+0x14D == 55` @0x801036D0-D8. Der
             * Port-Advancer laeuft im Game-Step VOR dem KI-Tick (re15_actors_anim_advance,
             * game_step_common.c), der Tick sieht also den bereits advancten Zaehler — gemessen:
             * der Wechsel faellt auf den Tick, in dem der Zaehler 55 zeigt, und das Bild davor
             * ist 54. Die Zusicherung gilt dem BILD-WERT (das ist die Instruktion), nicht der
             * Tick-Nummer. */
            if (fc > 55) {
                expect(sw_frame == 55 && before == 54,
                       "P7: Maske Beine+Rumpf genau bei Clip-Bild 55 (+0x14D == 55 @0x801036D0-D8)");
                expect(hit_eben_before == 0, "P7 vor Bild 55: EBEN-Pistole trifft NICHT (nur Beine)");
                expect(hit_eben_after == 1,  "P7 nach Bild 55: EBEN-Pistole trifft (Rumpf)");
            } else
                expect(sw_tick < 0 && (z->re2z_parts & 2u),
                       "P7 kuerzer als 55 Bilder: Rueckbau erst P8 @0x80103730");
            expect(rad_at_p8 == 500, "P8: +0x9A = 500 (@0x801036FC-700)");
            expect(z->re2z_parts == 3u, "P8: Maske Beine+Rumpf (@0x80103730-38)");
        }
    }
    printf("=== ENDE === Zellen geprueft: %d, Abweichungen: %d\n", s_cells, s_fail);
    if (s_pin) {
        if (s_fail) { printf("R16 TREFFERHOEHE PIN: FAIL (%d)\n", s_fail); return 1; }
        printf("R16 TREFFERHOEHE PIN: OK\n");
    }
    return 0;
}
