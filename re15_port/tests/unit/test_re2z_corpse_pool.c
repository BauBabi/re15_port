/**
 * @file test_re2z_corpse_pool.c
 * @brief REGRESSIONSWACHE fuer die zwei Nutzer-Befunde vom 2026-08-21 am RE2-Leichnam:
 *
 *  (A) "Der am Boden getoetete Zombie laeuft SOFORT aus, nicht langsam wie normal."
 *      = die Blutlache unter der Leiche ist schlagartig voll statt zu wachsen.
 *  (B) "Ausserdem gibt es keine finale Todesanimation."
 *
 * URSACHE (eine Wurzel): der Port fuhr von der RE2-Leichen-Wurzel FUN_8010A440 nur die Zelle 0
 * und hielt danach. Die elf uebrigen Zellen waren als "presentation-only" abgetan — sie sind es
 * NICHT. Selbst disassembliert (info/re2leon/COMMON/BIN/EMOVL10_S0.BIN, roh @0x80100000):
 *
 *   ROUTER @0x8010a440-8c:  `lbu v1,5(s0)` @0x8010a460 (+0x5), `lw s1,364(s0)` @0x8010a464
 *     (= [+0x16C], der Boden-Schatten-/Prim-Record), `sltiu v0,v1,0xc` @0x8010a468 (12 Zellen),
 *     `beq -> 0x8010a80c` @0x8010a46c, `lw v0,412(at)` @0x8010a480 = Tabelle @0x8010019c.
 *   Tabelle @0x8010019c: [0]=0x8010a490 [1]=0x8010a5d8 [2]=0x8010a60c [3]=0x8010a64c
 *     [4]=0x8010a69c [5]=0x8010a7e0 [6]=0x8010a6bc [7]=0x8010a80c [8]=0x8010a768
 *     [9]=0x8010a7b4 [10]=0x8010a7e0 [11]=0x8010a808.
 *
 *   ZELLE 0 (INIT) @0x8010a490: Clip 0x00070017 (23), bzw. 0x00070016 (22) wenn +0x21A&4
 *     (@0x8010a4a0-bc); Leichen-Tint [+0x16C]+28/+68 = (alt&0xff000000)|0x00BFBF10
 *     (@0x8010a4c0-508); hp = -1 (@0x8010a4d4); +0x5 = 1 (@0x8010a4e0); **+0x16A = 120**
 *     (@0x8010a508-10); +0x158 = 40 (@0x8010a514-18); speed = 0 (@0x8010a524); dann EIN
 *     RNG-Wurf (@0x8010a59c) und die 1/3-2/3-Weiche: `rand % 3 != 0` -> +0x5 = 8 und EPILOG
 *     (@0x8010a5a4-d0), sonst +0x15A = 0 (@0x8010a5d4) und FALLTHROUGH in Zelle 1.
 *   DER GROWER, wortgleich in Zelle 1 (@0x8010a5f0-608), Zelle 3 (@0x8010a664-678) und
 *     Zelle 8 (@0x8010a780-94):  `lhu [+0x16C]+4 / +6; addiu +5; sh` — je Tick, solange
 *     +0x16A != 0 (`lb v0,362` / `beq`, Dec @0x8010a5e8/@0x8010a65c/@0x8010a778).
 *   DER ADVANCE (Zelle 5 UND 10) @0x8010a7e0: `jal 0x8002959c` @0x8010a7ec mit a3 = 512
 *     (@0x8010a7f0), `+0x5 += Rueckgabe` @0x8010a7f4-804. Der Port-Advancer laesst state 7 aus
 *     (`if (a->state == RE15_AI_STATE_CORPSE) continue;`, player_common.c) — voellig richtig,
 *     denn im Original advanct die Leichen-Maschine SELBST. Ohne diesen Advance steht der
 *     Leichen-Clip fuer immer auf Frame 0 = Befund (B).
 *   DIE ZUCK-TAKTUNG: Zelle 2 @0x8010a60c (+0x16B = (rand&0xf)+1, +0x5 = 3, +0x158--),
 *     Zelle 4 @0x8010a69c, Zelle 6 @0x8010a6bc (+0x16B = (rand&0x3f)+60, +0x158-- -> +0x5 = 7).
 *
 * GEMESSEN vor dem Fix (probe_re2z_corpse, ROOM1140, echter Weg game_step+Pad R1/SQUARE,
 * RE2-Bank, 64 Seeds): 64/64 Leichen mit POOL_HX first=min=max=last=1320 (Wachstums-Spanne 0),
 * in state 7 nur die +0x5-Werte {0,1}, Clip 22 auf anim_frame 0 / Keyframe 722 ueber 900 Frames.
 * NACH dem Fix: 0/64 fuer alle vier Detektoren, Wachstum 600 -> 1200 ueber 120 Ticks,
 * +0x5-Werte {0,1,2,3,4,5,6} bzw. {0,7,8}. RE1.5-Kontrolle unveraendert (608 -> 1320, {0..4}).
 *
 * ⚠ OFFEN und bewusst nicht gebaut: die BASIS der Halb-Ausdehnungen. Das Original waechst auf
 * dem Wert weiter, den [+0x16C]+4/+6 beim Spawn haben; einen Overlay-Schreiber dafuer gibt es
 * NICHT (voller Scan von EMOVL10_S0.BIN nach `lw rX,364(rY)` findet genau ZWEI Stellen:
 * @0x80109ea8 ist eine Sprungtabelle, @0x8010a464 diese Wurzel). Die Basis setzt der
 * RE2-EXE-Spawnpfad, den der Port nicht hat — der Port legt den Grower deshalb auf seine schon
 * zitierte Zombie-Schattenbasis (FUN_8001af5c-Argumente 0x258/0x2bc = 600/700). Byte-true sind
 * RATE (+5/Tick), BUDGET (120 Ticks) und FARBE (0x00BFBF10).
 *
 * Die Pins brauchen die RE2-Bank EM010 (CDEMD0.EMS). Fehlt sie, endet der Test mit 77 (SKIP) —
 * ohne Bank ist re15_actor_clip_len() == 0 und jede clip-getriebene Aussage waere leer.
 */
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"
#include "re15_damage.h"
#include "re15_emd.h"
#include "re2_ems.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

