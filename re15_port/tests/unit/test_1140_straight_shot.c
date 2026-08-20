/* test_1140_straight_shot.c — PIN + Negativproben zum ELEVATIONS-BAND in ROOM1140 (Dining Room).
 *
 * Nutzer-Report v0.3.5: "Im Original kann man im Dining Room gerade schiessen und trifft den am
 * Boden fressenden Zombie. Hier nicht."
 *
 * ROOM1140 stellt (gemessen, Roh-Deskriptoren direkt nach dem SCD-Spawn) auf:
 *   slot01 Typ 0x16 Deskriptor 0x88  pos(-800,0,-20600)   = der GEFRESSENE (liegend, downed-Bit)
 *   slot02 Typ 0x10 Deskriptor 0x86  pos(-1800,0,-19600)  = Fresser
 *   slot03 Typ 0x10 Deskriptor 0x86  pos(-1800,0,-21600)  = Fresser
 *   slot04 Typ 0x11 Deskriptor 0x86  pos( 200,0,-21600)   = Fresser
 *   slot05 Typ 0x11 Deskriptor 0x86  pos( 200,0,-19600)   = Fresser
 *
 * SOLLSEITE (selbst disassembliert, Herleitung im Kopf von re15_band_stamp_aa4 in re15_damage.c):
 * der ACTIVE-Tail der Zombie-Familie stempelt das Band VOR dem Sub-Dispatch @0x8011f80c[+0x9&0xf]
 * (@0x80101640-70), also auch fuer Fresser (Nibble 6) und Liegende (Nibble 7/8):
 *   @0x80101600-04  jal FUN_80012aa4, a0 = 0xBB8 (3000)   — IMMER, auch bei gesetztem 0x80
 *   FUN_80012aa4: word0 &= 0x1FFFFFFF (@0x80012af0-afc); s0 = (playerY-enemyY)/1800
 *                 (@0x80012ad8/b0c-b1c); (unsigned)s0 < 2 -> LEVEL (@0x80012b20-44);
 *                 |s0|*1000 < dist < R+|s0|*1000 -> s0>0 ? UP : DOWN (@0x80012b88-bd4)
 *   @0x80101614-3c  downed (+0x9 & 0x80): LEVEL weg, dann FUN_80012974(0x1388=5000) -> DOWN
 *
 * Daraus die sechs Faelle, die dieser Test festnagelt (flacher Boden -> s0 = 0):
 *   Fresser (grid 0x86 -> Decoder setzt +0x9 = 0)   gerade  6000 -> TREFFER  (LEVEL, kein Ring)
 *   Fresser                                          runter 2800 -> TREFFER  (Ring 0<2800<3000)
 *   Fresser                                          runter 6000 -> KEIN     (ausserhalb Ring)
 *   Liegender (grid 0x88)                            gerade 3000 -> KEIN     (LEVEL geloescht)
 *   Liegender                                        runter 3000 -> TREFFER  (974: 3000<5000)
 *   Liegender                                        runter 6000 -> KEIN     (974: 6000>=5000)
 * Die drei KEIN-Faelle sind die Negativproben: sie zeigen, dass die Hoehenpruefung nicht einfach
 * aufgeweicht wurde.
 *
 * Zusaetzlich der RE2-Flavor: derselbe gerade Schuss auf einen Fresser aus 2600 (dort loest der
 * RE2-Limpet-Latch +0x10E & 0x4000 ueber ai_dist < 0xFA0) muss ebenfalls treffen — Beleg, dass
 * das Band im RE2-Modus nicht blockiert.
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
extern int  re15_player_aim_elevation(void);

static re15_rdt_t         s_rdt;
static re15_camera_view_t s_cam;
static re15_game_ctx_t    s_ctx;
static int                s_fail = 0;

static void ok(int cond, const char *what)
{
    printf("  [%s] %s\n", cond ? "OK  " : "FAIL", what);
    if (!cond) s_fail = 1;
}

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

/* Deskriptor-Schnappschuss VOR dem ersten AI-Tick (danach raeumt der Spawn-Decoder +0x9 auf). */
static uint8_t s_desc[RE15_ACTOR_MAX];

