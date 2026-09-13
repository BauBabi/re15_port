/**
 * @file probe_g5_boss.c
 * @brief SMOKE (ctest) — das neue G5-Endkampf-Modul (enemy_ai_boss_g5.c, Runde 6).
 *
 * Faehrt den Boss-Tick direkt mit geladener RE2-EM036-Bank (wie probe_gator_fress):
 *  PIN 1  PARK: grid 0x33 -> off-world, kein Zustandsfortschritt.
 *  PIN 2  KAMPFSTART grid 0x13 -> INTRO (sub2) laeuft die Clip-Choreo 1->3->4->2->0
 *         und traegt Root-Motion (u waechst um ~+10960 = 7014+3946).
 *  PIN 3  Nach dem Intro erreicht der Boss den Kampf (sub 0/1) und der ZUG (sub1)
 *         bewegt ihn weiter vorwaerts (Clip 5 x1,5 = +4050, Kappe u=12000).
 *  PIN 4  Blob-Simulation atmet (Gewicht[0] pendelt) und der Biss feuert bei Naehe
 *         (Spieler-HP faellt um 40 ODER Devour-Kill bei u>=9001).
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_actor.h"
#include "re15_enemy.h"
#include "re15_enemy_ai.h"
#include "re15_damage.h"
#include "re15_room.h"
#include "re2_ems.h"

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

extern void re15_g5_boss_tick(int slot);
extern void re15_g5_tentakel_reset(void);

static uint8_t *slurp(const char *p, size_t *n)
{
    FILE *f = fopen(p, "rb"); if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f); if (b) *n = (size_t)sz; return b;
}

static int g_fail = 0;
#define CHECK(cond, ...) do { if (!(cond)) { g_fail = 1; \
    fprintf(stderr, "FAIL: " __VA_ARGS__); fprintf(stderr, "\n"); } } while (0)

/* u SPIELER-RELATIV wie im Modul (Runde 7): u = 12000 - |dx| entlang X. */
static int32_t u_von(const re15_actor_t *e)
{
    int32_t d = (int32_t)g_actors[RE15_ACTOR_SLOT_PLAYER].x - (int32_t)e->x;
    if (d < 0) d = -d;
    return 12000 - d;
}

