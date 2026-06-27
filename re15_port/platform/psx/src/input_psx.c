/* =============================================================================
 * RE 1.5 Port — PSX Input-Backend (PSn00bSDK Pad)
 * =============================================================================
 * Implementiert das Input-Interface (re15_input.h) für PSX über PSn00bSDK:
 *   - Liest Pad-Daten aus dem extern deklarierten Pad-Buffer (pad_buf)
 *   - Invertiert PSX-Button-Logik (0 = gedrückt → 1 = gedrückt)
 *   - Berechnet Edge-Detection: pressed (Rising Edge) und released (Falling Edge)
 *
 * Das Pad wird in main.c initialisiert (InitPAD, StartPAD). Dieses Modul
 * liest lediglich den Pad-Buffer einmal pro Frame aus.
 * ========================================================================== */

#include <stdint.h>
#include <psxpad.h>

#include "re15_types.h"
#include "re15_input.h"

/* ============================================================================
 * Externer Pad-Buffer (initialisiert in main.c via InitPAD/StartPAD)
 * ========================================================================= */

extern uint8_t pad_buf[2][34];

/* ============================================================================
 * Interner Zustand
 * ========================================================================= */

/** Aktueller Frame: Bitmaske gedrückter Buttons (1 = gedrückt) */
static uint16_t s_current = 0;

/** Vorheriger Frame: Bitmaske gedrückter Buttons */
static uint16_t s_previous = 0;

/** Berechneter Input-State für den aktuellen Frame */
static re15_input_state_t s_state = {0, 0, 0};

/* ============================================================================
 * Interface-Implementierung
 * ========================================================================= */

/**
 * Initialisiert das Input-Subsystem.
 *
 * Auf PSX ist das Pad bereits in main.c über InitPAD/StartPAD konfiguriert.
 * Diese Funktion setzt lediglich den internen Zustand zurück.
 */
void re15_input_init(void)
{
    s_current  = 0;
    s_previous = 0;
    s_state.held     = 0;
    s_state.pressed  = 0;
    s_state.released = 0;
}

/**
 * Aktualisiert den Input-Zustand. Einmal pro Frame aufrufen.
 *
 * Liest den Pad-Buffer (Port 0) aus und berechnet held/pressed/released:
 *   - held     = aktuell gedrückte Buttons
 *   - pressed  = in diesem Frame neu gedrückt (Rising Edge)
 *   - released = in diesem Frame losgelassen (Falling Edge)
 *
 * PSX-Besonderheit: pad->btn verwendet invertierte Logik (0 = gedrückt,
 * 1 = losgelassen). Wir invertieren auf unsere Konvention (1 = gedrückt).
 */
void re15_input_tick(void)
{
    PADTYPE* pad = (PADTYPE*)pad_buf[0];

    s_previous = s_current;

    if (pad->stat == 0) {
        /* Pad verbunden — invertiere PSX-Logik (0=gedrückt → 1=gedrückt) */
        s_current = ~pad->btn;
    } else {
        /* Pad nicht verbunden — keine Buttons aktiv */
        s_current = 0;
    }

    s_state.held     = s_current;
    s_state.pressed  = s_current & ~s_previous;
    s_state.released = ~s_current & s_previous;
}

/**
 * Gibt den aktuellen Input-Zustand zurück.
 *
 * Nur nach re15_input_tick() im selben Frame gültig.
 *
 * @return Kopie des aktuellen re15_input_state_t.
 */
re15_input_state_t re15_input_get(void)
{
    return s_state;
}
