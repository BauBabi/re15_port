/**
 * @file test_room1150_itembox.c
 * @brief Wave-6 closure (shots/inv_plan.md gap 18) — the MZD item box is a MESSAGE-ONLY
 *        examine AOT, parity-verified against the real ROOM1150.RDT bytes.
 *
 * SHIPPED MZD BEHAVIOUR (RDT ground truth, re-dumped this session from
 * re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT):
 *
 *   - main00 (RDT+0xD5E, main_scd table @RDT+0x40 -> 0xD5C) installs the box AOT at
 *     record offset main00+0x70 (file offset 0xDCE):
 *       2C 05 03 31 00 00  F4 9D  C8 CE  40 06  E8 03  FF 00  18 07 00 00
 *     = Aot_set slot=5, sce=3, flags=0x31 (bit 0x10 SET -> ACTION scan, bit 0x20 ->
 *     forward-620 test, pool 1), band pc[4]=0, rect NW=(-25100,-12600) extent
 *     (1600,1000) -> centre (-24300,-12100) half (800,500), act pc[14]=0xFF,
 *     eventId pc[17]=0x07 -> fires sub_scd[7]. (Same "18 NN" event tail as the
 *     proven slot-6 Irons cutscene AOT -> sub08, re15_aot.h.)
 *
 *   - sub07 (sub_scd table @RDT+0x44 -> 0xEA0, entry 7 off 0x258 -> file 0x10F8),
 *     14 bytes:  29 08 | 2B 03 FF FF | 02 00 | 2A 00 | 3C 01 | 01 00
 *     = Cut_chg(8) ; Message_on(msg 3, 0xFF, 0xFF) ; Evt_next ; Cut_old ; Nop ;
 *       Cut_auto(1) ; Evt_end.  NO item grant, NO door, NO box storage UI --
 *       the box only swings the camera to cut 8 and prints room message 3.
 *
 *   - message 3 (msg table @RDT+0x3C -> 0x12F8; off[3]=0x12E -> body @file 0x1426,
 *     44 bytes): 04 02 25 50 41 49 3E 4B 54 00 45 4F 00 4A 4B 50 00 3D 52 3D 45
 *                48 3D 3E 48 41 00 45 4A 08 50 44 45 4F 00 4C 4E 41 52 45 41 53
 *                57 01
 *     .msg glyph decode (char = code + 0x24, 0x00 = space, 0x08 = line break,
 *     0x01 = end):  "Itembox is not available in\nthis preview."
 *     (ROOM1150 msg 1 is the separate PHONE save-point text -- re15_savepoint.c
 *     maps {0x1150, msg 0x01}; msg 3 must NOT trigger the save flow.)
 *
 * The test loads the real RDT, brings the room up through its own SCD (main00+sub00,
 * exactly like test_room1140_combat), then fires the box examine the same way
 * game_step_common.c:496 does (action press -> re15_aot_scan -> scd_event_fire) and
 * asserts:
 *   (1) INSTALL  — slot 5 is a GENERIC action AOT, event 7, byte-true rect/flags/band.
 *   (2) MESSAGE  — sub07 raises room message id 3 (g_scd.message_active/message_id),
 *                  plain non-blocking dialog (no YES/NO query), and the camera does the
 *                  Cut_chg(8) -> Cut_old round trip ending with Cut_auto(1).
 *   (3) NOTHING ELSE — no inventory mutation, no pickup modal, no room change, no
 *                  save-point flow, and the AOT stays active (re-examinable).
 *   (4) RDT BYTES — the msg-3 body in the parsed message block == the 44 bytes above.
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_savepoint.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RE15_ASSET_PSX_DIR
#define RE15_ASSET_PSX_DIR "shared_assets/PSX"
#endif

/* scd_vm.c export (declared the same way scd_room_setup.c:72 does — no header). */
void scd_register_current_rdt(const re15_rdt_t *rdt);

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

/* msg 3 body @ROOM1150.RDT file offset 0x1426 (msg block 0x12F8 + off[3]=0x12E),
 * "Itembox is not available in\nthis preview." — see the header comment. */
