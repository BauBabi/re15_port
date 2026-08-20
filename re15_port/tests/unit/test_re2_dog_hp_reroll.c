/**
 * @file test_re2_dog_hp_reroll.c
 * @brief PIN — RE2-Hund, HP-Re-Roll @0x801037E8 („spielt tot"): TABELLE, EINMAL-GUARD,
 *        ERREICHBARKEIT.
 *
 * ANLASS (2026-08-20): gemeldeter Befund „der HURT-P4-HP-Re-Roll des RE2-Hundes zieht seinen
 * Wert weiter aus dem widerlegten Zweig 0x80105340 statt 0x801053B0".
 * NACHGEPRUEFT UND WIDERLEGT — der Port ist korrekt. Dieser Pin friert den Beleg ein.
 *
 * ===========================================================================================
 * DER BELEG (EMD0G_MOD0.BIN, laedt RAW @0x80100000; selbst disassembliert)
 * ===========================================================================================
 * Es gibt VIER Hunde-HP-Tabellen, aber ZWEI verschiedene Auswahl-Logiken:
 *
 *   INIT @0x80100150 — VIER-WEGIG ueber BEIDE Bits von DAT_800CFB74:
 *     80100160  andi v0,v0,0x40   80100164  beq -> 0x801001d0
 *     80100180  andi v1,v1,0x20   80100180  beq -> 0x801001ac
 *       0x801001A0 lhu 0x801053D0 | 0x801001C4 lhu 0x801053B0
 *       0x80100204 lhu 0x80105360 | 0x80100228 lhu 0x80105340
 *     Mit „0x40 immer gesetzt / 0x20 nie" (9 Schreiber von DAT_800CFB74, Beleg in
 *     re15_damage.c) ist der ausgelieferte INIT-Zweig **0x801053B0** — genau das macht
 *     re15_re2_init_hp (Commit 717d13e0, Hund 129er-Familie).
 *
 *   RE-ROLL @0x801037E8 — ZWEI-WEGIG, testet NUR Bit 0x20 (Bit 0x40 kommt nicht vor):
 *     80103818  lw   v1,-1164(v1)      ; DAT_800CFB74
 *     80103820  andi v1,v1,0x20
 *     80103824  beq  v1,zero,0x80103850
 *       0x80103844 lhu 0x80105360 | 0x80103868 lhu **0x80105340**
 *     -> mit 0x20 = 0 ist 0x80105340 der ausgelieferte Zweig. 0x801053B0 ist von hier aus
 *        gar nicht adressierbar (Voll-Scan aller Tabellen-`lhu` im Modul findet GENAU sechs:
 *        0x801001A0/0x801001C4/0x80100204/0x80100228 = INIT, 0x80103844/0x80103868 = Re-Roll).
 *
 * Die beiden Tabellen sind Wert fuer Wert um 10 verschoben, also trennbar:
 *   0x80105340 = {119, 85, 85, 85, 119, 70, 85, 85, 70, 85, 59, 70, 59, 85, 59, 70}
 *   0x801053B0 = {129, 95, 95, 95, 129, 80, 95, 95, 80, 95, 69, 80, 69, 95, 69, 80}
 * HP = tbl[rand&0xf] + (rand&3)  (@0x80103870 / @0x8010387C `sh v0,342(s0)`)
 *   -> erlaubte Werte aus 0x80105340: 59..62, 70..73, 85..88, 119..122
 *   -> EXKLUSIV fuer 0x801053B0:      95..98 und 129..132   (der NEGATIV-Test)
 *
 * ===========================================================================================
 * ERREICHBARKEIT (die zweite Haelfte des Befunds)
 * ===========================================================================================
 * „im HURT-Fluss unerreicht" stimmt — aber die Funktion ist KEIN toter Code:
 *   HURT   : generische Zeile 0x80103308 -> @0x80105588[+0x6]; [4] = 0x801037E8. +0x6 wird im
 *            Modul NIE > 3 (Voll-Scan aller `sb/sh rt,6(rs)`) -> unerreichbar.
 *   DEATH  : Router 0x80104118 -> bei +0x6 != 0 ueber @0x80105668[+0x6]; **[3] = 0x801037E8**
 *            (ebenso @0x80105688[3] / @0x80105698[3]). Erreichbar, weil der RE2-Applier
 *            FUN_800470C0 die Phase +0x6 beim Kill NICHT nullt (er schreibt auf die Entity nur
 *            +0x4/+0x5/+0x1D0/+0x1D2/+0x1D3).
 *   PORT   : der Root-Tick setzt bei jedem neuen Treffer die Phase auf 0 (dokumentiertes
 *            MAPPING an den RE1.5-Writer, der +0x6=1 stempelt) -> DEATH startet immer bei [0];
 *            die zweite Quelle fuer +0x6==3 in DEATH (P1-Soft-Landung @0x80103598) ist durch
 *            `bltz +0x156` @0x80103570 gesperrt, weil HP im Tod stets < 0 ist.
 *            Punkt (3) misst das auf dem ECHTEN Weg (ROOM1190, game_step, Pad R1+SQUARE).
 *
 * DIE PINS
 *   (1) Tabelle: ueber 4096 erzwungene Re-Rolls liegt JEDER HP-Wert in der 0x80105340-Menge,
 *       mindestens einer in 119..122 (dort exklusiv), und KEINER in 95..98 / 129..132
 *       (= dem behaupteten Zweig 0x801053B0). NEGATIV-TEST: taeuscht man die Tabelle gegen
 *       0x801053B0 aus, wird (1c) rot — die Mengen sind disjunkt genug.
 *   (2) Einmal-Guard @0x801037FC-0x80103810: mit +0x7 != 0 aendert der Re-Roll die HP NICHT.
 *   (3) Erreichbarkeit auf dem echten Weg: kein Hund erreicht in DEATH je Phase >= 3, und
 *       keinem Hund waechst nach dem Tod die HP wieder — der Zweig ist im Port zu.
 *   (4) Regressions-Wache: die normale Todeskette laeuft weiter bis CORPSE (state 7). Das
 *       pinnt, dass das ENTFERNEN des erfundenen `set_state_word(0x7)` im P4-Zweig die
 *       gewoehnliche Hunde-Todesanimation nicht angetastet hat.
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

extern int  re15_player_aim_ready(void);
extern void re15_player_aim_reset(void);
extern void re15_player_set_aim_clip_len(int fc);
extern int16_t re15_atan2_q12(int32_t dz, int32_t dx);

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

/* Die beiden Tabellen, byte-gleich aus dem Modul gelesen. */
static const uint16_t TBL_5340[16] = { 119, 85, 85, 85, 119, 70, 85, 85, 70, 85, 59, 70, 59, 85, 59, 70 };
static const uint16_t TBL_53B0[16] = { 129, 95, 95, 95, 129, 80, 95, 95, 80, 95, 69, 80, 69, 95, 69, 80 };

