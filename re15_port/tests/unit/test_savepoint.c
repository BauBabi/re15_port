/* test_savepoint.c — FE-4 phone save-point registry (the 14 telephone locations)
 * + the pending one-shot signal. */
#include <stdio.h>
#include "re15_savepoint.h"

int main(void)
{
    int fail = 0;
    printf("=== FE-4 phone save-point registry ===\n");

    /* the RE-verified phone save-points (room, main<NN> msg id) must match */
    struct { unsigned room; unsigned char msg; } yes[] = {
        {0x1070,0x14},{0x1071,0x14},{0x1120,0x06},{0x1121,0x06},{0x1150,0x01},{0x1151,0x01},
        {0x2010,0x03},{0x2011,0x03},{0x30A0,0x01},{0x30A1,0x01},{0x30B0,0x01},{0x30B1,0x01},
        {0x4010,0x2B},{0x4011,0x07},
    };
    for (unsigned i = 0; i < sizeof(yes)/sizeof(yes[0]); i++) {
        if (!re15_savepoint_is(yes[i].room, yes[i].msg)) {
            fprintf(stderr, "FAIL: room %04x msg %02x should be a save-point\n", yes[i].room, yes[i].msg);
            fail = 1;
        }
    }
    /* non-save-points must NOT match: wrong msg in a save room, and non-save rooms */
    if (re15_savepoint_is(0x1150, 0x02)) { fprintf(stderr, "FAIL: 1150/02 not a save-point\n"); fail = 1; }
    if (re15_savepoint_is(0x1140, 0x01)) { fprintf(stderr, "FAIL: 1140/01 (briefing) not a save-point\n"); fail = 1; }
    if (re15_savepoint_is(0x1240, 0x01)) { fprintf(stderr, "FAIL: 1240/01 (intro) not a save-point\n"); fail = 1; }
    if (!fail) printf("  14 phone locations match, negatives rejected\n");

    /* the pending one-shot signal */
    re15_savepoint_set_pending(0);
    if (re15_savepoint_pending()) { fprintf(stderr, "FAIL: pending should start 0\n"); fail = 1; }
    re15_savepoint_set_pending(1);
    if (!re15_savepoint_pending()) { fprintf(stderr, "FAIL: pending should be 1\n"); fail = 1; }
    re15_savepoint_set_pending(0);
    if (re15_savepoint_pending()) { fprintf(stderr, "FAIL: pending should clear\n"); fail = 1; }
    if (!fail) printf("  pending one-shot signal ok\n");

    /* Ortsnamen-Index (Patch-Uebernahme, re15_savepoint.h): Telefon ROOM1070/1071 -> 1
     * (AOT_TYPE1_HOOK @0x8007087c), Schreibmaschine ROOM1150/1151 -> 0 (SCD_SAVE_RET
     * @0x800708c0), alle PATCH-UNDEFINIERTEN + unbekannte Raeume -> 0, reset() wischt. */
    re15_savepoint_latch_loc(0x1070);
    if (re15_savepoint_loc() != 1) { fprintf(stderr, "FAIL: 1070 loc != 1\n"); fail = 1; }
    re15_savepoint_latch_loc(0x1071);
    if (re15_savepoint_loc() != 1) { fprintf(stderr, "FAIL: 1071 loc != 1 (Port-Entscheidung Elza-Spiegel)\n"); fail = 1; }
    re15_savepoint_latch_loc(0x1150);
    if (re15_savepoint_loc() != 0) { fprintf(stderr, "FAIL: 1150 loc != 0\n"); fail = 1; }
    re15_savepoint_latch_loc(0x1120);
    if (re15_savepoint_loc() != 2) { fprintf(stderr, "FAIL: 1120 loc != 2 (West Staircase 1F, PORT-S)\n"); fail = 1; }
    re15_savepoint_latch_loc(0x2010);
    if (re15_savepoint_loc() != 3) { fprintf(stderr, "FAIL: 2010 loc != 3 (PORT-L Listen-Reihenfolge)\n"); fail = 1; }
    re15_savepoint_latch_loc(0x5011);
    if (re15_savepoint_loc() != 7) { fprintf(stderr, "FAIL: 5011 loc != 7 (Monitor Room)\n"); fail = 1; }
    re15_savepoint_latch_loc(0x1070);
    re15_savepoint_latch_loc(0x1140);   /* kein Save-Raum -> 0 */
    if (re15_savepoint_loc() != 0) { fprintf(stderr, "FAIL: Nicht-Save-Raum loc != 0\n"); fail = 1; }
    re15_savepoint_latch_loc(0x1070);
    re15_savepoint_reset();
    if (re15_savepoint_loc() != 0) { fprintf(stderr, "FAIL: reset wischt loc nicht\n"); fail = 1; }
    if (!fail) printf("  loc latch (Patch-Indizes 0/1) ok\n");

    if (fail) { printf("SAVEPOINT: FAIL\n"); return 1; }
    printf("SAVEPOINT: all checks passed\n");
    return 0;
}
