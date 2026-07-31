/*
 * RE1.5 Rebuilt — Zustand des ORIGINAL-Debug-Menues ("UTILITY MENU", PSX.EXE @0x80014444).
 *
 * Die Felder tragen absichtlich dieselbe Bedeutung wie die drei Original-Bytes, damit ein
 * RAM-Vergleich Port gegen Original moeglich bleibt:
 *     row       <- 0x800BBE5D          Auswahlzeile, 0..2 (@0x80014604/18, Grenze @0x80014644)
 *     stage     <- 0x800BBE5E          Stage-Index (@0x800146E4)
 *     room_idx  <- 0x800BBE5F + stage  Raumindex je Stage, Wrap bei 0x31 (@0x80014734)
 *
 * Zweck im Port: Original und Port muessen einen Raum ueber DENSELBEN Weg betreten, sonst vergleicht
 * der Paritaets-Harness zwei verschiedene Situationen (genau daher kamen die Positions- und
 * Yaw-Differenzen in ROOM1140, die ich faelschlich dem Port zugeschrieben hatte).
 */
#ifndef RE15_DEBUG_MENU_H
#define RE15_DEBUG_MENU_H

#include <stdint.h>

#define RE15_DBG_STAGES 8

typedef struct {
    uint8_t open;                          /* Menue sichtbar (Original: eigener Pause-Zustand)   */
    uint8_t row;                           /* 0x800BBE5D — 0..2                                  */
    uint8_t stage;                         /* 0x800BBE5E                                         */
    uint8_t room_idx[RE15_DBG_STAGES];     /* 0x800BBE5F + stage, Wrap bei 0x31                  */
    uint8_t want_load;                     /* 1 = Raum laden (Square in der JUMP-Zeile)          */
    uint16_t load_room;                    /* der aufzuloesende Raum                             */
} re15_debug_menu_t;

void re15_debug_menu_reset(void);
void re15_debug_menu_toggle(void);
int  re15_debug_menu_open(void);
int  re15_debug_menu_tick(uint16_t vpad_edge);
const re15_debug_menu_t *re15_debug_menu_state(void);

#endif /* RE15_DEBUG_MENU_H */
