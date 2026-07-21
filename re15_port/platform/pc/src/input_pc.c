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
#define RE15_PAD_TRIANGLE 0x1000   /* C = Triangle (shares the DBG_NEXT bit) */
#define RE15_PAD_CIRCLE   0x2000   /* V = Circle (shares DBG_PREV) */
#define RE15_PAD_CROSS    0x4000   /* Shift = "X" — RUN modifier (held) + menu/dialog CANCEL
                                    * (virtual 0x8000 <- raw CROSS @0x80073dbc[15], wave-6 f4) */
#define RE15_PAD_SQUARE   0x8000   /* Enter = "Square" — ACTION + menu/dialog CONFIRM/fast-forward
                                    * (virtual 0x4000 <- raw SQUARE @0x80073dbc[14]); the YES/NO
                                    * toggle is d-pad L/R (virtual 0x3000), NOT Triangle/Circle */
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
        case 'S': case 's': return RE15_PAD_START;  /* status-screen open/close (wave 2) */
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
        int mash = 0;   /* 'B<secs>' = BUTTON-MASH: CROSS press-EDGE every other tick (the grab
                         * mash-escape probe; a constant hold is one edge and never re-triggers
                         * the FUN_80037024 press-edge test) */
        while (*p && ((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z'))) {
            if (*p == 'B' || *p == 'b') mash = 1; else bits |= script_bit_for(*p);
            p++;
        }
        char *end = NULL;
        double secs = strtod(p, &end);
        if (end == p) { p++; continue; }              /* malformed token: skip a char */
        p = end;
        int n = (int)(secs * fps + 0.5);
        for (int i = 0; i < n && ticks < RE15_SCRIPT_MAX_TICKS; i++)
            s_script[ticks++] = mash ? (((i & 1) ? 0 : RE15_PAD_CROSS) | bits) : bits;
    }
    s_script_len = ticks;
    fprintf(stderr, "[input-script] RE15_INPUT_SCRIPT=\"%s\" -> %d ticks @ %d fps, start frame %d\n",
            env, ticks, fps, s_script_start);
}

/* ==== GAMEPAD (SDL game controller) — Steam Deck / Xbox-style pad -> PSX pad bits ==============
 * The port's pad word IS the PSX digital-pad bitmask, so a controller maps 1:1 by POSITION (the
 * same correspondence a PSX emulator uses): A=Cross, B=Circle, X=Square, Y=Triangle; the
 * shoulders/triggers = L1/R1/L2/R2; the stick clicks = L3/R3; and the D-pad + left analog stick =
 * the four digital directions (RE1.5 tank controls). It works in EVERY screen because all input
 * flows through g_engine.pad_* — no screen reads the keyboard/controller directly.
 *
 * In-game (RE1.5): X/Square = ACTION + menu/dialog CONFIRM, A/Cross = RUN + menu/dialog CANCEL
 * (virtual remap @0x80073dbc, wave-6 finding 4), d-pad L/R = YES/NO toggle, RB/R1 = AIM,
 * Start = inventory. SELECT+START held together toggles fullscreen. */
static SDL_GameController *s_pad = NULL;

static void pad_ensure_open(void)
{
    if (s_pad && !SDL_GameControllerGetAttached(s_pad)) {   /* dropped -> release */
        SDL_GameControllerClose(s_pad);
        s_pad = NULL;
    }
    if (!s_pad) {                                           /* (re)open the first controller present */
        int n = SDL_NumJoysticks();
        for (int i = 0; i < n; i++)
            if (SDL_IsGameController(i)) { s_pad = SDL_GameControllerOpen(i); if (s_pad) break; }
    }
}

