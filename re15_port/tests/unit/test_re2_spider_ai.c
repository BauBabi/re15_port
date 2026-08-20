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
#include "re15_aot.h"        /* sce-8-Wasserzone fuer den FUN_800527B4-Kanal */
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

    /* ---- 1. Besitz-Gate (WELLE F: ausnahmslos) --------------------------------------------
     * Die INIT-Sprungtabelle @0x80100004 waehlt ueber (+0x10E & 0xF) den Oberflaechen-Modus:
     * 0/1 -> +0x222 = 0 (@0x80100460), 2/3 -> 1 (@0x80100480), 4..11 -> 3 (@0x801004C0),
     * >= 12 klemmt `sltiu v0,v1,0xc` @0x8010043C weg (bleibt 0). Mit Modus 1/2/3 portiert
     * gehoert jeder 0x25 UND jeder 0x26 dem RE2-Brain. */
    fresh(0, 0, 0);
    for (int g = 0; g < 16; g++) {
        SP->grid_id = (uint8_t)g;
        CHECK(re15_re2spider_owns(SP) == 1, "owns(grid %d) muss 1 sein (WELLE F, Tabelle @0x80100004)", g);
    }
    SP->grid_id = 0; SP->type = 0x26;
    CHECK(re15_re2spider_owns(SP) == 1, "owns(Baby 0x26) == 1 (EMS26.BIN-Brain @0x8010001C)");
    CHECK(re15_re2_owns_type(0x26) == 1, "0x26 muss im RE2-Asset-Ownership-Set sein (Modell+Brain)");
    /* NEGATIV: fremde Typen bleiben aussen vor. */
    SP->type = 0x27; CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV: 0x27 (Gorilla) gehoert nicht der Spinne");
    SP->type = 0x20; CHECK(re15_re2spider_owns(SP) == 0, "NEGATIV: 0x20 (Hund) gehoert nicht der Spinne");
    CHECK(re15_re2spider_owns(0) == 0, "NEGATIV: NULL -> 0");
    SP->type = 0x25;

    /* ---- 1b. WELLE F: die drei Oberflaechen-Modi laufen jetzt im Brain --------------------
     * INIT-Tabelle: grid 2/3 -> +0x222 = 1 (@0x80100480) mit Roll +0x78 = 2048 (@0x8010048C)
     * und +0x22C = Y + 1250 (@0x801004A4-B0); grid 4..11 -> +0x222 = 3 (@0x801004C0) mit
     * Quadrant (+0x10E-4)>>1 (@0x801004CC-DC) und Yaw = Quadrant<<10 (@0x801004E0). */
    {   fresh(0, 0, 2); SP->y = -1000; tick();
        CHECK(SP->re2s_mode222 == 1, "grid 2 -> +0x222 = 1 (DECKE) @0x80100480 (ist %u)", SP->re2s_mode222);
        CHECK(SP->rot_z == 2048, "grid 2 -> +0x78 = 2048 @0x8010048C (ist %d)", (int)SP->rot_z);
        CHECK(SP->re2s_y22c == -1000 + 1250, "grid 2 -> +0x22C = Y+1250 @0x801004A4-B0 (ist %d)",
              (int)SP->re2s_y22c);
        CHECK(SP->re2s_yaw21a == 2048, "grid 2 -> +0x21A = 2048 @0x80100488"); }
    {   fresh(0, 0, 7); tick();
        CHECK(SP->re2s_mode222 == 3, "grid 7 -> +0x222 = 3 (WAND) @0x801004C0 (ist %u)", SP->re2s_mode222);
        CHECK(SP->re2s_q230 == ((7 - 4) >> 1), "grid 7 -> +0x230 = (7-4)>>1 = 1 @0x801004CC-DC (ist %u)",
              SP->re2s_q230);
        CHECK(SP->rot_y == (int16_t)(1 << 10), "grid 7 -> +0x76 = Quadrant<<10 @0x801004E0 (ist %d)",
              (int)SP->rot_y); }
    /* NEGATIV: Boden-Spawn darf KEINEN dieser Werte tragen. */
    {   fresh(0, 0, 0); tick();
        CHECK(SP->re2s_mode222 == 0 && SP->rot_z == 0 && SP->re2s_q230 == 0,
              "NEGATIV: grid 0 bleibt Boden (@0x80100460), kein Roll/Quadrant"); }

    /* ---- 1c. WAND-Substate 0 @0x80102B7C: FUN_80101D04 + `+0x5 = 2` (@0x80102B90-98) ------ */
    {   fresh(0, 0, 7); tick();               /* INIT -> +0x222 = 3 */
        SP->sub_state_1 = 0; SP->sub_state_2 = 0; SP->sub_state_3 = 0;
        tick();
        CHECK(SP->sub_state_1 == 2, "WAND-Sub0 -> +0x5 = 2 @0x80102B94 (ist %u)", SP->sub_state_1);
        CHECK(SP->re2s_gs225 == 1, "FUN_80101D04 setzt +0x225 = 1 @0x80101D8C (ist %u)", SP->re2s_gs225);
        CHECK(SP->re2s_yaw226 == 0, "FUN_80101D04 setzt +0x226 = 0 @0x80101D9C");
        /* +0x230 = 1 (ungerade) -> +0x74 = +0x76, +0x78 = 0 (@0x80101D24-30) */
        CHECK(SP->rot_x == SP->rot_y && SP->rot_z == 0,
              "FUN_80101D04 (+0x230 ungerade): +0x74 = +0x76, +0x78 = 0 @0x80101D24-30"); }
    /* NEGATIV: bei GERADEM Quadranten dreht es genau andersherum (@0x80101D34-3C). */
    {   fresh(0, 0, 4); tick();               /* (4-4)>>1 = 0 -> gerade */
        SP->sub_state_1 = 0; SP->sub_state_2 = 0; SP->sub_state_3 = 0;
        tick();
        CHECK(SP->rot_x == 0 && SP->rot_z == 1024,
              "NEGATIV/Gegenprobe (+0x230 gerade): +0x74 = 0, +0x78 = 1024 @0x80101D34-3C"); }
    fresh(0, 0, 0);

    /* ---- 2. Ownership-Set + ENEMSE-Bank ---------------------------------------------------- */
    CHECK(re15_re2_owns_type(0x25) == 1, "0x25 muss im RE2-Ownership-Set sein (Asset-Loader)");
    CHECK(re15_re2_owns_type(0x27) == 0, "NEGATIV: Gorilla 0x27 bleibt RE1.5");
    /* ⛔ KORRIGIERT 2026-08-20: vorher Adult 53 / Baby 24. Der alte Pin las die Paar-Tabelle
     * @0x800A7400 als Liste von GEGNER-KINDS ("Zeile {0x25,0x1F}"). Sie fuehrt aber die
     * SOUND-ID aus dem Sce_em_set-Record (+7 -> entity+0x1FA, `lbu v0,7(v0)` @0x80057274;
     * verglichen wird nur +0x1FA, `lb v1,506(a0)` @0x80052C48) — die "0x25"/"0x26" dort sind
     * Sound-Ids fremder Gegner (Bank 53 = kind 0x12 + 0x41, Bank 24 = kind 0x34). Zensus ueber
     * alle 250 RDTs in info/re2leon/PL0/RDT: kind 0x25 -> Sound-Id 0x10 (12/12), kind 0x26 ->
     * 0x10 (27/28) — DIESELBE Id, also DIESELBE Bank. 0x10 steht in den Zeilen 11/65/66, immer
     * als k0 (flag2000 = 0); die Allein-Zeile ist 11. Daten-Gegenprobe: die erste Haelfte von
     * Bank 11 ist byte-identisch mit der von 65 und 66. Herleitung im Kopf von
     * enemy_ai_re2_spider.c. */
    CHECK(se_bank_sel == 11, "Adult-ENEMSE-Bank = 11 (Sound-Id 0x10 -> Zeile 11 {0x10,0x00})");
    re15_re2spider_audio_hook(se_cap, se_bank_cap, 1);
    CHECK(se_bank_sel == 11, "Baby teilt sich die Bank 11 mit der Adult (beide Sound-Id 0x10)");
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
        /* NEGATIV-HAELFTE des Wasser-Kanals: OHNE Wasserzone liefert der Pull FUN_800527B4
         * seinen Nicht-Treffer-Wert 0 (@0x800528C4) -> der Zweig `+0x10C != 0` @0x80103E3C
         * laeuft NICHT, also bleibt +0x23A = 0 (@0x80103E84 liegt INNERHALB des Zweigs). */
        CHECK(SP->re2s_c23a == 0,
              "+0x23A bleibt 0: @0x80103E84 liegt im Wasser-Zweig @0x80103E3C (ist %d)",
              (int)SP->re2s_c23a); }
    /* POSITIV-HAELFTE: eine ECHTE sce-8-Wasserzone (RE1.5 @0x8007469c[8] = 0x8004330c,
     * Zwilling von RE2 sce 7 @0x800A73C4[7] = 0x80051a2c) ueber der Spinne. Der Modul-Kopf
     * stempelt +0x10C aus dem Pull (re15_aot_water_at == FUN_800527B4 @0x800527B4) und der
     * Wasser-Zweig feuert genau wie im Original. Zonengeometrie/Nutzlast wie ROOM2090:
     * p0 = -1620 (Aot_set @0x09D6/@0x09EA/@0x09FE, Payload pc[14..15]). */
    {   fresh(2000, 0, 0);
        re15_aot_init();
        re15_aot_set(2, RE15_AOT_TYPE_WATER, 0, 0, 0, 8050, 4550);
        g_aot.env_params[2].p0 = -1620;
        tick();
        CHECK(SP->re2s_water10c == -1620,
              "Wasser-Stempel +0x10C == p0 der sce-8-Zone (@0x80051a3c / RE1.5 @0x8004331c), ist %d",
              (int)SP->re2s_water10c);
        SP->state = 3; SP->sub_state_1 = 1; SP->sub_state_2 = 0;
        int guard = 0; while (guard++ < 60 && SP->state == 3) tick();
        CHECK(SP->re2s_c23a == -1, "Wasser-Zweig: +0x23A = -1 @0x80103E84 (ist %d)", (int)SP->re2s_c23a);
        CHECK(SP->re2s_sink23e == 3 && SP->re2s_sink23f == 1,
              "Wasser-Zweig: +0x23E = 3 / +0x23F = 1 @0x80103E8C-94");
        /* CORPSE @0x80104D98 ruft unter +0x23A < 0 den Sink-Treiber FUN_80104F18. Mit
         * +0x23E == 3 laeuft der Duempel-Zweig @0x80105034: Y = Wasser + tab[..] + 70,
         * tab @0x8010667C = {0,8,16,32,32,16,8,0} -> Y in [-1550 .. -1518]. */
        int y0 = (int)SP->y;
        tick();
        CHECK(SP->y >= -1620 + 70 && SP->y <= -1620 + 32 + 70,
              "FUN_80104F18 Duempeln @0x8010503C-64: Y in [-1550..-1518], war %d (vorher %d)",
              (int)SP->y, y0);
        /* NEGATIV zum Sink-Treiber: verlaesst die Leiche die Zone, liefert der Pull 0 und
         * @0x80104F34-50 klemmt Y hart auf (s16)+0x1C2 und setzt +0x23A = 1 / +0x23F = 0. */
        SP->x = 100000; SP->dog_floor_y = -300;
        tick();
        CHECK(SP->y == -300 && SP->re2s_c23a == 1 && SP->re2s_sink23f == 0,
              "NEGATIV FUN_80104F18 @0x80104F44-50 (kein Wasser): Y=%d +0x23A=%d +0x23F=%u",
              (int)SP->y, (int)SP->re2s_c23a, SP->re2s_sink23f);
        re15_aot_init(); }
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


    /* ================= WELLE F ============================================================= */

    /* ---- F1. ZEILEN-UEBERSETZUNG: +0x5 (RE1.5-Waffen-Id) -> RE2-Zeile ---------------------
     * Die Karte ist waffen-identisch zu re2z_row_from_weapon (enemy_ai_re2_zombie.c, dort mit
     * Item-Tabelle @0x800A9E1C + PLW-Gegenprobe hergeleitet). Geprueft wird die WIRKUNG: die
     * Zeile landet in +0x23C (@0x80102C94-AC) und waehlt den Handler aus @0x80106518. */
    {   struct { uint8_t w, row; } map[] = {
            { 0, 1}, { 1, 1}, { 2, 1}, { 3, 3}, { 4, 2}, { 5, 4}, { 6, 4}, { 7, 5},
            { 8, 7}, { 9, 9}, {10,11}, {11,10}, {12,15}, {13, 8}, {14,16}, {15, 9},
            {16,11}, {17,10}, {18,17}, {19,18}, {20,13}, {21, 1} };
        for (unsigned i = 0; i < sizeof map / sizeof map[0]; i++) {
            fresh(2000, 0, 0); tick();
            SP->state = 2; SP->sub_state_1 = map[i].w; SP->sub_state_2 = 0;
            tick();
            CHECK(SP->re2s_row23c == map[i].row,
                  "Zeilen-Karte w%u -> RE2-Zeile %u (+0x23C @0x80102CAC), ist %u",
                  map[i].w, map[i].row, SP->re2s_row23c);
        }
        /* NEGATIV/Klemme: ausserhalb 0..21 -> Zeile 1 (PORT-SICHERUNG, im Original unmoeglich). */
        fresh(2000, 0, 0); tick();
        SP->state = 2; SP->sub_state_1 = 200; SP->sub_state_2 = 0; tick();
        CHECK(SP->re2s_row23c == 1, "NEGATIV: Zeile ausserhalb 0..21 klemmt auf 1 (ist %u)",
              SP->re2s_row23c); }

    /* ---- F2. HURT-Sonderzeile @0x80102FDC (Modus 0 @0x80103018) — NIEDERSCHLAG -------------
     * w7 (Magnum) -> Zeile 5. P0: Clip-Wort 0x00030001 (@0x80103088) + FUN_80105BF0 (Bein).
     * P2: Zustandswort 0x901 (@0x801030B4) — NICHT die Rand-Tabelle der generischen Zeile. */
    {   fresh(2000, 0, 0); tick();
        SP->state = 2; SP->sub_state_1 = 7; SP->sub_state_2 = 0;
        uint8_t legs0 = SP->re2s_legn221;
        tick();
        CHECK(SP->re2s_row23c == 5, "w7 -> Zeile 5 (Sonderzeile @0x80102FDC)");
        CHECK(SP->motion == 1, "Sonderzeile P0: Clip 1 @0x80103088 (ist %d)", (int)SP->motion);
        CHECK(SP->re2s_legn221 <= legs0, "FUN_80105BF0 darf Beine nur abziehen @0x80105C74");
        int guard = 0; while (guard++ < 40 && SP->state == 2) tick();
        CHECK(SP->state == 1 && SP->sub_state_1 == 9,
              "Sonderzeile P2: Zustandswort 0x901 @0x801030B4 (ist %u/%u)",
              SP->state, SP->sub_state_1); }

    /* ---- F3. FUN_80105BF0: Bein-Budget +0x221 (INIT 8 @0x801003B4), Gate `< 3` @0x80105C20 -- */
    {   fresh(2000, 0, 0); tick();
        CHECK(SP->re2s_legn221 == 8, "INIT: +0x221 = 8 @0x80100374/@0x801003B4 (ist %u)",
              SP->re2s_legn221);
        int guard = 0;
        while (guard++ < 400 && SP->re2s_legn221 > 2) {
            SP->state = 2; SP->sub_state_1 = 7; SP->sub_state_2 = 0; tick();
            int g2 = 0; while (g2++ < 40 && SP->state == 2) tick();
        }
        CHECK(SP->re2s_legn221 >= 2,
              "NEGATIV: +0x221 darf nie unter 2 fallen (Gate `< 3` @0x80105C20), ist %u",
              SP->re2s_legn221); }

    /* ---- F4. HURT-Zeile 11 @0x801039AC: Bein abschiessen, danach GENERISCHE Zeile ---------
     * w10 -> Zeile 11. Der Handler setzt +0x5 = 2 (@0x80103AE0) und faellt in @0x80102D18. */
    {   fresh(2000, 0, 0); tick();
        SP->state = 2; SP->sub_state_1 = 10; SP->sub_state_2 = 0;
        uint8_t legs0 = SP->re2s_legn221;
        tick();
        CHECK(SP->re2s_row23c == 11, "w10 -> Zeile 11 @0x801039AC (ist %u)", SP->re2s_row23c);
        CHECK(SP->sub_state_1 == 2, "Zeile 11 setzt +0x5 = 2 @0x80103AE0 (ist %u)", SP->sub_state_1);
        CHECK(SP->re2s_legn221 == (uint8_t)(legs0 - 1) || SP->re2s_legs220 != 0,
              "Zeile 11: +0x221-- @0x80103A3C bzw. Bein schon weg"); }

    /* ---- F5. DEATH-Sonderzeile @0x801044D0 (Modus 0 @0x8010452C) -> BABY-SPINNEN -----------
     * w7 -> Zeile 5. P0: +0x23A = 1 (@0x80104574), Clip-Wort 0x0007000C (@0x80104578),
     * FUN_80105D38(self, 0x2002, (rand&3)+6) (@0x80104590-A4) = 6..9 Kinder vom Typ 0x26. */
    {   fresh(2000, 0, 0); tick();
        SP->state = 3; SP->sub_state_1 = 7; SP->sub_state_2 = 0;
        tick();
        CHECK(SP->re2s_row23c == 5, "DEATH w7 -> Zeile 5 (@0x801044D0)");
        CHECK(SP->re2s_c23a == 1, "DEATH-Sonderzeile P0: +0x23A = 1 @0x80104574 (ist %d)",
              (int)SP->re2s_c23a);
        CHECK(SP->motion == 12, "DEATH-Sonderzeile P0: Clip 12 @0x80104578 (ist %d)", (int)SP->motion);
        int babies = 0;
        for (int i = 0; i < RE15_ACTOR_MAX; i++)
            if (g_actors[i].active && g_actors[i].type == 0x26) babies++;
        CHECK(babies >= 6 && babies <= 9,
              "DEATH-Sonderzeile spawnt 6..9 Babys (a2 = (rand&3)+6 @0x801045A8), sind %d", babies);
        /* Das Spawn-Wort ist 0x2002 (@0x8010459C) -> Variante 2 -> Start-Substate 1 (BODEN,
         * Byte-Tabelle @0x801010A4). */
        for (int i = 0; i < RE15_ACTOR_MAX; i++)
            if (g_actors[i].active && g_actors[i].type == 0x26) {
                CHECK(g_actors[i].re2z_f10e == 0x2002,
                      "Kind +0x10E = 0x2002 @0x8010459C/@0x80105E0C (ist 0x%04X)",
                      g_actors[i].re2z_f10e);
                break;
            } }

    /* ---- F6. Der SPAWN-DECKEL: 17 lebende 0x26 ausser self (`sltiu 0x12` @0x80105DB0) ------ */
    {   fresh(2000, 0, 0); tick();
        for (int n = 0; n < 12; n++) {
            SP->state = 3; SP->sub_state_1 = 7; SP->sub_state_2 = 0; SP->re2s_done224 = 0;
            tick();
        }
        int babies = 0;
        for (int i = 0; i < RE15_ACTOR_MAX; i++)
            if (g_actors[i].active && g_actors[i].type == 0x26) babies++;
        CHECK(babies <= 17, "NEGATIV: nie mehr als 17 lebende 0x26 @0x80105DB0-C0 (sind %d)", babies); }

    /* ---- F7. Zeile 14 vetot den Spawn (@0x80105D88) und das Bein-Abtrennen (@0x80105C04) --- */
    {   fresh(2000, 0, 0); tick();
        /* Zeile 14 ist im Port ueber die Waffen-Karte NICHT erreichbar (RE2-Id 14 = Spark Shot).
         * Deshalb direkt gesetzt UND +0x6 != 0, damit re2s_row_translate den Wert nicht als
         * RE1.5-Waffen-Id umdeutet (die Uebersetzung laeuft nur auf dem frischen Treffer). */
        SP->state = 3; SP->sub_state_1 = 14; SP->sub_state_2 = 1;
        SP->re2s_row23c = 14;
        SP->re2s_done224 = 0;
        tick();
        int babies = 0;
        for (int i = 0; i < RE15_ACTOR_MAX; i++)
            if (g_actors[i].active && g_actors[i].type == 0x26) babies++;
        CHECK(babies == 0, "NEGATIV: +0x23C == 14 verbietet JEDEN Spawn @0x80105D84-88 (sind %d)",
              babies);
        /* 30 @0x80104C98, danach zieht der FX-Emitter FUN_80106140 im Root-Tail (@0x80100188,
         * Dec @0x80106164-68) noch im SELBEN Frame eins ab -> 29. */
        CHECK(SP->re2s_fx23b == 29, "Zeile 14: +0x23B = 30 @0x80104C98 minus 1 Root-Tick "
              "@0x80106164 (ist %u)", SP->re2s_fx23b); }

    /* ---- F8. Generischer DEATH-Abschluss @0x80104440: 1..4 Babys ---------------------------
     * Die generische Zeile setzt beim ersten Tod HP=1 und +0x224 = 1 (@0x80103EB0/B8); ein
     * WEITERER Treffer schickt sie erneut in DEATH und damit in den Abschluss-Ausbruch. */
    {   fresh(2000, 0, 0); tick();
        SP->state = 3; SP->sub_state_1 = 1; SP->sub_state_2 = 0;
        int guard = 0; while (guard++ < 60 && SP->state == 3) tick();
        CHECK(SP->re2s_done224 == 1, "generische Zeile setzt +0x224 = 1 @0x80103EB8");
        SP->state = 3; SP->sub_state_1 = 1; SP->sub_state_2 = 0;
        tick();
        int babies = 0;
        for (int i = 0; i < RE15_ACTOR_MAX; i++)
            if (g_actors[i].active && g_actors[i].type == 0x26) babies++;
        CHECK(babies >= 1 && babies <= 4,
              "Abschluss-Ausbruch @0x80104478: 1..4 Babys ((rand&3)+1), sind %d", babies);
        CHECK(SP->motion == 13, "Abschluss-Ausbruch: Clip 13 @0x80104460 (ist %d)", (int)SP->motion); }

    /* ---- F9. BABY-BRAIN (EMS26.BIN) -------------------------------------------------------
     * INIT @0x801000DC: HP = 1 (@0x801000F8), Clip-Wort 0x000F0000 (@0x80100120),
     * +0x21E = 120 (@0x80100128), +0x1D3 = 6 (@0x801001B0); Start-Substate aus der Byte-Tabelle
     * @0x801010A4 = {0,0,1,4,2,2,0,0} nach (+0x10E & 0xFF). */
    {   re15_actor_init();
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        PL = &g_actors[RE15_ACTOR_SLOT_PLAYER]; SP = &g_actors[1];
        memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
        PL->active = 1; PL->hp = 100; PL->x = 4000;
        SP->active = 1; SP->type = 0x26; SP->state = 0; SP->re2z_f10e = 0x2002;
        re15_re2spider_baby_tick(1);
        CHECK(SP->hp == 1, "Baby-INIT: HP = 1 @0x801000F8 (ist %d)", (int)SP->hp);
        CHECK(SP->motion == 0 && SP->anim_frac == 15,
              "Baby-INIT: Clip-Wort 0x000F0000 @0x80100120 (Rate 15; INIT ruft KEIN "
              "FUN_8002959C, also kein Decay), ist %d/%u", (int)SP->motion, SP->anim_frac);
        CHECK(SP->re2s_c21e == 120, "Baby-INIT: +0x21E = 120 @0x80100128 (ist %u)", SP->re2s_c21e);
        CHECK(SP->state == 1 && SP->sub_state_1 == 1,
              "Baby +0x10E = 0x2002 -> Variante 2 -> Substate 1 (Tabelle @0x801010A4), ist %u/%u",
              SP->state, SP->sub_state_1);
        /* NEGATIV: Variante 4 (0x2004) startet in Substate 2 (FALL) und traegt +0x144 = 200. */
        memset(SP, 0, sizeof *SP);
        SP->active = 1; SP->type = 0x26; SP->state = 0; SP->re2z_f10e = 0x2004;
        re15_re2spider_baby_tick(1);
        CHECK(SP->sub_state_1 == 2, "Baby 0x2004 -> Substate 2 (FALL) @0x801010A4[4] (ist %u)",
              SP->sub_state_1);
        CHECK(SP->speed_h == 200, "Baby 0x2004: +0x144 = 200 @0x80100234 (ist %d)", (int)SP->speed_h);
        /* NEGATIV: Variante 0 ist UNVERWUNDBAR (HP = -1 @0x80100204) und setzt +0x10E |= 0x4000. */
        memset(SP, 0, sizeof *SP);
        SP->active = 1; SP->type = 0x26; SP->state = 0; SP->re2z_f10e = 0x0000;
        re15_re2spider_baby_tick(1);
        CHECK(SP->hp == -1, "Baby-Variante 0: HP = -1 @0x80100204 (ist %d)", (int)SP->hp);
        CHECK((SP->re2z_f10e & 0x4000u) != 0, "Baby-Variante 0: +0x10E |= 0x4000 @0x80100214-20"); }

    /* ---- F10. BABY: ZERTRETEN @0x801003D4-448 ---------------------------------------------
     * Vier Bedingungen: dist < 500 (@0x801003DC), Schrittbits & 6 (@0x801003F4), gleiche
     * Etage +0x106 (@0x80100410), Spieler-Zustandswort in {0x101,0x201,0x301,0x401}
     * (@0x80100424-3C). Substate 5 (RENNEN) ist AUSGESCHLOSSEN. */
    {   re15_actor_init(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        PL = &g_actors[RE15_ACTOR_SLOT_PLAYER]; SP = &g_actors[1];
        memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
        PL->active = 1; PL->hp = 100; PL->x = 100; PL->state = 1; PL->sub_state_1 = 1;
        PL->motion = 105;                                  /* WALK -> 0x800CFBF6 |= 2 */
        SP->active = 1; SP->type = 0x26; SP->state = 1; SP->sub_state_1 = 1;
        SP->re2z_f10e = 0x2002;
        re15_re2spider_baby_tick(1);
        CHECK(SP->state == 3, "Baby zertreten -> Zustand 3 @0x80100448 (ist %u)", SP->state);
        /* NEGATIV a: RENNEN (Spieler-Substate 5) darf NICHT zertreten. */
        memset(SP, 0, sizeof *SP);
        SP->active = 1; SP->type = 0x26; SP->state = 1; SP->sub_state_1 = 1; SP->re2z_f10e = 0x2002;
        PL->sub_state_1 = 5; PL->motion = 100;
        re15_re2spider_baby_tick(1);
        CHECK(SP->state != 3, "NEGATIV: Spieler-Substate 5 (Rennen) zertritt NICHT @0x80100424-3C");
        /* NEGATIV b: ohne Schrittpuls (Stillstand) kein Zertreten. */
        memset(SP, 0, sizeof *SP);
        SP->active = 1; SP->type = 0x26; SP->state = 1; SP->sub_state_1 = 1; SP->re2z_f10e = 0x2002;
        PL->sub_state_1 = 1; PL->motion = 0;
        re15_re2spider_baby_tick(1);
        CHECK(SP->state != 3, "NEGATIV: kein Schrittpuls (0x800CFBF6 & 6 == 0) -> kein Zertreten");
        /* NEGATIV c: falsche Etage. */
        memset(SP, 0, sizeof *SP);
        SP->active = 1; SP->type = 0x26; SP->state = 1; SP->sub_state_1 = 1; SP->re2z_f10e = 0x2002;
        SP->y = -5400; SP->re2s_f106 = 3;   /* +0x106 wird erst im Root-TAIL @0x80100080-B0
                                             * geschrieben -> hier der Wert des Vorframes */
        PL->motion = 105;
        re15_re2spider_baby_tick(1);
        CHECK(SP->state != 3, "NEGATIV: andere Etage (+0x106) -> kein Zertreten @0x80100410"); }

    /* ---- F11. BABY-Tod @0x80100BFC: erster Tick = Platscher, zweiter = Despawn ------------- */
    {   re15_actor_init(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
        PL = &g_actors[RE15_ACTOR_SLOT_PLAYER]; SP = &g_actors[1];
        memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
        PL->active = 1; PL->hp = 100;
        SP->active = 1; SP->type = 0x26; SP->state = 3; SP->re2z_f10e = 0x2002;
        se_n = 0;
        re15_re2spider_baby_tick(1);
        CHECK(SP->active == 1 && SP->sub_state_2 == 1,
              "Baby-Tod: erster Tick setzt +0x6 = 1 @0x80100CA4, kein Despawn");
        CHECK(se_seen(7), "Baby-Tod: SE 7 @0x80100CF8");
        re15_re2spider_baby_tick(1);
        CHECK(SP->active == 0, "Baby-Tod: zweiter Tick despawnt (FUN_8001B250 @0x80100D24)"); }

    /* ---- F12. NEGATIV-Gesamttest fuer das Baby: RE1.5-Flavor unveraendert ------------------ */
    {   re15_actor_init(); re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
        PL = &g_actors[RE15_ACTOR_SLOT_PLAYER]; SP = &g_actors[1];
        memset(PL, 0, sizeof *PL); memset(SP, 0, sizeof *SP);
        PL->active = 1; PL->hp = 100; PL->x = 100; PL->state = 1; PL->sub_state_1 = 1;
        PL->motion = 105;
        SP->active = 1; SP->type = 0x26; SP->state = 1; SP->sub_state_1 = 1;
        re15_enemy_ai_run_all(1);
        CHECK(SP->state != 3,
              "NEGATIV: unter RE1.5-Flavor darf das RE2-Zertret-Gate NICHT feuern");
        re15_ai_flavor_set(RE15_AI_FLAVOR_RE2); }

    printf(fails ? "test_re2_spider_ai: %d FAIL\n" : "test_re2_spider_ai: OK\n", fails);
    return fails ? 1 : 0;
}
