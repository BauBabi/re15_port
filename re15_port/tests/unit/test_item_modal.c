/* test_item_modal.c — byte-true item-get modal (FUN_8001db28) with the Yes/No message box.
 *
 * The pickup presentation FREEZES the game, zooms/spins the item PICTURE in (17f), flips it like a coin
 * (9f), then opens a MESSAGE BOX — "WILL YOU TAKE THE <item>." (Yes/No) if there's room, or "YOU CAN'T
 * CARRY ANY MORE ITEMS" when full — and grants the item ONLY on Yes (deferred to state 7, @0x8001e068
 * andi 0x1 = the "No" flag). state 6 is PLAYER-GATED (dismiss on input, not a timer). RE'd workflows
 * wq41xdnn2 + wf_9e42f4cb. VIRTUAL pad edges (wave-6 f4): 0x3000 (raw d-pad L/R) toggle Yes/No,
 * 0x4000 (raw SQUARE) confirm, 0xc000 dismiss.
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
        re15_item_modal_tick(0, 0);   /* no input during zoom/flip */
    }
    if (out_zoom) *out_zoom = zoom;
    if (out_flip) *out_flip = flip;
}

/* Tick (no input) through the typewriter until the prompt has fully revealed (Yes/No selectable). */
static void run_reveal(void)
{
    int guard = 0;
    while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && guard++ < 400)
        re15_item_modal_tick(0, 0);
}