static int in_table_set(const uint16_t *t, int hp)
{
    for (int i = 0; i < 16; i++)
        for (int r = 0; r < 4; r++)
            if (hp == (int)t[i] + r) return 1;
    return 0;
}
/* Werte, die NUR aus 0x801053B0 stammen koennen (die 0x80105340-Menge enthaelt sie nicht). */
static int exclusive_to_53B0(int hp) { return in_table_set(TBL_53B0, hp) && !in_table_set(TBL_5340, hp); }
/* Werte, die NUR aus 0x80105340 stammen koennen. */
static int exclusive_to_5340(int hp) { return in_table_set(TBL_5340, hp) && !in_table_set(TBL_53B0, hp); }

/* ==========================================================================================
 * (1)+(2) — den DEATH-Phase-3-Dispatch direkt fahren
 * ========================================================================================== */
/* Der Re-Roll ist `static`; erreicht wird er ueber den echten Dispatch:
 * re15_re2dog_tick -> state 3 (DEATH) -> +0x6 == 3 -> @0x80105668[3].
 * re2z_prev_hp == hp haelt die Treffer-Erkennung im Root-Tick still (sie wuerde die Phase
 * sonst auf 0 zuruecksetzen — genau der Mechanismus, den Punkt (3) misst). */
static void reroll_once(int slot)
{
    re15_actor_t *e = &g_actors[slot];
    e->state       = 3;      /* DEATH */
    e->sub_state_2 = 3;      /* +0x6 = 3 -> @0x80105668[3] = 0x801037E8 */
    e->sub_state_3 = 0;      /* +0x7 = 0 -> Einmal-Guard offen */
    e->re2z_prev_hp = e->hp; /* keine Treffer-Erkennung */
    re15_re2dog_tick(slot);
}

