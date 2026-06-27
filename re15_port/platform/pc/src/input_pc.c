/**
 * @file input_pc.c
 * @brief PC Input-Backend: SDL2 Keyboard + Gamepad → PSX-Button-Layout
 *
 * Implementiert das Input-Interface (re15_input.h) für die PC-Plattform.
 * Mappt SDL2-Keyboard-Scancodes und Gamepad-Buttons auf die 16-Bit
 * PSX-Button-Bitmaske. Berechnet pro Frame Edge-Detection
 * (pressed/released) aus dem Vergleich mit dem vorherigen Frame.
 *
 * Standard-Tastenbelegung:
 *   W / Up Arrow    → RE15_BTN_UP
 *   S / Down Arrow  → RE15_BTN_DOWN
 *   A / Left Arrow  → RE15_BTN_LEFT
 *   D / Right Arrow → RE15_BTN_RIGHT
 *   Enter/Return    → RE15_BTN_CROSS  (Aktion)
 *   Space           → RE15_BTN_CIRCLE
 *   Q               → RE15_BTN_SQUARE
 *   E               → RE15_BTN_TRIANGLE
 *   Left Shift      → RE15_BTN_R1 (Rennen)
 *   Tab             → RE15_BTN_L1
 *   Z               → RE15_BTN_L2
 *   X               → RE15_BTN_R2
 *   Escape          → RE15_BTN_START
 *   Backspace       → RE15_BTN_SELECT
 */

#include <SDL.h>

#include "re15_input.h"
#include "re15_error.h"

/* ============================================================================
 * Statische Variablen
 * ========================================================================= */

/** Aktueller Frame: Buttons die gerade gehalten werden */
static uint16_t s_current = 0;

/** Vorheriger Frame: Buttons die im letzten Frame gehalten wurden */
static uint16_t s_previous = 0;

/** Berechneter Input-Zustand (held/pressed/released) */
static re15_input_state_t s_state = {0, 0, 0};

/** SDL Gamepad-Handle (NULL wenn kein Gamepad angeschlossen) */
static SDL_GameController* s_gamepad = NULL;

/* ============================================================================
 * Hilfsfunktion: Gamepad erkennen und öffnen
 * ========================================================================= */

/**
 * Sucht den ersten verfügbaren Game-Controller und öffnet ihn.
 * Wird bei Init und bei Hotplug-Events aufgerufen.
 */
static void input_pc_open_gamepad(void)
{
    int i;
    int num_joysticks;

    if (s_gamepad != NULL) {
        return; /* Bereits geöffnet */
    }

    num_joysticks = SDL_NumJoysticks();
    for (i = 0; i < num_joysticks; i++) {
        if (SDL_IsGameController(i)) {
            s_gamepad = SDL_GameControllerOpen(i);
            if (s_gamepad != NULL) {
                RE15_INFO("INPUT", "Gamepad erkannt: %s",
                          SDL_GameControllerName(s_gamepad));
                return;
            }
        }
    }
}

/* ============================================================================
 * Hilfsfunktion: Gamepad-Buttons auf PSX-Bitmaske mappen
 * ========================================================================= */

/**
 * Liest den aktuellen Gamepad-Zustand und gibt die gemappte
 * 16-Bit PSX-Button-Bitmaske zurück.
 */
static uint16_t input_pc_read_gamepad(void)
{
    uint16_t buttons = 0;

    if (s_gamepad == NULL) {
        return 0;
    }

    if (!SDL_GameControllerGetAttached(s_gamepad)) {
        SDL_GameControllerClose(s_gamepad);
        s_gamepad = NULL;
        RE15_WARN("INPUT", "Gamepad getrennt");
        return 0;
    }

    /* D-Pad */
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_DPAD_UP))
        buttons |= RE15_BTN_UP;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
        buttons |= RE15_BTN_DOWN;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
        buttons |= RE15_BTN_LEFT;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT))
        buttons |= RE15_BTN_RIGHT;

    /* Face Buttons (PSX-Layout) */
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_A))
        buttons |= RE15_BTN_CROSS;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_B))
        buttons |= RE15_BTN_CIRCLE;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_X))
        buttons |= RE15_BTN_SQUARE;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_Y))
        buttons |= RE15_BTN_TRIANGLE;

    /* Shoulder Buttons */
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
        buttons |= RE15_BTN_L1;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
        buttons |= RE15_BTN_R1;

    /* Triggers als digitale L2/R2 (Threshold > 50%) */
    if (SDL_GameControllerGetAxis(s_gamepad, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 16384)
        buttons |= RE15_BTN_L2;
    if (SDL_GameControllerGetAxis(s_gamepad, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 16384)
        buttons |= RE15_BTN_R2;

    /* Start/Select (Back) */
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_START))
        buttons |= RE15_BTN_START;
    if (SDL_GameControllerGetButton(s_gamepad, SDL_CONTROLLER_BUTTON_BACK))
        buttons |= RE15_BTN_SELECT;

    /* Left Stick als zusätzliches D-Pad (Deadzone: 8000 von 32767) */
    {
        int16_t lx = SDL_GameControllerGetAxis(s_gamepad, SDL_CONTROLLER_AXIS_LEFTX);
        int16_t ly = SDL_GameControllerGetAxis(s_gamepad, SDL_CONTROLLER_AXIS_LEFTY);

        if (lx < -8000) buttons |= RE15_BTN_LEFT;
        if (lx >  8000) buttons |= RE15_BTN_RIGHT;
        if (ly < -8000) buttons |= RE15_BTN_UP;
        if (ly >  8000) buttons |= RE15_BTN_DOWN;
    }

    return buttons;
}

