/**
 * @file re15_room.h
 * @brief Raum-Lade-System: Laden, Entladen und Zustandsverwaltung
 *
 * Definiert die API für das Raum-Management:
 * - re15_room_load(): Vollständige Lade-Sequenz (I/O → RDT-Parse → SCD-Init)
 * - re15_room_unload(): Vollständiges Entladen (Speicher, AOT, SCD)
 *
 * Der Room-Manager arbeitet auf der globalen Engine-Instanz (g_game) und
 * koordiniert alle Subsysteme beim Raumwechsel.
 */
#ifndef RE15_ROOM_H
#define RE15_ROOM_H

#include "re15_types.h"

/* ============================================================================
 * Raum-Lade-API
 * ========================================================================= */

/**
 * Lädt einen Raum vollständig.
 *
 * Sequenz:
 *   1. Aktuellen Raum entladen (re15_room_unload)
 *   2. RDT-Pfad konstruieren (re15_rdt_build_path)
 *   3. Datei via I/O-Backend öffnen und lesen
 *   4. RDT in-place parsen (re15_rdt_parse)
 *   5. SCD-VM initialisieren und Main-Thread starten
 *   6. Kamera auf Cut 0 setzen
 *
 * Null-Offset-Behandlung: Sektionen mit Offset 0 (NULL-Pointer nach Parse)
 * werden übersprungen — das jeweilige Subsystem wird ohne Daten initialisiert.
 *
 * @param stage      Stage-Nummer (1-6)
 * @param room_id    Raum-ID als Hex-Wert (0x00-0x27)
 * @param player_id  Spieler-ID (0=Leon, 1=Elza)
 * @return           0 bei Erfolg, negativer Fehlercode bei Fehler
 */
int re15_room_load(uint8_t stage, uint8_t room_id, uint8_t player_id);

/**
 * Entlädt den aktuellen Raum vollständig.
 *
 * Führt in dieser Reihenfolge aus:
 *   1. RDT-Puffer freigeben (free + NULL setzen)
 *   2. AOT-Slots zurücksetzen (re15_aot_init)
 *   3. SCD-VM reinitialisieren (scd_vm_init)
 *   4. RDT-Puffer-Größe auf 0 setzen
 *   5. Aktuelle RDT-Struktur nullen
 *
 * Kann sicher aufgerufen werden wenn kein Raum geladen ist (no-op für
 * bereits freigegebene Ressourcen).
 */
void re15_room_unload(void);

/* ============================================================================
 * Stage-Wechsel-API
 * ========================================================================= */

/**
 * Führt einen vollständigen Stage-Wechsel durch.
 *
 * Wird aufgerufen wenn ein Tür-Übergang in einen Raum einer anderen Stage
 * führt. Schritte:
 *   1. No-op bei gleicher Stage (new_stage == g_game.current_stage)
 *   2. Stage-Overlay laden (STAGE{N}.BIN — Stub auf PC)
 *   3. VAB-Sound-Bank wechseln (alte entladen, neue laden)
 *   4. Raumspezifische BGM-Tabelle für neuen Stage aktivieren
 *   5. g_game.current_stage aktualisieren
 *
 * Nach dem Stage-Wechsel muss der Aufrufer re15_room_load() für den
 * Zielraum aufrufen.
 *
 * @param new_stage  Ziel-Stage-Nummer (1-6)
 * @return           0 bei Erfolg, negativer Wert bei Fehler
 *                   (z.B. ungültige Stage-Nummer)
 */
int re15_stage_change(uint8_t new_stage);

#endif /* RE15_ROOM_H */
