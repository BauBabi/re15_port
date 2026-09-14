/* probe_1d3_zensus.c - MESSSCHIENE (2026-09-14, Auftrag "RE2-Trefferpause fuer Zombie,
 * Kraehe und Spinne nachmessen").
 *
 * FRAGE: laeuft der Bild-Zaehler +0x1D3 (low-7) in JEDEM Zustand ab? Der Zaehler wird beim
 * Treffer vom Applier gestempelt (`lw v0,4(a1)` @0x80047338 / `srl v0,v0,0x9` @0x80047340 /
 * `andi v0,v0,0x7f` @0x80047344 / `or` @0x80047348 / `sb` @0x8004734C) und vom Gegner-Root
 * jedes Bild abgezogen. Ein Ausschluss-Gate auf diesen Zaehler (RE2 @0x80047138-40) sperrt
 * dauerhaft, sobald ein Zustand den Root-Prolog nicht erreicht.
 *
 * MESSVERFAHREN
 *  (A) NATUR: echter Schussweg (game_step + Pad R1/SQUARE), EIN Treffer, dann Bild fuer Bild
 *      protokollieren, bis low-7 == 0.
 *  (B) KANARIENVOGEL: vor JEDEM Bild wird +0x1D3 auf 0x40 gestempelt; nach dem Bild sagt der
 *      Wert eindeutig, ob der Root-Prolog gelaufen ist (0x3F = dekrementiert, 0x40 = NICHT,
 *      alles andere = anderer Schreiber). Aggregiert nach (state, sub1).
 *  (C) ZWANGSZUSTAENDE: Zustaende, die (B) nicht natuerlich erreicht, werden gesetzt und mit
 *      demselben Kanarienvogel geprueft (Hund 4/5/6, Zombie-Sitz 0x12/0x0d, grid&0x20, ...).
 *
 * Reine Messschiene: bestanden = gelaufen. Keine Engine-Aenderung.
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

extern void    re15_player_aim_reset(void);
extern void    re15_player_set_aim_clip_len(int fc);
extern int     re15_player_aim_ready(void);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
extern void    re15_esp_fx_reset(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_room_id = 0x1140;
static int                s_fire_sub = -1;
static uint8_t           *s_keep = NULL;

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
    re15_esp_fx_reset();
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

static int load_room(const char *sub, int room_id, int fire_sub)
{
    char path[600];
    const char *base = getenv("RE15_ASSET_DIR");
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("  !! %s nicht lesbar\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) {
        printf("  !! %s Parse-Fehler\n", path); free(buf); return 0;
    }
    s_room_id = room_id; s_fire_sub = fire_sub;
    if (s_keep) free(s_keep);
    s_keep = buf;
    return 1;
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
    pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

static uint16_t elev_pad_for(const re15_actor_t *e)
{
    if (e->type == 0x21) {
        if (e->aim_band == 4) return RE15_PAD_BIT_UP;
        if (e->aim_band == 1) return RE15_PAD_BIT_DOWN;
        return 0;
    }
    if (e->grid_id & 0x80) return RE15_PAD_BIT_DOWN;
    return 0;
}

static int find_type(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        if (!g_actors[s].active || g_actors[s].type != type) continue;
        if (type >= 0x10 && type <= 0x18 && (g_actors[s].grid_id & 0x80)) continue;
        return s;
    }
    return -1;
}

/* ---------------------------------------------------------------------------------------- */
#define CEN_MAX 64
static uint8_t s_last_other;
typedef struct { int st, s1, ran, notran, other; unsigned char ov; } cen_t;
static cen_t s_cen[CEN_MAX];
static int   s_cen_n;
static void cen_reset(void) { s_cen_n = 0; memset(s_cen, 0, sizeof s_cen); }
static void cen_add(int st, int s1, int what)   /* 0=ran 1=notran 2=other */
{
    for (int i = 0; i < s_cen_n; i++)
        if (s_cen[i].st == st && s_cen[i].s1 == s1) {
            if (what == 0) s_cen[i].ran++;
            else if (what == 1) s_cen[i].notran++;
            else { s_cen[i].other++; s_cen[i].ov = s_last_other; }
            return;
        }
    if (s_cen_n >= CEN_MAX) return;
    s_cen[s_cen_n].st = st; s_cen[s_cen_n].s1 = s1;
    if (what == 0) s_cen[s_cen_n].ran = 1;
    else if (what == 1) s_cen[s_cen_n].notran = 1;
    else { s_cen[s_cen_n].other = 1; s_cen[s_cen_n].ov = s_last_other; }
    s_cen_n++;
}
static void cen_print(const char *tag)
{
    printf("  [%s] ZENSUS (state/sub1 -> Root-Prolog lief / lief NICHT / anderer Schreiber)\n", tag);
    for (int i = 0; i < s_cen_n; i++)
        printf("      st=%-2d sub1=%-3d  lief=%-5d NICHT=%-5d Setzer=%-4d (zuletzt 0x%02X) %s\n",
               s_cen[i].st, s_cen[i].s1, s_cen[i].ran, s_cen[i].notran, s_cen[i].other,
               s_cen[i].ov, s_cen[i].notran ? "  <== TOTER ZUSTAND" : "");
}

