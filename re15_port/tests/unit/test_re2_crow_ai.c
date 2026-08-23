/* test_re2_crow_ai.c — pins the RE2 CROW brain (WELLE D, OPTIONS -> AI -> RE2) to the RE2
 * crow module bytes.
 *
 * Source of truth: info/re2leon/COMMON/BIN/EMOVL21_S0.BIN (== CDEMD0.EMS Sektor 0x528),
 * loaded RAW @0x80100000 (Slot 0). Disassemble with
 *     .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMOVL21_S0.BIN
 * Every assertion cites its module address (all re-disassembled for Welle D, scratchpad
 * crow_self.asm). No enemy bank is loaded -> clips report done immediately; assertions that
 * depend on real frame counts live in test_re2_room10c0_ab.c / the live smoke instead. */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- SE capture hook --------------------------------------------------------------------- */
static int se_log[512]; static int se_fl[512]; static int se_n = 0;
static int se_bank_sel = -1;
static void se_cap(int id, int flag2000)
{ if (se_n < 512) { se_log[se_n] = id; se_fl[se_n] = flag2000; se_n++; } }
static void se_bank_cap(int bank) { se_bank_sel = bank; }
static int  se_seen(int id) { for (int i = 0; i < se_n; i++) if (se_log[i] == id) return 1; return 0; }
static int  se_count(int id) { int n = 0; for (int i = 0; i < se_n; i++) if (se_log[i] == id) n++; return n; }

static re15_actor_t *PL;
static re15_actor_t *CROW;

