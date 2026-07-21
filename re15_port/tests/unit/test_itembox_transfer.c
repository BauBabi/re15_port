/**
 * @file test_itembox_transfer.c
 * @brief ITEM BOX transfer engine (re15_itembox.c) — table-driven over the RE2
 *        FUN_800703b8 quirks catalog (shots/itembox_spec.md §3/§7; RE2-Leon cites)
 *        + savedata v4 box persistence (round-trip, v3 upgrade, memcard).
 *
 * Ammo caps are read from the embedded RE1.5 prop table @0x80074DA8 +0 (the blob,
 * re15_inv_ui.h) — nothing re-typed.
 */
#include <stdio.h>
#include <string.h>
#include "re15_itembox.h"
#include "re15_inventory.h"
#include "re15_inv_ui.h"
#include "re15_savedata.h"
#include "re15_memcard.h"
#include "re15_actor.h"
#include "re15_room.h"
#include "re15_gameflow.h"
#include "re15_damage.h"

static int fails = 0;
#define CHECK(cond, ...) do { if (!(cond)) { fprintf(stderr, "FAIL: " __VA_ARGS__); \
    fprintf(stderr, "\n"); fails++; } } while (0)

static uint8_t cap_of(uint8_t id)   /* prop @0x80074DA8 + id*12 + 0 (RE1.5 blob) */
{
    return *RE15_INV_PTR(0x80074DA8u + (uint32_t)id * 12u);
}

static void reset_all(void)
{
    re15_inv_init();
    re15_itembox_init();
    re15_inv_set_equipped_slot(0x80);
    re15_inv_set_prev_equip_slot(0x80);
}

static re15_inv_slot_t *box_at(int page, int slot)
{
    return &g_itembox.pages[page].slots[slot];
}

