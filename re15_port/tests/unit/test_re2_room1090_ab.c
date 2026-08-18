/**
 * @file test_re2_room1090_ab.c
 * @brief WELLE E — RE2-Spinnen-Brain A/B im ECHTEN Spinnen-Raum (Skill re15-room-probe).
 *
 * ROOM1090 ist der RE1.5-Spinnenraum (Memory reai-v2-stage1-enemy-rooms). Der Test faehrt
 * denselben deterministischen Spieler-Pfad einmal unter dem RE1.5-Default und einmal unter
 * RE2 durch re15_enemy_ai_run_all + re15_actors_anim_advance und prueft harte, aus dem
 * RE2-Modul EMS25.BIN zitierte Eigenschaften.
 *
 * RE2-Behauptungen (Belege in enemy_ai_re2_spider.c, alle selbst aus EMS25.BIN disassembliert):
 *   (1) INIT @0x801001EC: nach einem Frame state=1/sub=0 (`sw 1,4(s2)` @0x80100214),
 *       Clip 1 (@0x8010035C), HP aus @0x801062C4 + (rand&3) = 89..122, +0x222 = 0 (Boden).
 *   (2) Die Spinne BEWEGT sich (FUN_800152C8 @0x80100A58) und wechselt Substates —
 *       kein Einfrieren ueber 600 Frames.
 *   (3) Der Biss kostet den Spieler GENAU 20 HP (`addiu a0,zero,20` @0x80105A78,
 *       `jal 0x800401d4` @0x80105A7C) — Reichweite 1500 / dY 4300 (@0x80100EE4-E8).
 *   (4) Ein Kill endet in CORPSE (state 7) mit Clip 13 (@0x80104D58-60) und HP=1
 *       (@0x80103EB0), und der Zustand HAELT.
 *   (5) Die RE2-EM025-Bank aus CDEMD0.EMS (Sektor 1871) parst.
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

static int bringup(const re15_rdt_t *rdt, int slots[], int *n)
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
    *n = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++)
        if (g_actors[s].active && g_actors[s].type == 0x25) slots[(*n)++] = s;
    return (*n >= 1) ? 0 : -1;
}

/* Kandidatenraeume: der RE1.5-Adult-Spider ist laut Projektdoku ein STAGE2-Gegner
 * (enemy_ai_common.c "ADULT SPIDER (type 0x25, EM025, STAGE2)"); ROOM1090 ist der
 * STAGE1-Spinnenraum. Der Test nimmt den ERSTEN Raum, dessen SCD wirklich eine lebende
 * Typ-0x25-Entity aufstellt — und schlaegt FEHL, wenn keiner es tut (kein vacuous PASS). */
static const char *k_rooms[] = {
    "/STAGE2/ROOM2000.RDT", "/STAGE2/ROOM2010.RDT", "/STAGE2/ROOM2020.RDT",
    "/STAGE2/ROOM2040.RDT", "/STAGE2/ROOM2050.RDT", "/STAGE2/ROOM2060.RDT",
    "/STAGE1/ROOM1090.RDT", "/STAGE1/ROOM10D0.RDT", "/STAGE1/ROOM1260.RDT",
    "/STAGE3/ROOM3010.RDT", NULL
};

