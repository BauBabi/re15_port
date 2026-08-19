/**
 * @file test_re2_hit_repeat_crow_spider.c
 * @brief PIN — "zielen und feuern muss WIEDERHOLT treffen", KRAEHE und SPINNE.
 *
 * Schwester von test_re2_hit_repeat.c (Zombie ROOM1140 / Hund ROOM1190). Der Fix eb841053 hat
 * die Resolver-Latch-Freigabe +0x93 Bit 0 auch bei Kraehe und Spinne eingebaut, sie dort aber
 * NICHT gemessen ("in der Arena-Sonde treffen beide Flavors 0-mal"). Dieser Pin misst sie in
 * ihren ECHTEN Raeumen.
 *
 * ===== DER MECHANISMUS (unveraendert aus dem Zombie-/Hunde-Fix) ==============================
 * Der flavor-blinde Resolver FUN_80011f50 (RE1.5-EXE, laeuft in BEIDEN Flavors) setzt beim
 * Treffer den Ein-Treffer-Latch +0x93 |= 1 (`lbu v0,147(s1)` @0x800124e8 / `ori v0,v0,0x1`
 * @0x800124f0), schliesst Kandidaten mit +0x93-Bits 0|1 aus der Kandidatenschleife aus
 * (Maske 0x03000000 `lui s4,0x300` @0x800120c0, `lw v0,144(s0)` / `and v0,v0,s4` /
 * `beq v0,s4,0x80012124` @0x800120f4-0x80012100) und setzt beim Zweitkontakt Bit 1 + rekursiert
 * (@0x800123fc-0x80012418). Freigeber ist in RE1.5 IMMER der Treffer-Reaktions-Handler beim
 * Ruecksprung nach ACTIVE: `lbu v0,147(v1)` @0x80105f9c / `andi v0,v0,0xfe` @0x80105fa4 /
 * `sb v0,147(v1)` @0x80105fac.
 * Die RE2-Module kennen +0x93 GAR NICHT — eigener Voll-Scan aller Byte-/Halbwort-/Wort-Zugriffe
 * mit Offset 147 (2026-08-19): EMOVL21_S0.BIN 0, EMOVL21_S1.BIN 0, EMS25.BIN 0, EMS26.BIN 0,
 * EMOVL10_S0.BIN 0. Mit der Uebernahme des Dispatchs durch die RE2-Brains fiel der EINZIGE
 * Freigeber weg -> genau EIN Treffer, danach dauerhaft unverwundbar.
 *
 * ===== WARUM DIE FREIGABE AN DER HURT-WURZEL SITZT (disasm-belegt) ===========================
 * KRAEHE (EMOVL21_S0.BIN, HURT-Wurzel @0x801028BC): der ganze HURT-Baum ruft den Zustands-
 * setzer 0x80104078 GENAU 4x auf — @0x80102C7C, @0x80102ED0, @0x80103258, @0x801034B0 — und
 * ruft den ACTIVE-Handler NIE inline auf (jal-Zensus ueber 0x801028BC..0x801034B8). Von den
 * 4 Stellen fuehrt nur @0x80102C7C ins Leben:
 *     80102c10: jal 0x80015614 (Arc-Test, a3=1024)
 *     80102c20: addiu a1,zero,1 / 80102c28: addiu a2,zero,9   -> ACTIVE Sub 9
 *     80102c44: addiu a1,zero,1 / 80102c78: addu a2,zero,zero -> ACTIVE Sub 0
 *     80102c74: addiu a1,zero,7                               -> CORPSE (P5, tot)
 * Die anderen drei sind CORPSE: @0x80102ED0 (7,1 GIB), @0x80103258 (7,0 Boden-Auslauf; per
 * `j 0x80103258` @0x801031AC auch (7,2) Wand-Splat), @0x801034B0 (7,3 Launch). Es gibt also
 * KEINEN Kraehen-Zwilling des Zombie-Flinch 0x501 @0x801050A4, der die Reaktion an einen
 * ACTIVE-Sub weiterreicht — die Wurzel-Platzierung erfasst jeden lebenden Ausgang.
 *
 * SPINNE (EMS25.BIN, HURT-Wurzel @0x80102C78): die Wurzel schreibt das Zustandswort direkt.
 * Alle Schreibstellen im HURT-Baum (0x80102C78..0x80103C7C, eigener `sw rt,4(rs)`-Scan) landen
 * auf ZUSTAND 1: @0x80102E58 ((tbl<<8)+1, Tabelle @0x80106578), @0x80102ECC + @0x80102FA4 +
 * @0x8010313C + @0x80103474 + @0x80103C54 (Snapshot-Restore, jeweils mit INLINE-ACTIVE-
 * Re-Entry `jal 0x801005AC` im Delay-Slot-Paar), @0x80102FBC + @0x801034F0 (`addiu v0,zero,1025`
 * = 0x401 ACTIVE Sub 4), @0x801030B4 (`addiu v0,zero,2305` @0x80103068 = 0x901 ACTIVE Sub 9),
 * @0x8010335C (0x1). KEIN Schreiber auf Zustand 4 und keiner, der die Reaktion an einen
 * ACTIVE-Sub weiterreicht (ACTIVE-Subs Modus 0 @0x80106450: 0 Idle, 1-3 Lauf, 4 Drehen,
 * 5/6 Pirschen, 7 Angriff, 8 Wechsel, 9 Sprung — kein Niederschlag-Executor). Die 5 Inline-
 * Re-Entries laufen im SELBEN Tick, also VOR der Wurzel-Freigabe.
 *
 * ===== GEMESSEN (probe_re2_hit_crow_spider, echter Weg: re15_game_step + Pad R1(+D-Pad) -> SQUARE)
 *   OHNE Freigabe (Freigabe als No-op)          MIT Freigabe
 *   KRAEHE RE2 Pistole : 1 Treffer, +0x93=0x03  ->  6 Treffer, tot
 *   KRAEHE RE2 Messer  : 1 Treffer, +0x93=0x03  ->  6 Treffer, tot
 *   SPINNE RE2 Pistole : 1 Treffer, +0x93=0x03  -> 10 Treffer, CORPSE
 *   SPINNE RE2 Messer  : 1 Treffer, +0x93=0x03  -> 17 Treffer, CORPSE
 *   RE1.5 unveraendert : Kraehe 1 (stirbt), Spinne 10 / 17
 *
 * ===== WARUM DIE RE1.5-KRAEHE MIT EINEM TREFFER STIRBT (kein Defekt) ========================
 * Der RE1.5-ACTIVE-Tail schreibt die HP jeden Tick NEU: `hp = (vert_err >= 5200) ? -1 : 0`
 * (Tail-Helfer 0x80115F70, jal @0x801125CC, @0x80115F88-9C). Eine erreichbare RE1.5-Kraehe hat
 * also IMMER HP 0 und stirbt am ersten Treffer — der Latch kommt gar nicht zum Tragen. Die
 * RE2-Kraehe hat dagegen HP 10 (@0x80100324/348), braucht 5-6 Pistolentreffer und war damit im
 * RE2-Modus schlicht UNSTERBLICH.
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

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

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

/* Der Spieler dreht sich zum Ziel und haelt den Abstand — Tank-Drehung/Nachlaufen im
 * Zeitraffer. Position/Rotation zu setzen ist Harness-Hilfe; der SCHUSS-WEG bleibt echt
 * (Aim-FSM + re15_game_step + re15_player_weapon_fire). */
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
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Welches D-Pad braucht der Spieler, damit sein Elevations-Band zum Ziel passt? Die fliegende
 * Kraehe traegt ihr Band in aim_band (re15_damage.c, RE1.5-Tail @0x80112560-C8); ein
 * niedergeschlagener Gegner (grid 0x80) liegt im DOWN-Band (@0x80101630-38 -> @0x800129CC-F0).
 * Genau das tut ein Spieler: hoch zielen, wenn die Kraehe fliegt. */
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

