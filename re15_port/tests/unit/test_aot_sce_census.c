/**
 * @file test_aot_sce_census.c
 * @brief Byte-true AOT sce-dispatch semantics — the 4 census fixes (aot_sce_census
 *        d7376834, shots/aot_sce_census.md).
 *
 * Original mechanism (PSX.EXE):
 *   - Installers 0x2C/0x3B/0x50 store rec = pc+2 into DAT_800ac9b0[slot]
 *     (@0x80040580/0x80040608/0x800406d0); rec[0] = the sce byte.
 *   - The scan dispatches `jalr PTR_8007469c[rec[0]]` (@0x80042f7c-90); the 14-entry
 *     handler table starts @0x8007469c.
 *   - FIX 1  sce-0 = INERT: handler[0] @0x8004305C only restores the work-var latch;
 *            the ACTION scan skips sce-0 records outright (@0x80042f48-50).
 *   - FIX 2  Aot_reset 0x46 = FULL RETYPE @0x80040738: rec[0]=pc[2] (@0x80040764),
 *            rec[1]=(rec[1]&0x80)+pc[3] (@0x8004076c-78), payload +0/+2/+4 = pc[4..9]
 *            (@0x80040788-a8). rec[2] (band) NOT written.
 *   - FIX 3  Aot_on 0x47 = FIRE-NOW @0x800407bc: `jalr PTR_8007469c[rec[0]]`
 *            @0x8004082c with a0 = payload (@0x80040804/808) — no geometry test.
 *   - FIX 4  sce-5 markers: handler[5] @0x8004318C is a pure NOP; the effect is the
 *            scan's latch — centre hit -> work_vars[1]=slot (@0x80042ee4), forward hit
 *            -> work_vars[0]=slot (@0x80042f3c) — run per pool entity by the record's
 *            mask bits (objects = pool @0x800b3f98 stride 0x94, loop @0x80043768-b4;
 *            per-entity band gate @0x80042cac vs prop+0x82 = Obj_model_set pc[4]
 *            @0x8004096c-74). Nothing clears the work-var latch on exit.
 */

#include <stdio.h>
#include <string.h>
#include "re15_scd.h"
#include "re15_aot.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_item_modal.h"

static int g_pass = 0;
static int g_fail = 0;

#define TEST_ASSERT_EQ(name, expected, actual) do {                          \
    long long _e = (long long)(expected);                                    \
    long long _a = (long long)(actual);                                      \
    if (_e != _a) {                                                          \
        printf("  FAIL [%s]: expected %lld, got %lld (%s:%d)\n",             \
               name, _e, _a, __FILE__, __LINE__);                            \
        g_fail++;                                                            \
        return;                                                              \
    }                                                                        \
} while(0)

#define TEST_OK(name) do { printf("  PASS: %s\n", name); g_pass++; } while(0)

#define OP_EVT_NEXT 0x02

extern uint8_t g_scd_action_held;
extern unsigned g_current_room_id;

static void setup_vm(void)
{
    scd_vm_init();
    re15_aot_init();
    memset(&g_room_change, 0, sizeof(g_room_change));
    g_current_room_id = 0x1170;      /* an arbitrary current room for dest_id resolution */
    g_scd_action_held = 0;
}

static long run_one_opcode(const uint8_t *bytecode)
{
    g_scd.threads[0].active = 0;   /* free the slot — the Evt_next sentinel leaves the
                                    * previous opcode's thread yielded-ACTIVE */
    if (scd_thread_start(0, bytecode) != 0) return -1;
    scd_vm_tick();
    const scd_thread_t *th = &g_scd.threads[0];
    if (!th->active || th->pc == NULL) return -1;
    return (long)(th->pc - bytecode) - 1;
}

/* A 0x3B Door_aot_set: slot, sce, rect centre (1000,2000) half (500,500),
 * spawn (111,-222,333), yaw 100, dest stage 0 room 0x24 (-> ROOM1240), cut 7. */
