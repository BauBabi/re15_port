/**
 * @file test_re2_stagger_knockdown.c
 * @brief PIN — die ZWEI Nutzer-Befunde vom 2026-08-19 (RE2-KI, Dinner Room ROOM1140).
 *
 * (A) "Nach ein paar Treffern schwanken die Zombies nach hinten. Beim Original wuerden sie dann
 *     umfallen, wenn man sie noch mal trifft. Bei uns nicht."
 * (B) "Das Blut beim Treffen erscheint am Fuss."
 *
 * ---- (A) DIE GEMESSENE IST-LAGE VOR DEM FIX (probe_re2_stagger, echter Weg) ------------------
 *   f132 Treffer -> +0x222 = 1 -> Stagger 0x80105BC0 (Clip 4, Schub -450 = "nach hinten")
 *   f132..f174   -> Stagger-P1; +0x93 = 0x01 die GANZE Zeit -> JEDER Schuss wird abgewiesen
 *   f175         -> Stagger-P2: +0x223 neu, +0x222 = 0     -> das Flinch-Fenster ist zu
 *   ueber 900 Frames: EXEC[5] (Sturz) NIE erreicht, Zustandsfolge Stagger->Niederschlag fehlt.
 * Der Niederschlag IST der Flinch 0x501 (@0x801050A4) -> EXEC[5] @0x80103188 ("KNOCKDOWN-FALL").
 * Sein Gate ist `+0x222 == 1` (@0x80105080-98), und +0x222 ist AUSSCHLIESSLICH waehrend des
 * Staggers 1 (gesetzt @0x80105164, geloescht in Stagger-P2 @0x80106034 / EXEC[5]-P0 @0x801032F4).
 * Es braucht also zwingend einen Treffer MITTEN in der laufenden Reaktion.
 *
 * ---- DER ORIGINAL-FILTER (selbst disassembliert, info/re2leon/PSX.EXE, FUN_800470C0) --------
 *   80047138: lbu v0,467(s0)          ; +0x1D3
 *   80047140: bne v0,zero,0x8004740c  ; != 0 -> Kandidat UEBERSPRUNGEN
 * RE2 kennt +0x93 gar nicht; Haupt-Handler 0x80105438 und Stagger 0x80105BC0 schreiben +0x1D3
 * NIE (Voll-Scan aller `sb/lbu rt,467(rs)` in EMOVL10_S0.BIN) -> in beiden Reaktionen bleibt der
 * Zombie treffbar. Der Port spiegelt den Filter jetzt auf den Latch, den der geteilte RE1.5-
 * Resolver liest (re15_re2z_tick-Epilog). Dazu die beiden fehlenden Original-Clears in EXEC[5]:
 * `+0x1D3 &= 0x7f` @0x80103484-90 (Sturzphase) und @0x80103718-28 (P8).
 *
 * ---- (B) DER BLUT-ANKER ---------------------------------------------------------------------
 * FUN_8001BF10(a0=Id, a1=Winkel, a2=&Anker, a3=&Vektor) kopiert aus a2 32 Byte = eine ganze
 * MATRIX in den Effekt-Slot (@0x8001C03C-78) und merkt sich den Zeiger (@0x8001C094). a2 ist
 * immer `+0x198 + n*172 + 72` — eine KNOCHEN-Matrix. Die Anker-Wahl der Treffer-Emitter
 * (@0x801050B0-158 Wurzel, @0x80105650-704 Haupt-P0, @0x80105D14-DBC Stagger-P0):
 *   (+0x1D2 % 3) == 0 -> Part 1 (a2 = +0x198 + 244)   sonst -> Part 0 (a2 = +0x198 + 72)
 * Der Port spawnte positionslos an der Aktor-Wurzel — und die liegt auf dem BODEN.
 * GEMESSEN (ROOM1140, RE2-Bank EM010): Wurzel y = 0, Bone0/Bone1 y = -1166 -> 1166 Einheiten
 * Versatz, genau der berichtete "Blut am Fuss".
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

extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int  re15_re2z_last_fx_part(void);
extern void re15_re2z_last_fx_pos(int32_t out[3]);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

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

/* Die echte RE2-Bank (Spiegel von pc_enemy_load). OHNE sie ist re15_actor_clip_len() == 0,
 * jede clip-getriebene Phase endet im Setz-Tick und re15_enemy_bone_world_pos() faellt auf die
 * Aktor-Wurzel zurueck — dann misst der Test das Testgestell statt der Engine. */
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