int main(void)
{
    printf("=== byte-true item-get modal + Yes/No message box (wq41xdnn2 + wf_9e42f4cb) ===\n");

    /* ---- (1) NORMAL: zoom/flip -> take-prompt HOLDS for input -> confirm(Yes) grants at the end ---- */
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
        CHECK("typewriter starts un-revealed", re15_item_modal_reveal() == 0 && !re15_item_modal_prompt_ready());

        /* TYPEWRITER: the text types out (2 frames/glyph); the game HOLDS in state 6 the whole time and
         * accepts no confirm — feeding the confirm bit during typing must NOT grant. */
        int guard = 0;
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && guard++ < 400) {
            re15_item_modal_tick(0x4000, 0);             /* confirm bit during typing -> ignored */
            if (inv_has(0x15)) break;                 /* must never happen mid-typing */
        }
        CHECK("typewriter revealed the prompt while state 6 held", re15_item_modal_prompt_ready()
              && re15_item_modal_state() == 6);
        CHECK("confirm was IGNORED during typing (not granted yet)", !inv_has(0x15));
        CHECK("reveal reached the full text (>= 30 glyphs)", re15_item_modal_reveal() >= 30);

        re15_item_modal_tick(0x4000, 0);                 /* text is up -> virtual confirm = Yes */
        CHECK("confirm(Yes) grants + ends the modal", !re15_item_modal_active() && inv_has(0x15));
    }

    /* ---- (2) "No": toggle to No then confirm -> item NOT taken, box shrinks away ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 50, 0, -1);
        run_to_prompt(NULL, NULL);
        CHECK("No-path: take-prompt up", re15_item_modal_prompt(NULL, NULL) == 1);
        run_reveal();                                 /* type the text out first */
        re15_item_modal_tick(0x1000 | 0x4000, 0);        /* virt menu-L toggle -> No, then confirm */
        int saw_shrink = 0, guard = 0;
        while (re15_item_modal_active() && guard++ < 200) {
            if (re15_item_modal_state() == 8) saw_shrink++;
            re15_item_modal_tick(0, 0);
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
        run_reveal();                                 /* type the text out first */
        re15_item_modal_tick(0x4000, 0);                 /* any confirm dismisses */
        int saw_shrink = 0, guard = 0;
        while (re15_item_modal_active() && guard++ < 200) {
            if (re15_item_modal_state() == 8) saw_shrink++;
            re15_item_modal_tick(0, 0);
        }
        CHECK("full: shrink-away ran 17 frames", saw_shrink == 17);
        CHECK("full: item 0x24 NOT granted", !inv_has(0x24));
    }

    /* ---- (4) re-entry GUARD: a second start while running is ignored ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 10, 0, -1);
        re15_item_modal_tick(0, 0);                       /* into state 2 */
        uint8_t before = re15_item_modal_state();
        re15_item_modal_start(0x03, 1, 0, -1);         /* must be ignored */
        CHECK("guard: second start ignored", re15_item_modal_state() == before);
        int guard = 0;
        while (re15_item_modal_active() && guard++ < 200) re15_item_modal_tick(0x4000, 0);  /* Yes to finish */
        CHECK("guard: only the FIRST item granted (0x15, not 0x03)", inv_has(0x15) && !inv_has(0x03));
    }

    /* ---- (5) the item PICTURE quad is drawn during zoom, with the right item type ---- */
    {
        re15_inv_init();
        re15_item_modal_start(0x15, 50, 0, -1);
        int qx[4], qy[4], face; uint8_t type;
        CHECK("init frame: no quad yet", re15_item_modal_quad(qx, qy, &type, &face) == 0);
        re15_item_modal_tick(0, 0);                       /* state 1 -> 2 */
        re15_item_modal_tick(0, 0);                       /* first zoom frame */
        int drew = re15_item_modal_quad(qx, qy, &type, &face);
        CHECK("zoom: quad drawn with the right item type", drew == 1 && type == 0x15);
        int guard = 0;
        while (re15_item_modal_active() && guard++ < 200) re15_item_modal_tick(0x4000, 0);
    }

    /* ---- (6) TEXT FAST-FORWARD (byte-true FUN_80028134 @0x800281d8-0x80028238) ----
     * The item prompt is typed by the SHARED message VM (state 5 opens it as type 0x100
     * @0x8001df6c-94, and type 0x100 sets the FF-enable byte DAT_800b8522 = 0x80 @0x80027f28), so
     * HOLDING virtual 0x4000 (= physical SQUARE, preset @0x80073dbc[14]) must accelerate it 4x:
     *   timer -= 4 instead of 1 (@0x80028228 vs @0x800281f0) AND 2 glyphs per expiry
     *   (@0x80028238, gated on reload < 4 @0x8002822c; reload = 2<<s1 = 2 @0x800281b0-c4).
     * With seed 1 (@0x800281a0-ac) and reload 2, N glyphs cost:
     *   held   : 1 + ceil((N-1)/2) ticks   (first tick lands the timer exactly on 0 -> budget 1)
     *   normal : 1 + 2*(N-1) ticks
     * EDGE-only (the pre-existing case (1) feed) must NOT accelerate — the VM reads the HELD word
     * DAT_800ac768 (@0x8002820c), never the edge word. */
    {
        int n_norm = 0, n_held = 0, n_edge = 0, glyphs = 0, guard;

        re15_inv_init(); re15_item_modal_start(0x15, 50, 0, -1); run_to_prompt(NULL, NULL);
        glyphs = re15_item_modal_reveal_total();
        guard = 0;
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && guard++ < 500) {
            re15_item_modal_tick(0, 0); n_norm++;
        }
        while (re15_item_modal_active() && guard++ < 600) re15_item_modal_tick(0x4000, 0);

        re15_inv_init(); re15_item_modal_start(0x15, 50, 0, -1); run_to_prompt(NULL, NULL);
        guard = 0;
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && guard++ < 500) {
            re15_item_modal_tick(0, 0x4000); n_held++;      /* HELD virtual 0x4000 -> fast-forward */
        }
        while (re15_item_modal_active() && guard++ < 600) re15_item_modal_tick(0x4000, 0);

        re15_inv_init(); re15_item_modal_start(0x15, 50, 0, -1); run_to_prompt(NULL, NULL);
        guard = 0;
        while (re15_item_modal_active() && !re15_item_modal_prompt_ready() && guard++ < 500) {
            re15_item_modal_tick(0x4000, 0); n_edge++;      /* EDGE only -> must stay at base speed */
        }
        while (re15_item_modal_active() && guard++ < 600) re15_item_modal_tick(0x4000, 0);

        printf("  [ff] glyphs=%d  normal=%d ticks  held=%d ticks  edge-only=%d ticks\n",
               glyphs, n_norm, n_held, n_edge);
        CHECK("ff: prompt has glyphs to type", glyphs > 0);
        CHECK("ff: base cadence = 1 + 2*(N-1) ticks (seed 1 @0x800281a0-ac, reload 2 @0x800281b0-c4)",
              n_norm == 1 + 2 * (glyphs - 1));
        CHECK("ff: HELD 0x4000 = 1 + ceil((N-1)/2) ticks (-4/tick @0x80028228 + 2 glyphs @0x80028238)",
              n_held == 1 + (glyphs - 1 + 1) / 2);
        /* Steady-state speed-up is exactly 4x (-4 vs -1 per tick x 2 vs 1 glyph); the measured
         * TOTALS carry the shared 1-tick seed frame, so 32 glyphs = 63 vs 17 ticks (3.7x). */
        CHECK("ff: HELD is >3x faster than base", n_held * 3 < n_norm);
        CHECK("ff: EDGE-only does NOT accelerate (VM reads HELD DAT_800ac768 @0x8002820c)",
              n_edge == n_norm);
    }

    if (g_fail) { printf("ITEM-MODAL: FAIL\n"); return 1; }
    printf("ITEM-MODAL: all checks passed\n");
    return 0;
}
