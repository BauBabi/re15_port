/* test_item_modal.c — byte-true item-get "You got X" presentation MODAL (FUN_8001db28).
 *
 * The pickup presentation is a 9-state FSM (DAT_80072d3b) that FREEZES the game, zooms/spins the
 * item icon in (17 frames), flips it like a coin (9 frames), then GRANTS the item — the grant is
 * DEFERRED to the modal's end and gated on inventory-room (a FULL inventory shrinks the box away
 * over 17 frames and leaves the item in the world). Verifies the exact frame budget, the deferral,
 * the full-inventory branch, and the re-entry guard. (RE'd workflow wq41xdnn2.)
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_item_modal.h"
#include "re15_inventory.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

/* Is item `id` present in any inventory slot? */
static int inv_has(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == id) return 1;
    return 0;
}

int main(void)
{
    printf("=== byte-true item-get modal FSM (FUN_8001db28, wf wq41xdnn2) ===\n");

    /* ---- (1) NORMAL path: empty inventory -> zoom(17)+gate(1)+flip(9)+init(1)+tail(1) = 29 ticks,
     *          grant DEFERRED to the very last tick, item present at the end. ---- */
    {
        re15_inv_init();                                  /* all slots empty = room to grant */
        re15_item_modal_start(0x15, 50, 0, -1);           /* H.GUN BULLETS x50, no taken-bit / no AOT */
        CHECK("start -> state 1",        re15_item_modal_state() == 1);
        CHECK("start -> active",         re15_item_modal_active() == 1);
        CHECK("grant is DEFERRED (not in inventory yet)", !inv_has(0x15));

        int ticks = 0, granted_at = -1, saw_zoom = 0, saw_flip = 0, max_state = 0;
        while (re15_item_modal_active()) {
            uint8_t st = re15_item_modal_state();
            if (st == 2) saw_zoom++;
            if (st == 4) saw_flip++;
            if (st > max_state) max_state = st;
            re15_item_modal_tick();
            ticks++;
            if (granted_at < 0 && inv_has(0x15)) granted_at = ticks;
            if (ticks > 200) break;                       /* runaway guard */
        }
        CHECK("zoom ran 17 frames",      saw_zoom == 17);
        CHECK("flip ran 9 frames",       saw_flip == 9);
        CHECK("no shrink state on the normal path (max state <= 7)", max_state <= 7);
        CHECK("total tick budget = 29",  ticks == 29);
        CHECK("granted on the FINAL tick", granted_at == ticks);
        CHECK("item present at the end", inv_has(0x15));
        CHECK("modal idle after done",   re15_item_modal_active() == 0 && re15_item_modal_state() == 0);
    }

    /* ---- (2) The re-entry GUARD (@0x80043334): starting again while a modal runs is ignored. ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 10, 0, -1);
        re15_item_modal_tick();                            /* into state 2 */
        uint8_t before = re15_item_modal_state();
        re15_item_modal_start(0x03, 1, 0, -1);             /* must be ignored */
        CHECK("guard: second start ignored (state unchanged)", re15_item_modal_state() == before);
        while (re15_item_modal_active()) re15_item_modal_tick();
        CHECK("guard: only the FIRST item was granted (0x15, not 0x03)",
              inv_has(0x15) && !inv_has(0x03));
    }

    /* ---- (3) FULL inventory: no grant, the SHRINK-away path (state 8) runs, item stays out. ---- */
    {
        re15_inv_init();
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) { g_inv.slots[i].id = 0x22; g_inv.slots[i].qty = 1; }
        re15_item_modal_start(0x24, 1, 0, -1);             /* a key onto a FULL inventory */
        int ticks = 0, saw_shrink = 0;
        while (re15_item_modal_active()) {
            if (re15_item_modal_state() == 8) saw_shrink++;
            re15_item_modal_tick();
            ticks++;
            if (ticks > 200) break;
        }
        CHECK("full: shrink-away ran 17 frames", saw_shrink == 17);
        CHECK("full: item 0x24 NOT granted (inventory unchanged)", !inv_has(0x24));
        CHECK("full: total budget = 46 (29 + 17)", ticks == 46);
    }

    /* ---- (4) The quad is DRAWN during zoom + flip, and NOT on the init frame. ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 50, 0, -1);
        int qx[4], qy[4], face; uint8_t type;
        CHECK("init frame: no quad drawn yet", re15_item_modal_quad(qx, qy, &type, &face) == 0);
        re15_item_modal_tick();                            /* state 1 -> 2 */
        re15_item_modal_tick();                            /* first zoom frame computed */
        int drew = re15_item_modal_quad(qx, qy, &type, &face);
        CHECK("zoom: quad drawn with the right item type", drew == 1 && type == 0x15);
        while (re15_item_modal_active()) re15_item_modal_tick();
    }

    if (g_fail) { printf("ITEM-MODAL: FAIL\n"); return 1; }
    printf("ITEM-MODAL: all checks passed\n");
    return 0;
}
