/*
 * RE1.5 Rebuilt — PC input backend (Phase 4.4.5, 2026-05-19).
 *
 * Keyboard → PSX-style pad bits. The pad word in g_engine.pad_current
 * mirrors the layout of PSn00bSDK's PAD_* constants so cross-target
 * gameplay code can test the same bits on both platforms.
 *
 * Mapping (Phase 4.4.5 minimum for walking demo):
 *   Arrow Up / W    → PAD_UP    (0x0010)
 *   Arrow Right / D → PAD_RIGHT (0x0020)
 *   Arrow Down / S  → PAD_DOWN  (0x0040)
 *   Arrow Left / A  → PAD_LEFT  (0x0080)
 *
 * (Action buttons Z/X/A/S/Enter/Shift to follow in Phase 4.4.6+.)
 */
#include <SDL.h>
#include "re15_engine.h"

#define RE15_PAD_UP       0x0010
#define RE15_PAD_RIGHT    0x0020
#define RE15_PAD_DOWN     0x0040
#define RE15_PAD_LEFT     0x0080
#define RE15_PAD_L1       0x0400   /* Q  = "L1" — cycle cut backward */
#define RE15_PAD_R1       0x0800   /* E  = "R1" — cycle cut forward */
#define RE15_PAD_CROSS    0x4000   /* Shift = "X" — RUN modifier (held) */
#define RE15_PAD_SQUARE   0x8000   /* Enter = "Square" — ACTION button */
/* Phase 4.5.13-RE2 H5 (2026-05-21): motion-debug keys */
#define RE15_PAD_SELECT   0x0001   /* Tab    = toggle motion-debug-lock */
#define RE15_PAD_DBG_NEXT 0x1000   /* PageUp   = clip +1 */
#define RE15_PAD_DBG_PREV 0x2000   /* PageDown = clip -1 */

void re15_input_init(void) { /* SDL init is global, nothing per-input */ }

void re15_input_tick(void)
{
    g_engine.pad_previous = g_engine.pad_current;

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    uint16_t bits = 0;
    if (keys) {
        if (keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W]) bits |= RE15_PAD_UP;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) bits |= RE15_PAD_RIGHT;
        if (keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S]) bits |= RE15_PAD_DOWN;
        if (keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A]) bits |= RE15_PAD_LEFT;
        /* RUN = Shift held (mapped to the CROSS bit the shared player FSM reads). */
        if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) bits |= RE15_PAD_CROSS;
        /* ACTION = Enter (mapped to the SQUARE bit the action gate reads). */
        if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_KP_ENTER]) bits |= RE15_PAD_SQUARE;
        if (keys[SDL_SCANCODE_Q])         bits |= RE15_PAD_L1;
        if (keys[SDL_SCANCODE_E])         bits |= RE15_PAD_R1;
        /* DEV room-browser (globalization 2026-06-13): [ = prev room, ] = next room.
         * Cycles through every room in the shared tree (re15_room_list.h). Uses the
         * FREE L2/R2 bits (0x0100/0x0200) — NOT 0x4000/0x8000, which are CROSS (=RUN,
         * the sprint key Shift) and SQUARE (=ACTION), so the old binding made sprinting
         * change rooms. L2/R2 are unmapped/unused, so no collision. */
        if (keys[SDL_SCANCODE_LEFTBRACKET])  bits |= 0x0100;   /* L2 = prev room */
        if (keys[SDL_SCANCODE_RIGHTBRACKET]) bits |= 0x0200;   /* R2 = next room */
    }
    g_engine.pad_current = bits;

    g_engine.pad_pressed  = g_engine.pad_current & ~g_engine.pad_previous;
    g_engine.pad_released = ~g_engine.pad_current & g_engine.pad_previous;
}
