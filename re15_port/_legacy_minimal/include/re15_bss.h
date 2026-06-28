/**
 * @file re15_bss.h
 * @brief BSS-Hintergrund-Laden: 64KB-Chunk-Selektion nach Kamera-ID
 *
 * BSS-Dateien enthalten MDEC-kodierte Hintergründe, einen 64KB-Chunk pro
 * Kamera-Winkel. Der Chunk für Kamera-ID N beginnt bei Offset N*65536
 * in der BSS-Datei.
 *
 * Fehlerbehandlung: Wenn die BSS-Datei fehlt oder die Kamera-ID die
 * Anzahl verfügbarer Chunks überschreitet, wird eine Warnung ausgegeben
 * und der Aufrufer füllt den Ausgabepuffer mit Schwarz (0x00).
 */
#ifndef RE15_BSS_H
#define RE15_BSS_H

#include "re15_types.h"

/* ============================================================================
 * BSS-Lade-API
 * ========================================================================= */

/**
 * Lädt einen 64KB BSS-Chunk für die angegebene Kamera-ID.
 *
 * Sequenz:
 *   1. BSS-Dateipfad konstruieren: STAGE{N}/ROOM{Stage}{RoomHex}.BSS
 *   2. Datei via g_io öffnen und Größe ermitteln
 *   3. Validierung: camera_id * 65536 + 65536 <= file_size
 *   4. Gesamte Datei (oder relevanten Bereich) lesen
 *   5. 64KB-Chunk an Offset camera_id * 65536 nach out_chunk kopieren
 *   6. Handle schließen, Temp-Puffer freigeben
 *
 * Bei Erfolg enthält out_chunk den 64KB MDEC-Bitstream für die Kamera.
 * Bei Fehler bleibt out_chunk unverändert — der Aufrufer ist verantwortlich
 * für die Fallback-Behandlung (schwarzer Hintergrund).
 *
 * @param stage      Stage-Nummer (1-6)
 * @param room_id    Raum-ID als Hex-Wert (0x00-0x27)
 * @param camera_id  Kamera-Index (0-basiert)
 * @param out_chunk  Ausgabepuffer, mindestens RE15_BSS_CHUNK_SIZE (65536) Bytes
 * @return           0 (RE15_IO_OK) bei Erfolg, negativer Fehlercode bei Fehler
 */
int re15_bss_load_chunk(uint8_t stage, uint8_t room_id, uint8_t camera_id,
                        uint8_t* out_chunk);

/**
 * Konstruiert den BSS-Dateipfad im Format STAGE{N}/ROOM{Stage}{RoomHex}.BSS
 *
 * Im Gegensatz zu RDT-Dateien enthalten BSS-Dateien keine Spieler-Unterscheidung,
 * da der Hintergrund für beide Spielercharaktere identisch ist.
 *
 * @param stage     Stage-Nummer (1-6)
 * @param room_hex  Raum-Nummer als Hex-Wert (0x00-0x27)
 * @param out_path  Ausgabepuffer für den konstruierten Pfad
 * @param max_len   Maximale Länge des Ausgabepuffers
 * @return          0 bei Erfolg, -1 bei ungültigen Parametern oder Puffer zu klein
 */
int re15_bss_build_path(int stage, int room_hex, char* out_path, int max_len);

#endif /* RE15_BSS_H */
