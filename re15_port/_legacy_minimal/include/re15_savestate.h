/**
 * @file re15_savestate.h
 * @brief Savestate-Vergleichsmodus für Headless-Validierung
 *
 * Ermöglicht den Vergleich von Spielzuständen gegen referenzierte Savestates.
 * Positionswerte werden mit ±1 Fixkomma-Einheit Toleranz verglichen,
 * Flags und Inventar exakt.
 *
 * Savestate-Format (JSON):
 * {
 *   "player_pos": {"x": N, "y": N, "z": N},
 *   "yaw": N,
 *   "flags": [0, 1, 0, ...],
 *   "inventory": [item_id, ...]
 * }
 *
 * Exit-Codes:
 *   0 = Übereinstimmung
 *   1 = Abweichung (Mismatch)
 *   2 = Fehler (Datei nicht lesbar, ungültiges JSON, etc.)
 *
 * Validates: Requirements 12.3
 */
#ifndef RE15_SAVESTATE_H
#define RE15_SAVESTATE_H

#include "re15_types.h"

/* ============================================================================
 * Konstanten
 * ========================================================================= */

/** Toleranz für Positionsvergleich: ±1 Fixkomma-Einheit */
#define RE15_SAVESTATE_POS_TOLERANCE  1

/** Maximale Anzahl Flags im Savestate */
#define RE15_SAVESTATE_MAX_FLAGS      256

/** Maximale Anzahl Inventar-Einträge */
#define RE15_SAVESTATE_MAX_INVENTORY  64

/** Maximale Pfadlänge */
#define RE15_SAVESTATE_MAX_PATH       512

/* ============================================================================
 * Exit-Codes
 * ========================================================================= */

#define RE15_CMP_MATCH      0   /**< Alle Werte stimmen überein        */
#define RE15_CMP_MISMATCH   1   /**< Abweichung in mindestens 1 Feld   */
#define RE15_CMP_ERROR      2   /**< Fehler (I/O, Parse, etc.)         */

/* ============================================================================
 * Datenstrukturen
 * ========================================================================= */

/**
 * Repräsentation eines Savestate zum Vergleich.
 */
typedef struct {
    int32_t player_x;       /**< Erwartete X-Position (Fixkomma)    */
    int32_t player_y;       /**< Erwartete Y-Position (Fixkomma)    */
    int32_t player_z;       /**< Erwartete Z-Position (Fixkomma)    */
    int16_t yaw;            /**< Erwarteter Yaw (0-4095)            */

    uint8_t flags[RE15_SAVESTATE_MAX_FLAGS];  /**< Erwartete Flags  */
    int     flags_count;    /**< Anzahl Flags im Savestate          */

    int     inventory[RE15_SAVESTATE_MAX_INVENTORY]; /**< Erwartete Items */
    int     inventory_count; /**< Anzahl Inventar-Einträge          */

    /* Welche Felder im Savestate vorhanden sind */
    uint8_t has_position;   /**< 1 wenn player_pos im JSON          */
    uint8_t has_yaw;        /**< 1 wenn yaw im JSON                 */
    uint8_t has_flags;      /**< 1 wenn flags im JSON               */
    uint8_t has_inventory;  /**< 1 wenn inventory im JSON           */
} re15_savestate_t;

/**
 * Ergebnis eines Einzelfeld-Vergleichs.
 */
typedef struct {
    const char* field;      /**< Name des abweichenden Feldes       */
    int32_t     expected;   /**< Soll-Wert                          */
    int32_t     actual;     /**< Ist-Wert                           */
} re15_cmp_diff_t;

/** Maximale Anzahl gemeldeter Abweichungen */
#define RE15_CMP_MAX_DIFFS  32

/**
 * Gesamtergebnis eines Savestate-Vergleichs.
 */
typedef struct {
    int               result;   /**< RE15_CMP_MATCH / MISMATCH / ERROR */
    int               diff_count;
    re15_cmp_diff_t   diffs[RE15_CMP_MAX_DIFFS];
    char              error_msg[256]; /**< Bei ERROR: Fehlerbeschreibung */
} re15_cmp_result_t;

/* ============================================================================
 * API
 * ========================================================================= */

/**
 * Lädt einen Savestate aus einer JSON-Datei.
 *
 * @param path      Dateipfad zum JSON-Savestate
 * @param out       Ausgabe-Struktur (gefüllt bei Erfolg)
 * @return          0 bei Erfolg, -1 bei Fehler
 */
int re15_savestate_load(const char* path, re15_savestate_t* out);

/**
 * Vergleicht aktuellen Spielzustand gegen einen geladenen Savestate.
 *
 * Position: Toleranz ±1 Fixkomma-Einheit pro Achse.
 * Yaw: Exakter Vergleich.
 * Flags: Exakter Array-Vergleich.
 * Inventar: Exakter Array-Vergleich.
 *
 * @param expected  Referenz-Savestate (geladen via re15_savestate_load)
 * @param actual_x  Aktuelle X-Position
 * @param actual_y  Aktuelle Y-Position
 * @param actual_z  Aktuelle Z-Position
 * @param actual_yaw Aktueller Yaw-Wert
 * @param actual_flags  Aktuelles Flags-Array
 * @param actual_flags_count  Anzahl aktueller Flags
 * @param actual_inventory  Aktuelles Inventar-Array
 * @param actual_inventory_count  Anzahl Inventar-Einträge
 * @param out       Vergleichsergebnis
 */
void re15_savestate_compare(
    const re15_savestate_t* expected,
    int32_t actual_x, int32_t actual_y, int32_t actual_z,
    int16_t actual_yaw,
    const uint8_t* actual_flags, int actual_flags_count,
    const int* actual_inventory, int actual_inventory_count,
    re15_cmp_result_t* out
);

/**
 * Gibt das Vergleichsergebnis als JSON auf stdout aus.
 *
 * Bei Übereinstimmung: {"result": "match"}
 * Bei Abweichung: {"result": "mismatch", "mismatches": [...]}
 * Bei Fehler: {"result": "error", "message": "..."}
 *
 * @param result  Vergleichsergebnis
 */
void re15_savestate_print_result(const re15_cmp_result_t* result);

/**
 * Prüft ob zwei Positionswerte innerhalb der Toleranz liegen.
 *
 * @param expected  Erwarteter Wert (Fixkomma)
 * @param actual    Tatsächlicher Wert (Fixkomma)
 * @return          1 wenn |expected - actual| <= RE15_SAVESTATE_POS_TOLERANCE
 */
int re15_savestate_pos_match(int32_t expected, int32_t actual);

#endif /* RE15_SAVESTATE_H */
