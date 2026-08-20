/**
 * @file test_re2_dog_playdead_gate.c
 * @brief PIN — RE2-Hund „Totstellen" (@0x801037E8 als DEATH-Phase 3): der Zweig ist im
 *        ORIGINAL unerreichbar. Dieser Pin friert BEIDE Riegel ein, damit die Frage nicht
 *        ein drittes Mal aufgemacht wird.
 *
 * ANLASS: es stand zweimal im Repo, 0x801037E8 sei im Original erreichbar („der Hund spielt
 * tot und steht beim naechsten Treffer wieder auf") und im Port faelschlich zu. Beide Male
 * lag der Fehler am APPLIER-LESEN. Nachgemessen 2026-08-20:
 *
 * ===========================================================================================
 * RIEGEL 1 — der RE2-Applier NULLT die Phase (Wort-Store, nicht Byte-Store)
 * ===========================================================================================
 * FUN_800470C0 (info/re2leon/PSX.EXE, t_addr 0x80010000):
 *   80047264  lw    v1,4(s1)      ; altes routine-Wort  (+0x4 state | +0x5 sub | +0x6 phase | +0x7 guard)
 *   80047278  sw    v1,508(s1)    ; +0x1FC = altes Wort (nur wenn != 0xC02)
 *   8004727c  lh    v1,342(s1)    ; HP signed
 *   80047280  addiu v0,zero,2
 *   80047284  bgez  v1,0x80047294
 *   80047288  sw    v0,4(s1)      ; DELAY-SLOT: Wort = 2 (HURT) -> +0x5/+0x6/+0x7 = 0
 *   8004728c  addiu v0,zero,3
 *   80047290  sw    v0,4(s1)      ; Wort = 3 (DEATH)            -> +0x5/+0x6/+0x7 = 0
 *   80047324  sb    s5,5(s1)      ; ERST DANACH +0x5 = Waffen-Id (a3 & 0xff)
 * Zweiter Applier-Zweig identisch: @0x800474A8 / @0x800474AC / @0x800474B0 / @0x800474B4 und
 * @0x80047574. Unabhaengige Gegenprobe im Decompilat RE2_Quellcode_V2/FUN_800470c0.c Z.48/50:
 *   `puVar5[1] = 2;`  ...  `puVar5[1] = 3;`      (puVar5 ist `uint*` => 32-Bit-Store)
 * Ebenso der Glieder-Hitscan FUN_800410CC: @0x800418EC `sw v0(=2),4(t0)` / @0x800418FC
 * `sw v0(=3),4(t0)`.
 * Voll-Aufzaehlung ALLER Stores auf Entity+0x4 in der RE2-EXE (94 Treffer, funktionslokal auf
 * Basisregister gefiltert, die in derselben Funktion mit em-Offsets 0x156/0x1D2/0x1D3/0x10E/
 * 0x94/0x1EE benutzt werden): darunter nur SECHS Byte-Stores, und keiner trifft ein em —
 *   0x80051F08/0x80051F3C (Spieler ueber 0x800CE330),
 *   0x8005D814/0x8005DC24/0x8005DE14 (Spieler/Partner ueber 0x800CFBD8/0x800CFE18),
 *   0x80055CE8 = der SCD-Opcode `Member_set` (Sprungtabelle @0x80011228, 44 Faelle:
 *                idx2 = +0x4, idx3 = +0x5, idx4 = +0x6, idx5 = +0x7) — Skript, kein KI-Pfad.
 * Im Hundemodul EMD0G_MOD0.BIN gibt es KEINEN Byte-Store auf +0x4 (0x80100DD4/0x80100E10/
 * 0x80100E24 liegen auf +0x21C: Basis s1 = self+0x218, `lh 2(s1)` = +0x21A Fatigue, `slti …,451`).
 * FUN_80047664 (Radius-Schaden) scheidet aus: seine Liste 0x800CFBD8+572 = 0x800D023C ist die
 * SPIELER-Liste (Schleifenzahl s4 = 1|2 = Partner), der em-Applier laeuft ueber 0x800D3C34.
 *
 * ===========================================================================================
 * RIEGEL 2 — die einzige +0x6=3-Quelle im Tod haengt an HP >= 0
 * ===========================================================================================
 * In DEATH kann +0x6 nur ueber die geteilte Landung 0x801034C8 (= @0x80105668[1], ebenso
 * @0x80105688[1] / @0x80105698[1]) auf 3 gehen:
 *   80103534  jal   0x80015fe8            ; rand
 *   8010353c  andi  v0,v0,0xf
 *   80103540  addiu v1,zero,1028          ; 0x404 -> Bits 2 und 10 => 2/16 der Zuege
 *   80103544  srlv  v1,v1,v0
 *   80103548  andi  v1,v1,0x1
 *   8010354c  beq   v1,zero,0x8010359c    ; -> HARD
 *   80103554  lbu   v0,547(s0) / andi 0x80 / bne -> HARD          (+0x223)
 *   80103568  lh    v0,342(s0)            ; HP
 *   80103570  bltz  v0,0x8010359c         ; **HP < 0 -> IMMER HARD**
 *   80103578  lbu   v0,557(s0) / bne -> HARD                      (+0x22D)
 *   80103598  addiu v0,zero,3             ; SOFT
 *   801035a4  addiu v0,zero,2             ; HARD
 *   801035a8  sh    v0,6(s0)              ; Halbwort: +0x6 = 3|2 UND +0x7 = 0
 * In state 3 ist HP per Konstruktion < 0: jeder Uebergang nach 3 faellt unmittelbar aus einem
 * `bgez HP` (@0x80047284, @0x800474A8, @0x800418F0, @0x80065C58), und der EINZIGE HP-Store im
 * ganzen Hundemodul ist `sh v0,342(s0)` @0x8010387C — der steckt IN 0x801037E8 selbst.
 * Die beiden anderen +0x6=3-Schreiber (@0x80103A34 schreibt 259 = 0x0103, @0x80103BDC schreibt
 * 3) haengen am Gore-Router 0x801038C0, der NUR aus dem HURT-Root 0x801032A8 erreicht wird.
 * Praeziser Rueckwaerts-Dataflow ueber ALLE 103 `sb/sh rt,6|7(rs)` im Modul: kein einziger
 * Store setzt +0x6 > 3 — damit ist auch @0x80105588[4] (HURT-P4) unerreichbar.
 *
 * ===========================================================================================
 * GEGENPROBE — selbst ERZWUNGEN gaebe es kein „steht wieder auf"
 * ===========================================================================================
 * 0x801037E8 setzt `+0x1D3 |= 0x80` (@0x8010389C-A4). Beide Applier steigen bei +0x1D3 != 0
 * sofort aus (`lbu v0,467(s0)` / `bne v0,zero,<continue>` @0x80047138-40 bzw. @0x800476EC-F4);
 * der Root-Tick dekrementiert nur die LOW-7 (`andi v0,v1,0x7f` @0x80100030) und laesst Bit 0x80
 * stehen; geloescht wird es NUR in HURT-P2 (@0x80103710) und HURT-P3 (@0x801037A4), also in
 * state 2 — den ein unantastbarer Hund nie mehr erreicht. Die Erzaehlung „der naechste Treffer
 * holt ihn per `bgez` @0x80047284 zurueck" ist damit auch inhaltlich widerlegt.
 *
 * ===========================================================================================
 * DIE PINS
 * ===========================================================================================
 *  (A) RIEGEL 2, negativ: state 3 + Phase 1 + Landung, HP < 0 -> ueber 2048 RNG-Zuege NIE
 *      Phase 3, immer Phase 2 (`bltz` @0x80103570 / `addiu v0,zero,2` @0x801035A4).
 *  (A') POSITIV-KONTROLLE (der Detektor ist nicht blind): dieselbe Landung mit HP >= 0
 *      liefert Phase 3 in ~2/16 der Zuege (Bitmuster 1028 @0x80103540).
 *  (B) RIEGEL 1: der RE2-Schadenspfad hinterlaesst nach Treffer UND nach Kill +0x6 = 0 und
 *      +0x7 = 0 — genau das Wort-`sw` @0x80047288/@0x80047290. NEGATIV dazu: es bleibt NICHT
 *      der RE1.5-Stempel +0x6 = 1 (@0x80012FD8) stehen.
 *  (C) Halbwort-Semantik von @0x80103430/@0x80103450: HURT-P0 nullt +0x7 mit.
 *  (D) RE1.5-REGRESSIONSWACHE: unter RE15_AI_FLAVOR_RE15 bleibt der RE1.5-Stempel +0x6 = 1 /
 *      +0x7 = 0 (@0x80012FD8/@0x80012FD4) unangetastet — der RE2-Riegel faerbt nicht ab.
 */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } \
                           else { printf("  PASS: " __VA_ARGS__); printf("\n"); } } while (0)

