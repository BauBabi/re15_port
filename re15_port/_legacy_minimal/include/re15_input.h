/**
 * @file re15_input.h
 * @brief Eingabe-Abstraktion: Button-Konstanten und Input-State
 *
 * Definiert das plattformübergreifende Input-Interface für digitale Buttons.
 * Pro Frame werden drei Zustände bereitgestellt: held (aktuell gedrückt),
 * pressed (Edge: in diesem Frame neu gedrückt) und released (Edge: in
 * diesem Frame losgelassen).
 *
 * Button-Konstanten folgen dem PSX-Controller-Layout als 16-Bit-Bitmaske.
 */
#ifndef RE15_INPUT_H
#define RE15_INPUT_H

#include "re15_types.h"

/* ============================================================================
 * Button-Konstanten (PSX-Layout, 16-Bit-Bitmaske)
 *
 * Jeder Button belegt ein einzelnes Bit. Mehrere Buttons können per
 * bitweisem OR kombiniert werden.
 * ========================================================================= */

#define RE15_BTN_L2        0x0001
#define RE15_BTN_R2        0x0002
#define RE15_BTN_L1        0x0004
#define RE15_BTN_R1        0x0008
#define RE15_BTN_UP        0x0010
#define RE15_BTN_RIGHT     0x0020
#define RE15_BTN_DOWN      0x0040
#define RE15_BTN_LEFT      0x0080
#define RE15_BTN_SELECT    0x0100
#define RE15_BTN_START     0x0800
#define RE15_BTN_TRIANGLE  0x1000
#define RE15_BTN_CIRCLE    0x2000
#define RE15_BTN_CROSS     0x4000
#define RE15_BTN_SQUARE    0x8000

/* ============================================================================
 * Input-State-Struktur
 *
 * Wird pro Frame einmal aktualisiert (re15_input_tick). Die drei Felder
 * ermöglichen Edge-Detection ohne externe Zustandsspeicherung im Spielcode.
 * ========================================================================= */

/**
 * Eingabezustand für einen Frame.
 *
 * - held:     Bitmaske aller aktuell gedrückten Buttons
 * - pressed:  Bitmaske der in diesem Frame neu gedrückten Buttons (Rising Edge)
 * - released: Bitmaske der in diesem Frame losgelassenen Buttons (Falling Edge)
 */
typedef struct {
    uint16_t held;      /**< Aktuell gedrückt                    */
    uint16_t pressed;   /**< Neu gedrückt in diesem Frame (Edge) */
    uint16_t released;  /**< Losgelassen in diesem Frame (Edge)  */
} re15_input_state_t;

/* ============================================================================
 * Funktionsdeklarationen
 * ========================================================================= */

/**
 * Initialisiert das Input-Subsystem.
 * Muss einmalig vor dem ersten Aufruf von re15_input_tick() aufgerufen werden.
 */
void re15_input_init(void);

/**
 * Aktualisiert den Input-Zustand. Einmal pro Frame aufrufen.
 * Berechnet pressed/released-Edges aus dem Vergleich mit dem vorherigen Frame.
 */
void re15_input_tick(void);

/**
 * Gibt den aktuellen Input-Zustand zurück.
 * Nur nach re15_input_tick() im selben Frame gültig.
 */
re15_input_state_t re15_input_get(void);

#endif /* RE15_INPUT_H */
