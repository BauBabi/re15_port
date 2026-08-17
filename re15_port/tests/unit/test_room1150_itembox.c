/**
 * @file test_room1150_itembox.c
 * @brief The ROOM1150 item-box AOT — SPLIT test (ITEM BOX campaign, itembox_spec.md §6):
 *        DEFAULT = the box AOT OPENS THE BOX (the RE1.5-hybrid, default-on like the
 *        functional save system): the shipped flavor Message_on(3) is suppressed at the
 *        SCD level and the box-screen pending signal is raised instead.
 *        RE15_BOX_PREVIEW_MSG=1 = the byte-true SHIPPED behavior (message-only AOT),
 *        parity-verified against the real ROOM1150.RDT bytes exactly as before.
 *
 * SHIPPED MZD BEHAVIOUR (RDT ground truth, re-dumped from
 * re15_port/shared_assets/PSX/STAGE1/ROOM1150.RDT):
 *
 *   - main00 (RDT+0xD5E, main_scd table @RDT+0x40 -> 0xD5C) installs the box AOT at
 *     record offset main00+0x70 (file offset 0xDCE):
 *       2C 05 03 31 00 00  F4 9D  C8 CE  40 06  E8 03  FF 00  18 07 00 00
 *     = Aot_set slot=5, sce=3, flags=0x31, band 0, centre (-24300,-12100) half
 *     (800,500), act 0xFF, eventId 0x07 -> fires sub_scd[7].
 *
 *   - sub07 (sub_scd table @RDT+0x44 -> 0xEA0, entry 7 off 0x258 -> file 0x10F8),
 *     14 bytes:  29 08 | 2B 03 FF FF | 02 00 | 2A 00 | 3C 01 | 01 00
 *     = Cut_chg(8) ; Message_on(msg 3) ; Evt_next ; Cut_old ; Nop ; Cut_auto(1) ;
 *       Evt_end.  NO item grant, NO door — the shipped build only swings the camera
 *       and prints room message 3 ("Itembox is not available in this preview.").
 *
 *   - The box campaign intercepts EXACTLY the Message_on: (0x1150, msg 3) is in the
 *     re15_itembox_is registry (16 rooms game-wide, scanned like the savepoint set),
 *     so the default fires re15_itembox_set_pending(1) and skips the message; the
 *     camera ops still run byte-true. (ROOM1150 msg 1 is the separate PHONE
 *     save-point — msg 3 must never trigger the save flow.)
 */
#include "re15_rdt.h"
#include "re15_scd.h"
#include "re15_actor.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_inventory.h"
#include "re15_item_modal.h"
#include "re15_savepoint.h"
#include "re15_itembox.h"
#include "re15_msg.h"

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

/* Fire the box examine exactly like the game loop (action press -> re15_aot_scan
 * -> scd_event_fire; game_step_common.c:496-497) and run the sub to completion. */
static int fire_examine(int *saw_cut8)
{
    extern uint8_t g_aot_action_pressed;
    re15_actor_t *pl = &g_actors[RE15_ACTOR_SLOT_PLAYER];
    pl->active = 1;
    pl->x = -24300 - 620; pl->z = -12100; pl->rot_y = 0;

    g_aot.fired_event_id_this_frame = 0;
    g_aot_action_pressed = 1;
    re15_aot_scan(pl->x, pl->z, 0xFF);
    g_aot_action_pressed = 0;

    if (g_aot.fired_event_id_this_frame != 7) return -1;
    if (scd_event_fire(7) < 0) return -2;
    *saw_cut8 = 0;
    for (int i = 0; i < 8; i++) {
        scd_vm_tick();
        if (g_scd.cam_id == 8) *saw_cut8 = 1;
    }
    return 0;
}

/* MESSAGE-PUMP — der Dialog-FSM laeuft im Original TOP-LEVEL und UNGEGATET
 * (FUN_800280b4 @0x800280c8-dc, Caller @0x80010044): der Text tickt und laesst sich
 * wegdruecken, waehrend Spieler/AI/Skript stehen. Braucht dieser Test seit dem globalen
 * Text-Freeze, weil ROOM1150 sub07 `Message_on(3)` mit Maske 0xffff traegt (Census
 * 2026-08-17: sub @0x10fa `2b 03 ff ff`) und die VM danach bis zum Dismiss steht
 * (SCD-Gate @0x8003f04c). */