static void bringup(int flavor)
{
    re15_ai_flavor_set((re15_ai_flavor_t)flavor);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1140;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    memset(s_desc, 0, sizeof s_desc);
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active) s_desc[s] = g_actors[s].grid_id;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);            /* Pistole (Schuss-Streifen, unbegrenzt) */
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }
}

static int find_desc(uint8_t d)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && s_desc[s] == d) return s;
    return -1;
}

/* Spieler auf `back` Einheiten vor das Ziel stellen und es ansehen. */
static void face(int slot, int32_t back)
{
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - back; pl->z = e->z; pl->y = e->y;
    pl->rot_y = (int16_t)(((int)re15_atan2_q12(e->z - pl->z, e->x - pl->x) - 0x400) & 0x0fff);
}

/* Ein Schuss-Lauf. `elev_pad` = 0 (gerade) oder RE15_PAD_BIT_DOWN (nach unten zielen).
 * `knock_first` != 0: erst GERADE feuern, bis das Ziel niedergeschlagen ist (+0x9 & 0x80), und
 * DANN auf `elev_pad` umschalten — gezaehlt werden nur die Treffer der ZWEITEN Phase.
 * Rueckgabe = Anzahl HP-Abzuege. */
static int shoot_ex(int flavor, uint8_t desc, int32_t back, uint16_t elev_pad, int frames,
                    int knock_first, const char *tag);
static int shoot(int flavor, uint8_t desc, int32_t back, uint16_t elev_pad, int frames,
                 const char *tag)
{ return shoot_ex(flavor, desc, back, elev_pad, frames, 0, tag); }

static int shoot_ex(int flavor, uint8_t desc, int32_t back, uint16_t elev_pad, int frames,
                    int knock_first, const char *tag)
{
    bringup(flavor);
    /* AI hochlaufen lassen, Spieler dabei WEIT weg (sonst wachen die Fresser sofort auf) */
    for (int f = 0; f < 60; f++) {
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 30000;
        g_actors[RE15_ACTOR_SLOT_PLAYER].z = 30000;
        frame(0, 0);
    }
    int slot = find_desc(desc);
    if (slot < 0) { printf("  [FAIL] %s: kein Aktor mit Deskriptor 0x%02X\n", tag, desc);
                    s_fail = 1; return -1; }
    /* ARENA: nur dieses Ziel bleibt aktiv, sonst rastet die Auto-Aim-Latch woanders ein. */
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    face(slot, back);

    int knocked = 0;
    if (knock_first) {                             /* Phase 1: GERADE, bis er liegt */
        for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
            pl->hp = 100; face(slot, back); frame(RE15_PAD_BIT_R1, 0);
        }
        for (int f = 0; f < frames && !(e->grid_id & 0x80) && e->hp >= 0; f++) {
            pl->hp = 100; face(slot, back);
            frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE),
                  (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        }
        knocked = (e->grid_id & 0x80) ? 1 : 0;
        if (!knocked || e->hp < 0) {
            printf("  %-58s slot%02d VORBEDINGUNG VERFEHLT (grid=0x%02X hp=%d)\n",
                   tag, slot, e->grid_id, e->hp);
            s_fail = 1; return -1;
        }
        re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    }

    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) {
        pl->hp = 100; face(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | elev_pad), 0);
    }
    int hp_last = e->hp, hits = 0;
    for (int f = 0; f < frames; f++) {
        pl->hp = 100; face(slot, back);
        frame((uint16_t)(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE | elev_pad),
              (uint16_t)((f == 0) ? RE15_PAD_BIT_SQUARE : 0));
        if (e->hp < hp_last) hits++;
        hp_last = e->hp;
        if (e->hp < 0) break;
    }
    printf("  %-58s slot%02d grid=0x%02X mo=0x%02X elev=%d dist=%d%s -> %d Treffer\n",
           tag, slot, e->grid_id, e->motion, re15_player_aim_elevation(), (int)back,
           knocked ? " (vorher niedergeschlagen)" : "", hits);
    return hits;
}

