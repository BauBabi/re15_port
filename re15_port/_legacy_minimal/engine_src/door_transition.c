/**
 * @file door_transition.c
 * @brief Tür-Trigger-Logik: AOT + Raum-Lade + Spawn-Position zusammenführen
 *
 * Verbindet das AOT-Trigger-System mit dem Raum-Lade-System und der
 * Spieler-Entität. Wird pro Frame aus dem Game Loop aufgerufen und
 * prüft ob der Spieler eine Tür-AOT-Zone bei gedrückter Aktionstaste
 * aktiviert hat.
 *
 * Ablauf bei Auslösung:
 *   1. Aktionstaste (Cross) pressed-Edge erkennen
 *   2. re15_aot_check() — liefert ausgelösten Slot-Index oder -1
 *   3. Slot-Typ prüfen (nur AOT_TYPE_DOOR relevant)
 *   4. Tür-Daten extrahieren (dest_stage, dest_room, spawn_x/y/z, spawn_rot)
 *   5. Stage-Wechsel durchführen wenn dest_stage != current_stage
 *   6. Raum laden (re15_room_load)
 *   7. Spielerposition und -rotation auf Spawn-Werte setzen
 *   8. re15_aot_mark_entered() — Re-Trigger-Schutz am Spawn
 *
 * Validates: Requirements 4.2, 4.3
 */

#include "re15_door.h"
#include "re15_aot.h"
#include "re15_room.h"
#include "re15_engine.h"
#include "re15_error.h"

/* ============================================================================
 * re15_door_trigger_check — Frame-weise Tür-Trigger-Prüfung
 * ========================================================================= */

int re15_door_trigger_check(re15_player_t* player, re15_input_state_t input)
{
    int slot_idx;
    const re15_aot_slot_t* slot;
    uint8_t dest_stage;
    uint8_t dest_room;
    int16_t spawn_x, spawn_y, spawn_z, spawn_rot;
    int rc;

    /* --- Parametervalidierung --- */
    if (player == NULL) {
        return RE15_DOOR_NONE;
    }

    /* --- 1. Aktionstaste prüfen (pressed-Edge: nur in diesem Frame neu) --- */
    if (!(input.pressed & RE15_BTN_CROSS)) {
        return RE15_DOOR_NONE;
    }

    /* --- 2. AOT-Check: Spieler in einer aktiven Trigger-Zone? --- */
    slot_idx = re15_aot_check(
        (int16_t)Q12_TO_INT(player->x),
        (int16_t)Q12_TO_INT(player->z),
        player->floor_band
    );

    if (slot_idx < 0) {
        return RE15_DOOR_NONE; /* Kein Slot ausgelöst */
    }

    /* --- 3. Slot-Daten abrufen und Typ prüfen --- */
    slot = re15_aot_get_slot(slot_idx);
    if (slot == NULL) {
        return RE15_DOOR_NONE;
    }

    if (slot->type != AOT_TYPE_DOOR) {
        return RE15_DOOR_NONE; /* Kein Tür-Typ — andere Handler zuständig */
    }

    /* --- 4. Tür-Daten extrahieren --- */
    dest_stage = slot->data.door.dest_stage;
    dest_room  = slot->data.door.dest_room;
    spawn_x    = slot->data.door.spawn_x;
    spawn_y    = slot->data.door.spawn_y;
    spawn_z    = slot->data.door.spawn_z;
    spawn_rot  = slot->data.door.spawn_rot;

    RE15_INFO("DOOR", "Tür ausgelöst: Slot %d → Stage %d, Room 0x%02X, "
              "Spawn (%d, %d, %d), Rot %d",
              slot_idx, (int)dest_stage, (int)dest_room,
              (int)spawn_x, (int)spawn_y, (int)spawn_z, (int)spawn_rot);

    /* --- 5. Sonderfall: Same-Room-Door (Teleport ohne Neuladen) --- */
    if (dest_stage == g_game.current_stage && dest_room == g_game.current_room) {
        RE15_INFO("DOOR", "Same-Room-Teleport: Kein Raum-Neuladen");

        /* Spieler an Spawn-Position setzen (Q12-Fixkomma) */
        player->x   = Q12_FROM_INT((int32_t)spawn_x);
        player->y   = Q12_FROM_INT((int32_t)spawn_y);
        player->z   = Q12_FROM_INT((int32_t)spawn_z);
        player->yaw = spawn_rot;

        /* Kamera-Umschaltung: dest_cut aus Door-Daten anwenden */
        g_game.current_cut = slot->data.door.dest_cut;

        /* Überlappende Tür-AOT-Zonen als "betreten" markieren */
        re15_aot_mark_entered_doors(spawn_x, spawn_z, player->floor_band);

        return RE15_DOOR_OK;
    }

    /* --- 6. Stage-Wechsel wenn nötig --- */
    if (dest_stage != g_game.current_stage) {
        rc = re15_stage_change(dest_stage);
        if (rc != 0) {
            RE15_WARN("DOOR", "Stage-Wechsel fehlgeschlagen: Stage %d → %d (rc=%d)",
                      (int)g_game.current_stage, (int)dest_stage, rc);
            /* Fahre trotzdem fort — stage_change loggt intern den Fehler */
        }
    }

    /* --- 7. Zielraum laden --- */
    rc = re15_room_load(dest_stage, dest_room, g_game.current_player);
    if (rc != 0) {
        RE15_ERROR("DOOR", "Ziel-RDT nicht ladbar: Stage %d, Room 0x%02X (rc=%d). "
                   "Raumwechsel abgebrochen — Spieler bleibt an aktueller Position.",
                   (int)dest_stage, (int)dest_room, rc);
        return RE15_DOOR_LOAD_FAIL;
    }

    /* --- 8. Spieler an Spawn-Position/Rotation setzen --- */
    player->x   = Q12_FROM_INT((int32_t)spawn_x);
    player->y   = Q12_FROM_INT((int32_t)spawn_y);
    player->z   = Q12_FROM_INT((int32_t)spawn_z);
    player->yaw = spawn_rot;

    /* --- 9. Re-Trigger-Schutz: Tür-AOTs am Spawn als "entered" markieren --- */
    re15_aot_mark_entered_doors(spawn_x, spawn_z, player->floor_band);

    RE15_INFO("DOOR", "Raumwechsel abgeschlossen: Stage %d, Room 0x%02X, "
              "Spieler bei (%d, %d, %d), Yaw %d",
              (int)dest_stage, (int)dest_room,
              (int)spawn_x, (int)spawn_y, (int)spawn_z, (int)spawn_rot);

    return RE15_DOOR_OK;
}
