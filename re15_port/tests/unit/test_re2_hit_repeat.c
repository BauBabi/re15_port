/**
 * @file test_re2_hit_repeat.c
 * @brief PIN — "Spieler zielt und feuert in ROOM1140 -> Gegner-HP sinkt", WIEDERHOLT.
 *
 * Nutzer-Blocker 2026-08-19 (nach v0.3.0): "Bei der RE2-AI kann ich immer noch keinen Zombie
 * treffen. Weder mit Schusswaffe noch mit Messer. Getestet im Dinner Room."
 *
 * WARUM DIE BISHERIGEN TESTS DAS NICHT SAHEN: sie riefen re15_player_weapon_fire() EINMAL,
 * synthetisch, auf einen frisch gesetzten Gegner — und genau EIN Treffer ging auch mit dem Bug
 * durch. Dieser Pin faehrt deshalb den ECHTEN Weg (re15_game_step mit Pad R1 + SQUARE, echte
 * ROOM1140-Spawns aus dem raum-eigenen SCD) und misst, wie OFT es trifft.
 *
 * DER BUG (gemessen, probe_re2_hitpath):
 *   RE1.5-Flavor Pistole, Dauerfeuer -> 3 Treffer, +0x93 endet 0x80
 *   RE2  -Flavor Pistole, Dauerfeuer -> 1 Treffer, +0x93 bleibt 0x01  == ab dann UNTREFFBAR
 * Ursache: der flavor-blinde Resolver FUN_80011f50 setzt den Ein-Treffer-Latch +0x93 Bit 0
 * (@0x800124e8/@0x800124f0) und ueberspringt jeden Kandidaten mit Bits 0|1 (Maske 0x03000000
 * @0x800120c0, Test @0x800120f4-0x80012100, Zweitkontakt+Rekursion @0x800123fc-0x80012418).
 * Freigeber ist in RE1.5 der Treffer-Reaktions-Handler (@0x80105f9c-fac `lbu 147 / andi 0xfe /
 * sb 147`, ebenso @0x80106b8c-90 / @0x80106a1c-20 / @0x80103b5c-68). Der RE2-Brain hat den
 * Freigeber nicht — das RE2-Overlay kennt +0x93 gar nicht (eigener Voll-Scan nach
 * `sb/lbu rt,147(rs)`: EMOVL10_S0.BIN 0 Treffer, info/re2leon/PSX.EXE 0 Treffer).
 * Der FLINCH-Pfad (0x501 @0x801050A4 -> Knockdown-Executor) laeuft an ALLEN Reaktions-Handlern
 * vorbei; seine Freigabe sitzt deshalb im Executor-Ausgang P8 @0x801036F4 — dem Zwilling des
 * RE1.5-Knockdown-Ausgangs case 6, der `grid_id &= 0x7f` UND `hit_react &= ~1` ZUSAMMEN macht
 * (@0x80106b8c-90 / @0x80106a1c-20). Der grid-Teil stand im Port bereits (Review #18), der
 * +0x93-Teil fehlte.
 *
 * DIE PINS (alle vier Flavor/Waffen-Kombinationen, damit eine Regression auf BEIDEN Seiten
 * sofort auffaellt):
 *   (1) RE2   + Pistole : >= 3 getrennte Schadens-Ereignisse
 *   (2) RE2   + Messer  : >= 3 getrennte Schadens-Ereignisse UND der Zombie stirbt
 *   (3) RE1.5 + Pistole : >= 3 (Regressions-Wache fuer den RE1.5-Modus)
 *   (4) RE1.5 + Messer  : >= 3 (dito)
 *   (5) RE2   : ein niedergeschlagener Zombie verliert das DOWNED-Bit wieder (sonst bleibt er
 *               fuer einen LEVEL zielenden Spieler dauerhaft untreffbar)
 *   (6) RE2/RE1.5 + HUND in ROOM1190 (Hunde-Welle in sub13): derselbe Defekt steckte auch im
 *               RE2-Cerberus-Brain — gemessen RE1.5 8 / RE2 1 Treffer, +0x93 endete dort 0x03.
 *               RE1.5-Zwilling der Freigabe: `+0x93 = 0` @0x80110b70 / @0x80110d90.
 * NEGATIV-PROBE (gemessen 2026-08-19): macht man `re2z_hit_latch_release` zum No-op, faellt
 * RE2+Pistole von 20 auf 1 Treffer und RE2+Messer von 16 auf 1 — (1)/(2a)/(2b) werden ROT,
 * (3)/(4) bleiben unveraendert gruen (7 bzw. 5 Treffer).
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

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

static re15_rdt_t         s_rdt;
static int                s_room_id  = 0x1140;
static int                s_fire_sub = -1;      /* zusaetzlicher Spawn-Thread (Hunde-Welle) */
static uint8_t            s_want_type = 0;      /* 0 = Zombie-Familie 0x10..0x18 */
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

