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

    if (fail) { printf("AOT-EDGE: FAIL\n"); return 1; }
    printf("AOT-EDGE: all checks passed\n");
    return 0;
}
