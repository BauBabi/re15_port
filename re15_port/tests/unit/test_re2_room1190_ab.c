/**
 * @file test_re2_room1190_ab.c
 * @brief WELLE C — RE2-Cerberus-Brain A/B im ECHTEN ROOM1190 (Skill re15-room-probe).
 *
 * Laedt ROOM1190.RDT (der RE1.5-Hunde-Raum, Memory reai-v2-stage1-enemy-rooms), spawnt die
 * Hunde ueber das raum-eigene SCD und faehrt denselben deterministischen Spieler-Pfad einmal
 * unter dem RE1.5-Default und einmal unter RE15_AI_FLAVOR=RE2 durch re15_enemy_ai_run_all +
 * re15_actors_anim_advance (game_step-Reihenfolge).
 *
 * RE2-Behauptungen (Zitate: enemy_ai_re2_dog.c, alle selbst disassembliert aus EMD0G_MOD0.BIN):
 *   (1) INIT-Remap: jeder Hund verlaesst INIT nach 1 Frame (sw 1,4 @0x8010012C) und startet
 *       in Sub 0 IDLE (Spawn-Raw 0 -> tbl@0x80105458[0]).
 *   (2) WAKE + Eskalation: Spieler rennt neben dem Hund -> STALK (dist<4000 @0x801007B8) ->
 *       Aggro-Meter (+21/Tick rennend @0x80100990) -> RUN (>=65 @0x80100A38).
 *   (3) LUNGE: die Kette committet Sub 3, der Absprung traegt Speed 280/vy −280/Gravity 40
 *       (@0x80101320-24 / @0x801003A4 / @0x801014C0).
 *   (4) BISS/LATCH: der Flug-Biss kostet die byte-zitierten 20 HP (FUN_800401D4(20,0)
 *       @0x80104EB8-C0); ein toedlicher Biss (HP<-14 danach) latcht (Sub 7, Mash 12
 *       @0x80101D7C, Spieler-Yaw=Hund+2048 @0x80104F40-50).
 *   (5) HURT/DEATH/CORPSE: Beschuss -> Flinch Clip 17/Knockback 240; Doppel-Treffer ->
 *       Knockdown-Latch 0x80 (@0x801033FC); Kill -> SE-7-Latch + CORPSE state 7 (@0x80104290).
 *   (6) KEIN FREEZE ueber 600 Frames; lebende Hunde drehen den Loop.
 * Der RE1.5-Lauf ist die unveraenderte Baseline (Messwerte fuers Protokoll).
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
#include "re2_ems.h"        /* RE2-Bank fuer den B-Lauf (echte Clip-Laengen: Latch 145F usw.) */

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

static void frame(void) { re15_enemy_ai_run_all(1); re15_actors_anim_advance(); }

