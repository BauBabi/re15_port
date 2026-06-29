/**
 * @file test_room1140_spawn.c
 * @brief Phase 8.2 — ROOM1140 briefing-zombie spawn (byte-true, data-driven).
 *
 * Verifies the port spawns the STAGE1 briefing-room zombies by loading the REAL
 * ROOM1140.RDT and running its SCD (main00 + sub00) through the engine's own VM —
 * end to end, no mocks. Correction to a former handover premise: the briefing
 * zombies are NOT created by an overlay-only primitive; sub00 issues them via the
 * standard Sce_em_set (0x44) opcode (RE'd byte-true 2026-06-29 from ROOM1140.RDT
 * sub_scd sub00 + the EXE spawn handler LAB_800420a0, ghidra1_V2.txt:152496-152869).
 *
 * The roster is gated by `Ck zone3 flag 0xd2`; scd_vm_init clears all game flags, so
 * the flag is CLEAR = the FIRST-VISIT / briefing branch (5 lying/feeding zombies):
 *   pc1 0 -> actor 1: type 0x16 @(-800,0,-20600)  yaw 1024  pose 0x13 lying
 *   pc1 1 -> actor 2: type 0x10 @(-1800,0,-19600) yaw 512   pose 0x27 feeding
 *   pc1 2 -> actor 3: type 0x10 @(-1800,0,-21600) yaw 3584  pose 0x27 feeding
 *   pc1 3 -> actor 4: type 0x11 @(200,0,-21600)   yaw 2560  pose 0x27 feeding
 *   pc1 4 -> actor 5: type 0x11 @(200,0,-19600)   yaw 1536  pose 0x27 feeding
 * (SCRIPT_SLOT_TO_ACTOR(s)=s+1, so pc[1] n -> actor slot n+1.)
 *
 * This is the byte-true verification of the spawn DATA flowing through the port's
 * existing op_sce_em_set. The per-frame AI tick / lunge are NOT wired (separate
 * integration step); this only asserts the spawn.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Absolute path to the in-repo PSX asset tree (CMake-injected); fallback = cwd-relative. */
#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

static uint8_t *slurp(const char *path, long *out_sz)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz <= 0) { fclose(f); return NULL; }
    uint8_t *b = (uint8_t *)malloc((size_t)sz);
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *out_sz = sz;
    return b;
}

/* Expected first-visit roster (IF-branch). grid = the Sce_em_set behavior byte pc[3]
 * (byte-true LAB_800420a0: behavior -> entity +0x9, state +0x4 = 0). 0x88 = lying
 * (pose sel 8 -> action 0x13), 0x86 = feeding (sel 6 -> action 0x27). */
typedef struct { uint8_t type; int32_t x, y, z; int16_t rot_y; uint8_t grid; uint8_t motion; } expect_t;
static const expect_t s_expect[5] = {
    { 0x16,  -800, 0, -20600, 1024, 0x88, 0x13 },
    { 0x10, -1800, 0, -19600,  512, 0x86, 0x27 },
    { 0x10, -1800, 0, -21600, 3584, 0x86, 0x27 },
    { 0x11,   200, 0, -21600, 2560, 0x86, 0x27 },
    { 0x11,   200, 0, -19600, 1536, 0x86, 0x27 },
};

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1140.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }

    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) {
        fprintf(stderr, "FAIL: RDT parse of %s\n", path); free(buf); return 1;
    }
    if (!rdt.sub_scd[0]) { fprintf(stderr, "FAIL: ROOM1140 has no sub00\n"); free(buf); return 1; }

    /* Mirror scd_room_reenter's minimal room bring-up: clear flags (Ck 0xd2 -> clear =
     * first-visit branch), start main00 (init) + sub00 (the room script that spawns),
     * scenario 0, then tick the VM so sub00 reaches its Sce_em_set opcodes. */
    re15_actor_init();      /* slot 0 = player */
    re15_aot_init();
    scd_vm_init();          /* re15_game_state_init clears flags + actors/aot/inventory */

    if (rdt.main_scd) scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    g_scd.work_vars[10] = 0;   /* entry scenario 0 (sub00 dispatch) */

    for (int i = 0; i < 120; i++) scd_vm_tick();

    printf("=== ROOM1140 briefing-zombie spawn ===\n");
    int n = 0;
    int t16 = 0, t10 = 0, t11 = 0, other = 0;
    for (int s = 1; s < RE15_ACTOR_MAX; s++) {
        re15_actor_t *e = &g_actors[s];
        if (!e->active) continue;
        n++;
        printf("  actor %d: type=0x%02X pos=(%d,%d,%d) rot_y=%d state=0x%02X grid=0x%02X motion=%d\n",
               s, e->type, e->x, e->y, e->z, e->rot_y, e->state, e->grid_id, e->motion);
        if      (e->type == 0x16) t16++;
        else if (e->type == 0x10) t10++;
        else if (e->type == 0x11) t11++;
        else other++;
    }
    free(buf);

    int fail = 0;
    if (n != 5) { fprintf(stderr, "FAIL: expected 5 briefing zombies, got %d\n", n); fail = 1; }
    if (t16 != 1 || t10 != 2 || t11 != 2 || other != 0) {
        fprintf(stderr, "FAIL: type multiset {0x16:1,0x10:2,0x11:2}, got {0x16:%d,0x10:%d,0x11:%d,other:%d}\n",
                t16, t10, t11, other); fail = 1;
    }
    /* Exact per-slot roster (actor slot s = pc[1] s-1, byte-true positions). */
    for (int k = 0; k < 5; k++) {
        re15_actor_t *e = &g_actors[k + 1];
        const expect_t *x = &s_expect[k];
        if (!e->active || e->type != x->type ||
            e->x != x->x || e->y != x->y || e->z != x->z || e->rot_y != x->rot_y) {
            fprintf(stderr, "FAIL: actor %d expected type 0x%02X @(%d,%d,%d) yaw %d; "
                    "got active=%d type 0x%02X @(%d,%d,%d) yaw %d\n",
                    k + 1, x->type, x->x, x->y, x->z, x->rot_y,
                    e->active, e->type, e->x, e->y, e->z, e->rot_y);
            fail = 1;
        }
        /* byte-true field mapping: behavior pc[3] -> grid_id (+0x9), state (+0x4) = 0,
         * spawn pose (motion) decoded from behavior. */
        if (e->grid_id != x->grid || e->state != 0 || e->motion != x->motion) {
            fprintf(stderr, "FAIL: actor %d expected grid=0x%02X state=0 motion=%d; "
                    "got grid=0x%02X state=0x%02X motion=%d\n",
                    k + 1, x->grid, x->motion, e->grid_id, e->state, e->motion);
            fail = 1;
        }
    }

    if (fail) { fprintf(stderr, "\nROOM1140 SPAWN TEST FAILED\n"); return 1; }
    printf("\nPASS: ROOM1140 spawns 5 briefing zombies (0x16/0x10x2/0x11x2) byte-true\n");
    return 0;
}