static int standing_zombie(void)
{
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type >= 0x10 && g_actors[s].type <= 0x18
            && !(g_actors[s].grid_id & 0x80)) return s;
    return -1;
}

typedef struct {
    int hits;                 /* getrennte Schadens-Ereignisse                                */
    int stagger_frames;       /* Frames mit +0x222 == 1 in Zustand 2 (= das Schwanken)         */
    int hit_during_stagger;   /* Treffer, die WAEHREND des Staggers durchkamen                 */
    int knockdown;            /* EXEC[5] (Sturz) erreicht                                      */
    int kd_after_stagger;     /* … und zwar aus einem laufenden Stagger heraus                 */
    int self1d3_stuck;        /* +0x1D3 blieb nach dem Sturz stehen (Bricking-Wache)           */
    int fx_at_root;           /* Blut-Spawns auf der Aktor-Wurzel                              */
    int fx_at_bone;           /* Blut-Spawns auf der Knochen-Matrix                            */
    int fx_part_seen;         /* zuletzt benutzter Anker-Part                                  */
} result_t;

static void run(const char *tag, int budget, result_t *out)
{
    memset(out, 0, sizeof *out);
    out->fx_part_seen = -1;
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    bringup();
    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);                 /* Pistole = Tabellen-Zeile 3 */
    {   int es = re15_inv_equipped_slot();
        if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    for (int f = 0; f < 60; f++) { g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100; frame(0, 0); }
    re15_enemy_bank_t *b = load_re2_bank(0x10);
    if (!b || !b->ok) { printf("FAIL: RE2-Bank EM010 fehlt (shared_assets/RE2/CDEMD0.EMS)\n");
                       fails++; return; }
    int slot = standing_zombie();
    if (slot < 0) { printf("FAIL: %s — kein stehender Zombie in ROOM1140\n", tag); fails++; return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    re15_actor_t *e  = &g_actors[slot];
    pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    int hp_last = e->hp, fx_last = -1, stag_prev = 0, kd_prev = 0;
    for (int f = 0; f < budget; f++) {
        pl->hp = 100;
        int was_stagger = (e->state == 2 && e->re2z_flag222 == 1);
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        int hit = (e->hp < hp_last); hp_last = e->hp;
        if (hit) { out->hits++; if (was_stagger) out->hit_during_stagger++; }

        int stag = (e->state == 2 && e->re2z_flag222 == 1);
        if (stag) out->stagger_frames++;
        int kd = (e->state == 1 && e->sub_state_1 == 5);
        if (kd) { out->knockdown = 1; if (!kd_prev && stag_prev) out->kd_after_stagger = 1; }
        /* Bricking-Wache: nach dem Sturz (zurueck in den Gang) MUSS +0x1D3 wieder 0 sein —
         * sonst waere der Zombie mit dem Original-Filter fuer immer untreffbar. */
        if (kd_prev && !kd && e->re2z_self1d3 != 0) out->self1d3_stuck = 1;
        stag_prev = stag; kd_prev = kd;

        /* Blut-Position: wurde in diesem Frame gespawnt, wenn sich Part/Position aendert oder
         * ein Treffer lag. Wir vergleichen die zuletzt gespawnte Position mit der Aktor-Wurzel. */
        int part = re15_re2z_last_fx_part();
        if (part >= 0) {
            int32_t p[3]; re15_re2z_last_fx_pos(p);
            if (part != fx_last || hit) {
                if (p[0] == e->x && p[1] == e->y && p[2] == e->z) out->fx_at_root++;
                else                                             out->fx_at_bone++;
                out->fx_part_seen = part;
                fx_last = part;
            }
        }
        if (e->hp < 0) break;
    }
    printf("  [%s] %d Treffer | Stagger %d Frames, %d Treffer darin | Sturz=%d (aus Stagger=%d) | "
           "+0x1D3 haengt=%d | FX Wurzel/Knochen=%d/%d (Part %d)\n",
           tag, out->hits, out->stagger_frames, out->hit_during_stagger, out->knockdown,
           out->kd_after_stagger, out->self1d3_stuck, out->fx_at_root, out->fx_at_bone,
           out->fx_part_seen);
}

/* ---- (B) die ANKER-REGEL direkt: Zone 0 -> Part 1, sonst Part 0 ---------------------------- */
static void anchor_rule(re15_actor_t *e, int slot)
{
    static const struct { unsigned col; int part; const char *why; } cases[] = {
        { 0, 1, "Zone 0 -> Part 1 (a2 = +0x198+244, `addiu a2,a2,244` @0x80105110)" },
        { 1, 0, "Zone 1 -> Part 0 (a2 = +0x198+ 72, `addiu a2,s0,72`  @0x80105158)" },
        { 2, 0, "Zone 2 -> Part 0 (derselbe !=0-Zweig)"                              },
        { 3, 1, "Bracket 1 / Zone 0 -> Part 1 (die %3-Rechnung @0x801050B4-DC)"      },
        { 4, 0, "Bracket 1 / Zone 1 -> Part 0"                                       },
    };
    for (unsigned i = 0; i < sizeof cases / sizeof cases[0]; i++) {
        e->state = 2; e->sub_state_1 = 3; e->sub_state_2 = 0;
        e->re2z_hits1d2 = (uint8_t)cases[i].col;
        e->re2z_res223  = 20; e->re2z_flag222 = 0; e->re2z_flags21a = 0;
        e->re2z_self1d3 = 0; e->hp = 90;
        /* Der ZERLEGER (@0x80105288-3D8) laeuft VOR dem Dispatch und wuerde sonst als letzter
         * Emitter das Schienbein anschreiben. Seine drei Gates zumachen: mittlere Zonen-Reserve
         * >= 0 (`lb 338 / bgez` @0x80105294-9C) und Treffer NICHT von der Seite
         * (`+0x1D0 & 0xC0` @0x801052A4-B0). */
        e->re2z_pool151 = e->re2z_pool152 = e->re2z_pool153 = 13;
        e->re2z_hitdir1d0 = 0;
        re15_re2z_tick(slot);
        int part = re15_re2z_last_fx_part();
        CHECK(part == cases[i].part,
              "(B2) +0x1D2 = %u: Anker-Part %d erwartet, gemessen %d — %s",
              cases[i].col, cases[i].part, part, cases[i].why);
    }
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

    printf("=== ROOM1140 (Dinner Room), RE2-KI, Pistole — Schwanken -> Umfallen + Blut-Anker ===\n");
    result_t r;
    run("RE2 Pistole", 1200, &r);

    CHECK(r.stagger_frames > 0,
          "(A0) der Stagger (0x80105BC0, +0x222 == 1) wird ueberhaupt erreicht: %d Frames",
          r.stagger_frames);
    CHECK(r.hit_during_stagger > 0,
          "(A1) ein Treffer kommt WAEHREND des Staggers durch (%d) — vor dem Fix 0, weil der "
          "RE1.5-Latch +0x93 die ganze Reaktion sperrte; der Original-Filter ist `lbu 467 / "
          "bne v0,zero` @0x80047138-40 und +0x1D3 ist im Stagger 0", r.hit_during_stagger);
    CHECK(r.knockdown,
          "(A2) der NIEDERSCHLAG feuert: Flinch 0x501 @0x801050A4 -> EXEC[5] @0x80103188 "
          "(state 1 / sub 5). ⚠ ALLEIN NICHT AUSSAGEKRAEFTIG — EXEC[5] hat einen zweiten "
          "Produzenten (Griff-Ausgang 0x501 @0x80102D24-2C); der Diskriminator ist (A3)");
    CHECK(r.kd_after_stagger,
          "(A3) und zwar AUS dem laufenden Stagger heraus — genau die vom Nutzer beschriebene "
          "Folge 'schwanken -> nochmal treffen -> umfallen'");
    CHECK(!r.self1d3_stuck,
          "(A4) nach dem Sturz ist +0x1D3 wieder 0 (`andi 0x7f` @0x80103484-90 / @0x80103718-28) "
          "— sonst waere der Zombie mit dem Original-Filter dauerhaft untreffbar");
    CHECK(r.fx_at_bone > 0 && r.fx_at_root == 0,
          "(B1) JEDER Treffer-Effekt sitzt auf der Knochen-Matrix, keiner auf der Aktor-Wurzel "
          "(Wurzel/Knochen = %d/%d) — die Wurzel ist der Boden, das war 'Blut am Fuss'",
          r.fx_at_root, r.fx_at_bone);

    /* (B2) die Anker-REGEL direkt am Zustand, unabhaengig davon, welche Zone der Schuss trifft. */
    {   int slot = standing_zombie();
        if (slot > 0) anchor_rule(&g_actors[slot], slot);
        else { printf("FAIL: (B2) kein Zombie fuer die Anker-Regel\n"); fails++; }
    }

    free(buf);
    printf(fails ? "\nFEHLGESCHLAGEN: %d\n" : "\nALLE PINS GRUEN (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
