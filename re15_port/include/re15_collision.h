/**
 * @file re15_collision.h
 * @brief SCA (Scalar Collision Area) Datenstrukturen und Parser-API
 *
 * Definiert die Strukturen für geparste SCA-Kollisionsdaten. Die SCA-Sektion
 * wird über rdt.collision referenziert und enthält Kollisionszellen für
 * Raum-Begrenzungen (Rechtecke, Kreise, Treppen).
 *
 * Format:
 *   Header: 4 Bytes — uint16_t count, uint16_t reserved
 *   Entries: count × 14 Bytes pro Eintrag
 *
 * Eintragstypen:
 *   Typ 1  — Rechteck (corner_x/z = Ecke, width/depth = Ausdehnung)
 *   Typ 3  — Kreis (corner_x/z = Mittelpunkt, width = Radius)
 *   Typ 12 — Treppe aufwärts
 *   Typ 13 — Treppe abwärts
 *
 * Parsing-Strategie: In-Place — der Parser liefert Pointer in den
 * geladenen Puffer (kein Kopieren). Der Puffer muss für die gesamte
 * Lebensdauer der re15_sca_data_t-Struktur gültig bleiben.
 */
#ifndef RE15_COLLISION_H
#define RE15_COLLISION_H

#include "re15_types.h"

/* ============================================================================
 * SCA-Eintragstypen
 * ========================================================================= */

#define RE15_SCA_TYPE_RECTANGLE  1   /**< Rechteckige Kollisionszone     */
#define RE15_SCA_TYPE_CIRCLE     3   /**< Kreisförmige Kollisionszone    */
#define RE15_SCA_TYPE_STAIR_UP   12  /**< Treppe aufwärts                */
#define RE15_SCA_TYPE_STAIR_DOWN 13  /**< Treppe abwärts                 */

/* Größe eines einzelnen SCA-Eintrags in Bytes */
#define RE15_SCA_ENTRY_SIZE      14

/* Größe des SCA-Headers in Bytes */
#define RE15_SCA_HEADER_SIZE     4

/* ============================================================================
 * SCA-Eintrags-Struktur (14 Bytes im Binärformat)
 *
 * Wird vom Parser aus dem Roh-Puffer gelesen. Die Felder entsprechen
 * den originalen PSX-Kollisionsdaten (Little-Endian s16/u16).
 * ========================================================================= */

typedef struct re15_sca_entry_s {
    uint16_t type;       /**< Eintragstyp: 1=Rect, 3=Circle, 12/13=Stair */
    int16_t  corner_x;   /**< X-Koordinate Ecke (oder Mittelpunkt bei Kreis) */
    int16_t  corner_z;   /**< Z-Koordinate Ecke (oder Mittelpunkt bei Kreis) */
    int16_t  width;      /**< Breite (oder Radius bei Kreis)                 */
    int16_t  depth;      /**< Tiefe (Höhenausdehnung)                        */
    uint16_t density;    /**< Boden-Dichte / Band-Wert                       */
    uint16_t flags;      /**< Zusätzliche Flags (reserviert)                 */
} re15_sca_entry_t;

/* ============================================================================
 * SCA-Datencontainer
 *
 * Enthält Header-Informationen und einen Pointer auf das Array der
 * geparsten Einträge. Die Einträge liegen in einem separat allokierten
 * Array (nicht in-place im Puffer, da Byte-Alignment-Konvertierung nötig).
 * ========================================================================= */

typedef struct re15_sca_data_s {
    uint16_t          count;    /**< Anzahl der Kollisionseinträge            */
    uint16_t          reserved; /**< Reserviertes Header-Feld                 */
    re15_sca_entry_t* entries;  /**< Pointer auf geparste Einträge (im Puffer)*/
} re15_sca_data_t;

/* ============================================================================
 * SCA-Parser API
 * ========================================================================= */

/**
 * Parst SCA-Kollisionsdaten aus einem Roh-Puffer.
 *
 * Liest den 4-Byte-Header (count + reserved) und konvertiert die
 * nachfolgenden Einträge (je 14 Bytes) in re15_sca_entry_t-Strukturen.
 * Die Einträge werden in ein statisches internes Array geparst; der
 * entries-Pointer in out zeigt auf dieses Array.
 *
 * @param data      Pointer auf den Beginn der SCA-Sektion im RDT-Puffer
 * @param data_size Verfügbare Bytes ab data (für Bounds-Prüfung)
 * @param out       Ausgabe: Befüllte SCA-Datenstruktur
 * @return          0 (RE15_IO_OK) bei Erfolg, negativer Fehlercode bei Fehler
 *
 * @note Der data-Puffer muss für die Lebensdauer von out gültig bleiben.
 * @note Maximale Kapazität: 256 Einträge (typische RE1.5-Räume: 10-80).
 */
int re15_sca_parse(const uint8_t* data, int data_size, re15_sca_data_t* out);

/* ============================================================================
 * SCA-Kollisionsprüfung
 * ========================================================================= */

/* Vorwärtsdeklaration (definiert in re15_player.h) */
struct re15_player_s;

/**
 * Prüft den Spieler-Kollisionskörper gegen alle SCA-Einträge.
 *
 * Iteriert über alle SCA-Einträge und führt typspezifische Prüfungen durch:
 *   - Typ 1 (Rechteck): Zylinder-vs-AABB, Push entlang nächster Kante
 *   - Typ 3 (Kreis): Zylinder-vs-Kreis, radialer Push
 *   - Typ 12/13 (Treppen): Y-Versatz ±0x708 pro Band-Übergang
 *
 * Bei Durchdringung wird die Spielerposition direkt korrigiert.
 * Verwendet ausschließlich Integer/Fixpoint-Arithmetik.
 *
 * @param player    Spieler-Entität (Position wird bei Kollision modifiziert)
 * @param sca       Geparste SCA-Kollisionsdaten des aktuellen Raums
 */
void re15_collision_check(struct re15_player_s* player, const re15_sca_data_t* sca);

#endif /* RE15_COLLISION_H */
