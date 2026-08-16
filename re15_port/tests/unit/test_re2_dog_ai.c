/* test_re2_dog_ai.c — pins the RE2 CERBERUS brain (WELLE C, OPTIONS -> AI -> RE2) to the RE2
 * dog module bytes.
 *
 * Source of truth: info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN (== CDEMD0.EMS TOC sector 1206),
 * loaded RAW @0x80100000. Disassemble with
 *     .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMD0G_MOD0.BIN
 * Every assertion cites its module address (all re-disassembled for Welle C, scratchpad
 * dog_self.asm). No enemy bank is loaded -> clips report done immediately; assertions that
 * depend on real frame counts live in test_re2_room1190_ab.c / the live smoke instead.
 */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- SE capture hook (records what the brain would send to ENEMSE) ------------------------ */
static int se_log[256]; static int se_fl[256]; static int se_n = 0;
static int se_bank_sel = -1;
static void se_cap(int id, int flag2000)
{ if (se_n < 256) { se_log[se_n] = id; se_fl[se_n] = flag2000; se_n++; } }
static void se_bank_cap(int bank) { se_bank_sel = bank; }
static int  se_seen(int id) { for (int i = 0; i < se_n; i++) if (se_log[i] == id) return 1; return 0; }
static int  se_count(int id) { int n = 0; for (int i = 0; i < se_n; i++) if (se_log[i] == id) n++; return n; }

/* dog HP table @0x80105340 (self-read; default — 0x800CFB74 bits 0x40/0x20 have no port producer) */
static const uint16_t hp_tbl[16] = { 119,85,85,85,119,70,85,85,70,85,59,70,59,85,59,70 };
static int hp_in_table(int hp)
{
    for (int i = 0; i < 16; i++)
        for (int a = 0; a <= 3; a++)                       /* +rand&3 @0x801001E8/@0x80100230 */
            if (hp == (int)hp_tbl[i] + a) return 1;
    return 0;
}

static re15_actor_t *PL;
static re15_actor_t *DOG;

static void face_player(re15_actor_t *e, const re15_actor_t *pl, int behind)
{
    int yaw = ((int)re15_atan2_q12(pl->z - e->z, pl->x - e->x) - 0x400) & 0xfff;
    if (behind) yaw = (yaw + 2048) & 0xfff;
    e->rot_y = (int16_t)yaw;
}

static void fresh_dog(int32_t px, int32_t pz)
{
    re15_actor_init();                                     /* reseeds the RE2 PRNG + room flags */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL  = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    DOG = &g_actors[1];
    memset(PL,  0, sizeof *PL);
    memset(DOG, 0, sizeof *DOG);
    PL->active = 1; PL->hp = 100; PL->x = px; PL->z = pz; PL->floor = 0;
    DOG->active = 1; DOG->type = 0x20; DOG->state = 0; DOG->hp = 0; DOG->floor = 0;
    se_n = 0;
}

static void tick(void)
{
    re15_actor_t *e = DOG;
    e->ai_dist = 0;
    {   /* the caller-side dist feed (re15_dog_ai_tick hook computes it the same way) */
        int32_t dx = PL->x - e->x, dz = PL->z - e->z;
        int64_t d2 = (int64_t)dx * dx + (int64_t)dz * dz;
        uint32_t r = 0; while ((int64_t)(r + 1) * (r + 1) <= d2) r++;
        e->ai_dist = r;
    }
    re15_re2dog_tick(1);
}

/* =============================== tests ==================================================== */

