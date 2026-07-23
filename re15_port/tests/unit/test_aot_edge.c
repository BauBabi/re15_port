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
#include "re15_room.h"
#include "re15_inventory.h"   /* g_room_change — the DOOR fire observable */
#include "re15_item_modal.h"  /* item pickup starts the deferred zoom/flip modal (FUN_8001db28) */

int main(void)
{
    int fail = 0;
    memset(&g_aot, 0, sizeof g_aot);
    memset(g_actors, 0, sizeof g_actors);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
    g_aot_action_pressed = 0;

    printf("=== #14 AOT edge-removal: AUTO zone fires EVERY frame inside, not just on entry ===\n");

    /* (1) ITEM: ACTION-gated (wf_f536e1ee step 5 — all 162 shipped items carry flags bit 0x10 SET;
     * the old walk-in level trigger was divergence #1). Walk-in does NOT take it. The action press
     * inside STARTS the deferred pickup MODAL (byte-true FUN_8001db28) — the grant + AOT self-disable
     * happen at the modal's END (gated on inventory-room; a FULL inventory shrinks away and LEAVES the
     * item in the world, @0x8001e0ec). The FSM itself is covered by test_item_modal; here we check the
     * AOT-scan wiring. */
    re15_aot_set_item(0, 1000, 1000, 500, 500, 0x15, 1);
    re15_inv_init();
    g_aot_action_pressed = 0;
    re15_aot_scan(1000, 1000, 0xFF);                 /* inside, NO press -> stays, no modal */
    if (g_aot.slots[0].active != 1 || re15_item_modal_active()) {
        fprintf(stderr, "FAIL(1a): ITEM must NOT fire on walk-in (action-gated)\n"); fail = 1; }

    for (int fs = 0; fs < RE15_INV_MAX_SLOTS; fs++)  /* fill the inventory */
        re15_inv_grant((uint8_t)(0x40 + fs), 1);
    g_aot_action_pressed = 1;
    re15_aot_scan(1000, 1000, 0xFF);                 /* press, FULL -> modal STARTS */
    g_aot_action_pressed = 0;
    if (!re15_item_modal_active()) {
        fprintf(stderr, "FAIL(1b): ACTION press must START the pickup modal\n"); fail = 1; }
    while (re15_item_modal_active()) re15_item_modal_tick(0x4000);   /* virtual confirm dismisses the can't-carry box */
    if (g_aot.slots[0].active != 1) {
        fprintf(stderr, "FAIL(1b): a FULL inventory must keep the item AOT alive (item stays)\n"); fail = 1; }

    re15_inv_init();                                  /* make room */
    g_aot_action_pressed = 1;
    re15_aot_scan(1000, 1000, 0xFF);                 /* press inside -> modal STARTS */
    g_aot_action_pressed = 0;
    while (re15_item_modal_active()) re15_item_modal_tick(0x4000);   /* confirm(Yes) -> grant + self-disable */
    if (g_aot.slots[0].active != 0) {
        fprintf(stderr, "FAIL(1c): the modal must self-disable the item AOT on completion\n"); fail = 1; }
    {
        int got = 0;
        for (int s = 0; s < RE15_INV_MAX_SLOTS; s++) if (g_inv.slots[s].id == 0x15) got = 1;
        if (!got) { fprintf(stderr, "FAIL(1c): the modal must GRANT the item on completion\n"); fail = 1; }
    }
    if (!fail) printf("  (1) ITEM: action-gated pickup MODAL (walk-in no, full leaves item, press grants+disables)\n");

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

    /* (5) DOOR fires on a SQUARE press-EDGE (byte-true FUN_80042bac ACTION scan, kind=0x10 ->
     * sce-2 handler @0x800430bc; audit wf_adabbc59 + DuckStation ground truth 2026-07-23):
     * a single g_aot_action_pressed with the forward-620 point inside the door rect queues the
     * room change. NO hold, NO walk-in. (The old 9-frame-hold model was mis-attributed to the
     * BOX-PUSH updater FUN_8002bd44 — a different function.) */
    {
        int door_fail = 0;
        memset(&g_room_change, 0, sizeof g_room_change);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 0; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 0;
        g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0;          /* faces +X -> forward-reach point at (620, 0) */
        re15_aot_set_door(3, 620, 0, 900, 900, /*cut*/1, /*spawn*/100, 0, 100, 0);
        g_aot.door_params[3].dest_stage = 0;
        g_aot.door_params[3].dest_room  = 1;                 /* a valid dest so the fire queues a change */
        re15_collision_reset_band();                         /* band unknown -> ungated */
        /* in reach, NO press -> must NOT fire (walking in / just standing does nothing) */
        g_aot_action_pressed = 0;
        re15_aot_scan(0, 0, 0xFF);
        if (g_room_change.pending) {
            fprintf(stderr, "FAIL(5): door must NOT open without a SQUARE press\n"); door_fail = 1; }
        /* in reach, SQUARE press-edge -> fires IMMEDIATELY (single frame, no hold) */
        g_aot_action_pressed = 1;
        re15_aot_scan(0, 0, 0xFF);
        if (!g_room_change.pending) {
            fprintf(stderr, "FAIL(5): door must OPEN on a single SQUARE press-edge while in reach\n"); door_fail = 1; }
        /* out of reach (forward-620 point far from the door rect) + press -> no fire */
        memset(&g_room_change, 0, sizeof g_room_change);
        g_actors[RE15_ACTOR_SLOT_PLAYER].x = 50000; g_actors[RE15_ACTOR_SLOT_PLAYER].z = 50000;
        g_aot_action_pressed = 1;
        re15_aot_scan(50000, 50000, 0xFF);
        if (g_room_change.pending) {
            fprintf(stderr, "FAIL(5): door must NOT open when the forward point is outside its rect\n"); door_fail = 1; }
        if (door_fail) fail = 1;
        else printf("  (5) DOOR: SQUARE press-edge in reach opens; no press / out of reach = no open (byte-true)\n");
    }

    if (fail) { printf("AOT-EDGE: FAIL\n"); return 1; }
    printf("AOT-EDGE: all checks passed\n");
    return 0;
}
