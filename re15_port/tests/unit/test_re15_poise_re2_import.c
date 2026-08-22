/* test_re15_poise_re2_import.c — PIN fuer die RE2-STURZREGEL im RE1.5-Modus (Port-Option).
 *
 * NUTZER-ENTSCHEIDUNG 2026-08-22 (woertlich): "Ja, naja, der fette Zombie muss nicht jeden
 * 2. Schuss umfallen. Das ist unschoen."  Die Abweichung wird NICHT geraten, sondern aus RE2
 * uebernommen und haengt am SELBEN Schalter wie der RE2-Schadens-/HP-Import
 * (re15_re15_re2z_import / RE15_RE15_RE2Z_IMPORT).
 *
 * ---- die zwei Original-Regeln (selbst disassembliert) --------------------------------------
 * RE1.5 POISE +0x1DC (STAGE1.BIN roh @0x80100000):
 *   Seed  : 80100824 jal 0x8001af20 / 8010082c andi 0x3 / 80100838 addiu 4 / 8010083c sh 476(v1)
 *   Schritt: Tabelle @0x8011FE30, Pistole = Waffe 3 -> 0x80105A6C `addiu v0,v0,-3`,
 *            Aufruf @0x80105D28 (Phase-0-Block des Staggers) = einmal pro Treffer
 *   Tor    : 80105b24 lh v0,476(v0) / 80105b2c bgez -> return; sonst 80105b48 sb 1,4 /
 *            80105b58 sb 0x11,5 / 80105b68 sb 0,6  = KNOCKDOWN
 * RE2 RESISTENZ +0x223 (EMOVL10_S0.BIN roh @0x80100000):
 *   Seed/Neuladung : 80105618 andi v0,v0,0xf / 8010561c addiu v0,v0,16 / 80105620 sb v0,547(s1)
 *                    (identisch INIT @0x8010089C und Sturz-Tor @0x801050C0-C8)
 *   Kosten         : 801055d8 lbu v1,-13261(at) = 0x8010CC33 + Zeile / 801055e0 subu a0,a0,v1
 *                    Tabelle 0x8010CC33 = {_,9,15,15,35,0,...,0,20,0,...}; Pistole = Zeile 3 = 15
 *   HP-TOR         : 801055f4 bgtz (Leiste > 0 -> nichts) / 801055fc lh v0,342(s1) (HP) /
 *                    80105604 slti v0,v0,81 / 80105608 bne -> HP < 81: NICHT nachladen
 *
 * ---- was hier gepinnt wird -------------------------------------------------------------------
 *  (1) die beiden Bruecken-Funktionen Wert fuer Wert gegen die Tabelle/Konstanten oben
 *  (2) Schalter AUS  = byte-true RE1.5: Seed 4..7, Schritt -3, Sturz beim 2./3. Pistolentreffer
 *  (3) Schalter AN   = RE2: Seed 16..31, Schritt -15, KEIN Sturz solange HP >= 81
 *  (4) NICHT-Zombies bleiben in BEIDEN Stellungen unberuehrt (der Hund 0x20 benutzt +0x1DC als
 *      Blockade-Zaehler — ein Uebergriff waere dort sofort ein Verhaltensfehler)
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_ai_flavor.h"

static int g_fail = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: "); printf(__VA_ARGS__); printf("\n"); g_fail = 1; } } while (0)

/* ---- (1) die Bruecken-Funktionen ------------------------------------------------------------ */
static void t_bridge(void)
{
    /* Neuladung: `andi 0xf` + `addiu 16` @0x80105618-1C -> genau 16..31, und beide Raender
     * muessen ueber viele Wuerfe vorkommen (sonst waere die Maske falsch). */
    int lo = 0, hi = 0, ok = 1;
    for (int i = 0; i < 4096; i++) {
        int v = re15_re15_re2z_poise_reload();
        if (v < 16 || v > 31) ok = 0;
        if (v == 16) lo = 1;
        if (v == 31) hi = 1;
    }
    CHECK(ok,      "Neuladung ausserhalb 16..31 (`andi 0xf`/`addiu 16` @0x80105618-1C)");
    CHECK(lo && hi,"Neuladung erreicht 16 und 31 nicht (Maske 0xf @0x80105618)");

    /* Kosten aus 0x8010CC33 ueber die belegte Waffe->Zeile-Zuordnung (Pistole 3 -> RE2-Zeile 3). */
    CHECK(re15_re15_re2z_poise_step(20, 3, 200) == 5,
          "Pistole: 20 - 15 muss 5 sein (cost[3] = 0x0F @0x8010CC36), ist %d",
          re15_re15_re2z_poise_step(20, 3, 200));
    /* Messer (RE1.5 w1) -> RE2-Zeile 1 -> cost 9 (@0x8010CC34). */
    CHECK(re15_re15_re2z_poise_step(20, 1, 200) == 11,
          "Messer: 20 - 9 muss 11 sein (cost[1] = 0x09 @0x8010CC34), ist %d",
          re15_re15_re2z_poise_step(20, 1, 200));
    /* Schrot (RE1.5 w8) -> RE2-Zeile 7 -> cost 0 (@0x8010CC3A): die Leiste bleibt stehen. */
    CHECK(re15_re15_re2z_poise_step(20, 8, 200) == 20,
          "Schrot: cost[7] ist 0 (@0x8010CC3A), Leiste muss 20 bleiben, ist %d",
          re15_re15_re2z_poise_step(20, 8, 200));

    /* DAS HP-TOR: erschoepfte Leiste + HP >= 81 -> sofort nachladen (kein Sturz moeglich). */
    for (int i = 0; i < 64; i++) {
        int v = re15_re15_re2z_poise_step(10, 3, 81);
        CHECK(v >= 16 && v <= 31,
              "HP == 81 (`slti v0,v0,81` @0x80105604 nimmt den Zweig NICHT) muss nachladen, ist %d", v);
    }
    /* HP < 81 -> KEIN Nachladen, die Leiste geht negativ = der Sturz. */
    CHECK(re15_re15_re2z_poise_step(10, 3, 80) == -5,
          "HP == 80 (< 81) darf NICHT nachladen: 10 - 15 = -5, ist %d",
          re15_re15_re2z_poise_step(10, 3, 80));
    /* Leiste noch > 0 -> `bgtz` @0x801055F4 ueberspringt das Nachladen auch bei hoher HP. */
    CHECK(re15_re15_re2z_poise_step(31, 3, 200) == 16,
          "Leiste > 0 darf nicht nachladen (`bgtz` @0x801055F4), 31-15 muss 16 sein, ist %d",
          re15_re15_re2z_poise_step(31, 3, 200));
}