extern int      re15_actor_clip_len(const re15_actor_t *a);
extern void     re15_actors_anim_advance(void);
extern void     re15_re2z_rng_reset(void);
extern uint32_t re15_re2_rand(void);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static uint8_t *s_ems = NULL; static size_t s_ems_n = 0;
static int load_bank2(uint8_t type)
{
    if (!s_ems) s_ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &s_ems_n);
    if (!s_ems) return 0;
    re15_enemy_bank_t *eb = re15_enemy_find(type);
    if (eb && eb->ok) return 1;
    if (!eb) eb = re15_enemy_alloc(type);
    if (!eb) return 0;
    if (re2_ems_load_bank(s_ems, s_ems_n, (int)type, eb, NULL) == 0) { eb->buf = NULL; eb->ok = 1; return 1; }
    eb->type = 0; return 0;
}

/* Eine frische RE2-Leiche in Zelle 0, mit definiertem RNG-Vorlauf (waehlt die 1/3-2/3-Weiche). */
static int spawn_corpse(int rng_skip, uint16_t flags21a)
{
    re15_re2z_rng_reset();
    for (int i = 0; i < rng_skip; i++) (void)re15_re2_rand();
    int s = re15_actor_alloc(0x10);
    if (s < 0) return -1;
    re15_actor_t *e = &g_actors[s];
    e->x = 4000; e->y = 0; e->z = 0; e->rot_y = 0;
    e->state = (uint8_t)RE15_AI_STATE_CORPSE;      /* +0x4 = 7 */
    e->sub_state_1 = 0; e->sub_state_2 = 0;        /* +0x5 = 0 -> Zelle 0 */
    e->re2z_flags21a = flags21a;
    e->re2z_f10e = 0; e->grid_id = 0; e->hp = 10;
    e->crow_shadow_w = 0; e->crow_shadow_h = 0; e->crow_pool = 0;
    return s;
}

