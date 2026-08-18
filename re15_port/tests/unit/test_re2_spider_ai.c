/* test_re2_spider_ai.c — pinnt das RE2-SPINNEN-Brain (WELLE E, OPTIONS -> AI -> RE2) an die
 * Bytes des RE2-Moduls.
 *
 * Quelle: info/re2leon/COMMON/BIN/EMS25.BIN == shared_assets/RE2/CDEMD0.EMS[0x390800 .. +26324]
 * (SHA1-Gleichheit selbst nachgerechnet), gelinkt @0x80100000. Disassemblieren mit
 *     .claude/skills/re15-psx-disasm/scripts/re2_disasm.py dis <addr> <n> --bin EMS25.BIN
 * JEDE Assertion nennt ihre Modul-Adresse. Es ist KEINE Gegner-Bank geladen -> Clips melden
 * sofort "done"; alles, was echte Frame-Zahlen braucht, steht im A/B-Test (ROOM1090).
 *
 * NEGATIV-TESTS (jede zentrale Verankerung hat einen): RE1.5-Flavor darf NICHTS von alldem
 * tun, das Besitz-Gate darf Decken-/Wand-Spawns NICHT uebernehmen, und die Konstanten sind
 * per Gegenprobe gegen benachbarte Werte gepinnt. */
#include "re15_actor.h"
#include "re15_ai_flavor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

/* ---- SE-Mitschnitt ------------------------------------------------------------------------ */
static int se_log[512], se_fl[512], se_n = 0, se_bank_sel = -1;
static void se_cap(int id, int flag2000) { if (se_n < 512) { se_log[se_n] = id; se_fl[se_n] = flag2000; se_n++; } }
static void se_bank_cap(int bank) { se_bank_sel = bank; }
static int  se_seen(int id) { for (int i = 0; i < se_n; i++) if (se_log[i] == id) return 1; return 0; }

static re15_actor_t *PL, *SP;

static void fresh(int32_t px, int32_t pz, uint8_t grid)
{
    re15_actor_init();                       /* re-seedet den RE2-PRNG + g_re2_room_gflags */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    PL = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    SP = &g_actors[1];
    memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
    PL->active = 1; PL->hp = 100; PL->x = px; PL->z = pz;
    SP->active = 1; SP->type = 0x25; SP->state = 0; SP->grid_id = grid;
    SP->re2z_f10e = grid;
    se_n = 0;
}

static void tick(void)
{
    SP->ai_dist = (uint32_t)re15_enemy_player_dist(SP, PL);
    re15_re2spider_tick(1);
}

