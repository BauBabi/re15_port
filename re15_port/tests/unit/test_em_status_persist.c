/* test_em_status_persist.c — the byte-true enemy-status kill-flag persistence (audit SCEEM-SPAWN-GATE).
 *
 * Sce_em_set (0x44) @0x800420a0: pc[7] is the em-status kill-flag index. The spawn is SUPPRESSED when
 * that flag is already set in the stage's em-status zone (SCD flag zone 7 stage<3 / 8 stage>=3 =
 * PSX 0x800b1038/0x800b1058); pc[7]==0xFF bypasses. pc[7] is latched onto the actor (+0x1C6). On the
 * death-commit (state -> CORPSE) the enemy sets flag[em_flag_id] (re15_enemy_ai_run_all pass, byte-true
 * FUN_80109554/FUN_80106edc), so re-entering the room does NOT respawn a killed enemy. g_game.flags
 * persists across room re-entry (only cleared at scd_vm_init / new game). Asserts:
 *   (1) a preset kill flag suppresses the spawn; (2) pc[7]==0xFF always spawns; (3) spawn latches
 *   em_flag_id; (4) run_all sets the flag once a spawned enemy is a corpse; (5) after that, re-running
 *   the spawn (actors reset, flags kept) does NOT respawn it; (6) the stage->zone mapping.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_enemy_ai.h"
#include "re15_room.h"

#define OP_EVT_NEXT 0x02
#define ACTOR_OF(slot) ((int)(slot) + 1)   /* SCRIPT_SLOT_TO_ACTOR */

static void make_emset(uint8_t *bc /*[21]*/, uint8_t slot, uint8_t type, uint8_t persist)
{
    memset(bc, 0, 21);
    bc[0] = SCD_OP_SCE_EM_SET; bc[1] = slot; bc[2] = type; bc[3] = 0x02 /*behavior*/;
    bc[7] = persist;                 /* pc[7] = the em-status kill index */
    bc[20] = OP_EVT_NEXT;            /* the 0x44 handler advances +20, then this parks the thread */
}

/* full-room bring-up (clears flags) + spawn; returns the resulting actor .active. */
static int fresh_spawn(uint8_t slot, uint8_t type, uint8_t persist, int preset_idx, unsigned room)
{
    re15_actor_init(); re15_aot_init(); scd_vm_init();   /* scd_vm_init CLEARS g_game.flags */
    g_current_room_id = room;
    if (preset_idx >= 0) re15_game_flag_set(re15_em_status_zone(), (uint8_t)preset_idx, 1);
    uint8_t bc[21]; make_emset(bc, slot, type, persist);
    scd_thread_start(1, bc);
    scd_vm_tick();
    return g_actors[ACTOR_OF(slot)].active;
}

/* room RE-ENTRY: reset actors only (keep g_game.flags), spawn again — the respawn path. */
static int reenter_spawn(uint8_t slot, uint8_t type, uint8_t persist, unsigned room)
{
    re15_actor_init();                   /* clears actors; g_game.flags UNTOUCHED */
    g_current_room_id = room;
    uint8_t bc[21]; make_emset(bc, slot, type, persist);
    scd_thread_start(1, bc);
    scd_vm_tick();
    return g_actors[ACTOR_OF(slot)].active;
}

int main(void)
{
    int fail = 0;
    printf("=== enemy-status kill-flag persistence (byte-true FUN_800420a0 gate + death SET) ===\n");

    /* (1) preset flag -> spawn suppressed. */
    if (fresh_spawn(0, 0x10, /*persist*/5, /*preset*/5, 0x1140) != 0) {
        fprintf(stderr, "FAIL(1): a set kill flag must suppress the spawn\n"); fail = 1; }
    else printf("  (1) preset kill flag suppresses the spawn\n");

    /* (2) pc[7]==0xFF bypasses the gate (spawns even with bits set). */
    if (fresh_spawn(0, 0x10, /*persist*/0xFF, /*preset*/5, 0x1140) != 1) {
        fprintf(stderr, "FAIL(2): pc[7]==0xFF must always spawn\n"); fail = 1; }
    else if (g_actors[1].em_flag_id != 0xFF) {
        fprintf(stderr, "FAIL(2b): 0xFF must be latched as never-persist, got %#x\n", g_actors[1].em_flag_id); fail = 1; }
    else printf("  (2) pc[7]==0xFF always spawns (never-persist latched)\n");

    /* (3) a normal spawn latches em_flag_id = pc[7]. */
    if (fresh_spawn(0, 0x10, /*persist*/7, /*preset*/-1, 0x1140) != 1 || g_actors[1].em_flag_id != 7) {
        fprintf(stderr, "FAIL(3): spawn must latch em_flag_id=7, got active=%d id=%#x\n", g_actors[1].active, g_actors[1].em_flag_id); fail = 1; }
    else printf("  (3) spawn latches em_flag_id = pc[7]\n");

    /* (4) run_all sets the flag once the spawned enemy is a corpse (the death-commit persist). */
    g_actors[1].state = (uint8_t)RE15_AI_STATE_CORPSE;    /* the enemy just died */
    re15_enemy_ai_run_all(0);
    if (re15_game_flag_get(7, 7) != 1) {
        fprintf(stderr, "FAIL(4): a corpse with em_flag_id=7 must set flag[7,7]\n"); fail = 1; }
    else printf("  (4) run_all sets the kill flag for the corpse (zone 7 idx 7)\n");

    /* (5) RE-ENTRY (actors reset, flags kept) does NOT respawn the killed enemy. */
    if (reenter_spawn(0, 0x10, /*persist*/7, 0x1140) != 0) {
        fprintf(stderr, "FAIL(5): killed enemy (flag[7,7] set) must NOT respawn on re-entry\n"); fail = 1; }
    else printf("  (5) killed enemy does not respawn on room re-entry (flag persisted)\n");

    /* (6) stage->zone: STAGE1 rooms -> zone 7, STAGE4+ -> zone 8. */
    {
        g_current_room_id = 0x1140; uint8_t z1 = re15_em_status_zone();   /* stage0=0 */
        g_current_room_id = 0x3091; uint8_t z3 = re15_em_status_zone();   /* stage0=2 */
        g_current_room_id = 0x4010; uint8_t z4 = re15_em_status_zone();   /* stage0=3 */
        if (z1 != 7 || z3 != 7 || z4 != 8) {
            fprintf(stderr, "FAIL(6): zone map wrong (0x1140->%d 0x3091->%d 0x4010->%d, want 7/7/8)\n", z1, z3, z4); fail = 1; }
        else printf("  (6) stage->zone: STAGE1-3 -> 7, STAGE4-6 -> 8\n");
    }

    if (fail) { printf("EM-STATUS-PERSIST: FAIL\n"); return 1; }
    printf("EM-STATUS-PERSIST: all checks passed\n");
    return 0;
}
