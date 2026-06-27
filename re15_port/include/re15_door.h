/**
 * @file re15_door.h
 * @brief Tür-Übergangs-System: Integration von AOT + Raum-Lade + Spawn
 *
 * Stellt die zentrale Funktion bereit, die pro Frame aus dem Game Loop
 * aufgerufen wird und bei Aktionstaste + AOT-Tür-Trigger den vollständigen
 * Raumwechsel durchführt:
 *
 *   1. Aktionstaste (RE15_BTN_CROSS) auf pressed-Edge prüfen
 *   2. AOT-Check: Spieler im Tür-Polygon + Band-Match?
 *   3. Stage-Wechsel (wenn dest_stage != current_stage)
 *   4. Raum laden (re15_room_load)
 *   5. Spieler an Spawn-Position/Rotation setzen
 *   6. AOT-Zonen am Spawn als "entered" markieren (Re-Trigger-Schutz)
 *
 * Validates: Requirements 4.2, 4.3
 */
#ifndef RE15_DOOR_H
#define RE15_DOOR_H

#include "re15_player.h"
#include "re15_input.h"

/* ============================================================================
 * Rückgabewerte
 * ========================================================================= */

/** Kein Tür-Übergang ausgelöst (kein Button oder kein Trigger) */
#define RE15_DOOR_NONE       0

/** Tür-Übergang erfolgreich durchgeführt */
#define RE15_DOOR_OK         1

/** Tür-Übergang fehlgeschlagen (Ziel-RDT nicht ladbar) */
#define RE15_DOOR_LOAD_FAIL -1

/* ============================================================================
 * Öffentliche API
 * ========================================================================= */

/**
 * Prüft pro Frame ob ein Tür-Übergang ausgelöst werden soll.
 *
 * Wird aus dem Game Loop (main.c) einmal pro Frame aufgerufen.
 * Die Funktion prüft:
 *   - Ob RE15_BTN_CROSS in diesem Frame neu gedrückt wurde (pressed-Edge)
 *   - Ob ein aktiver AOT-Slot vom Typ DOOR die Spielerposition enthält
 *   - Ob die Spieler-Bodenhöhe (floor_band) mit dem Slot übereinstimmt
 *
 * Bei Auslösung wird der vollständige Raumwechsel durchgeführt:
 *   - Stage-Wechsel wenn nötig (re15_stage_change)
 *   - Raum laden (re15_room_load)
 *   - Spielerposition und -rotation auf Spawn-Werte setzen
 *   - Überlappende Tür-AOTs am Spawn als "entered" markieren
 *
 * Sonderfall: Wenn Quell- und Zielraum identisch sind (Same-Room-Door),
 * wird der Spieler teleportiert ohne den Raum neu zu laden.
 *
 * @param player  Zeiger auf die aktive Spieler-Entität
 * @param input   Input-Zustand des aktuellen Frames
 * @return        RE15_DOOR_NONE (0)  — kein Übergang
 *                RE15_DOOR_OK   (1)  — Übergang erfolgreich
 *                RE15_DOOR_LOAD_FAIL (-1) — Ziel-RDT nicht ladbar
 */
int re15_door_trigger_check(re15_player_t* player, re15_input_state_t input);

#endif /* RE15_DOOR_H */
