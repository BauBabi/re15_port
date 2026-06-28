/**
 * @file re15_camera.h
 * @brief RID (Room Image Data) und RVD (Room View Data) Datenstrukturen
 *
 * RID enthält Kameradefinitionen pro Cut (Position, Blickziel, FOV).
 * RVD enthält Trigger-Zonen (4-Punkt-Polygone) die bestimmen, welche
 * Kamera aktiv wird, wenn der Spieler eine Zone betritt.
 *
 * Parsing-Strategie: Copy — die Daten werden in Ausgabe-Strukturen kopiert
 * und aus dem Little-Endian-Rohdatenformat konvertiert.
 */
#ifndef RE15_CAMERA_H
#define RE15_CAMERA_H

#include "re15_types.h"

/* ============================================================================
 * RID — Room Image Data (Kameradefinitionen)
 *
 * Format (zeigt auf rdt.camera):
 *   Header: uint32_t count
 *   Dann count Einträge à 28 Bytes:
 *     int32_t cam_x, cam_y, cam_z       — Kameraposition (Weltkoordinaten)
 *     int32_t target_x, target_y, target_z — Blickziel
 *     uint32_t fov                        — Sichtfeld (Q12 Fixkomma)
 * ========================================================================= */

/** Maximale Anzahl unterstützter Kameras pro Raum */
#define RE15_RID_MAX_CAMERAS 32

/** Größe eines einzelnen RID-Kameraeintrags in Bytes */
#define RE15_RID_ENTRY_SIZE 28

/** Einzelne Kameradefinition */
typedef struct {
    int32_t  cam_x;       /**< Kamera X-Position (Weltkoordinaten)  */
    int32_t  cam_y;       /**< Kamera Y-Position (Weltkoordinaten)  */
    int32_t  cam_z;       /**< Kamera Z-Position (Weltkoordinaten)  */
    int32_t  target_x;    /**< Blickziel X (Weltkoordinaten)        */
    int32_t  target_y;    /**< Blickziel Y (Weltkoordinaten)        */
    int32_t  target_z;    /**< Blickziel Z (Weltkoordinaten)        */
    uint32_t fov;         /**< Sichtfeld (Q12 Fixkomma)             */
} re15_rid_camera_t;

/** Geparste RID-Daten — alle Kameras eines Raums */
typedef struct {
    uint32_t          count;                         /**< Anzahl der Kameras  */
    re15_rid_camera_t cameras[RE15_RID_MAX_CAMERAS]; /**< Kamera-Einträge     */
} re15_rid_data_t;

/* ============================================================================
 * RVD — Room View Data (Kamera-Trigger-Zonen)
 *
 * Format (zeigt auf rdt.zone):
 *   Header: uint16_t count
 *   Dann count Einträge à 22 Bytes:
 *     uint8_t  cam_from         — Quell-Kamera-ID
 *     uint8_t  cam_to           — Ziel-Kamera-ID bei Betreten
 *     int16_t  trigger_x[4]    — 4-Punkt-Polygon X-Koordinaten
 *     int16_t  trigger_z[4]    — 4-Punkt-Polygon Z-Koordinaten
 *     uint16_t flags           — Zonenflags
 * ========================================================================= */

/** Maximale Anzahl unterstützter Trigger-Zonen pro Raum */
#define RE15_RVD_MAX_ZONES 64

/** Größe eines einzelnen RVD-Zoneneintrags in Bytes */
#define RE15_RVD_ENTRY_SIZE 22

/** Einzelne Kamera-Trigger-Zone */
typedef struct {
    uint8_t  cam_from;       /**< Quell-Kamera-ID (aktive Kamera)       */
    uint8_t  cam_to;         /**< Ziel-Kamera-ID bei Betreten der Zone  */
    int16_t  trigger_x[4];   /**< 4-Punkt-Polygon X-Koordinaten         */
    int16_t  trigger_z[4];   /**< 4-Punkt-Polygon Z-Koordinaten         */
    uint16_t flags;          /**< Zonenflags                            */
} re15_rvd_zone_t;

/** Geparste RVD-Daten — alle Trigger-Zonen eines Raums */
typedef struct {
    uint16_t        count;                       /**< Anzahl der Zonen    */
    re15_rvd_zone_t zones[RE15_RVD_MAX_ZONES];   /**< Zonen-Einträge      */
} re15_rvd_data_t;

/* ============================================================================
 * Kamerazone-Dispatch-API
 * ========================================================================= */

/**
 * Prüft alle RVD-Trigger-Zonen gegen die aktuelle Spielerposition.
 *
 * Iteriert alle Zonen mit cam_from == current_cam und prüft per
 * Cross-Product Point-in-4P-Polygon-Test, ob der Spieler innerhalb liegt.
 * Bei Treffer wird die Ziel-Kamera in out_new_cam geschrieben.
 *
 * Der zugehörige BSS-Hintergrund-Chunk wird vom Aufrufer (Raum-Manager)
 * bei Kamerawechsel geladen — diese Funktion erkennt nur den Übergang.
 *
 * @param player_x    Spieler X-Position (Weltkoordinaten, int16)
 * @param player_z    Spieler Z-Position (Weltkoordinaten, int16)
 * @param rvd         Zeiger auf geparste RVD-Daten (Trigger-Zonen)
 * @param current_cam Aktuell aktive Kamera-ID
 * @param out_new_cam Ausgabe: Neue Kamera-ID (oder current_cam bei keinem Treffer)
 */
void re15_camera_check_zones(int16_t player_x, int16_t player_z,
                             const re15_rvd_data_t* rvd,
                             uint8_t current_cam,
                             uint8_t* out_new_cam);

/* ============================================================================
 * Parser-API
 * ========================================================================= */

/**
 * Parst RID-Kameradaten aus einem Rohdatenpuffer.
 *
 * Liest den count-Header (4 Bytes) gefolgt von count×28-Byte-Einträgen.
 * Konvertiert alle Felder von Little-Endian in die Zielstruktur.
 *
 * @param data      Zeiger auf die RID-Rohdaten (ab rdt.camera)
 * @param data_size Größe der verfügbaren Daten in Bytes
 * @param out       Ausgabe: Befüllte RID-Struktur
 * @return          0 bei Erfolg, negativer Fehlercode bei ungültigem Input
 */
int re15_rid_parse(const uint8_t* data, int data_size, re15_rid_data_t* out);

/**
 * Parst RVD-Trigger-Zonen aus einem Rohdatenpuffer.
 *
 * Liest den count-Header (2 Bytes) gefolgt von count×22-Byte-Einträgen.
 * Konvertiert alle Felder von Little-Endian in die Zielstruktur.
 *
 * @param data      Zeiger auf die RVD-Rohdaten (ab rdt.zone)
 * @param data_size Größe der verfügbaren Daten in Bytes
 * @param out       Ausgabe: Befüllte RVD-Struktur
 * @return          0 bei Erfolg, negativer Fehlercode bei ungültigem Input
 */
int re15_rvd_parse(const uint8_t* data, int data_size, re15_rvd_data_t* out);

#endif /* RE15_CAMERA_H */
