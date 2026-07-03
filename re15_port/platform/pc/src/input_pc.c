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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "re15_engine.h"

#define RE15_PAD_UP       0x0010
#define RE15_PAD_RIGHT    0x0020
#define RE15_PAD_DOWN     0x0040
#define RE15_PAD_LEFT     0x0080
#define RE15_PAD_L1       0x0400   /* Q  = "L1" — reserved (currently unused) */
#define RE15_PAD_R1       0x0800   /* E  = "R1" — AIM / raise the weapon (hold; game_step + player aim FSM) */
#define RE15_PAD_CROSS    0x4000   /* Shift = "X" — RUN modifier (held) */
#define RE15_PAD_SQUARE   0x8000   /* Enter = "Square" — ACTION button */
/* Phase 4.5.13-RE2 H5 (2026-05-21): motion-debug keys */
#define RE15_PAD_SELECT   0x0001   /* Tab    = toggle motion-debug-lock */
#define RE15_PAD_START    0x0008   /* I      = open the inventory/weapon-select (== engine RE15_PAD_BIT_START) */
#define RE15_PAD_DBG_NEXT 0x1000   /* PageUp   = clip +1 */
#define RE15_PAD_DBG_PREV 0x2000   /* PageDown = clip -1 */

/* ==== DETERMINISTIC SCRIPTED INPUT (RE15_INPUT_SCRIPT) — the parity-run driver ============
 * Feeds a fixed pad-bit timeline so the port replays the EXACT same input as the DuckStation
 * tank-path capture (re15-room-capture --path). Same input -> the port MUST produce the same
 * result as the PSX original; any divergence is a port bug (memory: reai-v2-parity-oracle).
 *
 * RE15_INPUT_SCRIPT = comma-separated tokens "<letters><seconds>", e.g. "R0.5,U2,R0.3,U6"
 * (identical string to the DuckStation --path). Letters (may combine, e.g. "XU2" = run fwd):
 *   U=UP(fwd) D=DOWN(back) L=LEFT(rotate) R=RIGHT(rotate) X=CROSS(run) A=SQUARE(action)
 *   M=R1(aim) W=wait(no bits). seconds*fps frames (fps = RE15_FPS or 30 = the PSX 30Hz cadence).
 * RE15_INPUT_SCRIPT_START = lead-in frames before the script begins (default 90 = 3s room
 *   settle/fade-in). After the timeline ends the pad returns to 0 (player idle). */
#define RE15_SCRIPT_MAX_TICKS 8192
static uint16_t s_script[RE15_SCRIPT_MAX_TICKS];
static int      s_script_len   = 0;      /* number of scripted ticks, -1 = none */
static int      s_script_start = 90;
static int      s_script_init  = 0;

static uint16_t script_bit_for(char c)
{
    switch (c) {
        case 'U': case 'u': return RE15_PAD_UP;
        case 'D': case 'd': return RE15_PAD_DOWN;
        case 'L': case 'l': return RE15_PAD_LEFT;
        case 'R': case 'r': return RE15_PAD_RIGHT;
        case 'X': case 'x': return RE15_PAD_CROSS;
        case 'A': case 'a': return RE15_PAD_SQUARE;
        case 'M': case 'm': return RE15_PAD_R1;
        case 'W': case 'w': default: return 0;   /* wait / unknown -> no bit */
    }
}

static void script_parse_once(void)
{
    s_script_init = 1;
    s_script_len  = -1;
    const char *env = getenv("RE15_INPUT_SCRIPT");
    if (!env || !*env) return;

    int fps = 30;
    const char *fenv = getenv("RE15_FPS");
    if (fenv && *fenv) { int f = atoi(fenv); if (f >= 15 && f <= 240) fps = f; }
    const char *senv = getenv("RE15_INPUT_SCRIPT_START");
    if (senv && *senv) { int s = atoi(senv); if (s >= 0) s_script_start = s; }

    int ticks = 0;
    const char *p = env;
    while (*p && ticks < RE15_SCRIPT_MAX_TICKS) {
        while (*p == ',' || *p == ' ') p++;
        if (!*p) break;
        uint16_t bits = 0;
        while (*p && ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z'))) {
            bits |= script_bit_for(*p); p++;
        }
        char *end = NULL;
        double secs = strtod(p, &end);
        if (end == p) { p++; continue; }              /* malformed token: skip a char */
        p = end;
        int n = (int)(secs * fps + 0.5);
        for (int i = 0; i < n && ticks < RE15_SCRIPT_MAX_TICKS; i++) s_script[ticks++] = bits;
    }
    s_script_len = ticks;
    fprintf(stderr, "[input-script] RE15_INPUT_SCRIPT=\"%s\" -> %d ticks @ %d fps, start frame %d\n",
            env, ticks, fps, s_script_start);
}

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
        /* START = I (user-chosen PC binding, 2026-07-02): opens the inventory/weapon-select menu.
         * PSX Start (bit 0x0008); inert until the inventory screen consumes it. */
        if (keys[SDL_SCANCODE_I])         bits |= RE15_PAD_START;
        /* DEV room-browser (globalization 2026-06-13): [ = prev room, ] = next room.
         * Cycles through every room in the shared tree (re15_room_list.h). Uses the
         * FREE L2/R2 bits (0x0100/0x0200) — NOT 0x4000/0x8000, which are CROSS (=RUN,
         * the sprint key Shift) and SQUARE (=ACTION), so the old binding made sprinting
         * change rooms. L2/R2 are unmapped/unused, so no collision. */
        if (keys[SDL_SCANCODE_LEFTBRACKET])  bits |= 0x0100;   /* L2 = prev room */
        if (keys[SDL_SCANCODE_RIGHTBRACKET]) bits |= 0x0200;   /* R2 = next room */
    }

    /* SCRIPTED-INPUT OVERRIDE (parity run): replace the live keyboard bits with the fixed
     * timeline for this frame. Keyed on g_engine.frame_count so it is deterministic; the
     * pad_pressed/pad_released derivation below stays correct (it diffs bits vs pad_previous). */
    if (!s_script_init) script_parse_once();
    if (s_script_len > 0) {
        int t = (int)g_engine.frame_count - s_script_start;
        bits = (t >= 0 && t < s_script_len) ? s_script[t] : 0;
    }

    g_engine.pad_current = bits;

    g_engine.pad_pressed  = g_engine.pad_current & ~g_engine.pad_previous;
    g_engine.pad_released = ~g_engine.pad_current & g_engine.pad_previous;
}