typedef struct {
    int hits;            /* getrennte Schadens-Ereignisse                       */
    int killed;          /* hp < 0                                              */
    int corpse;          /* state == 7 erreicht (die Spinne endet mit hp=1)     */
    int released;        /* +0x93 Bit 0 wurde nach einem Treffer wieder frei    */
    uint8_t last_react;
} res_t;

/* wait_air > 0: erst feuern, wenn die Kraehe wirklich FLIEGT (Spieler-Y minus Kraehen-Y >=
 * wait_air). Genau der Fall, den der Vorgaenger als "Grenze des Messrahmens" gemeldet hat.
 * Dafuer muss der Spieler NAEHER heran: der RE2-Wake-Gate ist dist < 0x709 = 1801
 * (`sltiu 0x709` @0x80100704) — aus 2000 Einheiten hebt die Kraehe nie ab. */
static void run_ex(const char *tag, re15_ai_flavor_t flavor, uint8_t type, int weapon,
                   int budget, int wait_air, res_t *out);
static void run(const char *tag, re15_ai_flavor_t flavor, uint8_t type, int weapon,
                int budget, res_t *out)
{ run_ex(tag, flavor, type, weapon, budget, 0, out); }

static void run_ex(const char *tag, re15_ai_flavor_t flavor, uint8_t type, int weapon,
                   int budget, int wait_air, res_t *out)
{
    memset(out, 0, sizeof *out);
    re15_ai_flavor_set(flavor);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(weapon);
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    /* AI hochlaufen lassen (Spieler weit weg + unsterblich), DANN das Ziel waehlen. */
    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }

    int slot = find_type(type);
    if (slot < 0) { printf("FAIL: %s — kein Gegner Typ 0x%02X im Raum %04X\n", tag, type,
                           s_room_id); fails++; return; }
    /* ARENA: nur dieser Gegner bleibt aktiv, sonst rastet die Auto-Aim-Latch woanders ein. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    const int32_t back = (weapon < 3) ? 900 : 2000;   /* Messer braucht Naehe (reach+Radius) */

    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    track(slot, back);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
    }

    int airborne = 0;
    if (wait_air > 0) {
        for (int w = 0; w < 900 && (pl->y - e->y) < wait_air; w++) {
            pl->hp = 100; track(slot, 1500);
            frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad_for(e)), 0);
        }
        airborne = ((pl->y - e->y) >= wait_air);
        printf("  [%s] Flug erreicht=%d (y=%d, vert=%d, band=%d, sub=%d)\n", tag, airborne,
               (int)e->y, (int)(pl->y - e->y), (int)e->aim_band, e->sub_state_1);
        if (!airborne) { printf("FAIL: %s — Kraehe hebt nicht ab\n", tag); fails++; return; }
    }

    /* ⛔ HARNESS-HILFE (2026-08-19, vollstaendiges RE2-Schadensmodell): in RE2 toetet EIN
     * Pistolen- oder Messertreffer die Kraehe — HP 10 (`addiu v0,zero,10` / `sh v0,342(s1)`
     * EMOVL21_S0.BIN @0x80100324/348) gegen Zone 0 der RE2-Zeile 0x800A45A0 = 15. Damit ist
     * "WIEDERHOLT treffbar" (= der +0x93-Latch, worum es diesem Pin geht) nicht mehr ueber
     * Schadens-Ereignisse messbar. Die Kraehe bekommt hier deshalb 60 HP = 4 Treffer — dieselbe
     * Sorte Harness-Hilfe wie das `pl->hp = 100` oben. Die byte-true Ein-Treffer-Letalitaet
     * selbst ist in test_re2_hp_model gepinnt (HP-Block + Schadenszeile 0x800A45A0). */
    if (type == 0x21u && flavor == RE15_AI_FLAVOR_RE2 && re15_re2_damage_model()) e->hp = 60;

    int hp_last = e->hp, hit_once = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100; track(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elev_pad_for(e)),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) { out->hits++; hit_once = 1; }
        hp_last = e->hp;
        if (hit_once && !(e->hit_react & 0x1)) out->released = 1;
        if (e->state == 7) out->corpse = 1;
        if (e->hp < 0)     { out->killed = 1; break; }
        if (out->corpse)   break;
    }
    out->last_react = e->hit_react;
    printf("  [%s] %d Treffer, hp %d, state %d/%d/%d, +0x93=0x%02X, tot=%d corpse=%d "
           "freigegeben=%d\n", tag, out->hits, e->hp, e->state, e->sub_state_1, e->sub_state_2,
           e->hit_react, out->killed, out->corpse, out->released);
}