static void test_init_and_wake(void)
{
    fresh_dog(20000, 0);
    tick();                                                /* INIT @0x801000F4 */
    CHECK(DOG->state == 1, "INIT schreibt state=1 zuerst (sw 1,4 @0x8010012C), state=%d", DOG->state);
    CHECK(DOG->sub_state_1 == 0, "Spawn-Raw 0 -> Start-Substate tbl@0x80105458[0]=0 (IDLE), sub=%d",
          DOG->sub_state_1);
    CHECK(hp_in_table(DOG->hp), "HP-Roll tbl@0x80105340[rand&0xf]+rand&3 (@0x80100210-234), hp=%d",
          DOG->hp);
    CHECK(DOG->motion == 1, "Start-Clip 1 (0xF0001|frame @0x801003A8-C8), motion=%d", DOG->motion);
    CHECK(DOG->anim_frame <= 63, "Zufalls-Startframe rand&0x3f (@0x801003B8-C0), frame=%d",
          DOG->anim_frame);

    /* fern + gleiche Etage: kein Wake (dist>=4000 @0x801007B8) */
    for (int i = 0; i < 5; i++) tick();
    CHECK(DOG->sub_state_1 == 0, "dist 20000: IDLE haelt (@0x801007B8), sub=%d", DOG->sub_state_1);

    /* nah -> STALK (dist<0xFA0 @0x801007B0-BC) */
    PL->x = DOG->x + 3000; PL->z = DOG->z;
    tick();
    CHECK(DOG->sub_state_1 == 1, "dist<4000 gleiche Etage -> Sub 1 STALK (@0x80100800), sub=%d",
          DOG->sub_state_1);

    /* andere Etage: floor-lost-Helfer wirft in Sub 9 (0x80105234) */
    PL->floor = 3;
    tick();
    CHECK(DOG->sub_state_1 == 9, "Etagen-Differenz -> Sub 9 (@0x8010524C-50), sub=%d", DOG->sub_state_1);
    PL->floor = 0;
    PL->x = DOG->x + 30000;                                /* fern: kein Re-Wake nach dem Timeout */
    /* Sub 9: 60 Frames Clip 0, dann zurueck in Sub 0 (@0x801023A0-D8) */
    CHECK(DOG->motion == 0 || DOG->re2z_t158 == 60 || DOG->sub_state_2 == 0,
          "Sub 9 Setup (Clip 0 + 60f @0x801023A0-AC)");
    for (int i = 0; i < 61; i++) tick();
    CHECK(DOG->sub_state_1 == 0 && DOG->motion == 1,
          "Sub 9 -> Sub 0 + Clip 1 nach 60f (@0x801023C8-D8), sub=%d motion=%d",
          DOG->sub_state_1, DOG->motion);
}

static void test_stalk_speed_aggro_run(void)
{
    fresh_dog(3000, 0);
    tick();                                                /* INIT */
    tick();                                                /* IDLE -> wake -> sub 1 */
    CHECK(DOG->sub_state_1 == 1, "wake -> STALK");
    face_player(DOG, PL, 1);                               /* abgewandt: kein Angriffs-Commit im
                                                            * P0-Tick (Kegel-Gate @0x80100998-A8) */
    tick();                                                /* STALK P0 */
    CHECK(DOG->speed_h == 30, "STALK Speed 30 (sh 30,324 @0x80100864-68), speed=%d", DOG->speed_h);
    CHECK(DOG->re2d_offx228 % 512 == 0 && DOG->re2d_offx228 >= -1024 && DOG->re2d_offx228 <= 1024,
          "Ziel-Offset aus tbl@0x801054A8 {1024,0,-512,0,-1024,0,512,0}, offx=%d", DOG->re2d_offx228);

    /* Aggro-Meter: Spieler RENNT hinter dem Hund, dist<3000 -> +21/Tick (@0x80100990-94),
     * Schwelle >=65 -> Sub 2 RUN (@0x80100A30-78). 21*4=84 >= 65 -> spaetestens Tick 4. */
    fresh_dog(2500, 0);
    tick(); tick();                                        /* INIT, wake */
    face_player(DOG, PL, 1);
    tick();                                                /* P0 (abgewandt) */
    PL->motion = 100;                                      /* RUN -> 0xFBF6-Bit 0x4 (Mapping) */
    int run_at = -1;
    for (int i = 1; i <= 6 && run_at < 0; i++) {
        PL->x = DOG->x + 2500; PL->z = DOG->z;             /* dist ~2500 < 3000 */
        face_player(DOG, PL, 1);                           /* Spieler NICHT im Frontkegel:
                                                            * kein Angriff-Commit dazwischen */
        tick();
        if (DOG->sub_state_1 == 2) run_at = i;
    }
    CHECK(run_at > 0 && run_at <= 4,
          "Aggro +21/Tick (rennend, @0x80100990) -> RUN bei >=65 (@0x80100A38) in <=4 Ticks, war %d",
          run_at);
}

