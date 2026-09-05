/**
 * @file test_re2_room1140_ab.c
 * @brief WELLE B — RE2-Zombie-Brain A/B im ECHTEN ROOM1140 (Skill re15-room-probe).
 *
 * Laedt ROOM1140.RDT, spawnt die 5 Briefing-Zombies ueber das raum-eigene SCD (wie
 * test_room1140_combat), und faehrt dann denselben deterministischen Spieler-Pfad einmal unter
 * dem RE1.5-Default und einmal unter RE15_AI_FLAVOR=RE2 (re15_ai_flavor_set) durch
 * re15_enemy_ai_run_all + re15_actors_anim_advance (die game_step-Reihenfolge).
 *
 * RE2-Behauptungen (Zitate: enemy_ai_re2_zombie.c):
 *   (1) Spawn-Remap: Feeder (Nibble 6) -> ACTIVE sub 8 (INIT 0x801 @0x80100AD4), Lyer -> sub 7
 *       (0x701 @0x801009E8-0x80100A84). Kein Zombie bleibt in INIT.
 *   (2) WAKE: Spieler neben einen Feeder -> er verlaesst sub 8 und laeuft (sub 1/0).
 *   (3) ANGRIFF: nah + Leiter -> Grab (0x0301 EXEC[3]) ODER Biss (0x0C01/0x0E01); ein
 *       Grab-Biss kostet die byte-zitierten 20 HP (param @0x80100014, Anwendung @0x801028F4).
 *   (4) HURT/DEATH/CORPSE: Beschuss -> Knockdown 0x501 (nach Resistenz-Abbau), Kill ->
 *       Kill-Latch 0x4000 (@0x80108294) -> CORPSE HP=-1 (@0x8010A4D4).
 *   (5) KEIN FREEZE: ueber den ganzen Lauf bleibt jeder Zombie in gueltigen Zustaenden und
 *       lebende Zombies zeigen Transitionen (der Kampf-Loop dreht).
 * Der RE1.5-Lauf dient als Baseline (unveraendertes Verhalten, Messwerte fuers Protokoll).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"    /* re15_actors_anim_advance */
#include "re15_damage.h"    /* re15_damage_seed_rng */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static int fails = 0;
#define CHECK(c, ...) do { if (!(c)) { printf("FAIL: " __VA_ARGS__); printf("\n"); fails++; } } while (0)

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long sz = ftell(f); fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

static int is_zombie(uint8_t t) { return t == 0x10 || t == 0x11 || t == 0x16; }

/* one game frame in game_step order: AI pass, then the shared anim advancer */
static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