/* Ein Frame in der main.c-Reihenfolge (msg-Tick, dann der geteilte Interpreter-Step). */
static void frame(uint16_t cur, uint16_t edge)
{
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
    s_ctx.pad_current = cur; s_ctx.pad_pressed = edge;
    re15_game_step(&s_ctx);
}

/* ⛔ ABSTAND NACHFUEHREN (2026-08-19): der RE2-Stagger stoesst den Zombie um -450 Einheiten
 * zurueck (`re2z_thrust(e, -450)`, `sh v0,324` @0x80105C88), der Knockdown noch weiter. Ohne
 * Nachlaufen faellt er aus der Messer-Reichweite (1100 + Radius) und dieser Pin misst
 * REICHWEITE statt der Trefferkette. Position/Rotation zu setzen ist Harness-Hilfe, der
 * Schuss-Weg (Aim-FSM + game_step + weapon_fire) bleibt echt — gleiches Verfahren wie
 * probe_re2_hit_crow_spider.c track(). */
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);
static void hr_track(int slot, int32_t back)
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

/* Der Raum faehrt aus seinem EIGENEN SCD hoch — dieselben Sce_em_set-Spawns wie im Spiel. */
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
    re15_player_set_aim_clip_len(12);          /* Mock-W-Bank: alle Aim-Clips 12 Frames */
}

/* Ein STEHENDER Zombie (die Liege-Spawns sind byte-true ueber das Elevations-Band
 * ausgeschlossen und wuerden die Messung verfaelschen). */