static int load_room(const char *sub, int room_id, uint8_t **keep)
{
    char path[600];
    snprintf(path, sizeof path, "%s/%s", RE15_ASSET_PSX_DIR, sub);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) return 0;
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { free(buf); return 0; }
    s_room_id = room_id;
    if (*keep) free(*keep);
    *keep = buf;
    return 1;
}

int main(void)
{
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;
    uint8_t *keep = NULL;
    res_t c2g, c2k, c15g, s2g, s2k, s15g;

    /* ===================== KRAEHE — ROOM10C0 (der RE1.5-Kraehenraum) ===================== */
    printf("=== ROOM10C0 (Kraehen): zielen + feuern auf dem ECHTEN Weg ===\n");
    if (!load_room("STAGE1/ROOM10C0.RDT", 0x10c0, &keep)) {
        printf("FAIL: STAGE1/ROOM10C0.RDT nicht lesbar/parsebar\n"); return 1;
    }
    run("RE2   Kraehe Pistole", RE15_AI_FLAVOR_RE2,  0x21, 3, 900, &c2g);
    run("RE2   Kraehe Messer",  RE15_AI_FLAVOR_RE2,  0x21, 1, 900, &c2k);
    run("RE1.5 Kraehe Pistole", RE15_AI_FLAVOR_RE15, 0x21, 3, 900, &c15g);
    res_t c2air;
    run_ex("RE2   Kraehe Pistole IN DER LUFT", RE15_AI_FLAVOR_RE2, 0x21, 3, 900, 1200, &c2air);

    CHECK(c2g.hits >= 3,
          "(1) RE2 + Kraehe + Pistole: %d Treffer (>=3 gefordert) — mit dem +0x93-Latch-Bug war "
          "es GENAU 1 und die Kraehe (HP 10 @0x80100324/348) blieb unsterblich "
          "(Freigabe @0x80105F9C-FAC, Latch @0x800124E8/@0x800120F4-0x80012100)", c2g.hits);
    CHECK(c2g.killed,
          "(1b) RE2 + Kraehe + Pistole: die Kraehe stirbt (hp<0) — end-to-end");
    CHECK(c2g.released,
          "(1c) RE2 + Kraehe: +0x93 Bit 0 wird nach dem Treffer wieder frei "
          "(HURT-Wurzel -> ACTIVE @0x80102C7C mit a1=1, a2=9/0)");
    CHECK(c2k.hits >= 3,
          "(2) RE2 + Kraehe + Messer: %d Treffer (>=3 gefordert)", c2k.hits);
    CHECK(c2k.killed, "(2b) RE2 + Kraehe + Messer: die Kraehe stirbt");
    /* RE1.5-Regressions-Wache: die RE1.5-Kraehe hat per HP-Latch (`hp = vert>=5200 ? -1 : 0`
     * @0x80115F88-9C) IMMER HP 0 und stirbt am ERSTEN Treffer. Ein Anstieg hier waere eine
     * Regression am RE1.5-Tail, kein Fortschritt. */
    CHECK(c15g.hits == 1 && c15g.killed,
          "(3) RE1.5 + Kraehe + Pistole: genau 1 Treffer und tot (%d/%d) — byte-true HP-Latch "
          "hp = (vert_err>=5200) ? -1 : 0 @0x80115F88-9C", c15g.hits, c15g.killed);
    /* (3b) GENAU DER FALL, der im Fix eb841053 unmessbar blieb ("die Kraehe fliegt aus dem
     *      Elevations-Band"): erst abheben lassen (Wake dist<0x709 @0x80100704), dann im
     *      Flug abschiessen — das Band folgt dem aim_band-Stempel ueber das D-Pad. */
    CHECK(c2air.hits >= 3 && (c2air.killed || c2air.corpse),
          "(3b) RE2 + Kraehe + Pistole IN DER LUFT: %d Treffer, tot=%d/corpse=%d (>=3 und tot "
          "gefordert) — die fliegende Kraehe ist im richtigen Elevations-Band treffbar UND "
          "wiederholt treffbar", c2air.hits, c2air.killed, c2air.corpse);

    /* ===================== SPINNE — der Raum mit einer lebenden 0x25 ===================== */
    /* ROOM1090 stellt KEINE Typ-0x25 auf (gemessen: 0x25=0, 0x26=7 Baby-Spinnen) — der
     * RE1.5-Adult-Spider ist ein STAGE2-Gegner. Dieselbe Raum-Suche wie
     * test_re2_room1090_ab.c: den ERSTEN Raum nehmen, dessen SCD wirklich eine 0x25 aufstellt;
     * findet keiner eine, ist das ein FAIL (kein vacuous PASS). */
    static const struct { const char *sub; int id; } cand[] = {
        { "STAGE1/ROOM1090.RDT", 0x1090 }, { "STAGE2/ROOM2000.RDT", 0x2000 },
        { "STAGE2/ROOM2010.RDT", 0x2010 }, { "STAGE2/ROOM2020.RDT", 0x2020 },
        { "STAGE2/ROOM2040.RDT", 0x2040 }, { "STAGE2/ROOM2050.RDT", 0x2050 },
        { "STAGE2/ROOM2060.RDT", 0x2060 }, { "STAGE1/ROOM10D0.RDT", 0x10d0 },
        { "STAGE1/ROOM1260.RDT", 0x1260 }, { "STAGE3/ROOM3010.RDT", 0x3010 },
    };
    int picked = -1;
    for (unsigned i = 0; i < sizeof cand / sizeof cand[0] && picked < 0; i++) {
        if (!load_room(cand[i].sub, cand[i].id, &keep)) continue;
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        bringup();
        for (int f = 0; f < 30; f++) frame(0, 0);
        if (find_type(0x25) >= 0) picked = (int)i;
    }
    if (picked < 0) {
        printf("FAIL: kein Raum mit einer lebenden Typ-0x25-Spinne gefunden\n"); fails++;
    } else {
        load_room(cand[picked].sub, cand[picked].id, &keep);
        printf("\n=== %s (Adult-Spinne): zielen + feuern auf dem ECHTEN Weg ===\n",
               cand[picked].sub);
        run("RE2   Spinne Pistole", RE15_AI_FLAVOR_RE2,  0x25, 3, 900, &s2g);
        run("RE2   Spinne Messer",  RE15_AI_FLAVOR_RE2,  0x25, 1, 900, &s2k);
        run("RE1.5 Spinne Pistole", RE15_AI_FLAVOR_RE15, 0x25, 3, 900, &s15g);

        CHECK(s2g.hits >= 3,
              "(4) RE2 + Spinne + Pistole: %d Treffer (>=3 gefordert) — mit dem Latch-Bug war es "
              "GENAU 1, +0x93 blieb 0x03", s2g.hits);
        CHECK(s2g.corpse || s2g.killed,
              "(4b) RE2 + Spinne + Pistole: die Spinne erreicht CORPSE (state 7) — die "
              "RE2-DEATH-Zeile setzt HP am Ende auf 1 (@0x80103EB0), tot ist der Zustand");
        CHECK(s2g.released,
              "(4c) RE2 + Spinne: +0x93 Bit 0 wird nach dem Treffer wieder frei "
              "(HURT-Wurzel -> ACTIVE, alle Schreiber @0x80102E58/@0x80102ECC/@0x80102FA4/"
              "@0x80102FBC/@0x801030B4/@0x8010313C/@0x8010335C/@0x80103474/@0x801034F0/"
              "@0x80103C54 landen auf Zustand 1)");
        CHECK(s2k.hits >= 3,
              "(5) RE2 + Spinne + Messer: %d Treffer (>=3 gefordert)", s2k.hits);
        CHECK(s15g.hits >= 3,
              "(6) RE1.5 + Spinne + Pistole: %d Treffer (>=3) — Regressions-Wache fuer den "
              "RE1.5-Modus", s15g.hits);
    }

    if (keep) free(keep);
    printf(fails ? "\nFEHLGESCHLAGEN: %d\n" : "\nALLE PINS GRUEN (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