static uint16_t pad_read_bits(void)
{
    pad_ensure_open();
    if (!s_pad) return 0;
    SDL_GameController *c = s_pad;
    uint16_t b = 0;
    #define BTN(sdlbtn, bit) do { if (SDL_GameControllerGetButton(c, (sdlbtn))) b |= (uint16_t)(bit); } while (0)
    BTN(SDL_CONTROLLER_BUTTON_A,             RE15_PAD_CROSS);      /* confirm / RUN            */
    BTN(SDL_CONTROLLER_BUTTON_B,             RE15_PAD_CIRCLE);     /* dialog cancel toggle     */
    BTN(SDL_CONTROLLER_BUTTON_X,             RE15_PAD_SQUARE);     /* ACTION                   */
    BTN(SDL_CONTROLLER_BUTTON_Y,             RE15_PAD_TRIANGLE);   /* dialog cancel toggle     */
    BTN(SDL_CONTROLLER_BUTTON_BACK,          RE15_PAD_SELECT);
    BTN(SDL_CONTROLLER_BUTTON_START,         RE15_PAD_START);      /* inventory                */
    BTN(SDL_CONTROLLER_BUTTON_LEFTSHOULDER,  RE15_PAD_L1);
    BTN(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, RE15_PAD_R1);         /* AIM                      */
    BTN(SDL_CONTROLLER_BUTTON_DPAD_UP,       RE15_PAD_UP);
    BTN(SDL_CONTROLLER_BUTTON_DPAD_DOWN,     RE15_PAD_DOWN);
    BTN(SDL_CONTROLLER_BUTTON_DPAD_LEFT,     RE15_PAD_LEFT);
    BTN(SDL_CONTROLLER_BUTTON_DPAD_RIGHT,    RE15_PAD_RIGHT);
    BTN(SDL_CONTROLLER_BUTTON_LEFTSTICK,     0x0002);              /* L3 (PSX bit 0x0002)      */
    BTN(SDL_CONTROLLER_BUTTON_RIGHTSTICK,    0x0004);              /* R3 (PSX bit 0x0004)      */
    #undef BTN
    /* analog triggers -> L2/R2 (past a half-press) */
    if (SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_TRIGGERLEFT)  > 16000) b |= 0x0100;  /* L2 */
    if (SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16000) b |= 0x0200;  /* R2 */
    /* left analog stick -> digital directions (tank controls), ~40%% deadzone */
    {
        const int DZ = 13000;
        int lx = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTX);
        int ly = SDL_GameControllerGetAxis(c, SDL_CONTROLLER_AXIS_LEFTY);
        if (ly < -DZ) b |= RE15_PAD_UP;
        if (ly >  DZ) b |= RE15_PAD_DOWN;
        if (lx < -DZ) b |= RE15_PAD_LEFT;
        if (lx >  DZ) b |= RE15_PAD_RIGHT;
    }
    return b;
}

void re15_input_init(void) { /* SDL init is global, nothing per-input */ }

/* ==== DEBUG FUNCTION KEYS (F1-F12) — a channel SEPARATE from the gameplay pad word ============
 * Debug tools bind ONLY to the function keys, never to the game keys, so they can never collide
 * with play. Edge-detected (press this frame). Read with re15_input_debug_fkey(n), n = 1..12. */
static uint16_t s_dbg_cur = 0, s_dbg_pressed = 0;   /* bit (n-1) = F<n> */
int re15_input_debug_fkey(int n)
{
    return (n >= 1 && n <= 12) ? (int)((s_dbg_pressed >> (n - 1)) & 1u) : 0;
}