static void build_door(uint8_t *bc, uint8_t slot, uint8_t sce)
{
    memset(bc, 0, 33);
    bc[0] = 0x3B; bc[1] = slot; bc[2] = sce; bc[3] = 0x31; bc[4] = 0x00;
    bc[6] = 0xF4; bc[7]  = 0x01;   /* rect_x  =  500 */
    bc[8] = 0xDC; bc[9]  = 0x05;   /* rect_z  = 1500 */
    bc[10] = 0xE8; bc[11] = 0x03;  /* rect_w  = 1000 */
    bc[12] = 0xE8; bc[13] = 0x03;  /* rect_d  = 1000 */
    bc[14] = 0x6F; bc[15] = 0x00;  /* next_pos_x = 111 */
    bc[16] = 0x22; bc[17] = 0xFF;  /* next_pos_y = -222 */
    bc[18] = 0x4D; bc[19] = 0x01;  /* next_pos_z = 333 */
    bc[20] = 0x64; bc[21] = 0x00;  /* next_cdir_y = 100 */
    bc[22] = 0x00;                 /* dest stage (0-based -> STAGE1) */
    bc[23] = 0x24;                 /* dest room 0x24 -> dest_id 0x1240 */
    bc[24] = 0x07;                 /* target cut */
    bc[32] = OP_EVT_NEXT;
}

/* Drive the door-fire path: player stands IN FRONT of the door rect [500..1500]x[1500..2500]
 * (centre 1000,2000 half 500) facing +X, so the byte-true forward-620 point (x+620) lands
 * INSIDE the rect; a SQUARE press-edge (g_aot_action_pressed) fires the door in ONE scan
 * (FUN_80042bac ACTION scan -> sce-2 handler @0x800430bc — NOT the old box-push 9-hold). */
static void scan_door_frames(int frames)
{
    extern uint8_t g_aot_action_pressed;
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = 600;    /* forward-620 -> (1220,2000), inside the rect */
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = 2000;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y = 0;
    g_aot_action_pressed = 1;
    for (int f = 0; f < frames; f++)
        re15_aot_scan(600, 2000, 0xFF);
    g_aot_action_pressed = 0;
}

/* ===========================================================================
 * FIX 1: sce-0 install = INERT + retype-to-door arms it (census: 37 sce-0 doors,
 * 16 armed by Aot_reset sce=2, 21 permanently dead e.g. ROOM1250 slots 0-2).
 * =========================================================================== */
static void test_sce0_inert_and_retype_to_door(void)
{
    uint8_t bc[33];
    setup_vm();
    build_door(bc, 3, 0x00);                     /* sce-0 door (ROOM1140 slot 1 shape) */
    run_one_opcode(bc);
    TEST_ASSERT_EQ("sce-0 door: registered", 1, g_aot.slots[3].active);
    TEST_ASSERT_EQ("sce-0 door: INERT type", RE15_AOT_TYPE_NONE, g_aot.slots[3].type);

    /* the scan must NOT fire it — press-and-hold 12 frames inside the rect */
    scan_door_frames(12);
    TEST_ASSERT_EQ("sce-0 door: no room change (permanently dead until retyped)",
                   0, g_room_change.pending);

    /* Aot_reset(3, sce=2, flags=0x31, {-21937, 0, -25713}) — the ROOM1140 sub02
     * shape (census p0..2 = 43599/0/39823): arms the door + overwrites ONLY the
     * next_pos block (payload +0/+2/+4 @0x80040788-a8). */
    {
        uint8_t rt[11] = { 0x46, 0x03, 0x02, 0x31,
                           0x4F, 0xAA,     /* p0 = 0xAA4F = -21937 */
                           0x00, 0x00,     /* p1 = 0 */
                           0x8F, 0x9B,     /* p2 = 0x9B8F = -25713 */
                           OP_EVT_NEXT };
        run_one_opcode(rt);
    }
    TEST_ASSERT_EQ("retype-to-2: type DOOR", RE15_AOT_TYPE_DOOR, g_aot.slots[3].type);
    TEST_ASSERT_EQ("retype-to-2: spawn_x = payload p0", -21937, g_aot.door_params[3].spawn_x);
    TEST_ASSERT_EQ("retype-to-2: spawn_y = payload p1", 0, g_aot.door_params[3].spawn_y);
    TEST_ASSERT_EQ("retype-to-2: spawn_z = payload p2", -25713, g_aot.door_params[3].spawn_z);
    /* fields beyond the 3 written halfwords keep the INSTALL values */
    TEST_ASSERT_EQ("retype-to-2: dest_room kept", 0x24, g_aot.door_params[3].dest_room);
    TEST_ASSERT_EQ("retype-to-2: target_cut kept", 7, g_aot.door_params[3].target_cut);
    TEST_ASSERT_EQ("retype-to-2: spawn_yaw kept", 100, g_aot.door_params[3].spawn_yaw_4096);

    /* NOW the door must fire (regression check for the 16 armed sce-0 doors) */
    scan_door_frames(12);
    TEST_ASSERT_EQ("retype-to-2: door fires after arm", 1, g_room_change.pending);
    TEST_ASSERT_EQ("retype-to-2: dest room id", 0x1240, g_room_change.room_id);
    TEST_ASSERT_EQ("retype-to-2: fires with the NEW spawn", -25713, g_room_change.z);

    /* a LIVE sce-2 install still works untouched (the 607 shipped doors) */
    setup_vm();
    build_door(bc, 5, 0x02);
    run_one_opcode(bc);
    TEST_ASSERT_EQ("sce-2 door: type DOOR", RE15_AOT_TYPE_DOOR, g_aot.slots[5].type);
    scan_door_frames(12);
    TEST_ASSERT_EQ("sce-2 door: fires (no regression)", 1, g_room_change.pending);

    TEST_OK("sce-0 inert install + Aot_reset sce=2 arm (fixes 1+2)");
}