/* ============================================================================================
 * PIN A — DER GROWER IST BYTE-TRUE: +5 pro Tick, GENAU 120 Ticks (+0x16A), dann Stillstand.
 * ========================================================================================== */
static void pin_a_pool_grower(void)
{
    int seen_branch1 = 0, seen_branch8 = 0;

    for (int k = 0; k < 12; k++) {
        re15_actor_init(); re15_enemy_ai_set_paused(0);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_damage_seed_rng(0x0badf00du);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;

        int s = spawn_corpse(k, 0);
        if (s < 0) { CHECK(0, "kein Aktor-Slot"); return; }
        re15_actor_t *e = &g_actors[s];

        re15_re2z_tick(s);                                    /* Zelle 0 */

        /* ---- der Init aus @0x8010a490-518 ----
         * ⚠ Zelle 0 FAELLT im 1/3-Zweig noch im SELBEN Tick nach Zelle 1 durch (@0x8010a5d4 ->
         * @0x8010a5d8 ohne Sprung) — dort ist die Lache also schon EINMAL gewachsen. Der
         * 2/3-Zweig verlaesst die Funktion dagegen ueber `j 0x8010a904` (@0x8010a5cc). */
        CHECK(e->sub_state_1 == 1 || e->sub_state_1 == 8,
              "k=%d: die Weiche @0x8010a5a4-d0 fuehrt nach +0x5 = 1 oder 8, nicht %u",
              k, e->sub_state_1);
        int fell = (e->sub_state_1 != 8);               /* 1/3-Zweig = Fallthrough in Zelle 1 */
        if (fell) seen_branch1 = 1; else seen_branch8 = 1;

        CHECK(e->crow_pool == 1,
              "k=%d: Zelle 0 muss die Lachen-Faerbung setzen (Tint 0x00BFBF10 @0x8010a4c0-508)", k);
        CHECK(e->crow_shadow_w == (uint16_t)(600 + 5 * fell) &&
              e->crow_shadow_h == (uint16_t)(700 + 5 * fell),
              "k=%d: Lachen-Basis (Port-Basis af5c 0x258/0x2bc) + %d Grower-Schritt erwartet"
              " %d/%d, ist %u/%u", k, fell, 600 + 5 * fell, 700 + 5 * fell,
              e->crow_shadow_w, e->crow_shadow_h);
        CHECK(e->re2z_dir16a == (uint8_t)(120 - fell),
              "k=%d: +0x16A muss %d sein (`addiu v0,zero,120` @0x8010a508 / `sb v0,362`"
              " @0x8010a510, minus der Fallthrough-Schritt), ist %u",
              k, 120 - fell, e->re2z_dir16a);
        CHECK(e->re2z_t158 == 40,
              "k=%d: +0x158 muss 40 sein (@0x8010a514-18), ist %d", k, (int)e->re2z_t158);
        CHECK(e->hp == -1, "k=%d: hp muss -1 sein (`sh -1,342` @0x8010a4d4), ist %d", k, e->hp);

        /* ---- die INVARIANTE ueber die ganze Lebensdauer der Lache ----
         * w == 600 + 5*(120 - +0x16A), h == 700 + 5*(120 - +0x16A). */
        int bad = -1, ticks_to_full = -1;
        for (int t = 1; t <= 400; t++) {
            re15_re2z_tick(s);
            int used = 120 - (int)e->re2z_dir16a;
            if (bad < 0 && (e->crow_shadow_w != (uint16_t)(600 + 5 * used) ||
                            e->crow_shadow_h != (uint16_t)(700 + 5 * used))) bad = t;
            if (ticks_to_full < 0 && e->re2z_dir16a == 0) ticks_to_full = t;
        }
        CHECK(bad < 0,
              "k=%d: Grower-Invariante verletzt ab Tick %d (w=%u h=%u bei +0x16A=%u) — die Rate"
              " ist +5 (@0x8010a5f0-608 / @0x8010a664-678 / @0x8010a780-94)",
              k, bad, e->crow_shadow_w, e->crow_shadow_h, e->re2z_dir16a);
        CHECK(ticks_to_full == 120 - fell,
              "k=%d: das Budget +0x16A = 120 muss nach GENAU 120 Wachstums-Ticks leer sein"
              " (davon %d schon im Init-Tick), gemessen %d", k, fell, ticks_to_full);
        CHECK(e->crow_shadow_w == 1200 && e->crow_shadow_h == 1300,
              "k=%d: Endgroesse muss Basis + 5*120 sein (1200/1300), ist %u/%u",
              k, e->crow_shadow_w, e->crow_shadow_h);
        CHECK(e->crow_pool == 1, "k=%d: die Faerbung darf nicht verloren gehen", k);
        re15_actor_free(s);
    }
    CHECK(seen_branch1 && seen_branch8,
          "die 1/3-2/3-Weiche @0x8010a5c4 muss BEIDE Zweige erreichen (Zelle 1 gesehen=%d,"
          " Zelle 8 gesehen=%d)", seen_branch1, seen_branch8);
    printf("  [A] Grower +5/Tick x 120 -> 600/700 auf 1200/1300, beide Weichen-Zweige erreicht\n");

    /* ---- NEGATIV-KONTROLLE: mit leerem Budget (+0x16A == 0) darf NICHTS wachsen ---------- */
    {
        re15_actor_init(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_damage_seed_rng(0x0badf00du);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        int s = spawn_corpse(0, 0);
        re15_actor_t *e = &g_actors[s];
        re15_re2z_tick(s);                          /* Zelle 0 setzt Basis + Budget */
        e->re2z_dir16a = 0;                         /* Budget kuenstlich leeren */
        uint16_t w0 = e->crow_shadow_w, h0 = e->crow_shadow_h;
        for (int t = 0; t < 60; t++) re15_re2z_tick(s);
        CHECK(e->crow_shadow_w == w0 && e->crow_shadow_h == h0,
              "NEGATIV-KONTROLLE: bei +0x16A == 0 darf die Lache NICHT wachsen (Gate `lb v0,362`"
              " / `beq` @0x8010a5d8-e0 / @0x8010a64c-54 / @0x8010a768-70) — %u/%u -> %u/%u",
              w0, h0, e->crow_shadow_w, e->crow_shadow_h);
        printf("  [A-NEG] Budget leer -> Lache bleibt bei %u/%u\n", w0, h0);
        re15_actor_free(s);
    }
}

/* ============================================================================================
 * PIN B — DIE LEICHEN-MASCHINE LAEUFT UND ADVANCT IHREN CLIP SELBST.
 * ========================================================================================== */
static void pin_b_corpse_machine(void)
{
    /* ---- B0 NEGATIV-KONTROLLE: der globale Advancer ruehrt eine Leiche NICHT an.
     * Das ist der Beweis, dass der Advance aus der Leichen-Maschine kommen MUSS. ---------- */
    {
        re15_actor_init(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_damage_seed_rng(0x0badf00du);
        int s = spawn_corpse(0, 0);
        re15_actor_t *e = &g_actors[s];
        e->motion = 23; e->anim_frame = 3; e->anim_flags = 0;
        for (int t = 0; t < 30; t++) re15_actors_anim_advance();
        CHECK(e->anim_frame == 3,
              "NEGATIV-KONTROLLE: re15_actors_anim_advance darf state 7 auslassen"
              " (player_common.c), anim_frame %u nach 30 Ticks", e->anim_frame);
        printf("  [B0] globaler Advancer laesst die Leiche aus (anim_frame bleibt %u)\n",
               e->anim_frame);
        re15_actor_free(s);
    }

    /* ---- B1 Leichen-Pose: Clip 23, mit +0x21A&4 Clip 22 (@0x8010a490-bc) ---------------- */
    for (int v = 0; v < 2; v++) {
        re15_actor_init(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_damage_seed_rng(0x0badf00du);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        int s = spawn_corpse(0, v ? 0x4u : 0u);
        re15_actor_t *e = &g_actors[s];
        re15_re2z_tick(s);
        CHECK(e->motion == (v ? 22 : 23),
              "+0x21A&4=%d: Leichen-Clip muss %d sein (`ori v0,v0,0x17` @0x8010a494 bzw."
              " `ori v0,v0,0x16` @0x8010a4b8), ist %d", v, v ? 22 : 23, (int)e->motion);
        re15_actor_free(s);
    }

    /* ---- B2 die Maschine laeuft: Advance + Zuck-Taktung ---------------------------------- */
    int seen_adv = 0, seen_wrap = 0, seen_rest = 0, states_max = 0;
    for (int k = 0; k < 12; k++) {
        re15_actor_init(); re15_enemy_ai_set_paused(0);
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        re15_damage_seed_rng(0x0badf00du);
        g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
        g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;
        int s = spawn_corpse(k, 0);
        re15_actor_t *e = &g_actors[s];
        re15_re2z_tick(s);
        int fc = re15_actor_clip_len(e);
        CHECK(fc > 1, "k=%d: Clip-Laenge muss aus der geladenen Bank kommen (fc=%d)", k, fc);

        unsigned seen = 0; int adv = 0, wrapped = 0, pin_run = 0, pin_max = 0;
        int prev = (int)e->anim_frame;
        int branch8 = (e->sub_state_1 == 8);
        int rest_at = -1;
        for (int t = 0; t < 500; t++) {
            re15_re2z_tick(s);                    /* KEIN globaler Advancer — wie im Spiel */
            if (e->sub_state_1 < 16) seen |= (1u << e->sub_state_1);
            int af = (int)e->anim_frame;
            if (af != prev) adv = 1;
            if (af < prev) wrapped = 1;
            if (fc > 1 && af == fc - 1) { pin_run++; if (pin_run > pin_max) pin_max = pin_run; }
            else pin_run = 0;
            prev = af;
            if (rest_at < 0 && e->sub_state_1 == 7) rest_at = t;
        }
        int nbits = 0; for (int i = 0; i < 16; i++) if (seen & (1u << i)) nbits++;
        if (nbits > states_max) states_max = nbits;

        if (branch8) {
            /* Zelle 8 advanct NICHT (kein `jal 0x8002959c` in @0x8010a768-b0) und endet in
             * Zelle 7 GENAU wenn +0x16A leer ist. */
            CHECK(rest_at == 119,
                  "k=%d Zweig 8: die Ruhe-Zelle 7 muss GENAU beim Leerlaufen von +0x16A kommen"
                  " (@0x8010a798-b0), gemessen Tick %d", k, rest_at);
            CHECK((seen & ~((1u << 7) | (1u << 8))) == 0,
                  "k=%d Zweig 8: nur die Zellen 8 und 7 duerfen vorkommen, Maske 0x%04X",
                  k, seen);
            seen_rest = 1;
        } else {
            CHECK(adv, "k=%d Zweig 1: der Leichen-Clip MUSS advancen — die Maschine ruft"
                       " 0x8002959c selbst (@0x8010a7ec)", k);
            CHECK(wrapped, "k=%d Zweig 1: der Frame-Zaehler muss wrappen (`sb zero,333(s2)`"
                           " @0x80029B48)", k);
            CHECK(pin_max <= 1, "k=%d Zweig 1: kein PIN auf dem letzten Frame — %d Ticks am"
                                " Stueck auf fc-1", k, pin_max);
            CHECK(nbits >= 3, "k=%d Zweig 1: die Zuck-Taktung muss mehr als zwei Zellen"
                              " durchlaufen, gesehen %d (Maske 0x%04X)", k, nbits, seen);
            seen_adv = 1; if (wrapped) seen_wrap = 1;
        }
        re15_actor_free(s);
    }
    CHECK(seen_adv && seen_wrap && seen_rest,
          "beide Zweige muessen ueber die 12 RNG-Vorlaeufe vorkommen (adv=%d wrap=%d ruhe=%d)",
          seen_adv, seen_wrap, seen_rest);
    printf("  [B] Maschine laeuft: Clip advanct + wrappt, bis zu %d verschiedene Zellen,"
           " Zweig 8 ruht nach 120 Ticks\n", states_max);
}

/* ============================================================================================
 * PIN C — RE1.5-REGRESSIONSWACHE: der RE1.5-Modus bleibt unberuehrt.
 * ========================================================================================== */
static void pin_c_re15_guard(void)
{
    re15_actor_init(); re15_enemy_ai_set_paused(0);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_damage_seed_rng(0x0badf00du);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].hp = 100;

    int s = re15_actor_alloc(0x10);
    if (s < 0) { CHECK(0, "kein Aktor-Slot"); return; }
    re15_actor_t *e = &g_actors[s];
    e->x = 4000; e->y = 0; e->z = 0;
    e->state = (uint8_t)RE15_AI_STATE_CORPSE; e->sub_state_1 = 0; e->sub_state_2 = 0;
    e->hp = -1; e->grid_id = 0; e->motion = 0x0b;
    e->crow_shadow_w = 0; e->crow_shadow_h = 0; e->crow_pool = 0;

    re15_enemy_ai_live_tick(s);                    /* RE1.5-Wurzel -> re15_enemy_corpse_settle */
    CHECK(e->grab_kill_ctr == 0x5a || e->grab_kill_ctr == 0x59,
          "RE1.5-Leichensettle muss das Pool-Budget +0x9E = 0x5a setzen (@0x8010959c), ist %d",
          (int)e->grab_kill_ctr);
    CHECK(e->sub_state_1 == 1,
          "RE1.5-Leichensettle muss in Sub 1 stehen, ist %u", e->sub_state_1);

    unsigned seen = 0;
    for (int t = 0; t < 300; t++) { re15_enemy_ai_live_tick(s); seen |= (1u << (e->sub_state_1 & 15)); }
    CHECK(e->crow_shadow_w == 0 && e->crow_shadow_h == 0 && e->crow_pool == 0,
          "RE1.5-Modus darf den RE2-Lachen-Kanal NICHT anfassen (der Renderer nimmt dort weiter"
          " die +0x9E-Formel) — w=%u h=%u pool=%u",
          e->crow_shadow_w, e->crow_shadow_h, e->crow_pool);
    CHECK((seen & (1u << 2)) && (seen & (1u << 3)),
          "RE1.5-Zuckmaschine (Subs 2/3 @0x80109824/@0x80109884) muss weiter laufen,"
          " Maske 0x%04X", seen);
    printf("  [C] RE1.5-Leichensettle unveraendert (+0x9E-Kanal), RE2-Kanal bleibt 0\n");
    re15_actor_free(s);
}

int main(void)
{
    if (!load_bank2(0x10)) {
        printf("SKIP: RE2-Bank EM010 (CDEMD0.EMS) nicht ladbar — ohne sie ist"
               " re15_actor_clip_len()==0 und jeder clip-getriebene Pin waere leer\n");
        return 77;
    }
    pin_a_pool_grower();
    pin_b_corpse_machine();
    pin_c_re15_guard();
    if (fails) { printf("\n%d FEHLER\n", fails); return 1; }
    printf("\nOK: RE2-Leichen-Lache waechst byte-true, die Leichen-Maschine laeuft,"
           " RE1.5 unveraendert\n");
    return 0;
}
