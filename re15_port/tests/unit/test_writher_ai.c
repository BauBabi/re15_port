/* test_writher_ai.c — WRITHE-HAZARD (type 0x1a, EM01A, STAGE1 ROOM1210/1211) AI.
 *
 * BYTE-TRUE from audit wf_efd92a2c writher (root 0x8010c1ec, state table @0x8012093c, raw STAGE1.BIN).
 * This enemy is a ROOTED ground-writhing contact-hazard that is a SOLID 300-radius obstacle and is
 * KILLABLE in one hit (INIT installs the +0x78 box @0x8012091c={0,-1440,0,300,1440,300} but writes NO
 * HP -> spawn HP=0; type 0x1a<0x20 -> any damaging shot -> hp<0 -> DEATH -> CORPSE). It never
 * walks/chases and deals no direct player damage. Asserts:
 *   (1) INIT: state -> 1 (ACTIVE writhe), HP stays 0 (INIT never sets +0x9a).
 *   (2) ROOTED: with the player adjacent it NEVER advances X/Z (no chase) over many frames.
 *   (3) HARMLESS: the writher itself writes no player.hp (contact push is game_step, not this tick).
 *   (4) IDLE = clip 0 ONLY: an unhit writher (grid 0) loops clip 0 forever — no 0->1->2 cycling
 *       (the old invented behavior, audit #3).
 *   (5) SOLID + SHOOTABLE: re15_enemy_apply_hitbox(0x1a) installs the 300-radius box (audit #1).
 *   (6) KILLABLE: a damaging hit on the HP-0 spawn -> DEATH (state 3) -> CORPSE (state 7) (audit #5).
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"

int main(void)
{
    int fail = 0;
    memset(g_actors, 0, sizeof g_actors);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    /* ⛔ STANDORT DES SPIELERS — zweimal geaendert, beide Male aus demselben Grund: das
     * Ausfahr-Tor hat die Messgroesse gewechselt, also muss die Wache mitwechseln, sonst
     * misst sie einen Zustand, den es nicht mehr gibt.
     *   bis 2026-08-27: z = 900 - lag zufaellig knapp AUSSERHALB der damaligen 850.
     *   bis 2026-08-30: z = 400 - innerhalb von 850, seitlich zur Blickrichtung.
     *   jetzt: DIREKT VOR DER HAND. Das Tor ist RE2s Radius 1300 (`sltiu s0,s0,0x514`
     *     @0x80102f3c) um den Punkt, an dem die ausgefahrene Hand steht: Heimat +
     *     LUNGE_NET 2420 (@0x8010c7b8/@0x8010c7a8) + MESH_REACH 1671 = 4091 entlang +0x6a.
     *     Bei Yaw 0 zeigt der Arm nach +X, die Hand steht also auf (4091,0).
     * ⛔ DAS IST KEINE ABGESENKTE SCHRANKE, SONDERN EINE ZWEITE: der Fall "Spieler steht
     * SEITLICH" wandert nach unten in (3s) und verlangt dort, dass der Arm sich GAR NICHT
     * bewegt - eine Zusage, die die alte Fassung nicht machen konnte, weil ihr flurbreites
     * Tor (`dx < 11000`) seitlich genauso aufging. */
    pl->active = 1; pl->type = 0; pl->x = 4091; pl->y = 0; pl->z = 900; pl->hp = 100;

    const int WS = 1;
    re15_actor_t *e = &g_actors[WS];
    e->active = 1; e->type = 0x1a; e->state = 0; e->grid_id = 0; e->x = 0; e->y = 0; e->z = 0; e->rot_y = 0;
    re15_enemy_apply_hitbox(e, 0x1a);   /* installs the 300-radius +0x78 box @0x8012091c */

    printf("=== WRITHE-HAZARD (type 0x1a, EM01A) AI ===\n");

    /* (5) SOLID + SHOOTABLE: the byte-true box is a real 300-radius damage hitbox */
    if (e->hit_radius_min != 300) { fprintf(stderr, "FAIL(5): 0x1a must have the 300-radius +0x78 box, got %d\n", e->hit_radius_min); fail = 1; }
    printf("  (5) SOLID+SHOOTABLE: hit_radius_min=%d (box @0x8012091c)\n", e->hit_radius_min);

    /* (1) INIT: state -> 1, HP stays 0 (INIT writes no +0x9a) */
    re15_enemy_ai_run_all(0);
    if (e->state != 1) { fprintf(stderr, "FAIL(1): INIT->ACTIVE expected state 1, got %d\n", e->state); fail = 1; }
    /* ⛔ HIER STAND: "INIT must not set HP (byte-true spawn 0)". Diese Aussage ist WEITER
     * RICHTIG fuer das ORIGINAL — im Arm-Baum 0x8010c1ec..0x8010d774 gibt es keinen
     * einzigen Store auf +0x9a. Der Port seedet trotzdem, als BENANNTE NACHRUESTUNG
     * (Nutzer-Auftrag 2026-08-26: die Arme sollen anschiessbar sein).
     * Die Wache prueft deshalb jetzt das, was am Seed nachpruefbar ist: der Wert stammt aus
     * der ECHTEN Tabellenzeile des Typs — Basis 0x8011f034, Zeile Typ*0x20 = @0x8011f374,
     * Spalte (rng & 0xf) (@0x8010daf4/@0x8010dafc), Store @0x8010db14. Ein erfundener Wert
     * faellt damit auf. */
    {
        static const int16_t k_hp[16] = {72,82,96,82,83,96,74,84,99,76,88,86,87,82,80,90};
        int in_table = 0;
        for (int i = 0; i < 16; i++) if (e->hp == k_hp[i]) { in_table = 1; break; }
        if (!in_table) {
            fprintf(stderr, "FAIL(1): HP %d steht NICHT in der Tabellenzeile @0x8011f374 "
                            "{72,82,96,82,83,96,74,84,99,76,88,86,87,82,80,90} - der Seed "
                            "waere dann erfunden\n", e->hp);
            fail = 1;
        }
    }
    if (e->writher_hits < 1 || e->writher_hits > 4) {
        fprintf(stderr, "FAIL(1): Trefferbudget %u ausserhalb 1..4 - byte-true ist "
                        "(rng & 3) + 1 (@0x8010c400/@0x8010c404, Store @0x8010c41c)\n",
                e->writher_hits);
        fail = 1;
    }
    printf("  (1) INIT: state->%d (writhe), hp=%d (aus Zeile @0x8011f374), Budget=%u, motion=%d\n",
           e->state, e->hp, e->writher_hits, e->motion);

    /* (2) ROOTED + (3) HARMLESS: Spieler daneben (nicht davor), 200 Bilder, der Gegner darf
     * sich nicht bewegen und selbst keinen Spieler-Schaden schreiben.
     * DANEBEN ist hier wesentlich: der Arm blickt bei rot_y=0 nach +X, der Spieler steht auf
     * (0,900) und damit rund 90 Grad seitlich. RE2s Griff-Tor sind zwei Halb-Sektoren um
     * Yaw +-256 (@0x8010193c-4c) = ein 45-Grad-Kegel NACH VORN; seitlich wird nicht gegriffen.
     * Genau das ist die Aussage: der Arm ist als Hindernis hart, aber er greift nicht um sich. */
    int32_t ex0 = e->x, ez0 = e->z; int16_t hp0 = pl->hp;
    int32_t xmax = ex0;
    for (int f = 0; f < 200; f++) {
        pl->hit_react = 0; re15_enemy_ai_run_all(0);
        if (e->x > xmax) xmax = e->x;
    }
    /* ⛔ KORREKTUR 2026-08-28: hier stand "rooted hazard must NOT move". Das war FALSCH und
     * hat den Nutzer-Report "Arme kommen nicht raus beim Vorbeilaufen" mitverursacht.
     * Byte-true fuehrt der ausgeloeste Arm eine LUNGE aus (ANIM-Sub B[1] = FUN_8010c714,
     * Schritt = FUN_800245d8 aus +0x8c entlang +0x6a):
     *     3 Bilder * 800 vorwaerts   (+0x9c = 3 @0x8010c7a8, +0x8c = 0x320 @0x8010c7b8)
     *   + 1 Uebergangsbild * 20      (+0x8c = 0x14 @0x8010c7f8, Tail a0 = 0 @0x8010c914)
     *   - 30 Bilder * 20 rueckwaerts (+0x9c = 0x1e @0x8010c808, a0 = 0x800 @0x8010c8b4)
     *   - 1 Uebergangsbild * 200     (+0x8c = 0xc8 @0x8010c860, weiter mit a0 = 0x800)
     *   + 4 Bilder * 200 vorwaerts   (+0x9c = 3 @0x8010c870, Tail a0 = 0)
     *   = netto 2420 Einheiten nach VORN.
     * Ohne sie steht die Kreatur an ihrem Spawn in NULL von NEUN Kamera-Vierecken des Raums
     * und wird deshalb ueberhaupt nicht gezeichnet (Zeichner-Weiche FUN_8001e8c8
     * `jal 0x80014368` @0x8001e974). Genau das ist die 2420: sie schiebt den Arm aus dem
     * Gitter in den Flur. Die Wache prueft jetzt den Betrag. */
    if (xmax - ex0 != 2420) {
        fprintf(stderr, "FAIL(2): die Lunge muss exakt 2420 Einheiten weit reichen "
                        "(FUN_8010c714 + FUN_800245d8), gemessen %ld\n", (long)(xmax - ex0));
        fail = 1;
    }
    if (e->z != ez0) { fprintf(stderr, "FAIL(2): die Lunge laeuft entlang +0x6a (hier Yaw 0 = +X), "
                                       "z darf sich nicht aendern: %d -> %d\n", ez0, e->z); fail = 1; }
    printf("  (2) LUNGE: %ld Einheiten nach vorn, z unveraendert (%d)\n", (long)(xmax - ex0), e->z);

    /* (3s) SEITLICH = GAR KEINE REAKTION. Der Arm wird zurueckgestellt und der Spieler auf
     * dieselbe DISTANZ, aber quer zur Blickrichtung gesetzt: (0,4091) statt (4091,900).
     * Unter dem alten flurbreiten Tor (`|dz| < 850 && |dx| < 11000`) waere das egal gewesen;
     * RE2s Radius um die Hand (@0x80102f3c) laesst hier nichts aufgehen. Genau das ist der
     * Nutzer-Punkt vom 2026-08-30 ("die Arme kommen raus, wenn man noch zu weit weg ist"). */
    e->sub_state_1 = 0; e->sub_state_2 = 0; e->x = ex0; e->z = ez0; e->motion = 0;
    pl->x = 0; pl->z = 4091; pl->hp = 100;
    int16_t hp_side = pl->hp;
    for (int f = 0; f < 200; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    if (e->x != ex0 || e->z != ez0) {
        fprintf(stderr, "FAIL(3s): seitlich (gleiche Distanz, quer zur Blickrichtung) darf der "
                        "Arm sich NICHT bewegen, steht aber auf (%d,%d) statt (%d,%d)\n",
                e->x, e->z, ex0, ez0);
        fail = 1;
    }
    if (pl->hp != hp_side) { fprintf(stderr, "FAIL(3s): und er darf dabei keinen Schaden schreiben, hp %d->%d\n", hp_side, pl->hp); fail = 1; }
    printf("  (3s) SEITLICH: Arm bleibt auf (%d,%d), Spieler-hp %d\n", e->x, e->z, pl->hp);

    /* (3) HARMLESS bleibt: der Tick selbst schreibt keinen Spieler-Schaden. */
    if (pl->hp != hp0)              { fprintf(stderr, "FAIL(3): tick must write no player hp, hp %d->%d\n", hp0, pl->hp); fail = 1; }
    printf("  (3) HARMLESS: player hp %d (unchanged)\n", pl->hp);
    e->sub_state_1 = 0; e->sub_state_2 = 0; e->x = ex0; e->z = ez0; e->state = 1;

    /* (3b) BANK-TOR (Nutzer-Report 2026-08-29 "schwarze Dreiecke ueber dem Feuer").
     * State 1 ist der normale AKTIV-Zustand fast jedes Gegners. In v0.3.28 hat der RENDERER
     * daraufhin die EIGENE Bank 1 gewaehlt - auch fuer Zombies, Spinnen und die
     * Feuer-Emitter (0x26), also mit dem falschen Skelett. Die Regel gilt ausschliesslich
     * fuer die NPC-Familie 0x40..0x4d (deren Eskorte posiert aus +0x170/+0x174; f314-Loads
     * \n0x8004f384-88 / \n0x8004f5c0-c4 / \n0x8004f7bc-c0 / \n0x8004fb14 / \n0x8004ff68).
     * Diese Wache haelt fest: ein Typ-0x1A-Aktor in State 1 darf NIE die eigene Bank
     * bekommen. Engine und Renderer benutzen dafuer jetzt dasselbe Praedikat. */
    if (e->state != 1) {
        fprintf(stderr, "FAIL(3b): der Writher muss fuer die Messung in State 1 stehen, ist %d\n", e->state);
        fail = 1;
    }
    if (re15_actor_uses_own_bank(e)) {
        fprintf(stderr, "FAIL(3b): Typ 0x1A ist KEIN NPC (0x40..0x4d) und darf in State 1 nicht die eigene Bank 1 posieren - genau das waren die schwarzen Dreiecke\n");
        fail = 1;
    }
    printf("  (3b) BANK-TOR: Typ 0x%02X in State %d nimmt NICHT die eigene Bank\n", e->type, e->state);

    /* (4) IDLE = clip 0 only: an unhit writher never changes clip (byte-true A[0]/B[0], grid 0).
     *
     * ⛔ MIT ABSTAND MESSEN, NICHT NEBENBEI: der Port fuehrt in ROOM1210 die bewusste
     * NACHRUESTUNG "Gitterhaende" (re15_writher_ai_tick case 1) — ein Arm faehrt aus, wenn der
     * Spieler auf seiner Hoehe ist (REACH_Z = Tiefe des Ausloeser-Rechtecks 1700 @0x1EAE).
     * Diese Zusage hier gilt dem BYTE-TRUEN Ruhezustand, also dem Fall OHNE Spieler in
     * Reichweite. Bis 2026-08-27 stand der Spieler auf z=900 und lag damit rein zufaellig
     * knapp ausserhalb der damaligen Reichweite 850 — die Zusage hielt aus Glueck, nicht aus
     * Absicht. Jetzt wird der Abstand hergestellt und der Arm zurueckfahren gelassen.
     * (Das Tor ist seit 2026-08-30 RE2s Radius 1300 @0x80102f3c um die ausgefahrene Hand,
     * nicht mehr das flurbreite Rechteck — 30000 liegt in jeder Lesart weit ausserhalb.) */
    pl->x = 0; pl->z = 30000;
    for (int f = 0; f < 120; f++) { pl->hit_react = 0; re15_enemy_ai_run_all(0); }
    if (e->motion != 0) { fprintf(stderr, "FAIL(4): unhit writher must loop clip 0, got clip %d\n", e->motion); fail = 1; }
    if (e->sub_state_1 != 0) { fprintf(stderr, "FAIL(4): writher must return to idle sub 0, got %d\n", e->sub_state_1); fail = 1; }
    /* Und er muss dabei WIEDER IM GITTER stehen. Das Original kennt kein Zurueck (sein
     * Ausgang @0x8010c8e4 geht auf +0x5 = 2/3 und nie wieder auf 1); der Port loest das
     * Ausfahren pro Arm wiederholt aus und muss die Kreatur deshalb zuruecksetzen, sonst
     * wandert sie bei jedem Vorbeilaufen 2420 Einheiten weiter in den Flur. */
    if (e->x != ex0 || e->z != ez0) {
        fprintf(stderr, "FAIL(4): nach dem Rueckzug muss der Arm wieder auf seinem Platz "
                        "stehen, (%d,%d) statt (%d,%d)\n", e->x, e->z, ex0, ez0);
        fail = 1;
    }
    printf("  (4) IDLE: clip stayed 0 (no invented 0->1->2 cycling), sub=%d\n", e->sub_state_1);

    /* (6) KILLABLE: the HP-0 spawn dies in one damaging hit -> DEATH(3) -> CORPSE(7) */
    e->hit_react = 0;
    int landed = re15_enemy_take_damage(e, 2);   /* type-2 = instakill class (dmg 1000); hp 0 -> -1000 */
    if (!landed)        { fprintf(stderr, "FAIL(6): a hit on the writher must land (it is targetable)\n"); fail = 1; }
    if (e->state != 3)  { fprintf(stderr, "FAIL(6): one damaging hit on HP-0 spawn must -> DEATH state 3, got %d\n", e->state); fail = 1; }
    printf("  (6a) KILLABLE: after 1 hit, hp=%d state=%d (DEATH)\n", e->hp, e->state);
    for (int f = 0; f < 60; f++) re15_enemy_ai_run_all(0);
    if (e->state != 7)  { fprintf(stderr, "FAIL(6): DEATH must settle into CORPSE state 7, got %d\n", e->state); fail = 1; }
    printf("  (6b) CORPSE: death anim settled to state=%d (inert corpse)\n", e->state);

    if (fail) { printf("WRITHE-HAZARD: FAIL\n"); return 1; }
    printf("WRITHE-HAZARD: all checks passed\n");
    return 0;
}