int main(void)
{
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1140.RDT", (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("RDT fehlt: %s\n", path); return 1; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("RDT-Parse\n"); return 1; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    printf("== ROOM1140 Spawn-Deskriptoren ==\n");
    bringup(RE15_AI_FLAVOR_RE15);
    ok(find_desc(0x86) >= 0, "ROOM1140 stellt mindestens einen Fresser (Deskriptor 0x86) auf");
    ok(find_desc(0x88) >= 0, "ROOM1140 stellt den Gefressenen (Deskriptor 0x88) auf");

    printf("\n== PIN: gerader Schuss auf den FRESSER (RE1.5) ==\n");
    ok(shoot(RE15_AI_FLAVOR_RE15, 0x86, 6000, 0, 240,
             "Fresser 0x86, GERADE, 6000") > 0,
       "Ein gerader Schuss trifft den fressenden Zombie (LEVEL @0x80012b20-44, kein Reichweiten-Cap)");

    printf("\n== Der Ring des Bandstempels (@0x80012b88-bd4) ==\n");
    ok(shoot(RE15_AI_FLAVOR_RE15, 0x86, 2800, RE15_PAD_BIT_DOWN, 240,
             "Fresser 0x86, RUNTER, 2800") > 0,
       "Nach unten gezielt trifft der Fresser INNERHALB des Rings (0 < 2800 < 3000)");
    ok(shoot(RE15_AI_FLAVOR_RE15, 0x86, 6000, RE15_PAD_BIT_DOWN, 240,
             "Fresser 0x86, RUNTER, 6000") == 0,
       "NEGATIV: nach unten gezielt trifft er AUSSERHALB des Rings NICHT (6000 >= 3000)");

    printf("\n== Der DOWN-Nachlauf FUN_80012974(0x1388) am niedergeschlagenen Zombie ==\n");
    ok(shoot_ex(RE15_AI_FLAVOR_RE15, 0x86, 3000, RE15_PAD_BIT_DOWN, 240, 1,
                "Fresser 0x86 niedergeschlagen, RUNTER, 3000") > 0,
       "Der niedergeschlagene Zombie (+0x9 |= 0x80) ist innerhalb 5000 nach unten treffbar");
    ok(shoot_ex(RE15_AI_FLAVOR_RE15, 0x86, 3000, 0, 240, 1,
                "Fresser 0x86 niedergeschlagen, GERADE, 3000") == 0,
       "NEGATIV: derselbe Liegende ist GERADE nicht mehr treffbar (LEVEL weg @0x80101624-3c)");
    ok(shoot_ex(RE15_AI_FLAVOR_RE15, 0x86, 6000, RE15_PAD_BIT_DOWN, 240, 1,
                "Fresser 0x86 niedergeschlagen, RUNTER, 6000") == 0,
       "NEGATIV: jenseits 5000 traegt der Niedergeschlagene gar kein Band mehr");

    printf("\n== Regressionswache: der GEFRESSENE (Deskriptor 0x88) bleibt unantastbar ==\n");
    /* Der passive Liege-Zweig setzt +0x93 |= 1 in JEDEM Tick (`lbu 147; ori 1; sb 147`
     * @0x80103aac-ab8) — der Resolver verwirft ihn ueber die Zweitkontakt-Rekursion
     * @0x800123fc-418, unabhaengig vom Band. Er ist also mit KEINER Zielhoehe treffbar. */
    ok(shoot(RE15_AI_FLAVOR_RE15, 0x88, 3000, 0, 240,
             "Liegender 0x88, GERADE, 3000") == 0,
       "NEGATIV: der Gefressene bleibt gegen den geraden Schuss immun");
    ok(shoot(RE15_AI_FLAVOR_RE15, 0x88, 3000, RE15_PAD_BIT_DOWN, 240,
             "Liegender 0x88, RUNTER, 3000") == 0,
       "NEGATIV: auch nach unten gezielt bleibt er immun (+0x93-Latch @0x80103aac-ab8)");

    printf("\n== RE2-Flavor: das Band blockiert dort ebenfalls nicht ==\n");
    ok(shoot(RE15_AI_FLAVOR_RE2, 0x86, 2600, 0, 240,
             "Fresser 0x86, GERADE, 2600 (RE2)") > 0,
       "RE2-Flavor: gerader Schuss trifft den Fresser, sobald der RE2-Trefferfilter ihn freigibt");

    printf("\n%s\n", s_fail ? "== FEHLGESCHLAGEN ==" : "== ALLE PRUEFUNGEN GRUEN ==");
    return s_fail;
}