/* room bring-up == test_room1140_combat (Sce_em_set spawns via the room's own sub00) */
static int bringup(const re15_rdt_t *rdt, int zslots[], int *nz)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    *nz = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && is_zombie(g_actors[s].type)) zslots[(*nz)++] = s;
    return (*nz == 5) ? 0 : -1;
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        fprintf(stderr, "FAIL: RDT parse/sub00\n"); free(buf); return 1;
    }

    int zslots[RE15_ACTOR_MAX], nz = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* =================== A: RE1.5-Baseline (Default-Flavor, unveraendert) =================== */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK(bringup(&rdt, zslots, &nz) == 0, "5 Zombies erwartet, %d gespawnt", nz);
    int a_first_attack = -1;
    {
        for (int f = 0; f < 30; f++) frame();
        /* Spieler neben den ersten Zombie stellen (derselbe Pfad wie im RE2-Lauf) */
        re15_actor_t *z0 = &g_actors[zslots[0]];
        for (int f = 30; f < 700; f++) {
            pl->x = z0->x + 400; pl->z = z0->z; pl->motion = 100;
            frame();
            if (a_first_attack < 0)
                for (int i = 0; i < nz; i++) {
                    re15_actor_t *e = &g_actors[zslots[i]];
                    if (e->state == 1 && (e->sub_state_1 == 3 || e->sub_state_1 == 4 ||
                                          e->sub_state_1 == 7))
                        { a_first_attack = f; break; }
                }
        }
        printf("  [A RE1.5] first attack commit frame=%d, player HP=%d\n", a_first_attack, pl->hp);
        CHECK(pl->hp <= 100, "baseline sanity");
    }

    /* ============================ B: RE2-Flavor, gleicher Pfad ============================= */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK(bringup(&rdt, zslots, &nz) == 0, "RE2: 5 Zombies erwartet, %d", nz);
    for (int f = 0; f < 10; f++) frame();

    /* (1) Spawn-Remap: alle in state 1, subs nur {7,8} */
    int n7 = 0, n8 = 0, feeder = -1, feeder2 = -1, lyer = -1;
    for (int i = 0; i < nz; i++) {
        re15_actor_t *e = &g_actors[zslots[i]];
        CHECK(e->state == 1, "RE2 spawn: Zombie slot %d muss ACTIVE sein (state=%d)",
              zslots[i], e->state);
        CHECK(e->sub_state_1 == 7 || e->sub_state_1 == 8,
              "RE2 spawn: slot %d sub 7/8 erwartet (INIT-Remap 0x701/0x801), sub=%d",
              zslots[i], e->sub_state_1);
        if (e->sub_state_1 == 7) { n7++; if (lyer   < 0) lyer   = zslots[i]; }
        else if (e->sub_state_1 == 8) { n8++; if (feeder < 0) feeder = zslots[i];
                                              else if (feeder2 < 0) feeder2 = zslots[i]; }
    }
    printf("  [B RE2] spawns: %d lying(sub7) + %d feeding(sub8)\n", n7, n8);
    CHECK(n8 >= 1, "ROOM1140 hat Feeder -> mindestens 1x sub 8");
    CHECK(n7 == 1 && lyer >= 0, "ROOM1140 hat GENAU EINEN Liege-Spawn (Deskriptor 0x88)");
    if (feeder  < 0) feeder  = zslots[0];
    if (feeder2 < 0) feeder2 = zslots[nz > 1 ? 1 : 0];

    /* (2)+(3): WAKE + Angriff auf dem Feeder-Pfad; HP-Verlust byte-zitiert 20/Biss
     * ⚠ KORRIGIERT 2026-08-20: hier stand `zA = zslots[0]` — das ist slot 1, der LIEGENDE
     * (Deskriptor 0x88, sub 7), nicht ein Fresser. Das Etikett war von Anfang an falsch; seit
     * der Liege-Spawn korrekt liegen bleibt (RE1.5-Zwilling @0x8011F9D8[0] = `jr ra`) waere der
     * Block sonst unerfuellbar. zA zeigt jetzt auf einen ECHTEN sub-8-Fresser. */
    int b_first_attack = -1, b_wake = -1, grab_seen = 0, bite_hp_drop = 0;
    int zA = feeder;
    {
        re15_actor_t *z0 = &g_actors[zA];
        int16_t hp_at_grab = pl->hp;
        for (int f = 10; f < 700; f++) {
            pl->x = z0->x + 400; pl->z = z0->z; pl->motion = 100;
            frame();
            if (b_wake < 0 && (z0->sub_state_1 == 0 || z0->sub_state_1 == 1)) b_wake = f;
            if (b_first_attack < 0 && z0->state == 1 &&
                (z0->sub_state_1 == 3 || z0->sub_state_1 == 12 || z0->sub_state_1 == 14))
                b_first_attack = f;
            if (z0->state == 1 && z0->sub_state_1 == 3) {
                if (!grab_seen) hp_at_grab = pl->hp;
                grab_seen = 1;
            }
            if (grab_seen && pl->hp < hp_at_grab) { bite_hp_drop = hp_at_grab - pl->hp; break; }
        }
        printf("  [B RE2] wake frame=%d, first attack frame=%d, grab=%d, bite drop=%d, HP=%d\n",
               b_wake, b_first_attack, grab_seen, bite_hp_drop, pl->hp);
        CHECK(b_wake >= 0, "RE2: der Feeder muss aufwachen (sub 8 -> walk)");
        CHECK(b_first_attack >= 0, "RE2: die Leiter muss einen Angriff committen (3/12/14)");
        /* HART (Review #19): der Grab MUSS zustande kommen UND exakt 20 HP kosten — die
         * Assertion darf auf keinem RNG-/Verhaltenspfad still verschwinden. */
        CHECK(grab_seen, "RE2: der Grab (sub 3) muss im Raumlauf zustande kommen");
        CHECK(bite_hp_drop == 20,
              "RE2 Grab-Biss = 20 HP (param @0x80100015), gemessen %d", bite_hp_drop);
    }

    /* (4) Beschuss-Kette an einem zweiten Zombie: Resistenz-Abbau -> Knockdown 0x501;
     * dann Kill -> Kill-Latch + CORPSE HP=-1. */
    {
        /* ⚠ KORRIGIERT 2026-08-20: war `zslots[1]` und traf damit denselben Zombie wie zA —
         * die NO-FREEZE-Messung (5) beobachtete danach eine LEICHE als Bezugspunkt. Der
         * Beschuss laeuft jetzt auf einem ZWEITEN Fresser. */
        int zB = feeder2;
        re15_actor_t *e = &g_actors[zB];
        pl->x = 30000; pl->z = 30000;                     /* Spieler weit weg (kein Grab-Rauschen) */
        int knocked = 0;
        for (int hits = 0; hits < 12 && !knocked; hits++) {
            /* Treffer-Analog EXAKT wie die RE2-Applier: `sw 2,4` nullt +0x5/+0x6/+0x7
             * (@0x80047288), danach `sb <Zeile>,5` (@0x80047324) und +0x1D2 = Zone 1
             * (@0x80047294-98). OHNE das +0x6-Nullen bliebe die Reaktions-Phase stehen und
             * der Kosten-Abzug @0x801055D8-EC liefe nur EINMAL. */
            e->hp = (int16_t)(e->hp - 10); e->state = 2;
            e->sub_state_1 = 2; e->sub_state_2 = 0; e->re2z_hits1d2 = 1;
            /* Schnellfeuer: EIN Frame je Treffer. Das ist die Bedingung, unter der der
             * Knockdown im Original ueberhaupt entsteht — der Zwischen-Handler 0x80105BC0
             * setzt in seiner Endphase `+0x223 = (rand&0x10)+15` @0x80106028-30 und
             * `+0x222 = 0` @0x80106034, laedt die Resistenz also wieder auf. Wer langsam
             * schiesst, bekommt nur Zuckungen; wer nachsetzt, wirft den Zombie um.
             * (Ohne EMD-Bank ist re2z_clip_done sofort wahr, die Erholung laeuft hier also
             * schneller ab als im Spiel — umso schaerfer ist der Pin.) */
            frame();
            if (e->sub_state_1 == 5 && e->state == 1) knocked = 1;
        }
        CHECK(knocked, "RE2: genug Treffer muessen den Knockdown 0x501 ausloesen (@0x801050A4)");
        /* Kill — Todes-ZEILE explizit auf die MAIN-Zelle ankern (2026-09-05, Welle B):
         * der Pin zitiert die MAIN-Kette (Todes-Clip 7 -> 0x907 @0x801084DC). Vorher stand
         * hier zufaellig eine MAIN-Zeile im +0x5; seit dem Draw-Strom-Umbau endet der
         * Beschuss im Knockdown-Sub 5 = DEATH-Zeile 5 = die MAGNUM-Zelle 0x801092C4 mit
         * 50%-Kopflos-Weiterlauf ((rand&0x3F)+30 Frames) — das 8-Frame-Fenster misst dann
         * eine andere Zelle. Zeile 3 (Pistole) dispatcht in JEDER Spalte die MAIN-Zelle. */
        e->hp = -20; e->state = 3; e->sub_state_1 = 3; e->sub_state_2 = 0;
        for (int f = 0; f < 8; f++) frame();
        CHECK(e->state == 7, "DEATH -> CORPSE 0x907 (@0x801084DC), state=%d", e->state);
        CHECK(e->hp == -1, "CORPSE HP=-1 (@0x8010A4D4), hp=%d", e->hp);
        CHECK((e->re2z_flags21a & 0x4000u) != 0, "Kill-Latch 0x4000 (@0x80108294)");
        printf("  [B RE2] shot chain: knockdown ok, corpse hp=%d flags21a=0x%04x motion=%d\n",
               e->hp, e->re2z_flags21a, e->motion);
    }

    /* (5) NO-FREEZE: 600 weitere Frames mit wanderndem Spieler; lebende Zombies bleiben in
     * gueltigen Zustaenden und zeigen Transitionen. */
    {
        uint32_t last_sig[RE15_ACTOR_MAX]; int trans[RE15_ACTOR_MAX];
        memset(trans, 0, sizeof trans);
        for (int i = 0; i < nz; i++)
            last_sig[i] = ((uint32_t)g_actors[zslots[i]].state << 8) | g_actors[zslots[i]].sub_state_1;
        for (int f = 0; f < 600; f++) {
            re15_actor_t *z0 = &g_actors[zA];
            pl->x = z0->x + ((f & 64) ? 500 : 2000); pl->z = z0->z; pl->motion = 100;
            if (pl->hp < 30) { pl->hp = 100; pl->state = 0; pl->hit_react = 0; }  /* Dauerlauf */
            frame();
            for (int i = 0; i < nz; i++) {
                re15_actor_t *e = &g_actors[zslots[i]];
                uint32_t sig = ((uint32_t)e->state << 8) | e->sub_state_1;
                if (sig != last_sig[i]) { trans[i]++; last_sig[i] = sig; }
                CHECK(e->state == 1 || e->state == 2 || e->state == 3 || e->state == 7 || e->state == 8,
                      "slot %d in ungueltigem State %d", zslots[i], e->state);
                if (e->state == 1)
                    CHECK(e->sub_state_1 <= 15, "slot %d ACTIVE sub %d > 15", zslots[i], e->sub_state_1);
            }
        }
        int live_moving = 0;
        for (int i = 0; i < nz; i++) {
            re15_actor_t *e = &g_actors[zslots[i]];
            if (e->state != 7 && trans[i] > 0) live_moving++;
            printf("  [B RE2] slot %d: state=%d sub=%d transitions=%d\n",
                   zslots[i], e->state, e->sub_state_1, trans[i]);
        }
        CHECK(live_moving >= 1, "mindestens ein lebender Zombie muss den Loop drehen (kein Freeze)");
    }

    printf("  [A/B] RE1.5 first-attack=%d vs RE2 first-attack=%d (Frames ab Raumstart)\n",
           a_first_attack, b_first_attack);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);               /* Default wiederherstellen */
    free(buf);
    if (fails == 0) printf("test_re2_room1140_ab: OK\n");
    else            printf("test_re2_room1140_ab: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