void re15_input_tick(void)
{
    g_engine.pad_previous = g_engine.pad_current;

    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    uint16_t bits = 0;
    if (keys) {
        /* ==== PSX-controller-EQUIVALENT keyboard layout =================================
         * Each key drives the SAME PSX pad bit the controller does, so keyboard and pad play
         * identically. D-pad on the left (arrows / WASD), the four ◇○✕□ face buttons + the
         * shoulders on the right of the board:
         *
         *   ↑ ↓ ← →  /  W A S D   = D-pad          (tank: ↑ forward, ↓ back, ← → turn)
         *   Shift                 = ✕ Cross        (RUN, held  +  menu confirm / dialog fast-forward)
         *   Space / Enter         = □ Square       (ACTION: examine / grab / fire)
         *   E                     = R1             (AIM, held)
         *   Q                     = L1
         *   C                     = △ Triangle     (dialog cursor / cancel)
         *   V                     = ○ Circle       (dialog cursor / cancel)
         *   I                     = Start          (inventory / weapon-select)
         *   Tab                   = Select
         *   [  ]                  = L2 / R2
         *   F11 / Alt+Enter       = toggle fullscreen (window is the default)
         * ALL debug is on the FUNCTION keys only — never the game keys: F1/F2 = prev/next room,
         * F3 = motion-lock, F4/F5 = clip cycle (F3-F5 need RE15_MOTION_DEBUG). See re15_input_debug_fkey.
         * (Tell me if you want any game key different — this is a single lookup table to edit.) */
        if (keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W]) bits |= RE15_PAD_UP;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) bits |= RE15_PAD_RIGHT;
        if (keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S]) bits |= RE15_PAD_DOWN;
        if (keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A]) bits |= RE15_PAD_LEFT;
        if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT]) bits |= RE15_PAD_CROSS;    /* ✕ Run / OK   */
        if (keys[SDL_SCANCODE_SPACE] || keys[SDL_SCANCODE_RETURN] ||
            keys[SDL_SCANCODE_KP_ENTER])                            bits |= RE15_PAD_SQUARE;   /* □ Action     */
        if (keys[SDL_SCANCODE_E])         bits |= RE15_PAD_R1;        /* R1 Aim                             */
        if (keys[SDL_SCANCODE_Q])         bits |= RE15_PAD_L1;        /* L1                                 */
        if (keys[SDL_SCANCODE_C])         bits |= RE15_PAD_TRIANGLE;  /* △ (dialog toggle / cancel, 0x3000) */
        if (keys[SDL_SCANCODE_V])         bits |= RE15_PAD_CIRCLE;    /* ○                                  */
        if (keys[SDL_SCANCODE_I])         bits |= RE15_PAD_START;     /* Start = inventory                  */
        if (keys[SDL_SCANCODE_TAB])       bits |= RE15_PAD_SELECT;    /* Select                             */
        if (keys[SDL_SCANCODE_LEFTBRACKET])  bits |= 0x0100;         /* L2                                 */
        if (keys[SDL_SCANCODE_RIGHTBRACKET]) bits |= 0x0200;         /* R2                                 */
    }

    /* F1-F12 -> the debug channel (edge-detected), completely OFF the gameplay pad word above. */
    {
        static const int FK[12] = { SDL_SCANCODE_F1, SDL_SCANCODE_F2, SDL_SCANCODE_F3, SDL_SCANCODE_F4,
                                    SDL_SCANCODE_F5, SDL_SCANCODE_F6, SDL_SCANCODE_F7, SDL_SCANCODE_F8,
                                    SDL_SCANCODE_F9, SDL_SCANCODE_F10, SDL_SCANCODE_F11, SDL_SCANCODE_F12 };
        uint16_t dbg = 0;
        if (keys) for (int i = 0; i < 12; i++) if (keys[FK[i]]) dbg |= (uint16_t)(1u << i);
        s_dbg_pressed = (uint16_t)(dbg & ~s_dbg_cur);
        s_dbg_cur = dbg;
    }

    /* GAMEPAD: OR the Steam Deck / Xbox-style controller bits into the same pad word (before the
     * scripted override, exactly like the keyboard) so a controller drives every screen. */
    bits |= pad_read_bits();

    /* SELECT+START held together = toggle fullscreen (the controller equivalent of F11); consume the
     * two bits while the combo is held so it does not also open the inventory / motion-debug. */
    {
        static int s_fs_prev = 0;
        int combo = ((bits & RE15_PAD_SELECT) && (bits & RE15_PAD_START)) ? 1 : 0;
        if (combo) {
            if (!s_fs_prev) { extern void re15_render_pc_toggle_fullscreen(void); re15_render_pc_toggle_fullscreen(); }
            bits &= (uint16_t)~(RE15_PAD_SELECT | RE15_PAD_START);
        }
        s_fs_prev = combo;
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
