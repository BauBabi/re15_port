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
#define RE15_PAD_L1       0x0400   /* Q  = "L1" (DuckStation Pad1) — reserved (currently unused) */
#define RE15_PAD_R1       0x0800   /* E  = "R1" — AIM / raise the weapon (hold; game_step + player aim FSM) */
#define RE15_PAD_TRIANGLE 0x1000   /* I = Triangle (shares the DBG_NEXT bit) */
#define RE15_PAD_CIRCLE   0x2000   /* L = Circle (shares DBG_PREV) */
#define RE15_PAD_CROSS    0x4000   /* K (also Shift) = "X" — RUN modifier (held) + menu/dialog CANCEL
                                    * (virtual 0x8000 <- raw CROSS @0x80073dbc[15], wave-6 f4) */
#define RE15_PAD_SQUARE   0x8000   /* J = "Square" — ACTION + menu/dialog CONFIRM/fast-forward
                                    * (virtual 0x4000 <- raw SQUARE @0x80073dbc[14]); the YES/NO
                                    * toggle is d-pad L/R (virtual 0x3000), NOT Triangle/Circle */
/* Phase 4.5.13-RE2 H5 (2026-05-21): motion-debug keys */
#define RE15_PAD_SELECT   0x0001   /* Backspace = toggle motion-debug-lock */
#define RE15_PAD_START    0x0008   /* Enter  = open the inventory/weapon-select (== engine RE15_PAD_BIT_START) */
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
static int      s_input_ticks  = 0;   /* rendered frames in ALL modes (front-end included) */

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
        /* Debug-Menue ("UTILITY MENU" @0x80014444): SELECT oeffnet, Dreieck wechselt die Stage.
         * Die Zeilen-/Raum-Navigation laeuft dort ueber das HELD-Halbwort 0x800AC760, ein Token
         * muss deshalb exakt so viele Frames lang sein wie Schritte gewuenscht sind. */
        case 'E': case 'e': return RE15_PAD_SELECT;
        case 'T': case 't': return RE15_PAD_TRIANGLE;
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

/* Ein Joystick, den SDL nicht als GameController kennt (kein Eintrag in der Mapping-DB), wurde
 * bisher STILL uebersprungen — das Pad steckt, tut aber nichts, ohne jede Meldung. Zwei Auswege,
 * beide Standard bei SDL2:
 *   1. gamecontrollerdb.txt neben der Exe (bzw. RE15_PAD_DB=<pfad>) wird geladen, falls vorhanden,
 *   2. RE15_PAD_MAPPING="<GUID>,<Name>,a:b0,b:b1,..." setzt ein Mapping direkt.
 * Danach faellt der Joystick in den normalen GameController-Pfad. */
static void pad_load_mappings_once(void)
{
    static int done = 0;
    if (done) return;
    done = 1;
    const char *db = getenv("RE15_PAD_DB");
    int added = SDL_GameControllerAddMappingsFromFile(db && *db ? db : "gamecontrollerdb.txt");
    if (added > 0) fprintf(stderr, "[pad] %d zusaetzliche Mappings geladen\n", added);
    const char *m = getenv("RE15_PAD_MAPPING");
    if (m && *m) {
        int rc = SDL_GameControllerAddMapping(m);
        fprintf(stderr, "[pad] RE15_PAD_MAPPING: %s\n",
                (rc >= 0) ? "uebernommen" : SDL_GetError());
    }
}

