/**
 * @file re15_player.h
 * @brief Spieler-Datenmodell und Bewegungskonstanten
 *
 * Definiert die Spieler-Entität (re15_player_t) mit Position, Rotation,
 * Bewegungszustand, Animation und Kollisionsparametern.
 * Geschwindigkeiten und Rotation in Q12-Einheiten pro Frame.
 *
 * Validates: Requirements 11.1, 11.2
 */
#ifndef RE15_PLAYER_H
#define RE15_PLAYER_H

#include "re15_types.h"
#include "re15_input.h"

/* ============================================================================
 * Bewegungskonstanten (Q12-Einheiten pro Frame bei 30 fps)
 * ========================================================================= */

/** Gehgeschwindigkeit: 75 Q12-Einheiten/Frame */
#define RE15_SPEED_WALK  0x4B

/** Renngeschwindigkeit: 200 Q12-Einheiten/Frame */
#define RE15_SPEED_RUN   0xC8

/** Rotationsgeschwindigkeit: 96 Q12-Einheiten/Frame (≈ 8.4°/Frame) */
#define RE15_ROT_SPEED   0x60

/* ============================================================================
 * Motion-States Enum
 * ========================================================================= */

/**
 * Bewegungszustand des Spielers.
 * Bestimmt aktive Animation und Geschwindigkeitsstufe.
 */
typedef enum {
    RE15_MOTION_IDLE = 0,   /**< Stehend — keine Bewegung          */
    RE15_MOTION_WALK = 1,   /**< Gehen — Geschwindigkeit SPEED_WALK */
    RE15_MOTION_RUN  = 2,   /**< Rennen — Geschwindigkeit SPEED_RUN */
} re15_motion_state_t;

/* ============================================================================
 * Spieler-Struktur
 * ========================================================================= */

/**
 * Spieler-Entität — Position, Rotation, Bewegung, Animation, Kollision.
 *
 * Positionen verwenden 20.12 Fixkomma (Q12). Rotation (Yaw) im Bereich
 * 0–4095 (Q12-Einheiten = 360°). Tank-Controls: Vorwärts/Rückwärts
 * entlang Blickrichtung, Links/Rechts dreht den Yaw.
 */
typedef struct re15_player_s {
    /* Position (20.12 Fixkomma) */
    int32_t x;              /**< X-Position (Fixkomma Q12)          */
    int32_t y;              /**< Y-Position (Fixkomma Q12)          */
    int32_t z;              /**< Z-Position (Fixkomma Q12)          */

    /* Rotation (0-4095, Q12-Einheiten = 360°) */
    int16_t yaw;            /**< Blickrichtung (0–4095)             */

    /* Bewegung */
    uint8_t motion_state;   /**< Aktueller Zustand (re15_motion_state_t) */
    int16_t speed;          /**< Aktuelle Geschwindigkeit (Q12/Frame)    */

    /* Animation */
    uint8_t  anim_clip;     /**< Aktiver EDD-Clip-Index             */
    uint16_t anim_frame;    /**< Frame innerhalb des Clips          */

    /* Kollision */
    int16_t  radius;        /**< Zylinderradius für Kollisionsprüfung */
    uint8_t  floor_band;    /**< Aktueller Kollisions-Band-Wert      */
} re15_player_t;

/* ============================================================================
 * Player-Update API
 * ========================================================================= */

/**
 * Aktualisiert die Spieler-Entität basierend auf dem aktuellen Input-State.
 *
 * Implementiert Tank-Controls:
 *   - Rotation per Links/Rechts (±ROT_SPEED)
 *   - Vorwärts/Rückwärts entlang Blickrichtung
 *   - Geschwindigkeitsstufe Walk/Run
 *   - Yaw-Wrap auf 0–4095
 *
 * Kollisionsprüfung erfolgt separat nach diesem Aufruf.
 *
 * @param player  Zeiger auf die Spieler-Entität (wird modifiziert)
 * @param input   Aktueller Input-State des Frames
 */
void re15_player_update(re15_player_t* player, re15_input_state_t input);

#endif /* RE15_PLAYER_H */
