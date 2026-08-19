/* probe_re2_hit_crow_spider.c — MESSUNG: der ECHTE Schuss-Weg auf KRAEHE (ROOM10C0) und
 * SPINNE (ROOM1090), je Flavor und je Waffe.
 *
 * Warum diese Sonde: probe_re2_hitpath misst Zombie (ROOM1140) und Hund (ROOM1190) und traf
 * Kraehe/Spinne 0-mal — der Vorgaenger nannte das "Grenze des Messrahmens". Genau die wird hier
 * repariert statt umgangen:
 *   (a) ECHTE Raeume: die Kraehe spawnt in ROOM10C0 (3x Sce_em_set type 0x21), die Spinne in
 *       ROOM1090 (type 0x25) — dieselben SCD-Spawns wie im Spiel.
 *   (b) ELEVATIONS-BAND: die fliegende Kraehe traegt ihr Band in aim_band (re15_damage.c:641-649).
 *       Der Spieler drueckt deshalb JEDEN Frame das D-Pad hoch/runter passend zum Band — genau
 *       das, was ein Spieler tut, wenn er auf eine fliegende Kraehe zielt (Aim-FSM
 *       player_common.c:489-495, `acaec` @0x80035164/1b8/204).
 *   (c) NACHFUEHREN: der Spieler dreht sich jeden Frame zum Ziel (rot_y) und haelt den Abstand —
 *       Tank-Drehung im Zeitraffer. Der SCHUSS-WEG bleibt echt: re15_game_step mit Pad
 *       R1 (+UP/DOWN) -> SQUARE, Aim-FSM, re15_player_fire_start, re15_player_weapon_fire.
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
static int                s_room_id = 0x10c0;
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

static int find_type(uint8_t type)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == type) return s;
    return -1;
}

/* Der Spieler dreht sich zum Ziel und haelt den Abstand (Tank-Drehung/Nachlaufen im
 * Zeitraffer). Y bleibt auf der Bodenebene des Ziels — die Hoehe regelt das Band. */
static void track(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    int32_t dx = e->x - pl->x, dz = e->z - pl->z;
    int64_t q  = (int64_t)dx*dx + (int64_t)dz*dz;
    double  d  = q > 0 ? __builtin_sqrt((double)q) : 0.0;
    if (d > 1.0) {
        /* auf `back` Einheiten heranfuehren, Richtung beibehalten */
        pl->x = e->x - (int32_t)((double)dx / d * back);
        pl->z = e->z - (int32_t)((double)dz / d * back);
    } else { pl->x = e->x - back; pl->z = e->z; }
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Welches D-Pad braucht der Spieler, damit das Band des Ziels passt?
 * player-band: elev>0 -> 0x80000000 UP, elev<0 -> 0x20000000 DOWN, 0 -> 0x40000000 LEVEL. */
static uint16_t elev_pad_for(const re15_actor_t *e, const re15_actor_t *pl)
{
    if (e->type == 0x21) {
        if (e->aim_band == 4) return RE15_PAD_BIT_UP;
        if (e->aim_band == 1) return RE15_PAD_BIT_DOWN;
        return 0;                                   /* Band 2 = LEVEL; Band 0 = kein Ziel */
    }
    if (e->grid_id & 0x80) {
        int32_t dx = e->x - pl->x, dz = e->z - pl->z;
        (void)dx; (void)dz;
        return RE15_PAD_BIT_DOWN;                   /* DOWNED -> DOWN-Band (dist<5000) */
    }
    return 0;                                       /* LEVEL */
}

typedef struct { int hits; int killed; int last_react; int state; int slot; } res_t;

/* wait_air > 0: erst feuern, wenn die Kraehe wirklich FLIEGT (y <= -wait_air unter dem
 * Spieler) — das ist der Fall, den der Vorgaenger als "Grenze des Messrahmens" gemeldet hat. */
static void run_ex(const char *tag, int flavor, uint8_t type, int weapon, int budget,
                   int verbose, int wait_air, res_t *out);
static void run(const char *tag, int flavor, uint8_t type, int weapon, int budget,
                int verbose, res_t *out)
{ run_ex(tag, flavor, type, weapon, budget, verbose, 0, out); }

static void run_ex(const char *tag, int flavor, uint8_t type, int weapon, int budget,
                   int verbose, int wait_air, res_t *out)
{
    memset(out, 0, sizeof *out);
    printf("\n===== %s (%s, Waffe %d, Raum %04X) =====\n", tag,
           flavor == RE15_AI_FLAVOR_RE2 ? "RE2" : "RE1.5", weapon, s_room_id);
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    /* AI hochlaufen lassen (Spieler weit weg + unsterblich) */
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = find_type(type);
    if (slot < 0) { printf("  KEIN GEGNER Typ 0x%02X\n", type); out->slot = -1; return; }
    out->slot = slot;
    /* ARENA: nur DIESER Gegner bleibt aktiv (sonst rastet die Auto-Aim-Latch woanders ein). */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;

    printf("  Ziel slot=%d typ=0x%02X st=%d/%d/%d hp=%d grid=0x%02X r=%d band=%d pos(%d,%d,%d)\n",
           slot, e->type, e->state, e->sub_state_1, e->sub_state_2, e->hp, e->grid_id,
           (int)e->hit_radius_min, (int)e->aim_band, (int)e->x, (int)e->y, (int)e->z);

    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);

    /* R1 halten bis die Waffe oben ist (mit dem passenden Elevations-Pad) */
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e, pl)), 0);
    }
    printf("  aim_ready=%d elev=%d\n", re15_player_aim_ready(), re15_player_aim_elevation());

    if (wait_air > 0) {   /* warten, bis die Kraehe oben ist (nur R1 halten, NICHT feuern).
                           * Naeher herangehen: der RE2-Wake-Gate ist dist < 0x709 = 1801
                           * (`sltiu 0x709` @0x80100704) — aus 2000 hebt sie NIE ab. */
        int w = 0;
        for (; w < 900 && (pl->y - e->y) < wait_air; w++) {
            pl->hp = 100; track(slot, 1500);
            frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e, pl)), 0);
        }
        printf("  Flug abgewartet: %d Frames, y=%d (vert=%d), band=%d, st=%d/%d/%d\n",
               w, (int)e->y, (int)(pl->y - e->y), (int)e->aim_band, e->state,
               e->sub_state_1, e->sub_state_2);
    }

    int hp0 = e->hp, hp_last = e->hp;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; track(slot, back);
        uint16_t ep = elev_pad_for(e, pl);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | ep),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) {
            out->hits++;
            if (verbose || out->hits <= 4)
                printf("   TREFFER #%d Frame %d: hp %d->%d, +0x93=0x%02X, st=%d/%d/%d band=%d "
                       "elev=%d\n", out->hits, f, hp_last, e->hp, e->hit_react, e->state,
                       e->sub_state_1, e->sub_state_2, (int)e->aim_band,
                       re15_player_aim_elevation());
        }
        hp_last = e->hp;
        if (e->hp < 0) { out->killed = 1; break; }
    }
    out->last_react = e->hit_react; out->state = e->state;
    printf("  ERGEBNIS: %d TREFFER, hp %d -> %d, state=%d/%d/%d, +0x93=0x%02X, tot=%d\n",
           out->hits, hp0, e->hp, e->state, e->sub_state_1, e->sub_state_2,
           e->hit_react, out->killed);
}

