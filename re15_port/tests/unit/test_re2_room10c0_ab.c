/**
 * @file test_re2_room10c0_ab.c
 * @brief WELLE D — RE2-Kraehen-Brain A/B im ECHTEN ROOM10C0 (Skill re15-room-probe).
 *
 * Laedt ROOM10C0.RDT (der RE1.5-Kraehen-Raum, Memory reai-v2-stage1-enemy-rooms; die drei
 * Sce_em_set 0x44/type 0x21 liegen @RDT 0xF8C/0xFA0/0xFB4 im sub00-Fenster — selbst gescannt)
 * und faehrt denselben deterministischen Spieler-Pfad einmal unter dem RE1.5-Default und
 * einmal unter RE2 durch re15_enemy_ai_run_all + re15_actors_anim_advance.
 *
 * RE2-Behauptungen (Zitate: enemy_ai_re2_crow.c, alle selbst disassembliert aus EMOVL21_S0.BIN):
 *   (1) INIT: state=1 nach 1 Frame (sw 1,4 @0x8010032C), HP=10 (@0x80100324/348), Start Sub 0.
 *   (2) WAKE/TAKEOFF: Spieler nah -> Alarm (sltiu 0x709 @0x80100704) -> Abheben (Clip 7,
 *       Speed 100+r&0x1f @0x80100CDC-EC) -> Kreisen Sub 4; die Kraehe GEWINNT HOEHE (y sinkt).
 *   (3) ARBITRIERUNG: es haelt NIE mehr als eine Kraehe den Claim +0x22A&4 / Mutex
 *       0x800CFBF4-Bit 0 (@0x8010426C-7C, Post-Pass-Release @0x801044C4-F0).
 *   (4) GRAB mit HP-Fall: der Peck kostet exakt 5 HP (0x800401d4(5,mode) @0x8010265C-64).
 *   (5) HURT-RECOVERY: eine im Flug angeschossene Kraehe (HP>=0) fliegt WIEDER (state 1,
 *       @0x80102C08-7C); ein Kill endet in CORPSE (7,0) mit Lache 400->+10/Tick
 *       (@0x80103A28-44/@0x80103AC8-FC) und Zuck-Zustand (state 7 haelt).
 *   (6) KEIN FREEZE ueber 600 Frames; lebende Kraehen drehen den Loop.
 * Der RE1.5-Lauf ist die unveraenderte Baseline (Messwerte fuers Protokoll).
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_enemy.h"
#include "re15_ai_flavor.h"
#include "re15_player.h"
#include "re15_damage.h"
#include "re2_ems.h"

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

static int bringup(const re15_rdt_t *rdt, int cslots[], int *nc)
{
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_enemy_ai_set_paused(0);
    re15_damage_seed_rng(0x0badf00du);
    if (rdt->main_scd) scd_thread_start(0, rdt->main_scd);
    scd_thread_start(1, rdt->sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1; pl->x = 0; pl->y = 0; pl->z = 0;
    pl->hp = 100; pl->hit_react = 0; pl->state = 0; pl->motion = 0; pl->floor = 0;
    *nc = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x21) cslots[(*nc)++] = s;
    return (*nc >= 1) ? 0 : -1;
}

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM10C0.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }
    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0 || !rdt.sub_scd[0]) {
        fprintf(stderr, "FAIL: RDT parse/sub00\n"); free(buf); return 1;
    }

    int cslots[RE15_ACTOR_MAX], nc = 0;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* =================== A: RE1.5-Baseline (Default-Flavor, unveraendert) =================== */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    CHECK(bringup(&rdt, cslots, &nc) == 0, ">=1 Kraehe erwartet, %d gespawnt", nc);
    printf("  [A RE1.5] %d Kraehen gespawnt\n", nc);
    {
        re15_actor_t *c0 = &g_actors[cslots[0]];
        int a_launch = -1;
        for (int f = 0; f < 300; f++) {
            pl->x = c0->x + 1200; pl->z = c0->z; pl->motion = 100;
            frame();
            if (a_launch < 0 && c0->state == 1 && c0->sub_state_1 >= 1) a_launch = f;
        }
        printf("  [A RE1.5] first flight-sub frame=%d state=%d sub=%d\n",
               a_launch, c0->state, c0->sub_state_1);
        CHECK(pl->hp <= 100, "baseline sanity");
    }

    /* ============================ B: RE2-Flavor, gleicher Pfad ============================= */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK(bringup(&rdt, cslots, &nc) == 0, "RE2: >=1 Kraehe erwartet, %d", nc);
    /* ECHTE RE2-Bank laden (Welle-A-Splitter): EM021.EMD @Sektor 0x543 (TOC @0x8009adf4),
     * dir @0x7E08, 8 Sektionen; Paar 1 = 12 Clips (EDD @0xC), EMR 13 Knochen kf 72; Paar 3 =
     * Victim {14,36,20}F auf 15-Bone-Player-Pool — alle Werte selbst geparst 2026-08-16. */
    {
        static uint8_t *ems = NULL; static long ems_sz = 0;
        if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
        CHECK(ems != NULL, "shared_assets/RE2/CDEMD0.EMS muss fuer den B-Lauf lesbar sein");
        if (ems) {
            re15_enemy_bank_t *eb = re15_enemy_find(0x21);
            if (!eb) eb = re15_enemy_alloc(0x21);
            int rc = eb ? re2_ems_load_bank(ems, (size_t)ems_sz, 0x21, eb, NULL) : -1;
            CHECK(rc == 0, "RE2 EM021-Bank muss parsen (Sektor 0x543, dir@0x7E08), rc=%d", rc);
            if (eb && rc == 0) {
                eb->buf = NULL; eb->ok = 1;
                CHECK(eb->anim.clip_count == 12,
                      "EM021 Haupt-Bank = 12 Clips (EDD @0xC, selbst geparst), n=%d",
                      eb->anim.clip_count);
                CHECK(eb->skel.bone_count == 13,
                      "EM021 EMR = 13 Knochen (Header @0x3E4+2), n=%d", eb->skel.bone_count);
                CHECK(eb->victim_ok, "EM021 Victim-Paar dir[5]/[6] (Clips 14/36/20F) muss parsen");
            }
        }
    }
    for (int f = 0; f < 3; f++) frame();

    /* (1) INIT: ACTIVE, Sub<=1, HP=10 */
    for (int i = 0; i < nc; i++) {
        re15_actor_t *e = &g_actors[cslots[i]];
        CHECK(e->state == 1, "RE2 spawn: Kraehe slot %d ACTIVE (sw 1,4 @0x8010032C), state=%d",
              cslots[i], e->state);
        CHECK(e->sub_state_1 <= 1, "RE2 spawn: slot %d Start-Sub 0/1, sub=%d",
              cslots[i], e->sub_state_1);
        CHECK(e->hp == 10, "RE2 HP=10 (@0x80100324/348), slot %d hp=%d", cslots[i], e->hp);
    }

    /* (2) WAKE -> TAKEOFF -> KREISEN; Hoehengewinn */
    int b_alarm = -1, b_takeoff = -1, b_circle = -1;
    int cA = cslots[0];
    {
        re15_actor_t *c0 = &g_actors[cA];
        int32_t y0 = c0->y;
        for (int f = 0; f < 600; f++) {
            pl->x = c0->x + 1200; pl->z = c0->z; pl->motion = 100; pl->floor = 0;
            frame();
            if (b_alarm   < 0 && c0->state == 1 && c0->sub_state_1 == 1) b_alarm = f;
            if (b_takeoff < 0 && c0->state == 1 && c0->sub_state_1 == 2) b_takeoff = f;
            if (b_circle  < 0 && c0->state == 1 && c0->sub_state_1 >= 4 && c0->sub_state_1 <= 6)
                { b_circle = f; break; }
        }
        printf("  [B RE2] alarm=%d takeoff=%d circle=%d y0=%d y=%d\n",
               b_alarm, b_takeoff, b_circle, (int)y0, (int)c0->y);
        /* (Der Alarm (Sub 1) dauert nur rand&7 Ticks und faellt oft in die 3 Warm-up-Frames —
         * die Sequenz-Assertion haengt am Takeoff/Circle; Sub 1 selbst pinnt der Unit-Test.) */
        CHECK(b_takeoff >= 0 || b_circle >= 0, "RE2: Abheben muss kommen (@0x80100C0C-14)");
        CHECK(b_circle  >= 0, "RE2: Kreisen/Steigen/Sinken muss erreicht werden (@0x80100E84-98)");
        CHECK(c0->y < y0, "RE2: die Kraehe muss Hoehe gewinnen (vy<0 @0x80100CF0-F4), y0=%d y=%d",
              (int)y0, (int)c0->y);
    }

    /* (3) ARBITRIERUNG: nie zwei Claims; irgendwann committet einer 11..14 */
    int b_commit = -1, dual_claim = 0;
    {
        for (int f = 0; f < 900; f++) {
            re15_actor_t *c0 = &g_actors[cA];
            pl->x = c0->x + 1500; pl->z = c0->z; pl->motion = 100;
            frame();
            int claims = 0;
            for (int i = 0; i < nc; i++) {
                re15_actor_t *e = &g_actors[cslots[i]];
                if (e->re2c_flags22a & 0x4u) claims++;
                if (b_commit < 0 && e->state == 1 && e->sub_state_1 >= 11 && e->sub_state_1 <= 14)
                    b_commit = f;
            }
            if (claims > 1) dual_claim = 1;
        }
        printf("  [B RE2] first attack-commit=%d dual_claim=%d gflags=%04x\n",
               b_commit, dual_claim, (unsigned)g_re2_room_gflags);
        CHECK(b_commit >= 0, "RE2: die Arbitrierung muss einen Angriff committen (@0x801042E4-304)");
        CHECK(!dual_claim, "RE2: NIE zwei Claims gleichzeitig (Mutex @0x8010426C-7C)");
    }

    /* (4) GRAB mit HP-Fall: exakt 5 HP pro Peck (@0x8010265C-64). Der STRIKE traegt im Port
     * keinen HP-Verlust (EXE-State-2-Handler OFFEN, Datei-Kopf enemy_ai_re2_crow.c) — der
     * messbare HP-Fall kommt aus dem GRAB. */
    {
        int grabbed = -1, drop_bad = 0, drops = 0;
        int16_t prev = pl->hp;
        for (int f = 0; f < 1800 && drops < 2; f++) {
            re15_actor_t *c0 = &g_actors[cA];
            pl->motion = 100; pl->floor = 0;
            if (f % 90 == 0) { pl->x = c0->x + 400; pl->z = c0->z; }
            prev = pl->hp;
            frame();
            if (pl->hp < prev) {
                drops++;
                if (prev - pl->hp != 5) drop_bad = prev - pl->hp;
            }
            for (int i = 0; i < nc; i++)
                if (g_actors[cslots[i]].state == 1 && g_actors[cslots[i]].sub_state_1 == 14
                    && grabbed < 0) grabbed = f;
            if (pl->hp < 30) { pl->hp = 100; }
        }
        printf("  [B RE2] grab=%d peck-drops=%d HP=%d\n", grabbed, drops, pl->hp);
        CHECK(grabbed >= 0, "RE2: der GRAB (Sub 14) muss erreicht werden (@0x80102444-48)");
        CHECK(drops >= 1, "RE2: mindestens ein Peck muss HP kosten (0x800401d4(5,..))");
        CHECK(drop_bad == 0, "RE2: jeder Peck kostet exakt 5 HP, war %d", drop_bad);
    }

    /* (5) HURT-RECOVERY + Kill -> Corpse mit Lache/Zucken */
    {
        re15_actor_t *e = &g_actors[cA];
        pl->hp = 100; pl->state = 0; pl->hit_react = 0;
        pl->x = e->x + 6000; pl->z = e->z;
        if (e->state != 1) { e->state = 1; e->sub_state_1 = 4; e->sub_state_2 = 0; }
        /* Anschuss (nicht toedlich): take_damage-Analog */
        e->hp = 8; e->state = 2; e->sub_state_2 = 0; e->sub_state_3 = 0;
        int rec = 0;
        for (int f = 0; f < 200 && !rec; f++) { frame(); if (e->state == 1) rec = 1; }
        CHECK(rec, "RE2: HP>=0 -> die Kraehe erholt sich und fliegt wieder (@0x80102C08-7C), "
                   "state=%d sub=%d", e->state, e->sub_state_1);
        /* Kill */
        e->hp = -1; e->state = 3; e->sub_state_2 = 0; e->sub_state_3 = 0;
        int corpse = 0;
        for (int f = 0; f < 200 && !corpse; f++) { frame(); if (e->state == 7) corpse = 1; }
        CHECK(corpse, "RE2: Kill -> CORPSE state 7 (@0x80102C70-7C), state=%d", e->state);
        CHECK(e->sub_state_1 == 0, "RE2: Flug-Kill -> Corpse-Sub 0 (Lache), sub=%d", e->sub_state_1);
        frame();                                           /* Ph0: Basis 400, faellt in Ph1 ->
                                                            * erster Grow-Tick sofort (410) */
        uint16_t w0 = e->crow_shadow_w;
        CHECK(w0 == 410, "RE2: Lache-Basis 400 + Eintritts-Grow (+10, Ph0-Fallthrough "
              "@0x80103A4C-50), w=%d", w0);
        for (int f = 0; f < 300; f++) frame();
        CHECK(e->crow_shadow_w > 400 && e->crow_shadow_w <= 1000,
              "RE2: Lache +10/Tick x60 (@0x80103AC8-FC, Cap 400+600), w=%d", e->crow_shadow_w);
        CHECK(e->state == 7, "RE2: Zuck-Zustand haelt state 7, state=%d", e->state);
    }

    /* (6) NO-FREEZE: 600 Frames; gueltige Zustaende + lebende Kraehen drehen den Loop */
    {
        uint32_t last_sig[RE15_ACTOR_MAX]; int trans[RE15_ACTOR_MAX];
        memset(trans, 0, sizeof trans);
        for (int i = 0; i < nc; i++)
            last_sig[i] = ((uint32_t)g_actors[cslots[i]].state << 8) | g_actors[cslots[i]].sub_state_1;
        for (int f = 0; f < 600; f++) {
            re15_actor_t *ref = NULL;
            for (int i = 0; i < nc; i++)
                if (g_actors[cslots[i]].state != 7) { ref = &g_actors[cslots[i]]; break; }
            if (ref) { pl->x = ref->x + ((f & 64) ? 700 : 2200); pl->z = ref->z; pl->motion = 100; }
            if (pl->hp < 30) { pl->hp = 100; pl->state = 0; pl->hit_react = 0; }
            frame();
            for (int i = 0; i < nc; i++) {
                re15_actor_t *e = &g_actors[cslots[i]];
                uint32_t sig = ((uint32_t)e->state << 8) | e->sub_state_1;
                if (sig != last_sig[i]) { trans[i]++; last_sig[i] = sig; }
                CHECK(e->state == 1 || e->state == 2 || e->state == 3 || e->state == 7,
                      "slot %d in ungueltigem State %d", cslots[i], e->state);
                if (e->state == 1)
                    CHECK(e->sub_state_1 <= 14, "slot %d ACTIVE sub %d > 14", cslots[i], e->sub_state_1);
            }
        }
        int live_moving = 0, live_total = 0;
        for (int i = 0; i < nc; i++) {
            re15_actor_t *e = &g_actors[cslots[i]];
            if (e->state != 7) { live_total++; if (trans[i] > 0) live_moving++; }
            printf("  [B RE2] slot %d: state=%d sub=%d transitions=%d\n",
                   cslots[i], e->state, e->sub_state_1, trans[i]);
        }
        if (live_total > 0)
            CHECK(live_moving >= 1, "lebende Kraehen muessen den Loop drehen (moving=%d live=%d)",
                  live_moving, live_total);
    }

    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    free(buf);
    if (fails == 0) printf("test_re2_room10c0_ab: OK\n");
    else            printf("test_re2_room10c0_ab: %d FAILURES\n", fails);
    return fails ? 1 : 0;
}
