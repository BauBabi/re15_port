/* test_scd_dir_set.c — the byte-true Dir_set (0x33) opcode (audit DIRSET-0x33, LAB_80041080).
 *
 * Dir_set writes ALL THREE rotation axes (rot_x=pc[2], rot_y=pc[4], rot_z=pc[6], LE s16) to the WORK
 * ENTITY (thread+0x154, Work_set-selected) at +0x68/+0x6a/+0x6c; it NEVER reads pc[1]. The port had
 * read pc[1] as a 'register', gated the write on pc[1]==0, hardcoded the PLAYER, and decoded only
 * rot_y — so `Work_set(prop); Dir_set 33 00 00 00 00 00 00 02` (ROOM11F0 sub[17], rotate props to
 * rot_z=0x200) set the PLAYER's rot_y=0 ten times and left every prop unrotated. Asserts:
 *   (1) the ROOM11F0 form writes rot_z=0x200 to the WORK actor (not the player); rot_x/rot_y=0.
 *   (2) pc[1] is ignored: a nonzero pc[1] still applies (the old reg!=0 short-circuit is gone).
 *   (3) the player is NOT touched when the work entity is a non-player actor.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"

#define TEST_SLOT 0
#define OP_EVT_NEXT 0x02

/* start the block, select `ws` as the work entity, tick once. */
static void run_dir_set(const uint8_t *bc, int ws)
{
    scd_vm_init();
    re15_aot_init();
    memset(g_actors, 0, sizeof g_actors);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0x111;   /* sentinel: must stay for the non-player cases */
    g_actors[ws].active = 1;
    g_actors[ws].rot_x = g_actors[ws].rot_y = g_actors[ws].rot_z = 0x777;   /* sentinel */
    scd_thread_start(TEST_SLOT, bc);
    g_scd.work_slot = (int8_t)ws;     /* thread work_slot is -1 -> op_dir_set falls back to g_scd.work_slot */
    scd_vm_tick();
}

int main(void)
{
    int fail = 0;
    printf("=== Dir_set (0x33) writes 3 axes to the WORK entity, ignores pc[1] (LAB_80041080) ===\n");

    /* (1) ROOM11F0 form: 33 00 00 00 00 00 00 02 -> rot_x=0, rot_y=0, rot_z=0x200, work entity = actor 3. */
    {
        uint8_t bc[] = { 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, OP_EVT_NEXT };
        run_dir_set(bc, 3);
        if (g_actors[3].rot_z != 0x200) { fprintf(stderr, "FAIL(1a): work actor rot_z must be 0x200, got %#x\n", (unsigned)(uint16_t)g_actors[3].rot_z); fail = 1; }
        if (g_actors[3].rot_x != 0 || g_actors[3].rot_y != 0) { fprintf(stderr, "FAIL(1b): rot_x/rot_y must be 0, got %#x/%#x\n", (unsigned)(uint16_t)g_actors[3].rot_x, (unsigned)(uint16_t)g_actors[3].rot_y); fail = 1; }
        if (g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y != 0x111) { fprintf(stderr, "FAIL(1c): PLAYER rot_y must be untouched (0x111), got %#x\n", (unsigned)(uint16_t)g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y); fail = 1; }
        if (!fail) printf("  (1) ROOM11F0 form: work actor rot_z=0x200, rot_x/y=0, player untouched\n");
    }

    /* (2) pc[1] is IGNORED: nonzero pc[1] must still apply (old code: reg!=0 -> whole handler no-op). */
    {
        uint8_t bc[] = { 0x33, 0x05, 0x00, 0x00, 0x34, 0x12, 0x00, 0x00, OP_EVT_NEXT };  /* rot_y=0x1234 */
        run_dir_set(bc, 2);
        if (g_actors[2].rot_y != 0x1234) { fprintf(stderr, "FAIL(2): nonzero pc[1] must NOT block the write; rot_y=%#x want 0x1234\n", (unsigned)(uint16_t)g_actors[2].rot_y); fail = 1; }
        else printf("  (2) pc[1]!=0 ignored: work actor rot_y still set to 0x1234\n");
    }

    /* (3) all three axes are distinct LE s16 fields. */
    {
        uint8_t bc[] = { 0x33, 0x00, 0x11, 0x02, 0x22, 0x04, 0x33, 0x06, OP_EVT_NEXT };  /* rx=0x211,ry=0x422,rz=0x633 */
        run_dir_set(bc, 4);
        if (g_actors[4].rot_x != 0x211 || g_actors[4].rot_y != 0x422 || g_actors[4].rot_z != 0x633) {
            fprintf(stderr, "FAIL(3): axes must be (0x211,0x422,0x633), got (%#x,%#x,%#x)\n",
                    (unsigned)(uint16_t)g_actors[4].rot_x, (unsigned)(uint16_t)g_actors[4].rot_y, (unsigned)(uint16_t)g_actors[4].rot_z);
            fail = 1;
        } else printf("  (3) rot_x=pc[2], rot_y=pc[4], rot_z=pc[6] all decoded (LE s16)\n");
    }

    if (fail) { printf("SCD-DIR-SET: FAIL\n"); return 1; }
    printf("SCD-DIR-SET: all checks passed\n");
    return 0;
}
