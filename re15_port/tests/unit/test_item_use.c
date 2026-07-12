/* test_item_use.c — byte-true inventory item-USE (heal) flow (RE'd wf_13443da6).
 *
 * RE1.5 has NO USE/CHECK/COMBINE sub-menu: confirm on a usable heal item (0x22..0x2e) opens the
 * "Will you use the X?" Yes/No prompt (entry[22]@0x8004b250); on Yes it applies the heal (table
 * @0x80010fbc) + consumes the slot (@0x8004aef0), then shows "You have used the X" (script[2]).
 * Byte-true quirks preserved: NO ceiling clamp on additive heals, consume even at full HP, First Aid
 * heals to 100 but doesn't cure poison. Models the modal's Yes/No input (0x4000=CROSS, 0x1000=toggle).
 */
#include <stdio.h>
#include <stdint.h>
#include "re15_item_use.h"
#include "re15_inventory.h"
#include "re15_actor.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static re15_actor_t *PL(void) { return &g_actors[RE15_ACTOR_SLOT_PLAYER]; }

/* Tick (no input) through the typewriter until the prompt has fully revealed. */
static void reveal(void)
{
    int g = 0;
    while (re15_item_use_active() && !re15_item_use_prompt_ready() && g++ < 400) re15_item_use_tick(0);
}

/* Drive the USE FSM to idle (reveal each message + confirm/dismiss) so the next test starts clean. */
static void drain(void)
{
    int g = 0;
    while (re15_item_use_active() && g++ < 800)
        re15_item_use_tick(re15_item_use_prompt_ready() ? 0x4000 : 0);
}

int main(void)
{
    printf("=== byte-true item-USE (heal) flow (wf_13443da6) ===\n");

    /* ---- (1) Green Medicine end-to-end: HP 40 -> 90, consumed ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;   /* Green Medicine */
        PL()->hp = 40;
        re15_item_use_start(0x24, 0);
        CHECK("classifier -> USE prompt (4)", re15_item_use_prompt(NULL, NULL) == 4);
        reveal();
        CHECK("HP unchanged while the prompt is up", PL()->hp == 40);
        re15_item_use_tick(0x4000);                          /* CROSS = Yes */
        CHECK("Yes -> HP += 50 (Green Medicine)", PL()->hp == 90);
        CHECK("advances to the 'used' message (5)", re15_item_use_prompt(NULL, NULL) == 5);
        CHECK("slot consumed on Yes", g_inv.slots[0].id == 0);
        reveal();
        re15_item_use_tick(0xc000);                          /* dismiss the used-message */
        CHECK("USE done, back to the grid", !re15_item_use_active());
    }

    /* ---- (2) No declines: HP + slot unchanged, item stays ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
        PL()->hp = 40;
        re15_item_use_start(0x24, 0);
        reveal();
        re15_item_use_tick(0x1000 | 0x4000);                 /* toggle to No + confirm */
        CHECK("No -> HP unchanged", PL()->hp == 40);
        CHECK("No -> item NOT consumed", g_inv.slots[0].id == 0x24);
        CHECK("No -> USE aborted", !re15_item_use_active());
    }

    /* ---- (3) byte-true NO ceiling clamp: HP 90 + Green -> 140 (not 100, @0x8004afb0) ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
        PL()->hp = 90;
        re15_item_use_start(0x24, 0);
        reveal();
        re15_item_use_tick(0x4000);
        CHECK("no clamp: HP 90 + Green = 140 (byte-true overheal)", PL()->hp == 140);
        drain();
    }

    /* ---- (4) First Aid Spray (0x22): absolute HP = 100 (sh s0 @0x8004afe4) ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x22; g_inv.slots[0].qty = 1;
        PL()->hp = 10;
        re15_item_use_start(0x22, 0);
        reveal();
        re15_item_use_tick(0x4000);
        CHECK("First Aid: HP 10 -> 100 (absolute)", PL()->hp == 100);
        CHECK("First Aid consumed", g_inv.slots[0].id == 0);
        drain();
    }

    /* ---- (5) consume even at full HP (state 2 unconditional): HP 100 + Green -> 150, consumed ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x24; g_inv.slots[0].qty = 1;
        PL()->hp = 100;
        re15_item_use_start(0x24, 0);
        reveal();
        re15_item_use_tick(0x4000);
        CHECK("full-HP: still overheals to 150", PL()->hp == 150);
        CHECK("full-HP: still consumed", g_inv.slots[0].id == 0);
        drain();
    }

    /* ---- (6) cure item (Antidote 0x23): HP += 25 AND poison bit 0x2 cleared (@0x8004af8c) ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x23; g_inv.slots[0].qty = 1;
        PL()->hp = 50;
        PL()->status_flags = 0x2;                            /* poisoned */
        re15_item_use_start(0x23, 0);
        reveal();
        re15_item_use_tick(0x4000);
        CHECK("Antidote: HP += 25", PL()->hp == 75);
        CHECK("Antidote: poison (bit 0x2) cured", (PL()->status_flags & 0x2) == 0);
        drain();
    }

    /* ---- (7) Red Medicine (0x25) is NOT heal-usable alone (byte-true `beq a0,0x25` exclusion) ---- */
    {
        re15_inv_init();
        g_inv.slots[0].id = 0x25; g_inv.slots[0].qty = 1;
        PL()->hp = 40;
        CHECK("Red Medicine (0x25) is not a heal item", !re15_item_use_is_heal(0x25));
        re15_item_use_start(0x25, 0);
        CHECK("Red: USE not started (inert alone)", !re15_item_use_active());
        CHECK("Red: HP + slot untouched", PL()->hp == 40 && g_inv.slots[0].id == 0x25);
        /* the real medicines around it ARE usable */
        CHECK("Green (0x24) IS a heal item", re15_item_use_is_heal(0x24));
        CHECK("Blue (0x26) IS a heal item",  re15_item_use_is_heal(0x26));
    }

    if (g_fail) { printf("ITEM-USE: FAIL\n"); return 1; }
    printf("ITEM-USE: all checks passed\n");
    return 0;
}
