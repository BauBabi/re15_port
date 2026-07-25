/* ============================================================================
 *  Door lock (locked-door message vs open-door transition) — roadmap S1-4 / PROG-1.
 *
 *  RE finding (audit wf_8d83d72a): RE1.5 does NOT encode door locks in the
 *  Door_aot_set bytes the way RE2 does — across all 262 STAGE1 doors pc[26..31]
 *  (door_type/lock/locked/key) are ALL ZERO, and the RE1.5 door sce-handler
 *  @0x800430bc transitions UNCONDITIONALLY (reads no lock byte). So PROG-1's
 *  "Door_aot_set lock operands" premise is RE2-only — a STAGE1 no-op.
 *
 *  The ACTUAL STAGE1 lock is a flag-gated sce1<->sce2 AOT-install SWAP in the room
 *  SCD: inside If(Ck(flag))…Else…, the SAME slot at the IDENTICAL reach-rect installs
 *  either a sce=1 MESSAGE AOT (locked -> shows the room's "The door is locked." text
 *  via handler @0x80043084 -> show_message) OR a sce=2 real Door_aot_set (unlocked ->
 *  transition via @0x800430bc). STAGE1 ships 412 such sce=1 message AOTs across 55
 *  rooms (e.g. ROOM1170 slot5 = "It's locked from the other side.", ROOM1030 x12).
 *
 *  This is ALREADY implemented byte-true in the port (op_ck polarity; op_aot_set sce=1
 *  -> RE15_AOT_TYPE_MESSAGE -> re15_scd_show_message; sce=2 -> DOOR transition). This
 *  test PINS that behavioural contract so a future change can't silently turn a locked
 *  door into a warp (or vice-versa):
 *    (A) a MESSAGE AOT (the "locked" half) fires the message FSM and does NOT queue a
 *        room change on an action press in its reach;
 *    (B) a DOOR AOT (the "unlocked" half) at the same reach DOES queue the transition.
 * ==========================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "re15_aot.h"
#include "re15_scd.h"        /* g_scd.message_fsm_active / cut_auto_enabled / tick_count */
#include "re15_actor.h"
#include "re15_room.h"       /* g_room_change, g_current_room_id */
#include "re15_collision.h"  /* re15_collision_set_band — the door band gate */

/* Reach: the action scan tests a point 620 units ahead of the player's facing
 * (FUN_80042bac). Player at origin facing +X -> forward point = (620,0); put the AOT
 * rect centred there so a SQUARE press in reach fires it. */
static void arm(void)
{
    re15_aot_init();
    g_scd.tick_count = 200;                 /* past the frame-0 race gate */
    g_scd.cut_auto_enabled = 0;
    g_scd.player_mode = 0;                  /* not scripted -> in_cinematic=0 (AOTs may fire) */
    g_scd.letterbox_countdown = 0;
    g_scd.message_fsm_active = 0;
    g_scd.message_query = 0;                /* clear any message left open by a prior case */
    g_scd.message_display_frames = 0;       /* (msg_block gates the door action while a msg is up) */
    g_room_change.pending = 0;
    g_aot_action_pressed = 0;
    g_current_room_id = 0x1140;             /* so a cross-room dest differs from "current" */
    re15_collision_reset_band();            /* band = -1 -> the door band gate is skipped (pb<0), fires band-free */
    for (int i = 0; i < SCD_THREAD_COUNT; i++) g_scd.threads[i].active = 0;   /* scd_idle */
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    memset(pl, 0, sizeof *pl);
    pl->active = 1; pl->type = 0; pl->x = 0; pl->z = 0; pl->rot_y = 0; pl->hp = 100;
}

static void press_action_9f(void)
{
    for (int f = 0; f < 9; f++) { g_aot_action_pressed = 1; re15_aot_scan(0, 0, /*active_cut=*/0); }
    g_aot_action_pressed = 0;
}

int main(void)
{
    int fail = 0;
    printf("=== PROG-1 door lock: MESSAGE(locked) shows text + no warp; DOOR(open) warps ===\n");

    /* (A) the LOCKED half: a sce=1 MESSAGE AOT at the reach point. */
    arm();
    re15_aot_set(0, RE15_AOT_TYPE_MESSAGE, /*event_id=msg*/12, /*cx*/620, /*cz*/0, /*hw*/500, /*hh*/500);
    press_action_9f();
    if (!g_scd.message_fsm_active) { fprintf(stderr, "FAIL(A): locked door did not show its message\n"); fail = 1; }
    if (g_room_change.pending)     { fprintf(stderr, "FAIL(A): locked door WARPED (should refuse entry)\n"); fail = 1; }
    printf("  (A) locked (MESSAGE): message_fsm_active=%d room_change=%d\n",
           g_scd.message_fsm_active, g_room_change.pending);

    /* (B) the UNLOCKED half: the same slot/reach installed as a real DOOR (sce=2). */
    arm();
    re15_aot_set_door(0, /*cx*/620, /*cz*/0, /*hw*/500, /*hh*/500,
                      /*target_cut*/2, /*spawn*/-1200, 0, 3400, /*yaw*/2048);
    g_aot.door_params[0].dest_stage = 0;    /* STAGE1 */
    g_aot.door_params[0].dest_room  = 0x30; /* -> ROOM1300? any != current; just needs to queue */
    press_action_9f();
    if (!g_room_change.pending)     { fprintf(stderr, "FAIL(B): open door did not queue a transition\n"); fail = 1; }
    printf("  (B) open (DOOR):     room_change=%d -> ROOM%04X\n",
           g_room_change.pending, g_room_change.room_id);

    if (fail) { printf("DOOR LOCK: FAIL\n"); return 1; }
    printf("DOOR LOCK: locked door shows message + refuses entry; open door transitions (byte-true sce1<->sce2)\n");
    return 0;
}