/* ---- (2)/(3) der INIT-Seed am ECHTEN Aktor --------------------------------------------------- */
static int init_seed(int type, int import_on)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(import_on);
    memset(g_actors, 0, sizeof g_actors);
    re15_damage_seed_rng(0x1234u);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 8000; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = (uint8_t)type; e->hp = 100;
    re15_enemy_ai_live_init(1);
    return (int)e->hit_stun;
}

static void t_init_seed(void)
{
    for (int i = 0; i < 16; i++) {
        int off = init_seed(0x10, 0);
        CHECK(off >= 4 && off <= 7,
              "Schalter AUS: der RE1.5-Seed muss (rng&3)+4 = 4..7 bleiben (@0x8010082C-3C), ist %d", off);
    }
    for (int i = 0; i < 16; i++) {
        int on = init_seed(0x10, 1);
        CHECK(on >= 16 && on <= 31,
              "Schalter AN: der Seed muss die RE2-Leiste 16..31 sein (@0x8010089C), ist %d", on);
    }
    /* NICHT-ZOMBIE: der Hund fuehrt +0x1DC als Blockade-Zaehler — die Option darf ihn in KEINER
     * Stellung anfassen. re15_enemy_ai_live_init ist der Zombie-INIT; hier reicht die
     * Besitz-Frage, sie ist das einzige Tor vor jedem der vier Schreiber. */
    re15_re15_re2z_import_set(1);
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK(re15_re15_re2z_import_owns(0x20) == 0, "Hund 0x20 darf die Sturzregel NICHT erben");
    CHECK(re15_re15_re2z_import_owns(0x21) == 0, "Kraehe 0x21 darf die Sturzregel NICHT erben");
    CHECK(re15_re15_re2z_import_owns(0x25) == 0, "Adult-Spinne 0x25 darf die Sturzregel NICHT erben");
    CHECK(re15_re15_re2z_import_owns(0x10) == 1, "Zombie 0x10 MUSS die Sturzregel tragen");
    /* Und im RE2-Flavor greift die RE1.5-Bruecke ueberhaupt nicht (dort laeuft das RE2-Gehirn
     * mit seiner eigenen +0x223-Leiste). */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK(re15_re15_re2z_import_owns(0x10) == 0, "Im RE2-Flavor darf die RE1.5-Bruecke nicht greifen");
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
}