/* ===========================================================================
 * FIX 2: Aot_reset payload overwrite per sce (@0x80040764-a8).
 * =========================================================================== */
static void test_aot_reset_retype_payloads(void)
{
    setup_vm();
    /* base slot: a GENERIC event AOT with band 4 + flags 0x31 */
    re15_aot_set(6, RE15_AOT_TYPE_GENERIC, 9, 100, 100, 50, 50);
    g_aot.slots[6].band      = 4;
    g_aot.slots[6].sce_flags = 0x31;

    /* sce=1 MESSAGE: msg u16@+0 (ROOM1130 sub01 shape {1, 0xFFFF, 0}) */
    {
        uint8_t rt[11] = { 0x46, 0x06, 0x01, 0x31, 0x01, 0x00, 0xFF, 0xFF, 0x00, 0x00, OP_EVT_NEXT };
        run_one_opcode(rt);
        TEST_ASSERT_EQ("retype-to-1: MESSAGE", RE15_AOT_TYPE_MESSAGE, g_aot.slots[6].type);
        TEST_ASSERT_EQ("retype-to-1: msg = p0", 1, g_aot.slots[6].event_id);
        TEST_ASSERT_EQ("retype-to-1: band NOT written (rec[2] untouched)", 4, g_aot.slots[6].band);
    }
    /* sce=3 EVENT: sub = payload byte +3 = p1>>8 (ROOM1140 sub01 shape
     * {0xFF, 0x0518, 0} -> sub05, NOT the install's old event 9) */
    {
        uint8_t rt[11] = { 0x46, 0x06, 0x03, 0x31, 0xFF, 0x00, 0x18, 0x05, 0x00, 0x00, OP_EVT_NEXT };
        run_one_opcode(rt);
        TEST_ASSERT_EQ("retype-to-3: GENERIC (flags 0x31 action)", RE15_AOT_TYPE_GENERIC, g_aot.slots[6].type);
        TEST_ASSERT_EQ("retype-to-3: event = payload+3 (p1>>8), not the old id",
                       5, g_aot.slots[6].event_id);
    }
    /* sce=3 with flags 0x41 (bit 0x10 clear) -> AUTO event (census: 9 retypes) */
    {
        uint8_t rt[11] = { 0x46, 0x06, 0x03, 0x41, 0xFF, 0x00, 0x18, 0x02, 0x00, 0x00, OP_EVT_NEXT };
        run_one_opcode(rt);
        TEST_ASSERT_EQ("retype-to-3 auto: AUTO_EVENT", RE15_AOT_TYPE_AUTO_EVENT, g_aot.slots[6].type);
        TEST_ASSERT_EQ("retype-to-3 auto: flags replaced", 0x41, g_aot.slots[6].sce_flags);
    }
    /* sce=9 ITEM re-arm: {type, amount, tk_bit} (ROOM1190 sub10 shape {0x37,1,136}) */
    {
        uint8_t rt[11] = { 0x46, 0x06, 0x09, 0x31, 0x37, 0x00, 0x01, 0x00, 0x88, 0x00, OP_EVT_NEXT };
        run_one_opcode(rt);
        TEST_ASSERT_EQ("retype-to-9: ITEM", RE15_AOT_TYPE_ITEM, g_aot.slots[6].type);
        TEST_ASSERT_EQ("retype-to-9: item_type = p0", 0x37, g_aot.item_params[6].item_type);
        TEST_ASSERT_EQ("retype-to-9: amount = p1", 1, g_aot.item_params[6].amount);
        TEST_ASSERT_EQ("retype-to-9: taken_bit = p2", 136, g_aot.item_params[6].taken_bit);
    }
    /* sce=4 FLAG: {group, bit, on} (ROOM1030 sub02 shape {5, 33, 1}) */
    {
        uint8_t rt[11] = { 0x46, 0x06, 0x04, 0x42, 0x05, 0x00, 0x21, 0x00, 0x01, 0x00, OP_EVT_NEXT };
        run_one_opcode(rt);
        TEST_ASSERT_EQ("retype-to-4: FLAG_CHG", RE15_AOT_TYPE_FLAG_CHG, g_aot.slots[6].type);
        TEST_ASSERT_EQ("retype-to-4: group", 5, g_aot.flag_params[6].group);
        TEST_ASSERT_EQ("retype-to-4: bit", 0x21, g_aot.flag_params[6].bit);
        TEST_ASSERT_EQ("retype-to-4: on", 1, g_aot.flag_params[6].on);
    }
    /* sce=0 disable (188 census uses): registered + inert, band still preserved */
    {
        uint8_t rt[11] = { 0x46, 0x06, 0x00, 0x00, 0, 0, 0, 0, 0, 0, OP_EVT_NEXT };
        run_one_opcode(rt);
        TEST_ASSERT_EQ("retype-to-0: NONE", RE15_AOT_TYPE_NONE, g_aot.slots[6].type);
        TEST_ASSERT_EQ("retype-to-0: still registered", 1, g_aot.slots[6].active);
        TEST_ASSERT_EQ("retype-to-0: band preserved", 4, g_aot.slots[6].band);
    }
    TEST_OK("Aot_reset full retype payloads (fix 2)");
}