int main(void)
{
    re15_re2spider_audio_hook(se_cap, se_bank_cap, 0);

    /* ---- 1. Besitz-Gate: nur BODEN-Spawns (INIT-Sprungtabelle @0x80100004, Index +0x10E&0xF).
     *      Index 0/1 -> +0x222 = 0 (@0x80100460); 2/3 -> 1 (@0x80100480); 4..11 -> 3
     *      (@0x801004C0). Welle E besitzt nur Modus 0. */
    fresh(0, 0, 0);
    CHECK(re15_re2spider_owns(SP) == 1, "owns(grid 0) muss 1 sein (INIT-Tabelle @0x80100460)");
    SP->grid_id = 1;  CHECK(re15_re2spider_owns(SP) == 1, "owns(grid 1) == 1 (@0x8010058C, kein Write)");
    /* NEGATIV: Decke/Wand duerfen NICHT uebernommen werden. */
    SP->grid_id = 2;  CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV owns(grid 2) muss 0 sein (Decke @0x80100480)");
    SP->grid_id = 3;  CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV owns(grid 3) muss 0 sein (Decke @0x80100480)");
    SP->grid_id = 7;  CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV owns(grid 7) muss 0 sein (Wand @0x801004C0)");
    SP->grid_id = 11; CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV owns(grid 11) muss 0 sein (Wand @0x801004C0)");
    /* NEGATIV: die BABY-Spinne (0x26) ist in Welle E bewusst NICHT im RE2-Brain — das
     * Modul EMS26.BIN hat eine eigene Zustandstabelle @0x80101084 und wuerde auf den
     * Adult-Tabellen @0x80106420 falsch laufen. */
    SP->grid_id = 0; SP->type = 0x26;
    CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV: Baby 0x26 darf NICHT vom Adult-Brain uebernommen werden");
    CHECK(re15_re2_owns_type(0x26) == 0, "NEGATIV: 0x26 darf nicht im RE2-Asset-Ownership-Set sein");
    SP->type = 0x25;

    /* ---- 2. Ownership-Set + ENEMSE-Bank ---------------------------------------------------- */
    CHECK(re15_re2_owns_type(0x25) == 1, "0x25 muss im RE2-Ownership-Set sein (Asset-Loader)");
    CHECK(re15_re2_owns_type(0x27) == 0, "NEGATIV: Gorilla 0x27 bleibt RE1.5");
    CHECK(se_bank_sel == 53, "Adult-ENEMSE-Bank = 53 (Paar-Tabelle @0x800A7400 Zeile {0x25,0x1F})");
    re15_re2spider_audio_hook(se_cap, se_bank_cap, 1);
    CHECK(se_bank_sel == 24, "Baby-ENEMSE-Bank = 24 (Zeile {0x26,0x00}) — belegt, noch ungenutzt");
    re15_re2spider_audio_hook(se_cap, se_bank_cap, 0);

    /* ---- 3. INIT @0x801001EC ---------------------------------------------------------------- */
    fresh(0, 0, 0);
    tick();
    CHECK(SP->state == 1 && SP->sub_state_1 == 0 && SP->sub_state_2 == 0,
          "INIT schreibt das Zustandswort 1 @0x80100214 (state=1, sub=0), ist=%u/%u/%u",
          SP->state, SP->sub_state_1, SP->sub_state_2);
    CHECK(SP->re2s_mode222 == 0, "grid 0 -> +0x222 = 0 (BODEN) @0x80100460, ist %u", SP->re2s_mode222);
    CHECK(SP->re2s_yaw21a == 0, "grid 0 -> +0x21A = 0 @0x80100464, ist %d", (int)SP->re2s_yaw21a);
    /* HP-Zug: Tabelle @0x801062C4 (99/99/99/99/119/99/99/119/99/99/99/119/99/89/99/99)
     * + (rand & 3); zwei Ziehungen @0x80100240 / @0x801002C0-E4. */
    CHECK(SP->hp >= 89 && SP->hp <= 122,
          "INIT-HP muss aus @0x801062C4 + (rand&3) stammen (89..122), ist %d", (int)SP->hp);
    /* NEGATIV: kein Wert aus den drei unerreichbaren Tabellen (Minimum 49 @0x801062E4). */
    CHECK(SP->hp != 49 && SP->hp < 129,
          "NEGATIV: HP darf nicht aus @0x801062E4/@0x80106334 stammen (Port-MAPPING: beide Bits 0)");
    CHECK(SP->motion == 1, "INIT-Clip = 1 @0x8010035C, ist %d", (int)SP->motion);
    CHECK(SP->re2z_pool151 == 6 && SP->re2z_pool153 == 6, "+0x151..+0x153 = 6 @0x80100318-20");

    /* ---- 4. IDLE (Sub 0) -> Lauf-Substate. FUN_801006C4: P0 setzt Clip 0/Rate 3
     *      (@0x80100704) und +0x158 = (rand+10)&0x1F (@0x80100718); P1 zaehlt herunter und
     *      schaltet auf 0x101 (Sub 1) bzw. mit rand&1 auf 0x401 (Sub 4) (@0x80100754-64).   */
    fresh(20000, 0, 0);                         /* weit weg: keine Distanz-Leiter feuert */
    tick();                                     /* INIT */
    tick();                                     /* ACTIVE Sub 0, P0 */
    CHECK(SP->motion == 0 && SP->anim_frac == 3,
          "IDLE-P0: Clip-Wort 0x00030000 @0x80100704-08 (Clip 0, Xfade 3), ist %d/%u",
          (int)SP->motion, SP->anim_frac);
    CHECK(SP->sub_state_2 == 1, "IDLE-P0 setzt +0x6 = 1 @0x80100714");
    CHECK(SP->re2z_t158 >= 0 && SP->re2z_t158 <= 31,
          "IDLE-Timer (rand+10)&0x1F @0x80100718-20 muss 0..31 sein, ist %d", (int)SP->re2z_t158);
    {   int guard = 0, reached = 0;
        while (guard++ < 200) { tick(); if (SP->sub_state_1 == 1 || SP->sub_state_1 == 4) { reached = 1; break; } }
        CHECK(reached, "IDLE muss auf Sub 1 (0x101 @0x80100754) oder Sub 4 (0x401 @0x80100764) laufen");
    }

    /* ---- 5. LAUF-Parameter @0x80106478 (3 Worte je Substate, aus der Datei gelesen):
     *      Sub 1 {Clip 2, Dreh 48, Speed 100} / Sub 2 {3,16,60} / Sub 3 {3,16,20}.        */
    for (int sub = 1; sub <= 3; sub++) {
        static const int want_clip[3] = { 2, 3, 3 }, want_spd[3] = { 100, 60, 20 };
        fresh(20000, 0, 0);
        tick();                                        /* INIT */
        SP->sub_state_1 = (uint8_t)sub; SP->sub_state_2 = 0;
        int32_t x0 = SP->x, z0 = SP->z;
        tick();                                        /* P0: Clip + Timer */
        CHECK(SP->motion == want_clip[sub - 1],
              "Sub %d: Clip %d aus @0x80106478+%d (ist %d)", sub, want_clip[sub - 1], (sub-1)*12, (int)SP->motion);
        /* P0 faellt im Original in P1 (@0x8010090C -> @0x80100954), und FUN_8002959C
         * dekrementiert den Crossfade-Zaehler +0x14E einmal (@0x800299C0-CC) — nach dem
         * ersten Tick steht also 3-1 = 2. */
        CHECK(SP->anim_frac == 2, "Sub %d: Xfade 3 (@0x80100938) minus 1 Advance-Decay (@0x800299C8)", sub);
        CHECK(SP->re2s_z148 == 0, "Sub %d: +0x148 = 0 @0x8010093C", sub);
        SP->re2z_t158 = 30;                            /* Timer offen halten */
        tick();                                        /* P1: Speed + Bewegung */
        CHECK(SP->speed_h == want_spd[sub - 1],
              "Sub %d: Speed %d aus @0x80106480+%d (ist %d)", sub, want_spd[sub - 1], (sub-1)*12, (int)SP->speed_h);
        CHECK(SP->x != x0 || SP->z != z0, "Sub %d muss sich bewegen (FUN_800152C8 @0x80100A58)", sub);
    }
    /* NEGATIV: die Speeds duerfen NICHT vertauscht sein. */
    {   fresh(20000, 0, 0); tick(); SP->sub_state_1 = 3; SP->sub_state_2 = 0; tick();
        SP->re2z_t158 = 30; tick();
        CHECK(SP->speed_h != 100 && SP->speed_h != 60,
              "NEGATIV: Sub 3 hat Speed 20 (@0x80106498), nicht 100/60"); }

    /* ---- 6. Nahbereichs-Abbruch: Modus 0 + Distanz < 1500 -> Zustandswort 1
     *      (`sltiu v0,v0,0x5dc` @0x80100A40, `sw v0,4(s0)` @0x80100A50).                    */
    fresh(1000, 0, 0);
    tick();                                            /* INIT */
    SP->sub_state_1 = 1; SP->sub_state_2 = 1; SP->re2z_t158 = 30;
    tick();
    CHECK(SP->sub_state_1 == 0, "Distanz < 1500 muss auf Sub 0 zurueckschalten @0x80100A40-50 (ist %u)",
          SP->sub_state_1);
    /* NEGATIV: bei 1500 exakt (nicht kleiner) darf NICHT zurueckgeschaltet werden. */
    fresh(2400, 0, 0);
    tick(); SP->sub_state_1 = 1; SP->sub_state_2 = 1; SP->re2z_t158 = 30;
    tick();
    CHECK(SP->sub_state_1 == 1,
          "NEGATIV: Distanz >= 1500 (`sltiu 0x5dc` @0x80100A40) darf NICHT zurueckschalten (ist %u)",
          SP->sub_state_1);

    /* ---- 7. Der ANGRIFF (Sub 7, Phasen @0x80100044): Clip 4 -> 5 -> Sprung -> Biss.
     *      P4 setzt +0x144 = 300 @0x80100E48; P5 prueft FUN_80105AE0(self, 1500, 4300)
     *      @0x80100EE4-E8 -> Spielerschaden 20 (`addiu a0,zero,20` @0x80105A78).            */
    fresh(600, 0, 0);
    tick();                                            /* INIT */
    SP->sub_state_1 = 7; SP->sub_state_2 = 0;
    tick();
    CHECK(SP->motion == 4, "Angriff-P0: Clip 4 @0x80100D48 (ist %d)", (int)SP->motion);
    {   int hp0 = PL->hp;                              /* VOR dem P4/P5-Durchlauf messen */
        SP->rot_y = (int16_t)(((int)re15_atan2_q12(PL->z - SP->z, PL->x - SP->x) - 0x400) & 0xfff);
        SP->sub_state_2 = 4;
        tick();                                        /* P4 -> P5 (Fallthrough, Biss moeglich) */
        CHECK(SP->speed_h == 300, "Angriff-P4: +0x144 = 300 @0x80100E48 (ist %d)", (int)SP->speed_h);
        int guard = 0;
        while (guard++ < 40 && PL->hp == hp0) { SP->re2z_t158 = 20; SP->sub_state_2 = 5; tick(); }
        CHECK(PL->hp == hp0 - 20,
              "Biss muss GENAU 20 HP kosten (`addiu a0,zero,20` @0x80105A78), war %d -> %d",
              hp0, (int)PL->hp);
        CHECK(se_seen(1), "Biss feuert SE 1 (`addiu a0,zero,1` @0x80105B34, jal @0x80105B38)");
        CHECK(SP->sub_state_2 == 6, "Treffer -> +0x6 = 6 @0x80100EF0-F8 (ist %u)", SP->sub_state_2); }
    /* NEGATIV: ausserhalb der Y-Toleranz 4300 (@0x80105B18) darf NICHTS passieren. */
    {   fresh(600, 0, 0); tick(); SP->y = -9000;      /* |dy| = 9000 > 4300 */
        int hp0 = PL->hp;
        SP->sub_state_1 = 7; SP->sub_state_2 = 4; tick();
        for (int i = 0; i < 20; i++) { SP->re2z_t158 = 20; SP->sub_state_2 = 5; tick(); }
        CHECK(PL->hp == hp0, "NEGATIV: |dY| >= 4300 (@0x80105B18) darf keinen Schaden machen"); }
    /* NEGATIV: ausserhalb der Reichweite 1500 (@0x80105A3C-40). */
    {   fresh(4000, 0, 0); tick();
        int hp0 = PL->hp;
        SP->sub_state_1 = 7; SP->sub_state_2 = 4; tick();
        SP->speed_h = 0;                                /* nicht naeher kommen */
        for (int i = 0; i < 20; i++) { SP->re2z_t158 = 20; SP->sub_state_2 = 5; SP->speed_h = 0; tick(); }
        CHECK(PL->hp == hp0, "NEGATIV: Distanz >= 1500 (@0x80105A3C) darf keinen Schaden machen"); }

    /* ---- 8. HURT @0x80102C78, generische Zeile, Modus 0 (@0x80102D54) ---------------------- */
    fresh(2000, 0, 0);
    tick();                                            /* INIT */
    SP->state = 2; SP->sub_state_1 = 1; SP->sub_state_2 = 0;   /* Zeile 1 = Nahkampf/Pistole */
    tick();
    CHECK(SP->motion == 1 && SP->anim_frac == 2,
          "HURT-P0: Clip-Wort 0x00030001 @0x80102DB0-C4 (Clip 1, Xfade 3 minus 1 Decay), ist %d/%u",
          (int)SP->motion, SP->anim_frac);
    CHECK(SP->re2s_row23c == 1, "HURT latcht +0x23C = +0x5 @0x80102CAC (ist %u)", SP->re2s_row23c);
    SP->sub_state_2 = 2;
    tick();
    CHECK(SP->state == 1 && SP->sub_state_1 == 9,
          "HURT-Zeile 1 kehrt fest auf 0x901 zurueck @0x80102E08-58 (ist state %u sub %u)",
          SP->state, SP->sub_state_1);
    /* Zeile != 1 mit +0x1D2 >= 3: rand&7 -> Byte-Tabelle @0x80106578 = {1,9,1,9,1,5,1,5}. */
    {   int saw1 = 0, saw9 = 0, saw5 = 0;
        fresh(2000, 0, 0); tick();                     /* EINMAL seeden — der PRNG-Strom muss
                                                        * ueber die Ziehungen laufen */
        for (int i = 0; i < 64; i++) {
            SP->state = 2; SP->sub_state_1 = 2; SP->sub_state_2 = 2; SP->re2z_hits1d2 = 3;
            tick();
            if (SP->sub_state_1 == 1) saw1 = 1;
            else if (SP->sub_state_1 == 9) saw9 = 1;
            else if (SP->sub_state_1 == 5) saw5 = 1;
            else CHECK(0, "HURT-Tabelle @0x80106578 darf nur 1/9/5 liefern, war %u", SP->sub_state_1);
        }
        CHECK(saw1 && saw9 && saw5, "alle drei Werte der Tabelle @0x80106578 muessen vorkommen"); }
    /* NEGATIV: +0x1D2 < 3 (Region 0) MUSS fest 0x901 liefern (@0x80102E10-30). */
    {   fresh(2000, 0, 0); tick();
        SP->state = 2; SP->sub_state_1 = 2; SP->sub_state_2 = 2; SP->re2z_hits1d2 = 2;
        tick();
        CHECK(SP->sub_state_1 == 9, "NEGATIV: +0x1D2/3 == 0 -> fest Sub 9 @0x80102E2C-58"); }

    /* ---- 9. DEATH @0x80103C80 (Modus 0 @0x80103D8C) -> CORPSE 7 -------------------------- */
    fresh(2000, 0, 0);
    tick();
    SP->state = 3; SP->sub_state_1 = 1; SP->sub_state_2 = 0;
    tick();
    CHECK(SP->motion == 12 && SP->anim_frac == 6,
          "DEATH-P0: Clip-Wort 0x0007000C @0x80103DC0-C8 (Clip 12, Xfade 7 minus 1 Decay), ist %d/%u",
          (int)SP->motion, SP->anim_frac);
    {   int guard = 0; while (guard++ < 60 && SP->state == 3) tick();
        CHECK(SP->state == 7, "DEATH muss auf CORPSE 7 gehen @0x80103E98-9C (ist %u)", SP->state);
        CHECK(SP->hp == 1, "CORPSE-Wiederbelebung HP = 1 @0x80103EB0 (ist %d)", (int)SP->hp);
        CHECK(SP->re2s_done224 == 1, "+0x224 = 1 @0x80103EB8");
        CHECK(SP->re2s_c23a == -1, "+0x23A = -1 @0x80103E84"); }
    /* NEGATIV: mit +0x239 != 0 darf es KEINE Wiederbelebung geben (@0x80103EA0-A8). */
    {   fresh(2000, 0, 0); tick();
        SP->state = 3; SP->sub_state_1 = 1; SP->sub_state_2 = 0; SP->re2s_dead239 = 1; SP->hp = 0;
        int guard = 0; while (guard++ < 60 && SP->state == 3) tick();
        CHECK(SP->state == 7, "DEATH -> CORPSE auch mit +0x239");
        CHECK(SP->hp == 0 && SP->re2s_done224 == 0,
              "NEGATIV: +0x239 != 0 blockt HP=1 und +0x224 @0x80103EA0-B8"); }

    /* ---- 10. CORPSE @0x80104CF8 ----------------------------------------------------------- */
    fresh(2000, 0, 0);
    tick();
    SP->state = 7; SP->sub_state_1 = 0; SP->sub_state_2 = 0;
    tick();
    CHECK(SP->re2s_gs225 == 2, "CORPSE armt +0x225 = 2 @0x80104D80 (ist %u)", SP->re2s_gs225);
    CHECK(SP->sub_state_1 == 1, "CORPSE: Clip fertig -> +0x5 = 1 @0x80104D2C");
    CHECK(SP->re2z_dir16a >= 30 && SP->re2z_dir16a <= 61,
          "CORPSE-Timer (rand&0x1F)+30 @0x80104D30-3C muss 30..61 sein, ist %u", SP->re2z_dir16a);
    {   int guard = 0; while (guard++ < 100 && SP->motion != 13) tick();
        CHECK(SP->motion == 13 && SP->anim_frac == 7,
              "CORPSE spielt Clip 13 mit Xfade 7 erneut @0x80104D58-60 (ist %d/%u)",
              (int)SP->motion, SP->anim_frac); }

    /* ---- 11. Zustand 4 ist ein `jr ra` @0x80104CF0 ----------------------------------------- */
    fresh(2000, 0, 0);
    tick();
    SP->state = 4; SP->sub_state_1 = 3; SP->sub_state_2 = 3; SP->motion = 5;
    { int16_t m = SP->motion; uint8_t s1 = SP->sub_state_1; tick();
      CHECK(SP->state == 4 && SP->sub_state_1 == s1 && SP->motion == m,
            "Zustand 4 ist ein leerer Stub @0x80104CF0-F4 (darf NICHTS aendern)"); }

    /* ---- 12. Root-Zaehler: +0x1D3 low-7 und +0x23D dekrementieren @0x801000EC-114 ---------- */
    fresh(2000, 0, 0);
    tick();
    SP->re2z_self1d3 = 0x85; SP->re2s_c23d = 4;
    tick();
    CHECK(SP->re2z_self1d3 == 0x84, "+0x1D3 low-7 -- unter Erhalt von Bit 0x80 @0x801000EC-100 (ist 0x%02X)",
          SP->re2z_self1d3);
    CHECK(SP->re2s_c23d == 3, "+0x23D -- @0x80100104-114 (ist %u)", SP->re2s_c23d);
    SP->re2z_self1d3 = 0x80;
    tick();
    CHECK(SP->re2z_self1d3 == 0x80, "NEGATIV: low-7 == 0 -> KEIN Dekrement (`andi 0x7f` @0x801000F4)");

    /* ---- 13. +0x106 = (-Y)/1800 (Magic-Div @0x80100140-70, numerisch = 1800, trunkiert) ---- */
    fresh(2000, 0, 0);
    tick();
    SP->y = -3600; tick();
    CHECK(SP->re2s_f106 == 2, "+0x106 = -Y/1800 @0x80100140-70: -3600 -> 2 (ist %u)", SP->re2s_f106);
    SP->y = -1799; tick();
    CHECK(SP->re2s_f106 == 0, "NEGATIV: -1799 -> 0 (Trunkierung, nicht Rundung), ist %u", SP->re2s_f106);

    /* ---- 14. NEGATIV-Gesamttest: unter RE1.5-Flavor darf das Brain NICHTS tun -------------- */
    {   re15_actor_init();
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        PL = &g_actors[RE15_ACTOR_SLOT_PLAYER]; SP = &g_actors[1];
        memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
        PL->active = 1; PL->hp = 100; PL->x = 600;
        SP->active = 1; SP->type = 0x25; SP->state = 0; SP->grid_id = 0;
        re15_enemy_ai_run_all(1);
        CHECK(SP->re2s_mode222 == 0 && SP->re2s_yaw21a == 0 && SP->re2s_f106 == 0,
              "NEGATIV: RE1.5-Flavor darf KEINE RE2-Spinnenfelder schreiben");
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2); }

    printf(fails ? "test_re2_spider_ai: %d FAIL\n" : "test_re2_spider_ai: OK\n", fails);
    return fails ? 1 : 0;
}
