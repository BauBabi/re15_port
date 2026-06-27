/**
 * @file player_common.c
 * @brief Tank-Controls Bewegungslogik für die Spieler-Entität
 *
 * Implementiert die Spielerbewegung im klassischen Tank-Control-Schema:
 * - Vorwärts/Rückwärts entlang der Blickrichtung (sin_q12/cos_q12)
 * - Links/Rechts Rotation mit fester Rotationsgeschwindigkeit pro Frame
 * - Yaw-Wrap auf Bereich 0–4095
 * - Geschwindigkeitsstufen: Walk (0x4B), Run (0xC8)
 *
 * Validates: Requirements 11.1, 11.2
 */

#include "re15_player.h"
#include "re15_input.h"

/* ============================================================================
 * Externe Mathematik-Funktionen (implementiert in math_common.c, Task 9.2)
 * ========================================================================= */

/** Sinus-Lookup für Q12-Winkel (0–4095). Rückgabe: Fixkomma Q12 [-4096, +4096] */
extern int32_t re15_sin_q12(int16_t angle);

/** Kosinus-Lookup für Q12-Winkel (0–4095). Rückgabe: Fixkomma Q12 [-4096, +4096] */
extern int32_t re15_cos_q12(int16_t angle);

/* ============================================================================
 * Yaw-Konstanten
 * ========================================================================= */

/** Bitmaske für Yaw-Wrap: hält Wert im Bereich 0–4095 */
#define YAW_MASK 0x0FFF

/* ============================================================================
 * Implementierung
 * ========================================================================= */

/**
 * Aktualisiert die Spieler-Entität basierend auf dem aktuellen Input-State.
 *
 * Ablauf pro Frame (30 Hz):
 *   1. Rotation: Links/Rechts → yaw ± ROT_SPEED, Wrap via & 0x0FFF
 *   2. Geschwindigkeit: UP → Walk/Run (je nach R1), DOWN → -Walk, sonst 0
 *   3. Position: dx = (speed * sin(yaw)) >> 12, dz = (speed * cos(yaw)) >> 12
 *   4. Motion-State aktualisieren (IDLE / WALK / RUN)
 *
 * Kollisionsprüfung erfolgt in separatem Schritt (Task 10).
 *
 * @param player  Zeiger auf die Spieler-Entität
 * @param input   Aktueller Input-State des Frames
 */
void re15_player_update(re15_player_t* player, re15_input_state_t input)
{
    int16_t speed;
    int32_t dx;
    int32_t dz;

    /* ------------------------------------------------------------------
     * 1. Rotation: Links/Rechts dreht den Yaw
     * ------------------------------------------------------------------ */
    if (input.held & RE15_BTN_LEFT) {
        player->yaw -= RE15_ROT_SPEED;
    }
    if (input.held & RE15_BTN_RIGHT) {
        player->yaw += RE15_ROT_SPEED;
    }

    /* Yaw-Wrap: Bereich 0–4095 beibehalten */
    player->yaw = player->yaw & YAW_MASK;

    /* ------------------------------------------------------------------
     * 2. Geschwindigkeit bestimmen
     * ------------------------------------------------------------------ */
    if (input.held & RE15_BTN_UP) {
        /* Vorwärts: Run wenn R1 gehalten, sonst Walk */
        if (input.held & RE15_BTN_R1) {
            speed = RE15_SPEED_RUN;
        } else {
            speed = RE15_SPEED_WALK;
        }
    } else if (input.held & RE15_BTN_DOWN) {
        /* Rückwärts: immer Walk-Geschwindigkeit (negativ) */
        speed = -((int16_t)RE15_SPEED_WALK);
    } else {
        speed = 0;
    }

    player->speed = speed;

    /* ------------------------------------------------------------------
     * 3. Position aktualisieren (Bewegung entlang Blickrichtung)
     * ------------------------------------------------------------------ */
    if (speed != 0) {
        dx = ((int32_t)speed * re15_sin_q12(player->yaw)) >> Q12_SHIFT;
        dz = ((int32_t)speed * re15_cos_q12(player->yaw)) >> Q12_SHIFT;

        player->x += dx;
        player->z += dz;
    }

    /* ------------------------------------------------------------------
     * 4. Motion-State aktualisieren basierend auf Geschwindigkeit
     * ------------------------------------------------------------------ */
    if (speed == 0) {
        player->motion_state = RE15_MOTION_IDLE;
    } else if (speed == RE15_SPEED_RUN) {
        player->motion_state = RE15_MOTION_RUN;
    } else {
        /* Walk-Geschwindigkeit (vorwärts oder rückwärts) */
        player->motion_state = RE15_MOTION_WALK;
    }
}
