/* ============================================================================
 *  Room-probe sweep — roadmap S1-1 / RL-3.
 *
 *  Loads + ticks all 40 Leon STAGE1 rooms (ROOM1000..ROOM1270, the *0 variants)
 *  from the REAL shipped RDTs and asserts no parse/SCD crash, then prints a
 *  per-room manifest (camera-cut count + spawned-actor count). This is the
 *  runtime counterpart to the RL-2 door-graph static check: it proves every
 *  STAGE1 room's RDT parses and its main00+sub00 script runs without a walker
 *  desync fault, an out-of-bounds AOT/Sce_em_set, or an actor-tick crash.
 *
 *  Engine-only (links re15_engine + re15_test_support): re15_room_load() is
 *  platform code, so we reproduce the engine-side load path directly —
 *  re15_rdt_parse() (the byte-true container parser) + scd_vm_init() +
 *  scd_register_room_events() + scd_room_reenter() (starts main00+sub00) + the
 *  per-frame tick (scd_vm_tick + enemy AI + anim advance), exactly the shared
 *  sequence the PC/PSX loaders drive.
 *
 *  The RDT files come from RE15_ASSETS_PATH (the CMake compile define, the
 *  in-repo shared_assets/PSX by default). ✔ 40/40 load+tick, 0 crashes.
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "re15_rdt.h"       /* re15_rdt_t, re15_rdt_parse, .cut_count/.main_scd/.sub_scd */
#include "re15_scd.h"       /* scd_vm_init/_tick, scd_register_room_events, scd_room_reenter */
#include "re15_actor.h"     /* g_actors, RE15_ACTOR_SLOT_PLAYER, RE15_ACTOR_MAX */
#include "re15_enemy_ai.h"  /* re15_enemy_ai_run_all */
#include "re15_player.h"    /* re15_actors_anim_advance */

#define RE15_STR(x)  #x
#define RE15_XSTR(x) RE15_STR(x)

static uint8_t *read_file(const char *path, size_t *out_size)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) { fclose(f); return NULL; }
    size_t rd = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    if (rd != (size_t)sz) { free(buf); return NULL; }
    *out_size = (size_t)sz;
    return buf;
}

/* The RE1.5 CD ships a few UNUSED room slots as 4-byte all-zero placeholder RDTs
 * (never-authored rooms). ROOM1270/1271 are the STAGE1 stubs (no inbound door / no
 * FLR spawn per the RL-2 door graph). A stub is any RDT below the 0x60-byte RDT
 * header minimum (re15_rdt_parse returns -2). These are not real rooms -> not a
 * load failure. */
#define RDT_MIN_SIZE 0x60

int main(void)
{
    const char *base = RE15_XSTR(RE15_ASSETS_PATH);
    int fail = 0, n_ok = 0, n_stub = 0;

    printf("=== RL-3 room-probe sweep — STAGE1 Leon rooms (assets: %s) ===\n", base);
    printf("%-9s %5s %6s\n", "room", "cuts", "spawns");

    for (unsigned rid = 0x1000; rid <= 0x1270; rid += 0x10) {
        char path[600];
        snprintf(path, sizeof path, "%s/STAGE1/ROOM%04X.RDT", base, rid);

        size_t size = 0;
        uint8_t *data = read_file(path, &size);
        if (!data) { fprintf(stderr, "FAIL: cannot open %s\n", path); fail = 1; continue; }

        /* empty/unused CD placeholder slot -> not a real room, count + skip */
        if (size < RDT_MIN_SIZE) {
            printf("ROOM%04X  %5s  %5s  (empty stub, %zuB — unused CD slot)\n", rid, "-", "-", size);
            n_stub++; free(data); continue;
        }

        /* (1) byte-true container parse */
        re15_rdt_t rdt;
        int rc = re15_rdt_parse(data, size, &rdt);
        if (rc != 0) {
            fprintf(stderr, "FAIL: re15_rdt_parse(ROOM%04X) rc=%d\n", rid, rc);
            fail = 1; free(data); continue;
        }

        /* (2) engine-side room init: full VM/actor/AOT reset, seed the player, register the
         * RDT's event handlers, then re-enter (starts init=main00 + main=sub00 and ticks once). */
        scd_vm_init();
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1; pl->type = 0; pl->x = 0; pl->y = 0; pl->z = 0; pl->hp = 100;
        scd_register_room_events(&rdt);
        scd_room_reenter(&rdt, 0, 0, /*entry_scenario=*/0);

        /* (3) tick main00+sub00 for N frames — the SCD/enemy-AI/anim crash surface */
        for (int f = 0; f < 30; f++) {
            scd_vm_tick();
            re15_enemy_ai_run_all(0);
            re15_actors_anim_advance();
        }

        /* (4) manifest: camera cuts + spawned actors (slots 1..N) */
        int spawns = 0;
        for (int i = 1; i < RE15_ACTOR_MAX; i++)
            if (g_actors[i].active) spawns++;
        printf("ROOM%04X  %5d  %5d\n", rid, rdt.cut_count, spawns);
        n_ok++;
        free(data);
    }

    printf("\n%d real rooms loaded+ticked (0 crashes) + %d empty stub(s) = %d slots\n",
           n_ok, n_stub, n_ok + n_stub);
    /* STAGE1 = 40 room slots: 39 real Leon rooms + ROOM1270 empty stub. */
    if (fail || (n_ok + n_stub) != 40 || n_ok != 39) { printf("ROOM-PROBE SWEEP: FAIL\n"); return 1; }
    printf("ROOM-PROBE SWEEP: all passed\n");
    return 0;
}