static void pin_table_and_guard(void)
{
    printf("=== (1)/(2) HP-Re-Roll @0x801037E8: Tabelle + Einmal-Guard ===\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(0);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;

    const int slot = 1;
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->hp = -7;
    e->x = 2000; e->z = 0; e->y = 0;

    int n_5340_excl = 0, n_53B0_excl = 0, n_foreign = 0, n = 0;
    int lo = 0x7fff, hi = -0x7fff;
    for (int i = 0; i < 4096; i++) {
        e->hp = -7;                       /* Startwert ausserhalb beider Mengen */
        reroll_once(slot);
        int hp = (int)e->hp;
        n++;
        if (hp < lo) lo = hp;
        if (hp > hi) hi = hp;
        if (!in_table_set(TBL_5340, hp)) n_foreign++;
        if (exclusive_to_5340(hp)) n_5340_excl++;
        if (exclusive_to_53B0(hp)) n_53B0_excl++;
    }
    printf("  %d Re-Rolls, HP-Spanne %d..%d, exklusiv-0x80105340 %d, exklusiv-0x801053B0 %d, "
           "tabellenfremd %d\n", n, lo, hi, n_5340_excl, n_53B0_excl, n_foreign);

    CHECK(n_foreign == 0,
          "(1a) jeder Re-Roll-Wert liegt in der Menge {0x80105340[i] + 0..3} (%d Ausreisser) "
          "— `lhu v0,21312(at)` @0x80103868 + `addu v0,v0,s1` @0x80103870", n_foreign);
    CHECK(n_5340_excl > 0,
          "(1b) mindestens ein Wert in 119..122 (nur aus 0x80105340 erreichbar): %d von %d",
          n_5340_excl, n);
    CHECK(n_53B0_excl == 0,
          "(1c) NEGATIV: kein einziger Wert aus 95..98 / 129..132 (das waere der gemeldete "
          "Zweig 0x801053B0 — der ist die INIT-Tabelle @0x801001C4, der Re-Roll testet Bit "
          "0x40 gar nicht): %d Treffer", n_53B0_excl);
    CHECK(hi <= 122,
          "(1d) Obergrenze 122 = 119+3 (0x80105340). 0x801053B0 wuerde bis 132 gehen (ist %d)", hi);

    /* (2) Einmal-Guard: +0x7 != 0 -> `bne v0,zero,0x801038a8` @0x80103804 -> keine Aenderung. */
    e->hp = -7; e->state = 3; e->sub_state_2 = 3; e->sub_state_3 = 1;
    e->re2z_prev_hp = e->hp;
    re15_re2dog_tick(slot);
    CHECK(e->hp == -7,
          "(2) Einmal-Guard @0x801037FC-0x80103804: mit +0x7 = 1 bleibt die HP unveraendert "
          "(ist %d, erwartet -7)", (int)e->hp);
}

/* ==========================================================================================
 * (3)+(4) — der ECHTE Weg: ROOM1190, RE2-Flavor, game_step mit Pad R1 + SQUARE
 * ========================================================================================== */
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

static void pin_reachability(void)
{
    printf("=== (3)/(4) ROOM1190, RE2-Hunde: erreicht DEATH je Phase >= 3? ===\n");
    const char *base = getenv("RE15_ASSET_DIR");
    char path[600];
    snprintf(path, sizeof path, "%s/STAGE1/ROOM1190.RDT",
             (base && *base) ? base : RE15_ASSET_PSX_DIR);
    size_t sz = 0; uint8_t *buf = slurp(path, &sz);
    if (!buf) { printf("FAIL: %s nicht lesbar\n", path); fails++; return; }
    if (re15_rdt_parse(buf, sz, &s_rdt) != 0) { printf("FAIL: RDT-Parse\n"); fails++; free(buf); return; }
    memset(&s_cam, 0, sizeof s_cam); memset(&s_ctx, 0, sizeof s_ctx);
    s_ctx.rdt = &s_rdt; s_ctx.rdt_ok = 1; s_ctx.cam_view = &s_cam; s_ctx.active_cut = 0;

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_aot_init(); scd_vm_init();
    re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_player_cmd_reset(); re15_player_aim_reset();
    re15_damage_seed_rng(0x0badf00du);
    g_current_room_id = 0x1190;
    if (s_rdt.main_scd)   scd_thread_start(0, s_rdt.main_scd);
    if (s_rdt.sub_scd[0]) scd_thread_start(1, s_rdt.sub_scd[0]);
    if (s_rdt.sub_scd_count > 13 && s_rdt.sub_scd[13]) scd_thread_start(2, s_rdt.sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->state = 0; pl->motion = 0; pl->floor = 0; pl->y = 0;
    re15_collision_set_band(0);
    re15_player_set_aim_clip_len(12);

    re15_inv_load_briefing();
    re15_player_set_equipped_weapon(3);                     /* Pistole */
    { int es = re15_inv_equipped_slot();
      if (es >= 0 && es < RE15_INV_MAX_SLOTS) g_inv.slots[es].qty = 250; }

    for (int f = 0; f < 60; f++) { pl->hp = 100; frame(0, 0); }

    int slot = -1;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) { slot = s; break; }
    if (slot < 0) { printf("FAIL: kein RE2-Hund in ROOM1190\n"); fails++; free(buf); return; }
    for (int s = 1; s < RE15_ACTOR_MAX; s++) if (s != slot) g_actors[s].active = 0;

    re15_actor_t *e = &g_actors[slot];
    pl->x = e->x - 2600; pl->z = e->z; pl->y = e->y; pl->rot_y = 1024; pl->hp = 100;
    re15_player_cmd_reset(); re15_player_aim_reset(); re15_player_set_aim_clip_len(12);
    for (int f = 0; f < 40 && !re15_player_aim_ready(); f++) { pl->hp = 100; frame(RE15_PAD_BIT_R1, 0); }

    int death_phase_max = -1;      /* groesste Phase, die der Hund je in state 3 zeigt */
    int hp_grew_after_death = 0;   /* HP-Anstieg nach dem Tod = der Re-Roll haette gefeuert */
    int corpse_seen = 0, hits = 0;
    int hp_last = e->hp, was_dead = 0;
    for (int f = 0; f < 1400; f++) {
        pl->hp = 100;
        track(slot, 2600);
        frame(RE15_PAD_BIT_R1 | RE15_PAD_BIT_SQUARE, (f == 0) ? RE15_PAD_BIT_SQUARE : 0);
        if (e->hp < hp_last) hits++;
        if (e->state == 3) {
            was_dead = 1;
            if ((int)e->sub_state_2 > death_phase_max) death_phase_max = (int)e->sub_state_2;
        }
        if (was_dead && e->hp > hp_last && e->hp >= 0) hp_grew_after_death = 1;
        if (e->state == 7) corpse_seen = 1;
        hp_last = e->hp;
    }
    printf("  %d Treffer, Endzustand state=%d sub=%d/%d hp=%d; groesste DEATH-Phase %d; "
           "CORPSE gesehen=%d\n", hits, e->state, e->sub_state_1, e->sub_state_2, (int)e->hp,
           death_phase_max, corpse_seen);

    CHECK(was_dead, "(3a) der Hund stirbt auf dem echten Weg (state 3 erreicht) — sonst misst "
                    "dieser Pin nichts");
    /* POSITIV-KONTROLLE fuer (3b)/(3c): Punkt (1) oben fuehrt GENAU diesen Dispatch aus
     * (re15_re2dog_tick mit state 3 + Phase 3) und sieht den Re-Roll 4096-mal feuern — der
     * Detektor ist also nicht blind, hier ist der Zweig wirklich zu.
     * GEMESSEN (Gegenprobe 2026-08-20): schaltet man die Phasen-Rueckstellung im Root-Tick ab,
     * bleibt die groesste DEATH-Phase trotzdem 2 — der RE1.5-Writer stempelt +0x6 = 1
     * (@0x80012fd8), bevor der Zustand auf 3 geht. Zwei unabhaengige Gruende. */
    CHECK(death_phase_max >= 0 && death_phase_max <= 2,
          "(3b) DEATH laeuft nur ueber die Phasen 0..2 (Kern @0x80104178 / Luft @0x801034C8 / "
          "Rutschen @0x80104200); Phase 3 = der HP-Re-Roll @0x80105668[3] wird NIE erreicht "
          "(groesste gesehene Phase: %d)", death_phase_max);
    CHECK(!hp_grew_after_death,
          "(3c) keinem toten Hund waechst die HP wieder — der Re-Roll (59..122) hat nicht "
          "gefeuert");
    CHECK(corpse_seen,
          "(4) REGRESSIONS-WACHE: die normale Todeskette endet weiterhin in CORPSE (state 7, "
          "`sw 7,4` @0x80104290) — das Entfernen des erfundenen `set_state_word(0x7)` im "
          "P4-Zweig hat die gewoehnliche Todesanimation nicht angetastet");
    free(buf);
}

int main(void)
{
    pin_table_and_guard();
    pin_reachability();
    printf(fails ? "\nFEHLGESCHLAGEN: %d\n" : "\nALLE PINS GRUEN (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