int main(void)
{
    printf("=== ITEM BOX transfer engine (RE2 FUN_800703b8 quirks, spec §3/§7) ===\n");
    uint8_t cap15 = cap_of(0x15);   /* H.GUN BULLETS max stack from the blob */
    CHECK(cap15 > 1, "(pre) ammo 0x15 cap from the prop blob must be sane, got %d", cap15);

    /* ---- QUIRK 1: ammo qty-SWAP when the inventory stack is full (@0x800704e0-f4) */
    reset_all();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = cap15;
    box_at(0, 0)->id = 0x15; box_at(0, 0)->qty = 30;
    CHECK(re15_itembox_transfer(0, 0, 0) == RE15_BOX_XFER_OK, "(q1) transfer ok");
    CHECK(g_inv.slots[0].id == 0x15 && g_inv.slots[0].qty == 30,
          "(q1) inv qty := boxqty (30), got %d", g_inv.slots[0].qty);
    CHECK(box_at(0, 0)->id == 0x15 && box_at(0, 0)->qty == cap15,
          "(q1) box qty := cap (%d), got %d", cap15, box_at(0, 0)->qty);

    /* ---- ammo sub-case (b): sum > cap -> inv := cap, box := sum - cap */
    reset_all();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = (uint8_t)(cap15 - 5);
    box_at(0, 0)->id = 0x15; box_at(0, 0)->qty = 20;
    re15_itembox_transfer(0, 0, 0);
    CHECK(g_inv.slots[0].qty == cap15, "(qb) inv := cap");
    CHECK(box_at(0, 0)->qty == (uint8_t)(cap15 - 5 + 20 - cap15),
          "(qb) box := sum-cap (%d), got %d", cap15 - 5 + 20 - cap15, box_at(0, 0)->qty);

    /* ---- ammo sub-case (c): sum <= cap -> inv += box, box zeroed (QUIRK 2:
     * the merge fires even though the player pressed confirm ON the box slot =
     * deposit intent) */
    reset_all();
    g_inv.slots[0].id = 0x15; g_inv.slots[0].qty = 10;
    box_at(0, 0)->id = 0x15; box_at(0, 0)->qty = 5;
    re15_itembox_transfer(0, 0, 0);
    CHECK(g_inv.slots[0].qty == 15, "(qc) inv += box");
    CHECK(box_at(0, 0)->id == 0, "(qc) box slot zeroed");

    /* ---- QUIRK 3: empty-cursor ammo withdraw redirects into ANY fitting stack,
     * whole-stack-only (@0x80070554-98) */
    reset_all();
    g_inv.slots[0].id = 0x01;                            /* knife            */
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = 10;   /* existing ammo    */
    box_at(0, 0)->id = 0x15; box_at(0, 0)->qty = 5;      /* fits whole       */
    re15_itembox_transfer(3, 0, 0);                      /* cursor on EMPTY 3 */
    CHECK(g_inv.slots[1].qty == 15, "(q3) redirect merged into slot 1");
    CHECK(box_at(0, 0)->id == 0, "(q3) box stack cleared");
    CHECK(g_inv.slots[3].id == 0, "(q3) the empty cursor slot stays empty");
    /* negative: whole stack does NOT fit -> normal swap (lands min(cursor,first-free)) */
    reset_all();
    g_inv.slots[0].id = 0x01;
    g_inv.slots[1].id = 0x15; g_inv.slots[1].qty = (uint8_t)(cap15 - 2);
    box_at(0, 0)->id = 0x15; box_at(0, 0)->qty = 5;      /* 5 > cap-(cap-2)  */
    re15_itembox_transfer(3, 0, 0);
    CHECK(g_inv.slots[1].qty == (uint8_t)(cap15 - 2), "(q3n) existing stack untouched");
    CHECK(g_inv.slots[2].id == 0x15 && g_inv.slots[2].qty == 5,
          "(q3n) whole stack lands at min(cursor=3, first-free=2)=2, got slot2 id %02x",
          g_inv.slots[2].id);
    CHECK(box_at(0, 0)->id == 0, "(q3n) box slot emptied by the swap");

    /* ---- QUIRK 4: withdraw lands at min(cursor, first-free) (case 0 dest) */
    reset_all();
    g_inv.slots[0].id = 0x01;                            /* knife            */
    g_inv.slots[3].id = 0x24; g_inv.slots[3].qty = 1;    /* herb at cursor 3
                                                          * (slots 1/2 empty) */
    box_at(1, 2)->id = 0x03; box_at(1, 2)->qty = 15;     /* handgun in box   */
    re15_itembox_transfer(3, 1, 2);
    CHECK(box_at(1, 2)->id == 0x24 && box_at(1, 2)->qty == 1,
          "(q4) herb deposited into the box slot");
    CHECK(g_inv.slots[1].id == 0x03 && g_inv.slots[1].qty == 15,
          "(q4) handgun landed at min(3, first-free=1)=1, got %02x", g_inv.slots[1].id);
    CHECK(g_inv.slots[2].id == 0 && g_inv.slots[3].id == 0,
          "(q4) cursor slot cleared + compacted");

    /* ---- QUIRK 5: 2-cell withdraw — THE ONLY reject (@0x800707dc-ec) + the
     * front-shift landing (slots 0+1, equip += 2) */
    reset_all();
    for (int i = 0; i < 10; i++) { g_inv.slots[i].id = 0x24; g_inv.slots[i].qty = 1; }
    box_at(0, 0)->id = 0x0e; box_at(0, 0)->qty = 100;
    box_at(0, 0)->flags = RE15_BOX_KIND_WIDE;
    CHECK(re15_itembox_transfer(0, 0, 0) == RE15_BOX_XFER_REJECT,
          "(q5) empties==0 -> reject");
    /* empties==1 && cursor EMPTY -> reject */
    reset_all();
    for (int i = 0; i < 9; i++) { g_inv.slots[i].id = 0x24; g_inv.slots[i].qty = 1; }
    box_at(0, 0)->id = 0x0e; box_at(0, 0)->qty = 100;
    box_at(0, 0)->flags = RE15_BOX_KIND_WIDE;
    CHECK(re15_itembox_transfer(9, 0, 0) == RE15_BOX_XFER_REJECT,
          "(q5) empties==1 && cursor empty -> reject");
    /* empties==1 && cursor OCCUPIED -> succeeds (the deposit frees a slot) */
    reset_all();
    for (int i = 0; i < 9; i++) { g_inv.slots[i].id = 0x24; g_inv.slots[i].qty = 1; }
    re15_inv_set_equipped_slot(4);                       /* equipped elsewhere */
    box_at(0, 0)->id = 0x0e; box_at(0, 0)->qty = 100;
    box_at(0, 0)->flags = RE15_BOX_KIND_WIDE;
    CHECK(re15_itembox_transfer(0, 0, 0) == RE15_BOX_XFER_OK, "(q5) withdraw ok");
    CHECK(g_inv.slots[0].id == 0x0e && g_inv.slots[0].flags == 1 &&
          g_inv.slots[1].id == 0x0e && g_inv.slots[1].flags == 2,
          "(q5) weapon into slots 0+1 kinds 1/2 (RE2 Size 1/2), got %d/%d",
          g_inv.slots[0].flags, g_inv.slots[1].flags);
    CHECK(g_inv.slots[0].qty == 100, "(q5) qty carried");
    CHECK(re15_inv_equipped_slot() == 5,
          "(q5) equip tracks: compaction closes the cursor hole (4->3, FUN_8004dadc "
          "equip-- rule @0x8004dbac-dbc8) then the front-shift += 2 (3->5; RE2 "
          "@0x80070830 / RE1.5 @0x8004dc84-9c), got %d", re15_inv_equipped_slot());
    CHECK(box_at(0, 0)->id == 0x24 && box_at(0, 0)->flags == 0,
          "(q5) the old cursor item went into the box");

    /* ---- QUIRK 6 + 7: deposit a 2-cell -> ONE box slot kind 3; compaction TWICE */
    reset_all();
    g_inv.slots[0].id = 0x0e; g_inv.slots[0].qty = 77; g_inv.slots[0].flags = 1;
    g_inv.slots[1].id = 0x0e; g_inv.slots[1].qty = 77; g_inv.slots[1].flags = 2;
    g_inv.slots[2].id = 0x24; g_inv.slots[2].qty = 1;
    g_inv.slots[3].id = 0x26; g_inv.slots[3].qty = 1;
    re15_itembox_transfer(0, 2, 5);
    CHECK(box_at(2, 5)->id == 0x0e && box_at(2, 5)->qty == 77 &&
          box_at(2, 5)->flags == RE15_BOX_KIND_WIDE,
          "(q6) boxed 2-cell = ONE slot, kind 3 (`li v0,3; sb` @0x80070ad0-dc), got kind %d",
          box_at(2, 5)->flags);
    CHECK(g_inv.slots[0].id == 0x24 && g_inv.slots[1].id == 0x26 && g_inv.slots[2].id == 0,
          "(q7) BOTH cells cleared + compaction TWICE closed both holes "
          "(@0x800709bc+@0x80070a24), got %02x/%02x/%02x",
          g_inv.slots[0].id, g_inv.slots[1].id, g_inv.slots[2].id);
    /* tail-cursor normalization: cursor on the TAIL cell behaves like the head */
    reset_all();
    g_inv.slots[0].id = 0x0e; g_inv.slots[0].qty = 3; g_inv.slots[0].flags = 1;
    g_inv.slots[1].id = 0x0e; g_inv.slots[1].qty = 3; g_inv.slots[1].flags = 2;
    re15_itembox_transfer(1, 0, 0);
    CHECK(box_at(0, 0)->flags == RE15_BOX_KIND_WIDE && g_inv.slots[0].id == 0,
          "(q6b) tail-cell cursor normalized to the head (@0x8004e910-38 shape)");

    /* ---- case 2 with an occupied box slot: box item into first-free */
    reset_all();
    g_inv.slots[0].id = 0x0e; g_inv.slots[0].qty = 9; g_inv.slots[0].flags = 1;
    g_inv.slots[1].id = 0x0e; g_inv.slots[1].qty = 9; g_inv.slots[1].flags = 2;
    g_inv.slots[2].id = 0x24; g_inv.slots[2].qty = 1;
    box_at(0, 3)->id = 0x03; box_at(0, 3)->qty = 15;
    re15_itembox_transfer(0, 0, 3);
    CHECK(box_at(0, 3)->id == 0x0e && box_at(0, 3)->flags == RE15_BOX_KIND_WIDE,
          "(c2) weapon boxed kind 3");
    CHECK(g_inv.slots[0].id == 0x24 && g_inv.slots[1].id == 0x03,
          "(c2) box item into first-free after the double compaction, got %02x/%02x",
          g_inv.slots[0].id, g_inv.slots[1].id);

    /* ---- case 3: 2-cell ⇄ 2-cell in place (@0x80070c30-3c/@0x80070d98) */
    reset_all();
    g_inv.slots[0].id = 0x0e; g_inv.slots[0].qty = 5; g_inv.slots[0].flags = 1;
    g_inv.slots[1].id = 0x0e; g_inv.slots[1].qty = 5; g_inv.slots[1].flags = 2;
    box_at(3, 7)->id = 0x13; box_at(3, 7)->qty = 40;
    box_at(3, 7)->flags = RE15_BOX_KIND_WIDE;
    re15_itembox_transfer(0, 3, 7);
    CHECK(g_inv.slots[0].id == 0x13 && g_inv.slots[0].flags == 1 &&
          g_inv.slots[1].id == 0x13 && g_inv.slots[1].flags == 2 &&
          g_inv.slots[0].qty == 40,
          "(c3) box weapon into both cells");
    CHECK(box_at(3, 7)->id == 0x0e && box_at(3, 7)->qty == 5 &&
          box_at(3, 7)->flags == RE15_BOX_KIND_WIDE,
          "(c3) inv weapon boxed kind 3");

    /* ---- QUIRK 8: equipped deposit silently unequips (every branch) */
    reset_all();
    g_inv.slots[0].id = 0x03; g_inv.slots[0].qty = 15;
    re15_inv_set_equipped_slot(0);
    re15_itembox_transfer(0, 0, 1);
    CHECK(re15_inv_equipped_slot() == 0x80,
          "(q8) case-0 deposit of the equipped item -> equip 0x80, got %02x",
          re15_inv_equipped_slot());
    reset_all();
    g_inv.slots[0].id = 0x0e; g_inv.slots[0].qty = 1; g_inv.slots[0].flags = 1;
    g_inv.slots[1].id = 0x0e; g_inv.slots[1].qty = 1; g_inv.slots[1].flags = 2;
    re15_inv_set_equipped_slot(0);
    re15_itembox_transfer(0, 0, 0);
    CHECK(re15_inv_equipped_slot() == 0x80, "(q8) case-2 wide deposit unequips");

    /* ---- empty ⇄ empty is a no-op */
    reset_all();
    CHECK(re15_itembox_transfer(0, 0, 0) == RE15_BOX_XFER_OK, "(nop) ok");
    CHECK(g_inv.slots[0].id == 0 && box_at(0, 0)->id == 0, "(nop) both stay empty");

    /* ================= savedata v4 persistence ================= */
    printf("=== savedata v4: box round-trip + v2/v3 upgrade + memcard ===\n");
    reset_all();
    memset(&g_actors[0], 0, sizeof g_actors[0]);
    g_current_room_id = 0x1150;
    g_inv.slots[0].id = 0x01;
    box_at(0, 0)->id = 0x03; box_at(0, 0)->qty = 15;
    box_at(1, 4)->id = 0x15; box_at(1, 4)->qty = 50;
    box_at(3, 7)->id = 0x0e; box_at(3, 7)->qty = 100;
    box_at(3, 7)->flags = RE15_BOX_KIND_WIDE;
    re15_savedata_t sd;
    re15_savedata_capture(&sd, 100, 1);
    CHECK(sd.version == 4, "(v4) capture stamps version 4, got %u", sd.version);
    CHECK(sd.box[0].id == 0x03 && sd.box[1 * 8 + 4].id == 0x15 &&
          sd.box[3 * 8 + 7].id == 0x0e && sd.box[3 * 8 + 7].flags == RE15_BOX_KIND_WIDE,
          "(v4) box flattened page*8+i into the block");
    re15_itembox_init();                                 /* clobber */
    uint16_t room = 0;
    CHECK(re15_savedata_restore(&sd, &room) == 0, "(v4) restore ok");
    CHECK(box_at(0, 0)->id == 0x03 && box_at(1, 4)->qty == 50 &&
          box_at(3, 7)->flags == RE15_BOX_KIND_WIDE,
          "(v4) box contents restored");

    /* v3 block (pre-box layout): old checksum word sits at offsetof(box);
     * must validate + load with an EMPTY box. */
    {
        re15_savedata_t v3 = sd;
        v3.version = 3;
        memset(v3.box, 0xAA, sizeof v3.box);             /* garbage where v3 ended */
        v3.checksum = 0xDEADBEEF;                        /* v4 word = garbage too  */
        const uint8_t *p = (const uint8_t *)&v3;
        uint32_t sum = 0;
        size_t old_off = (size_t)((const uint8_t *)&v3.box[0] - p);
        for (size_t i = 0; i < old_off; i++) sum += p[i];
        memcpy((uint8_t *)&v3 + old_off, &sum, 4);       /* the v3 checksum word   */
        box_at(0, 0)->id = 0x77;                         /* sentinel to clobber    */
        CHECK(re15_savedata_restore(&v3, &room) == 0, "(v3) old layout restores");
        CHECK(box_at(0, 0)->id == 0 && box_at(3, 7)->id == 0,
              "(v3) v3 load = EMPTY box, got %02x", box_at(0, 0)->id);
        /* corrupt v3 checksum must still be rejected */
        re15_savedata_t bad = v3;
        memset(bad.box, 0xAA, sizeof bad.box);
        uint32_t z = 0x12345678;
        memcpy((uint8_t *)&bad + old_off, &z, 4);
        CHECK(re15_savedata_restore(&bad, &room) == -1, "(v3) bad old checksum rejected");
    }

    /* memcard: box contents survive the .mcr round trip */
    {
        const char *mcr = "test_re15_box_card.mcr";
        remove(mcr);
        reset_all();
        box_at(2, 3)->id = 0x24; box_at(2, 3)->qty = 2;
        re15_savedata_t s2, l2;
        re15_savedata_capture(&s2, 1, 1);
        CHECK(re15_memcard_save(mcr, 0, &s2, "BOX TEST") == 0, "(mcr) save");
        re15_itembox_init();
        CHECK(re15_memcard_load(mcr, 0, &l2) == 0, "(mcr) load");
        CHECK(re15_savedata_restore(&l2, &room) == 0, "(mcr) restore");
        CHECK(box_at(2, 3)->id == 0x24 && box_at(2, 3)->qty == 2,
              "(mcr) box survived the card round trip");
        remove(mcr);
    }

    if (fails) { printf("ITEMBOX TRANSFER: %d FAIL\n", fails); return 1; }
    printf("ITEMBOX TRANSFER: all checks passed\n");
    return 0;
}
