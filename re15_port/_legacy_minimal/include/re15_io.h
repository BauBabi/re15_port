/**
 * @file re15_io.h
 * @brief Datei-I/O-Abstraktion: Backend-Funktionszeiger-Tabelle
 *
 * Definiert das plattformübergreifende I/O-Interface als Funktionszeiger-
 * Tabelle (re15_io_backend_t). Plattform-spezifische Implementierungen
 * (Standard-C-I/O auf PC, CD-Zugriff auf PSX) registrieren sich über
 * den globalen Zeiger g_io.
 *
 * Alle I/O-Funktionen verwenden synchrone Semantik und geben
 * re15_io_result_t (definiert in re15_error.h) als Fehlercodes zurück.
 */
#ifndef RE15_IO_H
#define RE15_IO_H

#include "re15_types.h"

/* ============================================================================
 * I/O-Backend Funktionszeiger-Tabelle
 *
 * Jede Plattform stellt eine statische Instanz dieser Struktur bereit.
 * Der globale Zeiger g_io wird beim Engine-Start auf das aktive Backend
 * gesetzt.
 * ========================================================================= */

/**
 * Plattform-I/O-Backend.
 *
 * Funktionszeiger für synchrone Dateioperationen:
 *
 * - open:     Öffnet eine Datei am gegebenen Pfad.
 *             Gibt 0 (RE15_IO_OK) bei Erfolg zurück und schreibt das
 *             opaque Handle nach *handle. Bei Fehler negativen Code.
 *
 * - read:     Liest bis zu 'size' Bytes aus dem Handle in 'buf'.
 *             Schreibt die tatsächlich gelesene Anzahl nach *bytes_read.
 *             Gibt 0 bei Erfolg, negativen Code bei Fehler zurück.
 *
 * - get_size: Ermittelt die Dateigröße in Bytes.
 *             Schreibt das Ergebnis nach *size_out.
 *             Gibt 0 bei Erfolg, negativen Code bei Fehler zurück.
 *
 * - close:    Schließt das Handle und gibt Ressourcen frei.
 *             Darf mit NULL-Handle aufgerufen werden (no-op).
 */
typedef struct {
    int  (*open)(const char* path, void** handle);
    int  (*read)(void* handle, uint8_t* buf, int size, int* bytes_read);
    int  (*get_size)(void* handle, int* size_out);
    void (*close)(void* handle);
} re15_io_backend_t;

/* ============================================================================
 * Globaler I/O-Backend-Zeiger
 *
 * Wird beim Engine-Start von der plattformspezifischen Initialisierung
 * auf das entsprechende Backend gesetzt:
 *   - PC:  &g_io_pc_backend   (fopen/fread/fclose)
 *   - PSX: &g_io_psx_backend  (CdSearchFile/CdRead)
 * ========================================================================= */

extern const re15_io_backend_t* g_io;

#endif /* RE15_IO_H */