/* ===========================================================================
 * FIX 3: Aot_on = fire-now (@0x800407bc/jalr @0x8004082c) — no geometry test.
 * =========================================================================== */
static void test_aot_on_fire_now(void)
{
    uint8_t bc[33];

    /* DOOR: player NOWHERE near the rect; Aot_on must fire it anyway (the census
     * ROOM1240 sub02 slot-0 intro handoff / ROOM1080 sub07-10 shape). */
    setup_vm();
    build_door(bc, 2, 0x02);
    run_one_opcode(bc);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].x = -50000;   /* far away */
    g_actors[RE15_ACTOR_SLOT_PLAYER].z = -50000;
    {
        uint8_t on[3] = { 0x47, 0x02, OP_EVT_NEXT };
        long n = run_one_opcode(on);
        TEST_ASSERT_EQ("Aot_on: opcode size = 2", 2, n);
    }
    TEST_ASSERT_EQ("Aot_on DOOR: room change queued without geometry", 1, g_room_change.pending);
    TEST_ASSERT_EQ("Aot_on DOOR: dest", 0x1240, g_room_change.room_id);
    TEST_ASSERT_EQ("Aot_on DOOR: spawn from payload", 111, g_room_change.x);

    /* sce-0/NONE slot: handler[0] @0x8004305C — Aot_on does NOTHING */
    setup_vm();
    build_door(bc, 4, 0x00);
    run_one_opcode(bc);
    {
        uint8_t on[3] = { 0x47, 0x04, OP_EVT_NEXT };
        run_one_opcode(on);
    }
    TEST_ASSERT_EQ("Aot_on on sce-0: inert (no fire)", 0, g_room_change.pending);

    /* ITEM: handler[9] @0x80043328 arms the pickup FSM = the port's modal
     * (census: 7 scripted grants, e.g. ROOM1051 sub03 slot 12). */
    setup_vm();
    re15_aot_set_item_tk(9, 5000, 5000, 100, 100, 0x15, 30, 0);
    TEST_ASSERT_EQ("Aot_on ITEM: modal not yet active", 0, re15_item_modal_active());
    {
        uint8_t on[3] = { 0x47, 0x09, OP_EVT_NEXT };
        run_one_opcode(on);
    }
    TEST_ASSERT_EQ("Aot_on ITEM: pickup modal armed without geometry", 1, re15_item_modal_active());

    /* FLAG_CHG: handler[4] @0x80043120 — the flag write runs once */
    setup_vm();
    re15_aot_set(7, RE15_AOT_TYPE_FLAG_CHG, 0, 5000, 5000, 100, 100);
    g_aot.flag_params[7].group = 5; g_aot.flag_params[7].bit = 0x21; g_aot.flag_params[7].on = 1;
    {
        uint8_t on[3] = { 0x47, 0x07, OP_EVT_NEXT };
        run_one_opcode(on);
    }
    TEST_ASSERT_EQ("Aot_on FLAG: flag written", 1, re15_game_flag_get(5, 0x21));

    TEST_OK("Aot_on fire-now (fix 3)");
}

