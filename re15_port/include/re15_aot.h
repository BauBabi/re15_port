/**
 * @file re15_aot.h
 * @brief AOT (Action Object Trigger) Datenstrukturen
 *
 * Definiert die Strukturen für das AOT-Trigger-System:
 * - 32 Slots für aktive Trigger-Zonen
 * - Typen: Door, Item, Generic, CamSwitch
 * - 4-Punkt-Polygon-Trigger mit Floor-Band-Zuordnung
 *
 * AOT-Slots werden durch SCD-Opcodes (Aot_set, Door_aot_set_4p,
 * Item_aot_set) registriert und per Frame gegen die Spielerposition
 * geprüft (Point-in-4P-Polygon + Band-Match).
 */
#ifndef RE15_AOT_H
#define RE15_AOT_H

#include "re15_types.h"

/* ============================================================================
 * AOT-Konfiguration
 * ========================================================================= */

/** Maximale Anzahl gleichzeitig aktiver AOT-Slots */
#define RE15_AOT_MAX_SLOTS 32

/* ============================================================================
 * AOT-Typen
 * ========================================================================= */

typedef enum {
    AOT_TYPE_DOOR       = 1,    /**< Tür-Übergang zu anderem Raum/Stage     */
    AOT_TYPE_ITEM       = 2,    /**< Item-Aufnahme                          */
    AOT_TYPE_GENERIC    = 3,    /**< Generischer Event-Callback             */
    AOT_TYPE_CAM_SWITCH = 4,    /**< Kamerazonen-Umschaltung                */
} aot_type_t;

/* ============================================================================
 * AOT-Slot-Struktur
 *
 * Jeder Slot definiert eine 4-Punkt-Polygonfläche (trigger_x/z) auf einem
 * bestimmten Floor-Band. Die Union enthält typspezifische Daten.
 * ========================================================================= */

/**
 * Einzelner AOT-Slot.
 *
 * Prüflogik pro Frame:
 *   1. active == 1?
 *   2. Spieler floor_band == slot floor_band?
 *   3. Point-in-4P-Polygon (trigger_x/z)?
 *   4. entered == 0? (verhindert sofortiges Re-Triggern nach Spawn)
 *   → Aktion gemäß type auslösen
 */
typedef struct {
    uint8_t   active;           /**< 0 = Slot inaktiv, 1 = aktiv            */
    uint8_t   type;             /**< aot_type_t Wert                        */
    uint8_t   entered;          /**< 1 = Spieler ist bereits in der Zone    */
    uint8_t   _pad0;

    /** 4-Punkt-Trigger-Polygon (X-Koordinaten, s16 Fixkomma) */
    int16_t   trigger_x[4];

    /** 4-Punkt-Trigger-Polygon (Z-Koordinaten, s16 Fixkomma) */
    int16_t   trigger_z[4];

    /** Floor-Band für Höhen-Zuordnung */
    uint8_t   floor_band;
    uint8_t   _pad1[3];

    /** Typspezifische Daten */
    union {
        /** AOT_TYPE_DOOR — Tür-Übergang */
        struct {
            uint8_t  dest_stage;    /**< Ziel-Stage (0-basiert)             */
            uint8_t  dest_room;     /**< Ziel-Room-ID                       */
            uint8_t  dest_cut;      /**< Ziel-Kamera-ID nach Übergang       */
            uint8_t  _pad_door;
            int16_t  spawn_x;       /**< Spawn-Position X (s16 LE)          */
            int16_t  spawn_y;       /**< Spawn-Position Y (s16 LE)          */
            int16_t  spawn_z;       /**< Spawn-Position Z (s16 LE)          */
            int16_t  spawn_rot;     /**< Spawn-Rotation (s16 LE, 0-4095)    */
        } door;

        /** AOT_TYPE_ITEM — Item-Aufnahme */
        struct {
            uint8_t  item_id;       /**< Item-Typ-ID                        */
            uint8_t  _pad;
            int16_t  amount;        /**< Anzahl / Munition                  */
        } item;

        /** AOT_TYPE_GENERIC — Generischer Event */
        struct {
            uint8_t  event_id;      /**< Event-Handler-ID für SCD           */
        } generic;

        /** AOT_TYPE_CAM_SWITCH — Kamerazonen-Wechsel */
        struct {
            uint8_t  cam_from;      /**< Quell-Kamera-ID                    */
            uint8_t  cam_to;        /**< Ziel-Kamera-ID                     */
        } cam_switch;
    } data;
} re15_aot_slot_t;

/* ============================================================================
 * AOT-System API
 * ========================================================================= */

/**
 * Initialisiert alle AOT-Slots (setzt active = 0).
 */
void re15_aot_init(void);

/**
 * Prüft alle aktiven AOT-Slots gegen die Spielerposition.
 *
 * @param player_x    Spieler X-Position (Welt-Koordinaten)
 * @param player_z    Spieler Z-Position (Welt-Koordinaten)
 * @param floor_band  Aktuelles Floor-Band des Spielers
 * @return            Index des ausgelösten Slots, oder -1 wenn keiner
 */
int re15_aot_check(int16_t player_x, int16_t player_z, uint8_t floor_band);

/**
 * Markiert alle AOT-Slots, deren Zone die gegebene Position enthält,
 * als "entered" (verhindert sofortiges Re-Triggern nach Spawn).
 *
 * @param player_x    Spieler X-Position
 * @param player_z    Spieler Z-Position
 * @param floor_band  Aktuelles Floor-Band
 */
void re15_aot_mark_entered(int16_t player_x, int16_t player_z, uint8_t floor_band);

/**
 * Markiert nur Tür-AOT-Slots (AOT_TYPE_DOOR), deren Zone die gegebene
 * Position enthält, als "entered".
 *
 * Wird nach Raumwechsel oder Same-Room-Teleport aufgerufen, damit
 * die Tür-Zone am Spawn-Punkt nicht sofort erneut auslöst.
 * Markiert ALLE Tür-AOTs die den Spawn enthalten, nicht nur die erste.
 *
 * @param player_x    Spieler X-Position
 * @param player_z    Spieler Z-Position
 * @param floor_band  Aktuelles Floor-Band
 *
 * Validates: Requirements 4.8
 */
void re15_aot_mark_entered_doors(int16_t player_x, int16_t player_z,
                                 uint8_t floor_band);

/**
 * Setzt einen AOT-Slot (wird durch SCD-Opcodes aufgerufen).
 *
 * @param slot_idx  Slot-Index (0..RE15_AOT_MAX_SLOTS-1)
 * @param slot      Zeiger auf die zu setzenden Slot-Daten
 * @return          0 bei Erfolg, -1 bei ungültigem Index
 */
int re15_aot_set_slot(int slot_idx, const re15_aot_slot_t* slot);

/**
 * Deaktiviert einen AOT-Slot.
 *
 * @param slot_idx  Slot-Index (0..RE15_AOT_MAX_SLOTS-1)
 */
void re15_aot_clear_slot(int slot_idx);

/**
 * Gibt einen Zeiger auf einen AOT-Slot zurück (read-only).
 *
 * @param slot_idx  Slot-Index (0..RE15_AOT_MAX_SLOTS-1)
 * @return          Zeiger auf den Slot, oder NULL bei ungültigem Index
 */
const re15_aot_slot_t* re15_aot_get_slot(int slot_idx);

#endif /* RE15_AOT_H */