static void msg_pump(void)
{
    extern uint16_t g_scd_pad_edge;
    extern uint16_t g_scd_pad_held;
    for (int i = 0; i < 600 && g_scd.message_active; i++) {
        g_scd_pad_held = 0x4000;   /* HELD virt. 0x4000 = Fast-Forward @0x80028214 */
        g_scd_pad_edge = 0x4000;   /* Press-Edge = Confirm/Dismiss @0x80028570/@0x80028698 */
        re15_msg_tick(0, 0, 0);
    }
    g_scd_pad_held = 0;
    g_scd_pad_edge = 0;
}

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
    printf("=== ROOM1150 box AOT: DEFAULT box-open + flagged shipped-message ===\n");

    /* (4) the RDT message bytes, independent of any VM state. */
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

    /* (0) registry identity: (0x1150, msg 3) is a box point; the phone msg 1 is NOT. */
    if (!re15_itembox_is(0x1150, 3) || re15_itembox_is(0x1150, 1)) {
        fprintf(stderr, "FAIL(0): registry — (1150,3) must be a box point, (1150,1) not\n");
        fail = 1;
    } else printf("  (0) box registry: (0x1150, msg 3) yes / (0x1150, msg 1 = phone) no\n");

    /* Room bring-up: same minimal pattern as test_room1140_combat. */
    re15_actor_init();
    re15_aot_init();
    scd_vm_init();
    re15_inv_init();
    re15_savepoint_reset();
    re15_itembox_reset();
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
            fprintf(stderr, "FAIL(1): slot5 rect centre=(%d,%d) half=(%d,%d)\n",
                    (int)a->x, (int)a->z, (int)a->half_w, (int)a->half_h); fail = 1;
        }
        if (!fail) printf("  (1) slot5 == Aot_set @main00+0x70: GENERIC ev=7 flags=0x31 "
                          "band=0 rect(-24300,-12100)+/-(800,500)\n");
    }

    re15_inventory_t inv_before = g_inv;
    uint8_t cut_before = g_scd.cam_id;

    /* ---- (2) DEFAULT: the box examine OPENS THE BOX (message suppressed) ---- */
    {
        extern void re15_collision_set_band(int band);
        re15_collision_set_band(0);
        int saw_cut8 = 0;
        int r = fire_examine(&saw_cut8);
        if (r != 0) { fprintf(stderr, "FAIL(2): examine fire rc=%d\n", r); fail = 1; }
        if (!re15_itembox_pending()) {
            fprintf(stderr, "FAIL(2): DEFAULT must raise the box-screen pending signal\n");
            fail = 1;
        }
        if (g_scd.message_active) {
            fprintf(stderr, "FAIL(2): DEFAULT must SUPPRESS the flavor message (got id %d)\n",
                    g_scd.message_id); fail = 1;
        }
        if (!saw_cut8 || g_scd.cam_id != cut_before || !g_scd.cut_auto_enabled) {
            fprintf(stderr, "FAIL(2): the sub07 camera ops must still run byte-true "
                    "(cut8=%d cam=%d auto=%d)\n", saw_cut8, g_scd.cam_id,
                    g_scd.cut_auto_enabled); fail = 1;
        }
        if (re15_savepoint_pending()) {
            fprintf(stderr, "FAIL(2): msg 3 must NOT trigger the phone save flow\n");
            fail = 1;
        }
        if (!fail) printf("  (2) DEFAULT: box pending raised, message suppressed, "
                          "Cut_chg(8)->Cut_old->Cut_auto(1) intact\n");
        re15_itembox_set_pending(0);
    }

    /* (3) the examine mutates NOTHING else. */
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
        if (!g_aot.slots[5].active) {
            fprintf(stderr, "FAIL(3): box AOT must stay active (re-examinable)\n"); fail = 1;
        }
        if (!fail) printf("  (3) no grant, no modal, no room change; AOT stays active\n");
    }

    /* ---- (5) RE15_BOX_PREVIEW_MSG=1: the byte-true SHIPPED message behavior ---- */
    {
        static char envflag[] = "RE15_BOX_PREVIEW_MSG=1";
        putenv(envflag);
        int saw_cut8 = 0;
        int r = fire_examine(&saw_cut8);
        if (r != 0) { fprintf(stderr, "FAIL(5): re-examine rc=%d\n", r); fail = 1; }
        if (!g_scd.message_active || g_scd.message_id != 3) {
            fprintf(stderr, "FAIL(5): shipped mode: message active=%d id=%d (want id 3)\n",
                    g_scd.message_active, g_scd.message_id); fail = 1;
        }
        if (g_scd.message_query != 0) {
            fprintf(stderr, "FAIL(5): plain message must not be a YES/NO query\n"); fail = 1;
        }
        /* GLOBALER TEXT-FREEZE (byte-true, RE 2026-08-17): sub07 ist
         *   Cut_chg(8) ; Message_on(3, Maske 0xffff) ; Evt_next ; Cut_old ; Nop ; Cut_auto(1)
         * Das Message_on ver-ODERt 0xffff0000 in g_pauseflags (@0x80040508 `lhu a3,2(v0)` +
         * @0x8004051c `sll a3,16` -> @0x80027ed0), das SCD-Gate @0x8003f04c haelt die VM ab
         * dem Folgeframe an. Die Nahaufnahme (Cut 8) STEHT also, solange der Text laeuft —
         * genau dafuer ist der Freeze da. Cut_old/Cut_auto laufen erst NACH dem Dismiss
         * (Restore @0x800285a4/@0x800286cc/@0x8002871c). */
        if (!saw_cut8 || g_scd.cam_id != 8) {
            fprintf(stderr, "FAIL(5): close-up cut 8 must HOLD while the frozen text is up "
                            "(cam_id=%d)\n", g_scd.cam_id); fail = 1;
        }
        msg_pump();                       /* Text wegdruecken -> Snapshot-Restore */
        for (int i = 0; i < 8; i++) scd_vm_tick();
        if (g_scd.cam_id != cut_before || !g_scd.cut_auto_enabled) {
            fprintf(stderr, "FAIL(5): camera round trip after dismiss broken "
                            "(cam_id=%d want %d, auto=%d)\n",
                    g_scd.cam_id, cut_before, g_scd.cut_auto_enabled); fail = 1;
        }
        if (re15_itembox_pending()) {
            fprintf(stderr, "FAIL(5): shipped mode must NOT raise the box pending\n");
            fail = 1;
        }
        if (!fail) printf("  (5) RE15_BOX_PREVIEW_MSG=1: sub07 shows msg 3 byte-true "
                          "(Cut_chg(8) -> Message_on(3) -> Cut_old -> Cut_auto(1))\n");
    }

    free(buf);
    if (fail) { printf("RESULT: FAIL\n"); return 1; }
    printf("RESULT: OK\n");
    return 0;
}