/* ===========================================================================
 * FIX 4: sce-5 markers — object/enemy-pool presence latch + no enum collision.
 * =========================================================================== */
static void test_sce5_markers(void)
{
    /* 0x44 = AUTO + centre + OBJECT pool (142 shipped installs): a prop inside
     * latches work_vars[1] = slot every frame (@0x80042ee4); handler[5] = NOP. */
    uint8_t bc[21] = {
        0x2C, 0x0A, 0x05, 0x44,  0x80, 0x00,   /* slot 10, sce 5, flags 0x44, band 0x80 any */
        0x00, 0x00, 0x00, 0x00,                 /* rect NW (0,0) */
        0xC8, 0x00, 0xC8, 0x00,                 /* w=200 d=200 -> centre (100,100) */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, OP_EVT_NEXT };
    setup_vm();
    run_one_opcode(bc);
    TEST_ASSERT_EQ("sce-5 0x44: installs as marker (EXAMINE_WORKVAR)",
                   RE15_AOT_TYPE_EXAMINE_WORKVAR, g_aot.slots[10].type);

    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_scd.work_vars[1] = -1;
    /* the PLAYER inside must NOT latch (pool mask = objects only) */
    re15_aot_scan(100, 100, 0xFF);
    TEST_ASSERT_EQ("sce-5 0x44: player does NOT latch (object pool only)",
                   -1, g_scd.work_vars[1]);
    /* a prop inside latches work_vars[1] = slot, WITHOUT any action press */
    g_scd.prop_count = 1;
    memset(&g_scd.props[0], 0, sizeof(g_scd.props[0]));
    g_scd.props[0].active = 1;
    g_scd.props[0].x = 120; g_scd.props[0].z = 90;
    re15_aot_scan(-5000, -5000, 0xFF);
    TEST_ASSERT_EQ("sce-5 0x44: OBJECT inside latches work_vars[1] = slot",
                   10, g_scd.work_vars[1]);
    /* byte-true: NOTHING clears the latch when the object leaves (FUN_800436a8
     * clears only entity+0xB per frame @0x8004371c/0x80043788) */
    g_scd.props[0].x = 90000;
    re15_aot_scan(-5000, -5000, 0xFF);
    TEST_ASSERT_EQ("sce-5 0x44: latch NOT engine-cleared on exit", 10, g_scd.work_vars[1]);

    /* per-entity band gate (@0x80042cac vs prop+0x82): band-0 marker + band-1 prop
     * -> no latch; matching band -> latch */
    setup_vm();
    bc[1] = 0x0B; bc[4] = 0x00;                  /* slot 11, band 0 (not 0x80) */
    run_one_opcode(bc);
    g_scd.prop_count = 1;
    memset(&g_scd.props[0], 0, sizeof(g_scd.props[0]));
    g_scd.props[0].active = 1;
    g_scd.props[0].x = 100; g_scd.props[0].z = 100;
    g_scd.props[0].band = 1;                     /* wrong floor */
    g_scd.work_vars[1] = -1;
    re15_aot_scan(-5000, -5000, 0xFF);
    TEST_ASSERT_EQ("sce-5 band gate: mismatched prop band does NOT latch",
                   -1, g_scd.work_vars[1]);
    g_scd.props[0].band = 0;
    re15_aot_scan(-5000, -5000, 0xFF);
    TEST_ASSERT_EQ("sce-5 band gate: matching prop band latches", 11, g_scd.work_vars[1]);

    /* 0x42 = AUTO + centre + ENEMY pool (the retype variant, ROOM1030 slot 6) */
    setup_vm();
    re15_aot_set(12, RE15_AOT_TYPE_GENERIC, 0, 0, 0, 1, 1);
    {
        uint8_t rt[11] = { 0x46, 0x0C, 0x05, 0x42, 0, 0, 0, 0, 0, 0, OP_EVT_NEXT };
        run_one_opcode(rt);
    }
    /* recreate the marker rect (the retype keeps geometry; give it one) */
    g_aot.slots[12].x = 100; g_aot.slots[12].z = 100;
    g_aot.slots[12].half_w = 100; g_aot.slots[12].half_h = 100;
    g_aot.slots[12].band = 0x80;
    TEST_ASSERT_EQ("retype-to-5: marker type", RE15_AOT_TYPE_EXAMINE_WORKVAR, g_aot.slots[12].type);
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_scd.work_vars[1] = -1;
    re15_aot_scan(100, 100, 0xFF);               /* player inside: mask is ENEMY only */
    TEST_ASSERT_EQ("sce-5 0x42: player does NOT latch", -1, g_scd.work_vars[1]);
    g_actors[3].active = 1; g_actors[3].x = 100; g_actors[3].z = 100; g_actors[3].floor = 0;
    re15_aot_scan(-5000, -5000, 0xFF);
    TEST_ASSERT_EQ("sce-5 0x42: ENEMY inside latches work_vars[1]", 12, g_scd.work_vars[1]);

    /* the 8 shipped ev=0xFF installs (ROOM1190/1191 slots 0-3, payload {0,0,0x80,0xFF,0,0})
     * must NOT collide with the port MESSAGE enum (the old ev-heuristic bug) */
    setup_vm();
    {
        uint8_t bcff[21] = {
            0x2C, 0x0D, 0x05, 0x31,  0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,  0xC8, 0x00, 0xC8, 0x00,
            0x00, 0x00, 0x80, 0xFF,               /* payload +2 = 0xFF80 -> pc[17] = 0xFF */
            0x00, 0x00, OP_EVT_NEXT };
        run_one_opcode(bcff);
    }
    TEST_ASSERT_EQ("sce-5 ev=0xFF: NOT a MESSAGE (enum collision fixed)",
                   RE15_AOT_TYPE_EXAMINE_WORKVAR, g_aot.slots[13].type);

    /* ROOM1150-style ACTION examine (flags 0x31) still works: action press with the
     * forward-620 point inside -> work_vars[0] = slot + one-shot sub01 poll flag */
    setup_vm();
    {
        uint8_t bca[21] = {
            0x2C, 0x02, 0x05, 0x31,  0x00, 0x00,
            0x08, 0x02, 0x9C, 0xFF,               /* rect NW (520,-100) */
            0xC8, 0x00, 0xC8, 0x00,               /* w=200 d=200 -> centre (620,0) */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, OP_EVT_NEXT };
        run_one_opcode(bca);
    }
    g_actors[RE15_ACTOR_SLOT_PLAYER].active = 1;
    g_actors[RE15_ACTOR_SLOT_PLAYER].rot_y  = 0;
    g_scd.work_vars[0] = -1;
    extern uint8_t g_aot_action_pressed;
    g_aot_action_pressed = 1;
    re15_aot_scan(0, 0, 0xFF);                   /* forward point (620,0) inside */
    g_aot_action_pressed = 0;
    TEST_ASSERT_EQ("sce-5 action examine: work_vars[0] = slot", 2, g_scd.work_vars[0]);
    TEST_ASSERT_EQ("sce-5 action examine: sub01 re-poll flagged", 1, g_scd.examine_poll_pending);

    TEST_OK("sce-5 marker latches (fix 4)");
}

int main(void)
{
    printf("=== AOT sce census fixes (d7376834) ===\n\n");
    test_sce0_inert_and_retype_to_door();
    test_aot_reset_retype_payloads();
    test_aot_on_fire_now();
    test_sce5_markers();
    printf("\n=== Ergebnis: %d PASS, %d FAIL ===\n", g_pass, g_fail);
    return (g_fail > 0) ? 1 : 0;
}