static int load_room(const char *sub, int room_id)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/%s", (base && *base) ? base : RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 0; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse %s\n", sub); return 0; }
    s_room_id = room_id;
    return 1;
}

int main(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    res_t r;

    if (load_room("STAGE1/ROOM10C0.RDT", 0x10c0)) {
        run("KRAEHE RE1.5 / Pistole", RE15_AI_FLAVOR_RE15, 0x21, 3, 900, 0, &r);
        run("KRAEHE RE2   / Pistole", RE15_AI_FLAVOR_RE2,  0x21, 3, 900, 0, &r);
        run("KRAEHE RE1.5 / Messer",  RE15_AI_FLAVOR_RE15, 0x21, 1, 900, 0, &r);
        run("KRAEHE RE2   / Messer",  RE15_AI_FLAVOR_RE2,  0x21, 1, 900, 0, &r);
        /* Der Fall, an dem der Vorgaenger gescheitert ist: die Kraehe ist IN DER LUFT. */
        run_ex("KRAEHE RE2   / Pistole / IN DER LUFT", RE15_AI_FLAVOR_RE2, 0x21, 3, 900, 0,
               1200, &r);
        run_ex("KRAEHE RE1.5 / Pistole / IN DER LUFT", RE15_AI_FLAVOR_RE15, 0x21, 3, 900, 0,
               1200, &r);
    }
    /* Die SPINNE steht NICHT in ROOM1090 (gemessen: 0 Aktoren vom Typ 0x25) — der
     * RE1.5-Adult-Spider ist ein STAGE2-Gegner. Dieselbe Raum-Suche wie
     * test_re2_room1090_ab.c: den ERSTEN Raum nehmen, dessen SCD wirklich eine 0x25 aufstellt. */
    static const struct { const char *sub; int id; } cand[] = {
        { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
        { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
        { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
        { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
        { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 },
    };
    int picked = -1;
    for (unsigned i = 0; i < sizeof cand / sizeof cand[0] && picked < 0; i++) {
        if (!load_room(cand[i].sub, cand[i].id)) continue;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        bringup();
        for (int f = 0; f < 30; f++) frame(0, 0);
        int n25 = 0, n26 = 0;
        for (int s = 1; s < RE15_ACTOR_MAX; s++) {
            if (!g_actors[s].active) continue;
            if (g_actors[s].type == 0x25) n25++;
            if (g_actors[s].type == 0x26) n26++;
        }
        printf("  [Spinnen-Suche] %s: 0x25=%d 0x26=%d\n", cand[i].sub, n25, n26);
        if (n25 > 0) picked = (int)i;
    }
    if (picked >= 0) {
        load_room(cand[picked].sub, cand[picked].id);
        printf("\n  >>> Spinnen-Raum: %s\n", cand[picked].sub);
        run("SPINNE RE1.5 / Pistole", RE15_AI_FLAVOR_RE15, 0x25, 3, 900, 0, &r);
        run("SPINNE RE2   / Pistole", RE15_AI_FLAVOR_RE2,  0x25, 3, 900, 0, &r);
        run("SPINNE RE1.5 / Messer",  RE15_AI_FLAVOR_RE15, 0x25, 1, 900, 0, &r);
        run("SPINNE RE2   / Messer",  RE15_AI_FLAVOR_RE2,  0x25, 1, 900, 0, &r);
    } else printf("\n  KEIN Raum mit einer lebenden Typ-0x25-Spinne gefunden\n");
    return 0;
}
