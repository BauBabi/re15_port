/* test_item_modal.c — byte-true item-get modal (FUN_8001db28) with the Yes/No message box.
 *
 * The pickup presentation FREEZES the game, zooms/spins the item PICTURE in (17f), flips it like a coin
 * (9f), then opens a MESSAGE BOX — "WILL YOU TAKE THE <item>." (Yes/No) if there's room, or "YOU CAN'T
 * CARRY ANY MORE ITEMS" when full — and grants the item ONLY on Yes (deferred to state 7, @0x8001e068
 * andi 0x1 = the "No" flag). state 6 is PLAYER-GATED (dismiss on input, not a timer). RE'd workflows
 * wq41xdnn2 + wf_9e42f4cb. Pad edges: 0x3000 toggle Yes/No, 0x4000 CROSS confirm, 0xc000 dismiss.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_item_modal.h"
#include "re15_inventory.h"

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { printf("  FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

static int inv_has(uint8_t id)
{
    for (int i = 0; i < RE15_INV_MAX_SLOTS; i++)
        if (g_inv.slots[i].id == id) return 1;
    return 0;
}

/* Tick (with no input) until the message box (state 6) is up, counting zoom/flip frames. */
static void run_to_prompt(int *out_zoom, int *out_flip)
{
    int zoom = 0, flip = 0, guard = 0;
    while (re15_item_modal_active() && re15_item_modal_state() != 6 && guard++ < 200) {
        uint8_t st = re15_item_modal_state();
        if (st == 2) zoom++;
        if (st == 4) flip++;
        re15_item_modal_tick(0);   /* no input during zoom/flip */
    }
    if (out_zoom) *out_zoom = zoom;
    if (out_flip) *out_flip = flip;
}

int main(void)
{
    printf("=== byte-true item-get modal + Yes/No message box (wq41xdnn2 + wf_9e42f4cb) ===\n");

    /* ---- (1) NORMAL: zoom/flip -> take-prompt HOLDS for input -> CROSS(Yes) grants at the end ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 50, 0, -1);       /* H.GUN BULLETS x50 */
        CHECK("start -> state 1 / active", re15_item_modal_state() == 1 && re15_item_modal_active());
        CHECK("grant DEFERRED (not in inventory yet)", !inv_has(0x15));

        int zoom = 0, flip = 0;
        run_to_prompt(&zoom, &flip);
        CHECK("zoom ran 17 frames", zoom == 17);
        CHECK("flip ran 9 frames",  flip == 9);
        CHECK("reached the message box (state 6)", re15_item_modal_state() == 6);
        CHECK("prompt = Yes/No take-prompt (1)", re15_item_modal_prompt(NULL, NULL) == 1);

        /* state 6 is PLAYER-GATED: no input -> holds indefinitely, item still not granted. */
        for (int i = 0; i < 30; i++) re15_item_modal_tick(0);
        CHECK("state 6 HOLDS without input", re15_item_modal_active() && re15_item_modal_state() == 6);
        CHECK("still not granted while the prompt is up", !inv_has(0x15));

        re15_item_modal_tick(0x4000);                 /* CROSS confirm (Yes) */
        CHECK("CROSS(Yes) grants + ends the modal", !re15_item_modal_active() && inv_has(0x15));
    }

    /* ---- (2) "No": toggle to No then confirm -> item NOT taken, box shrinks away ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 50, 0, -1);
        run_to_prompt(NULL, NULL);
        CHECK("No-path: take-prompt up", re15_item_modal_prompt(NULL, NULL) == 1);
        re15_item_modal_tick(0x1000 | 0x4000);        /* TRIANGLE toggle -> No, CROSS confirm */
        int saw_shrink = 0, guard = 0;
        while (re15_item_modal_active() && guard++ < 200) {
            if (re15_item_modal_state() == 8) saw_shrink++;
            re15_item_modal_tick(0);
        }
        CHECK("No: shrink-away ran 17 frames", saw_shrink == 17);
        CHECK("No: item 0x15 NOT granted (declined)", !inv_has(0x15));
    }

    /* ---- (3) FULL inventory: "can't-carry" line, dismiss, item stays, shrink-away ---- */
    {
        re15_inv_init();
        for (int i = 0; i < RE15_INV_MAX_SLOTS; i++) { g_inv.slots[i].id = 0x22; g_inv.slots[i].qty = 1; }
        re15_item_modal_start(0x24, 1, 0, -1);
        run_to_prompt(NULL, NULL);
        CHECK("full: can't-carry prompt (2)", re15_item_modal_prompt(NULL, NULL) == 2);
        re15_item_modal_tick(0x4000);                 /* any confirm dismisses */
        int saw_shrink = 0, guard = 0;
        while (re15_item_modal_active() && guard++ < 200) {
            if (re15_item_modal_state() == 8) saw_shrink++;
            re15_item_modal_tick(0);
        }
        CHECK("full: shrink-away ran 17 frames", saw_shrink == 17);
        CHECK("full: item 0x24 NOT granted", !inv_has(0x24));
    }

    /* ---- (4) re-entry GUARD: a second start while running is ignored ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 10, 0, -1);
        re15_item_modal_tick(0);                       /* into state 2 */
        uint8_t before = re15_item_modal_state();
        re15_item_modal_start(0x03, 1, 0, -1);         /* must be ignored */
        CHECK("guard: second start ignored", re15_item_modal_state() == before);
        int guard = 0;
        while (re15_item_modal_active() && guard++ < 200) re15_item_modal_tick(0x4000);  /* Yes to finish */
        CHECK("guard: only the FIRST item granted (0x15, not 0x03)", inv_has(0x15) && !inv_has(0x03));
    }

    /* ---- (5) the item PICTURE quad is drawn during zoom, with the right item type ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 50, 0, -1);
        int qx[4], qy[4], face; uint8_t type;
        CHECK("init frame: no quad yet", re15_item_modal_quad(qx, qy, &type, &face) == 0);
        re15_item_modal_tick(0);                       /* state 1 -> 2 */
        re15_item_modal_tick(0);                       /* first zoom frame */
        int drew = re15_item_modal_quad(qx, qy, &type, &face);
        CHECK("zoom: quad drawn with the right item type", drew == 1 && type == 0x15);
        int guard = 0;
        while (re15_item_modal_active() && guard++ < 200) re15_item_modal_tick(0x4000);
    }

    if (g_fail) { printf("ITEM-MODAL: FAIL\n"); return 1; }
    printf("ITEM-MODAL: all checks passed\n");
    return 0;
}