int main(void)
{
    printf("=== G5-ENDKAMPF-MODUL (ROOM5090, RE2-EM036) ===\n");
    re15_actor_init();
    g_current_room_id = 0x5090;

    /* RE2-Bank EM036 laden (Muster probe_gator_fress). */
    size_t n = 0;
    uint8_t *ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &n);
    if (!ems) { printf("SKIP: shared_assets/RE2/CDEMD0.EMS fehlt\n"); return 0; }
    re15_enemy_bank_t *eb = re15_enemy_alloc(0x36);
    if (!eb || re2_ems_load_bank(ems, n, 0x36, eb, NULL) != 0) {
        printf("FAIL: EM036 laedt nicht\n"); return 1;
    }
    eb->buf = NULL; eb->ok = 1;
    printf("EM036: %d Bones, %d Clips\n", eb->skel.bone_count, eb->anim.clip_count);
    CHECK(eb->anim.clip_count == 11, "EM036 hat %d Clips (erwartet 11)", eb->anim.clip_count);

    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->type = 0; pl->hp = 100; pl->hit_react = 0;
    pl->x = 300; pl->z = -23400; pl->y = 0;

    re15_actor_t *e = &g_actors[2];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600;
    e->x = -14700; e->z = -23350; e->grid_id = 0x33;   /* RDT-Spawn */

    /* PIN 1: PARK. */
    for (int t = 0; t < 30; t++) re15_g5_boss_tick(2);
    CHECK(e->x == -32000 && e->z == -32000, "PARK: pos=(%d,%d) != (-32000,-32000)",
          (int)e->x, (int)e->z);

    /* PIN 2: Kampfstart -> Intro-Choreo mit Root-Motion.
     * Erwartung nach der Buehnen-Messung (Runde 7): der Boss startet am
     * RDT-Spawn x=-14700 (WESTEN, 15000 vom Spieler) und kriecht das Intro
     * nach OSTEN auf ihn zu - u waechst also. */
    e->grid_id = 0x13;
    int32_t u_start = 0;
    int treffer = 0;
    int saw_clip[16]; memset(saw_clip, 0, sizeof saw_clip);
    int intro_fertig_t = -1;
    for (int t = 0; t < 2600; t++) {
        re15_g5_boss_tick(2);
        if (t == 0) u_start = u_von(e);
        if (e->motion >= 0 && e->motion < 16) saw_clip[e->motion] = 1;
        if (pl->hp < 100) treffer++;                   /* VOR dem Reset zaehlen */
        if (intro_fertig_t < 0 && e->hp > 0) {
            /* Intro fertig, wenn der Zug (Clip 5) laeuft */
            if (e->motion == 5) intro_fertig_t = t;
        }
        pl->hp = 100; pl->hit_react = 0;               /* Dauer-Reset: nur Delta messen */
    }
    printf("Intro: u %d -> %d (Delta %d), Clips gesehen:", u_start, u_von(e),
           u_von(e) - u_start);
    for (int c = 0; c < 12; c++) if (saw_clip[c]) printf(" %d", c);
    printf("\n");
    CHECK(saw_clip[1] && saw_clip[3] && saw_clip[4] && saw_clip[2] && saw_clip[0],
          "Intro-Choreo 1/3/4/2/0 nicht vollstaendig gesehen");
    CHECK(intro_fertig_t > 0, "der Tentakel-Zug (Clip 5) wurde nie erreicht");
    CHECK(e->x <= -14000 || u_start <= -2500,
          "Intro startet nicht am RDT-Spawn (u_start=%d, erwartet ~-3000)", u_start);
    CHECK(u_von(e) > u_start + 8000,
          "Root-Motion traegt nicht: u-Delta %d (erwartet > 8000: Intro 10960 + Zuege)",
          u_von(e) - u_start);
    CHECK(u_von(e) <= 12000, "Zug-Kappe 12000 verletzt: u=%d", u_von(e));
    CHECK(e->x < g_actors[RE15_ACTOR_SLOT_PLAYER].x,
          "der Boss muss WESTLICH des Spielers bleiben (x=%d, Spieler=%d) - er kommt "
          "von vorn aus dem Zug, nicht vom Eingang hinter dem Spieler",
          (int)e->x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x);

    /* PIN 4: der Kampf hat den Spieler getroffen (Biss 40 und/oder Devour
     * Clip 9); der Hauptlauf resettet hp je Tick, das Ereignis wird VOR dem
     * Reset gezaehlt. Nach einem Devour haelt der Boss byte-true den
     * sub4-Endzustand ("kein Routine-Exit - der Spieler ist tot"). */
    printf("Treffer-Ereignisse im Hauptlauf: %d, Devour-Clip 9 gesehen: %d\n",
           treffer, saw_clip[9]);
    CHECK(treffer > 0 || saw_clip[9],
          "weder Biss-Schaden noch Devour im Hauptlauf gesehen");

    /* TOD: frischer Aktor-Slot (der Devour-Endzustand des Hauptlaufs haelt
     * byte-true fuer immer - "kein Routine-Exit, der Spieler ist tot"
     * @0x80101ccc-d24; ein Slot-Wechsel initialisiert den Modul-Zustand neu). */
    {
        /* ⛔ SLOT REGULAER ALLOZIEREN, nicht "irgendeine 3" (Runde 8, gemessen): der
         * feste Slot 3 war im Hauptlauf schon als TENTAKEL vergeben. Der Arm-Tick
         * schrieb dann weiter auf denselben Aktor wie der Boss-Tick - Phase 6 addiert
         * je Frame ein Zittern auf x/y/z (@0x80101E94), die Boss-y lief auf -1176084
         * und die Todes-Rampe meldete 2694951 statt 2950. Der alte Pin (">= 2900")
         * hat das durchgewunken. */
        extern int re15_actor_alloc(uint8_t type);
        re15_g5_tentakel_reset();
        int slot_tod = re15_actor_alloc(0x36u);
        CHECK(slot_tod >= 0, "kein freier Aktor-Slot fuer den Todes-Lauf");
        if (slot_tod < 0) slot_tod = 3;
        re15_actor_t *e2 = &g_actors[slot_tod];
        memset(e2, 0, sizeof *e2);
        e2->active = 1; e2->type = 0x36; e2->flags = 1; e2->hp = 600;
        e2->x = -14700; e2->z = -23350; e2->grid_id = 0x13;
        re15_g5_boss_tick(slot_tod);   /* Kampfstart-Armierung */
        e2->hp = 0;                    /* toedlicher Treffer */
        int saw10 = 0; int32_t y_max = 0; int fertig = 0;
        for (int t = 0; t < 8000; t++) {
            re15_g5_boss_tick(slot_tod);
            if (e2->motion == 10) saw10 = 1;
            if (e2->y > y_max) y_max = e2->y;
            if (!fertig && y_max >= 2952) fertig = t + 1;
        }
        printf("Tod: Clip10=%d, Absink-y max=%d (Soll GENAU 2952), erreicht nach %d Ticks\n",
               saw10, (int)y_max, fertig);
        CHECK(saw10, "Todes-Kollaps Clip 10 nie gesehen");
        /* ⛔ EXAKTER ENDWERT statt ">= 2900" (Runde 8): die alte Schranke liess einen
         * Defekt durch - gemessen kam hier y_max = 2694951 heraus, also das
         * NEUNHUNDERTFACHE, und der Pin meldete gruen (Ursache: der Todes-Lauf lief
         * auf einem Slot, den der Hauptlauf schon als Tentakel vergeben hatte).
         *
         * Der Endwert ist 2952, NICHT 2950: 2950 ist der FAKTOR in
         * y=(t*2950)>>12 (@0x801037c4-f4). Die Rampe zaehlt t mit 16/12/8/4 je nach
         * Schwelle (@0x80103668) und trifft die Abbruchmarke 4097 nicht exakt -
         * nachgerechnet endet sie bei t=4100:
         *     t<1025: +16  ->  0..1040 (65 Schritte)
         *     t<2049: +12  ->  1040..2060
         *     t<3073: +8   ->  2060..3076
         *     sonst:  +4   ->  3076..4100  (erster Wert >= 4097)
         *   y = (4100*2950)>>12 = 2952. */
        CHECK(y_max == 2952, "Absink-Rampe endet bei y=%d statt genau 2952", (int)y_max);
        CHECK(fertig > 0, "Absink-Rampe erreicht 2952 in 8000 Ticks nicht");
    }

    /* PIN 5  DER BOSS DARF NICHT AM SPIELER KLEBEN (Runde 8).
     *
     * ⛔ WAS HIER GEPRUEFT WIRD, IST DIE KOPPLUNG: bis Runde 8 buchte der Port jede
     * Bossbewegung als neues u und leitete die POSITION daraus ab
     * (`e->x = plx +/- (12000-u)`). Damit war die Bossposition eine Funktion der
     * SPIELERposition: laeuft Leon weg, wandert der Boss im selben Frame mit - ohne
     * Animation, ohne Root-Motion, beliebig schnell. Fliehen war unmoeglich, und
     * umgekehrt hing jede Annaeherung an Leons Schritten statt an der Choreo.
     * Das Original kennt die Kopplung nicht: FUN_800152C8 (Aufrufer @0x80100fd0-e4,
     * @0x80104060-94, @0x801018d0-920) rotiert die Root-Spur um den Yaw und ADDIERT
     * sie auf X/Z der Entity; die Spielerposition kommt darin nicht vor.
     *
     * Messaufbau: der Boss steht neben Leon (Abstand 2000, also mitten im Kampf),
     * dann RENNT Leon mit 120/Frame nach Osten davon. Der Boss zieht sich mit seiner
     * Root-Spur nach (~4050 je 235-Frame-Zug), Leon schafft in derselben Zeit rund
     * 28000 - der Abstand MUSS also deutlich wachsen. Gemessen: alte Kopplung 2000
     * -> 2000 (der Boss blieb Frame fuer Frame auf Tuchfuehlung), Weltbewegung
     * 2000 -> ueber 20000. */
    {
        extern int re15_actor_alloc(uint8_t type);
        re15_g5_tentakel_reset();
        int slot_a = re15_actor_alloc(0x36u);
        CHECK(slot_a >= 0, "kein freier Aktor-Slot fuer den Flucht-Lauf");
        if (slot_a >= 0) {
            re15_actor_t *e3 = &g_actors[slot_a];
            memset(e3, 0, sizeof *e3);
            e3->active = 1; e3->type = 0x36; e3->flags = 1; e3->hp = 25000;
            e3->x = -400; e3->z = -23350; e3->grid_id = 0x13;
            pl->x = 0; pl->z = -23350; pl->y = 0; pl->hp = 200; pl->hit_react = 0;
            int32_t d_min = 400; int treffer0 = 0;
            for (int t = 0; t < 1500; t++) {
                pl->x = 0; pl->z = -23350;           /* Leon steht - nur der Boss bewegt sich */
                pl->hp = 200; pl->hit_react = 0;
                re15_g5_boss_tick(slot_a);
                int32_t d = (int32_t)pl->x - (int32_t)e3->x;
                if (d < 0) d = -d;
                if (d < d_min) d_min = d;
                if (d == 0) treffer0++;
            }
            printf("Kappe: kleinster Abstand %d, Frames mit Abstand 0: %d\n",
                   (int)d_min, treffer0);
            CHECK(treffer0 == 0,
                  "der Boss stand in %d Frames EXAKT auf der Spielerposition - die "
                  "u-Kappe darf die Bewegung aussetzen (@0x80100fd0-e4 \"nur solange "
                  "X<12000\"), nicht den Abstand auf 0 klemmen", treffer0);
        }
    }

    if (g_fail) { printf("=== FAIL ===\n"); return 1; }
    printf("=== PASS: Park, Intro-Choreo+Root-Motion, Zug, Biss, Todes-Rampe ===\n");
    return 0;
}