int main(void)
{
    long sz = 0;
    uint8_t *buf = NULL;
    re15_rdt_t rdt;
    int slots[RE15_ACTOR_MAX], n = 0;
    const char *picked = NULL;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];

    /* =================== A: RE1.5-Baseline (Default-Flavor, unveraendert) =================== */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE15);
    for (int r = 0; k_rooms[r] && !picked; r++) {
        char p2[512];
        snprintf(p2, sizeof p2, "%s%s", RE15_ASSET_PSX_DIR, k_rooms[r]);
        uint8_t *b = slurp(p2, &sz);
        if (!b) continue;
        re15_rdt_t t;
        if (re15_rdt_parse(b, (size_t)sz, &t) == 0 && t.sub_scd[0] &&
            bringup(&t, slots, &n) == 0) { buf = b; rdt = t; picked = k_rooms[r]; break; }
        free(b);
    }
    if (!picked) {
        printf("FAIL: kein Kandidatenraum stellt eine lebende Typ-0x25-Spinne auf\n");
        return 1;
    }
    printf("  [A RE1.5] %s: %d Adult-Spinnen gespawnt\n", picked, n);
    {
        re15_actor_t *s0 = &g_actors[slots[0]];
        int32_t x0 = s0->x, z0 = s0->z;
        int moved = 0;
        for (int f = 0; f < 400; f++) {
            pl->x = s0->x + 1200; pl->z = s0->z; pl->motion = 100;
            frame();
            if (s0->x != x0 || s0->z != z0) moved = 1;
        }
        printf("  [A RE1.5] state=%u sub=%u hp=%d moved=%d\n",
               s0->state, s0->sub_state_1, (int)s0->hp, moved);
    }

    /* ============================ B: RE2-Flavor, gleicher Pfad ============================= */
    re15_ai_flavor_set(RE15_AI_FLAVOR_RE2);
    CHECK(bringup(&rdt, slots, &n) == 0, "RE2: >=1 Spinne erwartet, %d", n);

    /* (5) Die ECHTE RE2-EM025-Bank aus CDEMD0.EMS (TOC @0x8009adf4: kind 0x25 EMD =
     *     Sektor 1871 / 0xD0C8 B — selbst nachgerechnet). */
    {
        static uint8_t *ems = NULL; static long ems_sz = 0;
        if (!ems) ems = slurp(RE15_ASSET_PSX_DIR "/../RE2/CDEMD0.EMS", &ems_sz);
        CHECK(ems != NULL, "shared_assets/RE2/CDEMD0.EMS muss fuer den B-Lauf lesbar sein");
        if (ems) {
            re15_enemy_bank_t *eb = re15_enemy_find(0x25);
            if (!eb) eb = re15_enemy_alloc(0x25);
            int rc = eb ? re2_ems_load_bank(ems, (size_t)ems_sz, 0x25, eb, NULL) : -1;
            CHECK(rc == 0, "RE2 EM025-Bank muss parsen (TOC-Sektor 1871), rc=%d", rc);
            if (eb && rc == 0) {
                eb->buf = NULL; eb->ok = 1;
                printf("  [B RE2] EM025: %d Clips, %d Knochen, %d Meshes\n",
                       eb->anim.clip_count, eb->skel.bone_count, eb->md1.mesh_count);
                CHECK(eb->anim.clip_count >= 14,
                      "EM025 braucht >= 14 Clips (das Modul benutzt bis Clip 14 @0x80101B20), n=%d",
                      eb->anim.clip_count);
            }
        }
    }
    frame();

    /* (1) INIT @0x801001EC */
    {
        int owned = 0;
        for (int i = 0; i < n; i++) {
            re15_actor_t *s = &g_actors[slots[i]];
            if (!re15_re2spider_owns(s)) continue;         /* Decken-/Wandspawn: RE1.5-Brain */
            owned++;
            CHECK(s->state == 1 && s->sub_state_1 == 0,
                  "INIT -> Zustandswort 1 @0x80100214 (slot %d: state %u sub %u)",
                  slots[i], s->state, s->sub_state_1);
            CHECK(s->hp >= 89 && s->hp <= 122,
                  "INIT-HP aus @0x801062C4 + (rand&3) = 89..122 (slot %d: %d)", slots[i], (int)s->hp);
            CHECK(s->re2s_mode222 == 0, "Boden-Spawn -> +0x222 = 0 @0x80100460 (slot %d)", slots[i]);
        }
        CHECK(owned >= 1,
              "der B-Lauf MUSS mindestens eine vom RE2-Brain besessene Boden-Spinne haben "
              "(sonst waere alles hier vacuous), owned=%d von %d", owned, n);
        printf("  [B RE2] %d von %d Spinnen laufen auf dem RE2-Brain\n", owned, n);
    }

    /* (2) Bewegung + Substate-Wechsel, kein Freeze */
    {
        re15_actor_t *s0 = NULL;
        for (int i = 0; i < n && !s0; i++)
            if (re15_re2spider_owns(&g_actors[slots[i]])) s0 = &g_actors[slots[i]];
        CHECK(s0 != NULL, "eine RE2-Boden-Spinne fuer den Lauf-Test");
        if (s0) {
            int32_t x0 = s0->x, z0 = s0->z;
            int moved = 0, subs = 0; uint8_t last = s0->sub_state_1;
            for (int f = 0; f < 600; f++) {
                pl->x = s0->x + 6000; pl->z = s0->z; pl->motion = 100;   /* ausserhalb 1500 */
                frame();
                if (s0->x != x0 || s0->z != z0) moved = 1;
                if (s0->sub_state_1 != last) { subs++; last = s0->sub_state_1; }
            }
            printf("  [B RE2] moved=%d substate-Wechsel=%d state=%u sub=%u\n",
                   moved, subs, s0->state, s0->sub_state_1);
            CHECK(moved, "die RE2-Spinne MUSS sich bewegen (FUN_800152C8 @0x80100A58)");
            CHECK(subs >= 2, "die RE2-Spinne MUSS Substates wechseln (kein Freeze), n=%d", subs);
            CHECK(s0->state == 1, "sie bleibt in ACTIVE (state 1), ist %u", s0->state);
        }
    }

    /* (3) BISS: 20 HP (`addiu a0,zero,20` @0x80105A78) */
    {
        re15_actor_t *s0 = NULL;
        for (int i = 0; i < n && !s0; i++)
            if (re15_re2spider_owns(&g_actors[slots[i]])) s0 = &g_actors[slots[i]];
        if (s0) {
            pl->hp = 100; pl->hit_react = 0;
            /* Die Angriffsleiter in Sub 0 (@0x8010086C-80) verlangt 1001 <= dist < 6000 UND
             * a128 == 0 (Ausrichtung, @0x80100874-78) — bei 400 Einheiten waehlt das Original
             * NIE Sub 7. Der Spieler steht deshalb im Fenster und laesst die Spinne anlaufen. */
            int hp0 = pl->hp, bit = -1;
            for (int f = 0; f < 1500 && pl->hp == hp0; f++) {
                pl->y = s0->y; pl->motion = 100;
                if (s0->ai_dist > 2200u) { pl->x = s0->x + 2000; pl->z = s0->z; }
                frame();
                if (pl->hp != hp0) bit = f;
            }
            printf("  [B RE2] Biss bei Frame %d, HP %d -> %d\n", bit, hp0, (int)pl->hp);
            CHECK(bit >= 0, "die RE2-Spinne MUSS im Nahbereich beissen (FUN_80105AE0 @0x80100EE4)");
            CHECK(pl->hp == hp0 - 20,
                  "der Biss kostet GENAU 20 HP (@0x80105A78), war %d -> %d", hp0, (int)pl->hp);
        }
    }

    /* (4) KILL -> CORPSE (state 7), Clip 13, HP = 1 */
    {
        re15_actor_t *s0 = NULL;
        for (int i = 0; i < n && !s0; i++)
            if (re15_re2spider_owns(&g_actors[slots[i]])) s0 = &g_actors[slots[i]];
        if (s0) {
            s0->state = 3; s0->sub_state_1 = 1; s0->sub_state_2 = 0; s0->hp = 0;
            /* DEATH/CORPSE direkt ueber den Brain-Einstieg fahren: re15_enemy_ai_run_all
             * filtert tote Aktoren VOR dem Typ-Dispatch heraus (RE1.5-Verhalten), der
             * RE2-Root laeuft im Original aber in JEDEM Zustand (Tabelle @0x80106420[3]). */
            int reached = 0, dslot = (int)(s0 - &g_actors[0]);
            for (int f = 0; f < 600; f++) {
                re15_re2spider_tick(dslot); re15_actors_anim_advance();
                if (s0->state == 7) { reached = 1; break; }
            }
            CHECK(reached, "DEATH muss CORPSE erreichen (@0x80103E98-9C), state=%u", s0->state);
            CHECK(s0->hp == 1, "CORPSE-Revive HP = 1 @0x80103EB0, ist %d", (int)s0->hp);
            int clip13 = 0;
            for (int f = 0; f < 400; f++) {
                re15_re2spider_tick(dslot); re15_actors_anim_advance();
                if (s0->motion == 13) clip13 = 1;
            }
            CHECK(s0->state == 7, "CORPSE HAELT (state 7), ist %u", s0->state);
            CHECK(clip13, "CORPSE spielt Clip 13 @0x80104D58-60");
            printf("  [B RE2] CORPSE ok (clip13=%d, +0x225=%u)\n", clip13, s0->re2s_gs225);
        }
    }

    free(buf);
    printf(fails ? "test_re2_room1090_ab: %d FAIL\n" : "test_re2_room1090_ab: OK\n", fails);
    return fails ? 1 : 0;
}