/* Kanarienvogel: +0x1D3 := 0x40 vor dem Bild, danach klassifizieren.
 * 0x3F / 0xBF -> low-7 dekrementiert = Root-Prolog LIEF (0xBF: ein Handler hat zusaetzlich
 *                Bit 0x80 gesetzt).
 * 0x40 / 0xC0 -> low-7 unveraendert  = Root-Prolog lief NICHT.
 * alles andere -> ein SETZER hat das ganze Byte ueberschrieben (sb 15/128/0/6/255,467). */
static int canary_step(re15_actor_t *e, uint16_t cur, uint16_t edge)
{
    e->re2z_self1d3 = 0x40u;
    frame(cur, edge);
    uint8_t v = e->re2z_self1d3;
    if (v == 0x3Fu || v == 0xBFu) return 0;
    if (v == 0x40u || v == 0xC0u) return 1;
    s_last_other = v;
    return 2;
}

/* ---------------------------------------------------------------------------------------- */
typedef struct {
    int slot;
    int stamp;
    int frames_to_zero;
    int gaps;
    int bit80_survived;
} natA_t;

static int setup_target(re15_ai_flavor_t flavor, uint8_t type, int weapon, int baby_force)
{
    re15_ai_flavor_set(flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    if (baby_force) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == 0x26)
                g_actors[s].re2s_baby_spawned = 1;
    }
    for (int f = 0; f < 60; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        if (baby_force)
            for (int s = 1; s < RE15_ACTOR_MAX; s++)
                if (g_actors[s].active && g_actors[s].type == 0x26)
                    g_actors[s].re2s_baby_spawned = 1;
        frame(0, 0);
    }
    int slot = find_type(type);
    if (slot < 0) return -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;
    if (type == 0x20 && g_actors[slot].state == 4 && g_actors[slot].grid_id == 0x40)
        g_actors[slot].grid_id = 0x43;
    return slot;
}

static void aim_up(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);
    for (int f = 0; f < 60 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
    }
}