/* Einen Hund in „DEATH, Phase 1, im selben Tick auf dem Boden aufschlagend" setzen und einen
 * einzigen Root-Tick fahren. Der Dispatch ist der echte:
 *   re15_re2dog_tick -> state 3 -> @0x80105668[+0x6 == 1] = 0x801034C8 (geteilte Landung).
 * re2z_prev_hp == hp haelt die Treffer-Erkennung im Root-Tick still. */
static int land_once(int slot, int hp, int state)
{
    re15_actor_t *e = &g_actors[slot];
    e->state        = (uint8_t)state;
    e->sub_state_1  = 0;
    e->sub_state_2  = 1;                /* Phase 1 = Luft/Landung */
    e->sub_state_3  = 0;
    e->hp           = (int16_t)hp;
    e->re2z_prev_hp = (int16_t)hp;
    e->re2d_dbl223  = 0;                /* +0x223 & 0x80 == 0  (@0x80103554-60) */
    e->re2d_wound22d = 0;               /* +0x22D == 0         (@0x80103578-80) */
    e->re2d_vy146   = 0;
    e->dog_floor_y  = 0;
    e->y            = 0;                /* y >= floor -> Landung im selben Tick @0x80103504-08 */
    re15_re2dog_tick(slot);
    return (int)e->sub_state_2;
}

