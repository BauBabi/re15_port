/**
 * @file test_re2_zombie_pose_bank.c
 * @brief PIN — DIE POSE-BANK DES RE2-ZOMBIES (re15_re2z_poses_loco_bank).
 *
 * ⛔ NUTZER-REPORT (DRITTER Anlauf, nach v0.3.7, beide RE2-Modi): "Die Zombies fallen teilweise
 * IMMER NOCH komisch hin, wenn sie einmal mit Waffe getroffen werden, oder haben manchmal eine
 * kurze Hinfall-Animation, stehen dann aber sofort wieder."
 *
 * GEMESSEN (probe_re2z_fallstand, 128 Seeds x 4 Schuss-Fahrplaene x 900 Frames = 512 Laeufe,
 * ROOM1140, echter game_step-Weg, GELADENE RE2-Bank EM010, Pose ueber die Renderer-Bankwahl).
 * EIN Pistolenschuss, Seed 19, Brustknochen 8 relativ zum Aktor-Boden:
 *   f59  st=2 s1=3 s2=2 clip=2 af=15/60 ACT   b8dy=-2660
 *   f68  st=2 s1=3 s2=2 clip=2 af=24/60 ACT   b8dy=-1882
 *   f70  st=2 s1=3 s2=3 clip=2 af=26/60 ACT   b8dy=-1521
 *   f71  st=1 s1=2 s2=0 clip=2 af=27/54 LOCO  b8dy=-2728   <- STEHT WIEDER, in EINEM Frame
 * Der Zombie faellt gar nicht: der Port spielt DENSELBEN Clip-Index aus der FALSCHEN BANK.
 * Clip 2 ist in der LOCO-Bank (Paar 1) der 54-Frame-GANG und in der Aktions-Bank (Paar 2) der
 * 60-Frame-STURZ (re2z_param_clips[1] = 0x02 @0x80100044).
 *
 * URSACHE (selbst disassembliert, EMOVL10_S0.BIN + info/re2leon/PSX.EXE): das Original hat keine
 * per-Frame-Bankregel — die Bank ist das (EMR,EDD)-Argument der Advance-Aufrufstelle:
 *   800295a8: lbu  v0,332(a0)   ; +0x14C Clip-Byte
 *   800295d0: lbu  v0,333(a0)   ; +0x14D Frame-Byte
 *   800295f8: sw   a2,376(a0)   ; entity+0x178 = der POSIERTE Frame-Eintrag
 *   800295fc: jal  0x80029614   ; posiert mit dem UEBERGEBENEN EMR
 * Der Wurzel-Dispatcher reicht PAAR 2 durch (`lw a1,0x180` @0x801004C8/@0x80100548); einzelne
 * Handler laden PAAR 1 EXPLIZIT (`lw a1,264`=+0x108 / `lw a2,380`=+0x17C). Eigener Scan ueber
 * ALLE 54 `jal 0x8002959c` des Overlays — die Paar-1-Stellen sind GENAU:
 *   EXEC[0] STAND P2/P3 @0x80101610 | EXEC[1] WALK @0x80101CD0 | EXEC[2] BUMP @0x801023F0
 *   EXEC[10] @0x80104240 | hit_MAIN P1 @0x80105790, P2 @0x801058C0
 *   hit_SLIDE @0x801071E4/@0x8010729C/@0x80107364 | death_MAGNUM P4 @0x801095D0
 *   death_RIP P6 @0x80109288 | Zustand-8-Wurzel @0x80109E30
 * Die Port-Regel re15_actor_uses_loco_bank() (enemy_ai_common.c) kennt davon nur WALK und BUMP.
 *
 * A/B UEBER DENSELBEN SWEEP (probe_re2z_fallstand 128 900 -1 3 25 -1500 900 <0|1>):
 *   Port-Regel heute : (1) 136 Vorfaelle in 88/512 Laeufen, (4) 219 Pose-Spruenge in 106/512
 *   Disasm-Karte     : (1)   8 Vorfaelle in  8/512 Laeufen, (4)  36 Pose-Spruenge in   8/512
 *
 * DIESER TEST PINNT DIE KARTE — jede Zelle mit ihrer Advance-Adresse. Er ist die Regressionswache
 * gegen ein erneutes "nur WALK/BUMP".
 *
 * ⚠ OFFEN (mit Adresse, nicht geraten): der UEBERGABE-TICK. hit_MAIN P3 committet 0xB01
 * (@0x80105ACC-D4) bzw. 0xC01 (@0x80105B08-10); im Original posiert dieser eine Frame noch den
 * Paar-1-Eintrag aus P2 (entity+0x178 steht bis zum naechsten Advance), im Port kippt die
 * per-Frame-Regel sofort auf Paar 2. Das sind die verbleibenden 8 Ein-Frame-Vorfaelle oben.
 * Dieselbe Klasse ist fuer RE1.5 schon benannt (POISE-BREAK-UEBERGABETICK, enemy_ai_common.c);
 * sie braucht einen +0x178-Zwilling und wird NICHT mit einer Heuristik gefuellt.
 */
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_ai_flavor.h"