/* ============================================================================
 * Hilfsfunktion: Keyboard-Scancodes auf PSX-Bitmaske mappen
 * ========================================================================= */

/**
 * Liest den aktuellen Keyboard-Zustand und gibt die gemappte
 * 16-Bit PSX-Button-Bitmaske zurück.
 */
static uint16_t input_pc_read_keyboard(void)
{
    uint16_t buttons = 0;
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys == NULL) {
        return 0;
    }

    /* D-Pad: WASD + Pfeiltasten */
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP])
        buttons |= RE15_BTN_UP;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN])
        buttons |= RE15_BTN_DOWN;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT])
        buttons |= RE15_BTN_LEFT;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT])
        buttons |= RE15_BTN_RIGHT;

    /* Face Buttons */
    if (keys[SDL_SCANCODE_RETURN] || keys[SDL_SCANCODE_KP_ENTER])
        buttons |= RE15_BTN_CROSS;
    if (keys[SDL_SCANCODE_SPACE])
        buttons |= RE15_BTN_CIRCLE;
    if (keys[SDL_SCANCODE_Q])
        buttons |= RE15_BTN_SQUARE;
    if (keys[SDL_SCANCODE_E])
        buttons |= RE15_BTN_TRIANGLE;

    /* Shoulder Buttons */
    if (keys[SDL_SCANCODE_TAB])
        buttons |= RE15_BTN_L1;
    if (keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT])
        buttons |= RE15_BTN_R1;

    /* L2/R2 */
    if (keys[SDL_SCANCODE_Z])
        buttons |= RE15_BTN_L2;
    if (keys[SDL_SCANCODE_X])
        buttons |= RE15_BTN_R2;

    /* Start/Select */
    if (keys[SDL_SCANCODE_ESCAPE])
        buttons |= RE15_BTN_START;
    if (keys[SDL_SCANCODE_BACKSPACE])
        buttons |= RE15_BTN_SELECT;

    return buttons;
}

/* ============================================================================
 * Exportierte Funktionen (re15_input.h Interface)
 * ========================================================================= */

/**
 * Initialisiert das Input-Subsystem.
 *
 * Setzt den internen Zustand zurück und versucht, einen Game-Controller
 * zu öffnen, falls einer angeschlossen ist. Setzt voraus, dass
 * SDL_Init(SDL_INIT_GAMECONTROLLER) bereits aufgerufen wurde.
 */
void re15_input_init(void)
{
    s_current  = 0;
    s_previous = 0;
    s_state.held     = 0;
    s_state.pressed  = 0;
    s_state.released = 0;

    /* Gamepad-Subsystem sollte bereits via SDL_Init initialisiert sein */
    input_pc_open_gamepad();

    RE15_INFO("INPUT", "PC Input-Backend initialisiert (Keyboard + Gamepad)");
}

/**
 * Aktualisiert den Input-Zustand. Einmal pro Frame aufrufen.
 *
 * Liest Keyboard- und Gamepad-Zustand, kombiniert beide per OR,
 * und berechnet Edge-Detection:
 *   pressed  = current & ~previous  (neu gedrückt)
 *   released = ~current & previous  (losgelassen)
 */
void re15_input_tick(void)
{
    s_previous = s_current;
    s_current  = 0;

    /* Keyboard-Mapping lesen */
    s_current |= input_pc_read_keyboard();

    /* Gamepad-Mapping lesen (OR mit Keyboard) */
    s_current |= input_pc_read_gamepad();

    /* Versuche Gamepad bei Hotplug zu erkennen */
    if (s_gamepad == NULL) {
        input_pc_open_gamepad();
    }

    /* Edge-Detection berechnen */
    s_state.held     = s_current;
    s_state.pressed  = s_current & ~s_previous;
    s_state.released = (uint16_t)(~s_current & s_previous);
}

/**
 * Gibt den aktuellen Input-Zustand zurück.
 * Nur nach re15_input_tick() im selben Frame gültig.
 */
re15_input_state_t re15_input_get(void)
{
    return s_state;
}
