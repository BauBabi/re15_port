/* test_msg_dialog.c — byte-true dialog FSM buttons + cadence (audit wf_6aad95ad).
 *
 * (1) YES/NO toggle on the FACE buttons (0x3000), not the D-pad L/R (@0x800285b8).
 * (2) YES/NO confirm on CROSS (0x4000), not SQUARE (@0x80028570).
 * (3) fast-forward on CROSS held (@0x80028214) reveals TWO glyphs/frame (s2=2 @0x80028238).
 * (4) PAGE_WAIT first-glyph timer = 1 (@0x80028494), not scroll.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "re15_scd.h"
#include "re15_msg.h"
#include "re15_aot.h"

extern uint16_t g_scd_pad_edge;
extern uint16_t g_scd_pad_held;

static int g_fail = 0;
#define CHECK(name, cond) do { if (!(cond)) { fprintf(stderr, "FAIL: %s\n", name); g_fail = 1; } \
                               else printf("  PASS: %s\n", name); } while (0)

/* drive the FSM one frame with a given edge/held pad word */
static void tick(uint16_t edge, uint16_t held)
{
    g_scd_pad_edge = edge; g_scd_pad_held = held;
    const unsigned char *raw; int len, id;
    re15_msg_tick(&raw, &len, &id);
}

int main(void)
{
    printf("=== byte-true dialog FSM buttons + cadence (audit wf_6aad95ad) ===\n");

    /* --- (1)+(2): YES/NO toggle + confirm buttons --- */
    {
        scd_vm_init(); re15_aot_init();
        /* choice blob: one glyph, then 0x03 (choice), 0x01 0x00 (end). */
        uint8_t blob[] = { 0x30, 0x03, 0x01, 0x00 };
        re15_msg_install_text(7, blob, sizeof blob);
        re15_dialog_open(7, 1);
        for (int i = 0; i < 20 && g_scd.message_fsm != 3; i++) tick(0, 0);   /* run to SELECT */
        CHECK("dialog reaches SELECT (fsm 3)", g_scd.message_fsm == 3);

        uint8_t c0 = g_scd.message_choice;
        tick(0x0080, 0);                       /* D-pad LEFT must NOT toggle (the old wrong button) */
        CHECK("toggle: D-pad LEFT does NOT flip the cursor", g_scd.message_choice == c0);
        tick(0x2000, 0);                       /* CIRCLE (face button) toggles */
        CHECK("toggle: CIRCLE (0x2000) flips the cursor", g_scd.message_choice == (c0 ^ 1));
        tick(0x1000, 0);                       /* TRIANGLE toggles back */
        CHECK("toggle: TRIANGLE (0x1000) flips the cursor", g_scd.message_choice == c0);

        tick(0x8000, 0);                       /* SQUARE must NOT confirm (still selecting) */
        CHECK("confirm: SQUARE does NOT confirm", g_scd.message_fsm == 3);
        tick(0x4000, 0);                       /* CROSS confirms */
        CHECK("confirm: CROSS (0x4000) confirms (fsm leaves SELECT)", g_scd.message_fsm != 3);
    }

    /* --- (3): fast-forward reveals TWO glyphs per frame on CROSS held --- */
    {
        scd_vm_init(); re15_aot_init();
        uint8_t blob[] = { 0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30, 0x01, 0x00 };   /* 8 glyphs */
        re15_msg_install_text(8, blob, sizeof blob);
        re15_dialog_open(8, 1);
        /* run to the first steady TYPING reveal (timer drained). */
        for (int i = 0; i < 4 && g_scd.message_fsm != 0; i++) tick(0, 0);
        g_scd.message_timer = 0;
        uint16_t p0 = g_scd.message_parse;
        tick(0x4000, 0x4000);                  /* CROSS held -> fast-forward -> 2 glyphs this frame */
        int delta = (int)g_scd.message_parse - (int)p0;
        CHECK("fast-forward (CROSS held) reveals 2 glyphs/frame", delta == 2);
        /* control: no button -> 1 glyph */
        g_scd.message_timer = 0;
        uint16_t p1 = g_scd.message_parse;
        tick(0, 0);
        CHECK("normal cadence reveals 1 glyph/frame", (int)g_scd.message_parse - (int)p1 == 1);
    }

    /* --- (4): PAGE_WAIT first-glyph timer = 1 (not scroll) --- */
    {
        scd_vm_init(); re15_aot_init();
        uint8_t blob[] = { 0x30, 0x02, 0x00, 0x30, 0x01, 0x00 };   /* glyph, page-wait, glyph, end */
        re15_msg_install_text(9, blob, sizeof blob);
        re15_dialog_open(9, 1);
        for (int i = 0; i < 8 && g_scd.message_fsm != 1; i++) tick(0, 0);   /* run to PAGE_WAIT */
        CHECK("dialog reaches PAGE_WAIT (fsm 1)", g_scd.message_fsm == 1);
        tick(0x4000, 0);                       /* dismiss the page with CROSS */
        CHECK("PAGE_WAIT completion sets the first-glyph timer to 1", g_scd.message_timer == 1);
    }

    if (g_fail) { printf("MSG-DIALOG: FAIL\n"); return 1; }
    printf("MSG-DIALOG: all checks passed\n");
    return 0;
}