static void test_stalk_attack_gate_601(void)
{
    /* KORRIGIERTE Lane-D-Lesart: der STALK-Angriff braucht dist >= 0x259 (601) — sltiu 0x259;
     * bne -> SKIP @0x801009BC-C0, Commit sb 3,5 @0x801009C8 nur im >=-Fall. */
    fresh_dog(2000, 0);
    tick(); tick();                                        /* INIT, wake */
    PL->x = DOG->x + 2000; PL->z = DOG->z;
    face_player(DOG, PL, 0);                               /* im 128er-Frontkegel */
    tick();                                                /* P0 + Kegel/Dist-Check */
    face_player(DOG, PL, 0);
    tick();
    CHECK(DOG->sub_state_1 == 3,
          "STALK: Frontkegel 128 + dist>=601 -> Sub 3 (@0x801009C8), sub=%d", DOG->sub_state_1);

    fresh_dog(3000, 0);
    tick(); tick();
    PL->x = DOG->x + 400; PL->z = DOG->z;                  /* dist 400 < 601 */
    face_player(DOG, PL, 0);
    tick();
    face_player(DOG, PL, 0);
    tick();
    CHECK(DOG->sub_state_1 != 3,
          "STALK: dist<601 darf NICHT in Sub 3 committen (sltiu 0x259 @0x801009BC), sub=%d",
          DOG->sub_state_1);
}

static void test_run_accel_and_turnrate(void)
{
    fresh_dog(30000, 0);
    tick();                                                /* INIT */
    DOG->state = 1; DOG->sub_state_1 = 2; DOG->sub_state_2 = 0; DOG->sub_state_3 = 0;
    PL->x = DOG->x + 20000;                                /* weit: decide -> 0, kein Commit */
    tick();                                                /* P0 */
    CHECK(DOG->motion == 2, "RUN P0 Clip 2 (0xF0002 @0x80100BB4-D8), motion=%d", DOG->motion);
    CHECK(DOG->re2d_turn224 == 96,
          "RUN Turn-Rate 96 ohne Rudel (sh 96,548 @0x80100BCC/BE4), turn=%d", DOG->re2d_turn224);
    int prev = DOG->speed_h;
    for (int i = 1; i <= 7; i++) {
        tick();                                            /* P1: 0x80104BEC(40,240) @0x80100CBC */
        int want = 40 * i; if (want > 240) want = 240;
        CHECK(DOG->speed_h == want, "RUN Beschleunigung +40 bis 240 (Tick %d): %d != %d",
              i, DOG->speed_h, want);
        prev = DOG->speed_h;
    }
    (void)prev;

    /* Rudel: zweiter Hund aktiv -> Turn-Rate aus tbl@0x801054D8 {32,64,48,64} (@0x80100C2C-54) */
    fresh_dog(30000, 0);
    g_actors[2].active = 1; g_actors[2].type = 0x20;
    tick();
    DOG->state = 1; DOG->sub_state_1 = 2; DOG->sub_state_2 = 0;
    PL->x = DOG->x + 20000;
    tick();
    CHECK(DOG->re2d_turn224 == 32 || DOG->re2d_turn224 == 48 || DOG->re2d_turn224 == 64,
          "RUN Rudel-Turn-Rate aus @0x801054D8, turn=%d", DOG->re2d_turn224);
    g_actors[2].active = 0;
}

