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

/* g_scd is the shared VM state; the scan reads cut_auto_enabled + writes cam_id/cam_change_pending. */
static void arm_scan(void)
{
    re15_aot_init();
    g_scd.cut_auto_enabled = 1;      /* Cut_auto(1): RVD scan active */
    g_scd.tick_count = 200;          /* past the frame-0 race gate */
    g_scd.cam_change_pending = 0;
    g_scd.cam_id = 0xFF;
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
        arm_scan();
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
        arm_scan();
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
        arm_scan();
        re15_aot_set_cam_switch(63, X, Z, HW, HH, /*cam_from=*/9, 5);   /* wrong cam_from */
        re15_aot_scan(X, Z, /*active_cut=*/CAM_FROM);
        if (g_scd.cam_change_pending) { fprintf(stderr, "FAIL(3): cam_from!=active_cut must not fire (cam_id=%u)\n", g_scd.cam_id); fail = 1; }
        else printf("  (3) cam_from filter respected (mismatched zone does not fire)\n");
    }

    /* (4) non-overlap: player outside both zones -> no switch. */
    {
        arm_scan();
        re15_aot_set_cam_switch(63, X, Z, HW, HH, CAM_FROM, 5);
        re15_aot_scan(X + 5000, Z + 5000, CAM_FROM);
        if (g_scd.cam_change_pending) { fprintf(stderr, "FAIL(4): outside the zone must not fire\n"); fail = 1; }
        else printf("  (4) outside the zone does not fire\n");
    }

    if (fail) { printf("RVD-SCAN-ORDER: FAIL\n"); return 1; }
    printf("RVD-SCAN-ORDER: all checks passed\n");
    return 0;
}