static void passA(const char *tag, uint8_t type, const char *room, int room_id, int fire_sub,
                  int weapon, int baby_force, natA_t *out)
{
    memset(out, 0, sizeof *out); out->frames_to_zero = -1; out->slot = -1;
    if (!load_room(room, room_id, fire_sub)) { printf("  [%s] Raum fehlt\n", tag); return; }
    int slot = setup_target(RE15_AI_FLAVOR_RE2, type, weapon, baby_force);
    if (slot < 0) { printf("  [%s] kein Gegner Typ 0x%02X in %04X\n", tag, type, room_id); return; }
    out->slot = slot;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    e->hp = 9000;
    e->re2z_self1d3 = 0;

    int fired = -1;
    for (int f = 0; f < 200 && fired < 0; f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elev_pad_for(e)),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->re2z_self1d3 & 0x7fu) fired = f;
    }
    if (fired < 0) {
        printf("  [%s] KEIN Treffer im Messfenster (slot %d, st=%d/%d hp=%d +0x93=0x%02X)\n",
               tag, slot, e->state, e->sub_state_1, e->hp, e->hit_react);
        return;
    }
    out->stamp = e->re2z_self1d3 & 0x7f;
    out->bit80_survived = (e->re2z_self1d3 & 0x80u) ? 1 : 0;
    printf("  [%s] slot %d, Stempel low7=%d bit80=%d, st=%d/%d/%d\n",
           tag, slot, out->stamp, out->bit80_survived, e->state, e->sub_state_1, e->sub_state_2);

    int last = e->re2z_self1d3 & 0x7f;
    for (int f = 1; f <= 400; f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
        int now = e->re2z_self1d3 & 0x7f;
        if (f <= 40 || now != last)
            printf("      f%-3d low7=%-3d bit80=%d st=%d/%d/%d clip=%d hp=%d%s\n",
                   f, now, (e->re2z_self1d3 & 0x80u) ? 1 : 0, e->state, e->sub_state_1,
                   e->sub_state_2, (int)e->motion, e->hp, (now == last) ? "  <== STEHT" : "");
        if (now == last && now != 0) out->gaps++;
        last = now;
        if (now == 0) { out->frames_to_zero = f; break; }
    }
    printf("  [%s] => Pause lief nach %d Bildern ab (Stempel %d), Steh-Bilder=%d\n",
           tag, out->frames_to_zero, out->stamp, out->gaps);
}

static void passB(const char *tag, uint8_t type, const char *room, int room_id, int fire_sub,
                  int weapon, int baby_force, int budget)
{
    cen_reset();
    if (!load_room(room, room_id, fire_sub)) { printf("  [%s] Raum fehlt\n", tag); return; }
    int slot = setup_target(RE15_AI_FLAVOR_RE2, type, weapon, baby_force);
    if (slot < 0) { printf("  [%s] kein Gegner Typ 0x%02X in %04X\n", tag, type, room_id); return; }
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;
    aim_up(slot, back);
    e->hp = 30000;
    int deact = -1;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        if (e->hp < 200) e->hp = 30000;
        if (!e->active) { deact = f; break; }
        track(slot, back);
        int st = e->state, s1 = e->sub_state_1;
        int r = canary_step(e, (uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elev_pad_for(e)),
                            (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        cen_add(st, s1, r);
    }
    if (deact >= 0) printf("  [%s] Gegner wurde bei Bild %d INAKTIV (active=0)\n", tag, deact);
    cen_print(tag);
}

static void passC_one(const char *tag, int slot, int st, int s1, int s2, int extra_grid,
                      int re15_pose)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int ran = 0, no = 0, other = 0;
    for (int f = 0; f < 12; f++) {
        e->active = 1;
        e->state = (uint8_t)st; e->sub_state_1 = (uint8_t)s1; e->sub_state_2 = (uint8_t)s2;
        if (extra_grid >= 0) e->grid_id = (uint8_t)extra_grid;
        e->re2z_re15_pose = (uint8_t)re15_pose;
        e->hp = 30000; pl->hp = 100;
        int r = canary_step(e, RE15_PAD_BIT_R1, 0);
        if (r == 0) ran++; else if (r == 1) no++; else other++;
    }
    printf("      %-48s lief=%2d NICHT=%2d Setzer=%2d(zuletzt 0x%02X) %s\n", tag, ran, no,
           other, s_last_other, no ? " <== ROOT-PROLOG LAEUFT NICHT" : "");
}

