/**
 * @file re15_error.h
 * @brief Fehlerbehandlung: I/O-Ergebnis-Codes und Logging-Makros
 *
 * Definiert einheitliche Fehler-Rückgabewerte für das I/O-System sowie
 * plattformübergreifende Logging-Makros (fprintf(stderr) auf PC,
 * printf auf PSX).
 */
#ifndef RE15_ERROR_H
#define RE15_ERROR_H

#include <stdio.h>
#include "re15_types.h"

/* ============================================================================
 * I/O-Ergebnis-Codes
 *
 * Werden von allen I/O-Operationen als Rückgabewert verwendet.
 * Erfolg = 0, Fehler = negative Werte.
 * ========================================================================= */

typedef enum {
    RE15_IO_OK          =  0,   /**< Operation erfolgreich                */
    RE15_IO_NOT_FOUND   = -1,   /**< Datei nicht gefunden                 */
    RE15_IO_READ_ERROR  = -2,   /**< Lesefehler (I/O-Fehler, CD-Fehler)   */
    RE15_IO_BUFFER_FULL = -3,   /**< Puffer zu klein für die Daten        */
    RE15_IO_INVALID_ARG = -4,   /**< Ungültiges Argument (NULL-Pointer etc.) */
} re15_io_result_t;

/* ============================================================================
 * Logging-Makros
 *
 * Auf PC: Ausgabe über fprintf(stderr) mit Subsystem-Präfix.
 * Auf PSX: Ausgabe über printf (kein stderr auf PSX verfügbar).
 *
 * Verwendung:
 *   RE15_ERROR("IO", "Datei nicht gefunden: %s", pfad);
 *   RE15_WARN("SCD", "Unbekannter Opcode: 0x%02X", opcode);
 *   RE15_INFO("ROOM", "Raum %d geladen (%d Bytes)", room_id, size);
 * ========================================================================= */

#ifdef RE15_PLATFORM_PSX

#define RE15_ERROR(subsys, msg, ...) \
    printf("[RE15 ERROR] %s: " msg "\n", subsys, ##__VA_ARGS__)

#define RE15_WARN(subsys, msg, ...) \
    printf("[RE15 WARN]  %s: " msg "\n", subsys, ##__VA_ARGS__)

#define RE15_INFO(subsys, msg, ...) \
    printf("[RE15 INFO]  %s: " msg "\n", subsys, ##__VA_ARGS__)

#else /* RE15_PLATFORM_PC oder Fallback */

#define RE15_ERROR(subsys, msg, ...) \
    fprintf(stderr, "[RE15 ERROR] %s: " msg "\n", subsys, ##__VA_ARGS__)

#define RE15_WARN(subsys, msg, ...) \
    fprintf(stderr, "[RE15 WARN]  %s: " msg "\n", subsys, ##__VA_ARGS__)

#define RE15_INFO(subsys, msg, ...) \
    fprintf(stderr, "[RE15 INFO]  %s: " msg "\n", subsys, ##__VA_ARGS__)

#endif /* RE15_PLATFORM_PSX */

#endif /* RE15_ERROR_H */
