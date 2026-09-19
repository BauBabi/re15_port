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

/* u = ABSOLUTES X wie im Modul seit Phase 2 (analysis/befunde_2026-09-19/birkin-g5.md,
 * Skeptiker 4.1.3): RE2 liest fuer jede Schwelle das Entity-X direkt
 * (`lw a0,56(s1); slti a0,a0,12000` @0x80100fd0, `slti v0,v1,12001` @0x80104074,
 * `slti v1,v1,9001` @0x801008a0) - die Kappe wandert NICHT mit dem Spieler.
 * HIER STAND `12000 - |dx|` (spielerrelativ), das Port-Konstrukt der Runde 7. */
static int32_t u_von(const re15_actor_t *e) { return (int32_t)e->x; }

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
    /* SPIELERSTAND BEIM KAMPFSTART - gemessen, nicht aus Plc_dest abgelesen.
     * @0x1322 ist Modus 9 (Handler 0x80073e30[9] = 0x80031360: nur Yaw-Schleifer
     * 0x8001aac4 + Kegeltest 0x8001ab9c, KEIN pos_advance 0x800245d8) - der Spieler
     * DREHT sich dort, er geht nicht nach x=300. Er bleibt stehen, wo ihn der
     * Kamerawechsel 13->12 erwischt (RVD @0x04F4, x 12249..13873): gemessen x=13699,
     * Nutzer-Marke 13600. */
    pl->x = 13699; pl->z = -23400; pl->y = 0;

    re15_actor_t *e = &g_actors[2];
    memset(e, 0, sizeof *e);
    e->active = 1; e->type = 0x36; e->flags = 1; e->hp = 600;
    e->x = -14700; e->z = -23350; e->grid_id = 0x33;   /* RDT-Spawn @0x124A */

    /* PIN 1: UNARMIERT WIRD DIE POSITION NICHT ANGETASTET.
     * ⛔ Hier stand `CHECK(e->x == -32000 && e->z == -32000)` - der Port parkte den Boss
     * off-world, solange grid != 0x13. Dieses Fenster ist aber genau das Sleep(1)
     * @0x1306 ZWISCHEN Pos_set(1200) @0x12FE und Member_set @0x130A: der Park warf den
     * Skriptwert weg, bevor der Kampfstart ihn lesen konnte (gemessen "nach SCD x=1200
     * -> nach game_step x=-32000"). Das Original tut unarmiert NICHTS (@0x801011d0-dc). */
    for (int t = 0; t < 30; t++) re15_g5_boss_tick(2);
    CHECK(e->x == -14700 && e->z == -23350,
          "UNARMIERT: die Position wurde angetastet - pos=(%d,%d), erwartet (-14700,-23350)",
          (int)e->x, (int)e->z);

    /* PIN 2: Kampfstart -> Intro-Choreo mit Root-Motion.
     * Der Boss startet dort, wo das SKRIPT ihn hinsetzt: Pos_set(1200, 0, -23350)
     * @0x12FE, ein Bild vor dem Member_set. Von dort traegt die RE2-Intro-Spur
     * +10960 (7014+3946) nach OSTEN auf den Spieler zu - u waechst. */
    e->x = 1200; e->z = -23350;                    /* Pos_set @0x12FE */
    e->grid_id = 0x13;                             /* Member_set(0x0c,0x13) @0x130A */
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
    /* ⛔ Hier stand erst `e->x <= -14000 || u_start <= -2500` ("Intro startet am RDT-Spawn"),
     * dann `u_start ~ -500` ("auf dem Skriptwert 1200"). BEIDES war eine Port-Buehne:
     * RE2s [T0] UEBERSCHREIBT beim Armieren den Skript-/Spawnwert selbst -
     * `addiu v0,zero,-9000 / sw v0,56(s0)` @0x801011d0 und `addiu v0,zero,-23400 /
     * sw v0,64(s0)` @0x801011d8. Der Wert ist uebertragbar, weil das Westende von
     * ROOM5090 (SCA-Eckbloecke [21]/[25] x=-5683) mit RE2 room7040 ([2]/[4] x=-5683)
     * x-identisch ist; G5 kommt bei X=-9000 durch die Luecke ("hinten durch das
     * Zug-Rechteck"). Pos_set(1200) @0x12FE galt dem RE1.5-Humanoiden 0x30. */
    CHECK(u_start == -9000 && e->z == -23400,
          "Kampfstart setzt die Position nicht selbst auf (-9000,-23400) @0x801011d0/d8"
          " (ist x=%d z=%d)", u_start, (int)e->z);
    CHECK(u_von(e) > u_start + 8000,
          "Root-Motion traegt nicht: X-Delta %d (erwartet > 8000: Intro 10960 + Zuege)",
          u_von(e) - u_start);
    /* ⛔ Hier stand `u_von(e) <= 12000`. Byte-true ist die Kappe ein VORAB-Test:
     * `lw a0,56(s1); slti a0,a0,12000; beq a0,zero,<ueberspringen>` (@0x80100fc8-d4) -
     * der Schritt wird nur ausgelassen, wenn X SCHON >= 12000 ist; der letzte Schritt
     * darf also ueberschiessen. Im Hauptlauf ist die Kappe nicht messbar, weil der Boss
     * vorher DEVOURt und byte-true fuer immer in sub4 stehen bleibt - der Kappen-Pin
     * steht deshalb unten im Flucht-Lauf (Spieler auf Dauer-HP, kein Devour-Gate).
    CHECK(e->x < g_actors[RE15_ACTOR_SLOT_PLAYER].x,
          "der Boss muss WESTLICH des Spielers bleiben (x=%d, Spieler=%d) - er kommt "
          "von vorn aus dem Zug, nicht vom Eingang hinter dem Spieler",
          (int)e->x, (int)g_actors[RE15_ACTOR_SLOT_PLAYER].x);

    /* PIN 4: der Kampf hat den Spieler getroffen (Biss 40 und/oder Devour
     * Clip 9); der Hauptlauf resettet hp je Tick, das Ereignis wird VOR dem
     * Reset gezaehlt. Nach einem Devour haelt der Boss byte-true den
     * sub4-Endzustand ("kein Routine-Exit - der Spieler ist tot"). */
    printf("Treffer-Ereignisse im Hauptlauf: %d, Devour-Clip 9 gesehen: %d,"
           " Boss-X am Ende %d, Abstand %d\n",
           treffer, saw_clip[9], (int)e->x,
           (int)(g_actors[RE15_ACTOR_SLOT_PLAYER].x - e->x));
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
            int32_t x_max = e3->x, x_wachs_nach_kappe = 0;
            for (int t = 0; t < 2500; t++) {
                pl->x = 0; pl->z = -23350;           /* Leon steht - nur der Boss bewegt sich */
                pl->hp = 200; pl->hit_react = 0;
                re15_g5_boss_tick(slot_a);
                int32_t d = (int32_t)pl->x - (int32_t)e3->x;
                if (d < 0) d = -d;
                if (d < d_min) d_min = d;
                if (d == 0) treffer0++;
                if (e3->x > x_max) { if (x_max >= 12000) x_wachs_nach_kappe++; x_max = e3->x; }
            }
            printf("Kappe: kleinster Abstand %d, Frames mit Abstand 0: %d, groesstes X %d,"
                   " Wachstum NACH der Kappe: %d\n",
                   (int)d_min, treffer0, (int)x_max, x_wachs_nach_kappe);
            CHECK(x_max >= 12000, "Zug-Kappe nie erreicht (groesstes X %d)", (int)x_max);
            CHECK(x_wachs_nach_kappe == 0,
                  "X waechst noch, nachdem 12000 ueberschritten war (%d Schritte) - der"
                  " Vorab-Test `slti a0,a0,12000` @0x80100fd0 greift nicht",
                  x_wachs_nach_kappe);
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