/* ---- (2)/(3) der SCHRITT pro Treffer am ECHTEN Trefferpfad ----------------------------------- */
/* Ein Pistolentreffer ueber re15_enemy_take_damage-Zwilling: +0x5 = Waffe 3 wie im Gun-Applier
 * (`sb weapon,+0x5` @0x800124BC), +0x4 = 2 (HURT), +0x7 = 0 -> der naechste AI-Tick laeuft in den
 * Stagger-Phase-0-Block, in dem der Poise-Schritt sitzt (@0x80105D28). */
static int hit_once(int import_on, int hp, int stun_before)
{
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    re15_re15_re2z_import_set(import_on);
    memset(g_actors, 0, sizeof g_actors);
    re15_damage_seed_rng(0x99u);
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 8000; pl->hp = 100;
    re15_actor_t *e = &g_actors[1];
    e->active = 1; e->type = 0x10; e->state = 2; e->sub_state_1 = 3; e->sub_state_2 = 1;
    e->sub_state_3 = 0; e->hp = (int16_t)hp; e->hurt_clip = 3; e->hit_stun = (int16_t)stun_before;
    re15_enemy_ai_run_all(0);
    return (int)e->hit_stun;
}

static void t_step(void)
{
    /* Schalter AUS: -3 (Tabelle @0x8011FE3C -> 0x80105A6C). */
    CHECK(hit_once(0, 100, 6) == 3,
          "Schalter AUS: 6 - 3 muss 3 sein (@0x80105A6C), ist %d", hit_once(0, 100, 6));
    CHECK(hit_once(0, 100, 4) == 1,
          "Schalter AUS: 4 - 3 muss 1 sein, ist %d", hit_once(0, 100, 4));
    /* Schalter AN, HP >= 81: -15, und bei Erschoepfung sofortiges Nachladen -> nie negativ. */
    CHECK(hit_once(1, 100, 31) == 16,
          "Schalter AN: 31 - 15 muss 16 sein (cost[3] @0x8010CC36), ist %d", hit_once(1, 100, 31));
    for (int i = 0; i < 32; i++) {
        int v = hit_once(1, 100, 10);
        CHECK(v >= 16 && v <= 31,
              "Schalter AN + HP >= 81: erschoepfte Leiste muss nachladen (@0x80105610-20), ist %d", v);
    }
    /* Schalter AN, HP < 81: KEIN Nachladen -> negativ -> das RE1.5-Tor `bgez` @0x80105B2C
     * schaltet in den Knockdown. Genau so soll der Zombie im ENDSPIEL noch fallen. */
    CHECK(hit_once(1, 80, 10) == -5,
          "Schalter AN + HP < 81: 10 - 15 = -5 (kein Nachladen, `slti 81` @0x80105604), ist %d",
          hit_once(1, 80, 10));
}

int main(void)
{
    printf("== RE2-STURZREGEL im RE1.5-Modus (Port-Option, Nutzer-Entscheidung 2026-08-22) ==\n");
    t_bridge();
    t_init_seed();
    t_step();
    /* Auslieferungszustand des Schalters wiederherstellen. */
    re15_re15_re2z_import_set(1);
    printf(g_fail ? "FAILED\n" : "OK\n");
    return g_fail ? 1 : 0;
}
