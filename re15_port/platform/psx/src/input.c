/*
 * RE1.5 Rebuilt — Input (Phase 4.1).
 *
 * Read controller pad state via BIOS pad init/read.
 * Phase 4.1: digital pad only. Phase 4.7+ will add analog if needed.
 */

#include <stdint.h>
#include <psxapi.h>
#include <psxpad.h>
#include "re15_engine.h"

static uint8_t s_pad_buf[2][34];

void re15_input_init(void)
{
    InitPAD(s_pad_buf[0], 34, s_pad_buf[1], 34);
    StartPAD();
    ChangeClearPAD(0);   /* don't autoclear pad on VSync — read it ourselves */
}

void re15_input_tick(void)
{
    PADTYPE *pad = (PADTYPE *) s_pad_buf[0];

    g_engine.pad_previous = g_engine.pad_current;

    if (pad->stat == 0) {
        /* Pad bits are active-low in the BIOS read — invert to get active-high */
        g_engine.pad_current = ~pad->btn;
    } else {
        g_engine.pad_current = 0;
    }

    g_engine.pad_pressed  = g_engine.pad_current & ~g_engine.pad_previous;
    g_engine.pad_released = ~g_engine.pad_current & g_engine.pad_previous;
}
