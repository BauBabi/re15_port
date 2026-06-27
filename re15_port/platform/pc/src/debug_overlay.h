/**
 * @file debug_overlay.h
 * @brief Debug-Overlays für PC-Build (F1-F5 Tastenkürzel)
 *
 * Stellt visuelle Debug-Informationen bereit, die zur Laufzeit über
 * Tastenkürzel ein-/ausgeschaltet werden können:
 *
 *   F1: Kollisionsboxen (SCA als Drahtgitter)
 *   F2: AOT-Zonen (Tür/Item/Event farbcodiert)
 *   F3: Kamerazonen (RVD-Polygone mit Kamera-ID)
 *   F4: SCD-Trace (letzte 10 Opcodes)
 *   F5: Speicher-Belegung (RAM-Balken)
 *
 * Nur im PC-Build aktiv (RE15_PLATFORM_PC Guard).
 * Rendert nach dem Spielinhalt, vor SDL_RenderPresent.
 */
#ifndef DEBUG_OVERLAY_H
#define DEBUG_OVERLAY_H

#ifdef RE15_PLATFORM_PC

#include <SDL.h>

/* ============================================================================
 * Overlay-Flags (Bit-Positionen)
 * ========================================================================= */

#define DEBUG_OVERLAY_SCA       (1 << 0)  /**< F1: Kollisionsboxen          */
#define DEBUG_OVERLAY_AOT       (1 << 1)  /**< F2: AOT-Zonen                */
#define DEBUG_OVERLAY_RVD       (1 << 2)  /**< F3: Kamerazonen              */
#define DEBUG_OVERLAY_SCD_TRACE (1 << 3)  /**< F4: SCD-Trace                */
#define DEBUG_OVERLAY_MEMORY    (1 << 4)  /**< F5: Speicher-Belegung        */

/* ============================================================================
 * SCD-Trace: Ringpuffer für letzte 10 Opcodes
 * ========================================================================= */

#define DEBUG_SCD_TRACE_SIZE 10

typedef struct {
    uint32_t offset;    /**< Byte-Offset im SCD-Skript      */
    uint8_t  opcode;    /**< Opcode-Wert (0x00-0xFF)         */
} debug_scd_trace_entry_t;

/* ============================================================================
 * API
 * ========================================================================= */

/**
 * Initialisiert das Debug-Overlay-System.
 * Setzt alle Overlays auf deaktiviert, leert den SCD-Trace.
 */
void debug_overlay_init(void);

/**
 * Verarbeitet einen SDL-Tastendruck und toggelt das entsprechende Overlay.
 *
 * @param key  SDL_Keycode des gedrückten Keys (SDLK_F1 bis SDLK_F5)
 * @return     1 wenn ein Overlay getoggelt wurde, 0 sonst
 */
int debug_overlay_toggle(SDL_Keycode key);

/**
 * Rendert alle aktiven Debug-Overlays auf den SDL_Renderer.
 * Aufrufen nach dem Spielinhalt-Rendering, vor SDL_RenderPresent.
 *
 * @param renderer  Der aktive SDL_Renderer
 */
void debug_overlay_render(SDL_Renderer* renderer);

/**
 * Zeichnet einen SCD-Opcode-Aufruf in den Trace-Ringpuffer auf.
 * Wird vom SCD-VM-Dispatcher aufgerufen.
 *
 * @param offset  Byte-Offset des Opcodes im SCD-Skript
 * @param opcode  Opcode-Wert
 */
void debug_overlay_scd_record(uint32_t offset, uint8_t opcode);

/**
 * Gibt zurück ob aktuell mindestens ein Overlay aktiv ist.
 *
 * @return  Bitmaske der aktiven Overlays (0 = keins aktiv)
 */
uint32_t debug_overlay_get_active(void);

#endif /* RE15_PLATFORM_PC */

#endif /* DEBUG_OVERLAY_H */