static int standing_zombie(void)
{
    if (s_want_type) {
        for (int s = 1; s < RE15_ACTOR_MAX; s++)
            if (g_actors[s].active && g_actors[s].type == s_want_type) return s;
        return -1;
    }
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

typedef struct { int hits; int killed; int downed_seen; int downed_cleared; int slot; } result_t;

/* Der ECHTE Weg: hinstellen -> R1 halten bis die Waffe oben ist -> SQUARE halten. */
static void run(const char *tag, re15_ai_flavor_t flavor, int weapon, int budget, result_t *out)
{
    memset(out, 0, sizeof *out);
    re15_ai_flavor_set(flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    /* Munition auffuellen: der Pin misst die TREFFERKETTE, nicht die 15-Schuss-Kapazitaet. */
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250;
    }

    /* AI hochlaufen lassen (Spieler weit weg + unsterblich), DANN das Ziel waehlen: erst nach
     * dem INIT traegt grid_id die laufende Semantik statt des Spawn-Verhaltensbytes. */
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = standing_zombie();
    if (slot < 0) { printf("FAIL: %s — kein passender Gegner im Raum %04X\n", tag, s_room_id);
                    fails++; return; }
    out->slot = slot;

    /* ARENA: nur dieser Gegner bleibt aktiv, sonst rastet die Auto-Aim-Latch auf einem anderen
     * ein und der Test misst Geometrie statt der Trefferkette. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - ((weapon < 3) ? 1200 : 2600);   /* Messer braucht Naehe (reach 1100 + r 400) */
    pl->z = e->z; pl->y = e->y; pl->rot_y = 1024;  /* rot_y 1024 = Blick +X = zum Zombie */
    pl->hp = 100;
    (void)hr_track;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);

    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; frame(RE15_PAD_BIT_R1, 0);
    }

    /* ⛔ HARNESS-HILFE (2026-08-19, vollstaendiges RE2-Schadensmodell): das Messer macht in RE2
     * nur noch 3 Schaden (Zeile 0x800A412C Zeile 1 Zone 0 = 3; RE1.5 @0x8006E650[1] = 6), und
     * der Zombie traegt RE2-HP (0x8010C670, 50..128). Bis zum Tod waeren ~30 Treffer noetig —
     * im Rahmen dieses Pins nicht erreichbar. Punkt (2b) misst NICHT Letalitaet, sondern dass
     * beide Latches (+0x93 Bit 0 und das DOWNED-Bit) wieder freigegeben werden; der Tod ist nur
     * der end-to-end-Beleg dafuer. Der Zombie bekommt deshalb 15 HP = 5 Messertreffer, genau
     * wie der Spieler oben unsterblich gehalten wird. Die byte-true RE2-Zahlen selbst pinnt
     * test_re2_hp_model (Schadenszeile + HP-Tabelle je Typ). */
    if (flavor == RE15_AI_FLAVOR_RE2 && weapon == 1 && re15_re2_damage_model()) e->hp = 15;

    int hp_last = e->hp;
    uint8_t grid_last = e->grid_id;
    const int32_t back = (weapon < 3) ? 1200 : 2600;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        hr_track(slot, back);   /* Rueckstoss -450 @0x80105C88 nachlaufen (s. Helfer) */
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        if (e->hp < hp_last) out->hits++;
        hp_last = e->hp;
        if ((e->grid_id & 0x80) && !(grid_last & 0x80)) out->downed_seen = 1;
        if (out->downed_seen && !(e->grid_id & 0x80))   out->downed_cleared = 1;
        grid_last = e->grid_id;
        if (e->hp < 0) { out->killed = 1; break; }
    }
    printf("  [%s] %d Treffer, hp %d, state %d, +0x93=0x%02X, grid=0x%02X, "
           "downed gesehen/geloest=%d/%d\n",
           tag, out->hits, e->hp, e->state, e->hit_react, e->grid_id,
           out->downed_seen, out->downed_cleared);
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("=== ROOM1140 (Dinner Room): zielen + feuern auf dem ECHTEN Weg ===\n");
    result_t r2g, r2k, r15g, r15k;
    run("RE2   Pistole", RE15_AI_FLAVOR_RE2,  3, 900, &r2g);
    run("RE2   Messer",  RE15_AI_FLAVOR_RE2,  1, 900, &r2k);   /* HP-Klemme s. run() */
    run("RE1.5 Pistole", RE15_AI_FLAVOR_RE15, 3, 900, &r15g);
    run("RE1.5 Messer",  RE15_AI_FLAVOR_RE15, 1, 900, &r15k);

    CHECK(r2g.hits >= 3,
          "(1) RE2 + Pistole: %d Treffer (>=3 gefordert) — mit dem +0x93-Latch-Bug war es genau 1 "
          "(Freigabe @0x80105f9c-fac, Latch @0x800124e8/@0x800120f4-0x80012100)", r2g.hits);
    CHECK(r2k.hits >= 3,
          "(2a) RE2 + Messer: %d Treffer (>=3 gefordert)", r2k.hits);
    CHECK(r2k.killed,
          "(2b) RE2 + Messer: der Zombie stirbt (hp<0) — end-to-end-Beweis, dass beide Latches "
          "(+0x93 Bit 0 und das DOWNED-Bit grid 0x80) wieder freigegeben werden");
    CHECK(r15g.hits >= 3,
          "(3) RE1.5 + Pistole: %d Treffer (>=3) — Regressions-Wache fuer den RE1.5-Modus",
          r15g.hits);
    CHECK(r15k.hits >= 3,
          "(4) RE1.5 + Messer: %d Treffer (>=3) — Regressions-Wache", r15k.hits);
    /* (6) DIESELBE Trefferkette beim RE2-HUND (ROOM1190, Hunde-Welle in sub13). GEMESSEN vor
     *     dem Fix: RE1.5 8 Treffer / RE2 1 Treffer mit +0x93 = 0x03 (beide Latch-Bits). */
    {
        snprintf(path, sizeof path, "%s/STAGE1/ROOM1190.RDT",
                 (base && *base) ? base : RE15_ASSET_PSX_DIR);
        size_t dsz = 0; uint8_t *dbuf = slurp(path, &dsz);
        if (!dbuf || re15_rdt_parse(dbuf, dsz, &s_rdt) != 0) {
            printf("FAIL: ROOM1190.RDT nicht lesbar/parsebar\n"); fails++;
        } else {
            s_room_id = 0x1190; s_fire_sub = 13; s_want_type = 0x20;
            result_t d2, d15;
            run("RE2   HUND",  RE15_AI_FLAVOR_RE2,  3, 900, &d2);
            run("RE1.5 HUND",  RE15_AI_FLAVOR_RE15, 3, 900, &d15);
            CHECK(d2.hits >= 3,
                  "(6a) RE2 + Hund: %d Treffer (>=3) — vor dem Fix genau 1, +0x93 blieb 0x03 "
                  "(RE1.5-Zwilling `+0x93 = 0` @0x80110b70 / @0x80110d90)", d2.hits);
            CHECK(d15.hits >= 3,
                  "(6b) RE1.5 + Hund: %d Treffer (>=3) — Regressions-Wache", d15.hits);
            free(dbuf);
        }
    }

    CHECK(!r2k.downed_seen || r2k.downed_cleared,
          "(5) RE2: das DOWNED-Bit (grid 0x80, gesetzt im Knockdown-Executor P0) wird beim "
          "Aufstehen wieder geloescht (@0x801022b8-bc) — sonst bleibt der Zombie fuer einen "
          "LEVEL zielenden Spieler dauerhaft untreffbar (gesehen=%d, geloest=%d)",
          r2k.downed_seen, r2k.downed_cleared);

    free(buf);
    printf(fails ? "\nFEHLGESCHLAGEN: %d\n" : "\nALLE PINS GRUEN (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