static void pin_hp_lock(void)
{
    printf("=== (A)/(A') RIEGEL 2: `bltz +0x156` @0x80103570 sperrt die Soft-Landung im Tod ===\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init();
    re15_enemy_reset();
    re15_enemy_ai_set_paused(0);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->y = 0; pl->z = 0;

    const int slot = 1;
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->x = 2000; e->z = 0;

    const int N = 2048;
    int dead_soft = 0, dead_hard = 0, dead_other = 0;
    for (int i = 0; i < N; i++) {
        int ph = land_once(slot, -7, 3);           /* TOT: HP < 0 */
        if (ph == 3)      dead_soft++;
        else if (ph == 2) dead_hard++;
        else              dead_other++;
    }
    int alive_soft = 0, alive_hard = 0, alive_other = 0;
    for (int i = 0; i < N; i++) {
        int ph = land_once(slot, 50, 3);           /* dieselbe Landung, nur HP >= 0 */
        if (ph == 3)      alive_soft++;
        else if (ph == 2) alive_hard++;
        else              alive_other++;
    }
    printf("  tot (HP<0): Phase3=%d Phase2=%d sonst=%d | HP>=0: Phase3=%d Phase2=%d sonst=%d\n",
           dead_soft, dead_hard, dead_other, alive_soft, alive_hard, alive_other);

    CHECK(dead_soft == 0,
          "(A) NEGATIV: in %d Landungen mit HP < 0 wird die Soft-Phase 3 NIE gesetzt "
          "(`bltz v0,0x8010359c` @0x80103570 -> `addiu v0,zero,2` @0x801035A4): %d Treffer",
          N, dead_soft);
    CHECK(dead_hard == N,
          "(A) alle %d toten Landungen enden in Phase 2 = 0x80104200 (Rutschen -> CORPSE): %d",
          N, dead_hard);
    CHECK(alive_soft > 0,
          "(A') POSITIV-KONTROLLE: mit HP >= 0 feuert die Soft-Landung sehr wohl (%d von %d) — "
          "der Detektor ist nicht blind, HP ist der einzige Riegel", alive_soft, N);
    CHECK(alive_soft * 16 < N * 4 && alive_soft * 16 > N / 2,
          "(A') die Soft-Rate liegt in der Groessenordnung 2/16 (Bitmuster 1028 = Bits 2|10 "
          "@0x80103540-4C): %d von %d", alive_soft, N);
}

/* ---------------------------------------------------------------------------------------- */
static void stamp_and_read(int slot, int hp_before, uint8_t atk, int *ph, int *guard, int *st)
{
    re15_actor_t *e = &g_actors[slot];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x20; e->x = 2000; e->z = 0;
    e->hp = (int16_t)hp_before;
    e->sub_state_1 = 2; e->sub_state_2 = 3; e->sub_state_3 = 1;   /* „ueberlebende" Phase */
    e->hit_react = 0;
    re15_enemy_take_damage(e, atk);
    *ph = (int)e->sub_state_2; *guard = (int)e->sub_state_3; *st = (int)e->state;
}

static void pin_applier_word_store(void)
{
    printf("=== (B)/(D) RIEGEL 1: der Treffer nullt +0x6/+0x7 (Wort-`sw` @0x80047288/90) ===\n");
    re15_actor_init(); re15_enemy_reset();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100;

    int ph, gd, st;

    /* Angriffstyp 0 = 10 Schaden (re15_damage_table[0]) -> ueberlebt; Typ 2 = 1000 -> toedlich. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    stamp_and_read(1, 200, 0, &ph, &gd, &st);      /* Treffer, ueberlebt -> HURT */
    printf("  RE2 ueberlebt: state=%d +0x6=%d +0x7=%d\n", st, ph, gd);
    CHECK(st == 2 && ph == 0 && gd == 0,
          "(B1) RE2-Treffer: state 2 und +0x6/+0x7 = 0/0 (Wort-`sw v0,4(s1)` @0x80047288) — "
          "gemessen %d / %d / %d", st, ph, gd);
    CHECK(ph != 1,
          "(B1-NEG) es bleibt NICHT der RE1.5-Stempel +0x6 = 1 (@0x80012FD8) stehen");

    stamp_and_read(1, 1, 2, &ph, &gd, &st);        /* Treffer, toedlich -> DEATH */
    printf("  RE2 toedlich:  state=%d +0x6=%d +0x7=%d\n", st, ph, gd);
    CHECK(st == 3 && ph == 0 && gd == 0,
          "(B2) RE2-Kill: state 3 und +0x6/+0x7 = 0/0 (`sw v0,4(s1)` @0x80047290) — DEATH "
          "startet IMMER bei Phase 0 (@0x80105618[+0x5]), nie bei @0x80105668[3] = 0x801037E8; "
          "gemessen %d / %d / %d", st, ph, gd);

    /* (D) RE1.5-Regressionswache: der RE1.5-Pfad behaelt seinen eigenen Stempel. */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    stamp_and_read(1, 200, 0, &ph, &gd, &st);
    printf("  RE1.5 ueberlebt: state=%d +0x6=%d +0x7=%d\n", st, ph, gd);
    CHECK(st == 2 && ph == 1 && gd == 0,
          "(D) REGRESSIONSWACHE RE1.5: unveraendert +0x6 = 1 (@0x80012FD8) / +0x7 = 0 "
          "(@0x80012FD4) — der RE2-Riegel faerbt nicht auf den RE1.5-Hund ab; gemessen "
          "%d / %d / %d", st, ph, gd);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
}

/* ---------------------------------------------------------------------------------------- */
static void pin_p0_halfword(void)
{
    printf("=== (C) Halbwort-Semantik @0x80103430/@0x80103450: HURT-P0 nullt +0x7 mit ===\n");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    re15_actor_init(); re15_enemy_reset(); re15_enemy_ai_set_paused(0);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->x = 0; pl->z = 0; pl->rot_y = 0;

    const int slot = 1;
    re15_actor_t *e = &g_actors[slot];

    /* Boden-Treffer -> `sh 2,6` @0x80103450 ; Luft-Treffer -> `sh 1,6` @0x80103430. */
    for (int air = 0; air <= 1; air++) {
        memset(e, 0, sizeof *e);
        e->active = 1; e->type = 0x20; e->x = 2000; e->z = 0;
        e->state = 2; e->sub_state_1 = 2; e->sub_state_2 = 0;
        e->sub_state_3 = 1;                       /* Guard steht — das Halbwort muss ihn loeschen */
        e->hp = 60; e->re2z_prev_hp = 60;
        e->re2d_air219 = (uint8_t)air;
        e->re2d_dbl223 = 0; e->re2d_wound22d = 0;
        re15_re2dog_tick(slot);
        printf("  air=%d -> +0x6=%d +0x7=%d\n", air, (int)e->sub_state_2, (int)e->sub_state_3);
        CHECK((int)e->sub_state_2 == (air ? 1 : 2) && e->sub_state_3 == 0,
              "(C) HURT-P0 %s: `sh %d,6(s1)` @%s schreibt +0x6 UND nullt +0x7 — gemessen "
              "%d / %d", air ? "Luft" : "Boden", air ? 1 : 2,
              air ? "0x80103430" : "0x80103450", (int)e->sub_state_2, (int)e->sub_state_3);
    }
}

int main(void)
{
    pin_hp_lock();
    pin_applier_word_store();
    pin_p0_halfword();
    printf(fails ? "\nFEHLGESCHLAGEN: %d\n" : "\nALLE PINS GRUEN (%d Fehler)\n", fails);
    return fails ? 1 : 0;
}
