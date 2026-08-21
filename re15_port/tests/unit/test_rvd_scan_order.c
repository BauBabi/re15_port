/* test_rvd_scan_order.c — the RVD camera-cut scan picks the FIRST table zone (audit rvd-scan-order).
 *
 * When two RVD CAM_SWITCH zones of the same cam_from overlap, byte-true FUN_80014230 selects the one
 * FIRST in RVD table order. The port installs RVD zones top-down (install#0 = lowest table index ->
 * highest slot) and scanned slots ASCENDING returning on the first inside match, so it picked the
 * LOWEST slot = the LAST table zone = the wrong cut (e.g. ROOM1190 dog cam_from=3: zone[14]->cut5 vs
 * zone[15]->cut4 overlap; PSX shows cut5, the port showed cut4). The fix records the highest-slot
 * inside match (= lowest table index) instead of returning on the first. Asserts the lower table
 * index (higher install slot) wins on an overlap, for either install order.
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_aot.h"
#include "re15_scd.h"
#include "re15_collision.h"   /* re15_collision_set_band — the CAM_SWITCH floor gate */

/* g_scd is the shared VM state; the scan reads cut_auto_enabled + writes cam_id/cam_change_pending. */
/* `active` = der AKTIVE (angeforderte) Cut. Er MUSS in g_scd.cam_id stehen: das ist
 * DAT_800afbb5, und genau den liest der Zonen-Scan (FUN_80014230 @0x8001423c
 * `lbu v0,-0x44b(v0)=>DAT_800afbb5` fuer den Gruppen-Eintrittstest und @0x800142c8 fuer
 * die Schleifen-Fortsetzung). Frueher stand hier der Sentinel 0xFF und der aktive Cut kam
 * nur als Parameter — das war ein Harness-Artefakt, das die byte-true Quelle verdeckte. */
static void arm_scan(uint8_t active)
{
    re15_aot_init();
    g_scd.cut_auto_enabled = 1;      /* Cut_auto(1): RVD scan active */
    g_scd.tick_count = 200;          /* past the frame-0 race gate */
    g_scd.cam_change_pending = 0;
    g_scd.cam_id = active;           /* DAT_800afbb5 */
    for (int i = 0; i < SCD_THREAD_COUNT; i++) g_scd.threads[i].active = 0;   /* scd_idle */
}

int main(void)
{
    int fail = 0;
    printf("=== RVD CAM_SWITCH scan picks the FIRST table zone on overlap (audit rvd-scan-order) ===\n");

    const uint8_t CAM_FROM = 3;
    const int32_t X = 1000, Z = 1000, HW = 600, HH = 600;   /* two zones overlapping around (1000,1000) */

    /* (1) ROOM1190 shape: install#0 (lower table index) = cut 5 at the HIGH slot (top-down), install#1
     *     = cut 4 at the next-lower slot. Both cover (X,Z). The scan must pick cut 5 (first in table). */
    {
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, /*cut=*/5);   /* install#0 -> slot 63 */
        re15_aot_set_cam_switch(62, X, Z, HW, HH, CAM_FROM, /*cut=*/4);   /* install#1 -> slot 62 */
        re15_aot_scan(X, Z, /*active_cut=*/CAM_FROM);
        if (!g_scd.cam_change_pending || g_scd.cam_id != 5) {
            fprintf(stderr, "FAIL(1): overlap must pick cut 5 (slot 63 = first table zone), got pending=%d cam_id=%u\n",
                    g_scd.cam_change_pending, g_scd.cam_id);
            fail = 1;
        } else printf("  (1) overlap picks cut 5 (highest slot = lowest table index), not cut 4\n");
    }

    /* (2) reversed slots (sanity: the RULE is highest-slot-wins, independent of which cut value):
     *     install#0 = cut 4 at slot 63, install#1 = cut 5 at slot 62 -> picks cut 4. */
    {
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, 4);
        re15_aot_set_cam_switch(62, X, Z, HW, HH, CAM_FROM, 5);
        re15_aot_scan(X, Z, CAM_FROM);
        if (!g_scd.cam_change_pending || g_scd.cam_id != 4) {
            fprintf(stderr, "FAIL(2): highest slot (63) must win regardless of cut value, got cam_id=%u\n", g_scd.cam_id);
            fail = 1;
        } else printf("  (2) highest slot wins regardless of cut value (cut 4 at slot 63)\n");
    }

    /* (3) cam_from filter: a zone whose cam_from != active_cut must NOT fire. */
    {
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, /*cam_from=*/9, 5);   /* wrong cam_from */
        re15_aot_scan(X, Z, /*active_cut=*/CAM_FROM);
        if (g_scd.cam_change_pending) { fprintf(stderr, "FAIL(3): cam_from!=active_cut must not fire (cam_id=%u)\n", g_scd.cam_id); fail = 1; }
        else printf("  (3) cam_from filter respected (mismatched zone does not fire)\n");
    }

    /* (4) non-overlap: player outside both zones -> no switch. */
    {
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, 5);
        re15_aot_scan(X + 5000, Z + 5000, CAM_FROM);
        if (g_scd.cam_change_pending) { fprintf(stderr, "FAIL(4): outside the zone must not fire\n"); fail = 1; }
        else printf("  (4) outside the zone does not fire\n");
    }

    /* (5) FLOOR gate: the RVD zone's floor byte (a->band, from entry+1) must match the player's
     *     collision band unless 0xFF. */
    {
        re15_collision_set_band(2);
        /* (5a) mismatched floor -> no fire */
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, 5);
        g_aot.slots[63].band = 4;                 /* zone floor 4, player on band 2 */
        re15_aot_scan(X, Z, CAM_FROM);
        if (g_scd.cam_change_pending) { fprintf(stderr, "FAIL(5a): wrong-floor zone must not fire (cam_id=%u)\n", g_scd.cam_id); fail = 1; }
        /* (5b) matching floor -> fire */
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, 5);
        g_aot.slots[63].band = 2;                 /* zone floor 2 == player band 2 */
        re15_aot_scan(X, Z, CAM_FROM);
        if (!g_scd.cam_change_pending || g_scd.cam_id != 5) { fprintf(stderr, "FAIL(5b): matching-floor zone must fire\n"); fail = 1; }
        /* (5c) floor 0xFF -> fires on any band */
        arm_scan(CAM_FROM);
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, 5);
        g_aot.slots[63].band = 0xFF;              /* any floor */
        re15_aot_scan(X, Z, CAM_FROM);
        if (!g_scd.cam_change_pending || g_scd.cam_id != 5) { fprintf(stderr, "FAIL(5c): floor 0xFF must fire on any band\n"); fail = 1; }
        re15_collision_set_band(-1);              /* reset */
        if (!fail) printf("  (5) floor gate: wrong floor blocked, matching floor + 0xFF fire\n");
    }

    if (fail) { printf("RVD-SCAN-ORDER: FAIL\n"); return 1; }
    printf("RVD-SCAN-ORDER: all checks passed\n");
    return 0;
}