static void face_player(re15_actor_t *e, const re15_actor_t *pl)
{
    e->rot_y = (int16_t)(((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff);
}

static void fresh_crow(int32_t px, int32_t pz)
{
    re15_actor_init();                                     /* reseeds RE2-PRNG + g_re2_room_gflags */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL   = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    CROW = &g_actors[1];
    memset(PL,   0, sizeof *PL);
    memset(CROW, 0, sizeof *CROW);
    PL->active = 1; PL->hp = 100; PL->x = px; PL->z = pz;
    CROW->active = 1; CROW->type = 0x21; CROW->state = 0;
    se_n = 0;
}

static void tick_slot(int slot)
{
    re15_actor_t *e = &g_actors[slot];
    int32_t dx = PL->x - e->x, dz = PL->z - e->z;
    int64_t d2 = (int64_t)dx * dx + (int64_t)dz * dz;
    uint32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
    e->ai_dist = r;
    re15_re2crow_tick(slot);
}
static void tick(void) { tick_slot(1); }

/* =============================== tests ===================================================== */

static void test_init_wake_takeoff(void)
{
    fresh_crow(20000, 0);
    tick();                                                /* INIT @0x801002FC */
    CHECK(CROW->state == 1 && CROW->sub_state_1 == 0,
          "INIT: sw 1,4 cleart +0x5/6/7 (@0x8010032C), state=%d sub=%d", CROW->state, CROW->sub_state_1);
    CHECK(CROW->hp == 10, "HP=10 (sh 10,342 @0x80100324/348), hp=%d", CROW->hp);
    CHECK(CROW->hit_radius_min == 96 && CROW->hit_height == 512,
          "Hitbox-Record 96/96/512 (@0x801003A0-B4), r=%d h=%d", CROW->hit_radius_min, CROW->hit_height);
    CHECK(CROW->crow_shadow_w == 200 || CROW->crow_shadow_w == 400,
          "Schatten-Init 200x200 (0x80016480 a2=0xC800C8 @0x80100408-18; Root-Tail setzt am Boden "
          "400 @0x801002E4-F0), w=%d", CROW->crow_shadow_w);
    CHECK(CROW->target_z == -350, "INIT-Pitch +0x98=-350 (@0x801003B8-C4), tz=%d", CROW->target_z);

    /* fern: Idle haelt (DEC-Gates @0x80100704/0x80100728 zu) */
    for (int i = 0; i < 40; i++) tick();
    CHECK(CROW->state == 1 && CROW->sub_state_1 == 0,
          "dist 20000: IDLE haelt, sub=%d", CROW->sub_state_1);

    /* nah -> ALARM (dist<=1800 sltiu 0x709 @0x80100704) -> Takeoff nach rand&7 Ticks
     * (@0x80100BCC-D8) -> (bankless clips) Sub 4 Kreisen (@0x80100E94-98) */
    PL->x = CROW->x + 1500; PL->z = CROW->z;
    tick();
    CHECK(CROW->sub_state_1 == 1, "dist<=1800 -> Sub 1 ALARM (0x80104088(1) @0x80100788), sub=%d",
          CROW->sub_state_1);
    int got2 = 0, got4 = 0;
    for (int i = 0; i < 20; i++) {
        tick();
        if (CROW->sub_state_1 == 2) got2 = 1;
        if (CROW->sub_state_1 == 4) { got4 = 1; break; }
    }
    CHECK(got2, "ALARM -> Sub 2 ABHEBEN (@0x80100C0C-14)");
    CHECK(got4, "Takeoff-Clip done -> Sub 4 KREISEN (@0x80100E84-98)");
    /* ENEMSE-Bank 7 (korrigiert 2026-08-20, Nutzer-Report "bei RE2-AI haben die Kraehen den
     * falschen Sound"). Der VORHERIGE Pin stand auf 21 mit der Begruendung "Paar-Zeile 21 =
     * {0x21,0x00} @0x800A7400" — diese Begruendung ist WIDERLEGT: die Tabelle @0x800A7400
     * fuehrt keine Gegner-kinds, sondern die SOUND-ID aus dem Sce_em_set-Record (+7). Kurz:
     *   - Sce_em_set = Opcode 0x44 (Dispatch @0x800A74C8, [0x44] @0x800A75D8 = 0x8005714C);
     *     `lbu a0,3(v0)` @0x800571EC -> jal 0x8001b710 = kind, `lbu v0,7(v0)` @0x80057274 ->
     *     `sb v0,506(s0)` @0x80057280 = Sound-Id; verglichen wird NUR +0x1FA (`lb v1,506(a0)`
     *     @0x80052C48). Die Tabelle fuehrt ausserdem 43 Zeilen mit Werten < 0x10, die es im
     *     kind-Raum (ab 0x10) nicht gibt.
     *   - Zensus der echten RE2-RDTs: kind 0x21 traegt Sound-Id 0x0D (37/37 Records,
     *     ROOM1090 x28 + ROOM2110 x9). 0x0D steht nur in Zeile 7 = {0x0D,0x00} -> Bank 7.
     * Die vollstaendige Herleitung mit allen Byte-Ankern (EXE, RDT-Zensus, ENEMSE-Map)
     * laeuft als eigener Test: tests/unit/test_re2_crow_se_bank.c. NICHT auf 21 zurueckdrehen,
     * ohne dort die Anker zu widerlegen. */
    CHECK(se_bank_sel == 7,
          "Kraehen-ENEMSE-Bank 7 (Sound-Id 0x0D -> Paar-Zeile 7={0x0D,0} @0x800A7400), bank=%d",
          se_bank_sel);
    for (int i = 0; i < se_n; i++)
        CHECK(se_fl[i] == 0, "Kraehen-SEs laufen ueber die ERSTE Map-Haelfte (flag2000=0)");
}

static void test_takeoff_values(void)
{
    fresh_crow(1500, 0);
    tick();                                                /* INIT */
    CROW->state = 1; CROW->sub_state_1 = 2; CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    tick();                                                /* Takeoff P0 @0x80100C84 */
    CHECK(CROW->motion == 7 || CROW->sub_state_1 == 4,
          "Takeoff-Clip 7 ((s1<<8)|0x70007 @0x80100CC8-D8), motion=%d", CROW->motion);
    CHECK(CROW->speed_h >= 100 && CROW->speed_h <= 132,
          "Takeoff-Speed 100+(rand&0x1f)+Accel (@0x80100CDC-EC/@0x80100E5C-74), speed=%d", CROW->speed_h);
    CHECK(CROW->re2c_flags22a & 0x1u,
          "Takeoff armiert den Wake-Timer (+0x22A|=1 @0x80100D0C-20), fl=%04x", CROW->re2c_flags22a);
}

static void test_arbitration_single_claim(void)
{
    /* Fenster dist in [901,7195] (@0x80104254-60) + Mutex 0x800CFBF4-Bit0 frei (@0x8010426C-7C)
     * + LOS Bit 2 (@0x80104284-90) + Kegel 256 (@0x801042A8-B0) -> Claim; rand&0xf==0 -> Sub 11
     * sonst Sub 13 (@0x801042E4-304). NUR EINE Kraehe claimt. */
    fresh_crow(2500, 0);
    re15_actor_t *C2 = &g_actors[2];
    memset(C2, 0, sizeof *C2);
    C2->active = 1; C2->type = 0x21; C2->state = 0;
    C2->x = 400; C2->z = 400;
    tick(); tick_slot(2);                                  /* INIT beide */
    CROW->state = 1; CROW->sub_state_1 = 4; CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    C2->state = 1;   C2->sub_state_1 = 4;   C2->sub_state_2 = 0;   C2->sub_state_3 = 0;
    CROW->y = -3000; C2->y = -3000;                        /* fliegend */
    int claimed_slot = -1, both = 0;
    for (int i = 0; i < 40; i++) {
        face_player(CROW, PL); face_player(C2, PL);
        tick(); tick_slot(2);
        int c1 = (CROW->re2c_flags22a & 0x4u) ? 1 : 0;
        int c2 = (C2->re2c_flags22a  & 0x4u) ? 1 : 0;
        if (c1 && c2) both = 1;
        if (claimed_slot < 0 && (c1 || c2)) claimed_slot = c1 ? 1 : 2;
    }
    CHECK(claimed_slot > 0, "eine Kraehe muss den Mutex claimen (@0x801042C4-DC)");
    CHECK(!both, "NIE zwei Claims gleichzeitig (Mutex-Gate @0x8010426C-7C)");
    if (claimed_slot < 0) { C2->active = 0; return; }      /* Review-Fix #13: kein g_actors[-1]
                                                            * im Fehlerpfad (CHECK loggt nur) */
    re15_actor_t *cl = &g_actors[claimed_slot];
    CHECK(cl->sub_state_1 == 11 || cl->sub_state_1 == 13 || cl->sub_state_1 == 14
          || cl->sub_state_1 == 12 || cl->sub_state_1 == 8 || cl->sub_state_1 == 4,
          "Claimer in der Angriffs-Kette 11/13 (rand&0xf @0x801042E4-304), sub=%d", cl->sub_state_1);
    CHECK((g_re2_room_gflags & 0x1u) || cl->sub_state_1 <= 10,
          "Mutex-Bit 0 gesetzt solange 11..14 (Post-Pass-Release @0x801044C4-F0)");
    C2->active = 0;
}

static void test_grab_peck_damage_and_release(void)
{
    /* GRAB: Ph1-Peck = 0x800401d4(5, aliveflag) (@0x8010265C-64) — Schaden 5 STEHT IM MODUL.
     * mode 1 (lebend): one-save statt Tod (FUN_800401d4-Decompile, selbst gelesen). */
    fresh_crow(0, 0);
    tick();                                                /* INIT */
    g_re2_room_gflags |= 0x1u;                             /* Mutex ist beim Claimer */
    CROW->state = 1; CROW->sub_state_1 = 14; CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    CROW->re2c_flags22a = 0x2u | 0x4u;                     /* LOS + Claim */
    CROW->re2d_turn224 = -2880;                            /* Harass-Hoehe (Hover-P0-Analog) */
    CROW->x = 300; CROW->z = 0; CROW->y = -2900;
    PL->hp = 100;
    int16_t hp_entry = PL->hp;
    tick();                                                /* Eintritts-Tick = P0+P1+P2 (Fallthrough
                                                            * @0x801025C8→CC und @0x801026BC→C0) */
    CHECK(CROW->motion == 5, "GRAB-Clip 5 (0x70005 @0x801025A4-AC), motion=%d", CROW->motion);
    CHECK(CROW->re2d_air219 == 99, "GRAB-Timeout 100 (sb 100,537 @0x801025B0-B4) minus dem "
          "P2-Mash-Dec des Eintritts-Ticks (Fallthrough, Review-Fix #4), t=%d", CROW->re2d_air219);
    CHECK(PL->hp == hp_entry - 5, "erster 5-HP-Peck IM Eintritts-Tick (P0→P1-Fallthrough "
          "@0x801025C8-CC), hp=%d", PL->hp);
    CHECK(CROW->speed_h == 300, "GRAB-Speed 300 (sh 300,324 @0x801025B8-BC), spd=%d", CROW->speed_h);
    CHECK(CROW->y == -2880, "GRAB y-Snap auf +0x224 (@0x80102580-88), y=%d", (int)CROW->y);
    int16_t hp0 = PL->hp;
    int pecks = 0, released = 0;
    for (int i = 0; i < 400 && !released; i++) {
        int16_t before = PL->hp;
        tick();
        if (PL->hp != before) {
            CHECK(before - PL->hp == 5, "Peck = exakt 5 HP (a0=5 @0x80102660-64), war %d",
                  before - PL->hp);
            pecks++;
        }
        if (CROW->sub_state_1 == 4) released = 1;
    }
    CHECK(pecks >= 2, "mehrere Pecks im 100er-Timeout-Fenster (Zyklus P1..P4), n=%d", pecks);
    CHECK(released, "Timeout (+0x219 -= 3*mash+1 < 0 @0x801026DC-F8) -> Release -> Sub 4 "
                    "(@0x8010285C-64), sub=%d", CROW->sub_state_1);
    CHECK(PL->hp == hp0 - 5 * pecks, "HP-Buchhaltung konsistent, hp=%d", PL->hp);
    CHECK(se_seen(2), "Peck-SE 2 (jal 0x8005bd6c(2) @0x80102650-58)");

    /* One-Save (mode 1): HP 3 -> Peck -> HP=0 + ret 1 -> sofortiger Release */
    fresh_crow(0, 0);
    tick();
    re15_re2z_rng_reset();
    g_re2_room_gflags |= 0x1u;
    CROW->state = 1; CROW->sub_state_1 = 14; CROW->sub_state_2 = 0;
    CROW->re2c_flags22a = 0x6u;
    CROW->re2d_turn224 = -2880; CROW->x = 300; CROW->y = -2900;
    PL->hp = 3;
    tick();                                                /* Eintritts-Tick: P0→P1-Peck 3-5<0
                                                            * -> One-Save (Fallthrough #4) */
    CHECK(PL->hp == 0, "mode 1: One-Save statt Tod (HP=0, FUN_800401d4-Schwanz), hp=%d", PL->hp);
    CHECK(CROW->sub_state_1 == 4, "ret 1 -> abgeschuettelt -> Sub 4 (@0x80102668-70), sub=%d",
          CROW->sub_state_1);

    /* Tod (mode 0 + Latch): HP<=0-Spieler wird weiter gepickt -> ret 2 -> Broadcast 128 ->
     * Pacifier 120 bei den Artgenossen (Post-Pass @0x80104504-20) */
    re15_actor_t *C2 = &g_actors[2];
    memset(C2, 0, sizeof *C2);
    C2->active = 1; C2->type = 0x21; C2->state = 1; C2->sub_state_1 = 4;
    C2->x = 4000; C2->z = 0; C2->y = -3000; C2->hp = 10;
    CROW->sub_state_1 = 14; CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    CROW->re2c_flags22a = 0x6u; g_re2_room_gflags |= 0x1u;
    tick();                                                /* Eintritts-Tick: P0 (HP=0 -> mode 0)
                                                            * → P1-Peck 0-5=-5, Latch -> ret 2 */
    CHECK(PL->hp == -5, "mode 0 + One-Save-Latch: Peck toetet (ret 2), hp=%d", PL->hp);
    CHECK(C2->re2c_flags22a & 0x80u, "Broadcast 128 an die Artgenossen (@0x80102680-84), fl=%04x",
          C2->re2c_flags22a);
    tick_slot(2);
    CHECK(C2->re2c_pac221 == 120, "Post-Pass: Bit 0x80 -> +0x221=120 (@0x8010451C-20), pac=%d",
          C2->re2c_pac221);
    C2->active = 0;
}

static void test_direct_strike_on_contact(void)
{
    /* Direkt-STRIKE aus dem Kreisen: Spieler-Body-Kontakt (+0xD==0, Port crow_contact) +
     * Streak frisch + speed>=231 + Mutex frei + Kegel 256 -> Sub 12 (@0x80104310-80). */
    fresh_crow(600, 0);
    tick();
    CROW->state = 1; CROW->sub_state_1 = 4; CROW->sub_state_2 = 1;
    CROW->y = -2000; CROW->speed_h = 300;
    CROW->motion = 3;
    face_player(CROW, PL);
    CROW->crow_contact = 1;                                /* Body-Push-Stempel des Vorframes */
    tick();
    CHECK(CROW->sub_state_1 == 12, "Kontakt+speed>=231+Kegel -> Sub 12 STRIKE (@0x8010437C-80), sub=%d",
          CROW->sub_state_1);
    face_player(CROW, PL);
    tick();                                                /* STRIKE P0 @0x80102000 */
    CHECK(se_seen(4), "Strike-Impact SE 4 (@0x80102054-5C)");
    CHECK(PL->hit_react & 0x1, "Spieler-State (2,1/0) @0x8010201C-34 -> Port-Hit-Guard "
                               "(Schadenshoehe EXE-seitig OFFEN)");
    CHECK(CROW->motion == 9, "Strike-Clip 9 (0x70009 @0x80102060-68), motion=%d", CROW->motion);
}

static void test_hurt_recovery_and_corpse(void)
{
    /* Flug-Treffer (Zeile 4 = Kreisen): SE 3 + Clip 9 + Spin ±200; Boden -> Clip 0xB;
     * HP>=0 -> ERHOLUNG zurueck in ACTIVE (@0x80102C08-7C) — die Kraehe fliegt wieder! */
    fresh_crow(3000, 0);
    tick();
    CROW->state = 1; CROW->sub_state_1 = 4; CROW->sub_state_2 = 1; CROW->y = -2000;
    tick();
    CROW->hp = 8; CROW->state = 2;                         /* take_damage-Analog */
    CROW->sub_state_1 = 3;                                 /* Waffen-Stempel wie der Resolver
                                                            * (`sb s8,0x5` @0x800124bc): Pistole
                                                            * w3 -> Zeile 3 = Flug-Treffer
                                                            * (@0x80104A18; Fix 2026-08-23:
                                                            * Zeile = Waffe, nicht prev_sub) */
    CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    tick();
    CHECK(CROW->motion == 9, "HURT-Clip 9 (0x70009 @0x8010298C-94), motion=%d", CROW->motion);
    CHECK(se_seen(3), "HURT-Kreischen SE 3 (@0x80102980-88)");
    CHECK(CROW->re2d_offx228 == 200 || CROW->re2d_offx228 == -200,
          "Spiral-Spin ±200 (@0x801029E4-FC), spin=%d", CROW->re2d_offx228);
    int recovered = 0;
    for (int i = 0; i < 80 && !recovered; i++) { tick(); if (CROW->state == 1) recovered = 1; }
    CHECK(recovered, "HP>=0: RECOVERY zurueck in state 1 (@0x80102C20-7C), state=%d sub=%d",
          CROW->state, CROW->sub_state_1);
    CHECK(CROW->sub_state_1 == 9 || CROW->sub_state_1 == 0,
          "Recovery-Ziel (1,9) aussen / (1,0) innen (@0x80102C08-44), sub=%d", CROW->sub_state_1);

    /* Kill aus dem Flug -> Corpse (7,0) mit Lache 400 -> +10/Tick und Zuck-Maschine */
    fresh_crow(3000, 0);
    tick();
    CROW->state = 1; CROW->sub_state_1 = 4; CROW->sub_state_2 = 1; CROW->y = -1500;
    tick();
    se_n = 0;
    CROW->hp = -1; CROW->state = 3;                        /* toedlicher Treffer */
    CROW->sub_state_1 = 3;                                 /* Waffen-Stempel (Pistole w3 ->
                                                            * Flug-Treffer-Zeile, s.o.) */
    CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    int corpse = 0;
    for (int i = 0; i < 120 && !corpse; i++) { tick(); if (CROW->state == 7) corpse = 1; }
    CHECK(corpse, "HP<0 -> Liegen 11 Ticks -> CORPSE (7,0) (@0x80102B84-98/@0x80102C70-7C), state=%d",
          CROW->state);
    CHECK(CROW->sub_state_1 == 0, "Corpse-Sub 0 (Lache+Zucken), sub=%d", CROW->sub_state_1);
    tick();                                                /* Ph0: Basis 400 + 60 Ticks; Ph0 FAELLT
                                                            * in Ph1 (@0x80103A4C-50) -> erster
                                                            * Grow-Tick sofort: 410/59 */
    CHECK(CROW->crow_shadow_w == 410 && CROW->re2z_t15a == 59,
          "Lache-Basis 400 ([+0x16C]+4/+6 @0x80103A28-3C) + Eintritts-Tick-Grow (+10, "
          "Ph0-Fallthrough Review-Fix #5), w=%d t=%d",
          CROW->crow_shadow_w, CROW->re2z_t15a);
    uint16_t w0 = CROW->crow_shadow_w;
    int twitch_se = 0;
    for (int i = 0; i < 400; i++) {
        tick();
        if (se_seen(3)) twitch_se = 1;
        CHECK(CROW->state == 7, "Corpse bleibt state 7, state=%d", CROW->state);
    }
    CHECK(CROW->crow_shadow_w > w0, "Lache waechst +10/Tick in Ph1 (@0x80103AC8-FC), w=%d>%d",
          CROW->crow_shadow_w, w0);
    CHECK(CROW->crow_shadow_w <= 400 + 600, "Grower endet nach 60 Ticks (+0x15A @0x80103AB8-D0), w=%d",
          CROW->crow_shadow_w);
    CHECK(twitch_se, "Todeszucken spielt SE 3 (@0x80103B68-70)");
}

static void test_gib_and_launch_rows(void)
{
    /* HURT-Zeile 5 (Steigen) -> GIB (@0x80104A2C): 13-Part-Scatter (Stand-in crow_hide) ->
     * CORPSE Sub 1 mit Schatten-Disable ([+0x16C]+0xE=0 @0x80103C34). */
    fresh_crow(3000, 0);
    tick();
    CROW->state = 1; CROW->sub_state_1 = 5; CROW->sub_state_2 = 1; CROW->y = -3000;
    tick();
    CROW->hp = 6; CROW->state = 2;
    CROW->sub_state_1 = 7;                                 /* Waffen-Stempel: Magnum w7 -> RE2-
                                                            * Zeile 5 = GIB (@0x80104A2C; Tabelle
                                                            * s_re2c_row_from_weapon) */
    CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    tick();
    CHECK(CROW->state == 7 && CROW->sub_state_1 == 1,
          "Zeile 5 -> GIB -> (7,1) (@0x80102EC8-D4), state=%d sub=%d", CROW->state, CROW->sub_state_1);
    CHECK(CROW->crow_hide == 1, "GIB verbirgt das Mesh (Part-Scatter-Stand-in)");
    tick();
    CHECK(CROW->crow_shadow_w == 0, "GIB-Leiche: Schatten-Disable (@0x80103C34), w=%d",
          CROW->crow_shadow_w);

    /* HURT-Zeile 7 (Landen) -> LAUNCH (@0x80104A34): Sofort-Kill, Speed 600 in SPIELER-Yaw. */
    fresh_crow(3000, 0);
    tick();
    CROW->state = 1; CROW->sub_state_1 = 7; CROW->sub_state_2 = 1; CROW->y = -300;
    PL->rot_y = 777;
    tick();
    CROW->hp = 6; CROW->state = 2;
    CROW->sub_state_1 = 8;                                 /* Waffen-Stempel: Shotgun w8 -> RE2-
                                                            * Zeile 7 = LAUNCH (@0x80104A34) */
    CROW->sub_state_2 = 0; CROW->sub_state_3 = 0;
    tick();
    CHECK(CROW->hp == -1, "Launch-Treffer: Sofort-Kill (sh -1,342 @0x801032EC-F4), hp=%d", CROW->hp);
    CHECK(CROW->speed_h == 600, "Launch-Speed 600 (@0x80103320-24), spd=%d", CROW->speed_h);
    CHECK(CROW->rot_y == 777, "Launch-Richtung = SPIELER-Yaw 0x800CFC6E (@0x80103318-34), yaw=%d",
          CROW->rot_y);
    int corpse3 = 0;
    for (int i = 0; i < 80 && !corpse3; i++) { tick(); if (CROW->state == 7) corpse3 = 1; }
    CHECK(corpse3 && CROW->sub_state_1 == 3, "Launch endet in CORPSE Sub 3 (@0x80103434-40), sub=%d",
          CROW->sub_state_1);
}

static void test_no_freeze(void)
{
    fresh_crow(2000, 0);
    tick();
    uint32_t last = 0; int trans = 0;
    for (int f = 0; f < 600; f++) {
        PL->x = CROW->x + ((f & 64) ? 700 : 2500); PL->z = CROW->z;
        PL->motion = 100;
        if (PL->hp < 30) PL->hp = 100;
        tick();
        CHECK(CROW->state == 1 || CROW->state == 2 || CROW->state == 3 || CROW->state == 7,
              "gueltiger State, state=%d", CROW->state);
        if (CROW->state == 1)
            CHECK(CROW->sub_state_1 <= 14, "ACTIVE sub %d > 14", CROW->sub_state_1);
        uint32_t sig = ((uint32_t)CROW->state << 8) | CROW->sub_state_1;
        if (sig != last) { trans++; last = sig; }
    }
    CHECK(trans >= 3, "die Kraehe dreht den Loop (Transitionen=%d)", trans);
}

int main(void)
{
    re15_re2crow_audio_hook(se_cap, se_bank_cap);

    test_init_wake_takeoff();
    test_takeoff_values();
    test_arbitration_single_claim();
    test_grab_peck_damage_and_release();
    test_direct_strike_on_contact();
    test_hurt_recovery_and_corpse();
    test_gib_and_launch_rows();
    test_no_freeze();

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    if (fails == 0) printf("test_re2_crow_ai: OK\n");
    else            printf("test_re2_crow_ai: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