static int bringup(const re15_rdt_t *rdt, int dslots[], int *nd)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    /* ROOM1190s Hunde-Welle lebt in sub13 (RDT-Scan: die drei Sce_em_set 0x44/type 0x20/
     * behavior 0x40 liegen @RDT 0x2900/0x2914/0x2928 im sub13-Fenster 0x28AA..0x2B14 —
     * das Fenster-Sprung-Event; sub00 spawnt keine Gegner). Der Test zuendet das Event
     * direkt, wie es der AOT-Ausloeser im Spiel taete. */
    if (rdt->sub_scd_count > 13 && rdt->sub_scd[13]) scd_thread_start(2, rdt->sub_scd[13]);
    g_scd.work_vars[10] = 0;
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    *nd = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x20) dslots[(*nd)++] = s;
    return (*nd >= 1) ? 0 : -1;
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1190.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        fprintf(stderr, "FAIL: RDT parse/sub00\n"); free(buf); return 1;
    }

    int dslots[RE15_ACTOR_MAX], nd = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* =================== A: RE1.5-Baseline (Default-Flavor, unveraendert) =================== */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK(bringup(&rdt, dslots, &nd) == 0, ">=1 Hund erwartet, %d gespawnt", nd);
    printf("  [A RE1.5] %d Hunde gespawnt\n", nd);
    int a_first_attack = -1;
    {
        for (int f = 0; f < 30; f++) frame();
        re15_actor_t *d0 = &g_actors[dslots[0]];
        for (int f = 30; f < 700; f++) {
            pl->x = d0->x + 1500; pl->z = d0->z; pl->motion = 100;
            frame();
            if (a_first_attack < 0)
                for (int i = 0; i < nd; i++) {
                    re15_actor_t *e = &g_actors[dslots[i]];
                    if (e->state == 1 && e->sub_state_1 >= 5 && e->sub_state_1 <= 8)
                        { a_first_attack = f; break; }     /* RE1.5-Angriffs-Subs 5..8 */
                }
        }
        printf("  [A RE1.5] first attack commit frame=%d, player HP=%d\n", a_first_attack, pl->hp);
        CHECK(pl->hp <= 100, "baseline sanity");
    }

    /* ============================ B: RE2-Flavor, gleicher Pfad ============================= */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK(bringup(&rdt, dslots, &nd) == 0, "RE2: >=1 Hund erwartet, %d", nd);
    /* Die ECHTE RE2-Bank laden (Welle-A-Splitter, wie pc_enemy_load_re2 ohne TIM-Upload):
     * damit laufen Clip-Laengen (Latch 23=145F, Hang 24=45F, ...), Frame-Events und das
     * Mash-/Gravity-Fenster mit den Original-Zeiten — Voraussetzung fuer die gehaertete
     * Latch-Aufloesungs-/Boden-Assertion (Review-Fix #8). EMS bleibt resident (Bank aliast). */
    {
        static uint8_t *ems = NULL; static long ems_sz = 0;
        if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
        CHECK(ems != NULL, "shared_assets/RE2/CDEMD0.EMS muss fuer den B-Lauf lesbar sein");
        if (ems) {
            re15_enemy_bank_t *eb = re15_enemy_find(0x20);
            if (!eb) eb = re15_enemy_alloc(0x20);
            CHECK(eb && re2_ems_load_bank(ems, (size_t)ems_sz, 0x20, eb, NULL) == 0,
                  "RE2 EM020-Bank muss parsen (TOC Sektor 1250, dir@0x1D918)");
            if (eb) { eb->buf = NULL; eb->ok = 1; }
        }
    }
    for (int f = 0; f < 3; f++) frame();

    /* (1) INIT-Remap: alle Hunde ACTIVE, Start-Sub 0/1 (IDLE bzw. schon geweckt) */
    for (int i = 0; i < nd; i++) {
        re15_actor_t *e = &g_actors[dslots[i]];
        CHECK(e->state == 1, "RE2 spawn: Hund slot %d muss ACTIVE sein (sw 1,4 @0x8010012C), state=%d",
              dslots[i], e->state);
        CHECK(e->sub_state_1 <= 1, "RE2 spawn: slot %d Start-Sub 0 (tbl@0x80105458[0]), sub=%d",
              dslots[i], e->sub_state_1);
        CHECK(e->hp >= 59 && e->hp <= 122,
              "RE2 HP-Roll 59..119+3 (tbl@0x80105340), slot %d hp=%d", dslots[i], e->hp);
    }

    /* (2)+(3): WAKE -> STALK -> RUN -> Sub-3-Lunge auf dem Renn-Pfad */
    int b_wake = -1, b_run = -1, b_attack = -1, b_air = -1;
    int dA = dslots[0];
    {
        re15_actor_t *d0 = &g_actors[dA];
        for (int f = 0; f < 700; f++) {
            pl->x = d0->x + 1500; pl->z = d0->z; pl->motion = 100;
            pl->floor = d0->floor;
            frame();
            if (b_wake  < 0 && d0->state == 1 && d0->sub_state_1 == 1) b_wake = f;
            if (b_run   < 0 && d0->state == 1 && d0->sub_state_1 == 2) b_run = f;
            if (b_attack < 0 && d0->state == 1 && d0->sub_state_1 == 3) b_attack = f;
            if (b_attack >= 0 && b_air < 0 && d0->re2d_air219 && d0->speed_h == 280) b_air = f;
            if (b_air >= 0) break;
        }
        printf("  [B RE2] wake=%d run=%d attack=%d airborne(280/-280)=%d HP=%d\n",
               b_wake, b_run, b_attack, b_air, pl->hp);
        CHECK(b_wake >= 0, "RE2: der Hund muss aufwachen (Sub 0 -> 1, @0x80100800)");
        CHECK(b_run >= 0, "RE2: Aggro-Meter muss RUN committen (@0x80100A38-78)");
        CHECK(b_attack >= 0, "RE2: die Kette muss Sub 3 committen (@0x801009C8/@0x80100DD8)");
        CHECK(b_air >= 0, "RE2: der Absprung muss Speed 280 + Luft-Flag tragen (@0x80101320-2C)");
    }

    /* (4) BISS-Schaden 20 + LATCH: Spieler mit HP 100 im Landekorridor -> −20/Biss; dann
     * HP 4 -> toedlicher Biss -> Sub 7 Latch mit Spieler-Yaw = Hund+2048. */
    {
        re15_actor_t *d0 = &g_actors[dA];
        /* die uebrigen Hunde fuer die Einzel-Biss-Messung schlafen legen (mehrere Hunde
         * beissen sonst im selben Fenster -> kumulierter Drop) */
        uint8_t saved_active[RE15_ACTOR_MAX];
        for (int i = 1; i < nd; i++) {
            saved_active[i] = g_actors[dslots[i]].active;
            g_actors[dslots[i]].active = 0;
        }
        int16_t hp_prev = pl->hp;
        int bite_drop = 0, latch_seen = 0, yaw_ok = 0;
        for (int f = 0; f < 900; f++) {
            pl->motion = 100;
            pl->floor = d0->floor;
            /* Latch-Gate 0x80015910==0 = frontal (@0x80104EEC-F8): Leon schaut den Hund an */
            pl->rot_y = (int16_t)((re15_atan2_q12(d0->z - pl->z, d0->x - pl->x) - 0x400) & 0xfff);
            hp_prev = pl->hp;
            frame();
            if (pl->hp < hp_prev) { bite_drop = hp_prev - pl->hp; break; }   /* EIN Biss-Event */
            if (f % 120 == 119) { pl->x = d0->x + 1200; pl->z = d0->z; }   /* nachstellen */
        }
        printf("  [B RE2] first bite drop=%d HP=%d\n", bite_drop, pl->hp);
        CHECK(bite_drop == 20, "RE2 Flug-Biss = 20 HP (FUN_800401D4(20,0) @0x80104EB8-C0), war %d",
              bite_drop);

        /* Latch: HP 4 -> −16 < −14 -> return 2 -> Latch (@0x80104EE0-F74) */
        pl->hp = 4; pl->state = 0; pl->hit_react = 0;
        for (int f = 0; f < 900 && !latch_seen; f++) {
            pl->motion = 100;
            pl->floor = d0->floor;
            pl->rot_y = (int16_t)((re15_atan2_q12(d0->z - pl->z, d0->x - pl->x) - 0x400) & 0xfff);
            frame();
            if (d0->state == 1 && d0->sub_state_1 == 7) {
                latch_seen = 1;
                frame();                                   /* Sub-7-P0 stellt den Soll-Yaw
                                                            * (0x800CFC6E-Analog @0x80101E04-14) */
                yaw_ok = (((pl->rot_y - d0->rot_y) & 0xfff) == 2048);
            }
            if (f % 120 == 119) { pl->x = d0->x + 1200; pl->z = d0->z; }
        }
        for (int i = 1; i < nd; i++) g_actors[dslots[i]].active = saved_active[i];
        printf("  [B RE2] latch=%d yaw+2048=%d playerHP=%d\n", latch_seen, yaw_ok, pl->hp);
        CHECK(latch_seen, "RE2: der toedliche Biss muss latchen (Sub 7 @0x8010142C)");
        CHECK(!latch_seen || yaw_ok, "RE2 Latch: Spieler-Yaw = Hund+2048 (@0x80104F40-50)");
        /* Latch-Ausgang (Review-Fix #8 — die alte pl->hp<0-Klausel war vacuous, weil der Latch
         * NUR ueber den toedlichen Biss entsteht): ECHTE Aufloesung asserten = die Clip-
         * 23(145F)->24(45F)->25(20F)-Kette muss in Sub 12 muenden (sb 12,5 @0x8010221C), und
         * der Gravity-Block des Mash-Fensters (@0x80102070-A0, Boden-Klemme @0x80102098-A0)
         * muss den Hund waehrend des Bisses auf den Boden geholt haben. */
        int resolved = 0;
        for (int f = 0; f < 400 && !resolved; f++) {
            frame();
            if (d0->state == 1 && d0->sub_state_1 == 12) resolved = 1;
        }
        CHECK(resolved, "RE2 Latch muss in Sub 12 aufloesen (Clip 23/24/25-Kette @0x8010221C), "
                        "state=%d sub=%d motion=%d", d0->state, d0->sub_state_1, d0->motion);
        CHECK(d0->y == (int32_t)d0->dog_floor_y,
              "RE2 Latch endet am Boden (Gravity @0x80102070-A0, Klemme @0x80102098-A0), "
              "y=%d floor=%d", (int)d0->y, (int)d0->dog_floor_y);
        /* Spieler fuer die Folgephasen wiederbeleben */
        pl->hp = 100; pl->state = 0; pl->hit_react = 0; pl->motion = 0;
        re15_player_clear_hit_guard(pl);
    }

    /* (5) Beschuss-Kette an einem Hund: Flinch -> Doppel-Treffer-Knockdown -> Kill -> CORPSE */
    {
        re15_actor_t *e = &g_actors[dA];
        pl->x = 30000; pl->z = 30000;
        if (e->state != 1) { e->state = 1; e->sub_state_1 = 1; e->sub_state_2 = 1; }
        e->hp = 60;
        int flinched = 0, knocked = 0;
        /* Doppel-Treffer: der zweite Treffer muss landen, solange +0x223 noch steht — ohne
         * geladene Bank laufen die Hurt-Clips in ~3 Ticks durch (P2-Ende cleart +0x223
         * @0x8010370C), also Treffer 2 direkt im Folge-Frame (im Original haelt das Fenster
         * Clip 17=13F + Clip 7=50F offen). */
        e->hp = (int16_t)(e->hp - 10); e->state = 2;       /* take_damage-Analog */
        e->sub_state_2 = 0; e->sub_state_3 = 0;
        frame();
        if (e->motion == 17) flinched = 1;                 /* Clip 17 @0x80103460-7C */
        e->hp = (int16_t)(e->hp - 10); e->state = 2;       /* Treffer 2 waehrend +0x223==1 */
        frame();
        if (e->re2d_dbl223 & 0x80u) knocked = 1;           /* Knockdown-Latch @0x801033FC-400 */
        for (int f = 0; f < 30 && e->state == 2; f++) frame();   /* Knockdown auslaufen lassen */
        CHECK(flinched, "RE2: Beschuss muss den Flinch-Clip 17 spielen (@0x80103460-7C)");
        CHECK(knocked, "RE2: Doppel-Treffer muss den Knockdown latchen (@0x801033FC-400)");
        e->hp = -20; e->state = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;
        for (int f = 0; f < 30 && e->state != 7; f++) frame();
        CHECK(e->state == 7, "DEATH -> CORPSE (sw 7,4 @0x80104290), state=%d", e->state);
        for (int f = 0; f < 4; f++) frame();
        CHECK(e->hp == -1, "CORPSE HP=-1 (Port-Konvention, Init @0x80104A08), hp=%d", e->hp);
        printf("  [B RE2] shot chain: flinch=%d knockdown=%d corpse state=%d hp=%d\n",
               flinched, knocked, e->state, e->hp);
    }

    /* (6) NO-FREEZE: 600 Frames wandernder Spieler; gueltige Zustaende + Transitionen */
    {
        uint32_t last_sig[RE15_ACTOR_MAX]; int trans[RE15_ACTOR_MAX];
        memset(trans, 0, sizeof trans);
        for (int i = 0; i < nd; i++)
            last_sig[i] = ((uint32_t)g_actors[dslots[i]].state << 8) | g_actors[dslots[i]].sub_state_1;
        int live = 0;
        for (int i = 0; i < nd; i++) if (g_actors[dslots[i]].state != 7) live++;
        for (int f = 0; f < 600; f++) {
            re15_actor_t *ref = NULL;
            for (int i = 0; i < nd; i++)
                if (g_actors[dslots[i]].state != 7) { ref = &g_actors[dslots[i]]; break; }
            if (ref) {
                pl->x = ref->x + ((f & 64) ? 800 : 2500); pl->z = ref->z;
                pl->motion = 100; pl->floor = ref->floor;
            }
            if (pl->hp < 30) { pl->hp = 100; pl->state = 0; pl->hit_react = 0; }
            frame();
            for (int i = 0; i < nd; i++) {
                re15_actor_t *e = &g_actors[dslots[i]];
                uint32_t sig = ((uint32_t)e->state << 8) | e->sub_state_1;
                if (sig != last_sig[i]) { trans[i]++; last_sig[i] = sig; }
                CHECK(e->state == 1 || e->state == 2 || e->state == 3 || e->state == 7,
                      "slot %d in ungueltigem State %d", dslots[i], e->state);
                if (e->state == 1)
                    CHECK(e->sub_state_1 <= 16, "slot %d ACTIVE sub %d > 16", dslots[i], e->sub_state_1);
            }
        }
        int live_moving = 0, live_parked = 0, live_total = 0;
        for (int i = 0; i < nd; i++) {
            re15_actor_t *e = &g_actors[dslots[i]];
            if (e->state != 7) {
                live_total++;
                if (trans[i] > 0) live_moving++;
                /* Sub 11 (Rudel-Hilfe) ist ein BYTE-TRUE Park-Zustand: der Executor
                 * @0x80102490 hat keinen Exit-Commit (Tail nur advance @0x801025D8), und sein
                 * Eintritts-Bit 0xFBF4&0x40 (Mash-Release @0x8010205C) ist room-persistent —
                 * einziger EXE-Clear ist der Room-Init FUN_80052f3c. Nach einem Latch parkt
                 * das Rudel beim Opfer; das ist kein Port-Freeze. */
                else if (e->state == 1 && e->sub_state_1 == 11) live_parked++;
            }
            printf("  [B RE2] slot %d: state=%d sub=%d transitions=%d\n",
                   dslots[i], e->state, e->sub_state_1, trans[i]);
        }
        if (live_total > 0)
            CHECK(live_moving >= 1 || live_moving + live_parked == live_total,
                  "lebende Hunde muessen den Loop drehen ODER byte-true in Sub 11 parken "
                  "(moving=%d parked=%d live=%d)", live_moving, live_parked, live_total);
    }

    printf("  [A/B] RE1.5 first-attack=%d (Frames ab Raumstart)\n", a_first_attack);

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    free(buf);
    if (fails == 0) printf("test_re2_room1190_ab: OK\n");
    else            printf("test_re2_room1190_ab: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