static void pad_ensure_open(void)
{
    if (s_pad && !SDL_GameControllerGetAttached(s_pad)) {   /* abgezogen -> freigeben */
        fprintf(stderr, "[pad] Controller abgezogen\n");
        SDL_GameControllerClose(s_pad);
        s_pad = NULL;
    }
    if (s_pad) return;

    pad_load_mappings_once();
    int n = SDL_NumJoysticks();
    for (int i = 0; i < n; i++) {
        if (!SDL_IsGameController(i)) continue;
        s_pad = SDL_GameControllerOpen(i);
        if (!s_pad) continue;
        fprintf(stderr, "[pad] Controller: \"%s\" (Slot %d)\n",
                SDL_GameControllerName(s_pad) ? SDL_GameControllerName(s_pad) : "?", i);
        return;
    }
    /* Nichts geoeffnet: melden, WARUM — genau einmal je Steck-Zustand, damit ein nicht erkanntes
     * Pad nicht stumm bleibt. Die GUID ist das, was in gamecontrollerdb.txt gebraucht wird. */
    static int warned_for = -1;
    if (n != warned_for) {
        warned_for = n;
        if (n == 0) {
            fprintf(stderr, "[pad] kein Controller gefunden (Tastatur bleibt aktiv)\n");
        } else {
            for (int i = 0; i < n; i++) {
                char guid[64] = {0};
                SDL_JoystickGetGUIDString(SDL_JoystickGetDeviceGUID(i), guid, sizeof guid);
                fprintf(stderr, "[pad] Joystick %d \"%s\" GUID %s hat KEIN SDL-Mapping -> ignoriert."
                                " Abhilfe: gamecontrollerdb.txt neben die Exe legen (oder RE15_PAD_DB),"
                                " oder RE15_PAD_MAPPING setzen.\n",
                        i, SDL_JoystickNameForIndex(i) ? SDL_JoystickNameForIndex(i) : "?", guid);
            }
        }
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
    /* RE15_PAD_DEBUG=1: bei jeder Aenderung das PSX-Pad-Wort mit Klarnamen ausgeben. Damit laesst
     * sich Taste fuer Taste gegen die Hardware-Belegung pruefen, statt sie zu glauben. */
    if (getenv("RE15_PAD_DEBUG")) {
        static uint16_t last = 0xffff;
        if (b != last) {
            last = b;
            static const struct { uint16_t bit; const char *nm; } tbl[] = {
                { RE15_PAD_SELECT, "Select" }, { 0x0002, "L3" }, { 0x0004, "R3" },
                { RE15_PAD_START, "Start" },   { RE15_PAD_UP, "Up" },
                { RE15_PAD_RIGHT, "Right" },   { RE15_PAD_DOWN, "Down" },
                { RE15_PAD_LEFT, "Left" },     { 0x0100, "L2" }, { 0x0200, "R2" },
                { RE15_PAD_L1, "L1" },         { RE15_PAD_R1, "R1" },
                { RE15_PAD_TRIANGLE, "Dreieck" }, { RE15_PAD_CIRCLE, "Kreis" },
                { RE15_PAD_CROSS, "Kreuz" },   { RE15_PAD_SQUARE, "Quadrat" },
            };
            char line[256]; int p = 0;
            for (size_t i = 0; i < sizeof tbl / sizeof tbl[0]; i++)
                if (b & tbl[i].bit)
                    p += snprintf(line + p, sizeof line - (size_t)p, "%s%s", p ? "+" : "", tbl[i].nm);
            fprintf(stderr, "[pad] %04X %s\n", b, p ? line : "-");
        }
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
        /* ==== Keyboard layout — MIRRORS the current DuckStation [Pad1] bindings ==========
         * (settings.ini under %LOCALAPPDATA%/DuckStation, read 2026-07-26). The SAME physical
         * key drives the SAME PSX pad bit in the port and in the original under DuckStation, so
         * muscle memory transfers 1:1 for parity work. Movement = arrows (DuckStation D-pad) OR
         * WASD (DuckStation left analog stick — equivalent for RE1.5's digital tank controls):
         *
         *   ↑ ↓ ← →  /  W A S D   = D-pad / L-stick (tank: ↑ forward, ↓ back, ← → turn)
         *   K   (also L/R-Shift)  = ✕ Cross        (RUN, held  +  menu confirm / dialog fast-forward)
         *   J                     = □ Square       (ACTION: examine / grab / fire  +  menu confirm)
         *   L                     = ○ Circle       (dialog cursor / cancel)
         *   I                     = △ Triangle     (dialog cursor / cancel)
         *   Q  /  E               = L1 / R1        (R1 = AIM, held)
         *   1  /  3               = L2 / R2
         *   2  /  4               = L3 / R3
         *   Backspace             = Select
         *   Enter                 = Start          (inventory / weapon-select)
         *   F11 / Select+Start    = toggle fullscreen (window is the default)
         * L-Shift is kept as an EXTRA ✕/Run key (unbound in DuckStation) — the iconic RE run key.
         * ALL debug is on the FUNCTION keys only — never the game keys: F1/F2 = prev/next room,
         * F3 = motion-lock, F4/F5 = clip cycle (F3-F5 need RE15_MOTION_DEBUG). See re15_input_debug_fkey.
         * (Tell me if you want any game key different — this is a single lookup table to edit.) */
        if (keys[SDL_SCANCODE_UP]    || keys[SDL_SCANCODE_W]) bits |= RE15_PAD_UP;
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) bits |= RE15_PAD_RIGHT;
        if (keys[SDL_SCANCODE_DOWN]  || keys[SDL_SCANCODE_S]) bits |= RE15_PAD_DOWN;
        if (keys[SDL_SCANCODE_LEFT]  || keys[SDL_SCANCODE_A]) bits |= RE15_PAD_LEFT;
        if (keys[SDL_SCANCODE_K] || keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
                                          bits |= RE15_PAD_CROSS;    /* ✕ Run / OK   (DuckStation: K; L-Shift kept) */
        if (keys[SDL_SCANCODE_J])         bits |= RE15_PAD_SQUARE;   /* □ Action     (DuckStation: J) */
        if (keys[SDL_SCANCODE_L])         bits |= RE15_PAD_CIRCLE;   /* ○            (DuckStation: L) */
        if (keys[SDL_SCANCODE_I])         bits |= RE15_PAD_TRIANGLE; /* △            (DuckStation: I) */
        if (keys[SDL_SCANCODE_E])         bits |= RE15_PAD_R1;       /* R1 Aim       (DuckStation: E) */
        if (keys[SDL_SCANCODE_Q])         bits |= RE15_PAD_L1;       /* L1           (DuckStation: Q) */
        if (keys[SDL_SCANCODE_BACKSPACE]) bits |= RE15_PAD_SELECT;   /* Select       (DuckStation: Backspace) */
        if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_KP_ENTER])
                                          bits |= RE15_PAD_START;    /* Start = inv  (DuckStation: Enter) */
        if (keys[SDL_SCANCODE_1])         bits |= 0x0100;            /* L2           (DuckStation: 1) */
        if (keys[SDL_SCANCODE_3])         bits |= 0x0200;            /* R2           (DuckStation: 3) */
        if (keys[SDL_SCANCODE_2])         bits |= 0x0002;            /* L3           (DuckStation: 2) */
        if (keys[SDL_SCANCODE_4])         bits |= 0x0004;            /* R3           (DuckStation: 4) */
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
     * timeline for this frame. The pad_pressed/pad_released derivation below stays correct
     * (it diffs bits vs pad_previous).
     *
     * Keyed on OUR OWN tick counter, not g_engine.frame_count: the front-end loops (title,
     * char-select, load screen) render and poll input but never advance frame_count, so a
     * frame_count-keyed script froze at its start index for the whole front-end and COULD NOT
     * press a single button there. Deshalb musste frueher jeder Skript-Lauf per RE15_START_ROOM
     * direkt in einen Raum springen — und ein so betretener Raum ist NICHT der Zustand, in dem
     * ein echter Durchlauf ankommt. This counter advances once per re15_input_tick(), i.e. once
     * per rendered frame in EVERY mode, so a script can drive boot -> title -> NEW GAME -> the
     * intro rooms exactly like a player (and like the DuckStation --path capture it mirrors).
     * RE15_START_ROOM ist inzwischen ENTFERNT (2026-08-01); dieses Skript ist zusammen mit dem
     * Debug-Menue (Token E = SELECT, T = Dreieck, A = Quadrat) der einzige Weg in einen Raum. */
    if (!s_script_init) script_parse_once();
    s_input_ticks++;
    if (s_script_len > 0) {
        int t = s_input_ticks - 1 - s_script_start;
        bits = (t >= 0 && t < s_script_len) ? s_script[t] : 0;
    }

    g_engine.pad_current = bits;

    g_engine.pad_pressed  = g_engine.pad_current & ~g_engine.pad_previous;
    g_engine.pad_released = ~g_engine.pad_current & g_engine.pad_previous;
}