static void test_fatigue_and_breakoff(void)
{
    /* Fatigue >=451 (Review-Fix #1, byte-true): Abort feuert IMMER (`sb v0,4(s1)` @0x80100E10
     * = Branch-Ziel UND Fallthrough, Delay-Slot addiu v0,1 @0x80100E08); der Fatigue-Clear
     * `sh zero,2(s1)` @0x80100E0C liegt NUR auf dem HP>=21-Fallthrough.
     * decide==0 erzwungen ueber dist>=4000. */
    fresh_dog(30000, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 2; DOG->sub_state_2 = 1;
    DOG->re2z_flags21a = 460;
    PL->x = DOG->x + 20000;
    tick();
    CHECK(DOG->sub_state_2 == 2, "Fatigue 460 + HP>=21 -> P2 Abdrehen (sh 2,6 @0x80100E28), phase=%d",
          DOG->sub_state_2);
    CHECK(DOG->re2z_flags21a == 0,
          "HP>=21: Fatigue-Clear (sh zero,2(s1) @0x80100E0C), fat=%d", DOG->re2z_flags21a);

    fresh_dog(30000, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 2; DOG->sub_state_2 = 1;
    DOG->re2z_flags21a = 460;
    PL->hp = 15;                                           /* fast tot */
    PL->x = DOG->x + 20000;
    tick();
    CHECK(DOG->sub_state_2 == 2,
          "HP<21: Abort feuert TROTZDEM (@0x80100E10 beide Pfade), phase=%d", DOG->sub_state_2);
    CHECK(DOG->re2z_flags21a == 460,
          "HP<21: Fatigue bleibt stehen (bne @0x80100E04 springt am Clear vorbei), fat=%d",
          DOG->re2z_flags21a);
}

static void test_attack_launch(void)
{
    fresh_dog(2000, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 3; DOG->sub_state_2 = 0; DOG->sub_state_3 = 0;
    PL->x = DOG->x + 2000; PL->z = DOG->z;
    face_player(DOG, PL, 0);
    tick();                                                /* P0: Launch 0x80101380(65,-280) */
    CHECK(DOG->speed_h == 65, "Windup-Speed 65 (sh a1,324 @0x801003A0), speed=%d", DOG->speed_h);
    CHECK(DOG->re2d_vy146 == -280, "vy=-280 (sh a2,326 @0x801003A4), vy=%d", DOG->re2d_vy146);
    CHECK(DOG->motion == 20, "Sprung-Clip 20 (sw 0x70014 @0x801003A8), motion=%d", DOG->motion);
    CHECK(DOG->re2z_t158 == 8 - 1, "Windup 8 Frames (sh 8,344 @0x80100398), rest=%d", DOG->re2z_t158);
    for (int i = 0; i < 7; i++) tick();                    /* Windup ablaufen lassen */
    CHECK(DOG->sub_state_2 == 1, "Windup 0 -> P1 Flug (sh 1,6 @0x8010131C), phase=%d", DOG->sub_state_2);
    CHECK(DOG->speed_h == 280, "Absprung-Speed 280 (sh 280,324 @0x80101320-24), speed=%d", DOG->speed_h);
    CHECK(DOG->re2d_air219 == 1, "Luft-Flag (sb 1,537 @0x8010132C)");
    CHECK(se_seen(3), "Angriffs-Bark SE 3 (jal 0x8005bd6c(3) @0x80101344)");
    CHECK(se_bank_sel == 31, "Hunde-ENEMSE-Bank 31 (Paar-Zeile 31 {00,20} @0x800A7400)");
    for (int i = 0; i < se_n; i++)
        CHECK(se_fl[i] == 1, "Hunde-SEs laufen ueber die ZWEITE Map-Haelfte (flag2000)");

    /* Noise-Bit 0xFBF4|=0x20 (@0x8010134C-60): ein zweiter Hund in Sub 0 wacht dadurch auf */
    re15_actor_t *d2 = &g_actors[2];
    memset(d2, 0, sizeof *d2);
    d2->active = 1; d2->type = 0x20; d2->state = 1; d2->sub_state_1 = 0;
    d2->x = 30000; d2->z = 30000; d2->floor = 0; d2->hp = 50;
    d2->ai_dist = 40000;
    re15_re2dog_tick(2);
    CHECK(d2->sub_state_1 == 1, "Noise-Bit weckt den Rudel-Partner (0xFBF4&0x20 @0x801007D0-E0), sub=%d",
          d2->sub_state_1);
    d2->active = 0;

    /* Gravity: vy += 40/Frame (@0x801014B8-C8); Touchdown bei vy>=0 ohne Kontakt -> Landung
     * Clip 21 + Fatigue+20 (@0x80101424-68) */
    PL->x = DOG->x + 20000;                                /* kein Kontakt */
    int16_t vy0 = DOG->re2d_vy146;
    tick();
    CHECK(DOG->re2d_vy146 == vy0 + 40, "Gravity +40/Frame (@0x801014C0), vy=%d", DOG->re2d_vy146);
    uint16_t fat0 = DOG->re2z_flags21a;
    for (int i = 0; i < 12 && DOG->sub_state_2 == 1; i++) tick();
    CHECK(DOG->sub_state_2 == 2 && DOG->motion == 21,
          "Touchdown -> P2 Landung Clip 21 (0xF0015 @0x80101424/38), phase=%d motion=%d",
          DOG->sub_state_2, DOG->motion);
    CHECK(DOG->re2z_flags21a == fat0 + 20, "Fehlsprung: Fatigue+20 (@0x8010145C-68), fat=%d",
          DOG->re2z_flags21a);
    for (int i = 0; i < 60 && !se_seen(10); i++) tick();   /* Landung integriert bis Boden-Y */
    CHECK(se_seen(10), "Lande-Thud SE 10 (jal 0x8005bd6c(10) @0x801015FC)");
}

static void test_bite_damage_and_latch(void)
{
    /* Boden-Biss: FUN_800401D4(20,0) @0x80104EB8-C0 — der Schaden 20 steht IM MODUL.
     * HP 50-20=30 >= 0 -> return 0 -> Boden-Biss-Pfad (+0x21E=1 @0x80104F84). */
    fresh_dog(800, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 3; DOG->sub_state_2 = 1; DOG->sub_state_3 = 0;
    DOG->speed_h = 280; DOG->re2d_vy146 = -280; DOG->re2d_air219 = 1;
    DOG->motion = 20; DOG->dog_floor_y = 0;
    PL->x = DOG->x + 400; PL->z = DOG->z; PL->hp = 50;
    face_player(DOG, PL, 0);
    tick();
    CHECK(PL->hp == 30, "Flug-Biss = 20 HP (a0=20 @0x80104EB8), hp=%d", PL->hp);
    CHECK(DOG->re2d_bite21e == 1, "ueberlebter Biss -> Boden-Biss +0x21E=1 (@0x80104F84), val=%d",
          DOG->re2d_bite21e);
    CHECK(DOG->sub_state_2 == 3, "Boden-Biss-Phase +0x6=3 (@0x80104F7C), phase=%d", DOG->sub_state_2);

    /* LATCH: HP 4-20 = -16 < -14 -> FUN_800401D4 return 2 -> Latch (@0x80104EE0-F74):
     * +0x21E=2, Spieler-Yaw = Hund+2048, Hund geclaimt. */
    fresh_dog(800, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 3; DOG->sub_state_2 = 1; DOG->sub_state_3 = 0;
    DOG->speed_h = 280; DOG->re2d_vy146 = -280; DOG->re2d_air219 = 1;
    DOG->motion = 20; DOG->dog_floor_y = 0; DOG->rot_y = 0;
    PL->x = DOG->x + 400; PL->z = DOG->z; PL->hp = 4;
    face_player(DOG, PL, 0);
    /* Latch-Gate 0x80015910==0 (@0x80104EEC-F8) = FRONTAL: der Spieler SCHAUT den Hund an
     * (Port-Mapping !facing_aligned — Welle-B-Richtungs-Kollaps) */
    PL->rot_y = (int16_t)(((int)DOG->rot_y + 2048) & 0xfff);
    tick();
    CHECK(DOG->re2d_bite21e == 2, "toedlicher Biss -> LATCH +0x21E=2 (sb 2,6(s3) @0x80104F24), val=%d",
          DOG->re2d_bite21e);
    CHECK(((PL->rot_y - DOG->rot_y) & 0xfff) == 2048,
          "Spieler-Yaw = Hund+2048 (@0x80104F40-50), diff=%d", (PL->rot_y - DOG->rot_y) & 0xfff);
    /* Touchdown -> Sub 7 (@0x80101418-34), Clip 23 + Mash-Zaehler 12 (@0x80101D70-84) */
    for (int i = 0; i < 12 && DOG->sub_state_1 != 7; i++) tick();
    CHECK(DOG->sub_state_1 == 7, "+0x21E==2 bei Touchdown -> Sub 7 LATCH (@0x8010142C), sub=%d",
          DOG->sub_state_1);
    tick();
    CHECK(DOG->motion == 23, "Latch-Clip 23 (0x1F0017 @0x80101D5C-84), motion=%d", DOG->motion);
    CHECK((int8_t)DOG->re2z_dir16a <= 12 && (int8_t)DOG->re2z_dir16a >= 10,
          "Mash-Zaehler 12 (sb 12,362 @0x80101D7C), ctr=%d", (int8_t)DOG->re2z_dir16a);
}

static void test_hurt_knockdown_death(void)
{
    fresh_dog(4000, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 1; DOG->sub_state_2 = 1;
    DOG->hp = 60; DOG->dog_floor_y = 0;
    tick();
    /* Treffer 1: take_damage-Analog (state 2; Zeile = prev_sub) */
    DOG->hp = 50; DOG->state = 2; DOG->sub_state_2 = 0; DOG->sub_state_3 = 0;
    tick();
    CHECK(DOG->motion == 17, "Flinch-Clip 17 (0x70011 @0x80103460-7C), motion=%d", DOG->motion);
    CHECK(DOG->speed_h == 240, "Knockback-Speed 240 (sh 240,324 @0x80103454-5C), speed=%d",
          DOG->speed_h);
    CHECK(DOG->re2d_dbl223 == 1, "Doppel-Treffer-Marker +0x223=1 (sb 1,547 @0x80103488), val=%d",
          DOG->re2d_dbl223);
    CHECK(se_seen(1), "Jaulen SE 1 (@0x801033D4-DC)");
    /* Treffer 2 waehrend +0x223!=0 -> Knockdown-Router (+0x223|=0x80 @0x801033FC-400) */
    DOG->hp = 40; DOG->state = 2;
    tick();
    CHECK((DOG->re2d_dbl223 & 0x80u) != 0,
          "zweiter Treffer -> Knockdown-Latch 0x80 (@0x801033FC-400), val=0x%02x", DOG->re2d_dbl223);

    /* DEATH: SE 7 einmalig (+0x231-Latch @0x801041B8-D4), Ende -> CORPSE state 7 */
    fresh_dog(4000, 0);
    tick();
    DOG->state = 1; DOG->sub_state_1 = 1; DOG->sub_state_2 = 1; DOG->dog_floor_y = 0;
    tick();
    se_n = 0;
    DOG->hp = -20; DOG->state = 3; DOG->sub_state_2 = 0; DOG->sub_state_3 = 0;
    tick();
    CHECK(se_count(7) == 1, "Todesschrei SE 7 genau einmal (@0x801041C8-CC), n=%d", se_count(7));
    for (int i = 0; i < 20 && DOG->state != 7; i++) tick();
    CHECK(DOG->state == 7, "DEATH-Rutschphase -> CORPSE (sw 7,4 @0x80104290), state=%d", DOG->state);
    CHECK(se_count(7) == 1, "SE-7-Latch haelt ueber die Phasen (+0x231), n=%d", se_count(7));
    tick(); tick();
    CHECK(DOG->hp == -1 && DOG->sub_state_1 == 1,
          "CORPSE-Init (sb 1,5 @0x80104A08; Port-HP=-1), hp=%d sub=%d", DOG->hp, DOG->sub_state_1);
}

static void test_howl_claim(void)
{
    /* Heul-Claim 0xFBF4&0x80: nur EIN Hund heult, der zweite wartet tbl@0x80105430 Frames
     * (@0x8010191C-84). Beide fern (dist>3500), Noise-Bit gesetzt (via Launch-Pfad-Analog:
     * hier direkt ueber einen Angriff des ersten Hundes nicht noetig — der Claim-Pfad braucht
     * Noise, das ein vorheriger Absprung gesetzt hat; wir stellen ihn ueber einen Mini-Launch
     * her). */
    fresh_dog(20000, 0);
    tick();
    /* Noise-Bit setzen: Launch durchlaufen lassen (@0x8010134C-60) */
    DOG->state = 1; DOG->sub_state_1 = 3; DOG->sub_state_2 = 0; DOG->sub_state_3 = 0;
    for (int i = 0; i < 9; i++) tick();                    /* Windup 8 -> Noise gesetzt */
    /* jetzt beide Hunde in Sub 4 stellen */
    DOG->state = 1; DOG->sub_state_1 = 4; DOG->sub_state_2 = 0; DOG->sub_state_3 = 0;
    DOG->re2d_air219 = 0; DOG->speed_h = 0; DOG->re2d_vy146 = 0;
    re15_actor_t *d2 = &g_actors[2];
    memset(d2, 0, sizeof *d2);
    d2->active = 1; d2->type = 0x20; d2->state = 1; d2->sub_state_1 = 4;
    d2->x = 0; d2->z = 500; d2->floor = 0; d2->hp = 50; d2->ai_dist = 20000;
    PL->x = 20000; PL->z = 0;
    tick();                                                /* Hund 1: Claim + Clip 8 */
    CHECK(DOG->sub_state_2 == 2 && DOG->motion == 8,
          "erster Hund claimt + heult (Clip 8 @0x80101930-40, Claim @0x80101944-58), phase=%d motion=%d",
          DOG->sub_state_2, DOG->motion);
    d2->ai_dist = 20000;
    re15_re2dog_tick(2);                                   /* Hund 2: Claim vergeben -> warten */
    CHECK(d2->sub_state_2 == 1,
          "zweiter Hund wartet (P1, tbl@0x80105430 @0x80101964-84), phase=%d", d2->sub_state_2);
    d2->active = 0;
}

int main(void)
{
    re15_re2dog_audio_hook(se_cap, se_bank_cap);

    test_init_and_wake();
    test_stalk_speed_aggro_run();
    test_stalk_attack_gate_601();
    test_run_accel_and_turnrate();
    test_fatigue_and_breakoff();
    test_attack_launch();
    test_bite_damage_and_latch();
    test_hurt_knockdown_death();
    test_howl_claim();

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);               /* Default wiederherstellen */
    if (fails == 0) printf("test_re2_dog_ai: OK\n");
    else            printf("test_re2_dog_ai: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