/* Ersten Raum aus einer Liste finden, der den Typ wirklich aufstellt. */
typedef struct { const char *sub; int id; int fire_sub; } roomcand_t;
static int find_room_for_type(const roomcand_t *c, unsigned n, uint8_t type, int baby_force,
                              const char **out_sub, int *out_id, int *out_fire)
{
    for (unsigned i = 0; i < n; i++) {
        if (!load_room(c[i].sub, c[i].id, c[i].fire_sub)) continue;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2); bringup();
        for (int f = 0; f < 40; f++) {
            if (baby_force)
                for (int s = 1; s < RE15_ACTOR_MAX; s++)
                    if (g_actors[s].active && g_actors[s].type == 0x26)
                        g_actors[s].re2s_baby_spawned = 1;
            frame(0, 0);
        }
        if (find_type(type) >= 0) {
            *out_sub = c[i].sub; *out_id = c[i].id; *out_fire = c[i].fire_sub;
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    natA_t a;
    const char *spider_room = NULL; int spider_id = 0;
    const char *zg_room = NULL; int zg_id = 0, zg_fire = -1;

    printf("\n################ PASS A - NATUR: ein Treffer, dann Abbau #################\n");
    passA("ZOMBIE 0x10 R1140", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, &a);
    {
        static const roomcand_t zc[] = {
            { "STAGE1/ROOM1220.RDT", 0x1220, -1 }, { "STAGE1/ROOM1140.RDT", 0x1140, -1 },
            { "STAGE1/ROOM1100.RDT", 0x1100, -1 }, { "STAGE1/ROOM10D0.RDT", 0x10d0, -1 },
            { "STAGE1/ROOM1200.RDT", 0x1200, -1 }, { "STAGE1/ROOM1210.RDT", 0x1210, -1 },
            { "STAGE1/ROOM1240.RDT", 0x1240, -1 }, { "STAGE1/ROOM1030.RDT", 0x1030, -1 },
            { "STAGE1/ROOM1010.RDT", 0x1010, -1 }, { "STAGE1/ROOM1050.RDT", 0x1050, -1 },
        };
        if (find_room_for_type(zc, sizeof zc / sizeof zc[0], 0x16, 0, &zg_room, &zg_id, &zg_fire)) {
            printf("  (ZGirl 0x16 gefunden in %s)\n", zg_room);
            passA("ZGIRL  0x16", 0x16, zg_room, zg_id, zg_fire, 3, 0, &a);
        } else {
            printf("  !! KEIN Raum mit Typ 0x16 gefunden\n");
        }
    }
    passA("KRAEHE 0x21 R10C0", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, &a);
    passA("HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, &a);
    passA("BABY   0x26 R1090", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, &a);
    {
        static const struct { const char *sub; int id; } cand[] = {
            { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
            { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
            { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
            { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
            { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 },
        };
        for (unsigned i = 0; i < sizeof cand / sizeof cand[0]; i++) {
            if (!load_room(cand[i].sub, cand[i].id, -1)) continue;
            re15_ai_flavor_set(RE15_AI_FLAVOR_RE2); bringup();
            for (int f = 0; f < 30; f++) frame(0, 0);
            if (find_type(0x25) >= 0) {
                spider_room = cand[i].sub; spider_id = cand[i].id;
                printf("  (Adult-Spinne gefunden in %s)\n", spider_room);
                passA("SPINNE 0x25", 0x25, spider_room, spider_id, -1, 3, 0, &a);
                break;
            }
        }
        if (!spider_room) printf("  !! KEIN Raum mit einer lebenden Adult-Spinne 0x25 gefunden\n");
    }

    printf("\n################ PASS B - KANARIENVOGEL-ZENSUS im Dauerfeuer ##############\n");
    passB("ZOMBIE 0x10 R1140", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 3, 0, 900);
    if (zg_room) passB("ZGIRL  0x16", 0x16, zg_room, zg_id, zg_fire, 3, 0, 900);
    passB("KRAEHE 0x21 R10C0", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 3, 0, 900);
    passB("HUND   0x20 R1190", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 3, 0, 900);
    passB("BABY   0x26 R1090", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 3, 1, 900);
    if (spider_room) passB("SPINNE 0x25", 0x25, spider_room, spider_id, -1, 3, 0, 900);

    printf("\n################ PASS C - ZWANGSZUSTAENDE ################################\n");
    if (load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x10, 3, 0);
        if (slot > 0) {
            printf("  ZOMBIE 0x10:\n");
            uint8_t g0 = g_actors[slot].grid_id;
            passC_one("state 1 sub 0 (IDLE)",              slot, 1, 0, 0, g0, 0);
            passC_one("state 1 sub 1 (WALK)",              slot, 1, 1, 0, g0, 0);
            passC_one("state 1 sub 3 (GRIFF)",             slot, 1, 3, 0, g0, 0);
            passC_one("state 1 sub 5 (FRESSEN)",           slot, 1, 5, 0, g0, 0);
            passC_one("state 1 sub 7 (LIEGEND-Spawn)",     slot, 1, 7, 0, g0, 0);
            passC_one("state 1 sub 8 (FRESSER-Spawn)",     slot, 1, 8, 0, g0, 0);
            passC_one("state 2 sub 0 (HURT)",              slot, 2, 0, 0, g0, 0);
            passC_one("state 2 sub 5 (Niederschlag)",      slot, 2, 5, 0, g0, 0);
            passC_one("state 3 (DEATH)",                   slot, 3, 0, 0, g0, 0);
            passC_one("state 7 (CORPSE)",                  slot, 7, 0, 0, g0, 0);
            passC_one("state 8",                           slot, 8, 0, 0, g0, 0);
            passC_one("state 1 sub 0x12 SITZ + re15_pose",     slot, 1, 0x12, 1, g0, 1);
            passC_one("state 1 sub 0x0d AUFSTEH + re15_pose",  slot, 1, 0x0d, 0, g0, 1);
            passC_one("grid |0x20 (Per-Entity-Skip +0x9)",  slot, 1, 1, 0, g0 | 0x20, 0);
            g_actors[slot].grid_id = g0;
        }
    }
    if (load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x20, 3, 0);
        if (slot > 0) {
            printf("  HUND 0x20:\n");
            uint8_t g0 = g_actors[slot].grid_id;
            passC_one("state 1 sub 0",                     slot, 1, 0, 0, g0, 0);
            passC_one("state 2 (HURT)",                    slot, 2, 0, 0, g0, 0);
            passC_one("state 3 (DEATH)",                   slot, 3, 0, 0, g0, 0);
            passC_one("state 4 sub 0 (SKRIPT-Drop RE1.5)", slot, 4, 0, 0, g0, 0);
            passC_one("state 4 sub 1 (Fenster-Hund)",      slot, 4, 1, 0, g0, 0);
            passC_one("state 5 (Kill-Maschine RE1.5)",     slot, 5, 0, 0, g0, 0);
            passC_one("state 6 (Kill-Maschine RE1.5)",     slot, 6, 0, 0, g0, 0);
            passC_one("state 7 (CORPSE)",                  slot, 7, 0, 0, g0, 0);
            passC_one("grid |0x20 (Per-Entity-Skip)",      slot, 1, 0, 0, g0 | 0x20, 0);
            g_actors[slot].grid_id = g0;
        }
    }
    if (load_room("STAGE1/ROOM10C0.RDT", 0x10c0, -1)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x21, 3, 0);
        if (slot > 0) {
            printf("  KRAEHE 0x21:\n");
            uint8_t g0 = g_actors[slot].grid_id;
            for (int st = 0; st <= 8; st++) {
                char t[64]; snprintf(t, sizeof t, "state %d", st);
                passC_one(t, slot, st, 0, 0, g0, 0);
            }
            passC_one("grid |0x20 (Per-Entity-Skip)", slot, 1, 0, 0, g0 | 0x20, 0);
            g_actors[slot].grid_id = g0;
        }
    }
    if (load_room("STAGE1/ROOM1090.RDT", 0x1090, -1)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x26, 3, 1);
        if (slot > 0) {
            printf("  BABY-SPINNE 0x26 (re2s_baby_spawned=1):\n");
            uint8_t g0 = g_actors[slot].grid_id;
            for (int st = 0; st <= 4; st++) {
                char t[64]; snprintf(t, sizeof t, "state %d", st);
                passC_one(t, slot, st, 0, 0, g0, 0);
            }
            for (int s1 = 0; s1 <= 4; s1++) {
                char t[64]; snprintf(t, sizeof t, "state 1 sub %d", s1);
                passC_one(t, slot, 1, s1, 0, g0, 0);
            }
            printf("      --- und OHNE re2s_baby_spawned (RE1.5-Feuer-Semantik) ---\n");
            g_actors[slot].re2s_baby_spawned = 0;
            passC_one("state 1 sub 0, re2s_baby_spawned=0", slot, 1, 0, 0, g0, 0);
            g_actors[slot].re2s_baby_spawned = 1;
            g_actors[slot].grid_id = g0;
        }
    }
    if (spider_room && load_room(spider_room, spider_id, -1)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x25, 3, 0);
        if (slot > 0) {
            printf("  ADULT-SPINNE 0x25 (%s):\n", spider_room);
            uint8_t g0 = g_actors[slot].grid_id;
            for (int st = 0; st <= 8; st++) {
                char t[64]; snprintf(t, sizeof t, "state %d", st);
                passC_one(t, slot, st, 0, 0, g0, 0);
            }
            passC_one("grid |0x20 (Per-Entity-Skip)", slot, 1, 0, 0, g0 | 0x20, 0);
            g_actors[slot].grid_id = g0;
        }
    }

    if (zg_room && load_room(zg_room, zg_id, zg_fire)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x16, 3, 0);
        if (slot > 0) {
            printf("  ZGIRL 0x16 (%s):\n", zg_room);
            uint8_t g0 = g_actors[slot].grid_id;
            passC_one("state 1 sub 0",                     slot, 1, 0, 0, g0, 0);
            passC_one("state 1 sub 1 (WALK)",              slot, 1, 1, 0, g0, 0);
            passC_one("state 1 sub 0x0b (KRIECHER)",       slot, 1, 0x0b, 0, g0, 0);
            passC_one("state 2 (HURT)",                    slot, 2, 0, 0, g0, 0);
            passC_one("state 3 (DEATH)",                   slot, 3, 0, 0, g0, 0);
            passC_one("state 7 (CORPSE)",                  slot, 7, 0, 0, g0, 0);
            passC_one("state 1 sub 0x12 SITZ + re15_pose",     slot, 1, 0x12, 1, g0, 1);
            passC_one("state 1 sub 0x0d AUFSTEH + re15_pose",  slot, 1, 0x0d, 0, g0, 1);
            passC_one("grid |0x20 (Per-Entity-Skip)",      slot, 1, 1, 0, g0 | 0x20, 0);
            g_actors[slot].grid_id = g0;
        }
    }

    printf("\n################ PASS D - GLOBALE GATES #################################\n");
    {
        static const struct { const char *name; uint8_t type; const char *room; int id; int fs;
                              int baby; } d[] = {
            { "ZOMBIE 0x10", 0x10, "STAGE1/ROOM1140.RDT", 0x1140, -1, 0 },
            { "KRAEHE 0x21", 0x21, "STAGE1/ROOM10C0.RDT", 0x10c0, -1, 0 },
            { "HUND   0x20", 0x20, "STAGE1/ROOM1190.RDT", 0x1190, 13, 0 },
            { "BABY   0x26", 0x26, "STAGE1/ROOM1090.RDT", 0x1090, -1, 1 },
        };
        for (unsigned i = 0; i < sizeof d / sizeof d[0]; i++) {
            if (!load_room(d[i].room, d[i].id, d[i].fs)) continue;
            int slot = setup_target(RE15_AI_FLAVOR_RE2, d[i].type, 3, d[i].baby);
            if (slot <= 0) { printf("  [%s] kein Gegner\n", d[i].name); continue; }
            re15_actor_t *e = &g_actors[slot];
            printf("  %s:\n", d[i].name);
            /* (a) GLOBALER AI-FREEZE (g_re15_pauseflags & RE15_PAUSE_AI) — der Sammel-Gate vor
             *     re15_enemy_ai_run_all (game_step_common.c), Zwilling der 164 Root-Gates
             *     `lw g_pauseflags / lui 0x2000 / and / bne` (Zombie-Root @0x8010042c-3c). */
            g_re15_pauseflags |= RE15_PAUSE_AI;
            passC_one("(a) globaler AI-Freeze RE15_PAUSE_AI", slot, 1, 0, 0, -1, 0);
            g_re15_pauseflags &= (uint32_t)~RE15_PAUSE_AI;
            passC_one("(b) ohne Freeze (Gegenprobe)",         slot, 1, 0, 0, -1, 0);
            /* (c) re15_enemy_ai_set_paused (s_ai_paused) — das per-Brain-Pause-Gate. */
            re15_enemy_ai_set_paused(1);
            passC_one("(c) s_ai_paused = 1",                  slot, 1, 0, 0, -1, 0);
            re15_enemy_ai_set_paused(0);
        }
    }

    printf("\n################ PASS E - IST DER TOTE ZUSTAND UEBERHAUPT BESCHIESSBAR? ###\n");
    /* Der Hund in state 4 sub 0 sub2 0 (grid 0x40, wartet auf die SCD-Marke 0x43) tickt seinen
     * Root NICHT (PASS C) und traegt dort NOCH KEIN +0x93 |= 3 (das setzt erst der Sprung,
     * @0x801113f8). Frage: kann der Spieler ihn in diesem Zustand treffen? Nur dann ist der
     * fehlende Abbau ein erreichbares Loch. */
    if (load_room("STAGE1/ROOM1190.RDT", 0x1190, 13)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x20, 3, 0);
        if (slot > 0) {
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            e->grid_id = 0x40;                 /* Skript-Marke NICHT gesetzt -> bleibt wartend */
            e->state = 4; e->sub_state_1 = 0; e->sub_state_2 = 0; e->sub_state_3 = 0;
            e->hit_react = 0; e->hp = 9000; e->re2z_self1d3 = 0;
            aim_up(slot, 2000);
            e->grid_id = 0x40;
            e->state = 4; e->sub_state_1 = 0; e->sub_state_2 = 0;
            e->hit_react = 0; e->hp = 9000; e->re2z_self1d3 = 0;
            int hits = 0, hp_last = e->hp, max1d3 = 0;
            for (int f = 0; f < 300; f++) {
                pl->hp = 100; track(slot, 2000);
                if (e->state != 4) { e->state = 4; e->sub_state_1 = 0; e->sub_state_2 = 0; }
                e->grid_id = 0x40;
                frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
                      (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
                if (e->hp < hp_last) hits++;
                hp_last = e->hp;
                if ((e->re2z_self1d3 & 0x7f) > max1d3) max1d3 = e->re2z_self1d3 & 0x7f;
            }
            printf("  HUND state 4 sub 0 (wartend): %d Treffer in 300 Bildern, +0x1D3 low7 "
                   "hoechstens %d, am Ende %d, +0x93=0x%02X, Band=%d\n",
                   hits, max1d3, e->re2z_self1d3 & 0x7f, e->hit_react, (int)e->aim_band);
            if (hits > 0 && (e->re2z_self1d3 & 0x7f) != 0)
                printf("  => ERREICHBARES LOCH: er ist in diesem Zustand TREFFBAR und die Pause "
                       "bleibt stehen.\n");
            else if (hits == 0)
                printf("  => nicht treffbar in diesem Zustand -> das tote Dekrement ist hier "
                       "folgenlos.\n");
        }
    }
    /* Dasselbe fuer den Zombie mit +0x9 & 0x20 (der ROOM1200-Fall aus enemy_ai_re2_zombie.c). */
    if (load_room("STAGE1/ROOM1140.RDT", 0x1140, -1)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x10, 3, 0);
        if (slot > 0) {
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            aim_up(slot, 2000);
            e->hp = 9000; e->hit_react = 0; e->re2z_self1d3 = 0;
            uint8_t g = (uint8_t)(e->grid_id | 0x20);
            int hits = 0, hp_last = e->hp, max1d3 = 0;
            for (int f = 0; f < 300; f++) {
                pl->hp = 100; track(slot, 2000);
                e->grid_id = g;                       /* Per-Entity-Skip +0x9 & 0x20 halten */
                frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
                      (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
                if (e->hp < hp_last) hits++;
                hp_last = e->hp;
                if ((e->re2z_self1d3 & 0x7f) > max1d3) max1d3 = e->re2z_self1d3 & 0x7f;
            }
            printf("  ZOMBIE mit grid&0x20 (Per-Entity-Skip): %d Treffer in 300 Bildern, "
                   "+0x1D3 low7 hoechstens %d, am Ende %d, +0x93=0x%02X, st=%d/%d\n",
                   hits, max1d3, e->re2z_self1d3 & 0x7f, e->hit_react, e->state, e->sub_state_1);
            if (hits > 0 && (e->re2z_self1d3 & 0x7f) != 0)
                printf("  => ERREICHBARES LOCH: treffbar, Pause bleibt stehen.\n");
        }
    }

    /* Und der dritte Kandidat: der RE1.5-FEUER-EMITTER Typ 0x26 in ROOM1090 (re2s_baby_spawned=0).
     * re15_ai_re2_for_type() liefert im RE2-Flavor fuer JEDEN Typ 1, der Treffer-Stempel greift
     * also auch hier (s_re2_stun_spider[w] = 15) - sein Brain ist aber das RE1.5-Feuer
     * (re15_spider_ai_tick), das +0x1D3 nie abzieht. */
    if (load_room("STAGE1/ROOM1090.RDT", 0x1090, -1)) {
        int slot = setup_target(RE15_AI_FLAVOR_RE2, 0x26, 3, 0);   /* baby_force = 0 ! */
        if (slot > 0) {
            re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
            re15_actor_t *e  = &g_actors[slot];
            printf("  FEUER-EMITTER 0x26 (re2s_baby_spawned=%d):\n", e->re2s_baby_spawned);
            aim_up(slot, 2000);
            e->hp = 9000; e->hit_react = 0; e->re2z_self1d3 = 0;
            int hits = 0, hp_last = e->hp, max1d3 = 0;
            for (int f = 0; f < 300; f++) {
                pl->hp = 100; track(slot, 2000);
                frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
                      (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
                if (e->hp < hp_last) hits++;
                hp_last = e->hp;
                if ((e->re2z_self1d3 & 0x7f) > max1d3) max1d3 = e->re2z_self1d3 & 0x7f;
            }
            printf("    %d Treffer in 300 Bildern, +0x1D3 low7 hoechstens %d, am Ende %d, "
                   "+0x93=0x%02X, st=%d/%d\n", hits, max1d3, e->re2z_self1d3 & 0x7f,
                   e->hit_react, e->state, e->sub_state_1);
        }
    }

    printf("\nMESSSCHIENE DURCHGELAUFEN\n");
    if (s_keep) free(s_keep);
    return 0;
}