#include <stdio.h>
#include <string.h>

extern int re15_re2z_poses_loco_bank(const re15_actor_t *a);
extern int re15_re2z_owns_type(unsigned type);

static int s_fail = 0;
static void chk(int got, int want, const char *what)
{
    if (got != want) { printf("FAIL %-58s erwartet %d, bekommen %d\n", what, want, got); s_fail++; }
    else             printf("ok   %-58s = %d\n", what, got);
}

/* Ein frischer RE2-Zombie in einem definierten Zustand. */
static re15_actor_t mk(uint8_t st, uint8_t s1, uint8_t s2)
{
    re15_actor_t a;
    memset(&a, 0, sizeof a);
    a.active = 1; a.type = 0x10;
    a.state = st; a.sub_state_1 = s1; a.sub_state_2 = s2;
    a.re2z_hits1d2 = 1;            /* Basis-Zone 1, Bracket 0 — was re15_re2_stamp_hit stempelt */
    return a;
}

int main(void)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    printf("=== PIN: Pose-Bank des RE2-Zombies (Paar 1 = LOCO) ===\n");

    /* ---- Zustand 1: die Executor-Tabelle @0x8010C8CC ---------------------------------------- */
    {   re15_actor_t a = mk(1, 0, 0);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[0] STAND P0 -> Paar2 (@0x801014A0)");
        a.sub_state_2 = 1;
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[0] STAND P1 -> Paar2 (@0x801014A0)");
        a.sub_state_2 = 2;
        chk(re15_re2z_poses_loco_bank(&a), 1, "EXEC[0] STAND P2 -> Paar1 (@0x80101610)");
        a.sub_state_2 = 3;
        chk(re15_re2z_poses_loco_bank(&a), 1, "EXEC[0] STAND P3 -> Paar1 (@0x80101610)");
    }
    {   re15_actor_t a = mk(1, 1, 1);
        chk(re15_re2z_poses_loco_bank(&a), 1, "EXEC[1] WALK -> Paar1 (@0x80101CD0)"); }
    {   re15_actor_t a = mk(1, 2, 1);
        chk(re15_re2z_poses_loco_bank(&a), 1, "EXEC[2] BUMP -> Paar1 (@0x801023F0)"); }
    {   re15_actor_t a = mk(1, 3, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[3] GRAB -> Paar2 (@0x801027F8)"); }
    {   re15_actor_t a = mk(1, 5, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[5] KNOCKDOWN -> Paar2 (@0x8010337C)"); }
    {   re15_actor_t a = mk(1, 7, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[7] LIEGEND -> Paar2 (@0x801037F4)"); }
    {   re15_actor_t a = mk(1, 8, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[8] FRESSEN -> Paar2 (@0x80103C24)"); }
    {   re15_actor_t a = mk(1, 9, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[9] AUFSTEHEN -> Paar2 (@0x80104128)"); }
    {   re15_actor_t a = mk(1, 11, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[11] -> Paar2 (@0x801044EC)"); }
    {   re15_actor_t a = mk(1, 12, 1);
        chk(re15_re2z_poses_loco_bank(&a), 0, "EXEC[12] LUNGE -> Paar2 (@0x8010486C)"); }

    /* ---- Zustand 2: die HURT-Tabelle @0x8010C940, Spalte 1 (die einzige, die der Port stempelt)
     * Zeile 3 = Browning HP -> 0x80105438 hit_MAIN. Das IST der Fall des Nutzer-Reports. */
    {   re15_actor_t a = mk(2, 3, 2);
        chk(re15_re2z_poses_loco_bank(&a), 1,
            "HURT Zeile3 (MAIN) -> Paar1 (@0x80105790/@0x801058C0)");
        a.re2z_flag222 = 1;      /* @0x80105458-70: MAIN gibt sofort an hit_STAGGER ab */
        chk(re15_re2z_poses_loco_bank(&a), 0,
            "HURT Zeile3 + 0x222==1 -> STAGGER/Paar2 (@0x80105F3C)");
        a.re2z_flag222 = 0;
        a.re2z_flags21a = 2;     /* @0x80105168: liegend -> 0x60501, verlaesst Zustand 2 */
        chk(re15_re2z_poses_loco_bank(&a), 0, "HURT + 0x21A&2 (liegend) -> Paar2 (@0x8010517C)");
    }
    {   re15_actor_t a = mk(2, 2, 1);   /* Zeile 2 = SIG P228 -> MAIN */
        chk(re15_re2z_poses_loco_bank(&a), 1, "HURT Zeile2 (MAIN) -> Paar1"); }
    {   re15_actor_t a = mk(2, 7, 1);   /* Zeile 7 Spalte 1 = 0x801066FC RAGDOLL */
        chk(re15_re2z_poses_loco_bank(&a), 0, "HURT Zeile7 (RAGDOLL) -> Paar2 (@0x801069D4)"); }
    {   re15_actor_t a = mk(2, 10, 1);  /* Zeile 10 Spalte 1 = 0x80105BC0 STAGGER */
        chk(re15_re2z_poses_loco_bank(&a), 0, "HURT Zeile10 (STAGGER) -> Paar2 (@0x80105F3C)"); }
    {   re15_actor_t a = mk(2, 12, 1);  /* Zeile 12 Spalte 1 = 0x8010703C SLIDE */
        chk(re15_re2z_poses_loco_bank(&a), 1,
            "HURT Zeile12 (SLIDE) -> Paar1 (@0x801071E4/9C/@0x80107364)"); }
    {   re15_actor_t a = mk(2, 15, 1);  /* Zeile 15 = 0x80107EF0 LIGHT */
        chk(re15_re2z_poses_loco_bank(&a), 0, "HURT Zeile15 (LIGHT) -> Paar2 (@0x80108190)"); }
    {   re15_actor_t a = mk(2, 8, 0);   /* Zeile 8 Spalte 0 = 0x80107438 KNOCKDOWN-Treffer */
        a.re2z_hits1d2 = 0;
        chk(re15_re2z_poses_loco_bank(&a), 0,
            "HURT Zeile8 Spalte0 (KNOCKDOWN) -> Paar2 (@0x80107754)"); }

    /* ---- Zustand 3: die DEATH-Tabelle @0x8010CC24, phasengenau ------------------------------ */
    {   re15_actor_t a = mk(3, 5, 4);   /* Zeile 5 Spalte 1 = 0x801092C4 MAGNUM */
        chk(re15_re2z_poses_loco_bank(&a), 1, "DEATH MAGNUM P4 -> Paar1 (@0x801095D0)");
        a.sub_state_2 = 3;
        chk(re15_re2z_poses_loco_bank(&a), 0, "DEATH MAGNUM P3 -> Paar2 (@0x80109500)"); }
    {   re15_actor_t a = mk(3, 8, 6);   /* Zeile 8 Spalte 1 = 0x80108BEC RIP */
        chk(re15_re2z_poses_loco_bank(&a), 1, "DEATH RIP P6 -> Paar1 (@0x80109288)");
        a.sub_state_2 = 5;
        chk(re15_re2z_poses_loco_bank(&a), 0, "DEATH RIP P5 -> Paar2 (@0x801091B8)"); }
    {   re15_actor_t a = mk(3, 3, 1);   /* Zeile 3 Spalte 1 = 0x80108530 der normale Sturz-Tod */
        chk(re15_re2z_poses_loco_bank(&a), 0, "DEATH MAIN -> Paar2 (@0x801088E4)"); }

    /* ---- Der KRIECHER faehrt NIE Paar 1: eigene Tabellen @0x8010C90C/@0x8010C918, deren
     * Handler durchweg das Dispatcher-Paar bekommen (crawl_HURT-Advance @0x80107A38). --------- */
    {   re15_actor_t a = mk(1, 1, 1); a.re2z_f10e = 1;
        chk(re15_re2z_poses_loco_bank(&a), 0, "KRIECHER Zustand1 Sub1 -> Paar2 (@0x8010C918)");
        a = mk(2, 3, 2); a.re2z_f10e = 1;
        chk(re15_re2z_poses_loco_bank(&a), 0, "KRIECHER HURT -> Paar2 (@0x80107A38)"); }

    /* ---- Fremde Typen fasst die Karte nicht an. -------------------------------------------- */
    {   re15_actor_t a = mk(1, 1, 1); a.type = 0x20;    /* Cerberus */
        chk(re15_re2z_poses_loco_bank(&a), 0, "Fremdtyp 0x20 -> 0");
        a.type = 0x40;                                   /* NPC */
        chk(re15_re2z_poses_loco_bank(&a), 0, "Fremdtyp 0x40 -> 0"); }

    /* ---- Und die Typen, die die RE2-Zombie-Wurzel wirklich besitzt, MUESSEN alle die Karte
     * fahren (sonst greift der Fix genau bei dem Typ nicht, den der Nutzer sieht). ------------ */
    for (unsigned t = 0x10; t <= 0x18; t++) {
        if (!re15_re2z_owns_type(t)) continue;
        re15_actor_t a = mk(2, 3, 2); a.type = (uint8_t)t;
        char nm[80]; snprintf(nm, sizeof nm, "Typ 0x%02X HURT/MAIN -> Paar1", t);
        chk(re15_re2z_poses_loco_bank(&a), 1, nm);
    }

    printf(s_fail ? "\nFAIL: %d Abweichungen\n" : "\nPASS: Karte vollstaendig\n", s_fail);
    return s_fail ? 1 : 0;
}
