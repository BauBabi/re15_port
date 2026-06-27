/**
 * @file re15_io_common.h
 * @brief Gemeinsame I/O-Hilfsfunktionen: Case-insensitive Pfadauflösung,
 *        Asset-Root-Ermittlung und RDT-Pfad-Konstruktion
 *
 * Plattformübergreifend — auf Windows native Case-Insensitivität,
 * auf POSIX (Linux/macOS) opendir/readdir mit strcasecmp.
 */
#ifndef RE15_IO_COMMON_H
#define RE15_IO_COMMON_H

#include "re15_types.h"

/* Maximale Pfadlänge für alle I/O-Operationen */
#define RE15_MAX_PATH 260

/**
 * Case-insensitive Dateiauflösung innerhalb eines Verzeichnisses.
 *
 * Sucht im Verzeichnis `dir` nach einer Datei, deren Name case-insensitiv
 * mit `filename` übereinstimmt. Bei Erfolg wird der tatsächliche Dateiname
 * (mit korrekter Groß-/Kleinschreibung des Dateisystems) nach `out` kopiert.
 *
 * @param dir       Verzeichnispfad (muss existieren)
 * @param filename  Gesuchter Dateiname (beliebige Groß-/Kleinschreibung)
 * @param out       Ausgabepuffer für den aufgelösten Dateinamen (mind. RE15_MAX_PATH)
 * @return 0 bei Erfolg, -1 wenn nicht gefunden
 */
int re15_io_resolve_ci(const char* dir, const char* filename, char* out);

/**
 * Ermittelt den Asset-Root-Pfad.
 *
 * Prüft zuerst ob RE15_ASSETS_PATH als Compile-Time-Define gesetzt ist,
 * dann die Umgebungsvariable RE15_ASSETS_PATH.
 *
 * @return Pfad zum Asset-Root oder NULL wenn weder Define noch Env-Variable gesetzt
 */
const char* re15_assets_get_root(void);

/**
 * Konstruiert den RDT-Dateipfad im Format STAGE{N}/ROOM{Stage}{RoomHex}{Player}.RDT
 *
 * @param stage     Stage-Nummer (1-6)
 * @param room_hex  Raum-Nummer als Hex-Wert (0x00-0x27, wird 2-stellig hex formatiert)
 * @param player    Spieler-ID (0=Leon, 1=Elza)
 * @param out_path  Ausgabepuffer für den konstruierten Pfad
 * @param max_len   Maximale Länge des Ausgabepuffers
 * @return 0 bei Erfolg, -1 bei ungültigen Parametern oder Puffer zu klein
 */
int re15_rdt_build_path(int stage, int room_hex, int player, char* out_path, int max_len);

#endif /* RE15_IO_COMMON_H */
