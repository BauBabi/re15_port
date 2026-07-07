/* test_aot_edge.c — #14 AOT edge-trigger removal (byte-true FUN_80042bac @0x80043018).
 *
 * The AUTO zone has no prev-frame-inside field: it fires EVERY frame the entity is inside, NOT just on
 * the entry EDGE. Re-trigger is stopped handler-side (ITEM self-disables in the scan pass; an
 * AUTO_EVENT's SCD sub calls Aot_reset). This test locks in the every-frame semantics — the old
 * `inside && !was_inside` edge would fail check (2). Asserts:
 *   (1) ITEM (type 2): fires on the first inside frame AND self-disables (a->active=0) = fires once.
 *   (2) AUTO_EVENT (type 6): fires on the 1st AND 2nd inside frame (every-frame, no edge gate).
 *   (3) outside: no fire.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"   /* g_room_change — the DOOR fire observable */

int main(void)
{
    int fail = 0;
    memset(&g_aot, 0, sizeof g_aot);
    memset(g_actors, 0, sizeof g_actors);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
    g_aot_action_pressed = 0;

    printf("=== #14 AOT edge-removal: AUTO zone fires EVERY frame inside, not just on entry ===\n");

    /* (1) ITEM (type 2): fires on inside + self-disables (a->active=0 = the Item_aot_reset equivalent) */
    re15_aot_set(0, RE15_AOT_TYPE_ITEM, 5, 1000, 1000, 500, 500);
    re15_aot_scan(1000, 1000, 0xFF);                 /* player inside the item rect */
    if (g_aot.slots[0].active != 0) {
        fprintf(stderr, "FAIL(1): ITEM must self-disable (active=0) after firing while inside\n"); fail = 1; }
    else printf("  (1) ITEM: fired on inside + self-disabled (active=0) = fires once\n");

    /* (2) AUTO_EVENT (type 6): fires EVERY frame inside. The OLD edge gated the 2nd scan (was_inside=1);
     * the byte-true no-edge scan fires again. (No self-disabling sub runs in this unit harness, so the
     * every-frame firing is directly observable.) */
    re15_aot_set(1, RE15_AOT_TYPE_AUTO_EVENT, 8, 2000, 2000, 500, 500);
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(2000, 2000, 0xFF);                 /* frame 1: inside */
    int fired1 = (g_aot.fired_event_id_this_frame == 8);
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(2000, 2000, 0xFF);                 /* frame 2: STILL inside (was_inside is now 1) */
    int fired2 = (g_aot.fired_event_id_this_frame == 8);
    if (!fired1) { fprintf(stderr, "FAIL(2): AUTO_EVENT must fire on the 1st inside frame\n"); fail = 1; }
    if (!fired2) { fprintf(stderr, "FAIL(2): AUTO_EVENT must fire EVERY frame inside (edge removed) — the 2nd scan did NOT fire\n"); fail = 1; }
    if (fired1 && fired2) printf("  (2) AUTO_EVENT: fired on BOTH the 1st and 2nd inside-frame (byte-true every-frame, no edge)\n");

    /* (3) outside -> no fire */
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(50000, 50000, 0xFF);
    if (g_aot.fired_event_id_this_frame != 0) {
        fprintf(stderr, "FAIL(3): AUTO_EVENT must NOT fire when the entity is outside\n"); fail = 1; }
    else printf("  (3) outside: no fire\n");

    /* (4) #14(c) generic BAND-GATE: a wrong-floor event AOT does not fire. */
    extern void re15_collision_set_band(int band);
    extern void re15_collision_reset_band(void);
    re15_aot_set(2, RE15_AOT_TYPE_AUTO_EVENT, 9, 3000, 3000, 500, 500);
    g_aot.slots[2].band = 4;                         /* AOT on floor band 4 */

    re15_collision_set_band(4);                      /* player on band 4 -> MATCH -> fires */
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(3000, 3000, 0xFF);
    if (g_aot.fired_event_id_this_frame != 9) { fprintf(stderr, "FAIL(4a): matching-band AUTO must fire\n"); fail = 1; }

    re15_collision_set_band(0);                      /* player on band 0 -> MISMATCH -> gated */
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(3000, 3000, 0xFF);
    if (g_aot.fired_event_id_this_frame != 0) { fprintf(stderr, "FAIL(4b): wrong-band AUTO must be GATED (fired=%d)\n", g_aot.fired_event_id_this_frame); fail = 1; }

    g_aot.slots[2].band = 0x80 | 4;                  /* 0x80 ignore-band -> fires regardless of floor */
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(3000, 3000, 0xFF);
    if (g_aot.fired_event_id_this_frame != 9) { fprintf(stderr, "FAIL(4c): 0x80 ignore-band AUTO must fire on any floor\n"); fail = 1; }

    g_aot.slots[2].band = 4;
    re15_collision_reset_band();                     /* band unknown (-1) -> ungated (pre-band room) */
    g_aot.fired_event_id_this_frame = 0;
    re15_aot_scan(3000, 3000, 0xFF);
    if (g_aot.fired_event_id_this_frame != 9) { fprintf(stderr, "FAIL(4d): band -1 (unknown) must NOT gate\n"); fail = 1; }
    if (!fail) printf("  (4) BAND-GATE: match fires, mismatch gated, 0x80 ignores, band -1 ungated\n");

    /* (5) DOOR 9-frame press-and-HOLD accumulator (byte-true FUN_8002bd44 obj+0x8C): a door opens
     * only after the action button is HELD for 9 consecutive in-reach frames — NOT on a single tap
     * edge (the old bug). The counter resets the instant the button is released. */
    {
        extern uint8_t g_scd_action_held;
        int door_fail = 0;
        memset(&g_room_change, 0, sizeof g_room_change);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 0; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 0;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0;          /* faces +X -> forward-reach at (563, 0) */
        re15_aot_set_door(3, 563, 0, 900, 900, /*cut*/1, /*spawn*/100, 0, 100, 0);
        re15_collision_reset_band();                         /* band unknown -> ungated */
        /* Observe the accumulator directly (this test door has no dest_room, so it never queues a
         * room change — the FIRE is observable as the DOOR handler resetting door_hold to 0). */
        g_aot_action_pressed = 0; g_scd_action_held = 1;     /* HOLD, not a tap */
        for (int f = 0; f < 8; f++) re15_aot_scan(0, 0, 0xFF);   /* 8 held frames -> counts to 8, no fire */
        if (g_aot.slots[3].door_hold != 8) {
            fprintf(stderr, "FAIL(5): door_hold must accumulate to 8 over 8 held frames (no fire yet), got %d\n",
                    g_aot.slots[3].door_hold); door_fail = 1; }
        re15_aot_scan(0, 0, 0xFF);                            /* the 9th held frame -> FIRE (handler resets to 0) */
        if (g_aot.slots[3].door_hold != 0) {
            fprintf(stderr, "FAIL(5): door must OPEN on the 9th consecutive held frame (door_hold should reset to 0, got %d)\n",
                    g_aot.slots[3].door_hold); door_fail = 1; }
        /* release-then-hold: the counter resets to 0 the moment the button is not held */
        g_scd_action_held = 1; for (int f = 0; f < 3; f++) re15_aot_scan(0, 0, 0xFF);   /* re-accumulate to 3 */
        g_scd_action_held = 0; re15_aot_scan(0, 0, 0xFF);                                /* release */
        if (g_aot.slots[3].door_hold != 0) {
            fprintf(stderr, "FAIL(5): releasing the action button must reset door_hold to 0, got %d\n",
                    g_aot.slots[3].door_hold); door_fail = 1; }
        if (door_fail) fail = 1;
        else printf("  (5) DOOR HOLD: 8 held frames = no open, opens on the 9th, resets on release (byte-true)\n");
    }

    if (fail) { printf("AOT-EDGE: FAIL\n"); return 1; }
    printf("AOT-EDGE: all checks passed\n");
    return 0;
}