static const uint8_t k_msg3[] = {
    0x04, 0x02, 0x25, 0x50, 0x41, 0x49, 0x3E, 0x4B, 0x54, 0x00, 0x45, 0x4F,
    0x00, 0x4A, 0x4B, 0x50, 0x00, 0x3D, 0x52, 0x3D, 0x45, 0x48, 0x3D, 0x3E,
    0x48, 0x41, 0x00, 0x45, 0x4A, 0x08, 0x50, 0x44, 0x45, 0x4F, 0x00, 0x4C,
    0x4E, 0x41, 0x52, 0x45, 0x41, 0x53, 0x57, 0x01
};

int main(void)
{
    const char *path = RE15_ASSET_PSX_DIR "/STAGE1/ROOM1150.RDT";
    long sz = 0;
    uint8_t *buf = slurp(path, &sz);
    if (!buf) { fprintf(stderr, "FAIL: cannot open %s\n", path); return 1; }

    re15_rdt_t rdt;
    if (re15_rdt_parse(buf, (size_t)sz, &rdt) != 0) {
        fprintf(stderr, "FAIL: RDT parse of %s\n", path); free(buf); return 1;
    }
    if (!rdt.main_scd || !rdt.sub_scd[0] || !rdt.sub_scd[7]) {
        fprintf(stderr, "FAIL: ROOM1150 missing main00/sub00/sub07\n"); free(buf); return 1;
    }

    int fail = 0;
    printf("=== ROOM1150 item box = message-only AOT (inv_plan gap 18, wave 6) ===\n");

    /* (4) first — the RDT message bytes, independent of any VM state. */
    if (!rdt.messages || rdt.messages_size <= 0) {
        fprintf(stderr, "FAIL(4): no message block parsed\n"); fail = 1;
    } else {
        uint16_t off3 = (uint16_t)(rdt.messages[3 * 2] | (rdt.messages[3 * 2 + 1] << 8));
        if (off3 != 0x12E) {
            fprintf(stderr, "FAIL(4): msg off[3]=0x%x, expected 0x12E\n", off3); fail = 1;
        } else if ((int)(off3 + sizeof k_msg3) > rdt.messages_size ||
                   memcmp(rdt.messages + off3, k_msg3, sizeof k_msg3) != 0) {
            fprintf(stderr, "FAIL(4): msg-3 body != RDT bytes @0x1426\n"); fail = 1;
        } else {
            printf("  (4) msg 3 body == 44 RDT bytes @0x1426 (\"Itembox is not available "
                   "in this preview.\")\n");
        }
    }

    /* Room bring-up: same minimal pattern as test_room1140_combat. */
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_inv_init();
    re15_savepoint_reset();
    memset(&g_room_change, 0, sizeof g_room_change);
    g_current_room_id = 0x1150;
    scd_register_current_rdt(&rdt);

    scd_thread_start(0, rdt.main_scd);
    scd_thread_start(1, rdt.sub_scd[0]);
    for (int i = 0; i < 120; i++) scd_vm_tick();

    /* (1) the installed box AOT — slot 5, byte-true record fields. */
    {
        const re15_aot_t *a = &g_aot.slots[5];
        if (!a->active || a->type != RE15_AOT_TYPE_GENERIC || a->event_id != 7) {
            fprintf(stderr, "FAIL(1): slot5 active=%d type=%d ev=%d (want GENERIC ev 7)\n",
                    a->active, a->type, a->event_id); fail = 1;
        }
        if (a->sce_flags != 0x31 || a->band != 0) {
            fprintf(stderr, "FAIL(1): slot5 flags=0x%02x band=%d (want 0x31 / 0)\n",
                    a->sce_flags, a->band); fail = 1;
        }
        if (a->x != -24300 || a->z != -12100 || a->half_w != 800 || a->half_h != 500) {
            fprintf(stderr, "FAIL(1): slot5 rect centre=(%d,%d) half=(%d,%d) "
                    "(want (-24300,-12100) (800,500))\n",
                    (int)a->x, (int)a->z, (int)a->half_w, (int)a->half_h); fail = 1;
        }
        if (!fail) printf("  (1) slot5 == Aot_set @main00+0x70: GENERIC ev=7 flags=0x31 "
                          "band=0 rect(-24300,-12100)+/-(800,500)\n");
    }

    /* Snapshot the observables that must NOT change. */
    re15_inventory_t inv_before = g_inv;
    uint8_t cut_before = g_scd.cam_id;

    /* (2) fire the examine exactly like the game loop: the player stands 620 units
     * short of the rect centre facing +X (rot_y=0: forward point = pos + (620*cos)>>12
     * on X, aot_common.c:434-441 — flags 0x31 has NO centre test, forward only),
     * presses the action button, the scan fires GENERIC -> scd_event_fire(7)
     * (game_step_common.c:496-497). */
    {
        extern void re15_collision_set_band(int band);
        extern uint8_t g_aot_action_pressed;
        re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
        pl->active = 1;
        pl->x = -24300 - 620; pl->z = -12100; pl->rot_y = 0;
        re15_collision_set_band(0);                 /* AOT band pc[4] = 0 */

        g_aot.fired_event_id_this_frame = 0;
        g_aot_action_pressed = 1;
        re15_aot_scan(pl->x, pl->z, 0xFF);
        g_aot_action_pressed = 0;

        if (g_aot.fired_event_id_this_frame != 7) {
            fprintf(stderr, "FAIL(2): examine did not fire event 7 (got %d)\n",
                    g_aot.fired_event_id_this_frame); fail = 1;
        } else if (scd_event_fire(7) < 0) {
            fprintf(stderr, "FAIL(2): scd_event_fire(7) found no free slot\n"); fail = 1;
        } else {
            /* sub07 tick-by-tick: 29 08 (Cut_chg 8) + 2B 03 (Message_on 3) run on the
             * first tick, then 02 00 (Evt_next) yields; 2A 00 (Cut_old) + 3C 01
             * (Cut_auto 1) + 01 (Evt_end) complete on the next. Track the transient. */
            int saw_cut8 = 0;
            for (int i = 0; i < 8; i++) {
                scd_vm_tick();
                if (g_scd.cam_id == 8) saw_cut8 = 1;
            }
            if (!g_scd.message_active || g_scd.message_id != 3) {
                fprintf(stderr, "FAIL(2): message active=%d id=%d (want active id=3)\n",
                        g_scd.message_active, g_scd.message_id); fail = 1;
            }
            if (g_scd.message_query != 0) {
                fprintf(stderr, "FAIL(2): plain message must not be a YES/NO query\n");
                fail = 1;
            }
            if (!saw_cut8) {
                fprintf(stderr, "FAIL(2): Cut_chg(8) close-up never observed\n"); fail = 1;
            }
            if (g_scd.cam_id != cut_before) {
                fprintf(stderr, "FAIL(2): Cut_old did not restore the cut (cam=%d was %d)\n",
                        g_scd.cam_id, cut_before); fail = 1;
            }
            if (!g_scd.cut_auto_enabled) {
                fprintf(stderr, "FAIL(2): Cut_auto(1) tail must re-enable the auto scan\n");
                fail = 1;
            }
            if (!fail) printf("  (2) sub07: Cut_chg(8) -> Message_on(3) plain -> Cut_old "
                              "-> Cut_auto(1) (message id 3 live)\n");
        }
    }

    /* (3) the box opens NOTHING else. */
    {
        if (memcmp(&inv_before, &g_inv, sizeof g_inv) != 0) {
            fprintf(stderr, "FAIL(3): inventory changed on box examine\n"); fail = 1;
        }
        if (re15_item_modal_active()) {
            fprintf(stderr, "FAIL(3): pickup modal started on box examine\n"); fail = 1;
        }
        if (g_room_change.pending) {
            fprintf(stderr, "FAIL(3): room change queued on box examine\n"); fail = 1;
        }
        if (re15_savepoint_pending()) {
            fprintf(stderr, "FAIL(3): save flow triggered (msg 3 is NOT the 1150 phone "
                    "save-point msg 1)\n"); fail = 1;
        }
        if (!g_aot.slots[5].active) {
            fprintf(stderr, "FAIL(3): box AOT must stay active (re-examinable)\n"); fail = 1;
        }
        if (!fail) printf("  (3) no grant, no modal, no room change, no save flow; AOT "
                          "stays active\n");
    }

    free(buf);
    if (fail) { printf("RESULT: FAIL\n"); return 1; }
    printf("RESULT: OK\n");
    return 0;
}
